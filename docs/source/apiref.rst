.. _apiref:

*************
API Reference
*************

.. highlight:: c



Preliminaries 
=============

All declarations are in :file:`eskit.h`, so it's enough to

::

  #include <eskit.h>

in each source file.

All identifiers are prefixed with *ek*. Type names are typedef‘d so that the 
struct keyword need not be used.



Concepts
========

The library deals with two main concept: *optimizer* and *point distribution 
handler*.

The optimizer handle a population of candidate points. At each optimizer iteration,
*lambda* point are generated. Only the *mu* best points are kept to update the 
optimizer state, before performing another iteration.  The optimizer is indeed
a minimizer: it seeks always for a minimum. Thus if you search for a maximum,
just use the opposite of your fitness function...

The point distribution handler plays two roles:

+ Generating candidates points according to a probability distribution
+ Updating the probability distribution, according to the *mu* best points kept 
  by the optimizer.

Three point distribution handlers are provided: **CMA**, **SepCMA** and **CSA**. 

+ **CMA** rely on a multi-variate Gaussian distribution, and adapt the full 
  covariance matrix. It makes a powerful algorithm, able to adapt to the local 
  topology of a fitness landscape in  very robust fashion. The price of that power
  is a rather computationally expensive probability distribution update. But any
  real life fitness function is likely to easily dwarf that cost anyway. Also,
  that adaption also costs points evaluations. The size of the distributions is 
  also adapted with the Cumulative Step-length Adaption (CSA) heuristic. That way,
  **CMA** can work at various scale of the fitness landscape. (Reference: `CMA-ES tutorial`_ Hansen et al.)
+ **SepCMA** works exactly like **CMA**, at the notable exception that **SepCMA**
  doesn't bother with adapting the full covariance matrix of multi-variate Gaussian 
  distribution. It just adapt the diagonal of the covariance matrix. It makes 
  **SepCMA** much less computationally expensive and it uses much less point 
  evaluation for it adaption. But it also sacrifices the **CMA** rotational invariance.
  **SepCMA** can be interesting in separable problems, high dimensions, or as
  a bootstrap for **CMA**. (Reference: original `SepCMA-ES publication`_ Ros et al.)
+ **CSA** rely on an isotropic Gaussian distribution. The only adaption mechanism
  is the Cumulative Step-length Adaption (CSA) heuristic. It makes a robust hill-climber.

By default, no point distribution handler is set, you have to do it yourself. The
punishment for forgetting that is an optimizer that stops at the first iteration.

Optimizer
==========

The optimizer API works that way:

#. Initialization
#. Setup 
#. Iterate
	#. Start
	#. Sample
	#. Evaluate
	#. Update
	#. Stop

#. Disposal

Initialization
--------------

To use an optimizer, you first need to initialize it.

.. c:function:: void ekOptimizer_init(ekOptimizer* self, size_t N)

	Initializes an optimizer, where *N* is the search space dimension.

	The pseudo-random number generator used by the optimizer is seeded by 
	default. But if you want different results at each optimizer usage (which is
	highly recommanded), you should seed the pseudo-random number generator by 
	yourself.

	::

		ekOptimizer_init(&optim, 10);
		ekRandomizer_seed(ekOptimizer_getRandomizer(&optim), myRandomSeed);

	Depending on what you do and what you want, you can seed the pseudo-random 
	number just one time, after the optimizer initialization, or at a per 
	optimization basis.

Accessors
---------

Once an optimizer is initialized, its state can be read

.. c:function:: size_t ekOptimizer_N(const ekOptimizer* self)

	Returns the dimension of the search space.

.. c:function:: ekRandomizer* ekOptimizer_getRandomizer(const ekOptimizer* self)

	Returns the pseudo-random number generator used by the optimizer.

.. c:function:: size_t ekOptimizer_lambda(ekOptimizer* self)

	Returns the *lambda* parameter, ie. the number of points to evaluate at each 
	iteration. By default, *lambda = 4 + log(N)* where *N* is the search space 
	dimension.

.. c:function:: size_t ekOptimizer_mu(const ekOptimizer* self)

	Returns the *mu* parameter, ie. the number of points used for one update of 
	the optimizer state. By default, *mu* = *lambda* / 2

