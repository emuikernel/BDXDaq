/* if_bp.c - original VxWorks (and SunOS) backplane network interface driver */

/* Copyright 1984 - 1997 Wind River Systems, Inc. */
#ifndef UNIX
#include "copyright_wrs.h"
#else   /* UNIX */
static char *copyright=
     "Copyright 1984 - 1997 Wind River Systems, Inc.";
#endif  /* UNIX */

/*
modification history
--------------------
01u,09oct97,spm  removed driver initialization from ioctl support (SPR #8831)
01t,07apr97,spm  code cleanup, corrected statistics, and upgraded to BSD 4.4
01s,25aug95,hk   use cacheDmaMalloc() for shared memory on SH7600 targets.
01r,12nov94,tmk  removed some IMPORTed function decs and included unixLib.h
01q,29mar94,caf  use cacheDmaMalloc() for shared memory on MIPS targets.
01p,23apr93,caf  ansification: changed cast of free() parameter.
01o,22feb93,jdi  documentation cleanup.
01n,18sep92,jcf  clean up of warnings.
01m,09sep92,gae  documentation tweaks.
01l,18jul92,smb  Changed errno.h to errnoLib.h.
01k,11jun92,elh  fixed memory sizing bug - SPR 1481.
01j,04jun92,ajm  added test and set suport for mips
01i,26may92,rrr  the tree shuffle
		  -changed includes to have absolute path from h/
01h,26may92,ajm  got rid of HOST_DEC def's (new compiler)
04g,05may92,wmd  changed LITTLE_ENDIAN to _LITTLE_ENDIAN, removed conditional
		 for i960 to pass IPL vec parameter in sysMailboxEnable().
04f,07apr92,yao  removed duplicated macro ROUND_UP.  removed unneccessary
		 ROUND_UP for SPARC.  changed copyright notice.
04e,14feb92,wmd  added BP_NTOHS, BP_HTONS for BUF_NODE->b_len conversions.
		 Fixed ansi warnings.
04d,27nov91,rfs  fixed bug in bpStart(), which did not handle errors
                 correctly, causing memory depletion. Also fixed ANSI warnings.
04c,07oct91,rrr  turned off some checking in the r3000
04b,04oct91,rrr  passed through the ansification filter
                  -changed functions to ansi style
                  -changed functions to ansi style
		  -changed includes to have absolute path from h/
		  -fixed #else and #endif
		  -changed READ, WRITE and UPDATE to O_RDONLY O_WRONLY and O_RDWR
		  -changed VOID to void
		  -changed copyright notice
04a,01oct91,ajm  got rid of conditional comp (MIPS) for ItoKval, and KtoIval
03z,10sep91,wmd  added conditionals for MIPS.
03y,21aug91,ajm  added macros to allocate and free mips dynamic memory
                 from uncached space
                 cast line 1250 comparison with 0 to struct mbuf * to
		 resolve warning in mips land
03x,15aug91,rrr  added IPL to sysMailboxEnable() for I960_FAMILY
03w,23jul91,rrr  added hton and ntoh stuff so I960 would work
03v,12jul91,wmd  minor cleanup of 03u.  Added Al Maillet's fix to bpXmit
		 for the case that ringput returns > 1 available slots.
03u,24may91,hvh  deleted 'short-macros' because they screwed up
	   +jwt	 and besides, they are not needed since a short on
		 the SPARC is 2 bytes (so no aligment problem);
		 deleted 'KLUDGES' for SPARC since they make no sence
		 and introduce gigantic interrupt-latencys in, what
		 is supposed to be, a realtime operating system;
		 included ELH 29dec91 bpPollTask check for NULL bp;
		 fixed TAS(x) macro for Sun-1E and others - bpTas().
03t,29apr91,hdn  use SIZEOF_ETHERHEADER instead of sizeof(ether_header).
03s,10apr91,jdi  documentation cleanup; doc review by dnw.
03r,29dec91,elh  changed bpPollTask to check if bp is null.
03q,10aug90,dnw  fixed forward declarations for void functions.
		 added include of if_subr.h.
03p,12aug90,hjb  added bpSlotCheck() to help BOOTP.
		 changed bp pseudo-Ethernet addr to be all 00.
03o,18jul90,hjb  deleted cluster-loan code temporarily; fixed a bug caused
		 by the macro ROUND_UP.
03n,15jul90,dnw  added b_pad to BUF_NODE structure to make it long aligned.
		 changed to round up BP_BUFSIZ to be long aligned.
		 removed B_COPY, B_ASSIGN, B_FROM macros
		 coerced malloc() to (char*) where necessary.
03m,11jul90,hjb  removed references to ipintr().
03l,26jun90,hjb  fixes to accommodate SunOS; copy_from_mbufs() & m_freem() fix;
		 changed params to build_cluster()
03k,19apr90,hjb  deleted param.h, added BP_SLOT_NUM, BP_MAX_LOANS,
		 DEBUG_OPTION_5, bpRIngFree(), modified bpRead()
		 to optimize throughput via cluster-callback-hack.
03j,18mar90,hjb  isolated out SunOS compatible routines for if_subr.c.
03i,07aug89,gae  adjusted BUF_NODE definition to work on SPARC; changed
		   iv68k.h to iv.h; fixed ether_attach to not append unit.
03h,14jun89,gae  glitch in 03g: left bpParity on and wrong poll task priority.
03g,14mar89,gae  used unsigned int's for pointer arithmetic.
		 made Ethernet address unique for multiple units.
		 slight reorganization of bpPollTask. Added more DEBUG levels.
03f,28mar89,gae  turned off debugging; fixed bp BUFSIZ to be bigger
		   than MTU to allow for ether_header overhead -- bug fix.
03e,10feb89,gae  taught SunOS to do mailboxes and about bus interrupts;
	   +jcf  added testing in bpInit for sufficient memory;
           +dnw  added read mailboxes; fixed mbuf bug in copy_to_mbufs.
		 UNIX ver. now compiles under SunOS 4.0 and has SUN3_5 flag.
03c,10dec88,gae  SunOS version tweaks.
03b,20nov88,dnw  lint
03a,13sep88,gae  merged SunOS and VxWorks versions.  Documentation.
	   +dnw  debugged SunOS version.
	   +ak   wrote SunOS version.
02l,23jun88,rdc  added IFF_BROADCAST to if_flags.
02k,06jun88,dnw  changed taskSpawn/taskCreate args.
		 changed NCPU from 9 to 32.
02j,30may88,dnw  changed to v4 names.
02i,12apr88,gae  added include of ioLib.h for O_RDONLY/O_WRONLY/O_RDWR.
02h,28jan88,jcf  made kernel independent.
02g,20feb88,dnw  lint
02f,05jan88,rdc  added include of systm.h
02e,20nov87,ecs  lint
	    dnw  changed bpattach to take int vec num instead of vec adrs.
02d,13nov87,dnw  added heartbeat & anchor.
02c,03nov87,dnw  fixed bug in bpRead of throwing away packets that were
		   larger than 1500 bytes.
		 make bpPoll be global so that it is legible in i().
		 added intConnect() call for bus interrupt connects in
		   bpConnect().
		 changed bpReboot() into bpReset().
02b,26oct87,jcf	 added bpConnect that does a sysMailboxConnect in the case of
	     &    mailbox interrupts or a spawn of bpPoll for non-interrupt
	    dnw   driven backplanes
		 changed to not probe shared memory unless cpu 0.
02a,22apr87,dnw  rewritten.
01b,08apr87,rdc  moved call to bpConnect from bpAttach to bpInit.
01x,01apr87,jlf  delinted, removed ifdef CLEAN.
		 changed calls to tas () to vxTas ().
		 added copyright.
*/

/*
DESCRIPTION
This module implements the original VxWorks backplane network interface driver.

The backplane driver allows several CPUs to communicate via shared
memory.  Usually, the first CPU to boot in a card cage is considered the
backplane master.  This CPU has either dual-ported memory or an additional
memory board which all other CPUs can access.  Each CPU must be
interruptible by another CPU, as well as be able to interrupt all other
CPUs.  There are three interrupt types: polling, mailboxes, and VMEbus
interrupts.  Polling is used when there are no hardware interrupts; each
CPU spawns a polling task to manage transfers.  Mailbox interrupts are the
preferred method, because they do not require an interrupt level.  VMEbus
interrupts offer better performance than polling; however, they may
require hardware jumpers.

There are three user-callable routines: bpInit(), bpattach(), and bpShow().
The backplane master, usually processor 0, must initialize the backplane
memory and structures by first calling bpInit().  Once the backplane has
been initialized, all processors can be attached via bpattach().  Usually,
bpInit() and bpattach() are called automatically in usrConfig.c when
backplane parameters have been specified in the boot line.

The bpShow() routine is provided as a diagnostic aid
to show all the CPUs configured on a backplane.

For more information about SunOS use of this driver, see the 
.I "Guide to the VxWorks Backplane Driver for SunOS."

This driver has been replaced by if_sm and is included here for backwards
compatibility.

MEMORY LAYOUT
All pointers in shared memory are relative to the start of shared memory,
since dual-ported memory may appear in different places on different CPUs.

.ne 11
.CS
   ----------------------------	low address (anchor)
   |       ready value        |	    1234567
   |         heartbeat        |	    increments 1/sec
   |   pointer to bp header   |
   |         watchdog         |     for backplane master CPU
    --------------------------
   the backplane header may be contiguous or
   allocated elsewhere on the master CPU
.ne 7 
   ---------------------------- backplane header
   |    backplane header      |	    1234567
   |       num CPUs           |     unused
   |    Ethernet address      |     (6 bytes)
   |   pointer to free ring   |
   |--------------------------|

.ne 11
   |--------------------------| cpu descriptor
   |        active            |	    true/false
   |   processor number       |	    0-NCPU
   |         unit             |     0-NBP
   |   pointer to input ring  |
   |    interrupt type        |     POLL  | MAILBOX    |  BUS
   |    interrupt arg1        |     none  | addr space | level
   |    interrupt arg2        |     none  | address    | vector
   |    interrupt arg3        |     none  | value      | none
   |--------------------------|
              ...               repeated MAXCPU times
.ne 16
   |--------------------------|
   |       free ring          |	    contains pointers to buffer nodes
   |--------------------------|
   |       input ring 1       |	    contains pointers to buffer nodes
   |--------------------------|
	      ...
   |--------------------------|
   |       input ring n       |
   |--------------------------|

   |--------------------------| buffer node 1
   |     address, length      |
   |--------------------------|
   |       data buffer 1      |
   |--------------------------|
	      ...
.ne 6
   |--------------------------| buffer node m
   |     address, length      |
   |--------------------------|
   |       data buffer m      |
   ----------------------------	high address
.CE

BOARD LAYOUT
This device is "software only."  A jumpering diagram is not applicable.


INTERNAL
 SOFTWARE ARCHITECTURE
.CS
    bpIoctl   |    bpOutput   bpPoll   bpattach  bpReset  bpInit
      |    __/        |         |         / \               |
      |   /           |         |        /   \              |
      |  /            |         |       /     \             |
     bpIfinit    ether_output   |     bpAlive bpInit*    bpSizeMem
      / | \           |         |
 bpAlive|bpIntEnable bpPut      |
     bpConnect        |         | (interrupt level)
       |            bpXmit      /    /
       |               \       /    /
     bpPollTask         \     /    /
     bpPollUnix          \   /    /
                          \ /  __/
                         bpintr
                           |
                        bpReadAll
                           |
                         bpRead

 * UNIX only; called directly in VxWorks

.CE

SEE ALSO: ifLib
*/

