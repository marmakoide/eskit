.. _install:

***************
Getting started
***************

.. highlight:: c



Compiling and installing ESKit
==============================

The ESKit source is available at http://www.marmakoide.org/code/eskit/releases

Unpack the source tarball and change to the source directory:

::

	bunzip2 -c eskit-1.0.0.tar.bz2 | tar xf -
	cd eskit-1.0.0

The compilation and installation rely on `Waf`_, and is likely to be extremely 
simple. You just need to have Python installed, version 2.3 at least, nothing 
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