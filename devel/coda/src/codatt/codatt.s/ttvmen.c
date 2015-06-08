
/* ttvmen.c */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "coda.h"
#include "tt.h"


#define PRINT_BUFFER \
  { \
    unsigned int *aa = (PTDCPtr)&bufin[1]; \
    printf("ttvme first data: %08x %08x %08x %08x\n",aa[0],aa[1],aa[2],aa[3]); \
    i = 1; \
    printf("buffer start at 0x%08x, buffer end at 0x%08x\n", \
      aa,((&bufin[1])+bufin[0])); \
    while((long *)aa < (((&bufin[1])+bufin[0]))) \
    { \
      printf("[%4d]->0x%08x(0x%08x)->slot %2d, chan %3d, edge=%1d, data %7d\n", \
        i,aa,*aa,((*aa)>>27)&0x1F,((*aa)>>19)&0x7F,((*aa)>>26)&0x1,(*aa)&0x3FFFF); \
      aa++; \
      i++; \
    } \
  }

#define PRINT_FRAGMENTS \
{ \
  int i, j; \
  printf("\n nBanks=%d\n",nBanks); \
  for(i=0; i<=nBanks; i++) \
  { \
    printf("bank=0x%08x, nbanks=%d\n",FragTab[i].idnr,FragTab[i].n); \
    for(j=0; j<=FragTab[i].n; j++) \
    { \
      printf(" [%2d] start 0x%08x stop 0x%08x\n", \
        j, FragTab[i].begin[j], FragTab[i].end[j]); \
    } \
  } \
  printf("\n"); \
}

#define BANK_OPEN \
  savencol = NBOSCOL[ptdc->slot][ptdc->channel]; /* to be used in BANK_CLOSE */ \
  if(savencol <= 0) \
  { \
    printf("ttvme: ERROR: slot=%d channel=%d does not described in translation table\n", \
      ptdc->slot,ptdc->channel); \
	  printf("ttvme: ERROR: returns len=%d\n",len); \
/*PRINT_BUFFER;*/ \
/*printf("nslots=%d, ndatawords=%d\n",nslots,ndatawords);*/ \
    return(len); \
  } \
  ind = bosNopen(iw,NAME[ptdc->slot][ptdc->channel], \
    NR[ptdc->slot][ptdc->channel],NBOSCOL[ptdc->slot][ptdc->channel],0); \
  if(ind <= 0) \
  { \
    printf("ttvme: ERROR: bosNopen returned %d -> return len=%d\n",ind,len); \
    printf("ttvme: ERROR: slot=%d channel=%d name>%4.4s< nr=%d nboscol=%d\n", \
      ptdc->slot,ptdc->channel,NAME[ptdc->slot][ptdc->channel], \
      NR[ptdc->slot][ptdc->channel],NBOSCOL[ptdc->slot][ptdc->channel]); \
    return(len); \
  } \
  hit = (unsigned short *)&iw[ind+1]

#define BANK_CLOSE /* ptdc not defined any more, it points to next word after buffer !!! */ \
{ \
  hit[0] = hit[1] = hit[2] = 0; \
  hit = (unsigned short *)(((int)hit + 3)/4 * 4); \
  len += bosNclose(iw,ind,savencol, (unsigned long *)hit - (unsigned long *)&iw[ind+1] ); \
}

#define BANK_CLOSE_LONG /* ptdc not defined any more, it points to next word after buffer !!! */ \
{ \
  hitl[0] = hitl[1] = 0; \
  len += bosNclose(iw,ind,savencol, (unsigned long *)hitl - (unsigned long *)&iw[ind+1] ); \
}

/* search for minimum chan 0 value;  ptdc->slot ==0 for board header */
#define ZERO_CHAN \
  if(ptdc->slot != 0 && zerochan[ptdc->slot] > ptdc->data) \
  { \
    /*printf("[%d] oldzero=%d, data=%d\n",ptdc->slot,zerochan[ptdc->slot],ptdc->data);*/ \
    zerochan[ptdc->slot] = ptdc->data; \
    /*printf("[%d] newzero=%d\n",ptdc->slot,zerochan[ptdc->slot]);*/ \
  }


