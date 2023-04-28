/*
 * Copyright (c) 2009-2023 Alexandre Devert <marmakoide@hotmail.fr>
 *
 * ESKit is free software; you can redistribute it and/or modify it under the 
 * terms of the MIT license. See LICENSE for details.
 */

#ifndef ESKIT_DISTRIBUTION_H
#define ESKIT_DISTRIBUTION_H

#ifdef __cplusplus
extern "C" {
#endif



#include <eskit/Types.h>
#include <eskit/StopCriterionId.h>



typedef struct {
	/* Human readable identifier */
	const char* name;

	/* Called at each optimizer start */
	void(*start)(ekDistribution*, ekOptimizer*);

	/* Update the distribution, called at each optimizer update */
	void(*update)(ekDistribution*, ekOptimizer*);

	/* Sample one single point */
	void(*samplePoint)(ekDistribution*, ekOptimizer*, size_t, double*, double*);

	/* Sample several points */
	void(*sampleCloud)(ekDistribution*, ekOptimizer*, ekMatrix*, ekMatrix*);

	/* Test if further updates are possible */
	enum ekStopCriterionId(*stop)(ekDistribution*, ekOptimizer*);
} ekDistributionDelegate;



struct s_ekDistribution {
	void* data;
	ekDistributionDelegate delegate;
};



#define ekDistribution_start(self, optim) (self)->delegate.start((self), (optim))

#define ekDistribution_update(self, optim) (self)->delegate.update((self), (optim))

#define ekDistribution_samplePoint(self, optim, id, x, z) (self)->delegate.samplePoint((self), (optim), (id), (x), (z))

#define ekDistribution_sampleCloud(self, optim, x, z) (self)->delegate.sampleCloud((self), (optim), (x), (z))

#define ekDistribution_stop(self, optim) (self)->delegate.stop((self), (optim))



#ifdef __cplusplus
}
#endif

#endif /* ESKIT_DISTRIBUTION_H */
