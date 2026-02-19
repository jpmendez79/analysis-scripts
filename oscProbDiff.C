#include "MendezStyle.h"
TLegend * MakeLegend(float left=0.7, float bottom=0.5, float right=0.9, float top=0.85)
{
  auto leg = new TLegend(left, bottom, right, top);
  leg->SetFillStyle(0);  // unfortunately can't set this in TStyle :(

  return leg;
}

void oscProbDiff(const char *filename) {

  // === 1. Open the ROOT file ===
  TFile *file = TFile::Open(filename, "READ");

  // === 2. Get the TTree ===
  TTree *tree = (TTree*)file->Get("ttree");


  // === 3. Set up arrays and branch addresses ===
  const Int_t array_size = 3000; // adjust to your known array size
  double prob_van[array_size];
  double prob_dec[array_size];
  double diff[array_size];
  double Etrue[array_size];
  double dm2, sin2_theta_14, sin2_theta_24, g2, sin2_2theta_mue, sin2_2theta_ee, sin2_2theta_uu;
  TH1D *vanilla = new TH1D("ue_vanilla", "hist", 3000, 0, 2500);
  TH1D *dec = new TH1D("ue_dec","hist", 3000, 0, 2500);
  vanilla->SetLineColor(mendezstyle::colors::kOkabeItoSkyBlue);
  dec->SetLineColor(mendezstyle::colors::kOkabeItoYellow);

  TString title = "";
  if(tree->FindBranch("sin2_2thetamue")) {
    cout << "Found mue\n";
    tree->SetBranchAddress("dm2", &dm2);
    tree->SetBranchAddress("sin2_2thetamue", &sin2_2theta_mue);
    tree->SetBranchAddress("ig2", &g2);
    tree->SetBranchAddress("prob_ue", prob_van);
    tree->SetBranchAddress("prob_dec_ue", prob_dec);
    tree->GetEntry(0);
    title = "#nu_{#mu}#rightarrow #nu_{e} Oscillation Probability;E_{#nu} (MeV); Probability";
    vanilla->SetTitle(title);
      for (int i=0; i<array_size; i++) {
	vanilla->SetBinContent(i+1, prob_van[i]);
	dec->SetBinContent(i+1, prob_dec[i]);
      }
      TCanvas *c = new TCanvas("c1","Graph Draw Options", 200,10,600,400);
      TPad * p1;
      TPad * p2;
      p1 = p2 = nullptr;
  
      vanilla->GetXaxis()->SetRangeUser(200, 2500);
      vanilla->GetXaxis()->SetLabelSize(0);
      dec->SetLineStyle(10);
      // dec->GetXaxis()->SetLabelSize(0);
      mendezstyle::SplitCanvas(c, 0.3, p1, p2);
      dec->SetTitle(title);
      c->cd(); p1->Draw(); p1->cd();
      vanilla->Draw();
      dec->Draw("same");
      c->Update();
      c->cd(); p2->Draw(); p2->cd();
      TH1D* diff = (TH1D*)dec->Clone("diff");
      diff->Add(vanilla, -1);
      diff->Divide(vanilla);
      diff->GetYaxis()->SetTitle("(Decay-Vanilla)/Vanilla");
      diff->GetXaxis()->SetTitle("E_{#nu} (MeV)");
      mendezstyle::CenterTitles(diff);
      diff->SetLineColor(kRed);
      diff->SetLineStyle(1);
      diff->GetXaxis()->SetRangeUser(200, 2500);
      diff->GetYaxis()->SetRangeUser(-1, 1);
      diff->Draw();
      TLegend * leg = MakeLegend(0.65, 0.7, 0.7, 0.8);
      leg->AddEntry(vanilla,"Vanilla 3+1", "l");
      leg->AddEntry(dec,"Decay 3+1", "l");
      leg->Draw("same");
      auto pave1 = new TPaveText(0.65, 0.7, 0.7, 0.8, "NDC NB");
      pave1->SetBorderSize(0);
      pave1->SetFillStyle(0);
      pave1->SetTextSizePixels(12);
      TText* head1 = pave1->AddText("Parameters");
      head1->SetTextFont(62);
      pave1->AddText(Form("#Delta  m^{2}_{41}: %.2f", dm2));
      pave1->AddText(Form("sin^{2}(#theta_{#mu e})%.4f", sin2_2theta_mue));  
      pave1->Draw();		//
      c->Update();    
  }
  if(tree->FindBranch("sin2_2theta_uu")) {
    cout << "Found sin2_2theta_uu\n";
    tree->SetBranchAddress("dm2", &dm2);
    tree->SetBranchAddress("sin2_2theta_uu", &sin2_2theta_uu);
    tree->SetBranchAddress("ig2", &g2);
    tree->SetBranchAddress("prob_uu", prob_van);
    tree->SetBranchAddress("prob_dec_uu", prob_dec);
    tree->GetEntry(0);
    title = "#nu_{#mu}#rightarrow #nu_{#mu} Oscillation Probability;E_{#nu} (MeV); Probability";
    vanilla->SetTitle(title);
      for (int i=0; i<array_size; i++) {
	vanilla->SetBinContent(i+1, prob_van[i]);
	dec->SetBinContent(i+1, prob_dec[i]);
      }
      TCanvas *c = new TCanvas("c1","Graph Draw Options", 200,10,600,400);
      TPad * p1;
      TPad * p2;
      p1 = p2 = nullptr;
      vanilla->GetXaxis()->SetRangeUser(200, 2500);
      vanilla->GetXaxis()->SetLabelSize(0);
      dec->SetLineStyle(10);
      mendezstyle::SplitCanvas(c, 0.3, p1, p2);
      dec->SetTitle(title);
      c->cd(); p1->Draw(); p1->cd();
      vanilla->Draw();
      dec->Draw("same");
      c->Update();
      c->cd(); p2->Draw(); p2->cd();
      TH1D* diff = (TH1D*)dec->Clone("diff");
      diff->Add(vanilla, -1);
      diff->Divide(vanilla);
      diff->GetYaxis()->SetTitle("(Decay-Vanilla)/Vanilla");
      diff->GetXaxis()->SetTitle("E_{#nu} (MeV)");
      mendezstyle::CenterTitles(diff);
      diff->SetLineColor(kRed);
      diff->SetLineStyle(1);
      diff->GetXaxis()->SetRangeUser(200, 2500);
      diff->GetYaxis()->SetRangeUser(-1, 1);
      diff->Draw();
      TLegend * leg = MakeLegend(0.65, 0.7, 0.7, 0.8);
      leg->AddEntry(vanilla,"Vanilla 3+1", "l");
      leg->AddEntry(dec,"Decay 3+1", "l");
      leg->Draw("same");
      auto pave1 = new TPaveText(0.65, 0.7, 0.7, 0.8, "NDC NB");
      pave1->SetBorderSize(0);
      pave1->SetFillStyle(0);
      pave1->SetTextSizePixels(12);
      TText* head1 = pave1->AddText("Parameters");
      head1->SetTextFont(62);
      pave1->AddText(Form("#Delta  m^{2}_{41}: %.2f", dm2));
      pave1->AddText(Form("sin^{2}(#theta_{#mu #mu})%.4f", sin2_2theta_uu));  
      pave1->Draw();		//
      c->Update();    
  }
    if(tree->FindBranch("sin2_2theta_ee")) {
      cout << "Found sin2_2theta_ee\n";
      tree->SetBranchAddress("dm2", &dm2);
      tree->SetBranchAddress("sin2_2theta_ee", &sin2_2theta_ee);
      tree->SetBranchAddress("ig2", &g2);
      tree->SetBranchAddress("prob_ee", prob_van);
      tree->SetBranchAddress("prob_dec_ee", prob_dec);
      tree->GetEntry(0);
      title = "#nu_{ee}#rightarrow #nu_{ee} Oscillation Probability;E_{#nu} (MeV); Probability";
      vanilla->SetTitle(title);
      for (int i=0; i<array_size; i++) {
	vanilla->SetBinContent(i+1, prob_van[i]);
	dec->SetBinContent(i+1, prob_dec[i]);
      }
      TCanvas *c = new TCanvas("c1","Graph Draw Options", 200,10,600,400);
      TPad * p1;
      TPad * p2;
      p1 = p2 = nullptr;
  
      vanilla->GetXaxis()->SetRangeUser(200, 2500);
      vanilla->GetXaxis()->SetLabelSize(0);
      dec->SetLineStyle(10);
      mendezstyle::SplitCanvas(c, 0.3, p1, p2);
      dec->SetTitle(title);
      c->cd(); p1->Draw(); p1->cd();
      vanilla->Draw();
      dec->Draw("same");
      c->Update();
      c->cd(); p2->Draw(); p2->cd();
      TH1D* diff = (TH1D*)dec->Clone("diff");
      diff->Add(vanilla, -1);
      diff->Divide(vanilla);
      diff->GetYaxis()->SetTitle("(Decay-Vanilla)/Vanilla");
      diff->GetXaxis()->SetTitle("E_{#nu} (MeV)");
      mendezstyle::CenterTitles(diff);
      diff->SetLineColor(kRed);
      diff->SetLineStyle(1);
      diff->GetXaxis()->SetRangeUser(200, 2500);
      diff->GetYaxis()->SetRangeUser(-1, 1);
      diff->Draw();
      TLegend * leg = MakeLegend(0.65, 0.7, 0.7, 0.8);
      leg->AddEntry(vanilla,"Vanilla 3+1", "l");
      leg->AddEntry(dec,"Decay 3+1", "l");
      leg->Draw("same");
      auto pave1 = new TPaveText(0.65, 0.7, 0.7, 0.8, "NDC NB");
      pave1->SetBorderSize(0);
      pave1->SetFillStyle(0);
      pave1->SetTextSizePixels(12);
      TText* head1 = pave1->AddText("Parameters");
      head1->SetTextFont(62);
      pave1->AddText(Form("#Delta  m^{2}_{41}: %.2f", dm2));
      pave1->AddText(Form("sin^{2}(#theta_{ee})%.4f", sin2_2theta_mue));  
      pave1->Draw();		//
      c->Update();    
    }
}
  

