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

void graphCompare() {

  std::vector<std::string> files = {
    "mendez-invisible-g2_0-numu-exclusion-bnb-only-nature-dataset.root",
    "mendez-invisible-vanilla-numu-exclusion-bnb-numi-nature-dataset.root",
    "digitized-numu-exclusion-bnb-public-note.root",
    "neg-numu.root",
    "g2=0_bnb_only.root",
"g2=2.5pi_negative_solution_bnb_only.root",
"g2=2.5pi_positive_solution_bnb_only.root"
  };

  auto graphs = GetGraphsFromFiles(files);

  double xbins[61], ybins[81];

  double xmin = -2, xmax = 0;
  double ymin = -2, ymax = 2;

  // Construct log10 bin boundaries
  for (int i = 0; i <= 60; i++) {
    for (int j = 0; j <= 80; j++) {
      xbins[i] = pow(10, xmin + i * (xmax - xmin) / 60); // theta bins
      ybins[j] = pow(10, ymin + j * (ymax - ymin) / 80); // dm2 bins
    }
  }

  // Create a 2D histogram
  TH2D *h2 = new TH2D(
      "h2", "Data Exlusion CLs 95%;sin2_2theta_{#mu#mu};#Delta m_{41}^{2}", 60,
      &xbins[0], 80, &ybins[0]);

  TCanvas *c = new TCanvas("c", "Results", 900, 800);
  TLegend *leg = MakeLegend(0.65, 0.6, 0.9, 0.75);
  h2->Draw("axis");

  c->SetLogx();
  c->SetLogy();
  mendezstyle::WesZPalette(255);
  mendezstyle::CenterTitles(h2);
  int color = 1;
  for (size_t i = 0; i < graphs.size(); ++i) {
    auto &item = graphs[i];
    item.graph->SetLineColor(color);
    item.graph->SetMarkerColor(color);
    color++;
    item.graph->Draw("same");
    leg->AddEntry(item.graph, item.label.c_str(), "l");
  }
  leg->Draw();
}
