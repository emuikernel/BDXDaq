/*******************************************************************************
*              (c), Copyright, MOTOROLA  INC  - ECC                   *
*																			   *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MOTOROLA , INC.         *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MOTOROLA  OR ANY THIRD PARTY. MOTOROLA  RESERVES THE RIGHT AT ITS SOLE DISCRETION *
* TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MOTOROLA .        *
* THIS CODE IS PROVIDED "AS IS". MOTOROLA  MAKES NO WARRANTIES, EXPRESS, *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
*
* sysMemTest.c -- system Memory Test 
*
* Description : 
*      different memory tests :
*      8 bit, 16 bit, and 32 bit.
*
*
*
*
* Recommend pulling vxWorks memTop down to around 6MB.
* This test can then test 7MB to end of memory.
*
*
*
* input = start address to test, byte count, a test flag, and a 
*         ptr to an error count
*
*---------------------------------------------------------------
* Test Logic::::
*---------------------------------------------------------------
*
*
* 8bit, 16 bit and 32 bit tests are similar as below
*
*  Alternating Pattern test ......fill
*	fill each even unit with a pattern1, each odd unit with a pattern2
*  Alternating Pattern test ......test forward
*       For each address from start address,
*        Again1:
*	  read address
*	  if address not = expected pattern
*	     print error message
*	     increment error count
*	  if test flag == STOP, then return
*	  if test flag == Contine, goto next address
*	  if test flag == Loop on Fail, goto Again1 
*
*
*  Alternating reverse Pattern test ......fill
*	fill each even unit with a pattern2, each odd unit with a pattern1
*  Alternating reverse Pattern test ......test reverse
*       For each address from end address (top to bottom),
*	  similar verify logic as before
*
*
*
*  Walking ones test ......fill
*	write a 1 to each address, shifting 1 bit left each address and wrap
*	 around
*  Walking ones test ......test forward
*       For each address from start address,
*	  similar verify logic as before
*
*
*
*  Walking zeros test ......fill
*	write a 0 to each address (other bits are FF), shifting 1 bit left 
*       each address and wrap  around
*  Walking zeros test  ......test reverse
*       For each address from end address (top to bottom),
*	  similar verify logic as before
*
*  Increment test  ......fill
*	write an incrementing pattern (starts at zero)  to each address 
*       with wrap  around
*  Increment test  ......test forward
*       For each address from start address,
*	  similar verify logic as before
*
*
*
*---------------------------------------------------------------
* FOR Only 32BIT TEST
*---------------------------------------------------------------
*
*
*   March pattern test1  32bit ......fill
*	write all FF's to each address
*
*
*   March pattern test1 32bit ......test forward
*       For each address from start address,
*	  similar verify logic as before
*        after an address is verified, write zeros, then FF's, then
*        zeros to that address
*
*   March pattern test2 32bit ......test reverse
*       For each address from end address (top to bottom),
*	  similar verify logic as before
*        after an address is verified, write FF's, then write zeros 
*	  to that address
*
*    Address at Address test 32bit ......fill
*	write the address to each address (0x0010000 to 0x0010000, 0x00100004 to
*	 0x00100004, etc.)
*    Address at Address test 32bit ......test forward
*       For each address from start address,
*	  similar verify logic as before
*
* 
*******************************************************************************/
#include <stdio.h>

#include "test.h"
#include "taskLib.h"

#define TASKDELAY	taskDelay(1)
/*#define DEBUG_MEMTEST */

  
void sysUserLEDSet(int x, int y) {}

#define TICKS_PER_SEC	60		/* usually */
#define TICKS_PRINT (TICKS_PER_SEC * 5)
	
#define PRINTVALS	8		/* For debug */

IMPORT ULONG tickGet (void);

/**************************************************************************
* printDramErr - print a DRAM error 
*		
*
* RETURNS: N/A.   
**************************************************************************/
void printDramErr (
	UINT32 addr,			/* Addr of error */ 
	UINT32 pattern,			/* expected pattern */
	UINT32 value			/* value found */
	)
{
        printf ("\nERROR at location: 0x%x ", addr); 
        printf (" read : 0x%x ", value );
        printf (" should be: 0x%x \n", pattern );
}


LOCAL UINT32 ramTestLastTick;

/**************************************************************************
 * ramTestPrintDot - print a DOT so many ticks *		
 *
 * RETURNS: N/A.   
 *************************************************************************/
void
ramTestPrintDot ( void )
{
	UINT32 curTick;
	
	curTick = tickGet();

	if ((ramTestLastTick + TICKS_PRINT ) <  curTick)
	{

		ramTestLastTick = curTick;
		printf(".");
	}
}





/**************************************************************************
 * ramTest8bit - Test DRAM Basic 8 bit access
 *		
 *
 * RETURNS: ERROR if fail, else OK. 
 **************************************************************************/


