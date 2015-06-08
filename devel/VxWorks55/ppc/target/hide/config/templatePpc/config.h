/* config.h - template configuration header file */

/* Copyright 1984-2002 Wind River Systems, Inc. */

/*
TODO -	Remove the template modification history and begin a new history
	starting with version 01a and growing the history upward with
	each revision.

modification history
--------------------
01y,18apr02,rhe  Added C++ Protection
01x,20dec01,cjj  Changed RAM_HIGH_ADRS to 0x200000
01w,09dec01,dat  Adding USB support for ARM, MIPS, PPC, PENTIUM, SH, added
		 optional TFFS support for all.
01v,15dec00,pai  Simplified the network configuration macros used in this file.
                 References to INCLUDE_BSD, INCLUDE_DEFER_NET_INIT, and
                 INCLUDE_NET_INIT have been removed.  This template config.h
                 now specifies INCLUDE_NETWORK and INCLUDE_END.  The remaining
                 network configuration comes from configAll.h.
01u,11dec00,pai  made END driver support the default for new WRN code.
01t,11dec00,pai  introduced INCLUDE_DEFER_NET_INIT configuration constant for
                 new WRN code.
01s,08apr99,dat  SPR 26491, fixed PCI macro names
01r,29mar99,dat  SPR 26125, added INCLUDE_BSD
01q,02feb99,tm   added PCI AutoConfig support to template BSPs (SPR 24733)
01p,28sep98,dat  added default PCI and VME macros
01o,07nov98,dat  added note about the values for ROM_TEXT_ADRS, and
		 ROM_WARM_ADRS
01n,21jul98,db   defined INCLUDE_NETWORK, changed ROM_SIZE to 0x40000.
01m,07jul98,db   changed BSP_VERSION to "1.2" and BSP_REVISION to "/0".
		 added BSP_VER_1_2 macro, defined INCLUDE_END, removed
		 END_OVERRIDE(Tornado 2.0 release).
01l,25aug97,dat  code review comments from Kitty
01k,07jul97,dat  added supported for new END ethernet drivers
01j,23apr97,dat  added ROM_WARM_ADRS, INCLUDE_CDROMFS
01i,02apr97,dat  more documentation about VME and other macros.
01h,12mar97,dat  added VME mapping macros
01g,10mar97,dat  comments from reviewers
01f,28feb97,dat  added INCLUDE_USER_APPL, USER_APPL_INIT
01e,05feb97,dat  incorporated comments from Thierry P.
01d,23jan97,dat  moved timer constants to template.h
01c,17jan97,dat  added INCLUDE_SERIAL, INCLUDE_MMU, INCLUDE_VME
01b,16jan97,dat  added LOCAL_MEM_AUTOSIZE and moved INCLUDE_SCSI2 outside
		 of #if FALSE.
01a,22oct96,ms   derived from the mv147 BSP
*/

/*
This file contains the configuration parameters for the template BSP.
This template starts simple - with most facilities excluded by default.

TODO -
Present the user with simple straight forward options.  Do not ask the
user to encode values together to form a value to be loaded in a register.
Let the source code, or the pre-processor, do the computational work to
determine what value to load in a particular register.
*/

#ifndef	INCconfigh
#define	INCconfigh

