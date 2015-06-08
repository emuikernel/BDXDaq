/* configAll.h - VxWorks configuration header */

/* Copyright 1984-2002 Wind River Systems, Inc. */

/*
modification history
--------------------
01k,27mar03,pai  merged SPR 74633 fix into CP1
01j,27jan03,wap  sync with changes to usrNetwork.c (SPR #85436)
01i,30apr02,elr  Changed default definition of LOGIN_PASSWORD
01h,03dec01,g_h  Adding WDB_COMM_VTMD macro.
01g,06nov01,dat  Removing defines for _binArrayStart[End]
01f,03oct01,dee  Merge from ColdFire T2.1.0 release
01e,08nov00,zl   include VX_DSP_TASK in WDB_SPAWN_OPTS and WDB_TASK_OPTIONS 
                 for SuperH.
01d,11may00,zl   added Hitachi SuperH (SH) support
01c,02feb98,tm   added PCI configuration type constants (PCI_CFG_*)
01b,22jun98,ms   dos2unix script. added prototype for usrAppInit().
01a,11mar98,ms   written
*/

/*
DESCRIPTION
Replacement configAll.h for the new configuration system.
This file imports the old configAll.h parameter macros (e.g., STACK_SIZE),
but none of the INCLUDE macros. The INCLUDE_MACROS are now generated
in the project directory's config.h.
*/

#ifndef	INCconfigAllh
#define	INCconfigAllh

#include "vxWorks.h"
#include "prjComps.h"
#include "smLib.h"
#include "vme.h"
#include "iv.h"

/* function prototypes */

#ifndef _ASMLANGUAGE
extern void usrAppInit (void);
#endif

	/* processor interface */
#define USER_I_CACHE_MODE	CACHE_WRITETHROUGH  /* default mode */
#define USER_D_CACHE_MODE	CACHE_WRITETHROUGH  /* default mode */
#define USER_I_CACHE_ENABLE		    	    /* undef to leave disabled*/
#define USER_D_CACHE_ENABLE			    /* undef to leave disabled*/
#define USER_B_CACHE_ENABLE			    /* undef to leave disabled*/
#define USER_I_MMU_ENABLE			    /* undef to leave disabled*/
#define USER_D_MMU_ENABLE			    /* undef to leave disabled*/

	/* os */
#define SYM_TBL_HASH_SIZE_LOG2	8	/* 256 entry hash table symbol table */
#define STAT_TBL_HASH_SIZE_LOG2	6	/* 64 entry hash table for status */
#define MQ_HASH_SIZE		0	/* POSIX message queue hash table size 
					 * 0 = default */
#define NUM_SIGNAL_QUEUES	16	/* POSIX queued signal count */

#define FREE_RAM_ADRS		(end)	/* start right after bss of VxWorks */

#define NUM_DRIVERS		20	/* max 20 drivers in drvTable */
#define NUM_FILES		50	/* max 50 files open simultaneously */
#define NUM_DOSFS_FILES		20	/* max 20 dosFs files open */
#define NUM_RAWFS_FILES		5	/* max 5  rawFs files open */
#define NUM_RT11FS_FILES	5	/* max 5  rt11Fs files open */
#define MAX_LOG_MSGS		50      /* max 50 log msgs */

#define MAX_LIO_CALLS		0	/* max queued lio calls 0=default */
#define MAX_AIO_SYS_TASKS	0	/* max aio system tasks, 0 = default */
#define AIO_TASK_PRIORITY	0	/* aio tasks priority, 0 = default */
#define AIO_TASK_STACK_SIZE	0	/* aio tasks stack size, 0 = default */

	/* drivers */
#define	NUM_TTY			2	/* number of tty channels */
#define	CONSOLE_TTY		0	/* console channel */
#define CONSOLE_BAUD_RATE	9600	/* console baud rate */
#define	SLIP_TTY		1	/* serial line IP channel */

/* agent mode */

