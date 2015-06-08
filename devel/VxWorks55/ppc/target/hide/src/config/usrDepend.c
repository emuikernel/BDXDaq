/* usrDepend.c - include dependency rules */

/* Copyright 1992-2002 Wind River Systems, Inc. */

/*
modification history
--------------------
02h,08apr02,jmp  removed dependency between INCLUDE_NET_SYM_TBL and
		 INCLUDE_NET_INIT for simulators (SPR #75207).
02g,20feb02,mas  updated to support new SM components (SPR 73371)
02f,07dec01,wef  include USB configlettes.
02e,15nov01,nrv  including sysTffs.c when INCLUDE_TFFS is defined
02d,31oct01,gls  added INCLUDE_POSIX_PTHREADS to INCLUDE_POSIX_ALL
02c,03oct01,jkf  added INCLUDE_DISK_UTIL to INCLUDE_CONFIGURATION_5_2 and
                 STANDALONE ifdef conditions, this pulls in ls, cd, etc.
02b,20jan99,cdp  removed support for old ARM libraries.
02a,16nov98,cdp  added support for ARM MPU and generic ARM CPUs without cache.
01x,20jan99,dbs  change VXCOM to COM
01w,18dec98,dbs  add VXCOM as separate item
01v,06nov98,dbs  add DCOM dependencies on C++
01w,02mar99,dbt  remove WDB if we are using a standalone image without network
                 initialization and WDB needs network (fixed SPR #23450).
01v,08sep98,cth  changed default upload path for windview
01u,17apr98,nps  added WINDVIEW dependency on RBUFF.
01t,27jan98,cth  removed INCLUDE_RPC, added INCLUDE_WVUPLOAD_FILE, 
		 INCLUDE_WDB_TSFS within INCLUDE_WINDVIEW
01z,02jun98,dbt  INCLUDE_WDB no longer remove INCLUDE_DEBUG.
01y,21apr98,jpd  removed ARM-specific check against INCLUDE_PROTECT_VEC_TABLE.
01w,29apr98,dbt  removed RDB references (no longer supported).
01v,25mar98,dbt  added a line to define INCLUDE_NET_SYM_TBL if we define
		 INCLUDE_SYM_TBL_SYNC  (SPR #9052).
01u,19mar98,dbt  added more WDB dependencies.
01x,26mar97,cdp  Break cache/MMU codependence for ARM7TDMI_T.
01w,29jan97,gnn  Added a line to include the network if we do WDB_COMM_END.
01v,13feb97,jpd  define INCLUDE_MMU_FULL if INCLUDE_PROTECT_TEXT defined, add
		 ARM-specific check against INCLUDE_PROTECT_VEC_TABLE.
01u,30jan97,jpd  Break codependence of cache and MMU on ARM7TDMI to allow
                 dummy cacheLib.
01t,23jan97,jpd  added co-dependency of cache and MMU for ARM.
01t,04feb98,dbt  added WDB dependencies.
01s,13dec96,elp  added INCLUDE_SYM_TBL_SYNC (loader, unloader and symTbl).
01r,03dec96,dbt  added #ifdef INCLUDE_ANSI_ALL and #ifdef INCLUDE_MIB2_ALL.
01q,06aug96,dbt  added the ifdef INCLUDE_POSIX_ALL (SPR #5524).
01p,26jul96,ms	 WDB removes RDB and DEBUG. STAT_SYM_TBL includes SYM_TBL.
01o,19jul96,dbt	 added the ifdef INCLUDE_CONFIGURATION_5_2 (SPR #5603).
01n,27jun96,dbt  added INCLUDE_SHELL depedency for INCLUDE_EXC_TASK and
		 INCLUDE_EXC_HANDLING (SPR #5787).
		 Updated copyright.
01m,10jun96,dbt  added INCLUDE_POSIX_MQ dependency for INCLUDE_SYM_TBL
		 (SPR 6335)
01l,27sep95,ms   add INCLUDE_NET if WDB uses network communication (SPR 4506)
01k,28mar95,kkk  added INCLUDE_IO_SYSTEM dependency for INCLUDE_TTY_DEV
01j,27mar95,jag  added INCLUDE_NFS_SERVER dependancy INCLUDE_RPC
01i,22aug94,dzb  cut INCLUDE_NETWORK and STANDALONE ties (SPR #1147).
01h,03apr94,smb  added INCLUDE_NFS dependency for INCLUDE_MIB_VXWORKS.
01g,07dec93,smb  added INCLUDE_ WINDVIEW dependancy INCLUDE_RPC
01f,16aug93,jmm  including rdb now includes loader and unloader
01e,27feb93,rdc  changed logic of check for MMU_BASIC and MMU_FULL to include
		 FULL support if both defined.
01d,14nov92,jcf  guarded against definition of both _MMU_BASIC and _MMU_FULL.
01c,14nov91,kdl  made USER_D_CACHE_ENABLE definition depend on INCLUDE_BP_5_0
		 and SM_OFF_BOARD (SPR #1801).
01b,31oct91,jcf  removed INCLUDE_68881 definition.
01a,18sep92,jcf  written.
*/

