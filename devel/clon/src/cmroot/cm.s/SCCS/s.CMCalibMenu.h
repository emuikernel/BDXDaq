h55501
s 00088/00000/00000
d D 1.1 06/10/19 15:41:10 boiarino 1 0
c date and time created 06/10/19 15:41:10 by boiarino
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
#include <TVirtualPadEditor.h>
#include <TGResourcePool.h>
#include <TGListBox.h>
#include <TGListTree.h>
#include <TGFSContainer.h>
#include <TGClient.h>
#include <TGFrame.h>
#include <TGIcon.h>
#include <TGLabel.h>
#include <TGButton.h>
#include <TGTextEntry.h>
#include <TGNumberEntry.h>
#include <TGMsgBox.h>
#include <TGMenu.h>
#include <TGCanvas.h>
#include <TGComboBox.h>
#include <TGTab.h>
#include <TGSlider.h>
#include <TGDoubleSlider.h>
#include <TGFileDialog.h>
#include <TGTextEdit.h>
#include <TGShutter.h>
#include <TGProgressBar.h>
#include <TGColorSelect.h>
#include <TRootEmbeddedCanvas.h>
#include <TCanvas.h>
#include <TColor.h>
#include <TH1.h>
#include <TH2.h>
#include <TRandom.h>
#include <TSystem.h>
#include <TSystemDirectory.h>
#include <TEnv.h>
#include <TFile.h>
#include <TKey.h>
#include <TGDockableFrame.h>
#include <TGFontDialog.h>


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
