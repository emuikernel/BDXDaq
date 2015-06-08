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


FILE *spar_read = fopen("/usr/local/clas/parms/pedman/spar_archive/lac1_hard.spar","r");
FILE *spar_load = fopen("/usr/local/clas/parms/pedman/archive/lac1.ped","r");

   Int_t n = 512;
   Int_t i=1;
   Int_t ncols1, ncols2;
   Float_t hard[n], load[n], slot[n], x[n], y[n];
   Float_t slot_hard, channel_hard, spar_hard, ped_hard; 
   Float_t slot_load, channel_load, value_load, sigma, offset; 
   Float_t calcon;

   for(i=0;i<=511;i++) {
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

   gr->GetHistogram()->SetXTitle("LAC_spars                           Slot");
   gr->GetHistogram()->SetYTitle("Load/Measured");


   //Add axis titles.
   //A graph is drawn using the services of the TH1F histogram class.
   //The histogram is created by TGraph::Paint.
   //TGraph::Paint is called by TCanvas::Update. This function is called by default
   //when typing <CR> at the keyboard. In a macro, one must force TCanvas::Update.
fclose(spar_read);
fclose(spar_load);
}




