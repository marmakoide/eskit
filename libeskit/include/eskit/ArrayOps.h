/*
 * Copyright (c) 2010 Alexandre Devert <marmakoide@yahoo.fr>
 *
 * ESKit is free software; you can redistribute it and/or modify it under the 
 * terms of the MIT license. See LICENSE for details.
 */

#ifndef ESKIT_ARRAY_OPS_H
#define ESKIT_ARRAY_OPS_H

#ifdef __cplusplus
extern "C" {
#endif



#include <stdio.h>
#include <eskit/Randomizer.h>



/* Computes U = V */
extern void
ekArrayOpsD_copy(double* u, const double* v, size_t size);



/* Computes U = alpha * V */
extern void
ekArrayOpsD_copyMul(double* u, const double* v, size_t size, double value);



/* Computes U = (1.0 / alpha) * V */
extern void
ekArrayOpsD_copyDiv(double* u, const double* v, size_t size, double value);



/* Computes U[i] = alpha */
extern void
ekArrayOpsD_fill(double* u, size_t size, double alpha);



/* Compute U[i] = sqrt(U[i]) */
void
ekArrayOpsD_sqrt(double* u, size_t size);



/* Computes U[i] = U[i] * V[i] */
void
ekArrayOpsD_convolve(double* u, const double* v, size_t size);



/* Computes U = alpha * U */
extern void
ekArrayOpsD_scalarMul(double* u, size_t size, double alpha);



/* Computes U = (1.0 / alpha) * U */
extern void
ekArrayOpsD_scalarDiv(double* u, size_t size, double alpha);



/* Computes U = U + V */
extern void
ekArrayOpsD_inc(double* u, const double* v, size_t size);



/* Computes U = U - V */
extern void
ekArrayOpsD_dec(double* u, const double* v, size_t size);



/* Computes U = U + alpha * V */
extern void
ekArrayOpsD_incMul(double* u, const double* v, size_t size, double alpha);



/* Returns U.V */
extern double
ekArrayOpsD_dot(const double* u, const double* v, size_t size);



/* Returns sum of U[i] */
extern double
ekArrayOpsD_sum(const double* u, size_t size);



/* Returns sum of U[i]^2 */
extern double
ekArrayOpsD_squareSum(const double* u, size_t size);



/* Returns sum of |U[i]| */
extern double
ekArrayOpsD_absSum(const double* u, size_t size);



/* Returns minimun of U[i] */
extern double
ekArrayOpsD_min(const double* u, size_t size);



/* Returns maximum of U[i] */
extern double
ekArrayOpsD_max(const double* u, size_t size);



/* Fills U with normally distributed values */
extern void
ekArrayOpsD_gaussian(double* u, size_t size, ekRandomizer* randomizer, double sigma);



/* Fills U with uniformaly distributed values in the [min(a, b), max(a, b)] range */
extern void
ekArrayOpsD_uniform(double* u, size_t size, ekRandomizer* randomizer, double a, double b);



/* Print U in a human readable way ie. Octave notation */
extern void
ekArrayOpsD_print(double* u, size_t size, FILE* file);



#ifdef __cplusplus
}
#endif

#endif /* ESKIT_ARRAY_OPS_H */
