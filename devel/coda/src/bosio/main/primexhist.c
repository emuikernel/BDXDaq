
/* primexhist.c */
 
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "bosio.h"

#define NBCS 700000
#include "bcs.h"

#define RADDEG 57.2957795130823209
#define NWPAWC 5000000 /* Length of the PAWC common block. */
#define LREC 1024/*4096*/      /* Record length in machine words. */

struct {
  float hmemor[NWPAWC];
} pawc_;

struct {
  int iquest[100];
} quest_;



#define LENEVENT 9/*1730*/
#define NSLOTS1  7
#define NSLOTS2  11
#define NSLOTS3  9

main(int argc, char **argv)
{
  int nr,layer,strip,nl,ncol,nrow,i,j,l,l1,l2,ichan,nn,iev, ievgood;
  int ind,ind1,ind2,status,status1,handle,handle1,k,m;
  int scal,nw,scaler,scaler_old;
  unsigned int hel,str,strob,helicity,strob_old,helicity_old;
  int tmp1 = 1, tmp2 = 2, iret, bit1, bit2, rocid, slot, channel;
  float *bcsfl, rndm_();
  char strr[1000], title[80], bankname[5];

  char *HBOOKfile = "primex.hbook";
  int nwpawc,lun,lrec,istat,icycle,idn,nbins,nbins1,igood,offset;
  float x1,x2,y1,y2,ww,tmpx,tmpy,ref;
  unsigned short *tlv1, *tlv2, *buf16;

  int slots1[NSLOTS1] = {4,6,8,10,14,20,22};
  int slots2[NSLOTS2] = {4,6,8,10,12,14,16,18,20,22,24};
  int slots3[NSLOTS3] = {6,8,12,14,16,18,20,22,24};


  int lenevent = LENEVENT; /* the number of 32-bit words in event */
  int nprime = 4096;
  int nchtags;
  char chtags[LENEVENT][8] = {
    "iev     ",
	"ch1adc  ",
	"ch2adc  ",
	"ch3adc  ",
	"ch4adc  ",
	"ch5adc  ",
	"ch6adc  ",
	"ch7adc  ",
	"ch8adc  "
  };
  float event[LENEVENT];


  /* fill 'chtags' 
  nchtags = 0;
  sprintf(chtags[nchtags++],"iev     ");
  for(i=0; i<NSLOTS1; i++)
  {
    for(j=0; j<64; j++)
	{
      sprintf(chtags[nchtags++],"sl%02dch%02d",slots1[i],j);
    }
  }  
  for(i=0; i<NSLOTS2; i++)
  {
    for(j=0; j<64; j++)
	{
      sprintf(chtags[nchtags++],"sl%02dch%02d",slots2[i],j);
    }
  }  
  for(i=0; i<NSLOTS3; i++)
  {
    for(j=0; j<64; j++)
	{
      sprintf(chtags[nchtags++],"sl%02dch%02d",slots3[i],j);
    }
  }
  printf("nchtags = %d\n",nchtags);
  */

  printf(" primexhist reached !\n");

  if(argc != 2)
  {
    printf("Usage: primexhist <datafile>\n");
    exit(1);
  }

  bcsfl = (float*)bcs_.iw;
  bosInit(bcs_.iw,NBCS);

  nwpawc = NWPAWC;
  hlimit_(&nwpawc);

  quest_.iquest[9]=256000;

  lun = 11;
  lrec = LREC;
  hropen_(&lun,"NTUPEL",HBOOKfile,"NQE",&lrec,&istat,
     strlen("NTUPEL"),strlen(HBOOKfile),1);
  if(istat)
  {
    printf("\aError: cannot open RZ file %s for writing.\n", HBOOKfile);
    exit(0);
  }
  else
  {
    printf("RZ file >%s< opened for writing, istat = %d\n\n", HBOOKfile, istat);
    chmod(HBOOKfile,0777);
  }


  /* create Ntuple */

  idn = 2;
  hbookn_(&idn,"WOLF",&lenevent,"NTUPEL",&nprime,chtags,strlen("WOLF"),6,8);

  /* create histograms */

  nbins = 2000;
  x1 = 0.;
  x2 = 2000.;
  ww = 0.;

  for(rocid=1; rocid<=3; rocid++)
    for(slot=4; slot<=24; slot+=2)
      for(channel=0; channel<64; channel++)
	  {
        idn = rocid*10000+slot*100+channel;
        sprintf(title,"roc%1d slot%2d chan%2d",rocid,slot,channel);
        hbook1_(&idn,title,&nbins,&x1,&x2,&ww,strlen(title));
	  }

  sprintf(strr,"OPEN INPUT UNIT=1 FILE='%s' ",argv[1]);
  printf("fparm string: >%s<\n",strr);
  status = fparm_(strr,strlen(strr));

  ww = 1.0;

  ievgood = 0;
  for(iev=0; iev<1000; iev++)
  {
    frbos_(bcs_.iw,&tmp1,"E",&iret,1);

    for(i=0; i<LENEVENT; i++) event[i] = 0.0;


    for(rocid=1; rocid<=3; rocid++)
    {
      sprintf(bankname,"RC%02d",rocid);
      if((ind1=bosNlink(bcs_.iw,bankname,0)) > 0)
      {
        unsigned int *raw1;
        int edge,data,count,ncol1,nrow1,kk;
        ncol1 = bcs_.iw[ind1-6];
        nrow1 = bcs_.iw[ind1-5];
        nw = nrow1;
	    /*printf("ncol=%d nrow=%d nw=%d\n",ncol1,nrow1,nw);*/
        ww = 1.0;
        raw1 = (unsigned int *)&bcs_.iw[ind1];
        k=0;
        while(k<nrow1)
        {
          slot = (raw1[0]>>27)&0x1f;
          channel = (raw1[0]>>17)&0x3f;
          data = raw1[0]&0x3fff;
	  	  count = raw1[0]&0x7f;
	  	  /*printf("[%4d] slot=%2d chan=%2d data=%6d (count=%2d)\n",k,slot,channel,data,count);*/
          for(kk=1; kk<count; kk++)
          {
            slot = (raw1[kk]>>27)&0x1f;
            channel = (raw1[kk]>>17)&0x3f;
            data = raw1[kk]&0x3fff;
	  	    /*printf("hit: slot %d ch %d data %d\n",slot,channel,data);*/
	  	    idn = rocid*10000+slot*100+channel;
	  	    tmpx = (float)(data);
            hf1_(&idn,&tmpx,&ww);
	  	  }
          k+=count;
          raw1 +=count;
	    }
	  }
	}




    if(iret == -1 || iret > 0)
    {
      printf(" End-of-File flag, iret =%d\n",iret);
      goto ending;
    }
    else if(iret < 0)
    {
      printf(" Error1, iret =%d\n",iret);
      exit(0);
    }
    if(iret != 0)
    {
      printf(" Error2, iret =%d\n",iret);
    }
    bdrop_(bcs_.iw,"E",1);
    bgarb_(bcs_.iw);

    if(ievgood>700000)
	{
      printf("Exit on %d good events\n",ievgood);
      break;
	}

  }

ending:

  fparm_("CLOSE",5);

  /* closing HBOOK file */
  idn = 0;
  hrout_(&idn,&icycle," ",1);
  /*hprint_(&idn);*/
  hrend_("NTUPEL", 6);



  exit(0);
}
