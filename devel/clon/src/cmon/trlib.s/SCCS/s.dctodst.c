h42241
s 00002/00002/00084
d D 1.3 07/11/02 20:30:02 fklein 4 3
c *** empty log message ***
e
s 00000/00000/00086
d D 1.2 01/11/29 11:36:29 boiarino 3 1
c nothing
c 
e
s 00000/00000/00000
d R 1.2 01/11/19 19:06:01 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 clas/cmon/trlib.s/dctodst.c
e
s 00086/00000/00000
d D 1.1 01/11/19 19:06:00 boiarino 1 0
c date and time created 01/11/19 19:06:00 by boiarino
e
u
U
f e 0
t
T
I 1
/* NEED CHECK !!!
   Purpose and Methods : Writes the reconstructed event into DST. 
                         

   Inputs  :  common block

   Outputs :  DST (or MiniDST)

   Controls:

   Library belongs: libana.a

   Calls: BOS routines

   Created   AUG-14-1997  Bogdan Niczyporuk
   C remake by Sergey Boyarinov

   Called by sda_anal
*/

#include <stdio.h>
#include "dclib.h"
#include "sdakeys.h"
SDAkeys sdakeys_;

#define NcolEVT  40
#define NcolTGR  7

void
dctodst_(int *jw, int *ntr_out, float ev_out[ntrmx][36],
D 4
                                 int *ntagh, float tag_res[10][7])
E 4
I 4
                                 int *ntagh, float tag_res[TAG_MAX_HITS][7])
E 4
{
  dctodst(jw, *ntr_out, ev_out, *ntagh, tag_res);
  return;
}

void
dctodst(int *jw, int ntr_out, float ev_out[ntrmx][36],
D 4
                                 int ntagh, float tag_res[10][7])
E 4
I 4
                                 int ntagh, float tag_res[TAG_MAX_HITS][7])
E 4
{
  int i, it, id, ih, inde, indt, iret, tmp;
  float *rw;

  /* was if(ifirst) */
  {
    rw = (float *)jw;
    etNformat(jw,"EVTB","F");
    etNformat(jw,"TGR ","F");
    etNformat(jw,"TRAK","F");
  }

  etLctl(jw,"E=","HEADCALLTGBIDC0 CC  SC  EC  EC1 ");
  etLctl(jw,"E+","EVTBDC1 HBTRHBLAHDPLTBTRTBLATDPLECPI");
  if(sdakeys_.zbeam[0] == 0.) etLctl(jw,"E+","ST  TAGTTAGETGR ");
  /*etLctl(jw,"E+","PATH");*/
  /*
  etLctl(jw,"E=","HEADTGBITBTRTRAKDC1 ");
  */

  /* create event bank */
  inde = etNcreate(jw,"EVTB",0,NcolEVT,ntr_out);
  for(it=1; it<=ntr_out; it++)
  {
    id = (it - 1) * NcolEVT;
    for(i=1; i<=NcolEVT; i++) rw[inde+id+i-1] = ev_out[it-1][i-1];
  }

  /* create tagger bank */
  if(ntagh > 0)
  {
    indt = etNcreate(jw,"TGR ",0,NcolTGR,ntagh);
    for(ih=1; ih<=ntagh; ih++)
    {
      id = (ih - 1) * NcolTGR;
      for(i=1; i<=NcolTGR; i++) rw[indt+id+i-1] = tag_res[ih-1][i-1];
    }
  }

  tmp = 2;
  fwbos_(jw,&tmp,"E",&iret,1);
  /*do not drop it here
  etLdrop(jw,"E");
  etNgarb(jw);*/

  return;
}
E 1
