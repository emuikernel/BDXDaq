h62835
s 00000/00000/00000
d R 1.2 70/01/01 00:00:02 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 clas/bosio/bosio.s/namez.c
e
s 00033/00000/00000
d D 1.1 00/08/10 11:10:16 boiarino 1 0
c date and time created 00/08/10 11:10:16 by boiarino
e
u
U
f e 0
t
T
I 1
/*DECK ID>, NAMEZ. */
/*
     RETURNS NAMI = NAME-INDEX FOR INTEGER NAMA
                  = 0 IF NAME NOT YET USED
*/
 
#include "bos.h"
#include <stdlib.h>
 
int namez(int *jw, int nama)
{
  BOSptr b;
  int *w, nami, nwnam;
 
  b = (BOSptr)jw;
  w = jw-1;
 
  nwnam = w[b->idnam];		/* save w[b->idnam] */
  w[b->idnam] = nama;
  nami = w[b->idnam-INR] + ( abs(nama) % b->s->nprim ) - 3;
 
  do
  {
    nami = w[b->idptr+nami];
  }
  while(w[b->idnam+nami] != w[b->idnam]);
 
  w[b->idnam] = nwnam;		/* restore w[b->idnam] */
  if(nami != 0) nami = nami + b->s->nsyst;
 
  return(nami);
}
 
E 1
