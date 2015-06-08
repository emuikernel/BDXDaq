#include "get_time.h"
#include "../../k2.h"

IMPORT void	vxTimeBaseGet (UINT32 * pTbu, UINT32 * pTbl);

void pciget_time(struct timeval *tp)
{
UINT32  tbLo, tbHi;
double  tv;
vxTimeBaseGet(&tbHi, &tbLo);
tv = tbLo + tbHi * (double)0x100000000ULL;
    
tp->tv_sec = tv / (double)DIVI_SEC;
tp->tv_usec = (tv - ((double)tp->tv_sec * DIVI_SEC)) / (double)DIVI_USEC;
return;
}
