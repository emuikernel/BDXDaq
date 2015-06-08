h09109
s 00000/00000/00000
d R 1.2 03/06/20 17:32:04 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 clas/root/CMInputDialog.h
e
s 00061/00000/00000
d D 1.1 03/06/20 17:32:03 boiarino 1 0
c date and time created 03/06/20 17:32:03 by boiarino
e
u
U
f e 0
t
T
I 1
//////////////////////////////////////////////////
// This is a modified copy of tutorial/dialogs.C
//////////////////////////////////////////////////
//
// This file contains the class CMInputDialog.
// An InputDialog object prompts for an input string using a simple
// dialog box. The InputDialog class is also a good example of how
// to use the ROOT GUI classes via the interpreter. Since interpreted
// classes can not call virtual functions via base class pointers, all
// GUI objects are used by composition instead of by inheritance.
//
// This file contains also some utility functions that use
// the InputDialog class to either get a string, integer or
// floating point number. There are also two functions showing
// how to use the file open and save dialogs. The utility functions are:
//
// const char *OpenFileDialog()
// const char *SaveFileDialog()
// const char *GetStringDialog(const char *prompt, const char *defval)
// Int_t GetIntegerDialog(const char *prompt, Int_t defval)
// Float_t GetFloatDialog(const char *prompt, Float_t defval)
//
// To use the InputDialog class and the utility functions you just
// have to load the dialogs.C file as follows:
// .L dialogs.C
//
// Now you can use them like:
// {
//    const char *file = OpenFileDialog();
//    Int_t run   = GetIntegerDialog("Give run number:", 0);
//    Int_t event = GetIntegerDialog("Give event number:", 0);
//    printf("analyse run %d, event %d from file %s\n", run ,event, file);
// }
//

///////////////////////////////////////////////////////////////////////////
//                                                                       //
// Input Dialog Widget                                                   //
//                                                                       //
///////////////////////////////////////////////////////////////////////////

#include "TGTRA.h"
#include "TGTextEntry.h"
#include "TList.h"


class CMInputDialog {

private:
   TGTransientFrame *fDialog;  // transient frame, main dialog window
   TGTextEntry      *fTE;      // text entry widget containing
   TList            *fWidgets; // keep track of widgets to be deleted in dtor
   char             *fRetStr;  // address to store return string

public:
           CMInputDialog(const char *prompt, const char *defval, char *retstr);
          ~CMInputDialog();

  void     ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2);

};
E 1