.. c:function:: size_t ekOptimizer_nbUpdates(const ekOptimizer* self)

	Returns how much iterations have been performed so far.

.. c:function:: double* ekOptimizer_xMean(ekOptimizer* self)

	Returns a pointer on the search point of the optimizer. The coordinates of 
	the vector can be read or written. Note that changing during the optimization
	iterations is likely to disturb the optimizer and reduce it performances. In
	the other hand, it's strongly advised to setup the search point when setting
	up an optimizer. Also, once the optimization is done, the search point is 
	might be closer to a local optimum than the best point found so far.

.. c:function:: ekPoint ekOptimizer_point(ekOptimizer* self, size_t i)

	Reference on the i-th point, where i is in the *[0, lambda - 1]* range.

.. c:function:: ekPoint ekOptimizer_bestPoint(ekOptimizer* self)

	Reference on the point with the best fitness seen so far.

.. c:type:: ekPoint

	Represents a point in the search space, with 2 public fields:

	+ .. c:member:: ekPoint x

		An array of double, the coordinates of the point

	+ .. c:member:: ekPoint fitness

		The fitness of this point.

Setup
-----

Although an optimizer object has default settings, you probably wants to have 
your own settings.

.. c:function:: void ekOptimizer_setMuLambda(ekOptimizer* self, size_t mu, size_t lambda)

	Setup the *mu* and *lambda* parameter. *lambda* should be superior or equal 
	to *mu*.

Iterations
----------

.. c:function:: void ekOptimizer_start(ekOptimizer* self)

	Tells an optimizer than we will start a new optimization iterations cycle. The
	number of iterations returned by *ekOptimizer_nbUpdates* will be 0.

.. c:function:: void ekOptimizer_sampleCloud(ekOptimizer* self)

	Produce all the *alpha* candidate points to be evaluated.

.. c:function:: void ekOptimizer_samplePoint(ekOptimizer* self, size_t i)

	Sample the i-th point. You might be unhappy with one of the points 
	generated by *ekOptimizer_sampleCloud*. For instance, its fitness is not
	defined or does not meet some constraints. Re-sampling that point is one
	way to handle such case. Check the `CMA-ES tutorial`_ for more on that topic.

.. c:function:: void ekOptimizer_update(ekOptimizer* self)

	Update the optimizer state according to the sampled point fitnesses. The 
	number of iterations returned by *ekOptimizer_nbUpdates* will be incremented.

.. c:function:: enum ekStopCriterionId ekOptimizer_stop(ekOptimizer* self)

	Returns 0 if an optimizer can not perform further iterations. Else it returns a
	positive integer, which is a code that specify no more iterations can be done

	==================================== =========================================
	ekOptimizer_stop return value        meaning
	==================================== =========================================
	ekStopCriterionId_DistributionNotSet No point distribution handler have been
	                                     associated to the opimizer
	ekStopCriterionId_LowSigma           Sigma is bellow its minimum threshold
	ekStopCriterionId_NoEffectAxis       Axes of the Gaussian distribution 
	                                     are beyond what numerical precision can 
	                                     handle
	ekStopCriterionId_NoEffectCoord      Eigen vector basis of the Gaussian 
	                                     distribution are beyond what numerical 
	                                     precision can handle
	ekStopCriterionId_ConditionCov       Covariance matrix conditionning is beyond
	                                     what numerical precision can handle.
	ekStopCriterionId_EigenSolverFailure The eigen value solver is very unhappy,
	                                     the covariance matrix is probably broken
	                                     beyond repair.
	ekStopCriterionId_BestFitnessStall   The best fitness did not change since so
	                                     long that it is hopeless to wait more.
	==================================== =========================================

.. c:function:: const char* ekStopCriterionString(enum ekStopCriterionId id)

	Returns a string identifier unique for each possible ekStopCriterionId

Once you started to perform iterations, no optimizer settings changes are 
supposed to happen. It would mess-up the optimizer state, very likely to make 
your program crashing. You should call *ekOptimizer_start* again if any 
settings change happened.

Note that the optimizer *evaluation* step have to be performed explicitly. You
have to iterate through the *lambda* point of the optimizer and give them their
fitness, by writing the *fitness* field of each point (see the tutorials for a 
concrete example of this). It's for a matter of flexibility, one might want to 
various things like handling constraints or logging. Check the tutorial to see 
how to do that. 

