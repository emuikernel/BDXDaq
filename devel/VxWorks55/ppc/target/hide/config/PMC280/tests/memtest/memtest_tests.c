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
* Filename:memtest_tests.c                                                    *
*                                                                             *
* DESCRIPTION:	                                                              *
*       This file contains implementation of Extended memtests                *
*                                                                             *
* DEPENDENCIES:	                                                              *
*       None.                                                                 *
*                                                                             *
******************************************************************************/

#include <stdio.h>
#include "memtest_config.h"
#include "memtest_tests.h"

void extendedTestError (	UINT32 ,	UINT32 ,UINT32);

int
test_verify_success (ui32 *bp1, ui32 *bp2, ui32 count)
{
    volatile ui32 *p1 = (volatile ui32 *) bp1;
    volatile ui32 *p2 = (volatile ui32 *) bp2;
    ui32 i;

    if (!silent_test)
    {

    }

    for (i = 0; i < count; i++, p1++, p2++)
    {
        if (*p1 != *p2)
        {
        extendedTestError ((UINT32)p1,*p1,*p2);
            return (MEMTEST_ERROR);
        }
    }

    return (MEMTEST_OK);
}

int
test_xor_comparison (ui32 *bp1, ui32 *bp2, ui32 count)
{
    volatile ui32 *p1 = (volatile ui32 *) bp1;
    volatile ui32 *p2 = (volatile ui32 *) bp2;
    ui32 i;
    ui32 q = RAND32;
	
    silent_test = 1;		
    for (i = 0; i < count; i++)
    {
        *p1++ ^= q;
        *p2++ ^= q;
    }
    return (test_verify_success (bp1, bp2, count));
}


int
test_sub_comparison (ui32 *bp1, ui32 *bp2, ui32 count)
{
    volatile ui32 *p1 = (volatile ui32 *) bp1;
    volatile ui32 *p2 = (volatile ui32 *) bp2;
    ui32 i;
    ui32 q = RAND32;
	
    silent_test = 1;		
    for (i = 0; i < count; i++)
    {
        *p1++ -= q;
        *p2++ -= q;
    }
    return (test_verify_success (bp1, bp2, count));
}


int
test_mul_comparison (ui32 *bp1, ui32 *bp2, ui32 count)
{
    volatile ui32 *p1 = (volatile ui32 *) bp1;
    volatile ui32 *p2 = (volatile ui32 *) bp2;
    ui32 i;
    ui32 q = RAND32;

    silent_test = 1;		
    for (i = 0; i < count; i++)
    {
        *p1++ *= q;
        *p2++ *= q;
    }

    return (test_verify_success (bp1, bp2, count));
}


int
test_div_comparison (ui32 *bp1, ui32 *bp2, ui32 count)
{
	volatile ui32 *p1 = (volatile ui32 *) bp1;
	volatile ui32 *p2 = (volatile ui32 *) bp2;
	ui32 i;
	ui32 q = RAND32;
	
	silent_test = 1;		
	for (i = 0; i < count; i++)
	{
		if (!q)
			q++;
		*p1++ /= q;
		*p2++ /= q;
	}
	return (test_verify_success (bp1, bp2, count));
}


int
test_or_comparison (ui32 *bp1, ui32 *bp2, ui32 count)
{
    volatile ui32 *p1 = (volatile ui32 *) bp1;
    volatile ui32 *p2 = (volatile ui32 *) bp2;
    ui32 i;
    ui32 q = RAND32;
	
    silent_test = 1;		
    for (i = 0; i < count; i++)
    {
        *p1++ |= q;
        *p2++ |= q;

    }
    return (test_verify_success (bp1, bp2, count));
}


/*Slightly modified by vijay*/

int
test_and_comparison (ui32 *bp1, ui32 *bp2, ui32 count)
{
    volatile ui32 *p1 = (volatile ui32 *) bp1;
    volatile ui32 *p2 = (volatile ui32 *) bp2;
    ui32 i, retVal=0;
    ui32 q = RAND32;

    silent_test = 1;		
    for (i = 0; i < count; i++)
    {
        *p1=0;*p2 =0;
        *p1++ &= q;
        *p2++ &= q;

    }
 
    retVal = test_verify_success (bp1, bp2, count);
 

    return (retVal);
}


