/******************************************************************************
*              (c), Copyright, MOTOROLA  INC  - ECC                  *
*                                                                             *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MOTOROLA , INC.        *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT *
* OF MOTOROLA   OR ANY THIRD PARTY. MOTOROLA   RESERVES THE RIGHT AT ITS SOLE DISCRETION*
* TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MOTOROLA .       *
* THIS CODE IS PROVIDED "AS IS". MOTOROLA  MAKES NO WARRANTIES, EXPRESS,*
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.  *
*******************************************************************************

*******************************************************************************
* romInit.s - ROM initialization module
*
* DESCRIPTION:
*       This module contains the entry code for VxWorks images that start
*       running from ROM, such as 'bootrom' and 'vxWorks_rom'.
*       The entry point, romInit(), is the first code executed on power-up.
*       It performs the minimal setup needed to call
*       the generic C routine romStart() with parameter BOOT_COLD.
*
*       RomInit() typically masks interrupts in the processor, sets the initial
*       stack pointer (to STACK_ADRS which is defined in configAll.h), and
*       readies system memory by configuring the DRAM controller if necessary.
*       Other hardware and device initialization is performed later in the
*       BSP's sysHwInit() routine.
*
*       A second entry point in romInit.s is called romInitWarm(). It is called
*       by sysToMonitor() in sysLib.c to perform a warm boot.
*       The warm-start entry point must be written to allow a parameter on
*       the stack to be passed to romStart().
*
* WARNING:
*       This code must be Position Independent Code (PIC). This means that it
*       should not contain any absolute address references. If an absolute
*       address must be used, it must be relocated by the macro ROM_ADRS(x).
*       This macro will convert the absolute reference to the appropriate
*       address within ROM space no matter how the boot code was linked.
*       (For PPC, ROM_ADRS does not work. You must subtract _romInit and add
*       ROM_TEXT_ADRS to each absolute address).
*       (NOTE: ROM_ADRS(x) macro does not work for current PPC compiler).
*
*       This code should not call out to subroutines declared in other modules,
*       specifically sysLib.o, and sysALib.o. If an outside module is absolutely
*       necessary, it can be linked into the system by adding the module to the
*       makefile variable BOOT_EXTRA. If the same module is referenced by other
*       BSP code, then that module must be added to MACH_EXTRA as well.
*       Note that some C compilers can generate code with absolute addresses.
*       Such code should not be called from this module.  If absolute addresses
*       cannot be avoided, then only ROM resident code can be generated from
*       this module. Compressed and uncompressed bootroms or VxWorks images will
*       not work if absolute addresses are not processed by the macro ROM_ADRS.
*
* WARNING:
*       The most common mistake in BSP development is to attempt to do too much
*       in romInit.s.  This is not the main hardware initialization routine.
*       Only do enough device initialization to get memory functioning. All
*       other device setup should be done in sysLib.c, as part of sysHwInit().
*
*       Unlike other RTOS systems, VxWorks does not use a single linear device
*       initialization phase.  It is common for inexperienced BSP writers to
*       take a BSP from another RTOS, extract the assembly language hardware
*       setup code and try to paste it into this file.  Because VxWorks
*       provides 3 different memory configurations, compressed, uncompressed,
*       and rom-resident, this strategy will usually not work successfully.
*
* WARNING:
*       The second most common mistake made by BSP writers is to assume that
*       hardware or CPU setup functions done by romInit.s do not need to be
*       repeated in sysALib.s or sysLib.c. A vxWorks image needs only the
*       following from a boot program: The startType code, and the boot
*       parameters string in memory. Each VxWorks image will completely reset
*       the CPU and all hardware upon startup. The image should not rely on the
*       boot program to initialize any part of the system (it may assume that
*       the memory controller is initialized).
*
*       This means that all initialization done by romInit.s must be repeated
*       in either sysALib.s or sysLib.c.  The only exception here could be the
*       memory controller.  However, in most cases even that can be
*       reinitialized without harm.
*       
*       Failure to follow this rule may require users to rebuild bootrom's for
*       minor changes in configuration.  It is WRS policy that bootroms and
*       vxWorks images should not be linked in this manner.
*
* DEPENDENCIES:
*       None.
*
******************************************************************************/

/* includes */
#define  _ASMLANGUAGE
#include "vxWorks.h"
#include "sysLib.h"
#include "asm.h"
#include "regs.h"
#include "config.h"
#include "frcBoardId.h"
#include "gtInitLib.h"  
#include "cacheLib.h"


/*
 * Some releases of h/arch/ppc/toolPpc.h had a bad definition of
 * LOADPTR. So we will define it correctly. [REMOVE WHEN NO LONGER NEEDED].
 *
 * LOADPTR initializes a register with a 32 bit constant, presumably the
 * address of something.
 */
 
#undef LOADPTR
#define LOADPTR(reg,const32) \
                lis reg,HI(const32); ori reg,reg,LO(const32)
 


/* defines */
                       /* Get 16msb of internal registers base address */
#define INTERNAL_BASE  (INTERNAL_REG_ADRS >> 16) 
#define L2PMCR	      1016 /* for 7410 bug, L2PMCR not cleared on reset */
#define HID1          1009
/*#define BRDID_EE_ADDR1 0xa4*/

#define MEMSIZE_8MB             (0x01 << 23)
#define MEMSIZE_128MB           (0x01 << 27)
#define MEMSIZE_256MB           (0x01 << 28)
#define MEMSIZE_512MB           (0x01 << 29)
#define MEMSIZE_1G              (0x01 << 30)
#define MEMSIZE_2G              (0x01 << 31)

       /* Exported internal functions */

       .data
       FUNC_EXPORT(_romInit)        /* start of system code */
       FUNC_EXPORT(romInit)         /* start of system code */
       FUNC_EXPORT(_romInitWarm)    /* start of system code */
       FUNC_EXPORT(romInitWarm)     /* start of system code */
#ifdef PMC280_DEBUG_LED
       FUNC_EXPORT(dbg_led_23_on)
       FUNC_EXPORT(dbg_led_23_off)
       FUNC_EXPORT(dbg_led_23_blink)
#endif /* PMC280_DEBUG_LED */
       
       /* externals */
       .extern romStart        /* system initialization routine */

       .text
       .align 2


/******************************************************************************
*
* romInit - entry point for VxWorks in ROM
*
* romInit
*     (
*     int startType    /@ only used by 2nd entry point @/
*     )
*/
       /*
        * After reset, the processor fetches from ROM_BASE_ADRS + 0x100.
        * We offset the _romInit entry by 0x100 bytes to compensate.
        * In other WRS PPC BSP's, this offset was accounted for in the
        * elfHex stage of building the rom image by setting HEX_FLAGS
        * to "-a 100".  Explictly adding the space here seems more
        * straitforward, and it also helps when not using elfToHex
        * such as in a flash programmed binary image.   Therefore
        * the Yellowknife BSP Makefile uses HEX_FLAGS of "-a 0".
        * This also means that ROM_TEXT_ADRS must be defined
        * equivalent to ROM_BASE_ADRS + 0x100 in config.h and Makefile.
        */

       .space (0x100)


_romInit:
romInit:
       /* This is the cold boot entry (ROM_TEXT_ADRS) */

       /*
        * According to a comment in the DINK32 startup code, a 7400
        * erratum requires an isync to be the first instruction executed
        * here.  The isync plus the branch amount to 8 bytes, so omit the
        * nop which would otherwise follow the branch.
        */
       isync
       bl  cold

       /*
        * This warm boot entry point is defined as ROM_WARM_ADRS in config.h.
        * It is defined as an offset from ROM_TEXT_ADRS.  Please make sure
        * that the offset from _romInit to romInitWarm matches that specified
        * in config.h.  (WRS is standardizing this offset to 8 bytes.)
        */

_romInitWarm:
romInitWarm:

       /*
        * See above comment regarding a 7400 erratum.  Since DINK32
        * uses a common entry point for both cold and warm boot, it's
        * not clear which one requires the isync.  isync is harmless,
        * so do it both places to be safe.
        */
       isync
       bl  warm

	/*
	 * copyright notice appears at beginning of ROM (in TEXT segment)
	 * This is also useful for debugging images.
	 */
       .ascii   "Copyright 1984-1999 Wind River Systems, Inc."
       .align 2

cold:
       li        r11, BOOT_COLD
       bl        start           /* skip over next instruction */

warm:
       or       r11, r3, r3     /* startType to r11 */

start:

      /*
       * DINK32 does a sync after the isync.  It may or may not
       * be necessary, but should at least be harmless.
       */
       sync

        /* Zero-out registers: r0 & SPRGs */
        xor	r0, r0, r0
        mtspr	SPRG0, r0
        mtspr	SPRG1, r0
        mtspr	SPRG2, r0
        mtspr	SPRG3, r0

#ifdef PMC280_DUAL_CPU
        /*
         * We will not initialise sp here. This will be done in the
         * CPU dependent parts of the code.
         */
#else
       /* initialize the stack pointer */
        LOADPTR (sp, STACK_ADRS)
#endif /* PMC280_DUAL_CPU */

       /*
        * Set HID0 to a known state
        * Enable machine check input pin (EMCP)
        * Disable instruction and data caches
        */
        lis	r3, 0x8000    /* r3 <= 8000.0000 */
        sync                    /* required before changing DCE */
        isync                   /* required before chaning ICE  */
        mtspr	HID0, r3

       /* Set MPU/MSR to a known state, Turn on FP */
        LOADPTR (r3, _PPC_MSR_FP)
        sync
        mtmsr 	r3
        isync

       /* Init the floating point control/status register */
        mtfsfi	7,0x0
        mtfsfi	6,0x0
        mtfsfi	5,0x0
        mtfsfi	4,0x0
        mtfsfi	3,0x0
        mtfsfi	2,0x0
        mtfsfi	1,0x0
        mtfsfi	0,0x0
        isync

       /* Initialize the floating point data registers to a known state */
        bl	ifpdr_value
        .long	0x3f800000      /* 1.0 */

ifpdr_value:
       mfspr	r3,LR
       lfs	f0,0(r3)
       lfs	f1,0(r3)
       lfs	f2,0(r3)
       lfs	f3,0(r3)
       lfs	f4,0(r3)
       lfs	f5,0(r3)
       lfs	f6,0(r3)
       lfs	f7,0(r3)
       lfs	f8,0(r3)
       lfs	f9,0(r3)
       lfs	f10,0(r3)
       lfs	f11,0(r3)
       lfs	f12,0(r3)
       lfs	f13,0(r3)
       lfs	f14,0(r3)
       lfs	f15,0(r3)
       lfs	f16,0(r3)
       lfs	f17,0(r3)
       lfs	f18,0(r3)
       lfs	f19,0(r3)
       lfs	f20,0(r3)
       lfs	f21,0(r3)
       lfs	f22,0(r3)
       lfs	f23,0(r3)
       lfs	f24,0(r3)
       lfs	f25,0(r3)
       lfs	f26,0(r3)
       lfs	f27,0(r3)
       lfs	f28,0(r3)
       lfs	f29,0(r3)
       lfs	f30,0(r3)
       lfs	f31,0(r3)
	sync
	
      /* Set MPU/MSR to a known state, Turn off FP */
       andi.	r3, r3, 0
       sync
       mtmsr	r3
       isync
       sync	/* DINK32 does both isync and sync after mtmsr */

      /* Init the Segment registers */
       andi.	r3, r3, 0
       isync
       mtsr	0,r3
       isync
       mtsr	1,r3
       isync
       mtsr	2,r3
       isync
       mtsr	3,r3
       isync
       mtsr	4,r3
       isync
       mtsr	5,r3
       isync
       mtsr	6,r3
       isync
       mtsr	7,r3
       isync
       mtsr	8,r3
       isync
       mtsr	9,r3
       isync
       mtsr	10,r3
       isync
       mtsr	11,r3
       isync
       mtsr	12,r3
       isync
       mtsr	13,r3
       isync
       mtsr	14,r3
       isync
       mtsr	15,r3
       isync

      /* Turn off data and instruction cache control bits */
       mfspr	r3, HID0
       isync
       rlwinm	r4, r3, 0, 18, 15       /* r4 has ICE and DCE bits cleared */
       sync
       isync
       mtspr	HID0, r4                /* HID0 = r4 */
       isync

      /* Get cpu type */
       mfspr     r28, PVR              /* Get cpu type */
       rlwinm    r28, r28, 16, 16, 31  /* r28 should have 16 bits of CPU type */

      /* invalidate the MPU's data/instruction caches */

       lis       r3, 0x0                    /* clear r3 */
       cmpli     0, 0, r28, CPU_TYPE_750
       beq       CPU_IS_750
       cmpli     0, 0, r28, CPU_TYPE_750FX
       beq       CPU_IS_750
       cmpli     0, 0, r28, CPU_TYPE_7400
       beq       CPU_IS_750
       cmpli     0, 0, r28, CPU_TYPE_7410
       beq       CPU_IS_7410
       cmpli     0, 0, r28, CPU_TYPE_7450
       beq       CPU_IS_750
       cmpli     0, 0, r28, CPU_TYPE_7455
       beq       CPU_IS_750
       cmpli     0, 0, r28, CPU_TYPE_7447
       beq       CPU_IS_750
	 cmpli     0, 0, r28, CPU_TYPE_7447A
       beq       CPU_IS_750
       b         CPU_IS_750

CPU_IS_7410:
       andi.     r3, r3, 0
       mtspr     L2PMCR, r3       /* 7410 bug, L2PMCR not cleared on reset */

CPU_IS_750:

/* 
* Since the EPROM is an 8bit device, it can't be cached by the CPU 
* using the GT, therefore the _PPC_HID0_ICE(0x8000) was not set 
* here as it was set orignaly.
*/
      /* Handle 750, 7400, 7410, 7450. 7455 */
       mfspr   r3,HID0

       li      r4,0x0800       /* Setup bit pattern for ICE/ICFI */
       or      r3,r4,r3
       isync
       mtspr     HID0,r3         /* Set ICE/ICFI */
       isync

       li      r4,0x0800       /* Setup bit pattern for ICFI */
       andc    r3,r3,r4
       isync
       mtspr     HID0,r3         /* Clear ICFI (bit 16) */
       isync

       li      r4,0x2000       /* Setup bit pattern for ILOCK */
       andc    r3,r3,r4
       isync
       mtspr     HID0,r3         /* Clear ILOCK (bit 18) */
       isync
       sync

       /* In case of MPC7450/7455 */
       mfspr     r28, PVR              /* Get cpu type */                       
       rlwinm    r28, r28, 16, 16, 31  /*r28 should have 16 bits of CPU type */
       cmpli     0, 0, r28, CPU_TYPE_7450
       beq       init745xHid0
       cmpli     0, 0, r28, CPU_TYPE_7455
       beq       init745xHid0
       b         continue1

init745xHid0:       
       /* Enable MPC745x time base */
       mfspr     r3,HID0
       oris      r3,r3, 0x0400  /* Enable bit 5 TBEN                   */
       oris      r3,r3, 0x0010  /* Set DPM - Dynamic Power Management  */
       isync                    /* For safty                           */
       mtspr     HID0,r3                 

       /* Enable machine check input pin (EMCP) */
       mfspr     r3,HID1
       oris      r3,r3, 0x8000           /* Enable bit 0 EMCP */
       isync                             /* For safty */
       mtspr     HID1,r3

continue1:
       sync
       cmpi      0, 0, r11, BOOT_COLD    /* Check for cold/warm boot */
       bne       startCSetup

      /* Change GT Internal Register Base Address */
       lis       r9, 0x1400                 /* Wake up Internal Base Address  */
       ori       r9, r9, INTERNAL_SPACE_BASE_ADDR /* Remap register offset    */
       lwbrx     r6, 0, (r9)	            /* Read old  data from reg. */
       andi.	 r6, r6, 0x0000  	    /* clear 16 lower bits      */
       ori	 r6, r6, INTERNAL_BASE      /* data should be in bits 19-0    */
       stwbrx    r6, 0 ,(r9)                /* Store new Internal Base Addres */
       sync

       /* Note: Ensure that r15 is never used elsewhere */
       addis   r14,0,romInit@h
       ori     r14,r14,romInit@l
       addis   r15,0, ROM_TEXT_ADRS@h
       ori     r15,r15,ROM_TEXT_ADRS@l
       sub     r15, r15, r14         /* r15 = ROM_TEXT_ADRS - romInit */

