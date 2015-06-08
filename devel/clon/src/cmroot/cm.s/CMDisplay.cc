
//////////////////////////////////////////////////////////////////////////
//                                                                      //
// CMDisplay                                                            //
//                                                                      //
// Utility class to display Moller Polarimeter GUI                      //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include <TROOT.h>

#include <TGTab.h>
#include <TGButton.h>

#include <TButton.h>
#include <TCanvas.h>
#include <TView.h>
#include <TArc.h>
#include <TText.h>
#include <TPaveLabel.h>
#include <TPaveText.h>
#include <TList.h>
#include <TDiamond.h>
#include <TNode.h>
#include <TTUBE.h>
#include <TBRIK.h>
#include <TMath.h>
#include <X3DBuffer.h>
#include <TPolyLine3D.h>
#include <TPolyMarker3D.h>
#include <TTimer.h>
#include <TGFileDialog.h>
#include <TGWindow.h>
#include "TSystem.h"

#include "CMDisplay.h"
CMDisplay *gCMDisplay;

#include "CMDaq.h"
#include "CMEpics.h"
#include "CMInputDialog.h"


//for reloaded ExecuteEvent only !!!
#include "CMHist.h"
extern CMHist *gCMHist;

#include "CMTestSetup.h"
extern CMTestSetup *gCMTestSetup;


/*
//---- our TButton class

class CMTButton: public TButton
{

public:

CMTButton::CMTButton(const char *title, const char *method, Double_t x1, Double_t y1,Double_t x2, Double_t  y2)
           :TButton(const char *title, const char *method, Double_t x1, Double_t y1,Double_t x2, Double_t  y2)
{
   // Button normal constructor.
   //
   //   Note that the button coordinates x1,y1,x2,y2 are always in the range [0,1]

}


  void ExecuteEvent(Int_t event, Int_t x, Int_t y);

};
void CMTButton::ExecuteEvent(Int_t event, Int_t x, Int_t y)
{
  printf("CMTButton::ExecuteEvent: %d %d %d\n",event,x,y);
}
*/


// reload TButton's ExecuteEvent
void TButton::ExecuteEvent(Int_t event, Int_t px, Int_t py)
{
  //printf("TButton::ExecuteEvent: %d %d %d\n",event,px,py);

   // Execute action corresponding to one event.
   //
   //  This member function is called when a Button object is clicked.

   //check case where pressing a button deletes itself
   if (!TestBit(kNotDeleted)) return;

   if (IsEditable()) {
      TPad::ExecuteEvent(event,px,py);
      return;
   }

   TPad *cdpad = (TPad*)gROOT->GetSelectedPad();
   HideToolTip(event);

   switch (event) {

   case kMouseEnter:
      TPad::ExecuteEvent(event,px,py);
      break;

   case kButton1Down:
      SetBorderMode(-1);
      fFocused=1;
      Modified();
      Update();
      break;

   case kMouseMotion:

      break;

   case kButton1Motion:
      if (px<XtoAbsPixel(1) && px>XtoAbsPixel(0) &&
          py<YtoAbsPixel(0) && py>YtoAbsPixel(1)) {
         if (!fFocused) {
            SetBorderMode(-1);
            fFocused=1;
            Modified();
            GetCanvas()->Modified();
            Update();
         }
      } else if (fFocused) {
         SetBorderMode(1);
         fFocused=0;
         Modified();
         GetCanvas()->Modified();
         Update();
      }
      break;

   case kButton1Up:
      SetCursor(kWatch);
      if (fFocused) {
         if (cdpad) cdpad->cd();


		 printf("process >%s< here !!!\n",GetMethod());


         //gROOT->ProcessLine(GetMethod());
         if(!strcmp(GetMethod(),"DisplayPtr->Download()")) gCMDisplay->Download();
         else if(!strcmp(GetMethod(),"DisplayPtr->Prestart()")) gCMDisplay->Prestart();
         else if(!strcmp(GetMethod(),"DisplayPtr->StartLoop(1000)")) gCMDisplay->StartLoop(1000);
         else if(!strcmp(GetMethod(),"DisplayPtr->StopLoop()")) gCMDisplay->StopLoop();
         else if(!strcmp(GetMethod(),"DisplayPtr->Pause()")) gCMDisplay->Pause();
         else if(!strcmp(GetMethod(),"DisplayPtr->Resume()")) gCMDisplay->Resume();
         else if(!strcmp(GetMethod(),"HistPtr->PreviousView()")) gCMHist->PreviousView();
         else if(!strcmp(GetMethod(),"HistPtr->NextView()")) gCMHist->NextView();
         else if(!strcmp(GetMethod(),"TestSetupPtr->PreviousView()")) gCMTestSetup->PreviousView();
         else if(!strcmp(GetMethod(),"TestSetupPtr->NextView()")) gCMTestSetup->NextView();
         else if(!strcmp(GetMethod(),"TestSetupPtr->Change1()")) gCMTestSetup->Change1();
         else if(!strcmp(GetMethod(),"TestSetupPtr->Change2()")) gCMTestSetup->Change2();
         else if(!strcmp(GetMethod(),"TestSetupPtr->Menu()")) gCMTestSetup->Menu();
         else printf("unknown command, do nothing\n");

      }
      //check case where pressing a button deletes itself
      if (!TestBit(kNotDeleted)) return;
      SetBorderMode(1);
      Modified();
      Update();
      SetCursor(kCross);
      break;
   }
}





