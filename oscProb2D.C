#include "TCanvas.h"
#include "TFrame.h"
#include "TF1.h"
#include "TF2.h"
#include "TH1D.h"
#include "TH2D.h"
#include "THStack.h"
#include "TLegendEntry.h"
#include "TMath.h"
#include "Math/IntegratorOptions.h"
#include "TLegend.h"
#include "TPaletteAxis.h"
#include "TPaveText.h"
#include "MendezStyle.h"

// save a lot of useless repetitive typing
TLegend * MakeLegend(float left=0.7, float bottom=0.5, float right=0.9, float top=0.85)
{
  auto leg = new TLegend(left, bottom, right, top);
  leg->SetFillStyle(0);  // unfortunately can't set this in TStyle :(

  return leg;
}

void TwoDHist(TH2D * h2d, TCanvas * c)
{

  c->Clear();
  c->cd();

  mendezstyle::CenterTitles(h2d);
  mendezstyle::WesZPalette(255);
c->SetRightMargin(0.15);

  h2d->Draw("colz");
  c->SetLogy();
  c->SetLogz();
h2d->GetZaxis()->SetNoExponent();
h2d->GetZaxis()->SetMoreLogLabels();
h2d->GetYaxis()->SetNoExponent();
h2d->GetYaxis()->SetMoreLogLabels();

c->Update();
  

 }


void oscProb2D()
{

TString filename2 = "hist_ee-dm2-2-bl650.root";
  TString filename3 = "hist_ee-dm10-2-bl650.root";

  TFile *f2 = TFile::Open(filename2, "READ");
  TFile *f3 = TFile::Open(filename3, "READ");


  TH2D *hist_ee2 = (TH2D*)f2->Get("prob_ee");
  TH2D *hist_ee10 = (TH2D*)f3->Get("prob_ee");  
hist_ee2->SetTitle("NuMI Baseline #nu_{e} #rightarrow #nu_{e} Disappearance Probability;sin^{2}(2#theta_{ee}); g^{2};");
hist_ee10->SetTitle("NuMI Baseline #nu_{e} #rightarrow #nu_{e} Disappearance Probability;sin^{2}(2#theta_{ee}); g^{2};");
// hist_ee->
TCanvas c;

  TwoDHist(hist_ee2, &c);
  c.SaveAs("ee-dm2-bl650-avg0.5-1.5Gev.png");
  TwoDHist(hist_ee10, &c);
  c.SaveAs("ee-dm10-bl650-avg0.5-1.5Gev.png");
}

#ifndef __ROOTCLING__
int main()
{
  plot();
  return 0;
}
#endif
