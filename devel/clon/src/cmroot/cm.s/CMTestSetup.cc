//////////////////////////////////////////////////////////////////////////
//                                                                      //
//  CMTestSetup                                                         //
//                                                                      //
//  CH Test Setup Histogramming                                         //
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


#include "CMTestSetup.h"
CMTestSetup *gCMTestSetup; // to use it in buttons


ClassImp(CMTestSetup)


//_____________________________________________________________________________
CMTestSetup::CMTestSetup(Option_t *fname, CMDisplay *mDisplay) : CMClient(fname, mDisplay)
{
// Create one CMTestSetup object

  gCMTestSetup = this;

  hbook = new CMHbook();
  viewer = new CMViewer(fname);
  fDialog = new CMTestSetupMenu(gClient->GetRoot(),gClient->GetRoot(),400,200);

  downloaded = 0;
  nview = 0;
  iview = 0;

  strcpy(filename,fname);
  printf("CMTestSetup: Config File >%s<\n",filename);

  slot = -1;

  // display buttons
  this->Buttons();

  downloaded=0; prestarted=0; active=0; ended=1;
}


//_____________________________________________________________________________
CMTestSetup::~CMTestSetup()
{
  delete hbook;
  delete viewer;
  delete fDialog;
}


//_____________________________________________________________________________
void CMTestSetup::Download()
{
//
  Int_t i, j, num;

  // get slot number
  if((slot = fDialog->GetSlotNumber()) == -1)
  {
    printf("CMTestSetup::Download(): you must select slot number !\n");
    return;
  }

  // set histograms
  printf("CMTestSetup::Download(): slot number %d\n",slot);fflush(stdout);
  viewer->ViewDelete(0, hbook);
  Hists(slot,/*128*/32);
  //nview = viewer->ReadConfig(filename, hbook);

  printf("CMTestSetup::Download(): nview=%d\n",nview);fflush(stdout);
  for(i=0; i<nview; i++) viewer->ViewCreate(i, hbook);
  Status();

  downloaded=1; prestarted=0; active=0; ended=0;
}

//___ 2 params: slot number and the number of channels ___
void CMTestSetup::Hists(Int_t islot, Int_t nhists)
{
  Int_t i, j, k, num;

  num = (islot-18/*6*/)*/*200*/100+1;
  k = 0;
  for(i=0; i<16/*12*/; i++)
  {
    nview = viewer->AddView(4, 2);
    for(j=0; j<8; j++)
    {
	  printf("add hist %d to view %d\n",num,nview-1);
      viewer->AddHist(hbook, num++, nview-1, 0);
      if(k++ > nhists)
      {
        printf("CMTestSetup::Hists(): 1> %d nviews, %d histograms\n",nview,k);
        return;
	  }
    }
  }

  printf("CMTestSetup::Hists(): 2> %d nviews, %d histograms\n",nview,k);
}

//_____________________________________________________________________________
void CMTestSetup::Prestart(Int_t *iw)
{
//
  Int_t i, j;

  nevents = 0;
  nevents1 = 0;

  Reset();

  downloaded=0; prestarted=1; active=0; ended=0;
}



//_____________________________________________________________________________
void CMTestSetup::Go(Int_t *iw)
{
//

  time0 = time(NULL);
  downloaded=0; prestarted=0; active=1; ended=0;
}


//_____________________________________________________________________________
void CMTestSetup::End(Int_t *iw)
{
//
  Int_t ret;

  downloaded=0; prestarted=0; active=0; ended=1;
}


//_____________________________________________________________________________
int CMTestSetup::Event(Int_t *iw)
{
// one event processing

  Int_t ret, tmp;

  nevents ++;

  //printf("CMTestSetup: Event() reached\n");

  // read bos banks and create Root histograms
  // then convert desired histograms to timeline plots

  printf("CMTestSetup: Event()\n");fflush(stdout);
  ret = hbook->BosToRoot(iw);
  printf("CMTestSetup: Event() done, ret=%d\n",ret);fflush(stdout);

  return(ret);
}


//____________________________________________________________________________
void CMTestSetup::Update()
{
// update drawing

  Int_t time1, ret;

  m_Pad->cd();
  m_Pad->GetListOfPrimitives()->Clear("nodelete");
  viewer->ViewPlot(iview);

  time1 = time(NULL)-time0;
  //if(time1 > 0)
  //{
  //  printf("Event rate: overall %5.0f events/sec, useful %5.0f events/sec\n",
  //  ((Float_t)nevents)/((Float_t)time1),((Float_t)nevents1)/((Float_t)time1));
  //}

}


