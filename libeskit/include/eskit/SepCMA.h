/*
 * Copyright (c) 2010 Alexandre Devert <marmakoide@yahoo.fr>
 *
 * ESKit is free software; you can redistribute it and/or modify it under the 
 * terms of the MIT license. See LICENSE for details.
 */

#ifndef ESKIT_SEP_CMA_H
#define ESKIT_SEP_CMA_H

#ifdef __cplusplus
extern "C" {
#endif



#include <eskit/Matrix.h>
#include <eskit/Distribution.h>
#include <eskit/CMAConstants.h>



/*
   Implements a scaled Gaussian distribution, with the Covariance 
	 Matrix Adaption heuristic for the axis length and step-size adaption.

	 The stopping criterion is satisfied when numerical precision issues prevents
   to make any progress.	

	 This implementation is a close adaptation of 'cmaes.m' by Nikolaus Hansen et al.
 */

typedef struct {
  double sigmaInit;
	double sigmaStop;
	double sigma; 

	ekCMAConstants constants;

	double* sigmaPath;  /* Evolution path for sigma                            */
	double* cPath;      /* Evolution path for covariance matrix C              */

	int hasCustomCov;
	ekMatrix C;         /* Covariance matrix                                   */
	double* D;

	double* tmpVector;  /* Intermediate results storage                        */
} ekSepCMA;



extern const ekDistributionDelegate 
ekSepCMA_DistributionDelegate;



extern void
ekSepCMA_init(ekSepCMA* self, size_t N);



extern void
ekSepCMA_destroy(ekSepCMA* self);



extern void
ekSepCMA_setSigma(ekSepCMA* self, double sigmaInit, double sigmaStop);



extern void
ekSepCMA_setC(ekSepCMA* self, const ekMatrix* C);



extern void
ekSepCMA_setOptimizer(ekSepCMA* self, ekOptimizer* optim);



extern double
ekSepCMA_sigma(const ekSepCMA* self);



extern const ekMatrix*
ekSepCMA_C(const ekSepCMA* self);



extern const double*
ekSepCMA_D(const ekSepCMA* self);



#ifdef __cplusplus
}
#endif

#endif /* ESKIT_SEP_CMA_H */
