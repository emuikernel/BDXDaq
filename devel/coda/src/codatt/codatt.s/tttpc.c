
/* tttcp.c */


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "coda.h"
#include "tt.h"

static int diagcount=0;
static char* Version = " VERSION: tttage.c v1.0 - initial release " ;
static char* CompileTime = DAYTIME;

/* crate, slot, channel */
static int ttpad[6][23][128];
static int ttbanknum[6][23][128];

static int printcount;
static int printprescale = 1000;

int
rorcReadTable(int rocid)
{
  FILE *fd;
  int i, ret;
  char tmp[256];
  char bankname[10];
  char notinuse[128];
  int banknum, pad, crate, slot, channel;

  for(crate=0; crate<6; crate++)
  {
    for(slot=0; slot<23; slot++)
	{
      for(channel=0; channel<128; channel++)
	  {
        ttpad[crate][slot][channel] = -1;
        ttbanknum[crate][slot][channel] = 0;
	  }
	}
  }

  if(rocid==29)
  {
    fd = fopen("/usr/local/clas/parms/TT/TPC0.tab","r");
  }
  else if(rocid==31)
  {
    fd = fopen("/usr/local/clas/parms/TT/TPC1.tab","r");
  }
  else if(rocid==0)
  {
    fd = fopen("/usr/local/clas/parms/TT/TPC2.tab","r");
  }
  else if(rocid==28)
  {
    fd = fopen("/usr/local/clas/parms/TT/TPC3.tab","r");
  }
  else if(rocid==30) /*croctest5 (test setup)*/
  {
    fd = fopen("/usr/local/clas/parms/TT/TPC5.tab","r");
  }
  else
  {
    printf("ERROR (-1) in rorcReadTable: unknown rocid=%d\n",rocid);
    return(-1);
  }

  if(fd==NULL)
  {
    printf("ERROR (-2) in rorcReadTable\n");
    return(-2);
  }

  while(fgets(tmp,255,fd)!=NULL)
  {
    if(tmp[0]=='#')
    {
      printf("comment: %s",tmp);fflush(stdout);
      continue; /* skip comments */
	}
    ret = sscanf(tmp,"%s %d %d %d %d %d %s",
      bankname,&banknum,&pad,&crate,&slot,&channel,notinuse);
    if(ret != 7)
    {
      printf("ERROR in TPC table: ret=%d, tmp >%s<\n",ret,tmp);
	}
    else
	{
      /* fill the lookup table */
	  
      printf("ret=%d: >%s< %d %d %d %d %d >%s<\n",
        ret,bankname,banknum,pad,crate,slot,channel,notinuse);
	  
      for(i=0; i<16; i++)
	  {
        ttpad[crate][slot][channel+i] = pad+i;
        ttbanknum[crate][slot][channel+i] = banknum;
	  }
	}
  }

  fclose(fd);

  /* print whole table */
  printf("\n TPC translation table\n");
  i = 0;
  for(crate=0; crate<6; crate++)
  {
    for(slot=0; slot<23; slot++)
	{
      printf(" line#   crate   slot   channel  ->   banknum   pad\n");
      for(channel=0; channel<128; channel++)
	  {
        if(ttpad[crate][slot][channel] != -1)
		{
          printf("[%5d]   %2d      %2d      %3d            %1d  %6d\n",i,crate,slot,channel,
            ttbanknum[crate][slot][channel],ttpad[crate][slot][channel]);
          i++;
		}
	  }
	}
  }

  return(0);
}


#define NBINS 100
#define TIME_BIN_WIDTH 100 /* time bin width in nanosec */
#define NWCHANNEL  200000 /* must be bigger then # of bins + time + .. */

