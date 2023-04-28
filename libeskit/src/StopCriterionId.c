/*
 * Copyright (c) 2009-2023 Alexandre Devert <marmakoide@hotmail.fr>
 *
 * ESKit is free software; you can redistribute it and/or modify it under the 
 * terms of the MIT license. See LICENSE for details.
 */

#include "eskit/StopCriterionId.h"



static const char* 
ekStopCriterionStringArray[] =
{
	"None",
	"NoDistributionSet",
	"LowSigma",
	"NoEffectAxis",
	"NoEffectCoord",
	"ConditionCov",
	"EigenSolverFailure",
	"BestFitnessStall"
};



static const char*
ekStopCriterionStringUndefined = "Undefined";



const char*
ekStopCriterionString(enum ekStopCriterionId id) {
	if (id >= sizeof(ekStopCriterionStringArray) / sizeof(const char*))
		return ekStopCriterionStringUndefined;

	return ekStopCriterionStringArray[id];
}
