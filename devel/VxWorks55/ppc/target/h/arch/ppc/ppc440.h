/* ppc440.h - PowerPC 440 specific header */

/* Copyright 1984-2003 Wind River Systems, Inc. */
/*
modification history
--------------------
01g,04mar03,pch  turn off PORTABLE
01f,10jan03,jtp  SPR 82770 virtual i-cache for PPC440 requires unique
                 invalidation
01e,13jun02,jtp  identify class of MMU support (SPR #78396)
01d,24oct01,jtp  add _PPC440_TCR_DIE and ...S
01c,16aug01,pch  Remove definitions which are duplicated in asmPpc.h;
		 add "#define PORTABLE" for initial delivery.
01b,16aug01,pch  Fix typos, remove definition of DEC
01a,15aug01,pch  written, from ppc405.h vn 01f and the Fifth Preliminary
		 Edition (June 2001) of the IBM PPC440 Core User's Manual.
*/

#ifndef __INCppc440h
#define __INCppc440h

#ifdef __cplusplus
extern "C" {
#endif

/* MMU supports software TLB miss handler */
#define _WRS_TLB_MISS_CLASS_SW	1

/* I-cache of 440 core is virtually tagged, requiring extra invalidation */
#define _WRS_I_CACHE_TAG_VIRTUAL

#undef	_CACHE_ALIGN_SIZE
#define _CACHE_ALIGN_SIZE	32	/* all 440 have 32-byte cache lines */

#define	_CACHE_LINE_MASK	0x0000001F

#define _PPC_CACHE_UNIFIED	FALSE	

#define	PPC_NO_REAL_MODE

/* The following defines may be needed by the 4xx cache library. */

#define _PVR_CONF_403GA		0x0000	/* processor conf. bits for 403GA */
#define _PVR_CONF_403GC		0x0002	/* processor conf. bits for 403GC */
#define _PVR_CONF_403GCX	0x0014	/* processor conf. bits for 403GCX */

/* PPC440 SPR numbers */
#define	CCR0	0x3B3	/* Core Configuration Register 0 */
#define	CSRR0	0x03A	/* Critical Save/Restore Register 0 */
#define	CSRR1	0x03B	/* Critical Save/Restore Register 1 */
#define	DAC1	0x13C	/* Data Address Compare 1 */
#define	DAC2	0x13D	/* Data Address Compare 2 */
#define	DBCR0	0x134	/* Debug Control Register 0 */
#define	DBCR1	0x135	/* Debug Control Register 1 */
#define	DBCR2	0x136	/* Debug Control Register 2 */
#define	DBDR	0x3F3	/* Debug Data Register */
#define	DBSR	0x130	/* Debug Status Register */
#define	DCDBTRH	0x39D	/* Data Cache Debug Tag Register High */
#define	DCDBTRL	0x39C	/* Data Cache Debug Tag Register Low */
#define	DEAR	0x03D	/* Data Exception Address Register */
/*
 * Don't define DEC -- asmPpc.h has the correct value, and defining it
 * here breaks C and C++ code (in target/src/zinc/common/z_int.cpp and
 * target/src/os/fioLib.c) that uses DEC with OCT and HEX as members of
 * an enum.
 * #define	DEC	0x016	/@ Decrementer @/
 *
 * The following are also defined correctly in asmPpc.h.  They are
 * omitted here to prevent compiler warnings about redefinition.
 * #define	CTR	0x009	/@ Count Register @/
 * #define	LR	0x008	/@ Link Register @/
 * #define	PVR	0x11F	/@ Processor Version Register @/
 * #define	SPRG0	0x110	/@ Special Purpose Register General 0 @/
 * #define	SPRG1	0x111	/@ Special Purpose Register General 1 @/
 * #define	SPRG2	0x112	/@ Special Purpose Register General 2 @/
 * #define	SPRG3	0x113	/@ Special Purpose Register General 3 @/
 * #define	SRR0	0x01A	/@ Save/Restore Register 0 @/
 * #define	SRR1	0x01B	/@ Save/Restore Register 1 @/
 * #define	XER	0x001	/@ Integer Exception Register @/
 */
#define	DECAR	0x036	/* Decrementer Auto-Reload */
#define	DNV0	0x390	/* Data Cache Normal Victim 0 */
#define	DNV1	0x391	/* Data Cache Normal Victim 1 */
#define	DNV2	0x392	/* Data Cache Normal Victim 2 */
#define	DNV3	0x393	/* Data Cache Normal Victim 3 */
#define	DTV0	0x394	/* Data Cache Transient Victim 0 */
#define	DTV1	0x395	/* Data Cache Transient Victim 1 */
#define	DTV2	0x396	/* Data Cache Transient Victim 2 */
#define	DTV3	0x397	/* Data Cache Transient Victim 3 */
#define	DVC1	0x13E	/* Data Value Compare 1 */
#define	DVC2	0x13F	/* Data Value Compare 2 */
#define	DVLIM	0x398	/* Data Cache Victim Limit */
#define	ESR	0x03E	/* Exception Syndrome Register */
#define	IAC1	0x138	/* Instruction Address Compare 1 */
#define	IAC2	0x139	/* Instruction Address Compare 2 */
#define	IAC3	0x13A	/* Instruction Address Compare 3 */
#define	IAC4	0x13B	/* Instruction Address Compare 4 */
#define	ICDBDR	0x3D3	/* Instruction Cache Debug Data Register */
#define	ICDBTRH	0x39F	/* Instruction Cache Debug Tag Register High */
#define	ICDBTRL	0x39E	/* Instruction Cache Debug Tag Register Low */
#define	INV0	0x370	/* Instruction Cache Normal Victim 0 */
#define	INV1	0x371	/* Instruction Cache Normal Victim 1 */
#define	INV2	0x372	/* Instruction Cache Normal Victim 2 */
#define	INV3	0x373	/* Instruction Cache Normal Victim 3 */
#define	ITV0	0x374	/* Instruction Cache Transient Victim 0 */
#define	ITV1	0x375	/* Instruction Cache Transient Victim 1 */
#define	ITV2	0x376	/* Instruction Cache Transient Victim 2 */
#define	ITV3	0x377	/* Instruction Cache Transient Victim 3 */
#define	IVLIM	0x399	/* Instruction Cache Victim Limit */
			/* Interrupt Vector Offset Registers */
#define	IVOR0	0x190	/* Critical Input */
#define	IVOR1	0x191	/* Machine Check */
#define	IVOR2	0x192	/* Data Storage */
#define	IVOR3	0x193	/* Instruction Storage */
#define	IVOR4	0x194	/* External Input */
#define	IVOR5	0x195	/* Alignment */
#define	IVOR6	0x196	/* Program */
#define	IVOR7	0x197	/* Floating Point Unavailable */
#define	IVOR8	0x198	/* System Call */
#define	IVOR9	0x199	/* Auxiliary Processor Unavailable */
#define	IVOR10	0x19A	/* Decrementer */
#define	IVOR11	0x19B	/* Fixed Interval Timer */
#define	IVOR12	0x19C	/* Watchdog Timer */
#define	IVOR13	0x19D	/* Data TLB Error */
#define	IVOR14	0x19E	/* Instruction TLB Error */
#define	IVOR15	0x19F	/* Debug */
#define	IVPR	0x03F	/* Interrupt Vector Prefix Register */
#define	MMUCR	0x3B2	/* Memory Management Unit Control Register */
#define	PID	0x030	/* Process ID */
#define	PIR	0x11E	/* Processor ID Register */
#define	RSTCFG	0x39B	/* Reset Configuration */
#define	SPRG4_R	0x104	/* Special Purpose Register General 4, read */
#define	SPRG4_W	0x114	/* Special Purpose Register General 4, write */
#define	SPRG5_R	0x105	/* Special Purpose Register General 5, read */
#define	SPRG5_W	0x115	/* Special Purpose Register General 5, write */
#define	SPRG6_R	0x106	/* Special Purpose Register General 6, read */
#define	SPRG6_W	0x116	/* Special Purpose Register General 6, write */
#define	SPRG7_R	0x107	/* Special Purpose Register General 7, read */
#define	SPRG7_W	0x117	/* Special Purpose Register General 7, write */
#define	TBL_R	0x10C	/* Time Base Lower, read */
#define	TBL_W	0x11C	/* Time Base Lower, write */
#define	TBU_R	0x10D	/* Time Base Upper, read */
#define	TBU_W	0x11D	/* Time Base Upper, write */
#define	TCR	0x154	/* Timer Control Register */
#define	TSR	0x150	/* Timer Status Register */
#define	USPRG0	0x100	/* User Special Purpose Register General 0 */

/* INT_MASK definition (mask EE & CE bits) : overwrite the one in asmPpc.h */

#undef	INT_MASK
#define INT_MASK(src, des)	rlwinm  des, src, 0, 17, 15; \
				rlwinm  des, des, 0, 15, 13

/* MSR (upper half) definitions */
/* ppcArch.h defines a generic MSR.  Here we define changes from that base */

#undef 	_PPC_MSR_SF_U		/* 64 bit mode not implemented */
#undef 	_PPC_MSR_POW_U		/* power management not supported */
#undef 	_PPC_MSR_ILE_U		/* little endian mode not supported */

#define	_PPC_MSR_WE_U	0x0004		/* wait state enable */
#define _PPC_MSR_CE_U	0x0002		/* critical interrupt enable */
#define _PPC_MSR_WE	0x00040000	/* wait state enable */
#define _PPC_MSR_CE	0x00020000	/* critical interrupt enable */

/* MSR (lower half) definitions */

#undef	_PPC_MSR_IR		/* insn address translation (always on) */
#undef	_PPC_MSR_DR		/* data address translation (always on) */
#undef 	_PPC_MSR_SE		/* single step unsupported */
#undef 	_PPC_MSR_BE		/* branch trace not supported */
#undef	_PPC_MSR_IP		/* exception prefix bit not supported */
#undef 	_PPC_MSR_RI		/* recoverable interrupt unsupported */
#undef 	_PPC_MSR_LE		/* little endian mode unsupported */
#undef	_PPC_MSR_BIT_LE		/* little endian mode unsupported */

#define _PPC_MSR_DWE	0x0400  /* debug wait enable */
#define _PPC_MSR_DE	0x0200  /* debug exception enable */
#define _PPC_MSR_IS	0x0020  /* insn address space selector */
#define _PPC_MSR_DS	0x0010  /* data address space selector */

/* PPC440GP has no FPU, but some implementations of the PPC440 core might */
#ifndef INCLUDE_HW_FP
#undef 	_PPC_MSR_FP		/* floating point unsupported */
#undef 	_PPC_MSR_FE1		/* floating point not supported */
#undef 	_PPC_MSR_FE0		/* floating point not supported */
#endif /* INCLUDE_HW_FP */

/* The setting in archPpc.h is wrong for the PPC440 */
#undef	_PPC_MSR_POWERUP
#define _PPC_MSR_POWERUP	0x0000	/* state of MSR at powerup */

/* Bits in the upper half of CCR0 */
#define _PPC_CCR0_DSTG_U	0x0020	/* Disable Store Gathering */
#define _PPC_CCR0_DAPUIB_U	0x0010	/* Disable APU Instruction Broadcast */

/* Bits in the lower half of CCR0 */
#define _PPC_CCR0_DTB		0x8000	/* Disable Trace Broadcast */
#define _PPC_CCR0_GICBT		0x4000	/* Guaranteed Insn Cache Block Touch */
#define _PPC_CCR0_GDCBT		0x2000	/* Guaranteed Data Cache Block Touch */
#define _PPC_CCR0_FLSTA		0x0100	/* Force Load/Store Alignment Excptn */
#define _PPC_CCR0_ICSLC		0x000c	/* Insn Cache Speculative Line Count */
#define _PPC_CCR0_ICSLT		0x0003	/* Insn Cache Specul Line Thrshld */

/* Bits in the upper half of TCR */
#define _PPC_TCR_WP_U	0xc000	/* Watchdog Timer Period */
#define _PPC_TCR_WRC_U	0x3000	/* Watchdog Timer Reset Control */
#define _PPC_TCR_WIE_U	0x0800	/* Watchdog Timer Interrupt Enable */
#define _PPC_TCR_DIE_U	0x0400	/* Decrementer Interrupt Enable */
#define _PPC_TCR_FP_U	0x0300	/* Fixed Interval Timer Period */
#define _PPC_TCR_FIE_U	0x0080	/* Fixed Interval Timer Interrupt Enable */
#define _PPC_TCR_ARE_U	0x0040	/* Decrementer Auto-Reload Enable */

/* Bits in the upper half of TSR */
#define _PPC_TSR_ENW_U	0x8000	/* Enable Next Watchdog Timer Exception */
#define _PPC_TSR_WIS_U	0x4000	/* Watchdog Timer Interrupt Status */
#define _PPC_TSR_WRS_U	0x3000	/* Watchdog Timer Reset Status */
#define _PPC_TSR_DIS_U	0x0800	/* Decrementer Interrupt Status */
#define _PPC_TSR_FIS_U	0x0400	/* Fixed Interval Timer Interrupt Status */

/* versions of the aligned for 32-bit TCR/TSR register access */
#define _PPC440_TCR_DIE	(_PPC_TCR_DIE_U << 16)
#define _PPC440_TSR_DIS	(_PPC_TSR_DIS_U << 16)

/* debug control register 0 */
#define _DBCR0_EDM_U	0x8000		/* external debug mode */
#define _DBCR0_IDM_U	0x4000		/* internal debug mode */
#define _DBCR0_RST_U	0x3000		/* reset */
#define _DBCR0_ICMP_U	0x0800		/* instruction completion debug event */
#define _DBCR0_BRT_U	0x0400		/* branch taken debug event */
#define _DBCR0_IRPT_U	0x0200		/* interrupt debug event */
#define _DBCR0_TRAP_U	0x0100		/* trap debug event */
#define _DBCR0_IAC1_U	0x0080		/* instruction address compare 1 */
#define _DBCR0_IAC2_U	0x0040		/* instruction address compare 2 */
#define _DBCR0_IAC3_U	0x0020		/* instruction address compare 3 */
#define _DBCR0_IAC4_U	0x0010		/* instruction address compare 4 */
#define _DBCR0_DAC1R_U	0x0008		/* data address compare 1 Read */
#define _DBCR0_DAC1W_U	0x0004		/* data address compare 1 Write */
#define _DBCR0_DAC2R_U	0x0002		/* data address compare 2 Read */
#define _DBCR0_DAC2W_U	0x0001		/* data address compare 2 Write */
#define _DBCR0_EDM	0x80000000	/* external debug mode */
#define _DBCR0_IDM	0x40000000	/* internal debug mode */
#define _DBCR0_RST	0x30000000	/* reset */
#define _DBCR0_ICMP	0x08000000	/* instruction completion debug event */
#define _DBCR0_BRT	0x04000000	/* branch taken */
#define _DBCR0_IRPT	0x02000000	/* exception debug event */
#define _DBCR0_TRAP	0x01000000	/* trap debug event */
#define _DBCR0_IAC1	0x00800000	/* instruction address compare 1 */
#define _DBCR0_IAC2	0x00400000	/* instruction address compare 2 */
#define _DBCR0_IAC3	0x00200000	/* instruction address compare 3 */
#define _DBCR0_IAC4	0x00100000	/* instruction address compare 4 */
#define _DBCR0_DAC1R	0x00080000	/* data address compare 1 Read */
#define _DBCR0_DAC1W	0x00040000	/* data address compare 1 Write */
#define _DBCR0_DAC2R	0x00020000	/* data address compare 2 Read */
#define _DBCR0_DAC2W	0x00010000	/* data address compare 2 Write */
#define _DBCR0_RET	0x00008000	/* return debug event */
#define _DBCR0_FT	0x00000001	/* freeze timers on debug */

/* debug control register 1 */
#define _DBCR1_IAC1US_U	  0xc000	/* IAC 1 User/Supervisor */
#define _DBCR1_IAC1ER_U	  0x3000	/* IAC 1 Effective/Real */
#define _DBCR1_IAC2US_U	  0x0c00	/* IAC 2 User/Supervisor */
#define _DBCR1_IAC2ER_U	  0x0300	/* IAC 2 Effective/Real */
#define _DBCR1_IAC12M_U	  0x00c0	/* IAC 1/2 Mode */
#define _DBCR1_IAC12AT_U  0x0001	/* IAC 1/2 Auto-Toggle Enable */
#define _DBCR1_IAC1US	  0xc0000000	/* IAC 1 User/Supervisor */
#define _DBCR1_IAC1ER	  0x30000000	/* IAC 1 Effective/Real */
#define _DBCR1_IAC2US	  0x0c000000	/* IAC 2 User/Supervisor */
#define _DBCR1_IAC2ER	  0x03000000	/* IAC 2 Effective/Real */
#define _DBCR1_IAC12M	  0x00c00000	/* IAC 1/2 Mode */
#define _DBCR1_IAC12AT	  0x00010000	/* IAC 1/2 Auto-Toggle Enable */
#define _DBCR1_IAC3US	  0x0000c000	/* IAC 3 User/Supervisor */
#define _DBCR1_IAC3ER	  0x00003000	/* IAC 3 Effective/Real */
#define _DBCR1_IAC4US	  0x00000c00	/* IAC 4 User/Supervisor */
#define _DBCR1_IAC4ER	  0x00000300	/* IAC 4 Effective/Real */
#define _DBCR1_IAC34M	  0x000000c0	/* IAC 3/4 Mode */
#define _DBCR1_IAC34AT	  0x00000001	/* IAC 3/4 Auto-Toggle Enable */

/* debug control register 2 */
#define _DBCR2_DAC1US_U	  0xc000	/* DAC 1 User/Supervisor */
#define _DBCR2_DAC1ER_U	  0x3000	/* DAC 1 Effective/Real */
#define _DBCR2_DAC2US_U	  0x0c00	/* DAC 2 User/Supervisor */
#define _DBCR2_DAC2ER_U	  0x0300	/* DAC 2 Effective/Real */
#define _DBCR2_DAC12M_U	  0x00c0	/* DAC 1/2 Mode */
#define _DBCR2_DAC12A_U	  0x0020	/* DAC 1/2 Asynchronous */
#define _DBCR2_DVC1M_U	  0x000c	/* DVC 1 Mode */
#define _DBCR2_DVC2M_U	  0x0003	/* DVC 2 Mode */
#define _DBCR2_DAC1US	  0xc0000000	/* DAC 1 User/Supervisor */
#define _DBCR2_DAC1ER	  0x30000000	/* DAC 1 Effective/Real */
#define _DBCR2_DAC2US	  0x0c000000	/* DAC 2 User/Supervisor */
#define _DBCR2_DAC2ER	  0x03000000	/* DAC 2 Effective/Real */
#define _DBCR2_DAC12M	  0x00c00000	/* DAC 1/2 Mode */
#define _DBCR2_DAC12A	  0x00200000	/* DAC 1/2 Asynchronous */
#define _DBCR2_DVC1M	  0x000c0000	/* DVC 1 Mode */
#define _DBCR2_DVC2M	  0x00030000	/* DVC 2 Mode */
#define _DBCR2_DVC1BE	  0x0000000c	/* DVC 1 Byte Enables 0:3 */
#define _DBCR2_DVC2BE	  0x00000003	/* DVC 2 Byte Enables 0:3 */

/* debug status register */
#define _DBSR_IDE_U	0x8000		/* Imprecise Debug Event */
#define _DBSR_UDE_U	0x4000		/* Unconditional Debug Event */
#define _DBSR_MRR_U	0x3000		/* Most Recent Reset */
#define _DBSR_ICMP_U	0x0800		/* Instruction Completion Debug Event */
#define _DBSR_BRT_U	0x0400		/* Branch Taken Debug Event */
#define _DBSR_IRPT_U	0x0200		/* Interrupt Debug Event */
#define _DBSR_TRAP_U	0x0100		/* Trap Debug Event */
#define _DBSR_IAC1_U	0x0080		/* IAC 1 Debug Event */
#define _DBSR_IAC2_U	0x0040		/* IAC 2 Debug Event */
#define _DBSR_IAC3_U	0x0020		/* IAC 3 Debug Event */
#define _DBSR_IAC4_U	0x0010		/* IAC 4 Debug Event */
#define _DBSR_DAC1R_U	0x0008		/* DAC/DVC 1 Read Debug Event */
#define _DBSR_DAC1W_U	0x0004		/* DAC/DVC 1 Write Debug Event */
#define _DBSR_DAC2R_U	0x0002		/* DAC/DVC 2 Read Debug Event */
#define _DBSR_DAC2W_U	0x0001		/* DAC/DVC 2 Write Debug Event */
#define _DBSR_IDE	0x80000000	/* Imprecise Debug Event */
#define _DBSR_UDE	0x40000000	/* Unconditional Debug Event */
#define _DBSR_MRR	0x30000000	/* Most Recent Reset */
#define _DBSR_ICMP	0x08000000	/* Instruction Completion Debug Event */
#define _DBSR_BRT	0x04000000	/* Branch Taken Debug Event */
#define _DBSR_IRPT	0x02000000	/* Interrupt Debug Event */
#define _DBSR_TRAP	0x01000000	/* Trap Debug Event */
#define _DBSR_IAC1	0x00800000	/* IAC 1 Debug Event */
#define _DBSR_IAC2	0x00400000	/* IAC 2 Debug Event */
#define _DBSR_IAC3	0x00200000	/* IAC 3 Debug Event */
#define _DBSR_IAC4	0x00100000	/* IAC 4 Debug Event */
#define _DBSR_DAC1R	0x00080000	/* DAC/DVC 1 Read Debug Event */
#define _DBSR_DAC1W	0x00040000	/* DAC/DVC 1 Write Debug Event */
#define _DBSR_DAC2R	0x00020000	/* DAC/DVC 2 Read Debug Event */
#define _DBSR_DAC2W	0x00010000	/* DAC/DVC 2 Write Debug Event */
#define _DBSR_RET	0x00008000	/* Return Debug Event */
#define _DBSR_IAC12ATS	0x00000002	/* IAC 1/2 Auto-Toggle Status */
#define _DBSR_IAC34ATS	0x00000001	/* IAC 3/4 Auto-Toggle Status */

/* mask for hardware breakpoints */
#define _DBSR_HWBP_MSK	( _DBSR_IAC1 | _DBSR_IAC2 | \
			  _DBSR_IAC3 | _DBSR_IAC4 | \
			  _DBSR_DAC1R | _DBSR_DAC1W | \
			  _DBSR_DAC2R | _DBSR_DAC2W )