/*
DESCRIPTION
This file is used to include all modules necessary given the desired
configuration specified by configAll.h and config.h.  For instance 
if INCLUDE_NFS is defined, and INCLUDE_RPC is not, then INCLUDE_RPC
is automatically defined here.  This file is included by usrConfig.c.

SEE ALSO: usrExtra.c

NOMANUAL
*/

#ifndef  __INCusrDependc 
#define  __INCusrDependc 

/* check include dependencies */

#if	defined(INCLUDE_MIB_VXWORKS)
#define INCLUDE_NFS             /* nfs package */
#endif /* INCLUDE_MIB_VXWORKS */

#ifdef DOC		/* include when building documentation */
#define INCLUDE_SCSI
#endif	/* DOC */

#ifdef INCLUDE_TFFS
#include "sysTffs.c"      /* the BSP stub file, in the BSP directory */        
#endif /* INCLUDE_TFFS */

/* Begin including USB Configlettes */

#ifdef INCLUDE_USB
    #include "usbPciStub.c" 
#endif
#ifdef INCLUDE_USB_INIT
    #include "../comps/src/usrUsbInit.c"
#endif
#ifdef INCLUDE_UHCI_INIT
    #include "../comps/src/usrUsbHcdUhciInit.c"
#endif
#ifdef INCLUDE_OHCI_INIT
    #include "../comps/src/usrUsbHcdOhciInit.c"
#endif
#ifdef INCLUDE_OHCI_PCI_INIT
    #include "../comps/src/usrUsbPciOhciInit.c"
#endif
#ifdef INCLUDE_USBTOOL
    #include "../comps/src/usrUsbTool.c"
#endif
#ifdef INCDLUE_USB_AUDIO_DEMO
    #include "../comps/src/usrUsbAudioDemo.c"
#endif
#ifdef INCLUDE_USB_MOUSE_INIT
    #include "../comps/src/usrUsbMseInit.c"
#endif
#ifdef INCLUDE_USB_KEYBOARD_INIT
    #include "../comps/src/usrUsbKbdInit.c"
#endif
#ifdef INCLUDE_USB_PRINTER_INIT
    #include "../comps/src/usrUsbPrnInit.c"
#endif
#ifdef INCLUDE_USB_SPEAKER_INIT
    #include "../comps/src/usrUsbSpkrInit.c"
#endif
#ifdef INCLUDE_USB_MS_BULKONLY_INIT
    #include "../comps/src/usrUsbBulkDevInit.c"
#endif
#ifdef INCLUDE_USB_MS_CBI_INIT
    #include "../comps/src/usrUsbCbiUfiDevInit.c"
#endif
#ifdef INCLUDE_USB_PEGASUS_END_INIT
    #include "../comps/src/usrUsbPegasusEndInit.c"
#endif  

/* end USB Configlettes */

#ifdef  INCLUDE_CONFIGURATION_5_2
#define INCLUDE_LOADER          /* object module loading */
#define INCLUDE_NET_SYM_TBL     /* load symbol table from network */
#define INCLUDE_RLOGIN          /* remote login */
#define INCLUDE_SHELL           /* interactive c-expression interpreter */
#define INCLUDE_SHOW_ROUTINES   /* show routines for system facilities*/
#define INCLUDE_SPY             /* spyLib for task monitoring */
#define INCLUDE_STARTUP_SCRIPT  /* execute start-up script */
#define INCLUDE_STAT_SYM_TBL    /* create user-readable error status */
#define INCLUDE_SYM_TBL         /* symbol table package */
#define INCLUDE_TELNET          /* telnet-style remote login */
#define INCLUDE_UNLOADER        /* object module unloading */
#define INCLUDE_DEBUG           /* native debugging */
#define INCLUDE_DISK_UTIL	/* cd, mkdir, ls, xcopy, etc */
#undef  INCLUDE_WDB             /* tornado debug agent */
#endif  /* INCLUDE_CONFIGURATION_5_2 */