Nevertheless, a helper function is provided for the simple case.

.. c:function:: void ekOptimizer_evaluateFunction(ekOptimizer* self, double(*function)(const double*, size_t))

	Sets the fitness of all the points according to a given pure function 
	(ie. without side effects). The function should be like 
	::	

		double myFunc(const double* x, size_t N)

	where *x* is the point to evaluate, and *N* is the search space dimension.

.. _`CMA-ES tutorial`: http://www.lri.fr/~hansen/cmatutorial.pdf
.. _`SepCMA-ES publication`: http://hal.inria.fr/inria-00287367/en

Disposal
--------

Once you're done with an ekOptimizer, you can release the resources it is 
using.

.. c:function:: void ekOptimizer_destroy(ekOptimizer* self)

	Release the resources used by a previously initialized optimizer.



Point distribution handler
==========================

No point distribution handler is associated to an optimizer, don't forget to do
that association. The consequence of such a mistake is an optimizer that stops 
at the first iteration and returns *ekStopCriterionId_DistributionNotSet* as a 
reason to stop.

For all distributions the initial step length is set to 1.0, and they trigger a 
update stop for a step length inferior to 10e-12.

CMA
---

.. c:function:: void ekCMA_init(ekCMA* self, size_t N)

	Initialize a *CMA* point distribution handler, where *N* is the search space 
	dimension..

.. c:function:: void ekCMA_destroy(ekCMA* self)

	Release the resources used by a previously initialized *CMA* point 
	distribution handler.

.. c:function:: void ekCMA_setSigma(ekCMA* self, double sigmaInit, double sigmaStop)

	Initialize the initial step length and the minimum step length before 
	triggering an update stop. The practice is to set sigmaInit to 1/3 of the 
	initial search domain, whereas sigmaStop is set to 10e-12.

.. c:function:: void ekCMA_setC(ekCMA* self, const ekMatrix* C)

	Set a custom covariance matrix (the default one is the identity matrix). Note 
	that you have to do this setting before each optimizer start.

.. c:function:: void ekCMA_setOptimizer(ekCMA* self, ekOptimizer* optim)

	Sets the point distribution handler of an optimizer as the *CMA* point 
	distribution handler.

.. c:function:: double ekCMA_sigma(const ekCMA* self)

	Returns the *sigma* parameter (step length) of the Gaussian distribution.

.. c:function:: const ekMatrix* ekCMA_C(const ekCMA* self)

	Returns the covariance matrix of the Gaussian distribution.

.. c:function:: const ekMatrix* ekCMA_B(const ekCMA* self)

	Returns the principal axes of the Gaussian distribution.

.. c:function:: const double* ekCMA_D(const ekCMA* self)

	Returns the length of the principal axes the covariance matrix of the 
	Gaussian distribution.

SepCMA
------

.. c:function:: void ekSepCMA_init(ekSepCMA* self, size_t N)

	Initialize a *SepCMA* point distribution handler, where *N* is the search 
	space dimension.

.. c:function:: void ekSepCMA_destroy(ekCMA* self)

	Release the resources used by a previously initialized *SepCMA* point 
	distribution handler.

.. c:function:: void ekSepCMA_setSigma(ekSepCMA* self, double sigmaInit, double sigmaStop)

	Initialize the initial step length and the minimum step length before 
	triggering an update stop. The practice is to set sigmaInit to 1/3 of the 
	initial search domain, whereas sigmaStop is set to 10e-12.

.. c:function:: void ekSepCMA_setC(ekSepCMA* self, const ekMatrix* C)

	Set a custom covariance matrix (the default one is the identity matrix). Note 
	that you have to do this setting before each optimizer start.

.. c:function:: void ekSepCMA_setOptimizer(ekSepCMA* self, ekOptimizer* optim)

	Sets the point distribution handler of an optimizer as the *SepCMA* point 
	distribution handler.

.. c:function:: double ekSepCMA_sigma(const ekCMA* self)

	Returns the *sigma* parameter (step length) of the Gaussian distribution.

.. c:function:: const ekMatrix* ekSepCMA_C(const ekCMA* self)

	Returns the covariance matrix of the Gaussian distribution.

.. c:function:: const double* ekSepCMA_D(const ekCMA* self)

	Returns the length of the principal axes the covariance matrix of the 
	Gaussian distribution.

