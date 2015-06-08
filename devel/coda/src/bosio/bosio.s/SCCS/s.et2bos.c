h24232
s 00003/00001/00147
d D 1.5 06/12/04 15:58:27 boiarino 6 5
c comment out etfswap
c 
e
s 00003/00000/00145
d D 1.4 05/02/25 15:22:51 boiarino 5 4
c *** empty log message ***
e
s 00004/00003/00141
d D 1.3 03/10/16 09:20:12 boiarino 4 3
c nothing
e
s 00001/00000/00143
d D 1.2 03/04/17 16:47:02 boiarino 3 1
c *** empty log message ***
e
s 00000/00000/00000
d R 1.2 70/01/01 00:00:02 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 clas/bosio/bosio.s/et2bos.c
e
s 00143/00000/00000
d D 1.1 00/08/10 11:10:12 boiarino 1 0
c date and time created 00/08/10 11:10:12 by boiarino
e
u
U
f e 0
t
T
I 1

/* et2bos.c - reading ET buffer and filling BOS array

     input parameters:

  ev - pointer to ET buffer

     output parameters:

  jw - pointer to BOS array ( have to be initialized ! )
  list - name of list of BOS banks

*/

static int nnn=0;


I 3
#include <stdio.h>
E 3
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

I 5
D 6
  /* following call will swap input buffer if necessary */
E 6
I 6
  /* ET2ET should take care about that !!!!!!!!!!!!
  following call will swap input buffer if necessary
E 6
  etfswap((unsigned int *)ev);
I 6
  */
E 6

E 5
  error = bosWgarbage(jw);
  error = start = 0;
  lrlen = ev[irNWRD]+RECHEADLEN;
  bosNdrop(jw,"+REC",RECNUM);




/* extract nesessary info from record header here (do we need it ?) */

/*
  ev[irPTRN ] == SWAP;
  ev[irFORG ] == LOCAL*16;
  ev[irNRSG ] == 100*BIOS_NPHYSREC+(BIOS_NSEGM-1);
*/

  ind = RECHEADLEN;
  /*printf("lrlen=%d\n",lrlen);*/
  while(ind < lrlen)
  {
D 4
/*
E 4
I 4
	/*
E 4
    printf("ind=%d headlen=%d name>%4.4s< code=%d size=%d\n",
            ind,ev[ind+idNHEAD],&ev[ind+1],ev[ind+idCODE],ev[ind+idDATA]);
D 4
*/
E 4
I 4
    */
E 4
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
D 4
      printf("et2bos: NOBOSSPACE %d %d %d %d %d %d\n",b->iwk,start,ndata,ind,idDATA);
E 4
I 4
      printf("et2bos: NOBOSSPACE %d %d %d %d %d %d\n",
        b->iwk,start,ndata,ind,idDATA,NHW);
E 4
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




E 1
