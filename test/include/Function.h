/*
 * Copyright (c) 2010 Alexandre Devert <marmakoide@yahoo.fr>
 *
 * ESKit is free software; you can redistribute it and/or modify it under the 
 * terms of the MIT license. See LICENSE for details.
 */

#ifndef FUNCTION_H
#define FUNCTION_H

#include <stddef.h>



/*
   Interface to implementation of classical benchmark functions.

	 The definitions of those functions are taken from 
	 "Learning Probability Distributions in Continuous Evolutionary Algorithms"
	 Stephan Kern et al.
 */

typedef struct {
	const char* name;
	double minBound, maxBound;
	double(*compute)(const double*, size_t);
} Function;



extern const Function* DefaultFunction;

extern const Function*
getFunctionByName(const char* name);



#endif /* FUNCTION_H */