#define WDB_COMM_NETWORK 	0	/* vxWorks network	- task mode */
#define WDB_COMM_SERIAL		1	/* raw serial		- bimodal   */
#define WDB_COMM_TYCODRV_5_2	2	/* older serial driver	- task mode */
#define WDB_COMM_ULIP		3	/* vxSim packet device	- bimodal   */
#define WDB_COMM_NETROM		4	/* netrom packet device	- bimodal   */
#define WDB_COMM_CUSTOM		5	/* custom packet device	- bimodal   */
#define WDB_COMM_END		6	/* END packet device 	- bimodal   */
#define WDB_COMM_VTMD           8       /* TMD packet device    - bimodal   */

#define WDB_TTY_CHANNEL		1	/* default Sio SERIAL channel */
#define WDB_TTY_DEV_NAME    "/tyCo/1"	/* default TYCODRV_5_2 device name */
#define WDB_TTY_BAUD		9600	/* default baud rate */
#define	WDB_NETROM_TYPE		400	/* default is old 400 series */
#define	WDB_NETROM_WIDTH	1	/* width of a ROM word. This macro
					 * is not used for the 500 series. */
#define WDB_NETROM_INDEX	0	/* index into word of pod zero */
#define	WDB_NETROM_NUM_ACCESS	1	/* of pod zero per byte read */
#define	WDB_NETROM_ROMSIZE	ROM_SIZE /* size of a single ROM. If your
					 * board has N ROMs, this definition
					 * must be divided by N to be right */
#define WDB_NETROM_POLL_DELAY	2	/* # clock ticks to poll for input */

#define WDB_MTU         	1500	/* max RPC message size */
#define WDB_POOL_SIZE 		((sysMemTop() - FREE_RAM_ADRS)/16)
					 /* memory pool for host tools */
#define WDB_BP_MAX              50      /* max # of break points */

#define WDB_SPAWN_STACK_SIZE	0x5000	/* default stack size of spawned task */
#define WDB_SPAWN_PRI		100
#define WDB_SPAWN_OPTS		VX_FP_TASK

#define WDB_RESTART_TIME        10	/* error recovery for task agent */
#define WDB_MAX_RESTARTS        5	/* max # agent restarts on error */
#define WDB_TASK_PRIORITY       3       /* priority of task agent */
#define WDB_TASK_OPTIONS        VX_UNBREAKABLE | VX_FP_TASK  /* agent options */

#define EVT_STACK_SIZE		7000
#define WV_EVT_STACK		EVT_STACK_SIZE
#define EVT_PRIORITY		0
#define WV_EVT_PRIORITY		EVT_PRIORITY
#define EVTBUFFER_SIZE		20000
#define EVTBUFFER_ADDRESS	(char *)NULL

/* WindView command server task parameters */

#define WV_SERVER_STACK		10000
#define WV_SERVER_PRIORITY	100
#define WV_MODE			CONTINUOUS_MODE

/******************************************************************************/
/*                                                                            */
/*                   "GENERIC" BOARD CONFIGURATION                            */
/*                                                                            */
/******************************************************************************/

/* device controller I/O addresses when included */

#define IO_ADRS_EI      ((char *) 0x000fff00)   /* 32A,32D i82596CA Ethernet */
#define IO_ADRS_EX	((char *) 0x00ff0000)	/* 24A,32D Excelan Ethernet */
#define IO_ADRS_ENP	((char *) 0x00de0000)	/* 24A,32D CMC Ethernet */
#define IO_ADRS_EGL	((char *) 0x00004000)	/* 16A,16D Interphase Enet */

#define IO_AM_EX	VME_AM_STD_SUP_DATA	/* Excelan address modifier */
#define IO_AM_EX_MASTER	VME_AM_STD_SUP_DATA	/* Excellan AM for DMA access */
#define IO_AM_ENP	VME_AM_STD_SUP_DATA	/* CMC address modifier */

/* device controller interrupt vectors when included */

#define INT_VEC_ENP		192	/* CMC Ethernet controller*/
#define INT_VEC_EX		193	/* Excelan Ethernet controller*/
#define INT_VEC_EGL		200	/* Interphase Ethernet controller*/

/* device controller interrupt levels when included */

#define INT_LVL_EGL		5	/* Interphase Ethernet controller */
#define INT_LVL_EX		2	/* Excelan Ethernet controller */
#define INT_LVL_ENP		3	/* CMC Ethernet controller */


