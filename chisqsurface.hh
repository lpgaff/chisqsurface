// Header file for chisqsurface as it gets its classes

#ifndef __scan_hh__
#include "scan.hh"
#endif

#ifndef __rootobjs_hh__
#include "rootobjs.hh"
#endif

#include <chrono>
#include <iostream>
#include <string>

using namespace std;

// Get/Set arguments
string in_proj;
string intifile;
int Ndata_proj = 3;
int Ndata_targ = 5;
float low_tme = 0.1;
float upp_tme = 2.5;
int Nsteps_tme = 51;
float low_dme = 0.0;
float upp_dme = 0.0;
int Nsteps_dme = 1;
int Nmini = 1;
int Npara = 1;		// default back to linear mode
bool readflag = false;
bool g2 = true;		// default, gosia2
int tme_index = 1;	// default, index 1
int dme_index = 2;	// default, index 2
