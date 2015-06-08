/* mpc107MemInit.h - MPC107 Memory Controller register definitions */

/* Copyright 1984-2000 Wind River Systems, Inc. */

/*
modification history
--------------------
01a,06jun00,bri written
*/

#ifndef	__INCmpc107MemInith
#define	__INCmpc107MemInith

#ifdef __cplusplus
extern "C" {
#endif

/* includes */

#include "mpc107.h"

/* defines */


/* Memory control configuration Register #1 */

#define MPC107_MCC1_ROMNAL_MASK     0xf0000000 /* ROM nibble access time mask */
#define MPC107_MCC1_ROMNAL_SHIFT    28         /* ROM nibble access time shft */
#define MPC107_MCC1_ROMFAL_MASK     0x0f800000 /* ROM first access time mask */
#define MPC107_MCC1_ROMFAL_SHIFT    23         /* ROM first access time shift */
#define MPC107_MCC1_DBUS_SIZE_MASK  0x00600000 /* ROM/FLASH DBUS size mask */
#define MPC107_MCC1_64N32           0x00400000 /* 64-bit external data path */
#define MPC107_MCC1_8N64            0x00200000 /* 8-bit ROM/Flash */
#define MPC107_MCC1_BURST_MASK      0x00100000 /* burst mode ROM Mask */
#define MPC107_MCC1_BURST_SHIFT     20         /* burst mode SHIFT */
#define MPC107_MCC1_MEMGO_MASK      0x00080000 /* enable RAM interface mask */
#define MPC107_MCC1_MEMGO_SHIFT     19         /* enable RAM interface shift */
#define MPC107_MCC1_SREN_MASK       0x00040000 /* enable self refresh mask */
#define MPC107_MCC1_SREN_SHIFT      18         /* enable self refresh shift */
#define MPC107_MCC1_RAM_TYPE_MASK   0x00020000 /* RAM type: 0=SDRAM,1=FPM/EDO */
#define MPC107_MCC1_RAM_TYPE_SHIFT  17         /* RAM type: 0=SDRAM,1=FPM/EDO */
#define MPC107_MCC1_PCKEN_MASK      0x00010000 /* enable parity checking mask */
#define MPC107_MCC1_PCKEN_SHIFT     16         /* enable parity checking shif */
#define MPC107_MCC1_ROW_ADRS_MASK   0x0000ffff /* row address mask */

#define MPC107_MCC1_ROMNAL_DATA     0xf      /* ROM nibble access time */
#define MPC107_MCC1_ROMFAL_DATA     0xf8     /* ROM first access time  */
#define MPC107_MCC1_BURST_DATA      0x0      /* burst mode type */
#define MPC107_MCC1_MEMGO_DATA      0x0      /* enable RAM interface logic */
#define MPC107_MCC1_SREN_DATA       0x0      /* enable self refresh  */
#define MPC107_MCC1_RAM_TYPE_DATA   0x0      /* RAM type: 0=SDRAM,1=FPM/EDO */
#define MPC107_MCC1_PCKEN_DATA      0x0      /* enable parity checking  */
#define MPC107_MCC1_BANK0_ROWS      0x0      /* Number of rows for bank 0 */
#define MPC107_MCC1_BANK1_ROWS      0x0      /* Number of rows for bank 1 */
#define MPC107_MCC1_BANK2_ROWS      0x0      /* Number of rows for bank 2 */
#define MPC107_MCC1_BANK3_ROWS      0x0      /* Number of rows for bank 3 */
#define MPC107_MCC1_BANK4_ROWS      0x0      /* Number of rows for bank 4 */
#define MPC107_MCC1_BANK5_ROWS      0x0      /* Number of rows for bank 5 */
#define MPC107_MCC1_BANK6_ROWS      0x0      /* Number of rows for bank 6 */
#define MPC107_MCC1_BANK7_ROWS      0x0      /* Number of rows for bank 7 */


#define MPC107_MCC1_BANK7_ROWS_DATA   (MPC107_MCC1_BANK7_ROWS << 14)
#define MPC107_MCC1_BANK6_ROWS_DATA   (MPC107_MCC1_BANK6_ROWS << 12)
#define MPC107_MCC1_BANK5_ROWS_DATA   (MPC107_MCC1_BANK5_ROWS << 10)
#define MPC107_MCC1_BANK4_ROWS_DATA   (MPC107_MCC1_BANK4_ROWS << 8)
#define MPC107_MCC1_BANK3_ROWS_DATA   (MPC107_MCC1_BANK3_ROWS << 6)
#define MPC107_MCC1_BANK2_ROWS_DATA   (MPC107_MCC1_BANK2_ROWS << 4)
#define MPC107_MCC1_BANK1_ROWS_DATA   (MPC107_MCC1_BANK1_ROWS << 2)
#define MPC107_MCC1_BANK0_ROWS_DATA   (MPC107_MCC1_BANK0_ROWS << 0)




/* Memory control configuration  Register #2*/

#define MPC107_MCC2_TS_WAIT_TIMER_M 0xe0000000  /* ROM out disable timing  */
#define MPC107_MCC2_TS_WAIT_TIMER_S 29          /* mask and shift */
#define MPC107_MCC2_ASRISE_MASK     0x1e000000  /* AS_ falling edge timing */
#define MPC107_MCC2_ASRISE_SHIFT    25          /* mask and shift */
#define MPC107_MCC2_ASFALL_MASK     0x01e00000  /* AS_ rising edge timing */
#define MPC107_MCC2_ASFALL_SHIFT    21          /* mask and shift */
#define MPC107_MCC2_PAR_OR_ECC_M    0x00100000  /* ECC/parity mechanism mask */
#define MPC107_MCC2_PAR_OR_ECC_S    20          /* ECC/parity mechanism shift */
#define MPC107_MCC2_WR_PAR_CHK_EN   0x00080000  /* write parity check enable */
#define MPC107_MCC2_WR_PAR_CHK_M    0x00080000  /* write parity check mask */
#define MPC107_MCC2_WR_PAR_CHK_S    19          /* write parity check shift */
#define MPC107_MCC2_RD_PARECC_EN    0x00040000  /* inline mem bus read parity */
#define MPC107_MCC2_RD_PARECC_M     0x00040000  /* inline mem bus read mask */
#define MPC107_MCC2_RD_PARECC_S     18          /* inline mem bus read shift */
#define MPC107_MCC2_ECCEN_MASK      0x00020000  /* ECC enable */
#define MPC107_MCC2_ECCEN_SHIFT     17          /* ECC enable shift */
#define MPC107_MCC2_EDO             0x00010000  /* EDO DRAM */
#define MPC107_MCC2_REFINT_MASK     0x0000fffc  /* refresh interval mask */
#define MPC107_MCC2_REFINT_SHIFT    2           /* refresh interval shift */
#define MPC107_MCC2_RSV_PG_MASK     0x00000002  /* reserve 1 page mask */
#define MPC107_MCC2_RSV_PG_SHIFT    1           /* reserve 1 page shift */
#define MPC107_MCC2_RMW_PAR         0x00000001  /* parity gather/store */

#define MPC107_MCC2_REFINT_DATA        0x000006b8  /* refresh interval data */
#define MPC107_MCC2_TS_WAIT_TIMER_DATA 0x0       /* ROM out disable timing */
#define MPC107_MCC2_ASRISE_DATA        0x0       /* AS_ falling edge timing */
#define MPC107_MCC2_ASFALL_DATA        0x0       /* AS_ rising edge timing */
#define MPC107_MCC2_EDO_DATA           0x0       /* EDO DRAM */
#define MPC107_MCC2_ECCEN_DATA         0x0       /* ECC enable */
#define MPC107_MCC2_PAR_OR_ECC_D       0x0       /* ECC/parity mechanism data */
#define MPC107_MCC2_WR_PAR_CHK_D       0x0       /* write parity check enable */
#define MPC107_MCC2_RD_PARECC_D        0x0       /* inline mem bus read data */
#define MPC107_MCC2_RSV_PG_DATA        0x0       /* reserve 1 page data */
#define MPC107_MCC2_RMW_PAR_DATA       0x0       /* parity gather/store */



/* Memory control configuration  Register #3 */

#define MPC107_MCC3_BSTOPRE_25_M    0xf0000000  /* burst to precharge timing */
#define MPC107_MCC3_BSTOPRE_25_S    28          /* mask and shift */
#define MPC107_MCC3_REFREC_MASK     0x0f000000  /* refresh to active interval */
#define MPC107_MCC3_REFREC_SHIFT    24          /* timing mask and shift */
#define MPC107_MCC3_RDLAT_MASK      0x00f00000  /* read data latency timing */
#define MPC107_MCC3_RDLAT_SHIFT     20          /* mask and shift */
#define MPC107_MCC3_CPX_MASK        0x00080000  /* CAS write timing */
#define MPC107_MCC3_CPX_SHIFT       19          /* CAS write timing shift */
#define MPC107_MCC3_CPX             0x00080000  /* CAS write timing */
#define MPC107_MCC3_RAS6P_MASK      0x00078000  /* RAS low time for CBR */
#define MPC107_MCC3_RAS6P_SHIFT     15          /* refresh shift and mask */
#define MPC107_MCC3_CAS5_MASK       0x00007000  /* CAS low time for page mode */
#define MPC107_MCC3_CAS5_SHIFT      12          /* access mask and shift */
#define MPC107_MCC3_CP4_MASK        0x00000e00  /* CAS precharge mask */
#define MPC107_MCC3_CP4_SHIFT       9           /* CAS precharge shift */
#define MPC107_MCC3_CAS3_MASK       0x000001c0  /* CAS low time for first */
#define MPC107_MCC3_CAS3_SHIFT      6           /* data access mask and shift */
#define MPC107_MCC3_RCD2_MASK       0x00000038  /* RAS to CAS delay mask */
#define MPC107_MCC3_RCD2_SHIFT      3           /* RAS to CAS delay shift */
#define MPC107_MCC3_RP1_MASK        0x00000007  /* RAS precharge mask */
#define MPC107_MCC3_RP1_SHIFT       2           /* RAS precharge shift */

#define MPC107_MCC3_REFREC_DATA     0x8         /* refresh to active interval */
#define MPC107_MCC3_RDLAT_DATA      0x4         /* read data latency timing */
#define MPC107_MCC3_CPX_DATA        0           /* CAS write timing */
#define MPC107_MCC3_RAS6P_DATA      0x0         /* RAS low time for CBR */
#define MPC107_MCC3_CAS5_DATA       0x0         /* CAS low time for page mode */
#define MPC107_MCC3_CP4_DATA        0x2         /* CAS precharge data */
#define MPC107_MCC3_CAS3_DATA       0x4         /* CAS low time for first */
#define MPC107_MCC3_RCD2_DATA       0x0         /* RAS to CAS delay */
#define MPC107_MCC3_RP1_DATA        0x2         /* RAS precharge data */

/* Burst to precharge for MCCR3 */

#define MPC107_MCC3_BSTOPRE_DATA    ((MPC107_BSTOPRE_DATA & 0xf0) >> 4)

/* Memory control configuration  Register #4 */

#define MPC107_MCC4_PRETOACT_MASK   0xf0000000 /* precharge to active */
#define MPC107_MCC4_PRETOACT_SHIFT          28 /* interval mask and shift */
#define MPC107_MCC4_ACTOPRE_MASK    0x0f000000 /* active to precharge */
#define MPC107_MCC4_ACTOPRE_SHIFT           24 /* interval mask and shift */
#define MPC107_MCC4_WMODE_SHIFT             23 /* length of data */
#define MPC107_MCC4_INLINE          0x00400000 /* inline ECC/parity chk enabl */
#define MPC107_MCC4_REGISTERED      0x00100000 /* memory data interface */
#define MPC107_MCC4_BSTOPRE_01_MASK 0x000c0000 /* burst to precharge timing */
#define MPC107_MCC4_BSTOPRE_01_SHIFT	    18 /* mask and shift */
#define MPC107_MCC4_REGDIMM	    0x00008000 /* config mem bus for DIMMs */
#define MPC107_MCC4_SDMODE_CAS_SHF          12 /* CAS latency type shift */
#define MPC107_MCC4_SDMODE_MASK     0x00007f00 /* SDRAM mode mask */
#define MPC107_MCC4_SDMODE_SHIFT             8 /* SDRAM mode shift */
#define MPC107_MCC4_ACTORW_MASK     0x000000f0 /* activate to read/write */
#define MPC107_MCC4_ACTORW_SHIFT             4 /* interval mask and shift */
#define MPC107_MCC4_BSTOPRE_69_MASK 0x0000000f /* burst to prechrg timing msk */

#define MPC107_MCC4_PRETOACT_DATA     0x03  /* precharge to active data */
#define MPC107_MCC4_ACTOPRE_DATA      0x05  /* active to precharge data */
#define MPC107_MCC4_WMODE_DATA        0x01  /* Length of burst for 32bit data */
#define MPC107_MCC4_BUFTYPE_DATA      MPC107_MCC4_INLINE /* buffer type */
#define MPC107_BSTOPRE_DATA           0x07A   /* burst to precharge timing */
#define MPC107_MCC4_SDMODE_DATA       0x0     /* CAS latency type  */
#define MPC107_MCC4_ACTORW_DATA       0x3     /* activate to read/write */

/* Bitmasks to enable memory banks 0 - 7, used in config.h */

#define MPC107_BANK0_ENABLE	  0x1 	        /* 1/0 Enable/Disable BANK 0 */
#define MPC107_BANK1_ENABLE	  0x0	        /* 1/0 Enable/Disable BANK 1 */
#define MPC107_BANK2_ENABLE	  0x0           /* 1/0 Enable/Disable BANK 2 */
#define MPC107_BANK3_ENABLE       0x0           /* 1/0 Enable/Disable BANK 3 */
#define MPC107_BANK4_ENABLE       0x0           /* 1/0 Enable/Disable BANK 4 */
#define MPC107_BANK5_ENABLE       0x0           /* 1/0 Enable/Disable BANK 5 */
#define MPC107_BANK6_ENABLE       0x0           /* 1/0 Enable/Disable BANK 6 */
#define MPC107_BANK7_ENABLE       0x0           /* 1/0 Enable/Disable BANK 7 */

#define MPC107_BANK0_ENABLE_DATA  (MPC107_BANK0_ENABLE << 0x0 ) /* BANK 0 */
#define MPC107_BANK1_ENABLE_DATA  (MPC107_BANK1_ENABLE << 0x1 ) /* BANK 1 */
#define MPC107_BANK2_ENABLE_DATA  (MPC107_BANK2_ENABLE << 0x2 ) /* BANK 2 */
#define MPC107_BANK3_ENABLE_DATA  (MPC107_BANK3_ENABLE << 0x3 ) /* BANK 3 */
#define MPC107_BANK4_ENABLE_DATA  (MPC107_BANK4_ENABLE << 0x4 ) /* BANK 4 */
#define MPC107_BANK5_ENABLE_DATA  (MPC107_BANK5_ENABLE << 0x5 ) /* BANK 5 */
#define MPC107_BANK6_ENABLE_DATA  (MPC107_BANK6_ENABLE << 0x6 ) /* BANK 6 */
#define MPC107_BANK7_ENABLE_DATA  (MPC107_BANK7_ENABLE << 0x7 ) /* BANK 7 */


/* memory bank sizes */

#define MPC107_BANK0_SIZE	0x01000000	/* 16MB - 1/2 32MB DIMM */
#define MPC107_BANK1_SIZE	0x01000000	/* 16MB - 1/2 32MB DIMM */
#define MPC107_BANK2_SIZE	0x01000000	/* 16MB - 1/2 32MB DIMM */
#define MPC107_BANK3_SIZE	0x01000000	/* 16MB - 1/2 32MB DIMM */
#define MPC107_BANK4_SIZE	0x01000000	/* 16MB - 1/2 32MB DIMM */
#define MPC107_BANK5_SIZE	0x01000000	/* 16MB - 1/2 32MB DIMM */
#define MPC107_BANK6_SIZE	0x01000000	/* 16MB - 1/2 32MB DIMM */
#define MPC107_BANK7_SIZE	0x01000000	/* 16MB - 1/2 32MB DIMM */

/*
 * The macros MPC107_BANK[x]_ADRS indicate the starting addresses of
 * each memory bank.   The default definitions provide for a contiguous
 * and non-overlapping memory map for the eight banks regardless of the
 * defined size of each bank.
 *
 * The only configurable options are the BANK enables and the BANK
 * sizes which are fully configurable by the user.
 */

/* carve up a valid mapping based on  sizes */

#define MPC107_BANK0_ADRS	0x00000000	/* Mem starts at 0x0 */
#define MPC107_BANK1_ADRS	(MPC107_BANK0_ADRS + MPC107_BANK0_SIZE)
#define MPC107_BANK2_ADRS	(MPC107_BANK1_ADRS + MPC107_BANK1_SIZE)
#define MPC107_BANK3_ADRS	(MPC107_BANK2_ADRS + MPC107_BANK2_SIZE)
#define MPC107_BANK4_ADRS	(MPC107_BANK3_ADRS + MPC107_BANK3_SIZE)
#define MPC107_BANK5_ADRS	(MPC107_BANK4_ADRS + MPC107_BANK4_SIZE)
#define MPC107_BANK6_ADRS	(MPC107_BANK5_ADRS + MPC107_BANK5_SIZE)
#define MPC107_BANK7_ADRS	(MPC107_BANK6_ADRS + MPC107_BANK6_SIZE)
#define MPC107_BANK7_ADRS_END	((MPC107_BANK7_ADRS + MPC107_BANK7_SIZE) - 1)

/* setup which memory banks to enable */

#define MPC107_MBER_DEFAULT	MPC107_BANK0_ENABLE_DATA | \
                                MPC107_BANK1_ENABLE_DATA | \
                                MPC107_BANK2_ENABLE_DATA | \
                                MPC107_BANK3_ENABLE_DATA | \
                                MPC107_BANK4_ENABLE_DATA | \
                                MPC107_BANK5_ENABLE_DATA | \
                                MPC107_BANK6_ENABLE_DATA | \
                                MPC107_BANK7_ENABLE_DATA

/* break down the mapping addresses into MPC107 register settings */

#define MPC107_START_BANK0	(((MPC107_BANK0_ADRS)     >> 20) & (0x3ff))
#define MPC107_ENDOF_BANK0	(((MPC107_BANK1_ADRS - 1) >> 20) & (0x3ff))
#define MPC107_START_BANK1	(((MPC107_BANK1_ADRS)     >> 20) & (0x3ff))
#define MPC107_ENDOF_BANK1	(((MPC107_BANK2_ADRS - 1) >> 20) & (0x3ff))
#define MPC107_START_BANK2	(((MPC107_BANK2_ADRS)     >> 20) & (0x3ff))
#define MPC107_ENDOF_BANK2	(((MPC107_BANK3_ADRS - 1) >> 20) & (0x3ff))
#define MPC107_START_BANK3	(((MPC107_BANK3_ADRS)     >> 20) & (0x3ff))
#define MPC107_ENDOF_BANK3	(((MPC107_BANK4_ADRS - 1) >> 20) & (0x3ff))
#define MPC107_START_BANK4	(((MPC107_BANK4_ADRS)     >> 20) & (0x3ff))
#define MPC107_ENDOF_BANK4	(((MPC107_BANK5_ADRS - 1) >> 20) & (0x3ff))
#define MPC107_START_BANK5	(((MPC107_BANK5_ADRS)     >> 20) & (0x3ff))
#define MPC107_ENDOF_BANK5	(((MPC107_BANK6_ADRS - 1) >> 20) & (0x3ff))
#define MPC107_START_BANK6	(((MPC107_BANK6_ADRS)     >> 20) & (0x3ff))
#define MPC107_ENDOF_BANK6	(((MPC107_BANK7_ADRS - 1) >> 20) & (0x3ff))
#define MPC107_START_BANK7	(((MPC107_BANK7_ADRS)     >> 20) & (0x3ff))
#define MPC107_ENDOF_BANK7	(((MPC107_BANK7_ADRS_END) >> 20) & (0x3ff))

#define STMEM_MASK 		(0x0ff)	/* Start of Memory Mask */
#define XTMEM_MASK 		(0x003) /* Extended Memory Mask */

/* Setup the actual MPC107 register values.*/

/* memory starting address register one */

#define MPC107_MSAR1_DEFAULT	(((MPC107_START_BANK0 & STMEM_MASK) << 0)  | \
			 ((MPC107_START_BANK1 & STMEM_MASK) << 8)  | \
			 ((MPC107_START_BANK2 & STMEM_MASK) << 16) | \
			 ((MPC107_START_BANK3 & STMEM_MASK) << 24))