/******************************************************************************/
/*                                                                            */
/*                   "MISCELLANEOUS" CONSTANTS                                */
/*                                                                            */
/******************************************************************************/

/* shared memory objects parameters (unbundled) */

#define SM_OBJ_MAX_TASK		40	/* max # of tasks using smObj */
#define SM_OBJ_MAX_SEM		60	/* max # of shared semaphores */
#define SM_OBJ_MAX_MSG_Q	10	/* max # of shared message queues */
#define SM_OBJ_MAX_MEM_PART	4	/* max # of shared memory partitions */
#define SM_OBJ_MAX_NAME		100	/* max # of shared objects names */
#define SM_OBJ_MAX_TRIES	100	/* max # of tries to obtain lock */

/* shared memory network parameters  - defaults to values DEFAULT_PKTS_SIZE
 * and DEFAULT_CPUS_MAX in smPktLib.h respectively
 */

#define SM_PKTS_SIZE            0       /* shared memory packet size */
#define SM_CPUS_MAX             0       /* max # of cpus for shared network */
#define SM_ANCHOR_ADRS	((char *) (LOCAL_MEM_LOCAL_ADRS+SM_ANCHOR_OFFSET))

#define BOOT_LINE_ADRS	((char *) (LOCAL_MEM_LOCAL_ADRS+BOOT_LINE_OFFSET))
#define	BOOT_LINE_SIZE	255	/* use 255 bytes for bootline */
#define	NV_BOOT_OFFSET	0	/* store the boot line at start of NVRAM */

#define EXC_MSG_ADRS	((char *) (LOCAL_MEM_LOCAL_ADRS+EXC_MSG_OFFSET))

#define	SM_TAS_TYPE	SM_TAS_HARD	/* hardware supports test-and-set */

#if	(_STACK_DIR == _STACK_GROWS_DOWN)

#ifdef	ROM_RESIDENT
#define STACK_ADRS	STACK_RESIDENT
#else
#define STACK_ADRS	_romInit
#endif	/* ROM_RESIDENT */

#else	/* _STACK_DIR == _STACK_GROWS_UP */

#ifdef	ROM_RESIDENT
#define STACK_ADRS	(STACK_RESIDENT-STACK_SAVE)
#else
#define STACK_ADRS	(_romInit-STACK_SAVE)
#endif	/*  ROM_RESIDENT */

#endif	/* _STACK_DIR == _STACK_GROWS_UP */

#define	CLEAR_BSS


/* Default Boot Parameters */

#define HOST_NAME_DEFAULT	"bootHost"	/* host name */
#define TARGET_NAME_DEFAULT	"vxTarget"	/* target name (tn) */
#define HOST_USER_DEFAULT	"target"	/* user (u) */
#define HOST_PASSWORD_DEFAULT	""		/* password */
#define SCRIPT_DEFAULT		""	 	/* startup script (s) */
#define OTHER_DEFAULT		"" 		/* other (o) */

/* Default NFS parameters - constants may be changed here, variables
 * may be changed in usrConfig.c at the point where NFS is included.
 */

#define NFS_USER_ID		2001		/* dummy nfs user id */
#define NFS_GROUP_ID		100		/* dummy nfs user group id */


/* Login security initial user name and password.
 * Use vxencrypt on host to find encrypted password.
 * Default password provided here is "password".
 */

#define LOGIN_USER_NAME		"target"
#define LOGIN_PASSWORD		"RcQbRbzRyc"	/* "password" */


/* install environment variable task create/delete hooks */

#define	ENV_VAR_USE_HOOKS	TRUE

/* SNMP configuration parameters */
/* MIB-2 Variable defaults - see RFC 1213 for complete description */
 
#define MIB2_SYS_DESCR                  "VxWorks SNMPv1/v2c Agent"
#define MIB2_SYS_CONTACT                "Wind River Systems"
#define MIB2_SYS_LOCATION               "Planet Earth"

/* MIB2_SYS_OID_LEN is the number of elements in the object id
 * MIB2_SYS_OID is the object id.  The default is "0.0" which
 * has the length of 2
 */
 
#define MIB2_SYS_OID_LEN                2
#define MIB2_SYS_OID                    { 0, 0 }

