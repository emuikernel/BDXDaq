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

*******************************************************************************
* mem_perf.c - Source file for testing the memory performance   
* 
* DESCRIPTION:
*     This program calculates the memory bandwidth (no. of bytes /sec).  
*  
*
* DEPENDENCIES:	
*       None.
*
******************************************************************************/


#define MAX_PERF_SIZE (4*1024*1024)
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#ifdef VXWORKS

#ifndef __LCLINT__
#include <VMXA.h>   /* This file generates a lot of lclint problems
                       omitting it generates less warnings */
#endif
#include <vme.h>
#include <vxLib.h>
#include <intLib.h>
#include <taskLib.h>
#include <iv.h>
#include <ioLib.h>
#endif /* VXWORKS */
#include "tickLib.h"
#include "string.h"
/* Externals */

IMPORT int sysClkRateGet (void);
IMPORT int taskDelay(int);
IMPORT ULONG tickGet ();

/*  declarations  */
int perf(void);


int mem_perf(void)
{
   unsigned long i, mcount; 
   unsigned long  the_tick; 
   double d1, d2; 
   long bufsize;
   unsigned long *pBufx, *pBufy; 

   printf( "\nMemory Performance test program.\n" );

   if( (pBufx= malloc( MAX_PERF_SIZE )) == NULL )
   {
      printf( "malloc error\n" );
      return -1;
   }
   if( (pBufy= malloc( MAX_PERF_SIZE )) == NULL )
   {
      printf( "malloc error\n" );
      return -1;
   }

   printf( "pBufx= %08lx\n", (unsigned long)pBufx );
   printf( "pBufy= %08lx\n", (unsigned long)pBufy );        


   printf( "sysClkRateGet()= %d\n", sysClkRateGet() );

   printf( "     mcount,       bufsize,  ticks,   time [s], BW [MB/s]\n" );
   taskDelay(10);

   bufsize= 1024;
   while( bufsize<= MAX_PERF_SIZE )
   {
      the_tick= 0;
      mcount= 1;
      while( the_tick<100 )
      {
         mcount= 10*mcount;
         tickSet( 0 );
         for( i= 0; i<mcount; i++ )
         {
            memcpy( pBufy, pBufx, bufsize );
            the_tick= tickGet();
         }
      }
      d1= 2.0*mcount*bufsize/1024/1024; 
      d2= 1.0/sysClkRateGet()*the_tick;
      printf( "%12ld, %12ld, %5ld, %8.2f, %10.2f\n", mcount, bufsize, the_tick, d2, d1/d2 );
      taskDelay(10);
      bufsize= bufsize*2;
   }
   free( pBufx );
   free( pBufy );
   return 0;
}

