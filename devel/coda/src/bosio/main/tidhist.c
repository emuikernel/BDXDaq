
/* tidhist.c */
 
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


main(int argc, char **argv)
{
int nr,sec,layer,strip,nl,ncol,nrow,i,j,l,l1,l2,ichan,nn,iev;
int iev_spect=0, iev_START=1, iev_STOP=100000;
int N_base[16], b_data[16];
int offset_now=-1, offset_new=-1;
int ind,ind1,ind2,status,status1,handle,handle1,k,m;
int scal,nw,scaler,scaler_old;
unsigned int hel,str,strob,helicity,strob_old,helicity_old, tgbi1;
int tmp1 = 1, tmp2 = 2, iret, bit1, bit2, crate, slot, channel;
float *bcsfl, rndm_();
char title[100], strr[1000];
float sss[16], fff[16], ssst, ffft;

int ecgood1[7][7][37];
int ecbad1[7][7][37];

static int syn[32], id;

char *HBOOKfile = "tidhist.his";
int nwpawc,lun,lrec,istat,icycle,idn,nbins,nbins1,igood,offset;
int lookup[32] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                  25,27,29,31,33,35, 2, 4, 6, 8,10,12,14,16,18,20};
float x1,x2,y1,y2,ww,tmpx,tmpy,ttt,ref;
unsigned short *tlv1, *tlv2, *buf16;


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


  nbins = 4000;
  x1 = /*150.*/0.;
  x2 = /*350.*/4000.;
  ww = 0.;
  for(slot=2; slot<=21; slot++)
  {  
    for(channel=0; channel<16; channel++)
    {
      idn = slot*100 + channel;
      sprintf(title,"BaseLine sl%02d ch%02d",slot,channel);
      hbook1_(&idn,title,&nbins,&x1,&x2,&ww,strlen(title));
    }
  }

  /*
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

  for(iev=iev_START; iev<iev_STOP; iev++)
  {
    frbos_(bcs_.iw,&tmp1,"E",&iret,1);


if(iev < 1) goto nextevent;
/*
 printf("iev=%d\n",iev);
*/

	/* FADC RAW bank */

    if((ind1=bosNlink(bcs_.iw,"RC29",0)) > 0)
    {
      unsigned int *b32, *end;
      unsigned short *b16;
      int nsamples,notvalid,edge,data,count,ncol1,nrow1;
      int oldslot = 100;
      int ndata0[22], data0[21][8];
      int baseline, sum;

      float yyy;

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
	  /*
      printf("nw=%d, b32=0x%08x, end=0x%08x\n",nw,b32,end);
	  */
      for(i=0; i<16; i++) sss[i] = 0.;
      /*b32 += 8;*/ /* skip unrelated stuff */
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
		  /*
	  printf("%6d",data);
		  */
          notvalid = (b16[0]>>14)&0x1;
          if(notvalid>0) printf("not valid adc\n"); 

		  if(k<5) yyy += (float)data;
          if(k==4)
          {
            yyy = yyy / 5.;
        	tmpx = yyy;
	        ww   = 1.;
	        idn  = 100*slot+channel;
            if(slot<3||slot>20||slot==11||slot==12||channel<0||channel>15)
            {
              printf("ERROR: idn=%d tmpx=%f ww=%f\n",idn,tmpx,ww);
              printf("BUFFER:\n");
              b32 = (unsigned int *)&bcs_.iw[ind1];
			  printf("0x%08x 0x%08x %d\n",b32,end,(end-b32)/4);
			  exit(0);
              while(b32 < end)
			  {
                printf("[%d][%d] %d\n",
					   (b32[0]>>24)&0xFF,(b32[0]>>16)&0xFF,(b32[0])&0xFFFF);
				b32 ++;
			  }
              exit(0);
			}
	        hf1_(&idn,&tmpx,&ww);
		  }

		  /*
	      if(k>=5)
	      {
	        tmpx = (float)k;
	        ww   = (float)data-yyy;
	        idn  = channel+1;
	        hf1_(&idn,&tmpx,&ww);
            if(k>10 && k<50) sss[channel] += ww;
	      }
		  */

          b16 ++;
	    }
		/*	
        printf("\n");
		*/
        b32 = (unsigned int *)b16;
      }

	  /*
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
*/
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

















