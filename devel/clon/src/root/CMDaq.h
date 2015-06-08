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

   char              filename[1024];         // Data file name or ET file name
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
   virtual void      SetFile(const char *filnam);
   virtual void      GetFile(char *filnam);
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





