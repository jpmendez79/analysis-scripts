void posNegVanilla1DStudy() {
  double sin2_2theta_uu = 0.1;
  double idm2 = 1;
  double ig2 = 2.5 * TMath::Pi();
/* double ig2 = 0; */
  double baseline = 470;

    auto neg_prob_uu = [idm2, baseline](double sin2_2theta_ee, double ig2, double Enu) { 
    double x = ( 1 - sqrt(1-sin2_2theta_ee) )/2;// solution of >1/2
  double Delta = 1.267 * idm2 * baseline / Enu;
  double cos_2Delta = cos(2*Delta);
  double lambda = Delta * ig2 / (4 * TMath::Pi());
  return 1 - 2*x*(1 - exp(-1 * lambda / 2) * cos_2Delta) + x*x*(1 - 2*exp(-1 * lambda / 2) * cos_2Delta +exp(-1 * lambda));
 };
    auto pos_prob_uu = [idm2, baseline](double sin2_2theta_ee, double ig2, double Enu) { 
    double x = ( 1 + sqrt(1-sin2_2theta_ee) )/2;// solution of >1/2
  double Delta = 1.267 * idm2 * baseline / Enu;
  double cos_2Delta = cos(2*Delta);
  double lambda = Delta * ig2 / (4 * TMath::Pi());
  return 1 - 2*x*(1 - exp(-1 * lambda / 2) * cos_2Delta) + x*x*(1 - 2*exp(-1 * lambda / 2) * cos_2Delta +exp(-1 * lambda));
 };

auto van_uu = [idm2, baseline](double sin2_2theta_uu, double Enu) {
  double Delta = 1.267 * idm2 * baseline / Enu;
  double sin2_Delta = sin(Delta)*sin(Delta);
  return 1 - sin2_2theta_uu * sin2_Delta;
};

int sample_size = 30000;
TH1D* pos = new TH1D("pos", "Probability;Energy;Prob", sample_size, 0, 2500);
TH1D* neg = new TH1D("neg", "Probability;Energy;Prob", sample_size, 0, 2500);
TH1D* van = new TH1D("van", "Probability;Energy;Prob", sample_size, 0, 2500);
  for (int i = 1; i <= sample_size; i++) {
    double Enu = van->GetBinCenter(i);
    double probp = pos_prob_uu(sin2_2theta_uu,ig2,Enu);
    double probn = neg_prob_uu(sin2_2theta_uu,ig2,Enu);
    double probv = van_uu(sin2_2theta_uu,Enu);

    pos->SetBinContent(i,probp);
    neg->SetBinContent(i,probn);
    van->SetBinContent(i,probv);
  }
pos->SetStats(0);
neg->SetStats(0);
van->SetStats(0);
pos->SetLineColor(kRed);
neg->SetLineColor(kBlue);
van->SetLineColor(kGreen);

/* van->Draw(); */
neg->Draw();
pos->Draw("same");
}