//_____________________________________________________________________________
void CMTestSetup::Status()
{
// check status and update status pad

  Int_t i;

  if(call1) m_Arc2->SetFillColor(kGreen); else m_Arc2->SetFillColor(kRed);
  if(call2) m_Arc3->SetFillColor(kGreen); else m_Arc3->SetFillColor(kRed);
  if(downloaded) m_Arc4->SetFillColor(kGreen); else m_Arc4->SetFillColor(kRed);

  for(i=0; i<nview; i++) m_Arc1[i]->SetFillColor(kYellow);
  m_Arc1[iview]->SetFillColor(kGreen);

  m_StatPad->Modified();

  this->Update();

}


//_____________________________________________________________________________
void CMTestSetup::Change1()
{
// (un)set call1

  if(call1 == 0)
  {
    call1 = 1;
  }
  else
  {
    call1 = 0;
  }

  printf("CMTestSetup::Change1(): set call1=%d\n",call1);

  Status();

}


//_____________________________________________________________________________
void CMTestSetup::Change2()
{
// (un)set call2

  if(call2 == 0)
  {
    call2 = 1;
  }
  else
  {
    call2 = 0;
  }

  printf("CMTestSetup::Change2(): set call2=%d\n",call2);

  Status();

}


//_____________________________________________________________________________
void CMTestSetup::Buttons()
{
// Create the buttons

  printf("CMTestSetup::Buttons reached, m_StatPad=0x%08x\n",m_StatPad);

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

  char *but1 = "TestSetupPtr->PreviousView()";
  button = new TButton("Previous View",but1,x0,y-dbutton,x01,y);
  button->SetFillColor(butcolor);
  button->Draw();
  char *but2 = "TestSetupPtr->NextView()";
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
  char *but3 = "TestSetupPtr->Change1()";
  button = new TButton("reserve1",but3,x0,y-dbutton,x1,y);
  button->SetFillColor(38);
  button->Draw();
  m_Arc2   = new TArc(dxtr/2.,(y-dbutton-dy*3.)*dytr,rarc);
  m_Arc2->Draw();

  y -= dbutton + dy*6.;
  char *but4 = "TestSetupPtr->Change2()";
  button = new TButton("reserve2",but4,x0,y-dbutton,x1,y);
  button->SetFillColor(38);
  button->Draw();
  m_Arc3   = new TArc(dxtr/2.,(y-dbutton-dy*3.)*dytr,rarc);
  m_Arc3->Draw();

  y -= dbutton + dy*6.;
  char *but5 = "TestSetupPtr->Menu()";
  button = new TButton("Board Chooser",but5,x0,y-dbutton,x1,y);
  button->SetFillColor(butcolor);
  button->Draw();
  m_Arc4   = new TArc(dxtr/2.,(y-dbutton-dy*3.)*dytr,rarc);
  m_Arc4->Draw();

  Status();

}


//_____________________________________________________________________________
void CMTestSetup::Reset()
{
// Reset histograms

  Int_t i;

  hbook->Clean();

}


//_____________________________________________________________________________
void CMTestSetup::Menu()
{
  //
  if(ended!=1)
  {
    printf("CMTestSetup::Menu(): you must be in 'Ended' state to use menu\n");
  }
  else
  {
    fDialog->MapWindow();
  }
}











//////////////
// menu class
//////////////


#define NSLOT 20
static char slotname[NSLOT][7] =
                {"slot6", "slot7", "slot8", "slot9", "slot10",
                 "slot11","slot12","slot13","slot14","slot15",
                 "slot16","slot17","slot18","slot19","slot20",
                 "slot21","slot22","slot23","slot24","slot25"};
// list of slots is hardcoded here !!!