//---- Timeout timer -----------------------------------------------------------
//
// This utility class is only used via CMDisplay::StartLoop(Int_t timeout)
//

class MyTimer : public TTimer
{
private:
  CMDisplay   *fDisplay;   //display to which this timer belongs

public:
  MyTimer(CMDisplay *m, Long_t ms);
  Bool_t  Notify();
};

MyTimer::MyTimer(CMDisplay *m, Long_t ms)
              : TTimer(ms, kTRUE)
{
  fDisplay = m;
  gSystem->AddTimer(this);
}

Bool_t MyTimer::Notify()
{
  // That function will be called in case of timeout INSTEAD OF
  // standart Notify() function from TTimer class

  //printf("MyTimer::Notify ?\n");fflush(stdout);
  //sleep(3);
  //printf("MyTimer::Notify !\n");fflush(stdout);

  fDisplay->Event();

  return kTRUE;
}
//------------------------------------------------------------------------------

MyTimer *tt;



ClassImp(CMDisplay)

//_____________________________________________________________________________
CMDisplay::CMDisplay(const char *title) : CMVirtualDisplay()
{
   printf("CMDisplay constructor called ...");

   gCMDisplay = this;
   printf(" done.\n");

   // set timer pointer to zero - will use it to see a current status:
   // if != 0 - we are runing, otherwise we are stopped

   tt = (MyTimer *)0;

   // have to cleanup all global pointers here, because later we may
   // check if the are zero or not and use it to see if for example
   // m_Epics was activated in startup script
   m_Daq  = 0;
   m_Epics = 0;
   m_Fruits    = 0;
   //printf("m_Epics=0x%08x\n",m_Epics);fflush(stdout);

   // Set front view by default
   m_Theta = 0;
   m_Phi   = -90;
   m_DrawAllViews  = kFALSE;
   m_DrawClusters  = kTRUE;
   m_DrawParticles = kTRUE;



   // Create display canvas
   m_Canvas = new TCanvas("Canvas", (char *)title,14,47,1130,600);


   /*
   m_MainFrame = new TGMainFrame(gClient->GetRoot(), 200, 400, kMainFrame | kVerticalFrame);
   //m_MainFrame = new TGMainFrame(m_Canvas, 200, 400, kVerticalFrame);
   m_MainFrame->Draw();
   return;
   */





   // Create main display pad
   m_Pad = new TPad("viewpad", "DAQ display",0.15,0.001,0.999,0.999);
   m_Pad->Draw();
   m_Pad->Modified();
   m_Pad->SetFillColor(0);
   m_Pad->SetBorderSize(2);

   // Create user interface control pad
   DisplayButtons();
   m_Canvas->cd();

   // Create status display pad
   Float_t dxtr = 0.15;
   Float_t dytr = 0.45;
   m_StatPad = new TPad("StatPad","status pad",0,0,dxtr,dytr);
   m_StatPad->Draw();
   m_StatPad->Modified();
   m_StatPad->SetFillColor(0);
   m_StatPad->SetBorderSize(2);

// START_GEOMETRY

   m_Brik = new TBRIK("BRIK","BRIK"," ", 30, 30, 30);
   m_Nodsda = new TNode("nodsda","SDA outline","BRIK",0,0,0," ");
   m_Nodsda->SetLineColor(7);

// END_GEOMETRY

   m_Canvas->cd();
   m_Canvas->Update();
}