#ifdef PMC280_DUAL_CPU
       /*
        * Check if this code is being executed on CPU0 or CPU1.
        * CPU0 is designated to be the master CPU responsible for 
        * initializing the system controller.
        */
        li      r4, 0x0
        lis     r3, INTERNAL_BASE
        ori     r3, r3, SMP_WHO_AM_I
        lwbrx   r5,0,(r3)
        sync

        cmplw   r5,r4 
        bne     _cpu_1
#endif /* PMC280_DUAL_CPU */

#ifdef PMC280_DUAL_CPU
       /* initialize the stack pointer for CPU0 */
        LOADPTR (sp, STACK_ADRS)
#endif /* PMC280_DUAL_CPU */

#ifdef PMC280_PCI_BOOT
      
       addis   r5, r0, INTERNAL_BASE
       ori     r5, r5, 0xF104
       addis   r6, r0, 0x0000
       ori     r6, r6, 0x0200
       stwbrx  r6, 0, (r5)                     // Store
       sync
#endif /* for PCI Boot GPP 9 is driven High */

#ifdef PMC280_DEBUG_LED
       /* Configure GPP[23] as Output */
       addis   r5, r0, INTERNAL_BASE
       ori     r5, r5, 0xF100
       addis   r6, r0, 0x0080
       ori     r6, r6, 0x0000
       stwbrx  r6, 0, (r5)                     // Store
       sync
#endif /* PMC280_DEBUG_LED */

       bl       _initialize_dbg_uart        /* Initialize MPSC 1 */
        
#ifdef PMC280_DUAL_CPU
       bl      waitPrint0
#else
       bl      waitPrint
#endif

#ifdef PMC280_DEBUG_UART
       bl      serialInitCompletePrint
#endif /* PMC280_DEBUG_UART */

 /* DDR configuration routine. Preform only in cold boot */ 
        bl      _ddrAutoConfig
        sync

#ifdef PMC280_DEBUG_UART
       bl      DdrInitCompletePrint
#endif /* PMC280_DEBUG_UART */

       /*
        * Use floating point registers to clean up first 128 bytes of SDRAM.
        * Note that this operation has to be done before ECC is enabled and
        * is implemented before ddrAutoConfig (which is where ECC is enabled)
        */

       /* Set MPU/MSR to a known state, Turn on FP */
        LOADPTR (r3, _PPC_MSR_FP)
        sync
        mtmsr 	r3
        isync

        lis     r3, 0x0000      /* Low (32-bit) Data  */
        ori     r3, r3, 0x0001
        lis     r4, 0x0000      /* High (32-bit) Data  */
        ori     r4, r4, 0x0000
        li      r5, 0x0000      /* Address */ 
        stw     r3, 0(r5)
        stw     r4, 4(r5)
        lfd     fr3, 0(r5)
        fsub    fr3, fr3, fr3
        sync

        /*
         * Turn on ECC here before you actually do the scrubbing.
         */
        lis      r5, INTERNAL_BASE
        ori      r5,r5, SDRAM_CONFIG /* SDRAM config reg */
        lwbrx    r6,0,(r5)
        oris     r6,r6,0x0004 /* bit 18 */
        stwbrx   r6,0,(r5)    /* store */

#ifdef PMC280_DEBUG_DRAM
        /* Print all DDR controller registers */
        lis     r4,INTERNAL_BASE
        ori     r4,r4,SDRAM_CONFIG
        lwbrx   r3,0,(r4)
        li      r4,28
        sync
        bl      PrintRegister

        li      r3, 0x0D
        bl      _dbg_transmit0
        li      r3, 0x0A
        bl      _dbg_transmit0

        lis     r4,INTERNAL_BASE
        ori     r4,r4,D_UNIT_CONTROL_LOW
        lwbrx   r3,0,(r4)
        li      r4,28
        sync
        bl      PrintRegister
        li      r3, 0x0D
        bl      _dbg_transmit0
        li      r3, 0x0A
        bl      _dbg_transmit0

        lis     r4,INTERNAL_BASE
        ori     r4,r4,D_UNIT_CONTROL_HIGH
        lwbrx   r3,0,(r4)
        li      r4,28
        sync
        bl      PrintRegister

        li      r3, 0x0D
        bl      _dbg_transmit0
        li      r3, 0x0A
        bl      _dbg_transmit0

        lis     r4,INTERNAL_BASE
        ori     r4,r4,SDRAM_TIMING_CONTROL_LOW
        lwbrx   r3,0,(r4)
        li      r4,28
        sync
        bl      PrintRegister

        li      r3, 0x0D
        bl      _dbg_transmit0
        li      r3, 0x0A
        bl      _dbg_transmit0

        lis     r4,INTERNAL_BASE
        ori     r4,r4,SDRAM_TIMING_CONTROL_HIGH
        lwbrx   r3,0,(r4)
        li      r4,28
        sync
        bl      PrintRegister

        li      r3, 0x0D
        bl      _dbg_transmit0
        li      r3, 0x0A
        bl      _dbg_transmit0

        lis     r4,INTERNAL_BASE
        ori     r4,r4,SDRAM_ADDR_CONTROL
        lwbrx   r3,0,(r4)
        li      r4,28
        sync
        bl      PrintRegister

        li      r3, 0x0D
        bl      _dbg_transmit0
        li      r3, 0x0A
        bl      _dbg_transmit0

        lis     r4,INTERNAL_BASE
        ori     r4,r4,SDRAM_OPEN_PAGES_CONTROL
        lwbrx   r3,0,(r4)
        li      r4,28
        sync
        bl      PrintRegister

        li      r3, 0x0D
        bl      _dbg_transmit0
        li      r3, 0x0A
        bl      _dbg_transmit0

        lis     r4,INTERNAL_BASE
        ori     r4,r4,SDRAM_OPERATION
        lwbrx   r3,0,(r4)
        li      r4,28
        sync
        bl      PrintRegister

        li      r3, 0x0D
        bl      _dbg_transmit0
        li      r3, 0x0A
        bl      _dbg_transmit0

        lis     r4,INTERNAL_BASE
        ori     r4,r4,SDRAM_MODE
        lwbrx   r3,0,(r4)
        sync
        li      r4,28

        bl      PrintRegister
        li      r3, 0x0D
        bl      _dbg_transmit0
        li      r3, 0x0A
        bl      _dbg_transmit0

        lis     r4,INTERNAL_BASE
        ori     r4,r4,EXTENDED_DRAM_MODE
        lwbrx   r3,0,(r4)
        li      r4,28
        sync
        bl      PrintRegister

        li      r3, 0x0D
        bl      _dbg_transmit0
        li      r3, 0x0A
        bl      _dbg_transmit0

        lis     r4,INTERNAL_BASE
        ori     r4,r4,SDRAM_CROSS_BAR_CONTROL_LOW
        lwbrx   r3,0,(r4)
        li      r4,28
        sync
        bl      PrintRegister

        li      r3, 0x0D
        bl      _dbg_transmit0
        li      r3, 0x0A
        bl      _dbg_transmit0

        lis     r4,INTERNAL_BASE
        ori     r4,r4,SDRAM_CROSS_BAR_CONTROL_HIGH
        lwbrx   r3,0,(r4)
        li      r4,28
        sync
        bl      PrintRegister


        li      r3, 0x0D
        bl      _dbg_transmit0
        li      r3, 0x0A
        bl      _dbg_transmit0

        lis     r4,INTERNAL_BASE
        ori     r4,r4,SDRAM_CROSS_BAR_TIMEOUT
        lwbrx   r3,0,(r4)
        li      r4,28
        sync
        bl      PrintRegister

        li      r3, 0x0D
        bl      _dbg_transmit0
        li      r3, 0x0A
        bl      _dbg_transmit0

        lis     r4,INTERNAL_BASE
        ori     r4,r4,SDRAM_ADDR_CTRL_PADS_CALIBRATION
        lwbrx   r3,0,(r4)
        li      r4,28
        sync
        bl      PrintRegister

        li      r3, 0x0D
        bl      _dbg_transmit0
        li      r3, 0x0A
        bl      _dbg_transmit0

        lis     r4,INTERNAL_BASE
        ori     r4,r4,SDRAM_DATA_PADS_CALIBRATION
        lwbrx   r3,0,(r4)
        li      r4,28
        sync
        bl      PrintRegister

        li      r3, 0x0D
        bl      _dbg_transmit0
        li      r3, 0x0A
        bl      _dbg_transmit0

        lis     r4,INTERNAL_BASE
        ori     r4,r4, 0x03C4
        lwbrx   r3,0,(r4)
        li      r4,28
        sync
        bl      PrintRegister

        li      r3, 0x0D
        bl      _dbg_transmit0
        li      r3, 0x0A
        bl      _dbg_transmit0

        lis     r4,INTERNAL_BASE
        ori     r4,r4, 0x03D4
        lwbrx   r3,0,(r4)
        li      r4,28
        sync
        bl      PrintRegister

        li      r3, 0x0D
        bl      _dbg_transmit0
        li      r3, 0x0A
        bl      _dbg_transmit0

        li      r3, 0x0D
        bl      _dbg_transmit0
        li      r3, 0x0A
        bl      _dbg_transmit0

        lis     r4,INTERNAL_BASE
        ori     r4,r4, CS_0_SIZE
        lwbrx   r3,0,(r4)
        li      r4,28
        sync
        bl      PrintRegister

        li      r3, 0x0D
        bl      _dbg_transmit0
        li      r3, 0x0A
        bl      _dbg_transmit0

        lis     r4,INTERNAL_BASE
        ori     r4,r4, CS_0_BASE_ADDR
        lwbrx   r3,0,(r4)
        li      r4,28
        sync
        bl      PrintRegister

        li      r3, 0x0D
        bl      _dbg_transmit0
        li      r3, 0x0A
        bl      _dbg_transmit0

        lis     r4,INTERNAL_BASE
        ori     r4,r4, CS_1_BASE_ADDR
        lwbrx   r3,0,(r4)
        li      r4,28
        sync
        bl      PrintRegister

        li      r3, 0x0D
        bl      _dbg_transmit0
        li      r3, 0x0A
        bl      _dbg_transmit0

        lis     r4,INTERNAL_BASE
        ori     r4,r4, BASE_ADDR_ENABLE
        lwbrx   r3,0,(r4)
        li      r4,28
        sync
        bl      PrintRegister

        li      r3, 0x0D
        bl      _dbg_transmit0
        li      r3, 0x0A
        bl      _dbg_transmit0

        lis     r4,INTERNAL_BASE
        ori     r4,r4, BANK0_SIZE_REG
        lwbrx   r3,0,(r4)
        li      r4,28
        sync
        bl      PrintRegister

        li      r3, 0x0D
        bl      _dbg_transmit0
        li      r3, 0x0A
        bl      _dbg_transmit0

        lis     r4,INTERNAL_BASE
        ori     r4,r4, BANK1_SIZE_REG
        lwbrx   r3,0,(r4)
        li      r4,28
        sync
        bl      PrintRegister

        li      r3, 0x0D
        bl      _dbg_transmit0
        li      r3, 0x0A
        bl      _dbg_transmit0

        lis     r4,INTERNAL_BASE
        ori     r4,r4, CS_1_SIZE
        lwbrx   r3,0,(r4)
        li      r4,28
        sync
        bl      PrintRegister

        li      r3, 0x0D
        bl      _dbg_transmit0
        li      r3, 0x0A
        bl      _dbg_transmit0

        mr      r3,r26
        li      r4,28
        sync
        bl      PrintRegister

        li      r3, 0x0D
        bl      _dbg_transmit0
        li      r3, 0x0A
        bl      _dbg_transmit0
#endif /* PMC280_DEBUG_DRAM */

	/* GT init. Preform only in cold boot */
       lis       r9, INTERNAL_BASE  /* Load r9 with default GT register base */
       lis       r6,0xfff0          /* Boot Device base address         */
       ori       r6,r6,reginittab@l /* Load r6 with init table (itab.c) */

gtInitLoop:
       sync
       lwz       r7,0(r6)        /* Load register offset   */
       lwz       r8,4(r6)        /* Load register data     */
       cmpl      0,0,r7,r8       /* End of table ?? (r7 == r8) = 0 */
       beq       scrubMem

       or        r7,r9,r7        /* Get register address   */
       stwbrx    r8, 0, (r7)
       addi      r6,r6,8         /* Next line in table     */
       b         gtInitLoop

scrubMem:
#ifdef  PMC280_DUAL_CPU  	/* Aravind 26/05/06*/


       lis      r6,0x0000
       ori      r6,r6,0x0e00
       lis      r5, INTERNAL_BASE 
       ori      r5, r5,DMA_BASE_ADDR_REG0
       stwbrx   r6,0,(r5)    /* store */

       lis      r6,0x1fff
       ori      r6,r6,0x0000
       lis      r5, INTERNAL_BASE 
       ori      r5, r5,DMA_SIZE_REG0
       stwbrx   r6,0,(r5)    /* store */

       lis      r6,0x0000
       ori      r6,r6,0x00fc
       lis      r5, INTERNAL_BASE
       ori      r5, r5,DMA_BASE_ADDR_ENABLE_REG
       stwbrx   r6,0,(r5)    /* store */

       lis      r6,0x0000
       ori      r6,r6,0xffff
       lis      r5, INTERNAL_BASE
       ori      r5, r5,DMA_CHANNEL1_ACCESS_PROTECTION_REG
       stwbrx   r6,0,(r5)    /* store */

       /* Set the channel 2 now */

       lis      r6,0x2000
       ori      r6,r6,0x0d00
       lis      r5, INTERNAL_BASE
       ori      r5, r5,DMA_BASE_ADDR_REG1
       stwbrx   r6,0,(r5)    /* store */

       lis      r6,0x1fff
       ori      r6,r6,0x0000
       lis      r5, INTERNAL_BASE 
       ori      r5, r5,DMA_SIZE_REG1
       stwbrx   r6,0,(r5)    /* store */

       /* Now scrub the memory */
 
       mr      r3,r26
 
       /* interations = length >> 23 (because each loop of 8MB) */
       /* interations = length >> 24 (because each loop of 16MB) */
 
       mr       r16, r3
       rotrwi   r16, r16, 23
       mtctr    r16
 
       /* r17 = 8MB */
       /* r17 = 16MB */
 
       lis      r17, MEMSIZE_8MB@h
       ori      r17,r17,0x0
 
       li       r18, 0x0
 
       /* r3 = Destination address */
 
       lis      r3,0x0
       ori      r3,r3,0x0

       /* Note : fr3 is set to 0x0 */ 
       lis      r5,0x0000
       ori      r5,r5,0x0000
       stfd     fr3,0(r5)
       sync
 
       lis      r5,0x0000
       ori      r5,r5,0x0008
       stfd     fr3,0(r5)
       sync
 
       lis      r5,0x0000
       ori      r5,r5,0x0010
       stfd     fr3,0(r5)
       sync
 
       lis      r5,0x0000
       ori      r5,r5,0x0018
       stfd     fr3,0(r5)
       sync
 
       lis      r5,0x0000
       ori      r5,r5,0x0020
       stfd     fr3,0(r5)
       sync
 
       lis      r5,0x0000
       ori      r5,r5,0x0028
       stfd     fr3,0(r5)
       sync
 
       lis      r5,0x0000
       ori      r5,r5,0x0030
       stfd     fr3,0(r5)
       sync
 
       lis      r5,0x0000
       ori      r5,r5,0x0038
       stfd     fr3,0(r5)
       sync
 
       lis      r5,0x0000
       ori      r5,r5,0x0040
       stfd     fr3,0(r5)
       sync
 
       lis      r5,0x0000
       ori      r5,r5,0x0048
       stfd     fr3,0(r5)
       sync
 
       lis      r5,0x0000
       ori      r5,r5,0x0050
       stfd     fr3,0(r5)
       sync
 
       lis      r5,0x0000
       ori      r5,r5,0x0058
       stfd     fr3,0(r5)
       sync
 
       lis      r5,0x0000
       ori      r5,r5,0x0060
       stfd     fr3,0(r5)
       sync
 
       lis      r5,0x0000
       ori      r5,r5,0x0068
       stfd     fr3,0(r5)
       sync
 
       lis      r5,0x0000
       ori      r5,r5,0x0070
       stfd     fr3,0(r5)
       sync
 
       lis      r5,0x0000
       ori      r5,r5,0x0078
       stfd     fr3,0(r5)
       sync

      
