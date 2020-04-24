// The main routine for performing the scans
// Liam Gaffney (liam.gaffney@liverpool.ac.uk) - 23/04/2020

#ifndef __scan_cc__
#define __scan_cc__

#include "scan.hh"

scan::scan(){
	
	//cout << Constructor << endl;

}

scan::~scan(){
	
	//cout << Destructor << endl;
	
}

void scan::SetupScan( string _in_proj, string _intifile,
					  float _tme_index, float _dme_index,
					  float _low_tme, float _upp_tme, int _Nsteps_tme,
					  float _low_dme, float _upp_dme, int _Nsteps_dme,
					  int _Ndata_proj, int _Ndata_targ,
					  int _Nmini, int _Npara,
					  bool _g2, bool _read, rootobjs _ro ) {
	
	// Assign variables for the scan
	in_proj = _in_proj;
	intifile = _intifile;
	tme_index = _tme_index;
	dme_index = _dme_index;
	low_tme = _low_tme;
	upp_tme = _upp_tme;
	low_dme = _low_dme;
	upp_dme = _upp_dme;
	Nsteps_tme = _Nsteps_tme;
	Nsteps_dme = _Nsteps_dme;
	Ndata_proj = _Ndata_proj;
	Ndata_targ = _Ndata_targ;

	stepSize_dme = ( upp_dme - low_dme ) / (float)(Nsteps_dme-1);
	stepSize_tme = ( upp_tme - low_tme ) / (float)(Nsteps_tme-1);
	if( Nsteps_dme == 1 ) stepSize_dme = 0;
	if( Nsteps_tme == 1 ) stepSize_tme = 0;

	Nmini = _Nmini;
	Npara = _Npara;

	g2 = _g2;
	read = _read;
	ro = _ro;
	
	GetAuxFiles();
	OpenOutputFiles();

	return;

}

void scan::GetAuxFiles(){
	
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

	return;
	
}

string scan::getDateTime() {
	
	// get time now
	time_t t = time(0);
	struct tm * now = localtime( & t );

	// make it into a nice string YYYYMMDD-HHMMSS
	string date_string = to_string( now->tm_year + 1900 );
	if ( now->tm_mon < 9 ) date_string += "0";
	date_string += to_string( now->tm_mon + 1 );
	if ( now->tm_mday < 10 ) date_string += "0";
	date_string += to_string( now->tm_mday );
	
	date_string += "_";
	if ( now->tm_hour < 10 ) date_string += "0";
	date_string += to_string( now->tm_hour );
	if ( now->tm_min < 10 ) date_string += "0";
	date_string += to_string( now->tm_min );
	if ( now->tm_sec < 10 ) date_string += "0";
	date_string += to_string( now->tm_sec );
	
	return date_string;

	
}

void scan::OpenOutputFiles(){
	
	// Open output text and root files
	//  if continuing, read output file first
	//  if not, copy old files to ...old
	outname = in_proj.substr( 0, in_proj.find_last_of(".") );
	scandir = "./.scan_" + outname + getDateTime();
	textname.push_back( outname + ".chisq" );
	textname.push_back( outname + ".chisq_" + getDateTime() );
	rsltname = outname + ".rslt";
	rootname = outname + ".root";
	string cmd;
	
	if ( read ) old.open( textname[0].c_str(), ios::in );
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
	ro.OpenRootFile( rootname );

	return;
	
}

void scan::CloseOutputs() {
	
	for ( int k = 0; k < 2; k++ ) out[k]->close();
	rslt.close();
	out.resize(0);   // clean up outfile vector

	cout << "I wrote the data to...\n\tROOT file: " << rootname << endl;
	cout << "\tTEXT files: " << textname[0] << " and " << textname[1] << endl;
	cout << "I wrote the results to " << rsltname << endl;
	
	return;
	
}

int scan::LookUpOldChisq() {

	// Cycle through array and return index that matches matrix elements
	int index = -1;
	
	for ( int i = 0; i < result_vector.size()/5; i++ ) {
	
		if ( TMath::Abs( dme - result_vector[5*i+0] ) < 1E-6 ) {
			if ( TMath::Abs( tme - result_vector[5*i+1] ) < 1E-6 ) {

				index = i; // found it!
				break; // stop looking once we've found it!

			}
		}
			
	}

	return index;

}

void scan::ContinueScan() {
	
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
				
	}
	
	else {
		
		cout << "Cannot open " << textname[0].c_str() << " in order to resume\n";
		read = false;
		
	}
	
	
}

string scan::FindFileName( string in_file, string tape ) {
	
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

double scan::ReadChiSqFromFile( string gosiaoutfile ) {

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
	float chisq_tmp = 999;
 
	getline(g2out,line);
	while ( !g2out.eof() ) {
		
		getline( g2out, line );
		if( line.compare( 0, qry.size(), qry ) == 0 ) {
			
			gosia_chisq.str("");
			gosia_chisq.clear();
			gosia_chisq << line.substr( qry.size(), qry.size()+15 );
			gosia_chisq >> chisq_tmp;
			flag = true;
			
		}
	}
	
	g2out.close();
	
	if( !flag ) cout << "Couldn't find chisq value in " << gosiaoutfile << endl;
	
	return chisq_tmp;

}

int scan::GetChiSq() {
	
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
		
		cout << "\nKilled!\n";
		exit( status );
		
	}
	
	else chisq_proj = ReadChiSqFromFile( out_proj );
	
	return 1;
	
}

