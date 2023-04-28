/*
 * Copyright (c) 2009-2023 Alexandre Devert <marmakoide@hotmail.fr>
 *
 * ESKit is free software; you can redistribute it and/or modify it under the 
 * terms of the MIT license. See LICENSE for details.
 */

#include <math.h>
#include "eskit/Matrix.h"
#include "eskit/Optimizer.h"
#include "eskit/ArrayOps.h"
#include "eskit/CMAConstants.h"



static double sqr(double x) { return x * x; }



void
ekCMAConstants_setup(ekCMAConstants* self, ekOptimizer* optim) {
	size_t N, mu;

	N = ekOptimizer_N(optim);
	mu = ekOptimizer_mu(optim);

	self->chiN = sqrt(N) * (1.0 - (1.0 / (4.0 * N)) + (1.0 / (21.0 * N * N)));	
	self->muW = 1.0 / ekArrayOpsD_squareSum(ekOptimizer_weights(optim), mu);
	self->cSigma = (self->muW + 2.0) / (self->muW + N + 5.0);
	self->dSigma = 1 + 2 * fmax(0.0, sqrt(((self->muW - 1.0) / (N + 1.0)) - 1.0)) + self->cSigma;
	self->cc = (4.0 + self->muW / N) / (N + 4.0 + 2.0 * self->muW / N);
	self->c1 = 2.0 / (sqr(N + 1.3) + self->muW); 
  self->cMu = 2.0 * (self->muW - 2.0 + (1.0 / self->muW)) / (sqr(N + 2.0) + self->muW);
}

