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
