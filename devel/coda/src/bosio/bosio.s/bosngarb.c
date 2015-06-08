/*DECK ID>, BOSNGARB. */
/*
*/
 
#include "bos.h"
 
#ifndef VAX
 
void bosngarbage_(int *jw)
{
  bosNgarbage(jw);
}
 
#endif
 
int bosNgarbage(int *jw)
{
  BOSptr b;
  int *w, ngap, ncop, nwrds, ind, icop, nwrd, nama,jnd,nr,nlpl,nami,indi,indj;
 
  w = jw-1;
  b = (BOSptr)jw;
 
  if(b->ick != MAGIC) return(ARRAY_NOT_INITIALIZED);
  b->s->jw1 = 0;
  if(b->ndn == 0) return(0);
 
  ngap = 0;
  ncop = 0;
  indj = 0;
  nwrds = 0;
  ind = b->ign;
 
  while(ind < b->igp)
  {
    if(w[ind] < 0)	/* bank is dropped */
    {
      if(ncop > 0)
      {
        bcopy((char *) &w[icop], (char *) &w[icop-ngap], ncop<<2);
        ncop = 0;
        indj = 0;
      }
      nwrd = -w[ind];
      ngap = ngap + nwrd;
      ind = ind + nwrd;
    }
    else		/* bank is not dropped */
    {
      nwrd = w[ind] + NHW;
      if(ngap != 0)
      {
        /* update index */
        if(indj == 0 || ind != w[indj-INXT])
        {
          if(ncop != 0)
          {
            bcopy((char *) &w[icop], (char *) &w[icop-ngap], ncop<<2);
            ncop = 0;
          }
          nama = w[ind-INAM];
          nami = namez(jw,nama);
          if(nami == 0) return(BGARB_ERROR_AT_INDEX);
 
          /* reset link banks */
 
          jnd = b->idl + nami - b->s->nsyst;
          if(w[jnd] != 0) w[w[jnd]] = 0;
          nr = w[ind-INR];
          nlpl = nindex(jw,nr,nami,&indi,&indj);
          if(indi == 0) return(-111);
          ind = indi;
        }
        w[indj-INXT] = w[indj-INXT] - ngap;
        indj = ind;
        if(ncop == 0) icop = ind - (NHW-1);
        ncop = ncop + nwrd;
      }
      ind = ind + nwrd;
    }
    nwrds = nwrd;
  }
 
  if(ncop != 0) bcopy((char *) &w[icop], (char *) &w[icop-ngap], ncop<<2);
  b->ndn = b->ndn - ngap;
  b->igp = b->igp - ngap;
  b->ign = b->njw;
  return(0);
}
 
