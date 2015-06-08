
/* test_stdarg.c */

#include <stdio.h>
#include <stdarg.h>

void
stepStd(int first, ...)

{
    va_list	argList;
    int		anum, bnum, cnum, dnum;

    va_start(argList, first);
    anum = first;
    bnum = va_arg(argList, int);
    cnum = va_arg(argList, int);
    dnum = va_arg(argList, int);
 printf("test_stdarg: a=%d, b=%d, c=%d, d=%d\n", anum, bnum, cnum, dnum);
   va_end(argList);

    return;
}

#ifdef VXWORKS
void
checkStdarg ()
{
    int a=1, b=2, c=3, d=4;
    stepStd (a, b, c, d);
}
#else
void
main()
{
    int a=1, b=2, c=3, d=4;
    stepStd (a, b, c, d);
}
#endif

/*
test_stdarg: a=1, b=2, c=3, d=4
*/
