/*
 * Copyright (c) 2009-2023 Alexandre Devert <marmakoide@hotmail.fr>
 *
 * ESKit is free software; you can redistribute it and/or modify it under the 
 * terms of the MIT license. See LICENSE for details.
 */

#include <math.h>
#include <string.h>
#include "Function.h"



#ifndef M_PI
  #define M_PI 3.14159265358979323846
#endif



static double sqr(double x) { return x * x; }



/* --- Sphere function ----------------------------------------------------- */

static double
SphereFunction_compute(const double* u, size_t size) {
	size_t i;
	double sum;

	sum = 0.0;
	for(i = size; i != 0; --i, ++u)
		sum += sqr(u[0]);

	return sum;
}

Function SphereFunction =
{
	"sphere", -3.0, 7.0, SphereFunction_compute
};



/* --- Ellipsoid function -------------------------------------------------- */

const double EllipsoidFunction_Factor = 100.0;

static double
EllipsoidFunction_compute(const double* u, size_t size) {
	size_t i;
	double sum, factor;
	
	factor = 1.0 / (size - 1);

	sum = 0.0;
	for(i = 0; i < size; ++i)
		sum += pow(EllipsoidFunction_Factor, factor * i) * sqr(u[i]);

	return sum;
}

Function EllipsoidFunction =
{
	"ellipsoid", -3.0, 7.0, EllipsoidFunction_compute
};



/* --- Cigar function ------------------------------------------------------ */

const double CigarFunction_Factor = 10000.0;

static double
CigarFunction_compute(const double* u, size_t size) {
	size_t i;
	double a, sum;

	a = sqr(*u);

	sum = 0.0;
	for(i = size - 1, ++u; i != 0; --i, ++u)
		sum += sqr(*u);

	return a + CigarFunction_Factor * sum;
}

Function CigarFunction =
{
	"cigar", -3.0, 7.0, CigarFunction_compute
};



/* --- Rosenbrock function ------------------------------------------------- */

const double RosenbrockFunction_Factor = 100.0;

static double
RosenbrockFunction_compute(const double* u, size_t size) {
	size_t i;
	double sum;

	sum = 0.0;
	for(i = size - 1; i != 0; --i, ++u)
		sum += RosenbrockFunction_Factor * sqr(sqr(u[0]) - u[1]) + sqr(u[0] - 1);

	return sum;
}

Function RosenbrockFunction =
{
	"rosenbrock", -5.0, 5.0, RosenbrockFunction_compute
};



/* --- Rastrigin function -------------------------------------------------- */

const double RastriginFunction_A = 10.0;

static double
RastriginFunction_compute(const double* u, size_t size) {
	size_t i;
	double sum;

	sum = size * RastriginFunction_A;
	for(i = size; i != 0; --i, ++u)
		sum += sqr(u[0]) - RastriginFunction_A * cos(2.0 * M_PI * (u[0]));

	return sum;
}

Function RastriginFunction =
{
	"rastrigin", -3.0, 7.0, RastriginFunction_compute
};



/* --- Ackley function ----------------------------------------------------- */

const double AckleyFunction_A = 20.0;
const double AckleyFunction_B = 0.2;
const double AckleyFunction_C = 2 * M_PI;

static double
AckleyFunction_compute(const double* u, size_t size) {
	size_t i;
	double sum1, sum2;

	sum1 = sum2 = 0.0;
	for(i = 0; i < size; ++i) {
		sum1 += sqr(u[i]);
		sum2 += cos(AckleyFunction_C * u[i]);
	}

	return 
  	 AckleyFunction_A + exp(1.0) -
		 AckleyFunction_A * exp(-AckleyFunction_B * sqrt(sum1 / size)) - 
		                    exp(sum2 / size);
}

Function AckleyFunction =
{
	"ackley", -30.0, 30.0, AckleyFunction_compute
};

/* --- Functions database -------------------------------------------------- */

const Function* FunctionsList[] =
{
	&SphereFunction,
	&EllipsoidFunction,
	&CigarFunction,
	&RosenbrockFunction,
	&RastriginFunction,
	&AckleyFunction,
	NULL
};



const Function* DefaultFunction = &SphereFunction;



const Function*
getFunctionByName(const char* name) {
	const Function** ret;

	for(ret = FunctionsList; (*ret) != NULL; ++ret) {
		if (strcmp((*ret)->name, name) == 0)
			break;
	}

	return (*ret);
}
