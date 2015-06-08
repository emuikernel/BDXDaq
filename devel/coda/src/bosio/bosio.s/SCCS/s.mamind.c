h52612
s 00000/00000/00000
d R 1.2 70/01/01 00:00:02 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 clas/bosio/bosio.s/mamind.c
e
s 00075/00000/00000
d D 1.1 00/08/10 11:10:15 boiarino 1 0
c date and time created 00/08/10 11:10:15 by boiarino
e
u
U
f e 0
t
T
I 1
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
 
E 1