/* Data Cache Debug Tag Register High */
/*
 * _DCDBTRH_TRA (Tag Real Address) is bits 0:23.  Due to the limitations
 * of PPC immediate operands, a corresponding mask is not all that useful.
 * Use rlwinm instead.
 */
#define	_DCDBTRH_V	0x00000080	/* Cache Line Valid */
#define	_DCDBTRH_TERA	0x0000000f	/* Tag Extended Real Address */

/* Data Cache Debug Tag Register Low */
#define	_DCDBTRL_D	0x000000f0	/* Dirty Indicators */
#define	_DCDBTRL_U0	0x00000008	/* U0 Storage Attribute */
#define	_DCDBTRL_U1	0x00000004	/* U1 Storage Attribute */
#define	_DCDBTRL_U2	0x00000002	/* U2 Storage Attribute */
#define	_DCDBTRL_U3	0x00000001	/* U3 Storage Attribute */

/* Data Cache Normal Victim Registers */
#define	_DNVx_VNDXA_U	0xff00		/* Victim Index A (EA[25:26] = 0b00) */
#define	_DNVx_VNDXB_U	0x00ff		/* Victim Index B (EA[25:26] = 0b01) */
#define	_DNVx_VNDXA	0xff000000	/* Victim Index A (EA[25:26] = 0b00) */
#define	_DNVx_VNDXB	0x00ff0000	/* Victim Index B (EA[25:26] = 0b01) */
#define	_DNVx_VNDXC	0x0000ff00	/* Victim Index C (EA[25:26] = 0b10) */
#define	_DNVx_VNDXD	0x000000ff	/* Victim Index D (EA[25:26] = 0b11) */