/* memory starting extended address register one */

#define MPC107_XMSAR1_DEFAULT ((((MPC107_START_BANK0 >> 8) & XTMEM_MASK) << 0) \
			 | (((MPC107_START_BANK1 >> 8) & XTMEM_MASK) << 8)   \
			 | (((MPC107_START_BANK2 >> 8) & XTMEM_MASK) << 16)  \
			 | (((MPC107_START_BANK3 >> 8) & XTMEM_MASK) << 24))

/* memory ending address register one */

#define MPC107_MEAR1_DEFAULT	(((MPC107_ENDOF_BANK0 & STMEM_MASK) << 0)  | \
			 ((MPC107_ENDOF_BANK1 & STMEM_MASK) << 8)  | \
			 ((MPC107_ENDOF_BANK2 & STMEM_MASK) << 16) | \
			 ((MPC107_ENDOF_BANK3 & STMEM_MASK) << 24))

/* memory ending extended address register one */

#define MPC107_XMEAR1_DEFAULT ((((MPC107_ENDOF_BANK0 >> 8) & XTMEM_MASK) << 0) \
			 | (((MPC107_ENDOF_BANK1 >> 8) & XTMEM_MASK) << 8)   \
			 | (((MPC107_ENDOF_BANK2 >> 8) & XTMEM_MASK) << 16)  \
			 | (((MPC107_ENDOF_BANK3 >> 8) & XTMEM_MASK) << 24))

