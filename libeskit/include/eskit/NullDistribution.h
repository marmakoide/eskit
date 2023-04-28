/*
 * Copyright (c) 2010 Alexandre Devert <marmakoide@yahoo.fr>
 *
 * ESKit is free software; you can redistribute it and/or modify it under the 
 * terms of the MIT license. See LICENSE for details.
 */

#ifndef ESKIT_NULL_DISTRIBUTION_H
#define ESKIT_NULL_DISTRIBUTION_H

#ifdef __cplusplus
extern "C" {
#endif



#include <eskit/Distribution.h>



/*
  Implements a do-nothing distribution, used by Optimizer as a default. It's
	allows a graceful way (no segmentation fault) to deal with a user who forgot
	to set the distribution to be used with an Optimizer
 */



extern const ekDistributionDelegate 
ekNullDistribution_DistributionDelegate;



#ifdef __cplusplus
}
#endif

#endif /* ESKIT_NULL_DISTRIBUTION_H */
