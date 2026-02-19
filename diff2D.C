#include "MendezStyle.h"
#include <TFile.h>
#include <TH2D.h>
#include <TList.h>
#include <TParameter.h>
#include <TString.h>
#include <TSystemDirectory.h>
#include <TSystemFile.h>

// save a lot of useless repetitive typing
TLegend *MakeLegend(float left = 0.7, float bottom = 0.5, float right = 0.9,
                    float top = 0.85) {
  auto leg = new TLegend(left, bottom, right, top);
  leg->SetFillStyle(0); // unfortunately can't set this in TStyle :(

  return leg;
}

std::string LabelFromFilename(const std::string &path) {
  // remove directory
  size_t slash = path.find_last_of("/\\");
  std::string name =
      (slash == std::string::npos) ? path : path.substr(slash + 1);

  // remove .root
  size_t dot = name.rfind(".root");
  if (dot != std::string::npos)
    name = name.substr(0, dot);
  std::replace(name.begin(), name.end(), '_', ' ');
/* name.replace( */
  return name;
}
struct GraphWithLabel {
  TGraph *graph;
  std::string label;
};

std::vector<GraphWithLabel>
GetGraphsFromFiles(const std::vector<std::string> &filenames) {
  std::vector<GraphWithLabel> out;

  for (const auto &fname : filenames) {
    TFile *f = TFile::Open(fname.c_str(), "READ");
    if (!f || f->IsZombie())
      continue;

    TIter nextkey(f->GetListOfKeys());
    TKey *key;

    while ((key = (TKey *)nextkey())) {
      TClass *cl = gROOT->GetClass(key->GetClassName());
      if (!cl)
        continue;

      if (cl->InheritsFrom(TGraph::Class())) {
        TGraph *g = (TGraph *)key->ReadObj();
        // g->SetDirectory(nullptr);

        out.push_back({g, LabelFromFilename(fname)});
        break;
      }
    }

    f->Close();
    delete f;
  }

  return out;
}

void diff2D() {
  TFile reffile("ref-g0-chisquare.root", "READ");
  TFile posfile("pos-soln.root", "READ");
  TFile negfile("neg-soln-chisquare.root", "READ");
  TFile uefile("ue4-chisquare.root", "READ");
  TH2D *ref = (TH2D*)reffile.Get("h2");
  TH2D *pos = (TH2D*)posfile.Get("h2");  
  TH2D *neg = (TH2D*)negfile.Get("h2");   
  TH2D *ue = (TH2D*)uefile.Get("h2");  

  TH2D *ratio = (TH2D*)pos->Clone("ratio");
  /* ratio->Reset(); */
ratio->Reset();
  ratio->SetDirectory(nullptr);
  ratio->SetStats(0);

for (int ix = 1; ix <= ref->GetNbinsX(); ++ix) {
  for (int iy = 1; iy <= ref->GetNbinsY(); ++iy) {

    double a = ue->GetBinContent(ix, iy);   // numerator
    double b = ref->GetBinContent(ix, iy); // reference

    double val = 0.0;

    if (b != 0.0) {
      double diff = a - b;
      if (diff != 0.0)
        val = diff / b;
    }

    ratio->SetBinContent(ix, iy, val);
  }
}
ratio->SetEntries(1);

ratio->SetMinimum(-1);
ratio->SetMaximum(1);
ratio->SetTitle("#chi^2 Relative Difference");
  mendezstyle::SymmetricPalette();
  mendezstyle::CenterTitles(ratio);
TCanvas* c = new TCanvas("c", "heatmap", 800, 600);
ratio->Draw("COLZ");
c->Update();
}
