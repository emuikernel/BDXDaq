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
* Filename:memtest_drv.h                                                      *
*                                                                             *
* DESCRIPTION:	                                                              *
*       This file contains declarations for the functions in memtest_drv.c,   *
*       contains structures, global variables etc.                            *
*																			  *
* DEPENDENCIES:	                                                              *
*       None.                                                                 *
*                                                                             *
******************************************************************************/

#ifndef _MEMTEST_DRV_H__
#define _MEMTEST_DRV_H__

#include "memtest_tests.h"

/********************* Includes  ***********************************/

/*********************  Defines  ***********************************/

/*********************  typedefs ***********************************/


/********************* Structures **********************************/

typedef enum testType
             {
                 BASIC_TESTS =1,
                 EXTENDED_TESTS,
                 ALL_TESTS
             }testType; 
      
typedef struct teststruct_noarg
{
	char *name;
	int (*fp)(ui32*, ui32*, ui32 );
}teststruct_0;

typedef struct teststruct_onearg
{
	char *name;
	int (*fp)(ui32*, ui32*, ui32 ,int );
	int arg;
} teststruct_1;

/********************* Globals  ************************************/
extern ui32 start_addr, end_addr;
extern ui32 totalerrors , maxruns, run,runerrors;
extern ui32 membytes, memsplit, count/*, waste*/;
extern ui8 *buf, *s1, *s2;
extern ui32 *bp1, *bp2;

extern teststruct_0 tests_noarg[];
extern teststruct_1 tests_1arg[];
extern teststruct_0 tests_2arg[];


/***************** Function declarations  **************************/
int   memboundary_check(void);
void  print_test_name(char *testname, ui32 *test);
void  memtest_usage(char *me);
void  release_mem(void);
int   frcTestMemTest(void);
int memTest_Basic(ui32 addr, ui32 count, i32 numRuns);


#endif /*_MEMTEST_DRV_H__*/