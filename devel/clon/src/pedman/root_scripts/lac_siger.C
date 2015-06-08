//=================================================== 
// LAC 3d histogramming for 
// [sigma] : [slot] : [channel] 
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
title->AddText("CLAS LAC");
title->Draw();

pad1 = new TPad("pad1","koko1",0.03,0.50,0.98,0.95,21);
pad2 = new TPad("pad2","koko2",0.03,0.02,0.98,0.48,21);
pad1->Draw();
pad2->Draw();


FILE *ped_new = fopen("/usr/local/clas/parms/pedman/archive/lac1.ped","r");


// Create 2d histogram
  hpxpy  = new TH2F("hpxpy"," LAC  Sigma",70,0,69,26,0,25);



   Int_t n = 510;
   Int_t i=1;
   Int_t ncols1, ncols2;
   Double_t z[510];
   Axis_t x[510],y[510];
   Float_t slot_ped, channel_ped, mean_ped, sigma, offset;
   Float_t slot_old, channel_old, value_old; 

   for(i=0;i<=509;i++) {
         ncols1 = fscanf(ped_new,"%f %f %f %f %f\n",&slot_ped,&channel_ped,&mean_ped,&sigma,&offset);
        if (ncols1 < 0) break;    


	//      ncols2 = fscanf(spar_old,"%f %f %f \n",&slot_old,&channel_old,&value_old);
	//      if (ncols2 < 0) break;    

     z[i] =sigma;
     x[i] = channel_ped;
     y[i] = slot_ped;

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
fclose(ped_new);
//fclose(spar_old);

}



