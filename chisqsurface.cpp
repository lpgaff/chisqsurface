// Read in gosia2 input files and do a loop over a range of matrix elements,
// extracting and plotting the chisq as we go

#include <cstdlib>
#include <ctime>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>

#include "TFile.h"
#include "TH2D.h"
#include "TGraph.h"
#include "TGraph2D.h"

#include "cxxopts.hpp"

using namespace std;

string convertInt( int number ) {
	
	stringstream ss;
	ss << number;
	return ss.str();
	
}

string convertFloat( float number ) {
	
	stringstream ss;
	ss << number;
	return ss.str();
	
}

string getDateTime() {
	
	// get time now
	time_t t = time(0);
	struct tm * now = localtime( & t );

	// make it into a nice string YYYYMMDD-HHMMSS
	string date_string = convertInt( now->tm_year + 1900 );
	if ( now->tm_mon < 9 ) date_string += "0";
	date_string += convertInt( now->tm_mon + 1 );
	if ( now->tm_mday < 10 ) date_string += "0";
	date_string += convertInt( now->tm_mday );
	
	date_string += "_";
	if ( now->tm_hour < 10 ) date_string += "0";
	date_string += convertInt( now->tm_hour );
	if ( now->tm_min < 10 ) date_string += "0";
	date_string += convertInt( now->tm_min );
	if ( now->tm_sec < 10 ) date_string += "0";
	date_string += convertInt( now->tm_sec );
	
	return date_string;

	
}

int LookUpOldChisq( const vector<float>& vec, float dme, float tme ) {

	// Cycle through array and return index that matches matrix elements
	int index = -1;
	
	for ( int i = 0; i < int(vec.size()/5); i++ ) {
	
		if ( TMath::Abs( dme - vec[5*i+0] ) < 1E-6 ) {
			if ( TMath::Abs( tme - vec[5*i+1] ) < 1E-6 ) {

				index = i; // found it!
				break; // stop looking once we've found it!

			}
		}
			
	}

	return index; 

}

string FindFileName( string in_file, string tape ) {
	
	// Open gosia input file for projectile and find the output file name
	ifstream gin;
	gin.open( in_file.c_str(), ios::in );
	if( !gin.is_open() ) {
		
		cout << "Unable to open " << in_file << endl;
		exit(1);
		
	}
	
	// Search for tape 26 (or other)
	bool flag1 = false;
	bool flag2 = false;
	string line1, line2, tmp;
	string qry1 = "OP,FILE";
	string qry2 = "OP,";
	string filename = "empty";
 
	getline( gin,line1 );
	while ( !gin.eof() && !flag1 ) {
		
		if( line1.compare( 0, qry1.size(), qry1 ) == 0 )
			flag1 = true;
		
		getline( gin, line1 );
		
	}
	
	getline( gin, line2 );
	while( !gin.eof() && !flag2 ) {
		
		if( line1.compare( 0, qry2.size(), qry2 ) == 0 ||
		   line2.compare( 0, qry2.size(), qry2 ) == 0 ) {
			
			break;
			
		}
		
		if( line1.compare( 0, tape.size(), tape ) == 0 ) {
			
			filename = line2;
			flag2 = true;
			
		}
		
		getline( gin, line1 );
		getline( gin, line2 );
		
	}
	
	gin.close();
	
	if( !flag1 ) cout << "Couldn't find OP,FILE in " << in_file << endl;
	if( !flag2 ) cout << "Couldn't find tape number " << tape << " in " << in_file << endl;
	else cout << "Found tape number " << tape << ": " << filename << endl;
	
	return filename;
	
}

double ReadChiSqFromFile( string gosiaoutfile ) {

	// Open gosia output file for projectile or target. Get from input 
	ifstream g2out;
	g2out.open( gosiaoutfile.c_str(), ios::in );	
	if( !g2out.is_open() ) {
	
		cout << "Unable to open " << gosiaoutfile << endl;
		return 999;

	}
	
	// Search for chisq value in file
	bool flag = false;
	string line, tmp;
	string qry = "     *** CHISQ=";
	stringstream gosia_chisq (stringstream::in | stringstream::out);
	double chisq = 999;
 
	getline(g2out,line);
	while ( !g2out.eof() ) {
		
		getline( g2out, line );
		if( line.compare( 0, qry.size(), qry ) == 0 ) {
			
			gosia_chisq.str("");
			gosia_chisq.clear();
			gosia_chisq << line.substr( qry.size(), qry.size()+15 );
			gosia_chisq >> chisq;
			flag = true;
			
		}
	}
	
	g2out.close();
	
	if( !flag ) cout << "Couldn't find chisq value in " << gosiaoutfile << endl;
	
	return chisq;

}

