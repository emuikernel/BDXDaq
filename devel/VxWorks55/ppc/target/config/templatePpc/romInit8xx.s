/* romInit8xx.s - generic PPC 8XX ROM initialization module */

/* Copyright 1984-2002 Wind River Systems, Inc. */

/*
TODO -	Remove the template modification history and begin a new history
	starting with version 01a and growing the history upward with
	each revision.

modification history
--------------------
01e,17jul02,dat  remove obsolete information
01d,08may01,pch  Add assembler abstractions (FUNC_EXPORT, FUNC_BEGIN, etc.)
01c,07nov98,dat  Added padding between cold and warm entries.
01b,23dec97.dat  SPR 20104, use LOADPTR for 32 bit constants
01a,21aug97,dat  written
*/

/*
TODO - Update documentation as necessary.

NOTICE
This template is generic only for the 8xx versions of PPC (821, 823, 860).

DESCRIPTION
This module contains the entry code for VxWorks images that start
running from ROM, such as 'bootrom' and 'vxWorks_rom'.
The entry point, romInit(), is the first code executed on power-up.
It performs the minimal setup needed to call
the generic C routine romStart() with parameter BOOT_COLD.

RomInit() typically masks interrupts in the processor, sets the initial
stack pointer (to STACK_ADRS which is defined in configAll.h), and
readies system memory by configuring the DRAM controller if necessary.
Other hardware and device initialization is performed later in the
BSP's sysHwInit() routine.

A second entry point in romInit.s is called romInitWarm(). It is called
by sysToMonitor() in sysLib.c to perform a warm boot.
The warm-start entry point must be written to allow a parameter on
the stack to be passed to romStart().

WARNING:
This code must be Position Independent Code (PIC).  This means that it
should not contain any absolute address references.  If an absolute address
must be used, it must be relocated by the macro ROM_ADRS(x).  This macro
will convert the absolute reference to the appropriate address within
ROM space no matter how the boot code was linked. (For PPC, ROM_ADRS does
not work.  You must subtract _romInit and add ROM_TEXT_ADRS to each
absolute address).

This code should not call out to subroutines declared in other modules,
specifically sysLib.o, and sysALib.o.  If an outside module is absolutely
necessary, it can be linked into the system by adding the module 
to the makefile variable BOOT_EXTRA.  If the same module is referenced by
other BSP code, then that module must be added to MACH_EXTRA as well.
Note that some C compilers can generate code with absolute addresses.
Such code should not be called from this module.  If absolute addresses
cannot be avoided, then only ROM resident code can be generated from this
module.  Compressed and uncompressed bootroms or VxWorks images will not
work if absolute addresses are not processed by the macro ROM_ADRS.

WARNING:
The most common mistake in BSP development is to attempt to do too much
in romInit.s.  This is not the main hardware initialization routine.
Only do enough device initialization to get memory functioning.  All other
device setup should be done in sysLib.c, as part of sysHwInit().

Unlike other RTOS systems, VxWorks does not use a single linear device
initialization phase.  It is common for inexperienced BSP writers to take
a BSP from another RTOS, extract the assembly language hardware setup
code and try to paste it into this file.  Because VxWorks provides 3
different memory configurations, compressed, uncompressed, and rom-resident,
this strategy will usually not work successfully.

WARNING:
The second most common mistake made by BSP writers is to assume that
hardware or CPU setup functions done by romInit.o do not need to be
repeated in sysALib.s or sysLib.c.  A vxWorks image needs only the following
from a boot program: The startType code, and the boot parameters string
in memory.  Each VxWorks image will completely reset the CPU and all
hardware upon startup.  The image should not rely on the boot program to
initialize any part of the system (it may assume that the memory controller
is initialized).

This means that all initialization done by romInit.s must be repeated in
either sysALib.s or sysLib.c.  The only exception here could be the
memory controller.  However, in most cases even that can be
reinitialized without harm.

Failure to follow this rule may require users to rebuild bootrom's for
minor changes in configuration.  It is WRS policy that bootroms and vxWorks
images should not be linked in this manner.
*/

/* defines */

#define	_ASMLANGUAGE
#include "vxWorks.h"
#include "sysLib.h"
#include "asm.h"
#include "config.h"
#include "regs.h"	
#include "drv/multi/ppc860Siu.h"


