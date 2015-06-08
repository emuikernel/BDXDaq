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
 