int GetChiSq( string in_proj, string out_proj, double &chisq_proj, int Nmini ) {
	
	string cmd = "gosia < " + in_proj;
	cmd.append(" > /dev/null 2>&1");
	
	int status = 0;
	
	// Run gosia Nmini times with system command
	for( int i = 0; i < Nmini; i++ ) {
		
		if( system(NULL) ) status = system( cmd.c_str() );
		else {
			
			cout << "Cannot run system command\n";
			exit(1);
			
		}
		
	}
	
	// Error handling
	if( status == 512 ) {
		
		cout << "Check that Gosia runs correctly\n";
		exit(1);
		
	}
	
	else if( status == 2 ) {
		
		cout << "Killed!\n";
		exit( status );
		
	}
	
	else chisq_proj = ReadChiSqFromFile( out_proj );
	
	return 1;
	
}

int GetChiSq2( string in_proj, string out_proj, string out_targ, double &chisq_proj, double &chisq_targ, int Nmini ) {
	
	string cmd = "gosia2 < " + in_proj;
	cmd.append(" > /dev/null 2>&1");
	
	int status = 0;
	
	// Run gosia Nmini times with system command
	for( int i = 0; i < Nmini; i++ ) {
		
		if( system(NULL) ) status = system( cmd.c_str() );
		else {
			
			cout << "Cannot run system command\n";
			return 0;
			
		}
		
	}
	
	// Error handling
	if( status == 512 ) {
		
		cout << "Check that Gosia2 runs correctly\n";
		exit(1);
		
	}
	
	else if( status == 2 ) {
		
		cout << "Killed!\n";
		exit( status );
		
	}
	
	else {
		
		chisq_proj = ReadChiSqFromFile( out_proj );
		chisq_targ = ReadChiSqFromFile( out_targ );
		
	}
	
	return 1;
	
}

int IntegrateProjectile( string intifile ) {

	string line, cmd;
	
	ifstream inti;
	inti.open( intifile.c_str(), ios::in );
	if( !inti.is_open() ) return 0;
	else {
		
		// Check first line of integral file
		getline( inti, line );

		// If it looks like a gosia2 input, use that, else just normal gosia
		if( line == "1" || line == "2" ) cmd = "gosia2 < ";
		else cmd = "gosia < ";
		
		inti.close();
		
	}
	
	cmd.append( intifile );
	cmd.append( " > /dev/null 2>&1" );
	
	if( system(NULL) ) system( cmd.c_str() );
	else {
		
		cout << "Cannot run system command\n";
		return 0;
		
	}

	return 1;
	
}

int WriteProjectileMatrixElementsToFile( string bst_proj, float tme, float dme, int tme_index, int dme_index ) {
	
	string litname;
	ofstream mefile;
	ifstream litfile;
	string cmd;
	double tmp;
	int index = 1;
	
	// Projectile matrix elements
	litname = bst_proj + ".lit";
	
	mefile.open( bst_proj.c_str(), ios::out );
	if( !mefile.is_open() ) return 1;
	
	litfile.open( litname.c_str(), ios::in );
	if( !litfile.is_open() ) return 1;
	
	litfile >> tmp; // read first me value
	
	while ( !litfile.eof() ) {
		
		if ( index == tme_index ) {
			
			mefile << tme << endl; // write current dme
			litfile >> tmp; // dump initial tme value and read next one
			
		}
		
		else if ( index == dme_index ) {
			
			mefile << dme << endl; // write current dme
			litfile >> tmp; // dump initial dme value and read next one
			
		}
		
		else {
			
			mefile << tmp << endl;	// write to file if there is a next value
			litfile >> tmp;	 // continue reading
			
		}
		
		index++; // increment matrix element index
		
	}
	
	mefile.close();
	litfile.close();
	
	return 0;
	
}

