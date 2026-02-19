#include "MendezStyle.h"
TLegend * MakeLegend(float left=0.7, float bottom=0.5, float right=0.9, float top=0.85)
{
  auto leg = new TLegend(left, bottom, right, top);
  leg->SetFillStyle(0);  // unfortunately can't set this in TStyle :(

  return leg;
}
void plotDecay2Dee() {
    double idm2 = 2;
    double sin2_2theta_ee_min = 0.01;
    double sin2_2theta_ee_max = 1;
    /* double baseline = 470; */
    double baseline = 679;
    double g2_min = 0;
    double g2_max = 4*TMath::Pi();
    int decades = std::log10(sin2_2theta_ee_max / sin2_2theta_ee_min);
    int xbins = 5000;
    int ybins = 5000;
    TH2D * h2 =
    new TH2D("prob_ee",
             "#nu_{e} #rightarrow #nu_{e} Disappearance; sin2_2theta_ee;g^{2};)",
             xbins, sin2_2theta_ee_min, sin2_2theta_ee_max, ybins, g2_min, g2_max);


    auto prob_ee = [idm2, baseline](double sin2_2theta_ee, double ig2, double Enu) { 
    double x = ( 1 + sqrt(1-sin2_2theta_ee) )/2;// solution of >1/2
  double Delta = 1.267 * idm2 * baseline / Enu;
  double cos_2Delta = cos(2*Delta);
  double lambda = Delta * ig2 / (4 * TMath::Pi());
  return 1 - 2*x*(1 - exp(-1 * lambda / 2) * cos_2Delta) + x*x*(1 - 2*exp(-1 * lambda / 2) * cos_2Delta +exp(-1 * lambda));
 };

  for (int i = 0; i <= xbins; i++) {
    double x2 = h2->GetXaxis()->GetBinCenter(i);
    for (int j = 0; j <= ybins; j++) {
      double y2 = h2->GetYaxis()->GetBinCenter(j);
      double probee_sum = 0;
      for (int k = 0; k < 6; k++ ) {
	// 0.5 to 1.5 GeV every 0.2 GeV a point, in total 6 points
	double energy = 500 + (k * 200);
	// prob_sum += (1 - prob_ee(x, y, energy));
	probee_sum += (1 - prob_ee(x2, y2, energy));
      }
      double avg_ee = probee_sum / 6;
      // double prob = 1 - prob_ee(x, y);
         h2->SetBinContent(i, j, avg_ee);
      // cout << prob;
    }
  }
mendezstyle::WesZPalette(255);
h2->Draw("colz");

}
