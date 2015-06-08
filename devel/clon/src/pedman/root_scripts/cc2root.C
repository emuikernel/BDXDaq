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

FILE *fp = fopen("/usr/local/clas/parms/pedman/raw/cc1_root.dat","r");


Float_t crate, slot, channel, value;
   Int_t ncols;
   Int_t nlines = 0;
  const Int_t kUPDATE = 7000;

   TFile *f = new TFile("cc1.root","RECREATE");

   hcc    = new TH1F("hcc","This is CC pedestal distribution",100,0,1000);
   TNtuple *ncc = new TNtuple("cc","CC_Pedman ntuple","crate:slot:channel:value");

  hcc->SetFillColor(48);
  ncc->SetFillColor(48);

   while (1) {
      ncols = fscanf(fp,"%f %f %f %f \n",&crate,&slot,&channel,&value);
      if (ncols < 0) break;    
     if (value<=0) printf("crate=%8f, slot=%8f, channel=%8f, value=%8f\n",crate,slot,channel,value);
     hcc->Fill(value);
     ncc->Fill(crate,slot,channel,value);
     if (!nlines%kUPDATE) {
      hcc->Draw();
      c1->Modified();
      c1->Update();
     }
      nlines++;
      //printf("%d\n",nlines);
   }
   printf(" found %d points\n",nlines);
    c1->Update();
   hcc->SetXTitle("Channel");
   hcc->SetYTitle("Evt");
  
   fclose(fp);

   f->Write();

}




