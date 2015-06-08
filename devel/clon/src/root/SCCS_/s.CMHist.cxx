h11486
s 00004/00005/00384
d D 1.6 03/04/17 17:01:05 boiarino 7 6
c *** empty log message ***
e
s 00001/00001/00388
d D 1.5 02/05/02 14:48:36 boiarino 6 5
c nothing
c 
e
s 00006/00003/00383
d D 1.4 02/03/26 15:18:08 boiarino 5 4
c *** empty log message ***
e
s 00005/00005/00381
d D 1.3 01/12/04 13:55:16 boiarino 4 3
c comment out printfs
c 
e
s 00004/00001/00382
d D 1.2 01/11/28 23:22:17 boiarino 3 1
c minor
c 
e
s 00000/00000/00000
d R 1.2 01/11/19 16:44:45 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 clas/root/CMHist.cxx
e
s 00383/00000/00000
d D 1.1 01/11/19 16:44:44 boiarino 1 0
c date and time created 01/11/19 16:44:44 by boiarino
e
u
U
f e 0
t
T
I 1
//////////////////////////////////////////////////////////////////////////
//                                                                      //
//  CMHist                                                               //
//                                                                      //
//  Clas Monitor Histogramming                                          //
//                                                                      //
//////////////////////////////////////////////////////////////////////////
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>


#include <TROOT.h>
D 5
#include "TRandom.h"
E 5
I 5
#include <TRandom.h>
E 5
#include <TButton.h>
#include <TLine.h>
#include <TArc.h>


#include "bos.h"
#include "bosio.h"
#include "bosfun.h"
#include "etbosio.h"


#include "CMHist.h"
CMHist *gCMHist; // to use it in buttons


ClassImp(CMHist)


// fast reconstruction stuff
#include "dclib.h"
#include "wcs.h"


//_____________________________________________________________________________
CMHist::CMHist(Option_t *fname) : CMClient()
{
// Create one CMHist object

  gCMHist = this;

  hbook = new CMHbook();
  viewer = new CMViewer(fname);

  callprlib = 0;
  calltrlib = 0;
  downloaded = 0;

  strcpy(filename,fname);
  printf("CMHist: Config File >%s<\n",filename);

}


//_____________________________________________________________________________
void CMHist::Download()
{
//
  // read config file
D 5
  nview = viewer->ReadConfig(filename);
E 5
I 5
  nview = viewer->ReadConfig(filename, hbook);
E 5

  this->Hist();
  this->Buttons();

  downloaded = 1;
}


//_____________________________________________________________________________
void CMHist::Prestart(Int_t *iw)
{
//
  Int_t i, j;

  nevents = 0;
  nevents1 = 0;

  Reset();

  if(callprlib || calltrlib)
  {
    bosInit(wcs_.jw,JNDIMB);
    cminit();
  }

  prestarted = 1;
}



//_____________________________________________________________________________
void CMHist::Go(Int_t *iw)
{
//

  time0 = time(NULL);
}


//_____________________________________________________________________________
void CMHist::End(Int_t *iw)
{
//
  Int_t ret;

  if(callprlib || calltrlib)
  {
    ret = hbook->HisToRoot();
    dcerun(iw);
    dclast(iw);
  }

}


//_____________________________________________________________________________
int CMHist::Event(Int_t *iw)
{
// one event processing

D 5
  Int_t ret;
E 5
I 5
  Int_t ret, tmp;
E 5

  nevents ++;

D 3
  //printf("CMHist::Event() reached\n");
E 3
I 3
  //printf("CMHist: Event() reached\n");
E 3
  if(callprlib || calltrlib)
  {
I 3
    //printf("CMHist: Event() cmloop\n");
E 3
    cmloop(iw,0,0,0,&ret); // process one event
    ret = 0;
    if((callprlib || calltrlib) && !(nevents%100)) ret = hbook->HisToRoot();
if(!(nevents%10)) ret = 1;
    //printf("nevents=%d ret=%d\n",nevents,ret);
  }
  else
  {
I 5
    // read bos banks and create Root histograms
    // then convert desired histograms to timeline plots

E 5
I 3
D 7
    //printf("CMHist: Event()\n");
E 7
I 7
    //printf("CMHist: Event()\n");fflush(stdout);
E 7
E 3
    ret = hbook->BosToRoot(iw);
I 3
D 6
    //printf("CMHist: Event() done\n");
E 6
I 6
D 7
    //printf("CMHist: Event() done, ret=%d\n",ret);
E 7
I 7
    //printf("CMHist: Event() done, ret=%d\n",ret);fflush(stdout);
E 7
E 6
E 3
  }

  return(ret);
}



