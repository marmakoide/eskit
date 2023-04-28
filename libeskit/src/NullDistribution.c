/*
 * Copyright (c) 2010 Alexandre Devert <marmakoide@yahoo.fr>
 *
 * ESKit is free software; you can redistribute it and/or modify it under the 
 * terms of the MIT license. See LICENSE for details.
 */

#include <stdlib.h>
#include "eskit/Macros.h"
#include "eskit/Matrix.h"
#include "eskit/ArrayOps.h"
#include "eskit/Optimizer.h"
#include "eskit/NullDistribution.h"



static void
ekNullDistribution_delegate_start(ekDistribution* ESKIT_UNUSED(self), ekOptimizer* ESKIT_UNUSED(optim)) {
	return;
}



static void
ekNullDistribution_delegate_update(ekDistribution* ESKIT_UNUSED(self), ekOptimizer* ESKIT_UNUSED(optim)) {
	return;
}



static void
ekNullDistribution_delegate_samplePoint(ekDistribution* ESKIT_UNUSED(self), ekOptimizer* optim, size_t ESKIT_UNUSED(id), double* x, double* ESKIT_UNUSED(z)) {
	ekArrayOpsD_fill(x, ekOptimizer_N(optim), 0.0);
}



static void
ekNullDistribution_delegate_sampleCloud(ekDistribution* ESKIT_UNUSED(self), ekOptimizer* ESKIT_UNUSED(optim), ekMatrix* x, ekMatrix* ESKIT_UNUSED(z)) {
	ekMatrix_fill(x, 0.0);
}



enum ekStopCriterionId
ekNullDistribution_delegate_stop(ekDistribution* ESKIT_UNUSED(self), ekOptimizer* ESKIT_UNUSED(optim)) {
	return ekStopCriterionId_DistributionNotSet;
}



const ekDistributionDelegate 
ekNullDistribution_DistributionDelegate =
{
	"null",
	ekNullDistribution_delegate_start,
	ekNullDistribution_delegate_update,
	ekNullDistribution_delegate_samplePoint,
	ekNullDistribution_delegate_sampleCloud,
	ekNullDistribution_delegate_stop
};
