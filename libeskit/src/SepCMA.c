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
#include "eskit/SepCMA.h"
#include "eskit/Optimizer.h"



static void
ekSepCMA_allocate(ekSepCMA* self, size_t N) {
	/* Allocation of vectors and matrixes */
	self->sigmaPath = newArray(double, N);
	self->cPath = newArray(double, N);
	ekMatrix_init(&(self->C), N, N);
	self->D = newArray(double, N);

	/* Allocation of temporary vectors and matrixes */
	self->tmpVector = newArray(double, N);
}



void
ekSepCMA_init(ekSepCMA* self, size_t N) {
	ekSepCMA_allocate(self, N);
	ekSepCMA_setSigma(self, 1.0, 10e-12);

	self->hasCustomCov = 0;
}



void
ekSepCMA_destroy(ekSepCMA* self) {
	free(self->sigmaPath);
	free(self->cPath);
	ekMatrix_destroy(&(self->C));
	free(self->D);

	free(self->tmpVector);
}



void
ekSepCMA_setSigma(ekSepCMA* self, double sigmaInit, double sigmaStop) {
	self->sigmaInit = sigmaInit;
	self->sigmaStop = sigmaStop;
}



void
ekSepCMA_setOptimizer(ekSepCMA* self, ekOptimizer* optim) {
	ekDistribution distrib;

	distrib.data = self;
	distrib.delegate = ekSepCMA_DistributionDelegate;

	ekOptimizer_setDistribution(optim, &distrib);
}



double
ekSepCMA_sigma(const ekSepCMA* self) {
	return self->sigma;
}



const ekMatrix*
ekSepCMA_C(const ekSepCMA* self) {
	return &(self->C);
}



const double*
ekSepCMA_D(const ekSepCMA* self) {
	return self->D;
}



void
ekSepCMA_setC(ekSepCMA* self, const ekMatrix* C) {
	ekMatrix_copy(&(self->C), C);
	self->hasCustomCov = 1;
}



/* --- Distribution delegate implementation --------------------------------- */

static void
ekSepCMA_covUpdate(ekSepCMA* self) {
	ekMatrix_getDiagonal(&(self->C), self->D);
	ekArrayOpsD_sqrt(self->D, ekMatrix_nbCols(&(self->C)));
}



static void
ekSepCMA_start(ekSepCMA* self, ekOptimizer* optim) {
	size_t N;
	double k;

	N = ekOptimizer_N(optim);

	self->sigma = self->sigmaInit;

	/* ekSepCMA constants are indeed CMA constants tweaked a little bit */
	ekCMAConstants_setup(&(self->constants), optim);

	k = (N + 1.5) / 3.0;
	self->constants.cMu = fmin(1.0 - self->constants.c1, k * self->constants.cMu);
	self->constants.c1  = fmin(1.0, k * self->constants.c1);

	/* Evolution path init */
	ekArrayOpsD_fill(self->sigmaPath, N, 0.0);
	ekArrayOpsD_fill(self->cPath, N, 0.0);

	/* Covariance init */
	if (self->hasCustomCov) {
		ekSepCMA_covUpdate(self);
		self->hasCustomCov = 0;
	}
	else {
		ekMatrix_setAsIdentity(&(self->C));
		ekArrayOpsD_fill(self->D, N, 1.0);
	}
}



static void
ekSepCMA_update(ekSepCMA* self, ekOptimizer* optim) {
	size_t i, N;
	double* D_zMean;
	double sigmaPathLength, HSigma;
	const ekCMAConstants* cma;

	N = ekOptimizer_N(optim);
	cma = &(self->constants);

	/* Cumulate sigma evolution path */
	ekArrayOpsD_scalarMul(self->sigmaPath, N, 1.0 - cma->cSigma);
	ekArrayOpsD_incMul(self->sigmaPath, ekOptimizer_zMean(optim), N, sqrt(cma->muW * cma->cSigma * (2.0 - cma->cSigma)));
	sigmaPathLength = sqrt(ekArrayOpsD_squareSum(self->sigmaPath, N));

	/* Compute D x zMean */
	D_zMean = self->tmpVector;
	ekArrayOpsD_copy(D_zMean, ekOptimizer_zMean(optim), N);
	ekArrayOpsD_convolve(D_zMean, self->D, N);

	/* Compute HSigma */
	HSigma = sigmaPathLength / sqrt(1.0 - pow(1.0 - cma->cSigma, 2.0 * (1.0 + ekOptimizer_nbUpdates(optim)))) / cma->chiN < (1.4 + 2.0 / (N + 1.0));

	/* Cumulate covariance matrix evolution path */
	ekArrayOpsD_scalarMul(self->cPath, N, 1.0 - cma->cc);
	ekArrayOpsD_incMul(self->cPath, D_zMean, N, HSigma * sqrt(cma->muW * cma->cc * (2.0 - cma->cc)));

	/* Adapt sigma */
	self->sigma *= exp((cma->cSigma / cma->dSigma) * ((sigmaPathLength / cma->chiN) - 1.0));

	/* Adapt covariance matrix C */
	ekMatrix_scalarMul(&(self->C), (1.0 - cma->c1 - cma->cMu) + (1.0 - HSigma) * cma->c1 * cma->cc * (2.0 - cma->cc)); /* C *= alpha * C */
	ekMatrix_incMulCross(&(self->C), self->cPath, cma->c1); /* C += c1 * cPath * cPath^T  */
	
	for(i = 0; i < ekOptimizer_mu(optim); ++i) {
		/* C += cMu * cPath * Dz_i * Dz_i^T */
		ekArrayOpsD_copy(self->tmpVector, ekOptimizer_point(optim, i).z, N);
		ekArrayOpsD_convolve(self->tmpVector, self->D, N);
		ekMatrix_incMulCross(&(self->C), self->tmpVector, cma->cMu * ekOptimizer_weights(optim)[i]);
	}

	/* Update D from C */
	ekSepCMA_covUpdate(self);
}



