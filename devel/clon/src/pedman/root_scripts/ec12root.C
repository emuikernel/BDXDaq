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


  
FILE *fp = fopen("/usr/local/clas/parms/pedman/raw/ec1_root.dat","r");


Float_t crate, slot, channel, value;
   Int_t ncols;
   Int_t nlines = 0;
  const Int_t kUPDATE = 7000;

   TFile *f = new TFile("ec1.root","RECREATE");

   hpec1    = new TH1F("hec1","This is EC1 pedestal distribution",100,0,1000);
   TNtuple *nec1 = new TNtuple("ec1","EC1_Pedman ntuple","crate:slot:channel:value");

  hec1->SetFillColor(48);
  nec1->SetFillColor(48);

   while (1) {
      ncols = fscanf(fp,"%f %f %f %f \n",&crate,&slot,&channel,&value);
      if (ncols < 0) break;    
     if (value<=0) printf("crate=%8f, slot=%8f, channel=%8f, value=%8f\n",crate,slot,channel,value);
     hpec1->Fill(value);
     nec1->Fill(crate,slot,channel,value);
     if (!nlines%kUPDATE) {
      hpec1->Draw();
      c1->Modified();
      c1->Update();
     }
      nlines++;
      //printf("%d\n",nlines);
   }
   printf(" found %d points\n",nlines);
   
   fclose(fp);

   f->Write();

}