//_____________________________________________________________________________
CMTestSetupMenu::CMTestSetupMenu(const TGWindow *p, const TGWindow *main,
                                 UInt_t w, UInt_t h, UInt_t options)
    : TGTransientFrame(p, main, w, h, options)
{
   // Create a dialog window. A dialog window pops up with respect to its
   // "main" window.

   // Used to store GUI elements that need to be deleted in the ctor.

   Int_t i;

   fSlot = -1;
   fFrame1 = new TGHorizontalFrame(this, 60, 20, kFixedWidth);

   fOkButton = new TGTextButton(fFrame1, "&Ok", 1);
   fOkButton->Associate(this);
   fCancelButton = new TGTextButton(fFrame1, "&Cancel", 2);
   fCancelButton->Associate(this);

   fL1 = new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandX,
                           2, 2, 2, 2);
   fL2 = new TGLayoutHints(kLHintsBottom | kLHintsRight, 2, 2, 5, 1);

   fFrame1->AddFrame(fOkButton, fL1);
   fFrame1->AddFrame(fCancelButton, fL1);

   fFrame1->Resize(150, fOkButton->GetDefaultHeight());
   AddFrame(fFrame1, fL2);


   // --------- create Tab widget and some composite frames for Tab testing

   fTab = new TGTab(this, 300, 300);
   fL3 = new TGLayoutHints(kLHintsTop | kLHintsLeft, 5, 5, 5, 5);



   // tab 'Boards'

   TGCompositeFrame *tf = fTab->AddTab("Boards");

   fF4 = new TGCompositeFrame(tf, 60, 20, kVerticalFrame);
   fF4->AddFrame(fListBox = new TGListBox(fF4, 89), fL3);
   tf->AddFrame(fF4, fL3);

   for(i=0; i<NSLOT; i++)
   {
     fListBox->AddEntry(slotname[i], i+1);
   }
   fFirstEntry = 1;
   fLastEntry  = NSLOT;

   fListBox->Resize(150, 330);


   // final preparations

   TGLayoutHints *fL5 = new TGLayoutHints(kLHintsBottom | kLHintsExpandX |
                                          kLHintsExpandY, 2, 2, 5, 1);
   AddFrame(fTab, fL5);

   MapSubwindows();
   Resize(GetDefaultSize());

   // position relative to the parent's window
   Window_t wdum;
   int ax, ay;
   gVirtualX->TranslateCoordinates(main->GetId(), GetParent()->GetId(),
                          (((TGFrame *) main)->GetWidth() - fWidth) >> 1,
                          (((TGFrame *) main)->GetHeight() - fHeight) >> 1,
                          ax, ay, wdum);
   Move(ax, ay);

   SetWindowName("Board Chooser");
}


//_____________________________________________________________________________
CMTestSetupMenu::~CMTestSetupMenu()
{
   // Delete test dialog widgets.

  Int_t i;

  delete fOkButton;
  delete fCancelButton;
  delete fFrame1;
  delete fListBox;
  delete fF4;
  delete fTab;
  delete fL1;
  delete fL2;
  delete fL3;
   
}

//_____________________________________________________________________________
Bool_t CMTestSetupMenu::ProcessMessage(Long_t msg, Long_t parm1, Long_t)
{
   // Process messages coming from widgets associated with the dialog.

   Int_t id, nd;
   TList *list = new TList; 
   char tmp[20];
   static int newtab = 0;

   switch (GET_MSG(msg)) {
      case kC_COMMAND:

         printf("kC_COMMAND\n");fflush(stdout);

         switch (GET_SUBMSG(msg)) {
            case kCM_LISTBOX:
         printf("kCM_LISTBOX\n");fflush(stdout);
               break;
            case kCM_BUTTON:
               switch(parm1) {
                  case 1:
                  case 2:
         printf("kCM_BUTTON\n");fflush(stdout);
                     printf("\nTerminating dialog: %s pressed\n",
                            (parm1 == 1) ? "OK" : "Cancel");

                     if(parm1 == 1)
                     {
                       printf("OK\n");
                       id = fListBox->GetSelected();
                       if(id != -1)
                       {
                         printf("Select entry %d\n",id);fflush(stdout);
                         fSlot = (id-1)+6;
                         printf("Slot num=%d name>%s<\n",fSlot,slotname[id-1]);
                         UnmapWindow();
                       }
                       else
                       {
                         printf("You must select entry !\n");fflush(stdout);
                       }
                     }
                     else
                     {
                       printf("Cancel\n");
                       UnmapWindow();
                     }

                     break;

                  default:
                     break;
               }
               break;

            default:
               break;
         }
         break;

      default:
         break;
   }
   return kTRUE;
}