int scan::GetChiSq2() {
	
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
		
		cout << "\nKilled!\n";
		exit( status );
		
	}
	
	else {
		
		chisq_proj = ReadChiSqFromFile( out_proj );
		chisq_targ = ReadChiSqFromFile( out_targ );
		
	}
	
	return 1;
	
}

int scan::IntegrateProjectile() {

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
	
	int status = 0;
	
	cmd.append( intifile );
	cmd.append( " > /dev/null 2>&1" );
	
	if( system(NULL) ) status = system( cmd.c_str() );
	else {
		
		cout << "Cannot run system command\n";
		return 0;
		
	}

	// Error handling
	if( status == 512 ) {
		
		cout << "Check that Gosia2 runs correctly\n";
		exit(1);
		
	}
	
	else if( status == 2 ) {
		
		cout << "\nKilled!\n";
		exit( status );
		
	}
	
	return 1;
	
}

int scan::WriteProjectileMatrixElementsToFile() {
	
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

int scan::WriteTargetMatrixElementsToFile() {
	
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

void scan::PrintStep() {
	
	// Print to terminal
	cout << dme << "\t" << tme << "\t";
	if( g2 ) cout << chisq_proj << "\t" << chisq_targ << "\t" << chisq << endl;
	else cout << chisq << endl;

	// Print to file
	for ( int k = 0; k < 2; k++ ) {
		
		(*out[k]) << dme << "\t" << tme << "\t";
		(*out[k]) << chisq_proj << "\t" << chisq_targ << "\t" << chisq << endl;
		
	}

	return;
	
}

void scan::PrintResults() {
		
	// Print to cout
	cout << "\nChisq minimum found at " << ro.GetChisqMin() << endl;
	cout << "X-axis ME = " << ro.GetXme() << "(-" << ro.GetXlow1sig();
	cout << "; +" << ro.GetXupp1sig() << ")1sig." << " (-" << ro.GetXlow2sig();
	cout << "; +" << ro.GetXupp2sig() << ")2sig." << endl << endl;
	cout << "Y-axis ME = " << ro.GetYme() << "(-" << ro.GetYlow1sig();
	cout << "; +" << ro.GetYupp1sig() << ")1sig." << " (-" << ro.GetYlow2sig();
	cout << "; +" << ro.GetYupp2sig() << ")2sig." << endl << endl;

	// Print to file
	rslt << "X-axis ME = " << ro.GetXme() << "(-" << ro.GetXlow1sig();
	rslt << "; +" << ro.GetXupp1sig() << ")1sig." << " (-" << ro.GetXlow2sig();
	rslt << "; +" << ro.GetXupp2sig() << ")2sig." << endl << endl;
	rslt << "Y-axis ME = " << ro.GetYme() << "(-" << ro.GetYlow1sig();
	rslt << "; +" << ro.GetYupp1sig() << ")1sig." << " (-" << ro.GetYlow2sig();
	rslt << "; +" << ro.GetYupp2sig() << ")2sig." << endl << endl;
	rslt << "Chisq minimum = " << ro.GetChisqMin() << endl;
	rslt << "Ndata projectile: " << Ndata_proj << endl;
	rslt << "          target: " << Ndata_targ << endl;
	
	// Check integration step was performed
	if( intiflag == 1 ) cout << "Integration performed at each step\n";
	else cout << "Integration performed with starting parameters only\n\n";

	
	return;
	
}

void scan::do_step() {
	
	// Write matrix elements
	metest = WriteProjectileMatrixElementsToFile();
	if( g2 )
		metest = WriteTargetMatrixElementsToFile();
	
	if( metest == 1 )
		cout << "Couldn't write projectile matrix elements to file\n";
	
	if( metest == 2 )
		cout << "Couldn't write target matrix elements to file\n";
	
	if( metest > 0 ) exit(1);
	
	// Integration step
	intiflag = IntegrateProjectile();
	
	// Run Gosia2 or standard Gosia and return chisq values
	if( g2 )
		minitest = GetChiSq2();
	
	else
		minitest = GetChiSq();
	
	if ( minitest == 0 ) {
		
		cout << "Unable to run gosia\n";
		exit(1);
		
	}
	
	chisq_proj *= Ndata_proj;
	chisq_targ *= Ndata_targ;
	
	if( g2 ) chisq = chisq_proj + chisq_targ;
	else chisq = chisq_proj;

	return;
	
}

void scan::run_scan() {
	
	// If continuing or reading old values, get last calculated values
	if ( read ) ContinueScan();

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
			
				index = LookUpOldChisq();
				if ( index < 0 ) do_calc = true;
				else {
					
					chisq_proj = result_vector.at( 5*index+2 );
					chisq_targ = result_vector.at( 5*index+3 );
					chisq = result_vector.at( 5*index+4 );
					do_calc = false;
					
				}
			
			}
			
			else do_calc = true;
			
			if ( do_calc ) do_step();

			// Print to terminal
			PrintStep();
			
			// Write in root graphs and histograms
			ro.AddChisqPoint( i, j, dme, tme, chisq_proj, chisq_targ );
			
		}

	}
	
	ro.MakeCuts();
	ro.WriteRootFile();
	
	PrintResults();
	CloseOutputs();
	
	return;
	
}



#endif
