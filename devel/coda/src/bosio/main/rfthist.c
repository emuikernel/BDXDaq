
/* rfthist.c */
 
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
  int scal,nw,scaler,scaler_old,counter,ndccount,nrccount;
  unsigned int hel,str,strob,helicity,strob_old,helicity_old, tgbi1;
  int tmp1 = 1, tmp2 = 2, iret, bit1, bit2, leading[2000], trailing[2000], nleading, ntrailing;
  float *bcsfl, rndm_(),dccount[7],rccount[7];
  char strr[1000], bankname[5], histname[128];
  static int syn[32], id;
  MTDCHead *mtdchead;
  MTDC *mtdc;

  char *HBOOKfile = "test.his";
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



  nbins=1000;
  x1 = 0.;
  x2 = 10000.;
  ww = 0.;
  for(idn=1; idn<=2; idn++)
  {
    sprintf(histname,"Reference channel %1d",idn);
    hbook1_(&idn,histname,&nbins,&x1,&x2,&ww,strlen(histname));
  }

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





  sprintf(strr,"OPEN INPUT UNIT=1 FILE='%s' ",argv[1]);
  printf("fparm string: >%s<\n",strr);
  status = fparm_(strr,strlen(strr));
  for(iev=0; iev<100000; iev++)
  {
    frbos_(bcs_.iw,&tmp1,"E",&iret,1);

    if(!(iev%1000)) printf("===== Event no. %d\n",iev);

    if((ind1=bosNlink(bcs_.iw,"RFT ",0)) > 0)
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
      ndccount = 0;
      nrccount = 0;
      while(fb < fbend)
      {
        layer = ((*fb)>>24)&0xFF;
        wire = ((*fb)>>16)&0xFF;
		/*		
        printf("layer=%d, wire=%d tdc=%d\n",layer,wire,(*fb)&0xFFFF);
		*/
		  
        if(wire==6)
        {
          tmpx = (*fb)&0xFFFF;
          idn=1;
          hf1_(&idn,&tmpx,&ww);
          if(ndccount<7) dccount[ndccount++] = tmpx;
		}
        else if(wire==7)
        {
          tmpy = (*fb)&0xFFFF;
          idn=2;
          hf1_(&idn,&tmpy,&ww);
          if(nrccount<7) rccount[nrccount++] = tmpy;
		}
        else printf("ERROR: id=%d\n",wire);

        fb ++;
	  }


      
     l1 = -1;
     counter = 0;
     for(i=0; i<ndccount; i++)
	  {
        for(j=0; j<nrccount; j++)
  	    {
          tmpx = dccount[i];
          tmpy = rccount[j];

          /*printf("tmpx=%f tmpy=%f\n",tmpx,tmpy);*/
          if(l1 == -1)
		  {
            if( (tmpx-tmpy) < 10.)
			{
              l1 = i;
              l2 = j;
              counter = MIN(ndccount-l1,nrccount-l2);
              /*printf("l1=%d l2=%d counter=%d\n",l1,l2,counter);*/
			}
		  }

          idn=3;
          hf2_(&idn,&tmpx,&tmpy,&ww);
	    }
	  }

      for(i=l1,j=l2,k=0; k<counter; k++)
  	  {
        tmpx = dccount[i];
        tmpy = rccount[j];

        idn=4;
        hf2_(&idn,&tmpx,&tmpy,&ww);
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

















