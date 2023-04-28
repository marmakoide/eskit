/*
 * Copyright (c) 2009-2023 Alexandre Devert <marmakoide@hotmail.fr>
 *
 * ESKit is free software; you can redistribute it and/or modify it under the 
 * terms of the MIT license. See LICENSE for details.
 */

#include <math.h>
#include "eskit/ArrayOps.h"



void
ekArrayOpsD_copy(double* u, const double* v, size_t size) {
	size_t i;

	for(i = size; i != 0; --i, ++u, ++v)
		(*u) = (*v);
}



void
ekArrayOpsD_copyMul(double* u, const double* v, size_t size, double alpha) {
	size_t i;

	for(i = size; i != 0; --i, ++u, ++v)
		(*u) = alpha * (*v);
}



void
ekArrayOpsD_copyDiv(double* u, const double* v, size_t size, double alpha) {
	size_t i;

	for(i = size; i != 0; --i, ++u, ++v)
		(*u) = (*v) / alpha;
}



void
ekArrayOpsD_fill(double* u, size_t size, double alpha) {
	size_t i;

	for(i = size; i != 0; --i, ++u)
		(*u) = alpha;
}



void
ekArrayOpsD_sqrt(double* u, size_t size) {
	size_t i;

	for(i = size; i != 0; --i, ++u)
		(*u) = sqrt(*u);
}



void
ekArrayOpsD_convolve(double* u, const double* v, size_t size) {
	size_t i;

	for(i = size; i != 0; --i, ++u, ++v)
		(*u) *= (*v);	
}



void
ekArrayOpsD_scalarMul(double* u, size_t size, double alpha) {
	size_t i;

	for(i = size; i != 0; --i, ++u)
		(*u) *= alpha;	
}



void
ekArrayOpsD_scalarDiv(double* u, size_t size, double alpha) {
	size_t i;

	for(i = size; i != 0; --i, ++u)
		(*u) /= alpha;	
}



void
ekArrayOpsD_inc(double* u, const double* v, size_t size) {
	size_t i;

	for(i = size; i != 0; --i, ++u, ++v)
		(*u) += (*v);
}



void
ekArrayOpsD_dec(double* u, const double* v, size_t size) {
	size_t i;

	for(i = size; i != 0; --i, ++u, ++v)
		(*u) -= (*v);
}



void
ekArrayOpsD_incMul(double* u, const double* v, size_t size, double alpha) {
	size_t i;

	for(i = size; i != 0; --i, ++u, ++v)
		(*u) += alpha * (*v);
}



double
ekArrayOpsD_dot(const double* u, const double* v, size_t size) {
	size_t i;
	double acc;

	acc = (*u) * (*v);
	for(i = size - 1, ++u, ++v; i != 0; --i, ++u, ++v)
		acc += (*u) * (*v);
	
	return acc;
}



double
ekArrayOpsD_sum(const double* u, size_t size) {
	size_t i;
	double acc;

	acc = *u;
	for(i = size - 1, ++u; i != 0; --i, ++u)
		acc += (*u);
	
	return acc;
}



double
ekArrayOpsD_squareSum(const double* u, size_t size) {
	size_t i;
	double acc;

	acc = (*u) * (*u);
	for(i = size - 1, ++u; i != 0; --i, ++u)
		acc += (*u) * (*u);
	
	return acc;
}



double
ekArrayOpsD_absSum(const double* u, size_t size) {
	size_t i;
	double acc;

	acc = fabs(*u);
	for(i = size - 1, ++u; i != 0; --i, ++u)
		acc += fabs(*u);
	
	return acc;
}



double
ekArrayOpsD_min(const double* u, size_t size) {
	size_t i;
	double acc;

	acc = *u;
	for(i = size - 1, ++u; i != 0; --i, ++u)
		acc = fmin(acc, *u);
	
	return acc;	
}



double
ekArrayOpsD_max(const double* u, size_t size) {
	size_t i;
	double acc;

	acc = *u;
	for(i = size - 1, ++u; i != 0; --i, ++u)
		acc = fmax(acc, *u);
	
	return acc;	
}



void
ekArrayOpsD_gaussian(double* u, size_t size, ekRandomizer* randomizer, double sigma) {
	size_t i;

	for(i = size; i != 0; --i, ++u)
		*u = ekRandomizer_nextGaussian(randomizer, sigma);
}



void
ekArrayOpsD_uniform(double* u, size_t size, ekRandomizer* randomizer, double a, double b) {
	size_t i;
	double min, factor;

	min = fmin(a, b);
	factor = fabs(a - b) / INT32_MAX;

	for(i = size; i != 0; --i, ++u)
		*u = factor * ekRandomizer_next(randomizer) + min;	
}



void
ekArrayOpsD_print(double* u, size_t size, FILE* file) {
	size_t i;

	fprintf(file, "[%e", *u);
	++u;

	for(i = size - 1; i != 0; --i, ++u)
		fprintf(file, "; %e", *u);

	fprintf(file, "]\n");
	fflush(file);
}

