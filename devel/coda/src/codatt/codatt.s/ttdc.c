

#define DO_FBPM


/* ttdc.c

this subroutine does demultiplexing cases for each event
 
modified April '97 by DJT
-> ??? while loop for  same channel edge counting < endofboard NOT <=  ?????
-> new demultiplexing algorithm for mux case handling
-> status word mux_index filled for future use 

mux_index     case                                     edges     diff 
 err  stat

  0           1 edge 
      11      2 edge W0                                          1-0
      12      2 edge W1                                          1-0
      13      2 edge W0+W0                                       1-0
      14      2 edge W1+W1                                       1-0
  1           2 edge W0+W1     edge timing too ambiguous         XXX 
  2           2 edge W0-W1     edge timing too ambiguous         XXX
  3           2 edge unknown                                     XXX
      15      3 edge dp W0 | W0+rndm lost last edge     010      2-1
      16      3 edge dp W1 | W1+rndm lost last edge     010      2-1
      17      3 edge mux W0+W1       lost last edge     010      2-0
      18      3 edge mux W1+W0       lost last edge     010      2-0
      19      3 edge mux W0 in W1    lost last edge     010      1-0
  4           3 edge unknown                            010      XXX
      20      3 edge W0+rndm | dp W0 lost first edge    101      1-0
      21      3 edge W1+rndm | dp W1 lost first edge    101      1-0
      22      3 edge mux W1+W0       lost first edge    101      2-0
      23      3 edge mux W0+W1       lost first edge    101      2-0
      24      3 edge mux W0 in W1    lost first edge    101      2-1
  5           3 edge unknown                            101
  6           3 edge unknown
      25      4 edge W0+W1                                       3-2  1-0
      26      4 edge W1+W0                                       3-2  1-0
      27      4 edge dp W0                                       3-2
      28      4 edge dp W1                                       3-2
      29      4 edge mux W0 in W1                                3-0  2-1
      30      4 edge mux W1+W0                                   3-1  2-0
      31      4 edge mux W0+W1                                   3-1  2-0
      32      4 edge only W0 found            3-2 | 3-1 | 3-0 | 2-1 | 2-0 | 1-0
      33      4 edge only W1 found            3-2 | 3-1 | 3-0 | 2-1 | 2-0 | 1-0
  7           4 edge unknown
 10           failed all the above
 
notes: tdc data used is larger edge (earlier time in common stop)
       order in table is order of testing for valid-hit edge-case
       negative mux_index indicates no time extracted 
       second pulse of double pulse (4 edge case) can have shorter width 
                                              hence add extra jitter in if stmt
       w0l= width1-jitter ;  w0h = width1+jitter
       w1l= width2-jitter ;  w1h = width2+jitter
       hit array is (points to) output buffer and has structure
       hit[0] wire ID
       hit[1] tdc data
       hit[2] wire ID
       hit[3] tdc data
       ARBITRARY RULE: put subchan 0 data out first when 2 channels decode 
       use address arithmetic to control the loop
       example:
                bufin                         bufout
                   n       0                     x
                   x       1                     x
       head->      H       2             hit->   x
       tdc ->      D       .
                   D       .
 endofboard->      H       .
                   H       
 endofbank ->      -     &[1]+n
       
-----------------------------------------------------------------------------*/

#ifndef VXWORKS
#include <stdio.h>
#endif

#include "coda.h"
#include "tt.h"

/*#include "uthbook.h"*/
/*#include "sgutil.h"*/


#define TEST(num)
/* \
if((hit[0]&0xff)>192 || (hit[0]>>8)>36 || (hit[0]&0xff)==0 || (hit[0]>>8)==0) \
printf("id%02d: wire=%d layer=%d slot=%d channel=%d\n", \
num,hit[0]&0xff,hit[0]>>8,SLOT,CHANNEL) \
*/

#define PRINT_BUFFER \
  /*printf("Event ===============> \n");*/ \
  tst = &bufin[NHC]; \
  while((long *)tst < ((&bufin[1])+bufin[0])) \
  { \
    /*printf("word=%08x      slot=%02u chan=%02u data=%05u (count=%05u)\n", \
      *tst,((*tst)>>27)&0x1f,((*tst)>>17)&0x7f,(*tst)&0xffff,(*tst)&0x7ff);*/ \
    tst++; \
  }

#define look ttp

#define SKIP_ZERO \
  while(((long *)mtdc < ((&bufin[1])+bufin[0])) && (mtdchead->slot == 0)) \
  { \
    /*{rcst[0].stat[8] ++; rcst[0].slot = 0; rcst[0].event ++;}*/ \
    mtdchead++; \
    mtdc++; \
    fb++; \
    printf("ttdc: ZERO word from DC - skipped ...\n"); \
  }

#define SKIP_ZERO_FIRST \
  while(((long *)mtdc < ((&bufin[1])+bufin[0])) && (mtdchead->slot == 0)) \
  { \
    /*{rcst[0].stat[8] ++; rcst[0].slot = 0; rcst[0].event ++;}*/ \
    mtdchead++; \
    mtdc++; \
    fb++; \
	printf("ttdc: FIRST ZERO header word from DC - skipped ...\n"); \
  } \
  if((long *)mtdc >= ((&bufin[1])+bufin[0])) \
  { \
    /*{rcst[0].stat[9] ++; rcst[0].slot = 0; rcst[0].event ++;}*/ \
	/*printf("ttdc: all headers are zero !!!\n");*/ \
    return(len); \
  }

