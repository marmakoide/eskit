#include <stdlib.h>
#include <eskit.h>

int
isValidPoint(const double* x, size_t N) {
  ...
}

double
fitness(const double* x, size_t N) {
  ...
}

int
main(int argc, char* argv[]) {
  size_t i;
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
    for(i = 0; i < ekOptimizer_lambda(&optim); ++i) {
      /* Resample if the point is not valid */
      while(!isValidPoint(ekOptimizer_point(&optim, i).x, ekOptimizer_N(&optim)))
        ekOptimizer_samplePoint(&optim, i);

      /* Evaluate the point */
      ekOptimizer_point(&optim, i).fitness = fitness(ekOptimizer_point(&optim, i).x, ekOptimizer_N(&optim));
    }

    /* Update */
    ekOptimizer_update(&optim);
  } while(!ekOptimizer_stop(&optim));

  /* Job done */
  ekCMA_destroy(&strategy);
  ekOptimizer_destroy(&optim);
  return EXIT_SUCCESS;
}
