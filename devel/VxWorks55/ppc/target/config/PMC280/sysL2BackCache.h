/*  sysL2BackCache - defines registers and values for L2 cache control */

/* Copyright 1984-1999 Wind River Systems, Inc. */

/* Copyright 1996,1998 Motorola, Inc. */

/*
modification history
--------------------
01f,24jan02,mil  Changed define of CPU7410 to SP7410.
01e,22jan02,pcs  Merge from Tor2.1
01d,13apr01,mil  Added -DCPU7410 to differentiate from 7410
01c,11apr01,mil  Added PM support for 7410
01b,22jan01,pcs  Add changes as received from teamF1
01b,15jan01,ksn   moved L2 Ram size assignment to bootom. (teamF1)
01a,10oct99,mtl   written from 750 by teamF1
*/

/*
DESCRIPTION

This file defines registers and values needed in defining l2 cache functions.
*/

#ifndef __INCsysL2BackCacheh
#define __INCsysL2BackCacheh

#ifdef __cplusplus
extern "C" {
#endif

#include "vxWorks.h"

#define L2CR_REG                1017    /* l2CR register number */
#define HID0_REG                1008
#define PVR_REG                 287     /* pvr register number */
#define ARTHUR                  8       /* Upper bit 16 bit value of 740/750 */
#define VGER                    0x8000  /* Upper bit 16 bit value of 740/750 */

#define WRITE_ADDR_U 		0x0060	/* upper 16 bits of write address */
#define L2_SIZE_2M_U 		0x0020	/* upper 16 bitts of 2 Meg */
#define L2_ADR_INCR 		0x100	/* address increament value */
#define L2_SIZE_2M 		0x2000  /* 2 MG (0x200000) / 0x100 = 0x2000 */
#define L2_SIZE_1M 		0x1000  /* 1 MG (0x100000) / 0x100 = 0x1000 */
#define L2_SIZE_HM 		0x800   /* 512K counts  */ 
#define L2_SIZE_QM 		0x400  /* 256K(0x40000) / L2_ADR_INCR = 0x40  */


#define BUFFER_ADDR             0x100000   /* buffer address for read */
#define L1CACHE_ALIGN_SIZE      32       /* 32 bytes */
#define L2RAM_SIZE_2M           2097152  /* 2M of L2 RAM */
#define L2RAM_SIZE_1M           1048576  /* 1M of L2 RAM */
#define L2RAM_SIZE_512KB        524288   /* 512 KB of L2 RAM */
#define L2RAM_SIZE_256KB        262144   /* 256 KB 0f L2 RAM */

/*
 * Defining values for L2CR register: 
 *  -  L2 cache enable (1) / disable (0)  (bit 0)
 *  -  cache size (bits 2-3; 3: 1 MB, 2: 512 KB, 1: 256 KB)
 *  -  1.5 clock ratio (bits 4-6)
 *  -  Pinpelined (register-register) synchronous burst RAM (bits 7-8)
 *  -  L2 Data only (bit 9)
 *  -  Test mode on (1) or off (0) (bit 13)
 *
 */

/* L2 Data Parity generation and checking enable */
    
#define L2CR_PE                 0x4000

/* values for the L2SIZ bits */
#define L2CR_SIZE_2MB            0x0000
#define L2CR_SIZE_256KB          0x1000
#define L2CR_SIZE_512KB          0x2000
#define L2CR_SIZE_1MB            0x3000

/* values for L2CLK bits */
#define L2CR_CLK_DISABLE        0x0000
#define L2CR_CLK_1              0x0200
#define L2CR_CLK_1_5            0x0400
#define L2CR_CLK_3_5            0x0600
#define L2CR_CLK_2              0x0800
#define L2CR_CLK_2_5            0x0a00
#define L2CR_CLK_3              0x0c00
#define L2CR_CLK_4              0x0e00

/* values for L2RAM Type */
#define L2CR_RAM_SYNBRST        0x0000  /* synchronous burst SRAM */ 
#define L2CR_RAM_PB3            0x0080  /* PB3 synchronous burst SRAM (7xx)*/
#define L2CR_RAM_PB2            0x0100  /* PB2 synchronous burst SRAM */
#define L2CR_RAM_SYNLTWRT       0x0180  /* Synchronouus Late write SRAM */


#if defined(SP7450)
  /* L2 data only */
  #define L2DO                    0x0001
  /* L2 Ins. only mode */
  #define L2IO                    0x0010
#else
  /* L2 data only */
  #define L2DO                    0x0040
  /* L2 Ins. only mode */
  #define L2IO                    0x0400
#endif
/* L2 Global invalidate */
#define L2CR_GBL_INV_U   	0x0020
    
/* L2 Ram Control (ZZ enable) */
#define L2CR_RAMCTL             0x0010

/* L2 Write Through */
#define L2CR_WT                 0x0008

/* L2 Test Support */
#define L2CR_TS                 0x0004

/* L2 Output Hold */
#define L2CR_OH_0_5ns           0x0000
#define L2CR_OH_1_0ns           0x0001
#define L2CR_OH_MORE            0x0002
#define L2CR_OH_EVEN_MORE       0x0003

#define L2CR_HWFLUSH            0x0800  /* hardware flush for 74X0 lower 16 bits*/
    
    
#define L2CR_CFG                ( L2_CACHE_SIZE |  \
                                  L2CR_RAM_PB2  |  \
                                  L2CR_OH_0_5ns)


