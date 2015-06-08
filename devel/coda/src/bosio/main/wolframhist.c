
/* wolframhist.c */
 
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
 
#include "bosio.h"

#define NBCS 700000
#include "bcs.h"

#define RADDEG 57.2957795130823209
#define NWPAWC 5000000 /* Length of the PAWC common block. */
#define LREC 4096      /* Record length in machine words. */

struct {
  float hmemor[NWPAWC];
} pawc_;

struct {
  int iquest[100];
} quest_;



#define LENEVENT 9

main(int argc, char **argv)
{
  int nr,layer,strip,nl,ncol,nrow,i,j,l,l1,l2,ichan,nn,iev, ievgood;
  int ind,ind1,ind2,status,status1,handle,handle1,k,m;
  int scal,nw,scaler,scaler_old;
  unsigned int hel,str,strob,helicity,strob_old,helicity_old;
  int tmp1 = 1, tmp2 = 2, iret, bit1, bit2;
  float *bcsfl, rndm_();
  char strr[1000];

  char *HBOOKfile = "wolf.hbook";
  int nwpawc,lun,lrec,istat,icycle,idn,nbins,nbins1,igood,offset;
  float x1,x2,y1,y2,ww,tmpx,tmpy,ref;
  unsigned short *tlv1, *tlv2, *buf16;

  int lenevent = LENEVENT; /* the number of 32-bit words in event */
  int nprime = 4096;
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

  printf(" bosio_ctof reached !\n");

  if(argc != 2)
  {
    printf("Usage: bosio_ctof <datafile>\n");
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
  }


  /* create Ntuple */

  idn = 2;
  hbookn_(&idn,"WOLF",&lenevent,"NTUPEL",&nprime,chtags,strlen("WOLF"),6,8);

  /* create histograms */

  nbins = 4000;
  x1 = 0.;
  x2 = 4000.;
  ww = 0.;

  idn = 101;
  hbook1_(&idn,"ch0 tdc",&nbins,&x1,&x2,&ww,4);
  idn = 102;
  hbook1_(&idn,"ch1 tdc",&nbins,&x1,&x2,&ww,4);
  idn = 103;
  hbook1_(&idn,"ch2 tdc",&nbins,&x1,&x2,&ww,4);
  idn = 104;
  hbook1_(&idn,"ch3 tdc",&nbins,&x1,&x2,&ww,4);
  idn = 105;
  hbook1_(&idn,"ch4 tdc",&nbins,&x1,&x2,&ww,4);
  idn = 106;
  hbook1_(&idn,"ch5 tdc",&nbins,&x1,&x2,&ww,4);
  idn = 107;
  hbook1_(&idn,"ch6 tdc",&nbins,&x1,&x2,&ww,4);
  idn = 108;
  hbook1_(&idn,"ch7 tdc",&nbins,&x1,&x2,&ww,4);


  sprintf(strr,"OPEN INPUT UNIT=1 FILE='%s' ",argv[1]);
  printf("fparm string: >%s<\n",strr);
  status = fparm_(strr,strlen(strr));

  ww = 1.0;

  ievgood = 0;
  for(iev=0; iev<500000; iev++)
  {
    frbos_(bcs_.iw,&tmp1,"E",&iret,1);

    for(i=0; i<LENEVENT; i++) event[i] = 0.0;

    if((ind=bosNlink(bcs_.iw,"STN1",0)) > 0)
    {
      unsigned short *cc;

      ievgood++;

      ncol = bcs_.iw[ind-6];
      nrow = bcs_.iw[ind-5];
	  /*
      printf("CC: ncol=%d nrow=%d\n",ncol,nrow);
      */

      cc = (unsigned short *)&bcs_.iw[ind];
      for(k=0; k<nrow; k++)
      {
        int id;
        id = cc[0]; /*channel#*/
		/*printf("id=%d\n",id);*/

        tmpx = cc[1]; /*adc*/

        /*randomize adc value*/
		/*printf("\nbefor %f\n",tmpy);*/
        tmpx = tmpx + ((float)rand())/((float)RAND_MAX);
		/*printf("after %f\n",tmpy);*/

		if(id>=1&&id<=8)
        {
          event[id] = tmpx;
		  /*printf("%f\n",tmpx);*/
          idn=100+id;
          hf1_(&idn,&tmpx,&ww);
		}
        cc+=2;
      }

      event[0] = iev;
      idn = 2;
      hfn_(&idn,event);
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