#ifndef SPLL_MUL_FACTOR_20MHZ
#   define	 SPLL_MUL_FACTOR_20MHZ ((20000000 / 4000000) - 1) /* 4Mhz crystal */
#endif SPLL_MUL_FACTOR_20MHZ

/* TODO - These MACROS belong elsewhere (templatePpc.h) */

#define	BCSR_BASE_ADDR	0x02100000	/* BCSR base address */
#define BCSR0		BCSR_BASE_ADDR 		/* Register 0 */
#define BCSR1		BCSR_BASE_ADDR + 0x04	/* Register 1 */
#define BCSR2		BCSR_BASE_ADDR + 0x08	/* Register 2 */
#define BCSR3		BCSR_BASE_ADDR + 0x0c	/* Register 3 */

#define BCSR2_FLASH_PD_2MEG_SIMM	0x80000000	/* Flash presence 2 M */
#define BCSR2_FLASH_PD_4MEG_SIMM	0x70000000	/* Flash presence 4 M */
#define BCSR2_FLASH_PD_8MEG_SIMM	0x60000000	/* Flash presence 8 M */
#define BCSR2_DRAM_EDO_L		0x00000000	/* Dram is EDO */
#define BCSR2_DRAM_NO_EDO_L		0x08000000	/* Dram is not EDO */
#define BCSR2_DRAM_PD_4MEG_SIMM		0x00000000	/* Dram presence  4 M */
#define BCSR2_DRAM_PD_32MEG_SIMM	0x00800000	/* Dram presence 32 M */
#define BCSR2_DRAM_PD_16MEG_SIMM	0x01000000	/* Dram presence 16 M */
#define BCSR2_DRAM_PD_8MEG_SIMM		0x01800000	/* Dram presence  8 M */
#define BCSR2_DRAM_PD_SIZE_MASK		0x01800000	/* Dram size mask */
#define BCSR2_DRAM_PD_70NS_SIMM		0x04000000	/* Dram presence 70ns */
#define BCSR2_DRAM_PD_60NS_SIMM		0x06000000	/* Dram presence 60ns */
#define BCSR2_DRAM_PD_SPEED_MASK	0x06000000	/* Dram spreed mask */

/* Can't use ROM_ADRS macro with HIADJ and LO macro functions, for PPC */

	/* Exported internal functions */

	.data
	FUNC_EXPORT(_romInit)		/* start of system code */
	FUNC_EXPORT(romInit)		/* start of system code */
	FUNC_EXPORT(_romInitWarm)	/* start of system code */
	FUNC_EXPORT(romInitWarm)	/* start of system code */

	/* externals */

	FUNC_IMPORT(romStart)		/* system initialization routine */

	.text
	.align 2

/******************************************************************************
*
* romInit - entry point for VxWorks in ROM
*

* romInit
*     (
*     int startType	/@ only used by 2nd entry point @/
*     )

*/

FUNC_BEGIN(_romInit)
FUNC_LABEL(romInit)
	bl	cold		/* jump to the cold boot initialization */
	nop
	
FUNC_LABEL(_romInitWarm)
FUNC_LABEL(romInitWarm)
	bl	start		/* jump to the warm boot initialization */

	/* copyright notice appears at beginning of ROM (in TEXT segment) */

	.ascii   "Copyright 1984-2002 Wind River Systems, Inc."
	.align 2

cold:
	li	r3, BOOT_COLD	/* set cold boot as start type */

	/*
	 * When the PowerPC 860 is powered on, the processor fetches the
	 * instructions located at the address 0x100. We need to jump
	 * from address 0x100 into the actual ROM space.
	 */

	LOADPTR (r4, start)
	LOADPTR (r5, romInit)
	LOADPTR (r6, ROM_TEXT_ADRS)

	sub	r4, r4, r5			/*  */
	add	r4, r4, r6 

	mtspr	LR, r4				/* save destination address*/
						/* into LR register */
	blr					/* jump to ROM */
			
