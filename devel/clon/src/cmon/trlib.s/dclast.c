/* NEED CHECK !!!
   Purpose and Methods   : Finishing of SDA program.

   Inputs  :
   Outputs :
   Controls:

   Library belongs: libsda.a

   Calls: sda_dlast, ana_statp

   Created     19-OCT-1990  Bogdan Niczyporuk

   Called by sda_erun
*/

#include <stdio.h>
#include <time.h>

#include "uttime.h"
#include "dclib.h"
#include "sdakeys.h"
SDAkeys sdakeys_;

int iucomp_();
float sran_();

void
dclast_(int *jw)
{
  dclast(jw);
  return;
}

void
dclast(int *jw)
{
  int ierr, dtime, tmp, tmp1;
  float RNlast;
  char dtimes[25];

  /* Trigger, acceptance, reconstruction efficiencies */
printf("dclast 1\n");fflush(stdout);
  tmp = 10;
  if(iucomp_("SIM ",&sdakeys_.levb,&tmp,4) != 0)
  {
    dc_statp2();

    RNlast = sran_(&sdakeys_.iseed);
    printf("\n Last RAN = %10.8f\n",RNlast);
  }

  /* Print statistics for the used cuts (if Batch Mode) */
  tmp = 1;
  if(iucomp_("BAT ",&sdakeys_.lmode,&tmp,4) != 0) dc_statp();

  /* Write last DST buffer "DSTOUTPUT" for reconstructed events */
  tmp = 10;
  tmp1 = 2;
  if(iucomp_("WDST",&sdakeys_.levb,&tmp,4) != 0)
                                    fwbos_(jw,&tmp1,"0",&ierr,1);


  /* Print I/O statistic and close the all opened FPACK units */
  fpstat_();
  fparm_("CLOSE",5);

  /* Get JOB Time */

  getunixtime_(&dtime);
  {int i; for(i=0; i<25; i++) dtimes[i] = ' '; dtimes[24] = '\0';}
  getasciitime_(&dtime,dtimes,strlen(dtimes));
  printf("\n                 JOB End= %24.24s\n",dtimes);

  return;
}


