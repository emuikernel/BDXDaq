/*DECK ID>, LISTN2. */
/*
     return nami - name-index for next name in list, or zero
*/
 
#include "bos.h"
 
int listn2(int *jw, char *list, int *ils, int *jls, int *nls)
{
  BOSptr b;
  int *w, *nama, nami;
 
  w = jw-1;
  b = (BOSptr)jw;
 
  if(*jls < 0)
  {
    nami = 0;
  }
  else if(*jls == 10)	/* list is bank names */
  {
    if(*ils < *nls)
    {
      *ils = (*ils) + 1;
      nama = (int *) &list[4*(*ils)-4];
      nami = namen(jw,*nama);
    }
    else
    {
      *jls = -1;
      nami = 0;
    }
  }
  else
  {
    if(*ils < *nls)
    {
      *ils = (*ils) + 1;
      nami = w[b->ilt[*jls]+(*ils)];
    }
    else
    {
      *jls = -1;
      nami = 0;
    }
  }
 
  return(nami);
}
 