/* Data Cache Transient Victim Registers */
#define	_DTVx_VNDXA_U	0xff00		/* Victim Index A (EA[25:26] = 0b00) */
#define	_DTVx_VNDXB_U	0x00ff		/* Victim Index B (EA[25:26] = 0b01) */
#define	_DTVx_VNDXA	0xff000000	/* Victim Index A (EA[25:26] = 0b00) */
#define	_DTVx_VNDXB	0x00ff0000	/* Victim Index B (EA[25:26] = 0b01) */
#define	_DTVx_VNDXC	0x0000ff00	/* Victim Index C (EA[25:26] = 0b10) */
#define	_DTVx_VNDXD	0x000000ff	/* Victim Index D (EA[25:26] = 0b11) */

/* Data Cache Victim Limit Register */
#define	_DVLIM_TFLOOR_U	0x3fc0		/* Transient Floor */
#define	_DVLIM_TFLOOR	0x3fc00000	/* Transient Floor */
#define	_DVLIM_TCEILING	0x0007f800	/* Transient Ceiling */
#define	_DVLIM_NFLOOR	0x000000ff	/* Normal Floor */

/* Instruction Cache Debug Tag Register High */
/*
 * _ICDBTRH_TRA (Tag Real Address) is bits 0:23.  Due to the limitations
 * of PPC immediate operands, a corresponding mask is not all that useful.
 * Use rlwinm instead.
 */
