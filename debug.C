#include "MendezStyle.h"
void debug() {
  TH1D *h3 = new TH1D("h3", "Hist;#Delta#chi^2;;", 30, -5,15);
  TH1D *h4 = new TH1D("h4", "Hist;#Delta#chi^2;;", 30, -5,15);

  int dm2 = 40;
  int theta = 30;
  TString filename = TString::Format("vanilla-numu_grid_60x80_sinsquare_theta_uu_%i_dm2_%i.root", theta, dm2);
  TFile distfile(filename);
  TVectorD *v3v = nullptr;
  TVectorD *v4v = nullptr;
  auto numtoys = (TParameter<int>*)distfile.Get("num_toys");
  distfile.GetObject("3v_deltachi2", v3v);   // name of the object inside the root file
  distfile.GetObject("4v_deltachi2", v4v);   // name of the object inside the root file
  int toys = numtoys->GetVal();
  for (int i=0; i<toys; i++) {
    h3->Fill((*v3v)[i]);
    h4->Fill((*v4v)[i]);
  }
  TCanvas* c = new TCanvas("c", "Results", 900, 800);
  h4->SetLineColor(kBlue);
  h4->Draw("hist");
  c->Update();
  h3->SetLineColor(kRed);
  h3->Draw("same");
  
  

    
/*   TFile obsfile("deltachi2_obs-old.root"); */
/*   TFile clsfile("cls_map-old.root"); */

/*   /\* TFile s("dir_root/sens-contours-all.root", "READ"); *\/ */
  
/*   TH2D* cls = (TH2D*)clsfile.Get("clsHeat205"); */
/*   TH2D* obs = (TH2D*)obsfile.Get("deltachi2obsHeat205"); */
/*   cls->SetDirectory(nullptr); */
/*   obs->SetDirectory(nullptr); */
/*   /\* auto cls = ch.Get("clsHeat_1"); *\/ */
/* mendezstyle::CenterTitles(cls); */
/*   mendezstyle::CenterTitles(obs); */
/*   auto contours = mendezstyle::GetContourGraphs(cls, 0.05); */
/*   TCanvas* c = new TCanvas("c", "Results", 900, 800); */
/*   c->SetLogx(); */
/*   c->SetLogy(); */
/*   /\* cls->Draw("colz"); *\/ */
/*   obs->Draw("colz"); */
/*   int color = 0; */
/* mendezstyle::WesZPalette(255); */
/* int colors = 0;   */
/* for (auto gr : contours) { */
/*    gr->SetLineColor(kGreen); */
/*    gr->Draw("same"); */
/*   colors++; */
/* } */
/*   c->Update(); */
}




