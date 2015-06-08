
/* tttlv1.c - translation program for TLV1 crate */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "coda.h"
#include "tt.h"

#define SKIP_ZERO \
  while((long *)mtdc < ((&bufin[1])+bufin[0]) && mtdchead->slot == 0) \
  { \
    mtdchead++; \
    mtdc++; \
    fb++; \
   /*printf("LIST2: ZERO header word from DC - skiped ...\n"); */ \
  }

#define SKIP_ZERO_FIRST \
 while ((long *)mtdc < ((&bufin[1])+bufin[0]) && \
       ( mtdchead->slot == 0 || mtdchead->count == 1) ) \
 { \
   mtdchead++; \
   mtdc++; \
   fb++; \
 	/*printf("LIST2: FIRST ZERO header word from DC - skiped ...\n");*/ \
 } \
 if((long *)mtdc >= ((&bufin[1])+bufin[0])) \
 { \
   /*printf("LIST2: FIRST ZERO: exit !!!\n");*/ \
   return(0); \
 }

#define SKIP_64BIT \
  if(((long *)mtdc < ((&bufin[1])+bufin[0])) && (mtdchead->slot == SLOT64)) \
  { \
    mtdchead++; \
    mtdc++; \
    fb++; \
	/*printf("ttdc: 64 bit signature skiped ...\n");*/ \
  }

#define MAXDIAG 50
#define DIAG if (diagcount<MAXDIAG) diagcount++; if (diagcount<MAXDIAG) printf


#define BANK_CLOSE /* fb not defined any more, it points to next word after buffer !!! */ \
          len += bosNclose(iw,ind,savencol,(hit-(unsigned short *)&iw[ind+1])/savencol)


#define BANK_OPEN \
          savencol = NBOSCOL[SLOT][CHANNEL]; /* to be used in BANK_CLOSE */ \
          if(savencol <= 0) \
          { \
            printf("tttlv1: ERROR: slot=%d channel=%d does not described in translation table\n", \
              SLOT,CHANNEL); \
            printf("tttlv1: returns len=%d\n",len); \
            return(len); \
          } \
          ind = bosNopen(iw,NAME[SLOT][CHANNEL],NR[SLOT][CHANNEL],NBOSCOL[SLOT][CHANNEL],0); \
          if(ind <= 0) \
          { \
            printf("tttlv1: bosNopen returned %d !!! -> return len=%d\n",ind,len); \
            printf("tttlv1: slot=%d channel=%d name>%4.4s< nr=%d nboscol=%d\n", \
              SLOT,CHANNEL,NAME[SLOT][CHANNEL],NR[SLOT][CHANNEL],NBOSCOL[SLOT][CHANNEL]); \
            return(len); \
          } \
          hit = (unsigned short *)&iw[ind+1]

#define BANK_OPEN1 \
          savencol = NBOSCOL[SLOT][CHANNEL]; /* to be used in BANK_CLOSE */ \
          if(savencol <= 0) \
          { \
            printf("tttlv11: ERROR: slot=%d channel=%d does not described in translation table\n", \
              SLOT,CHANNEL); \
            printf("tttlv11: returns len=%d\n",len); \
            return(len); \
          } \
          ind = bosNopen(iw,NAME[SLOT][CHANNEL],NR[SLOT][CHANNEL],NBOSCOL[SLOT][CHANNEL],0); \
          if(ind <= 0) \
          { \
            printf("tttlv11: bosNopen returned %d !!! -> return len=%d\n",ind,len); \
            printf("tttlv11: slot=%d channel=%d name>%4.4s< nr=%d nboscol=%d\n", \
              SLOT,CHANNEL,NAME[SLOT][CHANNEL],NR[SLOT][CHANNEL],NBOSCOL[SLOT][CHANNEL]); \
            return(len); \
          } \
          hit = (unsigned short *)&iw[ind+1]

/* new stuff
#define BANK_CLOSE \
          hit[0] = hit[1] = hit[2] = 0; \
          hit = (unsigned short *)(((int)hit + 3)/4 * 4); \
          len += bosNclose(iw,ind,savencol, (unsigned long *)hit - (unsigned long *)&iw[ind+1] )
*/