#define	_ICDBTRH_V	0x00000080	/* Cache Line Valid */

/* Instruction Cache Debug Tag Register Low */
#define	_ICDBTRL_TS	0x00000200	/* Translation Space */
#define	_ICDBTRL_TD	0x00000100	/* TID Disable */
#define	_ICDBTRL_TID	0x000000ff	/* Translation ID */

/* Instruction Cache Normal Victim Registers */
#define	_INVx_VNDXA_U	0xff00		/* Victim Index A (EA[25:26] = 0b00) */
#define	_INVx_VNDXB_U	0x00ff		/* Victim Index B (EA[25:26] = 0b01) */
#define	_INVx_VNDXA	0xff000000	/* Victim Index A (EA[25:26] = 0b00) */
#define	_INVx_VNDXB	0x00ff0000	/* Victim Index B (EA[25:26] = 0b01) */
#define	_INVx_VNDXC	0x0000ff00	/* Victim Index C (EA[25:26] = 0b10) */
#define	_INVx_VNDXD	0x000000ff	/* Victim Index D (EA[25:26] = 0b11) */

/* Instruction Cache Transient Victim Registers */
#define	_ITVx_VNDXA_U	0xff00		/* Victim Index A (EA[25:26] = 0b00) */
#define	_ITVx_VNDXB_U	0x00ff		/* Victim Index B (EA[25:26] = 0b01) */
#define	_ITVx_VNDXA	0xff000000	/* Victim Index A (EA[25:26] = 0b00) */
#define	_ITVx_VNDXB	0x00ff0000	/* Victim Index B (EA[25:26] = 0b01) */
#define	_ITVx_VNDXC	0x0000ff00	/* Victim Index C (EA[25:26] = 0b10) */
#define	_ITVx_VNDXD	0x000000ff	/* Victim Index D (EA[25:26] = 0b11) */