int
rorcDecodeEvent(int nwords, unsigned int *bufin,
                int *itpc, unsigned short tpc6[NWCHANNEL], int printflag, int rocid)
{
  int i, j, k, ii, jj;
  FILE *fd;
  int nsamples, adc, tdc;
  int crate, slot, channel, ntpc, maxtpcs;
  int goodtostore;

  float ww=1.;
  float tmpx;
  int idn;

  int ntmp;
  unsigned short tmp[NBINS+2];

  int type;
  unsigned int *b32;

  maxtpcs = *itpc;

  if(printflag)
  {
    if(rocid==28) fd = fopen("/home/clasrun/tpc0.out","w");
    if(rocid==31) fd = fopen("/home/clasrun/tpc1.out","w");
    if(rocid==0)  fd = fopen("/home/clasrun/tpc2.out","w");
    if(rocid==28) fd = fopen("/home/clasrun/tpc3.out","w");
    if(rocid==30) fd = fopen("/tmp/tpc5.out","w");
	/*fd = stdout;*/
  }

  /* process buffer */

  /* set crate number as expected */
  if(rocid==29) crate = 0;
  else if(rocid==31) crate = 1;
  else if(rocid==0) crate = 2;
  else if(rocid==28) crate = 3;
  else if(rocid==30) crate = 5;
  else printf("error in tttpc: rocid=%d\n",rocid);

  ntpc = 0;
  for(i=0; i<nwords; i++)
  {
    type = (bufin[i]>>30)&0x3;

    if(bufin[i]==0xffffffff) /*global header (8 words)*/
	{
      if(printflag) fprintf(fd,"[%5d] 0x%08x -> global header [0]\n",i,bufin[i]);
      if(printflag) fprintf(fd,"[%5d] 0x%08x -> global header [1]\n",i+1,bufin[i+1]);
      if(printflag) fprintf(fd,"[%5d] 0x%08x -> global header [2] (event %d)\n",i+2,bufin[i+2],bufin[i+2]&0xffffff);
      if(printflag) fprintf(fd,"[%5d] 0x%08x -> global header [3]\n",i+3,bufin[i+3]);
      if(printflag) fprintf(fd,"[%5d] 0x%08x -> global header [4]\n",i+4,bufin[i+4]);
      if(printflag) fprintf(fd,"[%5d] 0x%08x -> global header [5]\n",i+5,bufin[i+5]);
      if(printflag) fprintf(fd,"[%5d] 0x%08x -> global header [6]\n",i+6,bufin[i+6]);
      if(printflag) fprintf(fd,"[%5d] 0x%08x -> global header [7]\n",  i+7,bufin[i+7]);
      i += 7;
      k = 0;
	}
    else if(type==0) /*ALTRO channel payload ([29:20] [19:10] [9:0])*/
	{
      /* print all payload */
      if(printflag) fprintf(fd,"  from channel header: nsamples=%d (nwords=%d)\n",nsamples,(nsamples+2)/3);
      ntmp = 0;
      for(ii=0; ii<(nsamples+2)/3; ii++)
	  {
        if(printflag) fprintf(fd,"[%5d] 0x%08x -> payload: [%3d] %4d    [%3d] %4d    [%3d] %4d\n",i,bufin[i],
          j,(bufin[i]>>20)&0x3ff,j+1,(bufin[i]>>10)&0x3ff,j+2,bufin[i]&0x3ff);
        tmp[ntmp++] = (bufin[i]>>20)&0x3ff;
        tmp[ntmp++] = (bufin[i]>>10)&0x3ff;
        tmp[ntmp++] = bufin[i]&0x3ff;
        j+=3;
        k++;
        i++;
  	  }
      i--;


      /* decode and store */
      ii=0;
      while((ii+2)<ntmp) /* if difference between ii and ntmp less then 3, only trailing zeroes remains - skip them */ 
      {
		/*
        fprintf(fd,"------> ii=%d ntmp=%d\n",ii,ntmp);
		*/
        nsamples = tmp[ii++];
        tdc = tmp[ii++];
        if(printflag) fprintf(fd,"     Nsamples=%d, tdc=%d\n",nsamples,tdc);

        /*check if remaining space enough to store entire cluster*/
		if( (ntpc+nsamples+1)<maxtpcs )
        {
          goodtostore = 1;
		}
        else
		{
          goodtostore = 0;
          /*printf("no space - cut data\n")*/;
	    }

		if(goodtostore)
        {
          if(ttpad[crate][slot][channel] == -1)
		  {
            goodtostore = 0;
            if(printcount%printprescale)
			{
              printf("tttpc ERROR: crate=%d slot=%d channel=%d does not described - skip\n",
					 crate, slot, channel);
              return(-1);
              /*
              printcount ++;
              */
			}
		  }
          else
		  {
            tpc6[ntpc++] = ttpad[crate][slot][channel]&0xffff; /*channel id*/
            tpc6[ntpc++] = tdc; /*bin number for the first-in-the-list ADC*/
            tpc6[ntpc++] = nsamples-2; /* the number of ADCs */
		  }
		}

        for(jj=0; jj<(nsamples-2); jj++)
        {
          adc = tmp[ii++];
          if(goodtostore) tpc6[ntpc++] = adc;
          if(printflag) fprintf(fd,"         [%3d] adc=%d\n",jj,adc);
		}

      }


	}
    else if(type==1) /*ALTRO channel header (block length=[25:16] channel address=[11:0])*/
	{
      slot = (bufin[i]>>7)&0x1f;
      channel = bufin[i]&0x7f;
      nsamples = (bufin[i]>>16)&0x3ff;
      if(printflag) fprintf(fd,"[%5d] 0x%08x -> channel header: slot %2d, channel %3d, # of 10bit samples=%6d\n",
        i,bufin[i],slot,channel,nsamples);
      j=0;
      k++;
	}
    else if(type==2) /*RCU trailer word*/
	{
      if(printflag) fprintf(fd,"[%5d] 0x%08x -> trailer word: 0x%08x (possible len = %d, k=%d)\n",i,bufin[i],bufin[i],bufin[i]&0xffffff,k);
	}
    else if(type==3) /*last RCU trailer word*/
	{
      if(printflag) fprintf(fd,"[%5d] 0x%08x -> last trailer word: 0x%08x (%d)\n",i,bufin[i],bufin[i],bufin[i]);
	}
    else
	{
      if(printflag) fprintf(fd,"ERRORRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRR !!!\n");
      exit(0);
	}

  }


  /* print resulting array */
  if(printflag)
  {
    fprintf(fd,"\nTPC6 bank: ntpc=%d\n",ntpc);
	ii = 0;
    while(ii<ntpc)
    {
      fprintf(fd," [%6d] channelid=%d tdc=%d nadcs=%d\n",ii,tpc6[ii],tpc6[ii+1],tpc6[ii+2]);
      for(jj=0; jj<tpc6[ii+2]; jj++) fprintf(fd,"adc[%3d]=%d\n",jj,tpc6[ii+3+jj]);
      ii += (tpc6[ii+2] + 3);
	}
  }

  if(printflag) fclose(fd);

  *itpc = ntpc;
  return(ntpc);
}




