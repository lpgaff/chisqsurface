chisqsurface
============

DESCRIPTION
-----------

chisqsurface takes a gosia2 input file with the OP,MINI command and calculates the chisq surface for a given range and number of transitional and diagonal matrix elements. The output is in the form of a simple text file of chi2 values and a root file with many graphs and histograms including chi2 + 1 cuts


REQUIREMENTS
------------

chisqsurface is written in C++ with root libraries. You will need a working version of g++.
ROOT (root.cern.ch) must be available in a directory where the compiler can find it.
Further to this, gosia and gosia2 must be installed (with names "gosia" and "gosia2", respectively) and your PATH variable should point their directory(s).
Please check the Makefile and define the BINDIR for your needs.


USAGE
-----

```
chisqsurface <in_proj> <in_targ> <Ndata_proj=3> <Ndata_targ=3> <low_TME=0.1> 
   <upp_TME=2.5> <Nsteps_TME=51> <low_DME=0.0> <upp_DME=0.0> <Nsteps_DME=1>
```
 where <Ndata_proj=3> and <Ndata_targ=5> are the number of data
for the projectile and target, respectively. This includes the
sum of all g-ray yields, matrix elements, lifetimes, etc.

`<low_TME>` is the lower limit for the transitional matrix element (TME) range
`<upp_TME>` is the upper limit for the transitional matrix element (TME) range
`<Nsteps_TME>` is the number of steps within the TME range, including the limits
`<low_DME>` is the lower limit for the diagonal matrix element (DME) range
`<upp_DME>` is the upper limit for the diagonal matrix element (DME) range
`<Nsteps_DME>` is the number of steps within the DME range, including the limits

The total number of calculations or chi2 data points will thus be Nsteps_TME* Nsteps_TME.

If `cont` is included at the end, then the calculation will continue from the last point
reading other values from the .chisq file. Useful if your calculation is interrupted.
The "read" option is similar to the `cont` option and will read any matching chisq values from 
a previous .chisq file instead of recalculating them. Useful if you need to extend your range
If `-dN` is included at the end, where N is the index of the diagonal matrix
element other than the default 2, then correct lines of the .bst/.bst.lit
files will be read/written. Same is for `-tN` and the transitional matrix elements, where the default is 1.



FILE FORMAT AND PREPARATION
---------------------------

(Update: the example and description below assumes gosia1 for the integration step. The current recommendation is to use gosia2, so this should be updated.)

For the code to work, the input must be prepared in a specific way. That is to say that the integration step, 'OP,INTI', and the q-parameter map step, 'OP,MAP', must be run for both the projectile and target files using gosia1 with the correct gosia1 style filenames in each case. Keeping these filenames in the same format as <in_proj> is useful. If your gosia2 projectile file is named "example.inp", then your integration file should be named "example.INTI.inp" so that the code can run the integration step for each meshpoint. Similarly, you could also name the map input file as "example.MAP.inp".

Similarly, the output files must follow this convention with the ".out" prefix. This is so the code can read the chi2 value from the output files.

The gosia2 files for the projectile and target must be ready for the 'OP,MINI' step with the OP,REST command and NTAP=4. Matrix elements files should follow the same naming convention as the input file with the ".bst" extension and a backup copy with the ".bst.lit" extension to allow the code to reset the matrix elements for each calculation.

The OP,MINI command should also be tweaked to allow the printing of the chisq value immediately, without any time consuming minimisation in the projectile. This is done by setting the chisq criterion (the third value of the input) to a very high number, e.g:

```
OP,MINI
 2100,2,99999999.,.0001,1.1,1,10,1,1,0.0001
OP,EXIT
```

This should not be done for the target, where a certain amount of minimisation should be allowed, say 20 steps, with a reasonable chisq condition of ~0.001.

In order to run the integration step for each point on the surface, a gosia1 file should be present in the directory ready to run the integration step with the OP,REST command. This program will use the format of <inputfile> (foobar.inp) to look for the OP,INTI file (foobar.INTI.inp). If this doesn't exist, it will skip the integration step.

An example case is attached for 202Rn on 109Ag.


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
chisqsurface 202Rn_6Ag.inp 109Ag_6Ag.inp 6 21 0.5 1.5 11 -1.5 1.5 7 -d3
```
