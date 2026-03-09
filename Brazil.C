#include "MendezStyle.h"
TLegend * MakeLegend(float left=0.7, float bottom=0.5, float right=0.9, float top=0.85)
{
  auto leg = new TLegend(left, bottom, right, top);
  leg->SetFillStyle(0);  // unfortunately can't set this in TStyle :(

  return leg;
}

void Brazil() {
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
  // TH2D* htemp = new TH2D("htemp", "Parameter Map;X;Y", 60, &xbins[0], 80, &ybins[0]);


  TString senspath = "sens-contours-pointThreshold.root";
  TFile s(senspath, "READ");
  const int num_universe = 1000;
  double dm2_val = 2;

  std::vector<double> xvals;
  std::vector<double> w(xvals.size(),1);
  // int dm2 = htemp->FindBin(dm2_val);
  // double min = 1e6;
  // double max = 1e-6;
  for (int i=0; i<num_universe; i++) {
    TString graph = TString::Format("sens-contour_%i", i);
    TGraph* sgr = nullptr;
    s.GetObject(graph, sgr);
    TGraph temp(sgr->GetN(), sgr->GetY(), sgr->GetX());
    double x = temp.Eval(dm2_val);
    if (x>1 || x<0.01) {
      cout << "Unphysical theta: " << x << "\n";
    }
    else {
      xvals.push_back(temp.Eval(dm2_val));
    }
    // cout << temp.Eval(dm2_val) << end
    /* brazil->Fill(x); */
  }
  s.Close();
  TString title = "95% CLs;sin^{2}2#theta_{#mu#mu};";
  TH1D* brazil= new TH1D("brazil", title, 60, &xbins[0]);
  brazil->FillN(num_universe, xvals.data(), w.data());
  double p[5] = {0.021, 0.157, 0.498, .839, .975};
  double q[5];
  brazil->GetQuantiles(5,q,p);
  cout << "Sensitivity Result Histogram Method\n";
  cout << "Upper/lower 2 sigma: " << q[4] << " " << q[0] << "\n";
  cout << "Upper/lower 1 sigma: " << q[3] << " " << q[1] << "\n";
  cout << "Median: " << q[2] << "\n";

  std::sort(xvals.begin(),xvals.end());//Sorting the vector
  /* xvals.erase( unique( xvals.begin(), xvals.end() ), xvals.end() ); */
  // cout << xvals.size() << "\n";
  int qv[5];
  for (int i=0; i<5; i++) {
    qv[i] = p[i]*xvals.size();
    cout << qv[i] << "\n";
  }
  // 21, 157, 498, 839, 975
  cout << "Sensitivity Result Ordered Vector Method\n";
  cout << "Upper/lower 2 sigma: " << xvals[qv[4]] << " " << xvals[qv[0]] << "\n";
  cout << "Upper/lower 1 sigma: " << xvals[qv[3]] << " " << xvals[qv[1]] << "\n";
  cout << "Median: " << xvals[qv[2]] << "\n";

  TCanvas* c = new TCanvas("c", "Results", 900, 800);
  c->SetLogx();
  mendezstyle::CenterTitles(brazil);
  brazil->Draw("hist");
  int max = brazil->GetMaximum();
  TLine *l2 = new TLine(q[0],0,q[0],max+10);
  l2->SetLineStyle(9);
  l2->SetLineColor(kYellow-7);
  l2->Draw("same");
  TLine *u2 = new TLine(q[4],0,q[4],max+10);
  u2->SetLineStyle(9);
  u2->SetLineColor(kYellow-7);
  u2->Draw("same");
  TLine *l1 = new TLine(q[1],0,q[1],max+10);
  l1->SetLineStyle(9);
  l1->SetLineColor(kGreen-9);
  l1->Draw("same");
  TLine *u1 = new TLine(q[3],0,q[3],max+10);
  u1->SetLineStyle(9);
  u1->SetLineColor(kGreen-9);
  u1->Draw("same");
  TLine *m = new TLine(q[2],0,q[2],max+10);
  m->SetLineStyle(9);
  m->SetLineColor(kRed);
  m->Draw("same");
  TLegend * leg = MakeLegend(0.65, 0.6, 0.9, 0.75);
  leg->Clear();
  leg->AddEntry(brazil,TString::Format("BNB Sensitivity #Deltam^{2}_{41}=%.1f", dm2_val),"l");
  leg->AddEntry(l2, TString::Format("#pm2#sigma = %1.3f/%1.3f",q[4],q[0]),"l");
  leg->AddEntry(l1,TString::Format("#pm1#sigma = %1.3f/%1.3f",q[3],q[1]),"l");
  leg->AddEntry(m,TString::Format("Median Sensitivity = %1.3f",q[2]),"l");
  leg->Draw();
  c->Update();
}
