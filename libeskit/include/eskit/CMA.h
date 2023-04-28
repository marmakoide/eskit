/*
 * Copyright (c) 2009-2023 Alexandre Devert <marmakoide@hotmail.fr>
 *
 * ESKit is free software; you can redistribute it and/or modify it under the 
 * terms of the MIT license. See LICENSE for details.
 */

#ifndef ESKIT_CMA_H
#define ESKIT_CMA_H

#ifdef __cplusplus
extern "C" {
#endif



#include <eskit/EigenSolver.h>
#include <eskit/Distribution.h>
#include <eskit/CMAConstants.h>



/*
   Implements a rotated & scaled Gaussian distribution, with the Covariance 
	 Matrix Adaption heuristic for the covariance matrix and step-size adaption.

	 The stopping criterion is satisfied when numerical precision issues prevents
   to make any progress.	

	 This implementation is a close adaptation of 'purecmaes.m' and 'cmaes.m' by
   Nikolaus Hansen et al.
 */

typedef struct {
  double sigmaInit;
	double sigmaStop;
	double sigma; 

	ekCMAConstants constants;

	double* sigmaPath;    /* Evolution path for sigma                          */
	double* cPath;        /* Evolution path for covariance matrix C            */

	int hasCustomCov;
	ekMatrix B, C, BD;    /* Covariance matrix and its decomposition           */
	double* D;

	double* tmpVector;    /* Intermediate results storage                      */

	int eigenSolverFailure;
	size_t eigenUpdatePeriod;
	ekEigenSolver eigenSolver;
} ekCMA;



extern const ekDistributionDelegate 
ekCMA_DistributionDelegate;



extern void
ekCMA_init(ekCMA* self, size_t N);



extern void
ekCMA_destroy(ekCMA* self);



extern void
ekCMA_setSigma(ekCMA* self, double sigmaInit, double sigmaStop);



extern void
ekCMA_setC(ekCMA* self, const ekMatrix* C);



extern void
ekCMA_setOptimizer(ekCMA* self, ekOptimizer* optim);



extern double
ekCMA_sigma(const ekCMA* self);



extern const ekMatrix*
ekCMA_C(const ekCMA* self);



extern const ekMatrix*
ekCMA_B(const ekCMA* self);



extern const double*
ekCMA_D(const ekCMA* self);



#ifdef __cplusplus
}
#endif

#endif /* ESKIT_CMA_H */
