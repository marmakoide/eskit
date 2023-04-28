/*
 * Copyright (c) 2009-2023 Alexandre Devert <marmakoide@hotmail.fr>
 *
 * ESKit is free software; you can redistribute it and/or modify it under the 
 * terms of the MIT license. See LICENSE for details.
 */

#ifndef ESKIT_OPTIMIZER_H
#define ESKIT_OPTIMIZER_H

#ifdef __cplusplus
extern "C" {
#endif



#include <eskit/Distribution.h>
#include <eskit/Randomizer.h>



typedef struct {
	double* x;
	double* z;
	double fitness;
} ekPoint;



struct s_ekOptimizer {
	size_t N;
	size_t mu;
	size_t lambda;

	size_t nbUpdates;

	ekMatrix X;
	double* xMean;

	ekMatrix Z;
	double* zMean;

	int meanWeightsSetupDone;
	double* meanWeights;
	ekMeanWeightsGenerator* meanWeightsGen;

	ekPoint* pointArray;
	ekPoint** points;
	size_t nbPointsMax;
	ekPoint bestPoint;

	size_t nbUpdatesBestFitnessStalled;
	size_t nbUpdatesBestFitnessStalledLimit;

	ekRandomizer randomizer;
	ekDistribution distrib;
};



#define ekOptimizer_N(self) (self)->N

#define ekOptimizer_mu(self) (self)->mu

#define ekOptimizer_lambda(self) (self)->lambda

#define ekOptimizer_weights(self) (self)->meanWeights

#define ekOptimizer_xMean(self) (self)->xMean

#define ekOptimizer_zMean(self) (self)->zMean

#define ekOptimizer_nbUpdates(self) (self)->nbUpdates

#define ekOptimizer_point(self, index) (*(self)->points[(index)])

#define ekOptimizer_bestPoint(self) (self)->bestPoint

#define ekOptimizer_getRandomizer(self) &((self)->randomizer)



extern void
ekOptimizer_init(ekOptimizer* self, size_t N);



extern void
ekOptimizer_destroy(ekOptimizer* self);



extern void
ekOptimizer_setMuLambda(ekOptimizer* self, size_t mu, size_t lambda);



extern void
ekOptimizer_setMeanWeights(ekOptimizer* self, ekMeanWeightsGenerator* gen);



void
ekOptimizer_setDistribution(ekOptimizer* self, const ekDistribution* distrib);



extern void
ekOptimizer_start(ekOptimizer* self);



extern void
ekOptimizer_sampleCloud(ekOptimizer* self);



extern void
ekOptimizer_samplePoint(ekOptimizer* self, size_t index);



extern void
ekOptimizer_evaluateFunction(ekOptimizer* self, double(*function)(const double*, size_t));



extern void
ekOptimizer_update(ekOptimizer* self);



extern enum ekStopCriterionId
ekOptimizer_stop(ekOptimizer* self);



#ifdef __cplusplus
}
#endif

#endif /* ESKIT_OPTIMIZER_H */