int
test_seqinc_comparison (ui32 *bp1, ui32 *bp2, ui32 count)
{
    volatile ui32 *p1 = (volatile ui32 *) bp1;
    volatile ui32 *p2 = (volatile ui32 *) bp2;
    ui32 i;
    ui32 q = RAND32;
	
    silent_test = 1;		
    for (i = 0; i < count; i++)
    {
        *p1++ = *p2++ = (i + q);
    }
    return (test_verify_success (bp1, bp2, count));
}


int
test_solidbits_comparison (ui32 *bp1, ui32 *bp2, ui32 count)
{
    volatile ui32 *p1 = (volatile ui32 *) bp1;
    volatile ui32 *p2 = (volatile ui32 *) bp2;
    ui32 q, i, j;

    silent_test = 1;		
/*	printf( "    ");*/
    for (j = 0; j < 64; j++)
    {
        q = (j % 2) == 0 ? 0xFFFFFFFF : 0x00000000;
#if 0
        printf( "\b\b\b\b\b\b\b\b\b\b\b\b\b\bSetting... %3lu", j);
#endif
        p1 = (volatile ui32 *) bp1;
        p2 = (volatile ui32 *) bp2;
        for (i = 0; i < count; i++)
        {
            *p1++ = *p2++ = (i % 2) == 0 ? q : ~q;
        }
#if 0	
        printf( "\b\b\b\b\b\b\b\b\b\b\b\b\b\bTesting... %3lu", j);
#endif

        if (test_verify_success (bp1, bp2, count) == MEMTEST_ERROR)
        {
            return (MEMTEST_ERROR);
        }
    }
/*	printf( "\b\b\b\b");*/
    return (MEMTEST_OK);
}



int
test_checkerboard_comparison (ui32 *bp1, ui32 *bp2, ui32 count)
{
    volatile ui32 *p1 = (volatile ui32 *) bp1;
    volatile ui32 *p2 = (volatile ui32 *) bp2;
    ui32 q, i, j;

    silent_test = 1;		
/*	printf( "    ");*/
    for(j = 0; j < 64; j++)
    {
        q = (j % 2) == 0 ? 0x55555555 : 0xAAAAAAAA;
#if 0
        printf( "\b\b\b\b\b\b\b\b\b\b\b\b\b\bSetting... %3lu", j);
#endif
        p1 = (volatile ui32 *) bp1;
        p2 = (volatile ui32 *) bp2;
        for(i = 0; i < count; i++)
        {
            *p1++ = *p2++ = (i % 2) == 0 ? q : ~q;
        }
#if 0	
        printf( "\b\b\b\b\b\b\b\b\b\b\b\b\b\bTesting... %3lu", j);
#endif

        if (test_verify_success (bp1, bp2, count) == MEMTEST_ERROR)
        {
            return (MEMTEST_ERROR);
        }
    }
/*	printf( "\b\b\b\b");*/
    return (MEMTEST_OK);
}


int
test_blockseq_comparison (ui32 *bp1, ui32 *bp2, ui32 count)
{
    volatile ui32 *p1 = (volatile ui32 *) bp1;
    volatile ui32 *p2 = (volatile ui32 *) bp2;
    ui32 i, j;
	
    silent_test = 1;		
/*	printf( "    ");*/
    for (j = 0; j < 256; j++)
    {
#if 0
        printf( "\b\b\b\b\b\b\b\b\b\b\b\b\b\bSetting... %3lu", j);
#endif
        p1 = (volatile ui32 *) bp1;
        p2 = (volatile ui32 *) bp2;
        for (i = 0; i < count; i++)
        {
            *p1++ = *p2++ = MAKE32FROM8 (j);
        }
#if 0
        printf( "\b\b\b\b\b\b\b\b\b\b\b\b\b\bTesting... %3lu", j);
#endif

        if (test_verify_success (bp1, bp2, count) == MEMTEST_ERROR)
        {
            return (MEMTEST_ERROR);
        }
    }
/*	printf( "\b\b\b\b");*/
    return (MEMTEST_OK);
}



