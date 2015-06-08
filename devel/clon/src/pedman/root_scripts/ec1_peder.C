//=================================================== 
// EC1 3d histogramming for 
// [ped_mean+offset/loded_spar] : [slot] : [channel] 
//
// Author VHG CLAS online
// 01.30.98
//===================================================
{
   gROOT->Reset();
c1->Clear();
//c1 = new TCanvas("c1"," ",115,0,700,730);
c1->SetFillColor(42);
c1->SetHighLightColor(42);
gStyle->SetFrameFillColor(42);
title = new TPaveText(2.2,2,25,2.1);
title->SetFillColor(33);
title->AddText("CLAS EC1");
title->Draw();

pad1 = new TPad("pad1","koko1",0.03,0.50,0.98,0.95,21);
pad2 = new TPad("pad2","koko2",0.03,0.02,0.98,0.48,21);
pad1->Draw();
pad2->Draw();



FILE *spar_read = fopen("/usr/local/clas/parms/pedman/spar_archive/ec1_hard.spar","r");
FILE *spar_load = fopen("/usr/local/clas/parms/pedman/archive/ec1.ped","r");
FILE *calconst = fopen("/usr/local/clas/parms/pedman/Calib/ec1.cal","r");



// Create 2d histogram
  hpxpy  = new TH2F("hpxpy","EC1 log(Old/Current) ",70,0,69,26,0,25);



   Int_t n = 704;
   Int_t i=1;
   Int_t ncols1, ncols2;
   Double_t z[704];
   Axis_t x[704],y[704];
   Float_t slot_hard, channel_hard, spar_hard, ped_hard; 
   Float_t slot_load, channel_load, value_load, sigma, offset; 
   Float_t calcon;

    for(i=0;i<=703;i++) {
      ncols1 = fscanf(spar_read,"%f %f %f %f \n",&slot_hard,&channel_hard,&spar_hard,&ped_hard);

      if (ncols1 < 0 && ncols1 == 111) break;    
      ncols2 = fscanf(spar_load,"%f %f %f %f %f \n",&slot_load,&channel_load,&value_load,&sigma,&offset);
      if (ncols2 < 0) break;    

      ncols2 = fscanf(calconst,"%f %f %f  \n",&slot_load,&channel_load,&calcon);
      if (ncols2 < 0) break;    


if(ped_hard*calcon != 0.)
{
     z[i] = log(value_load/(ped_hard*calcon));
}
else
{
     z[i] = -1.;
     printf("%d -> %f %f %f -> set y=%f\n",i,value_load,ped_hard,calcon,z[i]);
}
     x[i] = channel_hard;
     y[i] = slot_hard;

}

 hpxpy->FillN(n,x,y,z);

 hpxpy->SetContour(40);
 hpxpy->SetFillColor(1);




pad1->cd();
pad1->SetTheta(90);
pad1->SetPhi(0);
 hpxpy->Draw("lego2");
        c1->Modified();
        c1->Update();

pad2->cd();
pad2->SetTheta(45);
pad2->SetPhi(40);
 hpxpy->Draw("lego2");
        c1->Modified();
        c1->Update();


   //Add axis titles.
   //A graph is drawn using the services of the TH1F histogram class.
   //The histogram is created by TGraph::Paint.
   //TGraph::Paint is called by TCanvas::Update. This function is called by default
   //when typing <CR> at the keyboard. In a macro, one must force TCanvas::Update.
fclose(spar_read);
fclose(spar_load);
fclose(calconst);

}



