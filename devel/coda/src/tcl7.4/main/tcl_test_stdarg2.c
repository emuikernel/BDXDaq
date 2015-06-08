
/* test_stdarg2.c */

#include "stdio.h"
#include "stdlib.h"
#include "stdarg.h"

void foo(int a,...)
{
   va_list argp;
 
   va_start(argp,a);

    printf("1st argument is %d\n",a);
    printf("2nd argument is %d\n", va_arg(argp, int)); /* 51 */
    printf("3rd argument is %d\n", va_arg(argp, int)); /* 63 */

    va_end(argp);
}

#ifdef VXWORKS
int test(void)  
{
   foo(1, 2, 3);
   return 0;
}
#else
int main(void)  
{
   foo(1, 2, 3);
   return 0;
}
#endif

/*
1st argument is 1
2nd argument is 2
3rd argument is 3
 */
