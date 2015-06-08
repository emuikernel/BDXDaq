/******************************************************************************
*              (c), Copyright, MOTOROLA  INC - ECC                  *
*                                                                             *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MOTOROLA.		      *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT *
* OF MOTOROLA OR ANY THIRD PARTY. MOTOROLA RESERVES THE RIGHT AT ITS SOLE     *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MOTOROLA.   *
* THIS CODE IS PROVIDED "AS IS". MOTOROLA MAKES NO WARRANTIES, EXPRESS,	      *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.  *
*******************************************************************************/
/* bootInit.c - ROM initialization module */

/* Copyright 1989-2002 Wind River Systems, Inc. */
#include "copyright_wrs.h"

/*
modification history
--------------------
05h,26mar02,sbs  correcting data section relocation for ROM compressed images
                 (SPR 73785)
05g,19mar02,dat  Removing previous fix for MIPS, no longer needed (72825)
05f,18jan02,dat  Don't use loop unrolling for MIPS
05f,22jan02,scm  Xscale specific validation added to update cache (to match
                 updates...)
05e,19dec01,aeg  prevented pc-relative addressing on MC680X0.
05d,10dec01,sbs  Corrected conditions around call to copyLongs for ROM 
                 resident images.
05c,27nov01,sbs  Added new labels, wrs_kernel_data_start and wrs_kernel_data_end
                 for sdata and edata respectively. Added second copy for data 
		 section of compressed ROM images. Corrected definition for 
                 binArrayEnd. Corrected definition for RESIDENT_DATA.
05b,27nov01,tpw  Manually unroll copyLongs and fillLongs.
05a,25oct01,pad  Removed definitions of _binArrayStart and _binArrayEnd, now
                 unnecessary.
04z,03oct01,dee  Merge from ColdFire T2.1.0 release
04y,11jan01,scm  Xscale specific validation added to update cache, can not assume uncached area....
04x,10jun99,jpd  fix error when BOOTCODE_IN_RAM defined (SPR #27775).
04w,13nov98,cdp  make Thumb support for ARM CPUs dependent on ARM_THUMB.
04w,10feb99,db   Fix to ensure that default bootline gets copied for
		 standalone and rom-resident images(SPR #21763).
04v,05oct98,jmp  doc: cleanup.
04u,17apr98,cdp  backed out 04t and made absEntry volatile for ARM.
04t,16apr98,cdp  for ARM, make UNCOMPRESS entry point in RAM.
04s,20mar98,cdp  make ROM_COPY_SIZE subject to #ifndef.
04r,11nov97,cdp  ARM7TDMI_T: force romStart to call entry point in Thumb state.
		 (SPR# 9716)
04q,14jul97,tam  changed remaining references to bfillLong to fillLong. 
04p,12feb97,dat  Added USER_RESERVED_MEM, SYS_MEM_TOP, SYS_MEM_BOTTOM, SPR 8030
04o,04feb97,ms   fixed compiler warning about protoype for bcopyLongs.
04o,28nov96,cdp  added ARM support.
04n,03sep96,hdn  added the compression support for pc[34]86 BSP.
04m,19aug96,ms   added UNCMP_RTN macro to use inflate instead of uncompress
04l,21jun96,jmb  long modhist -- deleted entries prior to 1994.  SPR #6528
03k,10jun96,tam  added rom resident support for PPC architecture. 
03j,14may96,dat  fixed compiler warnings for copyLongs, fillLongs. SPR #6536
03i,06mar96,tpr  changed absEntry to be volatile for PowerPC.
03h,22aug95,hdn  added support for I80X86.
03g,14mar95,caf  restored mips resident rom support (SPR #3856).
03f,16feb95,jdi  doc format change.
03f,23may95,yao  define binArrayStart and binArrayEnd for PowerPC
		 because tools don't prepend "_".
03e,09dec94,caf  undid mod 03a, use sdata for resident roms (SPR #3856).
03d,22jun94,caf  undid 16-byte alignment portion of mod 03c, below.
03c,14jun94,cd   corrected definitions of etext, edata and end.
	   +caf  for R4000 resident ROMs: data starts on 16-byte boundary.
		 for R4000 uncompressed ROMs: added volatile to absEntry type.
*/

