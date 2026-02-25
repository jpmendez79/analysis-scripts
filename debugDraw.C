/* void debugDraw() */
/* { */
/*     TFile p("DEBUG_0.100000_0.000000_PRED.root", "READ"); */

/*     TMatrixD* pred = nullptr; */
/*     p.GetObject("prediction", pred); */

/*     TH1D* h = new TH1D("h", "Matrix row as 1D histogram", 182, 0, 182); */
/*     for (int j = 0; j < 182; j++) { */
/*       h->SetBinContent(j + 1, (*pred)(0, j)); */
/*       cout << (*pred)(0, j) << "\n"; */
/*       cout <<  h->GetBinContent(j+1) << "\n"; */
/*     } */
/*     TCanvas* c = new TCanvas("c", "Results", 900, 800); */
/*     h->Draw("hist"); */
/*     c->Update(); */

/* } */
#include "MendezStyle.h"

void debugDraw()
{
  TFile p("DEBUG_0.100000_0.000000_PRED.root", "READ");
  TFile d("DEBUG_TOY1.root", "READ");

  TMatrixD* pred = nullptr;
  TMatrixD* data = nullptr;
    p.GetObject("prediction", pred);
    if (!pred) return;
    d.GetObject("pred", data);
    if (!pred) return;
    const int n = 182;

    auto grp = new TGraph(n);
    auto grd = new TGraph(n);
    grp->SetName("grp");
    grp->SetTitle("Matrix row as TGraph;Bin;Value");

    for (int j = 0; j < n; ++j) {
      grp->SetPoint(j, j, (*pred)(0, j));
      grd->SetPoint(j, j, (*data)(0, j));
        std::cout << (*pred)(0, j) << "\n";
    }

    grp->SetLineColor(kBlack);
    grp->SetLineWidth(2);
    grp->SetMarkerStyle(20);
    grd->SetLineColor(kBlue);
    grd->SetLineWidth(2);
    grd->SetMarkerStyle(20);

    auto c = new TCanvas("c", "c");
    grp->Draw("AL");
    grd->Draw("same AL");
    c->Update();
}
