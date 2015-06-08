#ifndef CMHbook_H
#define CMHbook_H

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// CMHbook                                                              //
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
#include "CMTline.h"

class CMHbook {

private:

  UThisti  histi[NHIST];  // 'cmon' format histos,
  UThistf  histf[NHIST];  //    initially filled after reading BOS banks 

  //TH1F    *hist1[NHIST];  // ROOT histograms, finally
  //TH2F    *hist2[NHIST];  //    filled by current class

  CMTline *tline[NHIST];  // time plot histograms, converted from ROOT
                          //    histograms on demand (method ...)
  Int_t tlinebin[NHIST][NTLGRAPH]; // bin# (from 1) to be displayed
  Int_t tlinenormbin[NHIST][NTLGRAPH]; // bin# (from 1) to normalize at

  TPad    *pads[NHIST];   // pointers to  pads
  Int_t   prompt[NHIST];  // prompt flags (1-cleanup every time, 0-accumulate)

  virtual Int_t     CMhisti2root(UThisti *hist, Int_t id);
  virtual Int_t     CMhistf2root(UThistf *hist, Int_t id);
  virtual Int_t     CMgetHISS(Int_t *jw, Int_t ind);
  virtual Int_t     CMgetHISI(Int_t *jw, Int_t ind);
  virtual Int_t     CMgetHISF(Int_t *jw, Int_t ind);
  virtual Int_t     CMgetHistFromBOS(Int_t *jw, Int_t id);


public:
                    CMHbook();
  virtual          ~CMHbook();

  TH1F    *hist1[NHIST];  // ROOT histograms, finally
  TH2F    *hist2[NHIST];  //    filled by current class

  virtual void      Clean();
  virtual void      SetListOfPads(TPad *pad[NHIST]);
  virtual void      SetPadAddress(Int_t id, TPad *pad);
  virtual void      SetPromptFlag(Int_t id, Int_t iprompt);
  virtual void      CreateTMLN(Int_t id, ULong_t timeinterval);
  virtual void      SetTMLNPad(Int_t id, TPad *pad);
  virtual void      SetTMLNGraph(Int_t id, Int_t ibin, Float_t min,
                                 Float_t max, Int_t alarm, Int_t norm);
  virtual Int_t     BosToRoot(Int_t *jw);
  virtual Int_t     HisToRoot();

  ClassDef(CMHbook, 0)
};

#endif



