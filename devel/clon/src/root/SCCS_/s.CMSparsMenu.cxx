h00425
s 00000/00000/00000
d R 1.2 03/06/20 17:33:12 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 clas/root/CMSparsMenu.cxx
e
s 00487/00000/00000
d D 1.1 03/06/20 17:33:11 boiarino 1 0
c date and time created 03/06/20 17:33:11 by boiarino
e
u
U
f e 0
t
T
I 1
//////////////////////////////////////////////////////////////////////////
// CMSparsMenu.cxx: CalibMenu constructor
//////////////////////////////////////////////////////////////////////////

#include "CMSparsMenu.h"


CMSparsMenu::CMSparsMenu(const TGWindow *p, const TGWindow *main, UInt_t w,
                       UInt_t h, UInt_t options)
    : TGTransientFrame(p, main, w, h, options)
{
   // Create a dialog window. A dialog window pops up with respect to its
   // "main" window.

   // Used to store GUI elements that need to be deleted in the ctor.
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



   // tab 1

   TGCompositeFrame *tf = fTab->AddTab("Tab 1");
   fF1 = new TGCompositeFrame(tf, 60, 20, kVerticalFrame);
   fF1->AddFrame(new TGTextButton(fF1, "&Test button", 0), fL3);
   fF1->AddFrame(fTxt1 = new TGTextEntry(fF1, new TGTextBuffer(100)), fL3);
   fF1->AddFrame(fTxt2 = new TGTextEntry(fF1, new TGTextBuffer(100)), fL3);

   fF1->AddFrame(fRad1 = new TGRadioButton(fF1, "&Tagger", 81), fL3);
   fF1->AddFrame(fRad2 = new TGRadioButton(fF1, "T&OF   ", 82), fL3);

   tf->AddFrame(fF1, fL3);
   fTxt1->Resize(150, fTxt1->GetDefaultHeight());
   fTxt2->Resize(150, fTxt2->GetDefaultHeight());

   fRad1->Associate(this);
   fRad2->Associate(this);




   // tab 2

   tf = fTab->AddTab("Tab 2");
   fL1 = new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandX,
                           200, 2, 2, 2);
   fF2 = new TGCompositeFrame(tf, 60, 20, kVerticalFrame);
   fF2->AddFrame(fBtn1 = new TGTextButton(fF2, "&Button 1", 0), fL1);
   fF2->AddFrame(fBtn2 = new TGTextButton(fF2, "B&utton 2", 0), fL1);
   fF2->AddFrame(fChk1 = new TGCheckButton(fF2, "C&heck 1", 0), fL1);
   fF2->AddFrame(fChk2 = new TGCheckButton(fF2, "Chec&k 2", 0), fL1);
   fF2->AddFrame(fRad3 = new TGRadioButton(fF2, "&Radio 1", 83), fL1);
   fF2->AddFrame(fRad4 = new TGRadioButton(fF2, "R&adio 2", 84), fL1);
   fCombo = new TGComboBox(fF2, 88);
   fF2->AddFrame(fCombo, fL3);

   tf->AddFrame(fF2, fL3);

   int i;
   for (i = 0; i < 20; i++) {
      char tmp[20];

      sprintf(tmp, "Entry %i", i+1);
      fCombo->AddEntry(tmp, i+1);
   }

   fCombo->Resize(150, 20);

   fBtn1->Associate(this);
   fBtn2->Associate(this);
   fChk1->Associate(this);
   fChk2->Associate(this);
   fRad3->Associate(this);
   fRad4->Associate(this);






   // tab 3 -------------- embedded canvas demo

   fFillHistos = kFALSE;
   fHpx   = 0;
   fHpxpy = 0;

   tf = fTab->AddTab("Tab 3");
   fF3 = new TGCompositeFrame(tf, 60, 20, kHorizontalFrame);
   fStartB = new TGTextButton(fF3, "Start &Filling Hists", 40);
   fStopB  = new TGTextButton(fF3, "&Stop Filling Hists", 41);
   fStartB->Associate(this);
   fStopB->Associate(this);
   fF3->AddFrame(fStartB, fL3);
   fF3->AddFrame(fStopB, fL3);

   fF5 = new TGCompositeFrame(tf, 60, 60, kHorizontalFrame);

   fL4 = new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandX |
                           kLHintsExpandY, 5, 5, 5, 5);
   fEc1 = new TRootEmbeddedCanvas("ec1", fF5, 100, 100);
   fF5->AddFrame(fEc1, fL4);
   fEc2 = new TRootEmbeddedCanvas("ec2", fF5, 100, 100);
   fF5->AddFrame(fEc2, fL4);

   tf->AddFrame(fF3, fL3);
   tf->AddFrame(fF5, fL4);

   fEc1->GetCanvas()->SetBorderMode(0);
   fEc2->GetCanvas()->SetBorderMode(0);

   // make tab yellow
   ULong_t yellow;
   fClient->GetColorByName("yellow", yellow);
   TGTabElement *tabel = fTab->GetTabTab(2);
   tabel->ChangeBackground(yellow);

   //-------------- end embedded canvas demo





   // tab 4

   TGTextButton *bt;
   tf = fTab->AddTab("Tab 4");
   fF4 = new TGCompositeFrame(tf, 60, 20, kVerticalFrame);
   fF4->AddFrame(bt = new TGTextButton(fF4, "A&dd Entry", 90), fL3);
   bt->Associate(this);
   fCleanup->Add(bt);
   fF4->AddFrame(bt = new TGTextButton(fF4, "Remove &Entry", 91), fL3);
   bt->Associate(this);
   fCleanup->Add(bt);
   fF4->AddFrame(fListBox = new TGListBox(fF4, 89), fL3);
   fF4->AddFrame(fCheckMulti = new TGCheckButton(fF4, "&Mutli Selectable", 92), fL3);
   fCheckMulti->Associate(this);
   tf->AddFrame(fF4, fL3);

   for (i=0; i < 20; ++i) {
      char tmp[20];

      sprintf(tmp, "Entry %i", i+1);
      fListBox->AddEntry(tmp, i+1);
   }
   fFirstEntry = 1;
   fLastEntry  = 20;

   fListBox->Resize(150, 80);





   //--- tab 5

   tf = fTab->AddTab("Tab 5");
   tf->SetLayoutManager(new TGHorizontalLayout(tf));

   fF6 = new TGGroupFrame(tf, "Options", kVerticalFrame);
   tf->AddFrame(fF6, fL3);

   // 2 column, n rows
   fF6->SetLayoutManager(new TGMatrixLayout(fF6, 0, 2, 10));
   char buff[100];
   int j;
   for (j = 0; j < 4; j++) {
      sprintf(buff, "Module %i", j+1);
      fF6->AddFrame(new TGLabel(fF6, new TGHotString(buff)));

      TGTextBuffer *tbuf = new TGTextBuffer(10);
      tbuf->AddText(0, "0.0");

      TGTextEntry  *tent = new TGTextEntry(fF6, tbuf);
      tent->Resize(50, tent->GetDefaultHeight());
      tent->SetFont("-adobe-courier-bold-r-*-*-14-*-*-*-*-*-iso8859-1");
      fCleanup->Add(tent);
      fF6->AddFrame(tent);
   }
   fF6->Resize(fF6->GetDefaultSize());

   // another matrix with text and buttons
   fF7 = new TGGroupFrame(tf, "Tab Handling", kVerticalFrame);
   tf->AddFrame(fF7, fL3);

   fF7->SetLayoutManager(new TGMatrixLayout(fF7, 0, 1, 10));

   fF7->AddFrame(bt = new TGTextButton(fF7, "Remove Tab", 101));
   bt->Associate(this);
   bt->Resize(90, bt->GetDefaultHeight());
   fCleanup->Add(bt);

   fF7->AddFrame(bt = new TGTextButton(fF7, "Add Tab", 103));
   bt->Associate(this);
   bt->Resize(90, bt->GetDefaultHeight());
   fCleanup->Add(bt);

   fF7->AddFrame(bt = new TGTextButton(fF7, "Remove Tab 5", 102));
   bt->Associate(this);
   bt->Resize(90, bt->GetDefaultHeight());
   fCleanup->Add(bt);

   fF7->Resize(fF6->GetDefaultSize());

   //--- end of last tab




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