/* memory starting address register two */

#define MPC107_MSAR2_DEFAULT 	(((MPC107_START_BANK4 & STMEM_MASK) << 0)  | \
                         ((MPC107_START_BANK5 & STMEM_MASK) << 8)  | \
			 ((MPC107_START_BANK6 & STMEM_MASK) << 16) | \
			 ((MPC107_START_BANK7 & STMEM_MASK) << 24))

/* memory starting extended address register two */

#define MPC107_XMSAR2_DEFAULT ((((MPC107_START_BANK4 >> 8) & XTMEM_MASK) << 0) \
			 | (((MPC107_START_BANK5 >> 8) & XTMEM_MASK) << 8)   \
			 | (((MPC107_START_BANK6 >> 8) & XTMEM_MASK) << 16)  \
			 | (((MPC107_START_BANK7 >> 8) & XTMEM_MASK) << 24))

/* memory ending address register two */

#define MPC107_MEAR2_DEFAULT	(((MPC107_ENDOF_BANK4 & STMEM_MASK) <<  0) | \
			 ((MPC107_ENDOF_BANK5 & STMEM_MASK) <<  8) | \
			 ((MPC107_ENDOF_BANK6 & STMEM_MASK) << 16) | \
			 ((MPC107_ENDOF_BANK7 & STMEM_MASK) << 24))