#define RAW_FRAGMENT_OPEN \
  /*while(ptdc->channel==0 && ((long *)ptdc < ((&bufin[1])+bufin[0])) ) \
  { \
    ZERO_CHAN; \
    ptdc++; \
	}*/ \
  if( ((long *)ptdc >= ((&bufin[1])+bufin[0])) ) \
  { \
    printf("END OF BUFFER in RAW_FRAGMENT_OPEN !!!\n"); \
    return(len); \
  } \
  for(i=0; i<=nBanks; i++) \
  { \
    if(TT_IDNR[ptdc->slot][ptdc->channel] == FragTab[i].idnr) \
    { \
      /*printf("RAW_FRAGMENT_OPEN: IDNR=%d already in FragTab[%d] - break\n", \
        TT_IDNR[ptdc->slot][ptdc->channel],i);*/ \
      break; \
    } \
  } \
  /*printf("RAW_FRAGMENT_OPEN: use slot %2d, channel %3d to start fragment\n", \
    ptdc->slot,ptdc->channel);*/ \
  FragTab[i].idnr = TT_IDNR[ptdc->slot][ptdc->channel]; \
  if(FragTab[i].n >= NPREFRAGS) \
  { \
    /*printf("ERROR: too many fragments !!! will overwrite last fragment ...\n");*/ \
  } \
  else \
  { \
    FragTab[i].n = FragTab[i].n + 1; \
  } \
  FragTab[i].begin[FragTab[i].n] = ptdc; \
  nBanks_cur = i; \
  if(nBanks_cur > nBanks) nBanks = nBanks_cur; \
  /*printf("RAW_FRAGMENT_OPEN: nBanks_cur=%d, FragTab[i].idnr=0x%08x,", \
    nBanks_cur,FragTab[i].idnr); \
  printf(" FragTab[i].n=%d,", \
    FragTab[i].n); \
  printf(" FragTab[i].begin[FragTab[i].n]=0x%08x\n", \
    FragTab[i].begin[FragTab[i].n]); \
  printf("RAW_FRAGMENT_OPEN: ptdc=0x%08x idnr=0x%08x type=%d roc=%d id=%d place=%d\n", \
		  ptdc, \
		  TT_IDNR[ptdc->slot][ptdc->channel], \
		  SLTYPE[ptdc->slot],ROC, \
		  ID[ptdc->slot][ptdc->channel], \
		  PLACE[ptdc->slot][ptdc->channel])*/

#define  RAW_FRAGMENT_CLOSE \
  FragTab[nBanks_cur].end[FragTab[nBanks_cur].n] = ptdc; \
  if(ptdc > ((&bufin[1])+bufin[0])) \
    printf("EEE0: 0x%08x 0x%08x\n",ptdc,((&bufin[1])+bufin[0])); \
  /*printf("\nRAW_FRAGMENT_CLOSE: %d of IDNR 0x%08x\n\n", \
	FragTab[i].n,FragTab[i].idnr)*/

#define  RAW_FRAGMENT_CLOSE1 \
  /*FragTab[nBanks_cur].end[FragTab[nBanks_cur].n] = ptdc;*/ \
  FragTab[nBanks_cur].end[FragTab[nBanks_cur].n] = ((&bufin[1])+bufin[0]); \
  if(ptdc > ((&bufin[1])+bufin[0])) \
  /*printf("EEE1: 0x%08x 0x%08x\n",ptdc,((&bufin[1])+bufin[0])); \
    printf("\nRAW_FRAGMENT_CLOSE: %d of IDNR 0x%08x\n\n", \
	FragTab[i].n,FragTab[i].idnr)*/


/* following value will be added to the reference (zero channel)
   before subtraction to make sure that result will be positive */
#define REFOFFSET 5000

static char* Version = " VERSION: ttvme.c v1.0 - initial release " ;
static char* CompileTime = DAYTIME;

