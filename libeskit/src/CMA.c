/*
 * Copyright (c) 2010 Alexandre Devert <marmakoide@yahoo.fr>
 *
 * ESKit is free software; you can redistribute it and/or modify it under the 
 * terms of the MIT license. See LICENSE for details.
 */

#include <math.h>
#include <float.h>
#include <stdlib.h>
#include "eskit/Macros.h"
#include "eskit/ArrayOps.h"
#include "eskit/CMA.h"
#include "eskit/Optimizer.h"



static void
ekCMA_allocate(ekCMA* self, size_t N) {
	/* Allocation of vectors and matrixes */
	self->sigmaPath = newArray(double, N);
	self->cPath = newArray(double, N);
	ekMatrix_init(&(self->B), N, N);
	ekMatrix_init(&(self->C), N, N);
	ekMatrix_init(&(self->BD), N, N);
	self->D = newArray(double, N);

	/* Allocation of temporary vectors and matrixes */
	self->tmpVector = newArray(double, N);

	/* Eigen solver init */
	ekEigenSolver_init(&(self->eigenSolver), N);
}



void
ekCMA_init(ekCMA* self, size_t N) {
	ekCMA_allocate(self, N);
	ekCMA_setSigma(self, 1.0, 10e-12);
	
	self->hasCustomCov = 0;
}



void
ekCMA_destroy(ekCMA* self) {
	free(self->sigmaPath);
	free(self->cPath);
	ekMatrix_destroy(&(self->B));
	ekMatrix_destroy(&(self->C));
	ekMatrix_destroy(&(self->BD));
	free(self->D);

	free(self->tmpVector);

	ekEigenSolver_destroy(&(self->eigenSolver));
}


void
ekCMA_setSigma(ekCMA* self, double sigmaInit, double sigmaStop) {
	self->sigmaInit = sigmaInit;
	self->sigmaStop = sigmaStop;
}



void
ekCMA_setOptimizer(ekCMA* self, ekOptimizer* optim) {
	ekDistribution distrib;

	distrib.data = self;
	distrib.delegate = ekCMA_DistributionDelegate;

	ekOptimizer_setDistribution(optim, &distrib);
}



double
ekCMA_sigma(const ekCMA* self) {
	return self->sigma;
}



const ekMatrix*
ekCMA_C(const ekCMA* self) {
	return &(self->C);
}



const ekMatrix*
ekCMA_B(const ekCMA* self) {
	return &(self->B);
}



const double*
ekCMA_D(const ekCMA* self) {
	return self->D;
}



void
ekCMA_setC(ekCMA* self, const ekMatrix* C) {
	ekMatrix_copy(&(self->C), C);
	self->hasCustomCov = 1;
}



/* --- Distribution delegate implementation --------------------------------- */

static void
ekCMA_covUpdate(ekCMA* self) {
	if (!ekEigenSolver_solve(&(self->eigenSolver), &(self->C), &(self->B), self->D))
		self->eigenSolverFailure = 1;

	ekArrayOpsD_sqrt(self->D, ekMatrix_nbCols(&(self->C)));
	ekMatrix_diagProd(&(self->B), self->D, &(self->BD));
}



static void
ekCMA_start(ekCMA* self, ekOptimizer* optim) {
	size_t N;

	N = ekOptimizer_N(optim);

	self->sigma = self->sigmaInit;

	ekCMAConstants_setup(&(self->constants), optim);

	/* Eigen solver schedule */
	self->eigenSolverFailure = 0;
	self->eigenUpdatePeriod = fmax(1.0, 1.0 / (10.0 * N * (self->constants.c1 + self->constants.cMu)));
		
	/* Evolution path init */
	ekArrayOpsD_fill(self->sigmaPath, N, 0.0);
	ekArrayOpsD_fill(self->cPath, N, 0.0);

	/* Covariance init */
	ekMatrix_setAsIdentity(&(self->C));
	if (self->hasCustomCov) {
		ekCMA_covUpdate(self);
		self->hasCustomCov = 0;
	}
	else {
		ekMatrix_setAsIdentity(&(self->B));
		ekMatrix_setAsIdentity(&(self->BD));
		ekArrayOpsD_fill(self->D, N, 1.0);
	}
}