/* Instruction Cache Victim Limit Register */
#define	_IVLIM_TFLOOR_U	0x3fc0		/* Transient Floor */
#define	_IVLIM_TFLOOR	0x3fc00000	/* Transient Floor */
#define	_IVLIM_TCEILING	0x0007f800	/* Transient Ceiling */
#define	_IVLIM_NFLOOR	0x000000ff	/* Normal Floor */

/* Memory Management Control Register */
#define	_MMUCR_SWOA_U	  0x0100	/* Store Without Allocate */
#define	_MMUCR_U1TE_U	  0x0040	/* U1 Transient Enable */
#define	_MMUCR_U2SWOAE_U  0x0020	/* U2 Store without Allocate Enable */
#define	_MMUCR_DULXE_U	  0x0008	/* Data Cache Unlock Exception Enable */
#define	_MMUCR_IULXE_U	  0x0004	/* Insn Cache Unlock Exception Enable */
#define	_MMUCR_STS_U	  0x0001	/* Search Translation Space */
#define	_MMUCR_SWOA	  0x01000000	/* Store Without Allocate */
#define	_MMUCR_U1TE	  0x00400000	/* U1 Transient Enable */
#define	_MMUCR_U2SWOAE	  0x00200000	/* U2 Store without Allocate Enable */
#define	_MMUCR_DULXE	  0x00080000	/* Data Cache Unlock Exception Enable */
#define	_MMUCR_IULXE	  0x00040000	/* Insn Cache Unlock Exception Enable */
#define	_MMUCR_STS	  0x00010000	/* Search Translation Space */
#define	_MMUCR_STID	  0x000000ff	/* Search Translation ID */

