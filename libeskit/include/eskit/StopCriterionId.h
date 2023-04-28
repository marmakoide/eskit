/*
 * Copyright (c) 2009-2023 Alexandre Devert <marmakoide@hotmail.fr>
 *
 * ESKit is free software; you can redistribute it and/or modify it under the 
 * terms of the MIT license. See LICENSE for details.
 */

#ifndef ESKIT_STOP_CRITERION_ID_H
#define ESKIT_STOP_CRITERION_ID_H

#ifdef __cplusplus
extern "C" {
#endif



enum ekStopCriterionId {
	ekStopCriterionId_None = 0,
	ekStopCriterionId_DistributionNotSet,
	ekStopCriterionId_LowSigma,
	ekStopCriterionId_NoEffectAxis,
	ekStopCriterionId_NoEffectCoord,
	ekStopCriterionId_ConditionCov,
	ekStopCriterionId_EigenSolverFailure,
	ekStopCriterionId_BestFitnessStall
};



extern const char*
ekStopCriterionString(enum ekStopCriterionId id);



#ifdef __cplusplus
}
#endif

#endif /* ESKIT_STOP_CRITERION_ID_H */
