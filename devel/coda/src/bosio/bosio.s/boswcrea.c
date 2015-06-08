/*DECK ID>, BOSWCREA. */
/*
*/
 
#include "bos.h"
 
#ifndef VAX
 
int wwbanc_(int *jw, int *id, int *ncol, int *nrow)
{
  return(wbanc(jw, id, *ncol, *nrow));
}
 
#endif
 
int wbanc(int *jw, int *id, int ncol, int nrow)
{
  BOSptr b;
  int nresrv, status;
 
  b = (BOSptr)jw;
 
  nresrv = b->s->nresr;
  b->s->nresr = 0;
  status = bosWcreate(jw, id, ncol, nrow);
  b->s->nresr = nresrv;
  return(status);
}
 
#ifndef VAX
 
int boswcreate_(int *jw, int *id, int *ncol, int *nrow)
{
  return(bosWcreate(jw, id, *ncol, *nrow));
}
 
#endif
 
int bosWcreate(int *jw, int *id, int ncol, int nrow)
{
  BOSptr b;
  static int hwork;
  int *w, i, *iname, nd, ld, jd, izer, nzer, ndif, icop, ncop;
  char *name="WORK";
 
  w = jw-1;
  b = (BOSptr)jw;
  iname = (int *)name;
 
  if(b->ick != MAGIC) return(berr("bosWcreate",ARRAY_NOT_INITIALIZED));
  if((nd=ncol*nrow) < 0) return(berr("bosWcreate",WRONG_ARGUMENT));
  if(hwork == 0) hwork = (*iname);	/* first work bank are created */
  ld = ((unsigned)id>>2) - (((int)jw)>>2) + 1;	/* shift (4-byte words) */
 
  if(*id == 0)			/* create new work bank */
  {
    if(nd + NHW + b->s->nresr > b->iwk - b->igp)
    {
      bosWgarbage(jw);
      b->s->jw1 = 0;
      return(W_INSUFFICIENT_SPACE_FOR_NEW_BANK);
    }
    b->iwk = b->iwk - (nd+NHW);
    *id = b->iwk + (NHW-1);
    w[(*id)-ICOL] = ncol;
    w[(*id)-IROW] = nrow;
    w[(*id)-INAM] = hwork;
    w[(*id)-INR ] = nd+NHW;
    w[(*id)-INXT] = ld;
    w[ *id   ] = nd;
    izer = *id + 1;
    nzer = nd;
  }
  else				/* dealing with existing bank */
  {
    /* verify value of work bank index */
    if(*id <= b->iwk || *id >= b->njw || w[*id] < 0 || w[(*id)-INXT] != ld)
    {
      b->s->jw1 = 0;
      return(W_INDEX_INCORRECT);
    }
    /* change length of existing bank */
    if((ndif = nd+NHW-w[(*id)-INR]) <= 0 && b->s->jw1!=4) /*change in place */
    {
      izer = *id + w[*id] + 1;
      nzer = nd - w[*id];
      w[*id] = nd;
    }
    else						   /* change requires */
    {							   /* shift */
      if(ndif > 0 && ndif+b->s->nresr > (b->iwk-b->igp))
      {
        bosWgarbage(jw);
        *id = w[ld];	/* FLR'S MOD for IBM compiler - ??????? */
        b->s->jw1 = 0;
        return(W_INSUFFICIENT_SPACE_TO_INCREASE_LENGTH_OF_EXISTING_BANK);
      }
      icop = b->iwk;
      if(ndif > 0)
      {
        ncop = *id + w[*id] + 1 - icop;
      }
      else
      {
        ncop = *id + nd + 1 - icop;
      }
      nzer = nd - w[*id];
      izer = *id + w[(*id)-INR] - nzer - (NHW-1);
      bcopy((char *) &w[icop], (char *) &w[icop-ndif], ncop<<2);
      if(b->igw != 0 && b-> igw < *id) b->igw = b->igw - ndif;
      *id = *id - ndif;
      w[(*id)-ICOL] = ncol;
      w[(*id)-IROW] = nrow;
      w[(*id)-INR ] = nd+NHW;
      w[ *id      ] = nd;
      b->iwk = b->iwk - ndif;
      jd = b->iwk + (NHW-1);
      while(jd < *id)
      {
        if(w[jd] >= 0) w[w[jd-INXT]] = jd;
        jd = jd + w[jd-INR];
      }
    }
  }
 
  if(nzer > 0 && b->s->jw1 != 1)	/* preset to zero */
  {
    for(i=izer; i<nzer; i++) w[i] = 0;
  }
 
  b->s->jw1 = 0;
  return(*id);
}
 
