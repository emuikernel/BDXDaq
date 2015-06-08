h03664
s 00002/00002/00053
d D 1.2 02/08/28 20:33:21 boiarino 3 1
c sc->sc1
e
s 00000/00000/00000
d R 1.2 70/01/01 00:00:02 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 pedman/s/sc2root.C
e
s 00055/00000/00000
d D 1.1 00/07/31 15:01:43 gurjyan 1 0
c CodeManager Uniquification: pedman/s/sc2root.C
c date and time created 00/07/31 15:01:43 by gurjyan
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
FILE *fp = fopen("/usr/local/clas/parms/pedman/raw/sc_root.dat","r");
E 3
I 3
FILE *fp = fopen("/usr/local/clas/parms/pedman/raw/sc1_root.dat","r");
E 3


Float_t crate, slot, channel, value;
   Int_t ncols;
   Int_t nlines = 0;
  const Int_t kUPDATE = 7000;

D 3
   TFile *f = new TFile("sc.root","RECREATE");
E 3
I 3
   TFile *f = new TFile("sc1.root","RECREATE");
E 3

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




E 1
