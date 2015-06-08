
/* tpchist.c */
 
#include <stdio.h>
#include <math.h>
 
#include "bosio.h"

#define NBCS 700000
#include "bcs.h"

#define RADDEG 57.2957795130823209
#define NWPAWC 10000000 /* Length of the PAWC common block. */
#define LREC 1024       /* Record length in machine words. */

struct {
  float hmemor[NWPAWC];
} pawc_;


#define MIN(a,b)          ( (a) < (b) ? (a) : (b) )


unsigned short
swap16(unsigned short word16)
{
  unsigned short res;
  unsigned char tmp;

  res = ((word16>>8)&0xFF) | ((word16&0xFF)<<8);

  return(res);
}

unsigned int
swap32(unsigned int word32)
{
  unsigned int res;
  unsigned char tmp;

  res = ((word32>>24)&0xFF) | (((word32>>16)&0xFF)<<8) | 
       (((word32>>8)&0xFF)<<16) | ((word32&0xFF)<<24);

  return(res);
}


/* decode one event

  inputs:

    rcuDataInBuf - data buffer
    evBegin - index of the first word of event (the one before 0xffffffff)
    evEnd - index of the last word of event (the one after previous 0xffffffff)
      NOTE: we are going backward, so evBegin bigger then evEnd !!!

  outputs:

    *ntpc - the number of records in 'tpc' array
    tpc - output array (will goto TPC bank)

  return value:

    event number, or negative value if error

*/

/*

=================================================
================ Event format ===================

---------------  --------------  -----------------------------
part             # 32-bit words  bit-fields
---------------  --------------  -----------------------------
CDH              8               [31:0]
ALTRO payload    N               [31:30] [29:20] [19:10] [9:0]
                                  (or [31:30] [29:0] for channel headers)
RCU trailer      9               [31:30] [29:0]
---------------  --------------  -----------------------------

Within the ALTRO payload and the RCU trailer there are two MSB  
[31:30] telling you the kind of the 32-bit word:
-------  ---------------------
[31:30]  type
-------  ---------------------
  0  1    ALTRO channel header
  0  0    ALTRO channel payload
  1  0    RCU trailer word
  1  1    last RCU trailer word
-------  ---------------------

A ALTRO channel header looks like:
--------  ------------  --------  ---------------
[29:26]   [25:16]       [15:10]   [11:0]
--------  ------------  --------  ---------------
reserved  block length  reserved  channel address
--------  ------------  --------  ---------------

The order of ALTRO words within a payload section is:
--------  --------  --------
[29:20]   [19:10]   [9:0]
--------  --------  --------
    CW        TS     S_{TS-1}
S_{TS-2}  S_{TS-3}  S_{TS-4}
    ...       ...      ...
    ...      (0x00)  (0x00)
--------  --------  --------
(Where this is of course an example.) The "(0x00)" shall indicate the  
fact, that there is possibly padding necessary.


The decoding strategy is as follows:
- Skip the first 8 words.
- Depending the MSB [31:30] of each following word:
    * "01" decode channel header, set current channel
    * "00" append channel payload to current channel
    * "10" decode RCU trailer word
    * "11" last RCU trailer word, finish

*/


#define NBINS 100
#define TIME_BIN_WIDTH 100 /* time bin width in nanosec */
#define NWCHANNEL  200000 /* must be bigger then # of bins + time + .. */


