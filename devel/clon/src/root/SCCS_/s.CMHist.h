h64964
s 00003/00001/00085
d D 1.4 03/06/20 17:35:42 boiarino 5 4
c make hbook global
e
s 00001/00002/00085
d D 1.3 02/03/26 15:17:56 boiarino 4 3
c *** empty log message ***
e
s 00005/00000/00082
d D 1.2 02/01/11 16:48:43 boiarino 3 1
c minor
c 
e
s 00000/00000/00000
d R 1.2 01/11/19 16:44:45 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 clas/root/CMHist.h
e
s 00082/00000/00000
d D 1.1 01/11/19 16:44:44 boiarino 1 0
c date and time created 01/11/19 16:44:44 by boiarino
e
u
U
f e 0
t
T
I 1
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
I 3
D 4
#include "CMTline.h"
E 4
E 3
#include "CMViewer.h"
#include "CMMenu.h"

I 3
// max number of timeline plots
#define NTLPLOT 1000
E 3

class CMHist : public CMClient {

private:

D 5
  CMHbook *hbook;
E 5
I 5
  //CMHbook *hbook;
E 5
I 3
D 4
  CMTline *tline[NTLPLOT];
E 4
I 4

E 4
  Int_t ntimeline;                    // current number of timeline plots
E 3
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

I 5
  CMHbook *hbook;

E 5
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


E 1