CMSparsMenu::~CMSparsMenu()
{
   // Delete test dialog widgets.

printf("m1\n");fflush(stdout);
   fCleanup->Delete();
   delete fCleanup;
   delete fOkButton;
   delete fCancelButton;
   delete fStartB; delete fStopB;
   delete fFrame1;
   delete fBtn1; delete fBtn2;
   delete fChk1; delete fChk2;
   delete fRad1; delete fRad2; delete fRad3; delete fRad4;
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
printf("m9\n");fflush(stdout);
}

void CMSparsMenu::FillHistos()
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

void CMSparsMenu::CloseWindow()
{
   // Called when window is closed via the window manager.

printf("m11\n");fflush(stdout);
   delete this;
printf("m12\n");fflush(stdout);
}

Bool_t CMSparsMenu::ProcessMessage(Long_t msg, Long_t parm1, Long_t)
{
   // Process messages coming from widgets associated with the dialog.

   char tmp[20];
   static int newtab = 0;

   switch (GET_MSG(msg)) {
      case kC_COMMAND:

         switch (GET_SUBMSG(msg)) {
            case kCM_BUTTON:
               switch(parm1) {
                  case 1:
                  case 2:
                     printf("\nTerminating dialog: %s pressed\n",
                            (parm1 == 1) ? "OK" : "Cancel");
                     CloseWindow();
                     break;
                  case 40:  // start histogram filling
                     fFillHistos = kTRUE;
                     FillHistos();
                     break;
                  case 41:  // stop histogram filling
                     fFillHistos = kFALSE;
                     break;
                  case 90:  // add one entry in list box
                     fLastEntry++;
                     sprintf(tmp, "Entry %i", fLastEntry);
                     fListBox->AddEntry(tmp, fLastEntry);
                     fListBox->MapSubwindows();
                     fListBox->Layout();
                     break;
                  case 91:  // remove one entry in list box
                     if (fFirstEntry < fLastEntry) {
                        fListBox->RemoveEntry(fFirstEntry);
                        fListBox->Layout();
                        fFirstEntry++;
                     }
                     break;
                  case 101:  // remove tabs
                     {
                        TString s = fTab->GetTabTab(0)->GetString();
                        if (s == "Tab 3") {
                           // Need to delete the embedded canvases
                           // since RemoveTab() will Destroy the container
                           // window, which in turn will destroy the embedded
                           // canvas windows.
                           SafeDelete(fEc1);
                           SafeDelete(fEc2);
                        }
                        fTab->RemoveTab(0);
                        fTab->Layout();
                     }
                     break;
                  case 102:  // remove tab 5
                     {
                        int nt = fTab->GetNumberOfTabs();
                        for (int i = 0 ; i < nt; i++) {
                           TString s = fTab->GetTabTab(i)->GetString();
                           if (s == "Tab 5") {
                              fTab->RemoveTab(i);
                              fTab->Layout();
                           }
                        }
                     }
                     break;
                  case 103:  // add tabs
                     sprintf(tmp, "New Tab %d", ++newtab);
                     fTab->AddTab(tmp);
                     fTab->MapSubwindows();
                     fTab->Layout();
                     break;
                  default:
                     break;
               }
               break;
            case kCM_RADIOBUTTON:
               switch (parm1) {
                  case 81:
                     fRad2->SetState(kButtonUp);
                     fRad3->SetState(kButtonUp);
                     fRad4->SetState(kButtonUp);
                     break;
                  case 82:
                     fRad1->SetState(kButtonUp);
                     fRad3->SetState(kButtonUp);
                     fRad4->SetState(kButtonUp);
                     break;
                  case 83:
                     fRad1->SetState(kButtonUp);
                     fRad2->SetState(kButtonUp);
                     fRad4->SetState(kButtonUp);
                     break;
                  case 84:
                     fRad1->SetState(kButtonUp);
                     fRad2->SetState(kButtonUp);
                     fRad3->SetState(kButtonUp);
                     break;
               }
               break;
            case kCM_CHECKBUTTON:
               switch (parm1) {
                  case 92:
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
