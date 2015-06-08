
/* utkinit.c - KUIP/HIGZ initialization for interactive version */


#include <stdio.h>
#include <math.h>
#include "dclib.h"
#include "sdadraw.h"
#include "sdakeys.h"
SDAkeys sdakeys_;

#define NWKUIP 30000

extern igterm_;  /* EXTERNAL IGTERM */



void
utkinit_()
{
  utkinit();
}

void
utkinit()
{
  int itmp, itmp1, KWKTYP;
  float tmp1, tmp2;

  /* HIGZ constants */
  KWKTYP    = 1;
  sdadraw_.sda_dv[0] = 20.;
  sdadraw_.sda_dv[1] = 20.;
    sdadraw_.sda_vc[0] = 0.;
    sdadraw_.sda_vc[1] = 1.;
    sdadraw_.sda_vc[2] = 0.;
    sdadraw_.sda_vc[3] = 1.;
  sdadraw_.sda_wc[0] = 0.;
  sdadraw_.sda_wc[1] = 20.;
  sdadraw_.sda_wc[2] = 0.;
  sdadraw_.sda_wc[3] = 20.;

  /* Initialize KUIP and HIGZ graphics */
  itmp = NWKUIP;
  kuinit_(&itmp);
  kuterm_(&igterm_);

  /* Initialize HIGZ and request for station type */
  itmp = 0;
  iginit_(&itmp);
  itmp = KWKTYP;
  kuproi_("Terminal type > ",&itmp,16);
  printf("Terminal %d selected.\n",KWKTYP);

  /* Initialize Graphic Package Control */
  itmp = 6;
  itmp1 = KWKTYP;
  igsse_(&itmp,&itmp1);

  /* Set up the graphic viewport with max. world co-ordinates (WC = DC) */
  if(KWKTYP == 7878)
  {
    itmp=5;
    iswn_(&itmp,&sdadraw_.sda_wc[0],&sdadraw_.sda_wc[1],
                                &sdadraw_.sda_wc[2],&sdadraw_.sda_wc[3]);
    tmp1=sdadraw_.sda_vc[1]*.8;
    tmp2=sdadraw_.sda_vc[3]*.8;
    isvp_(&itmp,&sdadraw_.sda_vc[0],&tmp1,
                         &sdadraw_.sda_vc[2],&tmp2);
    iselnt_(&itmp);
  }
  else
  {
    itmp=3;
    iswn_(&itmp,&sdadraw_.sda_wc[0],&sdadraw_.sda_wc[1],
                                &sdadraw_.sda_wc[2],&sdadraw_.sda_wc[3]);
    isvp_(&itmp,&sdadraw_.sda_vc[0],&sdadraw_.sda_vc[1],
                                &sdadraw_.sda_vc[2],&sdadraw_.sda_vc[3]);
    iselnt_(&itmp);
  }

  /* Initialize Command Processor (KUIP) commands */
  sda_def_();

  /* Yield control to the User Interface */
  kuexec_("SET/PROMPT SDA >",20);
  kuwhat_();

  /* End HIGZ Session */
  igend_();

  /* DO NOT execute code in "sda_run" */
  sdakeys_.Nevt = 0;

  return;
}


