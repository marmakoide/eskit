#include <stdlib.h>
#include <stdio.h>
#include <eskit.h>

double
fitness(const double* x, size_t N) {
  ...
}

int
main(int argc, char* argv[]) {
  size_t nbEvals;
  ekOptimizer optim;
  ekCMA strategy;

  /* Initialization */
  ekOptimizer_init(&optim, 10);
  ekCMA_init(&strategy, ekOptimizer_N(&optim));

  /* Setup */
  ekCMA_setOptimizer(&strategy, &optim);

  /* Iterations */
  nbEvals = 0;
  ekOptimizer_start(&optim);
  do {
    /* Sample */
    ekOptimizer_sampleCloud(&optim);

    /* Evaluate */
    ekOptimizer_evaluateFunction(&optim, fitness);
    nbEvals += ekOptimizer_lambda(&optim);

    /* Update */
    ekOptimizer_update(&optim);

    /* Print nb evals, fitness, sigma */
    printf("%u %e %e\n",
           nbEvals, 
           ekOptimizer_bestPoint(&optim).fitness,
           ekCMA_sigma(&strategy));
  } while(!ekOptimizer_stop(&optim));

  /* Print the best individual and the mean point */
  printf("# xBest=");
  ekArrayOpsD_print(ekOptimizer_bestPoint(&optim).x, ekOptimizer_N(&optim), stdout);
  printf("# xMean=");
  ekArrayOpsD_print(ekOptimizer_xMean(&optim), ekOptimizer_N(&optim), stdout);

  /* Job done */
  ekCMA_destroy(&strategy);
  ekOptimizer_destroy(&optim);
  return EXIT_SUCCESS;
}

