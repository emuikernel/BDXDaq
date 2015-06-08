h32703
s 00001/00001/00073
d D 1.6 05/02/25 14:59:11 boiarino 7 6
c *** empty log message ***
e
s 00017/00002/00057
d D 1.5 03/04/17 17:01:44 boiarino 6 5
c *** empty log message ***
e
s 00001/00001/00058
d D 1.4 02/05/04 20:54:23 boiarino 5 4
c set NVIEW=11
c 
e
s 00001/00001/00058
d D 1.3 02/05/02 14:48:22 boiarino 4 3
c increase NPADH limit
e
s 00006/00005/00053
d D 1.2 02/03/26 15:18:40 boiarino 3 1
c *** empty log message ***
e
s 00000/00000/00000
d R 1.2 01/11/19 16:44:46 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 clas/root/CMViewer.h
e
s 00058/00000/00000
d D 1.1 01/11/19 16:44:45 boiarino 1 0
c date and time created 01/11/19 16:44:45 by boiarino
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

D 5
#define NVIEW 10
E 5
I 5
D 6
#define NVIEW 11
E 6
I 6
D 7
#define NVIEW 12
E 7
I 7
#define NVIEW 16
E 7
E 6
E 5
D 4
#define NPADH 5
E 4
I 4
#define NPADH 6
E 4
#define NPADV 6


class CMViewer {

private:

  Int_t nview;                     // the number of active views
  Int_t nh[NVIEW];                 // Ndiv horizontal
  Int_t nv[NVIEW];                 // Ndiv vertical

D 3
  Int_t ids[NVIEW][NPADH][NPADV];  // histogram ids
  TPad *pad[NVIEW][NPADH][NPADV];  // pad pointers
E 3
I 3
  Int_t ids[NVIEW][NPADH][NPADV];     // histogram ids
  Int_t key[NVIEW][NPADH][NPADV];     // key: "    ", "HIST", "TMLN", "STAT"
  TPad *pad[NVIEW][NPADH][NPADV];     // pad pointers
E 3

  char  filename[1000];
   
public:
                    CMViewer(Option_t *option="tmp.tmp");
  virtual          ~CMViewer() {;}

I 6
  // get view info from config file
E 6
D 3
  virtual int       ReadConfig(Option_t *option="./hist.config");
  virtual void      WriteConfig(Option_t *option="./hist.config");
  virtual void      ViewCreate(Int_t num, CMHbook  *hbook);
E 3
I 3
  virtual int       ReadConfig(Option_t *fname, CMHbook *hbook);
I 6

  // set view info directly
  virtual int       AddView();
  virtual int       AddView(Int_t ih, Int_t iv);

  virtual void      AddHist(CMHbook *hbook, Int_t number, Int_t iview,
                            Int_t ih, Int_t iv, Int_t iprompt);
  virtual void      AddHist(CMHbook *hbook, Int_t number, Int_t iview,
                            Int_t iprompt);


E 6
  virtual void      WriteConfig(Option_t *fname);
  virtual void      ViewCreate(Int_t num, CMHbook *hbook);
E 3
D 6
  virtual void      ViewDelete(Int_t num);
E 6
I 6
  virtual void      ViewDelete(Int_t num, CMHbook *hbook);
E 6
  virtual void      ViewPlot(Int_t num);

I 6
  virtual TPad     *GetPad(Int_t iview, Int_t ipadh, Int_t ipadv)
                      {return(pad[iview][ipadh][ipadv]);}

E 6
  ClassDef(CMViewer, 0)
};

#endif


E 1
