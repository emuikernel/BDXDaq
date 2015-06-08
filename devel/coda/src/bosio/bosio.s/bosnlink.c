
/* bosnlink.c */
 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bos.h"
 
#ifndef VAX
 
int
bosnlink_(int *jw, char *name, int *nr, int lename)
{
  int status;
  char *nam;
 
  nam = (char *) MALLOC(lename+1);
  strncpy(nam,name,lename);
  nam[lename] = '\0';
  status = bosNlink(jw,nam,*nr);
  FREE(nam);
  return(status);
}
 
#endif
 
int
bosNlink(int *jw, char *name, int nr)
{
  BOSptr b;
  int *w, jnd, nlpl, i, nb, ind, knd, il, ih;
  int *nam, nama, nami, indi, indj;
 
  w = jw-1;
  b = (BOSptr)jw;

  memcpy(&nama,name,4);
  /*nam =  (int *)name;
  nama = *nam;*/

  if(b->ick != MAGIC) return(ARRAY_NOT_INITIALIZED);
  nami = namez(jw,nama);
  b->s->jw1 = 0;
  if(nami == 0) return(0);
  jnd = b->idl + nami - b->s->nsyst;
  if(w[jnd] == 0)
  {
    nlpl = nindex(jw,nr,nami,&indi,&indj);    /* use sequential search */
    if(nlpl > b->s->nlplm)	i = bosWcreate(jw, &w[jnd], 0, 1);
    return(indi);
  }
 
  /* binary search */
 
  if(w[w[jnd]] == 0)
  {
    nb = 0;	/* count the number of banks */
    ind = nami + 1;
    ind = w[ind-INXT];
    while(ind != 0)
    {
      nb++;
      ind = w[ind-INXT];
    }
 
    if(nb < b->s->nlplm)
    {
      i = bosWdrop(jw, &w[jnd]);	/* swith off binary search - ? */
      nlpl = nindex(jw,nr,nami,&indi,&indj);
      if(nlpl > b->s->nlplm) i = bosWcreate(jw, &w[jnd], 0, 1);
      return(indi);
    }
 
    if((w[w[jnd]-INR]-NHW) < (nb*2))
    {
      if(bosWcreate(jw, &w[jnd], nb*2+10, 1) < 0)
      {
        i = bosWdrop(jw, &w[jnd]);
        nlpl = nindex(jw,nr,nami,&indi,&indj);
        if(nlpl > b->s->nlplm) i = bosWcreate(jw, &w[jnd], 0, 1);
        return(indi);
      }
    }
    w[w[jnd]] = nb*2;
 
    /* fill number/index pairs into bank */
 
    knd = w[jnd];
    ind = nami + 1;
    ind=w[ind-INXT];
    while(ind != 0)
    {
      w[knd+1] = w[ind-INR];
      w[knd+2] = ind;
      knd = knd + 2;
      ind=w[ind-INXT];
    }
  }
 
  /* find number by binary search in link work bank */
 
  jnd = w[jnd];
  indi = 0;
  il = jnd + 1;
  ih = jnd + w[jnd] - 1;
  i = ih;
  if(w[il] <= nr && w[ih] >= nr)
  {
    if(w[il] == nr)
    {
      indi = w[il+1];
      return(indi);
    }
 
    do
    {
      if(w[i] < nr)
      {
        il = i;
      }
      else
      {
        ih = i;
      }
      i = il + ((ih-il)/4)*2;
    }
    while(i != il);
 
    if(w[i] == nr) indi = w[i+1];
    if(w[ih] == nr) indi = w[ih+1];
  }
 
  return(indi);
}
 