#define SNMP_TRACE_LEVEL            0    /* Must be >= 0 and <= 3 with higher */
                                         /* values giving more info and 0     */
                                         /* giving no info                    */ 




#define PPP_TTY				1	/* default PPP serial channel */
#define PPP_OPTIONS_STRUCT			/* use PPP options macros */
#define	PPP_OPTIONS_FILE		NULL	/* use PPP options file */
#define	PPP_CONNECT_DELAY		15	/* delay to establish link */

/* PPP options flags - set to 1 to turn on option */

#define	PPP_OPT_NO_ALL			0	/* Don't allow any options */
#define	PPP_OPT_PASSIVE_MODE		0	/* Set passive mode */
#define	PPP_OPT_SILENT_MODE		0	/* Set silent mode */
#define	PPP_OPT_DEFAULTROUTE		0	/* Add default route */
#define	PPP_OPT_PROXYARP		0	/* Add proxy ARP entry */
#define	PPP_OPT_IPCP_ACCEPT_LOCAL	0	/* Acpt peer's IP addr for us */
#define	PPP_OPT_IPCP_ACCEPT_REMOTE	0	/* Acpt peer's IP addr for it */
#define	PPP_OPT_NO_IP			0	/* Disable IP addr negot. */
#define	PPP_OPT_NO_ACC			0	/* Disable addr/control compr */
#define	PPP_OPT_NO_PC			0	/* Disable proto field compr */
#define	PPP_OPT_NO_VJ			0	/* Disable VJ compression */
#define	PPP_OPT_NO_VJCCOMP		0	/* Disable VJ conct-ID compr */
#define	PPP_OPT_NO_ASYNCMAP		0	/* Disable async map negot. */
#define	PPP_OPT_NO_MN			0	/* Disable magic num negot. */
#define	PPP_OPT_NO_MRU			0	/* Disable MRU negotiation */
#define	PPP_OPT_NO_PAP			0	/* Don't allow PAP auth */
#define	PPP_OPT_NO_CHAP			0	/* Don't allow CHAP auth */
#define	PPP_OPT_REQUIRE_PAP		0	/* Require PAP auth */
#define	PPP_OPT_REQUIRE_CHAP		0	/* Require CHAP auth */
#define	PPP_OPT_LOGIN			0	/* Use login dbase for PAP */
#define	PPP_OPT_DEBUG			0	/* Enable daemon debug mode */
#define	PPP_OPT_DRIVER_DEBUG		0	/* Enable driver debug mode */

/* PPP options strings - set to desired string to turn on option */

#define	PPP_STR_ASYNCMAP		NULL	/* desired async map */
#define	PPP_STR_ESCAPE_CHARS		NULL	/* chars to escape on xmits */
#define	PPP_STR_VJ_MAX_SLOTS		NULL	/* max VJ compr header slots */
#define	PPP_STR_NETMASK			NULL	/* netmask value */
#define	PPP_STR_MRU			NULL	/* MRU value for negotiation */
#define	PPP_STR_MTU			NULL	/* MTU value for negotiation */
#define	PPP_STR_LCP_ECHO_FAILURE	NULL	/* max LCP echo failures */
#define	PPP_STR_LCP_ECHO_INTERVAL	NULL	/* time for LCP echo requests */
#define	PPP_STR_LCP_RESTART		NULL	/* timeout for LCP */
#define	PPP_STR_LCP_MAX_TERMINATE	NULL	/* max LCP term-reqs */
#define	PPP_STR_LCP_MAX_CONFIGURE	NULL	/* max LCP conf-reqs */
#define	PPP_STR_LCP_MAX_FAILURE		NULL	/* max conf-naks for LCP */
#define	PPP_STR_IPCP_RESTART		NULL	/* timeout for IPCP */
#define	PPP_STR_IPCP_MAX_TERMINATE	NULL	/* max IPCP term-reqs */
#define	PPP_STR_IPCP_MAX_CONFIGURE	NULL	/* max IPCP conf-reqs */
#define	PPP_STR_IPCP_MAX_FAILURE	NULL	/* max # conf-naks for IPCP */
#define	PPP_STR_LOCAL_AUTH_NAME		NULL	/* local name for auth */
#define	PPP_STR_REMOTE_AUTH_NAME	NULL	/* remote name for auth */
#define	PPP_STR_PAP_FILE		NULL	/* PAP secrets file */
#define	PPP_STR_PAP_USER_NAME		NULL	/* username for PAP peer auth */
#define	PPP_STR_PAP_PASSWD		NULL	/* password for PAP peer auth */
#define	PPP_STR_PAP_RESTART		NULL	/* timeout for PAP */
#define	PPP_STR_PAP_MAX_AUTHREQ		NULL	/* max PAP auth-reqs */
#define	PPP_STR_CHAP_FILE		NULL	/* CHAP secrets file */
#define	PPP_STR_CHAP_RESTART		NULL	/* timeout for CHAP */
#define	PPP_STR_CHAP_INTERVAL		NULL	/* CHAP rechallenge interval */
#define	PPP_STR_CHAP_MAX_CHALLENGE	NULL	/* max CHAP challenges */

