// The main routine for performing the scans
// Liam Gaffney (liam.gaffney@liverpool.ac.uk) - 23/04/2020

#ifndef __scan_hh__
#define __scan_hh__

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <thread>
#include <future>
using namespace std;

#include <pthread.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/sysctl.h>

#if defined(__LINUX__)
#include <linux/limits.h>
#endif

#ifndef PATH_MAX
#define PATH_MAX 1024
#endif

#ifndef __rootobjs_hh__
#include "rootobjs.hh"
#endif

//#ifndef __workers_hh__
//#include "workers.hh"
//#endif

class scan {
	
public:
	
	// Constructor/destructor
	scan();
	~scan();
	
	// Lookup functions
	std::string	FindFileName( string in_file, string tape );
	void		LookUpOldChisq( float xme, float yme, float &chisq_proj, float &chisq_targ, bool &do_calc );
	void		ContinueScan();
	void		ReadChiSqFromFile( string gosiaoutfile, float &chisq );
	void		GetChiSq( string dirname, float &chisq_proj, float &chisq_targ );

	// Execution functions
	void		IntegrateProjectile( string dirname );
	void		WriteProjectileMatrixElementsToFile( string dirname, float xme, float yme );
	void		WriteTargetMatrixElementsToFile( string dirname, float xme, float yme );
	void		RunCmd( std::string cmd );
	
	// Main functions
	void		run_scan();
	void		do_step( string dirname, int i, int j, float xme, float yme );
	
	// Getting the files
	void		MakeScanDirectories();
	void		GetAuxFiles();
	void		CopyFileForScan( string filename );
	void		OpenOutputFiles();
	void		CloseOutputs();
	void		CleanDirectories();

	// Some nice functions
	std::string	getDateTime();
	void		PrintStep( float xme, float yme, float chisq_proj, float chisq_targ);
	void		PrintResults();
	void		PrintHeader();

	// Main Setup Function
	void	SetupScan( std::string _in_proj, std::string _intifile,
					   float _xme_index, float _yme_index,
					   float _low_xme, float _upp_xme, int _Nsteps_xme,
					   float _low_yme, float _upp_yme, int _Nsteps_yme,
					   int _Ndata_proj, int _Ndata_targ,
					   int _Nmini, int _Npara,
					   bool _g2, bool _readflag, rootobjs ro );


private:
	
	// Scan Variables
	std::string intifile;
	std::string in_proj;
	std::string in_targ;
	std::string out_proj;
	std::string out_targ;
	std::string bst_proj;
	std::string bst_targ;
	std::string cor_proj;
	std::string cor_targ;
	std::string yld_proj;
	std::string yld_targ;
	std::string map_proj;
	std::string map_targ;
	std::string raw_proj;
	std::string gdt_proj;

	std::string scanname;
	std::string maindir;
	std::vector<std::string> scandir;
	std::string tstamp;
	std::mutex mlock;
	
	std::vector<int> i_todo;
	std::vector<int> j_todo;
	std::vector<float> xme_todo;
	std::vector<float> yme_todo;

	int yme_index;
	int xme_index;
	float low_yme;
	float upp_yme;
	int Nsteps_yme;
	float low_xme;
	float upp_xme ;
	int Nsteps_xme;
	
	int Ndata_proj;
	int Ndata_targ;

	int Nmini;
	int Npara;
	bool g2;
	bool readflag;
	
	rootobjs ro;

	std::string outname;
	std::vector<std::string> textname;
	std::vector<std::ofstream*> out;
	std::ofstream outa, outb;
	std::ofstream rslt;
	std::ifstream old;
	std::string rsltname;
	std::string rootname;
	
	bool intiflag;
	bool no_calc;
	float stepSize_xme;
	float stepSize_yme;

	std::vector<float> result_vector;

};

#endif
