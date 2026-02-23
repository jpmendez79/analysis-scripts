#include <TFile.h>
#include <TParameter.h>
#include <TH2D.h>
#include <TSystemDirectory.h>
#include <TSystemFile.h>
#include <TList.h>
#include <TString.h>
#include <iostream>
#include "MendezStyle.h"
#include <fstream>

void plotAllSens(const char* dirname = "./") {
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
		      60, &xbins[0],
                      80, &ybins[0]);
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
    TString clspath = fullpath+".cls";
    // exclpath.ReplaceAll(".root", ".exclusion");
    TFile f(fullpath, "READ");
    if (!f.IsOpen()) {
    std::cerr << "Cannot open file " << fullpath << std::endl;
    continue;
    }
    TFile c(clspath, "READ");
    if (!c.IsOpen()) {
    std::cerr << "Cannot open file " << clspath << std::endl;
    continue;
    }

    // Retrieve parameter objects
    auto dm2 = (TParameter<double>*)c.Get("dm2");
    auto theta = (TParameter<int>*)c.Get("theta");
    TVectorD *v3v = nullptr;
    TVectorD *v4v = nullptr;
    TVectorD *cls = nullptr;
    c.GetObject("valcls", cls);
    f.GetObject("3v_deltachi2", v3v);   // name of the object inside the root file
    f.GetObject("4v_deltachi2", v4v);   // name of the object inside the root file
    double x = theta->GetVal();
    double y = dm2->GetVal();
    double z = (*cls)[0];
    h2->SetBinContent(x, y, z);
    // if (z==0) cout << "zero," << x <<"," << y <<endl;
    // if (z==1) cout << "inf," << x <<"," << y <<endl;
    f.Close();
    c.Close();
    }
  }
  TCanvas* c = new TCanvas("c", "Results", 900, 800);
  c->SetLogx();
  c->SetLogy();
mendezstyle::WesZPalette(255);
mendezstyle::CenterTitles(h2);
    double contour_level = 0.95;
    auto contours = mendezstyle::GetContourGraphs(h2, contour_level);

h2->Draw("colz");
for (auto gr : contours) {
        gr->SetLineColor(kBlue);
        gr->SetLineStyle(1);
        gr->SetLineWidth(3);
        gr->Draw("L SAME");
}

  
  

}