/* DHCP client parameters */

#ifdef INCLUDE_DHCPC
#define DHCPC_SPORT             67    /* Port monitored by DHCP servers. */
#define DHCPC_CPORT             68    /* Port monitored by DHCP clients. */
#define DHCPC_MAX_LEASES        4     /* Max. number of simultaneous leases */
#define DHCPC_OFFER_TIMEOUT     5     /* Seconds to wait for multiple offers */
#define DHCPC_DEFAULT_LEASE     3600  /* Desired lease length in seconds */
#define DHCPC_MIN_LEASE         30    /* Minimum allowable lease length */
#endif

/* DHCP server parameters */

#ifdef INCLUDE_DHCPS
#define DHCPS_LEASE_HOOK        NULL   /* Name of required storage routine */
#define DHCPS_ADDRESS_HOOK      NULL   /* Name of optional storage routine */
#define DHCPS_DEFAULT_LEASE     3600   /* default lease length (secs). */
#define DHCPS_MAX_LEASE         3600   /* default value of max lease (secs). */
#endif

/* DHCP server and relay agent parameters */

#if defined (INCLUDE_DHCPS) || defined (INCLUDE_DHCPR)
#define DHCP_MAX_HOPS           4       /* Hops before discard, up to 16. */
#define DHCPS_SPORT             67      /* Port monitored by DHCP servers */
#define DHCPS_CPORT             68      /* Port monitored by DHCP clients */
#endif

/* SNTP parameters */

#if defined (INCLUDE_SNTPC) || defined (INCLUDE_SNTPS)
#define SNTP_PORT 123

#ifdef INCLUDE_SNTPS
#define SNTPS_MODE SNTP_ACTIVE     /* SNTP_ACTIVE or SNTP_PASSIVE */
#define SNTPS_DSTADDR NULL         /* If NULL, uses subnet local broadcast. */
#define SNTPS_INTERVAL 64          /* Broadcast interval, in seconds. */
#define SNTPS_TIME_HOOK NULL       /* Name of required clock access routine. */
#endif    /* SNTP server. */
#endif /* SNTP server or client. */

/* DNS resolver parameters; You must change these defaults to your config. */

#ifdef INCLUDE_DNS_RESOLVER
#define RESOLVER_DOMAIN_SERVER  "90.0.0.3"    /* DNS server IP address */
#define RESOLVER_DOMAIN         "wrs.com"     /* Resolver domain */
#endif /* INCLUDE_DNS_RESOLVER */

/* XXX - changes */

/* romInit.s/romStart.c configuration parameters */

#undef  STACK_ADRS
#if	(_STACK_DIR == _STACK_GROWS_DOWN)
#   define STACK_ADRS	RAM_DATA_ADRS
#else	/* _STACK_DIR == _STACK_GROWS_UP */
#   define STACK_ADRS	(RAM_DATA_ADRS-STACK_SAVE)
#endif
#define	ROMSTART_BOOT_CLEAR
#define RESERVED	0

/* system clock rate */

#define	SYS_CLK_RATE	60

#define	WDB_TTY_ECHO	FALSE




/* ARCHITECTURE-SPECIFC PARAMTERS */

