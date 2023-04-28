/*
 * Copyright (c) 2009-2023 Alexandre Devert <marmakoide@hotmail.fr>
 *
 * ESKit is free software; you can redistribute it and/or modify it under the 
 * terms of the MIT license. See LICENSE for details.
 */

#include <math.h>
#include <stdlib.h>
#include "eskit/Macros.h"
#include "eskit/Matrix.h"
#include "eskit/ArrayOps.h"
#include "eskit/Optimizer.h"
#include "eskit/MeanWeights.h"
#include "eskit/Distribution.h"
#include "eskit/NullDistribution.h"



static void
ekOptimizer_cleanup(ekOptimizer* self) {
	free(self->meanWeights);
	free(self->pointArray);
	free(self->points);
	ekMatrix_destroy(&(self->X));
	ekMatrix_destroy(&(self->Z));
}



static void
ekOptimizer_setup(ekOptimizer* self, size_t popSize) {
	size_t i;
	ekPoint* point;

	/* Allocation for mean weights */
	self->meanWeights    = newArray(double, popSize);

	/* Allocation for the points population */
	self->pointArray     = newArray(ekPoint, popSize);
	self->points         = newArray(ekPoint*, popSize);
	ekMatrix_init(&(self->X), popSize, self->N);
	ekMatrix_init(&(self->Z), popSize, self->N);

	point = self->pointArray;
	for(i = 0; i < popSize; ++i, ++point) {
		self->points[i] = point;
		point->x = ekMatrix_col(&(self->X), i);
		point->z = ekMatrix_col(&(self->Z), i);
	}

	self->nbPointsMax = popSize;
}



void
ekOptimizer_init(ekOptimizer* self, size_t N) {
	size_t defaultLambda;

	self->N = N;

	/* Randomizer init & seeding (in case user forgot to seed) */
	ekRandomizer_init(&(self->randomizer), ekRandomizerSize_1024);
	ekRandomizer_seed(&(self->randomizer), 42);

	/* Allocation independent from population size */
	self->xMean        = newArray(double, N);
	self->zMean        = newArray(double, N);
	self->bestPoint.x  = newArray(double, N);
	self->bestPoint.z  = NULL;

	/* Default setting for mean weights */
	self->meanWeightsGen = &ekLog_MeanWeightsGenerator;
	self->meanWeightsSetupDone = 0;

	/* No distribution set yet */
	self->distrib.data = NULL;
	self->distrib.delegate = ekNullDistribution_DistributionDelegate;

	/* Default setting for mu & lambda */
	defaultLambda = 4.0 + 3.0 * log((double)N);
	ekOptimizer_setMuLambda(self, defaultLambda / 2, defaultLambda);

	ekOptimizer_setup(self, self->lambda);

  /* Initialize xMean, to avoid non-sense calculations if user forget to do it */
	ekArrayOpsD_fill(self->xMean, N, 0.0);
}



void
ekOptimizer_destroy(ekOptimizer* self) {
	ekOptimizer_cleanup(self);

	free(self->xMean);
	free(self->zMean);
	free(self->bestPoint.x);

	ekRandomizer_destroy(&(self->randomizer));
}



void
ekOptimizer_setMuLambda(ekOptimizer* self, size_t mu, size_t lambda) {
	self->mu = mu;
	self->lambda = lambda;
	self->nbUpdatesBestFitnessStalledLimit = 10 + floor((30.0 * self->N) / lambda);
	self->meanWeightsSetupDone = 0;
}



void
ekOptimizer_setMeanWeights(ekOptimizer* self, ekMeanWeightsGenerator* gen) {
	self->meanWeightsGen = gen;
	self->meanWeightsSetupDone = 0;	
}



void
ekOptimizer_setDistribution(ekOptimizer* self, const ekDistribution* distrib) {
	self->distrib = *distrib;
}



static void
ekOptimizer_setupMeanWeights(ekOptimizer* self) {
	double sum;

	self->meanWeightsGen->generate(self->meanWeights, self->mu);
	sum = ekArrayOpsD_sum(self->meanWeights, self->mu);
	ekArrayOpsD_scalarDiv(self->meanWeights, self->mu, sum);
}



void
ekOptimizer_start(ekOptimizer* self) {
	/* Allocate enough space for the run */
	if (self->nbPointsMax < self->lambda) {
		ekOptimizer_cleanup(self);
		ekOptimizer_setup(self, self->lambda);
	} 

	/* Generate the weights to compute the distribution center */
	if (self->meanWeightsSetupDone == 0) {
		ekOptimizer_setupMeanWeights(self);
		self->meanWeightsSetupDone = 1;
	}

	/* Start the distribution */
	ekDistribution_start(&(self->distrib), self);

	/* Job done */
	self->nbUpdates = 0;
	self->nbUpdatesBestFitnessStalled = 0;
}



void
ekOptimizer_samplePoint(ekOptimizer* self, size_t index) {
	ekPoint* point;

	point = self->points[index];
	ekDistribution_samplePoint(&(self->distrib), self, index, point->x, point->z);
}



void
ekOptimizer_sampleCloud(ekOptimizer* self) {
	ekDistribution_sampleCloud(&(self->distrib), self, &(self->X), &(self->Z));
}



int
ekPoint_cmpFitness(const void* a, const void* b) {
	const ekPoint *u, *v;

	u = *((const ekPoint**)a);
	v = *((const ekPoint**)b);

	if (u->fitness < v->fitness)
		return -1;

	return u->fitness > v->fitness;
}



void
ekOptimizer_update(ekOptimizer* self) {
	size_t i;
	ekPoint* point;

	/* Sort points according to their fitnesses (TODO : partial sort of the mu first) */
	qsort(self->points, self->lambda, sizeof(ekPoint*), ekPoint_cmpFitness);

	/* Update best point ever */
	point = self->points[0];
	if ((self->nbUpdates == 0) || (self->bestPoint.fitness > point->fitness)) {
		self->bestPoint.fitness = point->fitness;
		ekArrayOpsD_copy(self->bestPoint.x, point->x, self->N);
		self->nbUpdatesBestFitnessStalled = 0;
	}
	else
		self->nbUpdatesBestFitnessStalled += 1;

	/* Update xMean */
	ekArrayOpsD_fill(self->xMean, self->N, 0.0);
	for(i = 0; i < self->mu; ++i)
		ekArrayOpsD_incMul(self->xMean, self->points[i]->x, self->N, self->meanWeights[i]);

	/* Update zMean */
	ekArrayOpsD_fill(self->zMean, self->N, 0.0);
	for(i = 0; i < self->mu; ++i)
		ekArrayOpsD_incMul(self->zMean, self->points[i]->z, self->N, self->meanWeights[i]);
	
	/* Update the distribution */
	ekDistribution_update(&(self->distrib), self);

	/* Job done */
	self->nbUpdates += 1;
}



void
ekOptimizer_evaluateFunction(ekOptimizer* self, double(*function)(const double*, size_t)) {
	size_t i;
	ekPoint* point;

	point = self->pointArray;
	for(i = self->lambda; i != 0; --i, ++point)
		point->fitness = function(point->x, self->N);
}



enum ekStopCriterionId
ekOptimizer_stop(ekOptimizer* self) {
	if (self->nbUpdatesBestFitnessStalled > self->nbUpdatesBestFitnessStalledLimit)
		return ekStopCriterionId_BestFitnessStall;

	return ekDistribution_stop(&(self->distrib), self);
}

