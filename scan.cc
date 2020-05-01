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
					  float _dme_index, float _tme_index,
					  float _low_dme, float _upp_dme, int _Nsteps_dme,
					  float _low_tme, float _upp_tme, int _Nsteps_tme,
					  int _Ndata_proj, int _Ndata_targ,
					  int _Nmini, int _Npara,
					  bool _g2, bool _readflag, rootobjs _ro ) {
	
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
	readflag = _readflag;
	ro = _ro;
	
	tstamp = getDateTime();
	
	if( intifile == "empty" ) intiflag = false;
	else intiflag = true;

	MakeScanDirectories();
	GetAuxFiles();
	OpenOutputFiles();

	return;

}

void scan::MakeScanDirectories() {
	
	// Return current working directory
	char buff[PATH_MAX];
	getcwd( buff, PATH_MAX );
	maindir = buff;

	string dirname_tmp;
	cout << "Running in " << maindir << endl;

	scanname = "scan_" + tstamp;
	cout << "Scan outputs in " << scanname << endl;
	mkdir( scanname.data(), 0755 );

	for( int i = 0; i < Npara; ++i ){
		
		dirname_tmp = scanname + "/" + to_string(i);
		mkdir( dirname_tmp.data(), 0755 );
		scandir.push_back( dirname_tmp );

	}
	
	return;
	
}

void scan::CleanDirectories() {
	
	//cout << "Scan directories have not been deleted" << endl;

	//for( unsigned int i = 0; i < scandir.size(); ++i ){
	//
	//	cout << "rm -rf " << scandir[i] << endl;
	//
	//}
	
	return;
	
}

void scan::GetAuxFiles(){
	
	string bstlit;
	
	// Copy INTI and MINI files
	CopyFileForScan( in_proj );
	if( intifile != "empty" ) CopyFileForScan( intifile );
	
	// Detector definition files
	raw_proj = FindFileName( in_proj, "8" );
	gdt_proj = FindFileName( in_proj, "9" );
	
	if( raw_proj == "empty" || gdt_proj == "empty" ) exit(1);
	else {
	
		CopyFileForScan( raw_proj );
		CopyFileForScan( gdt_proj );
		
	}
	
	// Projectile yield, map and matrix element files
	yld_proj = FindFileName( in_proj, "3" );
	cor_proj = FindFileName( in_proj, "4" );
	map_proj = FindFileName( in_proj, "7" );
	bst_proj = FindFileName( in_proj, "12" );

	if( yld_proj == "empty" || cor_proj == "empty" ||
	    map_proj == "empty" || bst_proj == "empty" ) exit(1);
	
	else {
		
		CopyFileForScan( yld_proj );
		CopyFileForScan( cor_proj );
		CopyFileForScan( map_proj );
		CopyFileForScan( bst_proj );
		bstlit = bst_proj + ".lit";
		CopyFileForScan( bstlit );

	}


	// Find corresponsing target file for Gosia2
	if( g2 ){
		
		in_targ = FindFileName( in_proj, "26" );
	
		if( in_targ == "empty" ) {
			
			cout << "Check your input files if you want to use Gosia2\n";
			exit(1);
			
		}
		
		else {
			
			out_targ = FindFileName( in_targ, "22" );
			yld_targ = FindFileName( in_targ, "3" );
			cor_targ = FindFileName( in_targ, "4" );
			map_targ = FindFileName( in_targ, "27" );
			bst_targ = FindFileName( in_targ, "32" );
			
			if( out_targ == "empty" || bst_targ == "empty" ||
			    yld_targ == "empty" || cor_targ == "empty" ||
				map_targ == "empty" ) exit(1);
			
			else {
				
				CopyFileForScan( yld_targ );
				CopyFileForScan( cor_targ );
				CopyFileForScan( map_targ );
				CopyFileForScan( bst_targ );
				bstlit = bst_targ + ".lit";
				CopyFileForScan( bstlit );

			}

		}
		
		CopyFileForScan( in_targ );

	}

	// Find corresponsing output files and matrix element files
	out_proj = FindFileName( in_proj, "22" );
	bst_proj = FindFileName( in_proj, "12" );
	
	if( out_proj == "empty" || out_targ == "empty" ||
	   bst_proj == "empty" || bst_targ == "empty" ) exit(1);

	return;
	
}