_idmaloop:
 

       lis      r5,INTERNAL_BASE
       ori      r5,r5,DMA_CHANNEL1_CONTROL

waitDmaEngine1:
       lis      r4,0x0
       ori      r4,r4,0x4000
       lwbrx    r6,0,(r5)
       and.     r4,r4,r6
       bne      waitDmaEngine1

       lis      r4,0x0
       ori      r4,r4,0x0

       lis      r6,MEMSIZE_8MB@h           /* r6 = Count       */
       ori      r6,r6,0x0000


       /* DMA Source Address */
       lis      r5,INTERNAL_BASE
       ori      r5,r5,DMA_CHANNEL1_SOURCE_ADDR
       stwbrx   r4,0,(r5)

      /* DMA Destination Address */
       lis      r5,INTERNAL_BASE
       ori      r5,r5,DMA_CHANNEL1_DESTINATION_ADDR
       stwbrx   r3,0,(r5)

       /* DMA Count */
       lis      r5,INTERNAL_BASE
       ori      r5,r5,DMA_CHANNEL1_BYTE_COUNT
       stwbrx   r6,0,(r5)

       lis      r5,INTERNAL_BASE
       ori      r5,r5,DMA_CHANNEL1_CONTROL
       lis      r3,0x8000
       ori      r3,r3,0x1b0c    /*0x1b0c*/   /*0x1a08*/
       stwbrx   r3,0,(r5)

       sync
       isync

       bdz startCSetup 

       add     r18, r18, r17
       mr      r3, r18

    
       lis      r5,INTERNAL_BASE
       ori      r5,r5,DMA_CHANNEL3_CONTROL

waitDmaEngine3:
       lis      r4,0x0
       ori      r4,r4,0x4000
       lwbrx    r6,0,(r5)
       and.     r4,r4,r6
       bne      waitDmaEngine3

       lis      r4,0x0
       ori      r4,r4,0x0

       lis      r6,MEMSIZE_8MB@h           /* r6 = Count       */
       ori      r6,r6,0x0000

      /* DMA Source Address */
       lis      r5,INTERNAL_BASE
       ori      r5,r5,DMA_CHANNEL3_SOURCE_ADDR
       stwbrx   r4,0,(r5)

      /* DMA Destination Address */
       lis      r5,INTERNAL_BASE
       ori      r5,r5,DMA_CHANNEL3_DESTINATION_ADDR
       stwbrx   r3,0,(r5)

       /* DMA Count */
       lis      r5,INTERNAL_BASE
       ori      r5,r5,DMA_CHANNEL3_BYTE_COUNT
       stwbrx   r6,0,(r5)

       lis      r5,INTERNAL_BASE
       ori      r5,r5,DMA_CHANNEL3_CONTROL
       lis      r3,0x8000
       ori      r3,r3,0x1b0c    /*0x1b0c*/   /*0x1a08*/
       stwbrx   r3,0,(r5)

       sync
       isync

/*
chanAct: 
       lis      r4,0x0
       ori      r4,r4,0x4000
       lwbrx    r6,0,(r5)
       and.     r4,r4,r6
       bne      chanAct
*/
#ifdef PMC280_DEBUG_UART
       /* This will tell us if there was any scrubbing error */
       lis      r8,INTERNAL_BASE
       ori      r8,r8, SDRAM_ECC_ERROR_COUNTER  /* Error Address */
       lwbrx    r3,0,(r8)
       li       r30,28
       bl       _print_reg
#endif /* PMC280_DEBUG_UART */
 
        sync
 
        /* Increment destination address by 8MB, r18 <- r18 + 8MB */
        /* Increment destination address by 16MB, r18 <- r18 + 16MB */
 
        add     r18, r18, r17
        mr      r3, r18
        bdnz    _idmaloop



#else  /* Aravind 26/05/06*/

 /* Aravind 26/05/06*/
	lis	r5, INTERNAL_BASE
	ori	r5,r5, CS_0_BASE_ADDR
	lwbrx	r6,0,(r5)
	slwi	r6, r6, 16

	ori     r6,r6,0x0e00
	lis     r5, INTERNAL_BASE
	ori     r5, r5, DMA_BASE_ADDR_REG0
	stwbrx  r6, 0, (r5)

        lis     r5, INTERNAL_BASE
        ori     r5,r5, CS_0_SIZE
        lwbrx   r6,0,(r5)
        slwi    r6, r6, 16

        lis     r5, INTERNAL_BASE
        ori     r5, r5, DMA_SIZE_REG0
        stwbrx  r6, 0, (r5)


        

        lis     r5, INTERNAL_BASE
        ori     r5, r5, BASE_ADDR_ENABLE
        lwbrx   r6,0,(r5)

        lis     r7,0x0000
        ori     r7,r7,0x00FF
        and     r6, r6, r7
        ori     r6, r6, 0x00F0

        lis     r5, INTERNAL_BASE
        ori     r5, r5, DMA_BASE_ADDR_ENABLE_REG
        stwbrx  r6, 0, (r5)

	lis	r6,0x0000
	ori	r6,r6,0xFFFF
	lis	r5, INTERNAL_BASE 	
	ori	r5, r5, DMA_CHANNEL1_ACCESS_PROTECTION_REG
	stwbrx	r6, 0, (r5)

	xor	r4, r4, r4
	xor	r5, r5, r5
	xor	r3, r3, r3
	xor	r13, r13, r13
	ori	r5, r5, 0x0080

	fsub	f3, f3, f3

f_loop:
	stfd	f3, 0(r4)
	addi	r4, r4, 8

	cmp   	0,0,r4, r5
	bne	f_loop

	xor	r27,r27,r27
	lis	r27, MEMSIZE_8MB@h
	xor	r18, r18, r18


	
	mr      r6, r26		/*r26 contains total size*/
	srwi    r6, r6, 23	/* Size >> 23 (each loop of 8MB)        */
	mtctr   r6
	
_idma_loop:
	
	xor	r4, r4, r4


	xor	r6, r6, r6	
	lis	r6, MEMSIZE_8MB@h

	
	/* DMA Source Address	*/
	lis	r5, INTERNAL_BASE
	ori	r5, r5, DMA_CHANNEL0_SOURCE_ADDR
	stwbrx	r4, 0, (r5)


	/* DMA Destination Address	*/
	lis	r5, INTERNAL_BASE
	ori	r5, r5, DMA_CHANNEL0_DESTINATION_ADDR


	stwbrx	r3, 0, (r5)

	
	/* DMA Count			*/


	lis	r5, INTERNAL_BASE
	ori	r5, r5, DMA_CHANNEL0_BYTE_COUNT
	stwbrx	r6, 0, (r5)

	lis	r5, INTERNAL_BASE
	ori	r5, r5, DMA_CHANNEL0_CONTROL
	lis	r6, 0x8000
	ori	r6, r6, 0x1b0c				
	stwbrx	r6, 0, (r5)
	
	sync
	isync
	
channel_activate:
	xor	r4, r4, r4
	ori	r4, r4, 0x4000
	lwbrx	r6, 0, (r5)
	and.	r4, r4, r6
	bne	channel_activate

	sync


	/* Increment destination address by 8MB, r18 <- r18 + 8MB */

	add	r18, r18, r27

	mr	r3, r18

	bdnz	_idma_loop	

	sync
	isync




#endif	/* Aravind 26/05/06*/

        
startCSetup:

#ifdef PMC280_DUAL_CPU
       /* Set up MMU tables for CPU0 */
        LOADPTR (r3, earlysysBatDescCPU0)
        LOADPTR (r4, romInit)
        LOADPTR (r5, ROM_TEXT_ADRS)

        sub             r3, r3, r4
        add             r3, r3, r5
        /* r3 = earlysysBatDescCPU0 - romInit + ROM_TEXT_ADRS */

        /* r3 points to head of list of BAT values */

        /* initialize the BAT registers (SPRs 528 - 543) */
        lwz     r5,0(r3)
        isync
        mtspr   528,r5                  /* SPR 528 (IBAT0U) */
        isync

        lwzu    r5,4(r3)
        isync
        mtspr   529,r5                  /* SPR 529 (IBAT0L) */
        isync

        lwzu    r5,4(r3)
        isync
        mtspr   530,r5                  /* SPR 530 (IBAT1U) */
        isync

        lwzu    r5,4(r3)
        isync
        mtspr   531,r5                  /* SPR 531 (IBAT1L) */
        isync

        lwzu    r5,4(r3)
        isync
        mtspr   532,r5                  /* SPR 532 (IBAT2U) */
        isync

        lwzu    r5,4(r3)
        isync
        mtspr   533,r5                  /* SPR 533 (IBAT2L) */
        isync

        lwzu    r5,4(r3)
        isync
        mtspr   534,r5                  /* SPR 534 (IBAT3U) */
        isync

        lwzu    r5,4(r3)
        isync
        mtspr   535,r5                  /* SPR 535 (IBAT3L) */
        isync

#ifdef PMC280_MMU_SUPPORT_FOR_MPC7447
        /* initialize the BAT registers (SPRs 560 - 567) */
        lwzu    r5,4(r3)
        isync
        mtspr   560,r5                  /* SPR 560 (IBAT4U) */
        isync

        lwzu    r5,4(r3)
        isync
        mtspr   561,r5                  /* SPR 561 (IBAT4L) */
        isync

        lwzu    r5,4(r3)
        isync
        mtspr   562,r5                  /* SPR 562 (IBAT5U) */
        isync

        lwzu    r5,4(r3)
        isync
        mtspr   563,r5                  /* SPR 563 (IBAT5L) */
        isync

        lwzu    r5,4(r3)
        isync
        mtspr   564,r5                  /* SPR 564 (IBAT6U) */
        isync

        lwzu    r5,4(r3)
        isync
        mtspr   565,r5                  /* SPR 565 (IBAT6L) */
        isync

        lwzu    r5,4(r3)
        isync
        mtspr   566,r5                  /* SPR 566 (IBAT7U) */
        isync

        lwzu    r5,4(r3)
        isync
        mtspr   567,r5                  /* SPR 567 (IBAT7L) */
        isync
#endif /* PMC280_MMU_SUPPORT_FOR_MPC7447 */

        lwzu    r5,4(r3)
        isync
        mtspr   536,r5                  /* SPR 536 (DBAT0U) */
        isync

        lwzu    r5,4(r3)
        isync
        mtspr   537,r5                  /* SPR 537 (DBAT0L) */
        isync

        lwzu    r5,4(r3)
        isync
        mtspr   538,r5                  /* SPR 538 (DBAT1U) */
        isync

        lwzu    r5,4(r3)
        isync
        mtspr   539,r5                  /* SPR 539 (DBAT1L) */
        isync

        lwzu    r5,4(r3)
        isync
        mtspr   540,r5                  /* SPR 540 (DBAT2U) */
        isync

        lwzu    r5,4(r3)
        isync
        mtspr   541,r5                  /* SPR 541 (DBAT2L) */
        isync

        lwzu    r5,4(r3)
        isync
        mtspr   542,r5                  /* SPR 542 (DBAT3U) */
        isync

        lwzu    r5,4(r3)
        isync
        mtspr   543,r5                  /* SPR 543 (DBAT3L) */
        isync

#ifdef PMC280_MMU_SUPPORT_FOR_MPC7447
        lwzu    r5,4(r3)
        isync
        mtspr   568,r5                  /* SPR 568 (DBAT4U) */
        isync

        lwzu    r5,4(r3)
        isync
        mtspr   569,r5                  /* SPR 569 (DBAT4L) */
        isync

        lwzu    r5,4(r3)
        isync
        mtspr   570,r5                  /* SPR 570 (DBAT5U) */
        isync

        lwzu    r5,4(r3)
        isync
        mtspr   571,r5                  /* SPR 571 (DBAT5L) */
        isync

        lwzu    r5,4(r3)
        isync
        mtspr   572,r5                  /* SPR 572 (DBAT6U) */
        isync

        lwzu    r5,4(r3)
        isync
        mtspr   573,r5                  /* SPR 573 (DBAT6L) */
        isync

        lwzu    r5,4(r3)
        isync
        mtspr   574,r5                  /* SPR 574 (DBAT7U) */
        isync

        lwzu    r5,4(r3)
        isync
        mtspr   575,r5                  /* SPR 575 (DBAT7L) */
        isync
#endif /* PMC280_MMU_SUPPORT_FOR_MPC7447 */

        /* invalidate entries within both TLBs */
        li      r4,128
        xor     r3,r3,r3                /* r3 = 0    */
        mtctr   r4                      /* CTR = 32  */

        isync                           /* context sync req'd before tlbie   */
earlyvxpbi0:
        tlbie   r3
        addi    r3,r3,0x1000            /* increment bits 15-19              */
        bdnz    earlyvxpbi0             /* decrement CTR, branch if CTR != 0 */
        sync                            /* sync instr req'd after tlbie */

#ifdef PMC280_DUAL_CPU
#else
        /* Enable MMU for CPU0 */

        mfmsr   r4
        /* load the MSR value in P1 */
        ori  r4, r4, _PPC_MSR_IR|_PPC_MSR_DR   /* set the IR bit of the MSR */
        mtmsr   r4
        /* Enable the instrunction mmu */
        isync
        /* instruction synchronization */
        sync
        /* synchronization */
#endif /* PMC280_DUAL_CPU */

#ifdef PMC280_DEBUG_UART
        bl      BATInitCompletePrint0
#endif /* PMC280_DEBUG_UART */

#ifdef PMC280_DEBUG_UART
        bl      CentryPrint0
#endif /* PMC280_DEBUG_UART */

        b       dual_cpu_continue
#endif /* PMC280_DUAL_CPU */

#ifdef PMC280_DUAL_CPU
_cpu_1:

       /* initialize the stack pointer for CPU1 
        LOADPTR (sp, (STACK_ADRS + PHY_CPU1_MEM_BOT))
        */

        addis   r5, r0, INTERNAL_BASE
        ori     r5, r5, CPU1_STACK_ADRS_REG
        lwbrx   sp, 0, (r5)


#ifdef PMC280_DUAL_CPU
       bl      waitPrint1
#else
       bl      waitPrint
#endif

       /* Set up MMU tables for CPU1 */
        LOADPTR (r3, earlysysBatDescCPU1)
        LOADPTR (r4, romInit)
        LOADPTR (r5, ROM_TEXT_ADRS)

        sub             r3, r3, r4
        add             r3, r3, r5
        /* r3 = earlysysBatDescCPU1 - romInit + ROM_TEXT_ADRS */

        /* r3 points to head of list of BAT values */

        /* initialize the BAT registers (SPRs 528 - 543) */
        lwz     r5,0(r3)
        isync
        mtspr   528,r5                  /* SPR 528 (IBAT0U) */
        isync

        lwzu    r5,4(r3)
        isync
        mtspr   529,r5                  /* SPR 529 (IBAT0L) */
        isync

        lwzu    r5,4(r3)
        isync
        mtspr   530,r5                  /* SPR 530 (IBAT1U) */
        isync

        lwzu    r5,4(r3)
        isync
        mtspr   531,r5                  /* SPR 531 (IBAT1L) */
        isync

        lwzu    r5,4(r3)
        isync
        mtspr   532,r5                  /* SPR 532 (IBAT2U) */
        isync

        lwzu    r5,4(r3)
        isync
        mtspr   533,r5                  /* SPR 533 (IBAT2L) */
        isync

        lwzu    r5,4(r3)
        isync
        mtspr   534,r5                  /* SPR 534 (IBAT3U) */
        isync

        lwzu    r5,4(r3)
        isync
        mtspr   535,r5                  /* SPR 535 (IBAT3L) */
        isync