#ifdef	UNIX

#ifndef	KERNEL
#define	KERNEL
#endif	/* KERNEL */

#ifdef	SUN3_5
#include "h/types.h"
#include "h/systm.h"
#include "h/mbuf.h"
#include "h/buf.h"
#include "h/map.h"
#include "h/socket.h"
#include "h/vmmac.h"
#include "h/errno.h"
#else	/* SUN3_5 */
#include "sys/types.h"
#include "sys/systm.h"
#include "sys/mbuf.h"
#include "sys/buf.h"
#include "sys/map.h"
#include "sys/socket.h"
#include "sys/param.h"
#include "sys/vmmac.h"
#include "sys/errno.h"
#endif	/* SUN3_5 */

#include "machine/param.h"
#include "machine/pte.h"

#ifdef	SUN3_5
#include "h/time.h"
#include "h/kernel.h"
#include "h/ioctl.h"
#else	/* SUN3_5 */
#include "sys/time.h"
#include "sys/kernel.h"
#include "sys/ioctl.h"
#endif	/* SUN3_5 */

#include "net/if.h"
#include "netinet/in.h"
#include "netinet/if_ether.h"

#include "sundev/mbvar.h"

#ifdef	SUN3_5
#include "h/protosw.h"
#else	/* SUN3_5 */
#include "sys/protosw.h"
#endif	/* SUN3_5 */

#include "net/route.h"
#include "netinet/in_systm.h"
#include "netinet/ip.h"
#include "netinet/ip_var.h"

#ifdef	INET		/* "vxWorks.h" defines INET */
#undef	INET
#endif	/* INET */

#include "vxWorks.h"
#include "vme.h"	/* vxWorks vme AM's */

#define BP_MASTER       (bp_procnum == 0)

IMPORT int bp_tas;
IMPORT int bp_procnum;
IMPORT int bp_n;
IMPORT int bp_mem_size;
IMPORT char *bp_mem_adrs;
IMPORT int bp_int_type;
IMPORT int bp_int_arg1;
IMPORT int bp_int_arg2;
IMPORT int bp_int_arg3;

IMPORT void wakeup ();
IMPORT struct mbuf *copy_to_mbufs ();

#else	/* UNIX */

#include "vxWorks.h"

#include "iv.h"
#include "taskLib.h"
#include "memLib.h"
#include "ioLib.h"
#include "ioctl.h"
#include "etherLib.h"
#include "wdLib.h"
#include "iosLib.h"
#include "intLib.h"
#include "logLib.h"
#include "netLib.h"
#include "net/mbuf.h"
#include "socket.h"
#include "net/protosw.h"
#include "errnoLib.h"
#include "net/systm.h"
#include "net/route.h"
#include "netinet/in.h"
#include "netinet/in_systm.h"
#include "netinet/ip.h"
#include "netinet/ip_var.h"
#include "net/if.h"
#include "net/unixLib.h"
#include "netinet/if_ether.h"
#include "net/if_subr.h"
#include "netinet/in_var.h"
#include "vxLib.h"
#include "stdio.h"
#include "stdlib.h"
#include "sysLib.h"
#if	((CPU_FAMILY==MIPS) || (CPU_FAMILY==SH))
#include "cacheLib.h"
#endif	/* CPU_FAMILY==MIPS || CPU_FAMILY==SH */

#endif	/* UNIX */

#include "drv/netif/if_ring.h"
#include "if_bp.h"

IMPORT struct ifnet *ifunit ();
#if defined (BSD43_DRIVER) || defined (UNIX)
IMPORT int    ether_output (struct ifnet *ifp, struct mbuf *m0,
		     struct sockaddr *dst, FUNCPTR startRtn,
		     struct arpcom *pArpcom);
#endif
IMPORT int    set_if_addr (struct ifnet *ifp,char *data,u_char *enaddr);
IMPORT void   m_freem (struct mbuf *m);

#define	MAXCPU		32		/* don't change! */
#define	NCPU		32		/* number of input rings to init */
#define	MINPKTSIZE	60		/* minimum packet size to send */
#define BP_SLOT_NUM	32		/* number of the input ring slots */

#define BP_MTU		((2 * 1024) + 128)
int bp_mtu = BP_MTU;			/* patch before calling bpInit */

/* round up bufsiz to be long aligned */

#define BP_BUFSIZ       (ROUND_UP (bp_mtu + SIZEOF_ETHERHEADER, 4))

#ifdef UNIX
#define logMsg          printf
#define	WDOG_ID		int
#define vir2phys(v)	(unsigned)ptob((unsigned)hat_getkpfnum(v))
#define	splimp()	spl3()
#define NBP		1		/* max # bp interfaces */
#else	/* UNIX */
#define	hz	(sysClkRateGet ())
#define NBP		2		/* max. # bp interfaces */
#define vir2phys(v)	(v)
#endif	/* UNIX */


typedef struct ring RING;

typedef struct			/* buffer node */
    {
    char	*b_addr;	/* buffer address (as offset) */
    short       b_len;		/* buffer length */
    short	b_pad;		/* padding to make structure long aligned */
    } BUF_NODE;

typedef struct			/* CPU descriptor in bp header */
    {
    BOOL	cd_active;	/* CPU is present */
    int		cd_cpuNum;	/* processor number */
    int		cd_unit;	/* unit on own CPU */
    RING	*cd_pInputRing;
    int		cd_intType;	/* interrupt: poll, bus, or mailbox */
    int		cd_intArg1;
    int		cd_intArg2;
    int		cd_intArg3;
    } CPU_DESC;

typedef struct			/* bp header */
    {
    int		hdr_ready;	/* used only as marker for debugging */
    short	hdr_ncpu;	/* unused */
    u_char	hdr_enetAdrs[6];/* "Ethernet address" */
    RING	*hdr_pFreeRing;
    CPU_DESC	hdr_cpuDesc[MAXCPU];
    } BP_HEADER;

typedef struct			/* bp anchor */
    {
    int		an_ready;	/* special value 1234.... */
    UINT	an_heartbeat;	/* increments every second */
    BP_HEADER	*an_pHdr;	/* separate to minimize usage of low memory */
    WDOG_ID	an_wdog;	/* for VxWorks version only */
    } BP_ANCHOR;


#define BP_READY	  0x12345678	/* distinctive ready value */
#define BP_READY_SOFT_TAS 0x12344321	/* ready value with software tas */

#if _BYTE_ORDER==_LITTLE_ENDIAN
#define BP_NTOHL(v)	ntohl(v)
#define BP_NTOHS(v)	ntohs(v)
#define BP_HTONL(v)	htonl(v)
#define BP_HTONS(v)	htons(v)
#else
#define BP_NTOHL(v)	(v)
#define BP_NTOHS(v)	(v)
#define BP_HTONL(v)	(v)
#define BP_HTONS(v)	(v)
#endif  /* _BYTE_ORDER==_LITTLE_ENDIAN */

#define	ItoKval(p,type,base)	((type)(ntohl((u_int)(p)) + (u_int)(base)))
#define	KtoIval(p,type,base)	((type)htonl((u_int)(p) - (u_int)(base)))

/*
 * Net software status per interface.
 *
 * Each interface is referenced by a network interface structure,
 * bs_if, which the routing code uses to locate the interface.
 * This structure contains the output queue for the interface, its address, ...
 */

typedef struct
    {
    struct	arpcom bs_ac;	/* common Ethernet structures */
#define bs_if           bs_ac.ac_if     /* network-visible interface */
#define bs_enaddr       bs_ac.ac_enaddr /* hardware Ethernet address */
    int		bs_procNum;	/* processor number on this net */
    BP_ANCHOR	*bs_pAnchor;	/* pointer to bp anchor */
    BP_HEADER	*bs_pHdr;	/* pointer to bp header */
    volatile RING       *bs_pFreeRing;  /* pointer to free ring */
    volatile RING       *bs_pInputRing; /* pointer to our input ring */
    volatile BOOL       bs_readPending; /* read interrupt is outstanding */
    int		bs_intType;	/* interrupt type: poll, bus, mailbox */
    int		bs_intArg1;
    int		bs_intArg2;
    int		bs_intArg3;
#ifdef	UNIX
    caddr_t 	bs_addr;
    u_char 	bs_ivec;
#endif	/* UNIX */
    } BP_SOFTC;

#ifdef	UNIX
extern struct mb_driver bpdriver;
struct mb_device *bpinfo[NBP];

LOCAL BP_SOFTC bp_softc [NBP];
#define	BP_SOFTC_P	&bp_softc
#define	dbgprintf	printf
#define	sysProcNumGet()	(bp_procnum)
FUNCPTR bpProbeType;

LOCAL char *cpudesc_vaddr [MAXCPU];
#else	/* UNIX */

/* bpPollTask parameters */
int bpPollTaskId;
int bpPollTaskOptions	= VX_SUPERVISOR_MODE | VX_UNBREAKABLE;
int bpPollTaskStackSize	= 2000;
int bpPollTaskPriority	= 250;

LOCAL BP_SOFTC *bp_softc[NBP];
#define	BP_SOFTC_P	bp_softc
#define	dbgprintf	logMsg
int bpProbeType;
#endif	/* UNIX */

LOCAL  BOOL vxTasac ();

/*
 * the default TAS, use undef if you dont like it
 */
#define TAS(x) 	(bpTasOK ? sysBusTas (x) : vxTasac (x))
#if     (CPU_FAMILY==MIPS)
#define TAS_CLEAR(x)    (bpTasOK ? sysBusClearTas (&(x)) : ((x)=0))
#endif  /* CPU_FAMILY==MIPS */

#if 	CPU_FAMILY==SPARC
IMPORT BOOL vxTas ();
IMPORT BOOL sysBusTas ();
FUNCPTR         bpTasRtn = vxTasac;  /* Test-and-Set Routine */
#undef TAS(x)
#define TAS(x)  (*bpTasRtn)(x)

/*******************************************************************************
*
* bpTas - determine backplane test-and-set macro TAS(x)
*
* NOMANUAL
*/

void bpTas (bp_tas, bp_off_board, bp_target)
    BOOL bp_tas;	/* BP_TAS */
    BOOL bp_off_board;	/* BP_OFF_BOARD */
    int  bp_target;	/* target id */

    {
    if (!bp_tas)
      bpTasRtn = vxTasac;       /* software TAS */
    else
	{
	/* hardware TAS */
	if (bp_target == 0x1E)
	    {
	    /* Sun-1E */
	    if (bp_off_board)
		bpTasRtn = vxTas;
	    else
		bpTasRtn = sysBusTas;
	    }
	else
	    {
	    if (bp_off_board)
		bpTasRtn = sysBusTas;
	    else
		bpTasRtn = vxTas;
	    }
	}
    }

