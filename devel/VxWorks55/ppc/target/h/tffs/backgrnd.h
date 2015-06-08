
/*
 * $Log:   V:/backgrnd.h_v  $
   
      Rev 1.2   07 Jul 1997 15:23:18   amirban
   Ver 2.0
   
      Rev 1.1   18 Aug 1996 13:48:20   amirban
   Comments
   
      Rev 1.0   31 Jul 1996 14:31:58   amirban
   Initial revision.
 */

/************************************************************************/
/*                                                                      */
/*		FAT-FTL Lite Software Development Kit			*/
/*		Copyright (C) M-Systems Ltd. 1995-1996			*/
/*									*/
/************************************************************************/

#ifndef BACKGRND_H
#define BACKGRND_H

#ifdef __cplusplus
extern "C" {
#endif

#include "flsocket.h"

#define BG_RESUME	1
#define	BG_SUSPEND	2

#ifdef BACKGROUND

extern int flForeground(int sendValue);
extern int flBackground(int sendValue);
extern int flStartBackground(unsigned volNo, void (*routine)(void *), void *object);
extern void flCreateBackground(void);

#else

#define flForeground(n)   BG_RESUME
#define flBackground(n)

#endif

#ifdef __cplusplus
}
#endif

#endif