/*
DESCRIPTION
This module provides a generic boot ROM facility.  The target-specific
romInit.s module performs the minimal preliminary board initialization and
then jumps to the C routine romStart().  This routine, still executing out
of ROM, copies the first stage of the startup code to a RAM address and
jumps to it.  The next stage clears memory and then uncompresses the
remainder of ROM into the final VxWorks ROM image in RAM.

A modified version of the Public Domain \f3zlib\fP library is used to
uncompress the VxWorks boot ROM executable linked with it.  Compressing
object code typically achieves over 55% compression, permitting much
larger systems to be burned into ROM.  The only expense is the added few
seconds delay while the first two stages complete.

ROM AND RAM MEMORY LAYOUT
Example memory layout for a 1-megabyte board:
.CS
    --------------  0x00100000 = LOCAL_MEM_SIZE = sysMemTop()
    |            |
    |    RAM     |
    |  0 filled  |
    |            |
    |------------| = (romInit+ROM_COPY_SIZE) or binArrayStart
    | ROM image  |
    |----------- |  0x00090000  = RAM_HIGH_ADRS
    | STACK_SAVE |
    |------------|
    |            |  0x00080000  = 0.5 Megabytes
    |            |
    |            |
    | 0 filled   |
    |            |
    |            |  0x00001000  = RAM_ADRS & RAM_LOW_ADRS
    |            |
    |            |  exc vectors, bp anchor, exc msg, bootline
    |            |
    |            |
    --------------  0x00000000  = LOCAL_MEM_LOCAL_ADRS
.CE
.CS
    --------------
    |    ROM     |
    |            |  0xff8xxxxx  = binArrayStart
    |            |
    |            |  0xff800008  = ROM_TEXT_ADRS
    --------------  0xff800000  = ROM_BASE_ADRS
.CE

SEE ALSO:
inflate(), romInit(), and deflate

AUTHOR
The original compression software for zlib was written by Jean-loup Gailly
and Mark Adler. See the manual pages of inflate and deflate for
more information on their freely available compression software.
*/

#include "vxWorks.h"
#include "sysLib.h"
#include "config.h"
#include "errno.h"
#include "sioLib.h"

#ifdef PMC280_DUAL_CPU
#include "frcMmu.c"
#else
/*#include "dbg_vsprintf.c"*/
#include "gtCore.h"
#endif /* PMC280_DUAL_CPU */

#ifdef PMC280_DEBUG_UART
#include "stdarg.h"
#include "dbgprints.h"
#endif /* PMC280_DEBUG_UART */

#define	UNCMP_RTN	inflate

#ifndef USER_RESERVED_MEM
#   define USER_RESERVED_MEM 0
#endif

#define BINARRAYEND_ROUNDOFF	(ROUND_DOWN(binArrayEnd, sizeof(long))) 
#define BANK0_SIZE_REG      DMA_CHANNEL0_SOURCE_ADDR
#define BANK1_SIZE_REG      DMA_CHANNEL0_BYTE_COUNT
#define DMA_CHANNEL0_SOURCE_ADDR     0x810
#define BYTE_SWAP_32(data) \
((((data) & 0x000000ff) << 24) | (((data) & 0x0000ff00) << 8) | (((data) & 0x00ff0000) >> 8) | \
   (((data) & 0xff000000) >> 24))

IMPORT void	romInit ();
IMPORT STATUS	UNCMP_RTN ();
IMPORT void	usrInit ();
IMPORT void	sysInitAlt ();
IMPORT void	start ();

IMPORT UCHAR	binArrayStart [];	/* compressed binary image */
IMPORT UCHAR	binArrayEnd [];		/* end of compressed binary image */
IMPORT char	etext [];		/* defined by the loader */
IMPORT char	end [];			/* defined by the loader */
IMPORT UCHAR	wrs_kernel_data_start [];  /* defined by the loader */
IMPORT UCHAR	wrs_kernel_data_end [];    /* defined by the loader */

#if	((CPU_FAMILY == MIPS) || (CPU_FAMILY==PPC) || \
	 (CPU_FAMILY==COLDFIRE))
#define	RESIDENT_DATA	RAM_DST_ADRS
#else
#define	RESIDENT_DATA 	wrs_kernel_data_start	
#endif

#ifndef RAM_DST_ADRS                	/* default uncompress dest. */
#define RAM_DST_ADRS        RAM_HIGH_ADRS
#endif

/* If the boot code is in RAM and the RAM is already initialized,
 * clearing the RAM is not necessary.  Macro BOOTCODE_IN_RAM is
 * used not to clear the RAM.
 */
