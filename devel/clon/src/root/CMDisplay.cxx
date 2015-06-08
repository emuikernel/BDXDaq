
//////////////////////////////////////////////////////////////////////////
//                                                                      //
// CMDisplay                                                            //
//                                                                      //
// Utility class to display Moller Polarimeter GUI                      //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include <TROOT.h>
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
#include "TSystem.h"

#include "CMDisplay.h"
CMDisplay *gCMDisplay;

#include "CMDaq.h"
#include "CMEpics.h"
#include "CMInputDialog.h"


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

  //printf("MyTimer::Notify\n");

  fDisplay->Event();

  return kTRUE;
}
//------------------------------------------------------------------------------

MyTimer *tt;



ClassImp(CMDisplay)

//_____________________________________________________________________________
CMDisplay::CMDisplay() : CMVirtualDisplay()
{
   m_Daq  = 0;
   m_Fruits    = 0;
}

//_____________________________________________________________________________
CMDisplay::CMDisplay(const char *title) : CMVirtualDisplay()
{
   printf("CMDisplay constructor called ...");

   gCMDisplay = this;
   printf(" done.\n");

   // set timer pointer to zero - will use it to see a current status:
   // if != 0 - we are runing, otherwise we are stoped

   tt = (MyTimer *)0;


   // Set front view by default
   m_Theta = 0;
   m_Phi   = -90;
   m_DrawAllViews  = kFALSE;
   m_DrawClusters  = kTRUE;
   m_DrawParticles = kTRUE;

   // Create display canvas
   m_Canvas = new TCanvas("Canvas", (char *)title,14,47,1130,600);
   //old version: m_Canvas->SetEditable(kIsNotEditable);

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
   /*
   char *but35 = "DisplayPtr->MoveTarget(2)";
   button = new TButton("2",but35,0.56,y-dbutton,0.67,y);
   button->SetFillColor(butcolor);
   button->Draw();
   char *but45 = "DisplayPtr->MoveTarget(3)";
   button = new TButton("3",but45,0.68,y-dbutton,0.81,y);
   button->SetFillColor(butcolor);
   button->Draw();
   char *but25 = "DisplayPtr->MoveTarget(0)";
   button = new TButton("4",but25,0.82,y-dbutton,0.95,y);
   button->SetFillColor(butcolor);
   button->Draw();
   */
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

  printf("Start Event Loop\n");

  if(tt)
  {
    printf("we are running already !!!\n");
  }
  else
  {
    if(!m_Daq)
    {
      printf("CMDisplay::StartLoop: m_Daq = 0 - can not start event loop.\n");
    }
    else
    {
      if(status = m_Daq->Start())
      {
        printf("CMDisplay::StartLoop: m_Daq->Start() returns %d\n",status);
        return;
      }
      m_Daq->Reset();
    }
    if(m_Epics) m_Epics->Reset();
    timeout = 20; // 20ms -> 50 Hz
    tt = new MyTimer(this, timeout);
    printf("timer started, time = %f sec\n",(float)timeout/1000.);
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

   if(m_Daq) m_Daq->Loop();
   if(m_Epics) m_Epics->Status();
   if(tt) tt->Reset();

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













