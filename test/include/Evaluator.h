/*
 * Copyright (c) 2010 Alexandre Devert <marmakoide@yahoo.fr>
 *
 * ESKit is free software; you can redistribute it and/or modify it under the 
 * terms of the MIT license. See LICENSE for details.
 */

#ifndef EVALUATOR_H
#define EVALUATOR_H

#include <eskit/Matrix.h>
#include "Function.h"



/*
   Implements a tunable ellipsoid objective function. This function is 
   computationally cheap, yet allows to validate the implementation of most
   commons ES. The ellipse center is not centered on the origin, but shifted 
   randomly at a given distance of the origin. A tendency for an algorithm 
   to fall in a singular point (typically, the origin) will be thus detected.

   Evaluator_setup provides the following tunings :
      - 'scaling' controls the length of the 1st axis of the ellipse. 
      - 'randRot' applies a random rotation to the ellipse.
      - 'seed' is the random seed, allowing to reproduce a tuning
 */

typedef struct {
	size_t N;
	const Function* function;
	double* u;
	int randRot;
	ekMatrix rot;

	double* xMeanInit;
	double sigmaInit;
} Evaluator;



extern void
Evaluator_init(Evaluator* self, const Function* function, size_t N, int randRot);



void
Evaluator_setup(Evaluator* self, uint32_t seed);



extern void
Evaluator_destroy(Evaluator* self);



extern double
Evaluator_evaluate(Evaluator* self, const double* u);



#endif /* EVALUATOR_H */
