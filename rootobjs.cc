// Making root plots to store the results
// Liam Gaffney (liam.gaffney@liverpool.ac.uk) - 23/04/2020

#ifndef __rootobjs_cc__
#define __rootobjs_cc__

#include "rootobjs.hh"

rootobjs::rootobjs(){
	
	//cout << "Hello " <<  __PRETTY_FUNCTION__ << endl;
	
}

rootobjs::~rootobjs(){
	
	//cout << "Hello " <<  __PRETTY_FUNCTION__ << endl;

}

void rootobjs::SetupRoot( float _low_dme, float _upp_dme, int _Nsteps_dme,
						  float _low_tme, float _upp_tme, int _Nsteps_tme ) {
	
	low_dme = _low_dme;
	low_tme = _low_tme;
	upp_dme = _upp_dme;
	upp_tme = _upp_tme;
	Nsteps_dme = _Nsteps_dme;
	Nsteps_tme = _Nsteps_tme;

	// Float calculate step size
	stepSize_dme = ( upp_dme - low_dme ) / (float)(Nsteps_dme-1);
	stepSize_tme = ( upp_tme - low_tme ) / (float)(Nsteps_tme-1);

	// A 2-dimensional chisq graph in root plus 1sigma cut
	gChisq = new TGraph2D();
	gChisq_proj = new TGraph2D();
	gChisq_targ = new TGraph2D();
	gChisq_1sigma = new TGraph2D();
	gChisq_2sigma = new TGraph2D();
	gChisq_1sigma_rotorlim = new TGraph2D();
	gChisq_2sigma_rotorlim = new TGraph2D();

	gChisq->SetName("gChisq");
	gChisq->SetTitle("#chi^{2} surface plot (N_{p}#chi^{2}_{p} + N_{t}#chi^{2}_{t});<2^{+}||E2||2^{+}> [eb];<0^{+}||E2||2^{+}> [eb];#chi^{2}");

	gChisq_proj->SetName("gChisq_proj");
	gChisq_proj->SetTitle("#chi^{2} surface plot for projectile;<2^{+}||E2||2^{+}> [eb];<0^{+}||E2||2^{+}> [eb];#chi^{2}");

	gChisq_targ->SetName("gChisq_targ");
	gChisq_targ->SetTitle("#chi^{2} surface plot for target;<2^{+}||E2||2^{+}> [eb];<0^{+}||E2||2^{+}> [eb];#chi^{2}");

	gChisq_1sigma->SetName("gChisq_1sigma");
	gChisq_1sigma->SetTitle("#chi^{2}+1 cut;<2^{+}||E2||2^{+}> [eb];<0^{+}||E2||2^{+}> [eb];#chi^{2}");

	gChisq_2sigma->SetName("gChisq_2sigma");
	gChisq_2sigma->SetTitle("#chi^{2}+2 cut;<2^{+}||E2||2^{+}> [eb];<0^{+}||E2||2^{+}> [eb];#chi^{2}");

	gChisq_1sigma_rotorlim->SetName("gChisq_1sigma_rotorlim");
	gChisq_1sigma_rotorlim->SetTitle("#chi^{2}+1 cut plus rigid rotor limits;<2^{+}||E2||2^{+}> [eb];<0^{+}||E2||2^{+}> [eb];#chi^{2}");

	gChisq_2sigma_rotorlim->SetName("gChisq_2sigma_rotorlim");
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
	for( int id = 0; id < Nsteps_dme; id++ ){
		dme_str = to_string( (float)(low_dme + id*stepSize_dme) );
		gName = "gChisq_" + to_string(id);
		gTitle = "#chi^{2} surface plot for DME = " + dme_str;
		gTitle += " eb;<0^{+}||E2||2^{+}> [eb];#chi^{2}";
		gChisqDME.push_back( new TGraph(Nsteps_tme) );
		gChisqDME[id]->SetTitle(gTitle.c_str());
	}
	
	// 2-D histograms for projectile and target
	hChisq = new TH2D("hChisq",
		"#chi^{2} surface plot (N_{p}#chi^{2}_{p} + N_{t}#chi^{2}_{t});<2^{+}||E2||2^{+}> [eb];<0^{+}||E2||2^{+}> [eb];#chi^{2}",
		Nsteps_dme, low_dme-0.5*stepSize_dme, upp_dme+0.5*stepSize_dme,
		Nsteps_tme, low_tme-0.5*stepSize_tme, upp_tme+0.5*stepSize_tme);
	hChisq_proj = new TH2D("hChisq_proj",
		"#chi^{2} surface plot for projectile N_{p}#chi^{2}_{p};<2^{+}||E2||2^{+}> [eb];<0^{+}||E2||2^{+}> [eb];#chi^{2}",
		Nsteps_dme, low_dme-0.5*stepSize_dme, upp_dme+0.5*stepSize_dme,
		Nsteps_tme, low_tme-0.5*stepSize_tme, upp_tme+0.5*stepSize_tme);
	hChisq_targ = new TH2D("hChisq_targ",
		"#chi^{2} surface plot for target N_{t}#chi^{2}_{t};<2^{+}||E2||2^{+}> [eb];<0^{+}||E2||2^{+}> [eb];#chi^{2}",
		Nsteps_dme, low_dme-0.5*stepSize_dme, upp_dme+0.5*stepSize_dme,
		Nsteps_tme, low_tme-0.5*stepSize_tme, upp_tme+0.5*stepSize_tme);
	hChisq_1sigma = new TH2D("hChisq_1sigma",
		"#chi^{2}+1 cut;<2^{+}||E2||2^{+}> [eb];<0^{+}||E2||2^{+}> [eb];#chi^{2}",
		Nsteps_dme, low_dme-0.5*stepSize_dme, upp_dme+0.5*stepSize_dme,
		Nsteps_tme, low_tme-0.5*stepSize_tme, upp_tme+0.5*stepSize_tme);
	hChisq_1sigma_rotorlim = new TH2D("hChisq_1sigma_rotorlim",
		"#chi^{2}+1 cut plus rigid rotor limits;<2^{+}||E2||2^{+}> [eb];<0^{+}||E2||2^{+}> [eb];#chi^{2}",
		Nsteps_dme, low_dme-0.5*stepSize_dme, upp_dme+0.5*stepSize_dme,
		Nsteps_tme, low_tme-0.5*stepSize_tme, upp_tme+0.5*stepSize_tme);
	hChisq_2sigma = new TH2D("hChisq_2sigma",
		"#chi^{2}+2 cut;<2^{+}||E2||2^{+}> [eb];<0^{+}||E2||2^{+}> [eb];#chi^{2}",
		Nsteps_dme, low_dme-0.5*stepSize_dme, upp_dme+0.5*stepSize_dme,
		Nsteps_tme, low_tme-0.5*stepSize_tme, upp_tme+0.5*stepSize_tme);
	hChisq_2sigma_rotorlim = new TH2D("hChisq_2sigma_rotorlim",
		"#chi^{2}+2 cut plus rigid rotor limits;<2^{+}||E2||2^{+}> [eb];<0^{+}||E2||2^{+}> [eb];#chi^{2}",
		Nsteps_dme, low_dme-0.5*stepSize_dme, upp_dme+0.5*stepSize_dme,
		Nsteps_tme, low_tme-0.5*stepSize_tme, upp_tme+0.5*stepSize_tme);

	//cout << "Made ROOT objects" << endl;
	
	return;
	
}