/* Posix facilities */

#ifdef 	INCLUDE_POSIX_ALL
#define INCLUDE_POSIX_AIO        /* POSIX async I/O support */
#define INCLUDE_POSIX_AIO_SYSDRV /* POSIX async I/O system driver */
#define INCLUDE_POSIX_FTRUNC     /* POSIX ftruncate routine */
#define INCLUDE_POSIX_MEM        /* POSIX memory locking */
#define INCLUDE_POSIX_MQ         /* POSIX message queue support */
#define INCLUDE_POSIX_PTHREADS   /* POSIX pthreads support */
#define INCLUDE_POSIX_SCHED      /* POSIX scheduling */
#define INCLUDE_POSIX_SEM        /* POSIX semaphores */
#define INCLUDE_POSIX_SIGNALS    /* POSIX queued signals */
#define INCLUDE_POSIX_TIMERS     /* POSIX timers */
#endif

/* Ansi facilities */

#ifdef	INCLUDE_ANSI_ALL
#define	INCLUDE_ANSI_ASSERT	/* ANSI-C assert library functionality */
#define	INCLUDE_ANSI_CTYPE	/* ANSI-C ctype library functionality */
#define	INCLUDE_ANSI_LOCALE	/* ANSI-C locale library functionality */
#define	INCLUDE_ANSI_MATH	/* ANSI-C math library functionality */
#define	INCLUDE_ANSI_STDIO	/* ANSI-C stdio library functionality */
#define	INCLUDE_ANSI_STDLIB	/* ANSI-C stdlib library functionality */
#define	INCLUDE_ANSI_STRING	/* ANSI-C string library functionality */
#define	INCLUDE_ANSI_TIME	/* ANSI-C time library functionality */
#endif

#ifdef	INCLUDE_MIB2_ALL
#define	INCLUDE_MIB2_SYSTEM	/* the system group */
#define	INCLUDE_MIB2_TCP	/* the TCP group */
#define	INCLUDE_MIB2_ICMP	/* the ICMP group */
#define	INCLUDE_MIB2_UDP	/* the UDP group */
#define	INCLUDE_MIB2_IF		/* the interfaces group */
#define	INCLUDE_MIB2_AT		/* the AT group */
#define	INCLUDE_MIB2_IP		/* the IP group */
#endif	/* INCLUDE_MIB2_ALL */

#ifdef	INCLUDE_SYM_TBL_SYNC
#define INCLUDE_WDB		/* WDB debug agent */
#define INCLUDE_LOADER		/* object module loading */
#define INCLUDE_SYM_TBL		/* symbol table package */
#define INCLUDE_UNLOADER	/* object module unloading */
#define INCLUDE_NET_SYM_TBL	/* load symbol table from network */
#endif	/* INCLUDE_SYM_TBL_SYNC */

#if     defined(INCLUDE_WINDVIEW)

#define INCLUDE_RBUFF

#if	defined(INCLUDE_WVUPLOAD_ALL)
#define INCLUDE_WVUPLOAD_FILE
#define INCLUDE_WVUPLOAD_SOCK
#define INCLUDE_WVUPLOAD_TSFSSOCK
#endif

#if    (!defined(INCLUDE_WVUPLOAD_SOCK) &&	\
        !defined(INCLUDE_WVUPLOAD_TSFSSOCK) && 	\
	!defined(INCLUDE_WVUPLOAD_FILE))
#define INCLUDE_WVUPLOAD_FILE
#define INCLUDE_WVUPLOAD_TSFSSOCK
#endif

#if     defined (INCLUDE_WVUPLOAD_TSFSSOCK)
#define INCLUDE_WDB_TSFS
#endif
#endif	/* INCLUDE_WINDVIEW */

#if     defined(INCLUDE_NFS_SERVER)
#define INCLUDE_RPC             /* rpc required by nfs */
#endif	/* INCLUDE_NFS_SERVER  */

#if     defined(INCLUDE_NFS)
#define INCLUDE_RPC             /* rpc required by nfs */
#endif	/* INCLUDE_NFS */

