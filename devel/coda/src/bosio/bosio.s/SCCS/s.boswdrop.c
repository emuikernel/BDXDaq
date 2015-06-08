h45792
s 00000/00000/00000
d R 1.2 70/01/01 00:00:02 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 clas/bosio/bosio.s/boswdrop.c
e
s 00065/00000/00000
d D 1.1 00/08/10 11:10:07 boiarino 1 0
c date and time created 00/08/10 11:10:07 by boiarino
e
u
U
f e 0
t
T
I 1
/*DECK ID>, BOSWDROP. */
/*
   drop work banks with indices id[0] ... id[ndim]
*/
 
#include "bos.h"
 
#ifndef VAX
 
int boswdrop_(int *jw, int *id)
{
  return(bosWdrop(jw,id));
}
 
#endif
 
int bosWdrop(int *jw, int *id)
{
  BOSptr b;
  int *w, i, jd, ld, ndim;
 
  w = jw-1;			/* put pointers */
  b = (BOSptr)jw;
 
  if(b->ick != MAGIC) return(ARRAY_NOT_INITIALIZED);
  ndim = (1 > b->s->jw1) ? 1 : b->s->jw1 ;
  b->s->jw1 = 0;
  ld = ((unsigned)id>>2) - (((int)jw)>>2) + 1;	/* shift (4-byte words) */
 
  /* drop bank if index valid */
 
  for(i=0; i<ndim; i++)
  {
    if(id[i] != 0)
    {
      /* verify value of work bank index */
      if(id[i] <= b->iwk || id[i] >= b->njw || w[id[i]-INXT] != ld+i)
      {
        return(W_INDEX_INCORRECT);
      }
      else
      {
        b->ndw = b->ndw + w[id[i]-INR];	/* the number of dropped words */
        b->igw = (b->igw > id[i]+w[id[i]-INR]-(NHW-1)) ?
         b->igw : id[i]+w[id[i]-INR]-(NHW-1);/* index of highest deleted bank */
        w[id[i]] = -w[id[i]-INR];
        id[i]=0;
      }
    }
  }
 
  /* try to regain space of dropped banks */
 
  jd = b->iwk + (NHW-1);
  while(w[ld] < 0)
  {
    b->iwk = b->iwk - w[ld];
    b->ndw = b->ndw + w[ld];
    jd = jd - w[ld];
  }
  if(b->ndw == 0) b->igw = 0;
 
  return(0);
}
 
E 1
