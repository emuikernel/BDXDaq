h11451
s 00000/00000/00000
d R 1.2 02/04/25 09:51:21 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 clas/root/CMTline.h
e
s 00084/00000/00000
d D 1.1 02/04/25 09:51:20 boiarino 1 0
c date and time created 02/04/25 09:51:20 by boiarino
e
u
U
f e 0
t
T
I 1
#ifndef CMTline_H
#define CMTline_H

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// CMTline                                                              //
//                                                                      //
// CLAS Monitor Timeline graph                                          //
//                                                                      //
//////////////////////////////////////////////////////////////////////////


#ifndef ROOT_TObject
#include <TObject.h>
#endif
#include "TH1.h"
#include "TCanvas.h"
#include "TText.h"
#include "TPolyLine.h"

// max number of graphs on one timeline plot
#define NTLGRAPH 10

// max number of bins on one timeline plot
#define NTLDIM  3600
#define NTLDIM2 NTLDIM*2

// lowest and highest limits for the graph values
#define MINTLVALUE -100000.0
#define MAXTLVALUE  100000.0

class CMTline {

private:

  char       grname[1000];             // graph name
  TPad      *pad;                      // pad the graph to be displayed

  Int_t      ngraph;                   // the number of graphs booked
  Int_t      igraph;                   // current graph number

  TH1F      *hist;                     // histogram where to draw

  TPolyLine *Pline[NTLGRAPH];          // pointers to TPolyLine() objects
  Int_t      colortable[NTLGRAPH];     // graph colors
  Float_t    grminalarm[NTLGRAPH];     // lower limit to be alarmed
  Float_t    grmaxalarm[NTLGRAPH];     // upper limit to be alarmed
  Int_t      gralarmbeep[NTLGRAPH];    // beep ON/OFF

  Int_t      npoints[NTLGRAPH];        // the number of points in a graph
  ULong_t    times[NTLGRAPH][NTLDIM2]; // UNIX time for every bin
  Float_t    graph[NTLGRAPH][NTLDIM2]; // graphs as arrays of points

  ULong_t    current_time;             // current UNIX time
  ULong_t    timeinterval;             // time interval (seconds) to be drawn
  Float_t    minlimit, maxlimit;       // min and max limits for the graphs

public:
                   CMTline(Option_t *option="Timeline Graph");
  virtual         ~CMTline() {;}

  // entire class methods
  virtual void     SetPadAddress(TPad *pad1);
  //virtual void     SetPadAddress(Int_t id, TPad *pad1);
  virtual void     SetTimeInterval(ULong_t seconds);

  // graph-related methods
  virtual Int_t    AddGraph();
  virtual void     SetAlarmBeep(Int_t igr, Int_t on_off);
  virtual void     SetAlarmLimits(Int_t igr, Float_t min, Float_t max);
  virtual void     AddPointToGraph(Int_t igr, ULong_t unixtime, Float_t value);
  virtual void     Update();

  ClassDef(CMTline, 0)
};

#endif







E 1
