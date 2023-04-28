/*
 * Copyright (c) 2010 Alexandre Devert <marmakoide@yahoo.fr>
 *
 * ESKit is free software; you can redistribute it and/or modify it under the 
 * terms of the MIT license. See LICENSE for details.
 */

#include <stdlib.h>
#include "eskit/Macros.h"
#include "eskit/EigenSolver.h"



#ifdef USE_LAPACK
/* --- LAPACK based implementation ----------------------------------------- */

void
ekEigenSolver_init(ekEigenSolver* self, size_t size) {
	self->JOBZ = 'V';
	self->UPLO = 'U';
	self->N = size;
	self->LDA = size;
	self->LWORK = size * size;

	self->scratchMem = newArray(double, size * size);
}



void
ekEigenSolver_destroy(ekEigenSolver* self) {
	free(self->scratchMem);
}



void
dsyev_(char*, char*, int*, double*, int*, double*, double*, int*, int*);

int
ekEigenSolver_solve(ekEigenSolver* self, const ekMatrix* M, ekMatrix* vectors, double* values) {
	int ret;

	ekMatrix_copy(vectors, M);
	dsyev_(&(self->JOBZ), 
				 &(self->UPLO), 
         &(self->N), 
				 vectors->tuple, 
				 &(self->LDA), 
         values, 
         self->scratchMem, 
				 &(self->LWORK), 
				&ret);

	return ret == 0;
}



#else
/* --- Householder & QL based implementation ------------------------------- */

void
ekEigenSolver_init(ekEigenSolver* self, size_t size) {
	self->scratchMem = newArray(double, size);
}



void
ekEigenSolver_destroy(ekEigenSolver* self) {
	free(self->scratchMem);
}



int
ekEigenSolver_solve(ekEigenSolver* self, const ekMatrix* M, ekMatrix* vectors, double* values) {
	ekMatrix_copy(vectors, M);
	ekMatrix_algo_Householder(vectors, values, self->scratchMem);
  ekMatrix_algo_QL(vectors, values, self->scratchMem);

	return 1;
}

#endif /* #ifdef USE_LAPACK */

