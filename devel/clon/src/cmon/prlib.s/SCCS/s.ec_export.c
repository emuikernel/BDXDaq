h15985
s 00000/00000/00000
d R 1.2 01/11/19 18:57:05 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 clas/cmon/prlib.s/ec_export.c
e
s 00069/00000/00000
d D 1.1 01/11/19 18:57:04 boiarino 1 0
c date and time created 01/11/19 18:57:04 by boiarino
e
u
U
f e 0
t
T
I 1

/* ec_export.c - fills some BOS banks

  input:  nhits, hit

*/

#include <stdio.h>
#include "eclib.h"

int
ec_makeECPI(int *jw, int nhits, ECHitPtr hit)
{
  ECHitPtr hitin;
  ECCedPtr ced;
  int nr, ncol, ind, i, j, nhl;

  /* create bank */

  nr = 0;
  ncol = sizeof(ECCed)/4;
  ind = etNcreate(jw,"ECPI",nr,ncol,nhits);
  if(ind <= 0) return(-1);

  /* fill bank */

  hitin = hit;
  ced = (ECCedPtr)&jw[ind];
  nhl = 1;
  for(i=0; i<nhits; i++)
  {
    ced->sector = hitin->sector;    /* sector number */
    ced->layer=hitin->layer+9;      /* layer number: 3->9-whole, 1->10-inner, 2->11-outer */
      if(ced->layer==12) ced->layer=9;
    if(i > 0)
    {
      if(ced->sector == (ced-1)->sector && ced->layer == (ced-1)->layer)
      {
        nhl++;
      }
      else
      {
        for(j=1; j<=nhl; j++) {(ced-j)->nhl = nhl; (ced-j)->ihl = nhl-(j-1);}
        nhl = 1;
      }
    }
    ced->iloc   = hitin->i;         /* position of the hit in the local coordinate system */
    ced->jloc   = hitin->j;         /* position of the hit in the local coordinate system */
    ced->diloc  = hitin->di;        /* i width of the hit */
    ced->djloc  = hitin->dj;        /* j width of the hit */
    ced->radius = hitin->width;     /* radius of the shower */
    ced->energy = hitin->energy;    /* energy */
    ced++;
    hitin++;
  }
  for(j=1; j<=nhl; j++) {(ced-j)->nhl = nhl; (ced-j)->ihl = nhl-(j-1);} /* for last hit */

/*
ced = (ECCedPtr)&jw[ind];
for(i=0; i<nhits; i++)
{
  printf("sec,lay,nhl,ihl,i,j,di,dj,width,energy=%d %d %d %d %f %f %f %f %f %f\n",
ced->sector,ced->layer,ced->nhl,ced->ihl,ced->iloc,ced->jloc,ced->diloc,ced->djloc,ced->radius,ced->energy);
  ced++;
}
*/

  return(ind);
}
E 1