static void
ekSepCMA_sampleCloud(ekSepCMA* self, ekOptimizer* optim, ekMatrix* x, ekMatrix* z) {
	size_t i;

	/* Generate z */
	ekMatrix_setAsGaussian(z, ekOptimizer_getRandomizer(optim), 1.0);

	/* Compute x */
	ekMatrix_copy(x, z);
	ekMatrix_diagMul(x, self->D);
	ekMatrix_scalarMul(x, self->sigma);

	for(i = 0; i < ekMatrix_nbCols(x); ++i)
		ekArrayOpsD_inc(ekMatrix_col(x, i), ekOptimizer_xMean(optim), ekMatrix_nbRows(x));
}



static void
ekSepCMA_samplePoint(ekSepCMA* self, ekOptimizer* optim, double* x, double* z) {
	size_t N;

	N = ekOptimizer_N(optim);
	
	/* Generate z */
	ekArrayOpsD_gaussian(z, N, ekOptimizer_getRandomizer(optim), 1.0);

	/* Compute x */
	ekArrayOpsD_copy(x, z, N);
	ekArrayOpsD_convolve(x, self->D, N);
	ekArrayOpsD_scalarMul(x, N, self->sigma);
	ekArrayOpsD_inc(x, ekOptimizer_xMean(optim), N);
}



static enum ekStopCriterionId
ekSepCMA_stop(ekSepCMA* self, ekOptimizer* optim) {
	size_t i, N;
	double a, b, DMin, DMax;

	N = ekOptimizer_N(optim);

	/* TolX criterion */
	if (self->sigma <= self->sigmaStop)
		return ekStopCriterionId_LowSigma;

	/* NoEffectCoord criterion */
	for(i = 0; i < N; ++i) {
		a = ekOptimizer_xMean(optim)[i];
		b = a + 0.2 * self->sigma * sqrt(ekMatrix_at(&(self->C), i, i));

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



/* --- ekSepCMA delegate ----------------------------------------------------- */

static void
ekSepCMA_delegate_start(ekDistribution* self, ekOptimizer* optim) {
	ekSepCMA_start((ekSepCMA*)self->data, optim);
}



static void
ekSepCMA_delegate_update(ekDistribution* self, ekOptimizer* optim) {
	ekSepCMA_update((ekSepCMA*)self->data, optim);
}



static void
ekSepCMA_delegate_samplePoint(ekDistribution* self, ekOptimizer* optim, size_t ESKIT_UNUSED(id), double* x, double* z) {
	ekSepCMA_samplePoint((ekSepCMA*)self->data, optim, x, z);
}



static void
ekSepCMA_delegate_sampleCloud(ekDistribution* self, ekOptimizer* optim, ekMatrix* x, ekMatrix* z) {
	ekSepCMA_sampleCloud((ekSepCMA*)self->data, optim, x, z);
}



enum ekStopCriterionId
ekSepCMA_delegate_stop(ekDistribution* self, ekOptimizer* optim) {
	return ekSepCMA_stop((ekSepCMA*)self->data, optim);
}



const ekDistributionDelegate 
ekSepCMA_DistributionDelegate =
{
	"SepCMA",
	ekSepCMA_delegate_start,
	ekSepCMA_delegate_update,
	ekSepCMA_delegate_samplePoint,
	ekSepCMA_delegate_sampleCloud,
	ekSepCMA_delegate_stop
};
