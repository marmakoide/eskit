/*
 * Copyright (c) 2010 Alexandre Devert <marmakoide@yahoo.fr>
 *
 * ESKit is free software; you can redistribute it and/or modify it under the 
 * terms of the MIT license. See LICENSE for details.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <eskit.h>
#include "Evaluator.h"
#include "Configuration.h"



int
main(int argc, char* argv[]) {
	uint32_t runSeed, evalSeed;
	ekRandomizer randomizer;
	size_t i, nbEvals, nbRuns;
	ekOptimizer optim;
	Evaluator evaluator;
	Configuration config;
	ekDistribution distrib;

	char logFileName[256];
	FILE* logFile;
	
	/* Command line parsing */
	Configuration_init(&config);

	if (!Configuration_parseCmdLine(&config, argc, argv))
		return EXIT_FAILURE;

	if (!Configuration_setup(&config))
		return EXIT_FAILURE;

	/* Init */
	ekOptimizer_init(&optim, config.dim);

	ekRandomizer_init(&randomizer, ekRandomizerSize_4096);
	ekRandomizer_seed(&randomizer, config.seed);

	if ((config.setMu) && (config.setLambda))
		ekOptimizer_setMuLambda(&optim, config.mu, config.lambda);
	
	evalSeed = ekRandomizer_next(&randomizer);
	Evaluator_init(&evaluator, config.function, config.dim, config.randRot);
	Evaluator_setup(&evaluator, evalSeed);
	
	ekDistribution_initFromBuilder(&distrib, config.distribBuilder, config.dim, evaluator.sigmaInit, 10e-12);
	ekOptimizer_setDistribution(&optim, &distrib);

	/* Perform each run */
	for(nbRuns = 0; nbRuns < config.nbRuns; ++nbRuns) {
		/* Open the log file */
		if (config.nbRuns > 1)
			sprintf(logFileName, "run-%zu.dat", nbRuns);
		else
			strcpy(logFileName, "run.dat");

		logFile =	fopen(logFileName, "w");

		/* Run */
		runSeed = ekRandomizer_next(&randomizer);
		ekArrayOpsD_copy(ekOptimizer_xMean(&optim), evaluator.xMeanInit, ekOptimizer_N(&optim));

		ekRandomizer_seed(ekOptimizer_getRandomizer(&optim), runSeed);
		ekOptimizer_start(&optim);

		fprintf(logFile, "# evaluator : seed = %u function = '%s' N = %zu rotated = %d\n", 
						evalSeed, 
						evaluator.function->name, 
						ekOptimizer_N(&optim),
						evaluator.randRot); 
		fprintf(logFile, "# optimizer : mu = %zu, lambda = %zu, seed = %u, distrib = '%s'\n", 
					  ekOptimizer_mu(&optim), 
  	        ekOptimizer_lambda(&optim), 
  	        runSeed, 
  	        distrib.delegate.name);

		nbEvals = 0;
		do {
			ekOptimizer_sampleCloud(&optim);
		
			for(i = 0; i < ekOptimizer_lambda(&optim); ++i)
				ekOptimizer_point(&optim, i).fitness = Evaluator_evaluate(&evaluator, ekOptimizer_point(&optim, i).x);
			nbEvals += ekOptimizer_lambda(&optim);

			ekOptimizer_update(&optim);

			/* Log best fitness */
			fprintf(logFile, "%zu %e\n",
              nbEvals, 
						  ekOptimizer_bestPoint(&optim).fitness);
			/*
			fprintf(logFile, "%zu %e %e\n", 
              nbEvals, 
						  ekOptimizer_bestPoint(&optim).fitness, 
              distrib.sigma);
	    */
			fflush(logFile);
		} while((ekOptimizer_stop(&optim) == 0) && (nbEvals < config.nbEvals) && (ekOptimizer_bestPoint(&optim).fitness > 10e-10));

		/* Close the log file */
		fclose(logFile);
	}

	/* Job done */
	Evaluator_destroy(&evaluator);
	ekOptimizer_destroy(&optim);
	ekDistribution_destroy(&distrib, config.distribBuilder);
	ekRandomizer_destroy(&randomizer);

	return EXIT_SUCCESS;
}