start:
	/* set the MSR register to a known state */

	xor	r4, r4, r4		/* clear register R4 */
	mtmsr 	r4			/* cleat the MSR register */

	/* DER - clear the Debug Enable Register */

	mtspr	DER, r4

	/* ICR - clear the Interrupt Cause Register */

	mtspr	ICR, r4

	/* ICTRL - initialize the Intstruction Support Control register */

	LOADPTR (r5, 0x00000007)
	mtspr	ICTRL, r5

	/* disable the instruction/data cache */
	
	LOADPTR (r4, CACHE_CMD_DISABLE)
	mtspr	IC_CST, r4				/* disable I cache */
	mtspr	DC_CST, r4				/* disable D cache */

	/* unlock the instruction/data cache */

	LOADPTR (r4, CACHE_CMD_UNLOCK_ALL)	/* load unlock cmd */
	mtspr	IC_CST, r4			/* unlock all I cache lines */
	mtspr	DC_CST, r4			/* unlock all D cache lines */

	/* invalidate the instruction/data cache */

	LOADPTR (r4, CACHE_CMD_INVALIDATE)	/* load invalidate cmd*/
	mtspr	IC_CST, r4		/* invalidate all I cache lines */
	mtspr	DC_CST, r4		/* invalidate all D cache lines */

	/*
	 * initialize the IMMR register before any non-core registers
	 * modification.
	 */

	LOADPTR (r4, INTERNAL_MEM_MAP_ADDR)	
	mtspr	IMMR, r4		/* initialize the IMMR register */

	mfspr	r4, IMMR		/* read it back, to be sure */
	rlwinm  r4, r4, 0, 0, 15	/* only high 16 bits count */

	/* SYPCR - turn off the system protection stuff */

	LOADPTR (r5, (SYPCR_SWTC | SYPCR_BMT | SYPCR_BME | SYPCR_SWF))
	stw	r5, SYPCR(0)(r4)

	/* set the SIUMCR register for important debug port, etc... stuff */

	lwz	r5, SIUMCR(0)(r4)
	LOADPTR (r6, SIUMCR_FRC	       | SIUMCR_DLK  | SIUMCR_DPC |
		   SIUMCR_MLRC_3STATES | SIUMCR_AEME | SIUMCR_GB5E)
	or	r5, r5, r6
	stw	r5, SIUMCR(0)(r4)

	/* TBSCR - initialize the Time Base Status and Control register */

	LOADPTR (r5, TBSCR_REFA | TBSCR_REFB)
	sth	r5, TBSCR(0)(r4)

	/* set PIT status and control init value */

	li	r5, PISCR_PS | PISCR_PITF
	sth	r5, PISCR(0)(r4)

	/* set the SPLL frequency to 20 Mhz */

	LOADPTR (r5, (SPLL_MUL_FACTOR_20MHZ << PLPRCR_MF_SHIFT) | PLPRCR_TEXPS)
	stw	r5, PLPRCR(0)(r4)

	/* divide by 16 */

	li	r5, MPTPR_PTP_DIV16 
	sth	r5, MPTPR(0)(r4)

	/* Machine A mode register setup */

	LOADPTR (r5, ( (0x13 << MAMR_PTA_SHIFT) | MAMR_PTAE |
			  MAMR_AMA_TYPE_2 | MAMR_DSA_1_CYCL | MAMR_G0CLA_A12 |
			  MAMR_GPL_A4DIS  | MAMR_RLFA_1X    | MAMR_WLFA_1X   |
			  MAMR_TLFA_4X))
	stw	r5, MAMR(0)(r4)

	/* Map bank 0 to the flash area.  */

	LOADPTR (r5, ((ROM_BASE_ADRS & BR_BA_MSK) | BR_V))
	stw	r5, BR0(0)(r4)


	/* TODO - setup the bank 0 configuration */

	LOADPTR (r5, 0xffe00000 | OR_CSNT_SAM | OR_BI | OR_SCY_5_CLK | OR_TRLX)
	stw	r5, OR0(0)(r4)

	/* Map bank 1 to the Board Status and Control Registers */

	LOADPTR (r5, (BCSR0 & BR_BA_MSK) | BR_V)
	stw	r5, BR1(0)(r4)

	/* TODO - setup the Bank 1 configuration */

	LOADPTR (r5, (0xffff8000 & OR_AM_MSK) | OR_BI | OR_SCY_1_CLK)
	stw	r5, OR1(0)(r4)

	/* Map main memory to bank 2 (and 3) */

	/* get the DRAM speed and if EDO capability supported */

	lis	r5, HIADJ(BCSR2)	/* load r5 with the BCSR2 address */
	lwz	r6, LO(BCSR2)(r5)	/* load r6 with the BCSR2 value */
	lis	r5, HI(BCSR2_DRAM_NO_EDO_L | BCSR2_DRAM_PD_SPEED_MASK)
	and	r6, r6, r5

	lis	r5, HI(BCSR2_DRAM_NO_EDO_L | BCSR2_DRAM_PD_60NS_SIMM)
	cmpw	r6, r5
	beq	dram60ns

	lis	r5, HI(BCSR2_DRAM_NO_EDO_L | BCSR2_DRAM_PD_70NS_SIMM)
	cmpw	r6, r5
	beq	dram70ns

	lis	r5, HI(BCSR2_DRAM_EDO_L | BCSR2_DRAM_PD_60NS_SIMM)
	cmpw	r6, r5
	beq	dramEdo60ns

	lis	r5, HI(BCSR2_DRAM_EDO_L | BCSR2_DRAM_PD_70NS_SIMM)
	cmpw	r6, r5
	beq	dramEdo70ns

	/* TODO - program additional memory configurations as needed */

