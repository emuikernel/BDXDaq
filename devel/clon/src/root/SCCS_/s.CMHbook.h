h35120
s 00005/00002/00069
d D 1.3 03/06/20 17:30:50 boiarino 4 3
c make hist1 and hist2 global
e
s 00020/00007/00051
d D 1.2 02/03/26 15:17:42 boiarino 3 1
c *** empty log message ***
e
s 00000/00000/00000
d R 1.2 01/11/19 16:44:45 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 clas/root/CMHbook.h
e
s 00058/00000/00000
d D 1.1 01/11/19 16:44:44 boiarino 1 0
c date and time created 01/11/19 16:44:44 by boiarino
e
u
U
f e 0
t
T
I 1
#ifndef CMHbook_H
#define CMHbook_H

//////////////////////////////////////////////////////////////////////////
//                                                                      //
D 3
// CMHbook                                                                //
E 3
I 3
// CMHbook                                                              //
E 3
//                                                                      //
// CLAS Online Analisys                                                 //
//                                                                      //
//////////////////////////////////////////////////////////////////////////


#ifndef ROOT_TObject
#include <TObject.h>
#endif
#include "TH1.h"
#include "TH2.h"
#include "TCanvas.h"
#include "TText.h"

#include "uthbook.h"
I 3
#include "CMTline.h"
E 3

D 3

E 3
class CMHbook {

private:

D 3
  TH1F    *hist1[NHIST];
  TH2F    *hist2[NHIST];
  UThisti  histi[NHIST];
  UThistf  histf[NHIST];
  TPad    *pads[NHIST];       // pointers to  pads
E 3
I 3
  UThisti  histi[NHIST];  // 'cmon' format histos,
  UThistf  histf[NHIST];  //    initially filled after reading BOS banks 
E 3

I 3
D 4
  TH1F    *hist1[NHIST];  // ROOT histograms, finally
  TH2F    *hist2[NHIST];  //    filled by current class
E 4
I 4
  //TH1F    *hist1[NHIST];  // ROOT histograms, finally
  //TH2F    *hist2[NHIST];  //    filled by current class
E 4

  CMTline *tline[NHIST];  // time plot histograms, converted from ROOT
                          //    histograms on demand (method ...)
  Int_t tlinebin[NHIST][NTLGRAPH]; // bin# (from 1) to be displayed
  Int_t tlinenormbin[NHIST][NTLGRAPH]; // bin# (from 1) to normalize at

  TPad    *pads[NHIST];   // pointers to  pads
  Int_t   prompt[NHIST];  // prompt flags (1-cleanup every time, 0-accumulate)

E 3
  virtual Int_t     CMhisti2root(UThisti *hist, Int_t id);
  virtual Int_t     CMhistf2root(UThistf *hist, Int_t id);
  virtual Int_t     CMgetHISS(Int_t *jw, Int_t ind);
  virtual Int_t     CMgetHISI(Int_t *jw, Int_t ind);
  virtual Int_t     CMgetHISF(Int_t *jw, Int_t ind);
  virtual Int_t     CMgetHistFromBOS(Int_t *jw, Int_t id);


public:
                    CMHbook();
  virtual          ~CMHbook();

I 4
  TH1F    *hist1[NHIST];  // ROOT histograms, finally
  TH2F    *hist2[NHIST];  //    filled by current class

E 4
  virtual void      Clean();
  virtual void      SetListOfPads(TPad *pad[NHIST]);
  virtual void      SetPadAddress(Int_t id, TPad *pad);
I 3
  virtual void      SetPromptFlag(Int_t id, Int_t iprompt);
  virtual void      CreateTMLN(Int_t id, ULong_t timeinterval);
  virtual void      SetTMLNPad(Int_t id, TPad *pad);
  virtual void      SetTMLNGraph(Int_t id, Int_t ibin, Float_t min,
                                 Float_t max, Int_t alarm, Int_t norm);
E 3
  virtual Int_t     BosToRoot(Int_t *jw);
  virtual Int_t     HisToRoot();

  ClassDef(CMHbook, 0)
};

#endif



E 1
