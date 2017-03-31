{
  //TFile *f = new TFile("trigtest2_376_lookup.root");
  TFile *f = new TFile("trigtest2.root");
  f.ls();

  TCanvas *c1 = new TCanvas("c1","title",1024,768);
  c1->Clear();
  c1->Update();


  c1->Divide(2,2); /* Automatic pad generation by division */

  c1->cd(1);
  denergy->Draw();

  c1->cd(2);
  dcoordu->Draw();

  c1->cd(3);
  dcoordv->Draw();

  c1->cd(4);
  dcoordw->Draw();

}