CSA
---

.. c:function:: void ekCSA_init(ekCSA* self, size_t N)

	Initialize a *CSA* point distribution handler, where *N* is the search space 
	dimension.

.. c:function:: void ekCSA_destroy(ekCMA* self)

	Release the resources used by a previously initialized *CSA* point 
	distribution handler.

.. c:function:: void ekCSA_setSigma(ekCSA* self, double sigmaInit, double sigmaStop)

	Initialize the initial step length and the minimum step length before 
	triggering an update stop. The practice is to set sigmaInit to 1/3 of the 
	initial search domain, whereas sigmaStop is set to 10e-12.

.. c:function:: void ekCSA_setOptimizer(ekCSA* self, ekOptimizer* optim)

	Sets the point distribution handler of an optimizer as the *CSA* point 
	distribution handler.

.. c:function:: double ekCSA_sigma(self)

	Returns the *sigma* parameter (step length) of the Gaussian distribution.



Utilities
=========

Some utilities structure and functions, used internally, are likely to be of 
great use.

Randomizer
----------

A Randomizer is a pseudo-random number generator. The number it generates are
member of a finite length sequence, although that sequence is rather large. The 
generator is the Complementary-Multiply-With-Carry generator from George 
Marsaglia, it should work identically on any platform. Various state size can
be specified, a larger state size means a longer number sequence without 
repetition. 

.. c:function:: void ekRandomizer_init(ekRandomizer* self, enum ekRandomizerSize size)

	Initializes a Randomizer. The generator is not seeded and you have to do it by
	yourself.

.. c:function:: void ekRandomizer_destroy(ekRandomizer* self)

	Release the resources used by a previously initialized Randomizer.

.. c:function:: void ekRandomizer_copy(ekRandomizer* self, const ekRandomizer* rnd)

	Copy to a previously initialized Randomizer the state from another previously 
	initialized Randomizer.

.. c:function:: void ekRandomizer_seed(ekRandomizer* self, uint32_t seed)

	Setup the internal state of a Randomizer, by using a given seed. The seed
	should be different from 0.

.. c:function:: uint32_t ekRandomizer_next(ekRandomizer* self)

	Returns the next number in the pseudo-random number sequence.