void scan::CopyFileForScan( string filename ){
	
	string newfilename = scanname + "/" + filename;
	
	ifstream source( filename );
	ofstream dest( newfilename );
	
	dest << source.rdbuf();
	dest.close();
	
	for( unsigned int i = 0; i < scandir.size(); ++i ){
		
		source.clear();
		source.seekg(0);
	
		newfilename = scandir[i] + "/" + filename;
		dest.open( newfilename );
		dest << source.rdbuf();
		dest.close();
	
	}

	source.close();
	
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

void scan::RunCmd( std::string cmd ){
	
	int status = 0;
	
	if( system(NULL) ) status = system( cmd.data() );
	else {
		
		cerr << "Cannot run system command:" << endl;
		cerr << "\t" << cmd << endl;
		exit( status );

	}
	
	// Error handling
	if( status == 512 ) {
		
		cerr << "Check that this command runs correctly:";
		cerr << "\t" << cmd << endl;
		exit( status );

	}
	
	else if( status == 2 ) {
		
		cerr << "\nKilled!\n";
		exit( status );
		
	}

	return;
	
}

void scan::OpenOutputFiles(){
	
	// Open output text and root files
	//  if continuing, read output file first
	//  if not, copy old files to ...old
	outname = in_proj.substr( 0, in_proj.find_last_of(".") );
	textname.push_back( outname + ".chisq" );
	textname.push_back( scanname + "/" + outname + ".chisq" );
	rsltname = scanname + "/" + outname + ".rslt";
	rootname = scanname + "/" + outname + ".root";
	string cmd;
	
	if( readflag ){
		
		old.open( textname[0].data(), ios::in );
		
		if( !old.is_open() ) {
			
			cout << "Couldn't open " << textname[0] << endl;
			cout << "Cannot read previous results, starting fresh " << endl;
			readflag = false;
			
		}
	
	}
	
	if( !readflag ) {
		
		outa.open( textname[0].data(), ios::out );
		outb.open( textname[1].data(), ios::out );
		out.push_back( &outa );
		out.push_back( &outb );
		
	}

	rslt.open( rsltname.data(), ios::out );
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

int scan::LookUpOldChisq( float dme, float tme ) {

	// Cycle through array and return index that matches matrix elements
	int index = -1;
	
	for ( unsigned int i = 0; i < result_vector.size()/5; i++ ) {
	
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
	
	float dme_prv, tme_prv;
	float chisq, chisq_proj, chisq_targ;
	
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
		outa.open( textname[0].data(), ios::out );
		outb.open( textname[1].data(), ios::out );
		out.push_back( &outa );
		out.push_back( &outb );
				
	}
	
	else {
		
		cout << "Cannot open " << textname[0].data() << " in order to resume\n";
		readflag = false;
		
	}
	
	
}

string scan::FindFileName( string in_file, string tape ) {
	
	// Open gosia input file for projectile and find the output file name
	ifstream gin;
	gin.open( in_file.data(), ios::in );
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

void scan::ReadChiSqFromFile( string gosiaoutfile, float &chisq ) {

	// Open gosia output file for projectile or target. Get from input
	ifstream g2out;
	g2out.open( gosiaoutfile.data(), ios::in );
	if( !g2out.is_open() ) {
	
		cout << "Unable to open " << gosiaoutfile << endl;
		exit(1);

	}
	
	// Search for chisq value in file
	bool flag = false;
	string line, tmp;
	string qry = "     *** CHISQ=";
	stringstream gosia_chisq (stringstream::in | stringstream::out);
	float chisq_tmp;
 
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
	
	if( !flag ) {
		
		cout << "Couldn't find chisq value in " << gosiaoutfile << endl;
		exit(1);
		
	}
	
	else chisq = chisq_tmp;
	
	return;

}

void scan::GetChiSq( string dirname, float &chisq_proj, float &chisq_targ ) {
	
	string cmd = "cd " + dirname + " && ";
	if( g2 ) cmd.append( "gosia2 < " );
	else cmd.append( "gosia < " );
	cmd.append( in_proj );
	cmd.append( " > /dev/null 2>&1" );
	
	string outfile_p = dirname + "/" + out_proj;
	string outfile_t = dirname + "/" + out_targ;
	
	// Run gosia Nmini times with system command
	for( int i = 0; i < Nmini; i++ ) {
		
		std::thread minithread( &scan::RunCmd, this, cmd );
		minithread.join();
		
	}
	
	ReadChiSqFromFile( outfile_p, std::ref(chisq_proj) );
	if( g2 ) ReadChiSqFromFile( outfile_t, std::ref(chisq_targ) );
	
	return;
	
}

void scan::IntegrateProjectile( string dirname ) {

	string line;
	string cmd = "cd " + dirname + " && ";
	
	ifstream inti;
	string infile = dirname + "/" + intifile;
	inti.open( infile.data(), ios::in );
	if( !inti.is_open() ) {
		
		cerr << "Cannot open " << infile << endl;
		exit(2);
		
	}

	else {
		
		// Check first line of integral file
		getline( inti, line );

		// If it looks like a gosia2 input, use that, else just normal gosia
		if( line == "1" || line == "2" ) cmd.append( "gosia2 < " );
		else cmd.append( "gosia < " );
		
		inti.close();
		
	}
	
	cmd.append( intifile );
	cmd.append( " > /dev/null 2>&1" );
	
	std::thread intithread( &scan::RunCmd, this, cmd );
	intithread.join();
	
	return;
	
}

void scan::WriteProjectileMatrixElementsToFile( string dirname, float dme, float tme ) {
	
	string bstname;
	string litname;
	ofstream mefile;
	ifstream litfile;
	
	float tmp;
	int index = 1;

	// Projectile matrix elements
	bstname = dirname + "/" + bst_proj;
	litname = bstname + ".lit";
	
	mefile.open( bstname.data(), ios::out );
	litfile.open( litname.data(), ios::in );
	if( !mefile.is_open() ) {
		
		cerr << "Cannot open " << bstname << endl;
		exit(2);

	}
	
	if( !litfile.is_open() ) {
		   
		cerr << "Cannot open " << litname << endl;
		exit(2);

	}

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
	
	return;
	
}

void scan::WriteTargetMatrixElementsToFile( string dirname, float dme, float tme ) {
	
	string bstname;
	string litname;
	ofstream mefile;
	ifstream litfile;


	// Target matrix elements, copy from backup file
	bstname = dirname + "/" + bst_targ;
	litname = bstname + ".lit";
	
	litfile.open( litname.data(), ios::in );
	if( !litfile.is_open() ) {
		   
		cerr << "Cannot open " << litname << endl;
		exit(2);

	}

	else litfile.close();
	
	RunCmd( "cp " + litname + " " + bstname );

	return;
	
}

void scan::PrintResults() {
		
	// Print to cout
	cout << "\nChisq minimum found at " << ro.GetChisqMin() << endl;
	cout << "X-axis ME = " << ro.GetXme() << "(-" << ro.GetXlow1sig();
	cout << "; +" << ro.GetXupp1sig() << ")1sig." << " (-" << ro.GetXlow2sig();
	cout << "; +" << ro.GetXupp2sig() << ")2sig." << endl;
	cout << "Y-axis ME = " << ro.GetYme() << "(-" << ro.GetYlow1sig();
	cout << "; +" << ro.GetYupp1sig() << ")1sig." << " (-" << ro.GetYlow2sig();
	cout << "; +" << ro.GetYupp2sig() << ")2sig." << endl << endl;

	// Print to file
	rslt << "X-axis ME = " << ro.GetXme() << "(-" << ro.GetXlow1sig();
	rslt << "; +" << ro.GetXupp1sig() << ")1sig." << " (-" << ro.GetXlow2sig();
	rslt << "; +" << ro.GetXupp2sig() << ")2sig." << endl;
	rslt << "Y-axis ME = " << ro.GetYme() << "(-" << ro.GetYlow1sig();
	rslt << "; +" << ro.GetYupp1sig() << ")1sig." << " (-" << ro.GetYlow2sig();
	rslt << "; +" << ro.GetYupp2sig() << ")2sig." << endl << endl;
	rslt << "Chisq minimum = " << ro.GetChisqMin() << endl;
	rslt << "Ndata projectile: " << Ndata_proj << endl;
	rslt << "          target: " << Ndata_targ << endl;
	
	// Check integration step was performed
	if( intiflag && !no_calc ) cout << "Integration performed at each step\n";
	else if( no_calc ) cout << "No calculations needed, results taken from chisq file\n\n";
	else cout << "Integration performed with starting parameters only\n\n";

	
	return;
	
}

void scan::PrintStep( float dme, float tme,
					  float chisq_proj, float chisq_targ ) {
	
	float chisq = chisq_proj;
	if( g2 ) chisq += chisq_targ;

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

void scan::PrintHeader() {
	
	// Get chisq values and write to file
	cout << "\n\t\t  Chi-squared value\n";
	if( g2 ) cout << "  x\t  y\tProj\tTarg\tTotal\n";
	else cout << "  x\t  y\tTotal\n";

	return;
	
}

void scan::do_step( string dirname, int i, int j, float dme, float tme ) {
	
	// Variables
	float chisq_proj, chisq_targ = 0.0;

	// Write matrix elements
	WriteProjectileMatrixElementsToFile( dirname, dme, tme );
	if( g2 ) WriteTargetMatrixElementsToFile( dirname, dme, tme );

	// Integration step
	if( intiflag ) IntegrateProjectile( dirname );
	
	// Run Gosia2 or standard Gosia and return chisq values
	GetChiSq( dirname, std::ref(chisq_proj), std::ref(chisq_targ) );

	chisq_proj *= Ndata_proj;
	chisq_targ *= Ndata_targ;

	{
		// Lock the printing and ROOT access
		std::unique_lock<std::mutex> lock(mlock);
		
		PrintStep( dme, tme, chisq_proj, chisq_targ );
		ro.AddChisqPoint( i, j, dme, tme, chisq_proj, chisq_targ );
		
		// Update the flags
		no_calc = false;

	}
	
	return;
	
}

void scan::run_scan() {
	
	float dme, tme;
	float chisq, chisq_proj, chisq_targ;
	
	// If continuing or reading old values, get last calculated values
	if ( readflag ) {
		ContinueScan();
		PrintHeader();
	}
	
	// Check if any calculations were performed
	no_calc = true;

	// Loop over matrix elements and check if we need to calculate chisq
	for ( int i=0; i<Nsteps_dme; ++i ) {
	
		dme = low_dme + i*stepSize_dme;

		for ( int j=0; j<Nsteps_tme; ++j ) {
		
			tme = low_tme + j*stepSize_tme;
			
			if ( readflag ) {
			
				index = LookUpOldChisq( dme, tme );
				if ( index < 0 ) do_calc = true;
				else {
					
					chisq_proj = result_vector.at( 5*index+2 );
					chisq_targ = result_vector.at( 5*index+3 );
					chisq = result_vector.at( 5*index+4 );
					do_calc = false;
					
				}
			
			}
			
			else do_calc = true;
			
			if ( do_calc ) {
								
				i_todo.push_back( i );
				j_todo.push_back( j );
				dme_todo.push_back( dme );
				tme_todo.push_back( tme );
				
			}
			
			else {
				
				// Write to terminal and file
				PrintStep( dme, tme, chisq_proj, chisq_targ );

				// Write in root graphs and histograms
				ro.AddChisqPoint( i, j, dme, tme, chisq_proj, chisq_targ );

			}

		}

	}
	
	// How many calculations do we need to do?
	int todo, each, left = 0;
	todo = i_todo.size();
	each = todo / Npara;
	if( todo > 0 ) left = todo % ( Npara * each );
	
	cout << endl << "Total calculations to do   = " << todo << endl;
	cout << "Number of parallel threads = " << Npara << endl;
	cout << "Calculations per thread    = " << each << endl;
	cout << "Left over calculations     = " << left << endl << endl;
	
	if( todo > 0 ) PrintHeader();

	vector<std::thread> op;
	for( int k = 0; k < each; ++k ){
				
		for( int l = 0; l < Npara; ++l ){
			
			int jobN = k + l*each;
			
			std::thread calc( &scan::do_step,
							 this, scandir[l],
							 i_todo[jobN], j_todo[jobN],
							 dme_todo[jobN], tme_todo[jobN] );
			
			// Move to CPU of choice
			cpu_set_t cpuset;
			CPU_ZERO(&cpuset);
			CPU_SET( l+1, &cpuset );
			int rc = pthread_setaffinity_np(
				calc.native_handle(), sizeof(cpu_set_t), &cpuset );
			if( rc != 0 )
				std::cerr << "Error calling pthread_setaffinity_np: " << rc << "\n";

			// Move in to the vector
			op.push_back( std::move( calc ) );

		}
				
		// Join the threads
		for( unsigned int m = 0; m < op.size(); ++m ) {
			
			if( op[m].joinable() ) op[m].join();
			//else op.erase( op.begin()+m );
			
		}

	}
	
	// Just linear for whatever is left over
	for( int k = todo-left; k < todo; ++k ) {
		
		do_step( scandir[0], i_todo[k], j_todo[k], dme_todo[k], tme_todo[k] );
		
	}

	ro.MakeCuts();
	ro.WriteRootFile();
	
	PrintResults();
	CloseOutputs();
	CleanDirectories();
	
	return;
	
}



#endif