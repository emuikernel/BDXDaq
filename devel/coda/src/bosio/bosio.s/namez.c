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
 