.. c:function:: double ekRandomizer_nextUniform(ekRandomizer* self)

	Returns a floating point value, drawn from a uniform probability distribution, in the [0, 1[ range.

.. c:function:: double ekRandomizer_nextGaussian(ekRandomizer* self, double sigma)

	Returns a floating point value, drawn from a normal probability distribution.

===================== ===============
State size identifier Sequence length
===================== ===============
ekRandomizerSize_8    2^285
ekRandomizerSize_16   2^540
ekRandomizerSize_32   2^1053
ekRandomizerSize_64   2^2077
ekRandomizerSize_128  2^4118
ekRandomizerSize_256  2^8182
ekRandomizerSize_512  2^16410
ekRandomizerSize_1024 2^32794
ekRandomizerSize_4096 2^131104
===================== ===============

ArrayOpsD
---------

Although a vector  of double type is not defined, function to manipulate vector
of doubles, as arrays are provided. A *size* argument specify the length of the
vectors to manipulate.

.. c:function:: void ekArrayOpsD_copy(double* U, const double* V, size_t size)

	Computes *U* = *V*

.. c:function:: void ekArrayOpsD_copyMul(double* U, const double* V, size_t size, double value)

	Computes U = alpha * V

.. c:function:: void ekArrayOpsD_copyDiv(double* U, const double* V, size_t size, double value)

	Computes U = (1.0 / alpha) * V

.. c:function:: void ekArrayOpsD_fill(double* U, size_t size, double alpha)

	Computes U[i] = alpha

.. c:function:: void ekArrayOpsD_sqrt(double* U, size_t size)

	Compute U[i] = sqrt(U[i])

.. c:function:: void ekArrayOpsD_convolve(double* U, const double* V, size_t size)

	Computes U[i] = U[i] * V[i]

.. c:function:: void ekArrayOpsD_scalarMul(double* U, size_t size, double alpha)

	Computes U = alpha * U

.. c:function:: void ekArrayOpsD_scalarDiv(double* U, size_t size, double alpha)

	Computes U = (1.0 / alpha) * U

.. c:function:: void ekArrayOpsD_inc(double* U, const double* V, size_t size)

	Computes U = U + V

.. c:function:: void ekArrayOpsD_dec(double* U, const double* V, size_t size)

	Computes U = U - V

.. c:function:: void ekArrayOpsD_incMul(double* U, const double* V, size_t size, double alpha)

	Computes U = U + alpha * V

.. c:function:: double ekArrayOpsD_dot(const double* U, const double* V, size_t size)

	Returns U.V 

.. c:function:: double ekArrayOpsD_sum(const double* U, size_t size)

	Returns sum of U[i]

.. c:function:: double ekArrayOpsD_squareSum(const double* U, size_t size)

	Returns sum of U[i]^2

.. c:function:: double ekArrayOpsD_absSum(const double* U, size_t size)

	Returns sum of abs(U[i])

.. c:function:: double ekArrayOpsD_min(const double* U, size_t size)

	Returns minimun of U[i]

.. c:function:: double ekArrayOpsD_max(const double* U, size_t size)

	Returns maximum of U[i]

.. c:function:: void ekArrayOpsD_gaussian(double* U, size_t size, ekRandomizer* randomizer, double sigma)

	Fills U with normally distributed values

.. c:function:: void ekArrayOpsD_uniform(double* U, size_t size, ekRandomizer* randomizer, double a, double b)

	Fills U with uniformly distributed values in the [min(a, b), max(a, b)] range

.. c:function:: void ekArrayOpsD_print(double* U, size_t size, FILE* file)

	Print U in a human readable way ie. `Octave`_ notation

.. _`Octave`: http://www.gnu.org/software/octave

Matrix
------

.. c:function:: void ekMatrix_init(ekMatrix* self, size_t nbCols, size_t nbRows)

	Initialize a matrix. The elements values are not initialized, up to you to do it.

.. c:function:: void ekMatrix_destroy(ekMatrix* self)

	Release the resources used by a previously initialized matrix.

.. c:function:: double ekMatrix_at(ekMatrix* self, size_t col, size_t row)

	Read-write access to a matrix element	

.. c:function:: double ekMatrix_col(ekMatrix* self, size_t col)

	Read-write access to a matrix column as an array of double

.. c:function:: void ekMatrix_copy(ekMatrix* self, const ekMatrix* U)

	Computes self = U

.. c:function:: void ekMatrix_copyMul(ekMatrix* self, const ekMatrix* u, double alpha)

	Computes self = alpha * U

.. c:function:: void ekMatrix_fill(ekMatrix* self, double alpha)

	Computes self[i][j] = alpha

.. c:function:: void ekMatrix_scalarMul(ekMatrix* self, double alpha)

	Computes self = alpha * self

.. c:function:: void ekMatrix_scalarDiv(ekMatrix* self, double alpha)

	Computes self = (1.0 / alpha) * self

.. c:function:: void ekMatrix_diagMul(ekMatrix* self, const double* U)

	Computes self = U * self where U is a diagonal matrix

.. c:function:: void ekMatrix_inc(ekMatrix* self, const ekMatrix* U)

	Computes self = self + U

.. c:function:: void ekMatrix_dec(ekMatrix* self, const ekMatrix* U)

	Computes self = self - U 

.. c:function:: void ekMatrix_incMul(ekMatrix* self, const ekMatrix* U, double alpha)

	Computes self = self + alpha * U

.. c:function:: void ekMatrix_vectorProd(ekMatrix* self, const double* U, double* v)

	Computes V = self * U 

.. c:function:: void ekMatrix_diagProd(ekMatrix* self, const double* U, ekMatrix* V)

	Computes V = self * U where U is a diagonal matrix

.. c:function:: void ekMatrix_matrixProd(ekMatrix* self, const ekMatrix* U, ekMatrix* V)

	Computes V = self * U

.. c:function:: void ekMatrix_setDiagonal(ekMatrix* self, const double* U)

	Computes self[i][i] = U[i]

.. c:function:: void ekMatrix_getDiagonal(const ekMatrix* self, double* U)

	Computes U[i] = self[i][i]

.. c:function:: void ekMatrix_print(ekMatrix* self, FILE* file)

	Human readable output of a matrix