//_____________________________________________________________________________
CMDisplay::~CMDisplay()
{
   m_Daq->Clear();
}

//_____________________________________________________________________________
void CMDisplay::Clear(Option_t *)
{
//    Delete graphics temporary objects

   m_Fruits->Delete();

}

/*
//_____________________________________________________________________________
void CMDisplay::DisplayButtons()
{
//    Create the user interface buttons

  TGFrame *m_Button;
  m_Button = new TGFrame(gCMDisplay);
  m_Button->Draw();
  //this->AddFrame(new TGFrame(this->GetContainer(),
  //                            32, 32, 0, 1),
  //                            new TGLayoutHints(kLHintsExpandY | kLHintsRight));


  //m_Buttons = new TPad("buttons", "newpad",0,0.45,0.15,1);
  //m_Buttons->Draw();
  //m_Buttons->SetFillColor(38);
  //m_Buttons->SetBorderSize(2);
  //m_Buttons->cd();
}
*/


//_____________________________________________________________________________
void CMDisplay::DisplayButtons()
{
//    Create the user interface buttons

   m_Buttons = new TPad("buttons", "newpad",0,0.45,0.15,1);
   m_Buttons->Draw();
   m_Buttons->SetFillColor(38);
   m_Buttons->SetBorderSize(2);
   m_Buttons->cd();

   Int_t butcolor = 33;
   Float_t dbutton = 0.08;
   Float_t y  = 0.96;
   Float_t dy = 0.014;
   Float_t x0 = 0.05;
   Float_t x1 = 0.95;

   TButton *button;
   char *but1 = "DisplayPtr->Download()";
   button = new TButton("Download",but1,x0,y-dbutton,x1,y);
   button->SetFillColor(38);
   button->Draw();
   /*
   TButton *button;
   char *but1 = "DisplayPtr->Download()";
   button = new TButton("Download",but1,x0,y-dbutton,x1,y);
   button->SetFillColor(38);
   button->Draw();
   */
   y -= dbutton +dy;
   char *but2 = "DisplayPtr->Prestart()";
   button = new TButton("Prestart",but2,x0,y-dbutton,x1,y);
   button->SetFillColor(38);
   button->Draw();

   y -= dbutton +dy;
   //   char *but3 = "2*2";
   char *but3 = "DisplayPtr->StartLoop(1000)";
   button = new TButton("Go",but3,x0,y-dbutton,x1,y);
   button->SetFillColor(butcolor);
   button->Draw();

   y -= dbutton +dy;
   //   char *but4 = "2*2";
   char *but4 = "DisplayPtr->StopLoop()";
   button = new TButton("End",but4,x0,y-dbutton,x1,y);
   button->SetFillColor(butcolor);
   button->Draw();

   y -= dbutton +dy;
   //   char *but5 = "2*2";
   char *but5 = "DisplayPtr->Pause()";
   button = new TButton("Pause",but5,x0,y-dbutton,0.49,y);
   button->SetFillColor(butcolor);
   button->Draw();
   char *but15 = "DisplayPtr->Resume()";
   button = new TButton("Resume",but15,0.51,y-dbutton,0.95,y);
   button->SetFillColor(butcolor);
   button->Draw();

   //char *but35 = "DisplayPtr->MoveTarget(2)";
   //button = new TButton("2",but35,0.56,y-dbutton,0.67,y);
   //button->SetFillColor(butcolor);
   //button->Draw();
   //char *but45 = "DisplayPtr->MoveTarget(3)";
   //button = new TButton("3",but45,0.68,y-dbutton,0.81,y);
   //button->SetFillColor(butcolor);
   //button->Draw();
   //char *but25 = "DisplayPtr->MoveTarget(0)";
   //button = new TButton("4",but25,0.82,y-dbutton,0.95,y);
   //button->SetFillColor(butcolor);
   //button->Draw();

   y -= dbutton +dy;
   //   char *but6 = "2*2";
   char *but6 = "DisplayPtr->MoveTarget(12)";
   button = new TButton("5",but6,x0,y-dbutton,0.75,y);
   button->SetFillColor(butcolor);
   button->Draw();
   char *but16 = "DisplayPtr->MoveTarget(12)";
   button = new TButton("6",but16,0.76,y-dbutton,0.84,y);
   button->SetFillColor(butcolor);
   button->Draw();
   char *but26 = "DisplayPtr->MoveTarget(13)";
   button = new TButton("7",but26,0.85,y-dbutton,x1,y);
   button->SetFillColor(butcolor);
   button->Draw();

   y -= dbutton +dy;
   char *but7 = "DisplayPtr->FileSelect()";
   //char *but7 = "m_Epics->SetValue("kepco_command",-1.0)";
   button = new TButton("Input from ...",but7,x0,y-dbutton,x1,y);
   button->SetFillColor(butcolor);
   button->Draw();

   y -= dbutton +dy;
   char *but8 = "DisplayPtr->ChangeClient()";
   button = new TButton("Change Client",but8,x0,y-dbutton,x1,y);
   button->SetFillColor(38);
   button->Draw();

   // display logo
   diamond = new TDiamond(0.05,0.015,0.95,0.22);
   diamond->SetFillColor(6);
   diamond->SetTextAlign(22);
   diamond->SetTextColor(5);
   diamond->SetTextSize(0.13);
   diamond->Draw();
   diamond->AddText("CLAS");
}


