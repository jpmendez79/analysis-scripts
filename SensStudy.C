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

void SensStudy(const char* dirname = "./") {

  double xbins[61], ybins[81];

  double xmin = -2, xmax = 0;
  double ymin = -2, ymax = 2;

  // Construct log10 bin boundaries
  for (int i = 0; i <= 60; i++) {
    for (int j = 0; j <= 80; j++) {
      xbins[i] = pow(10, xmin + i*(xmax-xmin)/60);  // theta bins
      ybins[j] = pow(10, ymin + j*(ymax-ymin)/80);  // dm2 bins
    }
  }
 
  // Create a 2D histogram
  TH2D* h2 = new TH2D("h2", "Parameter Map;X;Y", 
                      // 60, -4, 0,   // X bins
                      // 80, -2, 2);  // Y bins
		      60, &xbins[0],
                      80, &ybins[0]);
  // 60, -4, 0,   // X bins
  // 80, -2, 2);  // Y bins

  
  double value = 0.05;
  // 2D grid where each element is a vector
  // Access the directory

  int num_universe = 1;
  for (int n = 1; n<=num_universe; n++) {
    TString deltapath = TString::Format("dir_root/vanilla-numu_grid_60x80_chi2obs_universe_%i.root", n);
    TFile deltafile(deltapath, "READ");
    TMatrixD* matrix = nullptr;
    deltafile.GetObject("obs_map", matrix);
    // Access the directory
    TSystemDirectory dir(dirname, dirname);
    TList* files = dir.GetListOfFiles();
    if (!files) {
      std::cerr << "No files found in directory!" << std::endl;
      return;
    }

    // Loop over all files in the directory
    TIter next(files);
    TSystemFile* file;
    TString fname;
    while ((file = (TSystemFile*)next())) {
      fname = file->GetName();
      // Only process .root files
      if (!file->IsDirectory() && fname.EndsWith(".root")) {
        TString fullpath = TString::Format("%s/%s", dirname, fname.Data());
        std::cout << "Processing " << fullpath << std::endl;
        TFile f(fullpath, "READ");
        if (!f.IsOpen()) {
          std::cerr << "Cannot open file " << fullpath << std::endl;
          continue;
        }
        // Retrieve parameter objects
        auto dm2 = (TParameter<double>*)f.Get("dm2_grid");
        auto theta = (TParameter<double>*)f.Get("theta_grid");
	auto numtoys = (TParameter<int>*)f.Get("num_toys");
	TVectorD *v3v = nullptr;
	TVectorD *v4v = nullptr;
	f.GetObject("3v_deltachi2", v3v);   // name of the object inside the root file
	f.GetObject("4v_deltachi2", v4v);   // name of the object inside the root file
	// d.GetObject("deltachi2_obs", obs);   // name of the object inside the root file
        double x = theta->GetVal();
        double y = dm2->GetVal();
	double ref = (*matrix)(x,y);
	// double ref = (*obs)[0];
	// cout << ref << "\n";
	// double ref = 0.05;
	int toys = numtoys->GetVal();
	// TParameter<int>  num_toys;1
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

	  
        h2->SetBinContent(x, y, z);
	// if (z==0) cout << "zero," << x <<"," << y <<endl;
	// if (z==1) cout << "inf," << x <<"," << y <<endl;
        f.Close();
      }
    }
    // TGraph *nature = new TGraph("", "%lg,%lg");

    //     nature->SetLineColor(kBlue);
    //     nature->SetLineWidth(3);
    // Get contour line at z <= 0.05 --> level = 0.05
    double contour_level = 0.05;
    auto contours = mendezstyle::GetContourGraphs(h2, contour_level);
    // auto nature_contours = mendezstyle::GetContourGraphs(nature, contour_level);
    // Draw contour line
    // Draw result
    TCanvas* c = new TCanvas("c", "Results", 900, 800);

    // nature->Draw("COLZ");
    c->SetLogx();
    c->SetLogy();
    mendezstyle::WesZPalette(255);
    mendezstyle::CenterTitles(h2);
    h2->Draw("axis");   // show bin text labels
    // h2->Draw("colz");
    // h2->SaveAs("ue4-chisquare.root");
    // h2->SetTitle("#nu_{#mu} Disappearance Only;2|U_{e4}|^{2};#Delta m^{2}");
    // g->Draw("P Same");

    // for (auto gr : contours) {
    //   gr->SetLineColor(kBlue);
    //   gr->SetLineStyle(1);
    //   gr->SetLineWidth(3);
    //   gr->Draw("L SAME");
    // }
    for (auto gr : contours) {
            contours[1]->SetLineColor(kBlue);
            contours[1]->SetLineWidth(3);
            contours[0]->Draw("L SAME");
    contours[0]->SaveAs("../mendez-vanilla-numu-data-exc-psuedofix.root");
    }
  }
}


