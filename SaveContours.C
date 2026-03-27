#include "MendezStyle.h"

void SaveContours() {
  TFile clsfile("202603-cls_map.root", "READ");
  TFile outfile("202603-alt005-map-.root", "RECREATE");
  int numUniverse = 1000;
  double contour_level = 0.05;
  for (int universe = 1; universe <= numUniverse; universe++) {
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
