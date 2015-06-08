/*DECK ID>, BOSTESTC. */
 
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




#define TDCFILL(mysec) \
        /*printf("id=%d tdcl=%d\n",id,tdcl);*/ \
        tmpx = (float)(tdc); \
        if(lr==0) idn=id; \
        else      idn=100+id; \
        idn = idn + 1000*mysec; \
        hf1_(&idn,&tmpx,&ww)

#define ADCFILL(mysec) \
        tmpx = (float)(adcl); \
        if(tmpx > 0.) \
        { \
          idn=200+id; \
          hf1_(&idn,&tmpx,&ww); \
          idn=400+id; \
          tmpx = tmpx - (float)pedestals[0][id-1][0]; \
          hf1_(&idn,&tmpx,&ww); \
        } \
        tmpx = (float)(adcr); \
        if(tmpx > 0.) \
        { \
          idn=300+id; \
          hf1_(&idn,&tmpx,&ww); \
          idn=500+id; \
          tmpx = tmpx - (float)pedestals[0][id-1][1]; \
          hf1_(&idn,&tmpx,&ww); \
        }


main(int argc, char **argv)
{
  FILE *fd;
  int nr,sec,layer,strip,nl,ncol,nrow,i,j,l,l1,l2,ichan,nn,iev;
  int ind,ind1,ind2,status,status1,handle,handle1,k,m,i1,i2;
  int scal,nw,scaler,scaler_old;
  unsigned int hel,str,strob,helicity,strob_old,helicity_old, tgbi1;
  int tmp1 = 1, tmp2 = 2, iret, bit1, bit2;
  float f1, f2, *bcsfl, rndm_();
  char strr[1000];

  int pedestals[6][57][2];

  int ecgood1[7][7][37];
  int ecbad1[7][7][37];

  static int syn[32], id;

  char *HBOOKfile = "test.his";
  int nwpawc,lun,lrec,istat,icycle,idn,nbins,nbins1,igood,offset;
  int lookup[32] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                    25,27,29,31,33,35, 2, 4, 6, 8,10,12,14,16,18,20};
  float x1,x2,x22,y1,y2,ww,tmpx,tmpy,ttt,ref,xx[24];
  unsigned short *tlv1, *tlv2, *buf16;


  printf(" boshist reached !\n");


  if(argc != 2)
  {
    printf("Usage: boshist <fpack_filename>\n");
    exit(1);
  }


  for(i=0; i<6; i++)
	for(j=0; j<57; j++)
	  {
        pedestals[i][j][0] = 0;
        pedestals[i][j][1] = 0;
	  }


  /*reading pedestals from local directory (TODO: take it from Tfiles)*/
  if((fd=fopen("peds.txt","r"))==NULL)
  {
    printf("Cannot open pedestal file - ignore\n");
  }
  else
  {
    printf("reading pedestal file >%s<\n","peds.txt");
    for(i=0; i<6; i++)
	  for(j=0; j<57; j++)
        fscanf(fd,"%d%d%d%f%d%f\n",
			   &i1,&i2,&pedestals[i][j][0],&f1,&pedestals[i][j][1],&f2);
    fclose(fd);
  }

  for(i=0; i<6; i++)
	for(j=0; j<57; j++)
      printf("sec %1d counter %2d pedestals: left %4d, right %4d\n",
			   i,j,pedestals[i][j][0],pedestals[i][j][1]);



  bcsfl = (float*)bcs_.iw;
  bosInit(bcs_.iw,NBCS);


  /* TOF test */

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


  ww = 0.;
  nbins=1500;
  x1 = 500.;
  x2 = 10000.;
  x22 = 250.;

  /*
  for(i1=1; i1<=6; i1++)
  {
	for(i2=1; i2<96; i2++)
	{
      idn = i1*100+i2;
      hbook1_(&idn,"TDC VALUE",&nbins,&x1,&x2,&ww,9);
	}
  }
  */

  for(i1=1; i1<=57; i1++)
  {
    idn = i1+1000;
    hbook1_(&idn,"TDC LEFT",&nbins,&x1,&x2,&ww,9);
    idn = i1+2000;
    hbook1_(&idn,"TDC LEFT",&nbins,&x1,&x2,&ww,9);
    idn = i1+3000;
    hbook1_(&idn,"TDC LEFT",&nbins,&x1,&x2,&ww,9);
    idn = i1+4000;
    hbook1_(&idn,"TDC LEFT",&nbins,&x1,&x2,&ww,9);
    idn = i1+5000;
    hbook1_(&idn,"TDC LEFT",&nbins,&x1,&x2,&ww,9);
    idn = i1+6000;
    hbook1_(&idn,"TDC LEFT",&nbins,&x1,&x2,&ww,9);

    idn = 100 + i1;
    hbook1_(&idn,"TDC RIGHT",&nbins,&x1,&x2,&ww,9);
    idn = 200 + i1;
    hbook1_(&idn,"ADC LEFT",&nbins,&x1,&x2,&ww,9);
    idn = 300 + i1;
    hbook1_(&idn,"ADC RIGHT",&nbins,&x1,&x2,&ww,9);
    idn = 400 + i1;
    hbook1_(&idn,"ADC-PED LEFT",&nbins,&x1,&x22,&ww,9);
    idn = 500 + i1;
    hbook1_(&idn,"ADC-PED RIGHT",&nbins,&x1,&x22,&ww,9);
  }




  sprintf(strr,"OPEN INPUT UNIT=1 FILE='%s' ",argv[1]);
  printf("fparm string: >%s<\n",strr);
  status = fparm_(strr,strlen(strr));
  for(iev=0; iev<20000; iev++)
  {

    frbos_(bcs_.iw,&tmp1,"E",&iret,1);



	/* vpk request 
    for(i1=0; i1<24; i1++) xx[i1] = 0.;
    if((ind1=bosNlink(bcs_.iw,"STN0",0)) > 0)
    {
      int ncol1,nrow1;
      int id,idh,tdcl,tdcr,adcl,adcr,tdc;

      ncol1 = bcs_.iw[ind1-6];
      nrow1 = bcs_.iw[ind1-5];
      offset = 0;

      buf16 = (unsigned short *)&bcs_.iw[ind1];
      for(k=0; k<nrow1; k++)
      {
        id = ((*buf16) & 0xFF);
        buf16 ++;
        tdc = *buf16 ++;

        xx[id-1] = (float)tdc;

      }
    }

    ww = 1.0;
    if(xx[0] > 0.)
	{
	  for(i1=1; i1<24; i1++)
	  {
        if(xx[i1] > 0.)
        {
          tmpx = (xx[0]-xx[i1]);
          idn=i1+1;
          hf1_(&idn,&tmpx,&ww);
		}
	  }
	}
	*/

	/*
goto peds;
	*/
	/*
    if((ind1=bosNlink(bcs_.iw,"SC  ",1)) > 0)
    {
      int ncol1,nrow1;
      int id,idh,tdcl,tdcr,adcl,adcr,tdc,lr;

      ncol1 = bcs_.iw[ind1-6];
      nrow1 = bcs_.iw[ind1-5];
      nw = nrow1;
      offset = 0;
      ww = 1.0;

      buf16 = (unsigned short *)&bcs_.iw[ind1];
      for(k=0; k<nrow1; k++)
      {
        id = ((*buf16) & 0xFF);
        buf16 ++;
        tdcl = *buf16 ++;
        adcl = *buf16 ++;
        tdcr = *buf16 ++;
        adcr = *buf16 ++;

        ADCFILL(1);

      }
    }
	*/

    if((ind1=bosNlink(bcs_.iw,"CCT ",1)) > 0)
    {
      int ncol1,nrow1;
      int id,idh,tdcl,tdcr,adcl,adcr,tdc,lr;

      ncol1 = bcs_.iw[ind1-6];
      nrow1 = bcs_.iw[ind1-5];
      nw = nrow1;
      offset = 0;
      ww = 1.0;

      buf16 = (unsigned short *)&bcs_.iw[ind1];
      for(k=0; k<nrow1; k++)
      {
        id = ((*buf16) & 0xFF);
        lr = ((*buf16)>>8) & 0x1;
        buf16 ++;
        tdc = *buf16 ++;

        TDCFILL(1);

      }
    }

peds:


	/*
goto uuu;
	*/
    if((ind1=bosNlink(bcs_.iw,"CCT ",2)) > 0)
    {
      int ncol1,nrow1;
      int id,tdcl,tdcr,adcl,adcr,tdc,lr;

      ncol1 = bcs_.iw[ind1-6];
      nrow1 = bcs_.iw[ind1-5];
      nw = nrow1;
      offset = 0;
      ww = 1.0;

      buf16 = (unsigned short *)&bcs_.iw[ind1];
      for(k=0; k<nrow1; k++)
      {
        id = ((*buf16) & 0xFF);
        lr = ((*buf16)>>8) & 0x1;
        buf16 ++;
        tdc = *buf16 ++;
        /*adcl = *buf16 ++;
        tdcr = *buf16 ++;
        adcr = *buf16 ++;*/

        TDCFILL(2);

      }
    }
    if((ind1=bosNlink(bcs_.iw,"CCT ",3)) > 0)
    {
      int ncol1,nrow1;
      int id,tdcl,tdcr,adcl,adcr,tdc,lr;

      ncol1 = bcs_.iw[ind1-6];
      nrow1 = bcs_.iw[ind1-5];
      nw = nrow1;
      offset = 0;
      ww = 1.0;

      buf16 = (unsigned short *)&bcs_.iw[ind1];
      for(k=0; k<nrow1; k++)
      {
        id = ((*buf16) & 0xFF);
        lr = ((*buf16)>>8) & 0x1;
        buf16 ++;
        tdc = *buf16 ++;
        /*adcl = *buf16 ++;
        tdcr = *buf16 ++;
        adcr = *buf16 ++;*/

        TDCFILL(3);
      }
    }
    if((ind1=bosNlink(bcs_.iw,"CCT ",4)) > 0)
    {
      int ncol1,nrow1;
      int id,tdcl,tdcr,adcl,adcr,tdc,lr;

      ncol1 = bcs_.iw[ind1-6];
      nrow1 = bcs_.iw[ind1-5];
      nw = nrow1;
      offset = 0;
      ww = 1.0;

      buf16 = (unsigned short *)&bcs_.iw[ind1];
      for(k=0; k<nrow1; k++)
      {
        id = ((*buf16) & 0xFF);
        lr = ((*buf16)>>8) & 0x1;
        buf16 ++;
        tdc = *buf16 ++;
        /*adcl = *buf16 ++;
        tdcr = *buf16 ++;
        adcr = *buf16 ++;*/

        TDCFILL(4);

      }
    }
    if((ind1=bosNlink(bcs_.iw,"CCT ",5)) > 0)
    {
      int ncol1,nrow1;
      int id,tdcl,tdcr,adcl,adcr,tdc,lr;

      ncol1 = bcs_.iw[ind1-6];
      nrow1 = bcs_.iw[ind1-5];
      nw = nrow1;
      offset = 0;
      ww = 1.0;

      buf16 = (unsigned short *)&bcs_.iw[ind1];
      for(k=0; k<nrow1; k++)
      {
        id = ((*buf16) & 0xFF);
        lr = ((*buf16)>>8) & 0x1;
        buf16 ++;
        tdc = *buf16 ++;
        /*adcl = *buf16 ++;
        tdcr = *buf16 ++;
        adcr = *buf16 ++;*/

        TDCFILL(5);

      }
    }
    if((ind1=bosNlink(bcs_.iw,"CCT ",6)) > 0)
    {
      int ncol1,nrow1;
      int id,tdcl,tdcr,adcl,adcr,tdc,lr;

      ncol1 = bcs_.iw[ind1-6];
      nrow1 = bcs_.iw[ind1-5];
      nw = nrow1;
      offset = 0;
      ww = 1.0;

      buf16 = (unsigned short *)&bcs_.iw[ind1];
      for(k=0; k<nrow1; k++)
      {
        id = ((*buf16) & 0xFF);
        lr = ((*buf16)>>8) & 0x1;
        buf16 ++;
        tdc = *buf16 ++;
        /*adcl = *buf16 ++;
        tdcr = *buf16 ++;
        adcr = *buf16 ++;*/

        TDCFILL(6);

      }
    }

uuu:

    if(iret == -1 || iret > 0)
    {
      printf(" End-of-File flag, iret =%d\n",iret);
      goto a1113;
    }
    else if(iret < 0)
    {
      printf(" Error1, iret =%d\n",iret);
      goto a1113;
    }
    if(iret != 0)
    {
      printf(" Error2, iret =%d\n",iret);
    }
    bdrop_(bcs_.iw,"E",1);
    bgarb_(bcs_.iw);
  }
a1113:

  fparm_("CLOSE",5);

  /* closing HBOOK file */
  idn = 0;
  hrout_(&idn,&icycle," ",1);
  /*hprint_(&idn);*/
  hrend_("NTUPEL", 6);



  exit(0);

  /* end of TOF stuff */

}