int
test_walkbits_comparison (ui32 *bp1, ui32 *bp2, ui32 count, int m)
{
    volatile ui32 *p1 = (volatile ui32 *) bp1;
    volatile ui32 *p2 = (volatile ui32 *) bp2;
    ui32 i, j;

    silent_test = 1;		
/*	printf( "    ");*/
    for (j = 0; j < 64; j++)
    {
#if 0
        printf( "\b\b\b\b\b\b\b\b\b\b\b\b\b\bSetting... %3lu", j);
#endif
        p1 = (volatile ui32 *) bp1;
        p2 = (volatile ui32 *) bp2;
        for (i = 0; i < count; i++)
        {
            if (j < 32)	/* Walk it up. */
            {
                *p1++ = *p2++ = (m == 0) ? 0x00000001 << j :
                                            0xFFFFFFFF ^ (0x00000001 << j);
            }
            else		/* Walk it back down. */
            {
                *p1++ = *p2++ = (m == 0)
                     ? 0x00000001 << (64 - j - 1)
                        : 0xFFFFFFFF ^ (0x00000001 << (64 - j - 1));
            }				
        }
#if 0
        printf( "\b\b\b\b\b\b\b\b\b\b\b\b\b\bTesting... %3lu", j);
#endif
        if (test_verify_success (bp1, bp2, count) == MEMTEST_ERROR)
        {
            return (MEMTEST_ERROR);
        }
    }
/*	printf( "\b\b\b\b");*/
    return (MEMTEST_OK);
}



int
test_bitspread_comparison (ui32 *bp1, ui32 *bp2, ui32 count)
{
    volatile ui32 *p1 = (volatile ui32 *) bp1;
    volatile ui32 *p2 = (volatile ui32 *) bp2;
    ui32 i, j;

    silent_test = 1;		
/*	printf( "    ");*/
    for (j = 0; j < 64; j++)
    {
#if 0
        printf( "\b\b\b\b\b\b\b\b\b\b\b\b\b\bSetting... %3lu", j);
#endif
        p1 = (volatile ui32 *) bp1;
        p2 = (volatile ui32 *) bp2;
        for (i = 0; i < count; i++)
        {
            if (j < 32)	/* Walk it up. */
            {
                *p1++ = *p2++ = (i % 2 == 0)
                       ? (0x00000001 << j) | (0x00000001 << (j + 2))
                                      : 0xFFFFFFFF ^ ((0x00000001 << j)
                                                   | (0x00000001 << (j + 2)));
            }
            else		/* Walk it back down. */
               {
                   *p1++ = *p2++ = (i % 2 == 0)
                         ? (0x00000001 << (63 - j)) | (0x00000001 << (65 - j))
                                          : 0xFFFFFFFF ^ (0x00000001 << (63 - j) 
                                                       | (0x00000001 << (65 - j)));
               }				
        }
#if 0
        printf( "\b\b\b\b\b\b\b\b\b\b\b\b\b\bTesting... %3lu", j);
#endif
        if (test_verify_success (bp1, bp2, count) == MEMTEST_ERROR)
        {
            return (MEMTEST_ERROR);
        }
    }
/*	printf( "\b\b\b\b");*/
    return (MEMTEST_OK);
}


int
test_bitflip_comparison (ui32 *bp1, ui32 *bp2, ui32 count)
{
    volatile ui32 *p1 = (volatile ui32 *) bp1;
    volatile ui32 *p2 = (volatile ui32 *) bp2;
    ui32 i, j, k;
    ui32 q;
		
    silent_test = 1;		
/*	printf( "    ");*/
    for (k = 0; k < 32; k++)
    {
        q = 0x00000001 << k;

        for (j = 0; j < 8; j++)
        {
            q = ~q;
#if 0
            printf( "\b\b\b\b\b\b\b\b\b\b\b\b\b\bSetting... %3lu", 
                                                            k * 8 + j);
#endif                
            p1 = (volatile ui32 *) bp1;
            p2 = (volatile ui32 *) bp2;
            for (i = 0; i < count; i++)
            {
                *p1++ = *p2++ = (i % 2) == 0 ? q : ~q;
            }
#if 0
            printf( "\b\b\b\b\b\b\b\b\b\b\b\b\b\bTesting... %3lu", 
                                                             k * 8 + j);
#endif
	
            if (test_verify_success (bp1, bp2, count) == MEMTEST_ERROR)
            {
                return (MEMTEST_ERROR);
            }
        }
		
    }
/*	printf( "\b\b\b\b");*/
    return (MEMTEST_OK);
}