#ifdef PMC280_MMU_SUPPORT_FOR_MPC7447
        /* initialize the BAT registers (SPRs 560 - 567) */
        lwzu    r5,4(r3)
        isync
        mtspr   560,r5                  /* SPR 560 (IBAT4U) */
        isync

        lwzu    r5,4(r3)
        isync
        mtspr   561,r5                  /* SPR 561 (IBAT4L) */
        isync

        lwzu    r5,4(r3)
        isync
        mtspr   562,r5                  /* SPR 562 (IBAT5U) */
        isync

        lwzu    r5,4(r3)
        isync
        mtspr   563,r5                  /* SPR 563 (IBAT5L) */
        isync

        lwzu    r5,4(r3)
        isync
        mtspr   564,r5                  /* SPR 564 (IBAT6U) */
        isync

        lwzu    r5,4(r3)
        isync
        mtspr   565,r5                  /* SPR 565 (IBAT6L) */
        isync

        lwzu    r5,4(r3)
        isync
        mtspr   566,r5                  /* SPR 566 (IBAT7U) */
        isync

        lwzu    r5,4(r3)
        isync
        mtspr   567,r5                  /* SPR 567 (IBAT7L) */
        isync
#endif /* PMC280_MMU_SUPPORT_FOR_MPC7447 */

        lwzu    r5,4(r3)
        isync
        mtspr   536,r5                  /* SPR 536 (DBAT0U) */
        isync

        lwzu    r5,4(r3)
        isync
        mtspr   537,r5                  /* SPR 537 (DBAT0L) */
        isync

        lwzu    r5,4(r3)
        isync
        mtspr   538,r5                  /* SPR 538 (DBAT1U) */
        isync

        lwzu    r5,4(r3)
        isync
        mtspr   539,r5                  /* SPR 539 (DBAT1L) */
        isync

        lwzu    r5,4(r3)
        isync
        mtspr   540,r5                  /* SPR 540 (DBAT2U) */
        isync

        lwzu    r5,4(r3)
        isync
        mtspr   541,r5                  /* SPR 541 (DBAT2L) */
        isync

        lwzu    r5,4(r3)
        isync
        mtspr   542,r5                  /* SPR 542 (DBAT3U) */
        isync

        lwzu    r5,4(r3)
        isync
        mtspr   543,r5                  /* SPR 543 (DBAT3L) */
        isync

#ifdef PMC280_MMU_SUPPORT_FOR_MPC7447
        lwzu    r5,4(r3)
        isync
        mtspr   568,r5                  /* SPR 568 (DBAT4U) */
        isync

        lwzu    r5,4(r3)
        isync
        mtspr   569,r5                  /* SPR 569 (DBAT4L) */
        isync

        lwzu    r5,4(r3)
        isync
        mtspr   570,r5                  /* SPR 570 (DBAT5U) */
        isync

        lwzu    r5,4(r3)
        isync
        mtspr   571,r5                  /* SPR 571 (DBAT5L) */
        isync

        lwzu    r5,4(r3)
        isync
        mtspr   572,r5                  /* SPR 572 (DBAT6U) */
        isync

        lwzu    r5,4(r3)
        isync
        mtspr   573,r5                  /* SPR 573 (DBAT6L) */
        isync

        lwzu    r5,4(r3)
        isync
        mtspr   574,r5                  /* SPR 574 (DBAT7U) */
        isync

        lwzu    r5,4(r3)
        isync
        mtspr   575,r5                  /* SPR 575 (DBAT7L) */
        isync
#endif /* PMC280_MMU_SUPPORT_FOR_MPC7447 */

        /* invalidate entries within both TLBs */
        li      r4,128
        xor     r3,r3,r3                /* r3 = 0    */
        mtctr   r4                      /* CTR = 32  */

        isync                           /* context sync req'd before tlbie   */
earlyvxpbi1:
        tlbie   r3
        addi    r3,r3,0x1000            /* increment bits 15-19              */
        bdnz    earlyvxpbi1             /* decrement CTR, branch if CTR != 0 */
        sync                            /* sync instr req'd after tlbie */

#ifdef PMC280_DEBUG_UART
        bl      BATInitCompletePrint1
#endif /* PMC280_DEBUG_UART */

#ifdef PMC280_DEBUG_UART
        bl	CentryPrint1
#endif /* PMC280_DEBUG_UART */

dual_cpu_continue:

#endif /* PMC280_DUAL_CPU */

       or        r3, r11, r11          /* put startType in r3 (p0) */
       addi      sp, sp, -FRAMEBASESZ  /* save one frame stack     */


       LOADPTR   (r6, romStart)
       LOADPTR   (r7, romInit)
       LOADPTR   (r8, ROM_TEXT_ADRS)

       sub       r6, r6, r7
       add       r6, r6, r8 

       mtlr      r6                  /* romStart - romInit + ROM_TEXT_ADRS */
       blr

_ddrAutoConfig:
/*****************************************************************************/

        /* DDR SDRAM configuration - refresh_count=0x200,
                                     phisical interleaving disable,
                                     virtual interleaving disable,
                                     standard SDRAM */

/*        bl      dfcdlInit
        mtlr    r27 */
                lis     r5, INTERNAL_BASE
                ori     r5, r5, SDRAM_CONFIG         /* 0x1400 */

/*62->22*/      addis   r6, r0, 0x5820
                ori     r6, r6, 0x0400    
                stwbrx  r6, 0, (r5)                  /* Store */
                sync


/* configure SDRAM Open pages */		
		addis   r5, r0, INTERNAL_BASE
		ori     r5, r5, SDRAM_OPEN_PAGES_CONTROL /* 0x1414 */
       		addis   r6, r0, 0x0
       		stwbrx  r6, 0, (r5)
       		sync
 
/* configure SDRAM Control High - bits 7:0 - val 0xf for 1gbit devices and 0x9(default)	for others */
		addis   r5 , r0, INTERNAL_BASE
		ori     r5, r5, SDRAM_TIMING_CONTROL_HIGH
		lwbrx   r6, 0, (r5)
		ori     r6, r6, 0xf
		stwbrx	r6, r0,(r5)		
        	sync 

/*  configure Dunit control Low */
		 addis   r5, r0, INTERNAL_BASE
                ori     r5, r5, D_UNIT_CONTROL_LOW   /* 0x1404 */
                lwbrx   r4, 0, (r5)
                lis     r6, 0x0
                ori     r6, r6, 0x1
                and.    r6, r6, r4
                bne     synchddr1
/*1b -> 3b */   addis   r6, r0, 0x1b11  /*asynchronious mode CL3=0x3c11 CL2.5=0x1b11 */
                ori     r6, r6, 0x07c0           /* also 141c */ /*  0780 to 07c0 */
                b       set1404_1
synchddr1:
                addis   r6, r0, 0x0311                  /*synchronious mode  */
                ori     r6, r6, 0x0000

set1404_1:
                andi.   r4, r4, 0x007f
                or      r4, r4, r6
                stwbrx  r4, 0, (r5)                     /* Store      1404 */
                sync
/* configure SDRAM Timing Low */

        	addis	r5, r0, INTERNAL_BASE
        	ori	r5, r5, SDRAM_TIMING_CONTROL_LOW  /* 0x1408 */            
        	addis	r6, r0, 0x1151                    /* 1261 */
        	ori	r6, r6, 0x1220                    /* 2330 */
        	stwbrx	r6, 0, (r5)                        /* Store  1408 */
        	sync

        mflr    r27
			bl	dfcdlInit 
        mtlr    r27

        addis	r5, r0, INTERNAL_BASE
        ori	r5, r5, SDRAM_OPERATION      /* 0x1418 */
        xor	r6, r6, r6
        ori	r6, r6, 0x5
        stwbrx	r6, r0, (r5)                 /* Store */
        sync

poll_1418:
        xor	r4, r4, r4
        ori	r4, r4, 0x5                   /* is channel active */
        lwbrx	r6, 0, (r5)
	 and.	r4, r4, r6
	 bne	poll_1418

/* precharge all command  step 2*/
		

                lis     r5, INTERNAL_BASE
                ori     r5, r5, SDRAM_OPERATION      /* 0x1418 */
	        xor     r6, r6, r6
		ori     r6, r6, 0x1
		stwbrx  r6, 0, (r5)
		sync

poll_1418_precharge:
		xor   r6, r6, r6
		lwbrx r6, 0, (r5)
		andi.  r6 , r6, 0x01
		bne poll_1418_precharge	
/* Issue Nop command step 3*/

        	lis	r5, INTERNAL_BASE
        	ori	r5, r5, SDRAM_OPERATION      /* 0x1418 */
        	xor	r6, r6, r6
        	ori	r6, r6, 0x5
        	stwbrx	r6, r0, (r5)                 /* Store */
        	sync

poll_1418_new:
        	xor	r4, r4, r4
        	ori	r4, r4, 0x5                   /* is channel active */
        	lwbrx r6, 0, (r5)
	  	and. r4, r4, r6
	  	bne	 poll_1418_new

/* poll the sdram operation register to be in normal mode */
		lis     r5, INTERNAL_BASE
                ori     r5, r5, SDRAM_OPERATION
                xor     r4 , r4 , r4
                stwbrx  r4, 0, (r5)

	 
#if 0	 
        addis	r5, r0, INTERNAL_BASE
        ori	r5, r5, SDRAM_CONFIG         /* 0x1400 */
        addis	r6, r0, 0x5820
        ori	r6, r6, 0x0400
        stwbrx	r6, 0, (r5)                  /* Store */
        sync
#endif

/* poll the sdram operation register to be in normal mode */
		lis     r5, INTERNAL_BASE
                ori     r5, r5, SDRAM_OPERATION
                xor     r4 , r4 , r4
                stwbrx  r4, 0, (r5)

poll_normal_mode1:
		 xor  r4, r4, r4
		 xor  r6, r6, r6
		 sync
		 lwbrx   r4,0,(r5)
		 and.   r4, r4, r6		
		 bne   poll_normal_mode1		

/* write to the xtended mode register step 4*/



        addis	r5, r0, INTERNAL_BASE
        ori	r5, r5, SDRAM_MODE           /* 0x141c */
        xor	r6, r6, r6
        ori	r6, r6, 0x122        /*CL=2 val=0x22, see 0x1404 */ 
        stwbrx	r6, 0, (r5)                  /* Store   141c */
        sync

        addis	r5, r0, INTERNAL_BASE
        ori	r5, r5, SDRAM_OPERATION      /* 0x1418 */
        xor	r6, r6, r6
        ori	r6, r6, 0x3
        stwbrx	r6, 0, (r5)                  /* Store  */
        sync

poll_1418_2:
        xor	r4, r4, r4
        ori	r4, r4, 0x3                    /* is channel active */
        lwbrx	r6, 0, (r5)
        and.	r4, r4, r6
	 bne	poll_1418_2

/*  configure load mode register - end */

/* Issue Nop command - step 12*/

                lis     r5, INTERNAL_BASE
                ori     r5, r5, SDRAM_OPERATION      /* 0x1418 */
                xor     r6, r6, r6
                ori     r6, r6, 0x5
                stwbrx  r6, r0, (r5)                 /* Store */
                sync

poll_1418_nop2:
                xor     r4, r4, r4
                ori     r4, r4, 0x5                   /* is channel active */
                lwbrx r6, 0, (r5)
                and. r4, r4, r6
                bne      poll_1418_nop2

/* Issue Nop command - end*/

		xor r5, r5, r5
		xor r4, r4, r4
                li r5, 0x100
                mr r5, r4

delay_loop1 :
                sub r4, r4, 0x1
                cmp 0,0,r5,r4
                bne delay_loop1

/* precharge all command - step 13*/


                lis     r5, INTERNAL_BASE
                ori     r5, r5, SDRAM_OPERATION      /* 0x1418 */
                xor     r6, r6, r6
                ori     r6, r6, 0x1
                stwbrx  r6, 0, (r5)
                sync

poll_1418_precharge1:
                xor   r6, r6, r6
                lwbrx r6, 0, (r5)
                andi.  r6 , r6, 0x01
                bne poll_1418_precharge1

/* precharge all command - end */

/* Issue Nop command - step 14*/

                lis     r5, INTERNAL_BASE
                ori     r5, r5, SDRAM_OPERATION      /* 0x1418 */
                xor     r6, r6, r6
                ori     r6, r6, 0x5
                stwbrx  r6, r0, (r5)                 /* Store */
                sync

poll_1418_nop3:
                xor     r4, r4, r4
                ori     r4, r4, 0x5                   /* is channel active */
                lwbrx r6, 0, (r5)
                and. r4, r4, r6
                bne      poll_1418_nop3
		nop
		nop

/* Issue Nop command - end*/

/* Issue Refresh all command till tRFC - step 15 */

                lis     r5, INTERNAL_BASE
                ori     r5, r5, SDRAM_OPERATION      /* 0x1418 */
                xor     r6, r6, r6
                ori     r6, r6, 0x2
                stwbrx  r6, r0, (r5)                 /* Store */
                sync

poll_1418_refresh:
                xor     r4, r4, r4
                ori     r4, r4, 0x2                   /* is channel active */
                lwbrx r6, 0, (r5)
                and. r4, r4, r6
                bne      poll_1418_refresh

	        nop	
/* Issue Refresh all command till tRFC - end */

/* Issue Nop command - step 16*/

                lis     r5, INTERNAL_BASE
                ori     r5, r5, SDRAM_OPERATION      /* 0x1418 */
                xor     r6, r6, r6
                ori     r6, r6, 0x5
                stwbrx  r6, r0, (r5)                 /* Store */
                sync

poll_1418_nop4:
                xor     r4, r4, r4
                ori     r4, r4, 0x5                   /* is channel active */
                lwbrx r6, 0, (r5)
                and. r4, r4, r6
                bne      poll_1418_nop4

		nop
		nop
		nop
	 
	        xor r5, r5, r5
                xor r4, r4, r4
                li r5, 0x100
                mr r5, r4
delay_loop2 :
                sub r4, r4, 0x1
                cmp 0,0,r5,r4
                bne delay_loop2

/* Issue Nop command - end*/

/* Issue Refresh all command till tRFC - step 17 */

                lis     r5, INTERNAL_BASE
                ori     r5, r5, SDRAM_OPERATION      /* 0x1418 */
                xor     r6, r6, r6
                ori     r6, r6, 0x2
                stwbrx  r6, r0, (r5)                 /* Store */
                sync

poll_1418_refresh1:
                xor     r4, r4, r4
                ori     r4, r4, 0x2                   /* is channel active */
                lwbrx r6, 0, (r5)
                and. r4, r4, r6
                bne      poll_1418_refresh1

                nop
		nop

/* Issue Refresh all command till tRFC - end */

/* Issue Nop command - step 18*/

                lis     r5, INTERNAL_BASE
                ori     r5, r5, SDRAM_OPERATION      /* 0x1418 */
                xor     r6, r6, r6
                ori     r6, r6, 0x5
                stwbrx  r6, r0, (r5)                 /* Store */
                sync

poll_1418_nop5:
                xor     r4, r4, r4
                ori     r4, r4, 0x5                   /* is channel active */
                lwbrx r6, 0, (r5)
                and. r4, r4, r6
                bne      poll_1418_nop5

                nop
		xor r5, r5, r5
                xor r4, r4, r4
                li r5, 0x80
                mr r5, r4