int
ramTest8bit (
	UINT32 addr,			/* Start Addr */ 
	UINT32 ct,		    	/* byte count */
	UINT32 testFlag,		/* Test Flag */
	UINT32 * errCount		/* Error count to increment ;
					         *  could be a running count, or a new
					         *   count for each invocation 
					         */
	)
{
	UINT8 * 	pDram;
	UINT8 * 	pDramEnd;
	UINT8 		data ;		/* size of test units */
/*	UINT8 		trash ;*//*vijay-*/ /* unused data */
	UINT8 		val ;
	UINT32 		ix, jx;
	UINT32 		ixLast;
	UINT32 		jxLast;
	UINT32 		dramStart ;
	UINT32 		dramEnd ;
	UINT32 		startErrorCt ;
	UINT32 		dramUnits ;
	



	jxLast = 0;
	dramStart = addr ; 
	dramEnd = addr + ct - 1; 
	dramUnits  = ct /sizeof(data); 

	/* Starting Error count */
	startErrorCt = *errCount;


#ifdef DEBUG_MEMTEST
	printf ("\nramTest8bit: DBG startaddr 0x%x,  byte count 0x%x ",
			addr, ct);
	printf ("testFlag 0x%x,  errCount  0x%x \n",
			testFlag,  *errCount);
#endif



    /* ---------------- Alternating 8bit pattern -------------------*/

        printf("\nAlternating Pattern test 8bit......fill ");
#ifdef TEST_DGB_
        printf("In side tyne tests1--GOING TO ACCESS DATA\n");
        printf("Address of pDram= %u \n",dramStart);
        printf("dramUnits= %d \n",dramUnits);
#endif /*#ifdef TEST_DGB_*/

	for (ix=0, pDram = (UINT8 *)dramStart; /*dramStart+400000*/ix< dramUnits; ix++)
        {       
		if( ix & 1 ) 
                        val = (UINT8)PATTERN1;
                else
                        val = (UINT8)PATTERN2;
		*pDram = val ;
		 pDram++;
		ramTestPrintDot ( );
        }
             
        printf("finished\n");

	TASKDELAY;	/* For other processes */



#ifdef DEBUG_MEMTEST
 {
	UINT32 iy;
	pDram = (UINT8 *)dramStart; 

	for (iy=0; iy < PRINTVALS; iy++)
	{
		printf (" Addr 0x%x val = 0x%x ", (UINT32) pDram, *pDram);
		pDram++;
	}
	taskDelay (10);
 }
#endif


        printf("\rAlternating Pattern test 8bit......test forward");
	for (ix=0, pDram = (UINT8 *)dramStart; ix< dramUnits ; ix++)
        {        

loopsame1:
               /*This line is commented by vijay*/
               /*trash = *((UINT8*)TRASHLOCATION);*/ /* read something else..*/
                                                 /* ..to clear bus load  */
               if( ix & 1 ) 
			val = PATTERN1 & 0x0FF;
	       else
			val = PATTERN2 & 0x0FF;
               data = *pDram ;
               data &= 0x0ff ;
	       if ( data != val )
	  	{
		 printDramErr ( (UINT32) pDram, (UINT32) val, (UINT32) data);

	         *errCount +=1 ;		
		 if (testFlag & SYS_TESTS_FLAG_STOP)
		 	return(ERROR);
		 if (testFlag & SYS_TESTS_FLAG_CONTINUE	)
		 	goto next1;
		 if (testFlag & SYS_TESTS_FLAG_LOOPFAIL	)
		 	goto loopsame1;
	  	} /* End if  */

next1:
	    pDram++ ;
		ramTestPrintDot ( );
	      } /* End for */



	TASKDELAY;	/* For other processes */


    /* ---------------- Alternating 8bit reverse pattern -------------------*/

        printf("\nAlternating reverse Pattern test 8bit......fill");
	for (ix=0, pDram = (UINT8 *)dramStart; ix< dramUnits ; ix++)
        {       if( ix & 1 ) 
                        val = (UINT8)PATTERN2;
                else
                        val = (UINT8)PATTERN1;
		*pDram = val ;
		 pDram++;
		ramTestPrintDot ( );
        }
             

        pDram--;
	pDramEnd = pDram; /* Last addr */
	ixLast   = ix - 1;

	TASKDELAY;	/* For other processes */
#ifdef DEBUG_MEMTEST
 {
	UINT32 iy;
	pDram = (UINT8 *)dramStart; 

	for (iy=0; iy < PRINTVALS; iy++)
	{
		printf (" Addr 0x%x val = 0x%x ",(UINT32) pDram, *pDram);
		pDram++;
	}
	taskDelay (10);
 }
#endif


        printf("\rAlternating reverse Pattern test 8bit......test reverse");

	for ( ix = ixLast, pDram = (UINT8 *)pDramEnd ; ix > 0 ; ix-- )
        {        

loopsame2:
               /*commented by vijay                                  */
               /*trash = *((UINT8*)TRASHLOCATION);*/ /* read something else..*/
                                                  /* ..to clear bus load */
               if( ix & 1 ) 
			val = PATTERN2 & 0x0FF;
                else
                        val = PATTERN1 & 0x0FF;

               data = *pDram ;
               data &= 0x0ff ;
	       if ( data != val )
	  	{
		 printDramErr ( (UINT32) pDram, (UINT32) val, (UINT32) data);

	         *errCount +=1 ;		
		 if (testFlag & SYS_TESTS_FLAG_STOP)
		 	return(ERROR);
		 if (testFlag & SYS_TESTS_FLAG_CONTINUE	)
		 	goto next2;
		 if (testFlag & SYS_TESTS_FLAG_LOOPFAIL	)
		 	goto loopsame2;
	  	} /* End if  */

next2:
	        pDram-- ;
		ramTestPrintDot ( );
	      } /* End for */


    /* ---------------- Walking one test -----------------------------*/

        printf("\nWalking ones test 8bit ......fill");

	jx = 1;
	for (ix=0, pDram = (UINT8 *)dramStart; ix< dramUnits ; ix++)
        {       
		*pDram = (UINT8) jx ;
		jx <<=1;
		if (jx >= 0x100) jx = 1;
		pDram++;
		ramTestPrintDot ( );
        }
             
	TASKDELAY;	/* For other processes */
#ifdef DEBUG_MEMTEST
 {
	UINT32 iy;
	pDram = (UINT8 *)dramStart; 

	for (iy=0; iy < PRINTVALS; iy++)
	{
		printf (" Addr 0x%x val = 0x%x ", (UINT32) pDram, *pDram);
		pDram++;
	}
	taskDelay (10);
 }
#endif


        printf("\rWalking ones test 8bit ......test forward");
	jx = 1;
	for (ix=0, pDram = (UINT8 *)dramStart; ix< dramUnits ; ix++)
        {        

loopsame3:
               /*commented by vijay                                */
               /*trash = *((UINT8*)TRASHLOCATION);*/ /* read something else..*/
                                                  /* ..to clear bus load */
               data = *pDram ;
               data &= 0x0ff ;
	       if ( data != jx )
	  	{
		 printDramErr ( (UINT32) pDram, (UINT32) jx, (UINT32) data);

	         *errCount +=1 ;		
		 if (testFlag & SYS_TESTS_FLAG_STOP)
		 	return(ERROR);
		 if (testFlag & SYS_TESTS_FLAG_CONTINUE	)
		 	goto next3;
		 if (testFlag & SYS_TESTS_FLAG_LOOPFAIL	)
		 	goto loopsame3;
	  	} /* End if  */

next3:
	        pDram++ ;
		jx <<=1;
		if (jx >= 0x100) jx = 1;
		ramTestPrintDot ( );
	      } /* End for */


    /* ---------------- Walking zeros test -----------------------------*/

        printf("\nWalking zeros test 8bit ......fill");
	jx = 1;
	for (ix=0, pDram = (UINT8 *)dramStart; ix < dramUnits ; ix++)
        {
		*pDram = ~((UINT8) jx) ;
		jxLast = jx;
		jx <<=1;
		if (jx >= 0x100) jx = 1;
		pDram++;
		ramTestPrintDot ( );
	}


	/* back up one pointers to use for next test */ 
        pDram--;
	pDramEnd = pDram; /* Last addr */
	ixLast   = ix - 1;

	TASKDELAY;	/* For other processes */
#ifdef DEBUG_MEMTEST
 {
	UINT32 iy;
	pDram = (UINT8 *)dramStart; 

	for (iy=0; iy < PRINTVALS; iy++)
	{
		printf (" Addr 0x%x val = 0x%x ", (UINT32) pDram, *pDram);
		pDram++;
		ramTestPrintDot ( );
	}
	taskDelay (10);
 }
#endif


        printf("\rWalking zeros test 8bit ......test reverse");
	jx = jxLast;
	for (ix=ixLast, pDram = (UINT8 *)pDramEnd; ix > 0; ix--)
        {        

loopsame4:
               /*Commented by vijay                                */
               /*trash = *((UINT8*)TRASHLOCATION);*/ /* read something else..*/
                                                  /* ..to clear bus load */
               data = *pDram ;
               data =  (~data) & 0x0ff ;
	       if ( data != jx )
	  	{
		 printDramErr ( (UINT32) pDram, (UINT32) jx, (UINT32) data);

	         *errCount +=1 ;		
		 if (testFlag & SYS_TESTS_FLAG_STOP)
		 	return(ERROR);
		 if (testFlag & SYS_TESTS_FLAG_CONTINUE	)
		 	goto next4;
		 if (testFlag & SYS_TESTS_FLAG_LOOPFAIL	)
		 	goto loopsame4;
	  	} /* End if  */

next4:
	        pDram-- ;
		jx >>=1;
		if (jx == 0) jx = 0x80;
		ramTestPrintDot ( );
	      } /* End for */



    /* ---------------- Increment test -----------------------------*/

        printf("\nIncrement test 8bit ......fill");
	for (ix=0, pDram = (UINT8 *)dramStart; ix< dramUnits ; ix++)
        {
		*pDram++ = ix;
		ramTestPrintDot ( );
	}


	TASKDELAY;	/* For other processes */

        printf("\rIncrement test 8bit ......test forward");
	for (ix=0, pDram = (UINT8 *)dramStart; ix< dramUnits ; ix++)
        {        

loopsame5:
               /*Commented by vijay                                */
               /*trash = *((UINT8*)TRASHLOCATION);*/ /* read something else..*/
                                                  /* ..to clear bus load */

               data = *pDram ;
	       if ( data != (ix & 0x0FF) )
	  	{
		 printDramErr ( (UINT32) pDram, (UINT32) (ix & 0x0FF), 
								(UINT32) data);

	         *errCount +=1 ;		
		 if (testFlag & SYS_TESTS_FLAG_STOP)
		 	return(ERROR);
		 if (testFlag & SYS_TESTS_FLAG_CONTINUE	)
		 	goto next5;
		 if (testFlag & SYS_TESTS_FLAG_LOOPFAIL	)
		 	goto loopsame5;
	  	} /* End if  */
next5:
	        pDram++ ;
		ramTestPrintDot ( );
	      } /* End for */



 /*
  * Exit test - 
  *  if start error count != end error count, then we had an error
  */
	if (startErrorCt != *errCount)
		return (ERROR);
	else
		return (OK);



} /* End ramTest8bit   */