dram60ns:
#ifndef	EDO_DRAM
	/*
	 * load r6/r7 with the start/end address of the UPM table for a
	 * non-EDO 60ns Dram.
	 */

	LOADPTR (r6, upmTable60)
	LOADPTR (r7, upmTable60End)
	b	upmInit	
#endif

dramEdo60ns:
	/*
	 * load r6/r7 with the start/end address of the UPM table for an
	 * EDO 60ns Dram.
	 */

	LOADPTR (r6, upmTableEdo60)
	LOADPTR (r7, upmTableEdo60End)
	b	upmInit	

dram70ns:
#ifndef	EDO_DRAM
	/*
	 * load r6/r7 with the start/end address of the UPM table for a
	 * none EDO 70ns Dram.
	 */

	LOADPTR (r6, upmTable70)
	LOADPTR (r7, upmTable70End)
	b	upmInit	
#endif

dramEdo70ns:
	/*
	 * load r6/r7 with the start/end address of the UPM table for an
	 * EDO 70ns Dram.
	 */

	LOADPTR (r6, upmTableEdo70)
	LOADPTR (r7, upmTableEdo70End)

upmInit:
	/* init UPMA for memory access */

	sub	r5, r7, r6		/* compute table size */
	srawi	r5, r5, 2		/* in integer size */

	/* convert upmTable to ROM based addressing */

	LOADPTR (r7, romInit)
	LOADPTR (r8, ROM_TEXT_ADRS)

	sub	r6, r6, r7		/* subtract romInit base address */
	add	r6, r6, r8 		/* add in ROM_TEXT_ADRS address */

					/* Command: OP=Write, UPMA, MAD=0 */
	LOADPTR (r9, MCR_OP_WRITE | MCR_UM_UPMA | MCR_MB_CS0)

upmWriteLoop:	
	/* write the UPM table in the UPM */

	lwz	r10, 0(r6)		/* get data from table */
	stw	r10, MDR(0)(r4)		/* store the data to MD register */

	stw	r9, MCR(0)(r4)		/* issue command to MCR register */

	addi	r6, r6, 4		/* next entry in the table */
	addi	r9, r9, 1		/* next MAD address */
	cmpw	r9, r5			/* done yet ? */
	blt	upmWriteLoop

	/* get the DRAM size, and Map bank 2 & 3 to the Dram area */

	lis	r5, HIADJ(BCSR2)	/* load r5 with the BCSR2 address */
	lwz	r6, LO(BCSR2)(r5)	/* load r6 with the BCSR2 value */
	lis	r5, HI(BCSR2_DRAM_PD_SIZE_MASK)
	and	r6, r6, r5

	lis	r5, HI(BCSR2_DRAM_PD_4MEG_SIMM)
	cmpw	r6, r5
	beq	dram4meg

	lis	r5, HI(BCSR2_DRAM_PD_8MEG_SIMM)
	cmpw	r6, r5
	beq	dram8meg

	lis	r5, HI(BCSR2_DRAM_PD_16MEG_SIMM)
	cmpw	r6, r5
	beq	dram16meg

	lis	r5, HI(BCSR2_DRAM_PD_32MEG_SIMM)
	cmpw	r6, r5
	beq	dram32meg

	/* TODO - program additional memory configurations as needed */