delay_loop3 :
                sub r4, r4, 0x1
                cmp 0,0,r5,r4
                bne delay_loop3


/* Issue Nop command - end*/
/* poll the sdram operation register to be in normal mode */
		lis     r5, INTERNAL_BASE
                ori     r5, r5, SDRAM_OPERATION
                xor     r4 , r4 , r4
                stwbrx  r4, 0, (r5)

poll_normal_mode2:
		 xor  r4, r4, r4
		 xor  r6, r6, r6
		 sync
		 lwbrx   r4,0,(r5)
		 and.   r4, r4, r6		
		 bne   poll_normal_mode2

/* load mode register - LMR clear M8 bit - step 19*/

    		addis   r5, r0, INTERNAL_BASE
                ori     r5, r5, SDRAM_MODE           /* 0x141c */
                xor     r6, r6, r6
		lwbrx   r6, 0,(r5)
 	        andi.   r6, r6, 0xeff        
                stwbrx  r6, 0, (r5)         
                sync

		addis   r5, r0, INTERNAL_BASE
                ori     r5, r5, SDRAM_OPERATION
                xor     r6, r6, r6
                ori     r6, r6, 0x3
                stwbrx  r6, 0, (r5)                     /* Store */
                sync

poll_1418_step_18:
                xor     r4, r4, r4
                ori     r4, r4, 0x3                    /* is channel active */
                lwbrx   r6, 0, (r5)
                and.    r4, r4, r6
                bne     poll_1418_step_18


/* load mode register - LMR clear M8 bit - step 19 - end*/

/* Issue Nop command - step 20*/

                lis     r5, INTERNAL_BASE
                ori     r5, r5, SDRAM_OPERATION      /* 0x1418 */
                xor     r6, r6, r6
                ori     r6, r6, 0x5
                stwbrx  r6, r0, (r5)                 /* Store */
                sync

poll_1418_nop6:
                xor     r4, r4, r4
                ori     r4, r4, 0x5                   /* is channel active */
                lwbrx r6, 0, (r5)
                and. r4, r4, r6
                bne      poll_1418_nop6

                nop
                nop
		xor r5, r5, r5
                xor r4, r4, r4
                li r5, 0x100
                mr r5, r4
delay_loop4 :
                sub r4, r4, 0x1
                cmp 0,0,r5,r4
                bne delay_loop4

/* Issue Nop command - step 20 - end*/

                
#if 0
        addis	r5, r0, INTERNAL_BASE
        ori	r5, r5, D_UNIT_CONTROL_LOW   /* 0x1404 */
        lwbrx	r4, 0, (r5)
        lis	r6, 0x0
        ori	r6, r6, 0x1
        and.	r6, r6, r4
	 bne	synchddr
        
	addis	r6, r0, 0x1b11  /*asynchronious mode CL3=0x3c11 CL2.5=0x1b11 */
        ori	r6, r6, 0x0780           /* also 141c */
        b	set1404

synchddr:
        addis   r6, r0, 0x0311                  /*synchronious mode  */
        ori     r6, r6, 0x0000

set1404:
        andi.	r4, r4, 0x007f
        or	r4, r4, r6
        stwbrx	r4, 0, (r5)                     /* Store      1404 */
        sync


        addis	r5, r0, INTERNAL_BASE
        ori	r5, r5, SDRAM_OPERATION
        xor	r6, r6, r6
        ori	r6, r6, 0x3
        stwbrx	r6, 0, (r5)                     /* Store */
        sync

poll_1418_3:
        xor	r4, r4, r4
        ori	r4, r4, 0x3                    /* is channel active */
        lwbrx	r6, 0, (r5)
        and.	r4, r4, r6
	 bne	poll_1418_3
#endif
        /*DIMM configuration 64Mbit */
        addis	r4, r0, INTERNAL_BASE
        ori	r4, r4, SDRAM_ADDR_CONTROL /* r4 holds the SDRAM address 
                                              control register address */
        lis	r5, 0x0
        ori	r5, r5, 0x0012           /* in _DIMM1: 0x1410 <= 0x00000002 */ 
        stwbrx	r5, 0, (r4)                /* update register 0x1410  */
        sync
#if 0
        addis	r5, r0, INTERNAL_BASE
        ori	r5, r5, SDRAM_TIMING_CONTROL_LOW  /* 0x1408 */            
        addis	r6, r0, 0x1151    
        ori	r6, r6, 0x1220
        stwbrx	r6, 0, (r5)                        /* Store  1408 */
        sync
#endif

        addis   r5, r0, INTERNAL_BASE
        ori     r5, r5, PUNIT_MMASK               /* 0x3e4 */
        lwbrx   r6, 0, (r5) 
        ori     r6, r6,0x0001
        stwbrx	r6, 0, (r5) 
        sync

        mflr    r27
        bl      i2cInit
        mtlr    r27

        li      r4, BRDID_EE_ADDR    /* default is 0xa4 */
        li      r7, VARIANT_INFO1     /* offset 0x1fe1 */
        mflr    r27
        bl      i2cRead
        mtlr    r27

        /* r6 has the value , let bits 3:5 remain*/
        andi.   r6,r6,0x38
  
        addis	r5, r0, INTERNAL_BASE
        ori	r5, r5, BANK0_SIZE_REG   /* temp register for later use */

        cmpi    0,0,r6,0x08
        beq     size_256M
        cmpi    0,0,r6,0x10
        beq     size_512M
        cmpi    0,0,r6,0x18
        beq     size_1G
        cmpi    0,0,r6,0x20
        beq     size_2G

        /* Anything else make the default size as 128MB */
	  addis	r4, r0, INTERNAL_BASE
        ori	r4, r4, SDRAM_ADDR_CONTROL
                                         
        lis	r5, 0x0
        ori	r5, r5, 0x0002          
        stwbrx	r5, 0, (r4)            
        sync


        addis	r6, r0, 0x0
        ori	r6, r6, 128 
        addis   r4, r0, 0x0
        ori     r4, r4, 0x07ff 
        lis     r26,MEMSIZE_128MB@h  /* will be used in scrubMem */
        /* Load r19 with the stack pointer for CPU1 */
#ifdef PMC280_DUAL_CPU
        LOADPTR (r19, STACK_ADRS+PHY_CPU0_MEM_BOT+0x8000000/2+USER_RESERVED_MEM/2) 
#endif /* PMC280_DUAL_CPU */
        b       size_store
size_256M:
	  addis	r4, r0, INTERNAL_BASE
        ori	r4, r4, SDRAM_ADDR_CONTROL 
        lis	r5, 0x0
        ori	r5, r5, 0x0012           
        stwbrx	r5, 0, (r4)               
        sync	
	
        addis	r6, r0, 0x0
        ori	r6, r6, 256
        addis   r4, r0, 0x0
        ori     r4, r4, 0x0fff 
        lis     r26,MEMSIZE_256MB@h  /* will be used in scrubMem */
        /* Load r19 with the stack pointer for CPU1 */
#ifdef PMC280_DUAL_CPU
        LOADPTR (r19, STACK_ADRS+PHY_CPU0_MEM_BOT+0x10000000/2+USER_RESERVED_MEM/2) 
#endif /* PMC280_DUAL_CPU */
        b       size_store
size_512M:
	#ifndef PMC280_DUAL_CPU
	  addis	r4, r0, INTERNAL_BASE
        ori	r4, r4, SDRAM_ADDR_CONTROL
                                         
        lis	r5, 0x0
        #if 0
        ori	r5, r5, 0x0012 
        #endif
        ori	r5, r5, 0x0012 
        
        stwbrx	r5, 0, (r4)            
        sync	
	#endif

        addis	r6, r0, 0x0
        ori	r6, r6, 512
        addis   r4, r0, 0x0
        ori     r4, r4, 0x1fff 
        lis     r26,MEMSIZE_512MB@h  /* will be used in scrubMem */
        /* Load r19 with the stack pointer for CPU1 */
#ifdef PMC280_DUAL_CPU
        LOADPTR (r19, STACK_ADRS+PHY_CPU0_MEM_BOT+0x20000000/2+USER_RESERVED_MEM/2) 
#endif /* PMC280_DUAL_CPU */
        b       size_store
size_1G:

	  addis	r4, r0, INTERNAL_BASE
        ori	r4, r4, SDRAM_ADDR_CONTROL 
        lis	r5, 0x0
        ori	r5, r5, 0x0022          
        stwbrx	r5, 0, (r4)               
        sync	

        addis	r6, r0, 0x0
        ori	r6, r6, 1024
        addis   r4, r0, 0x0
        ori     r4, r4, 0x3fff 
        lis     r26,MEMSIZE_1G@h  /* will be used in scrubMem */
        /* Load r19 with the stack pointer for CPU1 */
#ifdef PMC280_DUAL_CPU
        LOADPTR (r19, STACK_ADRS+PHY_CPU0_MEM_BOT+0x40000000/2+USER_RESERVED_MEM/2) 
#endif /* PMC280_DUAL_CPU */
        b       size_store
size_2G:

	  addis	r4, r0, INTERNAL_BASE
        ori	r4, r4, SDRAM_ADDR_CONTROL 
        lis	r5, 0x0
        ori	r5, r5, 0x0022           
        stwbrx	r5, 0, (r4)                
        sync	


        addis	r6, r0, 0x0
        ori	r6, r6, 2048
        addis   r4, r0, 0x0
        ori     r4, r4, 0x7fff 
        lis     r26,MEMSIZE_2G@h  /* will be used in scrubMem */
        /* Load r19 with the stack pointer for CPU1 */
#ifdef PMC280_DUAL_CPU
        LOADPTR (r19, STACK_ADRS+PHY_CPU0_MEM_BOT+0x80000000/2+USER_RESERVED_MEM/2) 
#endif /* PMC280_DUAL_CPU */

size_store:
        stwbrx	r6, 0, (r5)                     /* Store */
        sync
        addis	r5, r0, INTERNAL_BASE
        ori	r5, r5, CS_0_SIZE
        stwbrx	r4, 0, (r5)                     /* Store */
        sync
        /* Load the base address for CS1 now itself, enabling will be done later */
        addi    r4, r4,1
        addis	r5, r0, INTERNAL_BASE
        ori	r5, r5, CS_1_BASE_ADDR
        stwbrx	r4, 0, (r5)                     /* Store */
        sync

        addis	r5, r0, INTERNAL_BASE
        ori	r5, r5, CPU1_STACK_ADRS_REG
        stwbrx	r19, 0, (r5)                     /* Store */
        sync

	
	/* BASE_ADDR_ENABLE bits    |   20-Internal Space |   16-PCI1-Mem1 */
	/* 0 is enabled !!!          \                    |   17-PCI1-Mem2 */
	/*               31-21-Reserved \                 |   18-PCI1-Mem3 */
	/*                               \                |  19-Internal SRAM*/
	
	/*   12-PCI0-Mem2 | + 08-Boot CS   | + 04-Dev CS0 | + 00-SDRAM CS0 */
	/*   13-PCI0-Mem3 | + 09-PCI0-IO   | + 05-Dev CS1 |   01-SDRAM CS1 */
	/*   14-PCI1-IO   | + 10-PCI0-Mem0 | + 06-Dev CS2 |   02-SDRAM CS2 */
	/* + 15-PCI1-Mem0 |   11-PCI0-Mem1 | + 07-Dev CS3 |   03-SDRAM CS3 */
        addis	r5, r0, INTERNAL_BASE
        ori	r5, r5, BASE_ADDR_ENABLE
        addis	r6, r0, 0x0007
#ifdef PMC280_PCI_BOOT
        ori	r6, r6, 0xfbfe    /*  register read has 0x0007 380e */ 
#else
#ifdef PMC280_USERFLASH_BOOT
        ori	r6, r6, 0xffee    /*  register read has 0x0007 380e */ 
#else
        ori	r6, r6, 0xfefe    /*  register read has 0x0007 380e */ 
#endif
#endif /* PMC280_PCI_BOOT */
        stwbrx	r6, 0, (r5)                     /* Store */
        sync

/* Read the other byte to know the size of CS1 */

        li      r4, BRDID_EE_ADDR    /* default is 0xa4 */
        li      r7, VARIANT_INFO2     /* offset 0x1fe2 */
        mflr    r27
        bl      i2cRead
        mtlr    r27

        /* r6 has the value , let bits 4:6 remain*/
        andi.   r6,r6,0x70
 
        addis   r5, r0, INTERNAL_BASE
        ori     r5, r5, BANK1_SIZE_REG   /* temp register for later use */
        stwbrx  r0, 0, (r5)              /* intialize to zero */
        sync

        cmpi    0,0,r6,0x10
        beq     size_128M_b1
        cmpi    0,0,r6,0x20
        beq     size_256M_b1
        cmpi    0,0,r6,0x30
        beq     size_512M_b1
        cmpi    0,0,r6,0x40
        beq     size_1G_b1
        cmpi    0,0,r6,0x50
        beq     size_2G_b1

        /* Anything else make the default size as 0MB */
        b       complete_b1

size_128M_b1:
	  
	  addis	r4, r0, INTERNAL_BASE
        ori	r4, r4, SDRAM_ADDR_CONTROL /* r4 holds the SDRAM address 
                                              control register address */
        lis	r5, 0x0
        ori	r5, r5, 0x0002           /* in _DIMM1: 0x1410 <= 0x00000002 */ 
        stwbrx	r5, 0, (r4)                /* update register 0x1410  */
        sync

        addis   r6, r0, 0x0
        ori     r6, r6, 128
        addis   r4, r0, 0x0
        ori     r4, r4, 0x07ff
        addis   r26, r26, MEMSIZE_128MB@h  /* will be used in scrubMem */
        /* Load r19 with the stack pointer for CPU1 */
#ifdef PMC280_DUAL_CPU
        LOADPTR (r19, STACK_ADRS+PHY_CPU0_MEM_BOT+0x8000000/2+USER_RESERVED_MEM/2)
#endif /* PMC280_DUAL_CPU */
        b       size_store_b1
size_256M_b1:
	
	  addis	r4, r0, INTERNAL_BASE
        ori	r4, r4, SDRAM_ADDR_CONTROL /* r4 holds the SDRAM address 
                                              control register address */
        lis	r5, 0x0
        ori	r5, r5, 0x0012           /* in _DIMM1: 0x1410 <= 0x00000002 */ 
        stwbrx	r5, 0, (r4)                /* update register 0x1410  */
        sync


        addis   r6, r0, 0x0
        ori     r6, r6, 256
        addis   r4, r0, 0x0
        ori     r4, r4, 0x0fff
        addis   r26, r26, MEMSIZE_256MB@h  /* will be used in scrubMem */
        /* Load r19 with the stack pointer for CPU1 */
#ifdef PMC280_DUAL_CPU
        LOADPTR (r19, STACK_ADRS+PHY_CPU0_MEM_BOT+0x10000000/2+USER_RESERVED_MEM/2)
#endif /* PMC280_DUAL_CPU */
        b       size_store_b1
size_512M_b1:
	#ifndef PMC280_DUAL_CPU
	  addis	r4, r0, INTERNAL_BASE
        ori	r4, r4, SDRAM_ADDR_CONTROL 
        lis	r5, 0x0
        ori	r5, r5, 0x0012           
        stwbrx	r5, 0, (r4)               
        sync	
	#endif

        addis   r6, r0, 0x0
        ori     r6, r6, 512
        addis   r4, r0, 0x0
        ori     r4, r4, 0x1fff
        addis   r26, r26, MEMSIZE_512MB@h  /* will be used in scrubMem */
        /* Load r19 with the stack pointer for CPU1 */
#ifdef PMC280_DUAL_CPU
        LOADPTR (r19, STACK_ADRS+PHY_CPU0_MEM_BOT+0x20000000/2+USER_RESERVED_MEM/2)
#endif /* PMC280_DUAL_CPU */
        b       size_store_b1
