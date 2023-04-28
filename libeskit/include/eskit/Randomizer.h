/*
 * Copyright (c) 2009-2023 Alexandre Devert <marmakoide@hotmail.fr>
 *
 * ESKit is free software; you can redistribute it and/or modify it under the 
 * terms of the MIT license. See LICENSE for details.
 */

#ifndef ESKIT_RANDOMIZER_H
#define ESKIT_RANDOMIZER_H

#ifdef __cplusplus
extern "C" {
#endif



#include <stdint.h>



enum ekRandomizerSize {
	ekRandomizerSize_8 = 0, /* period approx 2^285    */
	ekRandomizerSize_16,    /* period approx 2^540    */
	ekRandomizerSize_32,    /* period approx 2^1053   */
	ekRandomizerSize_64,    /* period approx 2^2077   */
	ekRandomizerSize_128,   /* period approx 2^4118   */
	ekRandomizerSize_256,   /* period approx 2^8182   */
	ekRandomizerSize_512,   /* period approx 2^16410  */
	ekRandomizerSize_1024,  /* period approx 2^32794  */
	ekRandomizerSize_4096   /* period approx 2^131104 */
};



typedef struct {
	uint32_t* array;
	uint64_t multiplier;
	uint32_t size;
	uint32_t carry;
	uint32_t index;
} ekRandomizer;



extern void
ekRandomizer_init(ekRandomizer* self, enum ekRandomizerSize size);



extern void
ekRandomizer_destroy(ekRandomizer* self);



extern void
ekRandomizer_copy(ekRandomizer* self, const ekRandomizer* rnd);



extern void
ekRandomizer_seed(ekRandomizer* self, uint32_t seed);



extern uint32_t
ekRandomizer_next(ekRandomizer* self);



extern double
ekRandomizer_nextUniform(ekRandomizer* self);



extern double
ekRandomizer_nextGaussian(ekRandomizer* self, double sigma);



#ifdef __cplusplus
}
#endif

#endif /* ESKIT_RANDOMIZER_H */
