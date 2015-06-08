
/* that function exist just to get GBOS from sda_geom.F */

#include <stdio.h>
#include "sdakeys.h"
SDAkeys sdakeys_;

void
getgbosforsda_ginit_(char *fname)
{
  char *parm;

  if((parm = getenv("CLON_PARMS")) == NULL)
  {
    printf("getgbosforsda_ginit_: environment variable CLON_PARMS is not set - exit.\n");
    exit(0);
  }
  sprintf(fname,"%s/TRLIB/%s",parm,sdakeys_.geomfile);
  /*printf("getgbosforsda_ginit_: >%s<\n",fname);*/

  return;
}
