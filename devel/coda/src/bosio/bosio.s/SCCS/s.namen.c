h59503
s 00000/00000/00000
d R 1.2 70/01/01 00:00:02 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 clas/bosio/bosio.s/namen.c
e
s 00029/00000/00000
d D 1.1 00/08/10 11:10:15 boiarino 1 0
c date and time created 00/08/10 11:10:15 by boiarino
e
u
U
f e 0
t
T
I 1
/*DECK ID>, NAMEN. */
/*
*/
 
#include "bos.h"
 
int namen(int *jw, int nama)
{
  BOSptr b;
  int *w, nami, nwnam;
 
  w = jw-1;
  b = (BOSptr)jw;
 
  if(b->ick != MAGIC) return(ARRAY_NOT_INITIALIZED);
  nwnam = w[b->idnam];
  w[b->idnam] = nama;
  nami = w[b->idnam-INR] + ( abs(w[b->idnam]) % b->s->nprim ) - 3;
  do
  {
    nami = w[b->idptr+nami];
  }
  while(w[b->idnam+nami] != w[b->idnam]);
  w[b->idnam] = nwnam;
  nami = nami + b->s->nsyst;
  if(nami == b->s->nsyst) nami = mamind(jw,nama);
  return(nami);
}
 
E 1