#ifdef	BOOTCODE_IN_RAM			/* not to clear RAM */
#undef	ROM_TEXT_ADRS
#undef	ROM_BASE_ADRS
#define	ROM_TEXT_ADRS	((UINT)romInit)
#define	ROM_BASE_ADRS	((UINT)romInit)
#endif	/* BOOTCODE_IN_RAM */

/* #if	defined (UNCOMPRESS) || defined (ROM_RESIDENT) */
#ifndef ROM_COPY_SIZE
#define	ROM_COPY_SIZE	(ROM_SIZE - (ROM_TEXT_ADRS - ROM_BASE_ADRS))
#endif
/* #endif*/	/* UNCOMPRESS */

#define ROM_OFFSET(adr)	(((UINT)adr - (UINT)romInit) + ROM_TEXT_ADRS)

/* forward declarations */

LOCAL void copyLongs (FAST UINT *source, FAST UINT *destination, UINT nlongs);
LOCAL UINT32 getSdramSize(void); 
#if (CPU==XSCALE)
int checkLongs (FAST UINT *source, FAST UINT *destination, UINT nlongs);
#endif

/*******************************************************************************
*
* romStart - generic ROM initialization
*
* This is the first C code executed after reset.
*
* This routine is called by the assembly start-up code in romInit().
* It clears memory, copies ROM to RAM, and possibly invokes the uncompressor.
* It then jumps to the entry point of the uncompressed object code.
*
* RETURNS: N/A
*/

void romStart
    (
    FAST int startType		/* start type */
    )

    {
#if ((CPU_FAMILY==SPARC) || (CPU_FAMILY==MIPS) || (CPU_FAMILY==I80X86) || \
     (CPU_FAMILY==PPC) || (CPU_FAMILY==ARM))
    volatile			/* to force absolute adressing */
#endif /* (CPU_FAMILY==SPARC) */
    FUNCPTR absEntry;		/* to avoid PC Relative Jump Subroutine */
#if (CPU_FAMILY==ARM) && (!defined(ROM_RESIDENT)) && !defined(BOOTCODE_IN_RAM)
    VOIDFUNCPTR ramfillLongs = fillLongs;     /* force call to RAM */
#define fillLongs(a,b,c) ramfillLongs(a,b,c)
#endif  /* (CPU_FAMILY==ARM) */
#if (CPU_FAMILY==MC680X0) && !defined(ROM_RESIDENT) && !defined(BOOTCODE_IN_RAM)
    volatile VOIDFUNCPTR romcopyLongs = &copyLongs;  /* force call to ROM */
#define copyLongs romcopyLongs
#endif /* (CPU_FAMILY==MC680X0) */

    sdramSize = getSdramSize();  

#ifdef PMC280_DUAL_CPU
    /*    
     * Enable MMU if we are CPU1. This is necessary before the copy of
     * of code to system memory.
     */   
    if(frcWhoAmI())
    {
#ifdef PMC280_DUAL_CPU_PTE_SETUP
        frcSetupPTEs();
#endif /* PMC280_DUAL_CPU_PTE_SETUP */
 
        frcEnableMMUAndResetStackPointer();
    }
#endif /* PMC280_DUAL_CPU */
 

    /*
     * Copy from ROM to RAM, minus the compressed image
     * if compressed boot ROM which relies on binArray
     * appearing last in DATA segment.
     */

#ifdef ROM_RESIDENT
dg
    /* If ROM resident code, then copy only data segment
     * from ROM to RAM, initialize memory and jump
     * to usrInit.
     */

    