#endif	/* CPU_FAMILY==SPARC */

#ifdef	UNIX
#define intLock		spl7
#define intUnlock	splx
#endif	/* UNIX */


#define	DELAY_COUNT	5000	/* how high to count when waiting on TAS */

#ifdef	UNIX
IMPORT int bpParity;
IMPORT int bpDebug;	/* flags: 1-usage, 2-dropped packets */
#else	/* UNIX */
int bpParity = FALSE;
int bpDebug;		/* flags: 1-usage, 2-dropped packets */
#endif	/* UNIX */

#define	DEBUG_OPTION_1	(bpDebug & 0x1)
#define	DEBUG_OPTION_2	(bpDebug & 0x2)
#define	DEBUG_OPTION_3	(bpDebug & 0x4)
#define	DEBUG_OPTION_4	(bpDebug & 0x8)
#define DEBUG_OPTION_5	(bpDebug & 0x10)

			/*  0, 0, 0, 0, unit, host */
u_char bpEnetAdrs [6]	= { 0, 0, 0, 0, 0, 0 };

/* local variables */

LOCAL BOOL bpTasOK;		/* whether hardward can do test-and-set */
LOCAL int bpHeartbeatRate;
int bpBitBucket;		/* spare change */

/* forward declarations */

void bpPollTask ();
void bpintr ();

/* forward static functions */

static void bpReset (int unit);
static STATUS bpAlive (BP_SOFTC *bs);
static STATUS bpIfInit (int unit);
static void bpReadAll (int unit);
static STATUS bpRead (BP_SOFTC *bs, BUF_NODE *pBufNode, BUF_NODE
		*pBufNodeOrig);
#if defined BSD43_DRIVER || defined (UNIX)
static void bpStart (int unit);
static int bpOutput (struct ifnet *ifp, struct mbuf *m0, struct sockaddr
		*dst);
#else
static void bpStart (BP_SOFTC * 	bs);
#endif

static STATUS bpPut (BP_SOFTC *bs, struct mbuf *m);
static void bpXmit (BP_SOFTC *bs, BUF_NODE *pBufNode, int destProcNum, int
		srcProcNum);
static void bpIntAck (BP_SOFTC *bs);
static STATUS bpConnect (BP_SOFTC *bs, int unit);
static STATUS bpIntEnable (BP_SOFTC *bs);
static void bpIntGen (CPU_DESC *pCpu, int srcProcNum);
static void bpHeartbeat (BP_ANCHOR *pAnchor);
static int bpSizeMem (caddr_t memAdrs, int memSize);
static int bpIoctl (struct ifnet *ifp, int cmd, caddr_t data);
static void ringinit (RING *rp, short size);
#if CPU==R3000
static int ringput ();
static int ringget ();
#else
static int ringput (RING *rp, int v);
static int ringget (RING *rp);
#endif
static int ringcount (RING *pRing);
static BOOL vxTasac (char *address);

#ifdef	UNIX
LOCAL void	bpRead ();
#else	/* UNIX */
LOCAL STATUS	bpRead ();
#endif	/* UNIX */

#ifdef	UNIX
/*******************************************************************************
*
* bpattach - attach the `bp' interface to the network
*
* Make the `bp' interface available by filling in the network interface record.
* The system will initialize the interface when it is ready to accept packets.
*
* RETURNS: N/A
*
* NOMANUAL
*/

void bpattach (md)
    struct mb_device *md;

    {
    int unit          = md->md_unit;
    FAST BP_SOFTC *bs = BP_SOFTC_P [unit];
    struct ifnet *ifp = &bs->bs_if;

    bs->bs_procNum = bp_procnum;

    bs->bs_intType = bp_int_type;
    bs->bs_intArg1 = bp_int_arg1;
    bs->bs_intArg2 = bp_int_arg2;
    bs->bs_intArg3 = bp_int_arg3;

    bs->bs_addr = md->md_addr;
    bs->bs_ivec = md->md_intr->v_vec;

    ether_attach (ifp, unit, "bp", bpIfInit, bpIoctl, bpOutput,
                  (FUNCPTR)bpReset);

    ifp->if_mtu = bp_mtu;

    bzero ((char*) cpudesc_vaddr, sizeof (cpudesc_vaddr));
    }

/*******************************************************************************
*
* bpPoke - map in physical memory and write value to address
*
* This routine is used to map in CPU boards' mailboxes.
* An array up to MAXCPU is kept so that a mailbox is
* only mapped in once per CPU.
*/

LOCAL void bpPoke (addr, type, value, cpuNum)
    char *addr;		/* physical address */
    int type;		/* mailbox size + read/write */
    int value;		/* value to write */
    int cpuNum;		/* which CPU */

    {
    char *vaddr;		/* virtual address */
    int kmx;			/* kernel map index */
    int numClicks;
    /* int numBytes; */
    int addrOffset;

    if (cpuNum < 0 && cpuNum > MAXCPU)
	{
	printf ("bp: bpPoke invalid cpu #%d\n", cpuNum);
	return;
	}

    if ((vaddr = cpudesc_vaddr [cpuNum]) == 0)
	{
	numClicks = btoc (sizeof (int));
	/* numBytes = ctob (numClicks); */
	addrOffset = (int)(addr - ptob (btop (addr)));

	/* get some virtual address space */

	if ((kmx = rmalloc (kernelmap, numClicks)) == 0)
	    {
	    printf ("bp: no kernelmap for bp mailbox\n");
	    panic ("bpPoke");
	    }

	vaddr = (char *)kmxtob (kmx);

	mapin (&Usrptmap [kmx], btop (vaddr), btop (addr) | PGT_VME_D16,
		numClicks, PG_V | PG_KW);

	cpudesc_vaddr [cpuNum] = vaddr += addrOffset;

	if (DEBUG_OPTION_1)
	    printf ("\nbp: cpu #%d mailbox addr = 0x%x, vaddr = 0x%x\n",
		    cpuNum, (int)addr, (int)vaddr);
	}

    switch (type)
	{
	case BP_INT_MAILBOX_1: *(char  *) vaddr = value; break;
	case BP_INT_MAILBOX_2: *(short *) vaddr = value; break;
	case BP_INT_MAILBOX_4: *(long  *) vaddr = value; break;
	case BP_INT_MAILBOX_R1: value = *(char  *) vaddr; break;
	case BP_INT_MAILBOX_R2: value = *(short *) vaddr; break;
	case BP_INT_MAILBOX_R4: value = *(long  *) vaddr; break;
	default: printf ("bp: bpPoke - bad mailbox type 0x%x\n", type); break;
	}

#ifdef	MAP_OUT
    /* example of mapping out memory */

    cpudesc_vaddr [cpuNum] = 0;
    mapout (&Usrptmap [kmx], numClicks, kmx);
    rmfree (kernelmap, numClicks, kmx);
#endif	/* MAP_OUT */
    }
/*******************************************************************************
*
* bpProbe - probe for backplane memory
*
* RETURNS:
* The amount of memory available, or 0 if error.
*
* NOMANUAL
*/

int bpProbe (addr, unit)
    caddr_t addr;	/* address of backplane memory */
    int unit;		/* device number */

    {
    int memSize = bp_mem_size;
    FAST BP_SOFTC *bs = BP_SOFTC_P [unit];
    extern int peek (), pokec ();

    if (BP_MASTER)
	bpProbeType = bpParity ? pokec : peek;
    else
	bpProbeType = peek;

    if (DEBUG_OPTION_1)
	printf ("bp%d: debugging on (level = 0x%x)\n", unit, bpDebug);

    /* anchor is at the start of shared memory */

    bs->bs_pAnchor = (BP_ANCHOR *) addr;

    if ((*bpProbeType) (addr, 0) == -1)
	{
	printf ("bp%d: shared memory not present at 0x%x (0x%x)\n",
		unit, (int)bs->bs_pAnchor,(int)vir2phys((char*)bs->bs_pAnchor));
	return (0);
	}

    if (BP_MASTER &&
	bpInit ((char*)bs->bs_pAnchor, addr, memSize, bp_tas) == ERROR)
	{
	printf ("bp%d: bpProbe cannot initialize anchor!\n", unit);
	return (0);
	}

    if (DEBUG_OPTION_1)
	printf ("bp%d: shared memory size is %d bytes, at 0x%x (0x%x)\n", unit,
	     memSize,(int)bs->bs_pAnchor, (int)vir2phys((char*)bs->bs_pAnchor));

    return (memSize);
    }

#else	/* UNIX */

/*******************************************************************************
*
* bpattach - publish the `bp' network interface and initialize the driver and device
*
* This routine attaches a `bp' interface to the network, if the
* interface exists.  This routine makes the interface available by filling in
* the network interface record.  The system will initialize the interface
* when it is ready to accept packets.
*
* RETURN: OK or ERROR.
*/

STATUS bpattach
    (
    int unit,           /* backplane unit number */
    char *pAnchor,      /* bus pointer to bp anchor */
    int procNum,        /* processor number in backplane */
    int intType,        /* interrupt type: poll, bus, mailbox */
    int intArg1,        /* as per interrupt type */
    int intArg2,        /* as per interrupt type */
    int intArg3         /* as per interrupt type */
    )
    {
    FAST BP_SOFTC *bs;
    FAST struct ifnet *ifp;
    FAST BP_ANCHOR *pBpAnchor = (BP_ANCHOR *) pAnchor;

    if (procNum == 0)
	bpProbeType = bpParity ? O_WRONLY : O_RDONLY;
    else
	bpProbeType = O_RDONLY;

    /* allocate bp_softc structure */

    if ((bs = (BP_SOFTC *) malloc (sizeof (BP_SOFTC))) == NULL)
	return (ERROR);

    bp_softc [unit] = bs;	/* XXX check if non-zero for re-attach? */
    ifp = &bs->bs_if;

    /* initialize the interface structure */

    bzero ((char *) bs, sizeof (BP_SOFTC));

    bs->bs_pAnchor = pBpAnchor;
    bs->bs_procNum = procNum;
    bs->bs_intType = intType;
    bs->bs_intArg1 = intArg1;
    bs->bs_intArg2 = intArg2;
    bs->bs_intArg3 = intArg3;

    /* wait for shared memory to be initialized; master CPU does sanity check */

    if (bpAlive (bs) == ERROR)
	{
	(void) free ((void *) bs);
	return (ERROR);
	}

    if (bpDebug != 0)
	{
	printf ("bp%d: debugging on (level = 0x%x)\n", unit, bpDebug);
	printf ("bpattach: anchor CPU can%s do real TAS.\n",
		bpTasOK ? "" : "not");
	}

#ifdef BSD43_DRIVER
    ether_attach (ifp, unit, "bp", bpIfInit, bpIoctl, bpOutput,
                  (FUNCPTR)bpReset);
#else
    ether_attach (
                 ifp,
                 unit,
                 "bp",
                 (FUNCPTR) bpIfInit,
                 (FUNCPTR) bpIoctl,
                 (FUNCPTR) ether_output,
                 (FUNCPTR) bpReset
                 );
    ifp->if_start = (FUNCPTR)bpStart;
#endif

    ifp->if_mtu = bp_mtu;

    if (bpIfInit (unit) == ERROR)
        {
        (void) free ((void *) bs);
        return (ERROR);
        }

    return (OK);
    }
