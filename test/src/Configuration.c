/*
 * Copyright (c) 2010 Alexandre Devert <marmakoide@yahoo.fr>
 *
 * ESKit is free software; you can redistribute it and/or modify it under the 
 * terms of the MIT license. See LICENSE for details.
 */

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <errno.h>
#include <getopt.h>

#include "Configuration.h"



void
Configuration_init(Configuration* self) {
	self->dim = 10;
	self->nbEvals = 100000;
	self->nbRuns = 1;
	self->generateSeed = 1;
	self->setMu = 0;
	self->setLambda = 0;
	self->randRot = 0;
	self->function = DefaultFunction;
	self->distribBuilder = DefaultDistributionBuilder;
}



static struct option long_options[] = 
{
	{"dim",      1, NULL, 'd'},
	{"eval",     1, NULL, 'e'},
	{"nbruns",   1, NULL, 'n'},
	{"seed",     1, NULL, 's'},
	{"function", 1, NULL, 'f'},
	{"mu",       1, NULL, 'm'},
	{"lambda",   1, NULL, 'l'},
	{"update",   1, NULL, 'u'},
	{"rotate",   0, NULL, 'r'},
	{"help",     0, NULL, 'h'},
	{NULL,       0, NULL, 0}
};

static char* option_string = "d:e:n:s:f:m:l:u:r";



static const char* usage =
"Usage: eskit-test [OPTION...]\n\n"
"  -d, --dim=NUMBER       dimension of the search space\n"
"  -e, --eval=NUMBER      number of evaluations per run\n"
"  -n, --nbruns=NUMBER    number of independant runs\n"
"  -s, --seed=NUMBER      random seed\n"
"  -f, --function=NAME    benchmark function\n"
"  -m, --mu=NUMBER        mu ES parameter\n"
"  -l, --lambda=NUMBER    lambda ES parameter\n"
"  -u, --update=NAME      point distribution update\n"
"  -r, --rotate           apply random rotation to benchmark function\n";



int
str2uint32(const char* str, uint32_t* value) {
	long long ret;
	char* endPtr;

	ret = strtoll(str, &endPtr, 10);

	if ((errno == ERANGE && (ret == LONG_MAX || ret == LONG_MIN)) || (errno != 0 && ret == 0)) {
		perror("Invalid format");
		return 0;
	 }

	if (endPtr == str) {
		fprintf(stderr, "No digits were found\n");
		return 0;
	}

	if (ret < 0) {
		fprintf(stderr, "Negative value\n");
		return 0;
	}

	if (ret > 4294967295L) {
		fprintf(stderr, "Value out of range\n");
		return 0;
	}

	*value = (uint32_t)ret;
	return 1;
}



int
Configuration_parseCmdLine(Configuration* self, int argc, char* argv[]) {
	int ret;
	uint32_t value;
	int optionIndex;

	/* Get first option */
	ret = getopt_long(argc, argv, option_string, long_options, &optionIndex);

	/* While we have options */	
	while(ret != -1) {
		switch(ret) {
			/* Dimension of the problem */
			case 'd':
			if (!str2uint32(optarg, &value)) {
				fprintf(stderr, "Bogus dimension setting\n");
				return 0;
			}
			self->dim = value;
			break;

			/* Number of evaluations per run */
			case 'e':	
			if (!str2uint32(optarg, &value)) {
				fprintf(stderr, "Bogus number of evaluations setting\n");
				return 0;
			}
			self->nbEvals = value;
			break;	

			/* Nb of runs to perform */
			case 'n':
			
			if (!str2uint32(optarg, &value)) {
				fprintf(stderr, "Bogus Number of runs setting\n");
				return 0;
			}
			self->nbRuns = value;
			break;

			/* User provided random seed */
			case 's':
			if (!str2uint32(optarg, &value)) {
				fprintf(stderr, "Bogus Random seed setting\n");
				return 0;
			}
			self->seed = value;
			self->generateSeed = 0;
			break;

			/* User function choice */
			case 'f':
			self->function = getFunctionByName(optarg);
			if (self->function == NULL) {
				fprintf(stderr, "function '%s' not defined\n", optarg);
				return 0;
			}
			break;

			/* User mu setting */
			case 'm':
			self->setMu = 1;
			if (!str2uint32(optarg, &value)) {
				fprintf(stderr, "Bogus 'mu' parameter setting\n");
				return 0;
			}
			self->mu = value;
			break;

			/* User lambda setting */
			case 'l':
			self->setLambda = 1;
			if (!str2uint32(optarg, &value)) {
				fprintf(stderr, "Bogus 'lambda' parameter setting\n");
				return 0;
			}
			self->lambda = value;
			break;

			/* Help switch */
			case 'h':
				printf("%s", usage);
				return 0;
			break;

			/* User distribution choice */
			case 'u':
			self->distribBuilder = getDistributionBuilderByName(optarg);
			if (self->distribBuilder == NULL) {
				fprintf(stderr, "update '%s' not defined\n", optarg);
				return 0;
			}
			break;

			/* Can haz random rotation ? */
			case 'r':
			self->randRot = 1;
			break;

			/* LOL, WTF happened */
			default:
				return 0;
				break;
		}

		/* Get next option */
		ret = getopt_long(argc, argv, option_string, long_options, &optionIndex);
	}

	/* Job done */
	return 1;
}



int
getRandomSeed(uint32_t* outSeed) {
	FILE* file;

	file = fopen("/dev/urandom", "rb");
	if (!file) {
		perror("Failed to open /dev/urandom");
		return 0;
	}

	if (fread(outSeed, sizeof(uint32_t), 1, file) != 1) {
		perror("Failed to read random seed");
		return 0;
	}

	fclose(file);
	return 1;	
}



int
Configuration_setup(Configuration* self) {
	/* Random seed handling */
	if (self->generateSeed)
		if (!getRandomSeed(&(self->seed)))
			return 0;

	/* mu & lambda settings check */
	if ((self->setMu) && (!self->setLambda)) {
		fprintf(stderr, "User settings for 'mu' but not for 'lambda'\n");
		return 0;
	}

	if ((!self->setMu) && (self->setLambda)) {
		fprintf(stderr, "User settings for 'lambda' but not for 'mu'\n");
		return 0;
	}

	if ((self->setMu) && (self->setLambda) && (self->mu > self->lambda)) {
		fprintf(stderr, "'lambda' inferior to 'mu'\n");
		return 0;
	}
	
	/* Job done */
	return 1;
}
