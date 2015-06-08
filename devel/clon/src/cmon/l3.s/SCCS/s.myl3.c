h24930
s 00000/00000/00000
d R 1.2 01/11/19 18:54:36 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 clas/cmon/l3.s/myl3.c
e
s 00052/00000/00000
d D 1.1 01/11/19 18:54:35 boiarino 1 0
c date and time created 01/11/19 18:54:35 by boiarino
e
u
U
f e 0
t
T
I 1

/* myl3.c - level3 from Sergey Boyarinov */
/*
    Return: 8 LOW bits contains information of electron candidate;
            If Yes, bit 7 is 0;
                    bits 0-5 containes 1 in case of electron candidate in sector 1-6 
            If NO, then bit 7 = 1, bits 0-6 contains the reason number for rejection :
                        1 : ...
                        2 : ...
                        3 : ...
*/

#include <stdio.h> 
#include <string.h>
#include <math.h> 
#include "sdakeys.h"
SDAkeys keys;

int
l3_init3(int runnum)
{
  const int option[3] = {0,0,0};

  ecinit(runnum, option[0], option[1], option[2]);

  return(0);
}


/*
  Purpose and methods : Checks if there was an electron candidate in the event.

  Inputs : TGBI bank for sectors with level1 trigger.

*/

int
l3_event3(int *iw)
{
  int ret, cand;
  const float threshold[3] = {0.003, 0.01, 0.03}; /* GeV ? */

  cand = ecl3(iw, threshold, 0);

  return(cand);
}






E 1
