/*
 * Copyright (c) 2009-2023 Alexandre Devert <marmakoide@hotmail.fr>
 *
 * ESKit is free software; you can redistribute it and/or modify it under the 
 * terms of the MIT license. See LICENSE for details.
 */

#include <math.h>
#include <float.h>
#include <stdlib.h>
#include "eskit/Macros.h"
#include "eskit/Matrix.h"
#include "eskit/ArrayOps.h"
#include "eskit/CSA.h"
#include "eskit/Optimizer.h"



static void
ekCSA_allocate(ekCSA* self, size_t N) {
	self->c = 1.0 / sqrt((double)N);
	self->dampening = 1.0 / (2.0 * N * sqrt((double)N));
	self->sigmaPath = newArray(double, N);
}



void
ekCSA_init(ekCSA* self, size_t N) {
	ekCSA_allocate(self, N);
	ekCSA_setSigma(self, 1.0, 10e-12);
}



void
ekCSA_destroy(ekCSA* self) {
	free(self->sigmaPath);
}



void
ekCSA_setSigma(ekCSA* self, double sigmaInit, double sigmaStop) {
	self->sigmaInit = sigmaInit;
	self->sigmaStop = sigmaStop;
}



void
ekCSA_setOptimizer(ekCSA* self, ekOptimizer* optim) {
	ekDistribution distrib;

	distrib.data = self;
	distrib.delegate = ekCSA_DistributionDelegate;

	ekOptimizer_setDistribution(optim, &distrib);
}



double
ekCSA_sigma(const ekCSA* self) {
	return self->sigma;
}



/* --- Distribution delegate implementation --------------------------------- */

static void
ekCSA_start(ekCSA* self, ekOptimizer* optim) {
	size_t N;

	N = ekOptimizer_N(optim);

	self->sigma = self->sigmaInit;
	ekArrayOpsD_fill(self->sigmaPath, N, 0.0);
}



static void
ekCSA_update(ekCSA* self, ekOptimizer* optim) {
	size_t N;
	double mu, sigmaPathLength;

	N = ekOptimizer_N(optim);
	mu = ekOptimizer_mu(optim);

	/* Cumulate sigma evolution path */
	ekArrayOpsD_scalarMul(self->sigmaPath, N, 1.0 - self->c);
	ekArrayOpsD_incMul(self->sigmaPath, ekOptimizer_zMean(optim), N, sqrt(mu * self->c * (2.0 - self->c)));
	sigmaPathLength = sqrt(ekArrayOpsD_squareSum(self->sigmaPath, N));

	/* Adapt sigma */
	self->sigma *= exp(self->dampening * (sigmaPathLength - N));
}



static void
ekCSA_sampleCloud(ekCSA* self, ekOptimizer* optim, ekMatrix* x, ekMatrix* z) {
	size_t i;

	/* Generate z */
	ekMatrix_setAsGaussian(z, ekOptimizer_getRandomizer(optim), 1.0);

	/* Compute x */
	ekMatrix_copyMul(x, z, self->sigma);

	for(i = 0; i < ekMatrix_nbCols(x); ++i)
		ekArrayOpsD_inc(ekMatrix_col(x, i), ekOptimizer_xMean(optim), ekMatrix_nbRows(x));	
}



static void
ekCSA_samplePoint(ekCSA* self, ekOptimizer* optim, double* x, double* z) {
	size_t N;

	N = ekOptimizer_N(optim);

	/* Generate z */
	ekArrayOpsD_gaussian(z, N, ekOptimizer_getRandomizer(optim), 1.0);

	/* Compute x */
	ekArrayOpsD_copyMul(x, z, N, self->sigma);	
	ekArrayOpsD_inc(x, ekOptimizer_xMean(optim), N);	
}



static enum ekStopCriterionId
ekCSA_stop(ekCSA* self, ekOptimizer* ESKIT_UNUSED(optim)) {
	if (self->sigma <= self->sigmaStop)
		return ekStopCriterionId_LowSigma;

	return ekStopCriterionId_None;
}



/* --- IsotropicGaussian delegate ------------------------------------------- */

static void
ekCSA_delegate_start(ekDistribution* self, ekOptimizer* optim) {
	ekCSA_start((ekCSA*)self->data, optim);
}



static void
ekCSA_delegate_update(ekDistribution* self, ekOptimizer* optim) {
	ekCSA_update((ekCSA*)self->data, optim);
}



static void
ekCSA_delegate_samplePoint(ekDistribution* self, ekOptimizer* optim, size_t ESKIT_UNUSED(id), double* x, double* z) {
	ekCSA_samplePoint((ekCSA*)self->data, optim, x, z);
}



static void
ekCSA_delegate_sampleCloud(ekDistribution* self, ekOptimizer* optim, ekMatrix* x, ekMatrix* z) {
	ekCSA_sampleCloud((ekCSA*)self->data, optim, x, z);
}



enum ekStopCriterionId
ekCSA_delegate_stop(ekDistribution* self, ekOptimizer* optim) {
	return ekCSA_stop((ekCSA*)self->data, optim);
}



const ekDistributionDelegate 
ekCSA_DistributionDelegate =
{
	"CSA",
	ekCSA_delegate_start,
	ekCSA_delegate_update,
	ekCSA_delegate_samplePoint,
	ekCSA_delegate_sampleCloud,
	ekCSA_delegate_stop
};