/* memory ending extended address register two */

#define MPC107_XMEAR2_DEFAULT ((((MPC107_ENDOF_BANK4 >> 8) & XTMEM_MASK) << 0) \
			 | (((MPC107_ENDOF_BANK5 >> 8) & XTMEM_MASK) <<  8)  \
			 | (((MPC107_ENDOF_BANK6 >> 8) & XTMEM_MASK) << 16)  \
			 | (((MPC107_ENDOF_BANK7 >> 8) & XTMEM_MASK) << 24))

/*
 * initial values for the MPC107 SDRAM memory control registers.
 * Refer to MPC107 users manual and addendums for details on values.
 * These defaults are to support SDRAM
 */

/*  The Memory control Configuration  Register - 1  (MCCR1) */

#define MPC107_MCCR1_DEFAULT	 (MPC107_MCC1_ROMNAL_DATA << \
                                  MPC107_MCC1_ROMNAL_SHIFT) | \
                                 (MPC107_MCC1_ROMFAL_DATA << \
                                  MPC107_MCC1_ROMFAL_SHIFT) | \
                                  MPC107_MCC1_BANK7_ROWS_DATA | \
                                  MPC107_MCC1_BANK6_ROWS_DATA | \
                                  MPC107_MCC1_BANK5_ROWS_DATA | \
                                  MPC107_MCC1_BANK4_ROWS_DATA | \
                                  MPC107_MCC1_BANK3_ROWS_DATA | \
                                  MPC107_MCC1_BANK2_ROWS_DATA | \
                                  MPC107_MCC1_BANK1_ROWS_DATA | \
                                  MPC107_MCC1_BANK0_ROWS_DATA |\
                                ((MPC107_MCC1_BURST_DATA << \
                                  MPC107_MCC1_BURST_SHIFT) & \
                                  MPC107_MCC1_BURST_MASK)



