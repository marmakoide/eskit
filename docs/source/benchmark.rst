.. _benchmark:

**********************
Testing & benchmarking
**********************

.. highlight:: c


A benchmark program is included in ESKit, allowing to test if the algorithms
works as they should. For instance, you might discover some bug, or that the
implementation does not not behave as it should on some particular cases.

The benchmark program, *eskit-test*, is installed in the *test* directory of
the *eskit-1.0.0* source directory.



Benchmark problem
-----------------

Fitness function
~~~~~~~~~~~~~~~~

You can pick a fitness function as a benchmark. Use the *-fNAME* or 
*--function=NAME* switch. The following functions are supported:

+ **sphere**
+	**ellipsoid**
+	**cigar**
+	**rosenbrock**
+	**rastrigin**
+ **ackley**

By default, **sphere** is used. 

Random rotation
~~~~~~~~~~~~~~~

You can ask to apply a random rotation to the coordinate system of the 
benchmark function, by using the *-r* or *--rotate* switch. This is not activated
by default

Number of dimensions
~~~~~~~~~~~~~~~~~~~~

By default, the problem are in dimension 10. You can set another number of 
dimension with the *-dNUMBER* or *--dim=NUMBER* switch.



Optimizer setup
---------------

Evolution strategies parameters
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

You can set the *mu* and *lambda* strategie parameters with respectively the
*-mNUMBER* and *-lNUMBER*, or *--mu=NUMBER* and *--lambda=NUMBER*. Both 
*mu* and *lambda* should be set if you are not happy with the default values. 
The defaults are *lambda = 4 + 3 log(N)* and *mu = lambda / 2*

Point distribution handler
~~~~~~~~~~~~~~~~~~~~~~~~~~

You can set the point distribution handler used for the optimization. Use the 
*-uNAME* or *--update=NAME* switch, which accept 3 names.

+ **CMA**
+ **SepCMA**
+ **CSA**

By default, **CMA** is used.

Stopping criterion
~~~~~~~~~~~~~~~~~~

The maximum number of evaluation, 100000 by default, can be set wit the *-eNUMBER*
or *--eval=NUMBER* switch.


Experimental setup
------------------

*eskit-test* provides some rudimentary help for doing statistical tests. By
default, *eskit-test* perform only one run, outputing a log file called 'run.dat'
in the current directory. The switch *-nNUMBER* or *--nbruns=NUMBER* allows to
launch several runs. The log files will be numbered as 'run-0.dat', 'run-1.dat',
etc...

In order to have reproducible experiments, a seed for the pseudo-random number 
generator can be set, with the *-sNUMBER* or *--seed=MUMBER* switch. Note that
it does not guaranty cross-plateform reproducibility. Numerical precision and
compilation options have an influence as well.