/**************************************************************************
 * ramTest16bit - Test DRAM Basic 16 bit access
 *		
 *
 * RETURNS: ERROR if fail, else OK. 
 ***************************************************************************/


int
ramTest16bit (
	UINT32 addr,			/* Start Addr */ 
	UINT32 ct,			/* byte count */
	UINT32 testFlag,		/* Test Flag */
	UINT32 * errCount		/* Error count to increment ;
					 *  could be a running count, or a new
					 *   count for each invocation 
					 */
	)
{

	volatile UINT16 * 	pDram;
	volatile UINT16 * 	pDramEnd;
	UINT16 		data ;		/* size of test units */
/*	UINT16 		trash ;*//*vijay *//* unused data */
	UINT16 		val ;
	UINT32 		ix, jx;
	UINT32 		ixLast, jxLast;
	UINT32 		dramStart ;
	UINT32 		dramEnd ;
	UINT32 		startErrorCt ;
	UINT32 		dramUnits ;
	

#define TESTMASK_16	0x0FFFF




	dramStart = addr ; 
	dramEnd = addr + ct - 1; 
	dramEnd = dramEnd & 0xFFFFFFFE;		/* Round down */
	dramUnits = ct / (sizeof (data) ) ; 

	/* Starting Error count */
	startErrorCt = *errCount;


#ifdef DEBUG_MEMTEST
	printf ("\nramTest16bit: DBG startaddr 0x%x,  byte count 0x%x ",
			addr, ct);
	printf ("testFlag 0x%x,  errCount  0x%x \n",
			testFlag,  *errCount);
#endif







    /* ---------------- Alternating 16bit pattern -------------------*/

        printf("\nAlternating Pattern test 16bit......fill");
	for (ix=0, pDram = (UINT16 *)dramStart; ix< dramUnits ; ix++)
        {       if( ix & 1 ) 
                        val = (UINT16)PATTERN1;
                else
                        val = (UINT16)PATTERN2;
		*pDram = val ;
		 pDram++;
		ramTestPrintDot ( );
        }
             


	TASKDELAY;	/* For other processes */

#ifdef DEBUG_MEMTEST
 {
	UINT32 iy;
	pDram = (UINT16 *)dramStart; 

	for (iy=0; iy < PRINTVALS; iy++)
	{
		printf (" Addr 0x%x val = 0x%x ", (UINT32) pDram, *pDram);
		pDram++;
	}
	taskDelay (10);
 }
#endif


        printf("\rAlternating Pattern test 16bit......test forward");
	for (ix=0, pDram = (UINT16 *)dramStart; ix< dramUnits ; ix++)
        {        

loopsame1:
               /*Commented by vijay                                */
               /*trash = *((UINT16*)TRASHLOCATION);*/ /* read something else..*/
                                                  /* ..to clear bus load */
               if( ix & 1 ) 
			val = PATTERN1 & TESTMASK_16	;
	       else
			val = PATTERN2 & TESTMASK_16;
               data = *pDram ;
               data &= TESTMASK_16 ;
	       if ( data != val )
	  	{
		 printDramErr ( (UINT32) pDram, (UINT32) val, (UINT32) data);

	         *errCount +=1 ;		
		 if (testFlag & SYS_TESTS_FLAG_STOP)
		 	return(ERROR);
		 if (testFlag & SYS_TESTS_FLAG_CONTINUE	)
		 	goto next1;
		 if (testFlag & SYS_TESTS_FLAG_LOOPFAIL	)
		 	goto loopsame1;
	  	} /* End if  */

next1:
	        pDram++ ;
		ramTestPrintDot ( );
	      } /* End for */



	TASKDELAY;	/* For other processes */

#ifdef DEBUG_MEMTEST
 {
	UINT32 iy;
	pDram = (UINT16 *)dramStart; 

	for (iy=0; iy < PRINTVALS; iy++)
	{
		printf (" Addr 0x%x val = 0x%x ", (UINT32) pDram, *pDram);
		pDram++;
	}
	taskDelay (10);
 }
#endif

    /* ---------------- Alternating 16bit reverse pattern -------------------*/

        printf("\nAlternating reverse Pattern test 16bit......fill");
	for (ix=0, pDram = (UINT16 *)dramStart; ix< dramUnits ; ix++)
        {       if( ix & 1 ) 
                        val = (UINT16)PATTERN2;
                else
                        val = (UINT16)PATTERN1;
		*pDram = val ;
		 pDram++;
		ramTestPrintDot ( );
        }
             

	pDram--;
        pDramEnd = pDram;
	ixLast = ix - 1;

	TASKDELAY;	/* For other processes */
#ifdef DEBUG_MEMTEST
 {
	UINT32 iy;
	pDram = (UINT16 *)dramStart; 

	for (iy=0; iy < PRINTVALS; iy++)
	{
		printf (" Addr 0x%x val = 0x%x ", (UINT32) pDram, *pDram);
		pDram++;
	}
	taskDelay (10);
 }
#endif

        printf("\rAlternating reverse Pattern test 16bit......test reverse");
	for (ix=ixLast, pDram = pDramEnd; ix > 0 ; ix--)
        {        

loopsame2:
               /*Commented by vijay                                */
               /*trash = *((UINT16*)TRASHLOCATION);*/ /* read something else..*/
                                                  /* ..to clear bus load */
               if( ix & 1 ) 
			val = PATTERN2 & TESTMASK_16;
                else
                        val = PATTERN1 & TESTMASK_16;
               data = *pDram ;
               data &= TESTMASK_16 ;
	       if ( data != val )
	  	{
		 printDramErr ( (UINT32) pDram, (UINT32) val, (UINT32) data);

	         *errCount +=1 ;		
		 if (testFlag & SYS_TESTS_FLAG_STOP)
		 	return(ERROR);
		 if (testFlag & SYS_TESTS_FLAG_CONTINUE	)
		 	goto next2;
		 if (testFlag & SYS_TESTS_FLAG_LOOPFAIL	)
		 	goto loopsame2;
	  	} /* End if  */

next2:
	        pDram-- ;
		ramTestPrintDot ( );
	      } /* End for */


    /* ---------------- Walking one test -----------------------------*/

        printf("\nWalking ones test 16bit ......fill");
	jx = 1;
	for (ix=0, pDram = (UINT16 *)dramStart; ix< dramUnits ; ix++)
        {       
		*pDram = (UINT16) jx ;
		jx <<=1;
		if (jx >= 0x10000) jx = 1;
		pDram++;
		ramTestPrintDot ( );
        }
             
	TASKDELAY;	/* For other processes */
#ifdef DEBUG_MEMTEST
 {
	UINT32 iy;
	pDram = (UINT16 *)dramStart; 

	for (iy=0; iy < PRINTVALS; iy++)
	{
		printf (" Addr 0x%x val = 0x%x ", (UINT32) pDram, *pDram);
		pDram++;
	}
	taskDelay (10);
 }
#endif

        printf("\rWalking ones test 16bit ......test forward");
	jx = 1;
	for (ix=0, pDram = (UINT16 *)dramStart; ix < dramUnits ; ix++)
        {        

loopsame3:
               /*Commented by vijay                                */
               /*trash = *((UINT16*)TRASHLOCATION);*/ /* read something else..*/
                                                  /* ..to clear bus load */
               data = *pDram ;
               data &= TESTMASK_16 ;
	       if ( data != jx )
	  	{
		 printDramErr ( (UINT32) pDram, (UINT32) jx, (UINT32) data);

	         *errCount +=1 ;		
		 if (testFlag & SYS_TESTS_FLAG_STOP)
		 	return(ERROR);
		 if (testFlag & SYS_TESTS_FLAG_CONTINUE	)
		 	goto next3;
		 if (testFlag & SYS_TESTS_FLAG_LOOPFAIL	)
		 	goto loopsame3;
	  	} /* End if  */

next3:
	        pDram++ ;
		jx <<=1;
		if (jx >= 0x10000) jx = 1;
		ramTestPrintDot ( );
	      } /* End for */


    /* ---------------- Walking zeros test -----------------------------*/

        printf("\nWalking zeros test 16bit ......fill");
	jx = 1;
	for (ix=0, pDram = (UINT16 *)dramEnd; ix< dramUnits ; ix++)
        {
		*pDram = ~((UINT16) jx) ;
		jxLast = jx;
		jx <<=1;
		if (jx >= 0x10000) jx = 1;
		pDram--;
		ramTestPrintDot ( );
	}


	pDram--;
	pDramEnd = pDram;
	ixLast = ix - 1;



	TASKDELAY;	/* For other processes */

#ifdef DEBUG_MEMTEST
 {
	UINT32 iy;
	pDram = (UINT16 *)dramStart; 

	for (iy=0; iy < PRINTVALS; iy++)
	{
		printf (" Addr 0x%x val = 0x%x ", (UINT32) pDram, *pDram);
		pDram++;
	}
	taskDelay (10);
 }
#endif
        printf("\rWalking zeros test 16bit ......test reverse");
	jx = jxLast;
	for (ix=ixLast, pDram = pDramEnd; ix < 0 ; ix--)
        {        

loopsame4:
               /*Commented by vijay                                */
               /*trash = *((UINT16*)TRASHLOCATION);*/ /* read something else..*/
                                                  /* ..to clear bus load */
               data = *pDram ;
               data =  (~data) & TESTMASK_16 ;
	       if ( data != jx )
	  	{
		 printDramErr ( (UINT32) pDram, (UINT32) jx, (UINT32) data);

	         *errCount +=1 ;		
		 if (testFlag & SYS_TESTS_FLAG_STOP)
		 	return(ERROR);
		 if (testFlag & SYS_TESTS_FLAG_CONTINUE	)
		 	goto next4;
		 if (testFlag & SYS_TESTS_FLAG_LOOPFAIL	)
		 	goto loopsame4;
	  	} /* End if  */
next4:
	        pDram-- ;
		jx >>=1;
		if (jx == 0x00000) jx = 0x08000;
		ramTestPrintDot ( );
	      } /* End for */



    /* ---------------- Increment test -----------------------------*/

        printf("\nIncrement test 16bit ......fill");
	for (ix=0, pDram = (UINT16 *)dramStart; ix< dramUnits ; ix++)
        {
		*pDram++ = ix;
		ramTestPrintDot ( );
	}

	TASKDELAY;	/* For other processes */

#ifdef DEBUG_MEMTEST
 {
	UINT32 iy;
	pDram = (UINT16 *)dramStart; 

	for (iy=0; iy < PRINTVALS; iy++)
	{
		printf (" Addr 0x%x val = 0x%x ", (UINT32) pDram, *pDram);
		pDram++;
	}
	taskDelay (10);
 }
#endif
        printf("\rIncrement test 16bit ......test forward");
	for (ix=0, pDram = (UINT16 *)dramStart; ix < dramUnits ; ix++)
        {        

loopsame5:
               /*Commented by vijay                                */
               /*trash = *((UINT16*)TRASHLOCATION); *//* read something else..*/
                                                  /* ..to clear bus load */

               data = *pDram ;
	       if ( data != (ix & TESTMASK_16) )
	  	{
		 printDramErr ( (UINT32) pDram, (UINT32) (ix & TESTMASK_16), 
								(UINT32) data);

	         *errCount +=1 ;		
		 if (testFlag & SYS_TESTS_FLAG_STOP)
		 	return(ERROR);
		 if (testFlag & SYS_TESTS_FLAG_CONTINUE	)
		 	goto next5;
		 if (testFlag & SYS_TESTS_FLAG_LOOPFAIL	)
		 	goto loopsame5;
	  	} /* End if  */
next5:
	        pDram++ ;
		ramTestPrintDot ( );
	      } /* End for */



 /*
  * Exit test - 
  *  if start error count != end error count, then we had an error
  */
	if (startErrorCt != *errCount)
		return (ERROR);
	else
		return (OK);



} /* End ramTest16bit   */







