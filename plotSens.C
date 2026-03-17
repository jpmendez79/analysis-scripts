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
void plotSens() {
  TFile infile("dir_root/clsHeat-fixed.root", "READ");

  std::vector<TH2D*> clsHeatRead;

  TIter next(infile.GetListOfKeys());
  TKey* key = nullptr;

  while ((key = (TKey*)next())) {
    TObject* obj = key->ReadObj();
    if (obj->InheritsFrom(TH2D::Class())) {
      TH2D* h = (TH2D*)obj;
      h->SetDirectory(nullptr);   // detach from file
      clsHeatRead.push_back(h);
    }
  }
  // while ((key = (TKey*)next())) {
  //   TObject* obj = key->ReadObj();
  //   if (obj->InheritsFrom(TH2D::Class())) {
      
  //     clsHeatRead.push_back((TH2D*)obj);
  //   }
  // }
  TCanvas* c = new TCanvas("c", "Results", 900, 800);
  double contour_level = 0.05;
  // nature->Draw("COLZ");
  // c->SetLogx();
  // c->SetLogy();
  // mendezstyle::WesZPalette(255);
  // mendezstyle::CenterTitles(clsHeatRead[0]);
  // clsHeatRead[0]->Draw("axis");   // show bin text labels
int pointThreshold = 50;
TFile out("sens-contours-pointThreshold.root", "RECREATE");
for (int i=0; i<1000; i++) {
    auto contours = mendezstyle::GetContourGraphs(clsHeatRead[i], contour_level);
    int graphIndex = 0;
    for (auto gr : contours) {
      if (gr->GetN() < pointThreshold) {
	graphIndex++;
      }
      else {
	break;
      }
    }
    contours[graphIndex]->SetName(Form("sens-contour_%i", i));
    contours[graphIndex]->Write();
    cout << "Found Universe " << i << "in " << graphIndex << endl;

    
      // gr->SetLineColor(kBlue);
      // gr->SetLineStyle(1);
      // gr->SetLineWidth(3);
      // gr[0]->Draw("L SAME");
      // contours[0]->Draw("L SAME");
    
  }
  // c->Update();
  out.Close();
  // c->SaveAs("allsens.png");
}

