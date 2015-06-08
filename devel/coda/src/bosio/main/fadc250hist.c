
/* fadc250hist.c */
 
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

typedef struct LeCroy1881MHead *ADCHeadPtr;
typedef struct LeCroy1881MHead
{
  unsigned slot    :  5;
  unsigned empty   : 20;
  unsigned count   :  7;
} ADCHead;

typedef struct LeCroy1881M *ADCPtr;
typedef struct LeCroy1881M
{
  unsigned slot    :  5;
  unsigned type    :  3;
  unsigned channel :  7;
  unsigned empty   :  3;
  unsigned data    : 14;
} ADC;

void
add_bank(char *name, int num, char *format, int ncol, int nrow, int ndata, int data[])
{
  int i,ind;

printf("format >%s<\n",format);
printf("name >%s<\n",name);

  bosNformat(bcs_.iw,name,format);
/*bosnformat_(bcs_.iw,name,format,strlen(name),strlen(format));*/

  ind=bosNcreate(bcs_.iw,name,num,ncol,nrow);
printf("ind=%d\n",ind);

  /*  for(i=0; i<ndata; i++) bcs_.iw[ind+i]=data[i];*/

  return;
}





main(int argc, char **argv)
{
int nr,sec,layer,strip,nl,ncol,nrow,i,j,l,l1,l2,ichan,nn,iev;
int iev_spect=0, iev_START=100, iev_STOP=2000000;
int N_base[16], b_data[16];
int offset_now=-1, offset_new=-1;
int ind,ind1,ind2,status,status1,handle,handle1,k,m;
int scal,nw,scaler,scaler_old;
unsigned int hel,str,strob,helicity,strob_old,helicity_old, tgbi1;
int tmp1 = 1, tmp2 = 2, iret, bit1, bit2;
float *bcsfl, rndm_();
char strr[1000];
float sss[16], fff[16], ssst, ffft;

int ecgood1[7][7][37];
int ecbad1[7][7][37];


static char *str1 = "OPEN INPUT UNIT=1 FILE=\"FPACK.DAT\" RECL=36000";
static char *str2 = "OPEN OUTPUT UNIT=2 FILE=\"FPACK.A00\" RECL=36000 SPLITMB=2 RAW WRITE SEQ NEW BINARY";

static char *str3 = "OPEN INPUT UNIT=1 FILE=\"/raid/stage_in/dctest_037348.A00\" ";
static char *str4 = "OPEN OUTPUT UNIT=2 FILE=\"./test.dat\" RECL=32768 SPLITMB=2047 WRITE SEQ NEW BINARY";

static char *str5 = "OPEN INPUT UNIT=1 FILE=\"FPACK6.DAT\" RECL=36000";
static char *str6 = "OPEN INPUT UNIT=1 FILE=\"/work/clas/disk3/sep97/cerenkov/run9470.B00.00\" RECL=36000";

static int syn[32], id;

char *HBOOKfile = "fadc250hist.his";
int nwpawc,lun,lrec,istat,icycle,idn,nbins,nbins1,igood,offset;
int lookup[32] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                  25,27,29,31,33,35, 2, 4, 6, 8,10,12,14,16,18,20};
float x1,x2,y1,y2,ww,tmpx,tmpy,ttt,ref;
unsigned short *tlv1, *tlv2, *buf16;


