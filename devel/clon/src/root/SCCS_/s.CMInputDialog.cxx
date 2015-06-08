h29296
s 00000/00000/00000
d R 1.2 03/06/20 17:32:07 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 clas/root/CMInputDialog.cxx
e
s 00185/00000/00000
d D 1.1 03/06/20 17:32:06 boiarino 1 0
c date and time created 03/06/20 17:32:06 by boiarino
e
u
U
f e 0
t
T
I 1
///////////////////////////////////////////////////////////////////////////
//                                                                       //
// CMInputDialog.cxx                                                     //
//                                                                       //
// Input Dialog Widget                                                   //
//                                                                       //
///////////////////////////////////////////////////////////////////////////

#include "TGWindow.h"
#include "TGLabel.h"
#include "TGButton.h"
#include "CMInputDialog.h"

//ClassImp(CMInputDialog)

//_____________________________________________________________________________
CMInputDialog::CMInputDialog(const char *prompt, const char *defval,
                             char *retstr)
{
  // Create simple input dialog.

  fWidgets = new TList;

  TGWindow *main = (TGWindow *) gClient->GetRoot(); // need cast !!??
  fDialog = new TGTransientFrame(main, main, 10, 10);

  // command to be executed by buttons and text entry widget
  char cmd[128];
  sprintf(cmd,"{long r__ptr=0x%x; ((CMInputDialog*)r__ptr)->ProcessMessage($MSG,$PARM1,$PARM2);}", this);

  // create prompt label and textentry widget
  TGLabel *label = new TGLabel(fDialog, prompt);
  fWidgets->Add(label);

  TGTextBuffer *tbuf = new TGTextBuffer(256);  //will be deleted by TGtextEntry
  tbuf->AddText(0, defval);

  fTE = new TGTextEntry(fDialog, tbuf);
  fTE->Resize(260, fTE->GetDefaultHeight());
  fTE->SetCommand(cmd);

  TGLayoutHints *l1 = new TGLayoutHints(kLHintsTop | kLHintsLeft, 5, 5, 5, 0);
  TGLayoutHints *l2 = new TGLayoutHints(kLHintsTop | kLHintsLeft, 5, 5, 5, 5);
  fWidgets->Add(l1);
  fWidgets->Add(l2);

  fDialog->AddFrame(label, l1);
  fDialog->AddFrame(fTE, l2);

  // create frame and layout hints for Ok and Cancel buttons
  TGHorizontalFrame *hf = new TGHorizontalFrame(fDialog, 60, 20, kFixedWidth);
  TGLayoutHints     *l3 = new TGLayoutHints(kLHintsCenterY | kLHintsExpandX, 5, 5, 0, 0);

  // put hf as last in list to be deleted
  fWidgets->Add(l3);

  // create OK and Cancel buttons in their own frame (hf)
  UInt_t  nb = 0, width = 0, height = 0;
  TGTextButton *b;

  b = new TGTextButton(hf, "&Ok", cmd, 1);
  fWidgets->Add(b);
  b->Associate(fDialog);
  hf->AddFrame(b, l3);
  height = b->GetDefaultHeight();
  width  = TMath::Max(width, b->GetDefaultWidth()); ++nb;

  b = new TGTextButton(hf, "&Cancel", cmd, 2);
  fWidgets->Add(b);
  b->Associate(fDialog);
  hf->AddFrame(b, l3);
  height = b->GetDefaultHeight();
  width  = TMath::Max(width, b->GetDefaultWidth()); ++nb;

  // place button frame (hf) at the bottom
  TGLayoutHints *l4 = new TGLayoutHints(kLHintsBottom | kLHintsCenterX, 0, 0, 5, 5);
  fWidgets->Add(l4);
  fWidgets->Add(hf);

  fDialog->AddFrame(hf, l4);

  // keep buttons centered and with the same width
  hf->Resize((width + 20) * nb, height);

  // set dialog title
  fDialog->SetWindowName("Get Input");

  // map all widgets and calculate size of dialog
  fDialog->MapSubwindows();

  width  = fDialog->GetDefaultWidth();
  height = fDialog->GetDefaultHeight();

  fDialog->Resize(width, height);

  // position relative to the parent window (which is the root window)
  Window_t wdum;
  int      ax, ay;

  gVirtualX->TranslateCoordinates(main->GetId(), main->GetId(),
                          (((TGFrame *) main)->GetWidth() - width) >> 1,
                          (((TGFrame *) main)->GetHeight() - height) >> 1,
                          ax, ay, wdum);
  fDialog->Move(ax, ay);
  fDialog->SetWMPosition(ax, ay);

  // make the message box non-resizable
  fDialog->SetWMSize(width, height);
  fDialog->SetWMSizeHints(width, height, width, height, 0, 0);

  fDialog->SetMWMHints(kMWMDecorAll | kMWMDecorResizeH  | kMWMDecorMaximize |
                                      kMWMDecorMinimize | kMWMDecorMenu,
                       kMWMFuncAll  | kMWMFuncResize    | kMWMFuncMaximize |
                                      kMWMFuncMinimize,
                       kMWMInputModeless);

  // popup dialog and wait till user replies
  fDialog->MapWindow();

  fRetStr = retstr;

  gClient->WaitFor(fDialog);
}

//_____________________________________________________________________________
CMInputDialog::~CMInputDialog()
{
  // Cleanup dialog.

  fWidgets->Delete();
  delete fWidgets;

  delete fTE;
  delete fDialog;
}

//_____________________________________________________________________________
void CMInputDialog::ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2)
{
  // Handle button and text enter events

  switch(GET_MSG(msg))
  {
    case kC_COMMAND:
      switch(GET_SUBMSG(msg))
      {
        case kCM_BUTTON:
          switch(parm1)
          {
            case 1:
              // here copy the string from text buffer to return variable
              strcpy(fRetStr, fTE->GetBuffer()->GetString());
              delete this;
              break;

            case 2:
              fRetStr[0] = 0;
              delete this;
              break;
          }
        default:
          break;
      }
      break;

    case kC_TEXTENTRY:
      switch(GET_SUBMSG(msg))
      {
        case kTE_ENTER:
          // here copy the string from text buffer to return variable
          strcpy(fRetStr, fTE->GetBuffer()->GetString());
          delete this;
          break;
        default:
          break;
      }
      break;

    default:
      break;
  }

}


E 1
