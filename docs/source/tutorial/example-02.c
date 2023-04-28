#include <stdlib.h>
#include <eskit.h>

double
fitness(const double* x, size_t N) {
  ...
}

int
main(int argc, char* argv[]) {
  ekOptimizer optim;
  ekSepCMA strategy;

  /* Initialization */
  ekOptimizer_init(&optim, 10);
  ekSepCMA_init(&strategy, ekOptimizer_N(&optim));

  /* Setup */
  ekSepCMA_setOptimizer(&strategy, &optim);

  ekOptimizer_setMuLambda(&optim, ekOptimizer_N(&optim) / 2, ekOptimizer_N(&optim));

  ekRandomizer_seed(ekOptimizer_getRandomizer(&optim), 1337);
  ekArrayOpsD_uniform(ekOptimizer_xMean(&optim), ekOptimizer_N(&optim), ekOptimizer_getRandomizer(&optim), -1.0, 1.0);

  /* Iterations */
  ekOptimizer_start(&optim);
  do {
    /* Sample */
    ekOptimizer_sampleCloud(&optim);

    /* Evaluate */
    ekOptimizer_evaluateFunction(&optim, fitness);

    /* Update */
    ekOptimizer_update(&optim);
  } while(!ekOptimizer_stop(&optim));

  /* Job done */
  ekSepCMA_destroy(&strategy);
  ekOptimizer_destroy(&optim);
  return EXIT_SUCCESS;
}
