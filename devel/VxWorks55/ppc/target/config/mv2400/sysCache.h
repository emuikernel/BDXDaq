/* sysCache.h - secondary (L2) cache header for the Hawk SMC */

/* Copyright 1999 Motorola, Inc., All Rights Reserved. */

/*
modification history
--------------------
01a,12jun02,kab  SPR 74987: cplusplus protection
01b,20jan99 mdp	 Added defines for code cleanup.
01a,05jan99 mdp  Written.
*/

#ifndef INCsysCacheh
#define INCsysCacheh

#ifdef __cplusplus
    extern "C" {
#endif

/* L2 Cache Packet Field Identifiers */

#define L2C_MID			0x00 /* Manufacturer's identifier */
#define L2C_DID			0x02 /* Manufacturer's device identifier */
#define L2C_DDW			0x04 /* Device data width */
#define L2C_NOD			0x05 /* Number of devices present */
#define L2C_NOC			0x06 /* Number of columns */
#define L2C_CW			0x07 /* Column width in bits */
#define L2C_TYPE		0x08 /* L2 cache type */
#define L2C_ASSOCIATE		0x09 /* Associative microprocessor number */
#define L2C_OPERATIONMODE	0x0a /* Operation mode */
#define L2C_ERROR_DETECT	0x0b /* Error detection type */
#define L2C_SIZE		0x0c /* L2 cache size */
#define L2C_TYPE_BACKSIDE	0x0d /* L2 backside cache type */
#define L2C_RATIO_BACKSIDE	0x0e /* Core to cache ratio */

/* Cache type */

#define L2C_TYPE_ARTHUR		0x00
#define L2C_TYPE_EXTERNAL	0x01
#define L2C_TYPE_INLINE		0x02

/* Cache operation mode */

#define L2C_OM_WTWBSW		0x00 /* Write-through/write-back. S/W config */ 
#define L2C_OM_WTWBHW		0x01 /* Write-through/write-back. H/W config */
#define L2C_OM_WT		0x02 /* Write-through */
#define L2C_OM_WB		0x03 /* Write-back */

/* Cache error detection type */

#define L2C_ED_NONE		0x00
#define L2C_ED_PARITY		0x01
#define L2C_ED_ECC		0x02

/* Cache size */

#define L2C_SIZE_256K           0x00
#define L2C_SIZE_512K           0x01
#define L2C_SIZE_1M             0x02
#define L2C_SIZE_2M             0x03
#define L2C_SIZE_4M             0x04

/* Cache backside type */

#define L2C_TYPE_BS_LWP         0x00
#define L2C_TYPE_BS_BP          0x01
#define L2C_TYPE_BS_LWNP        0x02
#define L2C_TYPE_BS_BNP         0x03

/* Cache backside ratio */

#define L2C_RATIO_DISABLE	0x00
#define L2C_RATIO_1_1		0x01
#define L2C_RATIO_3_2		0x02
#define L2C_RATIO_2_1		0x03
#define L2C_RATIO_5_2		0x04
#define L2C_RATIO_3_1		0x05

/* Memory sizes */

#define L2C_MEM_NONE		0x0
#define L2C_MEM_256K		0x40000
#define L2C_MEM_512K		0x80000
#define L2C_MEM_1M		0x100000
#define L2C_MEM_2M		0x200000
#define L2C_MEM_4M		0x400000

/* L2 Cache Control Register, Arthur (MPC750) & Max */

#define MPC750_L2CR_E           0x80000000	/* L2 enable */
#define MPC750_L2CR_PE		0x40000000	/* L2 data parity enable */
#define MPC750_L2CR_256K        0x10000000      /* Arthur only */
#define MPC750_L2CR_512K        0x20000000
#define MPC750_L2CR_1024K       0x30000000
#define MPC750_L2CR_2048K       0x00000000      /* Max only */
#define MPC750_L2CR_RAM_PLTWR	0x01800000	/* Pipelined late-write */
#define MPC750_L2CR_RAM_PBRST	0x01000000	/* Pipelined burst */
#define MPC750_L2CR_I           0x00200000
#define MPC750_L2CR_OH_1NS	0x00010000	/* Output hold = 1.0 ns */
#define MPC750_L2CR_SL          0x00008000
#define MPC750_L2CR_DF		0x00004000
#define MPC750_L2CR_HWF         0x00000800      /* Max only */
#define MPC750_L2CR_IP          0x00000001

#ifdef __cplusplus
    }
#endif

#endif /* INCsysCacheh */