/* run 9183 */
int nev[] = {1358,1573,2027,2166,2838,4242,4350,6143,6645,7212,
      7608,9227,9279,10626,12002,12437,13116,14429,14814,16829,
      17093,17430,18568,18569,20426,20784,21338,21495,21792,
      24377,24466,24941,27990,28409,29950};


  printf(" boshist reached !\n");


  if(argc != 2)
  {
    printf("Usage: boshist <fpack_filename>\n");
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



  nbins=100;
  x1 = 0.;
  x2 = 100.;
  ww = 0.;
  for(idn=1; idn<=16; idn++)
  {
    hbook1_(&idn,"FADC SPECTRA",&nbins,&x1,&x2,&ww,12);
  }

  nbins = 200;
  x1 = 150.;
  x2 = 350.;
  ww = 0.;
  for(idn=21; idn<=36; idn++)
  {
    hbook1_(&idn,"FADC BaseLine",&nbins,&x1,&x2,&ww,13);
  }

  nbins = 1000;
  x1 = 0.;
  x2 = 10000.;
  ww = 0.;
  for(idn=41; idn<=56; idn++)
  {
    hbook1_(&idn,"FADC spectra",&nbins,&x1,&x2,&ww,12);
  }

  nbins = 1000;
  x1 = 0.;
  x2 = 10000.;
  ww = 0.;
  for(idn=61; idn<=76; idn++)
  {
    hbook1_(&idn,"1881 spectra",&nbins,&x1,&x2,&ww,12);
  }


  nbins=60;
  x1 = 0.;
  x2 = 400.;
  ww = 0.;
  idn = 100;
  hbook1_(&idn,"FADC PULSE",&nbins,&x1,&x2,&ww,10);

  nbins=1000;
  x1 = 0.;
  x2 = 8000.;
  idn = 200;
  hbook1_(&idn,"FADC CHARGE",&nbins,&x1,&x2,&ww,11);
  idn = 300;
  hbook1_(&idn,"1881 CHARGE",&nbins,&x1,&x2,&ww,11);





  nbins=80;
  nbins1=80;
  x1 = 0.;
  x2 = 2000.;
  y1 = 0.;
  y2 = 8000.;
  ww = 0.;
  for(idn=1001; idn<=1036; idn++)
  {
    hbook2_(&idn,"YvxX",&nbins,&x1,&x2,&nbins1,&y1,&y2,&ww,4);
  }


  nbins=600;
  x1 = 0./*4000.*/;
  x2 = 6000./*5000.*/;
  ww = 0.;



  /*
#define PRINT1 1
  */
  /*
#define PRINT2 1
  */


  sprintf(strr,"OPEN INPUT UNIT=1 FILE='%s' ",argv[1]);
  printf("fparm string: >%s<\n",strr);
  status = fparm_(strr,strlen(strr));

  for(k=0; k<16; k++)
  {
    b_data[k] = 0;
    N_base[k] = 0;
  }
  for(i=0; i<16; i++) fff[i] = 0.;
  for(i=0; i<16; i++) sss[i] = 0.;

  for(iev=iev_START; iev<iev_STOP/*104*//*213*/; iev++)
  {
    frbos_(bcs_.iw,&tmp1,"E",&iret,1);

	/*
    if(N_base[15] == 1000 && (offset_now != offset_new))
    {
      for(k=0; k<16; k++)
      {
	tmpx = (float)offset_now;
	if(N_base[k]==0) N_base[k]=1;
	ww   = (float)(b_data[k]/N_base[k]);
	idn  = k+21;
	hf1_(&idn,&tmpx,&ww);

printf("iev=%d  b_data[%2d]=%d  offset=%d new=%d \n",iev,k,b_data[k],offset_now,offset_new);
 	b_data[k] = 0;
	N_base[k] = 0;
      }
    }

    if(offset_new > 3490) break;

    if(offset_now != offset_new)
    {
printf("\n iev=%d   offset=%d new=%d \n",iev,offset_now,offset_new);
      offset_now = offset_new;
    }
	*/


    /*
printf("===== Event no. %d\n",iev);
    */

if(iev < 103) goto nextevent;



	/* FADC RAW bank */

    if((ind1=bosNlink(bcs_.iw,"RC20",0)) > 0)
    {
      unsigned int *b32, *end;
      unsigned short *b16;
      int crate,slot,channel,nsamples,notvalid,edge,data,count,ncol1,nrow1;
      int oldslot = 100;
      int ndata0[22], data0[21][8];
      int baseline, sum;

      float yyy;
	  float zzz[16] = {275.6,249.6,283.5,279.2,245.6,247.0,288.3,267.2,
                       298.4,313.0,279.5,297.5,270.1,253.3,264.5,267.2};

      ncol1 = bcs_.iw[ind1-6];
      nrow1 = bcs_.iw[ind1-5];
      nw = nrow1;
      offset = 0;
	  /*
      printf("ncol=%d nrow=%d nw=%d\n",ncol1,nrow1,nw);
	  */
      ww = 1.0;


      baseline = sum = 0.;
      b32 = (unsigned int *)&bcs_.iw[ind1];
      end = b32 + nw - 1;
      offset_new = end[0];
      /*
      printf("\n new=%d   offset=%d %d %d\n",offset_new, end[-1],end[0],end[1]);
      */

      /*printf("nw=%d, b32=0x%08x, end=0x%08x\n",nw,b32,end);*/
      for(i=0; i<16; i++) sss[i] = 0.;
      while(b32 < end)
      {
        slot = (b32[0]>>24)&0xFF;
        channel = (b32[0]>>16)&0xFF;
        nsamples = (b32[0])&0xFFFF;
		/*
	  printf("RAW: slot=%2d chan=%2d nsamples=%3d:",slot,channel,nsamples);
		*/
        b16 = (unsigned short *)(b32+1);
        yyy = 0.;
        for(k=0; k<nsamples; k++)
        {
          data = b16[0]&0x1FFF;
          notvalid = (b16[0]>>14)&0x1;
          if(notvalid>0) printf("not valid adc\n"); 

		  if(k<5) yyy += (float)data;
          if(k==4)
          {
            yyy = yyy / 5.;
            /*printf("[%d] yyy=%f\n",channel,yyy);*/
        	tmpx = yyy;
	        ww   = 1.;
	        idn  = 21+channel;
	        hf1_(&idn,&tmpx,&ww);
		  }

		  /*
		  if(k>=28&&k<60) printf("%5d",data);
		  */
	      if(k>=5)
	      {
	        tmpx = (float)k;
	        ww   = (float)data-yyy/*zzz[channel]*/;
	        idn  = channel+1;
	        hf1_(&idn,&tmpx,&ww);
	        /*printf("filling: id=%d tmpx=%f ww=%f\n",idn,tmpx,ww);*/
            if(k>10 && k<50) sss[channel] += ww;
	      }

          b16 ++;
	    }
		/*		
        printf("\n");
		*/
        b32 = (unsigned int *)b16;
      }

      for(i=0; i<16; i++)
      {
        tmpx = sss[i];
	    ww   = 1.;
	    idn  = 41+i;
	    if(tmpx>100.) hf1_(&idn,&tmpx,&ww);
	  }

      for(i=0; i<16; i++)
	  {
        idn=1001+i;
        ww = 1.;
        tmpx = fff[i];
        tmpy = sss[i];
        hfill_(&idn,&tmpx,&tmpy,&ww);
	  }

	  /*
      ssst = 200.;
      ffft = 200.;
	  if(sss[0]>ssst||sss[1]>ssst||sss[2]>ssst||sss[3]>ssst||sss[4]>ssst||
		 sss[6]>ssst||sss[7]>ssst||sss[8]>ssst||sss[9]>ssst||sss[10]>ssst||
		 sss[11]>ssst||sss[12]>ssst||sss[13]>ssst||sss[14]>ssst||sss[15]>ssst)
      {
        printf("sss --> %3.0f %3.0f %3.0f %3.0f %3.0f - %3.0f %3.0f %3.0f %3.0f %3.0f - %3.0f %3.0f %3.0f %3.0f %3.0f\n",
				   sss[0],sss[1],sss[2],sss[3],sss[4],
				   sss[6],sss[7],sss[8],sss[9],sss[10],
				   sss[11],sss[12],sss[13],sss[14],sss[15]);
        printf("fff --> %3.0f %3.0f %3.0f %3.0f %3.0f - %3.0f %3.0f %3.0f %3.0f %3.0f - %3.0f %3.0f %3.0f %3.0f %3.0f\n",
				   fff[0],fff[1],fff[2],fff[3],fff[4],
				   fff[6],fff[7],fff[8],fff[9],fff[10],
				   fff[11],fff[12],fff[13],fff[14],fff[15]);
	  }
	  */

    }



    /* 1881 */
    if((ind1=bosNlink(bcs_.iw,"EC  ",3)) > 0)
    {
      int ncol1,nrow1,layer,strip;
      int id,tdcl,tdcr,adcl,adcr;

      ncol1 = bcs_.iw[ind1-6];
      nrow1 = bcs_.iw[ind1-5];
      nw = nrow1;
      offset = 0;
      ww = 1.0;

      for(i=0; i<16; i++) fff[i] = 0.;
      buf16 = (unsigned short *)&bcs_.iw[ind1];
      for(k=0; k<nrow1; k++)
      {
        id   = *buf16 ++;
        tdcl = *buf16 ++;
        adcl = *buf16 ++;

        layer = (id>>8)&0xFF;
        strip = id&0xFF;
		/*printf("adc1881: layer=%d strip=%d adc=%d\n",layer,strip,adcl);*/

		/*
		if(layer==1&&strip>=21&&strip<=25 ||
		   layer==2&&strip>=22&&strip<=26 ||
		   layer==3&&strip>=22&&strip<=26)
		*/
        tmpx = (float)adcl;
        if(     layer==1&&strip==21) fff[0] = tmpx;
        else if(layer==1&&strip==22) fff[1] = tmpx;
        else if(layer==1&&strip==23) fff[2] = tmpx;
        else if(layer==1&&strip==24) fff[3] = tmpx;
        else if(layer==1&&strip==25) fff[4] = tmpx;
        else if(layer==2&&strip==22) fff[6] = tmpx;
        else if(layer==2&&strip==23) fff[7] = tmpx;
        else if(layer==2&&strip==24) fff[8] = tmpx;
        else if(layer==2&&strip==25) fff[9] = tmpx;
        else if(layer==2&&strip==26) fff[10] = tmpx;
        else if(layer==3&&strip==22) fff[11] = tmpx;
        else if(layer==3&&strip==23) fff[12] = tmpx;
        else if(layer==3&&strip==24) fff[13] = tmpx;
        else if(layer==3&&strip==25) fff[14] = tmpx;
        else if(layer==3&&strip==26) fff[15] = tmpx;

		/*
		if(layer==1&&strip>=21&&strip<=25)
        {
          tmpx = (float)adcl;
          idn=1000+strip;
          ww = 1.;
          if(tmpx>600.)
          {
            tmpy = sss[0];
            hfill_(&idn,&tmpx,&tmpy,&ww);
		  }

          if(strip>=21)
		  {
	        idn = 61+(strip-21);
	        hf1_(&idn,&tmpx,&ww);
		  }
		}
		*/

      }
    }


nextevent:

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

















