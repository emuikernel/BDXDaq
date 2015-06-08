h58751
s 00001/00000/00391
d D 1.3 05/02/25 15:22:41 boiarino 4 3
c *** empty log message ***
e
s 00012/00006/00379
d D 1.2 03/04/17 16:46:16 boiarino 3 1
c *** empty log message ***
e
s 00000/00000/00000
d R 1.2 70/01/01 00:00:02 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 clas/bosio/bosio.s/bosread.c
e
s 00385/00000/00000
d D 1.1 00/08/10 11:10:06 boiarino 1 0
c date and time created 00/08/10 11:10:06 by boiarino
e
u
U
f e 0
t
T
I 1

/* bosRead.c - reading FPACK file records and filling BOS array

     some terminology & info:

  block - physical record
  record - logical record
  IP - pointer to record header
  JP - pointer to data header
  KP - pointer to data
  BIOS_RECHEAD[irCODE] = 0 - complete segment
  BIOS_RECHEAD[irCODE] = 1 - first segment
  BIOS_RECHEAD[irCODE] = 2 - middle segment
  BIOS_RECHEAD[irCODE] = 3 - last segment

*/

I 3
#include <stdio.h>
#include <stdlib.h>
E 3
#include "bosio.h"
#include "bos.h"
#include "fpack.h"

/* #define COND_DEBUG */
/* #define DEBUG    */
#include "dbgpr.h"

 
int
D 3
bosread_(int *descriptor, int *jw, char *list, int lilen)
E 3
I 3
bosread_(BOSIO *descriptor, int *jw, char *list, int lilen)
E 3
{
  char *li;
  int status;
  li = (char *) MALLOC(lilen+1);
  strncpy(li,list,lilen);
  li[lilen] = '\0';
D 3
  status = bosRead(*descriptor, jw, li);
E 3
I 3
  status = bosRead(descriptor, jw, li);
E 3
  FREE(li);
  return(status);
}
 
