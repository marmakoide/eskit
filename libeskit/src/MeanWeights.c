/*
 * Copyright (c) 2009-2023 Alexandre Devert <marmakoide@hotmail.fr>
 *
 * ESKit is free software; you can redistribute it and/or modify it under the 
 * terms of the MIT license. See LICENSE for details.
 */

#include <math.h>
#include "eskit/Types.h"
#include "eskit/ArrayOps.h"
#include "eskit/MeanWeights.h"


/* --- log weighting ------------------------------------------------------- */

static void
ekLog_MeanWeightsGenerator_generate(double* weights, size_t size) {
	size_t i;

	for(i = 0; i < size; ++i)
		weights[i] = log(size + 0.5) - log(i + 1.0);
}



ekMeanWeightsGenerator ekLog_MeanWeightsGenerator =
{
	"log",
	ekLog_MeanWeightsGenerator_generate
};



/* --- equal weighting ----------------------------------------------------- */

static void
ekEqual_MeanWeightsGenerator_generate(double* weights, size_t size) {
	ekArrayOpsD_fill(weights, size, 1.0);
}



ekMeanWeightsGenerator ekEqual_MeanWeightsGenerator =
{
	"equal",
	ekEqual_MeanWeightsGenerator_generate
};



/* --- linear weighting ---------------------------------------------------- */

static void
ekLinear_MeanWeightsGenerator_generate(double* weights, size_t size) {
	size_t i;

	for(i = 0; i < size; ++i)
		weights[i] = size - i;
}



ekMeanWeightsGenerator ekLinear_MeanWeightsGenerator =
{
	"linear",
	ekLinear_MeanWeightsGenerator_generate
};
