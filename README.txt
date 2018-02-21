=============
=== USAGE ===
=============

To compile:

  $ make

To clean:

  $ make clean

To run:

  $ ./msim <program> <data>

Better yet, pipe the output to a file:

  $ ./msim <program> <data> > <outfile>

To generate a data file:

  $ python memgen.py

=====================
=== CONFIGURATION ===
=====================

To configure with the system, go to the configuration file:

  src/config.hpp

Note: any adjustments to the file will not be detected by gcc automatically,
      you will need to clean then recompile.

Some extra output is commented out at the bottom of src/main.cpp, this includes
outputs to file for specific statistics.
