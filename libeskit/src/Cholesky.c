/*
 * Copyright (c) 2009-2023 Alexandre Devert <marmakoide@hotmail.fr>
 *
 * ESKit is free software; you can redistribute it and/or modify it under the 
 * terms of the MIT license. See LICENSE for details.
 */

#include <math.h>
#include <stdlib.h>
#include "eskit/Macros.h"
#include "eskit/Cholesky.h"



#ifdef USE_LAPACK
/* --- LAPACK based implementation ----------------------------------------- */

void
ekCholesky_init(ekCholesky* self, size_t size) {
	self->UPLO = 'U';
	self->N = size;
	self->LDA = size;
}



void
ekCholesky_destroy(ekCholesky* ESKIT_UNUSED(self)) { }



void
dpotrf_(char*, int*, double*, int*, int*);

int
ekCholesky_solve(ekCholesky* self, ekMatrix* A) {
	int ret;

	dpotrf_(&(self->UPLO), 
          &(self->N), 
				  A->tuple,
					&(self->LDA), 
					&ret);

	return ret == 0;
}



#else
/* --- Fallback implementation --------------------------------------------- */

void
ekCholesky_init(ekCholesky* self, size_t size) {
	self->N = size;
	self->diagonal = newArray(double, size);
}



void
ekCholesky_destroy(ekCholesky* self) {
	free(self->diagonal);
}



int
ekCholesky_solve(ekCholesky* self, ekMatrix* A) {
	size_t i, j, k;
  double sum;
     
	for(i = 0; i < self->N; ++i) {
		for(j = i; j < self->N; ++j) {
			sum = A->cols[i][j];
    	for(k = 0; k < i; ++k) 
				sum -= A->cols[i][k] * A->cols[j][k];

			if (i == j) {
        if (sum <= 0.0)
					return 0;
 
       	self->diagonal[i] = sqrt(sum);
			}
			else {
				A->cols[j][i] = sum / self->diagonal[i];
				A->cols[i][j] = A->cols[j][i];
			}     
		}
	}

	for(i = 0; i < self->N; i++)
		A->cols[i][i] = self->diagonal[i];
     
	return 1;    
}
   
#endif /* #ifdef USE_LAPACK */

