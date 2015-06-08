h64306
s 00389/00000/00000
d D 1.1 06/10/19 15:41:13 boiarino 1 0
c date and time created 06/10/19 15:41:13 by boiarino
e
u
U
f e 0
t
T
I 1
//////////////////////////////////////////////////////////////////////////
// CMSparsMenuCrate.cxx
//////////////////////////////////////////////////////////////////////////


#include "CMSparsMenuCrate.h"

// list of crates is hardcoded here !!!
#define NCRATE 6
char cratenames[NCRATE][20] = {"cc1","ec1","ec2","sc1","lac1","primexroc1"};
// list of crates is hardcoded here !!!

CMSparsMenuCrate::CMSparsMenuCrate(const TGWindow *p,
                                   const TGWindow *main,
                                   UInt_t w, UInt_t h, char *par, 
                                   UInt_t options)
    : TGTransientFrame(p, main, w, h, options)
{
   // Create a dialog window. A dialog window pops up with respect to its
   // "main" window.

   // Used to store GUI elements that need to be deleted in the ctor.

   Int_t i;

localpar = par;

   fCleanup = new TList;

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



   // tab 'Crates'

   TGTextButton *bt;
   TGCompositeFrame *tf = fTab->AddTab("Crates");
   fF4 = new TGCompositeFrame(tf, 60, 20, kVerticalFrame);

   fF4->AddFrame(bt = new TGTextButton(fF4, "A&dd Entry", 90), fL3);
   bt->Associate(this);
   fCleanup->Add(bt);

   fF4->AddFrame(bt = new TGTextButton(fF4, "Remove &Entry", 91), fL3);
   bt->Associate(this);
   fCleanup->Add(bt);

   fF4->AddFrame(bt = new TGTextButton(fF4, "&Select", 92), fL3);
   bt->Associate(this);
   fCleanup->Add(bt);

   fF4->AddFrame(fListBox = new TGListBox(fF4, 89), fL3);
   fF4->AddFrame(fCheckMulti = new TGCheckButton(fF4, "&Mutli Selectable", 93), fL3);
   fCheckMulti->Associate(this);
   tf->AddFrame(fF4, fL3);

   for(i=0; i<NCRATE; i++)
   {
     fListBox->AddEntry(cratenames[i], i+1);
   }
   fFirstEntry = 1;
   fLastEntry  = NCRATE;

   fListBox->Resize(150, 80);





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

   SetWindowName("Calibration Menu");

   MapWindow();
   //fClient->WaitFor(this);    // otherwise canvas contextmenu does not work
}


CMSparsMenuCrate::~CMSparsMenuCrate()
{
   // Delete test dialog widgets.

  Int_t i;

   fCleanup->Delete();
   delete fCleanup;
   delete fOkButton;
   delete fCancelButton;
   delete fStartB; delete fStopB;
   delete fFrame1;
   delete fBtn1; delete fBtn2;
   delete fChk1; delete fChk2;


//for(i=0; i<32; i++) delete fRad[i];


   delete fTxt1; delete fTxt2;
   delete fEc1; delete fEc2;
   delete fListBox; delete fCheckMulti;
   delete fF1; delete fF2; delete fF3; delete fF4; delete fF5;
   delete fF6; delete fF7;
   delete fCombo;
   delete fTab;
   delete fL3; delete fL4;
   delete fL1; delete fL2;
   delete fHpx; delete fHpxpy;
}

void CMSparsMenuCrate::FillHistos()
{
   // Fill histograms till user clicks "Stop Filling" button.

   static int cnt;

   if (!fHpx) {
      fHpx   = new TH1F("hpx","This is the px distribution",100,-4,4);
      fHpxpy = new TH2F("hpxpy","py vs px",40,-4,4,40,-4,4);
      fHpx->SetFillColor(kRed);
      cnt = 0;
   }

   const int kUPDATE = 1000;
   float px, py;
   TCanvas *c1 = fEc1->GetCanvas();
   TCanvas *c2 = fEc2->GetCanvas();

   while (fFillHistos) {
      gRandom->Rannor(px,py); //px and py will be two gaussian random numbers
      fHpx->Fill(px);
      fHpxpy->Fill(px,py);
      cnt++;
      if (!(cnt % kUPDATE)) {
         if (cnt == kUPDATE) {
            c1->cd();
            fHpx->Draw();
            c2->cd();
            fHpxpy->Draw("cont");
         }
         c1->Modified();
         c1->Update();
         c2->Modified();
         c2->Update();
         gSystem->ProcessEvents();  // handle GUI events
      }
   }
}

