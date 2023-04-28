/*
 * Copyright (c) 2010 Alexandre Devert <marmakoide@yahoo.fr>
 *
 * ESKit is free software; you can redistribute it and/or modify it under the 
 * terms of the MIT license. See LICENSE for details.
 */

#ifndef ESKIT_MATRIX_H
#define ESKIT_MATRIX_H

#ifdef __cplusplus
extern "C" {
#endif



#include <stdio.h>
#include <eskit/Types.h>
#include <eskit/Randomizer.h>



/*
   Implements a dense, column-major matrix
 */

struct s_ekMatrix {
	size_t nbCols;
	size_t nbRows;
	size_t tupleSize;

	double* tuple;
	double** cols;
};



#define ekMatrix_at(self, col, row) (self)->cols[(col)][(row)]



#define ekMatrix_col(self, col) (self)->cols[(col)]



#define ekMatrix_nbCols(self) (self)->nbCols



#define ekMatrix_nbRows(self) (self)->nbRows



extern void
ekMatrix_init(ekMatrix* self, size_t nbCols, size_t nbRows);



extern void
ekMatrix_destroy(ekMatrix* self);



/* Computes self = U */
extern void
ekMatrix_copy(ekMatrix* self, const ekMatrix* u);



/* Computes self = alpha * U */
extern void
ekMatrix_copyMul(ekMatrix* self, const ekMatrix* u, double alpha);



/* Computes self[i][j] = alpha */
extern void
ekMatrix_fill(ekMatrix* self, double alpha);



/* Computes self = alpha * self */
extern void
ekMatrix_scalarMul(ekMatrix* self, double alpha);



/* Computes self = (1.0 / alpha) * self */
extern void
ekMatrix_scalarDiv(ekMatrix* self, double alpha);



/* Computes self = U * self where U is a diagonal matrix */
extern void
ekMatrix_diagMul(ekMatrix* self, const double* u);



/* Computes self = self + U */
extern void
ekMatrix_inc(ekMatrix* self, const ekMatrix* u);



/* Computes self = self - U */
extern void
ekMatrix_dec(ekMatrix* self, const ekMatrix* u);



/* Computes self = self + alpha * U */
extern void
ekMatrix_incMul(ekMatrix* self, const ekMatrix* u, double alpha);



/* Computes self = self + alpha * U * U^t */
extern void
ekMatrix_incMulCross(ekMatrix* self, const double* u, double alpha);



/* Computes V = self * U */
extern void
ekMatrix_vectorProd(ekMatrix* self, const double* u, double* v);



/* Computes V = self * U where U is a diagonal matrix */
void
ekMatrix_diagProd(ekMatrix* self, const double* u, ekMatrix* v);



/* Computes V = self * U */
extern void
ekMatrix_matrixProd(ekMatrix* self, const ekMatrix* u, ekMatrix* v);



/* Computes self = self^t */
extern void
ekMatrix_transpose(ekMatrix* self);



/* Computes U[i] = self[i][i] */
extern void
ekMatrix_getDiagonal(const ekMatrix* self, double* u);



/* Computes self[i][i] = U[i][i] */
extern void
ekMatrix_setDiagonal(ekMatrix* self, const double* u);



/* Set as identity matrix */
extern void
ekMatrix_setAsIdentity(ekMatrix* self);



extern void
ekMatrix_setAsGaussian(ekMatrix* self, ekRandomizer* randomizer, double sigma);



/* Set as a rotation matrix, uniformaly sampled in SO(N) */
extern void
ekMatrix_setAsRandomRotation(ekMatrix* self, ekRandomizer* randomizer);



extern void
ekMatrix_algo_QL(ekMatrix* self, double* d, double* e);



extern void
ekMatrix_algo_Householder(ekMatrix* self, double* d, double* e);



/* Human readable output of a matrix */
extern void
ekMatrix_print(ekMatrix* self, FILE* file);



#ifdef __cplusplus
}
#endif

#endif /* ESKIT_ARRAY_OPS_H */