void rootobjs::OpenRootFile( string filename ) {
	
	// Open ROOT file
	rootname = filename;
	root = new TFile( rootname.c_str(), "RECREATE" );

	//cout << "Opened ROOT file" << endl;

	return;
	
}

void rootobjs::WriteRootFile() {
	
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
		gName = "gChisq_DME_" + to_string(id);
		gChisqDME[id]->Write(gName.c_str());
	}
	
	root->Close();

	return;
	
}

void rootobjs::MakeCuts() {

	// Make 1sigma and 2sigma cuts (chisq_min + 1 and 2) and write values
	chisq_min = gChisq->GetZmin();
	int ix, iy, iz;

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
	dme_min = hChisq->GetXaxis()->GetBinCenter(ix);
	tme_min = hChisq->GetYaxis()->GetBinCenter(iy);


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

	return;
	
}

void rootobjs::AddChisqPoint( int i, int j, float dme, float tme,
							  float chisq_proj, float chisq_targ ){
	
	
	float chisq = chisq_proj+chisq_targ;
	
	gChisq->SetPoint( i*Nsteps_tme + j, dme, tme, chisq );
	hChisq->SetBinContent( i+1, j+1, chisq );
	gChisq_proj->SetPoint( i*Nsteps_tme + j, dme, tme, chisq_proj );
	gChisq_targ->SetPoint( i*Nsteps_tme + j, dme, tme, chisq_targ );
	hChisq_proj->SetBinContent( i+1, j+1, chisq_proj );
	hChisq_targ->SetBinContent( i+1, j+1, chisq_targ );
	gChisqDME[i]->SetPoint( j, tme, chisq );
	
	return;
	
}

#endif
