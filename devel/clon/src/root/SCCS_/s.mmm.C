h18701
s 00000/00000/00000
d R 1.2 01/11/19 16:44:50 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 clas/root/mmm.C
e
s 00126/00000/00000
d D 1.1 01/11/19 16:44:49 boiarino 1 0
c date and time created 01/11/19 16:44:49 by boiarino
e
u
U
f e 0
t
T
I 1
void mmm()
{
   //gROOT->Reset();

TH1F *hhh;

   TFile example("mmm.root");
   //example.ls();
   

   hhh = (TH1F *)h221->Clone();
   hhh->Draw("e1p");

   Double_t par[21];

   TF1 *g10 = new TF1("g10","pol1",0.70,0.75);
   g10->SetLineColor(2);
   hhh->Fit("g10","R0");
   g10->GetParameters(&par[0]);
   cout << par[0] << " " << par[1] << " " <<par[2] << endl;

   TF1 *g20 = new TF1("g20","pol1",1.40,1.60);
   g20->SetLineColor(2);
   g20->SetParLimits(0, par[0], par[0]+0.01);
   hhh->Fit("g20","R0+");
   g20->GetParameters(&par[0]);
   cout << par[0] << " " << par[1] << " " <<par[2] << endl;




   TH1F *htemp = (TH1F*)hhh->Clone();
   htemp->Reset();

   Int_t nbins = htemp->GetNbinsX();
   cout << "nbins=" << nbins << endl;

   for (Int_t bin=1;bin<=nbins;bin++)
   {
      Float_t x = hhh->GetBinCenter(bin);
      //Double_t fval = par[0] + par[1]*x + par[2]*x*x;
      Double_t fval = par[0] + par[1]*x;
      Double_t diff = TMath::Abs(fval - hhh->GetBinContent(bin));
      htemp->Fill(x,diff);
   }

   htemp->Draw("e1p");



g1    = new TF1("g1","gaus",0.75,0.92);
g1->SetLineColor(2);
g2    = new TF1("g2","gaus",0.94,0.99);
g2->SetLineColor(2);
g3    = new TF1("g3","gaus",1.10,1.40);
g3->SetLineColor(2);
htemp->Fit("g1","R0");
htemp->Fit("g2","R0+");
htemp->Fit("g3","R0+");
total = new TF1("total","gaus(0)+gaus(3)+gaus(6)",0.75,1.40);
total->SetLineColor(3);
g1->GetParameters(&par[0]);
g2->GetParameters(&par[3]);
g3->GetParameters(&par[6]);
total->SetParameters(par);
htemp->Fit("total","R0+");



/*
g3 = new TF1("g3","gaus",1.08,1.30);
g3->SetLineColor(2);
g4 = new TF1("g4","gaus",1.40,1.60);
g4->SetLineColor(2);
hhh->Fit("g3","R0");
hhh->Fit("g4","R0+");

tot1 = new TF1("tot1","gaus(0)+gaus(3)",1.08,1.60);
tot1->SetLineColor(3);
g3->GetParameters(&par[0]);
g4->GetParameters(&par[3]);
tot1->SetParameters(par);
hhh->Fit("tot1","R0+");


g2 = new TF1("g2","gaus",0.91,0.95);
g2->SetLineColor(2);
hhh->Fit("g2","R0+");

tot2 = new TF1("tot2","gaus(0)+gaus(3)+gaus(6)",0.91,1.60);
tot2->SetLineColor(4);
g2->GetParameters(&par[0]);
tot1->GetParameters(&par[3]);
tot2->SetParameters(par);
hhh->Fit("tot2","R0+");

g1 = new TF1("g1","gaus",0.8,0.90);
g1->SetLineColor(2);
hhh->Fit("g1","R0+");

tot3 = new TF1("tot3","gaus(0)+gaus(3)+gaus(6)+gaus(9)",0.8,1.60);
tot3->SetLineColor(5);
g1->GetParameters(&par[0]);
tot2->GetParameters(&par[3]);
tot3->SetParameters(par);
hhh->Fit("tot3","R0+");

g0 = new TF1("g0","gaus",0.65,0.78);
g0->SetLineColor(2);
hhh->Fit("g0","R0+");

tot4 = new TF1("tot4","gaus(0)+gaus(3)+gaus(6)+gaus(9)+gaus(12)",0.65,1.60);
tot4->SetLineColor(6);
g0->GetParameters(&par[0]);
tot3->GetParameters(&par[3]);
tot4->SetParameters(par);
hhh->Fit("tot4","R0+");
*/



sleep(1);


}

E 1
