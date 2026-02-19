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
  TH1::AddDirectory(kFALSE);
  std::vector<double> sensitivity;
  int count_zerozero = 0;
  int count_inf = 0;
  int count_big =0;
  double xbins[61], ybins[81];

  double xmin = -2, xmax = 0;
  double ymin = -2, ymax = 2;
  // TH1D* h1 = new TH1D("h1", "#Delta #chi^2;Distribution;", 20, 1, 20);
  // Construct log10 bin boundaries
  for (int i = 0; i <= 60; i++) {
    for (int j = 0; j <= 80; j++) {
    xbins[i] = pow(10, xmin + i*(xmax-xmin)/60);  // theta bins
    ybins[j] = pow(10, ymin + j*(ymax-ymin)/80);  // dm2 bins
    }
  }
  cout << "x bins" << endl;
  for (int i = 0; i <= 60; i++) {
    cout << i << " " << xbins[i] << endl;
  }
  cout << "y bins" << endl;
  for (int j = 0; j <= 80; j++) {
    cout << j << " " << ybins[j] << endl;
  }
  // Create a 2D histogram
  // TH2D* h2 = new TH2D("h2", "Parameter Map;X;Y", 
  // 		      60, &xbins[0],
  //                     80, &ybins[0]);
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
  std::vector<TVectorD> vecd3v; // 61 deltachi2_3v vectors
  std::vector<TVectorD> vecd4v; // 61 deltachi2_4v vectors 
  std::vector<TVectorD> vecobs; // 61 deltachi2obs 1000 observations. vecobs[3][400] = 4th files 401th reference value
  // vecnv[31][30] refers to the 30th files 29th 
  std::vector<TH1D*> testcls; // 61 CLs vectors
  double target = 0.05; // Target CLs value
  while ((file = (TSystemFile*)next())) {
    fname = file->GetName();
    // Only process .root files
    if (!file->IsDirectory() && fname.EndsWith(".root")) {
      TString rootpath = TString::Format("%s/%s", dirname, fname.Data());
      // std::cout << "Processing " << rootpath << std::endl;
      TString deltapath = rootpath + ".deltachi2";
      TFile f(rootpath, "READ");
      TFile d(deltapath, "READ");
      if (!f.IsOpen()) {
        std::cerr << "Cannot open file " << rootpath << std::endl;
        continue;
      }
      if (!d.IsOpen()) {
        std::cerr << "Cannot open deltachi2 file " << deltapath << std::endl;
        continue;
      }
      // Retrieve parameter objects
      auto numdeltachi2 = (TParameter<int>*)f.Get("num_toys");
      auto numobs = (TParameter<int>*)d.Get("num_toys");
      TVectorD *obs = nullptr;
      TVectorD *v3v = nullptr;
      TVectorD *v4v = nullptr;
      d.GetObject("deltachi2_obs", obs);   // name of the object inside the root file
      f.GetObject("4v_deltachi2", v4v);   // name of the object inside the root file
      f.GetObject("3v_deltachi2", v3v);   // name of the object inside the root file

      // Save objects
      vecobs.push_back(*obs);
      vecd3v.push_back(*v3v);
      vecd4v.push_back(*v4v);
	  
    }
  }
  // cout << "vecobs[0][4]: " << vecobs[0][4] << endl;
  // cout << "vecd3v[0][4]: " << vecd3v[0][4] << endl;

  // Now for each 1000 observations
  for(int obs=0; obs<vecobs[0].size(); obs++) {
    // cout << "Psuedo " << obs << "\n";
    // cout << "=========\n";
    // Create a temp histogram to hold cls values for each theta
    TH1D* ch1 = new TH1D("ch1", "CLs", 60, &xbins[0]);
    ch1->SetDirectory(nullptr);   // ROOT won't auto-delete it
    ch1->SetStats(0);             // optional: hide stats box

    // TH1D ch1("ch1", "CLs", 61,0,61);
    // Then for each 61 theta
    for (int theta=0; theta<vecobs.size(); theta++) {
      // cout << theta << "\n";
      // Find the cls value
      double count3v = 0;
      double count4v = 0;
      //Loop through all the delta 3v delta 4v values
      for (int i = 0; i < vecd3v[theta].size(); i++) {
	if(vecd3v[theta][i] >= vecobs[theta][obs]) {
	  count3v++;
	}
	if(vecd4v[theta][i] >= vecobs[theta][obs]) {
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
      ch1->SetBinContent(theta, z);
    }
    // Once all theta bins are filled save the histogram then delete temp hist
    // double min = ch1->
    ch1->SetDirectory(nullptr); // ROOT won't auto-delete
    ch1->SetStats(0);
    // ch1->SetEntries(61);
    testcls.push_back(ch1);
    std::vector<double> vals = FindXAtValue(ch1, target);
    for (int k = 0; k< vals.size(); k++) {
      sensitivity.push_back(vals[k]);
    }
    
  }
  // TCanvas* c = new TCanvas("c", "Results", 900, 800);

  // testcls[101]->Draw();        // first draw → creates axes
  // testcls[101]->GetYaxis()->SetRangeUser(0, 1.5);
  // testcls[101]->Draw("hist");        // first draw → creates axes
  cout << sensitivity.size()  << endl;
TCanvas* c = new TCanvas("c", "Results", 900, 800);
TH1D* hs = new TH1D("hs", "CLs Sensitivity;sin^{2}(2#theta_{#mu#mu});", 60, &xbins[0]);
  for (int i=0; i<sensitivity.size(); i++) {
    cout << sensitivity[i] << endl;
    hs->Fill(sensitivity[i]);
  }
  mendezstyle::CenterTitles(hs);
  hs->Draw();
  double p[5] = {0.025, 0.16, 0.5, 0.84, 0.975};
  double q[5];
  hs->GetQuantiles(5,q,p);
  std::cout << "-2sigma = " << q[0] << std::endl;
  std::cout << "-1sigma = " << q[1] << std::endl;
  std::cout << "median = " << q[2] << std::endl;
  std::cout << "+1sigma = " << q[3] << std::endl;
  std::cout << "+2sigma = " << q[4] << std::endl;
double min = 0;
double max = 125;

// Colors: 2σ = yellow, 1σ = green, median = black
int colors[5] = {kYellow+1, kGreen+2, kBlack, kGreen+2, kYellow+1};
int styles[5] = {1, 1, 2, 1, 1};   // dashed for median if desired

std::vector<TLine*> lines;
for (int i = 0; i < 5; ++i) {
  TLine* l = new TLine(q[i], min, q[i], max);
  l->SetLineColor(colors[i]);
  l->SetLineStyle(styles[i]);
  l->SetLineWidth(2);
  l->Draw("SAME");
  lines.push_back(l);  // keep alive
}
auto fmt = [](const char* name, double x) {
  return Form("%s = %.3g", name, x);
};
TLegend * leg = MakeLegend(0.65, 0.6, 0.9, 0.75);
  leg->Clear();
  leg->AddEntry(hs,"Bin 64: #Deltam^{2} 16.37 CLs 95% Sensitivity","l");
// Add entries in the same order as the lines
leg->AddEntry(lines[0], fmt("-2#sigma", q[0]), "l");
leg->AddEntry(lines[1], fmt("-1#sigma", q[1]), "l");
leg->AddEntry(lines[2], fmt("Median",   q[2]), "l");
leg->AddEntry(lines[3], fmt("+1#sigma", q[3]), "l");
leg->AddEntry(lines[4], fmt("+2#sigma", q[4]), "l");

leg->Draw();
// c->Update();
}

// -2sigma = 0.0686999
// -1sigma = 0.131638
// median = 0.256082
// +1sigma = 0.527334
// +2sigma = 0.706882
//   // cout << testcls[100].GetYaxis()->GetYmin() << "\n";
//   // cout << testcls[100].GetYaxis()->GetYmax() << "\n";

//   // TH1D* h1 = testcls[100];
//   // double v = testcls[0].GetBinContent(3);
//   // int numbins = h1->GetNbinsX();
//   // 
//   // 
//   // c->Update();


