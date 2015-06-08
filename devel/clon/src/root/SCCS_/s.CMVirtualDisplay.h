h13851
s 00000/00000/00000
d R 1.2 01/11/19 16:44:47 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 clas/root/CMVirtualDisplay.h
e
s 00027/00000/00000
d D 1.1 01/11/19 16:44:46 boiarino 1 0
c date and time created 01/11/19 16:44:46 by boiarino
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