#endif	/* UNIX */

/*******************************************************************************
*
* bpInit - initialize the backplane anchor
*
* This routine initializes the backplane anchor.  Typically, <pAnchor> and
* <pMem> both point to the same block of shared memory.  If the first
* processor is dual-porting its memory, then, by convention, the anchor is
* at 0x600 (only 16 bytes are required) and the start of memory <pMem> is
* dynamically allocated by using the value NONE (-1).  <memSize> should be at
* least 64 Kbytes.  The <tasOK> parameter is provided for CPUs that do
* not support the test-and-set instruction.  If the system includes any
* test-and-set deficient CPUs, then all CPUs must use the software
* "test-and-set".
*
* RETURNS:
* OK, or ERROR if data structures cannot be set up or memory is insufficient.
*/

STATUS bpInit
    (
    FAST char *pAnchor, /* backplane anchor address */
    FAST char *pMem,    /* start of backplane shared memory, NONE = alloc */
    int memSize,        /* no. bytes in bp shared memory, 0 = 0x100000 */
    BOOL tasOK          /* TRUE = hardware can do test-and-set */
    )
    {
    FAST BP_ANCHOR *pBpAnchor = (BP_ANCHOR *) pAnchor;
    FAST RING *pFreeRing;
    FAST int ix;
    FAST char *pMemEnd;	/* end of backplane shared memory */
    FAST CPU_DESC *pCpu;
    FAST BP_HEADER *pHdr;
    int npackets = 0;

    memSize = (memSize == 0) ? 0x100000 : memSize;
    bpTasOK = tasOK;

    /* if bp header specified, probe it, otherwise malloc it */

    if (pMem != (char *) NONE)
	{
	memSize = min (memSize, bpSizeMem (pMem, memSize));
	if (memSize == 0)
	    {
	    printf ("bp: no shared memory at address 0x%x\n", (int)pMem);
	    return (ERROR);
	    }

	/* if anchor is at start of shared region, skip over it */

	if (pMem == pAnchor)
	    {
	    pMem    += sizeof (BP_ANCHOR);
	    memSize -= sizeof (BP_ANCHOR);
	    }
	}
    else
	{
#ifndef	UNIX
#if	((CPU_FAMILY==MIPS) || (CPU_FAMILY==SH))
	if ((pMem = (char *) cacheDmaMalloc ((unsigned) memSize)) == NULL)
#else	/* CPU_FAMILY==MIPS || CPU_FAMILY==SH */
        if ((pMem = (char *) malloc ((unsigned) memSize)) == NULL)
#endif	/* CPU_FAMILY==MIPS || CPU_FAMILY==SH */
#endif	/* UNIX */
	    {
	    printf ("bp: can't allocate shared memory\n");
	    return (ERROR);
	    }
	}

    pMemEnd = pMem + memSize;
    pHdr = (BP_HEADER *) pMem;

    /* initialize bp anchor region */

    pBpAnchor->an_ready     = 0;		/* set bp not ready */
    pBpAnchor->an_heartbeat = 0;		/* clear heartbeat */
    pBpAnchor->an_pHdr      = KtoIval (pHdr, BP_HEADER *, pAnchor);

    if (pMem + sizeof (BP_HEADER) > pMemEnd)
	{
	printf ("bp: bpInit out of memory!\n");
	return (ERROR);
	}

    /* initialize bp header region */

    pHdr->hdr_ready = 0;                /* set CPU region not ready */
    pHdr->hdr_ncpu = BP_HTONS(NCPU);    /* set number of CPUs */

    bcopy ((char *) bpEnetAdrs, (char *) pHdr->hdr_enetAdrs, 6);

    /* initialize each CPU descriptor */

    for (ix = 0; ix < MAXCPU; ++ix)
	{
	pCpu = &pHdr->hdr_cpuDesc[ix];

	pCpu->cd_active      = FALSE;
	pCpu->cd_cpuNum      = BP_HTONL(ix);
	pCpu->cd_pInputRing  = NULL;
	}

    /* initialize free ring */

    pMem += sizeof (BP_HEADER);

    if ((pMem + sizeof (struct ring256) + NCPU * sizeof (struct ring32))
								> pMemEnd)
	{
	printf ("bp: bpInit out of memory!\n");
	return (ERROR);
	}

    ringinit ((pFreeRing = (RING *) pMem), 256);
    pHdr->hdr_pFreeRing =  KtoIval (pFreeRing, RING *, pAnchor);
    pMem += sizeof (struct ring256);

    /* initialize each input ring */

    for (ix = 0; ix < NCPU; ++ix)
	{
	ringinit ((RING *) pMem, 32);
	pHdr->hdr_cpuDesc[ix].cd_pInputRing = KtoIval (pMem, RING *, pAnchor);
	pMem += sizeof (struct ring32);
	}

    /* initialize as many buffer blocks as will fit in remaining shared memory*/

    if ((pMem + sizeof (BUF_NODE) + BP_BUFSIZ) > pMemEnd)
	{
	printf ("bp: bpInit out of memory!\n");
	return (ERROR);
	}

    while ((pMem + sizeof (BUF_NODE) + BP_BUFSIZ) <= pMemEnd)
	{
	((BUF_NODE *)pMem)->b_addr = KtoIval (pMem + sizeof (BUF_NODE),
					      char*, pAnchor);
        ((BUF_NODE *)pMem)->b_len = BP_BUFSIZ;

	(void) ringput (pFreeRing, KtoIval (pMem, int, pAnchor));
	pMem += sizeof (BUF_NODE) + BP_BUFSIZ;
	npackets++;
	}

    if (DEBUG_OPTION_1)
	{
	printf ("bp: number of %d byte packets is %d\n", BP_BUFSIZ, npackets);
	printf ("bp: %d bytes unused from 0x%x to 0x%x\n",
		(int) (pMemEnd - pMem), 
		(int) vir2phys (pMem), 
		(int) vir2phys (pMemEnd));
	}

    /* mark bp as ready */

    if (bpTasOK)
	pHdr->hdr_ready = pBpAnchor->an_ready = BP_HTONL(BP_READY);
    else
	pHdr->hdr_ready = pBpAnchor->an_ready = BP_HTONL(BP_READY_SOFT_TAS);

    /* start heartbeat */

    bpHeartbeatRate = hz;
#ifdef	UNIX
    timeout (bpHeartbeat, (caddr_t) pAnchor, bpHeartbeatRate);
#else	/* UNIX */
    /* create watchdog timer for heartbeat */

    if ((pBpAnchor->an_wdog = wdCreate ()) == NULL)
	return (ERROR);
    wdStart (pBpAnchor->an_wdog, bpHeartbeatRate, (FUNCPTR) bpHeartbeat,
             (int) pAnchor);
#endif	/* UNIX */

    return (OK);
    }
/*******************************************************************************
*
* bpReset - mark a bp inactive
*
* called from reboot and panic
*/

LOCAL void bpReset
    (
    FAST int unit
    )
    {
    FAST BP_SOFTC *bs;
    FAST BUF_NODE *pBufNode;
#ifdef	UNIX
    FAST struct mb_device *md;

    if (unit >= NBP || (md = bpinfo[unit]) == 0 || md->md_alive == 0)
	return;
#endif	/* UNIX */

    bs = BP_SOFTC_P [unit];

    if (bs != NULL && bs->bs_pHdr != NULL)
	{
	/* turn off active flag in our CPU descriptor in the
	 * shared header, then throw away anything on our input ring */

	bs->bs_pHdr->hdr_cpuDesc[bs->bs_procNum].cd_active = FALSE;

	while ( (pBufNode = (BUF_NODE *)ringget ( (RING *)bs->bs_pInputRing)))
	    (void)ringput ( (RING *)bs->bs_pFreeRing, (int)pBufNode);
	}
    }
/*******************************************************************************
*
* bpAlive - check for throbbing heartbeat
*
* This routine is called for CPU 0 (master) as well as the rest.
* The timeout is quick if the master can't see its own ticker.
*
* RETURNS: OK or ERROR if unable to find anchor after 10 minutes
*/

