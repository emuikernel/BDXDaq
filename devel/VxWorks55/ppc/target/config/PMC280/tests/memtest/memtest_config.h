/******************************************************************************
*              (c), Copyright, FORCE COMPUTERS INDIA Limited                  *
*                                                                             *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF FORCE COMPUTERS, INC.        *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT *
* OF FORCE OR ANY THIRD PARTY. FORCE RESERVES THE RIGHT AT ITS SOLE DISCRETION*
* TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO FORCE COMPUTERS.       *
* THIS CODE IS PROVIDED "AS IS". FORCE COMPUTERS MAKES NO WARRANTIES, EXPRESS,*
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.  *
*******************************************************************************
*                                                                             *
* Filename:memtest_config.h                                                   *
*                                                                             *
* DESCRIPTION:	                                                              *
*       This file contains configuration information for memtests             *
*                                                                             *
* DEPENDENCIES:	                                                              *
*       None.                                                                 *
*                                                                             *
******************************************************************************/

#ifndef _MEMTEST_CONFIG__
#define _MEMTEST_CONFIG__


/********************* Includes  ***********************************/

/*********************  Defines  ***********************************/
#define MEMTEST_ERROR             -1
#define MEMTEST_OK                0

#define RAND32	                  0x5A5A
#define MAKE32FROM8(X)	          (ui32)(X | X << 8 | X << 16 | X << 24)
#define MEM_PATTERN1              0x505050


/*********************  typedefs ***********************************/
typedef  unsigned long ui32;
typedef  unsigned char ui8;
typedef  long i32;

/********************* Structures **********************************/

/********************* Globals  ************************************/
int silent_test;


#endif /*_MEMTEST_CONFIG__*/