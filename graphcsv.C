void graphcsv() {
// Open CSV file
 std::ifstream csv("digitized-numu-exclusion-bnb-ysort.csv");
 std::string line;
 std::vector<double> x, y;
 while (std::getline(csv, line)) {
        std::stringstream ss(line);
        std::string xs, ys;

        if (!std::getline(ss, xs, ',')) continue;
        if (!std::getline(ss, ys, ',')) continue;

        x.push_back(std::stod(xs));
        y.push_back(std::stod(ys));
    }

 auto g = new TGraph(x.size(), x.data(), y.data());
 g->SetLineColor(kRed);
 g->Draw();
}


 
