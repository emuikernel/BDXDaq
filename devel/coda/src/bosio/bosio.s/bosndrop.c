/*DECK ID>, BOSNDROP. */
/*
*/
 
#include "bos.h"
 
#ifndef VAX
 
int bosndrop_(int *jw, char *name, int *nr, int lename)
{
  int status;
  char *nam;
 
  nam = (char *) MALLOC(lename+1);
  strncpy(nam,name,lename);
  nam[lename] = '\0';
  status = bosNdrop(jw,nam,*nr);
  FREE(nam);
  return(status);
}
 
#endif
 
int bosNdrop(int *jw, char *name, int nr)
{
  BOSptr b;
  int *w, jw1, nlpl, ndif, jnd;
  int *nam, nama, nami, indi, indj;
 
  w = jw-1;
  b = (BOSptr)jw;
  nam =  (int *)name;
  nama = *nam;
 
  if(b->ick != MAGIC) return(ARRAY_NOT_INITIALIZED);
  jw1 = b->s->jw1;
  b->s->jw1 = 0;
  nami = namez(jw,nama);
  if(nami == 0) return(0);
  nlpl = nindex(jw,nr,nami,&indi,&indj);
  if(indi != 0)
  {
    /* drop bank */
    w[indj-INXT] = w[indi-INXT];
    ndif = w[indi] + NHW;
    if(jw1 != 1 && b->igp == indi + ndif - (NHW-1))	/* last bank */
    {
      b->igp = b->igp - ndif;
    }
    else			/* not last bank or jw1=1 (force drop) */
    {
      w[indi] = -ndif;
      b->ndn = b->ndn + ndif;
      b->ign = (b->ign < indi) ? b->ign : indi ;
    }
    jnd = b->idl + nami - b->s->nsyst;
    if(w[jnd] != 0) w[w[jnd]] = 0;
  }
 
  return(0);
}
 