#if  (CPU_FAMILY == SPARC)
    copyLongs ((UINT *)(etext + 8), (UINT *) RESIDENT_DATA,
#else
    copyLongs ((UINT *)etext, (UINT *) RESIDENT_DATA,
#endif
        ((UINT) wrs_kernel_data_end - (UINT) RESIDENT_DATA) / sizeof (long));

#else	/* ROM_RESIDENT */

#ifdef UNCOMPRESS
#if	(CPU_FAMILY == MIPS)
    /*
     * copy text to uncached locations to avoid problems with
     * copy back caches
     */
    ((FUNCPTR)ROM_OFFSET(copyLongs)) (ROM_TEXT_ADRS, (UINT)K0_TO_K1(romInit),
		ROM_COPY_SIZE / sizeof (long));
#else	/* CPU_FAMILY == MIPS */
    ((FUNCPTR)ROM_OFFSET(copyLongs)) (ROM_TEXT_ADRS, (UINT)romInit,
		ROM_COPY_SIZE / sizeof (long));
#endif	/* CPU_FAMILY == MIPS */

#else	/* UNCOMPRESS */

#if	(CPU_FAMILY == MIPS)
    /*
     * copy text to uncached locations to avoid problems with
     * copy back caches
     * copy the entire data segment because there is no way to ensure that
     * binArray is the last thing in the data segment because of GP relative
     * addressing
     */
    ((FUNCPTR)ROM_OFFSET(copyLongs)) (ROM_TEXT_ADRS, (UINT)K0_TO_K1(romInit),
		((UINT)wrs_kernel_data_end - (UINT)romInit) / sizeof (long));
#else	/* CPU_FAMILY == MIPS */
    ((FUNCPTR)ROM_OFFSET(copyLongs)) (ROM_TEXT_ADRS, (UINT)romInit,
		((UINT)binArrayStart - (UINT)romInit)/ sizeof (long));

    ((FUNCPTR)ROM_OFFSET(copyLongs)) 
           ((UINT *)((UINT)ROM_TEXT_ADRS + ((UINT)BINARRAYEND_ROUNDOFF - 
           (UINT)romInit)), (UINT *)BINARRAYEND_ROUNDOFF, 
           ((UINT)wrs_kernel_data_end - (UINT)binArrayEnd) / sizeof (long));

#if (CPU==XSCALE)
    /* validate coherence, can not assume uncached area... */
    ((FUNCPTR)ROM_OFFSET(checkLongs))
                 (ROM_TEXT_ADRS, (UINT)romInit,
                 ((UINT)binArrayStart - (UINT)romInit) / sizeof (long));

    ((FUNCPTR)ROM_OFFSET(checkLongs))
           ((UINT *)((UINT)ROM_TEXT_ADRS + ((UINT)BINARRAYEND_ROUNDOFF - 
            (UINT)romInit)), (UINT *)BINARRAYEND_ROUNDOFF,
            ((UINT)wrs_kernel_data_end - (UINT)binArrayEnd) / sizeof (long));
#endif
#endif	/* CPU_FAMILY == MIPS */

#endif	/* UNCOMPRESS */
#endif	/* ROM_RESIDENT */

#ifdef PMC280_DEBUG_UART
#ifdef PMC280_DUAL_CPU
    if(frcWhoAmI())
    {
        dbg_puts1("CPU1: Copied VxWorks from ROM to RAM...\n");
    }
    else
    {
        dbg_puts0("CPU0: Copied VxWorks from ROM to RAM...\n");
    }
#else
    dbg_puts0("Copied VxWorks from ROM to RAM...\n");
#endif /* PMC280_DUAL_CPU */
#endif /* PMC280_DEBUG_UART */
 

#ifdef PMC280_DUAL_CPU
#ifdef PMC280_DUAL_CPU_PTE_SETUP
    if(!frcWhoAmI())
    {
        frcSysPgtMemoryInit();
        frcSetupPTEs();
    }
#endif /* PMC280_DUAL_CPU_PTE_SETUP */
#endif /* PMC280_DUAL_CPU */

#ifdef PMC280_DUAL_CPU
    if(!frcWhoAmI())
    {
        frcEnableMMU();
    }
#endif /* PMC280_DUAL_CPU */


#ifdef PMC280_DUAL_CPU_DBG
    if(frcWhoAmI())
    {
        /* IBAT */
        data = read_ibat0u();
        dbg_printregval(data, 1);
        data = read_ibat0l();
        dbg_printregval(data, 1);

        data = read_ibat1u();
        dbg_printregval(data, 1);
        data = read_ibat1l();
        dbg_printregval(data, 1);

        data = read_ibat2u();
        dbg_printregval(data, 1);
        data = read_ibat2l();
        dbg_printregval(data, 1);

        data = read_ibat3u();
        dbg_printregval(data, 1);
        data = read_ibat3l();
        dbg_printregval(data, 1);

#ifdef PMC280_MMU_SUPPORT_FOR_MPC7447
        data = read_ibat4u();
        dbg_printregval(data, 1);
        data = read_ibat4l();
        dbg_printregval(data, 1);
 
        data = read_ibat5u();
        dbg_printregval(data, 1);
        data = read_ibat5l();
        dbg_printregval(data, 1);
 
        data = read_ibat6u();
        dbg_printregval(data, 1);
        data = read_ibat6l();
        dbg_printregval(data, 1);
 
        data = read_ibat7u();
        dbg_printregval(data, 1);
        data = read_ibat7l();
        dbg_printregval(data, 1);
#endif
 
        /* DBAT */
        data = read_dbat0u();
        dbg_printregval(data, 1);
        data = read_dbat0l();
        dbg_printregval(data, 1);
 
        data = read_dbat1u();
        dbg_printregval(data, 1);
        data = read_dbat1l();
        dbg_printregval(data, 1);
 
        data = read_dbat2u();
        dbg_printregval(data, 1);
        data = read_dbat2l();
        dbg_printregval(data, 1);
 
        data = read_dbat3u();
        dbg_printregval(data, 1);
        data = read_dbat3l();
        dbg_printregval(data, 1);
 
#ifdef PMC280_MMU_SUPPORT_FOR_MPC7447
        data = read_dbat4u();
        dbg_printregval(data, 1);
        data = read_dbat4l();
        dbg_printregval(data, 1);
 
        data = read_dbat5u();
        dbg_printregval(data, 1);
        data = read_dbat5l();
        dbg_printregval(data, 1);
 
        data = read_dbat6u();
        dbg_printregval(data, 1);
        data = read_dbat6l();
        dbg_printregval(data, 1);
 
        data = read_dbat7u();
        dbg_printregval(data, 1);
        data = read_dbat7l();
        dbg_printregval(data, 1);
#endif
    }
    else
    {
        /* IBAT */
        data = read_ibat0u();
        dbg_printregval(data, 0);
        data = read_ibat0l();
        dbg_printregval(data, 0);
 
        data = read_ibat1u();
        dbg_printregval(data, 0);
        data = read_ibat1l();
        dbg_printregval(data, 0);
 
        data = read_ibat2u();
        dbg_printregval(data, 0);
        data = read_ibat2l();
        dbg_printregval(data, 0);
 
        data = read_ibat3u();
        dbg_printregval(data, 0);
        data = read_ibat3l();
        dbg_printregval(data, 0);
 
#ifdef PMC280_MMU_SUPPORT_FOR_MPC7447
        data = read_ibat4u();
        dbg_printregval(data, 0);
        data = read_ibat4l();
        dbg_printregval(data, 0);
 
        data = read_ibat5u();
        dbg_printregval(data, 0);
        data = read_ibat5l();
        dbg_printregval(data, 0);
 
        data = read_ibat6u();
        dbg_printregval(data, 0);
        data = read_ibat6l();
        dbg_printregval(data, 0);
 
        data = read_ibat7u();
        dbg_printregval(data, 0);
        data = read_ibat7l();
        dbg_printregval(data, 0);
#endif
 
        /* DBAT */
        data = read_dbat0u();
        dbg_printregval(data, 0);
        data = read_dbat0l();
        dbg_printregval(data, 0);
 
        data = read_dbat1u();
        dbg_printregval(data, 0);
        data = read_dbat1l();
        dbg_printregval(data, 0);
 
        data = read_dbat2u();
        dbg_printregval(data, 0);
        data = read_dbat2l();
        dbg_printregval(data, 0);
 
        data = read_dbat3u();
        dbg_printregval(data, 0);
        data = read_dbat3l();
        dbg_printregval(data, 0);
 
#ifdef PMC280_MMU_SUPPORT_FOR_MPC7447
        data = read_dbat4u();
        dbg_printregval(data, 0);
        data = read_dbat4l();
        dbg_printregval(data, 0);
 
        data = read_dbat5u();
        dbg_printregval(data, 0);
        data = read_dbat5l();
        dbg_printregval(data, 0);
 
        data = read_dbat6u();
        dbg_printregval(data, 0);
        data = read_dbat6l();
        dbg_printregval(data, 0);
 
        data = read_dbat7u();
        dbg_printregval(data, 0);
        data = read_dbat7l();
        dbg_printregval(data, 0);
#endif
    }
      
#endif /* PMC280_DUAL_CPU_DBG */

#if	(CPU_FAMILY != MIPS) && (!defined (BOOTCODE_IN_RAM))

    /* clear all memory if cold booting */

    if (startType & BOOT_CLEAR)
	{

#ifdef PMC280_DEBUG_UART
#ifdef PMC280_DUAL_CPU
        if(frcWhoAmI())
        { 
            dbg_puts1("CPU1: Zeroed low and high memory.\n");
        }
        else
        {
            dbg_puts0("CPU0: Zeroed low and high memory.\n");
        }
#else     
        dbg_puts0("Zeroed low and high memory.\n");
#endif /* PMC280_DUAL_CPU */
#endif /* PMC280_DEBUG_UART */

	/* 
	 * Ensure the boot line is null. This is necessary for those
	 * targets whose boot line is excluded from cleaning.
	 */

	*(BOOT_LINE_ADRS) = EOS;
	}

#endif	/* (CPU_FAMILY != MIPS) && (!defined (BOOTCODE_IN_RAM)) */

    /* jump to VxWorks entry point (after uncompressing) */

#if	defined (UNCOMPRESS) || defined (ROM_RESIDENT)
#if	(CPU_FAMILY == I960)
    absEntry = (FUNCPTR)sysInitAlt;			/* reinit proc tbl */
#else
    absEntry = (FUNCPTR)usrInit;			/* on to bootConfig */
#endif	/* CPU_FAMILY == I960 */

#else
    {
#if	(CPU_FAMILY == MIPS)
    volatile FUNCPTR absUncompress = (FUNCPTR) UNCMP_RTN;
    if ((absUncompress) ((UCHAR *)ROM_OFFSET(binArrayStart),
			 (UCHAR *)K0_TO_K1(RAM_DST_ADRS),
			 (int)((UINT)binArrayEnd - (UINT)binArrayStart)) != OK)
#elif	(CPU_FAMILY == I80X86) || (CPU_FAMILY == ARM)
    volatile FUNCPTR absUncompress = (FUNCPTR) UNCMP_RTN;
    if ((absUncompress) ((UCHAR *)ROM_OFFSET(binArrayStart),
	            (UCHAR *)RAM_DST_ADRS, binArrayEnd - binArrayStart) != OK)
#else
    if (UNCMP_RTN ((UCHAR *)ROM_OFFSET(binArrayStart),
	            (UCHAR *)RAM_DST_ADRS, binArrayEnd - binArrayStart) != OK)
#endif	/* (CPU_FAMILY == MIPS) */

#ifdef PMC280_DEBUG_UART
#ifdef PMC280_DUAL_CPU
    {
        if(frcWhoAmI())
            dbg_puts1("Uncompress failed\n");
        else
            dbg_puts0("Uncompress failed\n");
        return;         /* if we return then ROM's will halt */
    }     
#else
    {
        dbg_puts0("Uncompress failed\n");
        return;         /* if we return then ROM's will halt */
    }     
#endif /* PMC280_DUAL_CPU */
#else
        return;         /* if we return then ROM's will halt */
#endif /* PMC280_DEBUG_UART */
 
#ifdef PMC280_DEBUG_UART
#ifdef PMC280_DUAL_CPU
    if(frcWhoAmI())
    {
        dbg_puts1("CPU1: Uncompressing VxWorks done.\n");
    }
    else
    {
        dbg_puts0("CPU0: Uncompressing VxWorks done.\n");
    }
#else
    dbg_puts0("Uncompressing VxWorks done.\n");
#endif /* PMC280_DUAL_CPU */
#endif /* PMC280_DEBUG_UART */
 
    absEntry = (FUNCPTR)RAM_DST_ADRS;		/* compressedEntry () */
    }
#endif	/* defined UNCOMPRESS || defined ROM_RESIDENT */

#if	((CPU_FAMILY == ARM) && ARM_THUMB)
    absEntry = (FUNCPTR)((UINT32)absEntry | 1);		/* force Thumb state */
#endif	/* CPU_FAMILY == ARM */

    (absEntry) (startType);
    }

