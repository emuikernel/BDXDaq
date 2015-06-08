
/* makefulldict.c - generates pattern recognition dictionary starting from layer 1 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "prlib.h"

#include "sdabos.h"

#define ABS(x) ((x) < 0 ? -(x) : (x))
#define MIN(x,y) ((x) < (y) ? (x) : (y))
#define MAX(x,y) ((x) > (y) ? (x) : (y))

#define IWMIN 1
#define IWMAX 130
#define PMIN 0.02
#define PMAX 6.0
#define THETAMIN 1.
#define THETAMAX 140.

void
main(int argc, char **argv)
{
  int iwmin, iwmax, icharge, tmp, inter, ifevb, Nevt;
  float pmin, pmax, charge, thetamin, thetamax, ltarget, rtarget, ztarget;
  char fname[1000];

  if(argc != 6 && argc != 8)
  {
    printf("Usage: makefulldict IWmin IWmax Pmin Pmax Charge [THETAmin THETAmax]\n");
    printf("                                GeV/c             Degree\n");
    printf("Output will go to the prlink.bos file in the current directory.\n");
    exit(0);
  }

  iwmin  = atoi(argv[1]);
  iwmax  = atoi(argv[2]);
  if(iwmin < IWMIN) iwmin = IWMIN;
  if(iwmax > IWMAX) iwmax = IWMAX;
  if(iwmin > iwmax) {printf("error in wire number - exit.\n"); exit(0);}

  pmin   = atof(argv[3]);
  pmax   = atof(argv[4]);
  if(pmin < PMIN) pmin = PMIN;
  if(pmax > PMAX) pmax = PMAX;
  if(pmin > pmax) {printf("error in momentum - exit.\n"); exit(0);}

  icharge = atoi(argv[5]);
  if(icharge != 1 && icharge != -1 && icharge != 0)  {printf("error in charge - exit.\n"); exit(0);}
  charge = (float)icharge;

  if(argc == 8)
  {
    thetamin = atof(argv[6]);
    thetamax = atof(argv[7]);
    if(thetamin < THETAMIN) thetamin = THETAMIN;
    if(thetamax > THETAMAX) thetamax = THETAMAX;
    if(thetamin >= thetamax) {printf("error in theta - exit.\n"); exit(0);}
  }
  else
  {
    thetamin = THETAMIN;
    thetamax = THETAMAX;
  }

  printf("\nParameters: IWmin=%8d     IWmax=%8d\n"
         "            Pmin=%9.3f     Pmax=%9.3f\n"
         "            Charge=%7.0f\n"
         "            THETAmin=%5.1f     THETAmax=%5.1f\n",
          iwmin, iwmax, pmin, pmax, charge, thetamin, thetamax);

  bosInit(bcs_.iw,NBCS);
  /*bosInit(wcs_.iw,NBCS);-for new geomerty*/

  cminit_();

  inter = 0;         /* no interactive for dictionary generating */
  ifevb = 2;
  Nevt = 1000000000; /* set maximum event to big number */
  ltarget = -2.; /* if negative - start from 1st DC layer */
  /*
  sim_loop_(bcs_.iw,&ifevb,&inter,&Nevt,&iwmin,&iwmax,&pmin,&pmax,
            &charge,&thetamin,&thetamax,&ltarget,&rtarget,&ztarget);
  */
  sim_loop_fromlayer1_(bcs_.iw,&ifevb,&inter,&Nevt,&iwmin,&iwmax,&pmin,&pmax,
            &charge,&thetamin,&thetamax);

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










