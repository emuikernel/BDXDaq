
/* dc12.c */
 
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

typedef struct section
 {
  int hcounter;
  int WLlist[32][6];
} section;


main(int argc, char **argv)
{
  int nr,sec,layer,strip,wire,nl,ncol,nrow,i,j,l,l1,l2,ichan,nn,iev,value;
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

  char *HBOOKfile = "dc12.his";
  int nwpawc,lun,lrec,istat,icycle,idn,nbins,nbins1,igood,offset;
  float x1,x2,y1,y2,ww,tmpx,tmpy,ttt,ref;
  unsigned short *tlv1, *tlv2, *buf16;

  int lyr,wir;
  int htotal=0,tester=0;
  section sectlist[6];

  printf(" dc12 reached  !\n");
 
  if(argc != 2)
  {
    printf("Usage: dc12 <fpack_filename>\n");
    exit(1);
  }


  /* BOS initialization */
  bcsfl = (float*)bcs_.iw;
  bosInit(bcs_.iw,NBCS);


  /* HBOOK initialization */
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
  for(idn=101; idn<=106; idn++)
  {
    sprintf(histname,"DC load REG1 SEC%1d",idn-100);
    hbook1_(&idn,histname,&nbins,&x1,&x2,&ww,strlen(histname));
  }


  nbins=192;
  x1 = 0.;
  x2 = 192.;
  nbins1=36;
  y1 = 0.;
  y2 = 36.;
  ww = 0.;
  for(idn=201; idn<=206; idn++)
  {
    sprintf(histname,"DC load SEC%1d",idn-200);
    hbook2_(&idn,histname,&nbins,&x1,&x2,&nbins1,&y1,&y2,&ww,strlen(histname));
  }




  sprintf(strr,"OPEN INPUT UNIT=1 FILE='%s' ",argv[1]);
  printf("fparm string: >%s<\n",strr);
  status = fparm_(strr,strlen(strr));
  for(iev=0; iev<900000; iev++)
  {
    frbos_(bcs_.iw,&tmp1,"E",&iret,1);

    if(!(iev%100)) printf("===== Event no. %d\n",iev);

    /*for(sec=1; sec<=6; sec++)*/
    sec = 2;
    {
      if((ind1=bosNlink(bcs_.iw,"DC0 ",sec)) > 0)
      {
        unsigned int *fb, *fbend;
        int crate,slot,channel,edge,data,count,ncol1,nrow1;

        ncol1 = bcs_.iw[ind1-6];
        nrow1 = bcs_.iw[ind1-5];
        nw = nrow1;
        offset = 0;
	    
        /*printf("ncol=%d nrow=%d nw=%d\n",ncol1,nrow1,nw);*/
	    
        /*printf("event %d\n",iev);*/

        fb = (unsigned int *)&bcs_.iw[ind1];
        fbend = fb + nrow1;
        while(fb < fbend)
        {
          layer = ((*fb)>>24)&0xFF;
          wire = ((*fb)>>16)&0xFF;
          value = (*(fb))&0xFFFF;
          /*printf("  layer=%2d wire=%3d value=%5d\n",layer,wire,value);*/

          idn=sec+200;
          tmpx = wire;
          tmpy = layer;
          ww = 1.0;
          hf2_(&idn,&tmpx,&tmpy,&ww);

          fb ++;
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




  for(sec=0; sec<=5; sec++){
	
	printf("section %d has a total of %d hits with a ratio of %f\n", sec+1, sectlist[sec].hcounter, (float) sectlist[sec].hcounter/(float) htotal);
  }  

  
  printf("total number of hits is: %d\n", htotal);







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
