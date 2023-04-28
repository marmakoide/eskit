/*
 * Copyright (c) 2010 Alexandre Devert <marmakoide@yahoo.fr>
 *
 * ESKit is free software; you can redistribute it and/or modify it under the 
 * terms of the MIT license. See LICENSE for details.
 */

#ifndef ESKIT_CHOLESKY_H
#define ESKIT_CHOLESKY_H

#ifdef __cplusplus
extern "C" {
#endif



#include <eskit/Matrix.h>



typedef struct {
#ifdef USE_LAPACK
	char UPLO;
	int N;
	int LDA;
#else
	size_t N;
	double* diagonal;
#endif
} ekCholesky;



extern void
ekCholesky_init(ekCholesky* self, size_t size);



extern void
ekCholesky_destroy(ekCholesky* self);



extern int
ekCholesky_solve(ekCholesky* self, ekMatrix* A);



#ifdef __cplusplus
}
#endif

#endif /* ESKIT_CHOLESKY_H */
