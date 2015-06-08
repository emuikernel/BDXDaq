h57997
s 00000/00000/00000
d R 1.2 01/11/19 16:44:42 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 clas/root/CMCalib.h
e
s 00089/00000/00000
d D 1.1 01/11/19 16:44:41 boiarino 1 0
c date and time created 01/11/19 16:44:41 by boiarino
e
u
U
f e 0
t
T
I 1
#ifndef CMCalib_H
#define CMCalib_H

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// CMCalib                                                              //
//                                                                      //
// CLAS Online Calibration                                              //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#ifndef ROOT_TObject
#include <TObject.h>
#endif
#include "TH1.h"
#include "TH2.h"
#include "TCanvas.h"
#include "TText.h"

#include "CMHist.h"
#include "CMClient.h"
#include "CMCalibMenu.h"

#define NMAP 100

class CMCalib : public CMClient {

private:

  CMHbook *hbook;
  CMViewer *viewer;
  char  filename[1000];
  Int_t callprlib, calltrlib;
  Int_t downloaded, prestarted;

  Int_t nevents, nevents1, time0;
  Float_t rate;

  Int_t nview;                        // the number of views
  Int_t iview;                        // current view number
  Int_t nmap;                         // the number of maps
  Int_t imap;                         // current map number
  char maptype[NMAP][10];
  char mapname[NMAP][100], setname[NMAP][100], subsetname[NMAP][100];
  Int_t dim[NMAP], histid[NMAP], nbin[NMAP];
  Float_t xmin[NMAP], xmax[NMAP];

  TArc  *m_Arc11, *m_Arc12, *m_Arc13, *m_Arc14;
  TArc  *m_Arc2, *m_Arc3, *m_Arc4;
   
  CMCalibMenu *fDialog;

public:
                    CMCalib(Option_t *option="tmp.tmp");
  virtual          ~CMCalib() {;}

// overload CMClient methods

  virtual void      Download();
  virtual void      Prestart(Int_t *iw);
  virtual void      Go(Int_t *iw);
  virtual void      End(Int_t *iw);
  virtual void      Reset();
  virtual void      Hist();
  virtual void      Update();
  virtual int       Event(Int_t *iw);

// class-specific methods

  virtual void      Buttons();
  virtual void      PreviousView() {if(--iview < 0) iview=nview-1; Status();}
  virtual void      NextView() {if(++iview >= nview) iview=0; Status();}
  virtual void      Status();
  virtual int       ReadConfig(Option_t *option="./hist.config");
  virtual void      WriteConfig(Option_t *option="./hist.config");
  virtual void      ReadCalibration() {;}
  virtual void      WriteCalibration() {;}
  virtual void      Menu();

  ClassDef(CMCalib, 0)
};

#endif






E 1