/*#define PRESCALE 100*/
#define PRESCALE 100


static int diagcount=0;

static char* Version = " VERSION: tttlv1.c v1.0 - initial release " ;
static char* CompileTime = DAYTIME;


/******************************************************************************

  Routine             : TT_TranslateTLV1Bank

  Parameters          : bufin - input(CODA) fragment, bufout - output(BOS)

  Discription         : This routine translates one fragment from CODA to
                        BOS format for TLV1 only; 
                        
******************************************************************************/

int
TT_TranslateTLV1Bank(long *bufin, long *bufout, TTSPtr ttp)
{
  unsigned short *hit,*oldhit;
  unsigned long *iw, *endofbufout;
  MTDCPtr        endofboard,mtdc,fb;
  MTDCHeadPtr    mtdchead=(MTDCHeadPtr)&bufin[1];	/* first word is header */
  int            i, j, k, ind, len, bank, nhits=0, mux_index, savencol;

  iw = (unsigned long *)&bufout[2]; /* set BOS pointer to output buffer */
  len = 0;
  /* set end_of_bufout pointer */
  endofbufout = (unsigned long *)(bufout + bosleftspace(iw));

  mtdc = fb = (MTDCPtr)&bufin[2];		     /* first data word */

  /*if(PRESCALE > 0 && (TT_nev%PRESCALE)) return(len);*/ /* prescale data from that crate */

  SKIP_64BIT;
  BANK_OPEN;

  while((long *)mtdc < ((&bufin[1])+bufin[0])) /* loop over input buffer */
  {
    SKIP_64BIT;
    DBG("MTDC Header 0x%8.8X : slot %d count %d\n",*((unsigned long *)mtdchead),
         mtdchead->slot,(unsigned long)mtdchead->count); 
    endofboard = (MTDCPtr)mtdchead + mtdchead->count;  /* end of data for board */

    if(mtdchead->slot>25)
    {
      printf("Translation founds Bad Block Read signature 0x%8.8X -> resyncronize !!!\n",
              *((unsigned long *)mtdchead));
      mtdc++;
      fb = mtdc;
      mtdchead = (MTDCHeadPtr)mtdc;
      continue;
    }

    while(mtdc->slot==mtdchead->slot) /* loop over present board */
    {
      /*
      if(SLOT>25)
      {
        DIAG("Translation founds Bad Block Read signature 0x%8.8X -> resyncronize !!!\n",
             *((unsigned long *)mtdc));
        mtdc++;
        fb = mtdc;
        break;
      }
      */

      /* only number of BOS bank can be changed for DC translation */
      /* if changed - close old bank and start new one */

      if(TT_IDNR[SLOT][CHANNEL] != iw[ind-IDNR])
      /*if(NR[SLOT][CHANNEL] != iw[ind-INR])*/
      {
        BANK_CLOSE;
        BANK_OPEN1;
      }

      /* check the rest of space */

      if( (unsigned long *)(hit+2) + NHWRES > endofbufout )
      {
        printf(" ERROR: bufout overflow - skip the rest ...\n");
        printf("        bufout=%u hit=%u endofbufout=%u\n",bufout,hit,endofbufout);
        BANK_CLOSE;
        return(len);
      }

      hit[0] = ttp->id2[SLOT][0][CHANNEL];
      hit[1] = (unsigned short)(mtdc->data);
      DBG("Channel %d : %4.4X  -> BOS row : id %d  data %4.4X\n",
         mtdc->channel,mtdc->data,(unsigned long)hit[0],(unsigned long)hit[1]);
      hit += 2;
      mtdc++;
      fb = mtdc;
      if(mtdc->slot!=mtdchead->slot) break;
    }                         /* end of while mtdc < endofboard */

    mtdchead = (MTDCHeadPtr)mtdc;
    mtdc++;
    fb = mtdc;
  }                           /* end of while mtdc < ((&bufin[1])+bufin[0]) */

  BANK_CLOSE;

/* returns full fragment length (long words) */  

  return(len);
}