dram32meg:
	/* program BR3, for upper 16MB */

	LOADPTR (r5, ((0x01000000 + LOCAL_MEM_LOCAL_ADRS) & 
			BR_BA_MSK) | BR_MS_UPMA | BR_V)
	stw	r5, BR3(0)(r4)

dram16meg:	
	/* compute the OR3/OR2 value for a 16M block size */

	/* TODO - setup the Bank 3 configuration */

	LOADPTR (r5, 0xff000000 | OR_CSNT_SAM)
	stw	r5, OR3(0)(r4)	/* Set OR3. Used only when BR3 is valid */

	/* TODO - change the Address Multiplexing in MAMR */

	lwz	r6, MAMR(0)(r4)
	LOADPTR (r9, ~MAMR_AMA_MSK)	
	and	r6, r6, r9			/* clear the AMA bits in MAMR */
	LOADPTR (r9, MAMR_AMA_TYPE_3)
	or	r6, r6, r9			/* set the AMA bits */
	stw	r6, MAMR(0)(r4)

	b	dramInit

dram8meg:
	/* program BR3, for upper 4 MB */

	LOADPTR (r5, ((0x00400000 + LOCAL_MEM_LOCAL_ADRS) &
		    BR_BA_MSK) | BR_MS_UPMA | BR_V)
	stw	r5, BR3(0)(r4)

dram4meg:	
	/* compute the OR3/OR2 value for a 4M block size */

	LOADPTR (r5, 0xffc00000 | OR_CSNT_SAM)
	stw	r5, OR3(0)(r4)	/* Set OR3. Used only when BR3 is valid */
	b	dramInit

		
dramInit:	
	/* Map the bank 2 to the Dram area */

	stw	r5, OR2(0)(r4)	/* set OR2 to the previously computed value */

	LOADPTR (r5, (LOCAL_MEM_LOCAL_ADRS & BR_BA_MSK) | BR_MS_UPMA |
			BR_V)
	stw	r5, BR2(0)(r4)

	/* TODO - map additional banks as needed */

	/* TODO - reset external devices, if possible */

        /* initialize the stack pointer */

	LOADPTR (sp, STACK_ADRS)
	
        /* initialize r2 and r13 according to EABI standard */
#if	FALSE					/* SDA Not supported yet */

	LOADPTR (r2, _SDA2_BASE_)
	LOADPTR (r13, _SDA_BASE_)
#endif
	/* go to C entry point */

	addi	sp, sp, -FRAMEBASESZ		/* get frame stack */

	/* 
	 * calculate C entry point: routine - entry point + ROM base 
	 * routine	= romStart
	 * entry point	= romInit	= R7
	 * ROM base	= ROM_TEXT_ADRS = R8
	 * C entry point: romStart - R7 + R8 
	 */

	LOADPTR (r6, romStart)
	sub	r6, r6, r7			/* routine - entry point */
	add	r6, r6, r8 			/* + ROM base */

	mtlr	r6				/* move C entry point to LR */
	blr					/* jump to the C entry point */


	/*
	 * The following tables contain the DRAM controller
	 * configurations.  There are two versions of each
	 * table.  The first is for 50Mhz, the
	 * second is for 25 Mhz.
	 *
	 * TODO - add additional table entries as required.
	 */

#if	(SPLL_FREQ != FREQ_25_MHZ)

	/* UPM initialization table, 60ns, 50MHz */
upmTable60:

	/* single read. (offset 0 in upm RAM) */

        .long	0x8fffec24, 0x0fffec04, 0x0cffec04, 0x00ffec04
	.long	0x00ffec00, 0x37ffec47
        .long	0xffffffff, 0xffffffff

	/* burst read. (offset 8 in upm RAM) */

        .long	0x8fffec24, 0x0fffec04, 0x08ffec04, 0x00ffec0c
        .long	0x03ffec00, 0x00ffec44, 0x00ffcc08, 0x0cffcc44
        .long	0x00ffec0c, 0x03ffec00, 0x00ffec44, 0x00ffcc00
        .long	0x3fffc847
        .long	0xffffffff, 0xffffffff, 0xffffffff

	/* single write. (offset 18 in upm RAM) */

        .long	0x8fafcc24, 0x0fafcc04, 0x0cafcc00, 0x11bfcc47
        .long	0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff

	/*  burst write. (offset 20 in upm RAM) */

        .long	0x8fafcc24, 0x0fafcc04, 0x0cafcc00, 0x03afcc4c
        .long	0x0cafcc00, 0x03afcc4c, 0x0cafcc00, 0x03afcc4c
        .long	0x0cafcc00, 0x33bfcc4f
        .long	0xffffffff, 0xffffffff
        .long	0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff

	/* refresh. (offset 30 in upm RAM) */

        .long	0xc0ffcc84, 0x00ffcc04, 0x07ffcc04, 0x3fffcc06
        .long	0xffffcc85, 0xffffcc05
	.long	0xffffffff, 0xffffffff
        .long	0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff

	/* exception. (offset 3c in upm RAM) */

        .long	0x33ffcc07
        .long	0xffffffff, 0xffffffff, 0xffffffff