#define SKIP_64BIT \
  if(((long *)mtdc < ((&bufin[1])+bufin[0])) && (mtdchead->slot == SLOT64)) \
  { \
    mtdchead++; \
    mtdc++; \
    fb++; \
	printf("ttdc: 64 bit signature skipped ...\n"); \
  } \
  if((long *)mtdc >= ((&bufin[1])+bufin[0])) \
  { \
	/*printf("ttdc: end_of_buffer during SKIP_64BIT\n");*/ \
    return(len); \
  }

/*
#define MAXDIAG 100
#define DIAG if(diagcount<MAXDIAG) diagcount++; if(diagcount<MAXDIAG) printf
*/

#define BANK_OPEN \
  if(fb==(MTDCPtr)0) {/*printf("ERROR: ZERO !!!\n");*/exit(0);} \
  if(SLOT < 3 || SLOT > /*23-temporary !!!*/24 || CHANNEL < 0 || CHANNEL > 95) \
    printf("BANK_OPEN: slot=%d channel=%d\n",SLOT,CHANNEL); \
  savencol = NBOSCOL[SLOT][CHANNEL]; /* to be used in BANK_CLOSE */ \
  if(savencol <= 0) \
  { \
    printf("ttdc: ERROR: slot=%d channel=%d does not described in translation table\n", \
      SLOT,CHANNEL); \
    printf("ttdc: returns len=%d\n",len); \
    return(len); \
  } \
  ind = bosNopen(iw,NAME[SLOT][CHANNEL],NR[SLOT][CHANNEL], \
    NBOSCOL[SLOT][CHANNEL],0); \
  if(ind <= 0) \
  { \
    printf("ttdc: bosNopen returned %d !!! -> return len=%d\n",ind,len); \
    printf("ttdc: slot=%d channel=%d name>%4.4s< nr=%d nboscol=%d\n", \
      SLOT,CHANNEL,NAME[SLOT][CHANNEL],NR[SLOT][CHANNEL], \
      NBOSCOL[SLOT][CHANNEL]); \
    return(len); \
  } \
  hit = (unsigned short *)&iw[ind+1]

#define BANK_OPEN3 \
  if(fb==(MTDCPtr)0) {/*printf("ERROR: ZERO33 !!!\n");*/exit(0);} \
  if(SLOT < 3 || SLOT > 22 || CHANNEL < 0 || CHANNEL > 95) \
    printf("BANK_OPEN3: slot=%d channel=%d\n",SLOT,CHANNEL); \
  savencol = NBOSCOL[SLOT][CHANNEL]; /* to be used in BANK_CLOSE */ \
  if(savencol <= 0) \
  { \
    printf("ttdc: ERROR33: slot=%d channel=%d does not described in translation table\n", \
      SLOT,CHANNEL); \
	printf("ttdc: returns len=%d\n",len); \
    return(len); \
  } \
  ind = bosNopen(iw,NAME[SLOT][CHANNEL],NR[SLOT][CHANNEL], \
    NBOSCOL[SLOT][CHANNEL],0); \
  if(ind <= 0) \
  { \
    printf("ttdc: bosNopen3 returned %d !!! -> return len=%d\n",ind,len); \
    printf("ttdc: slot=%d channel=%d name>%4.4s< nr=%d nboscol=%d\n", \
      SLOT,CHANNEL,NAME[SLOT][CHANNEL],NR[SLOT][CHANNEL], \
      NBOSCOL[SLOT][CHANNEL]); \
    return(len); \
  } \
  hit = (unsigned short *)&iw[ind+1]

#define BANK_CLOSE /* fb not defined any more, it points to next word after buffer !!! */ \
  hit[0] = hit[1] = hit[2] = 0; \
  hit = (unsigned short *)(((int)hit + 3)/4 * 4); \
  len += bosNclose(iw,ind,savencol, (((unsigned long *)hit) - \
                                        ((unsigned long *)&iw[ind+1])) )

#define BANK_DISCARD /* discard opened bank */ \
  printf("discard bank\n"); \
  len += bosNclose(iw,ind,savencol,0)

#define PRESCALE 100
/* local event counter for l2 board prescaling */
static int l2_nev = 0;

static int diagcount = 0;
/*static int noisereduction = 0;*/ /* 0-OFF, 1-ON */
/*extern RCST rcst[NUM_SLOTS];*/ /* NUM_SLOTS slots, some of them empty */
static int dpres, jitter, width1, width2, w0l, w0h, w1l, w1h;


/******************************************************************************
******************************************************************************/
/*
void
TT_Prestart_TranslateDCBank()
{
  char *env;

  if( (env=getenv("NOISEREDUCTION")) != NULL )
  {
    if(*env == 'T')
    {
      noisereduction = 1;
      printf("ttdc: noise reduction is ON\n");
    }
    else
    {
      noisereduction = 0;
      printf("ttdc: noise reduction is OFF\n");
    }
  }

  return;
}
*/

