// The main routine for performing the scans
// Liam Gaffney (liam.gaffney@liverpool.ac.uk) - 23/04/2020

#ifndef __scan_hh__
#define __scan_hh__

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
using namespace std;

#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

#ifndef __rootobjs_hh__
#include "rootobjs.hh"
#endif

class scan {
	
public:
	
	// Constructor/destructor
	scan();
	~scan();
	
	// Lookup functions
	int		LookUpOldChisq( float dme, float tme );
	void	ContinueScan();
	string	FindFileName( string in_file, string tape );
	double	ReadChiSqFromFile( string gosiaoutfile );
	int		GetChiSq( string dirname, float &chisq_proj );
	int		GetChiSq2( string dirname, float &chisq_proj, float &chisq_targ );

	// Execution functions
	int		IntegrateProjectile( string dirname );
	int		WriteProjectileMatrixElementsToFile( string dirname, float dme, float tme );
	int		WriteTargetMatrixElementsToFile( string dirname, float dme, float tme );

	// Main functions
	void	run_scan();
	void	do_step( string dirname, int i, int j, float dme, float tme );
	
	// Getting the files
	void	MakeScanDirectories();
	void	GetAuxFiles();
	void	CopyFileForScan( string filename );
	void	OpenOutputFiles();
	void	CloseOutputs();
	void	CleanDirectories();
	
	// Some nice functions
	string	getDateTime();
	void	PrintStep( float dme, float tme, float chisq_proj, float chisq_targ);
	void	PrintResults();
	
	// Main Setup Function
	void	SetupScan( string _in_proj, string _intifile,
					   float _dme_index, float _tme_index,
					   float _low_dme, float _upp_dme, int _Nsteps_dme,
					   float _low_tme, float _upp_tme, int _Nsteps_tme,
					   int _Ndata_proj, int _Ndata_targ,
					   int _Nmini, int _Npara,
					   bool _g2, bool _readflag, rootobjs ro );


private:
	
	// Scan Variables
	string intifile;
	string in_proj;
	string in_targ;
	string out_proj;
	string out_targ;
	string bst_proj;
	string bst_targ;
	string cor_proj;
	string cor_targ;
	string yld_proj;
	string yld_targ;
	string map_proj;
	string map_targ;
	string raw_proj;
	string gdt_proj;

	string scanname;
	string maindir;
	vector<string> scandir;
	string tstamp;

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
	bool readflag;
	
	rootobjs ro;

	string outname;
	vector<string> textname;
	vector<ofstream*> out;
	ofstream outa, outb;
	ofstream rslt;
	ifstream old;
	string rsltname;
	string rootname;
	
	int intiflag;
	int metest;
	int minitest;
	bool do_calc;
	bool no_calc;
	float stepSize_dme;
	float stepSize_tme;

	vector<float> result_vector;
	int index;

};

#endif
