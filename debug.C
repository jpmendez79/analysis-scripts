
// save a lot of useless repetitive typing
TLegend * MakeLegend(float left=0.7, float bottom=0.5, float right=0.9, float top=0.85)
{
  auto leg = new TLegend(left, bottom, right, top);
  leg->SetFillStyle(0);  // unfortunately can't set this in TStyle :(

  return leg;
}

#include "MendezStyle.h"
void debug() {
  int dm2 = 41;
  int theta = 30;
  int universea = 205;
  int universeb = 470;
  TString hname = TString::Format("#Delta#chi^{2} pdf Comparison #Deltam^{2}=%i, sin^{2}(2#theta_{#mu#mu})=%i;#Delta#chi^{2};;", dm2, theta);
  TH1D *h3 = new TH1D("h3", hname, 30, -5,30);
  TH1D *h4 = new TH1D("h4", hname, 30, -5,30);
  TH1D *ho = new TH1D("ho", "obs", 30, -5,30);
  TH1D *hb = new TH1D("ho", "obs", 30, -5,30);

  TFile contourfile("202603-alt005-map.root ", "READ");
  /* for(int universe = 1; universe <=1000; universe++) { */
  /* } */



// vanilla-numu_grid_60x80_chi2obs_toyuniverse_1000.root
// old/vanilla-numu_grid_60x80_sinsquare_theta_uu_0_dm2_0.root
  TString filename = TString::Format("202603-vanilla-numu/202603-vanilla-numu-disappear-60x80-grid-sinsquare-twothetauu_%i-deltamsquare41_%i.root.probdist", theta, dm2);
  /* TString filename = TString::Format("old/vanilla-numu_grid_60x80_sinsquare_theta_uu_%i_dm2_%i.root", theta, dm2); */
  TString obsnamea = TString::Format("dir_root/vanilla-numu_grid_60x80_chi2obs_toyuniverse_%i.root", universea);
  TString obsnameb = TString::Format("dir_root/vanilla-numu_grid_60x80_chi2obs_toyuniverse_%i.root", universeb);

  TMatrixD *ma = nullptr;
  TMatrixD *mb = nullptr;
  TFile afile(obsnamea);
  afile.GetObject("obs_map", ma);
  TFile bfile(obsnameb);
  bfile.GetObject("obs_map", mb);
  double refa = (*ma)(theta,dm2);
  double refb = (*mb)(theta,dm2);
  cout << refa << " " << refb << "\n";


TLine *la = new TLine(refa,0,refa,1);
  la->SetLineStyle(9);
  la->SetLineColor(kRed);
TLine *lb = new TLine(refb,0,refb,1);
  lb->SetLineStyle(9);
  lb->SetLineColor(kBlue);

  TFile distfile(filename);
  TVectorD *v3v = nullptr;
  TVectorD *v4v = nullptr;

/* 	// cout << "Reading " << deltapath << "\n"; */
  auto numtoys = (TParameter<int>*)distfile.Get("pdf_size");
  /* auto numtoys = (TParameter<int>*)distfile.Get("num_toys"); */
  distfile.GetObject("3v_deltachi2", v3v);   // name of the object inside the root file
  distfile.GetObject("4v_deltachi2", v4v);   // name of the object inside the root file
  int toys = numtoys->GetVal();
  for (int universe = 1; universe <=1000; universe++) {
    TString universepath = TString::Format("universe%i", universe);
    TDirectory* dir = (TDirectory*)contourfile.Get(universepath);
    int ccount = dir->GetNkeys();
    TString deltapath = TString::Format("dir_root/vanilla-numu_grid_60x80_chi2obs_toyuniverse_%i.root", universe);
	TFile deltafile(deltapath, "READ");
	TMatrixD* matrix = nullptr;
	deltafile.GetObject("obs_map", matrix);
	deltafile.Close();
	double ref = (*matrix)(theta,dm2);
    ho->Fill(ref);
    if (ccount >= 5) {
      hb->Fill(ref);
    }

}
  for (int i=0; i<toys; i++) {

    h3->Fill((*v3v)[i]);

    h4->Fill((*v4v)[i]);
  }
  cout << "Original Counts \n";
  cout << "ho " << ho->Integral() << endl;
  cout << "h3 " << h3->Integral() << endl;
  cout << "hb " << hb->Integral() << endl;
  double scale = 1 / ho->Integral();
  cout << "scale" << scale << endl;
  TCanvas* c = new TCanvas("c", "Results", 900, 800);
  h4->SetLineColor(mendezstyle::colors::kOkabeItoBlueGreen);
  h4->SetFillColor(mendezstyle::colors::kOkabeItoBlueGreen);
  ho->SetLineColor(kBlue);
  ho->SetFillColor(kBlue);
  h4->SetFillStyle(3356);
  ho->SetFillStyle(3003);
  /* ho->SetMaximum(2500); */
  hb->SetFillColor(kRed);
  hb->SetFillStyle(3003);
  ho->Scale(1/ho->Integral());
  h3->Scale(1/h3->Integral());
  /* h4->Scale(1/h4->Integral()); */
  hb->Scale(scale);
  c->Update();
  cout << "Scaled Counts \n";
  cout << "ho " << ho->Integral() << endl;
  cout << "h3 " << h3->Integral() << endl;
  cout << "hb " << hb->Integral() << endl;

  h3->SetLineColor(mendezstyle::colors::kOkabeItoOrange);
  h3->SetFillColor(mendezstyle::colors::kOkabeItoOrange);
  h3->SetFillStyle(3365);
  h3->Draw("hist");
  ho->Draw("hist same");
  hb->Draw("hist same");
  /* h4->Draw("same"); */
  la->Draw("same");
  lb->Draw("same");
      TLegend * leg = MakeLegend(0.65, 0.6, 0.9, 0.75);
      leg->Clear();
      leg->AddEntry(h3,"3#nu pdf","f");
      leg->AddEntry(ho,"#Delta#chi^{2}_{obs} dist","f");
      leg->AddEntry(hb,"Fraction of bad #Delta#chi^{2}_{obs} dist","f");
      /* leg->AddEntry(h4,"#Delta#chi^{2}_{obs} dist","f"); */
      leg->AddEntry(la,TString::Format("Universe %i #Delta#chi^{2}_{obs}", universea),"l");
      leg->AddEntry(lb,TString::Format("Universe %i #Delta#chi^{2}_{obs}", universeb),"l");
      // leg->AddEntry(contours[0],"CLs Sensitivity 95% ","l");
      // leg->AddEntry(g,"Digitized Nature CLs 95%","p");
      leg->Draw();
      c->Update();

/*   TFile obsfile("deltachi2_obs-old.root"); */


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
