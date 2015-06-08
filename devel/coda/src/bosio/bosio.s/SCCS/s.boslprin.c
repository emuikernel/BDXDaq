h03107
s 00000/00000/00000
d R 1.2 70/01/01 00:00:02 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 clas/bosio/bosio.s/boslprin.c
e
s 00047/00000/00000
d D 1.1 00/08/10 11:10:03 boiarino 1 0
c date and time created 00/08/10 11:10:03 by boiarino
e
u
U
f e 0
t
T
I 1
/*DECK ID>, BOSLPRIN. */
/*
 print list of banks
*/
 
#include "bos.h"
 
#ifndef VAX
 
void boslprint_(int *jw, char *list, int lenlist)
{
  char *lst;
 
  lst = (char *) MALLOC(lenlist+1);
  strncpy(lst,list,lenlist);
  lst[lenlist] = '\0';
  bosLprint(jw,lst);
  FREE(lst);
}
 
#endif
 
bosLprint(int *jw, char *list)
{
  BOSptr b;
  int *w, jls, nls, ils, ind, nami;
 
  w = jw-1;
  b = (BOSptr)jw;
 
  if(b->s->lub > 0)
  {
    listn1(jw,list,&jls,&nls);
    ils = 0;
a:
    if((nami = listn2(jw,list,&ils,&jls,&nls)) > 0)
    {
      ind = nami + 1;
      while((ind = w[ind-INXT]) != 0)
      {
        bosprint(jw,ind);
      }
      goto a;
    }
  }
}
 
E 1
