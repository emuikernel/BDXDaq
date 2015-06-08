/******************************************************************************
*              (c), Copyright, MOTOROLA  INC  - ECC                  *
*                                                                             *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MOTOROLA , INC.        *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT *
* OF MOTOROLA  OR ANY THIRD PARTY. MOTOROLA  RESERVES THE RIGHT AT ITS SOLE DISCRETION*
* TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MOTOROLA .       *
* THIS CODE IS PROVIDED "AS IS". MOTOROLA  MAKES NO WARRANTIES, EXPRESS,*
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.  *
*******************************************************************************
*                                                                             *
* Filename:sysMemTest.h                                                       *
*                                                                             *
* DESCRIPTION:	                                                              *
*       This file contains declarations for the functions in sysMemTest.c     *
*                                                                             *
* DEPENDENCIES:	                                                              *
*       None.                                                                 *
*                                                                             *
******************************************************************************/


/******Function declarations*********/
int
ramTest16bit (
	UINT32 addr,			/* Start Addr */ 
	UINT32 ct,			/* byte count */
	UINT32 testFlag,		/* Test Flag */
	UINT32 * errCount		/* Error count to increment ;
					 *  could be a running count, or a new
					 *   count for each invocation 
					 */
	);

int
ramTest32bit (
	UINT32 addr,			/* Start Addr */ 
	UINT32 ct,			/* byte count */
	UINT32 testFlag,		/* Test Flag */
	UINT32 * errCount		/* Error count to increment ;
					 *  could be a running count, or a new
					 *   count for each invocation 
					 */
	);

int
ramTest8bit (
	UINT32 addr,			/* Start Addr */ 
	UINT32 ct,		    	/* byte count */
	UINT32 testFlag,		/* Test Flag */
	UINT32 * errCount		/* Error count to increment ;
					         *  could be a running count, or a new
					         *   count for each invocation 
					         */
	);


