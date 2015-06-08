/*
do_ypcall: clnt_call: RPC: Timed out
*/
#undef HAVE_SSP_DATA
#define NSSP 8

/* ttfa.c - translate FADC250 data into EVIO composite format - for HPS (sergey Feb 2012) */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "coda.h"
#include "tt.h"

#undef DO_PEDS

#undef DEBUG


#define LSWAP(x)        ((((x) & 0x000000ff) << 24) | \
                         (((x) & 0x0000ff00) <<  8) | \
                         (((x) & 0x00ff0000) >>  8) | \
                         (((x) & 0xff000000) >> 24))

/*THIS IS THE COMPOSITE OPEN*/
#define CCOPEN(btag,fmt,bnum) \
  /*if it is first board, open bank*/ \
  if(a_slot_old==-1) \
  { \
    { \
      int len1, n1; \
      char *ch; \
      len1 = strlen(fmt); /* format length in bytes */ \
      n1 = (len1+5)/4; /* format length in words */ \
      dabufp_save1 = dabufp ++; /*remember '0xf' bank length location*/ \
      *dabufp++ = (btag<<16) + (0xf<<8) + bnum; /*bank header*/ \
      /* tagsegment header following by format */ \
      *dabufp++ = (len1<<20) + (0x6<<16) + n1; \
      ch = (char *)dabufp; \
      strncpy(ch,fmt,len1); \
      ch[len1]='\0';ch[len1+1]='\4';ch[len1+2]='\4';ch[len1+3]='\4';ch[len1+4]='\4'; \
      dabufp += n1; \
      /* 'internal' bank header */ \
      dabufp_save2 = dabufp ++;  /*remember 'internal' bank length location*/ \
      *dabufp++ = (0<<16) + (0x0<<8) + 0; \
    } \
    b08 = (unsigned char *)dabufp; \
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
    *b64 = (((unsigned long long)a_trigtime[1])<<24) | a_trigtime[0];	\
    b08 += 8; \
    /*set pointer for the number of channels*/ \
    Nchan = (unsigned int *)b08; \
    Nchan[0] = 0; \
    b08 += 4; \
  }


#define CCCLOSE \
{ \
  unsigned int padding; \
  dabufp = (unsigned int *) ( ( ((unsigned int)b08+3)/4 ) * 4); \
  padding = (unsigned int)dabufp - (unsigned int)b08; \
  /*dabufp_save1[1] |= (padding&0x3)<<14;*/ \
  dabufp_save2[1] |= (padding&0x3)<<14; \
  /*printf("CCCLOSE: 0x%x %d --- 0x%x %d --> padding %d\n",dabufp,dabufp,b08,b08,((dabufp_save2[1])>>14)&0x3);*/ \
  *dabufp_save1 = (dabufp-dabufp_save1-1); \
  *dabufp_save2 = (dabufp-dabufp_save2-1); \
  len += (*dabufp_save1+1); \
  b08 = NULL; \
}




#define CPOPEN(btag,btyp,bnum) \
{ \
  dabufp_save1 = dabufp ++; /*remember beginning of the bank address*/ \
  *dabufp++ = (btag<<16) + (btyp<<8) + bnum; /*bank header*/ \
  b08 = (unsigned char *)dabufp; \
}


#define CPCLOSE \
{ \
  unsigned int padding; \
  dabufp = (unsigned int *) ( ( ((unsigned int)b08+3)/4 ) * 4); \
  padding = (unsigned int)dabufp - (unsigned int)b08; \
  dabufp_save1[1] |= (padding&0x3)<<14; \
  /*printf("CPCLOSE: 0x%x %d --- 0x%x %d --> padding %d\n",dabufp,dabufp,b08,b08,((dabufp_save2[1])>>14)&0x3);*/ \
  *dabufp_save1 = (dabufp-dabufp_save1-1); \
  len += (*dabufp_save1+1); \
  b08 = NULL; \
}









#ifdef VXWORKS
int mynev;
#else
extern int mynev; /*defined in tttrans.c */
#endif


static char* Version = " VERSION: ttvme.c v1.0 - initial release " ;
static char* CompileTime = DAYTIME;

/***********************************************************************************

  Routine             : TT_TranslateFADCBank

  Parameters          : bufin - input(CODA from ROL1), bufout - output(EVIO)

  Discription         : This routine translates one event from crate with FADC250s
                        
***********************************************************************************/

 /* 22-max# of blocks=slots, 256-max# of events*/
