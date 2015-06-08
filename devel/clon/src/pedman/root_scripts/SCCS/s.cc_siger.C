h43380
s 00001/00001/00089
d D 1.7 02/09/05 18:13:13 boiarino 8 7
c fix loop limit
e
s 00001/00001/00089
d D 1.6 02/08/28 21:55:57 boiarino 7 6
c //fclose(spar_old);
e
s 00001/00002/00089
d D 1.5 02/08/28 20:30:29 boiarino 6 5
c cc->cc1
e
s 00003/00003/00088
d D 1.4 01/10/18 10:44:02 gurjyan 5 4
c *** empty log message ***
e
s 00004/00004/00087
d D 1.3 00/08/16 13:41:23 gurjyan 4 3
c 
e
s 00004/00004/00087
d D 1.2 00/08/16 12:33:30 gurjyan 3 1
c 
e
s 00000/00000/00000
d R 1.2 70/01/01 00:00:02 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 pedman/s/cc_siger.C
e
s 00091/00000/00000
d D 1.1 00/07/31 15:01:41 gurjyan 1 0
c CodeManager Uniquification: pedman/s/cc_siger.C
c date and time created 00/07/31 15:01:41 by gurjyan
e
u
U
f e 0
t
T
I 1
//=================================================== 
// CC 3d histogramming for 
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
title->AddText("CLAS CC");
title->Draw();

pad1 = new TPad("pad1","koko1",0.03,0.50,0.98,0.95,21);
pad2 = new TPad("pad2","koko2",0.03,0.02,0.98,0.48,21);
pad1->Draw();
pad2->Draw();


D 6
FILE *ped_new = fopen("/usr/local/clas/parms/pedman/archive/cc.ped","r");
//FILE *spar_old = fopen("/usr/local/clas/parms/initfiles/croc07.spar","r");
E 6
I 6
FILE *ped_new = fopen("/usr/local/clas/parms/pedman/archive/cc1.ped","r");
E 6


// Create 2d histogram
  hpxpy  = new TH2F("hpxpy"," CC  Sigma",70,0,69,26,0,25);



D 3
   Int_t n = 448;
E 3
I 3
D 4
   Int_t n = 384;
E 4
I 4
D 5
   Int_t n = 320;
E 5
I 5
   Int_t n = 256;
E 5
E 4
E 3
   Int_t i=1;
   Int_t ncols1, ncols2;
D 3
   Double_t z[448];
   Axis_t x[448],y[448];
E 3
I 3
D 4
   Double_t z[384];
   Axis_t x[384],y[384];
E 4
I 4
D 5
   Double_t z[320];
   Axis_t x[320],y[320];
E 5
I 5
   Double_t z[256];
   Axis_t x[256],y[256];
E 5
E 4
E 3
   Float_t slot_ped, channel_ped, mean_ped, sigma, offset;
   Float_t slot_old, channel_old, value_old; 

D 3
   for(i=0;i<=447;i++) {
E 3
I 3
D 4
   for(i=0;i<=383;i++) {
E 4
I 4
D 8
   for(i=0;i<=319;i++) {
E 8
I 8
   for(i=0;i<=255;i++) {
E 8
E 4
E 3
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
D 7
fclose(spar_old);
E 7
I 7
//fclose(spar_old);
E 7

}



E 1