#if     (CPU_FAMILY==ARM) && (!defined(ROM_RESIDENT))
#undef fillLongs
#endif  /* (CPU_FAMILY==ARM) */


#if (CPU_FAMILY==MC680X0) && !defined(ROM_RESIDENT) && !defined(BOOTCODE_IN_RAM)
#undef copyLongs	/* undo effects from above define */
#endif /* CPU_FAMILY==MC680X0 */

/*******************************************************************************
*
* copyLongs - copy one buffer to another a long at a time
*
* This routine copies the first <nlongs> longs from <source> to <destination>.
*/

LOCAL void copyLongs (source, destination, nlongs)
    FAST UINT *source;		/* pointer to source buffer      */
    FAST UINT *destination;	/* pointer to destination buffer */
    UINT nlongs;		/* number of longs to copy       */

    {
    FAST UINT *dstend = destination + nlongs;
    FAST UINT nchunks=0;

    /* Hop by chunks of longs, for speed. */
    for (nchunks = nlongs / 8; nchunks; --nchunks)
	{
#if (CPU_FAMILY == MC680X0)
	*destination++ = *source++;	/* 0 */
	*destination++ = *source++;	/* 1 */
	*destination++ = *source++;	/* 2 */
	*destination++ = *source++;	/* 3 */
	*destination++ = *source++;	/* 4 */
	*destination++ = *source++;	/* 5 */
	*destination++ = *source++;	/* 6 */
	*destination++ = *source++;	/* 7 */
#else
	destination[0] = source[0];
	destination[1] = source[1];
	destination[2] = source[2];
	destination[3] = source[3];
	destination[4] = source[4];
	destination[5] = source[5];
	destination[6] = source[6];
	destination[7] = source[7];
	destination += 8, source += 8;
#endif /* CPU_FAMILY == MC680X0 */
	}

    /* Do the remainder one long at a time. */
    while (destination < dstend)
	*destination++ = *source++;
    }


