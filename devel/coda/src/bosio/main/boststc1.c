/*DECK ID>, BOSTESTC. */
 
#include <stdio.h>
 
#define NBCS 700000
#include "bcs.h"
#include "bosio.h"
 
main()
{
  int nr, nl, ncol, nrow, i, l, l1, l2, ichan, nn, iev, ind, status, status1,
      handle, handle1, k;
  float *bcsfl, rndm_();

  printf(" boststc1 reached !\n");

  bcsfl = (float*)bcs_.iw; /* pointer for float data */
  bosInit(bcs_.iw,NBCS);

/* open FPACK file for writing */

  /*status = FParm(str1,&handle));*/
  status = bosOpen("./test.dat","w",&handle);

/* bank formats */

    bosNformat(bcs_.iw,"HEAD","10I");
    bosNformat(bcs_.iw,"RAWD","F");
    bosNformat(bcs_.iw,"CHEN","(I,F)");
	bosNformat(bcs_.iw,"TEST","(I,2(I,F))");

/* define event-list */

    bosLctl(bcs_.iw,"E=","HEADRAWDCHENTEST");

  nn = 5;

  for(iev=1; iev<=nn; iev++)
  {
 
/*    printf(" iev= %d\n",iev); */
 
/* int format */

    nr=0;
    ncol=10;
    nrow=1;
    ind=bosNcreate(bcs_.iw,"HEAD",nr,ncol,nrow);
    if(ind == 0) break;
    bcs_.iw[ind+1] = 4711;
    bcs_.iw[ind+2] = iev;
    bcs_.iw[ind+3] = 1;
/*
    datime_(&bcs_.iw[ind+8], &bcs_.iw[ind+9]);
*/
 
/* float format */
 
    ncol=10.0+10.0*rndm_();
    nrow=1;
    ind=bosNcreate(bcs_.iw,"RAWD",nr,ncol,nrow);
    if(ind == 0) break;
    for(i=0; i<ncol; i++)
    {
      bcsfl[ind+i]=rndm_(&i);
    }
 
/* mixed format */
 
    ncol=6;
    nrow=1;
    nl=5.0+5.0*rndm_();
    for(l=1; l<=nl; l++)
    {
      nr=nr+1+2*rndm_(&l);
      ind=bosNcreate(bcs_.iw,"CHEN",nr,ncol,nrow);
      if(ind == 0) break;
      ichan=256*rndm_();
 
      bcs_.iw[ind] = ichan;
      bcsfl[ind+1]=rndm_(&l);
 
      bcs_.iw[ind+2] = ichan+1;
      l1=l+1;
      l2=l-1;
      bcsfl[ind+3]=rndm_(&l1)+rndm_(&l2);
 
      bcs_.iw[ind+4] = ichan+2;
      l1=l+2;
      bcsfl[ind+5]=rndm_(&l1);
    }
 
    ncol=5;
    nrow=10;
    ind=bosNcreate(bcs_.iw,"TEST",nr,ncol,nrow);
    if(ind == 0) break;
    for(i=0; i<nrow; i++)
    {
      bcs_.iw[ind]   = i;
      bcs_.iw[ind+1] = i*2;
      bcsfl[ind+2]   = (float)(i*2);
      bcs_.iw[ind+3] = i*4;
      bcsfl[ind+4]   = (float)(i*4);
      ind = ind + ncol;
    }

/*start_timer();*/
    status = bosWrite(handle,bcs_.iw,"E");
/*stop_timer(10000);*/

    /*bprnt_(bcs_.iw,"TEST",4);*/

    k=bosLdrop(bcs_.iw,"E");
    k=bosNgarbage(bcs_.iw);
 
  }

printf("done ...\n"); 
  status = bosWrite(handle,bcs_.iw,"0");
printf("done !\n"); 
  status = bosClose(handle);
  
  exit(0);
}
 
