h15325
s 00001/00001/00090
d D 1.7 02/09/05 18:13:31 boiarino 8 7
c fix loop limit
e
s 00011/00001/00080
d D 1.6 02/09/05 16:49:23 boiarino 7 6
c check for dividing by zero
e
s 00003/00003/00078
d D 1.5 02/08/28 20:29:50 boiarino 6 5
c cc->cc1
e
s 00001/00001/00080
d D 1.4 01/10/18 10:44:46 gurjyan 5 4
c *** empty log message ***
e
s 00002/00002/00079
d D 1.3 00/08/16 13:43:03 gurjyan 4 3
c 
e
s 00002/00002/00079
d D 1.2 00/08/16 12:32:00 gurjyan 3 1
c 
e
s 00000/00000/00000
d R 1.2 70/01/01 00:00:02 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 pedman/s/cc_chan_500.C
e
s 00081/00000/00000
d D 1.1 00/07/31 15:01:40 gurjyan 1 0
c date and time created 00/07/31 15:01:40 by gurjyan
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
FILE *calconst = fopen("/usr/local/clas/parms/pedman/Calib/cc.cal","r");
E 6
I 6
FILE *spar_read = fopen("/usr/local/clas/parms/pedman/spar_archive/cc1_hard.spar","r");
FILE *spar_load = fopen("/usr/local/clas/parms/pedman/archive/cc1.ped","r");
FILE *calconst = fopen("/usr/local/clas/parms/pedman/Calib/cc1.cal","r");
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
D 8
   for(i=0;i<=319;i++) {
E 8
I 8
   for(i=0;i<=255;i++) {
E 8
E 4
E 3
      ncols1 = fscanf(spar_read,"%f %f %f %f \n",&slot_hard,&channel_hard,&spar_hard,&ped_hard);

      if (ncols1 < 0 && ncols1 == 111) break;    
      ncols2 = fscanf(spar_load,"%f %f %f %f %f \n",&slot_load,&channel_load,&value_load,&sigma,&offset);
      if (ncols2 < 0) break;    

                 ncols2 = fscanf(calconst,"%f %f %f  \n",&slot_load,&channel_load,&calcon);
          if (ncols2 < 0) break;    

     hard[i] = ped_hard;

     load[i] = value_load;

     slot[i] = slot_hard;
    
     x[i] = i;
D 7
            y[i] = value_load/(ped_hard*calcon);
E 7
I 7

if(ped_hard*calcon != 0.)
{
    y[i] = value_load/(ped_hard*calcon);
}
else
{
    y[i] = -1.;
    printf("%d -> %f %f %f -> set y=%f\n",i,value_load,ped_hard,calcon,y[i]);
}

E 7
	  // COMMENTED OUT IF YOU ARE ALREADY CALIBRATED
	    //       y[i] = value_load/ped_hard;
	    //      	fprintf(calconst,"%f %f %f \n",slot_hard, channel_hard, load[i]/hard[i]);

     //           printf("%f %f %f \n",hard[i],load[i], y[i]);
}
   c1->SetGridx();
   c1->SetGridy();
   c1->GetFrame()->SetFillColor(21);
   c1->GetFrame()->SetBorderSize(12);

   gr = new TGraph(n,x,y);
   gr->SetFillColor(19);
   gr->SetLineColor(3);
   gr->SetLineWidth(-12);
   gr->SetMarkerColor(4);
   gr->SetMarkerStyle(21);
   gr->Draw("ASP");
   c1->Modified();
   c1->Update();

   gr->GetHistogram()->SetXTitle("CC_pedestals                           Channel");
   gr->GetHistogram()->SetYTitle("Old/Current");


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