upmTable60End:

	/* UPM initialization table, 70ns, 50MHz */
upmTable70:

	/* single read. (offset 0 in upm RAM) */

        .long	0x8fffcc24, 0x0fffcc04, 0x0cffcc04, 0x00ffcc04
        .long	0x00ffcc00, 0x37ffcc47
        .long	0xffffffff, 0xffffffff

	/* burst read. (offset 8 in upm RAM) */

        .long	0x8fffcc24, 0x0fffcc04, 0x0cffcc04, 0x00ffcc04
        .long	0x00ffcc08, 0x0cffcc44, 0x00ffec0c, 0x03ffec00
        .long	0x00ffec44, 0x00ffcc08, 0x0cffcc44, 0x00ffec04
        .long	0x00ffec00, 0x3fffec47 
        .long	0xffffffff, 0xffffffff

	/* single write. (offset 18 in upm RAM) */

        .long	0x8fafcc24, 0x0fafcc04, 0x0cafcc00, 0x11bfcc47
        .long	0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff

	/* burst write. (offset 20 in upm RAM) */

        .long	0x8fafcc24, 0x0fafcc04, 0x0cafcc00, 0x03afcc4c
        .long	0x0cafcc00, 0x03afcc4c, 0x0cafcc00, 0x03afcc4c
        .long	0x0cafcc00, 0x33bfcc4f
        .long	0xffffffff, 0xffffffff
        .long	0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff

	/* refresh. (offset 30 in upm RAM) */

        .long	0xe0ffcc84, 0x00ffcc04, 0x00ffcc04,  0x0fffcc04
        .long	0x7fffcc06, 0xffffcc85, 0xffffcc05
	.long	0xffffffff
        .long	0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff

	/* exception. (offset 3c in upm RAM) */

        .long	0x33ffcc07
        .long	0xffffffff, 0xffffffff, 0xffffffff
upmTable70End:


	/* UPM initialization table, EDO, 60ns, 50MHz */
upmTableEdo60:

	/* single read. (offset 0 in upm RAM) */

        .long	0x8ffbec24, 0x0ff3ec04, 0x0cf3ec04, 0x00f3ec04
	.long	0x00f3ec00, 0x37f7ec47
        .long	0xffffffff, 0xffffffff

	/* burst read. (offset 8 in upm RAM) */

        .long	0x8fffec24, 0x0ffbec04, 0x0cf3ec04, 0x00f3ec0c
        .long	0x0cf3ec00, 0x00f3ec4c, 0x0cf3ec00, 0x00f3ec4c
        .long	0x0cf3ec00, 0x00f3ec44, 0x03f3ec00, 0x3ff7ec47
        .long	0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff 

	/* single write. (offset 18 in upm RAM) */

        .long	0x8fffcc24, 0x0fefcc04, 0x0cafcc00, 0x11bfcc47
        .long	0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff

	/* burst write. (offset 20 in upm RAM) */

        .long	0x8fffcc24, 0x0fefcc04, 0x0cafcc00, 0x03afcc4c
        .long	0x0cafcc00, 0x03afcc4c, 0x0cafcc00, 0x03afcc4c
        .long	0x0cafcc00, 0x33bfcc4f
        .long	0xffffffff, 0xffffffff
        .long	0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff

	/* refresh. (offset 30 in upm RAM) */

        .long	0xc0ffcc84, 0x00ffcc04, 0x07ffcc04, 0x3fffcc06
        .long	0xffffcc85, 0xffffcc05
	.long	0xffffffff, 0xffffffff
        .long	0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff

	/* exception. (offset 3c in upm RAM) */

        .long	0x33ffcc07
        .long	0xffffffff, 0xffffffff, 0xffffffff