#if	CPU_FAMILY==I80X86
#define INT_LOCK_LEVEL          0x0     /* 80x86 interrupt disable mask */
#define ROOT_STACK_SIZE         10000   /* size of root's stack, in bytes */
#define SHELL_STACK_SIZE        10000   /* size of shell's stack, in bytes */
#define WDB_STACK_SIZE          0x1000  /* size of WDB agents stack, in bytes */
#define ISR_STACK_SIZE          1000    /* size of ISR stack, in bytes */
#define TRAP_DEBUG              0       /* not used */
#define VEC_BASE_ADRS           ((char *) LOCAL_MEM_LOCAL_ADRS)

#define GDT_BASE_OFFSET         0x1000
#define SM_ANCHOR_OFFSET        0x1100
#define BOOT_LINE_OFFSET        0x1200
#define EXC_MSG_OFFSET          0x1300

#define VM_PAGE_SIZE            4096	/* default page size */
#define STACK_SAVE              0x40    /* maximum stack used to preserve */
#endif



#if	CPU_FAMILY==I960
#define INT_LOCK_LEVEL          0x1f    /* i960 interrupt disable mask */
#define ROOT_STACK_SIZE         20000   /* size of root's stack, in bytes */
#define SHELL_STACK_SIZE        40000   /* size of shell's stack, in bytes */
#define WDB_STACK_SIZE          0x2000  /* size of WDB agents stack, in bytes */
#define ISR_STACK_SIZE          1000    /* size of ISR stack, in bytes */
#define TRAP_DEBUG              0       /* n/a for the 80960 */
#define VEC_BASE_ADRS           NONE    /* base register not reconfigurable */

#define SM_ANCHOR_OFFSET        0x600
#define BOOT_LINE_OFFSET        0x700
#define EXC_MSG_OFFSET          0x800

#define VM_PAGE_SIZE            8192

#define STACK_SAVE      512             /* maximum stack used to preserve */
#endif



#if	CPU_FAMILY==MC680X0
#define INT_LOCK_LEVEL          0x7     /* 68k interrupt disable mask */
#define ROOT_STACK_SIZE         10000   /* size of root's stack, in bytes */
#define SHELL_STACK_SIZE        10000   /* size of shell's stack, in bytes */
#define WDB_STACK_SIZE          0x1000  /* size of WDB agents stack, in bytes */
#define ISR_STACK_SIZE          1000    /* size of ISR stack, in bytes */
#define TRAP_DEBUG              2       /* trap 2 - breakpoint trap */
#define VEC_BASE_ADRS           ((char *) LOCAL_MEM_LOCAL_ADRS)

#define SM_ANCHOR_OFFSET        0x600
#define BOOT_LINE_OFFSET        0x700
#define EXC_MSG_OFFSET          0x800

#define VM_PAGE_SIZE            8192
#define STACK_SAVE      0x40            /* maximum stack used to preserve */

#undef	RESERVED
#define RESERVED        0x400           /* avoid zeroing MC68302 vector table */
#endif



#if	CPU_FAMILY==COLDFIRE
#define INT_LOCK_LEVEL          0x7     /* Coldfire interrupt disable mask */
#define ROOT_STACK_SIZE         10000   /* size of root's stack, in bytes */
#define SHELL_STACK_SIZE        10000   /* size of shell's stack, in bytes */
#define WDB_STACK_SIZE          0x1000  /* size of WDB agents stack, in bytes */
#define ISR_STACK_SIZE          1000    /* size of ISR stack, in bytes */
#define TRAP_DEBUG              2       /* trap 2 - breakpoint trap */
#define VEC_BASE_ADRS           ((char *) LOCAL_MEM_LOCAL_ADRS)

#define SM_ANCHOR_OFFSET        0x600
#define BOOT_LINE_OFFSET        0x700
#define EXC_MSG_OFFSET          0x800

#define VM_PAGE_SIZE            8192
#define STACK_SAVE      0x40            /* maximum stack used to preserve */

#endif



