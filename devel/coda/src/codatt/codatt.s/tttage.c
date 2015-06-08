
/* tttage.c */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "coda.h"
#include "tt.h"

#define DBGVERS 1

#define TEST_SLOT_CHANNEL0 \
  if(SLOT<=0 || SLOT>25 || CHANNEL<0 || CHANNEL>95) \
  { \
    printf("ERROR0: slot=%d channel=%d\n",SLOT,CHANNEL); \
  } 

#define TEST_SLOT_CHANNEL1 \
  if(SLOT<=0 || SLOT>25 || CHANNEL<0 || CHANNEL>95) \
  { \
    printf("ERROR1: slot=%d channel=%d\n",SLOT,CHANNEL); \
  } 

#define TEST_SLOT_CHANNEL2 \
  if(SLOT<=0 || SLOT>25 || CHANNEL<0 || CHANNEL>95) \
  { \
    printf("ERROR2: slot=%d channel=%d\n",SLOT,CHANNEL); \
  } 

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
   /*printf("tttage: SKIP_ZERO_FIRST INFO 0x%08x (slot=%2d count=%d) - skiped ...\n", \
	*((unsigned long *)mtdchead),mtdchead->slot,mtdchead->count);*/ \
   mtdchead++; \
   mtdc++; \
   fb++; \
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

#define BANK_OPEN \
          savencol = NBOSCOL[SLOT][CHANNEL]; /* to be used in BANK_CLOSE */ \
          if(savencol <= 0) \
          { \
            printf("tttage0: ERROR: slot=%d channel=%d does not described in translation table\n", \
              SLOT,CHANNEL); \
            printf("tttage0: returns len=%d\n",len); \
            return(len); \
          } \
          ind = bosNopen(iw,NAME[SLOT][CHANNEL],NR[SLOT][CHANNEL],NBOSCOL[SLOT][CHANNEL],0); \
          if(ind <= 0) \
          { \
            printf("tttage0: bosNopen returned %d !!! -> return len=%d\n",ind,len); \
            printf("tttage0: slot=%d channel=%d name>%4.4s< nr=%d nboscol=%d\n", \
              SLOT,CHANNEL,NAME[SLOT][CHANNEL],NR[SLOT][CHANNEL],NBOSCOL[SLOT][CHANNEL]); \
            return(len); \
          } \
          hit = (unsigned short *)&iw[ind+1]

#define BANK_OPEN1 \
          savencol = NBOSCOL[SLOT][CHANNEL]; /* to be used in BANK_CLOSE */ \
          if(savencol <= 0) \
          { \
            printf("tttage1: ERROR: slot=%d channel=%d does not described in translation table\n", \
              SLOT,CHANNEL); \
            printf("tttage1: returns len=%d\n",len); \
            return(len); \
          } \
          ind = bosNopen(iw,NAME[SLOT][CHANNEL],NR[SLOT][CHANNEL],NBOSCOL[SLOT][CHANNEL],0); \
          if(ind <= 0) \
          { \
            printf("tttage1: bosNopen returned %d !!! -> return len=%d\n",ind,len); \
            printf("tttage1: slot=%d channel=%d name>%4.4s< nr=%d nboscol=%d\n", \
              SLOT,CHANNEL,NAME[SLOT][CHANNEL],NR[SLOT][CHANNEL],NBOSCOL[SLOT][CHANNEL]); \
            return(len); \
          } \
          hit = (unsigned short *)&iw[ind+1]

#define BANK_CLOSE /* fb not defined any more, it points to next word after buffer !!! */ \
          hit[0] = hit[1] = hit[2] = 0; \
          hit = (unsigned short *)(((int)hit + 3)/4 * 4); \
          len += bosNclose(iw,ind,savencol, (unsigned long *)hit - (unsigned long *)&iw[ind+1] )

#define MAXDIAG 50
#define DIAG if (diagcount<MAXDIAG) diagcount++; if (diagcount<MAXDIAG) printf
/*#define DBG printf*/


static int diagcount=0;
static char* Version = " VERSION: tttage.c v1.0 - initial release " ;
static char* CompileTime = DAYTIME;


/******************************************************************************

  Routine             : TT_TranslateTAGEBank

  Parameters          : bufin - input(CODA) fragment, bufout - output(BOS)

  Discription         : This routine translates one fragment from CODA to
                        BOS format for TAGGER-E only; 
                        
******************************************************************************/

/* was in tttrans.c, need to be fitted somewhere here
        if(ROC == 17 && (jw[ind]+3) != bufin[0] &&
           (jw[ind]+6) != bufin[0])
        {
          printf("tttrans ERRO: bank length=%d(0x%x) != coda length=%d(0x%x)\n",
                  jw[ind],jw[ind],bufin[0],bufin[0]);
        }
*/