/*  The Memory control Configuration  Register - 2  (MCCR2) */

#define MPC107_MCCR2_DEFAULT	  ((MPC107_MCC2_REFINT_DATA << \
                                    MPC107_MCC2_REFINT_SHIFT) & \
                                    MPC107_MCC2_REFINT_MASK) | \
                                  ((MPC107_MCC2_TS_WAIT_TIMER_DATA << \
                                    MPC107_MCC2_TS_WAIT_TIMER_S) & \
                                    MPC107_MCC2_TS_WAIT_TIMER_M) | \
                                  ((MPC107_MCC2_ASRISE_DATA << \
                                    MPC107_MCC2_ASRISE_SHIFT) &\
                                    MPC107_MCC2_ASRISE_MASK) | \
                                  ((MPC107_MCC2_ASFALL_DATA << \
                                    MPC107_MCC2_ASFALL_SHIFT) & \
                                    MPC107_MCC2_ASFALL_MASK) | \
                                    MPC107_MCC2_EDO_DATA | \
                                  ((MPC107_MCC2_ECCEN_DATA << \
                                    MPC107_MCC2_ECCEN_SHIFT) & \
                                    MPC107_MCC2_ECCEN_MASK) | \
                                  ((MPC107_MCC2_PAR_OR_ECC_D <<\
                                    MPC107_MCC2_PAR_OR_ECC_S) &\
                                    MPC107_MCC2_PAR_OR_ECC_M) | \
                                  ((MPC107_MCC2_WR_PAR_CHK_D << \
                                    MPC107_MCC2_WR_PAR_CHK_S) & \
                                    MPC107_MCC2_WR_PAR_CHK_M) |\
                                  ((MPC107_MCC2_RD_PARECC_D << \
                                    MPC107_MCC2_RD_PARECC_S) & \
                                    MPC107_MCC2_RD_PARECC_M) | \
                                  ((MPC107_MCC2_RSV_PG_DATA << \
                                    MPC107_MCC2_RSV_PG_SHIFT) & \
                                    MPC107_MCC2_RSV_PG_MASK)| \
                                    MPC107_MCC2_RMW_PAR_DATA


