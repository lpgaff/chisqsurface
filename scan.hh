// The main routine for performing the scans
// Liam Gaffney (liam.gaffney@liverpool.ac.uk) - 23/04/2020

#ifndef __scan_hh__
#define __scan_hh__

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <filesystem>

#ifndef __rootobjs_hh__
#include "rootobjs.hh"
#endif

using namespace std;

class scan {
	
public:
	
	// Constructor/destructor
	scan();
	~scan();
	
	// Lookup functions
	int		LookUpOldChisq();
	void	ContinueScan();
	string	FindFileName( string in_file, string tape );
	double	ReadChiSqFromFile( string gosiaoutfile );
	int		GetChiSq();
	int		GetChiSq2();

	// Execution functions
	int		IntegrateProjectile();
	int		WriteProjectileMatrixElementsToFile();
	int		WriteTargetMatrixElementsToFile();

	// Main functions
	void	run_scan();
	void	do_step();
	
	// Getting the files
	void	GetAuxFiles();
	void	OpenOutputFiles();
	void	CloseOutputs();
	
	// Some nice functions
	string	getDateTime();
	void	PrintStep();
	void	PrintResults();
	
	// Get/Set Functions
	void	SetupScan( string _in_proj, string _intifile,
					   float _tme_index, float _dme_index,
					   float _low_tme, float _upp_tme, int _Nsteps_tme,
					   float _low_dme, float _upp_dme, int _Nsteps_dme,
					   int _Ndata_proj, int _Ndata_targ,
					   int _Nmini, int _Npara,
					   bool _g2, bool _read, rootobjs ro );

	
	inline void	SetMEs( float _tme, float _dme ) {
		tme = _tme;
		dme = _dme;
		return;
	};
	
	inline float GetChisqProj() { return chisq_proj; };
	inline float GetChisqTarg() { return chisq_targ; };
	
private:
	
	// Scan Variables
	string in_proj;
	string in_targ;
	string out_proj;
	string out_targ;
	string intifile;
	string bst_proj;
	string bst_targ;
	string scandir;

	int tme_index;
	int dme_index;
	float low_tme;
	float upp_tme;
	int Nsteps_tme;
	float low_dme;
	float upp_dme ;
	int Nsteps_dme;
	
	int Ndata_proj;
	int Ndata_targ;

	int Nmini;
	int Npara;
	bool g2;
	bool read;
	
	rootobjs ro;

	string outname;
	vector<string> textname;
	vector<ofstream*> out;
	ofstream outa, outb;
	ofstream rslt;
	ifstream old;
	string rsltname;
	string rootname;
	
	
	float chisq;
	float chisq_proj;
	float chisq_targ;

	int intiflag;
	int metest;
	int minitest;
	bool do_calc;
	float dme, tme;
	float dme_prv, tme_prv;
	float stepSize_dme;
	float stepSize_tme;

	vector<float> result_vector;
	int index;

};

#endif
