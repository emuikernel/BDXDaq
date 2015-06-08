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
* Filename:test.h                                                             *
*                                                                             *
* DESCRIPTION:	                                                              *
*       This file contains configuration information for basic tests          *
*                                                                             *
* DEPENDENCIES:	                                                              *
*       None.                                                                 *
*                                                                             *
******************************************************************************/
	
#ifndef _TEST__
#define _TEST__

/* sysTests.c -- tests defines */

  

/*
modification history
--------------------
12jul00	tl	Created.
*/

/*
This file contains constants for on board tests.
*/

#ifndef	INCsystestsh
#define	INCsystestsh



/* Tests flags ; 1 = OK */
#define SYSTEST_FLAG_DRAM		0x01		/* DRAM  */
#define SYSTEST_FLAG_COM		0x02		/* COM/uarts  */
#define SYSTEST_FLAG_ETHER		0x04		/* Ethernet(s)   */
#define SYSTEST_FLAG_ISABRIDGE		0x08		/* ISA - PCI bridge */
#define SYSTEST_FLAG_NVRAM		0x10		/* NVRAM */

#define SYSTEST_FLAG_ALL	(SYSTEST_FLAG_DRAM | SYSTEST_FLAG_COM \
				SYSTEST_FLAG_ETHER | SYSTEST_FLAG_ISABRIDGE | \
				SYSTEST_FLAG_NVRAM)



/* Some test patterns */
#define PATTERN1        0x55555555
#define PATTERN2        0xAAAAAAAA
#define PATTERN0        0x00000000
#define PATTERNF        0xFFFFFFFF

/* Address to read during DRAM tests -- XXX Architecture dependent */
#define TRASHADDR       0x0000000       /* unused addr to access during test*/

#define TRASHLOCATION   TRASHADDR       /* unused addr to access during test*/

/* 
 * System Test Flag
 *      Stop on error
 *      continue on error
 *      loop on failure (continuous repeat of test as long as failure)
 */
#define SYS_TESTS_FLAG_STOP     0x00000001
#define SYS_TESTS_FLAG_CONTINUE 0x00000002
#define SYS_TESTS_FLAG_LOOPFAIL 0x00000004  /* These 3 are mutually exclusive*/



/* defines */


 /* ethernet states */
#define FEILOOP_FNORM   0x0             /* Normal */
#define FEILOOP_FLPDS   0x40            /* Internal loop in CSMA */
#define FEILOOP_FLPPH   0x80            /* Internal loop in PHY unit */
#define FEILOOP_FRES    0xc0            /* reserved */
 
 /* test flag bits */
#define FEILOOP2_FNONE  0x00000000      /* No interrupts */
#define FEILOOP2_FINT   0x00000001      /* Use interrupts */
#define FEILOOP2_FGENI  0x00000002      /* Generate S/W interrupt */
 
 
 /* XXXX ??? define PAGE_SIZE       512 */
#define BYTESPSEC       512             /* Bytes per Sector */
 
#define DCACHE_L1_SIZE          0x00001000      /* L1 Data Cache TBD XXXX */
#define DCACHE_L2_SIZE          0x00100000      /* L2 Data Cache TBD XXXX */
 
 /* XXXX TBD */
#define ATA_CTRL_NUMB_DEF       0               /* Default ATA Controller */
#define ATA_DRIVE_NUMB_DEF      0               /* Default ATA Controller */
 
void systest (void);

#endif /* INCsystestsh */

#endif /*_TEST__*/