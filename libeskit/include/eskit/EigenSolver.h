/*
 * Copyright (c) 2009-2023 Alexandre Devert <marmakoide@hotmail.fr>
 *
 * ESKit is free software; you can redistribute it and/or modify it under the 
 * terms of the MIT license. See LICENSE for details.
 */

#ifndef ESKIT_EIGEN_SOLVER_H
#define ESKIT_EIGEN_SOLVER_H

#ifdef __cplusplus
extern "C" {
#endif



#include <eskit/Matrix.h>



typedef struct {
#ifdef USE_LAPACK
	char JOBZ;
	char UPLO;
	int N;
	int LDA;
	int LWORK;
#endif

	double* scratchMem;
} ekEigenSolver;



extern void
ekEigenSolver_init(ekEigenSolver* self, size_t size);



extern void
ekEigenSolver_destroy(ekEigenSolver* self);



extern int
ekEigenSolver_solve(ekEigenSolver* self, const ekMatrix* M, ekMatrix* vectors, double* values);



#ifdef __cplusplus
}
#endif

#endif /* ESKIT_EIGEN_SOLVER_H */
