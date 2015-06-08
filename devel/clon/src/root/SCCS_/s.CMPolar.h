h65311
s 00000/00000/00000
d R 1.2 01/11/19 16:44:46 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 clas/root/CMPolar.h
e
s 00060/00000/00000
d D 1.1 01/11/19 16:44:45 boiarino 1 0
c date and time created 01/11/19 16:44:45 by boiarino
e
u
U
f e 0
t
T
I 1
#ifndef CMPolar_H
#define CMPolar_H

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// CMPolar                                                              //
//                                                                      //
// Moller Polarimeter Class                                             //
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

#define NDIM    32
#define NBNK    10000


class CMPolar : public CMClient {

private:
   Int_t             nev;                //Event number
   Int_t             nevent_stat;

   TPad *pad1, *pad2, *pad3, *pad4, *pad5, *pad6, *pad7, *pad8;
   TH1F *tdc1, *adc1, *tdc2, *adc2,*adc3,*adc4;
   TH2F *tdc1adc1, *tdc2adc2, *adc1adc2;

   TText            *scal[12];           //scaler pads
   Float_t adc[NDIM], tdc[NDIM];
   Float_t old_asym;
   UInt_t scaler[NDIM], oldscaler[NDIM];
   UInt_t s_1[4][NBNK], s_2[4][NBNK];
   UInt_t scaler_h[NDIM], scaler_hbar[NDIM];
   UInt_t HLSscaler[NDIM][NDIM];
   UInt_t overflow[NDIM];
   Float_t scaler2[4];


public:
                     CMPolar(Option_t *option="tmp.tmp");
   virtual          ~CMPolar() {;}
   virtual void      Download();
   virtual void      Prestart(Int_t *jw);
   virtual void      Reset();
   virtual void      Hist();
   virtual void      Update();
   virtual int       Event(Int_t *jw);

   ClassDef(CMPolar, 0)   // Polar for Moller Polarimeter
};

#endif
E 1
