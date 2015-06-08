#ifndef CMHist_H
#define CMHist_H

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// CMHist                                                                //
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
#include "CMHbook.h"
#include "CMViewer.h"
#include "CMMenu.h"

// max number of timeline plots
#define NTLPLOT 1000

class CMHist : public CMClient {

private:

  //CMHbook *hbook;

  Int_t ntimeline;                    // current number of timeline plots
  CMViewer *viewer;
  Int_t nview;                        // the number of views
  Int_t iview;                        // current view number
  char  filename[1000];
  Int_t callprlib, calltrlib;
  Int_t downloaded, prestarted;

  TestMainFrame *fMenu;
  TestDialog *fDialog;
  TestMsgBox *fMsg;
  TestSliders *fSlider;

  Int_t nevents, nevents1, time0;
  Float_t rate;

  TArc  *m_Arc1[NVIEW];
  TArc  *m_Arc11, *m_Arc12, *m_Arc13, *m_Arc14;
  TArc  *m_Arc2, *m_Arc3, *m_Arc4;
   
public:
                    CMHist(Option_t *option="tmp.tmp");
  virtual          ~CMHist() {;}

  CMHbook *hbook;

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

  virtual void      Status();
  virtual void      Buttons();
  virtual void      PreviousView() {if(--iview < 0) iview=nview-1; Status();}
  virtual void      NextView() {if(++iview >= nview) iview=0; Status();}
  virtual void      ChangePrlib();
  virtual void      ChangeTrlib();

  virtual void      Menu();

  ClassDef(CMHist, 0)
};

#endif