#if (CPU==XSCALE)
int checkLongs (source, destination, nlongs)
    FAST UINT *source;          /* pointer to source buffer      */
    FAST UINT *destination;     /* pointer to destination buffer */
    UINT nlongs;                /* number of longs to copy       */

    {
    int fine = 1;

    FAST UINT *dstend = destination + nlongs;

    while (destination < dstend)
        {
        if (*destination++ != *source++)
            {
            fine = 0;
            break;
            }
        }
    return fine;
    }
#endif

#ifdef PMC280_DEBUG_UART
void dbg_puts0(const char *s)
{
    while(*s)
    {
        dbg_putc0(*s++);
    }
}

void dbg_putc0(char c)
{
    if(c == '\n')
        _dbg_transmit0('\r');
    _dbg_transmit0(c);
}

void dbg_puts1(const char *s)
{
    while(*s)
    {
        dbg_putc1(*s++);
    }
}

void dbg_putc1(char c)
{
    if(c == '\n')
        _dbg_transmit1('\r');
    _dbg_transmit1(c);
}

void dbg_printf0(const char *fmt, ...)
{
        va_list args;
        
        char    printbuffer[512];
          
        va_start(args, fmt);
 
        /*
         * For this to work, printbuffer must be larger than
         * anything we ever want to print.
         */
       /* i = dbg_vsprintf(printbuffer, fmt, args);*/
        va_end(args);
          
        /* Print the string */
        dbg_puts0(printbuffer);
}         
 
