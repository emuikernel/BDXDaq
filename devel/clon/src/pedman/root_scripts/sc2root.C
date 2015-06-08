{
//*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
//*-*
//*-*  This program creates :
//*-*    - a one dimensional histogram
//*-*    - a memory-resident ntuple
//*-*     by V. Gyurjyan
//*-*      01/19/98
//*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
  gROOT->Reset();

FILE *fp = fopen("/usr/local/clas/parms/pedman/raw/sc1_root.dat","r");


Float_t crate, slot, channel, value;
   Int_t ncols;
   Int_t nlines = 0;
  const Int_t kUPDATE = 7000;

   TFile *f = new TFile("sc1.root","RECREATE");

   hsc    = new TH1F("hsc","This is SC pedestal distribution",100,0,1000);
   TNtuple *nsc = new TNtuple("sc","SC_Pedman ntuple","crate:slot:channel:value");

  hsc->SetFillColor(48);
  nsc->SetFillColor(48);

   while (1) {
      ncols = fscanf(fp,"%f %f %f %f \n",&crate,&slot,&channel,&value);
      if (ncols < 0) break;    
     if (value<=0) printf("crate=%8f, slot=%8f, channel=%8f, value=%8f\n",crate,slot,channel,value);
     hsc->Fill(value);
     nsc->Fill(crate,slot,channel,value);
     if (!nlines%kUPDATE) {
      hsc->Draw();
      c1->Modified();
      c1->Update();
     }
      nlines++;
      //printf("%d\n",nlines);
   }
   printf(" found %d points\n",nlines);
    c1->Update();
   hsc->SetXTitle("Channel");
   hsc->SetYTitle("Evt");
  
   fclose(fp);

   f->Write();

}




