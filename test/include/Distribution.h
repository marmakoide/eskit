/*
 * Copyright (c) 2010 Alexandre Devert <marmakoide@yahoo.fr>
 *
 * ESKit is free software; you can redistribute it and/or modify it under the 
 * terms of the MIT license. See LICENSE for details.
 */

#ifndef DISTRIBUTION_H
#define DISTRIBUTION_H

#include <eskit.h>



typedef struct {
	void*(*create)(size_t N, double sigmaInit, double sigmaStop);
	void(*destroy)(void*);
	const ekDistributionDelegate* delegate;
} DistributionBuilder;



extern const DistributionBuilder* DefaultDistributionBuilder;

extern const DistributionBuilder*
getDistributionBuilderByName(const char* name);



extern void
ekDistribution_initFromBuilder(ekDistribution* self, const DistributionBuilder* builder, size_t N, double sigmaInit, double sigmaStop);



extern void
ekDistribution_destroy(ekDistribution* self, const DistributionBuilder* builder);



#endif /* DISTRIBUTION_H */
