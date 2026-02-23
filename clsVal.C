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

std::vector<double> FindXAtValue(const TH1* h, double target) {
  std::vector<double> xs;

  const int nb = h->GetNbinsX();

  for (int i = 1; i < nb; ++i) {
    double y1 = h->GetBinContent(i);
    double y2 = h->GetBinContent(i + 1);

    // Check if target is bracketed
    if ((y1 - target) * (y2 - target) > 0)
      continue;  // same side, no crossing

    if (y1 == y2)
      continue;  // flat segment → ambiguous

    double x1 = h->GetBinCenter(i);
    double x2 = h->GetBinCenter(i + 1);

    double x = x1 + (target - y1) * (x2 - x1) / (y2 - y1);
    xs.push_back(x);
  }

  return xs;
}



// save a lot of useless repetitive typing
TLegend * MakeLegend(float left=0.7, float bottom=0.5, float right=0.9, float top=0.85) {
  auto leg = new TLegend(left, bottom, right, top);
  leg->SetFillStyle(0);  // unfortunately can't set this in TStyle :(

  return leg;
}

void clsVal(const char* dirname = "./") {
  // Construct log10 bin boundaries
  double xbins[61], ybins[81];
  double xmin = -2, xmax = 0;
  double ymin = -2, ymax = 2;
  for (int i = 0; i <= 60; i++) {
    for (int j = 0; j <= 80; j++) {
      xbins[i] = pow(10, xmin + i*(xmax-xmin)/60);  // theta bins
      ybins[j] = pow(10, ymin + j*(ymax-ymin)/80);  // dm2 bins
    }
  }
  cout << "Pulling info from files \n";
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
  std::vector<TVectorD> vecd3v; //
  std::vector<TVectorD> vecd4v; //
  std::vector<TVectorD> vecobs; //
  double target = 0.05; // Target CLs value
  while ((file = (TSystemFile*)next())) {
    fname = file->GetName();
    // Only process .root files
    if (!file->IsDirectory() && fname.EndsWith(".root")) {
      TString rootpath = TString::Format("%s/%s", dirname, fname.Data());
      TString deltapath = rootpath + ".deltachi2";
      TString exclpath = rootpath + ".data";
      TFile r(rootpath, "READ");
      TFile d(deltapath, "READ");
      TFile e(exclpath, "READ");
      if (!r.IsOpen()) {
        std::cerr << "Cannot open file " << rootpath << std::endl;
        continue;
      }
      if (!d.IsOpen()) {
        std::cerr << "Cannot open obs sens file " << deltapath << std::endl;
        continue;
      }
      if (!e.IsOpen()) {
        std::cerr << "Cannot open obs data exclusion file " << deltapath << std::endl;
        continue;
      }

      // Retrieve parameter objects
      auto dm2 = (TParameter<double>*)r.Get("dm2_grid");
      auto theta = (TParameter<double>*)r.Get("theta_grid");
      auto numdeltachi2 = (TParameter<int>*)r.Get("num_toys");
      auto numobs = (TParameter<int>*)d.Get("num_toys");
      auto ref = (TParameter<double>*)e.Get("deltachi2_ref");
      auto numtoys = (TParameter<int>*)r.Get("num_toys");
      TVectorD *obs = nullptr;
      TVectorD *v3v = nullptr;
      TVectorD *v4v = nullptr;
      r.GetObject("4v_deltachi2", v4v);   // 4v tests vector
      r.GetObject("3v_deltachi2", v3v);   // 3v tests vector
      d.GetObject("deltachi2_obs", obs);   // sensitivity simulation vector
      int toys = numtoys->GetVal();
      double exclusion = ref->GetVal();
      double x = theta->GetVal();
      double y = dm2->GetVal();
      // Create a temporary array
      double valcls[1000];

      for(size_t obsindex = 0; obsindex<1000; obsindex++) {
	double ref = (*obs)[obsindex];
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
	    valcls[obsindex] = z;
      }
      r.Close();
      d.Close();
      TString outname = fname + ".cls";
      TFile *rootfile = new TFile(outname, "recreate");
      TVectorD arrcls(1000, valcls);
      // Create objects for cpp variables
      TParameter<int> grid_dm2("dm2", y);
      TParameter<int> grid_theta("theta", x);
      grid_dm2.Write();
      grid_theta.Write();
      arrcls.Write("valcls");
      rootfile->Write();
      rootfile->Close();
      delete rootfile;
    }
  }
}
