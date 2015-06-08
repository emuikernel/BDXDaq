/* cmonbat.c */


#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "sdabos.h"
#include "sdakeys.h"
#include "wcs.h"
SDAkeys sdakeys_;

int
main()
{
  int ifevb=1, Nevt, inter, iret, iwmin, iwmax;
  float pmin,pmax,charge,thetamin,thetamax,ltarget,rtarget,ztarget;
  int iucomp_(), tmp;

  /* Init FIRST (BCS) BOS array to store data */
  bosInit(bcs_.iw,NBCS);

  /* Init SECOND (WCS) BOS array to store geometry etc constants */
  bosInit(wcs_.jw,JNDIMB); /*for new geomerty*/

  /* set interactive flag to 0 */
  inter = 0;

  /* Initialization of SDA */
  cminit_();

  /* Event processing */
  tmp = 10;
  if( iucomp_("HSIM",&sdakeys_.levb,&tmp,4) != 0 )
  {
    printf("SIMULATION mode - DOES NOT WORK - road simulation only !!!\n");
    printf(" ---> use 'makefulldict'\n");
  }
  else
  {
	printf("RECONSTRUCTION mode\n");
    ifevb = 1;
    Nevt = sdakeys_.ltrig[1];
    cmloop_(bcs_.iw,&ifevb,&inter,&Nevt,&iret);
  }
  dcerun(bcs_.iw);
  dch2hbook();

  /* Closing of SDA */
  dclast_(bcs_.iw);

  exit(0);
}