size_1G_b1:

	  addis	r4, r0, INTERNAL_BASE
        ori	r4, r4, SDRAM_ADDR_CONTROL /* r4 holds the SDRAM address 
                                              control register address */
        lis	r5, 0x0
        ori	r5, r5, 0x0022           /* in _DIMM1: 0x1410 <= 0x00000002 */ 
        stwbrx	r5, 0, (r4)                /* update register 0x1410  */
        sync


        addis   r6, r0, 0x0
        ori     r6, r6, 1024
        addis   r4, r0, 0x0
        ori     r4, r4, 0x3fff
        addis   r26, r26, MEMSIZE_1G@h  /* will be used in scrubMem */
        /* Load r19 with the stack pointer for CPU1 */
#ifdef PMC280_DUAL_CPU
        LOADPTR (r19, STACK_ADRS+PHY_CPU0_MEM_BOT+0x40000000/2+USER_RESERVED_MEM/2)
#endif /* PMC280_DUAL_CPU */
        b       size_store_b1
size_2G_b1:

	  addis	r4, r0, INTERNAL_BASE
        ori	r4, r4, SDRAM_ADDR_CONTROL /* r4 holds the SDRAM address 
                                              control register address */
        lis	r5, 0x0
        ori	r5, r5, 0x0022           /* in _DIMM1: 0x1410 <= 0x00000002 */ 
        stwbrx	r5, 0, (r4)                /* update register 0x1410  */
        sync

        addis   r6, r0, 0x0
        ori     r6, r6, 2048
        addis   r4, r0, 0x0
        ori     r4, r4, 0x7fff
        addis   r26, r26, MEMSIZE_2G@h  /* will be used in scrubMem */
        /* Load r19 with the stack pointer for CPU1 */
#ifdef PMC280_DUAL_CPU
        LOADPTR (r19, STACK_ADRS+PHY_CPU0_MEM_BOT+0x80000000/2+USER_RESERVED_MEM/2)
#endif /* PMC280_DUAL_CPU */

size_store_b1:
        stwbrx  r6, 0, (r5)                     /* Store */
        sync
        addis   r5, r0, INTERNAL_BASE
        ori     r5, r5, CS_1_SIZE
        stwbrx  r4, 0, (r5)                     /* Store */
        sync
/*        addis   r5, r0, INTERNAL_BASE
        ori     r5, r5, CPU1_STACK_ADRS_REG
        stwbrx  r19, 0, (r5)                     
        sync
*/

        addis   r5, r0, INTERNAL_BASE
        ori     r5, r5, BASE_ADDR_ENABLE
        lwbrx   r6, 0, (r5)
        lis     r4, 0xffff
        ori     r4, r4, 0xfffd
        and     r6, r6, r4
        stwbrx  r6, 0, (r5)                     /* Store */
        sync


complete_b1:

        blr
        
/*****************************************************************************/
#define DFCDL_OFFSET 0
#if 0
dfcdlInit:
	addis   r5, r0, INTERNAL_BASE
	ori     r5,r5,SRAM_DATA0

       li      r3, 64        /* number of data to write */  
       li      r4, 0x0       /* as per. TB-92, Mar 13, 2003 */

       li      r6, 0x014F    /* as per. TB-92, Mar 13, 2003 */

dfcdl_loop:
	stwbrx  r6, 0, (r5)
	sync
	add     r6, r6, r4      /* addi r6,r6, 0x41 */
	subi    r3, r3, 1
	cmpi    0,0,r3,0x0
	bne     dfcdl_loop

	addis   r5, r0, INTERNAL_BASE
	ori     r5,r5,DFCDL_CONFIG0          /* ( 0x1480 ) */
	lis     r6, 0x0030
	ori     r6, r6, 0x0000
	stwbrx  r6, 0, (r5)                     
	sync

	/* addis   r5, r0, INTERNAL_BASE     */
	/* ori     r5,r5,DFCDL_CONFIG1        * ( 0x1484 ) */  
	/* lis     r6, 0x0100                */      
	/* ori     r6, r6, 0x01d8            */      
	/* stwbrx  r6, 0, (r5)               */
	/* sync                              */

	blr
#endif

dfcdlInit:
        mflr r23
        lis     r5, INTERNAL_BASE
        ori     r5, r5, SRAM_DATA0

        li      r3, 38

        li      r6, 0x02
        stwbrx  r6,0,(r5)
        sync

        li      r6, 0x02
        stwbrx  r6,0,(r5)
        sync

        li      r6, 0x03
        stwbrx  r6,0,(r5)
        sync

        li      r6, 0x03
        stwbrx  r6,0,(r5)
        sync


        li      r6, 0x04
        stwbrx  r6,0,(r5)
        sync

        li      r6, 0x04
        stwbrx  r6,0,(r5)
        sync

        li      r6, 0x45
        stwbrx  r6,0,(r5)
        sync

        li      r6, 0x85
        stwbrx  r6,0,(r5)
        sync

        li      r6, 0xC6
        stwbrx  r6,0,(r5)
        sync

        li      r6, 0x106
        stwbrx  r6,0,(r5)
        sync

        li      r6, 0x147
        stwbrx  r6,0,(r5)
        sync

        li      r6, 0x188
        stwbrx  r6,0,(r5)
        sync

        li      r6, 0x248
        stwbrx  r6,0,(r5)
        sync

        li      r6, 0x28A
        stwbrx  r6,0,(r5)
        sync

        li      r6, 0x28A
        stwbrx  r6,0,(r5)
        sync

        li      r6, 0x2CB
        stwbrx  r6,0,(r5)
        sync

        li      r6, 0x30C
        stwbrx  r6,0,(r5)
        sync

        li      r6, 0x34D
        stwbrx  r6,0,(r5)
        sync

        li      r6, 0x34D
        stwbrx  r6,0,(r5)
        sync

        li      r6, 0x38E
        stwbrx  r6,0,(r5)
        sync

        li      r6, 0x3CF
        stwbrx  r6,0,(r5)
        sync

        li      r6, 0x40F
        stwbrx  r6,0,(r5)
        sync

        li      r6, 0x44F
        stwbrx  r6,0,(r5)
        sync

        li      r6, 0x48F
        stwbrx  r6,0,(r5)
        sync


        li      r6, 0x4CF
        stwbrx  r6,0,(r5)
        sync

        li      r6, 0x50F
        stwbrx  r6,0,(r5)
        sync

        li      r6, 0x54F
        stwbrx  r6,0,(r5)
        sync

        li      r6, 0x54F
        stwbrx  r6,0,(r5)
        sync

        li      r6, 0x54F
        stwbrx  r6,0,(r5)
        sync

        li      r6, 0x54F
        stwbrx  r6,0,(r5)
        sync

        li      r6, 0x54F
        stwbrx  r6,0,(r5)
        sync

        li      r6, 0x54F
        stwbrx  r6,0,(r5)
        sync


        li      r6, 0x54F
        stwbrx  r6,0,(r5)
        sync

        li      r6, 0x54F
        stwbrx  r6,0,(r5)
        sync


        li      r6, 0x54F
        stwbrx  r6,0,(r5)
        sync


        li      r6, 0x54F
        stwbrx  r6,0,(r5)
        sync

        li      r6, 0x54F
        stwbrx  r6,0,(r5)
        sync

        li      r6, 0x54F
        stwbrx  r6,0,(r5)
        sync

        li      r6, 0x54F
        stwbrx  r6,0,(r5)
        sync

        li      r6, 0x54F
        stwbrx  r6,0,(r5)
        sync

        li      r6, 0x54F
        stwbrx  r6,0,(r5)
        sync

        li      r6, 0x54F
        stwbrx  r6,0,(r5)
        sync

        li      r6, 0x54F
        stwbrx  r6,0,(r5)
        sync

        li      r6, 0x54F
        stwbrx  r6,0,(r5)
        sync

        li      r6, 0x54F
        stwbrx  r6,0,(r5)
        sync

        li      r6, 0x54F
        stwbrx  r6,0,(r5)
        sync

        li      r6, 0x54F
        stwbrx  r6,0,(r5)
        sync

        li      r6, 0x54F
        stwbrx  r6,0,(r5)
        sync

        li      r6, 0x54F
        stwbrx  r6,0,(r5)
        sync

        li      r6, 0x54F
        stwbrx  r6,0,(r5)
        sync

        li      r6, 0x54F
        stwbrx  r6,0,(r5)
        sync

        li      r6, 0x54F
        stwbrx  r6,0,(r5)
        sync

        li      r6, 0x54F
        stwbrx  r6,0,(r5)
        sync

        li      r6, 0x54F
        stwbrx  r6,0,(r5)
        sync

        li      r6, 0x54F
        stwbrx  r6,0,(r5)
        sync

        li      r6, 0x54F
        stwbrx  r6,0,(r5)
        sync

        li      r6, 0x54F
        stwbrx  r6,0,(r5)
        sync

        li      r6, 0x54F
        stwbrx  r6,0,(r5)
        sync

        li      r6, 0x54F
        stwbrx  r6,0,(r5)
        sync

        li      r6, 0x54F
        stwbrx  r6,0,(r5)
        sync

        li      r6, 0x54F
        stwbrx  r6,0,(r5)
        sync

        li      r6, 0x54F
        stwbrx  r6,0,(r5)
        sync

        li      r6, 0x54F
        stwbrx  r6,0,(r5)
        sync

        li      r6, 0x54F
        stwbrx  r6,0,(r5)
        sync

        lis     r5, INTERNAL_BASE
        ori     r5,r5,DFCDL_CONFIG0          /* ( 0x1480 ) */
        lis     r6, 0x0030
        ori     r6, r6, 0x0000
        stwbrx  r6, 0, (r5)
        sync
        mtlr r23
        blr

i2cInit:
        /* Insure r0 is 0. */
        andi.   r0, r0, 0
 
        /* Reset the i2c Mechanism first */
        addis   r5, r0, INTERNAL_BASE
        ori     r5, r5, I2C_SOFT_RESET
        xor     r6, r6, r6
        stwbrx  r6, 0, (r5)
        sync
 
        addis   r5, r0, INTERNAL_BASE
        ori     r5, r5, I2C_CONTROL
        stwbrx  r6, 0, (r5)
        sync
 
        /*
         * Initializing the I2C mechanism. Assuming Tclock
         * frequency of 133MHz:
         *   
         * Fscl = 133000000 / (10 * (8+1) * 2^(3+1)) = 92 KHz
         */  
        addis   r5, r0, INTERNAL_BASE
        ori     r5, r5, I2C_STATUS_BAUDE_RATE
        addi    r6, r0, (8 << 3) | 3
        stwbrx  r6, 0, (r5)
        sync
 
        /* Enable the I2C mechanism and slave mode */
        addis   r5, r0, INTERNAL_BASE
        ori     r5, r5, I2C_CONTROL
        addi    r6, r0, 0x44
        stwbrx  r6, 0, (r5)
        sync
 
        blr

/******************************************************************************/
/* Read from I2C device
 * Input:       r4:     EEPROM address.
 *              r7:     Offset
 * Output:      r6:     Value
 * Used:        r0, r3, r4, r5, r6
 */
i2cRead:

        /* Give a delay , the MV requires this */
        lis     r6,0
        ori     r6,r6,0xFFFF
        lis     r5,0

delay1:
        addi    r5,r5,1
        cmp     0, 1, r5, r6
        bne     delay1

        /* Insure r0 is 0. */
        andi.   r0, r0, 0
 
 
        /* START READING THE DIMM */

        /* Set the Ack Bit */
        addis   r5, r0, INTERNAL_BASE
        ori     r5, r5, I2C_CONTROL
        lwbrx   r6, 0, (r5)
        ori     r6, r6, 4
        stwbrx  r6, 0, (r5)
        sync

        /* Transmit the device address and the desired offset within the EEPROM
         */

        /* Generate Start Bit */
        addis   r5, r0, INTERNAL_BASE
        ori     r5, r5, I2C_CONTROL
        lwbrx   r6, 0, (r5)
        ori     r6, r6, 32
        stwbrx  r6, 0, (r5)
        sync

        /* Wait for the status I2C_START_CONDITION_TRA = 0x8 */
        addis   r5, r0, INTERNAL_BASE
        ori     r5, r5, I2C_STATUS_BAUDE_RATE
        sync

loop_0:
        lwbrx   r6, 0, (r5)
        cmpi    0, 0, r6, 8
        bne     loop_0
        sync
 
        /* writing the address of (DIMM0 << 1) with write indication (bit0 
           is 0) */
        addis   r5, r0, INTERNAL_BASE
        ori     r5, r5, I2C_DATA
        xor     r6, r6, r6
/*    lwbrx   r6, 0, (r5) */
        or      r6, r6, r4         /* (DIMM_0/1_ADDR << 1) */
        stwbrx  r6, 0, (r5)
        sync
 
        /* Wait for the interrupt flag (bit3) to be set as a result of
           writing to the data register.*/
        addis   r5, r0, INTERNAL_BASE
        ori     r5, r5, I2C_CONTROL
 
loop_1:
        lwbrx   r6, 0, (r5)
        andi.   r6, r6, 8
        cmpi    0, 0, r6, 8
        bne     loop_1
        sync
 
        /* Clear the interrupt flag */
        addis   r5, r0, INTERNAL_BASE
        ori     r5, r5, I2C_CONTROL
        lis     r3, 0xffff
        ori     r3, r3, 0xfff7
        lwbrx   r6, 0, (r5)
        and     r6, r6, r3
        stwbrx  r6, 0, (r5)
        sync
 
        /* Waiting for the interrupt flag to be set which means that the
           address has been transmitted                                  */
        addis   r5, r0, INTERNAL_BASE
        ori     r5, r5, I2C_CONTROL
 
loop_2:
        lwbrx   r6, 0, (r5)
        andi.   r6, r6, 8
        cmpi    0, 0, r6, 8
        bne     loop_2
        sync
 
        /* Wait for status I2C_ADDR_PLUS_WRITE_BIT_TRA_ACK_REC = 0x18 */
        addis   r5, r0, INTERNAL_BASE
        ori     r5, r5, I2C_STATUS_BAUDE_RATE
 
loop_3:
        lwbrx   r6, 0, (r5)
        cmpi    0, 0, r6, 0x18
        bne     loop_3
        sync

        mr     r10,r7    /* Store it to split the add. */
        andi.  r7, r7, 0xff00
        li     r5, 8
        srw    r7, r7, r5   /* r7 = upper 8 bit offset */
        andi.  r10, r10, 0x00ff   /* r10 = lower 8 bit offset */ 
 
        /* Write the upper offset to be read from the EEPROM */
        addis   r5, r0, INTERNAL_BASE
        ori     r5, r5, I2C_DATA
        xor     r6, r6, r6
        or      r6, r6, r7
        stwbrx  r6, 0, (r5)
        sync
 
        /* Wait for the interrupt flag (bit3) to be set */
        addis   r5, r0, INTERNAL_BASE
        ori     r5, r5, I2C_CONTROL
 
loop_4:
        lwbrx   r6, 0, (r5)
        andi.   r6, r6, 8
        cmpi    0, 0, r6, 8
        bne     loop_4
        sync
 
        /* Clear the interrupt flag ==> signaling that the address can now
           be transmited                                                    */
        addis   r5, r0, INTERNAL_BASE
        ori     r5, r5, I2C_CONTROL
        lis     r3, 0xffff
        ori     r3, r3, 0xfff7
        lwbrx   r6, 0, (r5)
        and     r6, r6, r3
        stwbrx  r6, 0, (r5)
        sync
 
        /* Wait for the interrupt to be set again ==> address has been transmited */
        addis   r5, r0, INTERNAL_BASE
        ori     r5, r5, I2C_CONTROL
 
