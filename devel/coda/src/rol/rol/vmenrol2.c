
/* hps2.c - second readout list for VXS crates with FADC250 boards */

#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#ifndef VXWORKS
/*for fchmod*/
#include <sys/types.h>
#include <sys/stat.h>
#endif

#ifndef VXWORKS
#include <sys/types.h>
#include <time.h>
#endif

#include "circbuf.h"


#undef DEBUG
#undef DEBUG1 /*TI*/
#undef DEBUG2 /* TDC */
#undef DEBUG3 /*SSP*/
#undef DEBUG4 /*VSCM*/
#undef DEBUG5 /*HEAD*/
#undef DEBUG_BSCAN


#define ROL_NAME__ "VMENROL2"

/* name used by loader */
#ifdef STANDALONE
#define INIT_NAME vmenrol2_standalone__init
#else
#define INIT_NAME vmenrol2__init
#endif

/*#define POLLING_MODE*/
#define EVENT_MODE

#include "rol.h"
#include "EVENT_source.h"
/************************/
/************************/

#include "coda.h"
#include "tt.h"


#define MYNEV 21000

int mynev; /*defined in tttrans.c */




#undef SPLIT_BLOCKS


#define PASS_AS_IS


#define MODE7



/* open composite bank */
#define CCOPEN(btag,fmt,bnum) \
  /*if it is first board, open bank*/ \
  if(a_slot_old==-1) \
  { \
    { \
      int len1, n1; \
      char *ch; \
      len1 = strlen(fmt); /* format length in bytes */ \
      n1 = (len1+5)/4; /* format length in 32-bit words */ \
      dataout_save1 = dataout ++; /*remember '0xf' bank length location*/ \
      *dataout++ = (btag<<16) + (0xf<<8) + bnum; /*bank header*/ \
      /* tagsegment header following by format */ \
      *dataout++ = (len1<<20) + (0x6<<16) + n1; \
      ch = (char *)dataout; \
      strncpy(ch,fmt,len1); \
      ch[len1]='\0';ch[len1+1]='\4';ch[len1+2]='\4';ch[len1+3]='\4';ch[len1+4]='\4'; \
      dataout += n1; \
      /* 'internal' bank header */ \
      dataout_save2 = dataout ++;  /*remember 'internal' bank length location*/ \
      *dataout++ = (0<<16) + (0x0<<8) + 0; \
    } \
    b08 = (unsigned char *)dataout; \
    /*printf("CCOPEN: %x \n",*dataout_save1); */\
  } \
  /*if new slot, write stuff*/ \
  if(a_slot != a_slot_old) \
  { \
    a_channel_old = -1; /*for new slot, reset a_channel_old to -1*/ \
    a_slot_old = a_slot; \
    *b08++ = a_slot; \
    b32 = (unsigned int *)b08; \
    *b32 = a_triggernumber; \
    b08 += 4; \
    b64 = (unsigned long long *)b08; \
    *b64 = (((unsigned long long)a_trigtime[0])<<24) | a_trigtime[1];	\
    b08 += 8; \
    /*set pointer for the number of channels*/ \
    Nchan = (unsigned int *)b08; \
    Nchan[0] = 0; \
    b08 += 4; \
  /*  printf("CCOPEN: %x \n",*dataout_save1); */\
    }



/* close composite bank */
#define CCCLOSE \
{ \
  unsigned int padding; \
  dataout = (unsigned int *) ( ( ((unsigned int)b08+3)/4 ) * 4); \
  padding = (unsigned int)dataout - (unsigned int)b08; \
  /*dataout_save1[1] |= (padding&0x3)<<14;*/ \
  dataout_save2[1] |= (padding&0x3)<<14; \
 /* printf("CCCLOSE: 0x%08x %d --- 0x%08x %d --> padding %d\n",dataout,dataout,b08,b08,((dataout_save2[1])>>14)&0x3); */\
  *dataout_save1 = (dataout-dataout_save1-1); \
  *dataout_save2 = (dataout-dataout_save2-1); \
  lenout += (*dataout_save1+1); \
  lenev += (*dataout_save1+1); \
  b08 = NULL; \
}





#define CPOPEN(btag,btyp,bnum) \
{ \
  dataout_save1 = dataout ++; /*remember beginning of the bank address, exclusive bank length will be here*/ \
  *dataout++ = (btag<<16) + (btyp<<8) + bnum; /*bank header*/ \
 /* printf("CPOPEN: %x \n",*dataout_save1);*/ \
  b08 = (unsigned char *)dataout; \
}

#define CPCLOSE \
{ \
  unsigned int padding; \
  /*printf("CPCLOSE: dataout before = 0x%x\n",dataout);*/		 \
  dataout = (unsigned int *) ( ( ((unsigned int)b08+3)/4 ) * 4); \
  padding = (unsigned int)dataout - (unsigned int)b08; \
  dataout_save1[1] |= (padding&0x3)<<14; /*update bank header (2nd word) with padding info*/ \
  /*printf("CPCLOSE: 0x%x %d --- 0x%x %d --> padding %d\n",dataout,dataout,b08,b08,((dataout_save1[1])>>14)&0x3);*/  \
  *dataout_save1 = (dataout-dataout_save1-1); /*write bank length*/ \
 /*printf("CPCLOSE: *dataout_save1 = %d\n",*dataout_save1);	*/	\
  lenout += (*dataout_save1+1); \
  lenev += (*dataout_save1+1); \
  b08 = NULL; \
}

#define CPEXIT \
  rol->dabufp[0] = lenout - 1; \
  rol->dabufp[1] = rol->dabufpi[1]; \
  rol->dabufp[lenout] = 1;  /*for compatibility with new ROC2; will be redefined to the actual number events in block*/



/* following macros loops over input bank of banks, set pointers to internal banks data areas,
and extract info about banks found; to be used in ROL2 */ \
#define MAXBANKS 10



#define BANKSCAN \
  { \
    int nw, jj;	\
    unsigned int *ptr, *ptrend; \
    nbanks = 0; \
    ptr = (unsigned int *)((rol->dabufpi));	\
	ptrend = ptr + *(rol->dabufpi); \
    ptr +=2; /*skip bank-of-banks header*/ \
	/*printf("BANKSCAN: tag=%d typ=%d num=%d word=%x\n",((*(ptr-1))>>16)&0xFF,((*(ptr-1))>>8)&0xFF,(*(ptr-1))&0xFF,*(ptr-1));fflush(stdout);*/ \
	while(ptr < ptrend) \
    { \
    /*  printf("BANKSCAN[%d]: while begin: ptr=0x%08x ptrenv=0x%08x (%d)\n",nbanks,ptr,ptrend,(ptrend-ptr));fflush(stdout);*/\
      banknw[nbanks] = (*ptr) - 1; /*data length only*/ \
     /* printf("nw=%d\n",banknw[nbanks]);fflush(stdout); */\
      ptr ++; \
      banktag[nbanks] = ((*ptr)>>16)&0xffff; \
    /*  printf("tag=0x%08x\n",banktag[nbanks]);fflush(stdout); */\
      bankpad[nbanks] = ((*ptr)>>14)&0x3; \
      /*printf("pad=%d\n",bankpad[nbanks]);fflush(stdout); */\
      banktyp[nbanks] = ((*ptr)>>8)&0x3f; \
   /*   printf("typ=0x%08x\n",banktyp[nbanks]);fflush(stdout);*/ \
      banknr[nbanks] = (*ptr)&0xff; \
     /* printf("nr=%d\n",banknr[nbanks]);fflush(stdout);*/ \
      ptr ++; \
      bankdata[nbanks] = ptr; \
    /*  printf("data(ptr)=0x%08x\n",bankdata[nbanks]);fflush(stdout);*/ \
      ptr += banknw[nbanks]; \
      nbanks ++; \
      if(nbanks >= MAXBANKS) {printf("rol.h ERROR: nbanks=%d - exit\n");fflush(stdout);break;}; \
    /*  printf("BANKSCAN[%d]: while end: ptr=0x%08x ptrenv=0x%08x (%d)\n",nbanks,ptr,ptrend,(ptrend-ptr));fflush(stdout);*/ \
    } \
  /*  for(jj=0; jj<nbanks; jj++) printf("bankscan[%d]: tag 0x%08x typ=%d nr=%d nw=%d dataptr=0x%08x\n", \
      jj,banktag[jj],banktyp[jj],banknr[jj],banknw[jj],bankdata[jj]); */\
  }












#undef DO_PEDS



#define MAXBLOCK 22  /* 22-max# of blocks=boards of certain type */
#define MAXEVENT 256 /* max number of events in one block */


/* jlab boards data type defs */
#define DATA_TYPE_BLKHDR    0x10
#define DATA_TYPE_BLKTLR    0x11
#define DATA_TYPE_EVTHDR    0x12
#define DATA_TYPE_TRGTIME   0x13
#define DATA_TYPE_BCOTIME   0x14
#define DATA_TYPE_FSSREVT   0x18
#define DATA_TYPE_DNV       0x1E
#define DATA_TYPE_FILLER    0x1F


/* MUST COME FROM CONFIG FILE !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */
#define VSCM_BCO_FREQ 16

#define NSLOTS 22

#ifdef DO_PEDS
#define NPEDEVENTS 100
#define NCHANS 16
static int npeds[NSLOTS][NCHANS];
static float pedval[NSLOTS][NCHANS];
static float pedrms[NSLOTS][NCHANS];
static FILE *fd = NULL;
static FILE *fc = NULL;
static char *dir = NULL;
static char *expid = NULL;
#endif



/* v1190 stuff */
static int tdctypebyslot[NSLOTS];
static int slotnums[NSLOTS];
static int error_flag[NSLOTS];




/* user routines */

void rol2trig(int a, int b);
void rol2trig_done();

static void
__download()
{
  rol->poll = 1;

  return;
}

static void
__prestart()
{
  int ii, ntdcs;

  printf("INFO: Entering Prestart ROL2\n");

  /* Clear some global variables etc for a clean start */
  CTRIGINIT;

  /* init trig source EVENT */
  EVENT_INIT;

  /* Register a sync trigger source (up to 32 sources) */
  CTRIGRSS(EVENT, 1, rol2trig, rol2trig_done); /* second arg=1 - what is that ? */

  rol->poll = 1;

  rol->recNb = 0;


#ifndef NIOS

  /* get some tdc info fron rol1 */
  ntdcs = 1;
  slotnums[0]=13;
  tdctypebyslot[13]=2;
  printf("ROL2: ntdcs_1=%d\n",ntdcs);
  for(ii=0; ii<NSLOTS; ii++) printf(" slot %d, type %d\n",ii,tdctypebyslot[ii]);

  printf("ROL2: ntdcs_2=%d\n",ntdcs);
  for(ii=0; ii<ntdcs; ii++) printf(" tdc_id %d, slotnum %d\n",ii,slotnums[ii]);

#endif


  printf("INFO: Prestart ROL22 executed\n");

  return;
}

static void
__end()
{
  printf("INFO: User End 2 Executed\n");

  return;
}

static void
__pause()
{
  printf("INFO: User Pause 2 Executed\n");

  return;
}

static void
__go()
{
  printf("User Go 2 Reached\n");fflush(stdout);

  mynev = 0;

  printf("INFO: User Go 2 Executed\n");fflush(stdout);

  return;
}


#define NEV 50
#define NSL 22
#define NCH 16
#define NPL 4



