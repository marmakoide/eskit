/*
 * Copyright (c) 2010 Alexandre Devert <marmakoide@yahoo.fr>
 *
 * ESKit is free software; you can redistribute it and/or modify it under the 
 * terms of the MIT license. See LICENSE for details.
 */

#include <math.h>
#include <stddef.h>
#include "eskit/Matrix.h"
#include "eskit/ArrayOps.h"



/* sqrt(a^2 + b^2) numerically stable. */

static double
stable_hypot(double a, double b) {
	double ret;

	ret = 0.0;

	if (fabs(a) > fabs(b)) {
		ret = b / a;
		ret = fabs(a) * sqrt(1 + ret * ret);
	} else if (b != 0) {
		ret = a / b;
		ret = fabs(b) * sqrt(1 + ret * ret);
	}

	return ret;
}



void 
ekMatrix_algo_QL(ekMatrix* self, double *d, double *e) {
  /*
    -> n     : Dimension. 
    -> d     : Diagonale of tridiagonal matrix. 
    -> e[1..n-1] : off-diagonal, output from Householder
    -> V     : matrix output von Householder
    <- d     : eigenvalues
    <- e     : garbage?
    <- V     : basis of eigenvectors, according to d

    Symmetric tridiagonal QL algorithm, iterative 
    Computes the eigensystem from a tridiagonal matrix in roughtly 3N^3 operations
    
    code adapated from Java JAMA package, function tql2. 
  */

	int n = self->nbCols;
  int i, k, l, m;
  double f = 0.0;
  double tst1 = 0.0;
  double eps = 2.22e-16; /* Math.pow(2.0,-52.0);  == 2.22e-16 */
  
      /* shift input e */
      for (i = 1; i < n; i++) {
         e[i-1] = e[i];
      }
      e[n-1] = 0.0; /* never changed again */
   
      for (l = 0; l < n; l++) { 

	/* Find small subdiagonal element */
   
	 if (tst1 < fabs(d[l]) + fabs(e[l]))
	   tst1 = fabs(d[l]) + fabs(e[l]);
         m = l;
         while (m < n) {
	   if (fabs(e[m]) <= eps*tst1) {
	     /* if (fabs(e[m]) + fabs(d[m]+d[m+1]) == fabs(d[m]+d[m+1])) { */
               break;
            }
            m++;
         }
   
         /* If m == l, d[l] is an eigenvalue, */
         /* otherwise, iterate. */
   
         if (m > l) { 
            int iter = 0;
            do { /* while (fabs(e[l]) > eps*tst1); */
	       double dl1, h;
               double g = d[l];
               double p = (d[l+1] - g) / (2.0 * e[l]); 
	       double r = stable_hypot(p, 1.); 

	       iter = iter + 1;  /* Could check iteration count here */
   
               /* Compute implicit shift */
   
               if (p < 0) {
                  r = -r;
               }
               d[l] = e[l] / (p + r);
               d[l+1] = e[l] * (p + r);
               dl1 = d[l+1];
               h = g - d[l];
               for (i = l+2; i < n; i++) {
                  d[i] -= h;
               }
               f = f + h;
   
               /* Implicit QL transformation. */
               p = d[m];
	     {
               double c = 1.0;
               double c2 = c;
               double c3 = c;
               double el1 = e[l+1];
               double s = 0.0;
               double s2 = 0.0;
               for (i = m-1; i >= l; i--) {
                  c3 = c2;
                  c2 = c;
                  s2 = s;
                  g = c * e[i];
                  h = c * p;
		  r = stable_hypot(p, e[i]);
                  e[i+1] = s * r;
                  s = e[i] / r;
                  c = p / r;
                  p = c * d[i] - s * g;
                  d[i+1] = h + s * (c * g + s * d[i]);
   
                  /* Accumulate transformation. */
                  for (k = 0; k < n; k++) {
                     h = self->cols[i+1][k];
                     self->cols[i+1][k] = s * self->cols[i][k] + c * h;
                     self->cols[i][k] = c * self->cols[i][k] - s * h;
                  }
               }
               p = -s * s2 * c3 * el1 * e[l] / dl1;
               e[l] = s * p;
               d[l] = c * p;
             }
   
               /* Check for convergence. */
   
            } while (fabs(e[l]) > eps*tst1);
         }
         d[l] = d[l] + f;
         e[l] = 0.0;
      }
}


/*
   Householder transformation of a symmetric matrix into tridiagonal form

 <- d             : diagonal
 <- e[0..n-1]     : off diagonal (elements 1..n-1) 

 code slightly adapted from the Java JAMA package, function private tred2()  
*/

void 
ekMatrix_algo_Householder(ekMatrix* self, double *d, double *e) {
  size_t i, j, k, n;
	double f, g, h, scale;

	n = self->nbCols;

  for(j = 0; j < n; j++)
  	d[j] = self->cols[j][n - 1];

  /* Householder reduction to tridiagonal form */
  for(i = n - 1; i > 0; i--) {
		/* Scale to avoid under/overflow */
		h = 0.0;
		scale = ekArrayOpsD_absSum(d, i);
    if (scale == 0.0) {
    	e[i] = d[i - 1];
      for(j = 0; j < i; j++) {
      	d[j] = self->cols[i - 1][j];
        self->cols[j][i] = 0.0;
        self->cols[i][j] = 0.0;
      }
    } 
		else {
	  	/* Generate Householder vector */
			ekArrayOpsD_scalarDiv(d, i, scale);
			h = ekArrayOpsD_squareSum(d, i);

      f = d[i - 1];
      g = sqrt(h);
      if (f > 0.0)
      	g = -g;
 
      e[i] = scale * g;
      h = h - f * g;
      d[i - 1] = f - g;

   		ekArrayOpsD_fill(e, i, 0.0);

      /* Apply similarity transformation to remaining columns */
      for(j = 0; j < i; j++) {
      	f = d[j];
        self->cols[i][j] = f;
        g = e[j] + self->cols[j][j] * f;

        for(k = j + 1; k <= i-1; k++) {
        	g += self->cols[j][k] * d[k];
          e[k] += self->cols[j][k] * f;
        }

        e[j] = g;
      }

			ekArrayOpsD_scalarDiv(e, i, h);
			f = ekArrayOpsD_dot(e, d, i);

			ekArrayOpsD_incMul(e, d, i, -f / (2.0 * h));

     	for(j = 0; j < i; j++) {
        ekArrayOpsD_incMul(self->cols[j] + j, e + j, i - j, -d[j]);
   			ekArrayOpsD_incMul(self->cols[j] + j, d + j, i - j, -e[j]);

        d[j] = self->cols[j][i-1];
        self->cols[j][i] = 0.0;
      }
    }
    d[i] = h;
  }
   
  /* Accumulate transformations */
  for(i = 0; i < n - 1; i++) {
  	self->cols[i][n - 1] = self->cols[i][i];
    self->cols[i][i] = 1.0;
    h = d[i + 1];

    if (h != 0.0) {
			ekArrayOpsD_copyDiv(d, self->cols[i+1], i + 1, h);
      for(j = 0; j <= i; j++) {
				g = ekArrayOpsD_dot(self->cols[i+1], self->cols[j], i + 1);
				ekArrayOpsD_incMul(self->cols[j], d, i + 1, -g);
      }
    }

		ekArrayOpsD_fill(self->cols[i + 1], i + 1, 0.0);
  }

  for(j = 0; j < n; j++) {
  	d[j] = self->cols[j][n-1];
    self->cols[j][n-1] = 0.0;
  }

  self->cols[n-1][n-1] = 1.0;
  e[0] = 0.0;
}
