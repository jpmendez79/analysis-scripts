#include "MendezStyle.h"
void debug() {
  TFile ch("dir_root/clsHeat-fixed.root");
  /* TFile s("dir_root/sens-contours-all.root", "READ"); */
  
  TH2D* cls = (TH2D*)ch.Get("clsHeat_1");
  cls->SetDirectory(nullptr);
  /* auto cls = ch.Get("clsHeat_1"); */
  auto contours = mendezstyle::GetContourGraphs(cls, 0.05);
  TCanvas* c = new TCanvas("c", "Results", 900, 800);
  c->SetLogx();
  c->SetLogy();
  cls->Draw("colz");
  int color = 0;
mendezstyle::WesZPalette(255);
cout << "debug" << contours.size() << endl;
int ungraph = 0;
int graph = 0;
int threshold = 50;
int graphIndex = 0;
  
for (auto gr : contours) {
  int count = gr->GetN();
 
if (gr->GetN() < threshold) {

      /* gr->SetLineColor(kRed); */
      /* gr->Draw("same"); */
graphIndex++;
}
else {
}
}

cout << graphIndex << endl;
contours[graphIndex]->Draw();
  c->Update();
}
