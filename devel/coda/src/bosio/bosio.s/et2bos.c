
/* et2bos.c - reading ET buffer and filling BOS array

     input parameters:

  ev - pointer to ET buffer

     output parameters:

  jw - pointer to BOS array ( have to be initialized ! )
  list - name of list of BOS banks

*/

static int nnn=0;


#include <stdio.h>
#include "bosio.h"
#include "bos.h"
#include "fpack.h"

#define RECNUM 0 /* the number of temporary bank +REC */

int
et2bos(int *ev, int *jw, char *list)
{
  BOSptr b;
  int *w;
  int i,ii,ind,error,nami,lrlen;
  int start,nf,ird,ndata,ntot,nama,nw;
  char *fmt;
 
  b = (BOSptr)jw;
  w = jw - 1;

  /* ET2ET should take care about that !!!!!!!!!!!!
  following call will swap input buffer if necessary
  etfswap((unsigned int *)ev);
  */

  error = bosWgarbage(jw);
  error = start = 0;
  lrlen = ev[irNWRD]+RECHEADLEN;
  bosNdrop(jw,"+REC",RECNUM);




/* extract nesessary info from record header here (do we need it ?) */

/*
  printf("ev[irPTRN]=0x%08x\n",ev[irPTRN]);
*/

/*
  ev[irPTRN ] == SWAP;
  ev[irFORG ] == LOCAL*16;
  ev[irNRSG ] == 100*BIOS_NPHYSREC+(BIOS_NSEGM-1);
*/

  ind = RECHEADLEN;
  /*printf("lrlen=%d\n",lrlen);*/
  while(ind < lrlen)
  {
	/*
    printf("ind=%d headlen=%d name>%4.4s< code=%d size=%d\n",
            ind,ev[ind+idNHEAD],&ev[ind+1],ev[ind+idCODE],ev[ind+idDATA]);
    */
    /* open temporary BOS bank +REC */

    fmt = (char *)&ev[ind+idFRMT];
    nf = 0;
    while(fmt[nf] != ' ' && fmt[nf] != '\0' && nf < (ev[ind+idNHEAD]-idFRMT)*4) nf++;
    fmt[nf] = '\0';

    if(nf > 0 && ev[ind+idNCOL] > 0 && *fmt != ' ') bosNformat(jw,&ev[ind+idNAME1],fmt);
    if(start == 0)
    {
      if((ird = bosNcreate(jw,"+REC",RECNUM,0,1)) <= 0) return(-55);
      start = b->igp;
    }
    w[start+(NHW-1)-ICOL] = ev[ind+idNCOL];
    w[start+(NHW-1)-IROW] = ev[ind+idNROW];
    w[start+(NHW-1)-INAM] = ev[ind+idNAME1];
    w[start+(NHW-1)-INR ] = ev[ind+idNSGM];
    w[start+(NHW-1)-INXT] = 0;

    /* how much space left */
    ndata = ev[ind+idDATA];
    if((b->iwk - start - NHW) < ndata)
    {
      printf("et2bos: NOBOSSPACE %d %d %d %d %d %d\n",
        b->iwk,start,ndata,ind,idDATA,NHW);
      error = EBIO_NOBOSSPACE;
      goto error_exit;
    }

    /* get data */

    bcopy((char *)&ev[ind+ev[ind+idNHEAD]],(char *)&w[start+NHW],ndata<<2);
    w[start + (NHW-1)] = ndata;
    start = start + ndata + NHW;

    ind = ind+ev[ind+idNHEAD]+ev[ind+idDATA];
  }


  b->s->jw1 = 1;

  /* the total number of words = last index - first index */
  if((ird = bosNcreate(jw,"+REC",RECNUM,(start - b->igp),1)) == 0)
  {
    bosNdrop(jw,"+REC",RECNUM);
    return(-4);
  }
  if((ird = bosNlink(jw,"+REC",RECNUM)) == 0) return(-5);
  ntot = w[ird];
  ind = ird + NHW;
  nama = w[ind-INAM] + 1;
 
  do
  {
    nw = w[ind] + NHW;
    ii = (ntot < nw) ? ntot : nw;
    if(ii < NHW || nw > ntot)
    {
      bosNdrop(jw,"+REC",RECNUM);
      return(-6);
    }
    if(w[ind-INAM] != nama)
    {
      nama = w[ind-INAM];
      nami = namez(jw,nama);
      if(nami == 0) nami = mamind(jw,nama);
    }
    w[ind-INXT] = nami;
    ind = ind + nw;
  } while((ntot = ntot - nw) != 0);
 
  error = bosins(RECNUM,jw,list);
 
  return(error);


error_exit:
   
  bosNdrop(jw,"+REC",RECNUM);
  return(error);
}