void dbg_printf1(const char *fmt, ...)
{
        va_list args;
       
        char    printbuffer[512];
          
        va_start(args, fmt);
 
        /*
         * For this to work, printbuffer must be larger than
         * anything we ever want to print.
         */
       /* i = dbg_vsprintf(printbuffer, fmt, args);*/
        va_end(args);
          
        /* Print the string */
        dbg_puts1(printbuffer);
}
#endif /* PMC280_DEBUG_UART */
 
#ifdef PMC280_DUAL_CPU_DBG
void dbg_printregval(unsigned int regval, unsigned int id)
{
    int i=0;
    char chr=0;
 
    if(id)
    {
        _dbg_transmit1(0x0D);
        _dbg_transmit1(0x0A);
    }
    else
    {
        _dbg_transmit0(0x0D);
        _dbg_transmit0(0x0A);
    }
 
    for(i=7;i>=0;i--)
    {
        chr = (regval >> (i*4)) & 0x0F;
        if(chr > 0x9)
            chr = chr - 0xA + 0x41;
        else
            chr = chr + 0x30;
 
        if(id)
        {
            _dbg_transmit1(chr);
        }
        else
        {
            _dbg_transmit0(chr);
        }
    }
}
#endif /* PMC280_DUAL_CPU_DBG */

UINT32 getSdramSize(void) {
    unsigned int bank0size=0, bank1size=0;
    bank0size=BYTE_SWAP_32(*(UINT32 *)(INTERNAL_REG_ADRS | BANK0_SIZE_REG));
    bank1size=BYTE_SWAP_32(*(UINT32 *)(INTERNAL_REG_ADRS | BANK1_SIZE_REG));
    return((bank0size + bank1size) * _1M);
} 