/******************************************************************************

  Routine             : TT_TranslateVMEBank

  Parameters          : bufin - input(CODA) fragment, bufout - output(BOS)

  Discription         : This routine translates one fragment from CODA to
                        BOS format for VME crates 
                        
******************************************************************************/

int
TT_TranslateVMEBank(long *bufin, long *bufout, TTSPtr ttp)
{
  preStructS FragTab[NPREBANKS];
  int nBanks, nBanks_cur, nslots, ndatawords, refoffset;
  unsigned short *hit;
  unsigned long *iw, *hitl, *endofbufout, zerochan[22];
  PTDCPtr        endofboard,ptdc;
  PTDCHeadPtr    ptdchead; /* first word is header */
  int            i, j, k, ind, len, bank, nhits=0, mux_index, savencol, id;

  iw = (unsigned long *)&bufout[2]; /* set BOS pointer to output buffer */
  len = 0;
  /* set end_of_bufout pointer */
  endofbufout = (unsigned long *)(bufout + bosleftspace(iw));


  nBanks = -1;
  nBanks_cur = -1;  
  for(i=0; i<NPREBANKS; i++) FragTab[i].n = -1;    

  for(i=0; i<22; i++) zerochan[i] = 65535;
  nslots = ndatawords = 0;


  /* hack */
  if(ttp->roc==27) refoffset = 1000;
  else             refoffset = REFOFFSET;


  /*  
PRINT_BUFFER;
  */

  /*
  printf("\n========= FIRST  PASS ==============\n");
  */
  ptdchead = (PTDCHeadPtr)&bufin[1]; /* first tdc header */
  ptdc = (PTDCPtr)&bufin[2];    /* first tdc data word */

  RAW_FRAGMENT_OPEN;

  while((long *)ptdc < ((&bufin[1])+bufin[0]))  /* loop over input buffer */  
  {
	/* check if we have at least one hit */
    if(ptdchead->count==1)
	{
      printf("SEVERE ERROR: NO REFERENCE HITS, ptdchead->count=%d, slot %d\n",
       ptdchead->count,ptdchead->slot);
      return(len);
	}

    endofboard = (PTDCPtr)ptdchead + ptdchead->count;  /* end of data for board */
    nslots ++;
    ndatawords += ptdchead->count;

    if(ptdchead->slot>21 || ptdchead->slot<1)
    {
      printf("ROC # %d Slot # %d :  Bad Block Read signature 0x%8.8X -> resyncronize !!!\n",
             ttp->roc,ptdchead->slot,*((unsigned long *)ptdchead));
      ptdc++;
      ptdchead = (PTDCHeadPtr)ptdc;
/*
PRINT_BUFFER;
*/
      continue;
    }

    while(ptdc->slot==ptdchead->slot)	        /* loop over present board */
	{
      /* ignore channel=0 for a while, have to decide how to process */
      if(ptdc->channel==0)
      {
        ZERO_CHAN;
        /*ptdc++;
		continue;*/
      }

      if(TT_IDNR[ptdc->slot][ptdc->channel] != FragTab[nBanks_cur].idnr)
      {
        RAW_FRAGMENT_CLOSE;
        RAW_FRAGMENT_OPEN;
      }

	  ptdc++;

      /* end of buffer ? new slot ? goto loop over input buffer */
	  if((long *)ptdc >= ((&bufin[1])+bufin[0]) || ptdc->slot!=ptdchead->slot)
      {
		/*
		if((int)ptdc != (int)endofboard)
        {
          printf("event %7d -> end of buffer 0x%08x\n",TT_nev,((&bufin[1])+bufin[0]));
          printf("event %7d -> ptdc=0x%08x endofboard=0x%08x (slots %2d %2d) (adrs 0x%08x 0x%08x)\n",
          TT_nev,(int)ptdc,(int)endofboard,ptdchead->slot,ptdc->slot,(long *)ptdc,((&bufin[1])+bufin[0]));		  
        }
		*/
		/*
printf("early break: 0x%08x 0x%08x %d %d\n",
(long *)ptdc,((&bufin[1])+bufin[0]),ptdc->slot,ptdchead->slot);
*/
        break;
      }

      /*if((long *)ptdc >= (long *)endofboard) break;*/

	}                                   /* end of while ptdc < endofboard */

	ptdchead = (PTDCHeadPtr)ptdc;
	ptdc++;

  }                           /* end of while ptdc < ((&bufin[1])+bufin[0]) */

  RAW_FRAGMENT_CLOSE1;

  /* return if no any data words except board headers */
  /*if(nslots == ndatawords) return(len);*/

  /*  
  printf("\n========= SECOND PASS ==============\n");
  printf("found %d banks\n",nBanks);
  for(i=0; i<=nBanks; i++)
  {
    printf("bank %d has %d fragments\n",i,FragTab[i].n);
  }
  */

  for(i=0; i<=nBanks; i++)           /* Loop on BOS Banks  */
  {
    ptdc = FragTab[i].begin[0];      /* set input data pointer     */

    /* create BOS bank */
    BANK_OPEN;

    /* Loop on fragments of current BOS Bank */
    for(j=0; j<=FragTab[i].n; j++)
    {
      ptdc = FragTab[i].begin[j]; /* set input data pointer */

	  /*
	  printf("fragment: i=%d, j=%d, begin=0x%08x, end=0x%08x\n",
			 i,j,FragTab[i].begin[j],FragTab[i].end[j]);
	  */
      while(ptdc < FragTab[i].end[j]) /* Loop on data words inside one fragment */
      {
		/*
printf("ptdc=0x%08x, end=0x%08x\n",ptdc,FragTab[i].end[j]);
		*/
        /* ignore tdc headers (slot==0) */
        if(ptdc->slot==0)
        {
          ptdc++;
          continue;
        }

        /* check the rest of space ( (hit+4) because roc29 need (hitl+2) ) */
        if( (long *)(hit+4) + NHWRES > endofbufout )
        {
          printf(" ERROR: bufout overflow - skip the rest ...\n");
          printf("        bufout=%u hit=%u endofbufout=%u\n",bufout,hit,endofbufout);
          BANK_CLOSE;
          return(len);
        }

if(ptdc == FragTab[i].end[j])
printf("RRRRRRRRR: 0x%08x 0x%08x\n",ptdc,FragTab[i].end[j]);

        /* check id */
        id = ttp->id2[ptdc->slot][0][ptdc->channel];
        if(id >= MAXID || id == 0)
        {
		  /*
 		  printf(" untabled id=%d (slot=%d channel=%d data=0x%08x) 0x%08x\n",
            id,ptdc->slot,ptdc->channel,ptdc->data,ptdc);
		  */
		  /*
PRINT_BUFFER;
PRINT_FRAGMENTS;
		  */
          ptdc ++;
          continue;
        }


		/*
if((zerochan[ptdc->slot]<0x9380)||(zerochan[ptdc->slot]>0x9b00))
printf("ERROR: slot=%2d offset=0x%04x\n",ptdc->slot,zerochan[ptdc->slot]);
		*/

        /* negative values means that 'refoffset' too small to cover tdc window;
		   we'll ignore negative values */
		if( (zerochan[ptdc->slot]+refoffset) < ptdc->data)
		{
          /*printf("ERROR: zerochan(%d)+refoffset(%d) < data(%d)\n",
            zerochan[ptdc->slot],refoffset,ptdc->data)*/;
	    }
        else
		{
          /* fill output info */
          *hit = id;
	      *(hit+1) = (zerochan[ptdc->slot] + refoffset - ptdc->data);
	      hit += 2;
		}

		ptdc ++;
      }                      /* End of Loop on data words inside fragment */
    }                        /* End of Loop on fragments of current BOS Bank */

    /* close bank */
    BANK_CLOSE;
  }



  /* returns full fragment length (long words) */  
  return(len);
}