upmTableEdo60End:


	/* UPM initialization table, EDO, 70ns, 50MHz */
upmTableEdo70:

	/* single read. (offset 0 in upm RAM) */

        .long	0x8ffbcc24, 0x0ff3cc04, 0x0cf3cc04, 0x00f3cc04
        .long	0x00f3cc00, 0x37f7cc47
        .long	0xffffffff, 0xffffffff

	/* burst read. (offset 8 in upm RAM) */

        .long	0x8fffcc24, 0x0ffbcc04, 0x0cf3cc04, 0x00f3cc0c
        .long	0x03f3cc00, 0x00f3cc44, 0x00f3ec0c, 0x0cf3ec00
        .long	0x00f3ec4c, 0x03f3ec00, 0x00f3ec44, 0x00f3cc00
        .long	0x33f7cc47 
        .long	0xffffffff, 0xffffffff, 0xffffffff

	/* single write. (offset 18 in upm RAM) */

        .long	0x8fffcc24, 0x0fefcc04, 0x0cafcc00, 0x11bfcc47
        .long	0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff

	/* burst write. (offset 20 in upm RAM) */

        .long	0x8fffcc24, 0x0fefcc04, 0x0cafcc00, 0x03afcc4c
        .long	0x0cafcc00, 0x03afcc4c, 0x0cafcc00, 0x03afcc4c
        .long	0x0cafcc00, 0x33bfcc47
        .long	0xffffffff, 0xffffffff
        .long	0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff

	/* refresh. (offset 30 in upm RAM) */

        .long	0xe0ffcc84, 0x00ffcc04, 0x00ffcc04,  0x0fffcc04
        .long	0x7fffcc04, 0xffffcc86, 0xffffcc05
	.long	0xffffffff
        .long	0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff

	/* exception. (offset 3c in upm RAM) */

        .long	0x33ffcc07
        .long	0xffffffff, 0xffffffff, 0xffffffff
upmTableEdo70End:

#else	/* FALSE/TRUE */


	/* UPM initialization table, 60ns, 25MHz */
upmTable60:

	/* single read. (offset 0 in upm RAM) */

        .long	0x0fffcc04, 0x08ffcc00, 0x33ffcc47
        .long	0xffffffff
        .long	0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff

	/* burst read. (offset 8 in upm RAM) */

        .long	0x0fffcc24, 0x0fffcc04, 0x08ffcc00, 0x03ffcc4c
        .long	0x08ffcc00, 0x03ffcc4c, 0x08ffcc00, 0x03ffcc4c
        .long	0x08ffcc00, 0x33ffcc47
        .long	0xffffffff, 0xffffffff
        .long	0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff

	/* single write. (offset 18 in upm RAM) */

        .long	0x0fafcc04, 0x08afcc00, 0x3fbfcc47
        .long	0xffffffff
        .long	0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff

	/* burst write. (offset 20 in upm RAM) */

        .long	0x0fafcc04, 0x0cafcc00, 0x01afcc4c, 0x0cafcc00
        .long	0x01afcc4c, 0x0cafcc00, 0x01afcc4c, 0x0cafcc00
        .long	0x31bfcc43
        .long	0xffffffff, 0xffffffff, 0xffffffff
        .long	0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff

	/* refresh. (offset 30 in upm RAM) */

        .long	0x80ffcc84, 0x13ffcc04, 0xffffcc87, 0xffffcc05
        .long	0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff
        .long	0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff

	/* exception. (offset 3c in upm RAM) */

        .long	0x33ffcc07
        .long	0xffffffff, 0xffffffff, 0xffffffff
upmTable60End:

	/* UPM initialization table, 70ns, 25MHz */

