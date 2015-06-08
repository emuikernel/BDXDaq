
/* fadchist.c */
 
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
int ind,ind1,ind2,status,status1,handle,handle1,k,m;
int scal,nw,scaler,scaler_old;
unsigned int hel,str,strob,helicity,strob_old,helicity_old, tgbi1;
int tmp1 = 1, tmp2 = 2, iret, bit1, bit2;
float *bcsfl, rndm_();
char strr[1000];

int ecgood1[7][7][37];
int ecbad1[7][7][37];


static char *str1 = "OPEN INPUT UNIT=1 FILE=\"FPACK.DAT\" RECL=36000";
static char *str2 = "OPEN OUTPUT UNIT=2 FILE=\"FPACK.A00\" RECL=36000 SPLITMB=2 RAW WRITE SEQ NEW BINARY";

static char *str3 = "OPEN INPUT UNIT=1 FILE=\"/raid/stage_in/dctest_037348.A00\" ";
static char *str4 = "OPEN OUTPUT UNIT=2 FILE=\"./test.dat\" RECL=32768 SPLITMB=2047 WRITE SEQ NEW BINARY";

static char *str5 = "OPEN INPUT UNIT=1 FILE=\"FPACK6.DAT\" RECL=36000";
static char *str6 = "OPEN INPUT UNIT=1 FILE=\"/work/clas/disk3/sep97/cerenkov/run9470.B00.00\" RECL=36000";

static int syn[32], id;

char *HBOOKfile = "test.his";
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



  nbins=60;
  /*start*/
  x1 = 0.;
  x2 = 300.;
  
  /*stop
  x1 = 700.;
  x2 = 1000.;
  */
  ww = 0.;

  idn = 10;
  hbook1_(&idn,"FADC PULSE",&nbins,&x1,&x2,&ww,10);

  nbins=1000;
  x1 = 0.;
  x2 = 8000.;

  idn = 20;
  hbook1_(&idn,"FADC CHARGE",&nbins,&x1,&x2,&ww,11);
  idn = 30;
  hbook1_(&idn,"1881 CHARGE",&nbins,&x1,&x2,&ww,11);

  /*
  nbins=100.;
  nbins1=100.;
  x1 = 0.;
  x2 = 4000.;
  y1 = 8000.;
  y2 = 12000.;
  ww = 0.;
  idn = 70000;
  hbook2_(&idn,"YvxX",&nbins,&x1,&x2,&nbins1,&y1,&y2,&ww,4);
  */


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
  for(iev=100; iev<100000; iev++)
  {
    frbos_(bcs_.iw,&tmp1,"E",&iret,1);

	/*
printf("===== Event no. %d\n",iev);
	*/

	/* FADC RAW bank */

    if((ind1=bosNlink(bcs_.iw,"RC01",0)) > 0)
    {
      unsigned int *raw1;
      int crate,slot,channel,edge,data,count,ncol1,nrow1;
      int oldslot = 100;
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

      baseline = sum = 0.;
      raw1 = (unsigned int *)&bcs_.iw[ind1+1]; /*skip first word (board header)*/
      for(k=0; k<(nrow1-1); k++)
      {
        slot = (raw1[0]>>27)&0x1f;
        channel = (raw1[0]>>19)&0x1f;
        edge = (raw1[0]>>26)&0x1;
        data = raw1[0]&0x7ffff;
		/*
        printf("RAW: slot=%d chan=%d data=%d\n",
          slot,channel,data);
		*/
		tmpx = (float)(k*5); /* 5ns per FADC bin (200mHz FADC) */
		ww = (float)data;
        idn=10;
        hf1_(&idn,&tmpx,&ww);

        if(k<10) baseline += data;
        if(k>=10 && k<50) sum += data;

        raw1 ++;
	  }

/* printf("baseline1=%d\n",baseline); */
baseline = baseline / 10;
/* printf("baseline2=%d\n",baseline); */
baseline = baseline * 40;
/* printf("baseline3=%d\n",baseline); */

/* printf("sum1=%d\n",sum); */
sum = sum - baseline;
/* printf("sum2=%d\n",sum); */
sum = -sum;
/* printf("sum3=%d\n",sum); */




      /* integrated */
	  tmpx = (float)sum;
	  ww = 1.;
      idn=20;
      hf1_(&idn,&tmpx,&ww);


	}


    /* 1881 */
    if((ind1=bosNlink(bcs_.iw,"SC  ",3)) > 0)
    {
      int ncol1,nrow1;
      int id,tdcl,tdcr,adcl,adcr;

      ncol1 = bcs_.iw[ind1-6];
      nrow1 = bcs_.iw[ind1-5];
      nw = nrow1;
      offset = 0;
      ww = 1.0;

      buf16 = (unsigned short *)&bcs_.iw[ind1];
      for(k=0; k<nrow1; k++)
      {
        id   = *buf16 ++;
        tdcl = *buf16 ++;
        adcl = *buf16 ++;
        tdcr = *buf16 ++;
        adcr = *buf16 ++;

		if(id==2)
        {
          tmpx = (float)adcl;
          idn=30;
          hf1_(&idn,&tmpx,&ww);
		}

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

















