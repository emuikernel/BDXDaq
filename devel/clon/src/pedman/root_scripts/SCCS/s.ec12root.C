h45427
s 00000/00000/00000
d R 1.2 70/01/01 00:00:02 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 pedman/s/ec12root.C
e
s 00054/00000/00000
d D 1.1 00/07/31 15:01:41 gurjyan 1 0
c date and time created 00/07/31 15:01:41 by gurjyan
e
u
U
f e 0
t
T
I 1
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




E 1