upmTable70:

	/* single read. (offset 0 in upm RAM) */

        .long	0x0fffec04, 0x08ffec04, 0x00ffec00, 0x3fffcc47
        .long	0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff

	/* burst read. (offset 8 in upm RAM) */

        .long	0x0fffcc24, 0x0fffcc04, 0x08ffcc00, 0x03ffcc4c
        .long	0x08ffcc00, 0x03ffcc4c, 0x08ffcc00, 0x03ffcc4c
        .long	0x08ffcc00, 0x33ffcc47
        .long	0xffffffff, 0xffffffff
        .long	0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff

	/* single write. (offset 18 in upm RAM) */

        .long	0x0fafcc04, 0x08afcc00, 0x3fbfcc47
        .long	0xffffffff
        .long	0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff

	/* burst write. (offset 20 in upm RAM) */

        .long	0x0fafcc04, 0x0cafcc00, 0x01afcc4c, 0x0cafcc00
        .long	0x01afcc4c, 0x0cafcc00, 0x01afcc4c, 0x0cafcc00
        .long	0x31bfcc43
        .long	0xffffffff, 0xffffffff, 0xffffffff
        .long	0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff

	/* refresh. (offset 30 in upm RAM) */

        .long	0xc0ffcc84, 0x01ffcc04, 0x7fffcc86, 0xffffcc05
        .long	0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff
        .long	0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff

	/* exception. (offset 3c in upm RAM) */

        .long	0x33ffcc07
        .long	0xffffffff, 0xffffffff, 0xffffffff
upmTable70End:

	/* UPM initialization table, 60ns EDO DRAMs, 25MHz */

upmTableEdo60:

	/* single read. (offset 0 in upm RAM) */

        .long	0x0ffbcc04, 0x0cf3cc04, 0x00f3cc00, 0x33f7cc47
        .long	0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff

	/* burst read. (offset 8 in upm RAM) */

        .long	0x0ffbcc04, 0x09f3cc0c, 0x09f3cc0c, 0x09f3cc0c
        .long	0x08f3cc00, 0x3ff7cc47
        .long	0xffffffff, 0xffffffff
        .long	0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff
        .long	0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff

	/* single write. (offset 18 in upm RAM) */

        .long	0x0fefcc04, 0x08afcc04, 0x00afcc00, 0x3fbfcc47
        .long	0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff

	/* burst write. (offset 20 in upm RAM) */

        .long	0x0fefcc04, 0x08afcc00, 0x07afcc48, 0x08afcc48
        .long	0x08afcc48, 0x39bfcc47
        .long	0xffffffff, 0xffffffff
        .long	0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff
        .long	0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff

	/* refresh. (offset 30 in upm RAM) */

        .long	0x80ffcc84, 0x13ffcc04, 0xffffcc87, 0xffffcc05
        .long	0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff
        .long	0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff

	/* exception. (offset 3c in upm RAM) */

        .long	0x33ffcc07
        .long	0xffffffff, 0xffffffff, 0xffffffff
upmTableEdo60End:

	/* UPM initialization table, EDO, 70ns DRAM, 25MHz */

upmTableEdo70:

	/* single read. (offset 0 in upm RAM) */

        .long	0x0ffbcc04, 0x0cf3cc04, 0x00f3cc00, 0x33f7cc47
        .long	0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff

	/* burst read. (offset 8 in upm RAM) */

        .long	0x0ffbec04, 0x08f3ec04, 0x03f3ec48, 0x08f3cc00
        .long	0x0ff3cc4c, 0x08f3cc00, 0x0ff3cc4c, 0x08f3cc00
	.long	0x3ff7cc47
        .long	0xffffffff, 0xffffffff, 0xffffffff
        .long	0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff

	/* single write. (offset 18 in upm RAM) */

        .long	0x0fefcc04, 0x08afcc04, 0x00afcc00, 0x3fbfcc47
        .long	0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff

	/* burst write. (offset 20 in upm RAM) */

        .long	0x0fefcc04, 0x08afcc00, 0x07afcc4c, 0x08afcc00
        .long	0x07afcc4c, 0x08afcc00, 0x07afcc4c, 0x08afcc00
	.long	0x37bfcc47
        .long	0xffffffff, 0xffffffff, 0xffffffff
        .long	0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff

	/* refresh. (offset 30 in upm RAM) */

        .long	0xc0ffcc84, 0x01ffcc04, 0x7fffcc86, 0xffffcc05
        .long	0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff
        .long	0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff

	/* exception. (offset 3c in upm RAM) */

        .long	0x33ffcc07
        .long	0xffffffff, 0xffffffff, 0xffffffff
upmTableEdo70End:
	
	/* TODO - add additional tables for new memory configurations*/

#endif
FUNC_END(_romInit)
