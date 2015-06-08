h56279
s 00008/00000/00099
d D 1.3 02/09/05 16:58:20 boiarino 4 3
c check for dividing by zero
e
s 00003/00003/00096
d D 1.2 02/08/28 20:40:53 boiarino 3 1
c lac->lac1
e
s 00000/00000/00000
d R 1.2 70/01/01 00:00:02 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 pedman/s/lac_peder.C
e
s 00099/00000/00000
d D 1.1 00/07/31 15:01:42 gurjyan 1 0
c CodeManager Uniquification: pedman/s/lac_peder.C
c date and time created 00/07/31 15:01:42 by gurjyan
e
u
U
f e 0
t
T
I 1
//=================================================== 
// LAC 3d histogramming for 
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
title->AddText("CLAS LAC");
title->Draw();

pad1 = new TPad("pad1","koko1",0.03,0.50,0.98,0.95,21);
pad2 = new TPad("pad2","koko2",0.03,0.02,0.98,0.48,21);
pad1->Draw();
pad2->Draw();



D 3
FILE *spar_read = fopen("/usr/local/clas/parms/pedman/spar_archive/lac_hard.spar","r");
FILE *spar_load = fopen("/usr/local/clas/parms/pedman/archive/lac.ped","r");
FILE *calconst = fopen("/usr/local/clas/parms/pedman/Calib/lac.cal","r");
E 3
I 3
FILE *spar_read = fopen("/usr/local/clas/parms/pedman/spar_archive/lac1_hard.spar","r");
FILE *spar_load = fopen("/usr/local/clas/parms/pedman/archive/lac1.ped","r");
FILE *calconst = fopen("/usr/local/clas/parms/pedman/Calib/lac1.cal","r");
E 3



// Create 2d histogram
  hpxpy  = new TH2F("hpxpy","LAC log(Old/Current) ",70,0,69,26,0,25);



   Int_t n = 512;
   Int_t i=1;
   Int_t ncols1, ncols2;
   Double_t z[512];
   Axis_t x[512],y[512];
   Float_t slot_hard, channel_hard, spar_hard, ped_hard; 
   Float_t slot_load, channel_load, value_load, sigma, offset; 
   Float_t calcon;

    for(i=0;i<=511;i++) {
      ncols1 = fscanf(spar_read,"%f %f %f %f \n",&slot_hard,&channel_hard,&spar_hard,&ped_hard);

      if (ncols1 < 0 && ncols1 == 111) break;    
      ncols2 = fscanf(spar_load,"%f %f %f %f %f \n",&slot_load,&channel_load,&value_load,&sigma,&offset);
      if (ncols2 < 0) break;    

      ncols2 = fscanf(calconst,"%f %f %f  \n",&slot_load,&channel_load,&calcon);
      if (ncols2 < 0) break;    


I 4
if(ped_hard*calcon != 0.)
{
E 4
     z[i] = log(value_load/(ped_hard*calcon));
I 4
}
else
{
     z[i] = -1.;
     printf("%d -> %f %f %f -> set y=%f\n",i,value_load,ped_hard,calcon,z[i]);
}
E 4
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



E 1