loop_5:
        lwbrx   r6, 0, (r5)
        andi.   r6, r6, 8
        cmpi    0, 0, r6, 8
        bne     loop_5
        sync
 
        /* Wait for status I2C_MAS_TRAN_DATA_BYTE_ACK_REC = 0x28 */
        addis   r5, r0, INTERNAL_BASE
        ori     r5, r5, I2C_STATUS_BAUDE_RATE
 
loop_6:
        lwbrx   r6, 0, (r5)
        cmpi    0, 0, r6, 0x28
        bne     loop_6
        sync

        /* Write the lower offset to be read from the EEPROM */
        addis   r5, r0, INTERNAL_BASE
        ori     r5, r5, I2C_DATA
        xor     r6, r6, r6
        or      r6, r6, r10
        stwbrx  r6, 0, (r5)
        sync

        /* Wait for the interrupt flag (bit3) to be set */
        addis   r5, r0, INTERNAL_BASE
        ori     r5, r5, I2C_CONTROL

loop_4a:
        lwbrx   r6, 0, (r5)
        andi.   r6, r6, 8
        cmpi    0, 0, r6, 8
        bne     loop_4a
        sync

        /* Clear the interrupt flag ==> signaling that the address can now
           be transmited                                                    */
        addis   r5, r0, INTERNAL_BASE
        ori     r5, r5, I2C_CONTROL
        lis     r3, 0xffff
        ori     r3, r3, 0xfff7
        lwbrx   r6, 0, (r5)
        and     r6, r6, r3
        stwbrx  r6, 0, (r5)
        sync

        /* Wait for the interrupt to be set again ==> address has been transmited */
        addis   r5, r0, INTERNAL_BASE
        ori     r5, r5, I2C_CONTROL

loop_5a:
        lwbrx   r6, 0, (r5)
        andi.   r6, r6, 8
        cmpi    0, 0, r6, 8
        bne     loop_5a
        sync

        /* Wait for status I2C_MAS_TRAN_DATA_BYTE_ACK_REC = 0x28 */
        addis   r5, r0, INTERNAL_BASE
        ori     r5, r5, I2C_STATUS_BAUDE_RATE

loop_6a:
        lwbrx   r6, 0, (r5)
        cmpi    0, 0, r6, 0x28
        bne     loop_6a
        sync

        /* Retransmit the device address with read indication to get the data.*/
 
        /* generate a repeated start bit */
        addis   r5, r0, INTERNAL_BASE
        ori     r5, r5, I2C_CONTROL
        lwbrx   r6, 0, (r5)
        ori     r6, r6, 0x20
        stwbrx  r6, 0, (r5)
        sync
 
        /* Wait for the interrupt flag (bit3) to be set */
        addis   r5, r0, INTERNAL_BASE
        ori     r5, r5, I2C_CONTROL
 
loop_7:
        lwbrx   r6, 0, (r5)
        andi.   r6, r6, 8
        cmpi    0, 0, r6, 8
        bne     loop_7
        sync
 
        /* Clear the interrupt flag ==> the start bit will be transmitted. */
        addis   r5, r0, INTERNAL_BASE
        ori     r5, r5, I2C_CONTROL
        lis     r3, 0xffff
        ori     r3, r3, 0xfff7
        lwbrx   r6, 0, (r5)
        and     r6, r6, r3
        stwbrx  r6, 0, (r5)
        sync
 
        /* Wait for status I2C_REPEATED_START_CONDITION_TRA = 0x10 */
        addis   r5, r0, INTERNAL_BASE
        ori     r5, r5, I2C_STATUS_BAUDE_RATE
 
loop_8:
        lwbrx   r6, 0, (r5)
        cmpi    0, 0, r6, 0x10
        bne     loop_8
        sync
 
        /* Writing the address of (DIMM0 << 1) with raed indication (bit0 is 1)         */
        addis   r5, r0, INTERNAL_BASE
        ori     r5, r5, I2C_DATA
        xor     r6, r6, r6
        or      r6, r6, r4    /*  (DIMM_0/1_ADDR << 1)               */  
        ori     r6, r6, 1     /* ((DIMM_0/1_ADDR << 1) | 1) for read */
        stwbrx  r6, 0, (r5)
        sync
 
        /* Wait for the interrupt flag (bit3) to be set as a result of
        transmitting the address.                                     */
        addis   r5, r0, INTERNAL_BASE
        ori     r5, r5, I2C_CONTROL
 
loop_9:
        lwbrx   r6, 0, (r5)
        andi.   r6, r6, 8
        cmpi    0, 0, r6, 8
        bne     loop_9
        sync
 
        /* Clear the interrupt flag ==> the address will be transmitted */
        addis   r5, r0, INTERNAL_BASE
        ori     r5, r5, I2C_CONTROL
        lis     r3, 0xffff
        ori     r3, r3, 0xfff7
        lwbrx   r6, 0, (r5)
        and     r6, r6, r3
        stwbrx  r6, 0, (r5)
        sync
 
        /* Wait for status I2C_ADDR_PLUS_READ_BIT_TRA_ACK_REC = 0x40 */
        addis   r5, r0, INTERNAL_BASE
        ori     r5, r5, I2C_STATUS_BAUDE_RATE
 
loop_10:
        lwbrx   r6, 0, (r5)
        cmpi    0, 0, r6, 0x40
        bne     loop_10
        sync
 
        /* Clear the interrupt flag and signal no-acknowledge  */
        /* First, wait for the interrupt flag (bit3) to be set */
        addis   r5, r0, INTERNAL_BASE
        ori     r5, r5, I2C_CONTROL
 
loop_11:
        lwbrx   r6, 0, (r5)
        andi.   r6, r6, 8
        cmpi    0, 0, r6, 8
        bne     loop_11
        sync
 
        /* Clear the interrupt flag and the Acknoledge bit */
        addis   r5, r0, INTERNAL_BASE
        ori     r5, r5, I2C_CONTROL
        lis     r3, 0xffff
        ori     r3, r3, 0xfff0
        lwbrx   r6, 0, (r5)
        and     r6, r6, r3
        stwbrx  r6, 0, (r5)
        sync
 
        /* Wait for status I2C_MAS_REC_READ_DATA_ACK_NOT_TRA = 0x58 */
        addis   r5, r0, INTERNAL_BASE
        ori     r5, r5, I2C_STATUS_BAUDE_RATE
 
loop_12:
        lwbrx   r6, 0, (r5)
        cmpi    0, 0, r6, 0x58
        bne     loop_12
        sync
 
        /* Generate stop bit */
        addis   r5, r0, INTERNAL_BASE
        ori     r5, r5, I2C_CONTROL
        lwbrx   r6, 0, (r5)
        ori     r6, r6, 16
        stwbrx  r6, 0, (r5)
        sync
 
        /* Clear the interrupt flag */
        /* First, wait for the interrupt flag (bit3) to be set */
        addis   r5, r0, INTERNAL_BASE
        ori     r5, r5, I2C_CONTROL
 
loop_13:
        lwbrx   r6, 0, (r5)
        andi.   r6, r6, 8
        cmpi    0, 0, r6, 8
        bne     loop_13
        sync
 
        /* Clear the interrupt flag  */
        xor     r5, r5, r5
        addis   r5, r0, INTERNAL_BASE
        ori     r5, r5, I2C_CONTROL
        lis     r3, 0xffff
        ori     r3, r3, 0xfff7
        lwbrx   r6, 0, (r5)
        and     r6, r6, r3
        stwbrx  r6, 0, (r5)
        sync
 
        /* Store the data in r6. */
        addis   r5, r0, INTERNAL_BASE
        ori     r5, r5, I2C_DATA
        lwbrx   r6, 0, (r5)
 
        blr
 














/*****************************************************************************/

#ifdef PMC280_DEBUG_LED
        /* Uses r3, r4 */
dbg_led_23_on:
        addis   r3, r0, INTERNAL_BASE
        ori     r3, r3, 0xF104
        addis   r4, r0, 0x0080
        ori     r4, r4, 0x0000
        stwbrx  r4, 0, (r3)                     // Store
        blr

        /* Uses r3, r4 */
dbg_led_23_off:
        addis   r3, r0, INTERNAL_BASE
        ori     r3, r3, 0xF104
        addis   r4, r0, 0x0
        ori     r4, r4, 0x0
        stwbrx  r4, 0, (r3)                     // Store
        blr

        /* Uses r3, r4, r5 */
dbg_led_23_blink:
        mflr    r5
        bl      dbg_led_23_on
        nop
        nop
        bl      dbg_led_23_off
        nop
        nop
        mtlr    r5
        blr
#endif /* PMC280_DEBUG_LED */

        /* Initialize MPSC 0/1 as the debug port */
_initialize_dbg_uart:
 
        /* Set MPP[20] as BClkIn */
        lis     r3, INTERNAL_BASE
        ori     r3,r3,0xF008
        lis     r4,0x4
        stwbrx  r4,0,(r3)

        /* Make MPP[0], MPP[1], MPP[3] and MPP[5] as Tx/Rx pins for MPSC 0/1 */
        lis     r3, INTERNAL_BASE
        ori     r3,r3,0xF000
        lis     r4,0x0030
        ori     r4,r4,0x3022
        stwbrx  r4,0,(r3)
 
        /* MRR: Connect MPSC 0/1 */
        lis     r3,INTERNAL_BASE
        ori     r3,r3,0xB400
        lis     r4,0x0000
        ori     r4,r4,0x0000
        stwbrx  r4,0,(r3)
 
        /* RCR = BRG0 for MPSC0, BRG1 for MPSC1 */
        lis     r3,INTERNAL_BASE
        ori     r3,r3,0xB404
        lis     r4,0
        ori     r4,r4,0x100
        stwbrx  r4,0,(r3)
 
        /* TCR = BRG0 for MPSC0, BRG1 for MPSC1 */
        lis     r3,INTERNAL_BASE
        ori     r3,r3,0xB408
        lis     r4,0
        ori     r4,r4,0x100
        stwbrx  r4,0,(r3)

        /* BRG0 = 115200 bps@12.5 MHz using BClkIn */
        lis     r3,INTERNAL_BASE
        ori     r3,r3,0xB200
        lis     r4,0x0041    /* 0x0061 (SysClk)/0x0041 (BClkIn) */
        ori     r4,r4,0x0006 /* 115200@66MHz = 0x0022, 115200@12.5 = 0x0006 */
                             /* 9600@12.5MHz = 0x0051, 115200@133MHz = 0x0048 */
        stwbrx  r4,0,(r3)

        /* BRG1 = 115200 bps@12.5 MHz using BClkIn */
        lis     r3,INTERNAL_BASE
        ori     r3,r3,0xB208
        lis     r4,0x0041    /* 0x0061 (SysClk)/0x0041 (BClkIn) */
        ori     r4,r4,0x0006 /* 115200@66MHz = 0x0022, 115200@12.5 = 0x0006 */
                             /* 9600@12.5MHz = 0x0051, 115200@133MHz = 0x0048 */
        stwbrx  r4,0,(r3)
 
        /* MPCR = 8N1 for MPSC 0*/
        lis     r3,INTERNAL_BASE
        ori     r3,r3,0x8008
        lis     r4,0
        ori     r4,r4,0x3000
        stwbrx  r4,0,(r3)

        /* MMCRL for MPSC 0 */
        lis     r3,INTERNAL_BASE
        ori     r3,r3,0x8000
        lis     r4,0
        ori     r4,r4,0x444
        stwbrx  r4,0,(r3)

        /* MMCRH for MPSC 0 */
        lis     r3,INTERNAL_BASE
        ori     r3,r3,0x8004
        lis     r4,0x0240
        ori     r4,r4,0x03F8
        stwbrx  r4,0,(r3)

        /* MPCR = 8N1 for MPSC 1 */
        lis     r3,INTERNAL_BASE
        ori     r3,r3,0x9008
        lis     r4,0
        ori     r4,r4,0x3000
        stwbrx  r4,0,(r3)
 
        /* MMCRL for MPSC 1 */
        lis     r3,INTERNAL_BASE
        ori     r3,r3,0x9000
        lis     r4,0
        ori     r4,r4,0x444
        stwbrx  r4,0,(r3)
 
        /* MMCRH for MPSC 1 */
        lis     r3,INTERNAL_BASE
        ori     r3,r3,0x9004
        lis     r4,0x0240
        ori     r4,r4,0x03F8
        stwbrx  r4,0,(r3)
 
        sync
        blr

        /* Put a character from MPSC 0 (gets the character in r3) */
        /* Uses r4, r5 */
        .global _dbg_transmit0
_dbg_transmit0:
 
        lis     r4,0
        ori     r4,r4,0x0FFF
        lis     r5,0
 
_dbg_tx0_delay:
        addi    r5,r5,1
        cmp     0, 1, r5, r4
        bne     _dbg_tx0_delay
 
        /* Store char. (in r3) to CH1 register TCS field */
        lis     r4,INTERNAL_BASE
        ori     r4,r4,0x800C
        stwbrx  r3,0,(r4)
 
        /* Give Transmit TCS char. command */
        lis     r4,INTERNAL_BASE
        ori     r4,r4,0x8010
        lis     r5,0x0
        ori     r5,r5,0x200
        stwbrx  r5,0,(r4)
 
        sync
        blr

        /* Put a character from MPSC 1 (gets the character in r3) */
        /* Uses r4, r5 */
        .global _dbg_transmit1
_dbg_transmit1:
 
        lis     r4,0
        ori     r4,r4,0x0FFF
        lis     r5,0
 
_dbg_tx1_delay:
        addi    r5,r5,1
        cmp     0, 1, r5, r4
        bne     _dbg_tx1_delay
 
        /* Store char. (in r3) to CH1 register TCS field */
        lis     r4,INTERNAL_BASE
        ori     r4,r4,0x900C
        stwbrx  r3,0,(r4)
 
        /* Give Transmit TCS char. command */
        lis     r4,INTERNAL_BASE
        ori     r4,r4,0x9010
        lis     r5,0x0
        ori     r5,r5,0x200
        stwbrx  r5,0,(r4)
 
        sync
        blr

#ifdef PMC280_DEBUG_UART

/* This can print a 32 bit register passed to r3 if 28 is passed to r30 */
/* Uses r29,r31,r30 */
_print_reg:
        mflr    r29
        mr      r31,r3

again:   
        srw     r3,r31,r30
        andi.   r3,r3,0xf
        cmpwi   r3,0x9
        bgt     _hexprint
        ori     r3,r3,0x30
        b       over
_hexprint:
        subi    r3,r3,0xa
        addi    r3,r3,0x41
over:  
        bl      _dbg_transmit0
        subi    r30,r30,4
        cmpwi   r30,0
        bnl     again
        li      r3,0x20
        bl      _dbg_transmit0

        mtlr    r29
        blr

#endif /* PMC280_DEBUG_UART */

PrintLine0:
/* Register r6 contains the address of the ascii string. This address is
 * assembled as an absolute address and its value depends on whether this
 * image is built as ROM resident or RAM relocatable. At this point,
 * execution is still running in ROM, (for both types of image). We must
 * use the address of the string in ROM since the string is not yet located
 * in RAM. (ROM_TEXT_ADRS - romInit) was calculated earlier, and is either,
 * 0 for a ROM resident image, or ROM_TEXT_ADRS - RAM_HIGH_ADRS for
 * relocatable images.
 */
         add        r6, r6, r15     /* always get address from ROM */
PrintLoop0:
         lbz        r3,0(r6)        /* load print character */
         eieio
         cmpwi      cr2,r3,0x0000   /* compare to NULL = end of string */
         beq-       cr2,PriEnd0      /* Value != NULL(=end of string)? */

         mflr       r7
         bl         _dbg_transmit0  /* print it to serial now, (r3)=Character */
         mtlr       r7
         addic      r6,r6,1         /* base address+1 for next character */
         b          PrintLoop0
PriEnd0:
         blr

