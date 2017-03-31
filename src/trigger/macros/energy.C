{
  TFile *f = new TFile("trigtest2.root");
  f.ls();

  TCanvas *c1 = new TCanvas("c1","title",1024,768);
  c1->Clear();
  c1->Update();

  c1->Divide(2,2); /* Automatic pad generation by division */
  gStyle->SetOptFit(1);

  c1->cd(1);
  energy2->Draw();
  energy2->Fit("gaus");

  c1->cd(2);
  energy1->Draw();
  energy1->Fit("gaus");

  /*
  c1->cd(3);
  dcoordv->Draw();

  c1->cd(4);
  dcoordw->Draw();
  */
}
