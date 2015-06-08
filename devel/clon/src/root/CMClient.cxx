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

