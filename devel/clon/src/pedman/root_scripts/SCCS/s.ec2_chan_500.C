h25898
s 00011/00001/00079
d D 1.2 02/09/05 16:49:39 boiarino 3 1
c check for dividing by zero
e
s 00000/00000/00000
d R 1.2 70/01/01 00:00:02 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 pedman/s/ec2_chan_500.C
e
s 00080/00000/00000
d D 1.1 00/07/31 15:01:41 gurjyan 1 0
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


FILE *spar_read = fopen("/usr/local/clas/parms/pedman/spar_archive/ec2_hard.spar","r");
FILE *spar_load = fopen("/usr/local/clas/parms/pedman/archive/ec2.ped","r");
FILE *calconst = fopen("/usr/local/clas/parms/pedman/Calib/ec2.cal","r");

   Int_t n = 704;
   Int_t i=1;
   Int_t ncols1, ncols2;
   Float_t hard[n], load[n], slot[n], x[n], y[n];
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

     hard[i] = ped_hard;

     load[i] = value_load;

     slot[i] = slot_hard;
    
     x[i] = i;
D 3
          y[i] = value_load/(ped_hard*calcon);
E 3
I 3

if(ped_hard*calcon != 0.)
{
     y[i] = value_load/(ped_hard*calcon);
}
else
{
    y[i] = -1.;
    printf("%d -> %f %f %f -> set y=%f\n",i,value_load,ped_hard,calcon,y[i]);
}

E 3
	  // COMMENTED OUT IF YOU ARE ALREADY CALIBRATED
	  //	y[i] = value_load/ped_hard;
	  //       fprintf(calconst,"%f %f %f \n",slot_hard, channel_hard, load[i]/hard[i]);

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

   gr->GetHistogram()->SetXTitle("EC2_pedestals                       Channel");
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
