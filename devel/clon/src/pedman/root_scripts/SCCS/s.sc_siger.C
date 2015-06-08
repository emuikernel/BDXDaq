h63277
s 00001/00001/00089
d D 1.3 02/08/28 21:56:13 boiarino 4 3
c //fclose(spar_old);
e
s 00001/00002/00089
d D 1.2 02/08/28 20:36:38 boiarino 3 1
c sc->sc1
e
s 00000/00000/00000
d R 1.2 70/01/01 00:00:02 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 pedman/s/sc_siger.C
e
s 00091/00000/00000
d D 1.1 00/07/31 15:01:43 gurjyan 1 0
c date and time created 00/07/31 15:01:43 by gurjyan
e
u
U
f e 0
t
T
I 1
//=================================================== 
// SC 3d histogramming for 
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
title->AddText("CLAS SC");
title->Draw();

pad1 = new TPad("pad1","koko1",0.03,0.50,0.98,0.95,21);
pad2 = new TPad("pad2","koko2",0.03,0.02,0.98,0.48,21);
pad1->Draw();
pad2->Draw();


D 3
FILE *ped_new = fopen("/usr/local/clas/parms/pedman/archive/sc.ped","r");
//FILE *spar_old = fopen("/usr/local/clas/parms/initfiles/croc06.spar","r");
E 3
I 3
FILE *ped_new = fopen("/usr/local/clas/parms/pedman/archive/sc1.ped","r");
E 3


// Create 2d histogram
  hpxpy  = new TH2F("hpxpy"," SC  Sigma",70,0,69,26,0,25);



   Int_t n = 640;
   Int_t i=1;
   Int_t ncols1, ncols2;
   Double_t z[640];
   Axis_t x[640],y[640];
   Float_t slot_ped, channel_ped, mean_ped, sigma, offset;
   Float_t slot_old, channel_old, value_old; 

   for(i=0;i<=639;i++) {
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
D 4
fclose(spar_old);
E 4
I 4
//fclose(spar_old);
E 4

}



E 1
