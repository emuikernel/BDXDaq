#ifndef CMVirtualDisplay_H
#define CMVirtualDisplay_H

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// CMVirtualDisplay                                                     //
//                                                                      //
// Virtual base class for CLAS Monitor display                          //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#ifndef ROOT_TGWindow
#include <TGWindow.h>
#endif

class CMVirtualDisplay : public TGWindow {

public:
                     CMVirtualDisplay();
   virtual          ~CMVirtualDisplay();
   virtual void      Clear(Option_t *option="") = 0;
   virtual void      DisplayButtons() = 0;

   ClassDef(CMVirtualDisplay, 0)   //Virtual base class for Moller Polarimeter display
};

#endif
