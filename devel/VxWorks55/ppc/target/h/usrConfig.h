/* usrConfig.h - header file for usrConfig.c */

/* Copyright 1984-2002 Wind River Systems, Inc. */

/*
modification history
--------------------
03t,05mar02,mas  updated to support new SM components (SPR 73371)
03s,22jan02,mas  made smObjLib.h and smNetLib.h conditional inclusions
03r,21jan02,sn   include cplusLib.h
03q,13nov01,yvp  Added include for private/excLibP.h
03p,31oct01,gls  merged in pthread code from AE
03o,04oct01,jkf  added dosFs2 headers.
03n,14oct99,jk   added conditional include for sound driver (sb16drv.h).
03m,01sep98,drm  added extern for usrWindMPInit()
03l,18jun98,cjtc including header files to prevent compilation warnings for
                 WV2.0 (SPR 21494)
03s,04may98,cym  fixed end and edata for SIMNT.
03r,27jan98,cth  changed INCLUDE_INSTRUMENTATION to INCLUDE_WINDVIEW,
		 removed connLib.h
03p,21jan98,jmb  removed end and edata for SIMNT (only when using Visual C)
03o,02jan98,cym  removed end and edata for SIMNT.
03n,16nov97,cth  removed includes of evtTsfsSockLibP.h and evtSockLibP.h
03m,21aug97,cth  added conditional include of evtTsfsSockLibP.h
03l,18aug97,cdp  added conditional declaration of thumbSymTblAdd().
03k,07apr97,spm  added support for unit numbers
03j,24feb97,gnn  added conditional RIP includes
03i,27nov96,spm  added conditional includes for DHCP client
03h,28dec96,ms   added conditional include of tapeFs.h
03g,13dec96,elp  changed syncLib.c into symSyncLib.c
03f,30oct96,elp  Added syncLib.h.
03e,09aug96,dbt  include all posix headers if INCLUDE_POSIX_ALL is defined
		 (SPR #5524).
03d,26jul96,hdn  added a function declaration of usrAtaConfig().
03c,20mar96,hdn  made inclusion of pppLib.h conditional.
03b,15mar96,sgv  made inclusion of strmLib.h conditional.
03a,22jun95,caf  no longer need "_" for GHSxPPC.
02m,10oct95,dat	 removed #include "bspVersion.h"
02l,27jul95,dzb  added strmLib.h inclusion.  Added usrStrmInit().
02k,25jul95,dzb  added sockLib.h and bsdSockLib.h inclusion.
02j,09may95,ms	 prototype for WDB agent
02i,28mar95,kkk  changed edata and end to arrays (SPR #3917)
02h,09jan95,dzb  added prototype for PPP.
02h,22may95,yao  add "_" to edata, end for PowerPC with Green Hills tool.
02g,25oct93,hdn  added prototype for floppy and IDE driver.
02m,22feb94,elh  added function defs for snmp.
02m,25jul94,jmm  removed prototype of printConfig() (spr 3461)
02l,28jan94,dvs  changed INCLUDE_POSIX_MM_MAN to INCLUDE_POSIX_MEM
02k,12jan94,kdl  changed aio includes.
02j,10dec93,smb  added include of seqDrvP.h  for windview
02i,07dec93,dvs  added #ifdef's around POSIX includes for component releases
02h,02dec93,dvs  added includes for POSIX modules
02g,22nov93,dvs  added include of sched.h
02f,23aug93,srh  added include of private/cplusLibP.h.
02e,20oct92,pme  added vxLib.h.
02d,22sep92,rrr  added support for c++
02c,21sep92,ajm  fixes for lack of #elif support on mips
02b,20sep92,kdl  added include of private/ftpLibP.h.
02a,18sep92,jcf  major clean up.  added prototypes and #includes.
01d,04jul92,jcf  cleaned up.
01c,26may92,rrr  the tree shuffle
01b,04oct91,rrr  passed through the ansification filter
		  -fixed #else and #endif
		  -changed VOID to void
		  -changed copyright notice
01a,05oct90,shl created.
*/

#ifndef __INCusrConfigh
#define __INCusrConfigh