int
D 3
bosRead(int descriptor, int *jw, char *list)
E 3
I 3
bosRead(BOSIO *descriptor, int *jw, char *list)
E 3
{
  BOSIOptr BIOstream;
  BOSptr b;
  int *w, *bufin;
  int i,ii,ind,error,nami;
  int start,ihd,nread,ndim,nf,ird,ndata,ntot,nama,nw;
  int dathead[70]; /* intermediate buffers */
  char *fmt;
 
D 3
  BIOstream = (BOSIOptr)descriptor;	/* set pointers */
E 3
I 3
  BIOstream = descriptor; /* set pointers */
E 3
  b = (BOSptr)jw;
  w = jw - 1;
 
  error = bosWgarbage(jw);
  error = 0;

  /* check unit status (for sequential IO only) */

  CHECK_INPUT_UNIT(descriptor);

  /* allocate input buffer */

  ALLOCATE_INPUT_BUFFER(descriptor);

  /* scroll records until will get completed or first one */

  do
  {

    if(IP != 0 && NEXTIP(IP) < bufin[1])   /* we are in the middle of block */
    {
      IP = NEXTIP(IP);
    }
    else                                 /* block are finished - let's get another one */
    {
      ii = BIOS_RECL/sizeof(int);
      error = bosin(descriptor,&ii);
      bufin = BIOS_BUFP; /* could be changed in bosin() ! */

      if(error < 0)	/* end of file */
      {
        bosNdrop(jw,"+REC",BIOS_UNIT);
I 4
        printf("bosread: end of file\n");
E 4
        return(error);
      }
      if(error != 0)    /* some error */
      {
        bosNdrop(jw,"+REC",BIOS_UNIT);
        return(error);
      }
      ii = LOCAL; frconv_(bufin,&ii,&error);  /* convert buffer */
      IP = 2;	/* successful read - reset pointer */
    }

    bcopy((char *)&bufin[IP],(char *)BIOS_RECHEAD, RECHEADLEN*sizeof(int));
    BIOS_NLOGREC++;
    if(BIOS_RECHEAD[irCODE] < 0 || BIOS_RECHEAD[irCODE] > 3) return(EBIO_WRONGSEGMCODE);
 
  } while(BIOS_RECHEAD[irCODE] != 0 && BIOS_RECHEAD[irCODE] != 1);


  /* we are in the begining of completed or first segment */

  JP = 0;
  bosNdrop(jw,"+REC",BIOS_UNIT);
  error = start = 0;

  do
  {

    do
    {
      GET_DATA_HEADER(descriptor,done,error,error_exit);
    } while(dathead[idCODE] != 0 && dathead[idCODE] != 1); /* do we need this loop ??? */

    KP = 0;

    /* open temporary BOS bank +REC */

    fmt = (char *)&dathead[idFRMT];
    nf = 0;
    while(fmt[nf] != ' ' && fmt[nf] != '\0' && nf < (dathead[idNHEAD]-idFRMT)*4) nf++;
    fmt[nf] = '\0';
D 3

    if(nf > 0 && dathead[idNCOL] > 0 && *fmt != ' ') bosNformat(jw,&dathead[idNAME1],fmt);
E 3
I 3
/*printf("b1\n");fflush(stdout);*/
    if(nf > 0 && dathead[idNCOL] > 0 && *fmt != ' ')
    {
      bosNformat(jw,&dathead[idNAME1],fmt);
    }
/*printf("b2\n");fflush(stdout);*/
E 3
    if(start == 0)
    {
      if((ird = bosNcreate(jw,"+REC",BIOS_UNIT,0,1)) <= 0) return(-55);
      start = b->igp;
    }
    w[start+(NHW-1)-ICOL] = dathead[idNCOL];
    w[start+(NHW-1)-IROW] = dathead[idNROW];
    w[start+(NHW-1)-INAM] = dathead[idNAME1];
    w[start+(NHW-1)-INR ] = dathead[idNSGM];
    w[start+(NHW-1)-INXT] = 0;
    if((ndim = b->iwk - start - NHW) < 200) return(-66);

    nread = 0;
    while(1 == 1)
    {
      GET_DATA(&w[start+NHW+nread],ndim,error);
      if(nread == ndim || dathead[idCODE] == 0 || dathead[idCODE] == 3)
      {
        ndata = nread;
        w[start + (NHW-1)] = ndata;
        start = start + ndata + NHW;
        break; /* we got all data - exit from while(1 == 1) loop */
      }
      GET_DATA_HEADER(descriptor,done,error,error_exit);
      KP = 0;
      if(dathead[idCODE] == 0 || dathead[idCODE] == 1) return(EBIO_NOTCONTCODE);
    }
    /*printf("bosread: nread=%d (start=%d) < ndim=%d\n",nread,start,ndim); fflush(stdout);*/
    /*} while(start < ndim); ??????????????? */
  } while(nread < ndim);

  /* if we are here - something wrong */

  error = -333;
  printf("bosread: something wrong ... error=%d\n",error); fflush(stdout);

error_exit: /* error exit from GET_DATA_HEADER() - could be end-of-file !!! */
   
  printf("bosread: error_exit ... error=%d\n",error); fflush(stdout);
  bosNdrop(jw,"+REC",BIOS_UNIT);
  return(error);

done:

  b->s->jw1 = 1;

  /* the total number of words = last index - first index */
  if((ird = bosNcreate(jw,"+REC",BIOS_UNIT,(start - b->igp),1)) == 0)
  {
    bosNdrop(jw,"+REC",BIOS_UNIT);
    return(-4);
  }
  if((ird = bosNlink(jw,"+REC",BIOS_UNIT)) == 0) return(-5);
  ntot = w[ird];
  ind = ird + NHW;
  nama = w[ind-INAM] + 1;
 
  do
  {
    nw = w[ind] + NHW;
    ii = (ntot < nw) ? ntot : nw;
    if(ii < NHW || nw > ntot)
    {
      bosNdrop(jw,"+REC",BIOS_UNIT);
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

  error = bosins(BIOS_UNIT,jw,list);
 
  return(error);
}

int
bosins(int unit, int *jw, char *list)
{
  BOSptr b;
  int *w, error, nls, jls, *ilst, ird, ntot,next,leng,jnd,i,nsft,ind,nind,nami;
  char *name, *clst = "+LST";

  b = (BOSptr)jw;
  w = jw - 1;
  ilst = (int *)clst;
 
  error = 0;
  b->s->jw1 = 0;
 
/* define list of bank names: get list-index and initialize list */
 
  nls = 0;
 
  switch (*list)
  {
    case 'c':
    case 'C': jls = 0;
              break;
    case 'e':
    case 'E': jls = 1;
              break;
    case 'r':
    case 'R': jls = 2;
              break;
    case 's':
    case 'S': jls = 3;
              break;
    case 't':
    case 'T': jls = 4;
              break;
    default:  return(-765);
  }
 
  if(b->ilt[jls] == 0)
  {
    if((error=wbanc(jw,&b->ilt[jls],0,1)) < 0)	/* no space for work bank */
    {
      w[b->ilt[jls]] = 0;
      bosNdrop(jw,"+REC",unit);
      return(-2);
    }
    w[b->ilt[jls]-INAM] = *ilst;
  }
  w[b->ilt[jls]] = 0;
 
/* start record */
 
  if((ird = bosNlink(jw,"+REC",unit)) == 0) return(0); /* ?! */
  ntot = w[ird];
  next = ird + 1;	/* first index of banks area */
  nami = w[next+(NHW-1)-INXT] + 1;
 
/* bank loop */
 
  do
  {
    leng = w[next+(NHW-1)] + NHW;
    if(w[next+(NHW-1)-INXT] != nami)		/* new name */
    {
      nami = w[next+(NHW-1)-INXT];
      jnd = b->idl + nami - b->s->nsyst;	/* reset link banks */
      if(w[jnd] != 0) w[w[jnd]] = 0;
     /* store name-index in list */
      for(i=1; i<=nls; i++)
      {
        if(w[b->ilt[jls]+i] == nami) goto a30;
      }
      if(w[b->ilt[jls]-INR] - NHW <= nls)
      {
        w[b->ilt[jls]] = nls;
        if(wbanc(jw,&b->ilt[jls],nls+NHW,1) < 0) /* no space for work bank */
        {
          w[b->ilt[jls]] = 0;
          bosNdrop(jw,"+REC",unit);
          return(-2);
        }
      }
      nls++;
      w[b->ilt[jls]+nls] = nami;
    }
a30:
    next = next + leng;
    ntot = ntot - leng;
 
  } while(ntot > 0);
 
  if(ntot < 0)	/* word count error */
  {
    w[b->ilt[jls]] = 0;
    bosNdrop(jw,"+REC",unit);
    return(-3);
  }
  w[b->ilt[jls]] = nls;	/* length of list */
 
/* insert pointer into new banks */
/* start record */
  ntot = w[ird];
  nsft = 0;
  next = ird + 1;
  nami = w[next+(NHW-1)-INXT] + 1;
 
/* bank loop */
 
  do
  {
    leng = w[next+(NHW-1)] + NHW;
   /* compare name induces */
    if(w[next+(NHW-1)-INXT] != nami)	/* new name */
    {
      nami = w[next+(NHW-1)-INXT];
    }
    else				/* same name as previous */
    {
      if(w[ind-INR] == w[next+(NHW-1)-INR] && ind+w[ind]+1 == next)
      {					/* same number - add to same bank */
        nsft = nsft + NHW;
        w[ind] = w[ind] + leng - NHW;
 
/*        bcopy((char *)&w[next+NHW], (char *)&w[next], (ntot-NHW)<<2); */
/* overlap here (?!) - use for(..) loop instead of bcopy() */
        for(i=0; i<ntot-NHW; i++) w[next+i] = w[next+NHW+i];
 
        next = next + leng - NHW;
        goto a70;
      }
      else
      {
        nind = w[ind-INXT];
        if(w[ind-INR] < w[next+(NHW-1)-INR] && nind == 0) goto a60;
      }
    }
   /* find last bank to insert pointer */
    ind = nami + 1;
a50:
    nind = w[ind-INXT];
    if(nind != 0)
    {
      if(w[nind-INR] < w[next+(NHW-1)-INR])
      {
        ind = nind;
        goto a50;
      }
      else if(w[nind-INR] == w[next+(NHW-1)-INR]) /* drop bank same name, nr */
      {
        name = (char *)&w[next+(NHW-1)-INAM];
        b->s->jw1 = 1;
        bosNdrop(jw,name,w[next+(NHW-1)-INR]);	/* ??? */
        nind = w[ind-INXT];
      }
    }
   /* insert pointer */
a60:
    w[ind-INXT] = next+(NHW-1);
    ind = next+(NHW-1);
    w[ind-INXT] = nind;
    next = next + leng;
a70:
   /* transition to next bank */
    ntot = ntot - leng;
 
  } while(ntot != 0);
 
  if(nsft != 0)
  {
    nsft = w[ird] - nsft;
    ird = bosNcreate(jw,"+REC",unit,nsft,1);
  }
 /* record complete - drop +REC bank */
  w[ird] = 0;
  ird = bosNdrop(jw,"+REC",unit);
 
  return(0);
}
 
E 1