/**************************************************************************
 * ramTest32bit - Test DRAM 32 bit access
 *		
 *
 * RETURNS: ERROR if fail, else OK. 
 *************************************************************************/


int
ramTest32bit (
	UINT32 addr,			/* Start Addr */ 
	UINT32 ct,			/* byte count */
	UINT32 testFlag,		/* Test Flag */
	UINT32 * errCount		/* Error count to increment ;
					 *  could be a running count, or a new
					 *   count for each invocation 
					 */
	)
{

	volatile UINT32 * 	pDram;
	volatile UINT32 * 	pDramEnd;
	UINT32 		data ;		/* size of test units */
/*	UINT32 		trash ;*//*vijay-*//* unused data */
	UINT32 		val ;
	UINT32 		ix, jx;
	UINT32 		ixLast, jxLast;
	UINT32 		dramStart ;
	UINT32 		dramEnd ;
	UINT32 		startErrorCt ;
	UINT32 		dramUnits ;
	

#define TESTMASK_32	0xFFFFFFFF




	jxLast = 0;
	dramStart = addr ; 
	dramEnd = addr + ct - 1; 

        dramEnd = dramEnd & 0xFFFFFFFC;		/* Round down */
	dramUnits = ct / sizeof (data);


	/* Starting Error count */
	startErrorCt = *errCount;



#ifdef DEBUG_MEMTEST
	printf ("\nramTest32bit: DBG startaddr 0x%x,  byte count 0x%x ",
			addr, ct);
	printf ("testFlag 0x%x,  errCount  0x%x \n",
			testFlag,  *errCount);
#endif





    /* ---------------- Alternating 32bit pattern -------------------*/

        printf("\nAlternating Pattern test 32bit......fill");
	for (ix=0, pDram = (UINT32 *)dramStart; ix< dramUnits ; ix++)
        {       if( ix & 1 ) 
                        val = (UINT32)PATTERN1;
                else
                        val = (UINT32)PATTERN2;
		*pDram = val ;
		 pDram++;
		ramTestPrintDot ( );
        }
             


	TASKDELAY;	/* For other processes */
#ifdef DEBUG_MEMTEST
 {
	UINT32 iy;
	pDram = (UINT32 *)dramStart; 

	for (iy=0; iy < PRINTVALS; iy++)
	{
		printf (" Addr 0x%x val = 0x%x ", (UINT32) pDram, *pDram);
		pDram++;
	}
	taskDelay (10);
 }
#endif
        printf("\rIncrement test 16bit ......test forward");

        printf("\rAlternating Pattern test 32bit......test forward");
	for (ix=0, pDram = (UINT32 *)dramStart; ix< dramUnits ; ix++)
        {        

loopsame1:
               /*Commented by vijay                                */
               /*trash = *((UINT32*)TRASHLOCATION);*/ /* read something else..*/
                                                  /* ..to clear bus load */
               if( ix & 1 ) 
			val = PATTERN1 & TESTMASK_32	;
	       else
			val = PATTERN2 & TESTMASK_32;
               data = *pDram ;
               data &= TESTMASK_32 ;
	       if ( data != val )
	  	{
		 printDramErr ( (UINT32) pDram, (UINT32) val, (UINT32) data);

	         *errCount +=1 ;		
		 if (testFlag & SYS_TESTS_FLAG_STOP)
		 	return(ERROR);
		 if (testFlag & SYS_TESTS_FLAG_CONTINUE	)
		 	goto next1;
		 if (testFlag & SYS_TESTS_FLAG_LOOPFAIL	)
		 	goto loopsame1;
	  	} /* End if  */

next1:
	        pDram++ ;
		ramTestPrintDot ( );
	      } /* End for */



	TASKDELAY;	/* For other processes */

#ifdef DEBUG_MEMTEST
 {
	UINT32 iy;
	pDram = (UINT32 *)dramStart; 

	for (iy=0; iy < PRINTVALS; iy++)
	{
		printf (" Addr 0x%x val = 0x%x ", (UINT32) pDram, *pDram);
		pDram++;
	}
	taskDelay (10);
 }
#endif

    /* ---------------- Alternating 32bit reverse pattern -------------------*/

        printf("\nAlternating reverse Pattern test 32bit......fill");
	for (ix=0, pDram = (UINT32 *)dramStart; ix< dramUnits ; ix++)
        {       if( ix & 1 ) 
                        val = (UINT32)PATTERN2;
                else
                        val = (UINT32)PATTERN1;
		*pDram = val ;
		 pDram++;
		ramTestPrintDot ( );
        }
             


	pDram--;
	pDramEnd = pDram;
	ixLast = ix - 1;


	TASKDELAY;	/* For other processes */

#ifdef DEBUG_MEMTEST
 {
	UINT32 iy;
	pDram = (UINT32 *)dramStart; 

	for (iy=0; iy < PRINTVALS; iy++)
	{
		printf (" Addr 0x%x val = 0x%x ", (UINT32) pDram, *pDram);
		pDram++;
	}
	taskDelay (10);
 }
#endif


        printf("\rAlternating reverse Pattern test 32bit......test reverse");
	for (ix=ixLast, pDram = pDramEnd; ix > 0 ; ix--)
        {        

loopsame2:
               /*Commented by vijay                                */
               /*trash = *((UINT32*)TRASHLOCATION);*/ /* read something else..*/
                                                  /* ..to clear bus load */
               if( ix & 1 ) 
			val = PATTERN2 & TESTMASK_32;
                else
                        val = PATTERN1 & TESTMASK_32;
               data = *pDram ;
               data &= TESTMASK_32 ;
	       if ( data != val )
	  	{
		 printDramErr ( (UINT32) pDram, (UINT32) val, (UINT32) data);

	         *errCount +=1 ;		
		 if (testFlag & SYS_TESTS_FLAG_STOP)
		 	return(ERROR);
		 if (testFlag & SYS_TESTS_FLAG_CONTINUE	)
		 	goto next2;
		 if (testFlag & SYS_TESTS_FLAG_LOOPFAIL	)
		 	goto loopsame2;
	  	} /* End if  */

next2:
	        pDram-- ;
		ramTestPrintDot ( );
	      } /* End for */


    /* ---------------- Walking one test -----------------------------*/

        printf("\nWalking ones test 32bit ......fill");
	jx = 1;
	for (ix=0, pDram = (UINT32 *)dramStart; ix< dramUnits ; ix++)
        {       
		*pDram = (UINT32) jx ;
		jx <<=1;
		if (jx == 0) jx = 1;
		pDram++;
		ramTestPrintDot ( );
        }
             
	TASKDELAY;	/* For other processes */


#ifdef DEBUG_MEMTEST
 {
	UINT32 iy;
	pDram = (UINT32 *)dramStart; 

	for (iy=0; iy < PRINTVALS; iy++)
	{
		printf (" Addr 0x%x val = 0x%x ", (UINT32) pDram, *pDram);
		pDram++;
	}
	taskDelay (10);
 }
#endif

        printf("\rWalking ones test 32bit ......test forward");
	jx = 1;
	for (ix=0, pDram = (UINT32 *)dramStart; ix< dramUnits ; ix++)
        {        

loopsame3:
               /*Commented by vijay                                */
               /*trash = *((UINT32*)TRASHLOCATION);*/ /* read something else..*/
                                                  /* ..to clear bus load */
               data = *pDram ;
               data &= TESTMASK_32 ;
	       if ( data != jx )
	  	{
		 printDramErr ( (UINT32) pDram, (UINT32) jx, (UINT32) data);

	         *errCount +=1 ;		
		 if (testFlag & SYS_TESTS_FLAG_STOP)
		 	return(ERROR);
		 if (testFlag & SYS_TESTS_FLAG_CONTINUE	)
		 	goto next3;
		 if (testFlag & SYS_TESTS_FLAG_LOOPFAIL	)
		 	goto loopsame3;
	  	} /* End if  */

next3:
	        pDram++ ;
		jx <<=1;
		if (jx == 00) jx = 1;
		ramTestPrintDot ( );
	      } /* End for */


    /* ---------------- Walking zeros test -----------------------------*/

        printf("\nWalking zeros test 32bit ......fill");
	jx = 1;
	for (ix=0, pDram = (UINT32 *)dramStart; ix< dramUnits ; ix++)
        {
		*pDram = ~((UINT32) jx) ;
		jxLast = jx;
		jx <<=1;
		if (jx == 00) jx = 1;
		pDram++;
		ramTestPrintDot ( );
	}



	pDram--;
	pDramEnd = pDram;
	ixLast = ix - 1;


	TASKDELAY;	/* For other processes */

#ifdef DEBUG_MEMTEST
 {
	UINT32 iy;
	pDram = (UINT32 *)dramStart; 

	for (iy=0; iy < PRINTVALS; iy++)
	{
		printf (" Addr 0x%x val = 0x%x ", (UINT32) pDram, *pDram);
		pDram++;
	}
	taskDelay (10);
 }
#endif



        printf("\rWalking zeros test 32bit ......test reverse");
	jx = jxLast;
	for (ix=ixLast, pDram = pDramEnd; ix > 0 ; ix--)
        {        

loopsame4:
               /*Commented by vijay                                */
               /*trash = *((UINT32*)TRASHLOCATION);*/ /* read something else..*/
                                                  /* ..to clear bus load */
               data = *pDram ;
               data =  (~data) & TESTMASK_32 ;
	       if ( data != jx )
	  	{
		 printDramErr ( (UINT32) pDram, (UINT32) ~jx, (UINT32) ~data);

	         *errCount +=1 ;		
		 if (testFlag & SYS_TESTS_FLAG_STOP)
		 	return(ERROR);
		 if (testFlag & SYS_TESTS_FLAG_CONTINUE	)
		 	goto next4;
		 if (testFlag & SYS_TESTS_FLAG_LOOPFAIL	)
		 	goto loopsame4;
	  	} /* End if  */
next4:
	        pDram-- ;
		jx >>=1;
		if (jx == 00) jx = 0x80000000;
		ramTestPrintDot ( );
	      } /* End for */



    /* ---------------- Increment test -----------------------------*/

        printf("\nIncrement test 32bit ......fill");
	for (ix=0, pDram = (UINT32 *)dramStart; ix< dramUnits ; ix++)
	{
		*pDram++ = ix;
		ramTestPrintDot ( );
	}


	TASKDELAY;	/* For other processes */

#ifdef DEBUG_MEMTEST
 {
	UINT32 iy;
	pDram = (UINT32 *)dramStart; 

	for (iy=0; iy < PRINTVALS; iy++)
	{
		printf (" Addr 0x%x val = 0x%x ", (UINT32) pDram, *pDram);
		pDram++;
	}
	taskDelay (10);
 }
#endif




        printf("\rIncrement test 32bit ......test forward");
	for (ix=0, pDram = (UINT32 *)dramStart; ix< dramUnits ; ix++)
        {        

loopsame5:
               /*Commented by vijay                                */
               /*trash = *((UINT32*)TRASHLOCATION);*/ /* read something else..*/
                                                  /* ..to clear bus load */

               data = *pDram ;
	       if ( data != (ix & TESTMASK_32) )
	  	{
		 printDramErr ( (UINT32) pDram, (UINT32) (ix & TESTMASK_32), 
								(UINT32) data);

	         *errCount +=1 ;		
		 if (testFlag & SYS_TESTS_FLAG_STOP)
		 	return(ERROR);
		 if (testFlag & SYS_TESTS_FLAG_CONTINUE	)
		 	goto next5;
		 if (testFlag & SYS_TESTS_FLAG_LOOPFAIL	)
		 	goto loopsame5;
	  	} /* End if  */
next5:
	        pDram++ ;
		ramTestPrintDot ( );
	      } /* End for */




    /* ---------------- March test -----------------------------*/

        printf("\nMarch pattern test1  32bit ......fill");
	for (ix=0, pDram = (UINT32 *)dramStart; ix< dramUnits ; ix++)
	{
		*pDram++ = PATTERNF;
		ramTestPrintDot ( );
	}


	TASKDELAY;	/* For other processes */

#ifdef DEBUG_MEMTEST
 {
	UINT32 iy;
	pDram = (UINT32 *)dramStart; 

	for (iy=0; iy < PRINTVALS; iy++)
	{
		printf (" Addr 0x%x val = 0x%x ", (UINT32) pDram, *pDram);
		pDram++;
	}
	taskDelay (10);
 }
#endif



        printf("\rMarch pattern test1 32bit ......test forward");
	for (ix=0, pDram = (UINT32 *)dramStart; ix< dramUnits ; ix++)
        {        

loopsame6:
               /*Commented by vijay                                */
               /*trash = *((UINT32*)TRASHLOCATION);*/ /* read something else..*/
                                                  /* ..to clear bus load */

               data = *pDram ;
	       if ( data != PATTERNF  )
	  	{
		 printDramErr ( (UINT32) pDram, (UINT32) PATTERNF, (UINT32) data);

	         *errCount +=1 ;		
		 if (testFlag & SYS_TESTS_FLAG_STOP)
		 	return(ERROR);
		 if (testFlag & SYS_TESTS_FLAG_CONTINUE	)
		 	goto next6;
		 if (testFlag & SYS_TESTS_FLAG_LOOPFAIL	)
		 	goto loopsame6;
	  	} /* End if  */
next6:
                *pDram = PATTERN0;	/* some more exercises on same addr*/
                *pDram = PATTERNF;
                *pDram = PATTERN0;

	        pDram++ ;
		ramTestPrintDot ( );
	      } /* End for */


        pDram--;
        pDramEnd = pDram;
        ixLast = ix - 1;

        printf("\rMarch pattern test2 32bit ......test reverse");
	for (ix= ixLast, pDram = pDramEnd ; ix > 0  ; ix--)
        {        

loopsame7:
               /*Commented by vijay                                */
               /*trash = *((UINT32*)TRASHLOCATION);*/ /* read something else..*/
                                                  /* ..to clear bus load */
               data = *pDram ;

	       if ( data != PATTERN0  )
	  	{
		 printDramErr ( (UINT32) pDram, (UINT32) PATTERN0, (UINT32) data);

	         *errCount +=1 ;		
		 if (testFlag & SYS_TESTS_FLAG_STOP)
		 	return(ERROR);
		 if (testFlag & SYS_TESTS_FLAG_CONTINUE	)
		 	goto next7;
		 if (testFlag & SYS_TESTS_FLAG_LOOPFAIL	)
		 	goto loopsame7;
	  	} /* End if  */
next7:
                /* some more exercises on same addr*/
                *pDram = PATTERNF;
                *pDram = PATTERN0;

	        pDram-- ;
		ramTestPrintDot ( );

	      } /* End for */



    /* ---------------- Address at Address test ----------------------------*/

        printf("\nAddress at Address test 32bit ......fill");
	for (ix=0, pDram = (UINT32 *)dramStart; ix< dramUnits ; ix++)
	{
		*pDram++ = (UINT32) pDram;
		ramTestPrintDot ( );
	}



	TASKDELAY;	/* For other processes */



#ifdef DEBUG_MEMTEST
 {
	UINT32 iy;
	pDram = (UINT32 *)dramStart; 

	for (iy=0; iy < PRINTVALS; iy++)
	{
		printf (" Addr 0x%x val = 0x%x ", (UINT32) pDram, *pDram);
		pDram++;
	}
	taskDelay (10);
 }
#endif

        printf("\nAddress at Address test 32bit ......test forward");
	for (ix=0, pDram = (UINT32 *)dramStart; ix < dramUnits ; ix++)
        {        

loopsame8:
               /*Commented by vijay                                */
               /*trash = *((UINT32*)TRASHLOCATION);*/ /* read something else..*/
                                                  /* ..to clear bus load */
               data = *pDram ;

	       if ( data != (UINT32) pDram )
	  	{
		 printDramErr ( (UINT32) pDram, (UINT32) pDram, (UINT32) data);

	         *errCount +=1 ;		
		 if (testFlag & SYS_TESTS_FLAG_STOP)
		 	return(ERROR);
		 if (testFlag & SYS_TESTS_FLAG_CONTINUE	)
		 	goto next8;
		 if (testFlag & SYS_TESTS_FLAG_LOOPFAIL	)
		 	goto loopsame8;
	  	} /* End if  */
next8:
	        pDram++ ;
		ramTestPrintDot ( );

	      } /* End for */



 /*
  * Exit test - 
  *  if start error count != end error count, then we had an error
  */
	if (startErrorCt != *errCount)
		return (ERROR);
	else
		return (OK);



} /* End ramTest32bit   */




