
/* dsc2hist.c */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "bosio.h"

#define NBCS     700000
#define TDC_SLOT 7

#include "bcs.h"

#define RADDEG 57.2957795130823209
#define NWPAWC 10000000 /* Length of the PAWC common block. */
#define LREC 1024       /* Record length in machine words. */

struct {
  float hmemor[NWPAWC];
} pawc_;


  

typedef struct LeCroy1877Head
{
  unsigned slot    :  5;
  unsigned empty0  :  3;
  unsigned empty1  :  8;
  unsigned empty   :  5;
  unsigned count   : 11;
} MTDCHead;

typedef struct LeCroy1877
{
  unsigned slot    :  5;
  unsigned type    :  3;
  unsigned channel :  7;
  unsigned edge    :  1;
  unsigned data    : 16;
} MTDC;



main(int argc, char **argv)
{
  /*
  int nr,sec,layer,strip,wire,nl,ncol,nrow,i,j,l,l1,l2,ichan,nn,iev;
  int crate,slot,connector, tdc_width;
  int ind,ind1,ind2,status,status1,handle,handle1,k,m,rocid;
  int scal,nw,scaler,scaler_old, counter,dccount[7],rccount[7];
  int tmp1 = 1, tmp2 = 2, iret, bit1, bit2, nleading, ntrailing;
  BOSIO *descriptor;
  unsigned int   hel,str,strob,helicity,strob_old,helicity_old, tgbi1;
  unsigned int   leading[20000], trailing[20000];
  unsigned short *tlv1, *tlv2, *buf16;
  static int     syn[32], id;
  float    *bcsfl, rndm_();
  char     bankname[5];
  MTDCHead *mtdchead;
  MTDC     *mtdc;
  */
  int iev,nw;
  int iev_START, iev_STOP, done=0;
  int nevent=0;
  int kk,ll,tdc_width_now=0, tdc_width_new=0;
  int ns_fil[21][32],jj_fil[21][32];
  int ind1,status;
  int tmp1 = 1, iret;
  float *bcsfl;
  char strr[1000], histname[128];

  char *HBOOKfile = "dsc2_1290.his";
  int nwpawc,lun,lrec,istat,icycle,idn,idn2,nbins,offset;
  float x1,x2,y1,y2,ww,tmpx,tmpy;


  printf("\n dsc2hist reached !\n");

  /*
  leading = (int *)malloc(80000);
  trailing = (int *)malloc(80000);
  */

  if(argc != 4)
  {
    printf("\n Usage: dsc2hist data_file iev_START iev_STOP \n\n");
    exit(1);
  }
  else
  {
    iev_START  = atoi(argv[2]);
    iev_STOP   = atoi(argv[3]);
  }
  printf("===============\n");
  printf("argv0:    >%s<\n",argv[0]);
  printf("argv1:    >%s<\n",argv[1]);
  printf("argv2:    >%s<   iev_START = %d \n",argv[2],iev_START);
  printf("argv3:    >%s<   iev_STOP  = %d \n",argv[3],iev_STOP);
  printf("TDC_slot: >%d<\n",TDC_SLOT);
  printf("---------------\n");


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
    chmod(HBOOKfile,0666);
  }

  for(kk=0; kk<21; kk++)
    for(ll=0; ll<32; ll++)
    {
      ns_fil[kk][ll]=0;
      jj_fil[kk][ll]=0;
    }

  nbins = 3000;
  x1    = 0.;
  x2    = 3000.;
  ww    = 0.;
  for(kk=7; kk<11; kk++)
  for(idn=1; idn<=32; idn++)
  {
    idn2 = kk*100+idn;
    sprintf(histname,"TDC.1290.sl.%02d.ch.%02d",kk,idn-1);
    hbook1_(&idn2,histname,&nbins,&x1,&x2,&ww,strlen(histname));
  }

  nbins = 6000;
  x1    = 0.;
  x2    = 6000.;
  ww    = 0.;
  for(ll=1; ll<=4; ll++)         /* histos only for 10,20,30,40 ns */
  for(kk=7; kk<=10; kk++)        /* TDC slots */
    for(idn=1; idn<=32; idn++)   /* histos for all TDC channels */
    {
      idn2 = ll*10000+kk*100+idn;
      sprintf(histname,"%2dns.TDC.1290.sl.%02d.ch.%02d",ll*10,kk,idn-1);
      hbook1_(&idn2,histname,&nbins,&x1,&x2,&ww,strlen(histname));
    }

  /*
  nbins  = 250;
  x1     = 0.;
  x2     = 250.;
  nbins1 = 500;
  y1     = 0.;
  y2     = 500.;
  ww     = 0.;
  for(idn=201; idn<=206; idn++)
  {
    sprintf(histname,"DC load REG1 SEC%1d",idn-200);
    hbook2_(&idn,histname,&nbins,&x1,&x2,&nbins1,&y1,&y2,&ww,strlen(histname));
  }
  */

  sprintf(strr,"OPEN INPUT UNIT=1 FILE='%s' ",argv[1]);
  printf("fparm string: >%s<\n",strr);
  status = fparm_(strr,strlen(strr));


  while(done==0) /* skip loop */
  {
    nevent++;
    if(nevent>iev_START)
    {
      done = 1;
    }
    else
    {
      frbos_(bcs_.iw,&tmp1,"E",&iret,1);
      bdrop_(bcs_.iw,"E",1);
      bgarb_(bcs_.iw);
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
    }
  }


  /*
  for(iev=0; iev<100000; iev++)
  */
  for(iev=iev_START; iev<iev_STOP; iev++)
  {
    frbos_(bcs_.iw,&tmp1,"E",&iret,1);

    if(!(iev%5000)) printf("===== Event no. %d\n",iev);

    if((ind1=bosNlink(bcs_.iw,"RC00",0)) > 0)
    {
      unsigned int *fb, *fbend;
      int crate,slot,channel,edge,data,count,ncol1,nrow1;
      int ndata0[22], data0[21][8];
      int baseline, ii;
      int ref,ref2[21];
      int ns_tmp[21][32];

      ncol1 = bcs_.iw[ind1-6];
      nrow1 = bcs_.iw[ind1-5];
      nw = nrow1;
      offset = 0;
      for(kk=0; kk<21; kk++)
	for(ll=0; ll<32; ll++)
	  ns_tmp[kk][ll]=0;


      /*
      printf("ind1=%d  ncol=%d nrow=%d nw=%d\n",ind1,ncol1,nrow1,nw);
      */



      /* pass 0: find reference */
      fb = (unsigned int *)&bcs_.iw[ind1];
      fbend = fb + nrow1;
      /*fb += 6;*/
      ii=0;
      for(kk=1; kk<21; kk++)  ref2[kk]=0;
      tdc_width_new = *(fbend-1);

      if(tdc_width_now != tdc_width_new)
      {
	for(kk=0; kk<21; kk++)
	for(ll=0; ll<32; ll++)
	  if(jj_fil[kk][ll] != 0)
	  {
	    /* print histos ns.DSC2 vs ns.TDC */
	    tmpx = (float)(ns_fil[kk][ll]/jj_fil[kk][ll]);
	    ww   = (float)tdc_width_now;
	    idn  = kk*100 + ll + 1;
	    hf1_(&idn,&tmpx,&ww);

if(idn == 911 || idn == 922)
printf("------  id=%d    tmpx=%f   ww=%f  ns_fil=%d  jj_fil=%d  kk=%d ll=%d\n",
       idn,tmpx,ww,ns_fil[kk][ll],jj_fil[kk][ll],kk,ll);

	  }

	for(kk=0; kk<21; kk++)
	for(ll=0; ll<32; ll++)
	{
	  ns_fil[kk][ll]=0;
	  jj_fil[kk][ll]=0;
	}

printf("tdc_width_now=%d \n\ntdc_width_new=%d\n",tdc_width_now,tdc_width_new);

	tdc_width_now = tdc_width_new;
      }

      if(tdc_width_new > 40) break;


      while(fb<fbend /*&& ii<32*/)
      {
        slot=((*fb)>>27)&0x1f;
        edge=((*fb)>>26)&0x1;
        channel=((*fb)>>21)&0x1f;
        data=(*fb)&0xfffff;

        if(slot==0)
        {
	  /*          
printf("\n[%3d] board header: slot=%d len=%d\n",ii,(*fb)&0x1F,((*fb)>>5)&0x3FFF);
	  */
	}
	else
        {
          /*
printf("[%3d] slot %d chan %d -> data %d (%d)\n",ii,slot,channel,data,data+2000-ref);
	  */
          if((ref2[slot] == 0) && (channel == 0))
          {
            ref2[slot] = data;
	    /*
printf("ref=%d\n",ref);
	    */
          }
	}
        ii ++;
        fb ++;
      }


      /* pass 1: fill histos */
      ww = 1.0;
      fb = (unsigned int *)&bcs_.iw[ind1];
      fbend = fb + nrow1;
      /*fb += 6;*/
      ii=0;
      while(fb<fbend /*&& ii<32*/)
      {
        slot=((*fb)>>27)&0x1f;
        edge=((*fb)>>26)&0x1;
        channel=((*fb)>>21)&0x1f;
        data=(*fb)&0xfffff;

        if(slot!=0)
        {

	  if(ns_tmp[slot][channel] == 0)
	    ns_tmp[slot][channel] = data+2000-ref2[slot];
	  else
	  {
	    ns_fil[slot][channel] += data+2000-ref2[slot] - ns_tmp[slot][channel];
	    jj_fil[slot][channel]++;

if((jj_fil[slot][channel]<3 || jj_fil[slot][channel]>9997) && 
   ((slot==9 && channel==10) || (slot==9 && channel==21)))
printf("------  sl=%2d    ch=%2d   ns=%d  data=%d  tmp=%d  jj_fil=%d  \n",
       slot,channel,
       ns_fil[slot][channel],
       data+2000-ref2[slot],
       ns_tmp[slot][channel],
       jj_fil[slot][channel]);

	  }

	  if(tdc_width_now%10 == 0)
	  {
	    /*
printf("[%3d] slot %d chan %d -> data %d (%d)\n",ii,slot,channel,data,data+2000-ref2[slot]);
	    */
	    tmpx = (float)(data+2000-ref2[slot]);
	    idn  = tdc_width_now*1000 + slot*100 + channel+1;
     	    hf1_(&idn,&tmpx,&ww);
	    /*
printf("------  id=%d    tmpx=%f   ww=%f \n",idn,tmpx,ww);
	    */
	  }
	}
        ii ++;
        fb ++;
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

  /* end of dsc2 test */

}