/******************************************************************************

  Routine         : TT_TranslateDCBank

  Parameters      : bufin - pointer to the input RC## bank in BOSIO compressed
                            format (2-word header); bufin[0] is the number
                            of words in the RC## bank, following is the data
                    bufout - output(BOS)

  Discription     : This routine translates one fragment from CODA to
                      BOS format for DC only; 
                        
******************************************************************************/

int
TT_TranslateDCBank(long *bufin, long *bufout, TTSPtr ttp)
{
  long *iw, *tst, *word;
  unsigned short *hit, *hit3, *oldhit;
  MTDCPtr        endofboard, mtdc, fb;
  MTDCHeadPtr    mtdchead;	/* first word is header */
  int i, j, k=0, ind, bank, nhits=0, mux_index, len, len0, savencol, slot;
  int nwords, region1;
  long *endofbufout;
  int ntst = 0;
#ifdef DO_FBPM
  ADCHeadPtr adchead;
  ADCPtr adc;
  INDPtr fb1;
  int id,place,nwdadc;
  unsigned short *ids; /* partner address pointer */
  ids = ttp->ids;
#endif


  iw = &bufout[2];       /* set BOS pointer to output buffer */
  len = 0;
  endofbufout = bufout + bosleftspace(iw); /* set end_of_bufout pointer */

  region1 = 0;
  if(ROC == 5 || ROC == 6 || ROC == 11) region1 = 1;

  if(region1)
  {
    dpres  = R1_dpres ;
    jitter = R1_jitter;
    width1 = R1_width1;
    width2 = R1_width2;
  }
  else
  {
    dpres  = R23_dpres ;
    jitter = R23_jitter;
    width1 = R23_width1;
    width2 = R23_width2;
  }
  w0l  =  width1 - jitter;
  w0h  =  width1 + jitter;
  w1l  =  width2 - jitter;
  w1h  =  width2 + jitter;


  mtdchead = (MTDCHeadPtr)&bufin[1]; /* first tdc header */
  mtdc = fb = (MTDCPtr)&bufin[2];    /* first tdc data word */

  SKIP_64BIT;
  SKIP_ZERO_FIRST;

  slot = mtdchead->slot;
  if(slot <=0 || slot > 24)
  {
    ;
    /*printf("ttdc: ERROR: slot=%d\n",slot);*/
  }
  else
  {
    ;
    /*rcst[slot].slot = slot;
	  rcst[slot].event ++;*/
  }



#ifdef DO_FBPM /* TEMPORARY - FBPM ADC PROCESSING; SUPPOSE TO COME FIRST !!! */

  if(ttp->roc == 11 && mtdchead->slot == 19) /* loop over FBPM board */
  {
    adchead = (ADCHeadPtr)mtdchead;
    adc = (ADCPtr)mtdc;
    fb1 = (INDPtr)adc;
    nwdadc = adchead->count;
	/*
printf("FBPM bank handling, nw=%d ... (slot=%d ch=%d)\n",
nwdadc,fb1->slot,fb1->channel);
printf("NAME >%s<\n",&(NAME[fb1->slot][fb1->channel]));
printf("NR >%d<\n",NR[fb1->slot][fb1->channel]);
printf("NBOSCOL >%d<\n",NBOSCOL[fb1->slot][fb1->channel]);
	*/

    if(nwdadc > 0)
	{


      if(nwdadc>65)
      {
        printf("FBPM ERROR: nwdadc = %d\n",nwdadc);
        goto skipadc2;
      }



	  /*
mtdchead = mtdchead + nwdadc;
mtdc = fb = mtdchead + 1;
goto skipadc;
	  */





	  /* open FBPM bank */
      if(fb1==(INDPtr)0) {printf("FBPM_OPEN: ZERO !!!\n");exit(0);}
      if(fb1->slot != 19 || /*fb1->channel < 0 ||*/ fb1->channel > 63)
	  {
        printf("FBPM_OPEN: slot=%d channel=%d\n",fb1->slot,fb1->channel);
        goto skipadc0;
	  }
      savencol = NBOSCOL[fb1->slot][fb1->channel]; /* to be used in BANK_CLOSE */
      if(savencol <= 0)
      {
        printf("FBPM_OPEN: slot=%d channel=%d does not described in translation table\n",
          fb1->slot,fb1->channel);
        printf("FBPM_OPEN: returns len=%d\n",len);
        return(len);
      }
      ind = bosNopen(iw,NAME[fb1->slot][fb1->channel],NR[fb1->slot][fb1->channel],
                 NBOSCOL[fb1->slot][fb1->channel],0);
      if(ind <= 0)
      {
        printf("FBPM_OPEN: bosNopen returned %d !!! -> return len=%d\n",ind,len);
        printf("FBPM_OPEN: slot=%d channel=%d name>%4.4s< nr=%d nboscol=%d\n",
          fb1->slot,fb1->channel,NAME[fb1->slot][fb1->channel],NR[fb1->slot][fb1->channel],
          NBOSCOL[fb1->slot][fb1->channel]);
        return(len);
      }
      hit = (unsigned short *)&iw[ind+1];






      for(i=0; i<nwdadc; i++)
	  {
        id = ID[fb1->slot][fb1->channel];
        place = PLACE[fb1->slot][fb1->channel];

        if(ttp->partner[id] == 0)    /* met id first time */
        {
          *hit = id;
          *ids = id;
          ids++;
          ttp->partner[id] = hit;

		  for(k=1; k<NBOSCOL[fb1->slot][fb1->channel]; k++) *(hit+k) = 0;
		  *(hit+place+1) = adc->data;
		  hit += NBOSCOL[fb1->slot][fb1->channel];
		  DBG(" --> MET ID %4.4X (pl %i) FIRST  TIME\n",id,place);
		} 
        else                        /* met id second time */
		{
		  DBG(" --> MET ID %4.4X (pl %i) SECOND TIME\n",id,place);
	      *(ttp->partner[id]+place+1)= adc->data;
        }
        
        adc ++;
        fb1 ++;
      }

      /* cleanup partner array */

      k = (int)(ids - ttp->ids);
      ids = ttp->ids;
      DBG("Cleaning %i words in partners array\n",k);
      for(j=0; j<k; j++)
      {
        DBG("id = %i  partner[id] = 0x%8.8X\n",*ids,ttp->partner[*ids]);
        ttp->partner[*ids++] = 0;
      }
      ids = ttp->ids;


      /* close bank */
      hit[0] = hit[1] = hit[2] = 0;
      hit = (unsigned short *)(((int)hit + 3)/4 * 4);
      len += bosNclose(iw,ind,savencol,
                                (hit-(unsigned short *)&iw[ind+1])/savencol );
	  /*for DC was
      len += bosNclose(iw,ind,savencol,
              (((unsigned long *)hit) -((unsigned long *)&iw[ind+1])) )
	  */




    }
    else
	{
      printf("FBPM ERROR: adc header length is zero\n");
      goto skipadc1;
	}



    mtdc = (MTDCPtr)adc;

    mtdchead = (MTDCHeadPtr)mtdc;
    mtdc++;
    fb = mtdc;

    SKIP_64BIT;
    SKIP_ZERO;
	/*	
printf("... done.\n");
	*/
  }

#endif /* END OF FBPM ADC PROCESSING */


skipadc0:
skipadc:
skipadc1:
skipadc2:


  /*return(0);*/ /* EB crashes */



  BANK_OPEN;

  /*
TT_PrintRawBank((int *)&bufin[1], bufin[0], ttp);
printf("len: %d %d\n",bufin[0],bufin[-3]);
  */

  while((long *)mtdc < ((&bufin[1])+bufin[0]))   /* loop over input buffer */
  {
    SKIP_64BIT;

    DBG("MTDC Header 0x%8.8X : slot %d count %d\n",
               *((unsigned long *)mtdchead),
               mtdchead->slot,(unsigned long)mtdchead->count); 
    endofboard = (MTDCPtr)mtdchead + mtdchead->count;  /* end of data for board */

    if(mtdchead->slot<=0 || mtdchead->slot>25)
    {
      /*printf("ROC %d Event %d: Bad signature 0x%8.8X(slot %d) - resync ..\n",
             ttp->roc,TT_nev,*((unsigned long *)mtdchead),mtdchead->slot);
      printf("mtdc=0x%08x, end_of_buf=0x%08x (0x%08x 0x%08x)\n\n",
             (long *)mtdc,((&bufin[1])+bufin[0]),(&bufin[1]),bufin[0]);*/
/*
TT_PrintRawBank((int *)&bufin[1], bufin[0], ttp);
printf("len: %d %d\n\n\n",bufin[0],bufin[-3]);
*/
      mtdc++;
      fb = mtdc;
      mtdchead = (MTDCHeadPtr)mtdc;
      continue; /* goto loop over input buffer */
    }

/* TEMPORARY - IT IS AGAINST MAIN IDEA !!! */
    if(ttp->roc == 4 && mtdchead->slot == 5) /* loop over level 2 trigger board */
    {
      l2_nev ++;
      if((PRESCALE > 0) && (l2_nev>=PRESCALE)) /* prescale data from level 2 trigger board */
      {
        l2_nev = 0;
        while(mtdc->slot==mtdchead->slot)
        {
          /* search for all data from the same channel */
          k = 1;
          DBG("Channel %d : %x ",mtdc->channel,mtdc->data);
          while( ((long *)(mtdc+k)) < ((long *)endofboard) && /* was just endofboard */
                 (mtdc+k)->slot==mtdchead->slot &&
                 (mtdc+k)->channel==mtdc->channel )
          {
            DBG("%x ",(mtdc+k)->data);
            k++;
          }
          DBG("\n");

          /*rcst[mtdchead->slot].stat[0] ++;*/
          mtdc += k;
          fb = mtdc;

          /* end of buffer ? new slot ? goto loop over input buffer */
          if((long *)mtdc >= ((&bufin[1])+bufin[0]) ||
              mtdc->slot!=mtdchead->slot) break;
        }
      }
      else
      {
        while(mtdc->slot==mtdchead->slot)
        {
          /* search for all data from the same channel */
          k = 1;
          DBG("Channel %d : %x ",mtdc->channel,mtdc->data);
          while( (long *)(mtdc+k)<(long *)endofboard &&
                 (mtdc+k)->slot==mtdchead->slot &&
                 (mtdc+k)->channel==mtdc->channel )
          {
            DBG("%x ",(mtdc+k)->data);
            k++;
          }
          DBG("\n");

          /* both name and number of BOS bank can be changed for L2 translation */
          /* if changed - close old bank and start new one */

          if(TT_IDNR[SLOT][CHANNEL] != iw[ind-IDNR])
          /*if(NAME[SLOT][CHANNEL] != iw[ind-INAM] ||
			NR[SLOT][CHANNEL] != iw[ind-INR])*/
          {
            BANK_CLOSE;
            BANK_OPEN;
          }

          /* check the rest of space */
          if ( (long *)(hit+k) + NHWRES > endofbufout )
          {
            /*printf(" ERROR: bufout overflow 1 - skip the rest ...\n");*/
            BANK_DISCARD;
            /*rcst[mtdchead->slot].overflow ++;*/
            /*printf("        bufout=%u hit=%u endofbufout=%u\n",
              bufout,hit,endofbufout);*/
            goto exit;
          }

          /* filling bank */

          for(i=0; i<k; i++)
          {
            hit[0] = look->id2[SLOT][0][CHANNEL] + 256 * i;
            hit[1] = (mtdc+i)->data;
            hit += 2;
          }

          /*rcst[mtdchead->slot].stat[0] ++;*/
          mtdc += k;
          fb = mtdc;

          /* end of buffer ? new slot ? goto loop over input buffer */
          if((long *)mtdc >= ((&bufin[1])+bufin[0]) ||
              mtdc->slot!=mtdchead->slot) break;
        }
      }
    }
    else                            /* loop over present drift chamber board */
    {
      while(mtdc->slot==mtdchead->slot)
      {
        /* search for all data from the same channel */
        k = 1;
        DBG("Channel %d : %4.4X ",mtdc->channel,mtdc->data);
        while( (long *)(mtdc+k)<(long *)endofboard &&
               (mtdc+k)->channel==mtdc->channel &&
               (mtdc+k)->slot==mtdc->slot && (mtdc+k)->slot<26)
        {
          DBG("%4.4X ",(mtdc+k)->data);
          k++;
        }
        DBG("\n");

        /* only number of BOS bank can be changed for DC translation */
        /* if changed - close old bank and start new one */

        if(TT_IDNR[SLOT][CHANNEL] != iw[ind-IDNR])
        /*if(NR[SLOT][CHANNEL] != iw[ind-INR])*/
        {
          /* end of superlayer - remove the noise if applicable 
          if(noisereduction && ttp->roc!=5 && ttp->roc!=6 && ttp->roc!=11)
          {
            nwords = TT_DC_removenoise(iw, ind,
                        (unsigned long *)hit - (unsigned long *)&iw[ind+1]);

            hit3 = (unsigned short *)((long *)&iw[ind+1] + nwords);
            if(hit==hit3)
            {
              hit=hit3;
            }
            else
            {
              hit=hit3;
            }
          }
          */
          BANK_CLOSE;
          BANK_OPEN3;
        }
	    oldhit=hit;

        /* check the rest of space */

        if( (long *)(hit+4) + NHWRES > endofbufout )
        {
	      /*printf(" ERROR: bufout overflow 2 - skip the rest ...\n");*/
          BANK_DISCARD;
          /*rcst[mtdchead->slot].overflow ++;*/
	      /*printf("        bufout=%u hit=%u endofbufout=%u\n",
			bufout,hit,endofbufout);*/
          goto exit;
        }

        /* demultiplexing */
        mux_index=TT_DC_demux(&hit,mtdc,k,ttp);

        /*rcst[mtdchead->slot].stat[mux_index] ++;*/
        mtdc += k;
        fb = mtdc;

        /* end of buffer ? new slot ? goto loop over input buffer */
		if((long *)mtdc >= ((&bufin[1])+bufin[0]) ||
            mtdc->slot!=mtdchead->slot) break;

        /* done analyzing this channel; move to next channel on this board */	  
        DBG("  Mux case : %d -> %d hits\n",mux_index,(hit-oldhit)/2);

      } /* end of loop over present board */
    }

	mtdchead = (MTDCHeadPtr)mtdc;
	mtdc++;
	fb = mtdc;

    if((long *)mtdc < ((&bufin[1])+bufin[0]))
    {
      if(mtdchead->slot == 0)
      {
        /*PRINT_BUFFER;*/
        /*printf("k2=%d mux=%d\n",k,mux_index)*/;
      }
      if(mtdchead->slot > 0 && mtdchead->slot < 26)
      {
        ;
        /*rcst[mtdchead->slot].slot = mtdchead->slot;
		  rcst[mtdchead->slot].event ++;*/
      }
    }

  }                           /* end of while mtdc < ((&bufin[1])+bufin[0]) */

  BANK_CLOSE;

exit:

  /*if(len > 0)
    printf("ttdc: len=%d (0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x)\n",
      len,iw[0],iw[1],iw[2],iw[3],iw[4],iw[5]);
  */

  return(len); /* returns full fragment length (long words) */
}


