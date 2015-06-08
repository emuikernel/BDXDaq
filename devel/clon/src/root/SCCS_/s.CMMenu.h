h59159
s 00000/00000/00000
d R 1.2 01/11/19 16:44:45 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 clas/root/CMMenu.h
e
s 00142/00000/00000
d D 1.1 01/11/19 16:44:44 boiarino 1 0
c date and time created 01/11/19 16:44:44 by boiarino
e
u
U
f e 0
t
T
I 1
#ifndef CMMenu_H
#define CMMenu_H

/////////////////////////////////////
// CMMenu.h
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


class TestMainFrame : public TGMainFrame {

private:
   TGCompositeFrame   *fStatusFrame;
   TGCanvas           *fCanvasWindow;
   TGCompositeFrame   *fContainer;
   TGTextEntry        *fTestText;
   TGButton           *fTestButton;

   TGMenuBar          *fMenuBar;
   TGPopupMenu        *fMenuFile, *fMenuTest, *fMenuHelp;
   TGPopupMenu        *fCascadeMenu, *fCascade1Menu, *fCascade2Menu;
   TGLayoutHints      *fMenuBarLayout, *fMenuBarItemLayout, *fMenuBarHelpLayout;

public:
   TestMainFrame(const TGWindow *p, UInt_t w, UInt_t h);
   virtual ~TestMainFrame();

   virtual void CloseWindow();
   virtual Bool_t ProcessMessage(Long_t msg, Long_t parm1, Long_t);

   ClassDef(TestMainFrame, 0)
};


class TestDialog : public TGTransientFrame {

private:
   TGCompositeFrame    *fFrame1, *fF1, *fF2, *fF3, *fF4, *fF5, *fF6, *fF7;
   TGButton            *fOkButton, *fCancelButton, *fStartB, *fStopB;
   TGButton            *fBtn1, *fBtn2, *fChk1, *fChk2, *fRad1, *fRad2;
   TGPictureButton     *fPicBut1;
   TGRadioButton       *fRadio1, *fRadio2;
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

   void FillHistos();

public:
   TestDialog(const TGWindow *p, const TGWindow *main, UInt_t w, UInt_t h,
               UInt_t options = kMainFrame | kVerticalFrame);
   virtual ~TestDialog();

   virtual void CloseWindow();
   virtual Bool_t ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2);
};


class TestMsgBox : public TGTransientFrame {

private:
   TGCompositeFrame     *f1, *f2, *f3, *f4, *f5;
   TGButton             *fTestButton, *fCloseButton;
   TGPictureButton      *fPictButton;
   TGRadioButton        *fR[4];
   TGCheckButton        *fC[9];
   TGGroupFrame         *fG1, *fG2;
   TGLayoutHints        *fL1, *fL2, *fL3, *fL4, *fL5, *fL6, *fL21;
   TGTextEntry          *fTitle, *fMsg;
   TGTextBuffer         *fTbtitle, *fTbmsg;
   TGLabel              *fLtitle, *fLmsg;
   GContext_t            fRedTextGC;

public:
   TestMsgBox(const TGWindow *p, const TGWindow *main, UInt_t w, UInt_t h,
              UInt_t options = kMainFrame | kVerticalFrame);
   virtual ~TestMsgBox();

   virtual void CloseWindow();
   virtual Bool_t ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2);
};


class TestSliders : public TGTransientFrame {

private:
   TGVerticalFrame   *fVframe1, *fVframe2;
   TGLayoutHints     *fBly, *fBfly1;
   TGHSlider         *fHslider1, *fHslider2;
   TGVSlider         *fVslider1;
   TGDoubleVSlider   *fVslider2;
   TGTextEntry       *fTeh1, *fTev1, *fTeh2, *fTev2;
   TGTextBuffer      *fTbh1, *fTbv1, *fTbh2, *fTbv2;

public:
   TestSliders(const TGWindow *p, const TGWindow *main, UInt_t w, UInt_t h);
   virtual ~TestSliders();

   virtual void CloseWindow();
   virtual Bool_t ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2);
};

#endif
E 1