D 7
//_____________________________________________________________________________
E 7
I 7
//____________________________________________________________________________
E 7
void CMHist::Hist()
{
// Create views

  Int_t i;
  printf("Hist(): nview=%d\n",nview);
  for(i=0; i<nview; i++) viewer->ViewCreate(i, hbook);
D 7

E 7
}



D 7
//_____________________________________________________________________________
E 7
I 7
//____________________________________________________________________________
E 7
void CMHist::Update()
{
// update drawing

  Int_t time1, ret;

  m_Pad->cd();
  m_Pad->GetListOfPrimitives()->Clear("nodelete");
  viewer->ViewPlot(iview);

  time1 = time(NULL)-time0;
D 4
  if(time1 > 0)
  {
    printf("Event rate: overall %5.0f events/sec, useful %5.0f events/sec\n",
    ((Float_t)nevents)/((Float_t)time1),((Float_t)nevents1)/((Float_t)time1));
  }
E 4
I 4
  //if(time1 > 0)
  //{
  //  printf("Event rate: overall %5.0f events/sec, useful %5.0f events/sec\n",
  //  ((Float_t)nevents)/((Float_t)time1),((Float_t)nevents1)/((Float_t)time1));
  //}
E 4

}


//_____________________________________________________________________________
void CMHist::Status()
{
// check status and update status pad

  Int_t i;

  if(callprlib) m_Arc2->SetFillColor(kGreen); else m_Arc2->SetFillColor(kRed);
  if(calltrlib) m_Arc3->SetFillColor(kGreen); else m_Arc3->SetFillColor(kRed);
  if(downloaded) m_Arc4->SetFillColor(kGreen); else m_Arc4->SetFillColor(kRed);

  for(i=0; i<nview; i++) m_Arc1[i]->SetFillColor(kYellow);
  m_Arc1[iview]->SetFillColor(kGreen);

  m_StatPad->Modified();

  this->Update();

}


//_____________________________________________________________________________
void CMHist::ChangePrlib()
{
// (un)set callprlib

  if(callprlib == 0)
  {
    callprlib = 1;
  }
  else
  {
    callprlib = 0;
  }

  printf("CMHist::ChangePrlib(): set callprlib=%d\n",callprlib);

  Status();

}


//_____________________________________________________________________________
void CMHist::ChangeTrlib()
{
// (un)set calltrlib

  if(calltrlib == 0)
  {
    calltrlib = 1;
  }
  else
  {
    calltrlib = 0;
  }

  printf("CMHist::ChangeTrlib(): set calltrlib=%d\n",calltrlib);

  Status();

}


