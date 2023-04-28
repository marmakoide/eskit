#include <stdlib.h>
#include <eskit.h>

double
fitness(const double* x, size_t N) {
 ...
}

int
main(int argc, char* argv[]) {
  ekCMA strategy;
  ekOptimizer optim;

  /* Initialization */
  ekOptimizer_init(&optim, 10);
  ekCMA_init(&strategy, ekOptimizer_N(&optim));

  /* Setup */
  ekCMA_setOptimizer(&strategy, &optim);   

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
  ekCMA_destroy(&strategy);
  ekOptimizer_destroy(&optim);
  return EXIT_SUCCESS;
}
