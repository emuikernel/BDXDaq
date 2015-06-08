h21274
s 00074/00000/00000
d D 1.1 06/10/19 15:41:14 boiarino 1 0
c date and time created 06/10/19 15:41:14 by boiarino
e
u
U
f e 0
t
T
I 1
#ifndef CMViewer_H
#define CMViewer_H

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// CMViewer                                                             //
//                                                                      //
// CLAS Monitor Viewer                                                  //
//                                                                      //
//////////////////////////////////////////////////////////////////////////


#ifndef ROOT_TObject
#include <TObject.h>
#endif
#include "TH1.h"
#include "TH2.h"
#include "TCanvas.h"
#include "TText.h"

#include "CMClient.h"
#include "CMHbook.h"
#include "CMMenu.h"

#define NVIEW 16
#define NPADH 6
#define NPADV 6


class CMViewer {

private:

  Int_t nview;                     // the number of active views
  Int_t nh[NVIEW];                 // Ndiv horizontal
  Int_t nv[NVIEW];                 // Ndiv vertical

  Int_t ids[NVIEW][NPADH][NPADV];     // histogram ids
  Int_t key[NVIEW][NPADH][NPADV];     // key: "    ", "HIST", "TMLN", "STAT"
  TPad *pad[NVIEW][NPADH][NPADV];     // pad pointers

  char  filename[1000];
   
public:
                    CMViewer(Option_t *option="tmp.tmp");
  virtual          ~CMViewer() {;}

  // get view info from config file
  virtual int       ReadConfig(Option_t *fname, CMHbook *hbook);

  // set view info directly
  virtual int       AddView();
  virtual int       AddView(Int_t ih, Int_t iv);

  virtual void      AddHist(CMHbook *hbook, Int_t number, Int_t iview,
                            Int_t ih, Int_t iv, Int_t iprompt);
  virtual void      AddHist(CMHbook *hbook, Int_t number, Int_t iview,
                            Int_t iprompt);


  virtual void      WriteConfig(Option_t *fname);
  virtual void      ViewCreate(Int_t num, CMHbook *hbook);
  virtual void      ViewDelete(Int_t num, CMHbook *hbook);
  virtual void      ViewPlot(Int_t num);

  virtual TPad     *GetPad(Int_t iview, Int_t ipadh, Int_t ipadv)
                      {return(pad[iview][ipadh][ipadv]);}

  ClassDef(CMViewer, 0)
};

#endif


E 1
