h52518
s 00254/00000/00000
d D 1.1 06/10/19 15:41:13 boiarino 1 0
c date and time created 06/10/19 15:41:13 by boiarino
e
u
U
f e 0
t
T
I 1
//////////////////////////////////////////////////////////////////////////
//                                                                      //
//  CMTline                                                             //
//                                                                      //
//  CLAS Monitor Timeline graph                                         //
//                                                                      //
//    create one timeline plot with up to NTLGRAPH graphs on it         //
//                                                                      //
//////////////////////////////////////////////////////////////////////////
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#include <TROOT.h>
#include <TRandom.h>
#include <TButton.h>
#include <TLine.h>
#include <TArc.h>

#include "CMTline.h"
CMTline *gCMTline; // to use it in buttons


ClassImp(CMTline)



//_____________________________________________________________________________
CMTline::CMTline(Option_t *name)
{
// Create one CMTline object

  Int_t igr;

  strcpy(grname, name);
  printf("CMTline: graph name >%s<\n",grname);

  gCMTline = this;
  hist = NULL;
  ngraph = 0;
  timeinterval = 60; // default interval is one minute

  for(igr=0; igr<NTLGRAPH; igr++)
  {
    Pline[igr] = NULL;
    gralarmbeep[igr] = 0;
  }

  // colors for graphs
  colortable[0] = 1;
  colortable[1] = 2;
  colortable[2] = 3;
  colortable[3] = 4;
  colortable[4] = 5;
  colortable[5] = 6;
  colortable[6] = 7;
  colortable[7] = 13;
  colortable[8] = 14;
  colortable[9] = 15;

  printf("CMTline constractor called\n");
}


//_____________________________________________________________________________
void CMTline::SetPadAddress(TPad *pad1)
{
  Float_t xmin;

  pad = pad1;
  xmin = - ((Float_t)timeinterval);
  printf("CMTline::SetPadAddress: pad address 0x%08x\n",(Int_t)pad);

  pad->cd();
  hist = new TH1F("hist",grname,timeinterval,xmin,0.);
  hist->SetStats(0); // do not need statistic box
}


//_____________________________________________________________________________
void CMTline::SetTimeInterval(ULong_t seconds)
{
  Float_t xmin;
  Int_t nbins;

  xmin = - ((Float_t)seconds);
  nbins = timeinterval = seconds;
  if(nbins > 1000) nbins = 1000;
  printf("CMTline::SetTimeInterval: new xmin=%f nbins=%d\n",xmin,nbins);
  if(hist != NULL) delete hist;
  //pad->cd();
  hist = new TH1F("hist",grname,nbins,xmin,0.);
  hist->SetStats(0); // do not need statistic box
}


//_____________________________________________________________________________
void CMTline::SetAlarmBeep(Int_t igr, Int_t on_off)
{
  if(on_off == 0) gralarmbeep[igr] = 0;
  else            gralarmbeep[igr] = 1;
}


//_____________________________________________________________________________
void CMTline::SetAlarmLimits(Int_t igr, Float_t min, Float_t max)
{
  grminalarm[igr] = min;
  grmaxalarm[igr] = max;
}


//_____________________________________________________________________________
Int_t CMTline::AddGraph()
{
  Int_t i;

  if(ngraph < NTLGRAPH)
  {
    printf("CMTline::AddGraph: pad address 0x%08x\n",(Int_t)pad);

    npoints[ngraph] = 0;
    for(i=0; i<NTLDIM2; i++)
    {
      times[ngraph][i] =  0;
      graph[ngraph][i] =  0.0;
    }
    ngraph ++;

    printf("CMTline::AddGraph: graph number %d was added\n",ngraph-1);
    return(ngraph-1);
  }
  else
  {
    printf("CMTline::AddGraph: reached the max number of graphs = %d\n",
            NTLGRAPH);
    printf("CMTline::AddGraph: new graph was NOT created !!!\n");
    return(-1);
  }

}


//_____________________________________________________________________________
void CMTline::AddPointToGraph(Int_t igr, ULong_t unixtime, Float_t value)
{
  // if array is full, copy second half of it to the first half
  // so first half will be lost
  if(npoints[igr] >= NTLDIM2)
  {
    printf("CMTline::AddPointToGraph: copy half2->half1 ...");
    memcpy(&times[igr][0], &times[igr][NTLDIM], NTLDIM*sizeof(ULong_t));
    memcpy(&graph[igr][0], &graph[igr][NTLDIM], NTLDIM*sizeof(Float_t));
    printf(" done.\n");
    npoints[igr] -= NTLDIM;
  }

  // add new point
  times[igr][npoints[igr]] = unixtime;
  graph[igr][npoints[igr]] = value;
  npoints[igr] ++;

  // check alarm limits and beep if enabled
  if(gralarmbeep[igr])
  {
    if(value > grmaxalarm[igr])
    {
      printf("CMTline::AddPointToGraph: value=%f > max_limit=%f\n",
        value,grmaxalarm[igr]);
      printf("\007");
      printf("\007"); fflush(stdout);
    }
    if(value < grminalarm[igr])
    {
      printf("CMTline::AddPointToGraph: value=%f < min_limit=%f\n",
        value,grminalarm[igr]);
      printf("\007"); fflush(stdout);
    }
  }

  //printf("CMTline::AddPointToGraph: time=%lu value=%f\n",unixtime,value);
}


//_____________________________________________________________________________
void CMTline::Update()
{
  Int_t igr;                         // graph index
  Int_t first;                       // first point to be displayed
  Int_t k, kk[NTLGRAPH];             // the number of points to be displayed
  Int_t j;                           // point index
  Float_t mytime[NTLGRAPH][NTLDIM];  // time array for drawing (arguments)
  Float_t myvalue[NTLGRAPH][NTLDIM]; // value array for drawing (functions)
  Float_t dh;                        // vertical gap

  minlimit = MAXTLVALUE;
  maxlimit = MINTLVALUE;
  for(igr=0; igr<ngraph; igr++)
  {
    k = 0;
    for(j=npoints[igr]-1; j>=0; j--)
    {
      ULong_t dt;
      dt = current_time - times[igr][j];
      if(dt < timeinterval)
      {
        mytime[igr][k] = - ( (Float_t)dt );
        myvalue[igr][k] = graph[igr][j];
        if(minlimit > myvalue[igr][k]) minlimit = myvalue[igr][k];
        if(maxlimit < myvalue[igr][k]) maxlimit = myvalue[igr][k];
        k ++;
      }
    }
    kk[igr] = k;
  }

  dh = (maxlimit - minlimit)/10.0;
  hist->SetMinimum(minlimit-dh);
  hist->SetMaximum(maxlimit+dh);

  pad->cd();
  hist->Draw();

  current_time = time(0);
  //printf("CMTline::Update: current_time = %lu\n",current_time);

  for(igr=0; igr<ngraph; igr++)
  {
    k = kk[igr];
    if(k > 1)
    {
      first = npoints[igr] - k;

      //printf("CMTline::Update: table for the graph %d:\n",igr);
      //for(j=0; j<k; j++) printf(" time=%f value=%f\n",
      //mytime[igr][j],myvalue[igr][j]);

      if(Pline[igr]) {delete Pline[igr]; Pline[igr] = NULL;}
      Pline[igr] = new TPolyLine(k, mytime[igr], myvalue[igr], " ");
      Pline[igr]->SetLineColor(colortable[igr]);
      Pline[igr]->Draw();
    }
  }

}






E 1
