h30418
s 00000/00000/00000
d R 1.2 01/11/19 16:44:43 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 clas/root/CMClient.h
e
s 00039/00000/00000
d D 1.1 01/11/19 16:44:42 boiarino 1 0
c date and time created 01/11/19 16:44:42 by boiarino
e
u
U
f e 0
t
T
I 1
#ifndef CMClient_H
#define CMClient_H

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// CMClient                                                             //
//                                                                      //
// CLAS Monitor Client Class                                            //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#ifndef ROOT_TObject
#include <TObject.h>
#endif

class CMDisplay;

class CMClient : public TObject {

protected:

   TPad             *m_Pad;              // Pad to draw histograms/counters
   TPad             *m_StatPad;          // StatPad to draw status

public:
                     CMClient(Option_t *option="tmp.tmp");
   virtual          ~CMClient() {;}
   virtual void      Download();
   virtual void      Prestart(Int_t *jw);
   virtual void      Go(Int_t *jw) {;}
   virtual void      End(Int_t *jw) {;}
   virtual void      Reset();
   virtual void      Update();
   virtual int       Event(Int_t *jw);

   ClassDef(CMClient, 0)   // Client for the CLAS Monitor
};

#endif
E 1