#define MAXBLOCK 22
#define MAXEVENT 256

#ifdef DO_PEDS
#define NPEDEVENTS 100
#define NSLOTS 22
#define NCHANS 16
static int npeds[NSLOTS][NCHANS];
static float pedval[NSLOTS][NCHANS];
static float pedrms[NSLOTS][NCHANS];
static FILE *fd = NULL;
static char *dir = NULL;
static char *expid = NULL;
#endif



int
TT_TranslateFADCBank(long *bufin, long *bufout, TTSPtr ttp)
{
  int nB, iB[MAXBLOCK], sB[MAXBLOCK];
  int nE[MAXBLOCK], iE[MAXBLOCK][MAXEVENT], lenE[MAXBLOCK][MAXEVENT];
  unsigned int *iw, *adc, *ssp, *endofbufout;
  int i, j, k, m, iii, ind, len, bank, nhits=0, mux_index, rlen, printing, nnE;
  int nr = 0;
  int ncol = 2;
  int a_channel, a_nevents, a_blocknumber, a_triggernumber;
  int a_windowwidth, a_pulsenumber, a_firstsample;
  int a_adc1, a_adc2, a_valid1, a_valid2, a_nwords, a_slot, a_slot2;
  int a_qualityfactor, a_pulseintegral, a_pulsetime, a_vm, a_vp;
  int a_trigtime[4];
  int a_slot_old; 
  int a_channel_old; 
  int npedsamples;

  unsigned int *dabufp_save1, *dabufp_save2;
  unsigned int *StartOfBank;
  unsigned int *dabufp;
  char *ch;
  unsigned char *b08;
  unsigned short *b16;
  unsigned int *b32, *Nchan, *Npuls, *Nsamp;
  unsigned long long *b64;
  int islot, ichan, ii, jj, npulses;
  int banknum = 0;
#ifdef DO_PEDS
  char fname[1024];
#endif
  /*
  return(0);
  */

  if(ttp->roc==1) banknum=0;
  else if(ttp->roc==7) banknum=7;
  else if(ttp->roc==8) banknum=8;
  else if(ttp->roc==27) banknum=10;
  else
  {
    printf("ttfa: UNDESCRIBED ROCID = %d - DO NOTHING !!!\n",ttp->roc);
    return(0);
  }

  /* input info */
  rlen = bufin[0]; /* the number of 32bit data words */
  adc = (unsigned int *)&bufin[1]; /* first data word */

#ifndef VXWORKS
  /* swap input buffer (assume that data from VME is big-endian, and we are on little-endian Intel) */
  for(ii=0; ii<rlen; ii++) adc[ii] = LSWAP(adc[ii]);
#endif





#ifdef HAVE_SSP_DATA
  /*assume first NSSP words are ssp fifo*/
  if(ttp->roc==1)
  {
    ssp = adc;
    for(ii=0; ii<NSSP; ii++) ssp[ii] = LSWAP(ssp[ii]); /*swap it back, it came in correct endian from rol1*/
    adc += NSSP;
    rlen -= NSSP;
  }
#endif




#ifdef DEBUG
  printf("\n**********************\nmini-bos bank: rlen=%d\n",rlen);
#endif



/*******************************************************/
/* FIRST PASS: check data; fill nB, iB[22], nE and iE[22][256] */

#ifdef DEBUG
  printf("\nFIRST PASS\n\n");
#endif

  ii=0;
  printing=1;
  nB=0; /*cleanup block counter*/
  while(ii<rlen)
  {

    if( ((adc[ii]>>27)&0x1F) == 0x10) /*block header*/
    {
/*A. Celentano 7/30/2014*/
      a_slot = ((adc[ii]>>22)&0x1F);
      a_nevents = ((adc[ii])&0xFF);
      a_blocknumber = ((adc[ii]>>8)&0x3FF);
#ifdef DEBUG
	  printf("[%3d] BLOCK HEADER: slot %d, nevents %d, block number %d\n",ii,
				   a_slot,a_nevents,a_blocknumber);
      printf(">>> update iB and nB\n");
#endif
      nB++;            /*increment block counter*/
      iB[nB-1]=ii;     /*remember block start index*/
      sB[nB-1]=a_slot; /*remember slot number*/
      nE[nB-1]=0;      /*cleanup event counter in current block*/

	  ii++;
    }
    else if( ((adc[ii]>>27)&0x1F) == 0x11) /*block trailer*/
    {
      a_slot2 = ((adc[ii]>>22)&0x1F);
      a_nwords = (adc[ii]&0x3FFFFF);
#ifdef DEBUG
	  printf("[%3d] BLOCK TRAILER: slot %d, nwords %d\n",ii,
				   a_slot2,a_nwords);
      printf(">>> data check\n");
#endif

      /*"close" previous event if any*/
      k = nB-1; /*current block index*/
      if(nE[k] > 0)
	  {
        m = nE[k]-1; /*current event number*/
        lenE[k][m] = ii-iE[k][m]; /*#words in current event*/
	  }     

      if(a_slot2 != a_slot)
	  {
        if(printing)
        {
          printf("[%3d] ERROR1 in FADC data: blockheader slot %d != blocktrailer slot %d\n",
				 ii,a_slot,a_slot2);
          printing=0;
	    }
	  }
      if(a_nwords != (ii-iB[nB-1]+1))
      {
        if(printing)
        {
          printf("[%3d] ERROR2 in FADC data: trailer #words %d != actual #words %d\n",
				 ii,a_nwords,ii-iB[nB-1]+1);
          printing=0;
	    }
      }
	  ii++;
    }
    else if( ((adc[ii]>>27)&0x1F) == 0x12) /*event header*/
    {
      a_triggernumber = (adc[ii]&0x3FFFFF);
#ifdef DEBUG
	  printf("[%3d] EVENT HEADER: trigger number %d\n",ii,
				   a_triggernumber);
      printf(">>> update iE and nE\n");
#endif

      /*"close" previous event if any*/
      k = nB-1; /*current block index*/
      if(nE[k] > 0)
	  {
        m = nE[k]-1; /*current event number*/
        lenE[k][m] = ii-iE[k][m]; /*#words in current event*/
	  }     

      /*"open" next event*/
      nE[k]++; /*increment event counter in current block*/
      m = nE[k]-1; /*current event number*/
      iE[k][m]=ii; /*remember event start index*/

	  ii++;
    }
    else if( ((adc[ii]>>27)&0x1F) == 0x13) /*trigger time: remember timestamp*/
    {
      a_trigtime[0] = (adc[ii]&0xFFFFFF);
#ifdef DEBUG
	  printf("[%3d] TRIGGER TIME: 0x%06x\n",ii,
			   a_trigtime[0]);
#endif
	  ii++;
      iii=1;
      while( ((adc[ii]>>31)&0x1) == 0 && ii<rlen ) /*must be one more word*/
	  {
        a_trigtime[iii] = (adc[ii]&0xFFFFFF);
#ifdef DEBUG
        printf("   [%3d] TRIGGER TIME: 0x%06x\n",ii,
          a_trigtime[iii]);
        printf(">>> remember timestamp 0x%06x 0x%06x\n",a_trigtime[0],a_trigtime[1]);
#endif
        iii++;
        ii++;
	  }
    }
    else if( ((adc[ii]>>27)&0x1F) == 0x14) /*window raw data: remember channel#*/
    {
      a_channel = ((adc[ii]>>23)&0xF);
      a_windowwidth = (adc[ii]&0xFFF);
#ifdef DEBUG
	  printf("[%3d] WINDOW RAW DATA: channel %d, window width %d\n",ii,
			   a_channel,a_windowwidth);
      printf(">>> remember channel %d\n",a_channel);
#endif

	  ii++;
      while( ((adc[ii]>>31)&0x1) == 0 && ii<rlen ) /*loop over all samples*/
	  {
        a_valid1 = ((adc[ii]>>29)&0x1);
        a_adc1 = ((adc[ii]>>16)&0xFFF);
        a_valid2 = ((adc[ii]>>13)&0x1);
        a_adc2 = (adc[ii]&0xFFF);
#ifdef DEBUG
        printf("   [%3d] WINDOW RAW DATA: valid1 %d, adc1 0x%04x, valid2 %d, adc2 0x%04x\n",ii,
          a_valid1,a_adc1,a_valid2,a_adc2);
#endif

        ii++;
	  }
    }
    else if( ((adc[ii]>>27)&0x1F) == 0x15) /*window sum: obsolete*/
    {
#ifdef DEBUG
	  printf("[%3d] WINDOW SUM: must be obsolete\n",ii);
#endif
	  ii++;
    }
    else if( ((adc[ii]>>27)&0x1F) == 0x16) /*pulse raw data*/
    {
      a_channel = ((adc[ii]>>23)&0xF);
      a_pulsenumber = ((adc[ii]>>21)&0x3);
      a_firstsample = (adc[ii]&0x3FF);
#ifdef DEBUG
	  printf("[%3d] PULSE RAW DATA: channel %d, pulse number %d, first sample %d\n",ii,
			   a_channel,a_pulsenumber,a_firstsample);
#endif

	  ii++;

      while( ((adc[ii]>>31)&0x1) == 0)
	  {
        a_valid1 = ((adc[ii]>>29)&0x1);
        a_adc1 = ((adc[ii]>>16)&0xFFF);
        a_valid2 = ((adc[ii]>>13)&0x1);
        a_adc2 = (adc[ii]&0xFFF);
#ifdef DEBUG
        printf("   [%3d] PULSE RAW DATA: valid1 %d, adc1 0x%04x, valid2 %d, adc2 0x%04x\n",ii,
          a_valid1,a_adc1,a_valid2,a_adc2);
#endif

        ii++;
	  }

    }
    else if( ((adc[ii]>>27)&0x1F) == 0x17) /*pulse integral: assume that 'pulse time' was received ! */
    {
      a_channel = ((adc[ii]>>23)&0xF);
      a_pulsenumber = ((adc[ii]>>21)&0x3);
      a_qualityfactor = ((adc[ii]>>19)&0x3);
      a_pulseintegral = (adc[ii]&0x7FFFF);
#ifdef DEBUG
	  printf("[%3d] PULSE INTEGRAL: channel %d, pulse number %d, quality %d, integral %d\n",ii,
			   a_channel,a_pulsenumber,a_qualityfactor,a_pulseintegral);
#endif

	  ii++;
    }




    else if( ((adc[ii]>>27)&0x1F) == 0x18) /* pulse time */
    {
      a_channel = ((adc[ii]>>23)&0xF);
      a_pulsenumber = ((adc[ii]>>21)&0x3);
      a_qualityfactor = ((adc[ii]>>19)&0x3);
      a_pulsetime = (adc[ii]&0xFFFF);
#ifdef DEBUG
	  printf("[%3d] PULSE TIME: channel %d, pulse number %d, quality %d, time %d\n",ii,
			 a_channel,a_pulsenumber,a_qualityfactor,a_pulsetime);
#endif

	  ii++;
    }



    else if( ((adc[ii]>>27)&0x1F) == 0x19)
    {
#ifdef DEBUG
	  printf("[%3d] STREAMING RAW DATA: \n",ii);
#endif
	  ii++;
    }



    else if( ((adc[ii]>>27)&0x1F) == 0x1a) /* Vm Vp */
    {
      a_channel = ((adc[ii]>>23)&0xF);
      a_pulsenumber = ((adc[ii]>>21)&0x3);
      a_vm = ((adc[ii]>>12)&0x1FF);
      a_vp = (adc[ii]&0xFFF);
#ifdef DEBUG
	  printf("[%3d] PULSE VmVp: channel %d, pulse number %d, Vm %d, Vp %d\n",ii,
			 a_channel,a_pulsenumber,a_vm,a_vp);
#endif
	  ii++;
    }



    else if( ((adc[ii]>>27)&0x1F) == 0x1D)
    {
#ifdef DEBUG
	  printf("[%3d] EVENT TRAILER: \n",ii);
#endif
	  ii++;
    }
    else if( ((adc[ii]>>27)&0x1F) == 0x1E)
    {
	  printf("[%3d] : DATA NOT VALID\n",ii);
exit(0);
	  ii++;
    }
    else if( ((adc[ii]>>27)&0x1F) == 0x1F)
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
        printf("[%3d] ERROR: in FADC data format 0x%08x (bits31-27=0x%02x)\n",
			   ii,(int)adc[ii],(adc[ii]>>27)&0x1F);
        printing=0;
	  }
      ii++;
    }

  } /* loop over 'rlen' words */



  /*check if the number of events in every block is the same*/
  nnE = nE[0];
  for(k=1; k<nB; k++)
  {
    if(nE[k]!=nnE)
	{
      if(printing)
      {
        printf("SEVERE ERROR: different event number in difefrent blocks\n");
        printing=0;
        return(0);
	  }      
	}
  }


