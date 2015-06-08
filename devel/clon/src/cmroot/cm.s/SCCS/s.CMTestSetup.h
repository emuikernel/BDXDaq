h03010
s 00167/00000/00000
d D 1.1 06/10/19 15:41:13 boiarino 1 0
c date and time created 06/10/19 15:41:13 by boiarino
e
u
U
f e 0
t
T
I 1
#ifndef CMTestSetup_H
#define CMTestSetup_H

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// CMTestSetup                                                          //
//                                                                      //
// CH Test Setup Histograming                                           //
//                                                                      //
//////////////////////////////////////////////////////////////////////////



#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#ifndef ROOT_TObject
#include <TObject.h>
#endif

#include "TText.h"

#include <TROOT.h>
#include <TApplication.h>
#include <TVirtualX.h>

#include <TGListBox.h>
#include <TGClient.h>
#include <TGFrame.h>
#include <TGIcon.h>
#include <TGLabel.h>
#include <TGButton.h>
#include <TGTextEntry.h>
#include <TGMsgBox.h>
#include <TGMenu.h>
#include <TGCanvas.h>
#include <TGComboBox.h>
#include <TGTab.h>
#include <TGSlider.h>
#include <TGDoubleSlider.h>
#include <TGFileDialog.h>
#include <TRootEmbeddedCanvas.h>
#include <TCanvas.h>
#include <TH1.h>
#include <TH2.h>
#include <TRandom.h>
#include <TSystem.h>
#include <TEnv.h>




#include "CMClient.h"
#include "CMHbook.h"
#include "CMViewer.h"
#include "CMMenu.h"








//////////////
// menu class
//////////////





class CMTestSetupMenu : public TGTransientFrame {

private:

  TGCompositeFrame    *fFrame1, *fF4;
  TGButton            *fOkButton, *fCancelButton;
  TGListBox           *fListBox;
  TGTab               *fTab;
  TGLayoutHints       *fL1, *fL2, *fL3;

  Int_t                fFirstEntry;
  Int_t                fLastEntry;
  Int_t                fSlot;

public:
                 CMTestSetupMenu(const TGWindow *p, const TGWindow *main,
                                 UInt_t w, UInt_t h,
                                 UInt_t options = kMainFrame | kVerticalFrame);
  virtual       ~CMTestSetupMenu();

  virtual Bool_t ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2);
  virtual Int_t  GetSlotNumber() {return(fSlot);}
};







//////////////
// main class
//////////////


class CMTestSetup : public CMClient {

private:

  CMHbook *hbook;

  Int_t ntimeline;                    // current number of timeline plots
  CMViewer *viewer;
  Int_t nview;                        // the number of views
  Int_t iview;                        // current view number
  char  filename[1000];
  Int_t call1, call2;
  Int_t downloaded, prestarted, active, ended;

  CMTestSetupMenu *fDialog;
  Int_t slot; // slot number to be displayed

  Int_t nevents, nevents1, time0;
  Float_t rate;

  TArc  *m_Arc1[NVIEW];
  TArc  *m_Arc11, *m_Arc12, *m_Arc13, *m_Arc14;
  TArc  *m_Arc2, *m_Arc3, *m_Arc4;
   
public:
                    CMTestSetup(Option_t *option="tmp.tmp");
  virtual          ~CMTestSetup();

// overload CMClient methods

  virtual void      Download();
  virtual void      Prestart(Int_t *iw);
  virtual void      Go(Int_t *iw);
  virtual void      End(Int_t *iw);
  virtual void      Reset();
  virtual void      Update();
  virtual int       Event(Int_t *iw);

// class-specific methods

  virtual void      Status();
  virtual void      Hists(Int_t ihist1, Int_t nhists);
  virtual void      Buttons();
  virtual void      PreviousView() {if(--iview < 0) iview=nview-1; Status();}
  virtual void      NextView() {if(++iview >= nview) iview=0; Status();}
  virtual void      Change1();
  virtual void      Change2();

  virtual void      Menu();

  ClassDef(CMTestSetup, 0)
};

#endif




E 1
