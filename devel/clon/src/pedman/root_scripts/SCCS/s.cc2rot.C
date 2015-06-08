h03112
s 00002/00002/00053
d D 1.2 02/08/28 20:29:28 boiarino 3 1
c cc->cc1
e
s 00000/00000/00000
d R 1.2 70/01/01 00:00:02 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 pedman/s/cc2rot.C
e
s 00055/00000/00000
d D 1.1 00/07/31 15:01:40 gurjyan 1 0
c CodeManager Uniquification: pedman/s/cc2rot.C
c date and time created 00/07/31 15:01:40 by gurjyan
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
FILE *fp = fopen("/usr/local/clas/parms/pedman/raw/cc_root.dat","r");
E 3
I 3
FILE *fp = fopen("/usr/local/clas/parms/pedman/raw/cc1_root.dat","r");
E 3


Float_t crate, slot, channel, value;
   Int_t ncols;
   Int_t nlines = 0;
  const Int_t kUPDATE = 7000;

D 3
   TFile *f = new TFile("cc.root","RECREATE");
E 3
I 3
   TFile *f = new TFile("cc1.root","RECREATE");
E 3

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




E 1