#ifdef DEBUG
  printf("\n=================\n");
  for(k=0; k<nB; k++)
  {
    printf("Block %2d, block index %2d\n",k,iB[k]);
    for(m=0; m<nnE; m++)
	{
      printf("Event %2d, event index %2d, event lenght %2d\n",m,iE[k][m],lenE[k][m]);
    }
  }
  printf("\n=================\n");
#endif







  /********************************************************/
  /* SECOND PASS: disantangling and filling output buffer */

#ifdef DEBUG
  printf("\nSECOND PASS\n\n");
#endif


  /* set pointer to the 3rd word of the output buffer (first 2 words will be CODA header filles by rol2) */
  dabufp = (unsigned int *)&bufout[2];

  /* set BOS pointer to output buffer */
  iw = (unsigned int *)&bufout[2];

  len = 0;
  b08 = NULL;
  /* set end_of_bufout pointer */
  endofbufout = (unsigned int *)(bufout + bosleftspace(iw));






#ifdef HAVE_SSP_DATA
  /*assume first NSSP words are ssp fifo*/
  if(ttp->roc==1)
  {
    CPOPEN(0xe106,1,1);
    for(ii=0; ii<NSSP; ii++)
    {
      dabufp[ii] = ssp[ii];
      b08 += 4;
    }
	/*
    printf("rol2: 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x\n",
		 dabufp[0],dabufp[1],dabufp[2],dabufp[3],dabufp[4],dabufp[5],dabufp[6]);
	*/
    CPCLOSE;
  }
