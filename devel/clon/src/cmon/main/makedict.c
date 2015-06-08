
/* makedict.c - generates pattern recognition dictionary starting from the target */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "prlib.h"

#include "sdabos.h"

#define ABS(x) ((x) < 0 ? -(x) : (x))
#define MIN(x,y) ((x) < (y) ? (x) : (y))
#define MAX(x,y) ((x) > (y) ? (x) : (y))

#define PMIN 0.02
#define PMAX 6.0

void
main(int argc, char **argv)
{
  int iwmin, iwmax, icharge, tmp, inter, ifevb, Nevt;
  float pmin, pmax, charge, thetamin, thetamax, Ltarget, Rtarget, Ztarget;
  char fname[1000];

  Ltarget = Rtarget = Ztarget = -100.; /* default values */

  if(argc != 7)
  {
    printf("Usage: makedict Pmin Pmax Charge Ltarget Rtarget Ztarget\n");
    printf("                 GeV/c            cm      cm      cm\n");
    printf("Output will go to the prlink.bos file in the current directory.\n");
    exit(0);
  }

  pmin   = atof(argv[1]);
  pmax   = atof(argv[2]);
  if(pmin < PMIN) pmin = PMIN;
  if(pmax > PMAX) pmax = PMAX;
  if(pmin > pmax) {printf("error in momentum - exit.\n"); exit(0);}

  icharge = atoi(argv[3]);
  if(icharge != 1 && icharge != -1 && icharge != 0)  {printf("error in charge - exit.\n"); exit(0);}
  charge = (float)icharge;

  Ltarget = atof(argv[4]);
  Rtarget = atof(argv[5]);
  Ztarget = atof(argv[6]);

  printf("\nParameters: Pmin=%9.3f     Pmax=%9.3f\n"
         "            Charge=%7.0f\n"
         "            Ltarget=%9.3f Rtarget=%9.3f Ztarget=%9.3f\n\n",
          pmin, pmax, charge, Ltarget,Rtarget,Ztarget);

  bosInit(bcs_.iw,NBCS);
  /*bosInit(wcs_.iw,NBCS);-for new geomerty*/

  cminit_();

  inter = 0;         /* no interactive for dictionary generating */
  ifevb = 2;
  Nevt = 1000000000; /* set maximum event to big number */
  sim_loop_(bcs_.iw,&ifevb,&inter,&Nevt,&iwmin,&iwmax,&pmin,&pmax,
            &charge,&thetamin,&thetamax,&Ltarget,&Rtarget,&Ztarget);
  dcerun(bcs_.iw);
  /*dch2hbook();*/
  dclast(bcs_.iw);

  strcpy(fname,"prlink");
  tmp = strlen(fname);
  sprintf((char *)&fname[tmp],"_%3.1f_%3.1f_\0",pmin,pmax);
  tmp = strlen(fname);
  if(charge < -0.1) fname[tmp++] = 'e';
  else              fname[tmp++] = 'p';
  fname[tmp] = '\0';
  strcat(fname,".bos");
  prwrite(fname);

  exit(0);
}










