
#if 0
/*#include <vxPpcLib.h>*/

typedef unsigned int UINT32;

void tbReset (void)
    {
    UINT32	dummy;
    
    vxTimeBaseGet (&dummy, &tbStart);

    }
/*
int gettimeofday
    (
    struct timeval *tp, void *ptr
    )
    {
    UINT32      tbLo, tbHi;
    
    vxTimeBaseGet (&tbHi, &tbLo);

    tbLo -= tbStart;
    
    tp->tv_sec = tbLo / (double)DIVI_SEC;
    tp->tv_usec = (tbLo - (tp->tv_sec * DIVI_SEC)) / DIVI_USEC;

    return tp->tv_sec;
    }
*/

#endif /*#if0*/