#define L2CR_DISABLE_MASK_U     0x7fff	/* Disable L2 - upper 16 bits  */
#define L2CR_EN_U               0x8000  /* Set L2CR enable bit */
#define L2CR_IP                 0x0001  /* Invalidation in progress */

#if (L2_CACHE_SIZE == L2CR_SIZE_2MB)
   #if defined(INCLUDE_CACHE_L2) && defined(SP7410)
   #define L2RAM_SIZE    L2RAM_SIZE_2M
   #else
   #define L2RAM_SIZE    0
   #endif /* INCLUDE_CACHE_L2 && SP7410 */
#elif (L2_CACHE_SIZE == L2CR_SIZE_1MB)
#define L2RAM_SIZE    L2RAM_SIZE_1M
#elif (L2_CACHE_SIZE == L2CR_SIZE_512KB)
#define L2RAM_SIZE    L2RAM_SIZE_512KB
#elif (L2_CACHE_SIZE == L2CR_SIZE_256KB)
#define L2RAM_SIZE    L2RAM_SIZE_256KB
#endif


#ifdef INCLUDE_L2PM

/* L2PM Register address */
#define L2PM_REG                 1016    
/* Base Address for the L2 Private Memory equivalent to mem size*/    
#define L2PMBA                   ( L2PM_SRAM_ADRS >> 16 )
/* L2PM_SIZ bit configration to determine the size of L2 private memeory */
#if defined(SP7410)
#define L2PM_EN 0x0004		/* 7410 uses EN bit */
#define L2PM_SIZE_2M             L2PM_EN | 0x0000
#define L2PM_SIZE_256KB          L2PM_EN | 0x0001
#define L2PM_SIZE_512KB          L2PM_EN | 0x0002
#define L2PM_SIZE_1M             L2PM_EN | 0x0003   
#elif defined(SP755)
#define L2PM_SIZE_256KB          0x0001
#define L2PM_SIZE_512KB          0x0002
#define L2PM_SIZE_1M             0x0003
#endif /* SP7410 */

#endif /* INCLUDE_L2PM */

#define L1_DCACHE_ENABLE	0x4400
#define L1_DCACHE_INV_MASK	0xfbff
#define L1_DCACHE_DISABLE	0xbfff		    

#ifndef _ASMLANGUAGE

extern void  sysL1DcacheEnable(void);
extern void  sysL2BackEnable(CACHE_TYPE cache);
extern void  sysL1DcacheDisable(void);
extern void  sysL2BackDisable(void);
extern void  sysL2BackGlobalInv(void);
extern ULONG sysPVRReadSys();
extern void  sysL2BackInit(void);
extern void  sysL2BackHWFlush(void);
extern void  sysL2BackSetWT(void);
    
#if defined(INCLUDE_L2PM) && (defined(SP755) || defined(SP7410))
extern void  sysL2PMEnable();   
#endif /* INCLUDE_L2PM && (SP755 || SP7410) */

#endif

#ifdef __cplusplus
}
#endif

#endif /* __INCsysL2BackCacheh */
