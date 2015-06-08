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

#ifndef _DBGPRINTS_H_
#define _DBGPRINTS_H_

extern void dbg_printf0(const char *fmt, ...);
extern void dbg_printf1(const char *fmt, ...);
extern void dbg_puts0(const char *s);
extern void dbg_putc0(char c);
extern void dbg_puts1(const char *s);
extern void dbg_putc1(char c);
extern void _dbg_transmit0(char);
extern void _dbg_transmit1(char);

#endif /* _DBGPRINTS_H_ */
