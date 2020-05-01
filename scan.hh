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

#ifndef __rootobjs_hh__
#include "rootobjs.hh"
#endif

//#ifndef __workers_hh__
//#include "workers.hh"
//#endif

#if defined(__APPLE__)
#include <mach/thread_policy.h>
#include <mach/thread_act.h>

#define SYSCTL_CORE_COUNT	"machdep.cpu.core_count"

typedef struct cpu_set {
	uint32_t    count;
} cpu_set_t;

const inline void
CPU_ZERO( cpu_set_t *cs ) { cs->count = 0; }

const inline void
CPU_SET( int num, cpu_set_t *cs ) { cs->count |= (1 << num); }

const inline int
CPU_ISSET( int num, cpu_set_t *cs ) { return ( cs->count & (1 << num) ); }

inline int sched_getaffinity( pid_t pid, size_t cpu_size, cpu_set_t *cpu_set ) {
	
	int32_t core_count = 0;
	size_t len = sizeof(core_count);
	int ret = sysctlbyname( SYSCTL_CORE_COUNT, &core_count, &len, 0, 0 );
	
	if( ret ) {
		cout << "error while get core count " << ret;
		return -1;
	}
	cpu_set->count = 0;
	
	for( int i = 0; i < core_count; i++ )
		cpu_set->count |= (1 << i);
	
	return 0;
	
}

inline int pthread_setaffinity_np( pthread_t thread, size_t cpu_size,
						   cpu_set_t *cpu_set ) {
	
	thread_port_t mach_thread;
	int core = 0;

	for( core = 0; core < 8 * cpu_size; core++ ) {
		if( CPU_ISSET( core, cpu_set ) ) break;
	}
	
	//cout << "binding to core " << core << endl;
	thread_affinity_policy_data_t policy = { core };
	mach_thread = pthread_mach_thread_np( thread );
	thread_policy_set( mach_thread, THREAD_AFFINITY_POLICY,
					  (thread_policy_t)&policy, 1 );
	
	return 0;
	
}
#endif

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
