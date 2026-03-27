#include <TFile.h>
#include <TParameter.h>
#include <TH1D.h>
#include <TSystemDirectory.h>
#include <TSystemFile.h>
#include <TList.h>
#include <TString.h>
#include <iostream>
#include "MendezStyle.h"
#include <fstream>

void SensStudy() {
  TH2::AddDirectory(kFALSE);
  vector<TGraph*> sens_contour;
  double xbins[61], ybins[81];
  double xmin = -2, xmax = 0;
  double ymin = -2, ymax = 2;
  double contour_level = 0.05;
  // Construct log10 bin boundaries
  for (int i = 0; i <= 60; i++) {
    for (int j = 0; j <= 80; j++) {
      xbins[i] = pow(10, xmin + i*(xmax-xmin)/60);  // theta bins
      ybins[j] = pow(10, ymin + j*(ymax-ymin)/80);  // dm2 bins
    }
  }
  int num_universe = 10;
  
  std::vector<TH2D*> clsHeat;
  for (int n=0; n<num_universe; n++) {
    TH2D* h = new TH2D(Form("clsHeat%i",n+1), Form("Universe %i #Delta#chi^{2} Map;sin^{2}(2#theta_{#mu#mu});#Deltam^{2}_{41}",n+1),60, &xbins[0], 80, &ybins[0]);
    clsHeat.push_back(h);
  }
  for (int xindex=0; xindex<=60; xindex++) {
    for (int yindex=0; yindex<=80; yindex++) {
// cout << "Reading " << xindex << " " << yindex << endl;
      // Do this once per grid point
      // TString filepath = TString::Format("202603-vanilla-numu/vanilla-numu_grid_60x80_sinsquare_theta_uu_%i_dm2_%i.root", xindex,yindex);
      TString filepath = TString::Format("202603-vanilla-numu/202603-vanilla-numu-disappear-60x80-grid-sinsquare-twothetauu_%i-deltamsquare41_%i.root.probdist", xindex,yindex);
// 202603-vanilla-numu-disappear-60x80-grid-sinsquare-twothetauu_9-deltamsquare41_79.root.probdist
      // cout << "Reading " << filepath << "\n";
      TFile file(filepath, "READ");
      // auto numtoys = (TParameter<int>*)file.Get("num_toys");
      auto numtoys = (TParameter<int>*)file.Get("pdf_size");
      TVectorD *v3v = nullptr;
      TVectorD *v4v = nullptr;
      file.GetObject("3v_deltachi2", v3v);   // name of the object inside the root file
      file.GetObject("4v_deltachi2", v4v);   // name of the object inside the root file
      int toys = numtoys->GetVal();
      file.Close();
      for (int universe=1; universe<=num_universe; universe++) {
	// Do this once per universe file
	TString deltapath = TString::Format("dir_root/vanilla-numu_grid_60x80_chi2obs_toyuniverse_%i.root", universe);
	// cout << "Reading " << deltapath << "\n";
	TFile deltafile(deltapath, "READ");
	TMatrixD* matrix = nullptr;
	deltafile.GetObject("obs_map", matrix);
	deltafile.Close();
	double ref = (*matrix)(xindex,yindex);
	double count3v = 0;
	double count4v = 0;
	for (int i = 0; i < toys; i++) {
	  if((*v3v)[i] >= ref) {
	    count3v++;
	  }
	  if((*v4v)[i] >= ref) {
	    count4v++;
	  }
	}
	double z = 0;
	if( count3v == 0 ) { 
	  if( count4v == 0 ) z = 0;
	  else z = 1;
	}   
	else z = count4v / count3v;
	if( count4v>=count3v ) z = 1;
	// clsHeat[universe-1]->SetBinContent(xindex,yindex,ref);
	clsHeat[universe-1]->SetBinContent(xindex,yindex,z);
	delete matrix;
      }
      delete numtoys;
      delete v3v;
      delete v4v;
    }
cout << "Finished x bin " << xindex <<endl;
  }
  TFile outfile("202603-cls_map-setxiangpanscale.root", "RECREATE");
  for(auto h : clsHeat){
   h->SetMinimum(0.6);
   h->SetMaximum(1.); 
    h->Write();
    delete h;
  }
  outfile.Close();
}
