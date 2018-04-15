chisqsurface
============

DESCRIPTION
-----------

chisqsurface takes a gosia or gosia2 input file with the OP,MINI command and calculates the 2-dimensional chi^2 surface for a given range and number of transitional and diagonal matrix elements. The output is in the form of a simple text file of chi^2 values and a root file with many graphs and histograms including chi^2 + 1 cuts.


REQUIREMENTS
------------

chisqsurface is written in C++ with ROOT libraries.
You will need a compiler that accepts the c++11 standard or greater.
ROOT (root.cern.ch) must be installed and directories set correctly (i.e. using thisroot.sh).
Further to this, gosia and gosia2 must be installed (with names "gosia" and "gosia2", respectively) and your PATH variable should point their directory(s).
Please check the Makefile and define the BINDIR for your needs.


USAGE
-----

```
Program to create 2-dimensional chi^2 surfaces with Gosia/Gosia2
Usage:
  chisqsurface [OPTION...] positional parameters

  -m, --mini filename  OP,MINI file
  -i, --inti filename  OP,INTI file
      --np N           Number of projectile data
      --nt N           Number of target data
  -x, --x-index X      Index of x-axis matrix element
  -y, --y-index Y      Index of y-axis matrix element
      --nx N           Number of steps in the x-axis matrix element
      --ny N           Number of steps in the y-axis matrix element
      --x-low value    Lower limit for x-axis matrix element
      --x-upp value    Upper limit for x-axis matrix element
      --y-low value    Lower limit for y-axis matrix element
      --y-upp value    Upper limit for y-axis matrix element
      --g1             Standard Gosia selector
      --g2             Gosia2 selector (default)
  -h, --help           Print help
  -r, --read           Read previous results and continue
  -c, --cont           Continue previous calculation (use read instead!)

```
`np` and `nt` includes the sum of all g-ray yields, matrix elements, lifetimes, etc.

The total number of calculations or chi2 data points will thus be `nx`*`ny`.


FILE FORMAT AND PREPARATION
---------------------------

(Update: the example and description below assumes gosia1 for the integration step. The current recommendation is to use gosia2, so this should be updated.)

For the code to work, Gosia or Gosia2 should be prepared in such a way that they can be executed at the OP,MINI stage. That is to say that the integration step, 'OP,INTI', and the q-parameter map step, 'OP,MAP', must be run for both the projectile (and target files if using Gosia2). Keeping these filenames in the same format as input file is useful.

Similarly, the output files must follow this convention with the ".out" prefix. This is so the code can read the chi2 value from the output files. The code will soon read the input files to check the correct output filename in each case, so this will be redundant/flexible soon.

The Gosia/Gosia2 files for the projectile and target must be ready for the 'OP,MINI' step with the OP,REST command and NTAP=4. Matrix elements files should follow the same naming convention as the input file with the ".bst" extension and a backup copy with the ".bst.lit" extension to allow the code to reset the matrix elements for each calculation. Again, this could be a command line option in the future to change this.

The OP,MINI command should also be tweaked to allow the printing of the chisq value immediately, without any time consuming minimisation in the projectile. This is done by setting the chisq criterion (the third value of the input) to a very high number, e.g:

```
OP,MINI
 2100,2,99999999.,.0001,1.1,1,10,1,1,0.0001
OP,EXIT
```

This should not be done for the target, where a certain amount of minimisation should be allowed, say 20 steps, with a reasonable chisq condition of ~0.001.

In order to run the integration step for each point on the surface, you must have an OP,INTI file in the same directory. If your input file is named "example.inp", then your integration file should be named "example.INTI.inp" so that the code can run the integration step for each meshpoint. If it is named something different, it must be specified on the command line using the `-i` or `--inti` option.
An example case is attached for 202Rn on 109Ag. If it doesn't exist at all (or you specify `--inti=dummy`), then the integration is skipped.


OUTPUT
------

The output is given in two formats, a root file with a 2D graph of the data (including one cut at chisq+1) plus individual curves for every DME point, and a text file with an ascii list of the data. Each will be the projectile input file name appended with '.root' and '.chisq' respectively.

Further, a '.rslt' file is written with the 1 sigma limits extracted using the chisq+1 method. This is only valid when the full chisq+1 range is covered in the calculation.


EXAMPLE 
-------

This takes a little long to run because of the many levels and experiment. I'll try to update this with a simpler version, but for now it's useful compare the filename format etc.:

```
gosia < 109Ag_6Ag.INTI.inp
gosia < 109Ag_6Ag.MAP.inp
gosia < 202Rn_6Ag.INTI.inp
gosia < 202Rn_6Ag.MAP.inp
```
```
chisqsurface -m 202Rn_6Ag.inp -i 202Rn_6Ag.INTI.inp  --np=6 --nt=21 --x-low=-1.5 --x-upp=1.5 --nx=7 --y-low=0.5 --y-upp=1.5 --ny=11 --x-index=3 --y-index=1
```