#if	((defined(INCLUDE_RPC) || defined(INCLUDE_NET_SYM_TBL)) \
	 && !defined(INCLUDE_NET_INIT))
#if	((CPU != SIMNT) && (CPU != SIMSPARCSOLARIS))
#define INCLUDE_NET_INIT	/* net init required by rpc and net sym tbl */
#endif	/* (CPU != SIMNT) && (CPU != SIMSPARCSOLARIS) */
#endif	/* INCLUDE_RPC || INCLUDE_NET_SYM_TBL */

#if	defined(INCLUDE_WDB) && ((WDB_COMM_TYPE == WDB_COMM_NETWORK) || \
                                 (WDB_COMM_TYPE == WDB_COMM_END))
#define INCLUDE_NET_INIT
#endif	/* defined(INCLUDE_WDB) && (WDB_COMM_TYPE == WDB_COMM_NETWORK) */

#if	defined(INCLUDE_NET_INIT) && !defined(INCLUDE_NETWORK)
#define INCLUDE_NETWORK		/* network required for net init */
#endif	/* INCLUDE_NETWORK */

#if	defined(INCLUDE_TTY_DEV) && !defined(INCLUDE_IO_SYSTEM)
#define INCLUDE_IO_SYSTEM       /* io system require for tty device */
#endif	/* INCLUDE_TTY_DEV && !INCLUDE_IO_SYSTEM */

#if (defined(INCLUDE_SM_OBJ) && !defined(INCLUDE_SM_COMMON))
#  define INCLUDE_SM_COMMON
#endif /* INCLUDE_SM_OBJ && !INCLUDE_SM_COMMON */

#if (defined(INCLUDE_SM_NET_SHOW)    || \
     defined(INCLUDE_SECOND_SMNET)   || \
     defined(INCLUDE_SM_NET_ADDRGET) || \
     defined(INCLUDE_SM_SEQ_ADDR))
# ifndef   INCLUDE_SM_NET
#  define  INCLUDE_SM_NET
# endif /* INCLUDE_SM_NET */
#endif /* INCLUDE_SM_NET_SHOW || INCLUDE_SECOND_SMNET || ... */

#if (defined(INCLUDE_SECOND_SMNET) && !defined(INCLUDE_SM_NET_ADDRGET))
#  define INCLUDE_SM_NET_ADDRGET
#endif /* INCLUDE_SECOND_SMNET && !INCLUDE_SM_NET_ADDRGET */

#ifdef  INCLUDE_SM_NET
# ifndef   INCLUDE_BSD_BOOT
#  define  INCLUDE_BSD_BOOT
# endif /* INCLUDE_BSD_BOOT */
# ifndef   INCLUDE_SM_COMMON
#  define  INCLUDE_SM_COMMON
# endif /* INCLUDE_SM_COMMON */
#endif /* INCLUDE_SM_NET */

#if	defined(INCLUDE_BP_5_0) && (SM_OFF_BOARD == FALSE)
#undef USER_D_CACHE_ENABLE	/* disable data cache if old bp on-board */
#endif  /* INCLUDE_BP_5_0 && !SM_OFF_BOARD */

#if	defined(INCLUDE_FP_EMULATION)
#define INCLUDE_SW_FP		/* floating point software emulation */
#endif

#if	defined(INCLUDE_MMU_BASIC) && defined(INCLUDE_MMU_FULL)
#undef	INCLUDE_MMU_BASIC	/* leave only FULL defined. can't have both */
#endif

#if	defined(INCLUDE_PROTECT_TEXT) && !defined(INCLUDE_MMU_FULL)
#undef	INCLUDE_MMU_BASIC
#define INCLUDE_MMU_FULL	/* FULL required to protect areas */
#endif

#if (CPU_FAMILY == ARM)

#if     defined(INCLUDE_MMU_MPU) && defined(INCLUDE_MMU_FULL)
#undef  INCLUDE_MMU_FULL       /* leave only MPU defined. can't have both */
#endif
#if     defined(INCLUDE_MMU_MPU) && defined(INCLUDE_MMU_BASIC)
#undef  INCLUDE_MMU_BASIC       /* leave only MPU defined. can't have both */
#endif
#if	defined(INCLUDE_PROTECT_TEXT) && defined(INCLUDE_MMU_MPU)
#undef INCLUDE_PROTECT_TEXT	/* FULL required to protect areas */
#endif

