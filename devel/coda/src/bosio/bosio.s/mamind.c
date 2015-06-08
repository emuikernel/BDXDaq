/*
  mamind.c - get name-index from name
*/
 
#include "bos.h"
 
#ifndef VAX
 
int
bosnamind_(int *jw, char *name, int namlen)
{
  int nama;
  nama = *((int *)name);
  return(mamind(jw,nama));
}

int
mamind_(int *jw, char *name, int namlen)
{
  int nama;
  nama = *((int *)name);
  return(mamind(jw,nama));
}
 
#endif
 
int
bosNamind(int *jw, char *name)
{
  int nama;
  nama = *((int *)name);
  return(mamind(jw,nama));
}

int
mamind(int *jw, int nama)
{
  BOSptr b;
  int *w, nwnam, inam, jnam;
 
 
  w = jw-1;
  b = (BOSptr)jw;
 
  if(b->ick != MAGIC) return(ARRAY_NOT_INITIALIZED);
  nwnam = w[b->idnam];	/* b->idnam - index of the work bank with names */
  w[b->idnam] = nama;
 
  /* calculate HASH index from name */
 
  inam = w[b->idnam-INR] + ( abs(nama) % b->s->nprim ) - 3;
  do
  {
    jnam = inam;
    inam = w[b->idptr+inam];
  }
  while(w[b->idnam+inam] != nama); /* compare with entry in bank of names */
 
  w[b->idnam] = nwnam;
  if(inam == 0)	/* add new entry in the bank of names */
  {
    if(nwnam+NHW > w[b->idnam-INR])	/* too many names used */
    {
      return(TOO_MANY_DIFFERENT_NAMES_USED);
    }
    inam = nwnam + 1;
    w[b->idnam] = inam;
    w[b->idnam+inam] = nama;
    w[b->idptr+jnam] = inam;
  }
  b->s->jw1 = 0;
 
  return(inam+b->s->nsyst);
}
 