void
rol2trig(int a, int b)
{
  
  /* THIS IS CPINIT */
  unsigned int *dataout_save1; 
  unsigned int *dataout_save2; 
  unsigned char *b08; 
  unsigned short *b16; 
  unsigned int *b32; 
  unsigned long long *b64; 
  int lenin = *(rol->dabufpi) - 1;	
  int lenout = 2;  /* start from 3rd word, leaving first two for bank-of-banks header */ 
  int lenev; 
  unsigned int *datain = (unsigned int *)((rol->dabufpi)+2); 
  unsigned int *dataout = (unsigned int *)((rol->dabufp)+2); 
  unsigned int *header = (unsigned int *)(rol->dabufp); 
 
  /*THIS IS BANKINIT*/
  int nbanks; 
  unsigned int banktag[MAXBANKS]; 
  unsigned int banknr[MAXBANKS]; 
  unsigned int banknw[MAXBANKS];
  unsigned int banktyp[MAXBANKS]; 
  unsigned int bankpad[MAXBANKS]; 
  unsigned int *bankdata[MAXBANKS]; 

  int nB[MAXBANKS], iB[MAXBANKS][MAXBLOCK], sB[MAXBANKS][MAXBLOCK], nE[MAXBANKS][MAXBLOCK];
  int iE[MAXBANKS][MAXBLOCK][MAXEVENT], lenE[MAXBANKS][MAXBLOCK][MAXEVENT];
#ifdef PASS_AS_IS
  int nASIS, iASIS[MAXBANKS];
#endif

  unsigned int *iw;
  int i, j, k, m, iii, ind, bank, nhits=0, mux_index, rlen, printing, nnE, iev, ibl;
  int nr = 0;
  int ncol = 2;
  int a_channel, a_chan1, a_chan2, a_nevents, a_blocknumber, a_triggernumber, a_module_id;
  int a_windowwidth, a_pulsenumber, a_firstsample, samplecount;
  int a_adc1, a_adc2, a_valid1, a_valid2, a_nwords, a_slot, a_slot2, a_slot3;
  int a_hfcb_id, a_chip_id, a_chan;
  unsigned int a_bco, a_bco1;
  int a_slot_prev;
  int a_qualityfactor, a_pulseintegral, a_pulsetime, a_vm, a_vp;
  int a_trigtime[4];
  int a_tdc, a_edge;
  int a_slot_old;
  int a_channel_old;
  int npedsamples;
  int error;
  int ndnv, nw;
  char errmsg[256];
  unsigned int *StartOfBank;
  char *ch;
  unsigned int *Nchan, *Npuls, *Nsamp;
  int islot, ichan, ii, jj, kk;
  int banknum = 0;
  int have_time_stamp, a_nevents2, a_event_type;
  int a_event_number_l, a_timestamp_l, a_event_number_h, a_timestamp_h;
  int a_clusterN, a_clusterE, a_clusterY, a_clusterX, a_clusterT, a_type, a_data, a_time;
  long long timestamp, latency, latency_offset;

  int nheaders, ntrailers, nbcount, nbsubtract, nwtdc;
  unsigned int *tdchead, tdcslot, tdcchan, tdcval, tdc14, tdcedge, tdceventcount;
  unsigned int tdceventid, tdcbunchid, tdcwordcount, tdcerrorflags;
  unsigned int NBsubtract = 9; /*same for v1190 and v1290 ?*/

#ifdef MODE7
  /* because of wierd FADC data format in mode 7 (all channels reports PULSE INTEGRAL,
  then all channels reports PULSE TIME, we'll remember those values in pass 1 in array,
  and use that array in pass 2 instead of actual values from buffer */
  static int npulse_integral[NEV][NSL][NCH];
  static int npulse_time[NEV][NSL][NCH];
  static int npulse_VmVp[NEV][NSL][NCH];
  static int pulse_integral[NEV][NSL][NCH][NPL];
  static int pulse_time[NEV][NSL][NCH][NPL];
  static int pulse_Vm[NEV][NSL][NCH][NPL];
  static int pulse_Vp[NEV][NSL][NCH][NPL];
#endif


#ifdef DO_PEDS
  char fname[1024]; /* old format */
  char cname[1024]; /* cnf format */
#endif

  mynev ++; /* needed by ttfa.c */



#ifdef MODE7
  /* clean up pulse arrays */
  for(iev=0; iev<NEV; iev++)
  {
    for(i=0; i<NSL; i++)
    {
	  for(j=0; j<NCH; j++)
	  {
        npulse_integral[iev][i][j] = 0;
        npulse_time[iev][i][j] = 0;
        npulse_VmVp[iev][i][j] = 0;
		/*
	    for(k=0; k<NPL; k++)
	    {
          pulse_integral[iev][i][j][k] = 0;
          pulse_time[iev][i][j][k] = 0;
          pulse_Vm[iev][i][j][k] = 0;
          pulse_Vp[iev][i][j][k] = 0;
	    }
		*/
	  }
    }
  }
#endif


 BANKSCAN;

#ifdef DEBUG_BSCAN
  printf("nbanks=%d\n",nbanks);fflush(stdout);
  for(jj=0; jj<nbanks; jj++) printf("bankscan[%d]: tag 0x%08x typ=%d nr=%d nw=%d dataptr=0x%08x\n",
      jj,banktag[jj],banktyp[jj],banknr[jj],banknw[jj],bankdata[jj]);fflush(stdout);
#endif

 

  /* first for() over banks from rol1 */
#ifdef PASS_AS_IS
  nASIS = 0; /*cleanup AS IS banks counter*/
#endif
  for(jj=0; jj<nbanks; jj++)
  {
    datain = bankdata[jj];
    lenin = banknw[jj];
#ifndef VXWORKS
#ifndef NIOS
    /* swap input buffer (assume that data from VME is big-endian, and we are on little-endian Intel) */
    if(banktyp[jj] != 3) for(ii=0; ii<lenin; ii++) datain[ii] = LSWAP(datain[ii]);
#endif
#endif


/************************************/
/*Start first pass on raw banks here*/
/************************************/



    if(banktag[jj] == 0xe109) /* 0xe109 FADC250 hardware format */
	{
      banknum = rol->pid;

      /**************/
      /* FIRST PASS:*/

	  /* check data and fill following:
nB[jj]               - incremented on every block header, effectively count boards of this type
iB[jj][nB]           - block start index
sB[jj][nB]           - slot number

nE[jj][nB]           - event counter in current block - same for all blocks = nnE
iE[jj][nB][nE[nB]]   - event start index
lenE[jj][nB][nE[nB]] - event length in words
*/

#ifdef DEBUG
      printf("\nFIRST PASS FADC250\n\n");
#endif

      error = 0;
      ii=0;
      printing = 1;
      a_slot_prev = -1;
      nB[jj] = 0; /*cleanup block counter*/
      while(ii<lenin)
      {
#ifdef DEBUG
        printf("[%3d] 0x%08x\n",ii,datain[ii]);
#endif
        if( ((datain[ii]>>27)&0x1F) == 0x10) /*block header*/
        {
          a_slot_prev = a_slot;
          a_slot = ((datain[ii]>>22)&0x1F);
          a_module_id = ((datain[ii]>>18)&0xF);
          a_blocknumber = ((datain[ii]>>8)&0x3FF);
          a_nevents = (datain[ii]&0xFF);
#ifdef DEBUG
	      printf("[%3d] BLOCK HEADER: slot %d, nevents %d, block number %d module id %d\n",ii,
				 a_slot,a_nevents,a_blocknumber,a_module_id);
          printf(">>> update iB and nB\n");
#endif
          nB[jj]++;            /*increment block counter*/
          iB[jj][nB[jj]-1] = ii;     /*remember block start index*/
          sB[jj][nB[jj]-1] = a_slot; /*remember slot number*/
          nE[jj][nB[jj]-1] = 0;      /*cleanup event counter in current block*/
#ifdef DEBUG
		  printf("0xe109: jj=%d nB[jj]=%d\n",jj,nB[jj]);
#endif
	      ii++;
        }
        else if( ((datain[ii]>>27)&0x1F) == 0x11) /*block trailer*/
        {
          a_slot2 = ((datain[ii]>>22)&0x1F);
          a_nwords = (datain[ii]&0x3FFFFF);
#ifdef DEBUG
	      printf("[%3d] BLOCK TRAILER: slot %d, nwords %d\n",ii,
				   a_slot2,a_nwords);
          printf(">>> data check\n");
#endif

          /*"close" previous event if any*/
          k = nB[jj]-1; /*current block index*/
          if(nE[jj][k] > 0)
	      {
            m = nE[jj][k]-1; /*current event number*/
            lenE[jj][k][m] = ii-iE[jj][k][m]; /*#words in current event*/
	      }

          if(a_slot2 != a_slot)
	      {
            error ++;
            if(printing)
            {
              printf("[%3d][%3d] ERROR1 in FADC data: blockheader slot %d != blocktrailer slot %d\n",mynev,
				 ii,a_slot,a_slot2);
              sprintf(errmsg,"[%3d][%3d] ERROR1 in FADC data: blockheader slot %d != blocktrailer slot %d\n",mynev,
				 ii,a_slot,a_slot2);
              printing=0;
	        }
	      }
          if(a_nwords != (ii-iB[jj][nB[jj]-1]+1))
          {
            error ++;
            if(printing)
            {
              printf("[%3d][%3d] ERROR2 in FADC data: trailer #words %d != actual #words %d\n",mynev,
				 ii,a_nwords,ii-iB[jj][nB[jj]-1]+1);
              sprintf(errmsg,"[%3d[%3d]] ERROR2 in FADC data: trailer #words %d != actual #words %d\n",mynev,
				 ii,a_nwords,ii-iB[jj][nB[jj]-1]+1);
              printing=0;
	        }
          }
	      ii++;
        }
        else if( ((datain[ii]>>27)&0x1F) == 0x12) /*event header*/
        {
          a_slot3 = ((datain[ii]>>22)&0x1F);
          a_triggernumber = (datain[ii]&0x3FFFFF);
#ifdef DEBUG
	      printf("[%3d] EVENT HEADER: slot number %d, trigger number %d\n",ii,
				 a_slot3, a_triggernumber);
          printf(">>> update iE and nE\n");
#endif

          /*"close" previous event if any*/
          k = nB[jj]-1; /*current block index*/
          if(nE[jj][k] > 0)
	      {
            m = nE[jj][k]-1; /*current event number*/
            lenE[jj][k][m] = ii-iE[jj][k][m]; /*#words in current event*/
	      }

          /*"open" next event*/
          nE[jj][k]++; /*increment event counter in current block*/
          m = nE[jj][k]-1; /*current event number*/
          iE[jj][k][m]=ii; /*remember event start index*/

	      ii++;

        }
        else if( ((datain[ii]>>27)&0x1F) == 0x13) /*trigger time: remember timestamp*/
        {
          a_trigtime[0] = (datain[ii]&0xFFFFFF);
#ifdef DEBUG
	      printf("[%3d] TRIGGER TIME: 0x%06x\n",ii,a_trigtime[0]);
#endif
	      ii++;
          iii=1;
          while( ((datain[ii]>>31)&0x1) == 0 && ii<lenin ) /*must be one more word*/
	      {
            a_trigtime[iii] = (datain[ii]&0xFFFFFF);
#ifdef DEBUG
            printf("   [%3d] TRIGGER TIME: 0x%06x\n",ii,a_trigtime[iii]);
            printf(">>> remember timestamp 0x%06x 0x%06x\n",a_trigtime[0],a_trigtime[1]);
#endif
            iii++;
            ii++;
	      }
        }
        else if( ((datain[ii]>>27)&0x1F) == 0x14) /*window raw data: remember channel#*/
        {
          a_channel = ((datain[ii]>>23)&0xF);
          a_windowwidth = (datain[ii]&0xFFF);	  
#ifdef DEBUG
	      printf("[%3d] WINDOW RAW DATA: slot %d, channel %d, window width %d\n",ii,
			 a_slot,a_channel,a_windowwidth);
          printf(">>> remember channel %d\n",a_channel);
#endif
	      ii++;
          samplecount = 0;
          while( ((datain[ii]>>31)&0x1) == 0 && ii<lenin ) /*loop over all samples*/
	      {
            a_valid1 = ((datain[ii]>>29)&0x1);
            a_adc1 = ((datain[ii]>>16)&0xFFF);
            a_valid2 = ((datain[ii]>>13)&0x1);
            a_adc2 = (datain[ii]&0xFFF);
#ifdef DEBUG
            printf("   [%3d] WINDOW RAW DATA: valid1 %d, adc1 0x%04x, valid2 %d, adc2 0x%04x\n",ii,
              a_valid1,a_adc1,a_valid2,a_adc2);
#endif
            samplecount += 2;

            ii++;
	      }

          if(a_windowwidth != samplecount)
          {
            error ++;
            printf("[%3d][%3d] ERROR1: a_windowwidth=%d != samplecount=%d (slot %d (%d %d ?), channel %d)\n",mynev,ii,
              a_windowwidth,samplecount,a_slot,a_slot_prev,a_slot2,a_channel);
            sprintf(errmsg,"[%3d][%3d] ERROR1: a_windowwidth=%d != samplecount=%d (slot %d (%d %d ?), channel %d)\n",mynev,ii,
              a_windowwidth,samplecount,a_slot,a_slot_prev,a_slot2,a_channel);
            fflush(stdout);
          }

        }
        else if( ((datain[ii]>>27)&0x1F) == 0x15) /*window sum: obsolete*/
        {
#ifdef DEBUG
	      printf("[%3d] WINDOW SUM: must be obsolete\n",ii);
#endif
	      ii++;
        }
        else if( ((datain[ii]>>27)&0x1F) == 0x16) /*pulse raw data*/
        {
          a_channel = ((datain[ii]>>23)&0xF);
          a_pulsenumber = ((datain[ii]>>21)&0x3);
          a_firstsample = (datain[ii]&0x3FF);
#ifdef DEBUG
	      printf("[%3d] PULSE RAW DATA: channel %d, pulse number %d, first sample %d\n",ii,
			   a_channel,a_pulsenumber,a_firstsample);
#endif

	      ii++;

          while( ((datain[ii]>>31)&0x1) == 0)
	      {
            a_valid1 = ((datain[ii]>>29)&0x1);
            a_adc1 = ((datain[ii]>>16)&0xFFF);
            a_valid2 = ((datain[ii]>>13)&0x1);
            a_adc2 = (datain[ii]&0xFFF);
#ifdef DEBUG
            printf("   [%3d] PULSE RAW DATA: valid1 %d, adc1 0x%04x, valid2 %d, adc2 0x%04x\n",ii,
              a_valid1,a_adc1,a_valid2,a_adc2);
#endif

            ii++;
	      }

        }



















        else if( ((datain[ii]>>27)&0x1F) == 0x17) /* pulse integral */
        {
          a_channel = ((datain[ii]>>23)&0xF);
          a_pulsenumber = ((datain[ii]>>21)&0x3);
          a_qualityfactor = ((datain[ii]>>19)&0x3);
          a_pulseintegral = (datain[ii]&0x7FFFF);

#ifdef MODE7
          m = nE[jj][k]-1; /*current event number*/
          pulse_integral[m][a_slot][a_channel][a_pulsenumber] = a_pulseintegral;
          if(npulse_integral[m][a_slot][a_channel] != a_pulsenumber)
		  {
            printf("ERROR: slot=%d channel=%d npulse_integral=%d != a_pulsenumber=%d\n",a_slot,
				   a_channel,npulse_integral[m][a_slot][a_channel],a_pulsenumber);
		  }
          npulse_integral[m][a_slot][a_channel] ++;
#endif


#ifdef DEBUG
	      printf("[%3d] PULSE INTEGRAL: channel %d, pulse number %d, quality %d, integral %d\n",ii,
				 a_channel,a_pulsenumber,a_qualityfactor,a_pulseintegral);
#endif

	      ii++;
        }




        else if( ((datain[ii]>>27)&0x1F) == 0x18) /* pulse time */
        {
          a_channel = ((datain[ii]>>23)&0xF);
          a_pulsenumber = ((datain[ii]>>21)&0x3);
          a_qualityfactor = ((datain[ii]>>19)&0x3);
          a_pulsetime = (datain[ii]&0xFFFF);

#ifdef MODE7
          m = nE[jj][k]-1; /*current event number*/
          pulse_time[m][a_slot][a_channel][a_pulsenumber] = a_pulsetime;
          if(npulse_time[m][a_slot][a_channel] != a_pulsenumber)
		  {
            printf("ERROR: slot=%d channel=%d npulse_time=%d != a_pulsenumber=%d\n",a_slot,
				   a_channel,npulse_time[m][a_slot][a_channel],a_pulsenumber);
		  }
          npulse_time[m][a_slot][a_channel] ++;
#endif

#ifdef DEBUG
	      printf("[%3d] PULSE TIME: channel %d, pulse number %d, quality %d, time %d\n",ii,
				 a_channel,a_pulsenumber,a_qualityfactor,a_pulsetime);
#endif

	      ii++;
        }
















        else if( ((datain[ii]>>27)&0x1F) == 0x19)
        {
#ifdef DEBUG
	      printf("[%3d] STREAMING RAW DATA: \n",ii);
#endif
	      ii++;
        }









        else if( ((datain[ii]>>27)&0x1F) == 0x1a) /* Vm Vp */
        {
          a_channel = ((datain[ii]>>23)&0xF);
          a_pulsenumber = ((datain[ii]>>21)&0x3);
          a_vm = ((datain[ii]>>12)&0x1FF);
          a_vp = (datain[ii]&0xFFF);

#ifdef MODE7
          m = nE[jj][k]-1; /*current event number*/
          pulse_Vm[m][a_slot][a_channel][a_pulsenumber] = a_vm;
          pulse_Vp[m][a_slot][a_channel][a_pulsenumber] = a_vp;
          if(npulse_VmVp[m][a_slot][a_channel] != a_pulsenumber)
		  {
            printf("ERROR: slot=%d channel=%d npulse_VmVp=%d != a_pulsenumber=%d\n",a_slot,
				   a_channel,npulse_VmVp[m][a_slot][a_channel],a_pulsenumber);
		  }
          npulse_VmVp[m][a_slot][a_channel] ++;
#endif

#ifdef DEBUG
	      printf("[%3d] PULSE VmVp: channel %d, pulse number %d, Vm %d, Vp %d\n",ii,
				 a_channel,a_pulsenumber,a_vm,a_vp);
#endif
	      ii++;
        }






        else if( ((datain[ii]>>27)&0x1F) == 0x1D)
        {
#ifdef DEBUG
	      printf("[%3d] EVENT TRAILER: \n",ii);
#endif
	      ii++;
        }
        else if( ((datain[ii]>>27)&0x1F) == 0x1E)
        {
	      printf("[%3d] : DATA NOT VALID\n",ii);
          exit(0);
	      ii++;
        }
        else if( ((datain[ii]>>27)&0x1F) == 0x1F)
        {
#ifdef DEBUG
	      printf("[%3d] FILLER WORD: \n",ii);
          printf(">>> do nothing\n");
#endif
	      ii++;
        }
        else
        {
          error ++;
          if(printing) /* printing only once at every event */
          {
            printf("[%3d][%3d] pass1 ERROR: in FADC data format 0x%08x (bits31-27=0x%02x), slot=%d (%d %d ?)\n",mynev,
			   ii,(int)datain[ii],(datain[ii]>>27)&0x1F,a_slot,a_slot_prev,a_slot2);
            sprintf(errmsg,"[%3d][%3d] pass1 ERROR: in FADC data format 0x%08x (bits31-27=0x%02x), slot=%d (%d %d ?)\n",mynev,
			   ii,(int)datain[ii],(datain[ii]>>27)&0x1F,a_slot,a_slot_prev,a_slot2);
            printing=0;
	      }
          ii++;
        }

      } /* loop over 'lenin' words */










      /**********************************************************/
      /*check if the number of events in every block is the same*/

      nnE = nE[jj][0];
      for(k=1; k<nB[jj]; k++)
      {
        if(nE[jj][k]!=nnE)
	    {
          error ++;
          if(printing)
          {
            printf("[%3d] SEVERE ERROR: different event number in different blocks (nnE=%d != nE[%d]=%d)\n",mynev,
              nnE,k,nE[jj][k]);
            sprintf(errmsg,"[%3d] SEVERE ERROR: different event number in different blocks (nnE=%d != nE[%d]=%d)\n",mynev,
              nnE,k,nE[jj][k]);
            printing=0;
	      }      
	    }
      }


#ifdef DEBUG
      printf("\n=================\n");
      for(k=0; k<nB[jj]; k++)
      {
        printf("Block %2d, block index %2d\n",k,iB[jj][k]);
        for(m=0; m<nnE; m++)
	    {
          printf("Event %2d, event index %2d, event lenght %2d\n",m,iE[jj][k][m],lenE[jj][k][m]);
          sprintf(errmsg,"Event %2d, event index %2d, event lenght %2d\n",m,iE[jj][k][m],lenE[jj][k][m]);
        }
      }
      printf("\n=================\n");
#endif



      /***********************************************************/
      /* if any error was found, create raw data bank and return */
      if(error)
      {
        printf("\n\n\n!!!!!!!!!!!!! ERROR FOUND - RECORD RAW BANK\n\n\n");
	    {
          FILE *fd;
          char fname[256];
          sprintf(fname,"/tmp/hps1_error_event_%d.txt",mynev);
          fd = fopen(fname,"w");
          if(fd != NULL)
	      {
            fprintf(fd,"%s\n",errmsg);
            for(ii=0; ii<lenin; ii++) fprintf(fd,"[%5d] 0x%08x\n",ii,datain[ii]);
            fclose(fd);
	      }
	    }

        CPOPEN(rol->pid,1,0);
        for(ii=0; ii<lenin; ii++)
        {
          dataout[ii] = datain[ii];
          b08 += 4;
        }
        CPCLOSE;
        CPEXIT;
        return;
      }
      /***********************************************************/
      /***********************************************************/
	
	
	} /* if(0xe109) */



    else if(banktag[jj] == 0xe10B) /*  0xe10B v1190/v1290 hardware format */
	{
      banknum = rol->pid;

#ifdef DEBUG2
      printf("\nFIRST PASS TDC\n\n");
#endif

      error = 0;
      ii=0;
      printing=1;
      a_slot_prev = -1;
      have_time_stamp = 1;
      nB[jj]=0; /*cleanup block counter*/
      while(ii<lenin)
      {
#ifdef DEBUG2
        printf("[%5d] 0x%08x (lenin=%d)\n",ii,datain[ii],lenin);
#endif



		/*
        a_slot    = (datain[ii]>>27)&0x1F;
        a_edge    = (datain[ii]>>26)&0x1;
        a_channel = (datain[ii]>>19)&0x7F;
        a_tdc     = datain[ii]&0x7FFFF;

#ifdef DEBUG2
	    printf("[%5d] TDC DATA: 0x%08x (slot=%d channel=%d tdc=%d edge=%d)\n",ii,datain[ii],a_slot,a_channel,a_tdc,a_edge);
#endif
		*/

		
        if( ((datain[ii]>>27)&0x1F) == 0)
        {
          nbcount ++;

          tdcedge = ((datain[ii]>>26)&0x1);
		  /*printf("tdcslot=%d tdctypebyslot=%d\n",tdcslot,tdctypebyslot[tdcslot]);*/
          if( tdctypebyslot[tdcslot] == 2 ) /*1290*/
          {
            tdcchan = ((datain[ii]>>21)&0x1F);
            tdcval = (datain[ii]&0x1FFFFF);
          }
          else /*1190*/
          {
            tdcchan = ((datain[ii]>>19)&0x7F);
            tdcval = (datain[ii]&0x7FFFF);
			/*if(tdcchan==63) {reftdc=1; printf("[ev %d] befor: 0x%08x\n",*(rol->nevents),tdcval);}*/
            tdcval = tdcval<<2; /*report 1190 value with same resolution as 1290*/
			/*if(tdcchan==63) printf("[ev %d] after: 0x%08x (0x%08x)\n",*(rol->nevents),tdcval,tdcval&0x7FFFF);*/
		  }

#ifdef DEBUG2
		  if(1/*tdcchan<2*/)
          {
            printf("   DATA: type=%1d slot=%02d tdc=%02d ch=%03d edge=%01d value=%08d\n",
				   tdctypebyslot[tdcslot],tdcslot,(int)tdc14,(int)tdcchan,(int)tdcedge,(int)tdcval);
		  }
#endif
        }
        else if( ((datain[ii]>>27)&0x1F) == 1)
        {
          tdc14 = ((datain[ii]>>24)&0x3);
          tdceventid = ((datain[ii]>12)&0xFFF);
          tdcbunchid = (datain[ii]&0xFFF);
#ifdef DEBUG2
          printf(" HEAD: tdc=%02d event_id=%05d bunch_id=%05d\n",
            (int)tdc14,(int)tdceventid,(int)tdcbunchid);
#endif
        }
        else if( ((datain[ii]>>27)&0x1F) == 3)
        {
          tdc14 = ((datain[ii]>>24)&0x3);
          tdceventid = ((datain[ii]>12)&0xFFF);
          tdcwordcount = (datain[ii]&0xFFF);
#ifdef DEBUG2
          printf(" EOB:  tdc=%02d event_id=%05d word_count=%05d\n",
            (int)tdc14,(int)tdceventid,(int)tdcwordcount);
#endif
        }
        else if( ((datain[ii]>>27)&0x1F) == 0x18)
        {
#ifdef DEBUG2
          printf(" FILLER\n");
#endif
        }
        else if( ((datain[ii]>>27)&0x1F) == 0x11)
        {
          nbsubtract ++;
#ifdef DEBUG2
          printf(" TIME TAG: %08x\n",(int)(datain[ii]&0x7ffffff));
#endif
        }
        else if( ((datain[ii]>>27)&0x1F) == 8)
        {
          nbcount = 1; /* counter for the number of output words from board */
          nbsubtract = NBsubtract; /* # words to subtract including errors (5 for v1290N, 9 for others) */
          tdcslot = datain[ii]&0x1F;
          tdceventcount = (datain[ii]>>5)&0x3FFFFF;
#ifdef SLOTWORKAROUND
          tdcslot_h = tdcslot;
	      remember_h = datain[ii];

          /* correct slot number - cannot do it that was for multi-event readout
          if(slotnums[nheaders] != tdcslot)
		  {
            if( !((*(rol->nevents))%10000) ) printf("WARN: [%2d] slotnums=%d, tdcslot=%d -> use slotnums\n",
				   nheaders,slotnums[nheaders],tdcslot);
            tdcslot = slotnums[nheaders];
		  }
		  */
#endif
          nheaders++;

		  
#ifdef DEBUG2
		  
          printf("GLOBAL HEADER: 0x%08x (slot=%d nevent=%d)\n",
            datain[ii],datain[ii]&0x1F,(datain[ii]>>5)&0xFFFFFF);
		  
#endif





          a_slot = tdcslot;

          /* there is no block header for TDCs, only global header for every event; we'll
             assume that if slot shows up for the first time, we are in the beginning of block */
          if(a_slot_prev != a_slot)
		  {
#ifdef DEBUG2
            printf(">>> a_slot_prev=%d, a_slot=%d -> update iB and nB\n",a_slot_prev,a_slot);
#endif
            a_slot_prev = a_slot;

            nB[jj]++;                  /*increment block counter*/
            iB[jj][nB[jj]-1] = ii;     /*remember block start index*/
            sB[jj][nB[jj]-1] = a_slot; /*remember slot number*/
            nE[jj][nB[jj]-1] = 0;      /*cleanup event counter in current block*/

#ifdef DEBUG2
		    printf("0xe10B: jj=%d nB[jj]=%d\n",jj,nB[jj]);
#endif
		  }



          k = nB[jj]-1; /*current block index*/

          /*"open" next event*/
          nE[jj][k]++; /*increment event counter in current block*/
          m = nE[jj][k]-1; /*current event number*/
          iE[jj][k][m]=ii; /*remember event start index*/
#ifdef DEBUG2
		  printf("0xe10B: open event jj=%d event# %d index %d\n",jj,m,iE[jj][k][m]);
#endif



		  
        }
        else if( ((datain[ii]>>27)&0x1F) == 0x10)
        {
#ifdef SLOTWORKAROUND
          /* double check for slot number */
          tdcslot_t = datain[ii]&0x1F;

		  /*
          if(slotnums[ntrailers] != tdcslot_t)
		  {
            if( !((*(rol->nevents))%10000) ) printf("WARN: [%2d] slotnums=%d, tdcslot=%d\n",
				   ntrailers,slotnums[ntrailers],tdcslot_t);
		  }
		  */



          if(tdcslot_h>21||tdcslot_t>21||tdcslot_h!=tdcslot_t)
          {
            /*printf("WARN: slot from header=%d (0x%08x), from trailer=%d (0x%08x), must be %d\n",
			  tdcslot_h,remember_h,tdcslot_t,datain[ii],tdcslot)*/;
          }
#endif
          /* put nwords in header; substract 4 TDC headers, 4 TDC EOBs,
          global trailer and error words if any */
          if((((datain[ii]>>5)&0x3FFF) - nbsubtract) != nbcount)
          {			
			
            printf("ERROR: word counters: %d != %d (subtract=%d)\n",
			  (((datain[ii]>>5)&0x3FFF) - nbsubtract),nbcount,nbsubtract);
			
			/*
for(i=0; i<200; i++) tmpbad[i] = tdcbuf[i];
printf("tmpgood=0x%08x tmpbad=0x%08x\n",tmpgood,tmpbad);
			*/
          }
          ntrailers ++;

          nwtdc = (datain[ii]>>5)&0x3FFF;
#ifdef DEBUG2
          printf("GLOBAL TRAILER: 0x%08x (slot=%d nw=%d stat=%d)\n",
            datain[ii],datain[ii]&0x1F,(datain[ii]>>5)&0x3FFF,
            (datain[ii]>>23)&0xF);
#endif








          /*"close" previous event if any*/
          k = nB[jj]-1; /*current block index*/
          if(nE[jj][k] > 0)
	      {
            m = nE[jj][k]-1; /*current event number*/
            lenE[jj][k][m] = ii-iE[jj][k][m]; /*#words in current event*/
	      }

#ifdef DEBUG2
		  printf("0xe10B: close event jj=%d event# %d length %d\n",jj,m,lenE[jj][k][m]);
#endif








        }
        else if( ((datain[ii]>>27)&0x1F) == 4)
        {
          nbsubtract ++;
          tdc14 = ((datain[ii]>>24)&0x3);
          tdcerrorflags = (datain[ii]&0x7FFF);
		  /*
#ifdef DEBUG2
		  */
		  if( !(error_flag[tdcslot]%100) ) /* do not print for every event */
          {
            unsigned int ddd, lock, ntdc;
			
            printf(" ERR: event# %7d, slot# %2d, tdc# %02d, error_flags=0x%08x, err=0x%04x, lock=0x%04x\n",
              tdceventcount,tdcslot,(int)tdc14,(int)tdcerrorflags,ddd,lock);
			
		  }
          error_flag[tdcslot] ++;
		  /*
#endif
		  */
        }
        else
        {
#ifdef DEBUG2
          printf("ERROR: in TDC data format 0x%08x\n",(int)datain[ii]);
#endif
        }

        ii ++;

	  } /* while() */

	} /* else if(0xe10B) */



    else if(banktag[jj] == 0xe10C) /* SSP hardware format */
	{
      banknum = rol->pid;

#ifdef DEBUG3
      printf("\nFIRST PASS SSP\n\n");
#endif

      error = 0;
      ndnv = 0;
      ii=0;
      printing=1;
      a_slot_prev = -1;
      have_time_stamp = 1;
      nB[jj]=0; /*cleanup block counter*/
      while(ii<lenin)
      {
#ifdef DEBUG3
        printf("[%5d] 0x%08x (lenin=%d)\n",ii,datain[ii],lenin);
#endif
        if( ((datain[ii]>>27)&0x1F) == 0x10) /*block header*/
        {
          a_slot_prev = a_slot;
          a_slot = ((datain[ii]>>22)&0x1F);
          a_module_id = ((datain[ii]>>18)&0xF);
          a_blocknumber = ((datain[ii]>>8)&0x3FF);
          a_nevents = (datain[ii]&0xFF);
#ifdef DEBUG3
	      printf("[%3d] BLOCK HEADER: slot %d, nevents %d, block number %d module id %d\n",ii,
				 a_slot,a_nevents,a_blocknumber,a_module_id);
          printf(">>> update iB and nB\n");
#endif
          nB[jj]++;                  /*increment block counter*/
          iB[jj][nB[jj]-1] = ii;     /*remember block start index*/
          sB[jj][nB[jj]-1] = a_slot; /*remember slot number*/
          nE[jj][nB[jj]-1] = 0;      /*cleanup event counter in current block*/

#ifdef DEBUG3
		  printf("0xe10c: jj=%d nB[jj]=%d\n",jj,nB[jj]);
#endif

	      ii++;
        }
        else if( ((datain[ii]>>27)&0x1F) == 0x11) /*block trailer*/
        {
          a_slot2 = ((datain[ii]>>22)&0x1F);
          a_nwords = (datain[ii]&0x3FFFFF);
#ifdef DEBUG3
	      printf("[%3d] BLOCK TRAILER: slot %d, nwords %d\n",ii,
				   a_slot2,a_nwords);
          printf(">>> data check\n");
#endif

          /*"close" previous event if any*/
          k = nB[jj]-1; /*current block index*/
          if(nE[jj][k] > 0)
	      {
            m = nE[jj][k]-1; /*current event number*/
            lenE[jj][k][m] = ii-iE[jj][k][m]; /*#words in current event*/
	      }

          if(a_slot2 != a_slot)
	      {
            error ++;
            if(printing)
            {
              printf("[%3d][%3d] ERROR1 in SSP data: blockheader slot %d != blocktrailer slot %d\n",mynev,
				 ii,a_slot,a_slot2);
              printing=0;
	        }
	      }
          if(a_nwords != ( (ii-iB[jj][nB[jj]-1]+1) - ndnv ) )
          {
            error ++;
            if(printing)
            {
              printf("[%3d][%3d] ERROR2 in SSP data: trailer #words=%d != actual #words=%d - ndnv=%d)\n",
					 mynev,ii,a_nwords,ii-iB[jj][nB[jj]-1]+1,ndnv);
              printing=0;
	        }
          }
          if(ndnv>0)
          {
            if(printing)
            {
              printf("[%3d] WARN in SSP data: ndnv=%d)\n",mynev,ndnv);
              printing=0;
	        }
          }

	      ii++;
        }

        else if( ((datain[ii]>>27)&0x1F) == 0x12) /*event header*/
        {
          a_triggernumber = (datain[ii]&0x7FFFFFF);
#ifdef DEBUG3
	      printf("[%3d] EVENT HEADER: trigger number %d\n",ii,
				 a_triggernumber);
          printf(">>> update iE and nE\n");
#endif

          /*"close" previous event if any*/
          k = nB[jj]-1; /*current block index*/
          if(nE[jj][k] > 0)
	      {
            m = nE[jj][k]-1; /*current event number*/
            lenE[jj][k][m] = ii-iE[jj][k][m]; /*#words in current event*/
	      }

          /*"open" next event*/
          nE[jj][k]++; /*increment event counter in current block*/
          m = nE[jj][k]-1; /*current event number*/
          iE[jj][k][m]=ii; /*remember event start index*/

	      ii++;
        }
        else if( ((datain[ii]>>27)&0x1F) == 0x13) /*trigger time: remember timestamp*/
        {
          a_trigtime[0] = (datain[ii]&0xFFFFFF);
#ifdef DEBUG3
	      printf("[%3d] TRIGGER TIME: 0x%06x\n",ii,a_trigtime[0]);
#endif
	      ii++;
          iii=1;
          while( ((datain[ii]>>31)&0x1) == 0 && ii<lenin ) /*must be one more word*/
	      {
            a_trigtime[iii] = (datain[ii]&0xFFFFFF);
#ifdef DEBUG3
            printf("   [%3d] TRIGGER TIME: 0x%06x\n",ii,a_trigtime[iii]);
            printf(">>> remember timestamp 0x%06x 0x%06x\n",a_trigtime[0],a_trigtime[1]);
#endif
            iii++;
            if(iii>2) printf("ERROR1 in SSP: iii=%d\n",iii); 
            ii++;
	      }
        }

        else if( ((datain[ii]>>27)&0x1F) == 0x14) /*hps cluster*/
        {
          a_clusterN = ((datain[ii]>>23)&0xF);
          a_clusterE = ((datain[ii]>>10)&0x1FFF);
		  a_clusterY = ((datain[ii]>>6)&0xF);
          a_clusterX = (datain[ii]&0x3F);
#ifdef DEBUG3
	      printf("[%3d] CLUSTER_(N,E,Y,X): %d %d %d %d\n",ii,a_clusterN,a_clusterE,a_clusterY,a_clusterX);
#endif
	      ii++;
          iii=0;
          while( ((datain[ii]>>31)&0x1) == 0 && ii<lenin ) /*must be one more word*/
	      {
            a_clusterT = (datain[ii]&0x3FF);
#ifdef DEBUG3
            printf("   [%3d] CLUSTER_T: %d\n",ii,a_clusterT);
            printf(">>> remember timestamp 0x%06x 0x%06x\n",a_trigtime[0],a_trigtime[1]);
#endif
            iii++;
            if(iii>1) printf("ERROR2 in SSP: iii=%d\n",iii); 

            ii++;
	      }
        }

        else if( ((datain[ii]>>27)&0x1F) == 0x15) /*hps trigger*/
        {
          a_type = ((datain[ii]>>23)&0xF);
          a_data = ((datain[ii]>>16)&0x7F);
		  a_time = (datain[ii]&0x3FF);
#ifdef DEBUG3
	      printf("[%3d] TYPE %d, DATA %d, TIME %d\n",ii,a_type,a_data,a_time);
#endif
	      ii++;
		}

        else if( ((datain[ii]>>27)&0x1F) == 0x1F)
        {
#ifdef DEBUG3
	      printf("[%3d] FILLER WORD\n",ii);
          printf(">>> do nothing\n");
#endif
	      ii++;
        }
        else if( ((datain[ii]>>27)&0x1F) == 0x1E)
        {
          ndnv ++;
#ifdef DEBUG3
	      printf("[%3d] DNV\n",ii);
          printf(">>> do nothing\n");
#endif
	      ii++;
        }
        else
		{
          printf("SSP UNKNOWN data: [%3d] 0x%08x\n",ii,datain[ii]);
          ii++;
		}

	  } /* while() */

	} /* else if(0xe10C) */









    else if(banktag[jj] == 0xe112) /* HEAD bank raw format */
	{
      banknum = rol->pid;

#ifdef DEBUG5
      printf("\nFIRST PASS HEAD bank\n\n");
#endif

      error = 0;
      ii=0;
      printing=1;
      nB[jj]=0; /*cleanup block counter*/
      while(ii<lenin)
      {
#ifdef DEBUG5
        printf("[%5d] 0x%08x (lenin=%d)\n",ii,datain[ii],lenin);
#endif
        if( ((datain[ii]>>27)&0x1F) == 0x10) /*block header*/
        {
          a_nevents = (datain[ii]&0xFF);
#ifdef DEBUG5
	      printf("[%3d] BLOCK HEADER: nevents %d\n",ii,a_nevents);
          printf(">>> update iB and nB\n");
#endif
          nB[jj]++;                  /*increment block counter*/
          iB[jj][nB[jj]-1] = ii;     /*remember block start index*/
          sB[jj][nB[jj]-1] = 0;      /*slot number not used here*/
          nE[jj][nB[jj]-1] = 0;      /*cleanup event counter in current block*/

#ifdef DEBUG5
		  printf("0xe10c: jj=%d nB[jj]=%d\n",jj,nB[jj]);
#endif

	      ii++;
        }
        else if( ((datain[ii]>>27)&0x1F) == 0x11) /*block trailer*/
        {
          a_nwords = (datain[ii]&0x3FFFFF);
#ifdef DEBUG5
	      printf("[%3d] BLOCK TRAILER: nwords %d\n",ii,a_nwords);
          printf(">>> data check\n");
#endif

          /*"close" previous event if any*/
          k = nB[jj]-1; /*current block index*/
          if(nE[jj][k] > 0)
	      {
            m = nE[jj][k]-1; /*current event number*/
            lenE[jj][k][m] = ii-iE[jj][k][m]; /*#words in current event*/
	      }

          if(a_nwords != ( (ii-iB[jj][nB[jj]-1]+1)) )
          {
            error ++;
            if(printing)
            {
              printf("[%3d][%3d] ERROR2 in HEAD raw data: trailer #words=%d != actual #words=%d)\n",
					 mynev,ii,a_nwords,ii-iB[jj][nB[jj]-1]+1);
              printing=0;
	        }
          }

	      ii++;
        }

        else if( ((datain[ii]>>27)&0x1F) == 0x12) /*event header*/
        {
          a_triggernumber = (datain[ii]&0x7FFFFFF);
#ifdef DEBUG5
	      printf("[%3d] EVENT HEADER: trigger number %d\n",ii,
				 a_triggernumber);
          printf(">>> update iE and nE\n");
#endif

          /*"close" previous event if any*/
          k = nB[jj]-1; /*current block index*/
          if(nE[jj][k] > 0)
	      {
            m = nE[jj][k]-1; /*current event number*/
            lenE[jj][k][m] = ii-iE[jj][k][m]; /*#words in current event*/
	      }

          /*"open" next event*/
          nE[jj][k]++; /*increment event counter in current block*/
          m = nE[jj][k]-1; /*current event number*/
          iE[jj][k][m]=ii; /*remember event start index*/

	      ii++;
        }

        else if( ((datain[ii]>>27)&0x1F) == 0x14) /*head data*/
        {
          nw = (datain[ii]&0xFF);
#ifdef DEBUG5
	      printf("[%3d] head bank has %d data words\n",ii,nw);
#endif
	      ii++;
          iii=0;
          while( iii<nw && ii<lenin ) /*get all data words*/
	      {
#ifdef DEBUG5
            printf("   [%3d][%3d]: %d\n",ii,iii,datain[ii]);
#endif
            iii++;
            ii++;
	      }
        }

        else
		{
          printf("PASS1 HEAD UNKNOWN data: [%3d] 0x%08x\n",ii,datain[ii]);
          ii++;
		}

	  } /* while() */

	} /* else if(0xe112) */












    else if(banktag[jj] == 0xe104) /* VSCM hardware format */
	{
      banknum = rol->pid;

#ifdef DEBUG4
      printf("\nFIRST PASS VSCM\n\n");
#endif

      error = 0;
      ndnv = 0;
      ii=0;
      printing=1;
      a_slot_prev = -1;
      have_time_stamp = 1;
      nB[jj]=0; /*cleanup block counter*/
      while(ii<lenin)
      {
#ifdef DEBUG4
        printf("[%5d] VSCM 0x%08x (lenin=%d)\n",ii,datain[ii],lenin);
#endif
        if( ((datain[ii]>>27)&0x1F) == DATA_TYPE_BLKHDR) /*block header*/
        {
          a_slot_prev = a_slot;
          a_slot = ((datain[ii]>>22)&0x1F);
          /*a_module_id = ((datain[ii]>>18)&0xF);*/
          a_blocknumber = (datain[ii]&0x7FF);
          a_nevents = ((datain[ii]>>11)&0x7FF);
#ifdef DEBUG4
	      printf("[%3d] VSCM BLOCK HEADER: slot %d, nevents %d, block number %d module id %d\n",ii,
				 a_slot,a_nevents,a_blocknumber,a_module_id);
          printf(">>> VSCM update iB and nB\n");
#endif
          nB[jj]++;                  /*increment block counter*/
          iB[jj][nB[jj]-1] = ii;     /*remember block start index*/
          sB[jj][nB[jj]-1] = a_slot; /*remember slot number*/
          nE[jj][nB[jj]-1] = 0;      /*cleanup event counter in current block*/

#ifdef DEBUG4
		  printf("VSCM: jj=%d nB[jj]=%d\n",jj,nB[jj]);
#endif

	      ii++;
        }
        else if( ((datain[ii]>>27)&0x1F) == DATA_TYPE_BLKTLR) /*block trailer*/
        {
          a_slot2 = ((datain[ii]>>22)&0x1F);
          a_nwords = (datain[ii]&0x3FFFFF);
#ifdef DEBUG4
	      printf("[%3d] VSCM BLOCK TRAILER: slot %d, nwords %d\n",ii,
				   a_slot2,a_nwords);
          printf(">>> VSCM data check\n");
#endif

          /*"close" previous event if any*/
          k = nB[jj]-1; /*current block index*/
          if(nE[jj][k] > 0)
	      {
            m = nE[jj][k]-1; /*current event number*/
            lenE[jj][k][m] = ii-iE[jj][k][m]; /*#words in current event*/
#ifdef DEBUG4
            printf(">>> VSCM lenE(block trailer)=%d\n",lenE[jj][k][m]);
#endif
	      }

          if(a_slot2 != a_slot)
	      {
            error ++;
            if(printing)
            {
              printf("[%3d][%3d] ERROR1 in VSCM data: blockheader slot %d != blocktrailer slot %d\n",mynev,
				 ii,a_slot,a_slot2);
              printing=0;
	        }
	      }
          if(a_nwords != ( (ii-iB[jj][nB[jj]-1]+1) - ndnv ) )
          {
            error ++;
            if(printing)
            {
              printf("[%3d][%3d] ERROR2 in VSCM data: trailer #words=%d != actual #words=%d - ndnv=%d)\n",
					 mynev,ii,a_nwords,ii-iB[jj][nB[jj]-1]+1,ndnv);
              printing=0;
	        }
          }
          if(ndnv>0)
          {
            if(printing)
            {
              printf("[%3d] WARN in VSCM data: ndnv=%d)\n",mynev,ndnv);
              printing=0;
	        }
          }

	      ii++;
        }

        else if( ((datain[ii]>>27)&0x1F) == DATA_TYPE_EVTHDR) /*event header*/
        {
          a_triggernumber = (datain[ii]&0x7FFFFFF);

#ifdef DEBUG4
	      printf("[%3d] VSCM EVENT HEADER: trigger number %d\n",ii,
				 a_triggernumber);
          printf(">>> VSCM update iE and nE\n");
#endif

          /*"close" previous event if any*/
          k = nB[jj]-1; /*current block index*/
          if(nE[jj][k] > 0)
	      {
            m = nE[jj][k]-1; /*current event number*/
            lenE[jj][k][m] = ii-iE[jj][k][m]; /*#words in current event*/
#ifdef DEBUG4
            printf(">>> VSCM lenE(event header)=%d\n",lenE[jj][k][m]);
#endif
	      }

          /*"open" next event*/
          nE[jj][k]++; /*increment event counter in current block*/
          m = nE[jj][k]-1; /*current event number*/
          iE[jj][k][m]=ii; /*remember event start index*/


	      ii++;
        }
        else if( ((datain[ii]>>27)&0x1F) == DATA_TYPE_TRGTIME) /*trigger time: remember timestamp*/
        {
          a_trigtime[0] = (datain[ii]&0xFFFFFF);
#ifdef DEBUG4
	      printf("[%3d] VSCM TRIGGER TIME: 0x%06x\n",ii,a_trigtime[0]);
#endif
	      ii++;
          iii=1;
          while( ((datain[ii]>>31)&0x1) == 0 && ii<lenin ) /*must be one more word*/
	      {
            a_trigtime[iii] = (datain[ii]&0xFFFFFF);
#ifdef DEBUG4
            printf("   [%3d] VSCM TRIGGER TIME: 0x%06x\n",ii,a_trigtime[iii]);
            printf(">>> VSCM remember timestamp 0x%06x 0x%06x\n",a_trigtime[0],a_trigtime[1]);
#endif
            iii++;
            if(iii>2) printf("ERROR1 in VSCM: iii=%d\n",iii); 
            ii++;
	      }
        }

        else if( ((datain[ii]>>27)&0x1F) == DATA_TYPE_BCOTIME) /* bco start and stop */
		{
#ifdef DEBUG4
          printf("[%3d] VSCM  BCOTIME START: %u STOP: %u\n",ii, (datain[ii]>>0) & 0xFF, (datain[ii]>>16) & 0xFF);
#endif
          ii++;
		}

        else if( ((datain[ii]>>27)&0x1F) == DATA_TYPE_FSSREVT) /*FSSR event*/
        {
          a_hfcb_id = ((datain[ii]>>22)&0x1);
          a_chip_id = ((datain[ii]>>19)&0x7);
          a_chan = ((datain[ii]>>12)&0x7F);
          a_bco = ((datain[ii]>>4)&0xFF);
          a_adc1 = (datain[ii]&0x7);

#ifdef DEBUG4
	      printf("[%3d] VSCM FSSREVT: HFCBID=%1u CHIPID=%1u CHAN=%3u BCO=%3u ADC=%1u \n",ii,a_hfcb_id,a_chip_id,a_chan,a_bco,a_adc1);
#endif
	      ii++;
        }

        else if( ((datain[ii]>>27)&0x1F) == DATA_TYPE_FILLER)
        {
#ifdef DEBUG4
	      printf("[%3d] VSCM FILLER WORD\n",ii);
          printf(">>> VSCM do nothing\n");
#endif
	      ii++;
        }
        else if( ((datain[ii]>>27)&0x1F) == DATA_TYPE_DNV)
        {
          ndnv ++;
#ifdef DEBUG4
	      printf("[%3d] VSCM DNV\n",ii);
          printf(">>> VSCM do nothing\n");
#endif
	      ii++;
        }
        else
		{
          printf("VSCM UNKNOWN data: [%3d] 0x%08x\n",ii,datain[ii]);
          ii++;
		}

	  } /* while() */

	} /* else if(0xe104) */









#ifdef PASS_AS_IS
    else /*any other bank will be passed 'as is' */
	{
      iASIS[nASIS++] = jj; /* remember bank number as it reported by BANKSCAN */
      /*printf("mynev=%d: remember bank number %d\n",mynev,jj);*/
	}

#endif




  } /* first for() over banks from rol1 */







  /********************************************************/
  /********************************************************/
  /********************************************************/
  /* SECOND PASS: disantangling and filling output buffer */


#ifdef DEBUG
  printf("\n\n\nSECOND PASS\n\n");
#endif

  lenout = 2; /* already done in CPINIT !!?? */
  b08 = NULL;
  printing=1;

  nnE = nE[0][0];
#ifdef DEBUG
  printf("nnE=%d\n",nnE);
#endif


  /*loop over events*/
  for(iev=0; iev<nnE; iev++)
  {
    lenev = 2;

    banknum = iev; /* using event number inside block as bank number - for now */

    for(jj=0; jj<nbanks; jj++) /* loop over evio banks */
    {

      datain = bankdata[jj];

#ifdef DEBUG
      printf("iev=%d jj=%d nB=%d\n",iev,jj,nB[jj]);
#endif








      if(banktag[jj] == 0xe109) /* 0Xe109 FADC250 hardware format */
	  {
#ifdef DEBUG
        printf("SECOND PASS FADC250\n");
#endif

        a_slot_old = -1;

        for(ibl=0; ibl<nB[jj]; ibl++) /*loop over blocks*/
        {
          a_channel_old = -1;

#ifdef DEBUG
          printf("0xe109: Block %d, Event %2d, event index %2d, event lenght %2d\n",
            ibl,iev,iE[jj][ibl][iev],lenE[jj][ibl][iev]);
#endif
          a_slot = sB[jj][ibl];
          ii = iE[jj][ibl][iev];
          rlen = ii + lenE[jj][ibl][iev];
          while(ii<rlen)
          {
            if( ((datain[ii]>>27)&0x1F) == 0x12) /*event header: remember trigger#*/
            {
              a_slot3 = ((datain[ii]>>22)&0x1F);
              a_triggernumber = (datain[ii]&0x3FFFFF);
#ifdef DEBUG
	          printf("[%3d] EVENT HEADER: slot number %d, trigger number %d\n",ii,
				 a_slot3, a_triggernumber);
              printf(">>> remember trigger %d\n",a_triggernumber);
#endif
	          ii++;
            }
            else if( ((datain[ii]>>27)&0x1F) == 0x13) /*trigger time: remember timestamp*/
            {
              a_trigtime[0] = (datain[ii]&0xFFFFFF);
#ifdef DEBUG
	          printf("[%3d] TRIGGER TIME: 0x%06x\n",ii,a_trigtime[0]);
#endif
	          ii++;
              iii=1;
              while( ((datain[ii]>>31)&0x1) == 0 && ii<rlen ) /*must be one more word*/
	          {
                a_trigtime[iii] = (datain[ii]&0xFFFFFF);
#ifdef DEBUG
                printf("   [%3d] TRIGGER TIME: 0x%06x\n",ii,a_trigtime[iii]);
                printf(">>> remember timestamp 0x%06x 0x%06x\n",a_trigtime[0],a_trigtime[1]);
#endif
                iii++;
                ii++;
	          }
            }
            else if( ((datain[ii]>>27)&0x1F) == 0x14) /*window raw data: remember channel#*/
            {
              a_channel = ((datain[ii]>>23)&0xF);
              a_windowwidth = (datain[ii]&0xFFF);
#ifdef DEBUG
	          printf("[%3d] WINDOW RAW DATA: slot %d, channel %d, window width %d\n",ii,
			    a_slot,a_channel,a_windowwidth);
              printf(">>> remember channel %d\n",a_channel);
#endif

#ifdef DEBUG
              printf("0x%08x: befor CCOPEN(1)\n",b08);
              printf("0x%08x: befor CCOPEN(1), dataout=0x%08x\n",b08,dataout);
#endif

	          CCOPEN(0xe101,"c,i,l,N(c,Ns)",banknum);

#ifdef DEBUG
              printf("0x%08x: after CCOPEN(1)\n",b08);
              printf("0x%08x: after CCOPEN(1), dataout=0x%08x\n",b08,dataout);
#endif

#ifdef DO_PEDS
              if(mynev==10 && fd==NULL)
	          {
                printf("mynev=%d - opening pedestal file\n",mynev);
                if((dir=getenv("CLAS")) == NULL)
	            {
                  printf("ERROR: environment variable CLAS is not defined - exit\n");
                  exit(0);
		        }
                if((expid=getenv("EXPID")) == NULL)
	            {
                  printf("ERROR: environment variable EXPID is not defined - exit\n");
                  exit(0);
		        }

                sprintf(fname,"%s/parms/peds/%s/roc%02d.ped",dir,expid,rol->pid);
                fd = fopen(fname,"w");
                if(fd==NULL)
                {
                  printf("ttfa: ERROR: cannot open pedestal file >%s<\n",fname);
		        }
                else
	            {
                  printf("ttfa: pedestal file >%s< is opened for writing\n",fname);
                }

                sprintf(cname,"%s/parms/peds/%s/roc%02d.cnf",dir,expid,rol->pid);
                fc = fopen(cname,"w");
                if(fc==NULL)
                {
                  printf("ttfa: ERROR: cannot open pedestal-cnf file >%s<\n",cname);
		        }
                else
	            {
                  printf("ttfa: pedestal-cnf file >%s< is opened for writing\n",cname);
                }

                for(i=0; i<NSLOTS; i++)
	            {
                  for(j=0; j<NCHANS; j++)
  	              {
                    npeds[i][j] = 0;
                    pedval[i][j] = 0.0;
                    pedrms[i][j] = 0.0;
		          }
	            }
	          }
              else if(mynev==110 && fd!=NULL && fc!=NULL)
	          {
                printf("mynev=%d - closing pedestal file (Nmeasures=%d (%d %d ..))\n",
                  mynev,npeds[3][0],npeds[3][1],npeds[4][0]);
                for(i=0; i<NSLOTS; i++)
	            {
                  fprintf(fc,"FADC250_SLOT %d\n",i);
                  for(j=0; j<NCHANS; j++)
                  {
                    if(npeds[i][j]>0)
			        {
                      pedval[i][j] = pedval[i][j] / ((float)npeds[i][j]);
#ifdef VXWORKS
                      pedrms[i][j] = ( pedrms[i][j]/((float)npeds[i][j]) - (pedval[i][j]*pedval[i][j]) );
#else
                      pedrms[i][j] = sqrtf( pedrms[i][j]/((float)npeds[i][j]) - (pedval[i][j]*pedval[i][j]) );
#endif
                      fprintf(fd,"%2d %2d %5d %6.3f %2d\n",i,j,(int)pedval[i][j],pedrms[i][j],0);
                      fprintf(fc,"FADC250_CH_PED %d %7.3f\n",j,pedval[i][j]);
			        }
		          }
	            }

#ifndef VXWORKS
                /*open files for everybody*/
                if(chmod(fname,S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH) != 0)
                {
                  printf("ERROR: cannot change mode on output run config file\n");
                }
                if(chmod(cname,S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH) != 0)
                {
                  printf("ERROR: cannot change mode on output run config file\n");
                }
#endif

                fclose(fd);
                printf("ttfa: pedestal file >%s< is closed\n",fname);
                fd = NULL;

                fclose(fc);
                printf("ttfa: pedestal-cnf file >%s< is closed\n",fname);
                fc = NULL;
	          }
#endif

	          Nchan[0] ++; /* increment channel counter */
#ifdef DEBUG
              printf("0x%08x: increment Nchan[0]=%d\n",b08,Nchan[0]);
#endif

              *b08++ = a_channel; /* channel number */

#ifdef DEBUG
              printf("0x%08x: #samples %d\n",b08,a_windowwidth);
#endif
              b32 = (unsigned int *)b08;
              *b32 = a_windowwidth; /* the number of samples (same as window width) */
              b08 += 4;

	          ii++;

              /* use first 10% of the window to measure pedestal, or 2 if less then 2 */
              npedsamples = a_windowwidth / 10;
              if(npedsamples<2) npedsamples = 2;

              while( ((datain[ii]>>31)&0x1) == 0 && ii<rlen ) /*loop over all samples*/
	          {
                a_valid1 = ((datain[ii]>>29)&0x1);
                a_adc1 = ((datain[ii]>>16)&0xFFF);
                a_valid2 = ((datain[ii]>>13)&0x1);
                a_adc2 = (datain[ii]&0xFFF);

#ifdef DEBUG
                printf("   [%3d] WINDOW RAW DATA: valid1 %d, adc1 0x%04x, valid2 %d, adc2 0x%04x\n",ii,
                  a_valid1,a_adc1,a_valid2,a_adc2);
                printf("0x%08x: samples: %d %d\n",b08,a_adc1,a_adc2);
#endif

                b16 = (unsigned short *)b08;
                *b16 ++ = a_adc1;
                *b16 ++ = a_adc2;
                b08 += 4;

                ii++;

#ifdef DO_PEDS
                if(mynev>=10 && mynev<110 && npedsamples>0)
		        {
		          /*printf("[%2d][%2d] npedsamples=%d\n",a_slot,a_channel,npedsamples);*/
                  npedsamples -= 2;
                  npeds[a_slot][a_channel] += 2;
                  pedval[a_slot][a_channel] += *(b16-2);
                  pedval[a_slot][a_channel] += *(b16-1);
                  pedrms[a_slot][a_channel] += (*(b16-2))*(*(b16-2));
                  pedrms[a_slot][a_channel] += (*(b16-1))*(*(b16-1));
		        }
#endif
	          }
            }
            else if( ((datain[ii]>>27)&0x1F) == 0x15) /*window sum: obsolete*/
            {
#ifdef DEBUG
	          printf("[%3d] WINDOW SUM: must be obsolete\n",ii);
#endif
	          ii++;
            }
            else if( ((datain[ii]>>27)&0x1F) == 0x16) /*pulse raw data*/
            {
              a_channel = ((datain[ii]>>23)&0xF);
              a_pulsenumber = ((datain[ii]>>21)&0x3);
              a_firstsample = (datain[ii]&0x3FF);
#ifdef DEBUG
	          printf("[%3d] PULSE RAW DATA: channel %d, pulse number %d, first sample %d\n",ii,
			    a_channel,a_pulsenumber,a_firstsample);
#endif

              CCOPEN(0xe10F,"c,i,l,N(c,N(c,Ns))",banknum);
#ifdef DEBUG
              printf("0x%08x: CCOPEN(2)\n",b08);
#endif

              if(a_channel != a_channel_old)
              {
                a_channel_old = a_channel;

                Nchan[0] ++; /* increment channel counter */
#ifdef DEBUG
                printf("0x%08x: increment Nchan[0]=%d\n",b08,Nchan[0]);
#endif

                *b08++ = a_channel; /* channel number */

                Npuls = (unsigned int *)b08; /* remember the place to put the number of pulses */
                Npuls[0] = 0;
                b08 += 4;
              }


              Npuls[0] ++;
              *b08++ = a_firstsample; /* first sample */

              Nsamp = (unsigned int *)b08; /* remember the place to put the number of samples */
              Nsamp[0] = 0;
              b08 += 4;

	          ii++;

              while( ((datain[ii]>>31)&0x1) == 0)
	          {
                a_valid1 = ((datain[ii]>>29)&0x1);
                a_adc1 = ((datain[ii]>>16)&0xFFF);
                a_valid2 = ((datain[ii]>>13)&0x1);
                a_adc2 = (datain[ii]&0xFFF);
#ifdef DEBUG
                printf("   [%3d] PULSE RAW DATA: valid1 %d, adc1 0x%04x, valid2 %d, adc2 0x%04x\n",ii,
                  a_valid1,a_adc1,a_valid2,a_adc2);
#endif

                b16 = (unsigned short *)b08;
                *b16 ++ = a_adc1;
                *b16 ++ = a_adc2;
                b08 += 4;

                Nsamp[0] += 2;
                ii++;
	          }

            }












            else if( ((datain[ii]>>27)&0x1F) == 0x17) /* pulse integral */
            {
              a_channel = ((datain[ii]>>23)&0xF);
              a_pulsenumber = ((datain[ii]>>21)&0x3);
              a_qualityfactor = ((datain[ii]>>19)&0x3);
              a_pulseintegral = (datain[ii]&0x7FFFF);
#ifdef DEBUG
	          printf("[%3d] PULSE INTEGRAL: channel %d, pulse number %d, quality %d, integral %d\n",ii,
			    a_channel,a_pulsenumber,a_qualityfactor,a_pulseintegral);
#endif


#ifdef DEBUG
              printf("0x%08x: pulseintegral = %d\n",b08,a_pulseintegral);
#endif


#ifdef MODE7
              if(npulse_VmVp[iev][a_slot][a_channel] == 0) /* it means mode 3 - fill output, otherwise do nothing */
			  {
#endif
                b32 = (unsigned int *)b08;
                *b32 = a_pulseintegral;
                b08 += 4;
#ifdef MODE7
			  }
#endif


	          ii++;
            }








            else if( ((datain[ii]>>27)&0x1F) == 0x18) /* pulse time */
            {
              a_channel = ((datain[ii]>>23)&0xF);
              a_pulsenumber = ((datain[ii]>>21)&0x3);
              a_qualityfactor = ((datain[ii]>>19)&0x3);
              a_pulsetime = (datain[ii]&0xFFFF);
#ifdef DEBUG
	          printf("[%3d] PULSE TIME: channel %d, pulse number %d, quality %d, time %d\n",ii,
			    a_channel,a_pulsenumber,a_qualityfactor,a_pulsetime);
#endif


#ifdef MODE7
              if(npulse_VmVp[iev][a_slot][a_channel] > 0) /* it means mode 7 */
			  {
if(a_pulsenumber == 0)
{
                CCOPEN(0xe102,"c,i,l,N(c,N(s,i,s,s))",banknum);
#ifdef DEBUG
                printf("0x%08x: CCOPEN(2), dataout=0x%08x\n",b08,dataout);
#endif
}
			  }
			  else
			  {
#endif
                CCOPEN(0xe103,"c,i,l,N(c,N(s,i))",banknum);
#ifdef DEBUG
                printf("0x%08x: CCOPEN(3), dataout=0x%08x\n",b08,dataout);
#endif
#ifdef MODE7
			  }
#endif

              if(a_channel != a_channel_old)
              {
                a_channel_old = a_channel;

                Nchan[0] ++; /* increment channel counter */
#ifdef DEBUG
                printf("0x%08x: increment Nchan[0]=%d\n",b08,Nchan[0]);
#endif

#ifdef DEBUG
		        printf("0x%08x: channel %d\n",b08,a_channel);
#endif
                *b08++ = a_channel; /* channel number */

                Npuls = (unsigned int *)b08; /* remember the place to put the number of pulses */
                Npuls[0] = 0;
#ifdef DEBUG
		        printf("0x%08x: Npuls[0]\n",b08);
#endif
                b08 += 4;
              }


#ifdef MODE7
              if(npulse_VmVp[iev][a_slot][a_channel] > 0) /* it means mode 7 */
			  {
if(a_pulsenumber == 0)
{
			    Npuls[0] = npulse_integral[iev][a_slot][a_channel];
				if(Npuls[0]>4) printf("ERROR: Npuls=%d\n",Npuls[0]);
                for(i=0; i<Npuls[0]; i++)
			    {                
                  b16 = (unsigned short *)b08;
                  *b16 = pulse_time[iev][a_slot][a_channel][i];
                  b08 += 2;

                  b32 = (unsigned int *)b08;
                  *b32 = pulse_integral[iev][a_slot][a_channel][i];
                  b08 += 4;

                  b16 = (unsigned short *)b08;
                  *b16 = pulse_Vm[iev][a_slot][a_channel][i];
                  b08 += 2;

                  b16 = (unsigned short *)b08;
                  *b16 = pulse_Vp[iev][a_slot][a_channel][i];
                  b08 += 2;
			    }
}
			  }
			  else
			  {
                Npuls[0] ++;
                b16 = (unsigned short *)b08;
                *b16 = a_pulsetime;
                b08 += 2;
			  }
#else

              Npuls[0] ++;
#ifdef DEBUG
              printf("0x%08x: pulsetime = %d\n",b08,a_pulsetime);
#endif
              b16 = (unsigned short *)b08;
              *b16 = a_pulsetime;
              b08 += 2;
#endif




	          ii++;
            }
















            else if( ((datain[ii]>>27)&0x1F) == 0x19)
            {
#ifdef DEBUG
	          printf("[%3d] STREAMING RAW DATA: \n",ii);
#endif
	          ii++;
            }




















            else if( ((datain[ii]>>27)&0x1F) == 0x1a) /* Vm Vp */
            {
              a_channel = ((datain[ii]>>23)&0xF);
              a_pulsenumber = ((datain[ii]>>21)&0x3);
              a_vm = ((datain[ii]>>12)&0x1FF);
              a_vp = (datain[ii]&0xFFF);
#ifdef DEBUG
	          printf("[%3d] PULSE VmVp: channel %d, pulse number %d, Vm %d, Vp %d\n",ii,
			    a_channel,a_pulsenumber,a_vm,a_vp);
#endif

              /* do not output anything, will be done above in 'pulse time' processing */

	          ii++;
            }
















            else if( ((datain[ii]>>27)&0x1F) == 0x1D)
            {
#ifdef DEBUG
	          printf("[%3d] EVENT TRAILER: \n",ii);
#endif
	          ii++;
            }
            else if( ((datain[ii]>>27)&0x1F) == 0x1E)
            {
	          printf("[%3d] : DATA NOT VALID\n",ii);
              exit(0);
	          ii++;
            }
            else if( ((datain[ii]>>27)&0x1F) == 0x1F)
            {
#ifdef DEBUG
	          printf("[%3d] FILLER WORD: \n",ii);
              printf(">>> do nothing\n");
#endif
	          ii++;
            }
            else
            {
              if(printing) /* printing only once at every event */
              {
                printf("[%3d] pass2 ERROR: in FADC data format 0x%08x (bits31-27=0x%02x)\n",
			      ii,(int)datain[ii],(datain[ii]>>27)&0x1F);
                printing=0;
	          }
              ii++;
            }

          } /*while*/

        } /* loop over blocks */


        if(b08 != NULL) CCCLOSE; /*call CCCLOSE only if CCOPEN was called*/
#ifdef DEBUG
          printf("0x%08x: CCCLOSE1, dataout=0x%08x\n",b08,dataout);
          printf("0x%08x: CCCLOSE2, dataout=0x%08x\n",b08,(unsigned int *) ( ( ((unsigned int)b08+3)/4 ) * 4));
          printf("-> 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x\n",
		    (unsigned int)b08+3,((unsigned int)b08+3),
            ((unsigned int)b08+3) / 4,(((unsigned int)b08+3) / 4)*4, 
		    (unsigned int *)((((unsigned int)b08+3) / 4)*4) );
#endif


	  } /* FADC250 hardware format */


      else if(banktag[jj] == 0xe10A) /* TI hardware format */
	  {

        banknum = iev; /*rol->pid;*/

#ifdef DEBUG1
		printf("SECOND PASS TI\n");
#endif
        for(ibl=0; ibl<nB[jj]; ibl++) /*loop over blocks*/
        {
#ifdef DEBUG1
          printf("\n\n\n0xe10A: Block %d, Event %2d, event index %2d, event lenght %2d\n",
            ibl,iev,iE[jj][ibl][iev],lenE[jj][ibl][iev]);
#endif
          a_slot = sB[jj][ibl];
          ii = iE[jj][ibl][iev];
          rlen = ii + lenE[jj][ibl][iev];
          while(ii<rlen)
          {
#ifdef DEBUG1
            printf("[%5d] 0x%08x (rlen=%d)\n",ii,datain[ii],rlen);
#endif
           
            CPOPEN(0xe10A,1,banknum);
			
            /* event header */
            a_event_type = ((datain[ii]>>24)&0xFF);
            if(((datain[ii]>>16)&0xFF)!=0x01) printf("ERROR in TI event header word (0x%02x)\n",((datain[ii]>>16)&0xFF));
            a_nwords = datain[ii]&0xFF;
#ifdef DEBUG1
		    printf("[%3d] EVENT HEADER, a_nwords = %d\n",ii,a_nwords);
#endif
			
            dataout[0] = datain[ii];
            b08 += 4;

		    ii++;

		    if(a_nwords>0)
		    {
		      a_event_number_l = datain[ii];
#ifdef DEBUG1
		      printf("[%3d] a_event_number_1 = %d\n",ii,a_event_number_l);
#endif
			  
              dataout[1] = datain[ii];
              b08 += 4;
			  
		      ii++;
		    }

		    if(a_nwords>1)
		    {
		      a_timestamp_l = datain[ii];
#ifdef DEBUG1
		      printf("[%3d] a_timestamp_l = %d\n",ii,a_timestamp_l);
#endif
			  
              dataout[2] = datain[ii];
              b08 += 4;
			  
		      ii++;
		    }

		    if(a_nwords>2)
		    {
		      a_event_number_h = (datain[ii]>>16)&0xFFFF;
		      a_timestamp_h = datain[ii]&0xFFFF;
#ifdef DEBUG1
		      printf("[%3d] a_event_number_h = %d a_timestamp_h = %d \n",ii,a_event_number_h,a_timestamp_h);
#endif
			  
              dataout[3] = datain[ii];
              b08 += 4;
			  
		      ii++;
		    }

			
            CPCLOSE;


	      } /* while() */

        } /* loop over blocks */

	  } /* TI hardware format */












      else if(banktag[jj] == 0xe10B) /* 0xe10B TDC hardware format */
	  {

        banknum = iev; /*rol->pid;*/
#ifdef DEBUG2
        printf("CPOPEN 0xe107\n");
#endif
	
        CPOPEN(0xe107,1,banknum);

        for(ibl=0; ibl<nB[jj]; ibl++) /*loop over blocks*/
        {
#ifdef DEBUG2
		  printf("\nSECOND PASS TDC (ibl=%d, nB=%d)\n",ibl,nB[jj]);
          printf("\n\n\n0xe10B: Block %d, Event %2d, event index %2d, event lenght %2d\n",
            ibl,iev,iE[jj][ibl][iev],lenE[jj][ibl][iev]);
#endif





          a_slot = sB[jj][ibl];
          ii = iE[jj][ibl][iev];
          rlen = ii + lenE[jj][ibl][iev];
          while(ii<rlen)
          {
#ifdef DEBUG2
            printf("[%5d] 0x%08x (rlen=%d)\n",ii,datain[ii],rlen);
#endif
		    if( ((datain[ii]>>27)&0x1F) == 0) /* data */
            {
              nbcount ++;

              tdcedge = ((datain[ii]>>26)&0x1);
		      /*printf("tdcslot=%d tdctypebyslot=%d\n",tdcslot,tdctypebyslot[tdcslot]);*/
              if( tdctypebyslot[tdcslot] == 2 ) /*1290*/
              {
                tdcchan = ((datain[ii]>>21)&0x1F);
                tdcval = (datain[ii]&0x1FFFFF);
              }
              else /*1190*/
              {
                tdcchan = ((datain[ii]>>19)&0x7F);
                tdcval = (datain[ii]&0x7FFFF);
			    /*if(tdcchan==63) {reftdc=1; printf("[ev %d] befor: 0x%08x\n",*(rol->nevents),tdcval);}*/
                tdcval = tdcval<<2; /*report 1190 value with same resolution as 1290*/
			    /*if(tdcchan==63) printf("[ev %d] after: 0x%08x (0x%08x)\n",*(rol->nevents),tdcval,tdcval&0x7FFFF);*/
		      }

              /* output data in 'standard' format: slot[27-31]  edge[26]  chan[19-25]  tdc[0-18] */
              *dataout ++ = (tdcslot<<27) + (tdcedge<<26) + (tdcchan<<19) + (tdcval&0x7FFFF);
              b08 += 4;

#ifdef DEBUG2
		      if(1/*tdcchan<2*/)
              {
                printf("   DATA: type=%1d slot=%02d tdc=%02d ch=%03d edge=%01d value=%08d\n",
				   tdctypebyslot[tdcslot],tdcslot,(int)tdc14,(int)tdcchan,(int)tdcedge,(int)tdcval);
		      }
#endif
            }
            else if( ((datain[ii]>>27)&0x1F) == 1)
            {
              tdc14 = ((datain[ii]>>24)&0x3);
              tdceventid = ((datain[ii]>12)&0xFFF);
              tdcbunchid = (datain[ii]&0xFFF);
#ifdef DEBUG2
              printf(" HEAD: tdc=%02d event_id=%05d bunch_id=%05d\n",
                (int)tdc14,(int)tdceventid,(int)tdcbunchid);
#endif
            }
            else if( ((datain[ii]>>27)&0x1F) == 3)
            {
              tdc14 = ((datain[ii]>>24)&0x3);
              tdceventid = ((datain[ii]>12)&0xFFF);
              tdcwordcount = (datain[ii]&0xFFF);
#ifdef DEBUG2
              printf(" EOB:  tdc=%02d event_id=%05d word_count=%05d\n",
                (int)tdc14,(int)tdceventid,(int)tdcwordcount);
#endif
            }
            else if( ((datain[ii]>>27)&0x1F) == 0x18)
            {
#ifdef DEBUG2
              printf(" FILLER\n");
#endif
            }
            else if( ((datain[ii]>>27)&0x1F) == 0x11)
            {
              nbsubtract ++;
#ifdef DEBUG2
              printf(" TIME TAG: %08x\n",(int)(datain[ii]&0x7ffffff));
#endif
            }
            else if( ((datain[ii]>>27)&0x1F) == 8)
            {
              nbcount = 1; /* counter for the number of output words from board */
              nbsubtract = NBsubtract; /* # words to subtract including errors (5 for v1290N, 9 for others) */
              tdcslot = datain[ii]&0x1F;
              tdceventcount = (datain[ii]>>5)&0x3FFFFF;
              tdchead = (unsigned int *) dataout; /* remember pointer */
#ifdef SLOTWORKAROUND
              tdcslot_h = tdcslot;
	          remember_h = datain[ii];

              /* correct slot number - cannot do it that was for multi-event readout
              if(slotnums[nheaders] != tdcslot)
		      {
                if( !((*(rol->nevents))%10000) ) printf("WARN: [%2d] slotnums=%d, tdcslot=%d -> use slotnums\n",
				   nheaders,slotnums[nheaders],tdcslot);
                tdcslot = slotnums[nheaders];
		      }
		      */
#endif
			  /*sergey: do not output global header
              *dataout ++ = tdcslot;
              b08 += 4;
			  */

              nheaders++;

		  
#ifdef DEBUG2
		  
              printf("GLOBAL HEADER: 0x%08x (slot=%d nevent=%d) -> header=0x%08x\n",
                datain[ii],datain[ii]&0x1F,(datain[ii]>>5)&0xFFFFFF,*tdchead);
		  
#endif
		  
            }
            else if( ((datain[ii]>>27)&0x1F) == 0x10)
            {
#ifdef SLOTWORKAROUND
              /* double check for slot number */
              tdcslot_t = datain[ii]&0x1F;

		      /*
              if(slotnums[ntrailers] != tdcslot_t)
		      {
                if( !((*(rol->nevents))%10000) ) printf("WARN: [%2d] slotnums=%d, tdcslot=%d\n",
				   ntrailers,slotnums[ntrailers],tdcslot_t);
		      }
		      */


              if(tdcslot_h>21||tdcslot_t>21||tdcslot_h!=tdcslot_t)
              {
                /*printf("WARN: slot from header=%d (0x%08x), from trailer=%d (0x%08x), must be %d\n",
			      tdcslot_h,remember_h,tdcslot_t,datain[ii],tdcslot)*/;
              }
#endif
              /* put nwords in header; substract 4 TDC headers, 4 TDC EOBs,
              global trailer and error words if any */
              /**tdchead = (*tdchead) | (((datain[ii]>>5)&0x3FFF) - nbsubtract);*/
              *tdchead |= ((((datain[ii]>>5)&0x3FFF) - nbsubtract)<<5);
              if((((datain[ii]>>5)&0x3FFF) - nbsubtract) != nbcount)
              {			
                printf("ERROR: word counters: %d != %d (subtract=%d)\n",
			      (((datain[ii]>>5)&0x3FFF) - nbsubtract),nbcount,nbsubtract);
              }
              ntrailers ++;

              nwtdc = (datain[ii]>>5)&0x3FFF;
#ifdef DEBUG2
              printf("GLOBAL TRAILER: 0x%08x (slot=%d nw=%d stat=%d) -> header=0x%08x\n",
                datain[ii],datain[ii]&0x1F,(datain[ii]>>5)&0x3FFF,
                (datain[ii]>>23)&0xF,*tdchead);
#endif

#ifdef DEBUG2
		      printf("0xe10B: close event jj=%d event# %d length %d\n",jj,m,lenE[jj][k][m]);
#endif
            }
            else if( ((datain[ii]>>27)&0x1F) == 4)
            {
              nbsubtract ++;
              tdc14 = ((datain[ii]>>24)&0x3);
              tdcerrorflags = (datain[ii]&0x7FFF);
		  /*
#ifdef DEBUG2
		  */
		      if( !(error_flag[tdcslot]%100) ) /* do not print for every event */
              {
                unsigned int ddd, lock, ntdc;
			
                printf(" ERR: event# %7d, slot# %2d, tdc# %02d, error_flags=0x%08x, err=0x%04x, lock=0x%04x\n",
                  tdceventcount,tdcslot,(int)tdc14,(int)tdcerrorflags,ddd,lock);
		      }
              error_flag[tdcslot] ++;
		  /*
#endif
		  */
            }
            else
            {
#ifdef DEBUG2
              printf("ERROR: in TDC data format 0x%08x\n",(int)datain[ii]);
#endif
            }

		    ii ++;

	      } /* while() */


        } /* loop over blocks */

        CPCLOSE;
#ifdef DEBUG2
        printf("CPCLOSE 0xe107\n");
#endif

	  } /* TDC hardware format */







      else if(banktag[jj] == 0xe10C) /* SSP hardware format */
	  {

        banknum = iev; /*rol->pid;*/

#ifdef DEBUG3
		printf("SECOND PASS SSP\n");
#endif
        for(ibl=0; ibl<nB[jj]; ibl++) /*loop over blocks*/
        {
#ifdef DEBUG3
          printf("\n\n\n0xe10C: Block %d, Event %2d, event index %2d, event lenght %2d\n",
            ibl,iev,iE[jj][ibl][iev],lenE[jj][ibl][iev]);
#endif
         
          CPOPEN(0xe10C,1,banknum);

          a_slot = sB[jj][ibl];
          ii = iE[jj][ibl][iev];
          rlen = ii + lenE[jj][ibl][iev];
          while(ii<rlen)
          {
#ifdef DEBUG3
            printf("[%5d] 0x%08x (rlen=%d)\n",ii,datain[ii],rlen);
#endif
			
            if( ((datain[ii]>>27)&0x1F) == 0x12) /*event header*/
			{
              a_triggernumber = (datain[ii]&0x7FFFFFF);
#ifdef DEBUG3
		      printf("[%3d] EVENT HEADER, a_triggernumber = %d\n",ii,a_triggernumber);
#endif
			
              *dataout ++ = datain[ii];
              b08 += 4;

		      ii++;
			}

            else if( ((datain[ii]>>27)&0x1F) == 0x13) /*trigger time: remember timestamp*/
            {
              a_trigtime[0] = (datain[ii]&0xFFFFFF);
              *dataout ++ = datain[ii];
              b08 += 4;
	          ii++;

              a_trigtime[1] = (datain[ii]&0xFFFFFF);
              *dataout ++ = datain[ii];
              b08 += 4;
              ii++;

#ifdef DEBUG3
              printf(">>> remember timestamp 0x%06x 0x%06x\n",a_trigtime[0],a_trigtime[1]);
#endif
	        }

            else if( ((datain[ii]>>27)&0x1F) == 0x14) /*hps cluster*/
            {
              a_clusterN = ((datain[ii]>>23)&0xF);
              a_clusterE = ((datain[ii]>>10)&0x1FFF);
		      a_clusterY = ((datain[ii]>>6)&0xF);
              a_clusterX = (datain[ii]&0x3F);
              *dataout ++ = datain[ii];
              b08 += 4;
	          ii++;

              a_clusterT = (datain[ii]&0x3FF);
              *dataout ++ = datain[ii];
              b08 += 4;
              ii++;

#ifdef DEBUG3
		      printf("[%3d] HPS CLUSTER_(N,E,Y,X,T): %d %d %d %d\n",ii,a_clusterN,a_clusterE,a_clusterY,a_clusterX,a_clusterT);
#endif
            }

            else if( ((datain[ii]>>27)&0x1F) == 0x15) /*hps trigger*/
            {
              a_type = ((datain[ii]>>23)&0xF);
              a_data = ((datain[ii]>>16)&0x7F);
		      a_time = (datain[ii]&0x3FF);
              *dataout ++ = datain[ii];
              b08 += 4;
	          ii++;
#ifdef DEBUG3
	          printf("[%3d] HPS TRIGGER: TYPE %d, DATA %d, TIME %d\n",ii,a_type,a_data,a_time);
#endif
		    }

            else if( ((datain[ii]>>27)&0x1F) == 0x1F)
            {
#ifdef DEBUG3
	          printf("[%3d] FILLER WORD\n",ii);
              printf(">>> do nothing\n");
#endif
	          ii++;
            }
            else if( ((datain[ii]>>27)&0x1F) == 0x1E)
            {
#ifdef DEBUG3
	          printf("[%3d] DNV\n",ii);
              printf(">>> do nothing\n");
#endif
	          ii++;
            }
            else
		    {
              printf("SSP UNKNOWN data: [%3d] 0x%08x\n",ii,datain[ii]);
              ii++;
		    }

	      } /* while() */

          CPCLOSE;

        } /* loop over blocks */

	  } /* SSP hardware format */





      else if(banktag[jj] == 0xe112) /* HEAD bank raw format */
	  {

        banknum = iev; /*rol->pid;*/

#ifdef DEBUG5
		printf("\n\nSECOND PASS HEAD\n");
#endif
        for(ibl=0; ibl<nB[jj]; ibl++) /*loop over blocks*/
        {
#ifdef DEBUG5
          printf("\n\n\n0xe10C: Block %d, Event %2d, event index %2d, event lenght %2d\n",
            ibl,iev,iE[jj][ibl][iev],lenE[jj][ibl][iev]);
#endif
		
          CPOPEN(0xe10F,1,banknum);

          ii = iE[jj][ibl][iev];
          rlen = ii + lenE[jj][ibl][iev];
          while(ii<rlen)
          {
#ifdef DEBUG5
            printf("[%5d] 0x%08x (rlen=%d)\n",ii,datain[ii],rlen);
#endif
			
            if( ((datain[ii]>>27)&0x1F) == 0x12) /*event header*/
			{
              a_triggernumber = (datain[ii]&0x7FFFFFF);
#ifdef DEBUG5
		      printf("[%3d] EVENT HEADER, a_triggernumber = %d\n",ii,a_triggernumber);
#endif
		      ii++;
			}

            else if( ((datain[ii]>>27)&0x1F) == 0x14) /*head bank data*/
            {
              nw = (datain[ii]&0xFF);
#ifdef DEBUG5
	          printf("[%3d] head bank has %d data words\n",ii,nw);
#endif
	          ii++;
              iii=0;
              while( iii<nw ) /*get all data words*/
	          {
#ifdef DEBUG5
                printf("   [%3d][%3d]: %d\n",ii,iii,datain[ii]);
#endif
                *dataout ++ = datain[ii];
                b08 += 4;
                iii++;
                ii++;
	          }
            }

            else
		    {
              printf("PASS2 HEAD UNKNOWN data: [%3d] 0x%08x\n",ii,datain[ii]);
              ii++;
		    }

	      } /* while() */

          CPCLOSE;

        } /* loop over blocks */

	  } /* HEAD bank raw format */





      else if(banktag[jj] == 0xe104) /* VSCM hardware format */
	  {

        banknum = iev; /*rol->pid;*/

#ifdef DEBUG4
		printf("\n\nSECOND PASS VSCM\n");
#endif
        a_slot_old = -1;
        for(ibl=0; ibl<nB[jj]; ibl++) /*loop over blocks*/
        {
          a_channel_old = -1;
#ifdef DEBUG4
          printf("\n\n\nVSCM: Block %d, Event %2d, event index %2d, event lenght %2d\n",
            ibl,iev,iE[jj][ibl][iev],lenE[jj][ibl][iev]);
#endif

          a_slot = sB[jj][ibl];
          ii = iE[jj][ibl][iev];
          rlen = ii + lenE[jj][ibl][iev];
          while(ii<rlen)
          {
#ifdef DEBUG4
            printf("[%5d] VSCM 0x%08x (rlen=%d)\n",ii,datain[ii],rlen);
#endif
			
            if( ((datain[ii]>>27)&0x1F) == DATA_TYPE_EVTHDR) /*event header*/
			{
              a_triggernumber = (datain[ii]&0x7FFFFFF);
#ifdef DEBUG4
		      printf("[%3d] VSCM EVENT HEADER, a_triggernumber = %d\n",ii,a_triggernumber);
#endif
		      ii++;
			}

            else if( ((datain[ii]>>27)&0x1F) == DATA_TYPE_TRGTIME) /*trigger time: remember timestamp*/
            {
              a_trigtime[0] = (datain[ii]&0xFFFFFF);
	          ii++;

              a_trigtime[1] = (datain[ii]&0xFFFFFF);
              ii++;

		      timestamp = (((unsigned long long)a_trigtime[0])<<24) | (a_trigtime[1]);

#ifdef DEBUG4
              printf(">>> VCSM remember timestamp 0x%06x 0x%06x\n",a_trigtime[0],a_trigtime[1]);
		      printf(">>> VSCM timestamp=%lld (bco style = %lld)\n",timestamp,((timestamp / (long long)(16)) % 256)); /*16-from par file*/
#endif

	        }

            else if( ((datain[ii]>>27)&0x1F) == DATA_TYPE_BCOTIME) /* bco start and stop */
		    {
#ifdef DEBUG4
              printf("[%3d] VSCM  BCOTIME START: %u STOP: %u\n",ii, (datain[ii]>>0) & 0xFF, (datain[ii]>>16) & 0xFF);
#endif
              ii++;
		    }




            else if( ((datain[ii]>>27)&0x1F) == DATA_TYPE_FSSREVT) /*FSSR event*/
            {
              a_hfcb_id = ((datain[ii]>>22)&0x1);
              a_chip_id = ((datain[ii]>>19)&0x7);
              a_chan = ((datain[ii]>>12)&0x7F);
              a_bco = ((datain[ii]>>4)&0xFF);
              a_adc1 = (datain[ii]&0x7);

              a_channel = a_chan + (a_chip_id<<7) + (a_hfcb_id<<10);

              a_chan1 = a_chip_id + (a_hfcb_id<<3);
              a_chan2 = a_chan;

#ifdef DEBUG4
	          printf("[%3d] VSCM FSSREVT: HFCBID=%1u CHIPID=%1u CHAN=%3u (%4u)  BCO=%3u ADC=%1u \n",
					 ii,a_hfcb_id,a_chip_id,a_chan,a_channel,a_bco,a_adc1);
#endif



              CCOPEN(0xe111,"c,i,l,N(c,c,c,c)",banknum);
#ifdef DEBUG4
              printf("0x%08x: CCOPEN(VSCM), dataout=0x%08x\n",b08,dataout);
#endif


              if(a_channel != a_channel_old)
              {
                a_channel_old = a_channel;

	            Nchan[0] ++; /* increment channel counter */
#ifdef DEBUG4
                printf("0x%08x: VSCM increment Nchan[0]=%d\n",b08,Nchan[0]);
#endif

				/*
                b16 = (unsigned short *)b08;
                *b16++ = a_channel;
                b08 += 2;
				*/
                *b08++ = a_chan1;
                *b08++ = a_chan2;

                /* remember the place to put the number of pulses 
                Npuls = (unsigned int *)b08;
                Npuls[0] = 0;
                b08 += 4;
				*/
			  }

              /* calculate 'latency' and put it in a data instead of 'bco' (16*8 is bco period (ns)) */
              latency_offset = timestamp % ((long long)VSCM_BCO_FREQ);
              latency = ABS( ((timestamp / (long long)VSCM_BCO_FREQ) % 256) - (long long)(a_bco) );
              latency = latency*((long long)VSCM_BCO_FREQ) + latency_offset; /* in clocks=8ns */
#ifdef DEBUG4
              printf("VSCM>>> bco=%u timestamp=%lld latency=%lld\n",a_bco,timestamp,latency);
              printf("VSCM>>> latency=%d BCO's (%d ns)",latency,latency*8);
#endif

              /*Npuls[0] ++*/;
              *b08++ = a_bco/*latency*/;
              *b08++ = a_adc1;

	          ii++;
            }









            else if( ((datain[ii]>>27)&0x1F) == DATA_TYPE_FILLER)
            {
#ifdef DEBUG4
	          printf("[%3d] VSCM FILLER WORD\n",ii);
              printf(">>> VSCM do nothing\n");
#endif
	          ii++;
            }
            else if( ((datain[ii]>>27)&0x1F) == DATA_TYPE_DNV)
            {
#ifdef DEBUG4
	          printf("[%3d] VSCM DNV\n",ii);
              printf(">>> VSCM do nothing\n");
#endif
	          ii++;
            }
            else
		    {
              printf("VSCM UNKNOWN data: [%3d] 0x%08x\n",ii,datain[ii]);
              ii++;
		    }

	      } /* while() */


        } /* loop over blocks */


        if(b08 != NULL) CCCLOSE; /*call CCCLOSE only if CCOPEN was called*/
#ifdef DEBUG4
        printf("0x%08x: CCCLOSE1, dataout=0x%08x\n",b08,dataout);
        printf("0x%08x: CCCLOSE2, dataout=0x%08x\n",b08,(unsigned int *) ( ( ((unsigned int)b08+3)/4 ) * 4));
        printf("-> 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x\n",
		    (unsigned int)b08+3,((unsigned int)b08+3),
            ((unsigned int)b08+3) / 4,(((unsigned int)b08+3) / 4)*4, 
		    (unsigned int *)((((unsigned int)b08+3) / 4)*4) );
#endif


	  } /* VSCM hardware format */





    } /* loop over banks  */



#ifdef PASS_AS_IS
    /* if last event, loop over banks to be passed 'as is' and attach them to that last event */
    if(iev==(nnE-1))
	{
      for(ii=0; ii<nASIS; ii++)
      {
        jj = iASIS[ii]; /* bank number as it reported by BANKSCAN */
        datain = bankdata[jj];
        lenin = banknw[jj];
        /*printf("mynev=%d: coping bank number %d (header %d 0x%08x))\n",mynev,jj,*(datain-2),*(datain-1));*/
	    
     
        CPOPEN(banktag[jj],banktyp[jj],banknr[jj]);
        for(kk=0; kk<lenin; kk++)
        {
          dataout[kk] = datain[kk];
          b08 += 4;
        }        
        CPCLOSE;
	  }
    }
#endif


#ifdef SPLIT_BLOCKS

	/*
      at that point we want to close previout bank-of-banks and create header for new one;
      if iev==0, header already exist (came from ROL1); if iev==(nnE-1), do not create next header
    */

    header[0] = lenev - 1;
    header[1] = rol->dabufpi[1];

    /* header created by CEOPEN macros (see rol.h) */

    /* event type obtained from TI board have to be recorded into fragment header - event builder need it */
/*TEMP while(a_event_type>15) a_event_type --; TEMP*/
    header[1] = (header[1]&0xFF00FFFF) + (a_event_type<<16);


    /* time stamp obtained from TI board have to be recorded into fragment header - event builder need it */
    header[1] = (header[1]&0xFFFF00FF) + ((a_timestamp_l&0xFF)<<8);


    /* event number obtained from TI board have to be recorded into fragment header - event builder need it */
    header[1] = (header[1]&0xFFFFFF00) + (a_event_number_l&0xFF);
/*
	printf("HEADER: sync_flag=%d event_type=%d bank_type=%d event_number=%d\n",
      (header[1]>>24)&0xFF,(header[1]>>16)&0xFF,(header[1]>>8)&0xFF,header[1]&0xFF);
*/

  /*printf("(%d) header[0]=0x%08x (0x%08x)\n",iev,header[0],lenev - 1);*/
  /*printf("(%d) header[1]=0x%08x (0x%08x)\n",iev,header[1],rol->dabufpi[1]);*/

    /* if NOT the last event, create header for the next event */
    if(iev<(nnE-1))
	{
  /*printf("bump header pointer, iev=%d\n",iev);*/
      header = dataout;
      dataout += 2;
      lenout += 2;
	}


#endif



  } /* loop over events */





  /* returns full fragment length (long words) */  

#ifdef DEBUG 
  printf("return lenout=%d\n**********************\n\n",lenout);
#endif



/*
if(lenout>2) printf("return lenout=%d\n**********************\n\n",lenout);
*/

  rol->user_storage[0] = lenout;

#ifdef SPLIT_BLOCKS
  rol->user_storage[1] = nnE; /* report the number of events */
#else
  CPEXIT;
  rol->user_storage[1] = 1; /* report the number of events */
#endif

  /*printf("return lenout=%d\n**********************\n\n",lenout);*/

/*printf("--> %d %d\n",rol->user_storage[0],rol->user_storage[1]);*/


   /*print output buffer*/
  /*dataout = (unsigned int *)(rol->dabufp);
  printf("lenout=%d\n",lenout);
  printf("dabufi: 0x%08x 0x%08x 0x%08x 0x%08x \n",rol->dabufpi[0],rol->dabufpi[1],rol->dabufpi[2],rol->dabufpi[3]);
  for(ii=0; ii<lenout; ii++) printf("  DATA [%3d] 0x%08x (%d)\n",ii,dataout[ii],dataout[ii]);
  */

  return;
}