/* Process ID Register */
#define	_PID_PID	  0x000000ff	/* Process ID */

/* Processor Identification Register */
#define	_PIR_PIN	  0x000000ff	/* Processor Identification Number */

/* Reset Configuration Register */
#define	_RSTCFG_U0	  0x00008000	/* U0 Storage Attribute */
#define	_RSTCFG_U1	  0x00004000	/* U1 Storage Attribute */
#define	_RSTCFG_U2	  0x00002000	/* U2 Storage Attribute */
#define	_RSTCFG_U3	  0x00001000	/* U3 Storage Attribute */
#define	_RSTCFG_E	  0x00000080	/* E Storage Attribute */
#define	_RSTCFG_ERPN	  0x0000000f	/* Extended Real Page Number */

/* Integer Exception Register */
#define	_XER_SO_U	  0x8000	/* Summary Overflow */
#define	_XER_OV_U	  0x4000	/* Overflow */
#define	_XER_CA_U	  0x2000	/* Carry */
#define	_XER_SO		  0x80000000	/* Summary Overflow */
#define	_XER_OV		  0x40000000	/* Overflow */
#define	_XER_CA		  0x20000000	/* Carry */
#define	_XER_TBC	  0x0000007f	/* Transfer Byte Count */
/*
 * exception syndrome register mask bits:
 * 0 - error not occured 1 - error occured
 */
