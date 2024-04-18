// Making root plots to store the results
// Liam Gaffney (liam.gaffney@liverpool.ac.uk) - 23/04/2020

#ifndef __rootobjs_hh__
#define __rootobjs_hh__

#include "TFile.h"
#include "TH2D.h"
#include "TGraph.h"
#include "TGraph2D.h"

#include <string>
#include <iostream>

class rootobjs {
	
public:
	
	// Constructor/destructor
	rootobjs();
	~rootobjs();

	// Setup and the like
	void SetupRoot( float _low_dme, float _upp_dme, int _Nsteps_dme,
					float _low_tme, float _upp_tme, int _Nsteps_tme );
	void OpenRootFile( std::string filename );
	void WriteRootFile();
	void MakeCuts();

	// Get/Set Functions
	void AddChisqPoint( int i, int j, float dme, float tme,
					    float chisq_proj, float chisq_targ );
	inline float GetChisqMin(){ return chisq_min; };
	inline float GetXme(){ return dme_min; };
	inline float GetYme(){ return tme_min; };
	inline float GetXlow1sig(){ return dme_err_low[0]; };
	inline float GetXlow2sig(){ return dme_err_low[1]; };
	inline float GetXupp1sig(){ return dme_err_upp[0]; };
	inline float GetXupp2sig(){ return dme_err_upp[1]; };
	inline float GetYlow1sig(){ return tme_err_low[0]; };
	inline float GetYlow2sig(){ return tme_err_low[1]; };
	inline float GetYupp1sig(){ return tme_err_upp[0]; };
	inline float GetYupp2sig(){ return tme_err_upp[1]; };

	
private:
	
	// Objects - Graphs
	TGraph2D *gChisq;
	TGraph2D *gChisq_proj;
	TGraph2D *gChisq_targ;
	TGraph2D *gChisq_1sigma;
	TGraph2D *gChisq_2sigma;
	TGraph2D *gChisq_1sigma_rotorlim;
	TGraph2D *gChisq_2sigma_rotorlim;
	std::vector<TGraph*> gChisqDME;
	
	// Objects - Histograms
	TH2D *hChisq;
	TH2D *hChisq_proj;
	TH2D *hChisq_targ;
	TH2D *hChisq_1sigma;
	TH2D *hChisq_2sigma;
	TH2D *hChisq_1sigma_rotorlim;
	TH2D *hChisq_2sigma_rotorlim;

	// Files
	TFile *root;
	
	// Strings and temp variables
	std::string gName;
	std::string rootname;
	
	// Steps, limits, etc
	int Nsteps_dme;
	float low_dme;
	float upp_dme;
	float stepSize_dme;
	int Nsteps_tme;
	float low_tme;
	float upp_tme;
	float stepSize_tme;

	// Minimums and maximums
	float chisq_min;
	float dme_min;
	float tme_min;
	double tme_err_low[2], dme_err_low[2];
	double tme_err_upp[2], dme_err_upp[2];
	

};

#endif