void
rol2trig_done()
{
  return;
}  


void
__done()
{
  /* from parser */
  poolEmpty = 0; /* global Done, Buffers have been freed */

  return;
}
  
static void
__status()
{
  return;
}  

/*hps12 buring crash:
top - 17:36:34 up 5 days,  3:47,  1 user,  load average: 3.48, 1.20, 0
Tasks: 185 total,   3 running, 182 sleeping,   0 stopped,   0 zombie
Cpu(s):  3.3%us,  1.1%sy,  0.0%ni, 95.5%id,  0.0%wa,  0.0%hi,  0.0%si,
Mem:   8221276k total,   363480k used,  7857796k free,        0k buffe
Swap:        0k total,        0k used,        0k free,   182572k cache

  PID USER      PR  NI  VIRT  RES  SHR S %CPU %MEM    TIME+  COMMAND  
 8497 clasrun   19   0  5700 1428  516 R 100.0  0.0   0:04.84 csh     
 8496 clasrun   18   0  5708 1440  516 S 98.1  0.0   0:03.24 csh      
 8584 clasrun   18   0  2448 1000  732 R 80.2  0.0   0:05.15 top      
 3597 clasrun   18   0 1826m 2232  764 S 62.3  0.0   1:14.17 DiagGuiSe
 3211 root      17   0 12848 1300  560 S 25.4  0.0   0:31.02 pcscd    
*/