//_____________________________________________________________________________
void CMDisplay::Download()
{
  printf("Download: m_Daq=0x%08x\n",m_Daq);
  m_Daq->Download();
}

//_____________________________________________________________________________
void CMDisplay::FileSelect()
{
  //
  char title[1024];

  printf("FileSelect() reached\n");
  const char *file = OpenFileDialog();
  printf("FileSelect(): file>%s<\n",file);
  m_Daq->SetFile(file);

  // set Canvas title
  this->SetTitle(file);
}

//_____________________________________________________________________________
void CMDisplay::ChangeClient()
{
  printf("ChangeClient() is dummy now\n");
  return;


  m_Daq->ChangeClient(dtext);
  //printf("dtext >%s<\n",dtext);

  m_Buttons->cd();
  diamond->Delete();
  diamond = new TDiamond(0.05,0.015,0.95,0.22);
  diamond->SetFillColor(6);
  diamond->SetTextAlign(22);
  diamond->SetTextColor(5);
  diamond->SetTextSize(0.13);
  diamond->Draw();
  diamond->AddText(dtext);

}

//_____________________________________________________________________________
void CMDisplay::SetTitle(const char *datasrc)
{
  char title[1024];

printf("CMDisplay::SetTitle reached\n");
  sprintf(title,"DATA SOURCE: %s",datasrc);
  m_Canvas->SetTitle(title);
}

//_____________________________________________________________________________
void CMDisplay::Prestart()
{
  m_Daq->Prestart();
}