/* The Memory control Configuration  Register - 3  (MCCR3) */

#define MPC107_MCCR3_DEFAULT      ((MPC107_MCC3_BSTOPRE_DATA  << \
                                    MPC107_MCC3_BSTOPRE_25_S ) & \
                                    MPC107_MCC3_BSTOPRE_25_M)| \
                                  ((MPC107_MCC3_REFREC_DATA << \
                                    MPC107_MCC3_REFREC_SHIFT) & \
                                    MPC107_MCC3_REFREC_MASK) | \
                                  ((MPC107_MCC3_CPX_DATA << \
                                    MPC107_MCC3_CPX_SHIFT) & \
                                    MPC107_MCC3_CPX_MASK ) | \
                                  ((MPC107_MCC3_RAS6P_DATA << \
                                    MPC107_MCC3_RAS6P_SHIFT) & \
                                    MPC107_MCC3_RAS6P_MASK ) | \
                                  ((MPC107_MCC3_CAS5_DATA << \
                                    MPC107_MCC3_CAS5_SHIFT ) & \
                                    MPC107_MCC3_CAS5_MASK) | \
                                  ((MPC107_MCC3_CP4_DATA << \
                                    MPC107_MCC3_CP4_SHIFT) & \
                                    MPC107_MCC3_CP4_MASK) | \
                                  ((MPC107_MCC3_CAS3_DATA << \
                                    MPC107_MCC3_CAS3_SHIFT) & \
                                    MPC107_MCC3_CAS3_MASK ) | \
                                  ((MPC107_MCC3_RCD2_DATA << \
                                    MPC107_MCC3_RCD2_SHIFT ) & \
                                    MPC107_MCC3_RCD2_MASK) | \
                                  ((MPC107_MCC3_RP1_DATA << \
                                    MPC107_MCC3_RP1_SHIFT) & \
                                    MPC107_MCC3_RP1_MASK)




