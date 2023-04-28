/*
 * Copyright (c) 2009-2023 Alexandre Devert <marmakoide@hotmail.fr>
 *
 * ESKit is free software; you can redistribute it and/or modify it under the 
 * terms of the MIT license. See LICENSE for details.
 */

#ifndef ESKIT_CMA_CONSTANTS_H
#define ESKIT_CMA_CONSTANTS_H

#ifdef __cplusplus
extern "C" {
#endif



#include <eskit/Types.h>



/*
   Common constants for the different CMA-ES flavours.
 */

typedef struct {
	double muW;
	double cSigma;
	double dSigma;
	double cc;
	double c1;
	double cMu;
	double chiN;        /* Expectation of ||N(0,I)|| == norm(randn(N,1))       */
} ekCMAConstants; 



extern void
ekCMAConstants_setup(ekCMAConstants* self, struct s_ekOptimizer* optim);



#ifdef __cplusplus
}
#endif

#endif /* ESKIT_CMA_CONSTANTS_H */
