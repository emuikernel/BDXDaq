h01502
s 00000/00000/00000
d R 1.2 70/01/01 00:00:02 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 clas/bosio/bosio.s/bosnprin.c
e
s 00045/00000/00000
d D 1.1 00/08/10 11:10:05 boiarino 1 0
c date and time created 00/08/10 11:10:05 by boiarino
e
u
U
f e 0
t
T
I 1
/*DECK ID>, BOSNPRIN. */
/*
 print named bank
*/
 
#include "bos.h"
 
#ifndef VAX
 
void
bosnprint_(int *jw, char *name, int *nr, int lenname)
{
  char *nam;
 
  nam = (char *) MALLOC(lenname+1);
  strncpy(nam,name,lenname);
  nam[lenname] = '\0';
  bosNprint(jw,nam,*nr);
  FREE(nam);
}
 
#endif
 
void
bosNprint(int *jw, char *name, int nr)
{
  BOSptr b;
  int ind;
 
  b = (BOSptr)jw;
 
  if(b->s->lub > 0)
  {
    if((ind=bosNlink(jw,name,nr)) > 0)
    {
      bosprint(jw,ind);
    }
    else
    {
      printf("\nbosNprint: can't print bank \"%4.4s\", number=%d : ind=%d\n\n",
                  name,nr,ind);
    }
  }
}
 
E 1