#if	CPU_FAMILY==MIPS
#define INT_LOCK_LEVEL          0x1     /* R3K interrupt disable mask */
#define ROOT_STACK_SIZE         (20000) /* size of root's stack, in bytes */
#define SHELL_STACK_SIZE        (20000) /* size of shell's stack, in bytes */
#define WDB_STACK_SIZE          (0x2000)/* size of WDB agents stack, in bytes */
#define ISR_STACK_SIZE          (5000)  /* size of ISR stack, in bytes */
#define VEC_BASE_ADRS           ((char *) 0x0)  /* meaningless in R3k land */
#define VME_VECTORED            FALSE   /* use vectored VME interrupts */
#define TRAP_DEBUG              0       /* trap 0 - breakpoint trap */

#define SM_ANCHOR_OFFSET        0x600
#define BOOT_LINE_OFFSET        0x700
#define EXC_MSG_OFFSET          0x800

#define VM_PAGE_SIZE            8192
#define STACK_SAVE      	0x40    /* maximum stack used to preserve */

#define	UNCACHED(adr)	K0_TO_K1(adr)
#undef	ROMSTART_BOOT_CLEAR
#endif



#if	CPU_FAMILY==PPC
#define INT_LOCK_LEVEL          0x0     /* not used */
#define ROOT_STACK_SIZE         (20000) /* size of root's stack, in bytes */
#define SHELL_STACK_SIZE        (20000) /* size of shell's stack, in bytes */
#define WDB_STACK_SIZE          0x2000  /* size of WDB agents stack, in bytes */
#define ISR_STACK_SIZE          (5000)  /* size of ISR stack, in bytes */
#define VEC_BASE_ADRS           ((char *) 0x0)

#define SM_ANCHOR_OFFSET        0x4100
#define BOOT_LINE_OFFSET        0x4200
#define EXC_MSG_OFFSET          0x4300

#define VM_PAGE_SIZE            4096
#define STACK_SAVE      	0x1000

/* romInit.s/romStart.c */
#undef	RESERVED
#define RESERVED        0x4400          /* avoid zeroing EXC_MSG */
#endif



#if	CPU_FAMILY==SIMHPPA
#define INT_LOCK_LEVEL          0x1     /* interrupt disable mask */
#define ROOT_STACK_SIZE         20000   /* size of root's stack, in bytes */
#define SHELL_STACK_SIZE        50000   /* size of shell's stack, in bytes */
#define WDB_STACK_SIZE          0x2000  /* size of WDB agents stack, in bytes */
#define ISR_STACK_SIZE          50000   /* size of ISR stack, in bytes */
#define VEC_BASE_ADRS           0       /* dummy */

#define SM_ANCHOR_OFFSET        0x600
#define BOOT_LINE_OFFSET        0x700
#define EXC_MSG_OFFSET          0x800

#define VM_PAGE_SIZE            8192
#define STACK_SAVE		0x40    /* maximum stack used to preserve */

#undef	CLEAR_BSS
#endif



#if	CPU_FAMILY==SIMSPARCSOLARIS
#define INT_LOCK_LEVEL          0x1     /* interrupt disable mask */
#define ROOT_STACK_SIZE         20000   /* size of root's stack, in bytes */
#define SHELL_STACK_SIZE        50000   /* size of shell's stack, in bytes */
#define WDB_STACK_SIZE          0x2000  /* size of WDB agents stack, in bytes */
#define ISR_STACK_SIZE          50000   /* size of ISR stack, in bytes */
#define VEC_BASE_ADRS           0       /* dummy */

#define SM_ANCHOR_OFFSET        0x600
#define BOOT_LINE_OFFSET        0x700
#define EXC_MSG_OFFSET          0x800

#define VM_PAGE_SIZE            8192
#define STACK_SAVE              0x40    /* maximum stack used to preserve */

#undef	CLEAR_BSS
#endif



#if	CPU_FAMILY==SPARC
#define INT_LOCK_LEVEL          15      /* SPARC interrupt disable level */
#define ROOT_STACK_SIZE         10000   /* size of root's stack, in bytes */
#define SHELL_STACK_SIZE        50000   /* size of shell's stack, in bytes */
#define WDB_STACK_SIZE          0x2000  /* size of WDB agents stack, in bytes */
#define ISR_STACK_SIZE          10000   /* size of ISR stack, in bytes */
#define VEC_BASE                (LOCAL_MEM_LOCAL_ADRS + 0x1000)
#define VEC_BASE_ADRS           ((char *) VEC_BASE)