/******************************************************************************

  Routine             : TT_TranslateTPCBank

  Parameters          : bufin - input(CODA) fragment, bufout - output(BOS)

  Discription         : This routine translates one fragment from CODA to
                        BOS format for TPC only 
                        
******************************************************************************/

int
TT_TranslateTPCBank(long *bufin, long *bufout, TTSPtr ttp)
{
  unsigned long *iw, *in;
  unsigned short *out;
  int nw, ret, i, j, k, ind, len;
  int ntpc;
  unsigned short tpc6[NWCHANNEL];

  nw = bufin[0]; /* raw bank datalength; actual data starts from &bufin[1] */
  in = &bufin[1];
  iw = (unsigned long *)&bufout[2]; /* set BOS pointer to output buffer */
  len = 0;


  /*
  printf("rol2: len=%d\n",nw);
  printf("rol2: 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x\n",
		 bufin[1],bufin[2],bufin[3],bufin[4],bufin[5],bufin[6]);
  printf("      0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x\n",
		 bufin[7],bufin[8],bufin[9],bufin[10],bufin[11],bufin[12]);
  */

  ntpc = NWCHANNEL;
  ret = rorcDecodeEvent(nw, &bufin[1], &ntpc, tpc6, 0, ROC);

  if(ntpc>15000)
  {
    /*printf("tttpc: ERROR: ntpc=%d too big, cut it to 15000\n",ntpc);*/
    ntpc = 15000;
  }

  if(ROC==29)
    ind = bosNopen(iw,*((unsigned long *)"TPC6"),0,1,ntpc);
  else if(ROC==31)
    ind = bosNopen(iw,*((unsigned long *)"TPC6"),1,1,ntpc);
  else if(ROC==0)
    ind = bosNopen(iw,*((unsigned long *)"TPC6"),2,1,ntpc);
  else if(ROC==28)
    ind = bosNopen(iw,*((unsigned long *)"TPC6"),3,1,ntpc);
  else if(ROC==30)
    ind = bosNopen(iw,*((unsigned long *)"TPC6"),5,1,ntpc);
  else
    printf("tttpc ERROR: rocid=%d\n",ROC);

  if(ind <= 0)
  {
    printf("tttpc: bosNopen returned %d !!! -> return len=%d\n",ind,len);
    return(len);
  }
  else
  {
    out = (unsigned short *)&iw[ind+1];
    for(i=0; i<ntpc; i++)
    {
      *out++ = tpc6[i];
    }
  }

  len += bosNclose(iw,ind,1,ntpc);


  /* returns full fragment length (long words) */

  return(len);
}


