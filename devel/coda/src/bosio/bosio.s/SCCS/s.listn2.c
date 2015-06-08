h01427
s 00000/00000/00000
d R 1.2 70/01/01 00:00:02 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 clas/bosio/bosio.s/listn2.c
e
s 00050/00000/00000
d D 1.1 00/08/10 11:10:15 boiarino 1 0
c date and time created 00/08/10 11:10:15 by boiarino
e
u
U
f e 0
t
T
I 1
/*DECK ID>, LISTN2. */
/*
     return nami - name-index for next name in list, or zero
*/
 
#include "bos.h"
 
int listn2(int *jw, char *list, int *ils, int *jls, int *nls)
{
  BOSptr b;
  int *w, *nama, nami;
 
  w = jw-1;
  b = (BOSptr)jw;
 
  if(*jls < 0)
  {
    nami = 0;
  }
  else if(*jls == 10)	/* list is bank names */
  {
    if(*ils < *nls)
    {
      *ils = (*ils) + 1;
      nama = (int *) &list[4*(*ils)-4];
      nami = namen(jw,*nama);
    }
    else
    {
      *jls = -1;
      nami = 0;
    }
  }
  else
  {
    if(*ils < *nls)
    {
      *ils = (*ils) + 1;
      nami = w[b->ilt[*jls]+(*ils)];
    }
    else
    {
      *jls = -1;
      nami = 0;
    }
  }
 
  return(nami);
}
 
E 1