static void
ekCMA_update(ekCMA* self, ekOptimizer* optim) {
	size_t i, N;
	double *B_zMean, *B_D_zMean;
	double sigmaPathLength, HSigma;
	const ekCMAConstants* cma;

	N = ekOptimizer_N(optim);
	cma = &(self->constants);

	/* Compute B x zMean */
	B_zMean = self->tmpVector;
	ekMatrix_vectorProd(&(self->B), ekOptimizer_zMean(optim), B_zMean);

	/* Cumulate sigma evolution path */
	ekArrayOpsD_scalarMul(self->sigmaPath, N, 1.0 - cma->cSigma);
	ekArrayOpsD_incMul(self->sigmaPath, B_zMean, N, sqrt(cma->muW  * cma->cSigma * (2.0 - cma->cSigma)));
	sigmaPathLength = sqrt(ekArrayOpsD_squareSum(self->sigmaPath, N));

	/* Compute B x D x zMean */
	B_D_zMean = self->tmpVector;
	ekMatrix_vectorProd(&(self->BD), ekOptimizer_zMean(optim), B_D_zMean);

	/* Compute HSigma */
	HSigma = sigmaPathLength / sqrt(1.0 - pow(1.0 - cma->cSigma, 2.0 * (1.0 + ekOptimizer_nbUpdates(optim)))) / cma->chiN < (1.4 + 2.0 / (N + 1.0));

	/* Cumulate covariance matrix evolution path */
	ekArrayOpsD_scalarMul(self->cPath, N, 1.0 - cma->cc);
	ekArrayOpsD_incMul(self->cPath, B_D_zMean, N, HSigma * sqrt(cma->muW * cma->cc * (2.0 - cma->cc)));

	/* Adapt sigma */
	self->sigma *= exp((cma->cSigma / cma->dSigma) * ((sigmaPathLength / cma->chiN) - 1.0));

	/* Adapt covariance matrix C */
	ekMatrix_scalarMul(&(self->C), (1.0 - cma->c1 - cma->cMu) + (1.0 - HSigma) * cma->c1 * cma->cc * (2.0 - cma->cc)); /* C *= alpha * C */
	ekMatrix_incMulCross(&(self->C), self->cPath, cma->c1); /* C += c1 * cPath * cPath^T  */
	
	for(i = 0; i < ekOptimizer_mu(optim); ++i) {
		/* C += cMu * cPath * BDz_i * BDz_i^T */
		ekMatrix_vectorProd(&(self->BD), ekOptimizer_point(optim, i).z, self->tmpVector);
		ekMatrix_incMulCross(&(self->C), self->tmpVector, cma->cMu * ekOptimizer_weights(optim)[i]);
	}

	/* Update B and D from C */
	if ((self->eigenUpdatePeriod == 1) || (ekOptimizer_nbUpdates(optim) % self->eigenUpdatePeriod == 0))
		ekCMA_covUpdate(self);
}



static void
ekCMA_samplePoint(ekCMA* self, ekOptimizer* optim, double* x, double* z) {
	size_t N;

	N = ekOptimizer_N(optim);
	
	/* Generate z */
	ekArrayOpsD_gaussian(z, N, ekOptimizer_getRandomizer(optim), 1.0);

	/* Compute x */
	ekMatrix_vectorProd(&(self->BD), z, x);
	ekArrayOpsD_scalarMul(x, N, self->sigma);	
	ekArrayOpsD_inc(x, ekOptimizer_xMean(optim), N);
}



static void
ekCMA_sampleCloud(ekCMA* self, ekOptimizer* optim, ekMatrix* x, ekMatrix* z) {
	size_t i;

	/* Generate z */
	ekMatrix_setAsGaussian(z, ekOptimizer_getRandomizer(optim), 1.0);

	/* Compute x */
	ekMatrix_matrixProd(&(self->BD), z, x);
	ekMatrix_scalarMul(x, self->sigma);

	for(i = 0; i < ekMatrix_nbCols(x); ++i)
		ekArrayOpsD_inc(ekMatrix_col(x, i), ekOptimizer_xMean(optim), ekMatrix_nbRows(x));
}



static enum ekStopCriterionId
ekCMA_stop(ekCMA* self, ekOptimizer* optim) {
	size_t i, j, N;
	double a, b, factor, DMin, DMax;

	N = ekOptimizer_N(optim);

	/* The covariance matrix makes the eigen solver cry  */
	if (self->eigenSolverFailure)
		return ekStopCriterionId_EigenSolverFailure;

	/* TolX criterion */
	if (self->sigma <= self->sigmaStop)
		return ekStopCriterionId_LowSigma;

	/* NoEffectAxis criterion */
	for(i = 0; i < N; ++i) {
		factor = 0.1 * self->sigma * self->D[i];

		for(j = 0; j < N; ++j) {
			a = ekOptimizer_xMean(optim)[j];
			b = a + factor * ekMatrix_at(&(self->B), i, j);
			
			if (fabs(a - b) > DBL_EPSILON)
				break;
		}

		if (j == N)
			return ekStopCriterionId_NoEffectAxis;
	}

	/* NoEffectCoord criterion */
	for(i = 0; i < N; ++i) {
		a = ekOptimizer_xMean(optim)[i];

		if (fabs(a - b) <= DBL_EPSILON)
			return ekStopCriterionId_NoEffectCoord;
	}

	/* Get lowest and largest eigen value */
	DMin = ekArrayOpsD_min(self->D, N);
	DMax = ekArrayOpsD_max(self->D, N);

	/* ConditionCov criterion */
	if (DMax >= 1e14 * DMin)
		return ekStopCriterionId_ConditionCov;

	/* No reasons to stop so far */
	return ekStopCriterionId_None;
}



/* --- ekCMA delegate --------------------------------------------------------- */

static void
ekCMA_delegate_start(ekDistribution* self, ekOptimizer* optim) {
	ekCMA_start((ekCMA*)self->data, optim);
}



static void
ekCMA_delegate_update(ekDistribution* self, ekOptimizer* optim) {
	ekCMA_update((ekCMA*)self->data, optim);
}



static void
ekCMA_delegate_samplePoint(ekDistribution* self, ekOptimizer* optim, size_t ESKIT_UNUSED(id), double* x, double* z) {
	ekCMA_samplePoint((ekCMA*)self->data, optim, x, z);
}



static void
ekCMA_delegate_sampleCloud(ekDistribution* self, ekOptimizer* optim, ekMatrix* x, ekMatrix* z) {
	ekCMA_sampleCloud((ekCMA*)self->data, optim, x, z);
}



enum ekStopCriterionId
ekCMA_delegate_stop(ekDistribution* self, ekOptimizer* optim) {
	return ekCMA_stop((ekCMA*)self->data, optim);
}



const ekDistributionDelegate 
ekCMA_DistributionDelegate =
{
	"CMA",
	ekCMA_delegate_start,
	ekCMA_delegate_update,
	ekCMA_delegate_samplePoint,
	ekCMA_delegate_sampleCloud,
	ekCMA_delegate_stop
};
