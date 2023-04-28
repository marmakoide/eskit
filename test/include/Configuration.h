/*
 * Copyright (c) 2010 Alexandre Devert <marmakoide@yahoo.fr>
 *
 * ESKit is free software; you can redistribute it and/or modify it under the 
 * terms of the MIT license. See LICENSE for details.
 */

#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <stdint.h>
#include "Function.h"
#include "Distribution.h"



typedef struct {
	size_t dim;

	size_t nbRuns;

	uint32_t seed;
	int generateSeed;

	size_t mu;
	int setMu;

	size_t lambda;
	int setLambda;

	size_t nbEvals;

	int randRot;

	const Function* function;

	const DistributionBuilder* distribBuilder;
} Configuration;



extern void
Configuration_init(Configuration* self);



extern int
Configuration_parseCmdLine(Configuration* self, int argc, char* argv[]);



extern int
Configuration_setup(Configuration* self);



#endif /* CONFIGURATION_H */
