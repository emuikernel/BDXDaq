
/* bos2et.c - reading BOS array and filling ET buffer

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

     input parameters:

  jw - pointer to BOS array
  list - list of BOS banks
  ev - pointer to ET buffer
  et_size - size of ET buffer (bytes)

     output parameters:

  size of event in ET (bytes)
  control[0] - event type
  control[1] - 
  control[2] - 
  control[3] - trigger pattern

*/

#include <stdio.h>

#include "bos.h"
#include "bosio.h"
#include "fpack.h"


int
bos2et(int *jw, char *list, int *ev, int et_size, int *size, int control[4])
{
  BOSptr b;
  int *w, *buf, *fmtbuf, nbanks;
  int i,j,ind,error,nami;
  int start,id,nch,ndim,nf,ird,ntot,nama,nw;
  char *fmt, *ch;

  b = (BOSptr)jw;
  w = jw-1;
  error = start = 0;
  ndim = et_size / sizeof(int);

  /* write record header to ET buffer */

  if(ndim < RECHEADLEN) return(-1); /* not enough space for record header */

  ev[irPTRN]  = SWAP;     /* Pattern to recognize byte swapping                       */
  ev[irFORG]  = LOCAL*16; /* (format code: 1-IEEE,2-IBM,3-VAX,4-DEC)*16 + origin code */
  ev[irNRSG]  = 0;                          /* #logrec*100 + #segment                 */
  ev[irNAME1] = *((unsigned int *)"RUNE"); /* event name - part 1   (Hollerith)      */
  ev[irNAME2] = *((unsigned int *)"VENT"); /* event name - part 2   (Hollerith)      */
  ev[irNPHYS] = 0; /* # of several phys. records in one logical                       */
  ev[irCODE]  = 0; /* segment code: 0-complete, 1-first, 2-middle, 3-last             */
  ev[irNWRD]  = 0; /* the number of words in this record segment                      */
  if(ind = bosNlink(jw,"HEAD",0))
  {
    ev[irNRUN]  = w[ind+2]; /* run number   */
    ev[irNEVNT] = w[ind+3]; /* event number */
    ev[irTRIG]  = w[ind+5]; /* Trig pattern */
  }
  start += RECHEADLEN;
  control[0] = ev[irNRUN];
  control[1] = 0;
  control[2] = 0;
  control[3] = ev[irTRIG];

  /* get a list of BOS bank indexes */

  nbanks = bosLdump(jw,list,&buf,&fmtbuf);

  /* extract bank by bank from BOS array and fill ET buffer */

  for(i=0; i<nbanks; i++)
  {
    ind = buf[i];
    nami = fmtbuf[i];

    ev[start+idNHEAD] = 9;
    ev[start+idNAME1] = w[ind-INAM]; /* the name of bank */
    ev[start+idNAME2] = *((unsigned int *)"    "); /* blank */
    ev[start+idNSGM]  = w[ind-INR];  /* the number of bank */
    ev[start+idNCOL]  = w[ind-ICOL]; /* the number of columns */
    ev[start+idNROW]  = w[ind-IROW]; /* the number of rows */
    ev[start+idCODE]  = 0;
    ev[start+idNPREV] = 0;
    ev[start+idDATA]  = w[ind];      /* the number of data words */
    /* printf(" ind=%7d  name>%4.4s< len=%7d fmt>",ind,&w[ind-INAM],w[ind]);*/

    /* bank format (the same piece in bosWrite.c - make macros ? ) */

    if((id = w[b->idfmt + nami - b->s->nsyst]) == 0)
    {
      ev[start+idNHEAD] += 1;
      ev[start+idFRMT] = *((unsigned int *)"    ");
    }
    else
    {
      if(w[id+3] != 0)
      {
        ev[start+idNHEAD] += 1;
        if(w[id+3] == 1)      ev[start+idFRMT] = *((unsigned int *)"B32 ");
        else if(w[id+3] == 2) ev[start+idFRMT] = *((unsigned int *)"B16 ");
        else if(w[id+3] == 3) ev[start+idFRMT] = *((unsigned int *)"B08 ");
        else {printf("bos2et: error : unknown format\n"); error = -2; goto exit;}
      }
      else
      {
        ch = (char *)&w[id+4];
        nch = strlen(ch);
        fmt = (char *)&ev[start+idFRMT];
        bcopy((char *)ch, (char *)fmt, nch);
        fmt[nch++] = ' ';
        fmt[nch++] = ' ';
        fmt[nch] = ' ';
        nch = ((nch + 1)/4) * 4;
        ev[start+idNHEAD] += nch / 4;
      }
    }

    /*    ch = (char *)&ev[start+idFRMT];
	  for(j=0; j<(ev[start+idNHEAD]-9)*4; j++) printf("%c",ch[j]);
	  printf("<\n"); */

    /* !!! do we have space enough in ET buffer ? - check it before puting next bank !!! */

    if((start + ev[start+idNHEAD] + w[ind]) > ndim)
    {
      printf("bos2et: ERROR - ET buffer too small (%d < %d)\n",ndim,start + ev[start+idNHEAD] + w[ind]);
      error = EBIO_NOETSPACE;
      goto exit;
    }

    start += ev[start+idNHEAD];

    /* copy data */

    bcopy((char *)&w[ind+1], (char *)&ev[start], w[ind]<<2);
    start += w[ind];
  }

  /* printf("\n"); */

exit:

  ev[irNWRD] = start - RECHEADLEN; /* the number of words in this record segment */
  *size = start * sizeof(int);

  return(error);
}

 


