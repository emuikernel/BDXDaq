h00920
s 00002/00002/00053
d D 1.2 02/08/28 20:39:17 boiarino 3 1
c lac->lac1
e
s 00000/00000/00000
d R 1.2 70/01/01 00:00:02 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 pedman/s/lac2root.C
e
s 00055/00000/00000
d D 1.1 00/07/31 15:01:42 gurjyan 1 0
c date and time created 00/07/31 15:01:42 by gurjyan
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

D 3
FILE *fp = fopen("/usr/local/clas/parms/pedman/raw/lac_root.dat","r");
E 3
I 3
FILE *fp = fopen("/usr/local/clas/parms/pedman/raw/lac1_root.dat","r");
E 3


Float_t crate, slot, channel, value;
   Int_t ncols;
   Int_t nlines = 0;
  const Int_t kUPDATE = 7000;

D 3
   TFile *f = new TFile("lac.root","RECREATE");
E 3
I 3
   TFile *f = new TFile("lac1.root","RECREATE");
E 3

   hlac    = new TH1F("hlac","This is LAC pedestal distribution",100,0,1000);
   TNtuple *nlac = new TNtuple("lac","LAC_Pedman ntuple","crate:slot:channel:value");

  hlac->SetFillColor(48);
  nlac->SetFillColor(48);

   while (1) {
      ncols = fscanf(fp,"%f %f %f %f \n",&crate,&slot,&channel,&value);
      if (ncols < 0) break;    
     if (value<=0) printf("crate=%8f, slot=%8f, channel=%8f, value=%8f\n",crate,slot,channel,value);
     hlac->Fill(value);
     nlac->Fill(crate,slot,channel,value);
     if (!nlines%kUPDATE) {
      hlac->Draw();
      c1->Modified();
      c1->Update();
     }
      nlines++;
      //printf("%d\n",nlines);
   }
   printf(" found %d points\n",nlines);
    c1->Update();
   hlac->SetXTitle("Channel");
   hlac->SetYTitle("Evt");
  
   fclose(fp);

   f->Write();

}




E 1
