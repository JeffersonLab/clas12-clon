void ecal(Int_t nstrips=36)
{

  // params: top_left coords, bottom_right coords
  TCanvas *c1 = new TCanvas("c1","ecal",10,10,700,700);

  Double_t x0 = 0.15;
  Double_t y0 = 0.2;
  Double_t dx = 0.02;
  Double_t dy = dx * cos(30*3.14159265358979323846/180);
  Int_t nstrips = 36;
  Double_t x[4],y[4];
  Int_t i,j;


  // triangles
  while(nstrips>0)
  {
    for(i=0; i<nstrips; i++)
    {
      x[0] = x0 + dx*i;      y[0] = y0;
      x[1] = x[0] + dx;      y[1] = y[0];
      x[2] = x[0] + dx/2.0;  y[2] = y[0] + dy;
      x[3] = x[0];           y[3] = y[0];
      TPolyLine *pl = new TPolyLine(4,x,y);
      pl->SetUniqueID(i);
      pl->SetFillColor(2);
      pl->Draw("f");    
    }

    for(i=0; i<nstrips-1; i++)
    {
      x[0] = x0 + dx*i + dx; y[0] = y0;
      x[1] = x[0] + dx/2.0;  y[1] = y[0] + dy;
      x[2] = x[0] - dx/2.0;  y[2] = y[0] + dy;
      x[3] = x[0];           y[3] = y[0];
      TPolyLine *pl = new TPolyLine(4,x,y);
      pl->SetUniqueID(i);
      pl->SetFillColor(3);
      pl->Draw("f");    
    }

    x0 = x0 + dx/2.0;
    y0 = y0 + dy;
    nstrips --;
  }

  // bars
  

  c1->AddExec("ex","TriangleClicked()");
}


void TriangleClicked()
{
   //this action function is called whenever you move the mouse
   //it just prints the id of the picked triangle
   //you can add graphics actions instead
   int event = gPad->GetEvent();
   printf("event=%d x=%f y=%f\n",event,
          gPad->PixeltoX(gPad->GetEventX()),gPad->PixeltoY(gPad->GetEventY()));
   if (event != 11) return; //may be comment this line 
   TObject *select = gPad->GetSelected();
   if (!select) return;
   if (select->InheritsFrom(TPolyLine::Class()))
   {
     TPolyLine *pl = (TPolyLine*)select;
     printf("You have clicked triangle %d, color=%d\n",
              pl->GetUniqueID(),pl->GetFillColor());
   }
}
