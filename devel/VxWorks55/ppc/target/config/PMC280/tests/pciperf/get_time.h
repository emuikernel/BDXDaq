/* To test the functionality of i2c driver*/
#include <sys/times.h>
#include <VxWorks.h>
#include "k2.h"

/*fndef DEC_CLOCK_FREQ
       #define	DEC_CLOCK_FREQ	133000000		
#endif*/

#ifndef DEC_CLK_TO_INC
#define	DEC_CLK_TO_INC		4		/* # bus clks per increment */
#endif

#define DIVI_SEC        (DEC_CLOCK_FREQ/DEC_CLK_TO_INC)
#define DIVI_USEC       (DEC_CLOCK_FREQ/(DEC_CLK_TO_INC*1000000))

void pciget_time(struct timeval *tp);