//_____________________________________________________________________________
void CMHist::Buttons()
{
// Create the buttons

  printf("CMHist::Buttons reached, m_StatPad=0x%08x\n",m_StatPad);

  Int_t i;
  Float_t dx1;
  Float_t dy1;
  Float_t& a=dx1;
  Float_t& b=dy1;
  Float_t xt;
  Float_t xxt;
  Float_t yt;
  Float_t dyt;
  Float_t ddyt;
  Float_t xarc;
  Float_t rarc;
  TText *t = new TText();


  m_StatPad->cd();


  Float_t dxtr;
  Float_t dytr;

  //Float_t& c=dxtr;
  //Float_t& d=dytr;
  //m_StatPad->GetRange(a,b,c,d);
  //printf("dxtr,dytr = %f %f - %f %f\n",c,d,dxtr,dytr);

  dxtr = 0.15;
  dytr = 0.45;
  m_StatPad->Range(0,0,dxtr,dytr);

  xt   = 0.4*dxtr;
  xxt  = 0.5*dxtr;
  yt   = 0.88*dytr;
  dyt  = 0.12*dytr;
  ddyt = 0.4*dyt;
  xarc = 0.8*dxtr;
  rarc = 0.1*dyt;

  Int_t butcolor = 33;
  Float_t dbutton = 0.08;
  Float_t y  = 0.96;
  Float_t dy = 0.014;
  Float_t x0 = 0.01;
  Float_t x01 = 0.54;
  Float_t x02 = 0.58;
  Float_t x1 = 0.98;

  TButton *button; // button coordinates is always in range [0,1]

  char *but1 = "HistPtr->PreviousView()";
  button = new TButton("Previous View",but1,x0,y-dbutton,x01,y);
  button->SetFillColor(butcolor);
  button->Draw();
  char *but2 = "HistPtr->NextView()";
  button = new TButton("Next View",but2,x02,y-dbutton,x1,y);
  button->SetFillColor(butcolor);
  button->Draw();

  Float_t darc = 1./(Float_t)(NVIEW+1);
  Float_t ddarc = darc;
  for(i=0; i<NVIEW; i++)
  {
    m_Arc1[i] = new TArc(dxtr*ddarc,(y-dbutton-dy*3.)*dytr,rarc);
    m_Arc1[i]->Draw();
    ddarc += darc;
  }

  y -= dbutton + dy*6.;
  char *but3 = "HistPtr->ChangePrlib()";
  button = new TButton("Road Finding (ON/OFF)",but3,x0,y-dbutton,x1,y);
  button->SetFillColor(38);
  button->Draw();
  m_Arc2   = new TArc(dxtr/2.,(y-dbutton-dy*3.)*dytr,rarc);
  m_Arc2->Draw();

  y -= dbutton + dy*6.;
  char *but4 = "HistPtr->ChangeTrlib()";
  button = new TButton("Track Fiting (ON/OFF)",but4,x0,y-dbutton,x1,y);
  button->SetFillColor(38);
  button->Draw();
  m_Arc3   = new TArc(dxtr/2.,(y-dbutton-dy*3.)*dytr,rarc);
  m_Arc3->Draw();

  y -= dbutton + dy*6.;
  char *but5 = "HistPtr->FitSave()";
  button = new TButton("Save Fitting",but5,x0,y-dbutton,x1,y);
  button->SetFillColor(butcolor);
  button->Draw();
  m_Arc4   = new TArc(dxtr/2.,(y-dbutton-dy*3.)*dytr,rarc);
  m_Arc4->Draw();

  Status();

}


//_____________________________________________________________________________
void CMHist::Reset()
{
// Reset histograms

  Int_t i;

  hbook->Clean();

}


//_____________________________________________________________________________
void CMHist::Menu()
{
  //fMenu = new TestMainFrame(gClient->GetRoot(), 400, 220);
  fDialog = new TestDialog(gClient->GetRoot(), gClient->GetRoot(), 400, 200);
  //fMsg = new TestMsgBox(gClient->GetRoot(), gClient->GetRoot(), 400, 200);
  //fSlider = new TestSliders(gClient->GetRoot(), gClient->GetRoot(), 400, 200);
}


//_____________________________________________________________________________
//void CMHist::FitSave()
//{
//// Save fit results for current view
//  m_Pad->GetListOfPrimitives()->Clear("nodelete");
//  TObjLink *lnk = m_Pad->GetListOfPrimitives()->FirstLink();
//  while(lnk)
//  {
//    lnk->GetObject()->Draw(lnk->GetOption());
//    lnk = lnk->Next();
//  }
//}

E 1
