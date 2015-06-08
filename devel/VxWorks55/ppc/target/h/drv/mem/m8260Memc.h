/* m8260Memc.h - Motorola MPC8260 Memory Controller header file */

/* Copyright 1984-2001 Wind River Systems, Inc. */

/*
modification history
--------------------
01b,29may01,rcs  removed nonexistant MCR, set MAR and MDR to correct offsets,
                 changed MSTAT to MCMR (SPR#63210)
01a,12sep99,ms_  created from m8260Cpm.h, 01d.
*/

/*
 * This file contains constants for the Memory Controller in the 
 * Motorola MPC8260 PowerQUICC II integrated Communications Processor
 */

#ifndef __INCm8260Memch
#define __INCm8260Memch

#ifdef __cplusplus
extern "C" {
#endif
    
#ifndef M8260ABBREVIATIONS
#define M8260ABBREVIATIONS

#ifdef  _ASMLANGUAGE
#define CAST(x)
#else /* _ASMLANGUAGE */
typedef volatile UCHAR VCHAR;   /* shorthand for volatile UCHAR */
typedef volatile INT32 VINT32; /* volatile unsigned word */
typedef volatile INT16 VINT16; /* volatile unsigned halfword */
typedef volatile INT8 VINT8;   /* volatile unsigned byte */
typedef volatile UINT32 VUINT32; /* volatile unsigned word */
typedef volatile UINT16 VUINT16; /* volatile unsigned halfword */
typedef volatile UINT8 VUINT8;   /* volatile unsigned byte */
#define CAST(x) (x)
#endif  /* _ASMLANGUAGE */

#endif /* M8260ABBREVIATIONS */

/*
 * MPC8260 internal register/memory map (section 17 of prelim. spec)
 * note that these are offsets from the value stored in the IMMR
 * register. Also note that in the MPC8260, the IMMR is not a special
 * purpose register, but it is memory mapped.
 */
 
/* 
 * Memory Controller registers 
 */
 
/* Base Reg bank 0*/

#define M8260_BR0(base)         (CAST(VUINT32 *)((base) + 0x10100))

/* Opt Reg bank 0*/

#define M8260_OR0(base)         (CAST(VUINT32 *)((base) + 0x10104))

/* Base Reg bank 1*/

#define M8260_BR1(base)         (CAST(VUINT32 *)((base) + 0x10108))

/* Opt Reg bank 1*/

#define M8260_OR1(base)         (CAST(VUINT32 *)((base) + 0x1010c))

/* Base Reg bank 2*/

#define M8260_BR2(base)         (CAST(VUINT32 *)((base) + 0x10110))

/* Opt Reg bank 2*/

#define M8260_OR2(base)         (CAST(VUINT32 *)((base) + 0x10114))

/* Base Reg bank 3*/

#define M8260_BR3(base)         (CAST(VUINT32 *)((base) + 0x10118))

/* Opt Reg bank 3*/

#define M8260_OR3(base)         (CAST(VUINT32 *)((base) + 0x1011c))

/* Base Reg bank 4*/

#define M8260_BR4(base)         (CAST(VUINT32 *)((base) + 0x10120))

/* Opt Reg bank 4*/

#define M8260_OR4(base)         (CAST(VUINT32 *)((base) + 0x10124))

/* Base Reg bank 5*/

#define M8260_BR5(base)         (CAST(VUINT32 *)((base) + 0x10128))

/* Opt Reg bank 5*/

#define M8260_OR5(base)         (CAST(VUINT32 *)((base) + 0x1012c))

/* Base Reg bank 6*/

#define M8260_BR6(base)         (CAST(VUINT32 *)((base) + 0x10130))

/* Opt Reg bank 6*/

#define M8260_OR6(base)         (CAST(VUINT32 *)((base) + 0x10134))

/* Base Reg bank 7*/

#define M8260_BR7(base)         (CAST(VUINT32 *)((base) + 0x10138))

/* Opt Reg bank 7*/

#define M8260_OR7(base)         (CAST(VUINT32 *)((base) + 0x1013c))

/* Base Reg bank 8*/

#define M8260_BR8(base)         (CAST(VUINT32 *)((base) + 0x10140))

/* Opt Reg bank 8*/

#define M8260_OR8(base)         (CAST(VUINT32 *)((base) + 0x10144))

/* Base Reg bank 9*/

#define M8260_BR9(base)         (CAST(VUINT32 *)((base) + 0x10148))

/* Opt Reg bank 9*/

#define M8260_OR9(base)         (CAST(VUINT32 *)((base) + 0x1014c))

/* Base Reg bank 10*/

#define M8260_BR10(base)        (CAST(VUINT32 *)((base) + 0x10150))

/* Opt Reg bank 10*/

#define M8260_OR10(base)        (CAST(VUINT32 *)((base) + 0x10154))

/* Base Reg bank 11*/

#define M8260_BR11(base)        (CAST(VUINT32 *)((base) + 0x10158))

/* Opt Reg bank 11*/

#define M8260_OR11(base)        (CAST(VUINT32 *)((base) + 0x1015c))
 
#define MAR(base)       (CAST(VUINT32 *)((base) + 0x0168)) /* Memory Address */
#define MBMR(base)      (CAST(VUINT32 *)((base) + 0x0174)) /* Machine B Mode */
#define MCMR(base)      (CAST(VUINT16 *)((base) + 0x0178)) /* Machine C Mode */
#define MDR(base)       (CAST(VUINT32 *)((base) + 0x0188)) /* Memory Data */

/* Mem Timer Presc*/

#define M8260_MPTPR(base)       (CAST(VUINT16 *)((base) + 0x10184))
 
/* SDRAM refresh timer */

#define M8260_PSRT(base)        (CAST(VUINT16 *)((base) + 0x1019c))
 
/* 60x bus SDRAM mode register */

#define M8260_PSDMR(base)       (CAST(VUINT16 *)((base) + 0x10190))
 
/* Base Register bit definition */
 
#define M8260_BR_BA_MSK         0xffff8000      /* Base Address Mask */
#define M8260_BR_RES_MSK        0x00006000      /* reserved field Mask */
#define M8260_BR_PS_MSK         0x00001800      /* Port Size Mask */
#define M8260_BR_PS_8           0x00000800      /* 8 bit port size */
#define M8260_BR_PS_16          0x00001000      /* 16 bit port size */
#define M8260_BR_PS_32          0x00001800      /* 32 bit port size */
#define M8260_BR_PS_64          0x00000000      /* 64 bit port size */
#define M8260_BR_DECC_MSK       0x00000600      /* data error checking */
#define M8260_BR_DECC_DIS       0x00000000      /* error checking disabled */
#define M8260_BR_DECC_NOR       0x00000200      /* normal parity checking */
#define M8260_BR_DECC_RMW       0x00000400      /* RMW parity checking */
#define M8260_BR_DECC_ECC       0x00000600      /* ECC checking */
#define M8260_BR_WP             0x00000100      /* Write Protect */
#define M8260_BR_MS_MSK         0x000000e0      /* Machine Select Mask */
#define M8260_BR_MS_GPCM_60X    0x00000000      /* G.P.C.M. 60x Bus Machine */
#define M8260_BR_MS_GPCM_LOC    0x00000020      /* G.P.C.M. Local Bus Machine */
#define M8260_BR_MS_SDRAM_60X   0x00000040      /* SDRAM 60x Bus Machine */
#define M8260_BR_MS_SDRAM_LOC   0x00000060      /* SDRAM Local Bus Machine */
#define M8260_BR_MS_UPMA        0x00000080      /* U.P.M.A Machine */
#define M8260_BR_MS_UPMB        0x000000a0      /* U.P.M.B Machine */
#define M8260_BR_MS_UPMC        0x000000c0      /* U.P.M.C Machine */
#define M8260_BR_EMEMC          0x00000010      /* External Memory Controller */
#define M8260_BR_ATOM_MSK       0x0000000c      /* Atomic Operation Mask */
#define M8260_BR_ATOM_NOT       0x00000000      /* No Atomic Operation */
#define M8260_BR_ATOM_RAWA      0x00000004      /* RAWA Atomic Operation */
#define M8260_BR_ATOM_WARA      0x00000008      /* WARA Atomic Operation */
#define M8260_BR_DR             0x00000002      /* Data Pipelining */
#define M8260_BR_V              0x00000001      /* Bank Valid */
 
/* Option Register bit definition */
 
#define M8260_OR_AM_MSK         0xffff8000      /* Address Mask Mask */
#define M8260_OR_BCTLD          0x00001000      /* Data Buffer Control Disable*/
#define M8260_OR_CSNT_EARLY     0x00000800      /* Chip Select Negation Time */
#define M8260_OR_ACS_MSK        0x00000600      /* Addr to Chip Select Setup */
#define M8260_OR_ACS_DIV1       0x00000000      /* CS output at the same time */
#define M8260_OR_ACS_DIV4       0x00000400      /* CS output 1/4 clock later */
#define M8260_OR_ACS_DIV2       0x00000600      /* CS output 1/2 clock later */
#define M8260_OR_RES            0x00000100      /* reserved */
#define M8260_OR_SCY_MSK        0x000000f0      /* Cycle Lenght in Clocks */
#define M8260_OR_SCY_0_CLK      0x00000000      /* 0 clk cycles wait states */
#define M8260_OR_SCY_1_CLK      0x00000010      /* 1 clk cycles wait states */
#define M8260_OR_SCY_2_CLK      0x00000020      /* 2 clk cycles wait states */
#define M8260_OR_SCY_3_CLK      0x00000030      /* 3 clk cycles wait states */
#define M8260_OR_SCY_4_CLK      0x00000040      /* 4 clk cycles wait states */
#define M8260_OR_SCY_5_CLK      0x00000050      /* 5 clk cycles wait states */
#define M8260_OR_SCY_6_CLK      0x00000060      /* 6 clk cycles wait states */
#define M8260_OR_SCY_7_CLK      0x00000070      /* 7 clk cycles wait states */
#define M8260_OR_SCY_8_CLK      0x00000080      /* 8 clk cycles wait states */
#define M8260_OR_SCY_9_CLK      0x00000090      /* 9 clk cycles wait states */
#define M8260_OR_SCY_10_CLK     0x000000a0      /* 10 clk cycles wait states */
#define M8260_OR_SCY_11_CLK     0x000000b0      /* 11 clk cycles wait states */
#define M8260_OR_SCY_12_CLK     0x000000c0      /* 12 clk cycles wait states */
#define M8260_OR_SCY_13_CLK     0x000000d0      /* 13 clk cycles wait states */
#define M8260_OR_SCY_14_CLK     0x000000e0      /* 14 clk cycles wait states */
#define M8260_OR_SCY_15_CLK     0x000000f0      /* 15 clk cycles wait states */
#define M8260_OR_SETA           0x00000008      /* External Transfer Ack */
#define M8260_OR_TRLX           0x00000004      /* Timing Relaxed */
#define M8260_OR_EHTR_MSK       0x00000006      /* extended hold timings */
#define M8260_OR_EHTR_NORM      0x00000000      /* normal timing */
#define M8260_OR_EHTR_1         0x00000002      /* one idle clock cycle */
#define M8260_OR_EHTR_4         0x00000004      /* four idle clock cycles */
#define M8260_OR_EHTR_8         0x00000006      /* eight idle clock cycles */
 
#define M8260_OR_SDRAM_BPD_2    0x00000000      /* 2 banks per device */
#define M8260_OR_SDRAM_BPD_4    0x00002000      /* 4 banks per device */
#define M8260_OR_SDRAM_BPD_8    0x00004000      /* 8 banks per device */
#define M8260_OR_SDRAM_BPD_RES  0x00006000      /* reserved */
#define M8260_OR_SDRAM_ROWST_7  0x00000400      /* row start address is A7 */
#define M8260_OR_SDRAM_ROWST_8  0x00000800      /* row start address is A8 */
#define M8260_OR_SDRAM_ROWST_9  0x00000c00      /* row start address is A9 */
#define M8260_OR_SDRAM_ROWST_10 0x00001000      /* row start address is A10 */
#define M8260_OR_SDRAM_ROWST_11 0x00001400      /* row start address is A11 */
#define M8260_OR_SDRAM_ROWST_12 0x00001800      /* row start address is A12 */
#define M8260_OR_SDRAM_ROWST_13 0x00001c00      /* row start address is A13 */
#define M8260_OR_SDRAM_NUMR_9   0x00000000      /* 9 row address lines */
#define M8260_OR_SDRAM_NUMR_10  0x00000040      /* 10 row address lines */
#define M8260_OR_SDRAM_NUMR_11  0x00000080      /* 11 row address lines */
#define M8260_OR_SDRAM_NUMR_12  0x000000c0      /* 12 row address lines */
#define M8260_OR_SDRAM_NUMR_13  0x00000100      /* 13 row address lines */
#define M8260_OR_SDRAM_NUMR_14  0x00000140      /* 14 row address lines */
#define M8260_OR_SDRAM_NUMR_15  0x00000180      /* 15 row address lines */
#define M8260_OR_SDRAM_PM_NORM  0x00000000      /* back-to-back page mode */
#define M8260_OR_SDRAM_IBID     0x00000010      /* bank interleaving disable */
 
#define M8260_PSDMR_PBI         0x80000000      /* page-based interleaving */
#define M8260_PSDMR_RFEN        0x40000000      /* page-based interleaving */
#define M8260_PSDMR_OP_NORM     0x00000000      /* normal operation */
#define M8260_PSDMR_OP_CBR      0x08000000      /* CBR refresh */
#define M8260_PSDMR_OP_SELFR    0x10000000      /* self refresh */
#define M8260_PSDMR_OP_MODE     0x18000000      /* mode register write */
#define M8260_PSDMR_OP_PRE_BANK 0x20000000      /* precharge bank */
#define M8260_PSDMR_OP_PRE_ALL  0x28000000      /* precharge all banks */
#define M8260_PSDMR_OP_ACT      0x30000000      /* activate banks */
#define M8260_PSDMR_OP_RW       0x38000000      /* read/write */
 
#ifdef __cplusplus
}
#endif

#endif /* __INCm8260Memch */