LOCAL STATUS bpAlive
    (
    BP_SOFTC *bs
    )
    {
    int readyValue;
    int heartbeat;
    FAST unsigned int oldBeat = 0;
    FAST int countdown             = 10 * 60 * hz;	/* wait 10 minutes */
    FAST BP_ANCHOR *pAnchor        = bs->bs_pAnchor;
#ifdef	UNIX
    int level = spl3 ();	/* find out priority level */
    splx (level);
#endif	/* UNIX */

    if (bs->bs_procNum == 0)
	countdown = 2 * hz;	/* don't wait around for self if master CPU */

    while (--countdown > 0)
	{
	/* probe test:
	 *  1) anchor must be readable (memory must be visable to us),
	 *  2) anchor ready value must be BP_READY or BP_READY_SOFT_TAS,
	 *  3) anchor heartbeat must be increasing.
	 */

#ifdef	UNIX
	if (peek ((short *) &pAnchor->an_ready)     != -1 &&
	    peek ((short *) &pAnchor->an_heartbeat) != -1)
	    {
	    readyValue = BP_NTOHL(pAnchor->an_ready);
	    heartbeat  = (UINT)BP_NTOHL(pAnchor->an_heartbeat);
#else	/* UNIX */
	if (vxMemProbe ((char *) &pAnchor->an_ready,
			 O_RDONLY, 4, (char *) &readyValue) == OK &&
	    vxMemProbe ((char *) &pAnchor->an_heartbeat,
			 O_RDONLY, 4, (char *) &heartbeat) == OK)
	    {
	    readyValue = BP_NTOHL(readyValue);
	    heartbeat = (UINT)BP_NTOHL(heartbeat);
#endif	/* UNIX */

	    if ((countdown % 10) == 6)
		{
		printf ("\nbp: bpAlive ready = 0x%x, heartbeat = %d\n",
			    readyValue, heartbeat);
		}

	    if (readyValue == BP_READY || readyValue == BP_READY_SOFT_TAS)
		{
		if (oldBeat == 0 || heartbeat <= oldBeat)
		    oldBeat = heartbeat;
		else
		    {
		    /* heartbeat increased, passed test */
		    bpTasOK = readyValue == BP_READY;
		    return (OK);
		    }
		}
	    }
	else if ((countdown % 10) == 8)
	    printf ("\nbp: bpAlive bus error\n");

#ifdef	UNIX
	timeout (wakeup, (caddr_t)bs, 1);
        sleep (bs, level);
#else	/* UNIX */
	taskDelay (hz);
#endif	/* UNIX */
	}

#ifndef	UNIX
    (void) errnoSet (S_iosLib_CONTROLLER_NOT_PRESENT);
#endif	/* UNIX */

    /* give up - didn't get initialized */

    return (ERROR);
    }
/*******************************************************************************
*
* bpIfInit - initialize interface
*
* RETURNS: OK or ERROR
*/

LOCAL STATUS bpIfInit
    (
    int unit
    )
    {
    FAST CPU_DESC *pCpu;
    FAST BP_SOFTC *bs = BP_SOFTC_P [unit];
    FAST BP_HEADER *pHdr;

    /* find the anchor and CPU descriptor */
    pHdr = ItoKval (bs->bs_pAnchor->an_pHdr, BP_HEADER *, bs->bs_pAnchor);
    bs->bs_pHdr = pHdr;

#ifdef	UNIX
    /* wait for shared memory to be initialized */

    if (!BP_MASTER && bpAlive (bs) == ERROR)
	return (ERROR);
#endif	/* UNIX */

    pCpu = &pHdr->hdr_cpuDesc [bs->bs_procNum];

    bs->bs_pFreeRing = ItoKval (pHdr->hdr_pFreeRing, RING *, bs->bs_pAnchor);

    bs->bs_pInputRing= ItoKval(pHdr->hdr_cpuDesc[bs->bs_procNum].cd_pInputRing,
			       RING *, bs->bs_pAnchor);

    /* fill in Ethernet address, which is the backplane Ethernet address with
     * our processor number as the last byte. */

#ifdef	UNIX
    bcopy ((char *) pHdr->hdr_enetAdrs,
	    (char *) bs->bs_enaddr.ether_addr_octet, 6);
    bs->bs_enaddr.ether_addr_octet [4] = unit;
    bs->bs_enaddr.ether_addr_octet [5] = (char) bs->bs_procNum;
#else	/* UNIX */
    bcopy ((char *) pHdr->hdr_enetAdrs, (char *) bs->bs_enaddr, 6);
    bs->bs_enaddr [4] = unit;
    bs->bs_enaddr [5] = (char) bs->bs_procNum;
#endif	/* UNIX */

    /* check that our input buffer was established
     * (in case NCPU < our cpu < MAXCPU) */

    if (pCpu->cd_pInputRing == NULL)
	return (ERROR);

    /* fill in our info in the CPU descriptor in shared memory */

    pCpu->cd_unit = unit;	/* set our internal unit num */

    /* tell others how to interrupt us */

    pCpu->cd_intType = BP_HTONL(bs->bs_intType);
    pCpu->cd_intArg1 = BP_HTONL(bs->bs_intArg1);
    pCpu->cd_intArg2 = BP_HTONL(bs->bs_intArg2);
    pCpu->cd_intArg3 = BP_HTONL(bs->bs_intArg3);

    /* connect and enable our interprocessor interrupt */

    if (DEBUG_OPTION_1)
	printf ("bp%d: connecting...\n", unit);

    if (bpConnect (bs, unit) != OK || bpIntEnable (bs) != OK)
	return (ERROR);

    /* tell others we're here now */

    pCpu->cd_active = TRUE;

    if ((bs->bs_if.if_flags & IFF_RUNNING) == 0)
	bs->bs_if.if_flags |= IFF_UP | IFF_RUNNING;

    return (OK);
    }
/*******************************************************************************
*
* bpintr - interrupt handler
*
* RETURNS: N/A
*
* NOMANUAL
*/

void bpintr
    (
    int unit
    )
    {
    FAST BP_SOFTC *bs = BP_SOFTC_P [unit];

    if (bs != NULL && bs->bs_pInputRing != NULL)	/* input ring exists? */
	{
#ifndef	UNIX
	bpIntAck (bs);
#endif	/* UNIX */

	if (!bs->bs_readPending)
	    {
	    bs->bs_readPending = TRUE;
#ifdef	UNIX
	    bpReadAll (unit);
#else	/* UNIX */
	    (void) netJobAdd ((FUNCPTR)bpReadAll, unit, 0,0,0,0); /* read from input ring */
#endif	/* UNIX */
	    }
	}
    }

/*******************************************************************************
*
* bpReadAll -
*/

LOCAL void bpReadAll
    (
    int unit
    )
    {
    FAST BP_SOFTC *bs = BP_SOFTC_P [unit];
    FAST BUF_NODE *pBufNode;

    bs->bs_readPending = FALSE;

    while ((pBufNode = (BUF_NODE *) ringget ((RING *) bs->bs_pInputRing)) != 0)
	{
#ifdef UNIX
	bpRead (bs, ItoKval (pBufNode, BUF_NODE *, bs->bs_pAnchor));
	(void) ringput (bs->bs_pFreeRing, (int) pBufNode);
#else	/* UNIX */
	if (bpRead (bs, ItoKval (pBufNode, BUF_NODE *, bs->bs_pAnchor),
		    pBufNode) == OK)
	    (void) ringput ((RING *) bs->bs_pFreeRing, (int) pBufNode);
#endif	/* UNIX */
	}
    }

/*******************************************************************************
*
* bpRead -
*
* NOMANUAL
*/

#ifdef UNIX
LOCAL void bpRead (bs, pBufNode)
    BP_SOFTC *bs;
    BUF_NODE *pBufNode;
#else	/* UNIX */
LOCAL STATUS bpRead
    (
    BP_SOFTC *bs,
    BUF_NODE *pBufNode,
    BUF_NODE *pBufNodeOrig
    )
#endif  /* UNIX */
    {
    FAST struct ether_header	*eh;
    FAST struct mbuf 		*m;
    int 			len;
    int 			off;
    FAST unsigned char 		*pData;
    STATUS 			retval;
#ifdef BSD43_DRIVER
    u_short			ether_type;
#endif

    retval = OK;

    bs->bs_if.if_ipackets++;	/* count input packets */

    /* get length and pointer to packet */

    len = (int) (BP_NTOHS (pBufNode->b_len));
    eh = ItoKval (pBufNode->b_addr, struct ether_header *, bs->bs_pAnchor);

#ifndef	UNIX
    /* call input hook if any */

    if ((etherInputHookRtn != NULL) &&
	(* etherInputHookRtn) (&bs->bs_if, (char *)eh, BP_NTOHS(pBufNode->b_len)))
	return (retval);	/* input hook has already processed packet */
#endif	/* UNIX */

    len -= SIZEOF_ETHERHEADER;
    pData = ((unsigned char *) eh) + SIZEOF_ETHERHEADER;

#ifdef	UNIX
    check_trailer (eh, pData, &len, &off);

    if (len == 0)
	return;

    m = copy_to_mbufs (pData, len, off);

    if (m != NULL)
	do_protocol (eh, m, &bs->bs_ac, len);
    else
        bs->bs_if.if_ierrors++;     /* bump error count */
#else	/* UNIX */
#ifdef BSD43_DRIVER
    check_trailer (eh, pData, &len, &off, &bs->bs_if);

    if (len == 0)
	return (retval);

    ether_type	= eh->ether_type;
#endif

    m = copy_to_mbufs (pData, len, off, &bs->bs_if);

    if (m != NULL)
#ifdef BSD43_DRIVER
        do_protocol_with_type (ether_type, m, &bs->bs_ac, len);
#else
        do_protocol (eh, m, &bs->bs_ac, len);
#endif
    else
        bs->bs_if.if_ierrors++;     /* bump error count */
         
    return (retval);
#endif	/* UNIX */
    }

/*******************************************************************************
*
* bpStart -
*
* NOMANUAL
*/

#ifdef	UNIX
LOCAL void bpStart (dev)
    dev_t dev;
#else	/* UNIX */
#ifdef BSD43_DRIVER
LOCAL void bpStart
    (
    int unit
    )
#else
LOCAL void bpStart
    (
    BP_SOFTC * 	bs
    )
#endif    /* BSD43_DRIVER */
#endif  /* UNIX */
    {
#ifdef	UNIX
    int unit = minor (dev);
#endif	/* UNIX */

#if defined (BSD43_DRIVER) || defined (UNIX)
    FAST BP_SOFTC *bs = BP_SOFTC_P [unit];
#endif

    FAST struct mbuf *m;

    while (bs->bs_if.if_snd.ifq_head)               /* while queue not empty */
        {
        IF_DEQUEUE(&bs->bs_if.if_snd, m);           /* get next from queue */
        if (bpPut (bs, m) == ERROR)                 /* if couldn't send */
            {
#if defined (BSD43_DRIVER) || defined (UNIX)    
             /* vxWorks BSD 4.4 ether_output() doesn't bump statistic. */
            bs->bs_if.if_opackets--;                /* it wasn't sent! */
#endif
            bs->bs_if.if_oerrors++;                 /* it was an error */
            m_freem (m);                            /* free the mbufs */
            }
#if !defined (BSD43_DRIVER) && !defined (UNIX)
        else
            /* bump statistic for BSD 4.4 vxWorks driver. */

            bs->bs_if.if_opackets++;   
#endif
        }
    }

#if defined (BSD43_DRIVER) || defined (UNIX)
/*******************************************************************************
*
* bpOutput - net output routine
*
* Encapsulate a packet of type family for the local net.
* Use trailer local net encapsulation if enough data in first
* packet leaves a multiple of 512 bytes of data in remainder.
*
* RETURNS: 0 if successful, errno otherwise (as per network requirements)
*/

LOCAL int bpOutput
    (
    struct ifnet *ifp,
    struct mbuf *m0,
    struct sockaddr *dst
    )
    {
#ifdef	UNIX
    return (ether_output (ifp, m0, dst, bpStart,
	    &(BP_SOFTC_P [ifp->if_unit])->bs_ac));
#else	/* UNIX */
    return (ether_output (ifp, m0, dst, (FUNCPTR)bpStart,
	    &BP_SOFTC_P [ifp->if_unit]->bs_ac));
#endif	/* UNIX */
    }
#endif

/*******************************************************************************
*
* bpPut - copy from mbuf chain to shared memory buffer
*
* RETURNS: OK or ERROR if no room left in free ring
*/

LOCAL STATUS bpPut
    (
    FAST BP_SOFTC       *bs,
    FAST struct mbuf    *m
    )
    {
    FAST BUF_NODE 	*pBufNode;
    u_char 		*bufStart;
    int			len;

    if ((pBufNode = (BUF_NODE *) ringget ((RING *) bs->bs_pFreeRing)) == NULL)
	return (ERROR);

    pBufNode = ItoKval (pBufNode, BUF_NODE *, bs->bs_pAnchor);
    bufStart = ItoKval (pBufNode->b_addr, u_char *, bs->bs_pAnchor);

#ifdef	UNIX
    len = copy_from_mbufs (bufStart, m, len);
#else	/* UNIX */
    copy_from_mbufs (bufStart, m, len);
#endif	/* UNIX */

    len = max (MINPKTSIZE, len);
    pBufNode->b_len = BP_HTONS (len);

#ifndef	UNIX
    /* call output hook if any */

    if ((etherOutputHookRtn != NULL) &&
	(* etherOutputHookRtn) (&bs->bs_if, bufStart, pBufNode->b_len))
	return (OK);	/* output hook has already processed packet */
#endif	/* UNIX */

    /* send to specified processor:
     * proc number is in last byte of Ethernet address
     */

    bpXmit (bs, pBufNode, (int) ((struct ether_header *)
#ifdef	UNIX
	       (bufStart))->ether_dhost.ether_addr_octet[5], bs->bs_procNum);
#else	/* UNIX */
	       (bufStart))->ether_dhost[5], bs->bs_procNum);
