/******************************************************************************
*              (c), Copyright, MOTOROLA  INC - ECC                 *
*                                                                             *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MOTOROLA , INC.        *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT *
* OF MOTOROLA   OR ANY THIRD PARTY. MOTOROLA   RESERVES THE RIGHT AT ITS SOLE DISCRETION*
* TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MOTOROLA .       *
* THIS CODE IS PROVIDED "AS IS". MOTOROLA  MAKES NO WARRANTIES, EXPRESS,*
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.  *
******************************************************************************/

#if defined(PMC280_DEBUG_UART) || defined(PMC280_DEBUG_UART_VX)

#include "vxWorks.h"
#include "stdio.h"
#include "dbgprints.h"

void dbg_printf0(const char *fmt, ...)
{
        va_list args;
        unsigned int    i;
        char    printbuffer[512];

        va_start(args, fmt);

        /*
         * For this to work, printbuffer must be larger than
         * anything we ever want to print.
         */
        i = vsprintf(printbuffer, fmt, args);
        va_end(args);

        /* Print the string */
        dbg_puts0(printbuffer);
} 

void dbg_printf1(const char *fmt, ...)
{
        va_list args;
        unsigned int    i;
        char    printbuffer[512];

        va_start(args, fmt);

        /*
         * For this to work, printbuffer must be larger than
         * anything we ever want to print.
         */
        i = vsprintf(printbuffer, fmt, args);
        va_end(args);

        /* Print the string */
        dbg_puts1(printbuffer);
} 

void dbg_puts0(const char *s)
{
    while(*s)
    {
        dbg_putc0(*s++);
    }
}

void dbg_putc0(char c)
{
    if(c == '\n')
        _dbg_transmit0('\r');
    _dbg_transmit0(c);
}

void dbg_puts1(const char *s)
{
    while(*s)
    {
        dbg_putc1(*s++);
    }
}

void dbg_putc1(char c)
{
    if(c == '\n')
        _dbg_transmit1('\r');
    _dbg_transmit1(c);
}
#endif /* PMC280_DEBUG_UART||PMC280_DEBUG_UART_VX */