void CMSparsMenuCrate::CloseWindow()
{
   // Called when window is closed via the window manager.

   delete this;
}

Bool_t CMSparsMenuCrate::ProcessMessage(Long_t msg, Long_t parm1, Long_t)
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
                       printf("Select entry %d\n",id);fflush(stdout);
                       printf("Crate name >%s<\n",cratenames[id-1]);
                       strcpy(localpar,cratenames[id-1]);
					 }
                     else
                     {
                       printf("Cancel\n");
                     }

                     CloseWindow();
                     break;

                  case 90:  // add one entry in list box
					//fLastEntry++;
					//sprintf(tmp, "Entry %i", fLastEntry);
					//fListBox->AddEntry(tmp, fLastEntry);
					//fListBox->MapSubwindows();
					//fListBox->Layout();
                     break;

                  case 91:  // remove one entry in list box
					//if(fFirstEntry < fLastEntry)
					//{
					//  fListBox->RemoveEntry(fFirstEntry);
					//  fListBox->Layout();
					//  fFirstEntry++;
					//}
                     break;

                  case 92:  // select entry or entries in list box
                     id = fListBox->GetSelected();
                     printf("Select entry %d\n",id);fflush(stdout);
                     printf("Crate name >%s<\n",cratenames[id-1]);


                     //TGLBEntry *e;
                     //e = fListBox->GetSelectedEntry();
                     //printf("Selected entry %d 0x%08x<\n",e,e);fflush(stdout);

                     //fListBox->GetSelectedEntries(list);
                     //printf("first=%c\n", *((char *)list->First()) );
					 //{
                     //  char *str;
					 //  TIter nextin(fList);
					 //  while( str = (char *)nextin() )
					 //  {
					 //    printf("92: %3.3s\n",str);
					 //  }
					 //}


                     break;

                  case 101:  // remove tabs
					//{
					//  TString s = fTab->GetTabTab(0)->GetString();
					//  if (s == "Tab 3") {
					//     // Need to delete the embedded canvases
					//     // since RemoveTab() will Destroy the container
					//     // window, which in turn will destroy the embedded
					//     // canvas windows.
					//     SafeDelete(fEc1);
					//     SafeDelete(fEc2);
					//  }
					//  fTab->RemoveTab(0);
					//  fTab->Layout();
					//}
                     break;
                  case 102:  // remove tab 5
					//{
					//  int nt = fTab->GetNumberOfTabs();
					//  for (int i = 0 ; i < nt; i++) {
					//     TString s = fTab->GetTabTab(i)->GetString();
					//     if (s == "Tab 5") {
					//        fTab->RemoveTab(i);
					//        fTab->Layout();
					//     }
					//  }
					//}
                     break;
                  case 103:  // add tabs
					//sprintf(tmp, "New Tab %d", ++newtab);
					//fTab->AddTab(tmp);
					//fTab->MapSubwindows();
					//fTab->Layout();
                     break;
                  default:
                     break;
               }
               break;
            case kCM_RADIOBUTTON:
               switch (parm1) {
                  case 81:
					//fRad[2]->SetState(kButtonUp);
                     //fRad[3]->SetState(kButtonUp);
                     //fRad[4]->SetState(kButtonUp);
                     break;
                  case 82:
					//fRad[1]->SetState(kButtonUp);
                     //fRad[3]->SetState(kButtonUp);
                     //fRad[4]->SetState(kButtonUp);
                     break;
                  case 83:
					//fRad[1]->SetState(kButtonUp);
					//fRad[2]->SetState(kButtonUp);
                     //fRad[4]->SetState(kButtonUp);
                     break;
                  case 84:
					//fRad[1]->SetState(kButtonUp);
					//fRad[2]->SetState(kButtonUp);
                     //fRad[3]->SetState(kButtonUp);
                     break;
               }
               break;
            case kCM_CHECKBUTTON:
               switch (parm1) {
                  case 93:
                     fListBox->SetMultipleSelections(fCheckMulti->GetState());
                     break;
                  default:
                     break;
               }
               break;
            case kCM_TAB:
               printf("Tab item %ld activated\n", parm1);
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


//---- Main program ------------------------------------------------------------


/*

TROOT root("GUI", "GUI test environement");


int main(int argc, char **argv)
{
   TApplication theApp("App", &argc, argv);

   if (gROOT->IsBatch()) {
      fprintf(stderr, "%s: cannot run in batch mode\n", argv[0]);
      return 1;
   }

   TestMainFrame mainWindow(gClient->GetRoot(), 400, 220);

   theApp.Run();

   return 0;
}

*/


E 1
