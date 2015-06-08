/*
   Purpose and Methods : This routine gives access to a event bank 'EVTB'
                         for Level = 2 or 4
                         
   Inputs  : ev_out(36,itr)
   Outputs : printout
   Controls: none

   Library belongs: libsda.a

   Created   18-SEP-1997   Bogdan Niczyporuk
   C remake by Sergey Boyarinov

   Called by dcfit
*/

#include <stdio.h>
#include "dclib.h"
#include "sdakeys.h"
SDAkeys sdakeys_;

#define NITEM 36
#define NITEMT 7

void
dcpdst_(int *ntr_out,float ev_out[ntrmx][36], int *ntagh,float tag_res[TAG_MAX_HITS][7])
{
  dcpdst(*ntr_out, ev_out, *ntagh, tag_res);
  return;
}

void
dcpdst(int ntr_out,float ev_out[ntrmx][36], int ntagh,float tag_res[TAG_MAX_HITS][7])
{
  int i, itr, ntr, nhit, ih;
                          /* "Xv   " */
  static char *des[NITEM] = {"trk0 ","Yv   ","Zv   ","CXv  ","CYv  ","CZv  ",
                             "p    ","beta ","Q    ","ilink","Sec/L","Chi2 ",
                             "Xcc  ","Ycc  ","Zcc  ","CXcc ","CYcc ","CZcc ",
                             "Scc  ","IDcc ","Xsc  ","Ysc  ","Zsc  ","CXsc ",
                             "CYsc ","CZsc ","Ssc  ","IDsc ","Xec  ","Yec  ",
                             "Zec  ","CXec ","CYec ","CZec ","Sec  ","IDuvw"};
  static char *dest[NITEMT]={"IdE  ","IdT  ","EnE  ","EnT  ","TimE ","TimT ",
                             "TimTc"};

  printf("\n  DST:[EVTB bank]  Run =%6d  Evt =%8d  Ntrk =%3d\n",
               sdakeys_.ltrig[0],sdakeys_.ievt,ntr_out);
  ntr = ntr_out;
  if(ntr > 6) ntr = 6;
  printf("  No item     track1    track2    track3    track4"
         "    track5    track6\n");
  for(i=0; i<NITEM; i++)
  {
    printf("%4d %5s",i+1,des[i]);
    for(itr=0; itr<ntr; itr++) printf("%10.3f",ev_out[itr][i]);
    printf("\n");
  }

  if(sdakeys_.zbeam[0] == 0.)
  {
    nhit = ntagh;
    if(nhit>6) nhit=6;
    printf("\n  No item     TGhit1    TGhit2    TGhit3    TGhit4"
           "    TGhit5    TGhit6\n");
    for(i=0; i<NITEMT; i++)
    {
      printf("%4d %5s",i+1,dest[i]);
      for(ih=0; ih<nhit; ih++) printf("%10.3f",tag_res[ih][i]);
      printf("\n");
    }
  }

  return;
}