#endif	/* UNIX */

    return (OK);
    }

/*******************************************************************************
*
* bpXmit - transmit buffer
*/

LOCAL void bpXmit
    (
    FAST BP_SOFTC *bs,
    BUF_NODE *pBufNode,
    int destProcNum,    /* destination CPU */
    int srcProcNum      /* originator */
    )
    {
    BP_ANCHOR *pAnchor = bs->bs_pAnchor;
    u_char *buf;
    FAST CPU_DESC *pCpu;
    FAST BUF_NODE *pBufNode0;

    if (destProcNum < 0 || destProcNum >= MAXCPU)
	{
	/* broadcast to all CPUs on network */

	buf = ItoKval (pBufNode->b_addr, u_char *, pAnchor);

	for (destProcNum = MAXCPU - 1; destProcNum >= 0; --destProcNum)
	    {
	    pCpu = &bs->bs_pHdr->hdr_cpuDesc[destProcNum];

	    if (DEBUG_OPTION_3)
		dbgprintf ("bp: broadcast packet from %d\n", srcProcNum, 0, 0, 0, 0, 0);

	    if (!pCpu->cd_active)
		continue;

	    if ((pBufNode0 = (BUF_NODE *) ringget ((RING *) bs->bs_pFreeRing))
            == NULL)
		break;

	    pBufNode0 = ItoKval (pBufNode0, BUF_NODE *, pAnchor);

            pBufNode0->b_len = pBufNode->b_len;
	    bcopy ((caddr_t) buf,
		   (caddr_t) ItoKval (pBufNode0->b_addr, u_char *, pAnchor),
                   BP_NTOHS (pBufNode->b_len));

	    if (ringput (ItoKval (pCpu->cd_pInputRing, RING *, pAnchor),
			     KtoIval (pBufNode0, int, pAnchor)) == 0)
		{
		/* if input ring was full, then put bufNode back on free ring */

		if (DEBUG_OPTION_2)
		    dbgprintf ("bp: dropped packet\n", 0,0,0,0,0,0);
		(void)ringput ((RING *) bs->bs_pFreeRing,
			       KtoIval (pBufNode0, int, pAnchor));
		}
	    else
		{
		/* if input ring was empty, then interrupt destination CPU */

		bpIntGen (pCpu, srcProcNum);
		}
	    }

	(void) ringput ((RING *) bs->bs_pFreeRing,
                    KtoIval (pBufNode, int, pAnchor));
	}
    else
	{
	pCpu = &bs->bs_pHdr->hdr_cpuDesc [destProcNum];

	if (DEBUG_OPTION_3)
	    dbgprintf ("bp: packet from %d for %d\n", srcProcNum, destProcNum, 0, 0, 0, 0);

	if (!pCpu->cd_active)
	    (void) ringput ((RING *) bs->bs_pFreeRing,
                        KtoIval (pBufNode, int, pAnchor));
	else if (ringput (ItoKval (pCpu->cd_pInputRing, RING *, pAnchor),
	    		     KtoIval (pBufNode, int, pAnchor)) == 0)
	    {
	    /* input ring was full, then put bufNode back on free ring */

	    if (DEBUG_OPTION_2)
		dbgprintf ("bp: dropped packet\n", 0,0,0,0,0,0);
	    (void) ringput ((RING *) bs->bs_pFreeRing,
			    KtoIval (pBufNode, int, pAnchor));
	    }
	else
	    {
	    /* input ring had room, then interrupt destination CPU */

	    bpIntGen (pCpu, srcProcNum);
	    }
	}
    }

#ifdef	UNIX
/*******************************************************************************
*
* bpPollUnix - UNIX backplane poll
*
* This routine is called by UNIX's timeout routine to poll the
* backplane input rings for this CPU.
*/

LOCAL void bpPollUnix ()

    {
    FAST int unit;

    for (unit = 0; unit < NBP; unit++)
	{
	/* if header structure non-zero then unit is initialized */

	if (bp_softc[unit].bs_pHdr)
	    bpintr (unit);
	}

    /* loop forever, with a delay of 1 */

    timeout (bpPollUnix, (caddr_t)0, 1);
    }
#else	/* UNIX */
/*******************************************************************************
*
* bpPollTask - backplane poll task
*
* This routine is spawned as task to poll the backplane input rings for
* this CPU.  It is only used if no backplane interrupt mechanism is
* used.
*
* NOMANUAL
*/

void bpPollTask (void)
    {
    FAST int unit;
    FAST BP_SOFTC *bs;

    FOREVER
	{
	for (unit = 0; unit < NBP; unit++)
	    {
	    bs = BP_SOFTC_P [unit];
	    if (bs != NULL && !bs->bs_readPending)
		{
		bs->bs_readPending = TRUE;
		if (bs->bs_pInputRing != NULL)
		    (void) netJobAdd ((FUNCPTR)bpReadAll, unit,0,0,0,0);
		}
	    }
	}
    }
/*******************************************************************************
*
* bpIntAck - acknowledge our backplane interrupt
*/

LOCAL void bpIntAck
    (
    FAST BP_SOFTC *bs
    )
    {
    switch (bs->bs_intType)
	{
	case BP_INT_NONE:

	case BP_INT_MAILBOX_1:
	case BP_INT_MAILBOX_2:
	case BP_INT_MAILBOX_4:
	case BP_INT_MAILBOX_R1:
	case BP_INT_MAILBOX_R2:
	case BP_INT_MAILBOX_R4:
	    break;

	/* bus interrupt:
	 *   arg1 = level
	 *   arg2 = vector
	 */

	case BP_INT_BUS:
	    sysBusIntAck (bs->bs_intArg1);
	    break;
	}
    }
#endif	/* UNIX */
/*******************************************************************************
*
* bpConnect - connect to our backplane using the requested interrupt type
*
* RETURNS: OK or ERROR if interrupt type not supported
*/

LOCAL STATUS bpConnect
    (
    FAST BP_SOFTC *bs,
    FAST int unit
    )
    {
    STATUS status;

    switch (bs->bs_intType)
	{
	case BP_INT_NONE:
	/* no interprocessor interrupt - spawn background polling task */

#ifdef	UNIX
	    {
	    static BOOL bpIsPolling = FALSE;

	    if (!bpIsPolling)
		{
		bpIsPolling = TRUE;
		timeout (bpPollUnix, (caddr_t)0, 1);
		}
	    status = OK;
	    }
#else	/* UNIX */
	    bpPollTaskId = taskSpawn ("bpPollTask", bpPollTaskPriority,
				      bpPollTaskOptions, bpPollTaskStackSize,
				      (FUNCPTR) bpPollTask, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);

	    status = bpPollTaskId == ERROR ? ERROR : OK;
#endif	/* UNIX */

	    break;

	case BP_INT_MAILBOX_1:
	case BP_INT_MAILBOX_2:
	case BP_INT_MAILBOX_4:
	case BP_INT_MAILBOX_R1:
	case BP_INT_MAILBOX_R2:
	case BP_INT_MAILBOX_R4:
	/* interrupt by mailbox (write to bus address):
	 *   arg1 = bus address space
	 *   arg2 = address
	 *   arg3 = value
	 */

#ifdef	UNIX
	    printf ("bp%d: bpConnect interrupt type not implemented\n", unit);
	    status = ERROR;
#else	/* UNIX */
	    status = sysMailboxConnect ((FUNCPTR)bpintr, unit);
#endif	/* UNIX */
	    break;

	case BP_INT_BUS:
	/* bus interrupt:
	 *   arg1 = level
	 *   arg2 = vector
	 */
#ifdef	UNIX
	    status = OK;  /* nothing to do, done in kernel configuration */
#else	/* UNIX */
	    status = intConnect ((VOIDFUNCPTR *)INUM_TO_IVEC (bs->bs_intArg2), bpintr, unit);
#endif	/* UNIX */
	    break;

	default:
	    printf ("bp%d: bpConnect bad interrupt type: %d\n",
		    unit, bs->bs_intType);
	    status = ERROR;
	    break;
	}

    return (status);
    }
/*******************************************************************************
*
* bpIntEnable - enable our backplane interrupt
*
* RETURNS: OK or ERROR
*/

LOCAL STATUS bpIntEnable
    (
    FAST BP_SOFTC *bs
    )
    {
    STATUS status;
    int unit = bs->bs_if.if_unit;

    switch (bs->bs_intType)
	{
	case BP_INT_NONE:
	    status = OK;	/* no interrupt (polling) */
	    break;

	case BP_INT_MAILBOX_1:
	case BP_INT_MAILBOX_2:
	case BP_INT_MAILBOX_4:
	case BP_INT_MAILBOX_R1:
	case BP_INT_MAILBOX_R2:
	case BP_INT_MAILBOX_R4:
	/* interrupt by mailbox (write to bus address):
	 *   arg1 = bus address space
	 *   arg2 = address
	 *   arg3 = value
	 */

#ifdef	UNIX
	    status = ERROR;
	    printf ("bp%d: bpIntEnable interrupt type 0x%x not implemented\n",
		    unit, bs->bs_intType);
#else	/* UNIX */
	    status = sysMailboxEnable ((char *) bs->bs_intArg2);

	    if (status == ERROR)
		printf ("bp%d: error enabling mailbox interrupt addr 0x%x.\n",
			  unit, bs->bs_intArg2);
#endif	/* UNIX */
	    break;

	case BP_INT_BUS:
	/* bus interrupt:
	 *   arg1 = level
	 *   arg2 = vector
	 */

#ifdef	UNIX
	    status = OK;  /* nothing to do */
#else	/* UNIX */
	    status = sysIntEnable (bs->bs_intArg1);
	    if (status == ERROR)
		printf ("bp%d: error enabling bus interrupt level %d.\n",
			  unit, bs->bs_intArg1);
#endif	/* UNIX */
	    break;

	default:
	    printf ("bp%d: bpIntEnable bad interrupt type: %d\n",
		    unit, bs->bs_intType);
	    status = ERROR;
	    break;
	}

    return (status);
    }
/*******************************************************************************
*
* bpIntGen - interrupt another CPU
*/

