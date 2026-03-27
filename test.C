void test() {
  TFile contourfile("202603-contours.root ", "READ");
  for(int universe = 1; universe <=1000; universe++) {
    TString universepath = TString::Format("universe%i", universe);
    TDirectory* dir = (TDirectory*)contourfile.Get(universepath);
    /* cout << universe << " \n"; */
    cout << universe << " "  << dir->GetNkeys() << "\n";
  }
}