#ifdef __cplusplus
extern "C" {
#endif

#include "bootLib.h"
#include "bsdSockLib.h"
#include "cacheLib.h"
#include "cplusLib.h"
#include "ctype.h"
#include "dbgLib.h"
#include "dosFsLib.h"
#include "cbioLib.h"
#include "tarLib.h"
#include "dcacheCbio.h"
#include "dpartCbio.h"
#include "usrFdiskPartLib.h"
#include "ramDiskCbio.h"
#include "envLib.h"
#include "errno.h"
#include "esf.h"
#include "fcntl.h"
#include "fioLib.h"
#include "fppLib.h"
#include "ftpLib.h"
#include "ftpdLib.h"
#include "hostLib.h"
#include "ifLib.h"
#include "if_bp.h"
#include "if_sl.h"
#include "inetLib.h"
#include "intLib.h"
#include "ioLib.h"
#include "iosLib.h"
#include "iv.h"
#include "kernelLib.h"
#include "loadLib.h"
#include "logLib.h"
#include "loginLib.h"
#include "math.h"
#include "memLib.h"
#include "mmuLib.h"
#include "msgQLib.h"
#include "netDrv.h"
#include "netLib.h"
#include "netShow.h"
#include "xdr_nfs.h"
#include "nfsDrv.h"
#include "nfsLib.h"
#include "pipeDrv.h"
#include "proxyArpLib.h"
#include "qLib.h"
#include "qPriBMapLib.h"
#include "ramDrv.h"
#include "rawFsLib.h"
#include "rebootLib.h"
#include "remLib.h"
#include "rlogLib.h"
#include "routeLib.h"
#include "rpcLib.h"
#include "rt11FsLib.h"
#include "scsiLib.h"
#include "selectLib.h"
#include "semLib.h"
#include "shellLib.h"
#include "sockLib.h"
#include "spyLib.h"
#include "stdio.h"
#include "string.h"
#include "symSyncLib.h"
#include "sysLib.h"
#include "sysSymTbl.h"
#include "taskHookLib.h"
#include "taskLib.h"
#include "taskVarLib.h"
#include "telnetLib.h"
#include "tftpLib.h"
#include "tftpdLib.h"
#include "tickLib.h"
#include "time.h"
#include "timers.h"
#include "timexLib.h"
#include "unistd.h"
#include "unldLib.h"
#include "usrLib.h"
#include "version.h"
#include "vxLib.h"
#include "wdLib.h"
#include "net/mbuf.h"
#include "rpc/rpcGbl.h"
#include "rpc/rpctypes.h"
#include "rpc/xdr.h"
#include "private/cplusLibP.h"
#include "private/excLibP.h"
#include "private/ftpLibP.h"
#include "private/kernelLibP.h"
#include "private/sigLibP.h"
#include "private/vmLibP.h"
#include "private/workQLibP.h"

/* INCLUDE_SM_COMMON */

#include "smLib.h"
#include "smUtilLib.h"

/* INCLUDE_SM_OBJ */

#include "smObjLib.h"
#include "smDllLib.h"
#include "smMemLib.h"
#include "smNameLib.h"

/* INCLUDE_SM_NET */

#include "smPktLib.h"
#include "drv/netif/smNetLib.h"

#ifdef	INCLUDE_TAPEFS
#include "seqIo.h"
#include "tapeFsLib.h"
#endif	/* INCLUDE_TAPEFS */

#ifdef	INCLUDE_STANDALONE_SYM_TBL
#include "symbol.h"
#endif	/* INCLUDE_STANDALONE_SYM_TBL */

#ifdef	INCLUDE_PPP
#include "pppLib.h"
#endif	/* INCLUDE_PPP */

#if defined(INCLUDE_STREAMS) || defined(INCLUDE_STREAMS_ALL)
#include "strmLib.h"
#endif	/* INCLUDE_STREAMS */

#ifdef INCLUDE_RIP
#include "rip/ripLib.h"
#endif	/* INCLUDE_RIP */

#ifdef	INCLUDE_POSIX_ALL

#include "aio.h"
#include "aioSysDrv.h"
#include "private/mqPxLibP.h"
#include "mqPxShow.h"
#include "pthread.h"
#include "sched.h"
#include "private/semPxLibP.h"
#include "semPxShow.h"
#include "sys/mman.h"

#else   /* INCLUDE_POSIX_ALL */

#ifdef	INCLUDE_POSIX_AIO
#include "aio.h"
#ifdef	INCLUDE_POSIX_AIO_SYSDRV
#include "aioSysDrv.h"
#endif  /* INCLUDE_POSIX_AIO_SYSDRV */
#endif	/* INCLUDE_POSIX_AIO */

#ifdef	INCLUDE_POSIX_MQ
#include "private/mqPxLibP.h"
#ifdef	INCLUDE_SHOW_ROUTINES
#include "mqPxShow.h"
#endif  /* INCLUDE_SHOW_ROUTINES */
#endif  /* INCLUDE_POSIX_MQ */

#ifdef INCLUDE_POSIX_PTHREADS
#include "pthread.h"
#endif  /* INCLUDE_POSIX_PTHREADS */

#ifdef	INCLUDE_POSIX_SCHED
#include "sched.h"
#endif	/* INCLUDE_POSIX_SCHED */

#ifdef	INCLUDE_POSIX_SEM
#include "private/semPxLibP.h"
#ifdef	INCLUDE_SHOW_ROUTINES
#include "semPxShow.h"
#endif  /* INCLUDE_SHOW_ROUTINES */
#endif	/* INCLUDE_POSIX_SEM */

#ifdef	INCLUDE_POSIX_MEM
#include "sys/mman.h"
#endif  /* INCLUDE_POSIX_MEM */

#endif  /* INCLUDE_POSIX_ALL */

#if     defined(INCLUDE_RBUFF)
#include "rBuffLib.h"
#endif  /* INCLUDE_RBUFF */

#if 	defined(INCLUDE_WINDVIEW)
#include "wvLib.h"
#include "wvTmrLib.h"
#include "private/seqDrvP.h"
#endif  /* INCLUDE_WINDVIEW */

#if	defined(INCLUDE_TRIGGERING)
#include "private/trgLibP.h"
#endif  /* INCLUDE_TRIGGERING */

#if     defined(INCLUDE_AOUT)
#include "loadAoutLib.h"
#else   /* coff or ecoff */
#if     defined(INCLUDE_ECOFF)
#include "loadEcoffLib.h"
#else   /* coff */
#if     defined(INCLUDE_COFF)
#include "loadCoffLib.h"
#else   /* elf */
#if     defined(INCLUDE_ELF)
#include "loadElfLib.h"
#endif                                          /* mips cpp no elif */
#endif
#endif
#endif

#if defined(INCLUDE_SOUND) && defined(INCLUDE_SB16)
#include "drv/sound/sb16drv.h"
#endif /* INCLUDE_SOUND && INCLUDE_SB16 */

/* variable declarations */

#if (CPU!=SIMNT)
extern char	edata [];		/* automatically defined by loader */
extern char	end [];			/* automatically defined by loader */
#endif

/* function declarations */

#if defined(__STDC__) || defined(__cplusplus)

extern void	usrInit (int startType);
extern void	usrRoot (char *pMemPoolStart, unsigned memPoolSize);
extern void	usrClock (void);
extern STATUS	usrScsiConfig (void);
extern STATUS   usrFdConfig (int type, int drive, char *fileName);
extern STATUS   usrIdeConfig (int drive, char *fileName);
extern STATUS	usrAtaConfig (int ctrl, int drive, char *fileName);
extern void     devSplit (char *fullFileName, char *devName);
extern STATUS	usrNetInit (char *bootString);
extern STATUS	checkInetAddrField (char *pInetAddr, BOOL subnetMaskOK);
extern STATUS	loadSymTbl (char *symTblName);
extern STATUS	netLoadSymTbl (void);
extern STATUS	usrBootLineCrack (char *bootString, BOOT_PARAMS *pParams);
extern void	usrBootLineInit (int startType);
extern STATUS	usrBpInit (char * devName, int unitNum, u_long startAddr);
extern void	usrDemo (void);
extern STATUS	usrNetIfAttach (char *devName, int unitNum, char *inetAdrs);
extern STATUS	usrSlipInit (char * pBootDev, int unitNum, char * localAddr,
                             char *peerAddr);
extern STATUS	usrPPPInit (char * pBootDev, int unitNum, char * localAddr,
                            char *peerAddr);

#ifdef  INCLUDE_SM_OBJ
extern STATUS	usrSmObjInit (char * bootString);
#endif  /* INCLUDE_SM_OBJ */

extern STATUS	usrWindMPInit (char * bootString);
extern void	usrStartupScript (char *fileName);
extern STATUS	usrStrmInit (void);
extern STATUS	usrNetIfConfig (char *devName, int unitNum, char *inetAdrs,
                                char *inetName, int netmask);
extern STATUS   usrSnmpdInit (void);
extern STATUS 	usrMib2Init (void);
extern STATUS 	usrMib2CleanUp (void);
extern STATUS	wdbConfig (void);

#if (defined(INCLUDE_STANDALONE_SYM_TBL) && (CPU_FAMILY == ARM) && ARM_THUMB)
extern STATUS   thumbSymTblAdd (SYMTAB_ID, SYMBOL *);
#endif

#else	/* __STDC__ */

extern void	usrInit ();
extern void	usrRoot ();
extern void	usrClock ();
extern STATUS	usrScsiConfig ();
extern STATUS   usrFdConfig ();
extern STATUS   usrIdeConfig ();
extern STATUS	usrAtaConfig ();
extern void     devSplit ();
extern STATUS	usrNetInit ();
extern STATUS	checkInetAddrField ();
extern STATUS	loadSymTbl ();
extern STATUS	netLoadSymTbl ();
extern STATUS	usrBootLineCrack ();
extern void	usrBootLineInit ();
extern STATUS	usrBpInit ();
extern void	usrDemo ();
extern STATUS	usrNetIfAttach ();
extern STATUS	usrSlipInit ();
extern STATUS	usrPPPInit ();
#ifdef  INCLUDE_SM_OBJ
extern STATUS	usrSmObjInit ();
#endif  /* INCLUDE_SM_OBJ */
extern STATUS	usrWindMPInit ();
extern void	usrStartupScript ();
extern STATUS	usrStrmInit ();
extern STATUS	usrNetIfConfig ();
extern STATUS   usrSnmpdInit ();
extern STATUS 	usrMib2Init ();
extern STATUS 	usrMib2CleanUp ();
extern STATUS 	wdbConfig ();
#if (defined(INCLUDE_STANDALONE_SYM_TBL) && (CPU_FAMILY == ARM) && ARM_THUMB)
extern STATUS   thumbSymTblAdd ();
#endif

#endif	/* __STDC__ */

#ifdef __cplusplus
}
#endif

#endif /* __INCusrConfigh */
