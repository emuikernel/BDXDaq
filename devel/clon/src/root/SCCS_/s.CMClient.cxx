h33073
s 00000/00000/00000
d R 1.2 01/11/19 16:44:43 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 clas/root/CMClient.cxx
e
s 00079/00000/00000
d D 1.1 01/11/19 16:44:42 boiarino 1 0
c date and time created 01/11/19 16:44:42 by boiarino
e
u
U
f e 0
t
T
I 1
//////////////////////////////////////////////////////////////////////////
//                                                                      //
// CMClient                                                             //
//                                                                      //
// CLAS Monitor Client class                                            //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>

#include <TROOT.h>
#include <TText.h>
#include <TPad.h>
#include <TArc.h>

#include "CMClient.h"

#include "CMDisplay.h" // to get gCMDisplay
extern CMDisplay *gCMDisplay;

#include "CMDaq.h" // to get gCMDaq
extern CMDaq *gCMDaq;

ClassImp(CMClient)

//_____________________________________________________________________________
CMClient::CMClient(Option_t *fname) : TObject()
{
// Create a epics object.

  m_Pad = gCMDisplay->Pad();         // Pad to ...
  m_StatPad = gCMDisplay->StatPad(); // Pad to ...
  printf("CMClient constructor: m_Pad=0x%08x m_StatPad=0x%08x\n",m_Pad,m_StatPad);
  gCMDaq->AddClient(this);   // register myself into gCMDaq object

}

//_____________________________________________________________________________
void CMClient::Download()
{
// call it ones and forever ...

}

//_____________________________________________________________________________
void CMClient::Prestart(Int_t *jw)
{
// call it in the begining of every run

}

//_____________________________________________________________________________
int CMClient::Event(Int_t *jw)
{
// Process one event

  Int_t ret = 0;

  return(ret);
}


//______________________________________________________________________________
void CMClient::Reset()
{
// Reset some variables

}



//______________________________________________________________________________
void CMClient::Update()
{
// update drawing

}

E 1
