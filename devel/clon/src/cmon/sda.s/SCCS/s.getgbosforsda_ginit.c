h59497
s 00000/00000/00000
d R 1.2 01/11/19 19:01:57 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 clas/cmon/sda.s/getgbosforsda_ginit.c
e
s 00022/00000/00000
d D 1.1 01/11/19 19:01:56 boiarino 1 0
c date and time created 01/11/19 19:01:56 by boiarino
e
u
U
f e 0
t
T
I 1

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
E 1
