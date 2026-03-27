#include "MendezStyle.h"
TLegend * MakeLegend(float left=0.7, float bottom=0.5, float right=0.9, float top=0.85)
{
  auto leg = new TLegend(left, bottom, right, top);
  leg->SetFillStyle(0);  // unfortunately can't set this in TStyle :(

  return leg;
}

void debugDraw()
{
  TH1::AddDirectory(kFALSE);
  vector<TH1D*> histos;
  TFile p("3v_toy_spectrums.root", "READ");
  for (int i=0; i<5; i++) {
    TString toy = TString::Format("toy_spectrum_%i", i+1);
    cout << toy << endl;
    TMatrixD* spectrum = nullptr;
    p.GetObject(toy, spectrum);
    TH1D* h = new TH1D(toy, "#nu_{#mu} Energy Spectrum;E_{#nu}", 25, 0, 2500);
    int binindex = 1;
    for (int j = 52; j <= 77; j++) {
      h->SetBinContent(binindex, (*spectrum)(0, j));
      binindex++;
    }
    histos.push_back(h);
  }
  p.Close();
  TFile asimovfile("asimov_prediction.root", "READ");
  TMatrixD* asimov = nullptr;
  asimovfile.GetObject("asimov_mat", asimov);
  asimovfile.Close();
  TH1D* ha = new TH1D("ha", "#nu_{#mu} Energy Spectrum;E_{#nu}", 25, 0, 2500);    
  int binindex = 1;
  for (int j = 52; j <= 77; j++) {
    ha->SetBinContent(binindex, (*asimov)(0, j));
    binindex++;
  }
  TCanvas* c = new TCanvas("c", "Results", 900, 800);
mendezstyle::CenterTitles(ha);
  ha->SetMaximum(12000);
  ha->SetLineColor(kRed);
  ha->Draw("hist");
  /* histos[0]->Draw("same"); */
  for (int i=0; i<4; i++) {
    histos[i]->SetLineStyle(3);
    histos[i]->SetLineColor(mendezstyle::colors::kOkabeItoBlueGreen);
    histos[i]->Draw("same");
  }
  TLegend * leg = MakeLegend(0.65, 0.6, 0.9, 0.75);
  leg->Clear();
  leg->AddEntry(histos[0],"3#nu pseudo experiment","l");
  leg->AddEntry(ha,"3#nu Smooth CV Prediction","l");
  // leg->AddEntry(g,"Digitized Nature CLs 95%","p");
  leg->Draw();

  c->Update();

}


/* void debugDraw() */
/* { */
/*   TFile specfile("3v_toy_spectrums.root", "READ"); */

/*   TMatrixD* mat = nullptr; */
/*     specfile.GetObject("prediction", mat); */
/*     if (!pred) return; */
/*     const int n = 364; */

/*     auto grd = new TGraph(n); */
/*     grp->SetName("grp"); */
/*     grp->SetTitle("Matrix row as TGraph;Bin;Value"); */

/*     for (int j = 0; j < n; ++j) { */
/*       grp->SetPoint(j, j, (*pred)(0, j)); */

/*         std::cout << (*pred)(0, j) << "\n"; */
/*     } */

/*     grp->SetLineColor(kBlack); */
/*     grp->SetLineWidth(2); */
/*     grp->SetMarkerStyle(20); */
/*     grd->SetLineColor(kBlue); */
/*     grd->SetLineWidth(2); */
/*     grd->SetMarkerStyle(20); */

/*     auto c = new TCanvas("c", "c"); */
/*     grp->Draw("AL"); */
/*     grd->Draw("same AL"); */
/*     c->Update(); */
/* } */