//_____________________________________________________________________________
void CMDisplay::Pause()
{
  m_Daq->Pause();
}

//______________________________________________________________________________
void CMDisplay::Resume()
{
  m_Daq->Resume();
}


//______________________________________________________________________________
void CMDisplay::StartLoop(Long_t timeout)
{
  Int_t status;

  //printf("Start Event Loop ?\n");fflush(stdout);
  //sleep(3);
  //printf("Start Event Loop !\n");fflush(stdout);

  if(tt)
  {
    printf("we are running already !!!\n");
  }
  else
  {
    if(!m_Daq)
    {
      printf("CMDisplay::StartLoop: m_Daq = 0 - can not start event loop.\n");
      fflush(stdout);
    }
    else
    {


      printf("CMDisplay::StartLoop: calls m_Daq->Start()\n");fflush(stdout);
      status = m_Daq->Start();
      printf("CMDisplay::StartLoop: m_Daq->Start() returns %d\n",status);
      fflush(stdout);

      if(status)
      {
        printf("CMDisplay::StartLoop: m_Daq->Start() returns %d\n",status);
        return;
      }
      printf("CMDisplay::StartLoop: calls m_Daq->Reset()\n");fflush(stdout);
      m_Daq->Reset();
      printf("CMDisplay::StartLoop: m_Daq->Reset() done\n");fflush(stdout);
    }
	printf("111: m_Epics=%x\n",m_Epics);fflush(stdout);
    if(m_Epics) m_Epics->Reset();
	//printf("112\n");fflush(stdout);
    timeout = 20; // 20ms -> 50 Hz
	//printf("113\n");fflush(stdout);
    printf("CMDisplay::StartLoop: calls MyTimer\n");fflush(stdout);
	//printf("114\n");fflush(stdout);
    tt = new MyTimer(this, timeout);
	//printf("115\n");fflush(stdout);
    printf("CMDisplay::StartLoop: timer started, time = %f sec\n",(float)timeout/1000.);fflush(stdout);
  }
}

//______________________________________________________________________________
void CMDisplay::StopLoop()
{

  if(tt)
  {
    printf("Stop Event Loop\n");
    if(tt) delete tt;
    tt = (MyTimer *)0;
    if(m_Daq) m_Daq->Stop();
  }
  else
  {
    printf("We are stopped already !!!\n");
  }
}


//______________________________________________________________________________
void CMDisplay::Event()
{
   // process one event

  //printf("CMDisplay::Event() reached\n");fflush(stdout);
  //sleep(3);
  //printf("CMDisplay::Event() 1\n");fflush(stdout);
  if(m_Daq) m_Daq->Loop();
  //printf("CMDisplay::Event() 2\n");fflush(stdout);
  if(m_Epics) m_Epics->Status();
  //printf("CMDisplay::Event() 3\n");fflush(stdout);
  if(tt) tt->Reset();
  //printf("CMDisplay::Event() done\n");fflush(stdout);

}

//______________________________________________________________________________
void CMDisplay::VoltageOn()
{
  printf("Turning on all needed devices ...\n");
  m_Epics->SetValue("B_hv_BM_MOLLER_L.DVI",-1550.0);
  m_Epics->SetValue("B_hv_BM_MOLLER_R.DVI",-1410.0);
  m_Epics->SetValue("kepco_command",1.0);
  m_Epics->SetValue("DYNABRESET",1.0);
  m_Epics->SetValue("DYNABON",1.0);
  m_Epics->SetValue("DYNACRESET",1.0);
  m_Epics->SetValue("DYNACON",1.0);
  m_Epics->SetValue("moller_target_command",1.0); // calibrated target
  m_Epics->GetQuadCurrents(); // sets currents within subroutine
}

