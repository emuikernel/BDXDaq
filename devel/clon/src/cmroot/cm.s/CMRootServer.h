#ifndef CMRootServer_H
#define CMRootServer_H

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// CMRootServer                                                                //
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
#include "TArc.h"
#include "TFile.h"

#include "CMClient.h"
#include "CMHbook.h"
#include "CMViewer.h"
#include "CMMenu.h"

// max number of timeline plots
#define NTLPLOT 1000

class CMRootServer : public CMClient {

private:

  CMHbook *hbook;

  Int_t runnumber;
  Int_t oldrunnumber;

  Int_t ntimeline;                    // current number of timeline plots

  CMViewer *viewer;
  Int_t nview;                        // the number of views
  Int_t iview;

  char  filename[1000];
  Int_t downloaded, prestarted;
  Int_t nhistos;
  Int_t histos[NHIST];

  Int_t nevents, nevents1, time0;
  Float_t rate;

   
public:
                    CMRootServer(Option_t *option="tmp.tmp", CMDisplay *mDisplay=0);
  virtual          ~CMRootServer() {;}

  //CMHbook *hbook;

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



  ClassDef(CMRootServer, 0)
};

#endif


