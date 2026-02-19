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

// save a lot of useless repetitive typing
TLegend * MakeLegend(float left=0.7, float bottom=0.5, float right=0.9, float top=0.85)
{
  auto leg = new TLegend(left, bottom, right, top);
  leg->SetFillStyle(0);  // unfortunately can't set this in TStyle :(

  return leg;
}

void clsSensExcPlot(const char* dirname = "./")
{

  // std::vector<double> xbins(60+1);
  // std::vector<double> ybins(80+1);

  double xbins[61], ybins[81];

  double xmin = -2, xmax = 0;
  double ymin = -2, ymax = 2;

// Construct log10 bin boundaries
for (int i = 0; i <= 60; i++) {
  for (int j = 0; j <= 80; j++) {
    xbins[i] = pow(10, xmin + i*(xmax-xmin)/60);  // theta bins
    ybins[j] = pow(10, ymin + j*(ymax-ymin)/80);  // dm2 bins
}
  // for (int i = 0; i <= 60; i++) {
  //   for (int j = 0; i <= 80; i++) {
  //   double x_val = -4.0 + i * 60; // original center scale
  //   double y_val = -2.0 + j * 80;

    // xbins[i] = pow(10.0, x_val);  // 10^(-4) to 10^0
    // ybins[j] = pow(10.0, y_val);  // 10^(-2) to 10^2
}
 
    // Create a 2D histogram
    TH2D* h2 = new TH2D("h2", "Parameter Map;X;Y", 
                        // 60, -4, 0,   // X bins
                        // 80, -2, 2);  // Y bins
			60, &xbins[0],
                        80, &ybins[0]);
                        // 60, -4, 0,   // X bins
                        // 80, -2, 2);  // Y bins

 // Open CSV file
//  std::ifstream csv("digitize-nature0.csv");
//   std::vector<double> x, y;
//     double xv, yv;
//     char comma;
// while (csv >> xv >> comma >> yv) {
//   x.push_back(xv);
//   y.push_back(yv);
//     }

// TGraph *g = new TGraph(x.size(), x.data(), y.data());
// g->SetMarkerStyle(20);
// g->SetMarkerSize(0.8);
//   g->SetMarkerColor(kRed);
// g->SetLineColor(kRed);
  
double value = 0.05;

    // // Skip header if present
    // std::string line;
    // std::getline(csv, line);

    // Loop over file and fill histogram
  //   while (csv >> x >> comma >> y) {
//       int bin = nature->FindBin(log10(x),log10(y));
// cout << x << ", " << y << "," << bin << "\n";
//         nature->SetBinContent(bin, value);  // 'value' is the weight for this bin
//     }
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
	    TString exclpath = fullpath+".exclusion";
	    // exclpath.ReplaceAll(".root", ".exclusion");
            TFile f(fullpath, "READ");
            if (!f.IsOpen()) {
                std::cerr << "Cannot open file " << fullpath << std::endl;
                continue;
            }
            // TFile c(exclpath, "READ");
            // if (!c.IsOpen()) {
            //     std::cerr << "Cannot open file " << exclpath << std::endl;
            //     continue;
            // }

            // Retrieve parameter objects
            auto dm2 = (TParameter<double>*)f.Get("dm2_grid");
            auto theta = (TParameter<double>*)f.Get("theta_grid");
	    auto numtoys = (TParameter<int>*)f.Get("num_toys");
	    // auto chi2obs = (TParameter<double>*)c.Get("deltachi2_ref");
	    auto chi2obs = (TParameter<double>*)f.Get("deltachi2_ref");
	    TVectorD *v3v = nullptr;
	    TVectorD *v4v = nullptr;
	    f.GetObject("3v_deltachi2", v3v);   // name of the object inside the root file
	    f.GetObject("4v_deltachi2", v4v);   // name of the object inside the root file
            double x = theta->GetVal();
            double y = dm2->GetVal();
	    double ref = chi2obs->GetVal();
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
// h2->Draw("axis");   // show bin text labels
h2->SaveAs("ue4-chisquare.root");
// h2->SetTitle("#nu_{#mu} Disappearance Only;2|U_{e4}|^{2};#Delta m^{2}");
// g->Draw("P Same");

// for (auto gr : contours) {
//         gr->SetLineColor(kBlue);
//         gr->SetLineStyle(1);
//         gr->SetLineWidth(3);
//         gr->Draw("L SAME");
// }
// for (auto gr : contours) {
//         contours[1]->SetLineColor(kBlue);
//         contours[1]->SetLineWidth(3);
        // contours[0]->Draw("L SAME");
// contours[0]->SaveAs("../mendez-numu-exclusion-bnb-ue4.root");
// }

      // TLegend * leg = MakeLegend(0.65, 0.6, 0.9, 0.75);
      // leg->Clear();
      // leg->AddEntry(contours[0],"Data Exlusion CLs 95% ","l");
      // // leg->AddEntry(contours[0],"CLs Sensitivity 95% ","l");
      // // leg->AddEntry(g,"Digitized Nature CLs 95%","p");
      // leg->Draw();


}
