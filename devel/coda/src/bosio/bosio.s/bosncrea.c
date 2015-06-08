/*DECK ID>, BOSNCREA. */
/*
	create named bank
*/
 
#include "bos.h"
 
#ifndef VAX

int 
bosncreate_(int *jw, char *name, int *nr, int *ncol, int *nrow, int lename)
{
  int status;
  char *nam;
 
  nam = (char *) MALLOC(lename+1);
  strncpy(nam,name,lename);
  nam[lename] = '\0';
  status = bosNcreate(jw,nam,*nr,*ncol,*nrow);
  FREE(nam);
  return(status);
}
 
#endif
 
int
bosNcreate(int *jw, char *name, int nr, int ncol, int nrow)
{
  BOSptr b;
  int *w, jw1, ifmt, nd, ifd, ifl, ind, nn, idr, m, nlpl, ndif, izero, jnd,ncop;
  int *nam, nama, nami, indi, indj;
 
  w = jw-1;
  b = (BOSptr)jw;
  nam =  (int *)name;
  nama = *nam;
 
  if(b->ick != MAGIC) return(ARRAY_NOT_INITIALIZED);
  jw1 = b->s->jw1;
  b->s->jw1 = 0;
 
  /* calculate nd from ncol and nrow in according to bank format */
 
  nami = mamind(jw,nama);
  ifmt = w[b->idfmt + nami - b->s->nsyst];
  if(ifmt != 0)
  {
    ifmt = w[ifmt+3];
    if(ifmt == 2) nd = (ncol*nrow+1)/2;
    if(ifmt == 3) nd = (ncol*nrow+3)/4;
    if(ifmt != 2 && ifmt != 3) nd = ncol*nrow;
  }
  else
  {
    nd = ncol*nrow;
  }
  if(nd < 0) return(MBANK_WRONG_ARGUMENT);
 
  nami = namen(jw,nama);
  ifd = 0;
 
  /* drop other banks/renumber bank */
 
  if(jw1 == 3)
  {
    jw1 = 0;
    ifl = 0;
    ind = w[nami];
    if(ind != 0)
    {
a:    nn = w[ind-INR];
      if(w[ind-INXT] != 0)
      {
        if(nn == nr)
        {
          ifl = ind;
          ind = w[ind-INXT];
        }
        else
        {
          ind = w[ind-INXT];
          idr = bosNdrop(jw,(char *)nama,nn);
        }
        goto a;
      }
      if(ifl == 0 && w[ind-INR] != nr)
      {
        if(w[ind-INR] != nr)
        {
          w[ind-INR] = nr;
          for(m=1; m<=w[ind]; m++) w[ind+m] = 0;
          ifd = 1;
        }
      }
      else
      {
        nn = w[ind-INR];
        idr = bosNdrop(jw,(char *)nama,nn);
      }
    }
  }
 
  nlpl = nindex(jw,nr,nami,&indi,&indj);
  ind = indi;
  if(ind == 0)	/* bank not found, create new bank */
  {
    ndif = nd + NHW;
    if(b->igp+ndif > b->iwk)
    {
      bosWgarbage(jw);
      return(MBANK_INSUFFICIENT_SPACE_FOR_NEW_BANK);
    }
    ind = b->igp + (NHW-1);
    w[ind-INAM] = nama;
    w[ind-INR ] = nr;
    w[ind-INXT] = w[indj-INXT];
    w[ind  ] = nd;
    w[indj-INXT] = ind;
    b->igp = b->igp + ndif;
    if(nd > 0 && jw1 == 0)
    {
      for(m=1; m<=nd; m++) w[ind+m] = 0;
    }
  }
  else	/* bank found, change length of bank */
  {
    ndif = nd - w[ind];
    if(ndif == 0 && jw1 != 2) goto exit;
    if(ind+w[ind]+1 == b->igp)	/* last bank, change space */
    {
      if(b->igp+ndif > b->iwk)
      {
        bosWgarbage(jw);
        if(b->igp+ndif > b->iwk)	/* insufficient space to increase ... */
        {
          if(ifd != 0) idr = bosNdrop(jw,(char *)nama,nr);
          ind = MBANK_INSUFFICIENT_SPACE_TO_INCREASE_LENGTH_OR_MOVE;
          goto exit;
        }
      }
    }
    else			/* not last bank */
    {
      if(ndif+NHW <= 0 && jw1 != 2)	/* insert dummy dropped bank */
      {
        w[ind] = nd;
        w[ind+nd+NHW] = ndif;
        b->ign = (b->ign < ind+nd+NHW) ? b->ign : ind+nd+NHW ;
        b->ndn = b->ndn - ndif;
        goto exit;
      }
      else				/* move bank */
      {
        if(b->igp+nd+NHW >= b->iwk)
        {
          bosWgarbage(jw);
          if(b->igp+nd+NHW >= b->iwk)	/* insufficient space to increase ... */
          {
            if(ifd != 0) idr = bosNdrop(jw,(char *)nama,nr);
            ind = MBANK_INSUFFICIENT_SPACE_TO_INCREASE_LENGTH_OR_MOVE;
            goto exit;
          }
        }
        ncop = (nd < w[ind]) ? nd : w[ind] ;
        ncop = NHW + ncop;
        bcopy((char *) &w[ind-(NHW-1)], (char *) &w[b->igp], ncop<<2);
        w[ind] = -w[ind] - NHW;
        b->ndn = b->ndn - w[ind];
        b->ign = (b->ign < ind) ? b->ign : ind ;
        ind = b->igp + (NHW-1);
        b->igp = b->igp + ncop;
        w[ind] = ncop - NHW;
        w[indj-INXT] = ind;
        if(ndif <= 0) goto exit;
      }
    }
 
    /* change last bank */
 
    izero = ind + w[ind];
    w[ind] = w[ind] + ndif;
    b->igp = b->igp + ndif;
    if(ndif > 0 && jw1 == 0) for(m=1; m<=ndif; m++) w[izero+m] = 0;
  }
 
exit:
  if(ind != indi)	/* reset link banks */
  {
    jnd = b->idl + nami - b->s->nsyst;
    if(w[jnd] != 0) w[w[jnd]] = 0;
  }
  if(ind > 0)	/* write ncol and nrow if O'k */
  {
    w[ind-ICOL] = ncol;
    w[ind-IROW] = nrow;
  }
  return(ind);
}
 
