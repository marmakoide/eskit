/*
 * Copyright (c) 2010 Alexandre Devert <marmakoide@yahoo.fr>
 *
 * ESKit is free software; you can redistribute it and/or modify it under the 
 * terms of the MIT license. See LICENSE for details.
 */

#include <stdlib.h>
#include <string.h>
#include "eskit/Macros.h"
#include "eskit/Randomizer.h"



/*
  Complementary-multiply-with-carry random number generator.

  Devised by George Marsaglia, that generator have, according to my opinion,
  several advantages the popular Mersenne Twister :
     - Various settings for the inner state size (huge period VS memory usage)
     - Simple and elegant
*/



typedef struct {
	uint64_t multiplier;
	uint32_t size;
} ekRandomizerSetting;



const ekRandomizerSetting ekRandomizerSettingsList[] = 
{
	{ 716514398u,    8u },
	{ 487198574u,   16u },
	{ 547416522u,   32u },
	{ 647535442u,   64u },
	{ 8007626u,    128u },
	{ 8001634u,    256u },
	{ 123554632u,  512u },
	{ 123471786u, 1024u },
	{ 18782u,     4096u }
};

#define ekRandomizerSettingsListSize (sizeof(ekRandomizerSettingsList) / sizeof(ekRandomizerSetting))



static void
ekRandomizer_reset(ekRandomizer* self) {
	self->carry = 362436u;
	self->index = self->size - 1;
}



void
ekRandomizer_init(ekRandomizer* self, enum ekRandomizerSize size) {
	const ekRandomizerSetting* setting;

	setting =  ekRandomizerSettingsList + size;
	self->multiplier = setting->multiplier;
	self->size = setting->size;

	self->array = newArray(uint32_t, self->size);
}



void
ekRandomizer_destroy(ekRandomizer* self) {
	free(self->array);
}



void
ekRandomizer_copy(ekRandomizer* self, const ekRandomizer* rnd) {
	if (self->size != rnd->size) {
		free(self->array);
		self->size = rnd->size;
		self->multiplier = rnd->multiplier;
		self->array = newArray(uint32_t, self->size);
	}

	self->carry = rnd->carry;
	self->index = rnd->index;
	memcpy(self->array, rnd->array, sizeof(uint32_t) * self->size);	
}



void
ekRandomizer_seed(ekRandomizer* self, uint32_t seed) {
	uint32_t i, j;
	uint32_t* offset;

	offset = self->array;
	for(i = self->size, j = seed; i != 0; --i, ++offset) {
		j = j ^ (j << 13u);
		j = j ^ (j >> 17u);
		j = j ^ (j << 5u);
		(*offset) = j;
	}

	ekRandomizer_reset(self);
}



uint32_t
ekRandomizer_next(ekRandomizer* self) {
	uint64_t t;
	uint32_t x, result;
	const uint32_t r = 0xfffffffe;

	self->index += 1;
	self->index &= self->size - 1;

	t = self->multiplier * self->array[self->index] + self->carry;
	self->carry = (uint32_t)(t >> 32u);

	x = (uint32_t)(t + self->carry);
	if (x < self->carry) {
		x++;
		self->carry++;
	}

	result = r - x;
	self->array[self->index] = result;

	return result;
}



double
ekRandomizer_nextUniform(ekRandomizer* self) {
	double y;
	
	y = ekRandomizer_next(self);
	return y / 4294967295.0;
}
