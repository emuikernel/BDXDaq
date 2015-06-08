
/* dchist.c */
 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
 

#include "bosio.h"

/*
#define NBCS 700000
#include "bcs.h"
*/

#define RADDEG 57.2957795130823209
#define NWPAWC 10000000 /* Length of the PAWC common block. */
#define LREC 1024       /* Record length in machine words. */
#define EINTERVAL 90000 /* Number of events between error checking */
#define THRESHOLD 0.37  /* How low the percent change a given 6x6 
                           grid of layers and wires can be before being
                           flagged as an error*/

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

static int first = 1;

int
juanlib(int *jw)
{
  static int ended = 0;
  static int iev;

  int nr,sec,layer,strip,wire,nl,ncol,nrow,i,j,l,l1,l2,ichan,nn;
  int crate,slot,connector;
  int ind,ind1,ind2,status,status1,handle,handle1,k,m,rocid;
  int scal,nw,scaler,scaler_old, counter,dccount[7],rccount[7];
  unsigned int hel,str,strob,helicity,strob_old,helicity_old, tgbi1;
  int tmp1 = 1, tmp2 = 2, iret, bit1, bit2, nleading, ntrailing;
  char strr[1000], bankname[5], histname[128];
  static int syn[32], id;
  MTDCHead *mtdchead;
  MTDC *mtdc;
  unsigned int leading[20000], trailing[20000];

  char *HBOOKfile = "dcjuan.his";
  int nwpawc,lun,lrec,istat,icycle,idn,nbins,nbins1,igood,offset;
  float x1,x2,y1,y2,ww,tmpx,tmpy,ttt,ref;
  unsigned short *tlv1, *tlv2, *buf16;

  int lyr,wir;
  float ratio;
  int htotal=0,errors=0,lyrcount=0,sectcount=0;
  section sectlist[6], reference[6], errorlist[6];

  if(first)
  {
    first = 0;

    for(sec=0; sec<=5; sec++){
	  sectlist[sec].hcounter=0;
	  for(wir=0;wir<=31;wir++){
	    for(lyr=0; lyr<=5;lyr++){
		  sectlist[sec].WLlist[wir][lyr]=0;
		  reference[sec].WLlist[wir][lyr]=0;
		  errorlist[sec].WLlist[wir][lyr]=0;
	    }
	  }
    }

    printf(" dchist reached  !\n");


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
      return(0);
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

    nbins=32;
    x1 = 0.;
    x2 = 32.;
    nbins1=6;
    y1 = 0.;
    y2 = 6.;
    ww = 0.;
    for(idn=301; idn<=306; idn++)
    {
      sprintf(histname,"Load normalized SEC%1d",idn-300);
      hbook2_(&idn,histname,&nbins,&x1,&x2,&nbins1,&y1,&y2,&ww,strlen(histname));
    }


  }





  if(ended) return(0);

  if((ind1=etNlink(jw,"HEAD",0)) < 0) return(0);
  else
  {
    unsigned int *fb;
    int run,event,time,type;

    fb = (unsigned int *)&jw[ind1];
	/*
    printf("HEAD: bla=%d run=%d event=%d time=%d bla=%d bla=%d type=%d bla=%d\n",
      fb[0],fb[1],fb[2],fb[3],fb[4],fb[5],fb[6],fb[7]);
	*/
    run=fb[1];
    event=fb[2];
    time=fb[3];
    type=fb[6];

    if(type>14 || type<1) return(0);
  }

  iev ++;
  if(iev < EINTERVAL*2)
  {
    if(!(iev%15000)) printf("===== Event no. %d\n",iev);

	/*COPYING TO REFERENCE AFTER FIRST EVENT INTERVAL*/
	if(iev==EINTERVAL){
	  for(sec=0; sec<=5; sec++){
		for(wir=0;wir<=31;wir++){
		  for(lyr=0; lyr<=5;lyr++){
			reference[sec].WLlist[wir][lyr]=sectlist[sec].WLlist[wir][lyr];
			sectlist[sec].WLlist[wir][lyr]=0;
		  }
		}
	  }
	}

	/*COMPARE WITH REFERENCE TO SCAN FOR DROPS/JUMPS AND MARK IN ERRORLIST*/
	if( (!(iev%EINTERVAL)) && iev!=EINTERVAL && iev!=0){
	  for(sec=0; sec<=5; sec++){
		for(wir=0;wir<=31;wir++){
		  for(lyr=0; lyr<=5;lyr++){

		  for(sec=0; sec<=5; sec++){
			for(lyr=0;lyr<=5;lyr++){
			  for(wir=0; wir<=31;wir++){
				printf("count for sector:%d  superlayer:%d  group: %d      hits:%d\n", sec+1, lyr+1, wir+1, sectlist[sec].WLlist[wir][lyr]);
			
			  }
			}
		  }

          idn=sec+301;
          tmpx = wir;
          tmpy = lyr;
		  ratio = (float) sectlist[sec].WLlist[wir][lyr]/(float) reference[sec].WLlist[wir][lyr];

          if(reference[sec].WLlist[wir][lyr]<=300) ww = 0.;
          else
          {
            ww = ratio;
            hf2_(&idn,&tmpx,&tmpy,&ww);
		  }

		   	if(ratio <THRESHOLD && reference[sec].WLlist[wir][lyr] > 300){
			   errors++;
			   errorlist[sec].WLlist[wir][lyr] = sectlist[sec].WLlist[wir][lyr];
			  
			}
		   	if(ratio >(2-THRESHOLD) && reference[sec].WLlist[wir][lyr] > 300){
			   errors++;
			   errorlist[sec].WLlist[wir][lyr] = sectlist[sec].WLlist[wir][lyr];
			 
			}
		  }
		}
	  }
	  /*SEND ALARM IF THERE ARE GREATER THAN 4 ERRORS IN TIME INTERVAL*/
	  if(errors>=100){
		printf("OVER 100 ERRORS, MAJOR DROPS OF %d PERCENT OR MORE\nIF NOT COINCIDING WITH DROP IN BEAM CURRENT BE ALARMED\n", (100*(1-THRESHOLD)));
	  }
	  if(errors>4 && errors<100){

		/*CHECKS IF A SUPERLAYER OR SECTION IS OUT*/
	    for(sec=0; sec<=5; sec++){

		  sectcount=0;

		  for(lyr=0; lyr<=5;lyr++){

			lyrcount=0;

			for(wir=0;wir<=31;wir++){
			
				if(errorlist[sec].WLlist[wir][lyr]!=0){
				  lyrcount++;
				  sectcount++;
				  }
			  
			}
			/*IF THERE IS A MAJOR DROP IN SUPERLAYER SET VALUES TO -1
			AS TO CIRCUMVENT 6X6 ALARMS*/
			if(lyrcount>4){
			  for(wir=0;wir<=31;wir++){
				errorlist[sec].WLlist[wir][lyr]=-1;
			  }
			  printf("ALARM:DROP IN SUPERLAYER section:%d, superlayer:%d, %d of 32 sections dropped\n",sec+1,lyr+1,lyrcount);
			  sectcount=0;
			}
		  }
		  if(sectcount>4){
			errorlist[sec].hcounter++;
			printf("WARNING:5 or more 6x6 wire units dropped in section %d\n",sec+1);
		  }
		}

		/*6x6 ALARMS*/
		 for(sec=0; sec<=5; sec++){
		   for(wir=0;wir<=31;wir++){
			 for(lyr=0; lyr<=5;lyr++){

			   ratio =  (float) errorlist[sec].WLlist[wir][lyr]/(float) reference[sec].WLlist[wir][lyr];

			   /*PRINTS ERRORS IN SECTIONS WITH 5 OR MORE ERRORS*/
			   if(errorlist[sec].WLlist[wir][lyr]>0 && errorlist[sec].hcounter>0){
			     if(ratio<THRESHOLD){
				   printf("WARNING:DROP in section:%d, wires:%d-%d, layers:%d-%d, ",sec+1,wir*6,(wir+1)*6, lyr*6, (lyr+1)*6);
				   printf("drop of %f percent\n",100*(1- (float) sectlist[sec].WLlist[wir][lyr]/(float) reference[sec].WLlist[wir][lyr]));
				   //printf("reference is at:%d and current event is at: %d\n", reference[sec].WLlist[wir][lyr], sectlist[sec].WLlist[wir][lyr]);
				 }

				 if(ratio >(2-THRESHOLD)){ 
			      printf("WARNING:JUMP in section:%d, wires:%d-%d, layers:%d-%d, ",sec+1,wir*6,(wir+1)*6, lyr*6, (lyr+1)*6);
				  printf("increase of %f percent\n",-100*(1- (float) sectlist[sec].WLlist[wir][lyr]/(float) reference[sec].WLlist[wir][lyr]));
                  // printf("reference is at:%d and current event is at: %d\n", reference[sec].WLlist[wir][lyr], sectlist[sec].WLlist[wir][lyr]);
				 }

			   }
			 }
		   }
		 }
	  }
	
	  errors=0;
	  
	  for(sec=0; sec<=5; sec++){
		for(wir=0;wir<=31;wir++){
		  for(lyr=0; lyr<=5;lyr++){
			sectlist[sec].WLlist[wir][lyr]=0;
			errorlist[sec].WLlist[wir][lyr]=0;
		  }
		}
	  }
	 
    }
	 

    for(sec=1; sec<=6; sec++)
    {
	  dccount[sec] = 0;

      if((ind1=etNlink(jw,"DC0 ",sec)) > 0)
      {
        unsigned int *fb, *fbend;
        int crate,slot,channel,edge,data,count,ncol1,nrow1;

        ncol1 = etNcol(jw,ind1);
        nrow1 = etNrow(jw,ind1);
        nw = nrow1;
        offset = 0;
	    
        /*printf("ncol=%d nrow=%d nw=%d\n",ncol1,nrow1,nw);*/
	    
        ww = 1.0;

        fb = (unsigned int *)&jw[ind1];

        fbend = fb + nrow1;
        counter = 0;
        while(fb < fbend)
        {
          layer = ((*fb)>>24)&0xFF;
          wire = ((*fb)>>16)&0xFF;
		  htotal++;

          idn=sec+200;
          tmpx = wire;
          tmpy = layer;
          hf2_(&idn,&tmpx,&tmpy,&ww);

		  sectlist[sec-1].hcounter++;
		  sectlist[sec-1].WLlist[(wire-1)/6][(layer-1)/6]++;


          if(layer<13) counter ++;

          fb ++;
	    }

        dccount[sec] = counter;
        tmpx = (float)counter;
        idn=sec+100;
        hf1_(&idn,&tmpx,&ww);

	  }

	}

	/* */

  }
  else
  {
    for(sec=0; sec<=5; sec++){
	  printf("section %d has a total of %d hits with a ratio of %f\n", sec+1, sectlist[sec].hcounter, (float) sectlist[sec].hcounter/(float) htotal);
    }  

  
    printf("total number of hits is: %d\n", htotal);


    /* closing HBOOK file */
    idn = 0;
    hrout_(&idn,&icycle," ",1);
    /*hprint_(&idn);*/
    hrend_("NTUPEL", 6);

    ended = 1;
  }

  /* end of v1190 test */

}
