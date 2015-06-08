h00948
s 00027/00000/00000
d D 1.1 06/10/19 15:41:14 boiarino 1 0
c date and time created 06/10/19 15:41:14 by boiarino
e
u
U
f e 0
t
T
I 1
#ifndef CMVirtualDisplay_H
#define CMVirtualDisplay_H

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// CMVirtualDisplay                                                     //
//                                                                      //
// Virtual base class for CLAS Monitor display                          //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#ifndef ROOT_TObject
#include <TObject.h>
#endif

class CMVirtualDisplay : public TObject {

public:
                     CMVirtualDisplay();
   virtual          ~CMVirtualDisplay();
   virtual void      Clear(Option_t *option="") = 0;
   virtual void      DisplayButtons() = 0;

   ClassDef(CMVirtualDisplay, 0)   //Virtual base class for Moller Polarimeter display
};

#endif
E 1