int WriteTargetMatrixElementsToFile( string bst_targ, float tme, float dme, int tme_index, int dme_index ) {
	
	string litname;
	ofstream mefile;
	ifstream litfile;
	string cmd;

	// Target matrix elements, copy from backup file
	litname = bst_targ + ".lit";
	
	litfile.open( litname.c_str(), ios::in );
	if( !litfile.is_open() ) return 2;
	else litfile.close();
	
	cmd = "cp " + litname + " " + bst_targ;
	if( system(NULL) ) system( cmd.c_str() );
	else return 2;
	
	return 0;
	
}


void PrintUsage( char* progname ) {

	cout << "\nUsage: \n" << progname << " <in_proj> <in_targ> ";
	cout << "<Ndata_proj=3> <Ndata_targ=3> <low_TME=0.1> \n";
	cout << "   <upp_TME=2.5> <Nsteps_TME=51> <low_DME=0.0> <upp_DME=0.0> <Nsteps_DME=1>\n\n";
	cout << " where <Ndata_proj=3> and <Ndata_targ=5> are the number of data\n";
	cout << "for the projectile and target, respectively. This includes the\n";
	cout << "sum of all g-ray yields, matrix elements, lifetimes, etc.\n\n";
	cout << "Only <in_proj> and <in_targ>, the input projectile and target files ready\n";
	cout << "for minimisation, are required. All others have default values as \n";
	cout << "indicated in the usage.\n";
	cout << "If \"cont\" is included at the end, then the calculation will\n";
	cout << "continue from the last point reading other values from the .chisq file.\n";
	cout << "If \"read\" is included at the end, then the calculation will\n";
	cout << "check the corresponding .chisq file for previous calculations at each meshpoint\n";
	cout << "so as not to repeat a step. This cannot be declared together with \"cont\".\n";
	cout << "If \"-dN\" is included at the end, where N is the index of the diagonal matrix\n";
	cout << "element other than the default 2, then correct lines of the .bst/.bst.lit\n";
	cout << "files will be read/written.\n";
	cout << "Switch between Gosia and Gosia2 using the -g1 or -g2 flags\n";
	cout << "Gosia2 (-g2) is the default. Overwrite with -g1 for standard Gosia.\n";

	return;
	
}

