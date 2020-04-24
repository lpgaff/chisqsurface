// Read in gosia2 input files and do a loop over a range of matrix elements,
// extracting and plotting the chisq as we go

#include "cxxopts.hpp"
#include "chisqsurface.hh"

int main( int argc, char* argv[] ) {
	
	// Options parser
	try {
		
		cxxopts::Options options( "chisqsurface",
							 "Program to create 2-dimensional chi^2 surfaces with Gosia/Gosia2" );
	
		options.add_options()
		( "m,mini", "OP,MINI file", cxxopts::value<string>(), "filename" )
		( "i,inti", "OP,INTI file", cxxopts::value<string>(), "filename" )
		( "np", "Number of projectile data", cxxopts::value<int>(), "N" )
		( "nt", "Number of target data", cxxopts::value<int>(), "N" )
		( "x,x-index", "Index of x-axis matrix element", cxxopts::value<int>(), "X" )
		( "y,y-index", "Index of y-axis matrix element", cxxopts::value<int>(), "Y" )
		( "nx", "Number of steps in the x-axis matrix element", cxxopts::value<int>(), "N" )
		( "ny", "Number of steps in the y-axis matrix element", cxxopts::value<int>(), "N" )
		( "x-low", "Lower limit for x-axis matrix element", cxxopts::value<float>(), "value" )
		( "x-upp", "Upper limit for x-axis matrix element", cxxopts::value<float>(), "value" )
		( "y-low", "Lower limit for y-axis matrix element", cxxopts::value<float>(), "value" )
		( "y-upp", "Upper limit for y-axis matrix element", cxxopts::value<float>(), "value" )
		( "nm", "Number of minimisation calls per step", cxxopts::value<int>(), "N" )
		( "p,parallel", "Number of parallel calculations", cxxopts::value<int>(), "N" )
		( "g1", "Standard Gosia selector" )
		( "g2", "Gosia2 selector (default)" )
		( "h,help", "Print help" )
		( "r,read", "Read previous results and continue" )
		;
		
		options.parse_positional( { "mini", "np" } );
		
		auto result = options.parse(argc, argv);
		
		// Do help
		if( result.count("help") ) {
			
			cout << options.help({""}) << endl;
			return 0;
			
		}

		
		// Input files - OP,MINI
		if( result.count("m") ) {
			
			in_proj = result["m"].as<string>();
			
		}
		
		else {
			
			cout << "OP,MINI file must be given with option -m.\n";
			cout << "For help, run with the -h or --help option.\n";
			return 1;
			
		}

		// Input files - OP,INTI
		if( result.count("i") ) {
			
			intifile = result["i"].as<string>();
			
		}
		
		else {
			
			intifile = in_proj.substr( 0, in_proj.find_last_of(".") );
			intifile += ".INTI.inp";
			
		}
		
		// Gosia or Gosia2?
		if( result.count("g1") ) {
			
			g2 = false;
			cout << "Using standard Gosia\n";
			
		}
		
		else if( result.count("g2") ) {
			
			g2 = true;
			cout << "Using Gosia2\n";
			
		}

		else {
			
			g2 = true;
			cout << "Using Gosia2 as default (use --g1 to switch to standard Gosia)\n";
			
		}
		
		// Number of data - projectile
		if( result.count("np") ) {
			
			Ndata_proj = result["np"].as<int>();
			
		}
		
		else {
			
			cout << "Please give the number of projectile data with --np=X\n";
			exit(1);
			
		}
		
		// Number of data - target
		if( result.count("nt") ) {
			
			Ndata_targ = result["nt"].as<int>();
			
		}
		
		else if( !result.count("nt") && g2 ) {
			
			cout << "Please give the number of target data with --nt=X\n";
			exit(1);
			
		}
		
		// Matrix element indicies - "DME" or x-axis
		if( result.count("x") ) {
			
			dme_index = result["x"].as<int>();
			cout << "Matrix element on x-axis has index = " << dme_index << endl;
			
		}
		
		else {
			
			cout << "Matrix element on x-axis has the default index = " << dme_index << endl;
			
		}
		
		// Matrix element indicies - "TME" or y-axis
		if( result.count("y") ) {
			
			tme_index = result["y"].as<int>();
			cout << "Matrix element on y-axis has index = " << tme_index << endl;
			
		}
		
		else {
			
			cout << "Matrix element on y-axis has the default index = " << tme_index << endl;
			
		}
		
		// Trap problem if scanning the same matrix element against itself!
		if( dme_index == tme_index ) {
			
			cout << "Cannot scan matrix elements with index = " << dme_index << " and " << tme_index << endl;
			exit(1);
			
		}
		
		// Matrix element limits - "DME" or x-axis
		if( result.count("x-low") ) {
			
			low_dme = result["x-low"].as<float>();
			
		}
		
		if( result.count("x-upp") ) {
			
			upp_dme = result["x-upp"].as<float>();
			
		}
		
		// Matrix element limits - "TME" or y-axis
		if( result.count("y-low") ) {

			low_tme = result["y-low"].as<float>();
			
		}
		
		if( result.count("y-upp") ) {
			
			upp_tme = result["y-upp"].as<float>();
			
		}
		
		// Ranges summary
		cout << "Scanning x-axis from " << low_dme << " to " << upp_dme << endl;
		cout << "Scanning y-axis from " << low_tme << " to " << upp_tme << endl;
		
		// Number of scan steps
		if( result.count("nx") ) {
			
			Nsteps_dme = result["nx"].as<int>();
			cout << "Using " <<  Nsteps_dme << " steps in x-axis\n";
			
		}
		
		else {
			
			cout << "Using default number of steps in x-axis = " << Nsteps_dme << endl;
			
		}
		
		if( result.count("ny") ) {
			
			Nsteps_tme = result["ny"].as<int>();
			cout << "Using " <<  Nsteps_tme << " steps in y-axis\n";
			
		}
		
		else {
			
			cout << "Using default number of steps in y-axis = " << Nsteps_tme << endl;
			
		}
		
		// Number of minimisations per step
		if( result.count("nm") ) {
		
			Nmini = result["nm"].as<int>();
			cout << "Using " << Nmini << " minimisations per step" << endl;

		}
		
		else {
			
			cout << "Using default number of minimisations per step (nm = " << Nmini << ")" << endl;
			
		}

		// Number of parallel calculations
		if( result.count("parallel") ) {
		
			Nmini = result["parallel"].as<int>();
			cout << "Running " << Npara << " parallel calculations" << endl;

		}

		// Read and continue previous scan?
		if( result.count("r") ) {
			
			read = true;
			
		}

	}
	
	catch( const cxxopts::OptionException& e ) {
		
		cout << "error parsing options: " << e.what() << endl;
		exit(1);

	}
	
	
	// We need the ROOT objects to store the results of the scan
	//cout << "ROOTing...\n";
	rootobjs ro;
	ro.SetupRoot( low_dme, upp_dme, Nsteps_dme,
				  low_tme, upp_tme, Nsteps_tme );

	// Put all of this into the scan
	//cout << "Scanning...\n";
	scan s;
	s.SetupScan( in_proj, intifile, tme_index, dme_index,
		    low_tme, upp_tme, Nsteps_tme,
		    low_dme, upp_dme, Nsteps_dme,
			Ndata_proj, Ndata_targ,
			Nmini, Npara,
		    g2, read, ro );
	
	// Then actually perform the scanning procedure
	s.run_scan();

	return 0;
	
}
