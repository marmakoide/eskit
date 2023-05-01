.. _install:

***************
Getting started
***************

.. highlight:: c



Compiling and installing ESKit
==============================

The ESKit source is available at https://github.com/marmakoide/eskit

Clone the repository:

::

	git clone https://github.com/marmakoide/eskit
	cd eskit

The compilation and installation rely on `Waf`_, and is likely to be extremely 
simple. You just need to have Python 3 installed, nothing 
more.

::

	./waf configure
	./waf build
	./waf install

To change the destination directory (/usr/local by default), use the --prefix=DIR 
argument at the Waf configure step.

::

	./waf configure --prefix=DIR

If you want to uninstall what have been installed, just do

::

	./waf uninstall


*LAPACK* support
----------------

ESKit can optionally rely on `LAPACK`_ to compute the eigenvalues and eigenvector
of a real symetric matrix. It's used in the CMA Evolution Strategy, by far the 
most expensive part of the algorithm. First of all, the `LAPACK`_ code is definitly
more robust to numerical imprecisions than the default implementation provided in
ESKit. Furthermore, if you use optimized version of `LAPACK`_, as the one provided by CPU 
manufacturers, you might get a significant speed increase.

The `LAPACK`_ library is assumed to be in /usr/lib. The Waf  configure step 
becomes

::

	./waf configure --use_LAPACK



Compiling programs that use ESKit
=================================

ESKit involves one C header file, eskit.h, so it’s enough to put the line

::

	#include <eskit.h>

in the beginning of every source file that uses ESKit.

There’s also just one library to link with, libeskit. Compile and link the 
program as follows:

::

	cc -o prog prog.c -leskit

If you compile ESKit with LAPACK support, proceed as follows:

::

	cc -o -DUSE_LAPACK prog prog.c -leskit


There’s also support for pkg-config, which handle such issues transparently:

::

	cc -o prog prog.c `pkg-config --cflags --libs eskit`




.. _`Waf`: http://code.google.com/p/waf/
.. _`LAPACK`: http://www.netlib.org/lapack