#define	_PPC440_ESR_MCI_U 0x8000      /* Machine Check -- Insn Fetch */
#define	_PPC440_ESR_PIL_U 0x0800      /* Pgm Interrupt -- Illegal Insn */
#define	_PPC440_ESR_PPR_U 0x0400      /* Pgm Interrupt -- Previleged Insn */
#define	_PPC440_ESR_PTR_U 0x0200      /* Pgm Interrupt -- Trap */
#define	_PPC440_ESR_FP_U  0x0100      /* Floating Point Operation */
#define	_PPC440_ESR_ST_U  0x0080      /* Store Operation */
#define	_PPC440_ESR_DLK_U 0x0030      /* Data Storage Interrupt -- Locking */
#define	_PPC440_ESR_AP_U  0x0008      /* AP Operation */
#define	_PPC440_ESR_PUO_U 0x0004      /* Pgm Interrupt -- Unimplemented */
#define	_PPC440_ESR_BO_U  0x0002      /* Byte Ordering Exception */
#define	_PPC440_ESR_PIE_U 0x0001      /* Pgm Interrupt -- Imprecise */
#define	_PPC440_ESR_MCI	  0x80000000  /* Machine Check -- Insn Fetch */
#define	_PPC440_ESR_PIL	  0x08000000  /* Pgm Interrupt -- Illegal Insn */
#define	_PPC440_ESR_PPR	  0x04000000  /* Pgm Interrupt -- Previleged Insn */
#define	_PPC440_ESR_PTR	  0x02000000  /* Pgm Interrupt -- Trap */
#define	_PPC440_ESR_FP	  0x01000000  /* Floating Point Operation */
#define	_PPC440_ESR_ST	  0x00800000  /* Store Operation */
#define	_PPC440_ESR_DLK	  0x00300000  /* Data Storage Interrupt -- Locking */
#define	_PPC440_ESR_AP	  0x00080000  /* AP Operation */
#define	_PPC440_ESR_PUO	  0x00040000  /* Pgm Interrupt -- Unimplemented Opn */
#define	_PPC440_ESR_BO	  0x00020000  /* Byte Ordering Exception */
#define	_PPC440_ESR_PIE	  0x00010000  /* Pgm Interrupt -- Imprecise */
#define	_PPC440_ESR_PCRE  0x00000010  /* Pgm Interrupt -- Condition Reg Enab */
#define	_PPC440_ESR_PCMP  0x00000008  /* Pgm Interrupt -- Compare */
#define	_PPC440_ESR_PCRF  0x00000007  /* Pgm Interrupt -- Condition Reg Field */

#ifdef __cplusplus
}
#endif

#endif /* __INCppc440h */