PrintLine1:
/* Register r6 contains the address of the ascii string. This address is
 * assembled as an absolute address and its value depends on whether this
 * image is built as ROM resident or RAM relocatable. At this point,
 * execution is still running in ROM, (for both types of image). We must
 * use the address of the string in ROM since the string is not yet located
 * in RAM. (ROM_TEXT_ADRS - romInit) was calculated earlier, and is either,
 * 0 for a ROM resident image, or ROM_TEXT_ADRS - RAM_HIGH_ADRS for
 * relocatable images.
 */
         add        r6, r6, r15     /* always get address from ROM */
PrintLoop1:
         lbz        r3,0(r6)        /* load print character */
         eieio
         cmpwi      cr2,r3,0x0000   /* compare to NULL = end of string */
         beq-       cr2,PriEnd1      /* Value != NULL(=end of string)? */

         mflr       r7
         bl         _dbg_transmit1  /* print it to serial now, (r3)=Character */
         mtlr       r7
         addic      r6,r6,1         /* base address+1 for next character */
         b          PrintLoop1
PriEnd1:
         blr

#ifdef PMC280_DUAL_CPU

         /* Uses r3, r4, r5, r6, r7, r8, r15 */
waitPrint0:
         addis      r6,0, waitLine0@h /* Load base address of waitLine string */
         ori        r6,r6, waitLine0@l
         mflr       r8
         bl         PrintLine0
         mtlr       r8
         blr

         /* Uses r3, r4, r5, r6, r7, r8, r15 */
waitPrint1:
         addis      r6,0, waitLine1@h /* Load base address of waitLine string */
         ori        r6,r6, waitLine1@l
         mflr       r8
         bl         PrintLine1
         mtlr       r8
         blr
#else

         /* Uses r3, r4, r5, r6, r7, r8, r15 */
waitPrint:
         addis      r6,0, waitLine@h /* Load base address of waitLine string */
         ori        r6,r6, waitLine@l
         mflr       r8
         bl         PrintLine0
         mtlr       r8
         blr

#endif

#ifdef PMC280_DEBUG_UART
         /* Uses r3, r4, r5, r6, r7, r8, r15 */
OkayPrint:
         addis      r6,0, OkayLine@h /* Load base address of OkayLine string */
         ori        r6,r6,OkayLine@l
         mflr       r8
         bl         PrintLine0
         mtlr       r8
         blr

         /* Uses r3, r4, r5, r6, r7, r8, r15 */
DdrInitCompletePrint:
         addis      r6,0, DdrInitCompleteLine@h /* Load base address of OkayLine string */
         ori        r6,r6,DdrInitCompleteLine@l
         mflr       r8
         bl         PrintLine0
         mtlr       r8
         blr

         /* Uses r3, r4, r5, r6, r7, r8, r15 */
serialInitCompletePrint:
         addis      r6,0, serialInitCompleteLine@h /* Load base address of OkayLine string */
         ori        r6,r6,serialInitCompleteLine@l
         mflr       r8
         bl         PrintLine0
         mtlr       r8
         blr

         /* Uses r3, r4, r5, r6, r7, r8, r15 */
BATInitCompletePrint0:
         addis      r6,0, BATInitCompleteLine0@h /* Load base address of OkayLine string */
         ori        r6,r6,BATInitCompleteLine0@l
         mflr       r8
         bl         PrintLine0
         mtlr       r8
         blr

         /* Uses r3, r4, r5, r6, r7, r8, r15 */
BATInitCompletePrint1:
         addis      r6,0, BATInitCompleteLine1@h /* Load base address of OkayLine string */
         ori        r6,r6,BATInitCompleteLine1@l
         mflr       r8
         bl         PrintLine1
         mtlr       r8
         blr

         /* Uses r3, r4, r5, r6, r7, r8, r15 */
CentryPrint0:
         addis      r6,0, CentryLine0@h /* Load base address of OkayLine string */
         ori        r6,r6,CentryLine0@l
         mflr       r8
         bl         PrintLine0
         mtlr       r8
         blr

         /* Uses r3, r4, r5, r6, r7, r8, r15 */
CentryPrint1:
         addis      r6,0, CentryLine1@h /* Load base address of OkayLine string */
         ori        r6,r6,CentryLine1@l
         mflr       r8
         bl         PrintLine1
         mtlr       r8
         blr

         /*
          * PrintRegister: Print the contents of r3
          *
          * r3 = Contains the value to be printed
          * r4 = 4  (Number for characters - 1)
          *
          * Uses:
          */
PrintRegister:
         mflr   r9
         mr     r6,r3
         mr     r7,r4   /* Need to do this cos r4 is used by _dbg_transmit */
_loop:
         srw    r8,r6,r7
         andi.  r8,r8,0xf
         cmpwi  r8,0x9  /* Is it an alphabet? Goto _hex_print if yes */
         bgt    _alpha
         ori    r8,r8,0x30
         b      _print
_alpha:
         subi   r8,r8,0x0A
         addi   r8,r8,0x41
_print:
         mr     r3, r8
         bl     _dbg_transmit0       /* Uses r4, r5 and takes character in r3 */
         subi   r7,r7,4
         cmpwi  r7,0                /* Are we done? */
         bnl    _loop
         li     r3,0x20             /* If done, print "space" */
         bl     _dbg_transmit0

         /* Return */
         mtlr   r9
         blr
#endif /* PMC280_DEBUG_UART */

/*
         * Name  : frcWhoAmI
         *
         *         This function returns the ID of the CPU on which this
         *         code runs.
         *
         * Input : NONE
         * Output: 0(if running on CPU0)/1 (if running on CPU1)
         * Uses  : r3, r4
         */
        .global frcWhoAmI
frcWhoAmI:

        lis     r4, INTERNAL_BASE
        ori     r4, r4, SMP_WHO_AM_I
        lwbrx   r3,0,(r4)
        sync
        blr

#ifdef PMC280_DUAL_CPU

        .global frcEnableMMU
frcEnableMMU:

        mfmsr   r4
        ori     r4, r4, _PPC_MSR_IR|_PPC_MSR_DR  /* set the IR bit of the MSR */
        mtmsr   r4
        isync
        sync
        blr

        .global frcEnableMMUAndResetStackPointer
frcEnableMMUAndResetStackPointer:
        mfmsr   r4
        ori     r4, r4, _PPC_MSR_IR|_PPC_MSR_DR  /* set the IR bit of the MSR */
        mtmsr   r4
        isync
        sync
        LOADPTR (sp, STACK_ADRS)
        blr

        /*
         * Name  : frcWhoAmI
         *
         *         This function returns the ID of the CPU on which this
         *         code runs.
         *
         * Input : NONE
         * Output: 0(if running on CPU0)/1 (if running on CPU1)
         * Uses  : r3, r4
         */
/*	.global	frcWhoAmI
frcWhoAmI:

        lis     r4, INTERNAL_BASE
        ori     r4, r4, SMP_WHO_AM_I
        lwbrx   r3,0,(r4)
        sync
        blr
*/
	.global	frcPermitCPU1ToBoot
frcPermitCPU1ToBoot:

        /* Permit CPU 1 to boot */
        lis     r3, INTERNAL_BASE
        ori     r3, r3, CPU_MASTER_CONTROL
        lwbrx   r4,0,(r3)

        /* r5 = r5 & (~0x00000200) since bit[9] = MaskBR1 */
        lis     r5, 0x0
        ori     r5, r5, 0x0200
        andc    r4, r4, r5
        stwbrx  r4, 0, (r3)                     // Store
        sync
        blr

        /* IBAT */
        .global read_ibat0u
read_ibat0u:
        mfspr   r3, 528
        blr

        .global read_ibat0l
read_ibat0l:
        mfspr   r3, 529
        blr

        .global read_ibat1u
read_ibat1u:
        mfspr   r3, 530
        blr

        .global read_ibat1l
read_ibat1l:
        mfspr   r3, 531
        blr

        .global read_ibat2u
read_ibat2u:
        mfspr   r3, 532
        blr

        .global read_ibat2l
read_ibat2l:
        mfspr   r3, 533
        blr

        .global read_ibat3u
read_ibat3u:
        mfspr   r3, 534
        blr

        .global read_ibat3l
read_ibat3l:
        mfspr   r3, 535
        blr

#ifdef PMC280_MMU_SUPPORT_FOR_MPC7447

        .global read_ibat4u
read_ibat4u:
        mfspr   r3, 560
        blr

        .global read_ibat4l
read_ibat4l:
        mfspr   r3, 561
        blr

        .global read_ibat5u
read_ibat5u:
        mfspr   r3, 562
        blr

        .global read_ibat5l
read_ibat5l:
        mfspr   r3, 563
        blr

        .global read_ibat6u
read_ibat6u:
        mfspr   r3, 564
        blr

        .global read_ibat6l
read_ibat6l:
        mfspr   r3, 565
        blr

        .global read_ibat7u
read_ibat7u:
        mfspr   r3, 566
        blr

        .global read_ibat7l
read_ibat7l:
        mfspr   r3, 567
        blr
#endif /* PMC280_MMU_SUPPORT_FOR_MPC7447 */

        /* DBAT */
        .global read_dbat0u
read_dbat0u:
        mfspr   r3, 536
        blr

        .global read_dbat0l
read_dbat0l:
        mfspr   r3, 537
        blr

        .global read_dbat1u
read_dbat1u:
        mfspr   r3, 538
        blr

        .global read_dbat1l
read_dbat1l:
        mfspr   r3, 539
        blr

        .global read_dbat2u
read_dbat2u:
        mfspr   r3, 540
        blr

        .global read_dbat2l
read_dbat2l:
        mfspr   r3, 541
        blr

        .global read_dbat3u
read_dbat3u:
        mfspr   r3, 542
        blr

        .global read_dbat3l
read_dbat3l:
        mfspr   r3, 543
        blr

#ifdef PMC280_MMU_SUPPORT_FOR_MPC7447

        .global read_dbat4u
read_dbat4u:
        mfspr   r3, 568
        blr

        .global read_dbat4l
read_dbat4l:
        mfspr   r3, 569
        blr

        .global read_dbat5u
read_dbat5u:
        mfspr   r3, 570
        blr

        .global read_dbat5l
read_dbat5l:
        mfspr   r3, 571
        blr

        .global read_dbat6u
read_dbat6u:
        mfspr   r3, 572
        blr

        .global read_dbat6l
read_dbat6l:
        mfspr   r3, 573
        blr

        .global read_dbat7u
read_dbat7u:
        mfspr   r3, 574
        blr

        .global read_dbat7l
read_dbat7l:
        mfspr   r3, 575
        blr

#endif /* PMC280_MMU_SUPPORT_FOR_MPC7447 */

#ifdef PMC280_DUAL_CPU_PTE_SETUP
/******************************************************************************
*
* frcmmuPpcSrSet - Set the value of a Segment Register (SR)
*
* This funstion set the value of the an Segment Register(SR). The segment
* register number is handles by <srNb> and the value by <value>.
*
* RETURNS: N/A

* void frcmmuPpcSrSet
*     (
*     UINT srNb,                        /@ segment register number to set @/
*     UINT value                        /@ value to place in the register @/
*     )
*/
        .global frcmmuPpcSrSet
frcmmuPpcSrSet:
        /*
         * The segment register number should be placed in the MSB of
         * the register
         */
        rlwinm  p0, p0, 28, 0, 3        /* shift the register number */
        isync                           /* instruction SYNCHronization */
        mtsrin  p1, p0                  /* set the register value
        isync                           /* instruction SYNCHronisation */
        blr

/******************************************************************************
*
* frcmmuPpcSrGet - Get the value of a Segment Register (SR)
*
* This routine returns the value handled by a Segment Register. The
* segment register number is handled by <srNb>.
*
* RETURNS: the Segment Register (SR) value.

* UINT frcmmuPpcSrGet
*     (
*     UINT srNb                 /@ Segment Register number to get the value @/
*     )
*/
        .global frcmmuPpcSrGet   
frcmmuPpcSrGet:
        /*
         * The segment register number should be placed in the MSB of
         * the register
         */
        rlwinm  p0, p0, 28, 0, 3        /* shft the register number */
        mfsrin  p0, p0                  /* get the register value */
        blr

/******************************************************************************
*
* frcmmuPpcSdr1Set - Set the SDR1 register
*
* This routine set the SDR1 register value. The value to set is handled by
* the <sdr1Value> input argument.
*
* Note: This function MUST Not be called if the Data and/or Instruction MMU
* are enabled. The results are undefined. See: "PowerPc Mircoprocessor Family:
* The Programming Environments page 2-40 * note Nb 5."
*
* RETURNS: N/A
* void frcmmuPpcSdr1Set
*     (
*     UINT      sdr1Value               /@ value to set in the SDR1 register @/
*/
        .global frcmmuPpcSdr1Set
frcmmuPpcSdr1Set:
        sync                            /* SYNChronization */
        mtspr   SDR1, p0                /* set the SDR1 with the P0 value */
        sync                            /* SYNChronization */
        blr

/******************************************************************************
*
* frcmmuPpcSdr1Get - Get the SDR1 register value
*
* This routine returns the value handled by the SDR1 register.
*
* RETURNS: N/A
* UINT frcmmuPpcSdr1Get (void)
*/
        .global frcmmuPpcSdr1Get
frcmmuPpcSdr1Get:
        mfspr   p0, SDR1                /* move the SDR1 value to P0 */
        blr

/******************************************************************************
*
* frcmmuPpcTlbie - Invalidate the PTE in the TLB
*/
        .global frcmmuPpcTlbie
frcmmuPpcTlbie:
        sync
        tlbie   p0
        sync
#if     ((CPU == PPC603) || (CPU == PPCEC603) || (CPU == PPC604))
#ifdef  _GREEN_TOOL
        .long   0x7c00046c              /* tlbsync */
#else
        tlbsync
#endif
        sync
#endif  /* ((CPU == PPC603) || (CPU == PPCEC603) || (CPU == PPC604)) */
        blr


#endif /* PMC280_DUAL_CPU_PTE_SETUP */

#endif /* PMC280_DUAL_CPU */

#ifdef PMC280_PCI_BOOT

#ifdef PMC280_DUAL_CPU
waitLine0:
        .ascii "\r\nPlease wait for approx. 40 seconds\r\n"
        .dc.b 0,0,0,0

waitLine1:
        .ascii "\r\nPlease wait for approx. 40 seconds\r\n"
        .dc.b 0,0,0,0
#else

waitLine:
        .ascii "\r\nPlease wait for approx. 15 seconds\r\n"
        .dc.b 0,0,0,0

#endif /* PMC280_DUAL_CPU */

#else

#ifdef PMC280_DUAL_CPU

waitLine0:
        .ascii "\r\nPlease wait for approx. 35 seconds\r\n"
        .dc.b 0,0,0,0
waitLine1:
        .ascii "\r\nPlease wait for approx. 35 seconds\r\n"
        .dc.b 0,0,0,0

#else

waitLine:
        .ascii "\r\nPlease wait for approx. 25 seconds\r\n"
        .dc.b 0,0,0,0

#endif /* PMC280_DUAL_CPU */

#endif /* PMC280_PCI_BOOT */

#ifdef PMC280_DEBUG_UART

OkayLine:
        .ascii "\r\nOK\r\n"
        .dc.b 0,0,0,0

serialInitCompleteLine:
        .ascii "\r\nCPU0: MPSC 0/1 initialisation complete\r\n"
        .dc.b 0,0,0,0

DdrInitCompleteLine:
        .ascii "CPU0: DDR initialisation complete\r\n"
        .dc.b 0,0,0,0

BATInitCompleteLine0:
        .ascii "CPU0: BAT initialisation complete\r\n"
        .dc.b 0,0,0,0

BATInitCompleteLine1:
        .ascii "\r\nCPU1: BAT initialisation complete\r\n"
        .dc.b 0,0,0,0

CentryLine0:
        .ascii "CPU0: Jumping to romStart\r\n"
        .dc.b 0,0,0,0

CentryLine1:
        .ascii "CPU1: Jumping to romStart\r\n"
        .dc.b 0,0,0,0
#endif /* PMC280_DEBUG_UART */
/* End Of File */
