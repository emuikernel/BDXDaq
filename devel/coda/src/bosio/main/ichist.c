
/* ichist.c */
 
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
  int ind,ind1,ind2,status,status1,handle,handle1,k,m,rocid;
  int scal,nw,scaler,scaler_old;
  unsigned int hel,str,strob,helicity,strob_old,helicity_old, tgbi1;
  int tmp1 = 1, tmp2 = 2, iret, bit1, bit2, leading[2000], trailing[2000], nleading, ntrailing;
  float *bcsfl, rndm_();
  char strr[1000], bankname[5], histname[128];
  static int syn[32], id;
  MTDCHead *mtdchead;
  MTDC *mtdc;

  char *HBOOKfile = "ic.his";
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



  nbins=820;
  x1 = 0.;
  x2 = 8200.;
  ww = 0.;
  for(i=1; i<=24; i++)
  {
	for(j=1; j<=24; j++)
    {
      sprintf(histname,"ADC: x=%2d y=%2d",i,j);
      idn = i*100+j;
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
  for(iev=0; iev<10000000; iev++)
  {
    frbos_(bcs_.iw,&tmp1,"E",&iret,1);

    if(!(iev%1000)) printf("===== Event no. %d\n",iev);

    igood = 0;
    if((tgbi1=bosNlink(bcs_.iw,"TGBI",0)) > 0)
    {
      unsigned int *fb;
      unsigned short *slow, *shigh;
      fb = (unsigned int *)&bcs_.iw[tgbi1];
      slow = (unsigned short *)fb;
      shigh = slow + 1;
      if( ((*fb>>8)&0x1) ) igood = *fb; /* look for bit 9 */
	  /*
      if((*fb>>8)&0xf) printf("TGBI: 0x%08x (high 0x%04x, low 0x%04x)\n",*fb,*shigh,*slow);
	  */
	}
    if(igood && (ind1=bosNlink(bcs_.iw,"IC  ",0)) > 0)
    {
      unsigned int *fb, *fbend;
      unsigned short *fb16, *fb16end;
      int crate,slot,channel,edge,data,ncol1,nrow1,adc;

      printf("igood=0x%08x\n",igood);

      ncol1 = bcs_.iw[ind1-6];
      nrow1 = bcs_.iw[ind1-5];
      offset = 0;
	  /*
      printf("ncol=%d nrow=%d\n",ncol1,nrow1);
	  */
      ww = 1.0;

      /*printf("\n");*/

      fb16 = (unsigned short *)&bcs_.iw[ind1];
      fb16end = fb16 + nrow1*ncol1;
      while(fb16 < fb16end)
      {
        layer = (fb16[0]>>8)&0xFF;
        wire = (fb16[0])&0xFF;
        adc = fb16[2];
		/*
        printf("layer=%d, wire=%d adc=%d\n",layer,wire,adc);
		*/
		  
        if(layer>0&&layer<25&&wire>0&&wire<25)
        {
          tmpx = adc;
          idn=wire*100+layer;
          hf1_(&idn,&tmpx,&ww);
		}
        else printf("ERROR: layer=%d wire=%d (0x%08x)\n",layer,wire,*fb);

        fb16 += 3;
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

















