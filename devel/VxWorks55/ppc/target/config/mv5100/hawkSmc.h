/* hawkSmc.h - Hawk System Memory Controller (SMC) definitions file */

/* Copyright 1999-2001 Wind River Systems, Inc. */
/* Copyright 1998-2001 Motorola, Inc. */

/*
modification history
--------------------
01c,07mar01,rhk  More Wind River coding standards changes.
01b,16aug00,dmw  Wind River coding standards review.
01a,13mar00,dmw  Written (from version 01a of mcpn765/hawkSmc.h).
*/

/*
 * The HAWK consists of a single chip that combines that same functionality
 * of the HAWK chipset and the RAVEN chip.  Accesses to the CSR are
 * internal register set, data for the external register set can be
 * written and read on both the upper and lower halves of the PPC60x data
 * bus.
 *
 * The CSR (control and status register set) consists of the chip's internal
 * register set and its external register set.  The base address of the CSR
 * is hard coded to the address $FEF80000 (or $FEF9000 if the RD[5] pin is
 * low at reset).
 *
 */

#ifndef	INChawkSmch
#define	INChawkSmch

#ifdef __cplusplus
    extern "C" {
#endif /* __cplusplus */

#define HAWK_SDRAM_BANKS	6

/*
 * HAWK registers bit definitions, defined on a register by
 * register basis
 */

/*
 * HAWK_REGISTER_GENERAL_CONTROL_1
 * HAWK_REGISTER_GENERAL_CONTROL_2
 */

#define HAWK_GCR1_TBEN		(1<<0)	/* TIME BASE enable */
#define HAWK_GCR2_IHEN		(1<<0)	/* ISA HOLE enable */

/* HAWK_REGISTER_DRAM_ATTRIBUTES_A,B,C,D,E,F,G,H */

#define SDRAM_EN		(1<<7)	/* DRAM bank enable */
#define SDRAM_ATTR_MASK		(0x8F)  /* DRAM bask attribute mask */
#define SDRAM_SIZE_MASK		(15<<0)	/* DRAM bank size mask */
#define SDRAM_SIZE_0		(0<<0)	/* DRAM bank size 0MB */
#define SDRAM_SIZE_32_4MX16	(1<<0)	/* DRAM bank size 32MB(4Mx16) */
#define SDRAM_SIZE_64_8MX8	(2<<0)	/* DRAM bank size 64MB(8Mx8) */
#define SDRAM_SIZE_64_8MX16	(3<<0)	/* DRAM bank size 64MB(8Mx16) */
#define SDRAM_SIZE_128_16MX4	(4<<0)	/* DRAM bank size 128MB(16Mx4) */
#define SDRAM_SIZE_128_16MX8	(5<<0)	/* DRAM bank size 128MB(16Mx8) */
#define SDRAM_SIZE_128_16MX16	(6<<0)	/* DRAM bank size 128MB(16Mx16) */
#define SDRAM_SIZE_256_32MX4	(7<<0)	/* DRAM bank size 256MB(32Mx4) */
#define SDRAM_SIZE_256_32MX8	(8<<0)	/* DRAM bank size 256MB(32Mx8) */
#define SDRAM_SIZE_512_64MX4	(9<<0)	/* DRAM bank size 512MB(64Mx4) */
#define SDRAM_SIZE_RSRVD0	(10<<0)	/* DRAM bank size reserved */
#define SDRAM_SIZE_RSRVD1	(11<<0)	/* DRAM bank size reserved */
#define SDRAM_SIZE_RSRVD2	(12<<0)	/* DRAM bank size reserved */
#define SDRAM_SIZE_RSRVD3	(13<<0)	/* DRAM bank size reserved */
#define SDRAM_SIZE_RSRVD4	(14<<0)	/* DRAM bank size reserved */
#define SDRAM_SIZE_RSRVD5	(15<<0)	/* DRAM bank size reserved */

/* HAWK_REGISTER_DRAM_BASE_A,B,C,D,E,F,G,H */

#define SDRAM_BASE_ASHIFT	24	/* address shift value */
#define SDRAM_BASE_MASK		0xFF
#define SDRAM_BASE_WRITE(a)	((a)>>SDRAM_BASE_ASHIFT)
#define SDRAM_BASE_READ(a)	((a)<<SDRAM_BASE_ASHIFT)

/* HAWK_REGISTER_CLK_FREQUENCY */

#define HAWK_POR		(1<<0)	/* power-up status indicator */

/* HAWK_REGISTER_ECC_CONTROL */

#define HAWK_ECC_REFDIS	(1<<26) /* refresher clear bit enable */
#define HAWK_ECC_RWCB	(1<<25)	/* read/write check bits enable */
#define HAWK_ECC_DERC	(1<<24)	/* disable error correction */
#define HAWK_ECC_APIEN	(1<<20) /* address parity interrupt enable */
#define HAWK_ECC_SCIEN	(1<<19)	/* single-bit err count rollover int enable */
#define HAWK_ECC_DPIEN	(1<<18)	/* data parity interrupt enable */
#define HAWK_ECC_SIEN	(1<<17)	/* single-bit error interrupt enable */
#define HAWK_ECC_MIEN	(1<<16)	/* non-correctable interrupt enable */
#define HAWK_ECC_MBE_ME	(1<<0)	/* multiple-bit error machine check enable */

/*
 * HAWK_REGISTER_ERROR_LOGGER_1
 * HAWK_REGISTER_ERROR_LOGGER_2
 */

#define HAWK_ERRLOG1_ELOG	(1<<7)	/* single-bit/double-bit error logged */
#define HAWK_ERRLOG1_ESCB	(1<<3)	/* error source: 1 = scrubber, */
                                        /*               0 = 60x bus master */
#define HAWK_ERRLOG1_ESEN	(1<<2)	/* errors reported during scrub */
                                        /* status enable */
#define HAWK_ERRLOG1_EMBT	(1<<1)	/* last error was multiple-bit status */
#define HAWK_ERRLOG1_ESBT	(1<<0)	/* last error was single-bit status */
#define HAWK_ERRLOG2_SCOF	(1<<0)	/* SBE count rollover status */

/* HAWK_REGISTER_SCRUB */

#define HAWK_SCRUB_SWEN		(1<<0)	/* srubber write cycles enable */

/*
 * HAWK_REGISTER_ROM_A_BASE_SIZE
 * HAWK_REGISTER_ROM_B_BASE_SIZE
 */

#define HAWK_ROM_BASE_SHIFT	20	/* base address shift */
#define HAWK_ROM_BASE_MASK	(0xFFF<<HAWK_ROM_BASE_SHIFT) /* ROM base mask */
#define HAWK_ROM_ROM64		(1<<19)	/* ROM is 64-bits wide */
#define HAWK_ROM_SIZE_SHIFT	16	/* ROM size shift */
#define HAWK_ROM_SIZE_MASK	(7<<16)	/* ROM size mask */
#define HAWK_ROM_SIZE_1		(0<<16)	/* ROM size of 1MB */
#define HAWK_ROM_SIZE_2		(1<<16)	/* ROM size of 2MB */
#define HAWK_ROM_SIZE_4		(2<<16)	/* ROM size of 4MB */
#define HAWK_ROM_SIZE_8		(3<<16)	/* ROM size of 8MB */
#define HAWK_ROM_SIZE_16	(4<<16)	/* ROM size of 16MB */
#define HAWK_ROM_SIZE_32	(5<<16)	/* ROM size of 32MB */
#define HAWK_ROM_SIZE_64	(6<<16)	/* ROM size of 64MB */
#define HAWK_ROM_SIZE_RSRVD	(7<<16)	/* ROM size reserved */
#define HAWK_ROM_SIZE_INVALID   -1      /* ROM size invalid */ 
#define HAWK_ROM_RV		(1<<2)	/* ROM is at reset vector location */
#define HAWK_ROM_EN		(1<<1)	/* ROM bank enable */
#define HAWK_ROM_WE		(1<<0)	/* ROM write enable */

/* HAWK_REGISTER_ROM_SPEED_CONTROL */

#define HAWK_ROM_A_SPEED_SHIFT	4      /* ROM speed address shift */
#define HAWK_ROM_B_SPEED_SHIFT	0      /* ROM speed address shift */
#define HAWK_ROM_A_SPEED_MASK	(3<<HAWK_ROM_A_SPEED_SHIFT) /* ROM A spd mask */
#define HAWK_ROM_B_SPEED_MASK	(3<<HAWK_ROM_B_SPEED_SHIFT) /* ROM B spd mask */
#define HAWK_ROM_SPEED_12	(0<<0) /* ROM speed of 12 clock periods */
#define HAWK_ROM_SPEED_8	(1<<0) /* ROM speed of 8 clock periods */
#define HAWK_ROM_SPEED_5	(2<<0) /* ROM speed of 5 clock periods */
#define HAWK_ROM_SPEED_3	(3<<0) /* ROM speed of 3 clock periods */

#define HAWK_ROM_BASE(bankno)	(bankno & (UINT)HAWK_ROM_BASE_MASK) /* base */

#define HAWK_ROM_SIZE(bankno)	\
((1 << ((bankno & HAWK_ROM_SIZE_MASK) >> HAWK_ROM_SIZE_SHIFT)) * 0x100000)

/* HAWK_REGISTER_POWERUP_RESET_STATUS_U (Memory Configuration Register) */

#define HAWK_PWR_STAT_FLSHP_MASK  (0x38<<0) /* flash memory size */
#define HAWK_PWR_STAT_RA_TYP_MASK (0x7<<20) /* flash bank A type */
#define HAWK_PWR_STAT_RB_TYP_MASK (0x7<<16) /* flash bank B type */

#define HAWK_PWR_STAT_INTEL       (0x6)     /* Intel devices installed */

#define HAWK_ROM_TYPE(a,b) (a >> ((b == 1) ? 20 : 16)) /* ROM type */

#define HAWK_FLASH_BANK_A_MASK    (7<<3)  /* bank a flash memory size mask */

/* HAWK_SYNCHRONOUS DRAM CONTROL REGISTER */

#define SDRAM_CL3_SHIFT		28	/* SDRAM Ctrl Reg cl3 bits */
#define SDRAM_TRC_SHIFT		24	/* SDRAM Ctrl Reg tRC bits */ 
#define SDRAM_TRAS_SHIFT	20      /* SDRAM Ctrl Reg tRAS bits */
#define SDRAM_SWR_DPL_SHIFT	17      /* SDRAM Ctrl Reg swr_dpl bits */
#define SDRAM_TDP_SHIFT		16      /* SDRAM Ctrl Reg tDP bits */
#define SDRAM_TRP_SHIFT		12      /* SDRAM Ctrl Reg tRP bits */
#define SDRAM_TRCD_SHIFT	 8      /* SDRAM Ctrl Reg tRCD bits */

#define SDRAM_CL3_MASK		 1      /* SDRAM Ctrl Reg cl3 bit mask */
#define SDRAM_TRC_MASK		 7      /* SDRAM Ctrl Reg tRC bit mask */
#define SDRAM_TRAS_MASK		 3      /* SDRAM Ctrl Reg tRAS bit mask */
#define SDRAM_SWR_DPL_MASK	 1      /* SDRAM Ctrl Reg swr_dpl bit mask */
#define SDRAM_TDP_MASK		 2      /* SDRAM Ctrl Reg tDP bit mask */
#define SDRAM_TRP_MASK		 1      /* SDRAM Ctrl Reg tRP bit mask */
#define SDRAM_TRCD_MASK		 1      /* SDRAM Ctrl Reg tRCD bit mask */

#define SDRAM_CL3_DEFAULT	 1      /* SDRAM Ctrl Reg cl3 default */
#define SDRAM_TRC_DEFAULT	11      /* SDRAM Ctrl Reg tRC default */
#define SDRAM_TRAS_DEFAULT	 7      /* SDRAM Ctrl Reg tRAS default */
#define SDRAM_SWR_DPL_DEFAULT	 1      /* SDRAM Ctrl Reg swr_dpl default */
#define SDRAM_TDP_DEFAULT	 2      /* SDRAM Ctrl Reg tDP default */
#define SDRAM_TRP_DEFAULT	 3      /* SDRAM Ctrl Reg tRP default */
#define SDRAM_TRCD_DEFAULT	 3      /* SDRAM Ctrl Reg tRCD default */
#define SDRAM_TCYC_DEFAULT       0      /* SDRAM Cycle Time default */
#define SDRAM_DIMM_TYPE_DEFAULT  0      /* SDRAM Dimm Config Type default */

/* typedefs */

/* short-cut DRAM control structure template */

typedef struct hawkSmc /* HAWK_SMC */
    {
    UINT sdramAttrA;                    /* SDRAM attributes Blocks A to D */
    UINT sdramBaseA;                    /* SDRAM base address Blocks A to D */
    UCHAR clkFrequency;                 /* Operating clock frequency */
    UINT sdramAttrE;                    /* SDRAM attributes Blocks E to H */
    UINT sdramBaseE;                    /* SDRAM base address Block E to H */
    UINT sdramCtrl;                     /* SDRAM control register */
    }  HAWK_SMC;

#ifdef __cplusplus
    }
#endif /* __cplusplus */

#endif	/* INChawkSmch */

