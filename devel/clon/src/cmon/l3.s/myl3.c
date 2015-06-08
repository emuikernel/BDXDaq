
/* myl3.c - level3 from Sergey Boyarinov */

#include <stdio.h> 
#include <string.h>
#include <math.h> 
#include "sdakeys.h"
SDAkeys keys;


/* use noise reduction algorithm to identify sectors without track segments */

#include "prlib.h"
#include "sglib.h"

int
l3_init3(int runnum)
{
  sginit(); /* noise reduction initialization */

  return(0);
}

int
l3_event3(int *iw)
{
  int cand, isec, slcount;
  int nsl = 3; /* minimum number of superlayers required */
  int layershifts[6] = {0, 2, 3, 3, 4, 4};
  /*int layershifts[6] = {0, 5, 6, 7, 8, 9};*/

  cand = 0;

  /* last parameter - the number of SL required */
  for(isec=1; isec<=6; isec++)
  {
    slcount = sgtrigger(iw, isec, nsl, layershifts);
    if(slcount >= nsl)
    {
      cand |= (1<<(isec-1));
    }
  }

  if(cand>0) /* keep event */;
  else       cand |= 0x80/* remove event */;

  return(cand);
}






/*
    Return: 8 LOW bits contains information of electron candidate;
            If Yes, bit 7 is 0;
                    bits 0-5 containes 1 in case of electron candidate in sector 1-6 
            If NO, then bit 7 = 1, bits 0-6 contains the reason number for rejection :
                        1 : ...
                        2 : ...
                        3 : ...
*/



int
l3_init3_1(int runnum)
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
l3_event3_1(int *iw)
{
  int ret, cand;
  const float threshold[3] = {0.003, 0.01, 0.03}; /* GeV ? */

  cand = ecl3(iw, threshold, 0);

  return(cand);
}






