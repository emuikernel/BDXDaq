
/* bosnoise.c - test program for Drift Chamber noise reduction */
 
#include <stdio.h>
#include <math.h>
 
#include "bosio.h"
#define NBCS 700000
#include "bcs.h"

#include "prlib.h"
#include "sglib.h"


int
main(int argc, char **argv)
{
  BOSIO *handle, *handle1;
  int i, isec, ind, status, slcount, secount;
  float *bcsfl;

  int layershifts[6] = {0, 2, 3, 3, 4, 4};
  /*int layershifts[6] = {0, 5, 6, 7, 8, 9};*/

  /*
  if(argc != 2)
  {
    printf("Usage: bosnoise <fpack_filename>\n");
    exit(1);
  }
  */

  bcsfl = (float*)bcs_.iw;
  bosInit(bcs_.iw,NBCS);

  /*
  sprintf(strr,"OPEN INPUT UNIT=1 FILE='%s' ",argv[1]);
  printf("fparm string: >%s<\n",strr);
  status = fparm_(strr,strlen(strr));
  */

  status = bosOpen("bigfile","r",&handle);
  printf("after bosOpen(bigfile) status=%d\n",status);

  status = bosOpen("bosnoise.dat","w",&handle1);
  printf("after bosOpen(bosnoise.dat) status=%d\n",status);
  if(status) exit(1);


  sginit(); /* noise reduction initialization */


  while((status=bosRead(handle,bcs_.iw,"E")) == 0)
  {
    /* last parameter - the number of SL required */  
    secount = 6;
    for(isec=1; isec<=6; isec++)
    {
      slcount = sgremovenoise(bcs_.iw, isec, 6, layershifts);
      if(slcount < 3) /* at least 3 superlayers are required, otherwise delete sec */
      {
        secount--;
		/*
        bosNdrop(bcs_.iw,"DC0 ",isec);
        bosNdrop(bcs_.iw,"CC  ",isec);
        bosNdrop(bcs_.iw,"EC  ",isec);
        bosNdrop(bcs_.iw,"SC  ",isec);
		*/
      }
	}

    if(secount>0) bosWrite(handle1,bcs_.iw,"E");
    if((i=bosLdrop(bcs_.iw,"E")) < 0)
    {
      printf("Error in bosLdrop, number %d\n",i);
      exit(1);
    }
    if((i=bosNgarbage(bcs_.iw)) < 0)
    {
      printf("Error in bosNgarbage, number %d\n",i);
      exit(1);
    }
  }
  status = bosWrite(handle1,bcs_.iw,"0"); 
  status = bosClose(handle);
  status = bosClose(handle1);

  exit(0);
}










