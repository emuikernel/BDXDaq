/*DECK ID>, BOSLDROP. */
/*
*/
 
#include "bos.h"
 
#ifndef VAX
 
void bosldrop_(int *jw, char *list, int lenlist)
{
  char *lst;
 
  lst = (char *) MALLOC(lenlist+1);
  strncpy(lst,list,lenlist);
  lst[lenlist] = '\0';
  bosLdrop(jw,lst);
  FREE(lst);
}
 
#endif
 
int bosLdrop(int *jw, char *list)
{
  BOSptr b;
  int *w, jls, nls, ils, jnd, ind, ndif, indrop, nami;
 
  w = jw-1;
  b = (BOSptr)jw;
 
  if(b->ick != MAGIC) return(ARRAY_NOT_INITIALIZED);
  
  listn1(jw,list,&jls,&nls);
  ils = 0;
  nami = listn2(jw,list,&ils,&jls,&nls);
 
  while(nami != 0)
  {
    /* reset link banks */
    jnd = b->idl + nami - b->s->nsyst;
    if(w[jnd] != 0) w[w[jnd]] = 0;
    ind = nami + 1;
    while((ind=w[ind-INXT]) != 0)
    {
      ndif = w[ind] + NHW;
      b->ndn = b->ndn + ndif;
      b->ign = (b->ign < ind) ? b->ign : ind ;
      w[ind] = -ndif;
    }
    w[nami] = 0;
    nami = listn2(jw,list,&ils,&jls,&nls);
  }
 
  /* try to regain space */
 
  if((b->ign + b->ndn - (NHW-1)) == b->igp)
  {
    /* contigous space */
    b->igp = b->ign - (NHW-1);
    b->ndn = 0;
    b->ign = b->njw;
  }
  else
  {
    /* not contiguous space */
    indrop = 0;
    ind = b->ign;
    while(ind < b->igp)
    {
      if(w[ind] < 0)
      {
        if(indrop == 0) indrop = ind;
        ind = ind - w[ind];
      }
      else
      {
        ind = ind + w[ind] + NHW;
        indrop = 0;
      }
    }
    if(indrop != 0)
    {
      b->ndn = b->ndn + indrop - (NHW-1) - b->igp;
      b->igp = indrop - (NHW-1);
    }
  }
  b->s->jw1 = 0;
 
  return(0);
}
 