/*  Memory control Configuration  Register -4 (MCCR4) */

#define MPC107_MCCR4_DEFAULT       (MPC107_MCC4_PRETOACT_DATA << \
                                    MPC107_MCC4_PRETOACT_SHIFT) | \
                                   (MPC107_MCC4_ACTOPRE_DATA <<\
                                    MPC107_MCC4_ACTOPRE_SHIFT) | \
                                   (MPC107_MCC4_ACTOPRE_DATA << \
                                    MPC107_MCC4_ACTOPRE_SHIFT) | \
                                   (MPC107_MCC4_WMODE_DATA << \
                                    MPC107_MCC4_WMODE_SHIFT) | \
                                   (MPC107_MCC4_WMODE_DATA << \
                                    MPC107_MCC4_WMODE_SHIFT)| \
                                   (MPC107_MCC4_WMODE_DATA << \
                                    MPC107_MCC4_WMODE_SHIFT) | \
                                    MPC107_MCC4_BUFTYPE_DATA | \
                                  ((MPC107_BSTOPRE_DATA & \
                                    MPC107_MCC4_BSTOPRE_01_MASK) << \
                                    MPC107_MCC4_BSTOPRE_01_SHIFT)  | \
                                   (MPC107_BSTOPRE_DATA & \
                                    MPC107_MCC4_BSTOPRE_69_MASK) | \
                                  ((MPC107_MCC4_SDMODE_DATA << \
                                    MPC107_MCC4_SDMODE_SHIFT) & \
                                    MPC107_MCC4_SDMODE_MASK) | \
                                  ((MPC107_MCC4_ACTORW_DATA << \
                                    MPC107_MCC4_ACTORW_MASK) & \
                                    MPC107_MCC4_ACTORW_MASK)

/* SDRAM Page mode register initial value. */

#define MPC107_MPMR_DEFAULT	0x0032	/* SDRAM, 33Mhz w/ROMFAL =8 */


#ifdef __cplusplus
}
#endif

#endif	/* __INCmpc107MemInith */

