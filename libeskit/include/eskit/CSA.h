/*
 * Copyright (c) 2009-2023 Alexandre Devert <marmakoide@hotmail.fr>
 *
 * ESKit is free software; you can redistribute it and/or modify it under the 
 * terms of the MIT license. See LICENSE for details.
 */

#ifndef ESKIT_CSA_H
#define ESKIT_CSA_H

#ifdef __cplusplus
extern "C" {
#endif


#include <eskit/Distribution.h>



/*
   Implements an isotropic gaussian distribution, with Cumulative Step Length 
   Adaption heuristic for the step-size control.

	 The stopping criterion is satisfied when the step-length is bellow the
   CPU floating point precision.	

   The full algorithm is described in :
   Evolution Strategies with Cumulative Step Length Adaption on the Noisy Parabolic Ridge
   Dirk V. Arnold & Hans-Georg Beyer
 */

typedef struct {
	double sigmaInit;
	double sigmaStop;

	double sigma;       /* Mutation strength                                  */
	double c;           /* Cumulation parameter                               */
	double dampening;   /* Adaption dampening                                 */
	double* sigmaPath;  /* Evolution path for sigma                           */
} ekCSA;



extern const ekDistributionDelegate 
ekCSA_DistributionDelegate;



extern void
ekCSA_init(ekCSA* self, size_t N);



extern void
ekCSA_destroy(ekCSA* self);



extern void
ekCSA_setSigma(ekCSA* self, double sigmaInit, double sigmaStop);



extern void
ekCSA_setOptimizer(ekCSA* self, ekOptimizer* optim);



extern double
ekCSA_sigma(const ekCSA* self);



#ifdef __cplusplus
}
#endif

#endif /* ESKIT_CSA_H */