#endif




  ii=0;
  printing=1;
  /*
  while(ii<rlen)
  */

for(m=0; m<nnE; m++) /*loop over events*/
{
  a_slot_old = -1;
  a_channel_old = -1;

  if(b08 != NULL)
  {
    CCCLOSE; /*call CCCLOSE only if CCOPEN was called*/
#ifdef DEBUG
    printf("0x%08x: CCCLOSE1, dabufp=0x%08x\n",b08,dabufp);
    printf("0x%08x: CCCLOSE2, dabufp=0x%08x\n",b08,(unsigned int *) ( ( ((unsigned int)b08+3)/4 ) * 4));
    printf("-> 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x\n",
		 (unsigned int)b08+3,((unsigned int)b08+3),
         ((unsigned int)b08+3) / 4,(((unsigned int)b08+3) / 4)*4, 
		 (unsigned int *)((((unsigned int)b08+3) / 4)*4) );
#endif
  }

  for(k=0; k<nB; k++) /*loop over blocks*/
  {
#ifdef DEBUG
    printf("Block %d, Event %2d, event index %2d, event lenght %2d\n",k, m,iE[k][m],lenE[k][m]);
#endif
    a_slot = sB[k];
    ii = iE[k][m];
    rlen = ii + lenE[k][m];
    while(ii<rlen)
    {
      if( ((adc[ii]>>27)&0x1F) == 0x12) /*event header: remember trigger#*/
      {
        a_triggernumber = (adc[ii]&0x3FFFFF);
#ifdef DEBUG
	    printf("[%3d] EVENT HEADER: trigger number %d\n",ii,
				   a_triggernumber);
        printf(">>> remember trigger %d\n",a_triggernumber);
#endif
	    ii++;
      }
      else if( ((adc[ii]>>27)&0x1F) == 0x13) /*trigger time: remember timestamp*/
      {
        a_trigtime[0] = (adc[ii]&0xFFFFFF);
#ifdef DEBUG
	    printf("[%3d] TRIGGER TIME: 0x%06x\n",ii,
			   a_trigtime[0]);
#endif
	    ii++;
        iii=1;
        while( ((adc[ii]>>31)&0x1) == 0 && ii<rlen ) /*must be one more word*/
	    {
          a_trigtime[iii] = (adc[ii]&0xFFFFFF);
#ifdef DEBUG
          printf("   [%3d] TRIGGER TIME: 0x%06x\n",ii,a_trigtime[iii]);
          printf(">>> remember timestamp 0x%06x 0x%06x\n",a_trigtime[0],a_trigtime[1]);
#endif
          iii++;
          ii++;
	    }
      }
      else if( ((adc[ii]>>27)&0x1F) == 0x14) /*window raw data: remember channel#*/
      {
        a_channel = ((adc[ii]>>23)&0xF);
        a_windowwidth = (adc[ii]&0xFFF);
#ifdef DEBUG
	    printf("[%3d] WINDOW RAW DATA: channel %d, window width %d\n",ii,
			   a_channel,a_windowwidth);
        printf(">>> remember channel %d\n",a_channel);
#endif

	    CCOPEN(0xe101,"c,i,l,N(c,Ns)",banknum);
#ifdef DEBUG
        printf("0x%08x: CCOPEN(1)\n",b08);
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
          sprintf(fname,"%s/parms/peds/%s/roc%02d.ped",dir,expid,ttp->roc);
          fd = fopen(fname,"w");
          if(fd==NULL)
          {
            printf("ttfa: ERROR: cannot open pedestal file >%s<\n",fname);
		  }
          else
	      {
            printf("ttfa: pedestal file >%s< is opened for writing\n",fname);
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
        else if(mynev==110 && fd!=NULL)
	    {
          printf("mynev=%d - closing pedestal file (Nmeasures=%d (%d %d ..))\n",
            mynev,npeds[3][0],npeds[3][1],npeds[4][0]);
          for(i=0; i<NSLOTS; i++)
	      {
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
			  }
		    }
	      }
          fclose(fd);
          printf("ttfa: pedestal file >%s< is closed\n",fname);
          fd = NULL;
	    }
#endif

	    Nchan[0] ++; /* increment channel counter */

        *b08++ = a_channel; /* channel number */

#ifdef DEBUG
        printf("0x%08x: #samples %d\n",b08,a_windowwidth);
#endif
        b32 = (unsigned int *)b08;
        *b32 = a_windowwidth; /* the number of samples (same as window width) */
        b08 += 4;

	    ii++;
        npedsamples = a_windowwidth / 10; /* use first 10% of the window to measure pedestal */
        while( ((adc[ii]>>31)&0x1) == 0 && ii<rlen ) /*loop over all samples*/
	    {
          a_valid1 = ((adc[ii]>>29)&0x1);
          a_adc1 = ((adc[ii]>>16)&0xFFF);
          a_valid2 = ((adc[ii]>>13)&0x1);
          a_adc2 = (adc[ii]&0xFFF);
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
      else if( ((adc[ii]>>27)&0x1F) == 0x15) /*window sum: obsolete*/
      {
#ifdef DEBUG
	    printf("[%3d] WINDOW SUM: must be obsolete\n",ii);
#endif
	    ii++;
      }
      else if( ((adc[ii]>>27)&0x1F) == 0x16) /*pulse raw data*/
      {
        a_channel = ((adc[ii]>>23)&0xF);
        a_pulsenumber = ((adc[ii]>>21)&0x3);
        a_firstsample = (adc[ii]&0x3FF);
#ifdef DEBUG
	    printf("[%3d] PULSE RAW DATA: channel %d, pulse number %d, first sample %d\n",ii,
			   a_channel,a_pulsenumber,a_firstsample);
#endif

        CCOPEN(0xe102,"c,i,l,N(c,N(c,Ns))",banknum);
#ifdef DEBUG
        printf("0x%08x: CCOPEN(2)\n",b08);
#endif

        if(a_channel != a_channel_old)
        {
          a_channel_old = a_channel;

          Nchan[0] ++; /* increment channel counter */

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

        while( ((adc[ii]>>31)&0x1) == 0)
	    {
          a_valid1 = ((adc[ii]>>29)&0x1);
          a_adc1 = ((adc[ii]>>16)&0xFFF);
          a_valid2 = ((adc[ii]>>13)&0x1);
          a_adc2 = (adc[ii]&0xFFF);
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
      else if( ((adc[ii]>>27)&0x1F) == 0x17) /*pulse integral: assume that 'pulse time' was received ! */
      {
        a_channel = ((adc[ii]>>23)&0xF);
        a_pulsenumber = ((adc[ii]>>21)&0x3);
        a_qualityfactor = ((adc[ii]>>19)&0x3);
        a_pulseintegral = (adc[ii]&0x7FFFF);
#ifdef DEBUG
	    printf("[%3d] PULSE INTEGRAL: channel %d, pulse number %d, quality %d, integral %d\n",ii,
			   a_channel,a_pulsenumber,a_qualityfactor,a_pulseintegral);
#endif


#ifdef DEBUG
        printf("0x%08x: pulseintegral = %d\n",b08,a_pulseintegral);
#endif
        b32 = (unsigned int *)b08;
        *b32 = a_pulseintegral;
        b08 += 4;


	    ii++;
      }




      else if( ((adc[ii]>>27)&0x1F) == 0x18) /* pulse time */
      {
        a_channel = ((adc[ii]>>23)&0xF);
        a_pulsenumber = ((adc[ii]>>21)&0x3);
        a_qualityfactor = ((adc[ii]>>19)&0x3);
        a_pulsetime = (adc[ii]&0xFFFF);
#ifdef DEBUG
	    printf("[%3d] PULSE TIME: channel %d, pulse number %d, quality %d, time %d\n",ii,
			 a_channel,a_pulsenumber,a_qualityfactor,a_pulsetime);
#endif



        CCOPEN(0xe103,"c,i,l,N(c,N(s,i))",banknum);
#ifdef DEBUG
        printf("0x%08x: CCOPEN(3), dabufp=0x%08x\n",b08,dabufp);
#endif


        if(a_channel != a_channel_old)
        {
          a_channel_old = a_channel;

          Nchan[0] ++; /* increment channel counter */

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

        Npuls[0] ++;

#ifdef DEBUG
        printf("0x%08x: pulsetime = %d\n",b08,a_pulsetime);
#endif
        b16 = (unsigned short *)b08;
        *b16 = a_pulsetime;
        b08 += 2;

	    ii++;
      }


      else if( ((adc[ii]>>27)&0x1F) == 0x19)
      {
#ifdef DEBUG
	    printf("[%3d] STREAMING RAW DATA: \n",ii);
#endif
	    ii++;
      }
      else if( ((adc[ii]>>27)&0x1F) == 0x1a) /* Vm Vp */
      {
        a_channel = ((adc[ii]>>23)&0xF);
        a_pulsenumber = ((adc[ii]>>21)&0x3);
        a_vm = ((adc[ii]>>12)&0x1FF);
        a_vp = (adc[ii]&0xFFF);
#ifdef DEBUG
	    printf("[%3d] PULSE VmVp: channel %d, pulse number %d, Vm %d, Vp %d\n",ii,
			 a_channel,a_pulsenumber,a_vm,a_vp);
#endif
	    ii++;
      }



      else if( ((adc[ii]>>27)&0x1F) == 0x1D)
      {
#ifdef DEBUG
	    printf("[%3d] EVENT TRAILER: \n",ii);
#endif
	    ii++;
      }
      else if( ((adc[ii]>>27)&0x1F) == 0x1E)
      {
	    printf("[%3d] : DATA NOT VALID\n",ii);
exit(0);
	    ii++;
      }
      else if( ((adc[ii]>>27)&0x1F) == 0x1F)
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
          printf("[%3d] ERROR: in FADC data format 0x%08x (bits31-27=0x%02x)\n",
			   ii,(int)adc[ii],(adc[ii]>>27)&0x1F);
          printing=0;
	    }
        ii++;
      }

    } /*while*/

  } /* loop over blocks */

} /* loop over events */



  if(b08 != NULL) CCCLOSE; /*call CCCLOSE only if CCOPEN was called*/
#ifdef DEBUG
  printf("0x%08x: CCCLOSE, dabufp=0x%08x\n",b08,dabufp);
  dabufp = (unsigned int *) ( ( ((unsigned int)b08+3)/4 ) * 4);
  printf("0x%08x: CCCLOSE, dabufp=0x%08x\n",b08,dabufp);
  printf("-> 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x\n",
		 (unsigned int)b08+3,((unsigned int)b08+3),
         ((unsigned int)b08+3) / 4,(((unsigned int)b08+3) / 4)*4, 
		 (unsigned int *)((((unsigned int)b08+3) / 4)*4) );
#endif

  /* returns full fragment length (long words) */  

#ifdef DEBUG 
  printf("return len=%d\n**********************\n\n",len);
#endif

  return(len);
}






