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

void brazilScript(const char* dirname = "./") {
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
  std::vector<TVectorD> vecd3v; // vector of 4800 deltachi2_3v vectors
  std::vector<TVectorD> vecd4v; // vector of 4800 deltachi2_4v vectors 
  std::vector<TVectorD> vecobs; // vector of 4800 deltachi2obs observations vectors
  std::vector<double> vecexcl; // vector of 4800 data exclusion deltachi2obs observations 
  std::vector<double> vecx; // vector of 4800 deltachi2_3v vectors
  std::vector<double> vecy; // vector of 4800 deltachi2_4v vectors 

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
      TVectorD *obs = nullptr;
      TVectorD *v3v = nullptr;
      TVectorD *v4v = nullptr;
      r.GetObject("4v_deltachi2", v4v);   // 4v tests vector
      r.GetObject("3v_deltachi2", v3v);   // 3v tests vector
      d.GetObject("deltachi2_obs", obs);   // sensitivity simulation vector
      auto ref = (TParameter<double>*)e.Get("deltachi2_ref");
      double exclusion = ref->GetVal();
      double x = theta->GetVal();
      double y = dm2->GetVal();

      // Save objects
      vecobs.push_back(*obs);
      vecd3v.push_back(*v3v);
      vecd4v.push_back(*v4v);
      vecexcl.push_back(exclusion);
      vecx.push_back(x);
      vecy.push_back(y);
    }
  }
  // Create a vector of histograms
  cout << "Creating cls heatmaps \n";
  std::vector<TH2D*> clsHeat;
  for (size_t obsindex = 0; obsindex < vecobs[0].size(); obsindex++) { // repeat for each simulated universe
    // Create 1 histogram per simulation
        TH2D* h2 = new TH2D("h2", "Parameter Map;X;Y", 
			60, &xbins[0],
                            80, &ybins[0]);
        h2->SetDirectory(nullptr);   // ROOT won't auto-delete it
	h2->SetStats(0);             // optional: hide stats box

    // Loop over all files
    for (size_t fileindex = 0; fileindex < vecd3v.size(); fileindex++) { // repeat for each file index
      double count3v = 0;
      double count4v = 0;
      for (size_t toyindex = 0; toyindex < vecd3v[0].size(); toyindex++) { // loop over all the toys 
	if (vecd3v[fileindex][toyindex] >= vecobs[fileindex][obsindex]) {
	  count3v++;
	}
	if (vecd4v[fileindex][toyindex] >= vecobs[fileindex][obsindex]) {
	  count4v++;
	}
      }
      // calculate cls
      double cls = 0;
      if( count3v == 0 ) { 
	if( count4v == 0 ) cls = 0;
	else cls = 1;
      }   
      else cls = count4v / count3v;
      if( count4v>=count3v ) cls = 1;
      // Fill in the histogram
      h2->SetBinContent(vecx[fileindex], vecy[fileindex], cls);
    }
    // Fill in that universe heatmap
    clsHeat.push_back(h2);
  }
  // Writing root file
  cout << "Writing root file \n";
  TFile outfile("clsHeat.root", "RECREATE");

  for (size_t i = 0; i < clsHeat.size(); ++i) {
    if (!clsHeat[i]) continue;

    // Ensure a unique, stable name
    clsHeat[i]->SetName(Form("clsHeat_%zu", i));
    clsHeat[i]->Write();
  }

  outfile.Close();
}