int
TT_TranslateTAGEBank(long *bufin, long *bufout, TTSPtr ttp)
{
  unsigned short *hit, *oldhit;
  unsigned long *iw, *endofbufout;
  MTDCPtr        endofboard,mtdc,fb;
  MTDCHeadPtr    mtdchead; /* first word is header */
  int            i, j, k, ind, len, bank, nhits=0, mux_index, savencol;
  /*int *aa;*/

  iw = (unsigned long *)&bufout[2]; /* set BOS pointer to output buffer */
  len = 0;
  /* set end_of_bufout pointer */
  endofbufout = (unsigned long *)(bufout + bosleftspace(iw));


  /*aa = (MTDCPtr)&bufin[1];*/
  /*printf("tttage first data: %08x %08x %08x %08x\n",aa[0],aa[1],aa[2],aa[3]);*/
  /*
  i = 1;
  while((long *)aa < ((&bufin[1])+bufin[0]))
  {
    printf("tttage[%3d]-> %08x\n",i,*aa);
    aa++;
    i++;
  }
  */

  mtdchead = (MTDCHeadPtr)&bufin[1]; /* first tdc header */
  mtdc = fb = (MTDCPtr)&bufin[2];    /* first tdc data word */

#ifdef DBGVERS
  SKIP_ZERO_FIRST;
#endif

  /* open bank */
#ifdef DBGVERS
  TEST_SLOT_CHANNEL0;
#endif
  SKIP_64BIT;
  BANK_OPEN;

  while((long *)mtdc < ((&bufin[1])+bufin[0]))  /* loop over input buffer */  
  {
	/*
    DBG("MTDC Header 0x%8.8X : slot %d count %d\n",*((unsigned long *)mtdchead),
	mtdchead->slot,(unsigned long)mtdchead->count); */
    endofboard = (MTDCPtr)mtdchead + mtdchead->count;  /* end of data for board */

    if(mtdchead->slot>25 || mtdchead->slot==0)
    {
      printf("ROC # %d Event # %d :  Bad Block Read signature 0x%8.8X -> resyncronize !!!\n",
             ttp->roc,0/*TT_nev*/,*((unsigned long *)mtdchead));
      mtdc++;
      fb = mtdc;
      mtdchead = (MTDCHeadPtr)mtdc;
      continue;
    }

    while(mtdc->slot==mtdchead->slot)	        /* loop over present board */
	{
      /* both name and number of BOS bank can be changed for translation */
      /* if changed - close old bank and start new one */

#ifdef DBGVERS
      TEST_SLOT_CHANNEL1;
      if(TT_IDNR[SLOT][CHANNEL] != iw[ind-IDNR])
      /*if(NAME[SLOT][CHANNEL] != iw[ind-INAM] ||
         NR[SLOT][CHANNEL] != iw[ind-INR])*/
      {
printf("tttage: NEW BANK ??? slot=%d channel=%d\n",SLOT,CHANNEL);
        BANK_CLOSE;
        BANK_OPEN1;
      }
#endif

      /* check the rest of space */

#ifdef DBGVERS
      if( (unsigned long *)(hit+2) + NHWRES > endofbufout )
      {
	    printf(" ERROR: bufout overflow - skip the rest ...\n");
	    printf("        bufout=%u hit=%u endofbufout=%u\n",bufout,hit,endofbufout);
        /* close bank */
        BANK_CLOSE;
        return(len);
      }
#endif

#ifdef DBGVERS
      TEST_SLOT_CHANNEL2;
#endif


	  /* Sergey: keep hit only if it is in specified range */
      if(mtdc->data > 750)
	  {
	    hit[0] = ttp->id2[SLOT][0][CHANNEL];
	    hit[1] = (unsigned short)(mtdc->data);
	    /*
	    DBG("Channel %d : %4.4X  -> BOS row : id %d  data %4.4X\n",
		    mtdc->channel,mtdc->data,(unsigned long)hit[0],(unsigned long)hit[1]);
	    */
	    hit += 2;
	  }


	  mtdc++;
	  fb = mtdc;

      /* end of buffer ? new slot ? goto loop over input buffer */
	  if((long *)mtdc >= ((&bufin[1])+bufin[0]) || mtdc->slot!=mtdchead->slot)
      {
#ifdef DBGVERS
		if((int)mtdc != (int)endofboard)
        {
          printf("event %d -> mtdc=0x%08x endofboard=0x%08x (slots %2d %2d) (adrs 0x%08x 0x%08x)\n",
		  0/*TT_nev*/,(int)mtdc,(int)endofboard,mtdchead->slot,mtdc->slot,(long *)mtdc,((&bufin[1])+bufin[0]));
		  /*
  aa = (int *)&bufin[1];
  i = 1;
  while((long *)aa < ((&bufin[1])+bufin[0]))
  {
    printf("tttage[%3d]-> %08x\n",i,*aa);
    aa++;
    i++;
  }
		  */
        }
#endif
        break;
      }

      /*if((long *)mtdc >= (long *)endofboard) break;*/

	}                                   /* end of while mtdc < endofboard */

	mtdchead = (MTDCHeadPtr)mtdc;
	mtdc++;
	fb = mtdc;

  }                           /* end of while mtdc < ((&bufin[1])+bufin[0]) */

  /* close bank */
  BANK_CLOSE;

/* returns full fragment length (long words) */  

  return(len);
}



