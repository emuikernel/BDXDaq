h64082
s 00000/00000/00000
d R 1.2 01/11/19 16:44:49 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 clas/root/backsig.C
e
s 00083/00000/00000
d D 1.1 01/11/19 16:44:48 boiarino 1 0
c date and time created 01/11/19 16:44:48 by boiarino
e
u
U
f e 0
t
T
I 1
// example of macro fitting background + signal
// -STEP 1: Generates theoretical function
// -STEP 2: Generates an histogram by sampling the function
// -STEP 3: estimates background parameters
// -STEP 4: estimates signal parameters
// -STEP 5: Combined fit of background + signal
//
   Double_t kTH = -0.5;

Double_t Background(Double_t *x, Double_t *par)
// The background function
{
   Double_t arg = 0;
   if (par[2]) arg = (x[0] - par[1])/par[2];

   Double_t val = par[0]*TMath::Exp(kTH*arg*arg)*x[0]*x[0];
   return val;
}

Double_t Signal(Double_t *x, Double_t *par)
// The signal function: a gaussian
{
   Double_t arg = 0;
   if (par[2]) arg = (x[0] - par[1])/par[2];

   Double_t sig = par[0]*TMath::Exp(-0.5*arg*arg);
   return sig;
}

Double_t Total(Double_t *x, Double_t *par)
// Combined background + signal
{
   Double_t tot = Background(x,par) + Signal(x,&par[3]);
   return tot;
}

void backsig()
{
   // the control function

   //STEP 1: Generates theoretical function
   Int_t npar = 6;
   Double_t params[6] = {100,3,1,350,6,0.5};
   TF1 *theory = new TF1("theory",Total,0,10,npar);
   theory->SetParameters(params);

   //STEP 2: Generates an histogram by sampling the theory function
   TH1F *Data = new TH1F("Data","Data sampled from theory",100,0,10);
   Data->FillRandom("theory",10000);

////////////////////////////////////
// have a histogram at that moment
////////////////////////////////////

   //STEP 3: Estimates background parameters using a gaussian
   Data->Fit("gaus","q0");

///////////////////////////
// fit whole distribution
///////////////////////////

   //STEP 4: Subtract estimated background to original data
   // Creates a temporary histogram and fit a gaussian
   TH1F *htemp = (TH1F*)Data->Clone();
   htemp->Reset();
   TF1 *eback = Data->GetFunction("gaus");

   for (Int_t bin=1;bin<=100;bin++) {
      Float_t x = Data->GetBinCenter(bin);
      Double_t fval = eback->Eval(x);
      Double_t diff = TMath::Abs(fval - Data->GetBinContent(bin));
      htemp->Fill(x,diff);
   }
   htemp->Fit("gaus","q0");

   TF1 *esig = htemp->GetFunction("gaus");

   //STEP 5: Fit background + signal
   eback->GetParameters(&params[0]);
   esig->GetParameters(&params[3]);
   Data->Fit("theory");

}
E 1