//______________________________________________________________________________
void CMDisplay::VoltageOff()
{
  int i;
  float x;
  printf("Turning Off devices ...\n");
  m_Epics->SetValue("DYNABSETI",0.0);
  m_Epics->SetValue("DYNACSETI",0.0);
  m_Epics->SetValue("kepco_command",0.0);
  m_Epics->SetValue("moller_target_command",12.0);
  for(i=0;i<100000000;i++) x=sqrt(2.);
  m_Epics->SetValue("DYNABOFF",1.0);
  m_Epics->SetValue("DYNACOFF",1.0);
}

//______________________________________________________________________________
void CMDisplay::MoveTarget(int pos)
{
  //  printf("Moving Target to Position %d\n",pos);
  if(pos==0)
    {
      printf("inserting EMPTY Target\n");
      m_Epics->SetValue("moller_target_command",12.0);
    }
  if(pos==1)
    {
      printf("inserting LEFT Target\n");
      m_Epics->SetValue("moller_target_command",11.0);
    }
  if(pos==2)
    {
      printf("inserting RIGHT Target\n");
      m_Epics->SetValue("moller_target_command",13.0);
    }
  if(pos==3)
    {
      printf("inserting AL Target\n");
      m_Epics->SetValue("moller_target_command",14.0);
    }
  if(pos==12)
    {
      printf("Setting target magnet to POSITIVE field\n");
      m_Epics->SetValue("kepco_command",1.0);
    }
  if(pos==13)
    {
      printf("Setting target magnet to NEGATIVE field\n");
      m_Epics->SetValue("kepco_command",-1.0);
    }

}


//_____________________________________________________________________________
//--- Input Dialog Functions --------------------------------------------------

char *CMDisplay::OpenFileDialog()
{
  // Prompt for file to be opened. Depending on navigation in
  // dialog the current working directory can be changed.
  // The returned file name is always with respect to the
  // current directory.

  const char *gOpenAsTypes[] = {
     "ET Systems",   "et_sys_*",
     "FPACK files",  "*.A*",
     "Macro files",  "*.C",
     "ROOT files",   "*.root",
     "PostScript",   "*.ps",
     "Encapsulated PostScript", "*.eps",
     "Gif files",    "*.gif",
     "All files",    "*",
      0,              0
  };

  static TGFileInfo fi;
  fi.fFileTypes = (const char **) gOpenAsTypes; // does not compiled without  cast !?
  new TGFileDialog(gClient->GetRoot(), gClient->GetRoot(), kFDOpen, &fi);

  return(fi.fFilename);
}

char *CMDisplay::SaveFileDialog()
{
  // Prompt for file to be saved. Depending on navigation in
  // dialog the current working directory can be changed.
  // The returned file name is always with respect to the
  // current directory.

  const char *gSaveAsTypes[] = {
     "Macro files",  "*.C",
     "ROOT files",   "*.root",
     "PostScript",   "*.ps",
     "Encapsulated PostScript", "*.eps",
     "Gif files",    "*.gif",
     "All files",    "*",
      0,              0
  };

  static TGFileInfo fi;
  fi.fFileTypes = (const char **) gSaveAsTypes; // does not compiled without  cast !?
  new TGFileDialog(gClient->GetRoot(), gClient->GetRoot(), kFDSave, &fi);

  return(fi.fFilename);
}

char *CMDisplay::GetStringDialog(const char *prompt, const char *defval)
{
  // Prompt for string. The typed in string is returned.
  static char answer[128];

  new CMInputDialog(prompt, defval, answer);

  return(answer);
}

Int_t CMDisplay::GetIntegerDialog(const char *prompt, Int_t defval)
{
  // Prompt for integer. The typed in integer is returned.
  static char answer[32];

  char defv[32];
  sprintf(defv, "%d", defval);

  new CMInputDialog(prompt, defv, answer);

  return(atoi(answer));
}

Float_t CMDisplay::GetFloatDialog(const char *prompt, Float_t defval)
{
  // Prompt for float. The typed in float is returned.
  static char answer[32];

  char defv[32];
  sprintf(defv, "%f", defval);

  new CMInputDialog(prompt, defv, answer);

  return(atof(answer));
}













