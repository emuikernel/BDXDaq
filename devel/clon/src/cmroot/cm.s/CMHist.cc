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
#include <TRandom.h>
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




//_____________________________________________________________________________
CMHist::CMHist(Option_t *fname, CMDisplay *mDisplay) : CMClient(fname, mDisplay)
{
// Create one CMHist object

  gCMHist = this;

  hbook = new CMHbook();
  viewer = new CMViewer(fname);
  iview = 0; // use first view at startup

  callprlib = 0;
  calltrlib = 0;
  downloaded = 0;

  strcpy(filename,fname);
  printf("CMHist: Config File >%s<, gCMHist=0x%08x\n",filename,gCMHist);

}


//_____________________________________________________________________________
void CMHist::Download()
{
//
  // read config file
  nview = viewer->ReadConfig(filename, hbook);

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

  this->Reset();

  if(callprlib || calltrlib)
  {
    //bosInit(wcs_.jw,JNDIMB);
    //cminit();
  }

  prestarted = 1;
}



//_____________________________________________________________________________
void CMHist::Go(Int_t *iw)
{
//
  printf("CMHist::Go() reached\n");fflush(stdout);

  time0 = time(NULL);
  printf("CMHist::Go() done\n");fflush(stdout);
}


//_____________________________________________________________________________
void CMHist::End(Int_t *iw)
{
//
  Int_t ret;

  printf("CMHist::End() reached\n");fflush(stdout);
  if(callprlib || calltrlib)
  {
    ret = hbook->HisToRoot();
    //dcerun(iw);
    //dclast(iw);
  }

}


//_____________________________________________________________________________
int CMHist::Event(Int_t *iw)
{
// one event processing

  //printf("CMHist::Event() reached\n");fflush(stdout);
  Int_t ret, tmp;

  nevents ++;

  if(callprlib || calltrlib)
  {
    //printf("CMHist: Event() cmloop\n");
    //cmloop(iw,0,0,0,&ret); // process one event
    ret = 0;
    if((callprlib || calltrlib) && !(nevents%100)) ret = hbook->HisToRoot();
if(!(nevents%10)) ret = 1;
    //printf("nevents=%d ret=%d\n",nevents,ret);
  }
  else
  {
    // read bos banks and create Root histograms
    // then convert desired histograms to timeline plots

    //printf("CMHist: Event()\n");fflush(stdout);
    ret = hbook->BosToRoot(iw);
    //printf("CMHist: Event() done, ret=%d\n",ret);fflush(stdout);
  }

  return(ret);
}



//____________________________________________________________________________
void CMHist::Hist()
{
// Create views

  Int_t i;
  printf("CMHist::Hist(): nview=%d, calling ViewCreate()\n",nview);fflush(stdout);
  for(i=0; i<nview; i++) viewer->ViewCreate(i, hbook);
}



//____________________________________________________________________________
void CMHist::Update()
{
// update drawing

  Int_t time1, ret;

  printf("CMHist::Update() reached\n");fflush(stdout);
  m_Pad->cd();
  m_Pad->GetListOfPrimitives()->Clear("nodelete");
  printf("11\n");
  viewer->ViewPlot(iview);
  printf("12\n");

  time1 = time(NULL)-time0;
  printf("13\n");
  //if(time1 > 0)
  //{
  //  printf("Event rate: overall %5.0f events/sec, useful %5.0f events/sec\n",
  //  ((Float_t)nevents)/((Float_t)time1),((Float_t)nevents1)/((Float_t)time1));
  //}

}


//_____________________________________________________________________________
void CMHist::Status()
{
// check status and update status pad

  Int_t i;

  printf("CMHist::Status reached\n");

  //if(callprlib) m_Arc2->SetFillColor(kGreen); else m_Arc2->SetFillColor(kRed);
  //if(calltrlib) m_Arc3->SetFillColor(kGreen); else m_Arc3->SetFillColor(kRed);
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

  hbook->Reset();

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

