/*
 * Copyright (c) 2009-2023 Alexandre Devert <marmakoide@hotmail.fr>
 *
 * ESKit is free software; you can redistribute it and/or modify it under the 
 * terms of the MIT license. See LICENSE for details.
 */

#ifndef ESKIT_MEAN_WEIGHTS_H
#define ESKIT_MEAN_WEIGHTS_H

#ifdef __cplusplus
extern "C" {
#endif



#include <stddef.h>



struct s_ekMeanWeightsGenerator {
	const char* name;
	void(*generate)(double* weights, size_t size);
};



extern ekMeanWeightsGenerator ekLog_MeanWeightsGenerator;
extern ekMeanWeightsGenerator ekEqual_MeanWeightsGenerator;
extern ekMeanWeightsGenerator ekLinear_MeanWeightsGenerator;



#ifdef __cplusplus
}
#endif

#endif /* ESKIT_MEAN_WEIGHTS_H */

