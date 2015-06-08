/******************************************************************************
*
* ppcTimerLib - Routines to use the PowerPC Time Base hardware counter for 
*               timing resolution between 30-40 ns. 
*
*/

#ifdef VXWORKS
#include <vxWorks.h>
#else
#include <unistd.h>
#endif
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#ifdef VXWORKS

#include "ppcTimer.h"


/*
 * Calculate number of ticks equal to 1 microsecond
 *
 * The Time Base register and the Decrementer count at the same rate:
 * once per 4 System Bus cycles.
 *
 * e.g., 66666666 cycles     1 tick      1 second             16 tick
 *       ---------------  *  ------   *  --------          =  ----------
 *       second              4 cycles    1000000 microsec    microsec
 */

static unsigned int oneUsDelay = ((DEC_CLOCK_FREQ>>2) / 1000000);

static unsigned int ppcTimeBaseFreq = (DEC_CLOCK_FREQ>>2);


void
ppcTimeBaseGet(PPCTB *tb)
{

/* declare High and Low timebase values as a UINT assigned 
   to PPC registers 4 and 5 */

  register unsigned int tbL __asm__ ("r4");
  register unsigned int tbH __asm__ ("r5");

/*
 * read the time base counter (64 bits), put it in r4 and r5.
 */

  __asm__ (" mftb 4, 268");
  __asm__ (" mftb 5, 269");


/* Return the time base register values to the structure */
  tb->tbL = tbL;
  tb->tbH = tbH;

  return;
}

unsigned int
ppcTimeBaseFreqGet()
{

  /* Return the Bus freqency being used */
  return(ppcTimeBaseFreq<<2);

}

int
ppcTimeBaseFreqSet(unsigned int freq)
{

  if ((freq > 66000000) && (freq < 500000000)) {
    ppcTimeBaseFreq = (freq>>2);
    oneUsDelay = (ppcTimeBaseFreq / 1000000);
    return(0);
  }else{
    printf("Error: Bus Speed frequency = %d not in range\n",freq);
    return(-1);
  }

}

void
ppcTimeBaseZero()
{
  register unsigned int val __asm__ ("r3") = 0;

/*
 * Write Zeros into the timebase registers.
 */
  __asm__ (" sync");          
  __asm__ (" mtspr 285, 3"); /* Zero upper */
  __asm__ (" mtspr 284, 3"); /* zero lower */
  __asm__ (" isync");

  return;
}



double
ppcTimeBaseDuration(PPCTB *t1, PPCTB *t2)
{
  
  long long diff;
  double delta_t;

  if((t1 == NULL)||(t2 == NULL))
    return(0);

  diff = *(long long *)t2 - *(long long *)t1;

  delta_t = (diff/((float)ppcTimeBaseFreq));

  /*printf(" diff = %lli counts  delta_t = %.8f sec\n",diff,delta_t);*/

  return(delta_t);
}



void 
ppcUsDelay (unsigned int delay)		/* length of time in microsec to delay */
{
  long long baselineTickCount;
  long long terminalTickCount;
  long long currentTickCount;
  unsigned int ticksToWait;
  unsigned int requestedDelay;


  /* Exit if no delay count */
  
  if ((requestedDelay = delay) == 0)
    return;

  /*
   * Get the Time Base register tick count, this will be used
   * as the baseline.
   */

  ppcTimeBaseGet((PPCTB *)&baselineTickCount);

  /* Convert delay time into ticks */
  
  ticksToWait = requestedDelay * oneUsDelay;

  /* Compute when to stop */

  terminalTickCount = baselineTickCount + ticksToWait;

  do
    {
      /*
       * Get current Time Base register count.
       */

      ppcTimeBaseGet((PPCTB *)&currentTickCount);
      
    }
  while (currentTickCount < terminalTickCount);

  /*  printf(" t1 = %lli     t2 = %lli \n",baselineTickCount,currentTickCount); */

}

#else /* no UNIX version */

void
ppcTimer_dummy()
{
  return;
}

#endif
