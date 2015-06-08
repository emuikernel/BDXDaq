/* sysCache.h - secondary (L2) cache header for the Hawk SMC */

/* Copyright 1999-2000 Motorola, Inc., All Rights Reserved. */

/*
modification history
--------------------
01b,12jun01,dtr  adding defines required by cache invalidate function.
01a,13mar00,dmw  Written (from version 01b of mpcn765/sysCache.h).
*/

#ifndef    INCsysCacheh
#define    INCsysCacheh

#ifdef __cplusplus
    extern "C" {
#endif /* __cplusplus */

/* includes */

#define L2CR_REG                1017  /* l2CR special purpose register number */
#define L2CR_GBL_INV_U   	0x0020  /* L2 Global invalidate */
#define L2CR_DISABLE_MASK_U     0x7fff	/* Disable L2 - upper 16 bits  */
#define L2CR_IP                 0x0001  /* Invalidation in progress */
/* L2 Cache Packet Field Identifiers */
#define L2C_MID            0x00 /* Manufacturer's identifier */
#define L2C_DID            0x02 /* Manufacturer's device identifier */
#define L2C_DDW            0x04 /* Device data width */
#define L2C_NOD            0x05 /* Number of devices present */
#define L2C_NOC            0x06 /* Number of columns */
#define L2C_CW             0x07 /* Column width in bits */
#define L2C_TYPE           0x08 /* L2 cache type */
#define L2C_ASSOCIATE      0x09 /* Associative microprocessor number */
#define L2C_OPERATIONMODE  0x0a /* Operation mode */
#define L2C_ERROR_DETECT   0x0b /* Error detection type */
#define L2C_SIZE           0x0c /* L2 cache size */
#define L2C_TYPE_BACKSIDE  0x0d /* L2 backside cache type */
#define L2C_RATIO_BACKSIDE 0x0e /* Core to cache ratio */

/* Cache type */

#define L2C_TYPE_ARTHUR    0x00 /* L2 type arthur */
#define L2C_TYPE_EXTERNAL  0x01 /* L2 type external */
#define L2C_TYPE_INLINE    0x02 /* L2 type inline */

/* Cache operation mode */

#define L2C_OM_WTWBSW      0x00 /* Write-through/write-back. S/W config */ 
#define L2C_OM_WTWBHW      0x01 /* Write-through/write-back. H/W config */
#define L2C_OM_WT          0x02 /* Write-through */
#define L2C_OM_WB          0x03 /* Write-back */

/* Cache error detection type */

#define L2C_ED_NONE        0x00 /* no error detection */
#define L2C_ED_PARITY      0x01 /* parity error detection */
#define L2C_ED_ECC         0x02 /* ECC error detection and correction */

/* Cache size */

#define L2C_SIZE_256K      0x00 /* L2 cache size 256KB */
#define L2C_SIZE_512K      0x01 /* L2 cache size 512KB */
#define L2C_SIZE_1M        0x02 /* L2 cache size 1MB */
#define L2C_SIZE_2M        0x03 /* L2 cache size 2MB */
#define L2C_SIZE_4M        0x04 /* L2 cache size 4MB */

/* Cache backside type */

#define L2C_TYPE_BS_LWP    0x00 /* Late Write Pipelined */
#define L2C_TYPE_BS_BP     0x01 /* Burst Pipelined */
#define L2C_TYPE_BS_LWNP   0x02 /* Pipelined Burst */ 
#define L2C_TYPE_BS_BNP    0x03 /* Pipelined Late-Write */

/* Cache backside ratio (VPD defines) */

#define L2C_RATIO_DISABLE	0x00 /* L2 clock and DLL disabled */
#define L2C_RATIO_1_1		0x01 /* 1:1 (1.0) */
#define L2C_RATIO_3_2		0x02 /* 3:2 (1.5) */
#define L2C_RATIO_2_1		0x03 /* 2:1 (2.0) */
#define L2C_RATIO_5_2		0x04 /* 5:2 (2.5) */
#define L2C_RATIO_3_1		0x05 /* 3:1 (3.0) */

/* Memory sizes */

#define L2C_MEM_NONE       0x0      /* VPD L2 Memory None */
#define L2C_MEM_256K       0x40000  /* VPD L2 Memory 256K */
#define L2C_MEM_512K       0x80000  /* VPD L2 Memory 512K */
#define L2C_MEM_1M         0x100000 /* VPD L2 Memory 1M */
#define L2C_MEM_2M         0x200000 /* VPD L2 Memory 2M */
#define L2C_MEM_4M         0x400000 /* VPD L2 Memory 4M */

/* L2 Cache Control Register, Arthur (MPC750) & Max */

#define MPC750_L2CR_E         0x80000000 /* L2 enable */
#define MPC750_L2CR_PE        0x40000000 /* L2 data parity enable */
#define MPC750_L2CR_256K      0x10000000 /* Arthur only */
#define MPC750_L2CR_512K      0x20000000 /* L2 cache size 512KB */
#define MPC750_L2CR_1024K     0x30000000 /* L2 cache size 1MB */
#define MPC750_L2CR_2048K     0x00000000 /* Max only */
#define MPC750_L2CR_RAM_PLTWR 0x01800000 /* Pipelined late-write */
#define MPC750_L2CR_RAM_PBRST 0x01000000 /* Pipelined burst */
#define MPC750_L2CR_I         0x00200000 /* L2 global invalidate */
#define MPC750_L2CR_OH_1NS    0x00010000 /* Output hold = 1.0 ns */
#define MPC750_L2CR_SL        0x00008000 /* DLL slow */
#define MPC750_L2CR_DF        0x00004000 /* differential clock */
#define MPC750_L2CR_FA        0x00001000 /* Max only - L2 flush assist */
#define MPC750_L2CR_HWF       0x00000800 /* Max only - L2 H/W flush */
#define MPC750_L2CR_IP        0x00000001 /* L2 global invalidate in-progress */

#define MPC750_L2_CACHE_SPEED_THRESHOLD  100000000 /* normal L2 spd threshold */

#ifdef __cplusplus
    }
#endif /* __cplusplus */

#endif    /* INCsysCacheh */