/*
 * On the ARM processors, the cache and MMU are very closely related and it
 * is difficult and often dangerous to use one without the other. Some
 * combinations are architecturally undefined and others can have unfortunate
 * side-effects. Only change the following if you are certain you understand
 * the ramifications.
 */

#if	!ARM_HAS_NO_MMU
/*
 * processor has an MMU/MPU so, if cache support requested, must enable
 * support for the MMU/MPU
 */

#if	defined(INCLUDE_CACHE_SUPPORT) && \
	!defined(INCLUDE_MMU_BASIC) && !defined(INCLUDE_MMU_FULL) && \
	!defined(INCLUDE_MMU_MPU)
/* cache support requested so ensure we have some MMU/MPU support */

#if	ARM_HAS_MPU
#define INCLUDE_MMU_MPU
#else
#define INCLUDE_MMU_BASIC
#endif /* ARM_HAS_MPU */
#endif /* defined(INCLUDE_CACHE_SUPPORT) */
#endif /* !ARM_HAS_NO_MMU */

#if	(defined(INCLUDE_MMU_BASIC) || defined(INCLUDE_MMU_FULL) || \
	 defined(INCLUDE_MMU_MPU)) && \
	!defined(INCLUDE_CACHE_SUPPORT)
#define INCLUDE_CACHE_SUPPORT
#endif
#endif	/* (CPU_FAMILY == ARM) */


/* traditional STANDALONE configuration has network included but not
 * initialized, and standalone symbol table.  To include the network
 * modules, the INCLUDE_NETWORK conditional must be defined.
 */

#if	defined(STANDALONE)
#define	INCLUDE_STANDALONE_SYM_TBL
#undef	INCLUDE_NET_SYM_TBL
#define	INCLUDE_SHELL
#define	INCLUDE_DISK_UTIL
#define	INCLUDE_SHOW_ROUTINES
#define	INCLUDE_DEBUG
#ifndef STANDALONE_NET
#undef	INCLUDE_NET_INIT
#if     defined(INCLUDE_WDB) && ((WDB_COMM_TYPE == WDB_COMM_NETWORK) || \
                                  (WDB_COMM_TYPE == WDB_COMM_END))
#undef	INCLUDE_WDB
#endif  /* defined(INCLUDE_WDB) && (WDB_COMM_TYPE == WDB_COMM_NETWORK) */
#endif	/* STANDALONE_NET */
#endif	/* STANDALONE */

#if     defined(INCLUDE_NET_SYM_TBL) || defined(INCLUDE_STANDALONE_SYM_TBL) || \
        defined(INCLUDE_SECURITY) || defined(INCLUDE_POSIX_MQ)
#define INCLUDE_SYM_TBL 	/* include symbol table package */
#endif	/* INCLUDE_NET_SYM_TBL||INCLUDE_STANDALONE_SYM_TBL||INCLUDE_SECURITY
	||INCLUDE_POSIX_MQ */

#if 	defined(INCLUDE_SHELL)
#define	INCLUDE_EXC_TASK	/* miscelaneous support task */
#define	INCLUDE_EXC_HANDLING	/* include basic exception handling */
#endif	/* INCLUDE_SHELL */	

#if	defined(INCLUDE_STAT_SYM_TBL)
#define	INCLUDE_SYM_TBL
#endif	/* defined(INCLUDE_STAT_SYM_TBL) */

#if 	defined (INCLUDE_WDB_FUNC_CALL) || defined (INCLUDE_WDB_EXC_NOTIFY) || \
	defined (INCLUDE_WDB_EVENTPOINTS) || defined (INCLUDE_WDB_VIO) || \
	defined (INCLUDE_WDB_USER_EVENT)
#define INCLUDE_WDB_EVENTS              /* host async event notification */
#endif 	/* INCLUDE_WDB_FUNC_CALL */

#if	defined (INCLUDE_WDB_BP) || defined (INCLUDE_WDB_EXIT_NOTIFY) || \
	defined (INCLUDE_WDB_START_NOTIFY)
#define INCLUDE_WDB_EVENTS              /* host async event notification */
#define INCLUDE_WDB_EVENTPOINTS         /* eventpoints handling */
#endif	/* INCLUDE_WDB_BP */

#if	defined (INCLUDE_DCOM)
#define INCLUDE_COM
#endif

#if	defined (INCLUDE_COM)
#define INCLUDE_CPLUS_STL
#define INCLUDE_CPLUS_STRING
#endif

#endif /* __INCusrDependc */