/******************************************************************************

  Routine             : TT_DC_demux

  Parameters          : *phit - pointer to the empty space in BOS bank 
                        mtdc  - pointer to the first MTDC word for current channel
						k     - number of edges

  Discription         : This routine translates one MTDC channel from CODA to
                        BOS format for DC only; 
                        
******************************************************************************/

int
TT_DC_demux(unsigned short **phit, MTDCPtr mtdc, int k, TTSPtr ttp)
{

  MTDCPtr  fb = mtdc;
  int mux_index;
  unsigned short diff10, diff20, diff30;
  unsigned short diff21, diff31, diff32;

  mux_index = 0; /* just in case .. */
  switch(k) /* k = number of edges */
  {
    /* one-edge cases - send status only */
    case 1:
      mux_index = 0;
      break;
    /* two-edge cases */
	case 2:
      diff10 = (mtdc+1)->data - (mtdc)->data;
      if ( (diff10>=w0l) && (diff10<=w0h) )
      {
			(*phit)[0] = look->id2[SLOT][0][CHANNEL];
TEST(1);
	        (*phit)[1] = (mtdc+1)->data;
	        (*phit) += 2;
	        mux_index = 11;
		  }
		else if ( (diff10>=w1l) && (diff10<=w1h) )
		  {
			(*phit)[0] = look->id2[SLOT][1][CHANNEL];
TEST(2);
	        (*phit)[1] = (mtdc+1)->data;
	        (*phit) += 2;
	        mux_index = 12;
		  }
		else if ( (diff10>=2*w0l) && (diff10<=2*w0h) )
		  {
			(*phit)[0] = look->id2[SLOT][0][CHANNEL];
TEST(3);
	        (*phit)[1] = (mtdc+1)->data;
	        (*phit) += 2;
	        mux_index = 13;
		  }
		else if ( (diff10>=2*w1l)  && (diff10<=2*w1h) )
		  {
			(*phit)[0] = look->id2[SLOT][1][CHANNEL];
TEST(4);
	        (*phit)[1] = (mtdc+1)->data;
	        (*phit) += 2;
	        mux_index = 14;
		  }
		else if ( (diff10>=w0l+w1l) && (diff10<=w0h+w1h) )
		  {
			mux_index = 1;
		  }
		else if ( (diff10>=w1l-w0h) && (diff10<=w1h-w0l) )
		  {
			mux_index = 2;
		  }
		else
		  {
			mux_index = 3;
		  }
	    break;
/*
three-edge cases 
*/
	  case 3:
		diff10 = (mtdc+1)->data - mtdc->data;
		diff20 = (mtdc+2)->data - mtdc->data;
		diff21 = (mtdc+2)->data - (mtdc+1)->data;
		if ( ((mtdc+2)->edge == 0) &&
			 ((mtdc+1)->edge == 1) &&
			 ((mtdc+0)->edge == 0) )
		  {
			if ((diff21>=w0l) && (diff21<=w0h) && ((mtdc)->data<w0h))
			  {
				(*phit)[0] = look->id2[SLOT][0][CHANNEL];
TEST(5);
				(*phit)[1] = (mtdc+2)->data;
				(*phit) += 2;
				mux_index = 15;
			  }
			else if ((diff21>=w1l) && (diff21<=w1h) && ((mtdc)->data<w1h))
			  {
				(*phit)[0] = look->id2[SLOT][1][CHANNEL];
TEST(6);
				(*phit)[1] = (mtdc+2)->data;
				(*phit) += 2;
				mux_index = 16;
			  }
			else if ((diff20>=w0l) && (diff20<=w0h) && ((mtdc+1)->data<w1h))
			  {
				(*phit)[0] = look->id2[SLOT][0][CHANNEL];
TEST(7);
				(*phit)[1] = (mtdc+2)->data;
				(*phit) += 2;
				mux_index = 17;
			  }
			else if ((diff20>=w1l) && (diff20<=w1h) && ((mtdc+1)->data<w0h))
			  {
				(*phit)[0] = look->id2[SLOT][1][CHANNEL];
TEST(8);
				(*phit)[1] = (mtdc+2)->data;
				(*phit) += 2;
				mux_index = 18;
			  }
			else if ((diff10>=w0l) && (diff10<=w0h) && ((mtdc+2)->data<w1h))
			  {
				(*phit)[0] = look->id2[SLOT][0][CHANNEL];
TEST(9);
				(*phit)[1] = (mtdc+1)->data;
				(*phit) += 2;
				mux_index = 19;
			  }
			else 
			  {
				mux_index = 4;
			  }
		  }
		else if ( ((mtdc+2)->edge == 1) && \
				  ((mtdc+1)->edge == 0) && \
				  ((mtdc+0)->edge == 1) )
		  {
	        if ( (diff10>=w0l) && (diff10<=w0h) )
	          {
				(*phit)[0] = look->id2[SLOT][0][CHANNEL];
TEST(10);
				(*phit)[1] = (mtdc+1)->data;
				(*phit) += 2;
				mux_index = 20;
	          }
	        else if ( (diff10>=w1l) && (diff10<=w1h) )
	          {
				(*phit)[0] = look->id2[SLOT][1][CHANNEL];
TEST(11);
				(*phit)[1] = (mtdc+1)->data;
				(*phit) += 2;
				mux_index = 21;
	          }
	        else if ( (diff20>=w0l) && (diff20<=w0h) )
	          {
				(*phit)[0] = look->id2[SLOT][0][CHANNEL];
TEST(12);
				(*phit)[1] = (mtdc+2)->data;
				(*phit) += 2;
				mux_index = 22;
	          }
	        else if ( (diff20>=w1l) && (diff20<=w1h) )
	          {
				(*phit)[0] = look->id2[SLOT][1][CHANNEL];
TEST(13);
				(*phit)[1] = (mtdc+2)->data;
				(*phit) += 2;
				mux_index = 23;
	          }
	        else if ( (diff21>=w0l) && (diff21<=w0h) )
	          {
				(*phit)[0] = look->id2[SLOT][0][CHANNEL];
TEST(14);
				(*phit)[1] = (mtdc+2)->data;
				(*phit) += 2;
				mux_index = 24;
	          }
			else 
			  {
				mux_index = 5;
			  }
		  }  
		else
		  {
			mux_index = 6;
		  }
		break;	  
/*
  four-edge cases 
*/
	  case 4:
		diff10 = (mtdc+1)->data - mtdc->data;
		diff20 = (mtdc+2)->data - mtdc->data;
		diff30 = (mtdc+3)->data - mtdc->data;
		diff31 = (mtdc+3)->data - (mtdc+1)->data;
		diff21 = (mtdc+2)->data - (mtdc+1)->data;
		diff32 = (mtdc+3)->data - (mtdc+2)->data;
		
		if ( (diff10>=w1l) && (diff10<=w1h) && \
			 (diff32>=w0l) && (diff32<=w0h) )
		  {
			(*phit)[0] = look->id2[SLOT][0][CHANNEL];
TEST(15);
			(*phit)[1] = (mtdc+3)->data; 
			(*phit)[2] = look->id2[SLOT][1][CHANNEL];
TEST(16);
			(*phit)[3] = (mtdc+1)->data; 
			(*phit) += 4;	
			mux_index = 25;
		  }
		else if ( (diff10>=w0l) && (diff10<=w0h) && \
				  (diff32>=w1l) && (diff32<=w1h) )
		  {
			(*phit)[0] = look->id2[SLOT][0][CHANNEL];
TEST(17);
			(*phit)[1] = (mtdc+1)->data; 
			(*phit)[2] = look->id2[SLOT][1][CHANNEL];
TEST(18);
			(*phit)[3] = (mtdc+3)->data; 
			(*phit) += 4;	
			mux_index = 26;
		  }
		else if ( (diff10>=w0l-jitter) && (diff10<=w0h+jitter) && \
				  (diff32>=w0l) && (diff32<=w0h) )
		  {
			(*phit)[0] = look->id2[SLOT][0][CHANNEL];
TEST(19);
			(*phit)[1] = (mtdc+3)->data; 
			(*phit) += 2;	
			mux_index = 27;
		  }
		else if ( (diff10>=w1l-jitter) && (diff10<=w1h+jitter) && \
				  (diff32>=w1l) && (diff32<=w1h) )
		  {
			(*phit)[0] = look->id2[SLOT][1][CHANNEL];
TEST(20);
			(*phit)[1] = (mtdc+3)->data; 
			(*phit) += 2;	
			mux_index = 28;
		  }
		else if ( (diff30>=w1l) && (diff30<=w1h) && \
				  (diff21>=w0l) && (diff21<=w0h) )
		  {
			(*phit)[0] = look->id2[SLOT][0][CHANNEL];
TEST(21);
			(*phit)[1] = (mtdc+2)->data; 
			(*phit)[2] = look->id2[SLOT][1][CHANNEL];
TEST(22);
			(*phit)[3] = (mtdc+3)->data; 
			(*phit) += 4;	
			mux_index = 29;
		  }
		else if ( (diff31>=w1l) && (diff31<=w1h) && \
				  (diff20>=w0l) && (diff20<=w0h) )
		  {
			(*phit)[0] = look->id2[SLOT][0][CHANNEL];
TEST(23);
			(*phit)[1] = (mtdc+2)->data; 
			(*phit)[2] = look->id2[SLOT][1][CHANNEL];
TEST(24);
			(*phit)[3] = (mtdc+3)->data; 
			(*phit) += 4;	
			mux_index = 30;
		  }
		else if ( (diff20>=w1l) && (diff20<=w1h) && \
				  (diff31>=w0l) && (diff31<=w0h) )
		  {
			(*phit)[0] = look->id2[SLOT][0][CHANNEL];
TEST(25);
			(*phit)[1] = (mtdc+3)->data; 
			(*phit)[2] = look->id2[SLOT][1][CHANNEL];
TEST(26);
			(*phit)[3] = (mtdc+2)->data; 
			(*phit) += 4;	
			mux_index = 31;
		  }
      /* begin search for single hiits in 4 edge cases */
		else if ( (diff32>=w0l) && (diff32<=w0h) || \
				  (diff31>=w0l) && (diff31<=w0h) || \
				  (diff30>=w0l) && (diff30<=w0h) )
		  {
			(*phit)[0] = look->id2[SLOT][0][CHANNEL];
TEST(27);
			(*phit)[1] = (mtdc+3)->data; 
			(*phit) += 2;	
			mux_index = 32;
		  }
		else if ( (diff32>=w1l) && (diff32<=w1h) || \
				  (diff31>=w1l) && (diff31<=w1h) || \
				  (diff30>=w1l) && (diff30<=w1h) )
		  {
			(*phit)[0] = look->id2[SLOT][1][CHANNEL];
TEST(28);
			(*phit)[1] = (mtdc+3)->data; 
			(*phit) += 2;	
			mux_index = 33;
		  }
		else if ( (diff21>=w0l) && (diff21<=w0h) || \
				  (diff20>=w0l) && (diff20<=w0h) )
		  {
			(*phit)[0] = look->id2[SLOT][0][CHANNEL];
TEST(29);
			(*phit)[1] = (mtdc+2)->data; 
			(*phit) += 2;	
			mux_index = 32;
		  }
		else if ( (diff21>=w1l) && (diff21<=w1h) || \
				  (diff20>=w1l) && (diff20<=w1h) )
		  {
			(*phit)[0] = look->id2[SLOT][1][CHANNEL];
TEST(30);
			(*phit)[1] = (mtdc+2)->data; 
			(*phit) += 2;	
			mux_index = 33;
		  }
		else if ( (diff10>=w0l) && (diff10<=w0h) )
		  {
			(*phit)[0] = look->id2[SLOT][0][CHANNEL];
TEST(31);
			(*phit)[1] = (mtdc+1)->data; 
			(*phit) += 2;	
			mux_index = 32;
		  }
		else if ( (diff10>=w1l) && (diff10<=w1h) )
		  {
			(*phit)[0] = look->id2[SLOT][1][CHANNEL];
TEST(32);
			(*phit)[1] = (mtdc+1)->data; 
			(*phit) += 2;	
			mux_index = 33;
		  }
		else
		  {
			mux_index = 7;
		  }
	    break;
/*
very weird cases 
*/
	  default:
		mux_index = 10;
		break;
	  }                                                    /* switch(k) */

	  return(mux_index);
}


