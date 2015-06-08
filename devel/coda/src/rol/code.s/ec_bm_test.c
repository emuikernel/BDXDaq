
/* ec_bm_test.c */

#ifdef VXWORKS

#include <stdio.h>
#include <math.h>
#include <ctype.h>
#include <vxWorks.h>
#include <taskLib.h>


int sysBusToLocalAdrs();

/* sleep() for vxworks - delay n * 10(msec) */
static void
tsleep(int n)
{
  taskDelay ((sysClkRateGet() / 100) * n);
}

void
bm_test(int stat)
{
  int k;
  volatile unsigned long *p;
  unsigned long offset;

  sysBusToLocalAdrs(0x39,0,&offset);

  for(k=0; k<stat; k++)
  {
    p=(unsigned long*)(offset+0x020000+0x10);
    tsleep(1);
    printf("%11lu %11lu \n",*(p+2),*(p+8));
  }
}

#else /* no UNIX version */

void
ec_bm_test_dummy()
{
  return;
}

#endif
