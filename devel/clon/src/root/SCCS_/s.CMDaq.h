h61115
s 00003/00001/00075
d D 1.2 03/04/17 17:00:10 boiarino 3 1
c *** empty log message ***
e
s 00000/00000/00000
d R 1.2 01/11/19 16:44:44 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 clas/root/CMDaq.h
e
s 00076/00000/00000
d D 1.1 01/11/19 16:44:43 boiarino 1 0
c date and time created 01/11/19 16:44:43 by boiarino
e
u
U
f e 0
t
T
I 1
#ifndef CMDaq_H
#define CMDaq_H

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// CMDaq                                                                //
//                                                                      //
// Daq for Moller Polarimeter                                           //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#ifndef ROOT_TObject
#include <TObject.h>
#endif
#include "TH1.h"
#include "TH2.h"
#include "TCanvas.h"
#include "TText.h"

#define NCLIENT 2

class CMDisplay;
class CMClient;


class CMDaq : public TObject {

private:

   Int_t downloaded, prestarted, active, ended;

D 3
   Option_t         *filename;               // Data file name or ET
E 3
I 3
   char              filename[1024];         // Data file name or ET file name
E 3
   CMDisplay        *m_Display;              // pointer to CMDisplay object
   Int_t             fLoop;                  // Loop in progress
   Int_t             nev;                    // Event number
   Int_t             newdstfile;             // if != 0 open new dst file
   Int_t             forcestop;              // if != 0 stop data acquisition
   TPad             *m_Pad;

   Int_t             nclient;
   Int_t             clientActive[NCLIENT];  // 1 for active clients, 0 for ...
   CMClient         *m_Client[NCLIENT];      // the list of clients to be called
   
public:
                     CMDaq(Option_t *fname);
   virtual          ~CMDaq();

   virtual void      Download();
   virtual Int_t     Prestart();
   virtual Int_t     Pause();
   virtual Int_t     Resume();
   virtual Int_t     Start();
   virtual void      Stop();
I 3
   virtual void      SetFile(const char *filnam);
   virtual void      GetFile(char *filnam);
E 3
   virtual void      DstOpen();
   virtual void      DstClose();
   virtual Int_t     NewEvent(Int_t type);
   virtual Int_t     GetEvent();
   virtual void      ChangeClient(Option_t *dtext = "");

   virtual void      Clear(Option_t *option ="");
   virtual void      Reset();
   virtual void      Loop();
   virtual Int_t     InitET();
   virtual void      AddClient(CMClient *Client=(CMClient *)0);
   virtual void      RemoveClient(CMClient *Client=(CMClient *)0);
   CMDisplay        *Display() {return m_Display;}

   ClassDef(CMDaq, 0)   //Daq for Moller Polarimeter
};

#endif





E 1