#define SM_ANCHOR_OFFSET        0x600
#define BOOT_LINE_OFFSET        0x700
#define EXC_MSG_OFFSET          0x800

#define VM_PAGE_SIZE            8192
#define STACK_SAVE              0x1000  /* maximum stack used to preserve */

/* romInit.s/romStart.c */

#undef  RESERVED
#define RESERVED        0x2000
#endif


#if     CPU_FAMILY==ARM
#define INT_LOCK_LEVEL          0       /* interrupt disable mask - unused */
#define ROOT_STACK_SIZE         0x4000  /* size of root's stack, in bytes */
#define SHELL_STACK_SIZE        0x10000 /* size of shell's stack, in bytes */
#define WDB_STACK_SIZE          0x2000  /* size of WDB agents stack, in bytes */
#define TRAP_DEBUG              0       /* not used */
#define VEC_BASE_ADRS           ((char *) LOCAL_MEM_LOCAL_ADRS)
/*
 * NOTE: ISR_STACK_SIZE defined in config.h, not here - BSP interrupt
 *       structure dependent
 */
#define VM_PAGE_SIZE            4096

#define SM_ANCHOR_OFFSET        0x600
#define BOOT_LINE_OFFSET        0x700
#define EXC_MSG_OFFSET          0x800
#endif  /* CPU_FAMILY==ARM */

#if CPU==SIMNT
#define INT_LOCK_LEVEL          0x1     /* interrupt disable mask */
#define ROOT_STACK_SIZE         20000   /* size of root's stack, in bytes */
#define SHELL_STACK_SIZE        50000   /* size of shell's stack, in bytes */
#define WDB_STACK_SIZE          0x2000  /* size of WDB agents stack, in bytes */
#define ISR_STACK_SIZE          50000   /* size of ISR stack, in bytes */
#define VEC_BASE_ADRS           0       /* dummy */
#define BOOT_LINE_OFFSET        0x700	/* dummy */

#undef	FREE_RAM_ADRS
#define FREE_RAM_ADRS           simMemBlock

#undef	CLEAR_BSS
#endif

#if	CPU_FAMILY==SH
#define	INT_LOCK_LEVEL		15
#define	ROOT_STACK_SIZE		10000
#define	SHELL_STACK_SIZE	10000	/* size of shell's stack, in bytes */
#define WDB_STACK_SIZE	 	0x1000	/* size of WDB agents stack, in bytes */
#define	ISR_STACK_SIZE		1000
#define	VEC_BASE_ADRS		((char *) LOCAL_MEM_LOCAL_ADRS)

#undef  WDB_SPAWN_OPTS
#define WDB_SPAWN_OPTS		(VX_FP_TASK | VX_DSP_TASK)
#undef  WDB_TASK_OPTIONS
#define WDB_TASK_OPTIONS        (VX_UNBREAKABLE | VX_FP_TASK | VX_DSP_TASK)

#if	(CPU==SH7750 || CPU==SH7729 || CPU==SH7700)
#define SM_ANCHOR_OFFSET        0x1600	/* not used for SDRAM config. */
#define BOOT_LINE_OFFSET        0x1700
#define EXC_MSG_OFFSET          0x1800
#else
#define SM_ANCHOR_OFFSET        0x600
#define BOOT_LINE_OFFSET        0x700
#define EXC_MSG_OFFSET          0x800
#endif  /* (CPU==SH7750 || CPU==SH7729 || CPU==SH7700) */

#define VM_PAGE_SIZE		4096
#define STACK_SAVE      	0x40    /* maximum stack used to preserve */

#endif	/* CPU_FAMILY==SH */

/* PCI device configuration type constants */

#define PCI_CFG_FORCE 0
#define PCI_CFG_AUTO  1
#define PCI_CFG_NONE  2
 

/* for backward compatibility with old 1.0 BSPs */

#ifndef BSP_VERSION
#   define BSP_VERSION	"1.0"	/* old 1.0 style BSP */
#   define BSP_VER_1_0	TRUE
#endif

#ifndef BSP_REV
#   define BSP_REV	"/0"	/* old 1.0 style BSP */
#endif

#endif	/* INCconfigAllh */