LOCAL void bpIntGen
    (
    FAST CPU_DESC *pCpu,        /* ptr to descriptor of CPU to interrupt */
    int srcProcNum              /* our procNum on this network */
    )
    {
    char *pMailbox;
    int intType;

    /* if we're trying to interrupt ourself (a bad idea on some CPUs (is20))
     * simply invoke the interrupt handler */

    if (BP_NTOHL(pCpu->cd_cpuNum) == srcProcNum)
	{
	/* read input ring */

#ifdef	UNIX
	bpReadAll (pCpu->cd_unit);
#else	/* UNIX */
	(void) netJobAdd ((FUNCPTR)bpReadAll, pCpu->cd_unit, 0,0,0,0);
#endif	/* UNIX */
	return;
	}

    switch (intType = BP_NTOHL(pCpu->cd_intType))
	{
	case BP_INT_NONE:
	    /* no interrupt (CPU is polling) */
	    break;

	case BP_INT_MAILBOX_1:
	case BP_INT_MAILBOX_R1:
	/* interrupt by mailbox (read/write to bus address):
	 *   arg1 = bus address space
	 *   arg2 = address
	 *   arg3 = value
	 */

	    if (sysBusToLocalAdrs (BP_NTOHL(pCpu->cd_intArg1),
				   (char *) BP_NTOHL(pCpu->cd_intArg2),
				   &pMailbox) == OK)
		{
#ifdef	UNIX
		bpPoke (pMailbox, intType, BP_NTOHL(pCpu->cd_intArg3),
			BP_NTOHL(pCpu->cd_cpuNum));
#else	/* UNIX */
		if (intType == BP_INT_MAILBOX_R1)
		    bpBitBucket = * (char *)pMailbox;
		else
		    *pMailbox = BP_NTOHL(pCpu->cd_intArg3);
#endif	/* UNIX */
		}
	    else
		{
		logMsg ("bp%d: bpIntGen bad mailbox interrupt, addr space = 0x%x, addr = 0x%x\n",
		     pCpu->cd_unit, BP_NTOHL(pCpu->cd_intArg1),
		     BP_NTOHL(pCpu->cd_intArg2), 0,0,0);
		}
	    break;

	case BP_INT_MAILBOX_2:
	case BP_INT_MAILBOX_R2:
	    if (sysBusToLocalAdrs (BP_NTOHL(pCpu->cd_intArg1),
				   (char *) BP_NTOHL(pCpu->cd_intArg2),
				   &pMailbox) == OK)
		{
#ifdef	UNIX
		bpPoke (pMailbox, intType, BP_NTOHL(pCpu->cd_intArg3),
			BP_NTOHL(pCpu->cd_cpuNum));
#else	/* UNIX */
		if (intType == BP_INT_MAILBOX_R2)
		    bpBitBucket = * (short *)pMailbox;
		else
		    * (short *) pMailbox = BP_NTOHL(pCpu->cd_intArg3);
#endif	/* UNIX */
		}
	    else
		{
		logMsg (
    "bp%d: bpIntGen bad mailbox interrupt, addr space = 0x%x, addr = 0x%x\n",
		     pCpu->cd_unit, BP_NTOHL(pCpu->cd_intArg1),
		     BP_NTOHL(pCpu->cd_intArg2), 0, 0, 0);
		}
	    break;

	case BP_INT_MAILBOX_4:
	case BP_INT_MAILBOX_R4:
	    if (sysBusToLocalAdrs (BP_NTOHL(pCpu->cd_intArg1),
				   (char *) BP_NTOHL(pCpu->cd_intArg2),
				   &pMailbox) == OK)
		{
#ifdef	UNIX
		bpPoke (pMailbox, intType, BP_NTOHL(pCpu->cd_intArg3),
			BP_NTOHL(pCpu->cd_cpuNum));
#else	/* UNIX */
		if (intType == BP_INT_MAILBOX_R4)
		    bpBitBucket = * (long *)pMailbox;
		else
		    * (long *) pMailbox = BP_NTOHL(pCpu->cd_intArg3);
#endif	/* UNIX */
		}
	    else
		{
		logMsg (
    "bp%d: bpIntGen bad mailbox interrupt, addr space = 0x%x, addr = 0x%x\n",
		     pCpu->cd_unit, BP_NTOHL(pCpu->cd_intArg1),
		     BP_NTOHL(pCpu->cd_intArg2), 0, 0, 0);
		}
	    break;


	case BP_INT_BUS:
	/* bus interrupt:
	 *   arg1 = level
	 *   arg2 = vector
	 */

#ifndef	UNIX
	    if (sysBusIntGen (BP_NTOHL(pCpu->cd_intArg1),
			BP_NTOHL(pCpu->cd_intArg2)) == ERROR)
		{
		logMsg (
		"bp%d: bpIntGen interrupt failed (level=%d vector=0x%x)\n",
		     pCpu->cd_unit, BP_NTOHL(pCpu->cd_intArg1),
		     BP_NTOHL(pCpu->cd_intArg2), 0, 0, 0);
		}
	    break;
#endif	/* UNIX */
	/* UNIX drops through */

	default:
	    logMsg ("bp%d: bpIntGen bad interrupt type: proc = %d, type = %d\n",
		     pCpu->cd_unit, BP_NTOHL(pCpu->cd_cpuNum),
		     intType, 0, 0, 0);
	    break;
	}
    }
#ifdef	UNIX
/********************************************************************************
* sysBusToLocalAdrs - convert bus address to local address
*
* Given a VME memory address, this routine returns the local address
* that would have to be accessed to get to that byte.
*
* RETURNS: OK, or ERROR if the address space is unknown.
*
* SEE ALSO: sysLocalToBusAdrs()
*
* NOMANUAL
*/

STATUS sysBusToLocalAdrs (adrsSpace, busAdrs, pLocalAdrs)
    int adrsSpace;      /* bus address space in which busAdrs resides,     */
                        /* use address modifier codes as defined in vme.h, */
                        /* such as VME_AM_STD_SUP_DATA                     */
    char *busAdrs;      /* bus address to convert                          */
    char **pLocalAdrs;  /* where to return local address                   */

    {
    switch (adrsSpace)
        {
        case VME_AM_SUP_SHORT_IO:
        case VME_AM_USR_SHORT_IO:
            *pLocalAdrs = (char *) (0xffff0000 | (int) busAdrs);
            return (OK);

        case VME_AM_STD_SUP_ASCENDING:
        case VME_AM_STD_SUP_PGM:
        case VME_AM_STD_SUP_DATA:
        case VME_AM_STD_USR_ASCENDING:
        case VME_AM_STD_USR_PGM:
        case VME_AM_STD_USR_DATA:
            *pLocalAdrs = (char *) (0xff000000 | (int) busAdrs);
            return (OK);

        case VME_AM_EXT_SUP_ASCENDING:
        case VME_AM_EXT_SUP_PGM:
        case VME_AM_EXT_SUP_DATA:
        case VME_AM_EXT_USR_ASCENDING:
        case VME_AM_EXT_USR_PGM:
        case VME_AM_EXT_USR_DATA:
            *pLocalAdrs = busAdrs; /* XXX check if on board memory? */
            return (OK);

        default:
            return (ERROR);
        }
    }
#endif	/* UNIX */
/*******************************************************************************
*
* bpHeartbeat -
*/

LOCAL void bpHeartbeat
    (
    FAST BP_ANCHOR *pAnchor
    )
    {
    pAnchor->an_heartbeat =
		(UINT)BP_HTONL((UINT)BP_NTOHL(pAnchor->an_heartbeat) + 1);

#ifdef	UNIX
    timeout (bpHeartbeat, (caddr_t) pAnchor, bpHeartbeatRate);
#else	/* UNIX */
    wdStart (pAnchor->an_wdog, bpHeartbeatRate, (FUNCPTR) bpHeartbeat,
             (int) pAnchor);
#endif	/* UNIX */
    }
/*******************************************************************************
*
* bpSizeMem - size shared memory pool
*
* RETURNS: amount of available memory
*/

LOCAL int bpSizeMem
    (
    caddr_t memAdrs,    /* address of start of pool to start probing */
    int memSize         /* max size of memory pool */
    )
    {
    FAST int size;

    for (size = 0; size < memSize; size += 4096)
	{
#ifdef	UNIX
        if ((*bpProbeType) ((char *)(memAdrs + size), 0) == -1)
#else	/* UNIX */
	char bitBucket;

	if (vxMemProbe (memAdrs + size, bpProbeType, 1, &bitBucket) != OK)
#endif	/* UNIX */
	    break;
	}

    if (bpParity)
	bzero (memAdrs, memSize);

    /* return the amount of memory found */

    return (size);
    }
/*******************************************************************************
*
* bpIoctl - process an ioctl request
*
* RETURNS: 0 if successful, errno otherwise (as per network requirements)
*/

LOCAL int bpIoctl
    (
    FAST struct ifnet *pIf,
    int cmd,
    caddr_t data
    )
    {
    int error = 0;
    int unit  = pIf->if_unit;
    FAST BP_SOFTC *bs = BP_SOFTC_P [unit];
    int level = splimp ();

    switch (cmd)
	{
	case SIOCSIFADDR:
#ifdef	UNIX
            error = set_if_addr (pIf, data, &bs->bs_enaddr);
#else	/* UNIX */
            ((struct arpcom *)pIf)->ac_ipaddr = IA_SIN (data)->sin_addr;
            arpwhohas (pIf, &IA_SIN (data)->sin_addr);
#endif	/* UNIX */
	    break;

	case SIOCGIFADDR:
#ifdef	UNIX
	    bcopy ((caddr_t) &bs->bs_enaddr,
		  (caddr_t) &((struct ifreq *)data)->ifr_addr.sa_data[0],
		  sizeof (struct ether_addr));
#else	/* UNIX */
	    bcopy ((caddr_t) bs->bs_enaddr,
		  (caddr_t) &((struct ifreq *)data)->ifr_addr.sa_data[0],
		  6);
#endif	/* UNIX */
	    break;

        case SIOCGIFFLAGS:
            *(short *)data = pIf->if_flags;
            break;

        case SIOCSIFFLAGS:
            {
            short flags = *(short *)data;

            /* Sanity check - make sure IFF_RUNNING and IFF_UP flags match. */
              
            if (pIf->if_flags & IFF_UP)
                flags |= (IFF_UP|IFF_RUNNING);
            else
                flags &= ~(IFF_UP|IFF_RUNNING);

            pIf->if_flags = flags;
	    }
	break;

	default:
	    printf ("bp%d: ioctl 0x%x not implemented\n", unit, cmd);
	    error = EINVAL;
	    break;
	}

    (void) splx (level);
    return (error);
    }

/*******************************************************************************
*
* ringinit -
*
* Initialize the ring pointed to by "rp" to contain "size" slots.
* The ring is set up to be empty and unlocked.
* Size had better be a power of 2!
*/

LOCAL void ringinit
    (
    FAST RING *rp,
    short size
    )
    {
    rp->r_tas = rp->r_rdidx = rp->r_wrtidx = 0;
    rp->r_size = BP_HTONS(size);
    }
