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
			
			cout << "No INTI file given, ";
			cout << "so integration will not be performed" << endl;
			intifile = "empty";
			
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
		
		// Summarise data input
		cout << "Ndata projectile: " << Ndata_proj << endl;
		if( g2 ) cout << "          target: " << Ndata_targ << endl;
		
		// Matrix element indicies - "xme" or x-axis
		if( result.count("x") ) {
			
			xme_index = result["x"].as<int>();
			cout << "Matrix element on x-axis has index = " << xme_index << endl;
			
		}
		
		else {
			
			cout << "Matrix element on x-axis has the default index = " << xme_index << endl;
			
		}
		
		// Matrix element indicies - "yme" or y-axis
		if( result.count("y") ) {
			
			yme_index = result["y"].as<int>();
			cout << "Matrix element on y-axis has index = " << yme_index << endl;
			
		}
		
		else {
			
			cout << "Matrix element on y-axis has the default index = " << yme_index << endl;
			
		}
		
		// Trap problem if scanning the same matrix element against itself!
		if( xme_index == yme_index ) {
			
			cout << "Cannot scan matrix elements with index = " << xme_index << " and " << yme_index << endl;
			exit(1);
			
		}
		
		// Matrix element limits - "xme" or x-axis
		if( result.count("x-low") ) {
			
			low_xme = result["x-low"].as<float>();
			
		}
		
		if( result.count("x-upp") ) {
			
			upp_xme = result["x-upp"].as<float>();
			
		}
		
		// Matrix element limits - "yme" or y-axis
		if( result.count("y-low") ) {

			low_yme = result["y-low"].as<float>();
			
		}
		
		if( result.count("y-upp") ) {
			
			upp_yme = result["y-upp"].as<float>();
			
		}
		
		// Ranges summary
		cout << "Scanning x-axis from " << low_xme << " to " << upp_xme << endl;
		cout << "Scanning y-axis from " << low_yme << " to " << upp_yme << endl;
		
		// Number of scan steps
		if( result.count("nx") ) {
			
			Nsteps_xme = result["nx"].as<int>();
			cout << "Using " <<  Nsteps_xme << " steps in x-axis\n";
			
		}
		
		else {
			
			cout << "Using default number of steps in x-axis = " << Nsteps_xme << endl;
			
		}
		
		if( result.count("ny") ) {
			
			Nsteps_yme = result["ny"].as<int>();
			cout << "Using " <<  Nsteps_yme << " steps in y-axis\n";
			
		}
		
		else {
			
			cout << "Using default number of steps in y-axis = " << Nsteps_yme << endl;
			
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
		
			Npara = result["parallel"].as<int>();
			cout << "Running " << Npara << " parallel calculations" << endl;
			//cout << "Parallel calculations not yet implemented" << endl;

		}

		// Read and continue previous scan?
		if( result.count("r") ) {
			
			readflag = true;
			
		}

	}
	
	catch( const cxxopts::OptionException& e ) {
		
		cout << "error parsing options: " << e.what() << endl;
		exit(1);

	}
	
	
	// Start the timer to measure how long the scan takes
	auto start = std::chrono::high_resolution_clock::now();
	
	// We need the ROOT objects to store the results of the scan
	//cout << "ROOTing...\n";
	rootobjs ro;
	ro.SetupRoot( low_xme, upp_xme, Nsteps_xme,
				  low_yme, upp_yme, Nsteps_yme );

	// Put all of this into the scan
	//cout << "Scanning...\n";
	scan s;
	s.SetupScan( in_proj, intifile, yme_index, xme_index,
		    low_xme, upp_xme, Nsteps_xme,
		    low_yme, upp_yme, Nsteps_yme,
			Ndata_proj, Ndata_targ,
			Nmini, Npara,
		    g2, readflag, ro );
	
	// Then actually perform the scanning procedure
	s.run_scan();
	
	// Stop the timer
	auto stop = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
	cout << "Scan took " << duration.count()/1e6 << " seconds" << endl;

	return 0;
	
}
