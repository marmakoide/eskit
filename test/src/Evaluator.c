/*
 * Copyright (c) 2010 Alexandre Devert <marmakoide@yahoo.fr>
 *
 * ESKit is free software; you can redistribute it and/or modify it under the 
 * terms of the MIT license. See LICENSE for details.
 */

#include <math.h>
#include <stdlib.h>
#include <eskit/Macros.h>
#include <eskit/ArrayOps.h>
#include "Evaluator.h"



void
Evaluator_init(Evaluator* self, const Function* function, size_t N, int randRot) {
	self->N = N;
	self->function = function;
	self->randRot = randRot;
	self->u = newArray(double, N);
	self->xMeanInit = newArray(double, N);
	ekMatrix_init(&(self->rot), N, N);
}



void
Evaluator_destroy(Evaluator* self) {
	free(self->u);
	free(self->xMeanInit);
	ekMatrix_destroy(&(self->rot));
}



void
Evaluator_setup(Evaluator* self, uint32_t seed) {
	ekRandomizer randomizer;

	/* Setup a random number generator */
	ekRandomizer_init(&randomizer, ekRandomizerSize_1024);
	ekRandomizer_seed(&randomizer, seed);

	/* Build the initial point */
	ekArrayOpsD_uniform(self->xMeanInit, self->N, &randomizer, self->function->minBound, self->function->maxBound);

	/* Compute initial sigma */
	self->sigmaInit = 0.5f * (self->function->maxBound - self->function->minBound);
	
	/* Build the rotation */
	if (self->randRot)
		ekMatrix_setAsRandomRotation(&(self->rot), &randomizer);

	/* Job done */
	ekRandomizer_destroy(&randomizer);
}



double
Evaluator_evaluate(Evaluator* self, const double* u) {
	const double* y;

	if (self->randRot) {
		ekMatrix_vectorProd(&(self->rot), u, self->u);
		y = self->u;
	}
	else
		y = u;

	return self->function->compute(y, self->N);
}
