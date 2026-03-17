#include "MendezStyle.h"

void SaveContours() {
  TFile clsfile("cls_map-old.root", "READ");
  TFile outfile("cls-sensitivity-old.root", "RECREATE");
  int numUniverse = 10;
  double contour_level = 0.005;
  for (int universe = 1; universe <=1000; universe++) {
    TH2D *uhist = nullptr;
    TString histname = TString::Format("clsHeat%i", universe);
    clsfile.GetObject(histname, uhist);
    auto contours = mendezstyle::GetContourGraphs(uhist, contour_level);
    TDirectory* dir = outfile.mkdir(Form("universe%d",universe));
    dir->cd();
    for(size_t i=0;i<contours.size();i++){
        contours[i]->SetName(Form("contour%zu",i));
        contours[i]->Write();
    }
    outfile.cd();
  }
  clsfile.Close();
  outfile.Close();
}
