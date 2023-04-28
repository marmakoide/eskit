/*
 * Copyright (c) 2009-2023 Alexandre Devert <marmakoide@hotmail.fr>
 *
 * ESKit is free software; you can redistribute it and/or modify it under the 
 * terms of the MIT license. See LICENSE for details.
 */

#include <stdlib.h>
#include <math.h>
#include "eskit/Macros.h"
#include "eskit/Matrix.h"
#include "eskit/ArrayOps.h"



void
ekMatrix_init(ekMatrix* self, size_t nbCols, size_t nbRows) {
	size_t i;
	double* offset;

	self->nbCols = nbCols;
	self->nbRows = nbRows;
	self->tupleSize = nbCols * nbRows;

	self->cols = newArray(double*, nbCols);
	self->tuple = newArray(double, self->tupleSize);
	
	offset = self->tuple;
	for(i = 0; i < nbCols; ++i, offset += nbRows)
		self->cols[i] = offset;
}



void
ekMatrix_destroy(ekMatrix* self) {
	free(self->cols);
	free(self->tuple);
}



void
ekMatrix_copy(ekMatrix* self, const ekMatrix* u) {
	ekArrayOpsD_copy(self->tuple, u->tuple, self->tupleSize);
}



void
ekMatrix_copyMul(ekMatrix* self, const ekMatrix* u, double alpha) {
	ekArrayOpsD_copyMul(self->tuple, u->tuple, self->tupleSize, alpha);
}



void
ekMatrix_fill(ekMatrix* self, double alpha) {
	ekArrayOpsD_fill(self->tuple, self->tupleSize, alpha);
}



void
ekMatrix_scalarMul(ekMatrix* self, double alpha) {
	ekArrayOpsD_scalarMul(self->tuple, self->tupleSize, alpha);
}



void
ekMatrix_scalarDiv(ekMatrix* self, double alpha) {
	ekArrayOpsD_scalarDiv(self->tuple, self->tupleSize, alpha);
}



void
ekMatrix_diagMul(ekMatrix* self, const double* u) {
	size_t i;
	double* col;

	col = self->tuple;
	for(i = self->nbCols; i != 0; --i, col += self->nbRows)
		ekArrayOpsD_convolve(col, u, self->nbRows);
}



extern void
ekMatrix_inc(ekMatrix* self, const ekMatrix* u) {
	ekArrayOpsD_inc(self->tuple, u->tuple, self->tupleSize);
}



extern void
ekMatrix_dec(ekMatrix* self, const ekMatrix* u) {
	ekArrayOpsD_dec(self->tuple, u->tuple, self->tupleSize);
}



void
ekMatrix_incMul(ekMatrix* self, const ekMatrix* u, double alpha) {
	ekArrayOpsD_incMul(self->tuple, u->tuple, self->tupleSize, alpha);
}



void
ekMatrix_incMulCross(ekMatrix* self, const double* u, double alpha) {
	size_t i;
	double* col;
	const double* uT;

	uT = u;
	col = self->tuple;
	for(i = self->nbCols; i != 0; --i, ++u, col += self->nbCols)
		ekArrayOpsD_incMul(col, uT, self->nbCols, alpha * (*u));
}



void
ekMatrix_vectorProd(ekMatrix* self, const double* u, double* v) {
	size_t i;
	double* col;

	col = self->tuple;
	ekArrayOpsD_copyMul(v, col, self->nbRows, *u);

	col += self->nbRows;
	++u;
	for(i = self->nbCols - 1; i != 0; --i, ++u, col += self->nbRows)
		ekArrayOpsD_incMul(v, col, self->nbRows, *u);
}



void
ekMatrix_diagProd(ekMatrix* self, const double* u, ekMatrix* v) {
	size_t i;

	for(i = 0; i < self->nbCols; ++i)
		ekArrayOpsD_copyMul(v->cols[i], self->cols[i], self->nbRows, u[i]);
}



void
ekMatrix_matrixProd(ekMatrix* self, const ekMatrix* u, ekMatrix* v) {
	size_t i;

	for(i = 0; i < u->nbCols; ++i)
		ekMatrix_vectorProd(self, u->cols[i], v->cols[i]);
}



void
ekMatrix_transpose(ekMatrix* self) {
	double tmp;
	size_t i, j;

	for(i = 0; i < self->nbCols; ++i) {
		for(j = i + 1; j < self->nbRows; ++j) {
			swap(self->cols[i][j], self->cols[j][i], tmp);
		}
	}
}



void
ekMatrix_getDiagonal(const ekMatrix* self, double* u) {
	size_t i;
	const double* offset;
	
	offset = self->tuple;
	for(i = self->nbCols; i != 0; --i, offset += self->nbRows + 1, ++u)
		(*u) = (*offset);
}



void
ekMatrix_setDiagonal(ekMatrix* self, const double* u) {
	size_t i;
	double* offset;
	
	offset = self->tuple;
	for(i = self->nbCols; i != 0; --i, offset += self->nbRows + 1, ++u)
		(*offset) = (*u);	
}



void
ekMatrix_setAsIdentity(ekMatrix* self) {
	size_t i;
	double* offset;

	ekArrayOpsD_fill(self->tuple, self->tupleSize, 0.0);
	
	offset = self->tuple;
	for(i = self->nbCols; i != 0; --i, offset += self->nbRows + 1)
		(*offset) = 1.0;
}



void
ekMatrix_setAsGaussian(ekMatrix* self, ekRandomizer* randomizer, double sigma) {
	ekArrayOpsD_gaussian(self->tuple, self->tupleSize, randomizer, sigma);
}



void
ekMatrix_setAsRandomRotation(ekMatrix* self, ekRandomizer* randomizer) {
	size_t i, j;
	double sum;

	ekMatrix_setAsGaussian(self, randomizer, 1.0);

	/* Orthonormalize the matrix */
  /* TODO : Graham-Smidth orthogonalization is not stable, use Householder algo instead */
	for(i = 0; i < self->nbCols; i++) {
		for (j = 0; j < i; j++) {
			sum = ekArrayOpsD_dot(self->cols[i], self->cols[j], self->nbRows);
			ekArrayOpsD_incMul(self->cols[i], self->cols[j], self->nbRows, -sum);
    }

		sum = ekArrayOpsD_squareSum(self->cols[i], self->nbRows);
		ekArrayOpsD_scalarDiv(self->cols[i], self->nbRows, sqrt(sum));
  }
}



void
ekMatrix_print(ekMatrix* self, FILE* file) {
	size_t i, j;

	for(i = 0; i < self->nbRows; ++i) {
		fprintf(file, "%e", self->cols[0][i]);
		for(j = 1; j < self->nbCols; ++j) {
			fprintf(file, " %e", self->cols[j][i]);
		}
		fprintf(file, "\n");
	}

	fprintf(file, "\n");
	fflush(file);
}
