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
 
