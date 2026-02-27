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

void CollateAllRoot(const char* dirname = "./") {
  const int NX = 61;
  const int NY = 81;
  std::vector<std::vector<TVectorD>> gridv3v(NX, std::vector<TVectorD>(NY));
    std::vector<std::vector<TVectorD>> gridv4v(NX, std::vector<TVectorD>(NY));
  // 2D grid where each element is a vector
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
      TString rootpath = TString::Format("%s/%s", dirname, fname.Data());
      TFile r(rootpath, "READ");
      if (!r.IsOpen()) {
	std::cerr << "Cannot open file " << rootpath << std::endl;	continue;
      }
      TVectorD *v3v = nullptr;
      TVectorD *v4v = nullptr;
      r.GetObject("4v_deltachi2", v4v);   // 4v tests vector
      r.GetObject("3v_deltachi2", v3v);   // 3v tests vector
      auto dm2 = (TParameter<double>*)r.Get("dm2_grid");
      auto theta = (TParameter<double>*)r.Get("theta_grid");
      auto numtoys = (TParameter<int>*)r.Get("num_toys");
      int toys = numtoys->GetVal();
      double x = theta->GetVal();
      double y = dm2->GetVal();
      int gridx = static_cast<int>(x);
      int gridy = static_cast<int>(y);

      gridv3v[gridx][gridy] = *v3v;
      gridv4v[gridx][gridy] = *v4v;
    }
  }
  TFile outfile("deltachi2_grid.root", "RECREATE");
  TTree tree("tree", "grids of deltachi2");
  tree.Branch("gridv3v", &gridv3v);
  tree.Branch("gridv4v", &gridv4v);
  tree.Fill();          // single entry containing full grid
  tree.Write();
  outfile.Close();
}


