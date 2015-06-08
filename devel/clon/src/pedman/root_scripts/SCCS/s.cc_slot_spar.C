h36605
s 00001/00001/00074
d D 1.6 02/09/05 18:13:01 boiarino 7 6
c fix loop limit
e
s 00002/00002/00073
d D 1.5 02/08/28 20:30:49 boiarino 6 5
c cc->cc1
e
s 00001/00001/00074
d D 1.4 01/10/18 10:43:33 gurjyan 5 4
c *** empty log message ***
e
s 00002/00002/00073
d D 1.3 00/08/16 13:40:16 gurjyan 4 3
c 
e
s 00002/00002/00073
d D 1.2 00/08/16 12:34:04 gurjyan 3 1
c 
e
s 00000/00000/00000
d R 1.2 70/01/01 00:00:02 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 pedman/s/cc_slot_spar.C
e
s 00075/00000/00000
d D 1.1 00/07/31 15:01:41 gurjyan 1 0
c CodeManager Uniquification: pedman/s/cc_slot_spar.C
c date and time created 00/07/31 15:01:41 by gurjyan
e
u
U
f e 0
t
T
I 1
//=================================================== 
//
// Author VHG CLAS online
// 01.30.98
//===================================================
{
   //
   // To see the output of this macro, click  here. 
   //
   gROOT->Reset();
   c1->Clear();


D 6
FILE *spar_read = fopen("/usr/local/clas/parms/pedman/spar_archive/cc_hard.spar","r");
FILE *spar_load = fopen("/usr/local/clas/parms/pedman/archive/cc.ped","r");
E 6
I 6
FILE *spar_read = fopen("/usr/local/clas/parms/pedman/spar_archive/cc1_hard.spar","r");
FILE *spar_load = fopen("/usr/local/clas/parms/pedman/archive/cc1.ped","r");
E 6

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
   Float_t hard[n], load[n], slot[n], x[n], y[n];
   Float_t slot_hard, channel_hard, spar_hard, ped_hard; 
   Float_t slot_load, channel_load, value_load, sigma, offset; 
   Float_t calcon;

D 3
   for(i=0;i<=447;i++) {
E 3
I 3
D 4
   for(i=0;i<=383;i++) {
E 4
I 4
D 7
   for(i=0;i<=319;i++) {
E 7
I 7
   for(i=0;i<=255;i++) {
E 7
E 4
E 3
      ncols1 = fscanf(spar_read,"%f %f %f %f \n",&slot_hard,&channel_hard,&spar_hard,&ped_hard);

      if (ncols1 < 0 && ncols1 == 111) break;    
      ncols2 = fscanf(spar_load,"%f %f %f %f %f \n",&slot_load,&channel_load,&value_load,&sigma,&offset);
      if (ncols2 < 0) break;    


     hard[i] = ped_hard;

     load[i] = value_load;

     slot[i] = slot_hard;
    
     x[i] = i;
          y[i] = spar_hard/(value_load+offset);
	//fprintf(calconst,"%f %f %f \n",slot_hard, channel_hard, load[i]/hard[i]);

     //           printf("%f %f %f \n",hard[i],load[i], y[i]);
}
   c1->SetGridx();
   c1->SetGridy();
   c1->GetFrame()->SetFillColor(21);
   c1->GetFrame()->SetBorderSize(12);

   gr = new TGraph(n,slot,y);
   gr->SetFillColor(19);
   gr->SetLineColor(3);
   gr->SetLineWidth(-12);
   gr->SetMarkerColor(4);
   gr->SetMarkerStyle(21);
   gr->Draw("ASP");
   c1->Modified();
   c1->Update();

   gr->GetHistogram()->SetXTitle("CC_spars                           Slot");
   gr->GetHistogram()->SetYTitle("Load/Measured");


   //Add axis titles.
   //A graph is drawn using the services of the TH1F histogram class.
   //The histogram is created by TGraph::Paint.
   //TGraph::Paint is called by TCanvas::Update. This function is called by default
   //when typing <CR> at the keyboard. In a macro, one must force TCanvas::Update.
fclose(spar_read);
fclose(spar_load);
}




E 1
