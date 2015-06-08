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
                                 int *ntagh, float tag_res[TAG_MAX_HITS][7])
{
  dctodst(jw, *ntr_out, ev_out, *ntagh, tag_res);
  return;
}

void
dctodst(int *jw, int ntr_out, float ev_out[ntrmx][36],
                                 int ntagh, float tag_res[TAG_MAX_HITS][7])
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