int
rorcDecodeEvent(int nwords, unsigned int *bufin, int *itpc, unsigned short tpc[3][NWCHANNEL], int printflag)
{
  int iev, evIndex;
  int i, j, k, ii, jj;
  unsigned int data, datal, datar;
  int ich, nw, nwch, n10ch, n10f, nhits;
  int clusternw, clustertime, nclusters;
  unsigned short sd1, sd2;
  FILE *fd;
  int nsamples, adc, tdc;
  int slot, channel, ntpc, maxtpcs;

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
    /*fd = fopen("/home/clasrun/tpc.out","w");*/
    fd = stdout;
  }

  /* process buffer */
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

      /* decode and print */
      ii=0;
      while(ii<ntmp)
      {
        nsamples = tmp[ii++];
        tdc = tmp[ii++];
        if(printflag) fprintf(fd,"     Nsamples=%d, tdc=%d\n",nsamples,tdc);
        for(jj=0; jj<(nsamples-2); jj++)
        {
          int ttt;

          adc = tmp[ii++];
          ttt = tdc-jj;

		  
		  /*if(ttt>40&&ttt<60)*/
		  /*if(adc>70)*/
		  {
            tmpx = ttt;
            ww = ((float)adc)/*10.*/;
            idn = slot*1000+channel;
            hf1_(&idn,&tmpx,&ww);
		  
            tmpx = adc;
            ww = 1.;
            idn = 100000 + slot*1000+channel;
            hf1_(&idn,&tmpx,&ww);
		  }


          if(printflag) fprintf(fd,"         [%3d] adc=%d\n",jj,adc);

		  /*
		  if(ntpc < maxtpcs)
          {
            tpc[0][ntpc] = (slot << 8) + channel;
            tpc[1][ntpc] = (tdc-jj)*TIME_BIN_WIDTH;
            tpc[2][ntpc] = adc;
            ntpc ++;
          }
          else
		  {
            printf("no space - cut data\n");
		  }
		  */

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
  /*
  if(printflag) fprintf(fd,"ntpc=%d\n",ntpc);
  for(ii=0; ii<ntpc; ii++)
  {
    if(printflag) fprintf(fd," [%6d] slot=%d channel=%d tdc=%d adc=%d\n",
            ii,(tpc[0][ii]>>8)&0xff,tpc[0][ii]&0xff,tpc[1][ii],tpc[2][ii]);
  }
  */

  /*if(printflag) fclose(fd);*/


  /*
  printf("ntpc=%d\n",ntpc);
  for(ii=0; ii<ntpc; ii++)
  {
    printf(" [%6d] slot=%d channel=%d tdc=%d adc=%d\n",
            ii,(tpc[0][ii]>>8)&0xff,tpc[0][ii]&0xff,tpc[1][ii],tpc[2][ii]);
  }
  */

  *itpc = ntpc;

  return(ntpc);
}




int
main(int argc, char **argv)
{
  int nr,sec,layer,strip,wire,nl,ncol,nrow,i,j,l,l1,l2,ichan,nn,iev;
  int ind,ind1,ind2,status,status1,handle,handle1,k,m,rocid;
  int scal,nw,scaler,scaler_old, bit10;
  unsigned int hel,str,strob,helicity,strob_old,helicity_old, tgbi1;
  int tmp1 = 1, tmp2 = 2, iret, bit1, bit2, leading[2000], trailing[2000], nleading, ntrailing;
  float *bcsfl, rndm_();
  char strr[1000], bankname[5], histname[128];
  static int syn[32], id;

  int slots[6] = {1,3,5,16,18,20};

  int ntpc;
  unsigned short tpc[3][NWCHANNEL];

  char *HBOOKfile = "test1.his";
  int nwpawc,lun,lrec,istat,icycle,idn,nbins,nbins1,igood,offset;
  float x1,x2,y1,y2,ww,tmpx,tmpy,ttt,ref;
  unsigned short *tlv1, *tlv2, *buf16;

  printf(" dchist reached !\n");

  if(argc != 2)
  {
    printf("Usage: dchist <fpack_filename>\n");
    exit(1);
  }



  bcsfl = (float*)bcs_.iw;
  bosInit(bcs_.iw,NBCS);


  nwpawc = NWPAWC;
  hlimit_(&nwpawc);
  lun = 11;
  lrec = LREC;
  hropen_(&lun,"NTUPEL",HBOOKfile,"N",&lrec,&istat,
     strlen("NTUPEL"),strlen(HBOOKfile),1);
  if(istat)
  {
    printf("\aError: cannot open RZ file %s for writing.\n", HBOOKfile);
    exit(0);
  }
  else
  {
    printf("RZ file >%s< opened for writing, istat = %d\n\n", HBOOKfile, istat);
  }



  for(i=0; i<6; i++)
  {
	for(j=0; j<128; j++)
    {
  nbins=120;
  x1 = 0.;
  x2 = 120.;
  ww = 0.;
      sprintf(histname,"1: slot %2d channel %2d",slots[i],j);
      idn = slots[i]*1000+j;
      hbook1_(&idn,histname,&nbins,&x1,&x2,&ww,strlen(histname));

  nbins=102;
  x1 = 0.;
  x2 = 1024.;
  ww = 0.;
      sprintf(histname,"2: slot %2d channel %2d",slots[i],j);
      idn = 100000 + slots[i]*1000+j;
      hbook1_(&idn,histname,&nbins,&x1,&x2,&ww,strlen(histname));
    }
  }

  /*
  nbins=100;
  x1 = 0.;
  x2 = 10000.;
  nbins1=100;
  y1 = 0.;
  y2 = 10000.;
  ww = 0.;
  idn=3;
  sprintf(histname,"Reference: channel1 vs channel2",idn);
  hbook2_(&idn,histname,&nbins,&x1,&x2,&nbins1,&y1,&y2,&ww,strlen(histname));
  idn=4;
  sprintf(histname,"Reference: channel1 vs channel2 (sync)",idn);
  hbook2_(&idn,histname,&nbins,&x1,&x2,&nbins1,&y1,&y2,&ww,strlen(histname));
  */




  sprintf(strr,"OPEN INPUT UNIT=1 FILE='%s' ",argv[1]);
  printf("fparm string: >%s<\n",strr);
  status = fparm_(strr,strlen(strr));
  for(iev=0; iev<100000; iev++)
  {
    frbos_(bcs_.iw,&tmp1,"E",&iret,1);


    /*if(!(iev%1000))*/ {/*printf("===== Event no. %d\n",iev);fflush(stdout);*/}


    if((ind2 = bosNlink(bcs_.iw,"TGBI",0))>0)
	{
      unsigned int *fb;
      fb = (unsigned int *)&bcs_.iw[ind2];
      bit10 = (fb[0]>>9)&0x1;
      /*printf("tgbi 0x%08x (%d)\n",fb[0],bit10);*/
	}
    else
	{
      bit10 = 0;
	}

	if(bit10==1)
	{
      printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
      printf("===== Event no. %d\n",iev);

    if((ind1=bosNlink(bcs_.iw,"RC00",0)) > 0)
    {
      unsigned int *fb, *fbend;
      unsigned short *fb16;
      int crate,slot,channel,edge,data,ncol1,nrow1,tdc;

      ncol1 = bcs_.iw[ind1-6];
      nrow1 = bcs_.iw[ind1-5];
      nw = nrow1;
      offset = 0;
	  
      printf("ncol=%d nrow=%d nw=%d\n",ncol1,nrow1,nw);

      /*printf("\n");*/

      fb = (unsigned int *)&bcs_.iw[ind1];

      ntpc = NWCHANNEL;
      rorcDecodeEvent(nw, fb, &ntpc, tpc, 1);
	}

	}



	/* */

    if(iret == -1 || iret > 0)
    {
      printf(" End-of-File flag, iret =%d\n",iret);
      goto a111111;
    }
    else if(iret < 0)
    {
      printf(" Error1, iret =%d\n",iret);
      goto a111111;
    }
    if(iret != 0)
    {
      printf(" Error2, iret =%d\n",iret);
    }
    bdrop_(bcs_.iw,"E",1);
    bgarb_(bcs_.iw);
  }

a111111:

  fparm_("CLOSE",5);

  /* closing HBOOK file */
  idn = 0;
  hrout_(&idn,&icycle," ",1);
  /*hprint_(&idn);*/
  hrend_("NTUPEL", 6);



  exit(0);

  /* end of v1190 test */

}

















