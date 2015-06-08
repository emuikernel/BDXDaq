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
* Filename:memtest_tests.h                                                    *
*                                                                             *
* DESCRIPTION:	                                                              *
*       This file contains declarations for the functions in memtest_tests.c  *
*                                                                             *
* DEPENDENCIES:	                                                              *
*       None.                                                                 *
*                                                                             *
******************************************************************************/

#ifndef _MEMTEST_TESTS_H__
#define _MEMTEST_TESTS_H__

#include "memtest_config.h"

/********************* Includes  ***********************************/

/*********************  Defines  ***********************************/

/*********************  typedefs ***********************************/


/********************* Structures **********************************/

/********************* Globals  ************************************/



/***************** Function declarations  **************************/

/******************* Extended Tests   ******************************/
int test_verify_success (ui32 *bp1, ui32 *bp2, ui32 count);
int test_xor_comparison (ui32 *bp1, ui32 *bp2, ui32 count);
int test_sub_comparison (ui32 *bp1, ui32 *bp2, ui32 count);
int test_mul_comparison (ui32 *bp1, ui32 *bp2, ui32 count);
int test_div_comparison (ui32 *bp1, ui32 *bp2, ui32 count);
int test_or_comparison (ui32 *bp1, ui32 *bp2, ui32 count);
int test_and_comparison (ui32 *bp1, ui32 *bp2, ui32 count);
int test_seqinc_comparison (ui32 *bp1, ui32 *bp2, ui32 count);
int test_checkerboard_comparison (ui32 *bp1, ui32 *bp2, ui32 count);
int test_solidbits_comparison (ui32 *bp1, ui32 *bp2, ui32 count);
int test_blockseq_comparison (ui32 *bp1, ui32 *bp2, ui32 count);
int test_walkbits_comparison (ui32 *bp1, ui32 *bp2, ui32 count, int mode);
int test_bitspread_comparison (ui32 *bp1, ui32 *bp2, ui32 count);
int test_bitflip_comparison (ui32 *bp1, ui32 *bp2, ui32 count);
int test_stuck_address (ui32 *bp1, ui32 *bp2, ui32 count);
int test_moving_inv32(ui32 *startAddr, ui32 *endAddr, ui32 count);


/********************** Basic Tests   ******************************/


#endif /*_MEMTEST_TESTS_H__*/