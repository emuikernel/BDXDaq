#ifndef CMSda_H
#define CMSda_H

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// CMSda                                                                //
//                                                                      //
// CLAS Online Analisys                                                 //
//                                                                      //
//////////////////////////////////////////////////////////////////////////


#ifndef ROOT_TObject
#include <TObject.h>
#endif
#include "TH1.h"
#include "TH2.h"
#include "TCanvas.h"
#include "TText.h"

#include "CMClient.h"
#include "CMMenu.h"

#define NVIEWS   3

#define NDIM    32

#define MMSTEP  0.01
#define MMRES   100.001
// have to be MMRES*10
#define MMRANGE 1000

#define QSTEP   0.05
#define QRES    20.001
// have to be QRES
#define QRANGE  20

#define PBSTEP 1.0

class CMSda : public CMClient {

private:

   Int_t             nview;
   Int_t             viewActive[NVIEWS];  // 1 for active view, 0 for ...

   TestMainFrame *fMenu;
   TestDialog *fDialog;
   TestMsgBox *fMsg;
   TestSliders *fSlider;

   Int_t setnum, callprlib, calltrlib;
   Int_t downloaded, prestarted;

   Int_t Nhel[QRANGE][MMRANGE], Nhelbar[QRANGE][MMRANGE];
   Int_t fT;
   Float_t fQ1, fQ2, fPB;
   Float_t fMMe1, fMMe2, fMMe3, fMMe4;
   Float_t xMMe1, xMMe2, xMMe3, xMMe4;
   TText *QText[2], *TTText, *PBText;
   TText *MMText[4];

   Int_t nevents, nevents1, time0;
   Float_t rate;

   TH1F *tdc1, *adc1, *tdc2, *adc2, *adc3, *adc4, *adc5, *adc6, *adc7, *adc8;
   TH2F *tdc1adc1, *tdc2adc2, *adc1adc2, *adc5adc6;

   TPad *pad0;
   TPad *pad11, *pad12, *pad13, *pad14, *pad15, *pad16, *pad17, *pad18;
   TPad *pad21, *pad22, *pad23, *pad24;
   TPad *pad31, *pad32, *pad33, *pad34;
   TPad *pad41, *pad42, *pad43, *pad44;

   TH1F *cx, *cy, *cz, *target1, *target2, *target3, *mom1;
   TH1F *ebeam, *qsqrt, *melal, *melal00, *melal01, *melal02, *melal03, *melel, *wmass;
   TH1F *melal1, *melal2, *melal11, *melal21, *melal12, *melal22, *melal13, *melal23;

   TText *scal[18];
   UInt_t scaler[NDIM], oldscaler[NDIM], overflow[NDIM];
   Float_t scaler2[4];

   TArc  *m_Arc11, *m_Arc12, *m_Arc13, *m_Arc14;
   TArc  *m_Arc2, *m_Arc3, *m_Arc4;
   
public:
                     CMSda(Option_t *option="tmp.tmp");
   virtual          ~CMSda() {;}

// overload CMClient methods

   virtual void      Download();
   virtual void      Prestart(Int_t *iw);
   virtual void      Go(Int_t *iw);
   virtual void      End(Int_t *iw);
   virtual void      Reset();
   virtual void      Hist();
   virtual void      Update();
   virtual int       Event(Int_t *iw);

   Int_t             CMhist2root(Int_t id);
   Int_t             CMhistf2root(Int_t id);
   Int_t             CMbos2root(Int_t *jw);
   Int_t             CMgetHISS(Int_t *jw, Int_t ind);
   Int_t             CMgetHISI(Int_t *jw, Int_t ind);
   Int_t             CMgetHISF(Int_t *jw, Int_t ind);
   Int_t             CMgetHistFromBOS(Int_t *jw, Int_t id);

// class-specific methods

   virtual void      Create1();
   virtual void      Delete1();
   virtual void      Plot1();

   virtual void      Create2();
   virtual void      Delete2();
   virtual void      Plot2();

   virtual void      Create3();
   virtual void      Delete3();
   virtual void      Plot3();

   virtual void      Create4();
   virtual void      Delete4();
   virtual void      Plot4();

   virtual void      Status();
   virtual void      Buttons();
   virtual void      ChangeSetnum();
   virtual void      ChangePrlib();
   virtual void      ChangeTrlib();

   virtual void      MMe1Down() {fMMe1 -= MMSTEP; Update();}
   virtual void      MMe1Up()   {fMMe1 += MMSTEP; Update();}
   virtual void      MMe2Down() {fMMe2 -= MMSTEP; Update();}
   virtual void      MMe2Up()   {fMMe2 += MMSTEP; Update();}
   virtual void      MMe3Down() {fMMe3 -= MMSTEP; Update();}
   virtual void      MMe3Up()   {fMMe3 += MMSTEP; Update();}
   virtual void      MMe4Down() {fMMe4 -= MMSTEP; Update();}
   virtual void      MMe4Up()   {fMMe4 += MMSTEP; Update();}

   virtual void      Q1Down() {fQ1 -= QSTEP; Update();}
   virtual void      Q1Up()   {fQ1 += QSTEP; Update();}
   virtual void      Q2Down() {fQ2 -= QSTEP; Update();}
   virtual void      Q2Up()   {fQ2 += QSTEP; Update();}

   virtual void      PBDown() {fPB -= PBSTEP; Update();}
   virtual void      PBUp()   {fPB += PBSTEP; Update();}

   virtual void      TChange() {if(fT==0) fT=1; else fT=0; Update();}

   virtual void      Menu();

   ClassDef(CMSda, 0)
};

#endif


