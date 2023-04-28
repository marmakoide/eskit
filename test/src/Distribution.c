/*
 * Copyright (c) 2009-2023 Alexandre Devert <marmakoide@hotmail.fr>
 *
 * ESKit is free software; you can redistribute it and/or modify it under the 
 * terms of the MIT license. See LICENSE for details.
 */

#include <stdlib.h>
#include <string.h>
#include "Distribution.h"



void
ekDistribution_initFromBuilder(ekDistribution* self, const DistributionBuilder* builder, size_t N, double sigmaInit, double sigmaStop) {
	self->data = builder->create(N, sigmaInit, sigmaStop);
	self->delegate = *(builder->delegate);
}



void
ekDistribution_destroy(ekDistribution* self, const DistributionBuilder* builder) {
	builder->destroy(self->data);
	free(self->data);
}



/* --- Distribution builders database -------------------------------------- */

#define DECLARE_BUILDER(type) \
static void* \
DistributionBuilder_create_##type (size_t N, double sigmaInit, double sigmaStop) { \
	type* ret; \
\
	ret = malloc(sizeof(type));\
	type##_init(ret, N); \
	type##_setSigma(ret, sigmaInit, sigmaStop); \
	return ret; \
} \
\
static void \
DistributionBuilder_destroy_##type  (void * data) { \
	type##_destroy((type*)data); \
} \
\
static const \
DistributionBuilder type##_DistributionBuilder = \
{ \
	DistributionBuilder_create_##type , \
	DistributionBuilder_destroy_##type , \
	&type##_DistributionDelegate \
};



DECLARE_BUILDER(ekCSA)
DECLARE_BUILDER(ekCMA)
DECLARE_BUILDER(ekSepCMA)



static const
DistributionBuilder* DistributionBuildersList[] = 
{
	&ekCSA_DistributionBuilder,
	&ekCMA_DistributionBuilder,
	&ekSepCMA_DistributionBuilder,
	NULL
};



const DistributionBuilder* 
DefaultDistributionBuilder = &ekCMA_DistributionBuilder;



const DistributionBuilder*
getDistributionBuilderByName(const char* name) {
	const DistributionBuilder** ret;

	for(ret = DistributionBuildersList; (*ret) != NULL; ++ret) {
		if (strcmp((*ret)->delegate->name, name) == 0)
			break;
	}

	return (*ret);
}

