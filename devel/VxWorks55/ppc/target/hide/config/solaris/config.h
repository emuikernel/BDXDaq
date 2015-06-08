/* config.h - UNIX-target configuration header */

/* Copyright 1984-2002, Wind River Systems, Inc. */

/*
modification history
--------------------
02o,24jan02,hbh  Added SM_OFF_BOARD definition.
02n,08nov01,jmp  removed INCLUDE_TYCODRV_5_2.
		 added INCLUDE_SOLARIS_NET_CONFIG, PPP_PSEUDO_TTY_PATH and
                 VXSIM_IP_ADDR.
02m,29oct01,hbh  Removed ULIP and INCLUDE_END references and updated 
		 BSP revision for T2.2.
02l,05oct01,hbh  Fixed NUM_TTY definition (SPR 27291)
02k,06sep01,hbh  Removed ULIP references and modified NUM_TTY definition.
02j,20oct98,pr   added WindView 2.0 defines
02i,07jul98,db   changed BSP_VERSION to "1.2" and BSP_REVISION to "/0".
		 added BSP_VER_1_1 and BSP_VER_1_2 macros, 
		 (Tornado 2.0 release).
02h,08apr98,pdn  changed defaults to build intergrated simulator.
02g,15apr97,pr   Changed default WDB_MTU for ULIP driver.
02f,12feb97,ms   Added PPP network configuration macros
02e,03jan97,dat  BSP_REV 1, Tornado 1.0.1, (c) 1997
02d,30nov96,dvs  removing def's for testing
02c,28may96,ism  added USER_RESERVED_MEM
02b,22may96,dat  cleanup, added BSP_VERSION, BSP_REV
02a,20dec95,ism  added INCLUDE_TYCODRV_5_2
01k,20jul94,ms   removed the undef of INCLUDE_RDB.
01j,19feb94,gae  changed SM_INT_ARG2 from vector 1.
01i,14feb94,gae  changed SM_INT_ARG2 from vector 30.
01h,11jan94,gae  added SM parameters.
01g,14dec93,gae  moved TYCO_DEV here.
01f,30jul93,gae  reduced NV_RAM_SIZE from 2040; added MATH macro's.
01e,05jul93,gae  added sysBootLineMagic.
01d,23jan93,gae  cleanup.
01c,07aug92,gae  added INCLUDE_ULIP and NFS.
01b,29jul92,gae  variable change: lwplvlLock.
01a,22apr92,gae  derived from BNR.
*/

/*
This file contains the configuration parameters for the
pseudo BSP for a UNIX-target.
*/

#ifndef	INCconfigh
#define	INCconfigh

/* BSP version/revision identification, before configAll.h */

#define BSP_VER_1_0	1
#define BSP_VER_1_1	1	/* 1.2 is backward compatible with 1.1 */
#define BSP_VER_1_2	1
#define BSP_VERSION	"1.2"
#define BSP_REV		"/1"	/* 0 for first revision */	

#include "configAll.h"
#include "tyLib.h"

#undef USER_RESERVED_MEM
#define USER_RESERVED_MEM	0       /* user reserved memory (at top) */

#define	INCLUDE_PASSFS		/* VxSim: Pass-thru filesystem */

/* 
 * The default configuration is no network. If you have installed the
 * full featured simulator (unbundled), change TRUE to FALSE below.
 */

#if TRUE
#undef INCLUDE_NETWORK
#undef INCLUDE_NET_INIT
#undef WDB_COMM_TYPE
#define WDB_COMM_TYPE WDB_COMM_PIPE	/* default backend */
#else /* TRUE */
#define INCLUDE_PPP
#undef NUM_TTY
#define NUM_TTY	2
#endif /* TRUE */

#ifdef  INCLUDE_NETWORK
#undef  WDB_COMM_TYPE   /* modify backend if NETWORK included */ 
#define WDB_COMM_TYPE        WDB_COMM_NETWORK
#undef	INCLUDE_WDB_SYS
#endif /* INCLUDE_NETWORK */

