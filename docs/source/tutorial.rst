.. _tutorial:

********
Tutorial
********

.. highlight:: c



The minimal example
===================

Here is is a short usage example, which keeps the default settings and does just
a basic minimization without fancy.

.. literalinclude:: tutorial/example-01.c
	:language: c
	:linenos:

The initial search point for the optimization is not set, the default is the
null vector. In the case where you have no idea where the optimum lies, 
a better choice would be picking the initial point randomly in the search space.



Custom settings
===============

Here, the minimal example is augmented with some custom settings:

+ Use of the *SepCMA* strategy instead of the *CMA* strategy.
+ Custom population sizing.
+ Seeding the optimizer pseudo-random number generator
+ Initialize randomly the initial search point *xMean*

.. literalinclude:: tutorial/example-02.c
	:language: c
	:linenos:

The seed for the optimizer's pseudo-random number generator is fixed to
1337. You might want to pick a different seed at each run, which is doable but 
plateform dependent. I suggest the following procedures to pick a good seed:

+ Under Linux, read */dev/random* or */dev/urandom*.
+ Under any operating system, read the system time, the processor time usage, 
  the hostname, and the process identifier. Put them in a string, and hash the 
  string with a good hashing function like `Murmur`_.

.. _`Murmur`: http://sites.google.com/site/murmurhash/

Custom evaluation
=================

Here, the minimal example is augmented to show how to handle custome evaluation.
it can be useful to do resampling for instance, to deal with some singular 
points where the fitness is not defined. Another case is evaluating the points
in parallel, by exploiting multiple threads and/or a cluster of computers.

.. literalinclude:: tutorial/example-03.c
	:language: c
	:linenos:

Note the resampling loop might never returns, depending of the validity domain.
Using some maximum number of retries would be safer.



Logging
=======

It's usually quite useful to display the log of an optimization run. Here it is
the minimal example augmented to produce a basic log to the standard output.

.. literalinclude:: tutorial/example-04.c
	:language: c
	:linenos:



Custom covariance matrix setting
================================

*ESKit* allows the use of a custom covariance matrix setting for the *CMA* 
point handlers. The publication that introduces **SepCMA** suggest to use 
**SepCMA** to bootstrap **CMA**. Here it is the implementation of the strategy 
in the `SepCMA-ES publication`_.

.. literalinclude:: tutorial/example-05.c
	:language: c
	:linenos:

.. _`SepCMA-ES publication`: http://hal.inria.fr/inria-00287367/en

