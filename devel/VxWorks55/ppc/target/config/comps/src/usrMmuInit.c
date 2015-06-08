/* usrMmuInit.c - memory management unit initialization */

/* Copyright 1992-2002 Wind River Systems, Inc. */

/*
modification history
--------------------
01x,05jun02,hdn  added 36bit MMU support for P6 and P7
01w,17apr02,jtp  support PPC440 MMU
01v,27aug01,hdn  renamed mmuLibInit to mmuI86LibInit for PENTIUM2/3/4
01u,14aug01,hdn  added PENTIUM2/3/4 support
01t,26apr01,scm  remove xscale debug LED code...
01q,20jan99,cdp  prevent MMU initialisation when ARM_HAS_NO_MMU defined;
		 removed support for old ARM libraries.
01p,16nov98,cdp  added BSP selection of MMU for ARM. Added INCLUDE_MMU_MPU.
01n,10nov00,s_m  PPC405 support
01n,02mar00,zl   merged SH support into T2
01m,24aug98,tpr  added PowerPC EC 603 support.
01n,09apr98,hdn  added support for PentiumPro.
01m,28nov96,cdp  added ARM support.
01l,17jun96,tpr  added PowerPC 860 support.
01k,19sep95,tpr  added PowerPC support.
01j,06nov94,tmk  added MC68LC040 support.
01i,16jun94,tpr  added MC68060 cpu support.
01i,13nov93,hdn  added support for I80X86.
01i,02dec93,pme  added Am29K family support.
01h,30jul93,jwt  changed CPU to CPU_FAMILY for SPARC; include sun4.h to BSP.
01g,27feb93,rdc  removed check for both BASIC and FULL (done in usrDepend.c).
01f,13feb93,jcf  changed SPARC mmu initialization to utilize sysMmuLibInit.
01e,16oct92,jwt  included sun4.h to pull in prototype for mmuSun4LibInit().
01d,08oct92,rdc  ensured that both BASIC and FULL aren't set simultaneously.
01c,29sep92,ccc  renamed mmu1eLibInit() to mmuSun4LibInit().
01b,22sep92,rdc  cleanup, added support for sun1e.
01a,18sep92,jcf  written.
*/

/*
DESCRIPTION
This file is used to initialize the memory management unit.  This file is
included by usrConfig.c.

SEE ALSO: usrExtra.c

NOMANUAL
*/

#ifndef  __INCusrMmuInitc
#define  __INCusrMmuInitc 


#if defined(INCLUDE_MMU_BASIC) || defined(INCLUDE_MMU_FULL) || \
    defined(INCLUDE_MMU_MPU)

#if ((CPU == PPC601) || (CPU == PPC603) || (CPU == PPCEC603) || (CPU == PPC604))
#include "arch/ppc/mmuPpcLib.h"
#elif	(CPU == PPC860)
#include "arch/ppc/mmu800Lib.h"
#elif 	((CPU == PPC405) || (CPU == PPC405F))
#include "arch/ppc/mmu405Lib.h"
#elif 	((CPU == PPC440))
#include "arch/ppc/mmu440Lib.h"
#endif	/* (CPU == PPC*) */


/* externals */

IMPORT FUNCPTR sysMmuLibInit;		/* board-specific MMU library */
IMPORT PHYS_MEM_DESC sysPhysMemDesc [];
IMPORT int sysPhysMemDescNumEnt;
IMPORT UINT32 sysBatDesc [];
#if (CPU == PPC440)
IMPORT TLB_ENTRY_DESC sysStaticTlbDesc [];
IMPORT int sysStaticTlbDescNumEnt;
#endif

/*******************************************************************************
*
* usrMmuInit - initialize the memory management unit
*
* NOMANUAL
*/

void usrMmuInit (void)

    {
#if	((CPU == PPC601) || (CPU == PPC603) || (CPU == PPCEC603) || \
	 (CPU == PPC604) || (CPU == PPC860) || (CPU == PPC405) || \
	 (CPU == PPC405F) || (CPU == PPC440))
    int mmuType = 0;
#endif /* (CPU == PPC*) */


#if (CPU_FAMILY == SPARC)

    /* BSP selection of cache library */

    if ((sysMmuLibInit == NULL) || (((* sysMmuLibInit) (VM_PAGE_SIZE)) != OK))
	goto usrMmuPanic;
#endif /* CPU_FAMILY == SPARC */

#if (CPU==MC68020)				/* actually 68030 support */
    if (mmu30LibInit (VM_PAGE_SIZE) != OK)
	goto usrMmuPanic;
#endif /* CPU==MC68020 */

#if ((CPU==MC68040) || CPU==MC68060 || CPU==MC68LC040)
    if (mmu40LibInit (VM_PAGE_SIZE) != OK)
	goto usrMmuPanic;
#endif /* CPU==MC68040 || CPU==MC68060  || CPU==MC68LC040 */

#if (CPU_FAMILY==I80X86)
#   if (((CPU == PENTIUM2) && defined(INCLUDE_MMU_P6_32BIT)) || \
	((CPU == PENTIUM3) && defined(INCLUDE_MMU_P6_32BIT)) || \
	((CPU == PENTIUM4) && defined(INCLUDE_MMU_P6_32BIT)))

    if (mmuPro32LibInit (VM_PAGE_SIZE) != OK)
        goto usrMmuPanic;

#   ifdef INCLUDE_MMU_BASIC
    vmBaseArch32LibInit ();
#   else
    vmArch32LibInit ();
