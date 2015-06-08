
/* dchist.c */
 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
  int nr,sec,layer,strip,wire,nl,ncol,nrow,i,j,l,l1,l2,ichan,nn,iev;
  int crate,slot,connector;
  int ind,ind1,ind2,status,status1,handle,handle1,k,m,rocid;
  int scal,nw,scaler,scaler_old, counter,dccount[7],rccount[7];
  unsigned int hel,str,strob,helicity,strob_old,helicity_old, tgbi1;
  int tmp1 = 1, tmp2 = 2, iret, bit1, bit2, nleading, ntrailing;
  float *bcsfl, rndm_();
  char strr[1000], bankname[5], histname[128];
  static int syn[32], id;
  MTDCHead *mtdchead;
  MTDC *mtdc;
  unsigned int leading[20000], trailing[20000];

  char *HBOOKfile = "dchist.his";
  int nwpawc,lun,lrec,istat,icycle,idn,nbins,nbins1,igood,offset;
  float x1,x2,y1,y2,ww,tmpx,tmpy,ttt,ref;
  unsigned short *tlv1, *tlv2, *buf16;

  printf(" dchist reached !\n");

  /*
  leading = (int *)malloc(80000);
  trailing = (int *)malloc(80000);
  */

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



  /*per crate*/
  nbins=1000;
  x1 = 0.;
  x2 = 1000.;
  ww = 0.;
  for(idn=1; idn<=11; idn++)
  {
    sprintf(histname,"The number of hits ROC%02d",idn);
    hbook1_(&idn,histname,&nbins,&x1,&x2,&ww,strlen(histname));
  }

  nbins=4000;
  x1 = 0.;
  x2 = 8000.;
  ww = 0.;
  for(idn=21; idn<=31; idn++)
  {
    sprintf(histname,"TDC spectra ROC%02d",idn-20);
    hbook1_(&idn,histname,&nbins,&x1,&x2,&ww,strlen(histname));
  }
  /*per connector*/
  nbins=4000;
  x1 = 0.;
  x2 = 8000.;
  ww = 0.;
  for(crate=1; crate<=11; crate++)
  {
	for(slot=4; slot<=23; slot++)
	{
	  for(connector=1; connector<=6; connector++)
	  {
        idn = 200000 + crate*1000 + slot*10 + connector;
        sprintf(histname,"TDC spectra in crate %02d, slot %02d, conn %1d",crate,slot,connector);
        hbook1_(&idn,histname,&nbins,&x1,&x2,&ww,strlen(histname));
      }
    }
  }

  nbins=1500;
  x1 = -3000.;
  x2 = 0.;
  ww = 0.;
  for(idn=41; idn<=51; idn++)
  {
    sprintf(histname,"TDC pulses ROC%02d",idn-40);
    hbook1_(&idn,histname,&nbins,&x1,&x2,&ww,strlen(histname));
  }
  /*per connector*/
  nbins=1500;
  x1 = -3000.;
  x2 = 0.;
  ww = 0.;
  for(crate=1; crate<=11; crate++)
  {
	for(slot=4; slot<=23; slot++)
	{
	  for(connector=1; connector<=6; connector++)
	  {
        idn = 400000 + crate*1000 + slot*10 + connector;
        sprintf(histname,"TDC pulses in crate %02d, slot %02d, conn %1d",crate,slot,connector);
        hbook1_(&idn,histname,&nbins,&x1,&x2,&ww,strlen(histname));
      }
    }
  }




  nbins=100;
  x1 = 0.;
  x2 = 100.;
  ww = 0.;
  for(idn=101; idn<=106; idn++)
  {
    sprintf(histname,"DC load REG1 SEC%1d",idn-100);
    hbook1_(&idn,histname,&nbins,&x1,&x2,&ww,strlen(histname));
  }

  nbins=250;
  x1 = 0.;
  x2 = 250.;
  nbins1=500;
  y1 = 0.;
  y2 = 500.;
  ww = 0.;
  for(idn=201; idn<=206; idn++)
  {
    sprintf(histname,"DC load REG1 SEC%1d",idn-200);
    hbook2_(&idn,histname,&nbins,&x1,&x2,&nbins1,&y1,&y2,&ww,strlen(histname));
  }

  sprintf(strr,"OPEN INPUT UNIT=1 FILE='%s' ",argv[1]);
  printf("fparm string: >%s<\n",strr);
  status = fparm_(strr,strlen(strr));
  for(iev=0; iev<90000; iev++)
  {
    frbos_(bcs_.iw,&tmp1,"E",&iret,1);

    if(!(iev%100)) printf("===== Event no. %d\n",iev);



    for(sec=1; sec<=6; sec++)
    {
	dccount[sec] = 0;

    if((ind1=bosNlink(bcs_.iw,"DC0 ",sec)) > 0)
    {
      unsigned int *fb, *fbend;
      int crate,slot,channel,edge,data,count,ncol1,nrow1;

      ncol1 = bcs_.iw[ind1-6];
      nrow1 = bcs_.iw[ind1-5];
      nw = nrow1;
      offset = 0;
	  /*
      printf("ncol=%d nrow=%d nw=%d\n",ncol1,nrow1,nw);
	  */
      ww = 1.0;

      /*printf("\n");*/

      fb = (unsigned int *)&bcs_.iw[ind1];
      fbend = fb + nrow1;
      while(fb < fbend)
	  {
        printf("0x%08x\n",*fb);
        fb ++;
	  }
	  exit(0);	  

      fb = (unsigned int *)&bcs_.iw[ind1];
      fbend = fb + nrow1;
      counter = 0;
      while(fb < fbend)
      {
        layer = ((*fb)>>24)&0xFF;
        wire = ((*fb)>>16)&0xFF;
        if(layer<13) counter ++;

        fb ++;
	  }

      dccount[sec] = counter;
      tmpx = (float)counter;
      idn=sec+100;
      hf1_(&idn,&tmpx,&ww);
	}

	}


	/*
    if(dccount[3]<1 && dccount[5]<1) {
	*/

	for(sec=1; sec<=6; sec++) rccount[sec] = 0;
    for(rocid=1; rocid<=11; rocid++)
    {

    sprintf(bankname,"RC%02d",rocid);
    if((ind1=bosNlink(bcs_.iw,bankname,0)) > 0)
    {
      unsigned int *fb, *fbend;
      int crate,slot,channel,edge,data,count,ncol1,nrow1;
      int ndata0[22], data0[21][8];
      int baseline, sum;

      ncol1 = bcs_.iw[ind1-6];
      nrow1 = bcs_.iw[ind1-5];
      nw = nrow1;
      offset = 0;
	  /*
      printf("ncol=%d nrow=%d nw=%d\n",ncol1,nrow1,nw);
	  */
      ww = 1.0;

      /*printf("\n");*/

	  
      fb = (unsigned int *)&bcs_.iw[ind1];
      fbend = fb + nrow1;
      while(fb < fbend)
	  {
        printf("0x%08x\n",*fb);
        fb ++;
	  }
	  exit(0);	  

      fb = (unsigned int *)&bcs_.iw[ind1];
      fbend = fb + nrow1;
      counter = 0;
      nleading = 0;
      ntrailing = 0;
      for(j=0; j<20000; j++) {leading[j]=0; trailing[j]=0;}
      while(fb < fbend)
      {
        mtdchead = (MTDCHead *)fb;
        mtdc = mtdchead + 1;
        count = mtdchead->count;
        counter ++;
        /*printf("slot %d, count=%d\n",mtdchead->slot,mtdchead->count);*/
        for(j=0; j<(count-1); j++)
		{
          /*printf("   data: slot=%2d, channel=%2d, edge=%1d, tdc=%6d\n",mtdc->slot,mtdc->channel,mtdc->edge,mtdc->data);*/
          if(mtdc->edge==1) leading[nleading++] = mtdc->data;
          else              trailing[ntrailing++] = mtdc->data;
          if(nleading >=20000 || ntrailing >= 20000) printf("ERROR 1\n");
          tmpx = (float)mtdc->data;




          connector = (mtdc->channel/16) + 1;
          idn = 200000 + rocid*1000 + mtdc->slot*10 + connector;
		  /*
		  printf("crate %d slot %d chan %d -> conn %d -> idn %d\n",
            rocid,mtdc->slot,mtdc->channel,connector,idn);
		  */
          hf1_(&idn,&tmpx,&ww);




          idn=20+rocid;
          hf1_(&idn,&tmpx,&ww);
          counter ++;
          if(rocid==5 && mtdchead->slot>=12) rccount[1] ++; /*sec1*/
          if(rocid==5 && mtdchead->slot<=11) rccount[2] ++; /*sec2*/
          if(rocid==6 && mtdchead->slot>=12) rccount[4] ++; /*sec4*/
          if(rocid==6 && mtdchead->slot<=11) rccount[6] ++; /*sec6*/
          if(rocid==11 && mtdchead->slot>=12) rccount[3] ++; /*sec3*/
          if(rocid==11 && mtdchead->slot<=11) rccount[5] ++; /*sec5*/
          mtdc ++;
	    }

        fb += count;
	  }

      tmpx = (float)counter;
      idn=rocid;
      hf1_(&idn,&tmpx,&ww);

      for(j=1; j<=6; j++)
	  {
        tmpx = (float)dccount[j];
        tmpy = (float)rccount[j];
        idn=200+j;
        hf2_(&idn,&tmpx,&tmpy,&ww);
	  }


/*debug print
if(rocid==11 && rccount[5]>50 && dccount[5]>5)
{
*/
  printf("===== Event no. %d\n",iev);
  fb = (unsigned int *)&bcs_.iw[ind1];
  fbend = fb + nrow1;
  while(fb < fbend)
  {
    printf("0x%08x\n",*fb);
    fb ++;
  }
exit(0);
  /*
}
*/

      /*printf("nleading=%d, ntrailing=%d\n",nleading,ntrailing);*/
      for(i=0; i<nleading; i++)
	  {
        for(j=0; j<ntrailing; j++)
		{
          tmpx = (float)leading[i] - (float)trailing[j];
          /*printf("tmpx=%f\n",tmpx);*/
          idn=40+rocid;
          hf1_(&idn,&tmpx,&ww);
		}
	  }



	}

	}

	/*
	}
	*/







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

















