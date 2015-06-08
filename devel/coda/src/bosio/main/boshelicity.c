/* boshelicity.c - data file helicity test;
  check correspondence between HLS bank and TGBI(first word) bank;
  compare HLS with TGBI from current or previous event because of
  HLS bank can be injected into the data stream little later
*/
 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
 
#include "bosio.h"

#define NBCS 700000
static int jw[NBCS];



#define DEBUG




/**********************/
/**********************/
/**********************/

#include "cinclude/helicity.c"

/**********************/
/**********************/
/**********************/

#define FORCE_RECOVERY \
  forceRecovery(); \
  quad = -1; \
  strob = -1; \
  helicity = -1; \
  quad_old = -1; \
  strob_old = -1; \
  helicity_old = -1; \
  quad_old2 = -1; \
  strob_old2 = -1; \
  helicity_old2 = -1; \
  remember_helicity[0]=remember_helicity[1]=remember_helicity[2]=-1; \
  done = -1; /* will be change to '0' in the beginning of quartet and to '1' when prediction is ready */ \
  offset = 0


void
main(int argc,char **argv)
{
  int nr,nl,ncol,nrow,i,iev,ind,ind0,ind1,ind2,ind3,status,status1,handle,handle1,k;
  int scal,nw,scaler,scaler_old,tmp1=1,iret,neednextevent;
  unsigned int hel,count1,str,strob,helicity,quad,qua;
  unsigned int strob_old,helicity_old,quad_old,strob_old2,helicity_old2,quad_old2;
  unsigned int helicity1, strob1, quad1, offset, quadextr[4];
  int done;
  int present_reading;
  int skip = 0;
  int final_helicity;
  int remember_helicity[3];

  int predicted_reading;      /* prediction of present reading */
  int present_helicity;       /* present helicity (using prediction) */
  unsigned int iseed;         /* value of iseed for present_helicity */
  unsigned int iseed_earlier; /* iseed for predicted_reading */

  /* following for 1 flip correction */
  unsigned int offset1;
  int pred_read;
  int pres_heli;
  int tmp0, temp0;

  char str3[256];

  if(argc != 2)
  {
    printf("Usage: boshelicity <filename>\n");
    exit(1);
  }

  printf("boshelicity reached\n");
  bosInit(jw,NBCS);


  /********************************************************
     scan over HLS banks and compare with TGBI
     in the same or previous event
  *********************************************************/

  sprintf(str3,"OPEN INPUT UNIT=1 FILE=\"%s\"",argv[1]);
  printf("filename >%s<\n",str3);
  fparm_(str3, strlen(str3));

  iev = 0;





  FORCE_RECOVERY;
  for(;;)
  {
    frbos_(jw,&tmp1,"E",&iret,1);
    if(iret == -1 || iret > 0)
    {
      printf(" End-of-File flag, iret =%d\n",iret);
      goto a10;
    }
    else if(iret < 0)
    {
      printf(" Error1, iret =%d\n",iret);
      goto a10;
    }
    iev ++;

	/*
if(iev < 350)
{
  bdrop_(jw,"E",1);
  bgarb_(jw);
  continue;
}
*/

	/****************************************************************/
	/****************************************************************/
	/****************************************************************/


	if((ind1 = etNlink(jw,"HEAD",0))<=0)
	{
      printf("ERROR: no HEAD bank\n");
	}
    else
	{
      if(jw[ind1+4]<1 || jw[ind1+4]>9)
      {
        /*printf("non-physics evebt (type=%d) - skip\n",jw[ind1+4]);*/
        continue;
	  }
      else if(jw[ind1+6]==15)
	  {
        printf("HELICITY EVENT: ");
        if( (ind=etNlink(jw,"HLS ",0))>0 && (ind0=etNlink(jw,"HLS ",1))>0 )
        {
          ncol = etNcol(jw,ind);
          nrow = etNrow(jw,ind);
          /*printf("ncol=%d nrow=%d\n",ncol,nrow);*/
          for(i=0; i<nrow; i++)
          {
            str = (jw[ind+i*ncol]&0x80000000)>>31;
            hel = (jw[ind+i*ncol]&0x40000000)>>30;
            qua = (jw[ind0+i*ncol]&0x40000000)>>30;
            count1 = jw[ind+i*ncol]&0xFFFFFF;
			if(count1>1010)
			{		  
              printf("strob=%1d helicity=%1d quad=%1d\n",str,hel,qua);
			}
		  }
		}
        else
		{
          printf("SEVERE ERROR: AT LEAST ONE 'HLS ' BANK IS MISSING\n");
		}
	  }
      else
	  {
        if((ind2 = etNlink(jw,"TGBI",0))>0)
        {
          strob =    (jw[ind2]>>12)&0x1;
          helicity = (jw[ind2]>>13)&0x1;
          quad =     (jw[ind2]>>15)&0x1;
          printf("PHYSICS EVENT: strob=%1d helicity=%1d quad=%1d\n",strob,helicity,quad);
	    }
        else
        {
          printf("ERROR: no TGBI\n");
        }
	  }
	}

    bdrop_(jw,"E",1);
    bgarb_(jw);
  }
a10:

  fparm_("CLOSE",5);

  exit(0);
}