#ifdef __cplusplus
extern "C" {
#endif

/* BSP version/revision identification, before configAll.h */

#define BSP_VER_1_1	1	/* 1.2 is backward compatible with 1.1 */
#define BSP_VER_1_2	1
#define BSP_VERSION	"1.2"
#define BSP_REV		"/0"	/* 0 for first revision */

#include "configAll.h"		/* Set the VxWorks default configuration */

#define DEFAULT_BOOT_LINE \
	"ln(0,0)host:/usr/wpwr/target/config/template/vxWorks " \
	"h=90.0.0.3 e=90.0.0.50 u=username tn=targetname"

/* TODO - values in this header file are dummy values, update as needed */

/* memory configuration */

#define LOCAL_MEM_LOCAL_ADRS	0		/* fixed at zero */
#undef	LOCAL_MEM_AUTOSIZE			/* run-time memory sizing */
#define LOCAL_MEM_SIZE		0x400000	/* 4M memory */
#define	USER_RESERVED_MEM	0		/* see sysMemTop() */

/*
 * The constants ROM_TEXT_ADRS, ROM_SIZE, RAM_HIGH_ADRS, and RAM_LOW_ADRS
 * are defined in config.h and Makefile.
 * All definitions for these constants must be identical.
 *
 * TODO - Make sure these values match those in your Makefile.  The values
 * shown below are the defaults for a 68000 series system.  Pay particular
 * attention to ROM_TEXT_ADRS and ROM_WARM_ADRS.  The correct default values
 * are in the Makefile for each template BSP.
 */

#define ROM_BASE_ADRS		0xff800000	/* base address of ROM */
#define ROM_TEXT_ADRS		(ROM_BASE_ADRS+8) /* with PC & SP */
#define ROM_WARM_ADRS		(ROM_TEXT_ADRS+8) /* warm reboot entry */
#define ROM_SIZE		0x00040000	/* 256KB ROM space */
#define RAM_HIGH_ADRS		0x00200000	/* RAM address for bootrom */
#define RAM_LOW_ADRS		0x00010000	/* RAM address for vxWorks */

/* Serial port configuration */

#define	INCLUDE_SERIAL
#undef	NUM_TTY
#define	NUM_TTY			N_SIO_CHANNELS	/* defined in template.h */

/* Timer configuration */

#undef  INCLUDE_TIMESTAMP		/* no timestamp driver support */

/*
 * Cache configuration
 *
 * TODO -
 * All cache is turned off to begin with. Activate caching after basic
 * BSP functionality is achieved.  Start with instruction caching, followed
 * by data caching.  Begin without snooping options; and turn those on
 * later if the board supports it.
 *
 * Note that when MMU is enabled, cache modes are controlled by
 * the MMU table entries in sysPhysMemDesc[], not the cache mode
 * macros defined here.
 */

#undef	INCLUDE_CACHE_SUPPORT	/* no cacheLib support */
#undef	USER_I_CACHE_ENABLE
#undef	USER_I_CACHE_MODE
#define	USER_I_CACHE_MODE	CACHE_DISABLED
#undef	USER_D_CACHE_ENABLE
#undef	USER_D_CACHE_MODE
#define	USER_D_CACHE_MODE	CACHE_DISABLED
#undef	USER_B_CACHE_ENABLE

/* MMU configuration */

#undef	INCLUDE_MMU_BASIC	/* no MMU support */
#undef	INCLUDE_MMU_FULL	/* no MMU support */

/* Network driver configuration */

#define	INCLUDE_NETWORK
#define	INCLUDE_END		/* Enhanced Network Driver (see configNet.h) */

#undef	INCLUDE_EX
#undef	INCLUDE_ENP
#undef	INCLUDE_SM_NET

/* VME configuration */

/*
 * TODO -
 * Only those options that are actually user changeable should be defined
 * here.  Options that cannot be changed should be moved to template.h.
 */

#undef	INCLUDE_VME

/* A16 master window maps all of A16 to 0xff800000 locally */

#define	VME_A16_MSTR_BUS	0x0
#define	VME_A16_MSTR_SIZE	0x00010000
#define	VME_A16_MSTR_LOCAL	0xff800000

/* A24 master window maps all of A24 to 0xf8000000 locally */

#define	VME_A24_MSTR_BUS	0x0
#define	VME_A24_MSTR_SIZE	0x01000000
#define	VME_A24_MSTR_LOCAL	0xf8000000

/* A32 master window maps 32MB from bus 0x80000000 to 0x80000000 local. */

#define	VME_A32_MSTR_BUS	(0x80000000)
#define	VME_A32_MSTR_SIZE	(0x02000000)
#define	VME_A32_MSTR_LOCAL	(0x80000000)

/* A16 slave window is not supported */

#define	VME_A16_SLV_LOCAL	0x0
#define	VME_A16_SLV_BUS		0x0
#define	VME_A16_SLV_SIZE	0x0

/* A24 slave window maps 1st 8MB of local memory to bus address 0x00800000 */

#define	VME_A24_SLV_LOCAL	LOCAL_MEM_LOCAL_ADRS
#define	VME_A24_SLV_BUS		0x00800000
#define	VME_A24_SLV_SIZE	0x00800000

/* A32 slave window maps all local memory to bus address 0x80000000 */

#define	VME_A32_SLV_LOCAL	(LOCAL_MEM_LOCAL_ADRS)
#define	VME_A32_SLV_BUS		0x80000000
#define	VME_A32_SLV_SIZE	((UINT)sysPhysMemTop() - LOCAL_MEM_LOCAL_ADRS)

/* PCI configuration */

#undef	INCLUDE_PCI
#undef  INCLUDE_PCI_AUTOCONF

#ifndef PCI_CFG_TYPE
#   ifdef INCLUDE_PCI_AUTOCONF
#      define PCI_CFG_TYPE PCI_CFG_AUTO
#   else
#      define PCI_CFG_TYPE PCI_CFG_FORCE
#   endif /* INCLUDE_PCI_AUTOCONF */
#endif /* PCI_CFG_TYPE */
 

/*
 * TODO -
 * Only those options that are actually user changeable should be defined
 * here.  Options that cannot be changed should be moved to template.h.
 */

/* PCI I/O master window */

#define	PCI_MSTR_IO_LOCAL	0xff800000
#define	PCI_MSTR_IO_BUS		0x0
#define	PCI_MSTR_IO_SIZE	0x00010000

/* PCI MEMIO master (non-prefetch memory) window */

#define	PCI_MSTR_MEMIO_LOCAL	0xf8000000
#define	PCI_MSTR_MEMIO_BUS	0x0
#define	PCI_MSTR_MEMIO_SIZE	0x01000000

/* PCI MEM master (prefetchable memory) window */

#define	PCI_MSTR_MEM_LOCAL	0xfa000000
#define	PCI_MSTR_MEM_BUS	0x0
#define	PCI_MSTR_MEM_SIZE	0x01000000

/* A32 master window maps 32MB from bus 0x80000000 to 0x80000000 local. */

#define	VME_A32_MSTR_BUS	(0x80000000)
#define	VME_A32_MSTR_SIZE	(0x02000000)
#define	VME_A32_MSTR_LOCAL	(0x80000000)

/* A16 slave window is not supported */

#define	VME_A16_SLV_LOCAL	0x0
#define	VME_A16_SLV_BUS		0x0
#define	VME_A16_SLV_SIZE	0x0

/* A24 slave window maps 1st 8MB of local memory to bus address 0x00800000 */

#define	VME_A24_SLV_LOCAL	LOCAL_MEM_LOCAL_ADRS
#define	VME_A24_SLV_BUS		0x00800000
#define	VME_A24_SLV_SIZE	0x00800000

/* A32 slave window maps all local memory to bus address 0x80000000 */

#define	VME_A32_SLV_LOCAL	(LOCAL_MEM_LOCAL_ADRS)
#define	VME_A32_SLV_BUS		0x80000000
#define	VME_A32_SLV_SIZE	((UINT)sysPhysMemTop() - LOCAL_MEM_LOCAL_ADRS)

/* SCSI driver configuration */

#if	FALSE			/* change FALSE to TRUE for SCSI interface */
#define	INCLUDE_SCSI		/* include SCSI interface */
#define	INCLUDE_SCSI_DMA	/* include SCSI DMA interface */
#define	INCLUDE_SCSI_BOOT	/* include SCSI for boot option */
#define	INCLUDE_DOSFS		/* DOS file system */
#undef	INCLUDE_TAPEFS		/* tape file system */
#undef	INCLUDE_CDROMFS		/* CD-ROM (iso9660) file system */
#define SYS_SCSI_CONFIG		/* BSP scsi config routine */
#endif	/* FALSE/TRUE */

/* Select either SCSI-1 or SCSI-2, independent of INCLUDE_SCSI */

#define INCLUDE_SCSI2		/* undef for SCSI-1 support */

/*
 * User application initialization
 *
 * USER_APPL_INIT must be a valid C statement or block.  It is
 * included in the usrRoot() routine only if INCLUDE_USER_APPL is
 * defined.  The code for USER_APPL_INIT is only an example.  The
 * user is expected to change it as needed.  The use of taskSpawn
 * is recommended over direct execution of the user routine.
 */

#undef	INCLUDE_USER_APPL
#define	USER_APPL_INIT \
	{ \
	IMPORT int myAppInit(); \
	taskSpawn ("myApp", 30, 0, 5120, \
		   myAppInit, 0x1, 0x2, 0x3, 0,0,0,0,0,0,0); \
	}

/* Configurable interrupt vectors/levels (static defs go in template.h) */

#if	FALSE
#define	INT_VEC_ABORT		xxx
#define	INT_VEC_CLOCK		xxx
...
#define	INT_LVL_ABORT		xxx
#define	INT_LVL_CLOCK		xxx
...
#endif

/* Basic USB Support (PPC, PENTIUM, ARM, MIPS, SH only) */

#if FALSE	/* set TRUE to include USB support */

/* USB Host Components */

#   define INCLUDE_USB 			/* Main USB Component */
#   undef  INCLUDE_USB_INIT		/* USB Initialization */
#   define INCLUDE_UHCI			/* UHCI Controller Driver */
#   undef  INCLUDE_UHCI_INIT    	/* UHCI Initialization */
#   define INCLUDE_OHCI			/* OHCI Controller Driver */
#   undef  INCLUDE_OHCI_INIT		/* OHCI Initialization */
#   undef  INCLUDE_OHCI_PCI_INIT 	/* OHCI PCI Initialization */
#   define INCLUDE_USBTOOL		/* usbTool Application */
#   undef  INCDLUE_USB_AUDIO_DEMO	/* USB Audio Demo */
#   define INCLUDE_USB_MOUSE 		/* USB Mouse Driver */
#   undef  INCLUDE_USB_MOUSE_INIT 	/* Mouse Driver Initialization */
#   define INCLUDE_USB_KEYBOARD 	/* USB Keyboard Driver */
#   undef  INCLUDE_USB_KEYBOARD_INIT 	/* Keyboard Driver Initialization */
#   define INCLUDE_USB_PRINTER 		/* USB Printer Driver */
#   undef  INCLUDE_USB_PRINTER_INIT 	/* Printer Driver Initialization */
#   define INCLUDE_USB_SPEAKER 		/* USB Speaker Driver */
#   undef  INCLUDE_USB_SPEAKER_INIT 	/* Speaker Driver Initialization */
#   undef  INCLUDE_USB_MS_BULKONLY 	/* USB Bulk Driver */
#   undef  INCLUDE_USB_MS_BULKONLY_INIT /* Bulk Driver Initialization */
#   undef  INCLUDE_USB_MS_CBI 		/* USB CBI Driver */
#   undef  INCLUDE_USB_MS_CBI_INIT 	/* CBI Driver Initialization */
#   undef  INCLUDE_USB_PEGASUS_END 	/* USB Pegasus Network Driver */
#   undef  INCLUDE_USB_PEGASUS_END_INIT /* Pegaus Driver Initialization */

/*  USB Parameters */

#   define BULK_DRIVE_NAME "/bd" 		/* Bulk Drive Name */	
#   define CBI_DRIVE_NAME "/cbid" 		/* CBI Drive Name */
#   define PEGASUS_IP_ADDRESS "90.0.0.3" 	/* Pegasus IP Address */
#   define PEGASUS_DESTINATION_ADDRESS "90.0.0.53" /* Destination IP Address */
#   define PEGASUS_NET_MASK 0xffffff00 		/* Pegasus Net Mask */
#   define PEGASUS_TARGET_NAME "host" 		/* Pegasus Target Name */

#endif /* End of optional USB support */

/* Optional TFFS support.  Just define INCLUDE_TFFS to activate */

#undef	INCLUDE_TFFS

#include "template.h"		/* Verify user options, specify i/o addr etc */

#ifdef __cplusplus
}
#endif

#endif	/* INCconfigh */
#if defined(PRJ_BUILD)
#include "prjParams.h"
#endif

