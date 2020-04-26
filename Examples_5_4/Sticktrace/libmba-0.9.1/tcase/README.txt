To run these tests on Unix just do:

  $ make
  $ ./tmba

or on Windows do:

  > nmake -f Makefile.msvc
  > nmake -f Makefile.msvc test

and look for all 'pass's.

The mbs* functions will not work with pre-glibc 2.2 systems
(i.e. MbsSimple will generate many errors).
