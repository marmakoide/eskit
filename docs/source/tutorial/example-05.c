#include <stdlib.h>
#include <eskit.h>

double
fitness(const double* u, size_t N) {
  ...
}

void
optimize(ekOptimizer* optim, size_t nbIter) {
  size_t nbIterDec;

  if (nbIter == 0) {
    nbIter = 1;
    nbIterDec = 0;
  }
  else
   nbIterDec = 1;

  /* Iterations */
  ekOptimizer_start(optim);
  do {
    /* Sample */
    ekOptimizer_sampleCloud(optim);

    /* Evaluate */
    ekOptimizer_evaluateFunction(optim, fitness);

    /* Update */
    ekOptimizer_update(optim);
		nbIter -= nbIterDec;
  } while((nbIter != 0) && (!ekOptimizer_stop(optim)));
}

void
switchSepCMA2CMA(ekSepCMA* sepCMA, ekCMA* CMA) {
  size_t N;
  ekMatrix cov;

  N = ekMatrix_nbCols(ekSepCMA_C(sepCMA));

  ekMatrix_init(&cov, N, N);
  ekMatrix_fill(&cov, 0.0);
  ekMatrix_setDiagonal(&cov, ekSepCMA_D(sepCMA));
  ekCMA_setC(CMA, &cov);
  ekMatrix_destroy(&cov);
}

int
main(int argc, char* argv[]) {
  ekOptimizer optim;
  ekCMA CMA;
  ekSepCMA sepCMA;

  /* Initialization */
  ekOptimizer_init(&optim, 10);
  ekCMA_init(&CMA, ekOptimizer_N(&optim));
  ekSepCMA_init(&sepCMA, ekOptimizer_N(&optim));

  /* First optimization step: setup & run with SepCMA */
  ekSepCMA_setOptimizer(&sepCMA, &optim);
  optimize(&optim, 100.0 * ekOptimizer_N(&optim) / ekOptimizer_lambda(&optim));

  /* Second optimization step: switch to CMA & run */
  switchSepCMA2CMA(&sepCMA, &CMA);
  ekCMA_setOptimizer(&CMA, &optim);
  optimize(&optim, 0);

  /* Job done */
  ekCMA_destroy(&CMA);
  ekSepCMA_destroy(&sepCMA);
  ekOptimizer_destroy(&optim);
  return EXIT_SUCCESS;
}