int
test_stuck_address (ui32 *bp1, ui32 *unused, ui32 count)
{
    volatile ui32 *p1;
/* second argument is not used; just gives it a compatible signature. */
    ui32 i, j;

    count <<= 1;
/*	printf( "    ");*/
    for(j = 0; j < 16; j++)
    {
        p1 = (volatile ui32 *) bp1;
#if 0
        printf( "\b\b\b\b\b\b\b\b\b\b\b\b\b\bSetting... %3lu", j);
#endif
        for (i = 0; i < count; i++)
        {
            *p1++ = ((j + i) % 2) == 0 ? (ui32) p1 : ~((ui32) p1);
        }
#if 0
        printf( "\b\b\b\b\b\b\b\b\b\b\b\b\b\bTesting... %3lu", j);
#endif
        p1 = (volatile ui32 *) bp1;
        for (i = 0; i < count; i++, p1++)
        {
            if (*p1 != (((j + i) % 2) == 0 ? (ui32) p1 : ~((ui32) p1)))
            {
  			    printf("\nFAILURE: possible bad address line 
                                        at offset" " 0x%08lx.\n", i);
				printf("Skipping to next test...\n");

                return (MEMTEST_ERROR);
            }
        }
    }
/*	printf( "\b\b\b\b");*/
    return (MEMTEST_OK);
}


/**************** other tests      *********************************/
/**************** memtest86 ---tests *******************************/
int 
test_moving_inv32(ui32 *startAddr, ui32 *endAddr, ui32 count)
{
    volatile ui32 *stAd  = startAddr;
    volatile ui32 *endAd = endAddr;

    ui32 pattern     = MEM_PATTERN1;
    ui32 i, numWords;
    
    numWords = startAddr - endAddr;
    
    /*store a pattern in all the memory locations                  */
    for(i =0; i< count ; stAd++,endAd++,i++)
    {
        *stAd = *endAd = pattern;        
    }
    stAd = startAddr;
    endAd = endAddr;
   
    /*Verify the stored pattern and then store the anti pattern    */
    for(i =0; i< count ;stAd++,i++)
    {
        if((*stAd != pattern) && (*endAd != pattern))
        {
          return MEMTEST_ERROR;
        }
        *stAd  = *endAd = ~pattern; 
    }
    
    /*Verify for the anti pattern                                  */
    for(i =0; i< count ;stAd++,i++)
    {
        if((*stAd != pattern) && (*endAd != pattern))
        {
          return MEMTEST_ERROR;
        }
        *stAd = *endAd = 0;
    }
    return MEMTEST_OK;
}


/*To report extended test error*/
/*Added by Anish 24.6.2003*/
void extendedTestError (
	UINT32 addr,			/* Addr of error */ 
	UINT32 pattern,			/* expected pattern */
	UINT32 value			/* value found */
	)
{
        printf ("\nERROR at location: 0x%x ", addr); 
        printf (" read : 0x%x ", value );
        printf (" should be: 0x%x \n", pattern );
}



#if 0
UCHAR moving_inv1_test(ui32 *startAddr, ui32 *endAddr, ui32 count)
{
    volatile ui32 *stAd = startAddr;

    ui32 i, numWords;
    
    numWords = startAddr - endAddr;
    
    /*store a pattern in all the memory locations                  */
    for(i =0; i< numWords; stAd++,i++)
    {
        *stAd = 1;
    }
    stAd = startAddr;
    
    /*Verify the stored pattern and then store the anti pattern    */
    for(i =0; i< numWords ;stAd++,i++)
    {
        if(!*stAd)
        {
          return MEMTEST_FAIL;
        }
        *stAd = 0; 
    }
    
    /*Verify for the anti pattern                                  */
    for(i =0; i< numWords ;stAd++,i++)
    {
        if(*stAd)
        {
          return MEMTEST_FAIL;
        }
    }
    return MEMTEST_SUCCESS;    
}

UCHAR modulo20_test(ui32 *startAddr, ui32 *endAddr, ui32 count)
{
    volatile ui32 *stAd = startAddr, *endAd = endAddr;
    ui32 cnt1;
    
    for(cnt1= 0; stAd <= endAd; stAd++, cnt1++)
    {
        if(cnt1 % 20)
        {
            *stAd = MEM_PATTERN1;
        }        
    }

    stAd = startAddr;
    for(cnt1= 0; stAd <= endAd; stAd++, cnt1++)
    {
        if(!(cnt1 % 20))
        {
            *stAd = ~MEM_PATTERN1;
        }        
    }

    for(cnt1= 0; stAd <= endAd; stAd++, cnt1++)
    {
        if( !(!(cnt1 % 20) & (~MEM_PATTERN1 == *stAd )) || !((cnt1 % 20) & (MEM_PATTERN1 == *stAd )))
        {
          return MEMTEST_FAIL;
        }
    }
    return MEMTEST_SUCCESS;
}

#endif /*#if 0*/