#   endif /* INCLUDE_MMU_BASIC */

#   elif (((CPU == PENTIUM2) && defined(INCLUDE_MMU_P6_36BIT)) || \
	  ((CPU == PENTIUM3) && defined(INCLUDE_MMU_P6_36BIT)) || \
	  ((CPU == PENTIUM4) && defined(INCLUDE_MMU_P6_36BIT)))

    if (mmuPro36LibInit (VM_PAGE_SIZE) != OK)
        goto usrMmuPanic;

#   ifdef INCLUDE_MMU_BASIC
    vmBaseArch36LibInit ();
#   else
    vmArch36LibInit ();
#   endif /* INCLUDE_MMU_BASIC */

#   else

    if (mmuI86LibInit (VM_PAGE_SIZE) != OK)
        goto usrMmuPanic;

#   endif /* (CPU == PENTIUMX) && defined (INCLUDE_MMU_P6_32BIT) */
#endif /* CPU==I80X86 */

#if (CPU==AM29030)
    if (mmuAm29kLibInit (VM_PAGE_SIZE) != OK)
	goto usrMmuPanic;
#endif /* CPU==AM29030 */

#if (CPU==SH7750)
    if (mmuSh7750LibInit (VM_PAGE_SIZE) != OK)
	goto usrMmuPanic;
#endif /* CPU==SH7750 */

#if (CPU==SH7729 || CPU==SH7700)
    if (mmuSh7700LibInit (VM_PAGE_SIZE) != OK)
	goto usrMmuPanic;
#endif /* CPU==SH7729 || CPU==SH7700 */

#if	((CPU == PPC601) || (CPU == PPC603) || (CPU == PPCEC603) || \
	 (CPU == PPC604) || (CPU == PPC860) || (CPU == PPC405) || \
	 (CPU == PPC405F) || (CPU == PPC440))
#ifdef	USER_I_MMU_ENABLE
    mmuType |= MMU_INST;		/* select the instruction MMU */
#endif
    
#ifdef	USER_D_MMU_ENABLE
    mmuType |= MMU_DATA;		/* select the DATA MMU */
#endif
    
#if ((CPU == PPC601) || (CPU == PPC603) || (CPU == PPCEC603) || (CPU == PPC604))
    if (mmuPpcLibInit (mmuType, (int *) &sysPhysMemDesc[0],
			sysPhysMemDescNumEnt, (int *) &sysBatDesc [0]) != OK)
	goto usrMmuPanic;
#elif	(CPU == PPC860)
    if (mmu800LibInit (mmuType) != OK)
	goto usrMmuPanic;
#elif	((CPU == PPC405) || (CPU == PPC405F))
    if (mmu405LibInit (mmuType) != OK)
	goto usrMmuPanic;
#elif	(CPU == PPC440)
    if (mmu440LibInit (mmuType, sysStaticTlbDescNumEnt, &sysStaticTlbDesc[0])
	    != OK)
	goto usrMmuPanic;
#endif	
#endif	/* (CPU == PPC*) */

#if (CPU_FAMILY == ARM)
#if !ARM_HAS_NO_MMU
    /* BSP selection of MMU library */

    if ((sysMmuLibInit == NULL) || (((* sysMmuLibInit) (VM_PAGE_SIZE)) != OK))
	goto usrMmuPanic;
#endif /* !ARM_HAS_NO_MMU */

#ifdef INCLUDE_MMU_MPU

    if ((vmMpuLibInit (VM_PAGE_SIZE) != OK) ||
	(vmMpuGlobalMapInit (&sysPhysMemDesc[0],
			     sysPhysMemDescNumEnt, TRUE) == NULL))
	goto usrMmuPanic;
#endif /* INCLUDE_MMU_MPU */
#endif /* (CPU_FAMILY == ARM) */


#if (CPU_FAMILY != ARM) || !(ARM_HAS_NO_MMU)
#ifdef INCLUDE_MMU_BASIC		/* XXX TPR */

    if (vmBaseLibInit (VM_PAGE_SIZE) != OK)
	goto usrMmuPanic;
    if (vmBaseGlobalMapInit (&sysPhysMemDesc[0],
			      sysPhysMemDescNumEnt, TRUE) == NULL)
	goto usrMmuPanic;

#endif /* INCLUDE_MMU_BASIC */

#ifdef INCLUDE_MMU_FULL			/* unbundled mmu product */

    if ((vmLibInit (VM_PAGE_SIZE) != OK) ||
        (vmGlobalMapInit (&sysPhysMemDesc[0],
			  sysPhysMemDescNumEnt, TRUE) == NULL))
	goto usrMmuPanic;

#ifdef INCLUDE_SHOW_ROUTINES 

    vmShowInit ();
#endif /* INCLUDE_SHOW_ROUTINES */

#endif /* INCLUDE_MMU_FULL */
#endif /* CPU_FAMILY != ARM || !(ARM_HAS_NO_MMU) */

    return;

#if (CPU_FAMILY != ARM) || !(ARM_HAS_NO_MMU) 
usrMmuPanic:
    printExc ("usrRoot: MMU configuration failed, errno = %#x", errno, 0,0,0,0);
    reboot (BOOT_WARM_NO_AUTOBOOT);
#endif /* CPU_FAMILY != ARM || !(ARM_HAS_NO_MMU) */
    }

#endif	/* defined(INCLUDE_MMU_BASIC, INCLUDE_MMU_FULL, INCLUDE_MMU_MPU) */
#endif	/* __INCusrMmuInitc */