int main( int argc, char* argv[] ) {
	
	// If the number of arguments are wrong, exit with usage
	if( argc < 2 ) {
		
		PrintUsage(argv[0]);
		exit(1);
	
	}
	
	// Get/Set arguments
	string in_proj, in_targ;
	string out_proj, out_targ;
	string intifile;
	string bst_proj, bst_targ;
	int Ndata_proj = 3;
	int Ndata_targ = 5;
	float low_tme = 0.1;
	float upp_tme = 2.5;
	int Nsteps_tme = 51;
	float low_dme = 0.0;
	float upp_dme = 0.0;
	int Nsteps_dme = 1;
	int Nmini = 1;
	bool cont = false;
	bool read = false;
	bool g2 = true;		// default, gosia2
	int tme_index = 1;	// default, index 1
	int dme_index = 2;	// default, index 2
	
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
		( "g1", "Standard Gosia selector" )
		( "g2", "Gosia2 selector (default)" )
		( "h,help", "Print help" )
		( "r,read", "Read previous results and continue" )
		( "c,cont", "Continue previous calculation (use read instead!)" )
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
			
			cout << "OP,MINI file must be given with option -m\n";
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
		
		// Find corresponsing target file
		if( g2 ) in_targ = FindFileName( in_proj, "26" );
		if( in_targ == "empty" && g2 ) {
			
			cout << "Check your input files if you want to use Gosia2\n";
			exit(1);
			
		}
		
		// Find corresponsing output files and matrix element files
		out_proj = FindFileName( in_proj, "22" );
		if( g2 ) out_targ = FindFileName( in_targ, "22" );
		bst_proj = FindFileName( in_proj, "12" );
		if( g2 ) bst_targ = FindFileName( in_targ, "32" );
		if( out_proj == "empty" || out_targ == "empty" ||
		   bst_proj == "empty" || bst_targ == "empty" ) exit(1);

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
			
			cout << "Matrix element on t-axis has the default index = " << tme_index << endl;
			
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

		// Read or cont?
		if( result.count("r") ) {
			
			read = true;
			
		}
		
		else if( result.count("c") ) {
			
			cont = true;
			
		}
		
	}
	
	catch( const cxxopts::OptionException& e ) {
		
		cout << "error parsing options: " << e.what() << endl;
		exit(1);

	}

	// Open output text and root files
	//  if continuing "cont" read output file first
	//  if not, copy old files to ...old 
	string outname = in_proj.substr( 0, in_proj.find_last_of(".") );
	vector<string> textname;
	textname.push_back( outname + ".chisq" );
	textname.push_back( outname + ".chisq_" + getDateTime() );
	string rsltname = outname + ".rslt";
	string rootname = outname + ".root";
	vector<ofstream*> out;
	ofstream outa, outb;
	ofstream rslt;
	ifstream old;
	string cmd;
	
	if ( cont || read ) old.open( textname[0].c_str(), ios::in );
	else {
		
		outa.open( textname[0].c_str(), ios::out );
		outb.open( textname[1].c_str(), ios::out );
		out.push_back( &outa );
		out.push_back( &outb );
		
	}
	
	cmd = "cp " + textname[0] + " " + textname[0] + ".old";
	if( system(NULL) ) system( cmd.c_str() );
	cmd = "cp " + rootname + " " + rootname + ".old";
	if( system(NULL) ) system( cmd.c_str() );
	cmd = "cp " + rsltname + " " + rsltname + ".old";
	if( system(NULL) ) system( cmd.c_str() );

	rslt.open( rsltname.c_str(), ios::out );	
	
	TFile *root = new TFile( rootname.c_str(), "RECREATE" );
		
	// Initiate variables
	int intiflag;
	int metest;
	int minitest;
	bool do_calc = true;
	float dme, tme, dme_prv, tme_prv;
	double chisq = 999., chisq_proj = 999., chisq_targ = 999.;
	float stepSize_dme = ( upp_dme - low_dme ) / (float)(Nsteps_dme-1);
	float stepSize_tme = ( upp_tme - low_tme ) / (float)(Nsteps_tme-1);
	if( Nsteps_dme == 1 ) stepSize_dme = 0;
	if( Nsteps_tme == 1 ) stepSize_tme = 0;
	
	vector<float> result_vector;
	int index;

	// If continuing or reading old values, get last calculated values
	if ( cont || read ) {

		if ( old.is_open() ) {

			old >> dme_prv >> tme_prv >> chisq_proj >> chisq_targ >> chisq;
			while ( !old.eof() ) {

				result_vector.push_back( (float)dme_prv );
				result_vector.push_back( (float)tme_prv );
				result_vector.push_back( (float)chisq_proj );
				result_vector.push_back( (float)chisq_targ );
				result_vector.push_back( (float)chisq );

				old >> dme_prv >> tme_prv >> chisq_proj >> chisq_targ >> chisq;
			
			}
			
			chisq = chisq_proj = chisq_targ = 999.;
	
			cout << "\nRead array of " << int(result_vector.size()/5);
			cout << " results from previous calculation...\n";
							
			// Reopen file for writing
			old.close();
			outa.open( textname[0].c_str(), ios::out );
			outb.open( textname[1].c_str(), ios::out );
			out.push_back( &outa );
			out.push_back( &outb );
			
			if ( cont ) {			
				cout << "\nContinuing from a previous state...\n";
				cout << "Previous DME = " << dme_prv << endl;
				cout << "Previous TME = " << tme_prv << endl << endl;
			}
			
		}
		
		else {
		
			cout << "Cannot open " << textname[0].c_str() << " in order to resume\n";
			cont = false;
		
		}
		
	}

	// A 2-dimensional chisq graph in root plus 1sigma cut
	TGraph2D *gChisq = new TGraph2D( Nsteps_dme*Nsteps_tme );
	TGraph2D *gChisq_proj = new TGraph2D();
	TGraph2D *gChisq_targ = new TGraph2D();
	TGraph2D *gChisq_1sigma = new TGraph2D();
	TGraph2D *gChisq_2sigma = new TGraph2D();
	TGraph2D *gChisq_1sigma_rotorlim = new TGraph2D();
	TGraph2D *gChisq_2sigma_rotorlim = new TGraph2D();
	gChisq->SetName("gChisq");
	gChisq_proj->SetName("gChisq_proj");
	gChisq_targ->SetName("gChisq_targ");
	gChisq_1sigma->SetName("gChisq_1sigma");
	gChisq->SetTitle("#chi^{2} surface plot (N_{p}#chi^{2}_{p} + N_{t}#chi^{2}_{t});<2^{+}||E2||2^{+}> [eb];<0^{+}||E2||2^{+}> [eb];#chi^{2}");
	gChisq_proj->SetTitle("#chi^{2} surface plot for projectile;<2^{+}||E2||2^{+}> [eb];<0^{+}||E2||2^{+}> [eb];#chi^{2}");
	gChisq_targ->SetTitle("#chi^{2} surface plot for target;<2^{+}||E2||2^{+}> [eb];<0^{+}||E2||2^{+}> [eb];#chi^{2}");
	gChisq_1sigma->SetTitle("#chi^{2}+1 cut;<2^{+}||E2||2^{+}> [eb];<0^{+}||E2||2^{+}> [eb];#chi^{2}");
	gChisq_2sigma->SetTitle("#chi^{2}+2 cut;<2^{+}||E2||2^{+}> [eb];<0^{+}||E2||2^{+}> [eb];#chi^{2}");
	gChisq_1sigma_rotorlim->SetTitle("#chi^{2}+1 cut plus rigid rotor limits;<2^{+}||E2||2^{+}> [eb];<0^{+}||E2||2^{+}> [eb];#chi^{2}");
	gChisq_2sigma_rotorlim->SetTitle("#chi^{2}+2 cut plus rigid rotor limits;<2^{+}||E2||2^{+}> [eb];<0^{+}||E2||2^{+}> [eb];#chi^{2}");
	if( Nsteps_dme > 4 && Nsteps_dme < 999 ) {
		gChisq->SetNpx(Nsteps_dme-1);
		gChisq_1sigma->SetNpx(Nsteps_dme-1);
		gChisq_2sigma->SetNpx(Nsteps_dme-1);
		gChisq_1sigma_rotorlim->SetNpx(Nsteps_dme-1);
		gChisq_2sigma_rotorlim->SetNpx(Nsteps_dme-1);
	}
	if( Nsteps_tme > 4 && Nsteps_tme < 999 ) {
		gChisq->SetNpy(Nsteps_tme);
		gChisq_1sigma->SetNpy(Nsteps_tme);
		gChisq_2sigma->SetNpy(Nsteps_tme);
		gChisq_1sigma_rotorlim->SetNpy(Nsteps_tme);
		gChisq_2sigma_rotorlim->SetNpy(Nsteps_tme);
	}
	
	// 1-dimensional chisq graphs for each dme
	string gName, gTitle, dme_str;
	TGraph *gChisqDME[(const int)Nsteps_dme];
	for( int id = 0; id < Nsteps_dme; id++ ){
		dme_str = convertFloat( (float)(low_dme + id*stepSize_dme) );
		gName = "gChisq_" + convertInt(id);
		gTitle = "#chi^{2} surface plot for DME = " + dme_str;
		gTitle += " eb;<0^{+}||E2||2^{+}> [eb];#chi^{2}";
		gChisqDME[id] = new TGraph(Nsteps_tme);
		gChisqDME[id]->SetTitle(gTitle.c_str());
	}
	
	// 2-D histograms for projectile and target
	TH2D *hChisq = new TH2D("hChisq",
		"#chi^{2} surface plot (N_{p}#chi^{2}_{p} + N_{t}#chi^{2}_{t});<2^{+}||E2||2^{+}> [eb];<0^{+}||E2||2^{+}> [eb];#chi^{2}",
		Nsteps_dme, low_dme-0.5*stepSize_dme, upp_dme+0.5*stepSize_dme,
		Nsteps_tme, low_tme-0.5*stepSize_tme, upp_tme+0.5*stepSize_tme);
	TH2D *hChisq_proj = new TH2D("hChisq_proj",
		"#chi^{2} surface plot for projectile N_{p}#chi^{2}_{p};<2^{+}||E2||2^{+}> [eb];<0^{+}||E2||2^{+}> [eb];#chi^{2}",
		Nsteps_dme, low_dme-0.5*stepSize_dme, upp_dme+0.5*stepSize_dme,
		Nsteps_tme, low_tme-0.5*stepSize_tme, upp_tme+0.5*stepSize_tme);
	TH2D *hChisq_targ = new TH2D("hChisq_targ",
		"#chi^{2} surface plot for target N_{t}#chi^{2}_{t};<2^{+}||E2||2^{+}> [eb];<0^{+}||E2||2^{+}> [eb];#chi^{2}",
		Nsteps_dme, low_dme-0.5*stepSize_dme, upp_dme+0.5*stepSize_dme,
		Nsteps_tme, low_tme-0.5*stepSize_tme, upp_tme+0.5*stepSize_tme);

	// Get chisq values and write to file
	cout << "Ndata projectile: " << Ndata_proj << endl;
	cout << "          target: " << Ndata_targ << endl;
	cout << "\n\t\t  Chi-squared value\n";
	if( g2 ) cout << "  x\t  y\tProj\tTarg\tTotal\n";
	else cout << "  x\t  y\tTotal\n";

	for ( int i=0; i<Nsteps_dme; i++ ) {
	
		dme = low_dme + i*stepSize_dme;

		for ( int j=0; j<Nsteps_tme; j++ ) {
		
			tme = low_tme + j*stepSize_tme;
			
			if ( read ) {
			
				index = LookUpOldChisq( result_vector, dme, tme );
				if ( index < 0 ) do_calc = true;
				else {
					
					chisq_proj = result_vector.at( 5*index+2 );
					chisq_targ = result_vector.at( 5*index+3 );
					chisq = result_vector.at( 5*index+4 );		
					do_calc = false;
					
				}
			
			} 
			
			else if ( cont ) {
			
				// Read previous values
				dme_prv = result_vector.at( 5*i*Nsteps_tme+0 );
				tme_prv = result_vector.at( 5*i*Nsteps_tme+1 );
				chisq_proj = result_vector.at( 5*i*Nsteps_tme+2 );
				chisq_targ = result_vector.at( 5*i*Nsteps_tme+3 );
				chisq = result_vector.at( 5*i*Nsteps_tme+4 );		
				
				// Check for consistency
				if ( TMath::Abs(dme_prv-dme) < 1E-8 && TMath::Abs(tme_prv-tme) < 1E-8 ) {
				
					do_calc = false;
					
				} else {
					
					cout << "\nPrevious matrix elements don't match\n";
					cout << "Make sure you're re-running in the exact same state,\n";
					cout << "or else you should use the \"read\" option.\n";
					do_calc = true;
				
				}
			
			} 
			
			else do_calc = true;
			
			cout << dme << "\t" << tme << "\t";

			if ( do_calc == true ) {
			
				// Write matrix elements
				metest = WriteProjectileMatrixElementsToFile( bst_proj, tme, dme, tme_index, dme_index );
				if( g2 )
					metest = WriteTargetMatrixElementsToFile( bst_targ, tme, dme, tme_index, dme_index );

				if( metest == 1 )
					cout << "Couldn't write projectile matrix elements to file\n";

				if( metest == 2 )
					cout << "Couldn't write target matrix elements to file\n";
				
				if( metest > 0 ) exit(1);
				
				// Integration step
				intiflag = IntegrateProjectile( intifile );
				
				// Run Gosia2 or standard Gosia and return chisq values
				if( g2 )
					minitest = GetChiSq2( in_proj, out_proj, out_targ, chisq_proj, chisq_targ, Nmini );

				else
					minitest = GetChiSq( in_proj, out_proj, chisq_proj, Nmini );
					
				if ( minitest == 0 ) {
					
					cout << "Unable to run gosia\n";
					exit(1);

				}
				
				else if ( minitest == 2 ) {
					
					cout << "\nKill signal recieved" << endl;
					return 2;
					
				}
				
				chisq_proj *= Ndata_proj;
				chisq_targ *= Ndata_targ;
				
				if( g2 ) chisq = chisq_proj + chisq_targ;
				else chisq = chisq_proj;
			
			}
	
			// Print to terminal
			if( g2 ) cout << chisq_proj << "\t" << chisq_targ << "\t" << chisq << endl;
			else cout << chisq << endl;

			// Print to file
			for ( int k = 0; k < 2; k++ ) {
				
				(*out[k]) << dme << "\t" << tme << "\t";
				(*out[k]) << chisq_proj << "\t" << chisq_targ << "\t" << chisq << endl;
				
			}
			
			// Write in root graphs and histograms
			gChisq->SetPoint( i*Nsteps_tme + j, dme, tme, chisq );
			hChisq->SetBinContent( i+1, j+1, chisq );
			gChisq_proj->SetPoint( i*Nsteps_tme + j, dme, tme, chisq_proj );
			gChisq_targ->SetPoint( i*Nsteps_tme + j, dme, tme, chisq_targ );
			hChisq_proj->SetBinContent( i+1, j+1, chisq_proj );
			hChisq_targ->SetBinContent( i+1, j+1, chisq_targ );
			gChisqDME[i]->SetPoint( j, tme, chisq );
				
		}

	}
	
	// Make 1sigma and 2sigma cuts (chisq_min + 1 and 2) and write values
	double chisq_min = gChisq->GetZmin();
	int ix, iy, iz;
	TH2D *hChisq_1sigma = new TH2D("hChisq_1sigma",
		"#chi^{2}+1 cut;<2^{+}||E2||2^{+}> [eb];<0^{+}||E2||2^{+}> [eb];#chi^{2}",
		Nsteps_dme, low_dme-0.5*stepSize_dme, upp_dme+0.5*stepSize_dme,
		Nsteps_tme, low_tme-0.5*stepSize_tme, upp_tme+0.5*stepSize_tme);
	TH2D *hChisq_1sigma_rotorlim = new TH2D("hChisq_1sigma_rotorlim",
		"#chi^{2}+1 cut plus rigid rotor limits;<2^{+}||E2||2^{+}> [eb];<0^{+}||E2||2^{+}> [eb];#chi^{2}",
		Nsteps_dme, low_dme-0.5*stepSize_dme, upp_dme+0.5*stepSize_dme,
		Nsteps_tme, low_tme-0.5*stepSize_tme, upp_tme+0.5*stepSize_tme);
	TH2D *hChisq_2sigma = new TH2D("hChisq_2sigma",
		"#chi^{2}+2 cut;<2^{+}||E2||2^{+}> [eb];<0^{+}||E2||2^{+}> [eb];#chi^{2}",
		Nsteps_dme, low_dme-0.5*stepSize_dme, upp_dme+0.5*stepSize_dme,
		Nsteps_tme, low_tme-0.5*stepSize_tme, upp_tme+0.5*stepSize_tme);
	TH2D *hChisq_2sigma_rotorlim = new TH2D("hChisq_2sigma_rotorlim",
		"#chi^{2}+2 cut plus rigid rotor limits;<2^{+}||E2||2^{+}> [eb];<0^{+}||E2||2^{+}> [eb];#chi^{2}",
		Nsteps_dme, low_dme-0.5*stepSize_dme, upp_dme+0.5*stepSize_dme,
		Nsteps_tme, low_tme-0.5*stepSize_tme, upp_tme+0.5*stepSize_tme);

	float tme_tmp, dme_tmp;
	double chisq_tmp;
	int ctr1 = 0, ctr2 = 0, ctr3 = 0, ctr4 = 0;
	for( int p = 0; p < Nsteps_dme; p++ ) {

		dme_tmp = low_dme + p*stepSize_dme;

		for( int q = 0; q < Nsteps_tme; q++ ) {

			tme_tmp = low_tme + q*stepSize_tme;
			chisq_tmp = hChisq->GetBinContent( p+1, q+1 );

			if( chisq_tmp <= chisq_min+1 ) {
				
				gChisq_1sigma->SetPoint( ctr1, dme_tmp, tme_tmp, chisq_tmp );
				hChisq_1sigma->SetBinContent( p+1, q+1, chisq_tmp );
				ctr1++;
				
				if( TMath::Abs(dme_tmp) <= tme_tmp*1.19523 ) {
					
					gChisq_1sigma_rotorlim->SetPoint(ctr2,dme_tmp,tme_tmp,chisq_tmp);
					hChisq_1sigma_rotorlim->SetBinContent(p+1,q+1,chisq_tmp);
					ctr2++;
					
				}
				
			}
			
			if( chisq_tmp <= chisq_min+2 ) {
				
				gChisq_2sigma->SetPoint( ctr3, dme_tmp, tme_tmp, chisq_tmp );
				hChisq_2sigma->SetBinContent( p+1, q+1, chisq_tmp );
				ctr3++;
				
				if( TMath::Abs(dme_tmp) <= tme_tmp*1.19523 ) {
					
					gChisq_2sigma_rotorlim->SetPoint(ctr4,dme_tmp,tme_tmp,chisq_tmp);
					hChisq_2sigma_rotorlim->SetBinContent(p+1,q+1,chisq_tmp);
					ctr4++;
					
				}
				
			}
			
		}

	}
	
	hChisq->GetMinimumBin(ix,iy,iz);
	float dme_min = hChisq->GetXaxis()->GetBinCenter(ix);
	float tme_min = hChisq->GetYaxis()->GetBinCenter(iy);

	double tme_err_low[2], dme_err_low[2];
	double tme_err_upp[2], dme_err_upp[2];

	for( int i = 0; i < 2; i++ ) {
		
		tme_err_low[i] = 0.5*stepSize_tme;
		tme_err_upp[i] = 0.5*stepSize_tme;
		dme_err_low[i] = 0.5*stepSize_dme;
		dme_err_upp[i] = 0.5*stepSize_dme;
		
	}

	tme_err_low[0] += TMath::Abs( tme_min - gChisq_1sigma->GetYmin() );
	tme_err_upp[0] += TMath::Abs( tme_min - gChisq_1sigma->GetYmax() );
	dme_err_low[0] += TMath::Abs( dme_min - gChisq_1sigma->GetXmin() );
	dme_err_upp[0] += TMath::Abs( dme_min - gChisq_1sigma->GetXmax() );
	
	tme_err_low[1] += TMath::Abs( tme_min - gChisq_2sigma->GetYmin() );
	tme_err_upp[1] += TMath::Abs( tme_min - gChisq_2sigma->GetYmax() );
	dme_err_low[1] += TMath::Abs( dme_min - gChisq_2sigma->GetXmin() );
	dme_err_upp[1] += TMath::Abs( dme_min - gChisq_2sigma->GetXmax() );
	
	cout << "\nChisq minimum found at " << chisq_min << endl;
	cout << "<0+1||E2||2+1> = " << tme_min << "(-" << tme_err_low[0];
	cout << "; +" << tme_err_upp[0] << ")1sig." << " (-" << tme_err_low[1];
	cout << "; +" << tme_err_upp[1] << ")2sig." << endl;
	cout << "<2+1||E2||2+1> = " << dme_min << "(-" << dme_err_low[0];
	cout << "; +" << dme_err_upp[0] << ")1sig." << " (-" << dme_err_low[1];
	cout << "; +" << dme_err_upp[1] << ")2sig." << endl << endl;
	
	rslt << "<0+1||E2||2+1> = " << tme_min << "\t-" << tme_err_low[0] << "\t+" << tme_err_upp[0];
	rslt << " (1sig)\t-" << tme_err_low[1] << "\t+" << tme_err_upp[1] << " (2sig)" << endl;
	rslt << "<2+1||E2||2+1> = " << dme_min << "\t-" << dme_err_low[0] << "\t+" << dme_err_upp[0];
	rslt << " (1sig)\t-" << dme_err_low[1] << "\t+" << dme_err_upp[1] << " (2sig)" << endl;
	rslt << "Chisq minimum = " << chisq_min << endl;
	rslt << "Ndata projectile: " << Ndata_proj << endl;
	rslt << "          target: " << Ndata_targ << endl;
	
	if( intiflag == 1 ) cout << "Integration performed at each step\n";
	else cout << "Integration performed with starting parameters only\n\n";

	// Write files and close
	root->cd();
	gChisq->Write("gChisq");
	gChisq_proj->Write("gChisq_proj");
	gChisq_targ->Write("gChisq_targ");
	gChisq_1sigma->Write("gChisq_1sigma");
	gChisq_2sigma->Write("gChisq_2sigma");
	gChisq_1sigma_rotorlim->Write("gChisq_1sigma_rotorlim");
	gChisq_2sigma_rotorlim->Write("gChisq_2sigma_rotorlim");
	hChisq->Write("hChisq");
	hChisq_proj->Write("hChisq_proj");
	hChisq_targ->Write("hChisq_targ");
	hChisq_1sigma->Write("hChisq_1sigma");
	hChisq_2sigma->Write("hChisq_2sigma");
	hChisq_1sigma_rotorlim->Write("hChisq_1sigma_rotorlim");
	hChisq_2sigma_rotorlim->Write("hChisq_2sigma_rotorlim");
	for( int id = 0; id < Nsteps_dme; id++ ) {
		gName = "gChisq_DME_" + convertInt(id);
		gChisqDME[id]->Write(gName.c_str());
	}
	root->Close();
	for ( int k = 0; k < 2; k++ ) out[k]->close();
	rslt.close();
	out.resize(0);   // clean up outfile vector

	cout << "I wrote the data to...\n\tROOT file: " << rootname << endl;
	cout << "\tTEXT files: " << textname[0] << " and " << textname[1] << endl;
	cout << "I wrote the results to " << rsltname << endl;

	return 0;
	
}