/*******************************************************************************
*
* ringput -
*
* Add a new entry "v" to the ring "rp".
* Returns 0 if ring was full, else the new number of entries in the ring.
* Uses test-and-set routine, allowing multi-processor interlocked access
* to the ring IF the ring is in VME memory.
* Masks all interrupts, in order to prevent reentrancy and to
* minimize ring lockout.
*
* RETURNS: new number of entries, or 0 if ring was full
*/

LOCAL int ringput
    (
    FAST RING *rp,
    int v
    )
    {
    FAST int x = DELAY_COUNT;	/* don't wait forever for lock */
    FAST int s;

    for (;;)
	{
	/* disable interrupts, then lock the ring */
	s = intLock ();
	if (TAS(&rp->r_tas))
	    break;
	intUnlock (s);

	if (--x == 0)
	    return (0);	/* ring is stuck! */
	}

    if ((x = (BP_NTOHS(rp->r_wrtidx) + 1) &
	    (BP_NTOHS(rp->r_size) - 1)) != BP_NTOHS(rp->r_rdidx))
	{
	rp->r_slot[BP_NTOHS(rp->r_wrtidx)] = v;
	rp->r_wrtidx = BP_HTONS(x);
	}			/* if x == rdidx then ring is full */

    if ((x -= BP_NTOHS(rp->r_rdidx)) < 0)
	x += BP_NTOHS(rp->r_size);

#if     (CPU_FAMILY==MIPS)
    TAS_CLEAR(rp->r_tas);               /* unlock ring */
#else   /* CPU_FAMILY==MIPS */
    rp->r_tas = 0;                      /* unlock ring */
#endif  /* CPU_FAMILY==MIPS */


    intUnlock (s);

    return (x);
    }
/*******************************************************************************
*
* ringget -
*
* Extract next entry from the ring "rp".
* Uses test-and-set routine, allowing multi-processor interlocked access
* to the ring IF the ring is in VME memory.
* Masks all interrupts, in order to prevent reentrancy and to
* minimize ring lockout.
*
* RETURNS: 0 if ring is empty, else the extracted entry
*/

LOCAL int ringget
    (
    FAST RING *rp
    )
    {
    FAST int x = DELAY_COUNT;	/* don't wait forever for lock */
    FAST int s;

    for (;;)
	{
	/* disable interrupts, then lock the ring */
	s = intLock ();
	if (TAS(&rp->r_tas))
	    break;
	intUnlock (s);

	if (--x == 0)
	    return (0);	/* ring is stuck! */
	}

    if (rp->r_rdidx != rp->r_wrtidx)
	{
	x = rp->r_slot[BP_NTOHS(rp->r_rdidx)];
	rp->r_rdidx = BP_HTONS(
		(BP_NTOHS(rp->r_rdidx) + 1) & (BP_NTOHS(rp->r_size) - 1));
	}
    else
	x = 0;

#if     (CPU_FAMILY==MIPS)
    TAS_CLEAR(rp->r_tas);               /* unlock ring */
#else   /* CPU_FAMILY==MIPS */
    rp->r_tas = 0;                      /* unlock ring */
#endif  /* CPU_FAMILY==MIPS */


    intUnlock (s);

    return (x);
    }
/******************************************************************************
*
* ringcount - get number of entries in ring
*/

LOCAL int ringcount
    (
    FAST RING *pRing
    )
    {
    int n = BP_NTOHS(pRing->r_wrtidx) - BP_NTOHS(pRing->r_rdidx);

    if (n < 0)
	n += BP_NTOHS(pRing->r_size);

    return (n);
    }
/********************************************************************************
* vxTasac - test-and-set-and-check
*
* This routine provides quick and dirty test and set when hardware isn't up to
* the tas.
*
* RETURNS:
*   TRUE if value had been not set (but now is),
*   FALSE if value was already set.
*/

LOCAL BOOL vxTasac
    (
    FAST char *address          /* address to be tested */
    )
    {
    FAST int tasWait = 10;
    FAST int count;
    FAST int value = 0x80 + sysProcNumGet ();
    int oldLevel = intLock ();

    /* check for lock available */

    if (*address != 0)
        {
        intUnlock (oldLevel);
        return (FALSE);
        }

    /* lock available - take it */

    *address = value;

    /* check that we got it and nobody stepped on it, several times */

    for (count = 0; count < tasWait; count++)
        {
        if (* (unsigned char *) address != value)
            {
            intUnlock (oldLevel);
            return (FALSE);
            }
        }

    intUnlock (oldLevel);
    return (TRUE);
    }
/*******************************************************************************
*
* bpShow - display information about the backplane network
*
* This routine shows information about the different CPUs configured
* in the backplane network.
*
* EXAMPLE
* .CS
*     -> bpShow
*     Anchor at 0x800000
*     heartbeat = 705, header at 0x800010, free pkts = 237.
*
*     cpu int type    arg1       arg2       arg3    queued pkts rd index
*     --- -------- ---------- ---------- ---------- ----------- --------
*      0  poll            0x0        0x0        0x0       0        27
*      1  poll            0x0        0x0        0x0       0        11
*      2  bus-int         0x3       0xc9        0x0       0        9
*      3  mbox-2         0x2d     0x8000        0x0       0        1
*
*     input packets = 192     output packets = 164
*     output errors = 0       collisions = 0
*     value = 1 = 0x1
* .CE
*
* RETURNS: N/A
*/

void bpShow
    (
    char *bpName,       /* backplane interface name (NULL == "bp0") */
    BOOL zero           /* TRUE = zap totals */
    )
    {
    FAST BP_SOFTC *bs;
    FAST BP_ANCHOR *pAnchor;
    FAST BP_HEADER *pHdr;
    FAST CPU_DESC *pCpu;
    RING *pRing;
    FAST int cpuNum;

    /* use "bp0" if no interface name specified */

    if (bpName == NULL)
	bpName = "bp0";

    /* get pointer to interface descriptor */

    if ((bs = (BP_SOFTC *) ifunit (bpName)) == NULL)
	{
	printf ("interface %s not attached.\n", bpName);
	return;
	}

    /* check validity of anchor */

    pAnchor = bs->bs_pAnchor;

    printf ("Anchor at 0x%x", (int)pAnchor);
#ifdef	UNIX
    if (peek ((short *)pAnchor) == -1)
#else	/* UNIX */
    if (vxMemProbe ((char *) pAnchor, O_RDONLY, 4, (char *) &bpBitBucket) != OK)
#endif	/* UNIX */
	{
	printf (", UNREADABLE\n");
	return;
	}

    if (BP_NTOHL(pAnchor->an_ready) != BP_READY)
	{
	if (BP_NTOHL(pAnchor->an_ready) == BP_READY_SOFT_TAS)
	    printf (", SOFT TAS\n");
	else
	    {
	    printf (", UNINITIALIZED\n");
	    return;
	    }
	}

    if (bpAlive (bs) == ERROR)
	{
	printf (", NO HEARTBEAT\n");
	return;
	}


    /* anchor valid - print out anchor and header info */

    pHdr = ItoKval (((BP_ANCHOR *)pAnchor)->an_pHdr, BP_HEADER *, pAnchor);

    printf (" heartbeat = %d, header at 0x%x, free pkts = %d.\n\n",
	    (int)BP_NTOHL(pAnchor->an_heartbeat),
	    (int)pHdr, ringcount ((RING *)(bs->bs_pFreeRing)));


    /* print out info for each CPU */

    printf (
	"cpu int type    arg1       arg2       arg3    queued pkts rd index\n");
    printf (
	"--- -------- ---------- ---------- ---------- ----------- --------\n");

    for (cpuNum = 0; cpuNum < MAXCPU; ++cpuNum)
	{
	pCpu = &pHdr->hdr_cpuDesc[cpuNum];

	if (pCpu->cd_active)
	    {
	    printf ("%2d  ", cpuNum);

	    switch (BP_NTOHL(pCpu->cd_intType))
		{
		case BP_INT_NONE:	printf ("poll    "); break;
		case BP_INT_MAILBOX_1:	printf ("mbox-1  "); break;
		case BP_INT_MAILBOX_2:	printf ("mbox-2  "); break;
		case BP_INT_MAILBOX_4:	printf ("mbox-4  "); break;
		case BP_INT_MAILBOX_R1:	printf ("mbox-1r "); break;
		case BP_INT_MAILBOX_R2:	printf ("mbox-2r "); break;
		case BP_INT_MAILBOX_R4:	printf ("mbox-4r "); break;
		case BP_INT_BUS:	printf ("bus-int "); break;
		default:
			printf ("%#8x",BP_NTOHL(pCpu->cd_intType));
			break;
		}

	    printf (" %#10x %#10x %#10x",
		    BP_NTOHL(pCpu->cd_intArg1),
		    BP_NTOHL(pCpu->cd_intArg2),
		    BP_NTOHL(pCpu->cd_intArg3));

	    pRing = ItoKval (pCpu->cd_pInputRing, RING *, pAnchor);
	    printf ("     %3d       %3d\n", ringcount (pRing), BP_NTOHS(pRing->r_rdidx));
	    }
	}

    printf ("\ninput packets = %3ld  ", bs->bs_if.if_ipackets);
    if (zero)
	bs->bs_if.if_ipackets = 0;

    printf ("output packets = %3ld\n", bs->bs_if.if_opackets);
    if (zero)
	bs->bs_if.if_opackets = 0;

    printf ("output errors = %3ld  ", bs->bs_if.if_oerrors);
    if (zero)
	bs->bs_if.if_oerrors = 0;

    printf ("collisions     = %3ld\n", bs->bs_if.if_collisions);
    if (zero)
	bs->bs_if.if_collisions = 0;
    }
#ifndef UNIX
/******************************************************************************
*
* bpSlotCheck - check if a CPU is being serviced by a backplane unit
*
* This routine can be used on a backplane master CPU to find out
* if a particular slave CPU is being serviced by a particular
* backplane device unit.
*
* NOMANUAL
*/

BOOL bpSlotCheck
    (
    int unit,                   /* backplane device unit */
    int slot                    /* CPU number  */
    )
    {
    FAST BP_SOFTC  *bs;
    FAST BP_ANCHOR *pAnchor;
    FAST BP_HEADER *pHdr;
    FAST CPU_DESC  *pCpu;
    char	   devName [10];

    sprintf (devName, "bp%d", unit);

    bs = (BP_SOFTC *) ifunit (devName);

    if (bs == NULL)
        return (FALSE);

    pAnchor = bs->bs_pAnchor;

    if (vxMemProbe ((char *) pAnchor, O_RDONLY, 4, (char *) &bpBitBucket) != OK)
        return (FALSE);

    if (BP_NTOHL(pAnchor->an_ready) != BP_READY &&
		BP_NTOHL(pAnchor->an_ready) != BP_READY_SOFT_TAS)
        return (FALSE);

    pHdr = ItoKval (((BP_ANCHOR *)pAnchor)->an_pHdr, BP_HEADER *, pAnchor);
    pCpu = &pHdr->hdr_cpuDesc [slot];

    if (pCpu->cd_active)
        return (TRUE);

    return (FALSE);
    }
#endif