#define PPP_PSEUDO_TTY_PATH	"/dev/ptyr%x"	/* pseudo device for solaris */
						/* 2.9 and later version     */
#define VXSIM_IP_ADDR		"192.168.255.%d"

#define INCLUDE_SOLARIS_NET_CONFIG	/* needed to make VXSIM_IP_ADDR and   */
					/* and PPP_PSEUDO_TTY_PATH accessible */
					/* from the project tool              */

#undef	INCLUDE_EX
#undef	INCLUDE_ENP

#define	INCLUDE_FLOATING_POINT
#define	INCLUDE_HW_FP
#undef  INCLUDE_SW_FP

#undef	DEFAULT_BOOT_LINE
extern char *sysBootLineMagic;	/* automagically calculated */
#define	DEFAULT_BOOT_LINE	sysBootLineMagic

/* Miscellaneous definitions */

#define	NV_RAM_SIZE	512             /* 512 bytes */
#undef	NV_RAM_ADRS     		/* not applicable */
#undef	NV_BOOT_LINE    		/* not applicable */

typedef struct                  /* TYCO_DEV */
    {
    TY_DEV tyDev;
    BOOL created;       /* true if this device has really been created */
    int u_fd;		/* UNIX file descriptor */
    int u_pid;		/* UNIX pid */
    } TYCO_DEV;

#define	SYS_CLK_RATE_MIN	2	/* minimum system clock rate */
#define	SYS_CLK_RATE_MAX	100	/* maximum system clock rate */
#define	AUX_CLK_RATE_MIN	1	/* minimum auxiliary clock rate */
#define	AUX_CLK_RATE_MAX	1000	/* maximum auxiliary clock rate */

extern int u_getpid();
extern char *sysSmAddr;

#define SM_INT_TYPE     SM_INT_BUS	/* pseudo bus interrupts */
#define SM_INT_ARG1     u_getpid ()	/* "level" */
#define SM_INT_ARG2     2		/* "vector" (SIGINT) */
#define SM_INT_ARG3     0		/* unused */

#undef  SM_ANCHOR_ADRS
#define SM_ANCHOR_ADRS  sysSmAddr	/* anchor address */
#define SM_MEM_ADRS     SM_ANCHOR_ADRS  /* shared memory address */
#define SM_MEM_SIZE     0x00080000	/* 512K */
#define SM_OBJ_MEM_ADRS	(SM_MEM_ADRS+SM_MEM_SIZE)/* sh. mem Objects pool adrs */
#define SM_OBJ_MEM_SIZE	0x00080000	/* sh. mem Objects pool size 512K */
#define SM_OFF_BOARD	FALSE		/* memory pool is on-board */

#define	LOCAL_MEM_LOCAL_ADRS	sysMemBaseAdrs
extern char *sysMemBaseAdrs;

#define	LOCAL_MEM_SIZE	0x00300000	/* 3 Mbyte minimum memory available */

#undef	BOOT_LINE_ADRS
extern char *sysBootLine;
#define	BOOT_LINE_ADRS	sysBootLine

#undef	FREE_RAM_ADRS
#define	FREE_RAM_ADRS	sysMemBaseAdrs

#undef	EXC_MSG_ADRS
#define	EXC_MSG_ADRS	sysExcMsg

#ifdef	INCLUDE_NFS
extern int u_getgid ();
#undef	NFS_GROUP_ID
#define	NFS_GROUP_ID	u_getgid ()
#endif	/* INCLUDE_NFS */

#define INCLUDE_TRIGGERING      /* event triggering tool */

#define INCLUDE_WINDVIEW        /* windview control tool */
#define INCLUDE_WDB_TSFS        /* target-server file system */

/* windview upload paths */

#define  INCLUDE_WVUPLOAD_FILE
#define INCLUDE_WVUPLOAD_TSFSSOCK

#endif	/* INCconfigh */
#if defined(PRJ_BUILD)
#include "prjParams.h"
#endif /* defined(PRJ_BUILD) */