/******************************************************************************

  Routine             : TT_DC_removenoise

  Parameters          : 

  Discription         : This routine removes single hits and returns the number
                        of hits remains

******************************************************************************/
/*
int
TT_DC_removenoise(long *iw, int ind, int len)
{
  Word192 bitmap[6];
  DC0DataPtr hits0;
  int i, ret, lay;
  unsigned char wire, layer;
  unsigned long *hit, *hit1, *htt, tmp;
  int nlay = 6;
  int nwire = 192;
  static int layershifts[6] = {0, 1, 1, 1, 1, 1};

  hits0 = (DC0DataPtr)&iw[ind+1];
  memset(bitmap,0,6*sizeof(Word192));
  for(i=0; i<len; i++)
  {
    BREAKSHORT(hits0->id, wire, layer);
    if(GOODHIT)
    {
      wire--;
      layer--;
      lay = layer % 6;
      SetBitWord192(&bitmap[lay], wire);
    }
    hits0++;
  }

  ret = RemoveNoise(bitmap, nlay, nwire, layershifts);

  hits0 = (DC0DataPtr)&iw[ind+1];
  htt = hit = hit1 = (unsigned long *)&iw[ind+1];

  for(i=0; i<len; i++)
  {
    BREAKSHORT(hits0->id, wire, layer);
    if(GOODHIT)
    {
      wire--;
      layer--;
      lay = layer % 6;
      if(CheckBitWord192(&bitmap[lay], wire))
      {
        tmp = *hit1;
        *hit = tmp;
        hit++;
      }
    }
    hits0++;
    hit1++;
  }
  return(hit-(unsigned long *)&iw[ind+1]);
}
*/


/* return 1 if buffer make sense, otherwise return 0 */
/* bufin[0] contains the number of words */
/* tdc1877 information starts from bufin[1] */

int
TT_DC_BufferCheck(unsigned int *bufin)
{
  int j, slotold, counter;
  unsigned int *p, *end;
  INDPtr fb;    /* input data pointer */

  if(bufin[0] <= 1) return(0);
  slotold = -1;
  counter = 0;
  p = (unsigned int *) &bufin[1];
  for(end=p+bufin[0]; p<end; )
  {
    fb = (INDPtr)p;

    if(fb->empty == 0) /* header */
    {
      if(counter != 0) return;
      if(fb->slot == slotold) return(0);
      counter = (fb->data) & 0x7FF;
      if(counter <= 0) return(0);
      counter --;
    } 
    else /* data word */
    {
      counter --;
    }

    slotold = fb->slot;
    p++;
  }	

  return(1);
}








