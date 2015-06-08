h34707
s 00000/00000/00000
d R 1.2 01/11/19 16:44:43 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 clas/root/CMCalibMenu.h
e
s 00074/00000/00000
d D 1.1 01/11/19 16:44:42 boiarino 1 0
c date and time created 01/11/19 16:44:42 by boiarino
e
u
U
f e 0
t
T
I 1
#ifndef CMCalibMenu_H
#define CMCalibMenu_H

/////////////////////////////////////
// CMCalibMenu.h
/////////////////////////////////////

#include <stdlib.h>

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


class CMCalibMenu : public TGTransientFrame {

private:
   TGCompositeFrame    *fFrame1, *fF1, *fF2, *fF3, *fF4, *fF5, *fF6, *fF7;
   TGButton            *fOkButton, *fCancelButton, *fStartB, *fStopB;
   TGButton            *fBtn1, *fBtn2, *fChk1, *fChk2;
   TGButton            *fRad1, *fRad2, *fRad3, *fRad4;
   TGPictureButton     *fPicBut1;
   //TGRadioButton       *fRadio1, *fRadio2;
   TGCheckButton       *fCheck1;
   TGCheckButton       *fCheckMulti;
   TGListBox           *fListBox;
   TGComboBox          *fCombo;
   TGTab               *fTab;
   TGTextEntry         *fTxt1, *fTxt2;
   TGLayoutHints       *fL1, *fL2, *fL3, *fL4;
   TRootEmbeddedCanvas *fEc1, *fEc2;
   Int_t                fFirstEntry;
   Int_t                fLastEntry;
   Bool_t               fFillHistos;
   TH1F                *fHpx;
   TH2F                *fHpxpy;
   TList               *fCleanup;

   void            FillHistos();

public:
                   CMCalibMenu(const TGWindow *p, const TGWindow *main,
                               UInt_t w, UInt_t h,
                               UInt_t options = kMainFrame | kVerticalFrame);
   virtual        ~CMCalibMenu();

   virtual void    CloseWindow();
   virtual Bool_t  ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2);
};

#endif
E 1
