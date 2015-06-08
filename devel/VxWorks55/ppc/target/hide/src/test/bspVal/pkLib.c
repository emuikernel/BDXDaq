/* pkLib.c - BSP PortKit Validation Suite support routines */

/* Copyright 2002 Wind River Systems, Inc. */
#include "copyright_wrs.h"

/*
modification history
--------------------
02c,20jun02,jtp  evaluate config.h macros at runtime (SPR #78881).
02b,24jan02,pmr  using volatile declarations for for-loop-generated delays.
02a,22jan02,pmr  fixed non-ANSI compliant code in pkTimestampTestLong();
		 corrected use of NULL; fixed warnings.
01z,12dec01,pch  Don't use strncmp on binary data; make NvRam buffers global
		 to assist debugging.
01y,16oct99,sbs  corrected documentation.
01x,30sep99,sbs  changed pkUdpEchoT function arguments (SPR #27842).
                 added pkCatastrophicTest and pkCatastrophicClean to fix
                 problems with error2 test and wtxtcl (SPR #27822).
01w,23feb99,sbs  added pkPrintTest1 and made some changes to make
                 VTS compatible with Tornado 2.0 (SPR #22425).
01v,22jul98,sbs  added htons() for port assignment in pkUdpEchoT().
                 (SPR #20680)
01u,27may98,dat  removed ARM specific changes 01s and 01t.
01t,27apr98,cdp  make ARM pkTestNvRam buffers static to avoid stack overflow.
01s,09apr98,cdp  fix buffer alignment for ARM in pkTestNvRam.
01r,21oct97,db   fixed retrial of UDP packets in pkUdpEchoT().
01q,04sep97,sbs  added pkPrintSerialOutput, pkSerialTestInit, pkSerialTest
                 and pkSerChanOutput.
01p,28aug97,sbs  addded pkBusErrorTest for error1 test and changed scsi
                 routines to handle a larger buffer size.
01o,21aug97,db   renamed pkUdpEchoTest to pkUdpEchoT. added error handling.
01n,31jul97,db   included pingLib.h.
01m,19jul97,db   added pkTimestampTestShort and pkTimestampTestLong(timestamp
		 test), pkUdpEchoTest and pkPing (network test) and pkIntGet
		 and pkIntSet(busTas test). Removed conditional includes of
		 header files and macros.
01l,24jun97,sbs  added pkPrintTest() and pkDivideZeroTest() for serial line
                 testing and divide by zero testing respectively.
01k,02jun97,sbs  changed sp() in pkLoopbackTest to taskSpawn().
01j,02aug96,mas  added support for BSP-specific TAS clear routine in busTas.
01i,15jun96,dat  changed calc of # scsi blks to avoid overflow.
01h,15feb96,dat  limited pkRead to less than 512K cycles.
		 Changed pkGetMacroValue to do a table walk.
01h,02dec93,dzb  added pkTimestampTest().
01g,05may93,dzb  combined pkGet<macro>()'s into pkGetMacroValue().
                 integrated serial loopback and network udp echo tests.
		 added multiple serial loopback test.  ANSIfied.
		 added serial console echo test.  removed pkTasInit().
		 removed cache coherency code from busTas test.
           +caf  wrote sequential serial loopback test.
	   +rfs  wrote pkUdpEchod().
01f,19feb93,dzb  doc fixes.  included additional header files.
		 added support for non-writeable nvram. fixed ANSI warnings.
		 added check for existing physical SCSI device.
           +caf  changed VOID to void. changed READ to VX_READ. changed WRITE
		 to VX_WRITE. changed copyright notice. fixed ANSI warnings.
		 changed bitBucket from char to long in pkRead().
01d,30apr92,caf  added pkTestNvRam().
01c,07apr92,caf  don't #include header files when building documentation.
01b,31mar92,caf  added pkGet...() routines to read macros compiled into BSP.
                 changed copyright notice. changed void to VOID.
                 added routines for SCSI write/read/verify test.
                 added routines for sysBusTas() test.
01a,01mar92,caf  written.
*/

/*
DESCRIPTION
This library contains support routines for the BSP Developer's kit
Validation Test Suite.

This module should be copied in the BSP directory (with sysLib.c,
config.h, etc.) and compiled by simply typing:

    % make pkLib.o

Alternatively, this module may be linked in with the VxWorks image during
compilation.  This can be done by either adding "pkLib.o" to the MACH_EXTRA
macro in the makefiles, or by including "ADDED_MODULES=pkLib.o" on the command
line when building the VxWorks image.  This approach is necessary when using
standalone ROMS without network initialization, as pkLib.o cannot be
downloaded to the target during the tests.

The object module will have a couple of unresolved references, which
will be resolved by linking with VxWorks running on the target, or
rebuilding the VxWorks image on the host to include this module.

The routines contained in this library are called by the Expect scripts
during the BSP tests.  They contain operations that cannot be executed
directly from the shell by the scripts.
*/

/* includes */

#ifndef DOC

#include "vxWorks.h"
#include "vxLib.h"
#include "ioLib.h"
#include "taskLib.h"
#include "sysLib.h"
#include "cacheLib.h"
#include "string.h"
#include "stdio.h"
#include "config.h"
#include "usrLib.h"
#include "sockLib.h"
#include "logLib.h"
#include "in.h"
#include "intLib.h"
#include "tickLib.h"
#include "semLib.h"
#include "wdLib.h"
#include "pingLib.h"
#include "selectLib.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <time.h>
#include <signal.h>

/* header files used by timestamp test */

#include "drv/timer/timestampDev.h"
#include "intLib.h"
#include "tickLib.h"

/* header file used by scsi test */

#include "scsiLib.h"

#ifdef INCLUDE_SM_NET
IMPORT VOIDFUNCPTR smUtilTasClearRtn;
#endif /* INCLUDE_SM_NET */

#endif /* DOC */

/* defines */

#define PK_LOOP_PAT_LEN		(255)	/* length of loopback ASCII pattern */
#define PK_LOOP_OUT_LEN		(300)	/* length of loopback output buffer */
#define PK_LOOP_TASK_NUM	(4)	/* num of multiple loopback tasks */
#define	PK_READ_MAX_TESTS	(128*1024) /* max number of pkRead tests */
#define PK_TS_ERR               1.0      /* acceptable timestamp % error */
#define	MIN(X,Y)		(((X) < (Y)) ? (X) : (Y))

#define PK_MAX_SER_STR          20      /* max strings to receive(serial test)*/
#define PK_MAX_STR              20      /* max string length */

#define PATTERN_INC_LAW	0

/* values used to prefill RW_TEST_UNIT */

#define DEF_BUF_SIZE 	(64 * 1024)		/* default buffer size (64K) */
#define DEF_LAW		PATTERN_INC_LAW		/* incremental law */
#define ST_PATTERN		0xa5a5a5a5	/* start pattern */
#define DEF_NB_BUF		1		/* write one buffer */

/* get field from SCSI_BLK_DEV to SCSI_PHYS_DEV */

#define BLK_TO_PHYS(p,tag) (((SCSI_BLK_DEV*)p)->pScsiPhysDev->tag)

/* check for bad buffer size */

#define  IF_BAD_BUFSIZE(pBlkDev,size) \
         if ((size % ((SCSI_BLK_DEV*)pBlkDev)->pScsiPhysDev->blockSize) != 0)

typedef union		/* equivalence int char[] */
    {
    int ival;
    UINT8 uint8val[4];
    } EQUI_INT_UINT8;

typedef struct		/* structure used to start a RW test */
    {
    /* value to initialize to start the test */

    BLK_DEV *pBlk;		/* pointer to the block device to test */
    UINT8 *ptR;			/* pointer to the read Buffer */
    UINT8 *ptW;			/* pointer to the write Buffer */
    UINT32 size;		/* buffer size to use */
    int nbBuf;			/* # of buffer to write/Read */
    int law;			/* law use to fill the patterns */
    int stBlk;			/* # of the first block to start */
    int nbLoop;			/* # of loop for the test */
    int startPattern;		/* value for the first pattern */

    /* local value use by the routine useful for post checking */

    TBOOL abort;		/* flag to abort the test */
    TBOOL abortOnError;		/* stop at the first error */
    int countErr;		/* error counter */
    int curStartPattern;	/* first pattern to fill buffers */
    int curPattern;		/* current pattern  used */
    int curCycle;		/* # of Current cycle executed */
    int curBuf;			/* # of Current buffer tested */
    int curBlk;			/* # of the current block device */
    } RW_TEST_UNIT;

/* defines used by pkUdpEchoT */

#define MAX_PKT         4096            /* largest packet used */
#define N_SIZES         16              /* number of different packet sizes */
#define RESEND		1		/* times to resend before error */

#ifndef IPPORT_ECHO
#define IPPORT_ECHO	7		/* standard UDP echo port */
#endif


/* globals */

int	pkLoopbackAbort = 0;		/* non-zero for loopback task exit */
int	pkMultiLoopCount [NUM_TTY][PK_LOOP_TASK_NUM]; /* multi loopback data */
int	pkSeqLoopCount [NUM_TTY][2];	/* seq loopback data */
int	pkLoopFd [NUM_TTY];		/* array of loopback serial dev desc */
int	pkLoopTask [NUM_TTY];		/* count of multi loopback tasks */
int	pkLoopInit = 0;			/* init loopback data arrays ? */
SEM_ID  tsTmoSem;                       /* ID of timestamp timeout semaphore */
WDOG_ID tsTmoWd;                        /* ID of timestamp timeout watchdog */
int	tsInit=FALSE;			/* TRUE if tsTmoSem and tsTmoWd exist */
char    pkSerChanInput[PK_MAX_SER_STR][PK_MAX_STR]; /* serial I/O string array */
int     pkSerChanAbort=0;               /* non-zero for serial I/O task exit */
int     pkConsoleEchoAbort = 0 ;



RW_TEST_UNIT testUnit;			/* accessible from VxWorks shell */
RW_TEST_UNIT *pTestUnit = &testUnit;    /* pointer to testUnit */

/* Globals for udpEchod */

int	flagInitUdp = FALSE;
int    	skt1 = -1;                  /* socket descriptor */
struct 	sockaddr_in sin;
struct 	sockaddr_in from;
static 	char buf [MAX_PKT];           /* pkt buffer */

/* Globals for udpEchoTest */

int    resendCnt = 0;			/* times a single packet was resent */
int    resendTotal = 0;			/* total number of packets resent */
int    packetTMO;			/* packet timeout before resending */
int    packetNum;			/* current packet number being sent */
int    curPktSiz;                       /* current packet size sent/received */
int    skt;                             /* socket descriptor */
int    sktAddrLen;                      /* length of socket addr */
struct sockaddr_in serv_addr;           /* target socket data */
char   dataOut [MAX_PKT];               /* outgoing pkt buf */
char   dataIn  [MAX_PKT];               /* incoming pkt buf */

int    pktSizes [N_SIZES] =
    {
    128, 256, 512, 768,
    1024, 1280, 1536, 1792,
    2048, 2304, 2560, 2816,
    3072, 3328, 3584, 3840
    };                                  /* various packet sizes */


/* locals */

LOCAL int loopState [NUM_TTY];	/* current state of seq loopback input */

/* forward declarations */

	STATUS	pkRead();
	void	pkIncrement();
	int     pkGetMacroValue();
	void    pkTasTest();
	STATUS  pkTestRamByte();
	STATUS	pkConsoleEcho();
LOCAL	BOOL	pkMultiLoopHook();
LOCAL	BOOL	pkSeqLoopHook();
	STATUS	pkLoopbackTest();
	STATUS	pkSeqLoopShow();
	STATUS	pkMultiLoopShow();
	STATUS	pkUdpEchod();
	void	pkUdpInit();
        STATUS  pkTimestampTest();
	STATUS  pkTimestampTestShort();
	void    pkTimestampTestLong();
        STATUS  pkTimestampTestInit();
        void    pkPrintTest();
        void    pkDivideZeroTest();
	int	pkIntGet();
	void	pkIntSet();
	STATUS	pkTestNvRam();
	STATUS	pkBufFillPattern();
	STATUS	pkBlkWrite();
	STATUS	pkBlkRead();
	STATUS	pkTestUnit();
	STATUS	pkTestOneUnit();
        void    pkSerialTest();
        void    pkSerChanOutput();


/*******************************************************************************
*
* pkRead - probe memory from start to end <length> bytes at a time
*
* This routine probes memory from the start address to the end address
* using vxMemProbe(), <length> bytes at a time.  As soon as an error is
* detected, the routine returns an ERROR.
*
* RETURNS: OK, or ERROR if <startAdrs> through (<endAdrs> - 1) not readable.
*/

STATUS pkRead
    (
    char *startAdrs,	/* start of range to test */
    char *endAdrs,	/* end of range to test   */
    int	length		/* 1, 2, or 4 bytes       */
    )
    {
    char *	loopAdrs;
    long	bitBucket;
    STATUS	retValue = OK;
    int		increment = 1;
    int		size;

    size = ((UINT)endAdrs - (UINT)startAdrs) / length;

    /* limit the number of test cycles */
    if (size >= (UINT)PK_READ_MAX_TESTS)
	increment = (size / (UINT)PK_READ_MAX_TESTS) * 2;

    for (loopAdrs = startAdrs;
	loopAdrs < endAdrs;
	loopAdrs += (length * increment))
	{
	if (vxMemProbe (loopAdrs, VX_READ, length, (char *) &bitBucket)
	    == ERROR)
	    {
	    /* probe caused a bus error or address misalignment */
	    retValue = ERROR;
	    break;
	    }
	}

    return retValue;
    }

/*******************************************************************************
*
* pkIncrement - increment a counter
*
* This routine adds one to the integer pointed to by <pCount>.
*
* RETURNS: N/A
*/

void pkIncrement
    (
    int *pCount		/* address to increment */
    )
    {
    ++ *pCount;
    }

/*******************************************************************************
*
* pkGetMacroValue - read a macro value
*
* This routine returns the value of the macro associated with <macroName>,
* else the value -1 (ERROR) is returned.
*
* RETURNS: Value associated with macroName, or ERROR (-1).
*/

int pkGetMacroValue
    (
    char *macroName	/* name of desired macro */
    )
    {
    /*
     * Note: since the config macros may contain an expression that does
     * not have a constant integer value, it is important to evaluate
     * them inline here, rather than place them in a static array
     * initializer, which would be evaluated only at compile time.
     */

#ifdef LOCAL_MEM_LOCAL_ADRS
    if (strcmp (macroName, "LOCAL_MEM_LOCAL_ADRS") == 0)
	return (int)LOCAL_MEM_LOCAL_ADRS;
#endif
#ifdef ROM_BASE_ADRS
    if (strcmp (macroName, "ROM_BASE_ADRS") == 0)
	return (int)ROM_BASE_ADRS;
#endif
#ifdef ROM_SIZE
    if (strcmp (macroName, "ROM_SIZE") == 0)
	return (int)ROM_SIZE;
#endif
#ifdef NV_RAM_SIZE
    if (strcmp (macroName, "NV_RAM_SIZE") == 0)
	return (int)NV_RAM_SIZE;
#endif
#ifdef CONSOLE_TTY
    if (strcmp (macroName, "CONSOLE_TTY") == 0)
	return (int)CONSOLE_TTY;
#endif
#ifdef SLIP_TTY
    if (strcmp (macroName, "SLIP_TTY") == 0)
	return (int)SLIP_TTY;
#endif
#ifdef SYS_CLK_RATE_MIN
    if (strcmp (macroName, "SYS_CLK_RATE_MIN") == 0)
	return (int)SYS_CLK_RATE_MIN;
#endif
#ifdef SYS_CLK_RATE_MAX
    if (strcmp (macroName, "SYS_CLK_RATE_MAX") == 0)
	return (int)SYS_CLK_RATE_MAX;
#endif
#ifdef AUX_CLK_RATE_MIN
    if (strcmp (macroName, "AUX_CLK_RATE_MIN") == 0)
	return (int)AUX_CLK_RATE_MIN;
#endif
#ifdef AUX_CLK_RATE_MAX
    if (strcmp (macroName, "AUX_CLK_RATE_MAX") == 0)
	return (int)AUX_CLK_RATE_MAX;
#endif
#ifdef EXC_MSG_ADRS
    if (strcmp (macroName, "EXC_MSG_ADRS") == 0)
	return (int)EXC_MSG_ADRS;
#endif
#ifdef PK_LOOP_TASK_NUM
    if (strcmp (macroName, "PK_LOOP_TASK_NUM") == 0)
	return (int)PK_LOOP_TASK_NUM;
#endif

    return (int)ERROR;
    }

/*******************************************************************************
*
* pkTasTest - test-and-set test
*
* This routine, in conjunction with another pkTasTest() task, tests the
* test-and-set mechanism.  Both tasks (each on a separate target board)
* spin on the semaphore pointed to by <semaphore> using sysBusTas() for
* mutual exclusion.  When this routine gets the semaphore, it increments
* the <count> if it matches <odd>.  It then enters a busy-wait loop
* <delay> times, giving the other task a chance to take the semaphore.
* It may be necessary to tune this task by increasing or decreasing <delay
* if one task (board) is much faster than the other.
*
* RETURNS: N/A (does not return)
*/

void pkTasTest
    (
    char *semaphore,	/* semaphore to spin on */
    UINT32 *count,	/* counter address */
    BOOL odd,		/* odd/even assignment */
    UINT32 delay	/* delay so other task gets time */
    )
    {
    volatile UINT32	busyWait;

    taskDelay (sysClkRateGet ());		/* wait 1 second */

    FOREVER
	if (sysBusTas (semaphore))
	    {
	    /* check <odd> and bump counter if matches */

	    if ((((* count) & 1) && odd) || ((!((* count) & 1)) && !odd))
		(* count) ++;

#ifdef INCLUDE_SM_NET
	    /* use the utility routine, if available */

	    if (smUtilTasClearRtn != NULL)
		(smUtilTasClearRtn)(semaphore);
	    else
#endif /* INCLUDE_SM_NET */

	    * semaphore = 0;

	    /* enter busy-wait loop to give other task a chance */

	    for (busyWait = 0; busyWait < delay; busyWait ++)
		;				/* wait */
	    }
    }

/*******************************************************************************
*
* pkTestRamByte - test read and write access to one byte of RAM
*
* This routine uses vxMemProbe() to check that <adrs> may be written to
* and read from.
*
* RETURNS: OK, or ERROR if <adrs> is not both readable and writable.
*/
STATUS pkTestRamByte
    (
    char *adrs		/* address to test */
    )
    {
    char testR;		/* scratchpad for read */
    char write1;	/* 1st value to write  */
    char write2;	/* 2nd value to write  */

    (void) vxMemProbe (adrs, VX_READ, 1, &write2); /* save current value  */
    write1 = (write2 == 3) ? 1 : 3;		/* write1 != write2    */

    return ((vxMemProbe (adrs, VX_WRITE, 1, &write1) != OK) ||
	    (vxMemProbe (adrs, VX_READ, 1, &testR) != OK)   ||
	    (testR != write1)                               ||
	    (vxMemProbe (adrs, VX_WRITE, 1, &write2) != OK) ||
	    (vxMemProbe (adrs, VX_READ, 1, &testR) != OK)   ||
	    (testR != write2)) ? ERROR : OK;
    }

#ifdef NV_RAM_SIZE

char		vtsOrigBootLine [NV_RAM_SIZE + 2];	/* original boot line */
unsigned char	vtsNvRamBuf1 [NV_RAM_SIZE + 2];		/* scratch buffer 1   */
unsigned char	vtsNvRamBuf2 [NV_RAM_SIZE + 2];		/* scratch buffer 2   */

/*******************************************************************************
*
* pkTestNvRam - collection of non-volatile RAM tests
*
* This routine contains several tests to check the board's non-volatile
* RAM.  The parameter <testNum> indicates which test will be run.
*
* If the macro NV_RAM_SIZE_WRITEABLE is defined, only that number of bytes
* is used for tests which write to nvram.
*
* The tests in this routine write and read various patterns to and from nvram,
* making sure no errors are detected.  Additionally, parameter checking is
* performed on the sysNvRamGet() and sysNvRamSet() routines.
*
* RETURNS: OK, or ERROR if <testNum> is invalid or the test fails.
*/

STATUS pkTestNvRam
    (
    int testNum		/* test number (1-8) */
    )
    {
    static int          writeableSize;			/* writeable nvram sz */
    FAST int		ii;				/* scratch loop index */
    STATUS		retValue = ERROR;		/* return value       */
    int			offset = -NV_BOOT_OFFSET;	/* offset to nv start */

#ifdef NV_RAM_SIZE_WRITEABLE
    writeableSize=NV_RAM_SIZE_WRITEABLE;
#else /* NV_RAM_SIZE_WRITEABLE */
    writeableSize=NV_RAM_SIZE;
#endif /* NV_RAM_SIZE_WRITEABLE */

    if (writeableSize > NV_RAM_SIZE)
        return(retValue);
    switch (testNum)
	{
	case 1:	/* sysNvRamGet() of boot line */
	    {
	    if (sysNvRamGet((char *)vtsNvRamBuf1, NV_RAM_SIZE, offset) == ERROR)
		break;

            /* store the original bootline */

	    bcopy ((char *) vtsNvRamBuf1, (char *) vtsNvRamBuf2, NV_RAM_SIZE);
	    bcopy ((char *) vtsNvRamBuf1, vtsOrigBootLine, NV_RAM_SIZE);

            for (ii = 0; ii < NV_RAM_SIZE; ii ++)	/* complement string */
                vtsNvRamBuf1 [ii] ^= 0xff;

	    (void) sysNvRamGet ((char *) vtsNvRamBuf1, NV_RAM_SIZE, offset);

	    if (memcmp((char *)vtsNvRamBuf1, (char *)vtsNvRamBuf2, NV_RAM_SIZE)
		== 0)
		retValue = OK;

	    break;
	    }

	case 2:	/* sysNvRamSet() and sysNvRamGet() of complemented boot line  */
	    {
	    if (sysNvRamGet((char *)vtsNvRamBuf1, NV_RAM_SIZE, offset) == ERROR)
		break;

            for (ii = 0; ii < NV_RAM_SIZE; ii ++)	/* complement string */
                vtsNvRamBuf1 [ii] ^= 0xff;

	    if (sysNvRamSet((char *)vtsNvRamBuf1, writeableSize, offset)
		== ERROR)
		break;

	    (void) sysNvRamGet ((char *) vtsNvRamBuf2, NV_RAM_SIZE, offset);

	    if (memcmp((char *)vtsNvRamBuf1,(char *)vtsNvRamBuf2, writeableSize)
		== 0)
		retValue = OK;

	    break;
	    }

	case 3:	/* sysNvRamGet() with length zero */
	    {
	    if (NV_RAM_SIZE < 3)
		break;

	    vtsNvRamBuf1 [0] = '1';
	    vtsNvRamBuf1 [1] = '2';
	    vtsNvRamBuf1 [2] = '3';

	    if (sysNvRamGet ((char *) (vtsNvRamBuf1 + 1), 0, offset) == ERROR)
		break;

	    /* verify a zero in vtsNvRamBuf [1], while vtsNvRamBuf [0] and vtsNvRamBuf [2] unchanged  */

	    if ((vtsNvRamBuf1 [0] == '1') && (vtsNvRamBuf1 [1] == 0)
		&& (vtsNvRamBuf1 [2] == '3'))
		retValue = OK;

	    break;
	    }

	case 4:	/* sysNvRamSet() parameter checking */
	    {
	    if ((sysNvRamSet((char *)vtsNvRamBuf1, -1, offset)    == ERROR) &&
		(sysNvRamSet((char *)vtsNvRamBuf1, 0, offset - 1) == ERROR) &&
		(sysNvRamSet((char *)vtsNvRamBuf1, writeableSize + 1, offset)
								  == ERROR) &&
		(sysNvRamSet((char *)vtsNvRamBuf1,0, offset + writeableSize + 1)
								  == ERROR) &&
		(sysNvRamSet((char *)vtsNvRamBuf1, writeableSize / 2 + 1,
			      offset + (writeableSize / 2) + 1)	  == ERROR))
		{
		retValue = OK;
		}
	    break;
	    }

	case 5:	/* sysNvRamGet() parameter checking */
	    {
	    if ((sysNvRamGet((char *)vtsNvRamBuf1, -1, offset)    == ERROR) &&
		(sysNvRamGet((char *)vtsNvRamBuf1, 0, offset - 1) == ERROR) &&
		(sysNvRamGet((char *)vtsNvRamBuf1, NV_RAM_SIZE + 1, offset)
								  == ERROR) &&
		(sysNvRamGet((char *)vtsNvRamBuf1, 0, offset + NV_RAM_SIZE + 1)
								  == ERROR) &&
		(sysNvRamGet((char *)vtsNvRamBuf1, NV_RAM_SIZE / 2 + 1,
			      offset + (NV_RAM_SIZE / 2) + 1)	  == ERROR))
		{
		retValue = OK;
		}
	    break;
	    }

	case 6:	/* sysNvRamSet() and sysNvRamGet() of 0xff data */
	    {
	    if (writeableSize < 4)
		break;

	    vtsNvRamBuf1 [0] = vtsNvRamBuf1 [1]
	    = vtsNvRamBuf1 [2] = vtsNvRamBuf1 [3] = 0xff;
	    vtsNvRamBuf2 [0] = vtsNvRamBuf2 [1]
	    = vtsNvRamBuf2 [2] = vtsNvRamBuf2 [3] = 0x55;

	    if (sysNvRamSet ((char *) vtsNvRamBuf1, 4, offset) == ERROR)
		break;

	    if (sysNvRamGet ((char *) vtsNvRamBuf2, 3, offset) == ERROR)
		break;

	    if ((vtsNvRamBuf2 [0] == 0xff) && (vtsNvRamBuf2 [1] == 0xff) &&
		(vtsNvRamBuf2 [2] == 0xff) && (vtsNvRamBuf2 [3] == 0x00))
		{
		retValue = OK;
		}
	    break;
	    }

	case 7:	/* sysNvRamSet() and sysNvRamGet() of 0x00 data */
	    {
	    if (writeableSize < 4)
		break;

	    vtsNvRamBuf1 [0] = vtsNvRamBuf1 [1]
	    = vtsNvRamBuf1 [2] = vtsNvRamBuf1 [3] = 0x00;
	    vtsNvRamBuf2 [0] = vtsNvRamBuf2 [1]
	    = vtsNvRamBuf2 [2] = vtsNvRamBuf2 [3] = 0xff;

	    if (sysNvRamSet ((char *) vtsNvRamBuf1, 4, offset) == ERROR)
		break;

	    if (sysNvRamGet ((char *) vtsNvRamBuf2, 3, offset) == ERROR)
		break;

	    if ((vtsNvRamBuf2 [0] == 0x00) && (vtsNvRamBuf2 [1] == 0x00) &&
		(vtsNvRamBuf2 [2] == 0x00) && (vtsNvRamBuf2 [3] == 0x00))
		{
		retValue = OK;
		}
	    break;
	    }

	case 8:	/* sysNvRamSet() and sysNvRamGet() of boot line */
	    {
	    if (sysNvRamSet (vtsOrigBootLine, writeableSize, offset) == ERROR)
		break;

	    if (sysNvRamGet((char *)vtsNvRamBuf1, NV_RAM_SIZE, offset) == ERROR)
		break;

	    if (memcmp(vtsOrigBootLine, (char *)vtsNvRamBuf1, writeableSize)
		== 0)
		retValue = OK;

	    break;
	    }

	default:
	    {
	    break;
	    }
	}
    return (retValue);
    }

#endif /* NV_RAM_SIZE */

/*******************************************************************************
*
* pkConsoleEcho - routine to put console into echo mode
*
* This routine puts the serial console in raw mode, then turns on the character
* echo mode bit.  The console will remain in echo mode until <numToEcho>
* characters have been echoed, or an "eof" (ASCII 0x04) is received.  If
* <numToEcho> is zero, the only way to exit echo mode is to send an "eof".
*
* RETURNS: OK or ERROR.
*/

STATUS pkConsoleEcho
    (
    int numToEcho	/* number of characters to echo (0 means infinite) */
    )
    {
    if (numToEcho < 0)
	{
	printf ("pkConsoleEcho: invalid parameter\n");
	return (ERROR);
	}

    /* set the serial console to echo chars */

    (void) ioctl (0, FIOSETOPTIONS, OPT_RAW);
    (void) ioctl (1, FIOSETOPTIONS, OPT_RAW);
    (void) ioctl (0, FIOSETOPTIONS, OPT_ECHO);

    /* continue to echo until <numToEcho> exhausted or "eof" received  */

      while ( ! pkConsoleEchoAbort)
	{
        getc(stdin) ;
	if (numToEcho)
	    if (-- numToEcho == 0)
		break;
        }

    /* reset the console back to terminal mode */

    (void) ioctl (0, FIOSETOPTIONS, OPT_TERMINAL);
    (void) ioctl (1, FIOSETOPTIONS, OPT_TERMINAL);

    return (OK);
    }

/*******************************************************************************
*
* pkSeqLoopHook - sequential serial loopback test input hook routine
*
* This routine monitors serial inputs for the sequential serial loopback test.
* Whenever a character is written to serial port <channel>, this routine is
* called.  It updates the count of characters written to <channel> and checks
* that <inChar> matches the expected character from the sequential pattern
* being sent in pkLoopbackTest(0,X,X,X).
*
* RETURNS: TRUE.
*/

LOCAL BOOL pkSeqLoopHook
    (
    int channel,	/* channel number */
    int inChar		/* input character */
    )
    {
    char matchChar;	/* expected input character */

    if ((channel >= 0) && (channel < NUM_TTY))
	{
	++ pkSeqLoopCount [channel][0];		/* bump character counter */

	/* find next character in pattern, and bump state counter */

	if (loopState [channel] < PK_LOOP_OUT_LEN - 1)
	    matchChar = 0x00 + ((loopState [channel] ++) % PK_LOOP_PAT_LEN);
	else
	    {
	    /* output buffer turned over */

	    matchChar = '0' + (channel % 10);	/* '0' ... '9' */
	    loopState [channel] = 0;
	    }

	if (inChar != matchChar)		/* mismatch? */
	    {
	    ++ pkSeqLoopCount [channel][1];	/* bump error counter */
            loopState [channel]++;	/* compensate for dropped char */
            }
	}
    return (TRUE);		/* no further processing of the character */
    }

/*******************************************************************************
*
* pkMultiLoopHook - multiple loopback test input hook routine
*
* This routine monitors serial inputs for the multiple serial loopback test.
* Whenever a character is written to serial port <channel>, this routine is
* called.  It updates the count of characters written to <channel> from the
* task (pkLoopbackTest(1,X,X,X)) associated with <inChar>.
*
* RETURNS: TRUE.
*/

LOCAL BOOL pkMultiLoopHook
    (
    int channel,	/* channel number */
    int inChar		/* input character */
    )
    {
    int	offset;		/* offset to access correct data location */

    if ((channel >= 0) && (channel < NUM_TTY))
	{
	offset = inChar - 'A';
	++ pkMultiLoopCount [channel][offset];	/* bump character counter */
	}

    return (TRUE);		/* no further processing of the character */
    }

/*******************************************************************************
*
* pkLoopbackTest - loopback test routine
*
* This routine is called to perform either one of two tests, based upon
* the value of <mTest>.
*
* If <mTest> is 0, the sequential serial loopback test is run.  This test will
* write a sequential pattern of characters over and over, until <numToSend>
* chars are sent. If <numToSend>=0, then an infinite number of chars are sent.
* Characters are sent at <baud> baud rate to serial port <channel>.
*
* Else, if <mTest> is 1, the multiple serial test is started.  This test will
* recursively call pkLoopbackTest() until PK_LOOP_TASK_NUM number of calls
* have taken place.  Each call results in a task that will write <numToSend>
* unique characters to serial port <channel> at baud rate <baud>.
* If <numToSend>=0, then an infinite number of chars are sent.  The initial
* pkLoopbackTest opens the file descriptor to the serial port, makes the
* recursive calls, waits for the other tasks to end, and then closes the file
* descriptor.
*
* NOTE: Transmission will abort if the global variable <pkLoopbackAbort> is
* set to a non-zero value.
*
* RETURNS: OK or ERROR.
*/

STATUS pkLoopbackTest
    (
    int mTest,		/* 0 = sequential test, 1 = multiple task test */
    int channel,	/* channel number to test */
    int baud,		/* baud rate for test */
    int numToSend	/* number of chars to send (0 means infinite) */
    )
    {
    char loopName [10];	/* more than enough chars for "tyCo/NN" */
    char inBuf;         /* one character input buffer */
    char letterL = 'L';	/* one character output buffer */
    char outBuf [PK_LOOP_OUT_LEN]; /* loopback pattern output buffer */
    int  call=0;	/* number of tasks open in multiple test */
    int  loopFd;	/* file descriptor for <channel> */
    int  mule;		/* scratch */
    int  numSent=0;	/* characters sent so far in loop */

    /* check for valid parameters */

    if ((channel == CONSOLE_TTY) || (channel < 0) || (channel >= NUM_TTY) ||
        (baud == 0) || (numToSend < 0) || ((mTest != 0) && (mTest != 1)))
	{
	printf ("pkLoopbackTest: Error: invalid parameter\n");
	return (ERROR);
	}

    /* if running multiple loopback test, initialize data structures (once) */

    if (mTest == 1)
	{
        if (pkLoopInit == 0)
            {
	    pkLoopInit = 1;
 	    for (mule = 0; mule < NUM_TTY; mule ++)
 	        pkLoopTask [mule] = 0;
            }
#if (PK_LOOP_TASK_NUM < 1)
	printf ("pkLoopbackTest: Error: set for too few tasks\n");
	return (ERROR);
#else /* PK_LOOP_TASK_NUM */

        /* increment number of multiple loopback tasks open */

        call = pkLoopTask [channel] ++;
#endif /* PK_LOOP_TASK_NUM */
 	}

    /* on first call of multiple - always on sequential */

    if (call == 0)
	{
	/* open file descriptor for the loopback device */

        sprintf (loopName,"/tyCo/%d",channel);
        pkLoopFd [channel] = open (loopName, O_RDWR, 0);
	loopFd = pkLoopFd [channel];
        if (loopFd == ERROR)
	    {
	    printf ("pkLoopbackTest: Error: failed to open %s\n", loopName);
	    return (ERROR);
	    }

        /* set baud and mode, flush the buffers */

        (void) ioctl (loopFd, FIOBAUDRATE, baud);
        (void) ioctl (loopFd, FIOSETOPTIONS, OPT_RAW);
        (void) ioctl (loopFd, FIOFLUSH, 0);

        /* verify loopback operation (one character not counted in <numSent>) */

        inBuf = ~letterL;			/* inBuf differs from outBuf */
        (void) write (loopFd, &letterL, 1);	/* send a char */
        taskDelay (sysClkRateGet() / 4);	/* allow time for loopback */

        (void) ioctl (loopFd, FIONREAD, (int) (&mule));	/* any character? */
        if (mule == 1)
	    (void) read (loopFd, &inBuf, 1);		/* read it */

        if (inBuf != letterL)				/* mismatch? */
	    {
	    printf ("pkLoopbackTest: Error: loopback connection failed\n");
	    (void) close (loopFd);
	    return (ERROR);
	    }

       /* declare argument to be passed to protocol hook - the channel # */

        (void) ioctl (loopFd, FIOPROTOARG, channel);

        /* clear input counters for this channel (in case we've run before) */

	if (mTest == 0)
	    {
	    /* attach hook for receiving chars */

            (void) ioctl (loopFd, FIOPROTOHOOK, (int) pkSeqLoopHook);
	    pkSeqLoopCount [channel][0] = 0;
	    pkSeqLoopCount [channel][1] = 0;
	    }
        else
	    {
	    /* attach hook for receiving chars */

            (void) ioctl (loopFd, FIOPROTOHOOK, (int) pkMultiLoopHook);
	    for (mule = 0; mule < PK_LOOP_TASK_NUM; mule ++)
	        pkMultiLoopCount [channel][mule] = 0;
	    }

        loopState [channel] = 0;
        }
    else
       loopFd = pkLoopFd [channel];

    /* initialize output buffer */

    if (mTest == 0)
        {
        for (mule = 0; mule < PK_LOOP_OUT_LEN - 1; mule ++)
            outBuf [mule] = 0x00 + (mule % PK_LOOP_PAT_LEN); /* 'A' ... 'Z' */
        outBuf [mule] = '0' + (channel % 10);               /* '0' ... '9' */
        }
    else
	{
        for (mule = 0; mule < PK_LOOP_OUT_LEN; mule ++)
            outBuf [mule] = 'A' + call;			/* 'A' ... 'Z' */

        /* start another loopback task for multiple loopback test */

        if (call < (PK_LOOP_TASK_NUM - 1))
              taskSpawn (NULL, 100, VX_FP_TASK, 20000, pkLoopbackTest, 1,
               (int) channel, (int) baud, (int) numToSend, (int) NULL,
               (int) NULL, (int) NULL, (int) NULL, (int) NULL, (int) NULL);
	}

    /* write the characters out */

    while ((numSent < numToSend) || (numToSend == 0))
	{
	/* check abort flag */

	if (pkLoopbackAbort)
	    break;

	/* write the output buffer */

	if (numToSend)
	    mule = MIN (PK_LOOP_OUT_LEN, numToSend - numSent);/* # bytes? */
        else
	    mule = PK_LOOP_OUT_LEN;
	(void) write (loopFd, outBuf, mule);
	numSent += mule;
	}

    /* wait for i/o to complete */

    if (mTest == 1)
	pkLoopTask [channel] --;

    if (call == 0)
        {
        do
            {
            taskDelay (sysClkRateGet() / 4);
            (void) ioctl (loopFd, FIONWRITE, (int) (&mule)); /* wait for out */
            } while (mule != 0);
        taskDelay (sysClkRateGet());

        /* the first multiple task waits for all other multiple tasks to end */

        if (mTest == 1)
            while (pkLoopTask [channel] != 0)
                taskDelay (sysClkRateGet());

        (void) ioctl (loopFd, FIOPROTOHOOK, 0);
        (void) close (loopFd);
        }

    return (OK);
    }

/*******************************************************************************
*
* pkSeqLoopShow - display results of sequential loopback test
*
* This routine displays the data generated from running the sequential serial
* loopback test (pkLoopbackTest(0,X,X,X)).  The output consists of a count
* of characters written to serial port <channel> during the test.  Included is
* a count of errors encountered (an error was recorded any time a character
* was received out of order).
*
* RETURNS: OK or ERROR.
*/

STATUS pkSeqLoopShow
    (
    int	channel		/* serial channel */
    )
    {
    if ((channel == CONSOLE_TTY) || (channel < 0) || (channel >= NUM_TTY))
	{
	printf ("pkSeqLoopShow: Error: invalid parameter\n");
	return (ERROR);
	}
    printf ("characters = %d\n", pkSeqLoopCount [channel][0]);
    printf ("errors = %d\n", pkSeqLoopCount [channel][1]);
    printf ("OK\n");
    return (OK);
    }

/*******************************************************************************
*
* pkMultiLoopShow - display results of multiple loopback test
*
* This routine displays the data generated from running the multiple serial
* loopback test (pkLoopbackTest(1,X,X,X)).  The output consists of a list
* of tasks and the number of characters each task has written to the
* serial port specified by <channel>.
*
* RETURNS: OK or ERROR.
*/

STATUS pkMultiLoopShow
    (
    int	channel		/* serial channel */
    )
    {
    int mule;		/* task counter */

    if ((channel == CONSOLE_TTY) || (channel < 0) || (channel >= NUM_TTY))
	{
	printf ("pkMultiLoopShow: Error: invalid parameter\n");
	return (ERROR);
	}
    else
        {
        for (mule = 0; mule < PK_LOOP_TASK_NUM; mule ++)
            {
            printf ("task = %d    ", mule + 1);
	    printf ("count = %d\n", pkMultiLoopCount [channel][mule]);
	    }
        printf ("OK\n");
        }

    return (OK);
    }

/*******************************************************************************
*
* pkUdpEchod - echo daemon for UDP packets
*
* This routine is an echo daemon for UDP/IP networking packets.  Packets
* up to 4096 bytes are read in from port 7 (the standard UDP echo port) and
* then are written back to port 7.
*
* The function pkUdpInit() is called to create a socket and bind it.
*
* RETURNS: does not return unless error (error number given)
*/

int pkUdpEchod (void)
    {
    int cnt;                     /* size of packet received */
    int fromLen;                 /* size of host sock structure */

    fromLen = sizeof(from);

    if (!flagInitUdp)
    	{
    	pkUdpInit();
    	}

    /* loop, reading from socket, and echoing */

    while (1)
        {
        cnt = recvfrom (skt1, (char *) buf, sizeof (buf), 0,
			(struct sockaddr *) &from, &fromLen);
        if (cnt > 0)
            sendto (skt1, (char *) buf, cnt, 0, (struct sockaddr *) &from,
		    sizeof (from));
        }
    }


/*******************************************************************************
*
* pkUdpInit - initialize socket for UDP test
*
* This routine opens a socket and binds it to port 7.
*
* RETURNS: N/A
*/

void pkUdpInit (void)
    {
    int sktBufSiz;               /* size of largest packet */
    int fromLen;                 /* size of host sock structure */

    bzero ((char *) &sin, sizeof (sin));        /* clr sock addrs */
    bzero ((char *) &from, sizeof (from));

    skt1 = socket (AF_INET, SOCK_DGRAM, 0);      /* open socket */

    if (skt1 < 0)
        {
        logMsg ("pkUdpEchod: socket error\n",0,0,0,0,0,0);
        exit (1);
        }

    sin.sin_family = AF_INET;                   /* fill in our sock addr */
    sin.sin_port = htons (IPPORT_ECHO);                   /* the ECHO port */

    /* bind our sock addr */

    if (bind (skt1, (struct sockaddr *) &sin, sizeof (sin)) < 0)
        {
        logMsg ("pkUdpEchod: bind error\n",0,0,0,0,0,0);
        close (skt1);
        exit (2);
        }

    fromLen = sizeof (from);                    /* get size of sock addr */

    sktBufSiz = sizeof (buf);         /* increase size of socket buffers */

    if (setsockopt (skt1, SOL_SOCKET, SO_SNDBUF, (char *) &sktBufSiz,
        sizeof (sktBufSiz)) < 0)
        {
        close (skt1);
        logMsg ("pkUdpEchod: setsockopt error\n",0,0,0,0,0,0);
        exit (3);
        }

    if (setsockopt (skt1, SOL_SOCKET, SO_RCVBUF, (char *) &sktBufSiz,
        sizeof (sktBufSiz)) < 0)
        {
        close (skt1);
        logMsg ("pkUdpEchod: setsockopt error\n",0,0,0,0,0,0);
        exit (4);
        }
    flagInitUdp = TRUE;

    }

#ifdef	INCLUDE_TIMESTAMP

/*******************************************************************************
*
* pkTimestamp - tests sysTimestamp() and sysTimestampLock()
*
* This routine calls the function sysTimestamp() or sysTimestampLock()(after
* locking interrupts) depending on the boolean parameter locked.
*
* RETURNS: tick counter value
*/

UINT32 pkTimestamp
    (
    BOOL locked
    )
    {
    int lockKey;
    UINT32 tickRet;

    if (!locked)
        return (sysTimestampLock ());

    lockKey = intLock ();
    tickRet = sysTimestamp ();
    intUnlock (lockKey);

    return (tickRet);
    }

/*******************************************************************************
*
* pkTimestampRoll - returns a counter value or the current tick
*
* This routine reads the location pointed to by tickCounter if non null, or
* returns the current system clock tick.
*
* RETURNS: counter value or clock tick
*/

UINT32 pkTimestampRoll
    (
    UINT *tickCounter
    )
    {
    if (tickCounter == 0)
	return (tickGet());
    else
	return (*tickCounter);
    }


/*******************************************************************************
*
* pkTimestampTestShort - short-term test of timestamp timer
*
* This routine performs a short-term test of the timestamp timer. It reads
* values with a delay in-between. The test is repeated up to 16 times. If
* the counter is always increasing the test is successful.
*
* RETURNS: OK if success, ERROR otherwise.
*/

STATUS pkTimestampTestShort
    (
    UINT *tickCounter,
    BOOL locked
    )
    {
    UINT countVal [2];
    UINT32 tickVal [2];
    STATUS retValue = OK;
    int ix;
    volatile int iy;

    /* short term test: make sure timestamp is incrementing */

    for (ix = 0; (ix < 0x10) && (retValue == OK); ix++)
    	{
        countVal [0] = pkTimestampRoll (tickCounter);
        tickVal [0]  = pkTimestamp (locked);

	for (iy = 0; iy < 1000; iy++)
		;

        tickVal [1]  = pkTimestamp (locked);
        countVal [1] = pkTimestampRoll (tickCounter);
	if ((tickVal [0] >= tickVal [1]) && (countVal [0] >= countVal [1]))
		retValue = ERROR;
	}

        return (retValue);
    }


/*******************************************************************************
*
* pkTimestampTestInit - initialize timestamp test
*
* This routine creates a semahore and a watchdog timer for the timestamp
* test. The ids are stored in two global variables and a flag is set if
* the two are created successfuly.
*
* RETURNS: OK if success, ERROR otherwise.
*/

STATUS pkTimestampTestInit()
    {

    tsTmoSem = semBCreate (SEM_Q_FIFO, SEM_EMPTY);
    tsTmoWd  = wdCreate ();

    if ((tsTmoSem == NULL) || (tsTmoWd == NULL))
        {
        tsInit = FALSE;
        return ERROR;
        }
    else
        {
        tsInit = TRUE;
        return OK;
        }
    }



/*******************************************************************************
*
* pkTimestampTestLong - long-term timestamp test
*
* This routine performs a long-term timestamp test.
*
* RETURNS: OK, or ERROR.
*/

void pkTimestampTestLong
    (
    UINT *tickCounter,
    int timeout,
    BOOL locked,
    int	nslices
    )
    {
    UINT * pCountVal = malloc (nslices * 2 * sizeof (UINT));
    UINT32 * pTickVal = malloc (nslices * 2 * sizeof (UINT32));
    UINT * pCountPtr = pCountVal;
    UINT32 * pTickPtr = pTickVal;
    UINT32 calcTicks;
    volatile UINT32 tsBucket;
    float errorVal;
    int second;
    int delay;
    int ix;
    STATUS retValue = OK;

    second = timeout / nslices;
    delay = second * sysClkRateGet();
    calcTicks = second * sysTimestampFreq();

    if (tsInit == FALSE)
        {
        if (pkTimestampTestInit() == ERROR)
    	    {
    	    printf("FAIL long-term timestamp test\n");
	    goto pkTimestampTestLongExit;
	    }
         }

    printf ("\nStarting long-term timestamp test.\n");
    printf ("This test will run for approximately %.1f minute(s).\n\n",
	(float) timeout/60);

    for (ix = 0; ix < nslices; ix++)
	{
        if (wdStart (tsTmoWd, delay, semGive, (int) tsTmoSem) == ERROR)
        	{
    		printf("FAIL long-term timestamp test\n");
	        goto pkTimestampTestLongExit;
		}

        *pCountPtr++ = pkTimestampRoll (tickCounter);
        *pTickPtr++ = pkTimestamp (locked);

        while ((semTake (tsTmoSem, NO_WAIT)) != OK)
	    {
            taskDelay (2);
	    tsBucket = pkTimestamp (locked);
	    }

        *pCountPtr++ = pkTimestampRoll (tickCounter);
        *pTickPtr++ = pkTimestamp (locked);

	printf ("pkTimestampTest: %d/%d through test\n", (ix + 1), nslices);
	}

    /* convert to total ticks */

    for (pTickPtr = pTickVal, pCountPtr = pCountVal, ix = 0; 
	 ix < (nslices << 1); ix++, pTickPtr++, pCountPtr++)
	{
	*pTickPtr += *pCountPtr * sysTimestampPeriod();
	}

    /* convert to relative ticks and compute percent error */

    printf ("\ncalculated ticks: %u\n", calcTicks);

    pTickPtr = pTickVal;

    for (ix = 0, pCountPtr = pCountVal; ix < nslices; ix++, pCountPtr++)
	{

	/* the following two lines put the difference between two consecutive
	 * entries in the pTickVal array into the appropriate slot in the 
	 * pCountVal array.
	 */

	*pCountPtr = *pTickPtr++;
	*pCountPtr = *pTickPtr++ - *pCountPtr;

	/* calculate *percent* error */
	errorVal = 100 * ((float) calcTicks - (float) *pCountPtr) /
			  (float) calcTicks;

	printf ("Period %d:  measured ticks: %u,  percent error: %f\n",
	    (ix + 1), *pCountPtr, errorVal);

        if (errorVal < 0)
	    errorVal *= -1;
        if (errorVal > (float) PK_TS_ERR)
	    retValue = ERROR;
        }

    if (retValue == ERROR)
    	printf("FAIL: long-term timestamp test\n");
    else
    	printf("PASS: long-term timestamp test\n");

pkTimestampTestLongExit:
    free (pCountVal);
    free (pTickVal);
    }

#endif	/* INCLUDE_TIMESTAMP */

#ifdef INCLUDE_SCSI

/* the following are for testing SCSI */

/*******************************************************************************
*
* pkBufFillPattern - write a pattern to a memory buffer
*
* This routine write a pattern in a memory buffer.  <startPattern> indicates
* the
* pattern and the buffer will be fill with data0, data0+inc,....,datan+inc
* according to the filling law chosen.
*
* INTERNAL:
* Only the law with the increment 0x01010101 is implemented.
* A another law should be to write:
* data0, ~data0,data0+inc,~data0+inc, ...,datan+inc,~datan+inc.
*
* RETURNS: OK or ERROR.
*/

STATUS pkBufFillPattern
    (
    UINT8 *pBuffer,		/* pointer to the buffer              */
    UINT32 size,		/* size in bytes of the buf to fill   */
    int law,			/* rules to fill the buffer           */
    int *startPattern,		/* value of the start pattern         */
    int *endPattern		/* where return the end value pattern */
    )
    {
    EQUI_INT_UINT8  pattern;
    int 	ix;
    int 	iy;
    int 	firstBlock;
    int 	lastBlock;
    int *	pt = (int *)pBuffer;
    UCHAR *	upt;

    if (pBuffer == NULL)
	return (ERROR);
    if (size < sizeof (pattern.ival))
	return (ERROR);
    switch (law)
	{
	case PATTERN_INC_LAW:
	    {
	    /* incremental law */

	    iy = 0;
	    pattern.ival = *startPattern;
	    firstBlock = size / sizeof (pattern.ival);
	    lastBlock = size % sizeof (pattern.ival);

	    for (ix = 0; ix <= (firstBlock-1); ix ++)
		{
		*pt = pattern.ival;
		pattern.ival = pattern.ival + (int) 0x01010101;
		pt ++;
		}

	    /* fill last block in byte mode if the remain modulo exist */

	    ix = (ix * 4) - 1;

	    upt = (UCHAR *) pt;

	    while (lastBlock != 0)
		{
		 *upt = pattern.uint8val[ix % 4];

		if ((iy % 4) == 3)
		    pattern.ival = pattern.ival + 0x01010101;
		ix ++;
		iy ++;
		upt ++;

		lastBlock --;
		}
	    break;
	    }
	default:
	    {
	    printErr ("pkBufFillPattern: law not implemented : ERROR\n");
	    return (ERROR);
	    }
	}

    *endPattern = pattern.ival;
    return (OK);
    }

/*******************************************************************************
*
* pkBlkWrite - write blocks to a SCSI device
*
* Write a buffer to a SCSI device.  The buffer size must be a multiple
* of the block size of the device.  <blockNumber> is used both to describe
* the writing position and next free position when the function returns.
*
* RETURNS: OK, or ERROR if parameters don't match or the write failed.
*/

STATUS pkBlkWrite
    (
    SCSI_BLK_DEV *pBlkDev,	/* pointer to the block device     */
    char *pBuffer,		/* pointer to the buffer Read      */
    UINT32 size,		/* size of the pBuffer area        */
    int *blockNumber		/* logical # of the block to write */
    )
    {
    int numSects;
    int secSize = BLK_TO_PHYS(pBlkDev,blockSize);

    /* check pBlkDev */

    if  (pBlkDev == (SCSI_BLK_DEV *)NULL)
	{
	printErr ("pkBlkWrite: pBlkDev is NULL : ERROR\n");
	return (ERROR);
	}

    /* check for range block */

    if (((* blockNumber) > ((SCSI_BLK_DEV *) pBlkDev)->blkDev.bd_nBlocks) ||
        ((* blockNumber) < ((SCSI_BLK_DEV *) pBlkDev)->blockOffset))
	{
	printErr ("scsiWriteBlocks: blockNumber out of range : ERROR\n");
	return (ERROR);
	}

    /* check if size ok */

    if (size == (int) NULL)
	{
        printErr ("scsiWriteBlocks: buffer size is NULL : ERROR\n");
	return (ERROR);
	}

    /* check if buffer size fits sector size */

    IF_BAD_BUFSIZE (pBlkDev,size)
	{
        printErr ("scsiWriteBlocks: buffer size invalid : ERROR\n");
	return (ERROR);
	}

    numSects = size / secSize;

    if (scsiWrtSecs (pBlkDev,*blockNumber,numSects, pBuffer) == ERROR)
	{
	printErr ("pkBlkWrite: returned ERROR\n");
	return (ERROR);
	}
    *blockNumber = *blockNumber + numSects;
    return (OK);
    }

/*******************************************************************************
*
* pkBlkRead - read blocks from a SCSI device
*
* Read a buffer from a SCSI device.  The buffer size must be a multiple
* of the block size of the device.  <blockNumber> is used both to describe
* the reading position and next free position when the function returns.
*
* RETURNS: OK, or ERROR if parameters don't match or the read failed.
*/

STATUS pkBlkRead
    (
    SCSI_BLK_DEV *pBlkDev,	/* pointer to the block device     */
    char *pBuffer,		/* pointer to the buffer Read      */
    UINT32 size,		/* size of the pBuffer area        */
    int *blockNumber		/* logical # of the block to write */
    )
    {
    int numSects;
    int secSize = BLK_TO_PHYS(pBlkDev,blockSize);

    /* check pBlkDev */

    if  (pBlkDev == (SCSI_BLK_DEV *) NULL)
	{
	printErr ("pkBlkRead: pBlkDev is NULL : ERROR\n");
	return (ERROR);
	}

    /* check for range block */

    if (((* blockNumber) > ((SCSI_BLK_DEV *) pBlkDev)->blkDev.bd_nBlocks) ||
        ((* blockNumber) < ((SCSI_BLK_DEV *) pBlkDev)->blockOffset))
	{
	printErr ("pkBlkRead: blockNumber out of range : ERROR\n");
	}

    /* check if size ok */

    if (size == 0)
        {
        printErr ("pkBlkRead: buffer size is 0 : ERROR\n");
        return (ERROR);
        }

    /* make sure the buffer size fits the sector size */

    IF_BAD_BUFSIZE (pBlkDev,size)
	{
        printErr ("pkBlkRead: buffer size not a multiple of block size : ERROR\n");
	return (ERROR);
	}

    /* use previous IF_BAD_BUFSIZE to get an integer sector count */

    numSects = size / secSize;

    if (scsiRdSecs (pBlkDev,*blockNumber,numSects, pBuffer) == ERROR)
	{
	printErr ("pkBlkRead: returning ERROR\n");
	return (ERROR);
	}

    *blockNumber = *blockNumber + numSects;
    return (OK);
    }

/*******************************************************************************
*
* pkTestUnit - test a SCSI device by writing, reading and verifying data
*
* This routine tests write and read operation to a SCSI device.
* The unique parameter is pointer to a structure info.  First it writes
* a pattern to a range of blocks on the device.  Then it reads the range
* of blocks and verifies the pattern.  The pattern is generated by
* pkBufFillPattern().
*
* RETURNS: OK, or ERROR if the parameters are bad or the test fails.
*
* SEE ALSO: pkBufFillPattern()
*/

STATUS pkTestUnit
    (
    RW_TEST_UNIT *pTestInfo 	/* structure describes test to run */
    )
    {
    FAST RW_TEST_UNIT *		p = pTestInfo;
    FAST SCSI_BLK_DEV *		pB = (SCSI_BLK_DEV *) (p->pBlk);
    int iWrite;
    int iRead;
    int iLoop;
    int temp;
    int curEndPattern;

    /* check parameters */

    if (pB == (SCSI_BLK_DEV *) NULL)
	{
        printErr ("pkTestUnit: pBlkDev is NULL : ERROR\n");
        return (ERROR);
	}

    if (p->nbLoop == 0)
	{
        printErr ("pkTestUnit: nbLoop is 0 : ERROR\n");
        return (ERROR);
	}

    if (p->size == 0)
	{
        printErr ("pkTestUnit: bufSize is 0 : ERROR\n");
        return (ERROR);
	}

    if (p->nbBuf == 0)
	{
        printErr ("pkTestUnit: nbBufWrite is 0 : ERROR\n");
        return (ERROR);
	}

    IF_BAD_BUFSIZE(pB, p->size)
	{
        printErr ("pkTestUnit: bufSize not a multiple of block size : ERROR\n");
	return (ERROR);
	}

    /* check if enough place on the device for a cycle */

    temp = ((p->size / BLK_TO_PHYS(pB, blockSize)) *  p->nbBuf);

    if ((temp > (pB->blkDev.bd_nBlocks)) ||
	((temp + p->stBlk) > (pB->blkDev.bd_nBlocks + pB->blockOffset)))
	{
        printErr ("pkTestUnit: not enough room for a cycle : ERROR\n");
	return (ERROR);
	}

    /* allocate buffers */

    if ((p->ptR = (UINT8 *) malloc(p->size)) == NULL)
	{
        printErr ("pkTestUnit: ptRead allocation failed : ERROR\n");
	return (ERROR);
	}

    if ((p->ptW = (UINT8 *) malloc(p->size)) == NULL)
	{
        printErr ("pkTestUnit: ptWrite allocation failed : ERROR\n");
        free (p->ptR);
	return (ERROR);
	}

    /* main loop on nbLoop */

    p->curStartPattern = p->startPattern;
    p->countErr = 0;

    for (iLoop = 0; iLoop < p->nbLoop; iLoop ++)
	{
	p->curBlk = p->stBlk;
	p->curPattern = p->curStartPattern;

	/* refresh curCycle for spy */

        p->curCycle = iLoop;

	/* check for abort */

	if (p->abort == TRUE)
	   return (OK);

	/* write loop */

        for (iWrite = 0; iWrite < p->nbBuf; iWrite ++)
	    {
	    if (iWrite == 0)
		{
		printf ("pkTestUnit: beginning write loop\n");
		}
	    if (iWrite == p->nbBuf / 2)
		{
		printf ("pkTestUnit: half way through write loop\n");
		}

	    /* check for abort */

	    if (p->abort == TRUE)
               return (OK);

	    /* fill the buffer with pattern according to the law */

	    if ((pkBufFillPattern (p->ptW, p->size, p->law,
		                     &p->curPattern,
			             &curEndPattern)) == ERROR)
                {
		printErr ("pkTestUnit: fatal fill buffer error : ERROR\n");
		return (ERROR);
		}

	    if (pkBlkWrite (pB, (char *) p->ptW, p->size, &p->curBlk) == ERROR)
                {
		printErr ("pkTestUnit: fatal write error : ERROR\n");
		return (ERROR);
		}

            (p->curPattern) ++;
            }

	p->curBlk = p->stBlk;
	p->curPattern = p->curStartPattern;

	/* read loop */

        for (iRead = 0; iRead < p->nbBuf; iRead ++)
	    {
	    if (iRead == 0)
		{
		printf ("pkTestUnit: beginning read loop\n");
		}
	    if (iRead == p->nbBuf / 2)
		{
		printf ("pkTestUnit: half way through read loop\n");
		}

	    /* check for abort */

	    if (p->abort == TRUE)
		{
		return (OK);
		}

	    if ((pkBufFillPattern (p->ptW, p->size, p->law,
			             &p->curPattern, &curEndPattern)) == ERROR)
                {
		printErr ("pkTestUnit: fatal fill buffer error : ERROR\n");
		return (ERROR);
		}

	    if (pkBlkRead (pB, (char *) p->ptR , p->size, &p->curBlk) == ERROR)
                {
		printErr ("pkTestUnit: fatal read error : ERROR\n");
		return (ERROR);
		}

           /* compare buffers */

	   if (bcmp ((char *) p->ptR,(char *) p->ptW, p->size) != 0)
	       {
	       (p->countErr) ++;

	       printErr ("    Loop  :0x%08x\n", iLoop);
	       printErr ("    Buffer:0x%08x\n", iRead);
               printErr ("pkTestUnit: buffer compare error : ERROR\n");

	       /* check for abort on Error */

	       if (p->abortOnError == TRUE)
		   return (OK);
	       }

            p->curPattern ++;

            } /* nbBufRead */

        /* inc start pattern for the next cycle */

	p->curStartPattern ++;

        } /* nbLoop */

    /* release resources */

    free (p->ptR);
    free (p->ptW);

    return (p->countErr);
    }

/*******************************************************************************
*
* pkTestOneUnit - test one SCSI unit
*
* This routine creates a SCSI device as specified by <devId> and <devLun> then
* calls pkTestUnit() to perform a read/write test on the device.
* Because it uses a global RW_TEST_UNIT structure it is not reentrant.
*
* If <bufSize> = 0, use default of DEF_BUF_SIZE.
* If <nbBuf>   = 0, use default of DEF_NB_BUF.
* If <nbLoop>  = 0, write/read entire device.
*
* NOTE:
* Warning!  This routine calls scsiPhysDevCreate() and scsiBlkDevCreate().
* All data on the file system will be erased.
*
* RETURNS: OK, or ERROR if test fails.
*
* SEE ALSO: pkTestUnit()
*/

STATUS pkTestOneUnit
    (
    int devId,			/* scsi id of the device */
    int devLun,			/* lun id of the devicer */
    UINT32 bufSize,		/* buffer size to use */
    int nbBuf,			/* number of buffers to write */
    int nbLoop			/* number of loops to perform */
    )
    {
    SCSI_PHYS_DEV *pPhysDev;
    BLK_DEV *pBlkDev;
    STATUS status;

    /* create physical SCSI device */

    if ((pPhysDev=scsiPhysDevIdGet(0,devId,devLun))==(SCSI_PHYS_DEV *)NULL)
        if ((pPhysDev=scsiPhysDevCreate(pSysScsiCtrl,devId,devLun,0,NONE,0,0,0))         == (SCSI_PHYS_DEV *) NULL)
            {
	    printErr ("pkTestOneUnit: scsiPhysDevCreate() failed : ERROR\n");
	    return (ERROR);
	    }

    /* create logical SCSI device */

    if ((pBlkDev = scsiBlkDevCreate (pPhysDev, 0, 0)) == NULL)
	{
        printErr ("pkTestOneUnit: scsiBlkDevCreate() failed : ERROR\n");
	return (ERROR);
        }

    /* fill in the RW_TEST_UNIT information */

    pTestUnit->pBlk = pBlkDev;
    pTestUnit->size = (bufSize == 0) ? DEF_BUF_SIZE : bufSize;
    pTestUnit->nbBuf = (nbBuf == 0) ? DEF_NB_BUF : nbBuf;
    if (nbLoop == 0)
        {
        /* entire device */

        UINT32 nbblock       = ((SCSI_BLK_DEV *) pBlkDev)->blkDev.bd_nBlocks;
        UINT32 blsize        = BLK_TO_PHYS (pBlkDev, blockSize);
        pTestUnit->nbBuf  = nbblock / (pTestUnit->size / blsize);
        pTestUnit->nbLoop = 1;
        }
    else
        pTestUnit->nbLoop = nbLoop;

    pTestUnit->startPattern = ST_PATTERN;	/* start pattern */
    pTestUnit->law          = DEF_LAW;		/* fill law */
    pTestUnit->abortOnError = TRUE;		/* set abort on data error */
    pTestUnit->abort        = FALSE;		/* disable abort flag */

    /* call pkTestUnit() */

    status = (pkTestUnit (pTestUnit) || pTestUnit->countErr) ? ERROR : OK;

    /* releases resources and return */

    scsiPhysDevDelete (pPhysDev);

    if (status == OK)
        printf("PASS\n");
    else
        printf("ERROR\n");

    return (status);
    }

#endif /* INCLUDE_SCSI */

/*******************************************************************************
*
* pkPrintTest - print the given string to the console.
*
* This routine prints the given string.
*
* RETURNS: N/A
*/

void pkPrintTest
    (
    char *mesg,		/* message to be printed */
    int delayTime	/* task delay if required */
    )
    {
    taskDelay(delayTime);
    printf("%s\n",mesg);
    }


/*******************************************************************************
*
* pkPrintTest1 - print the given string to the console.
*
* This routine prints the given string.
*
* RETURNS: N/A
*/

void pkPrintTest1
    (
    int fd,
    char *mesg,		/* message to be printed */
    int delayTime	/* task delay if required */
    )
    {
    taskDelay(delayTime);
    write(fd, mesg, strlen(mesg));
    }

/*******************************************************************************
*
* pkDivideZeroTest - used to check for divide by zero exception.
*
*
* RETURNS: N/A
*/

void  pkDivideZeroTest (void)
      {
      int a;
      int b=5;
      int c=0;

      taskDelay(200);
      a=b/c;

      printf("The value of a is %d\n", a);
      }

/******************************************************************************
*
* pkUdpEchoT - main routine to open sockets, send and receive packets
*
* This routine opens an internet socket with a remote target, which is assumed
* to be running a UDP echo server.  Once connected, a loop is entered where
* UDP packets of differing sizes are sent to, and received from, the target.
* Each returning packet is checked for the correct size and content.  A
* packet sent, but not received back within the timeout may be resent by
* alarm_handler().  The test passes, exiting with value 0, if all packets
* were sent and received, regardless of how many individual packets
* needed to be resent. The IP address of the remote target has to be supplied
* with all individual bytes as seperate arguments.
*
* RETURNS: exit status: 0 if OK
*/

void pkUdpEchoT
    (
    int addr1,  /* first byte in the IP address */
    int addr2,
    int addr3,
    int addr4,  /* last byte in the IP address */
    int	    packetsToSend,
    int	    timeout
    )
    {
    int    div;				/* divisor for status messages */
    int    tenth = 0;			/* status message counter */
    int	   ix, iy;                      /* loop counters */
    int    cnt;				/* count of bytes sent/received */
    int    prevPktSiz = -99;		/* packet size just sent */
    int    localSktAddrLen;		/* local copy of sktAddrLen */
    short  *pShort;                     /* counter to form output buffer */
    struct sockaddr_in cli_addr;        /* host socket data */
    int    sktBufSiz;
    struct fd_set readFs;
    int	   ret;
    struct timeval pktTimeout;
    u_long  inetAdrs;
    char inetDotAdrs[60];

    sprintf(inetDotAdrs, "%d.%d.%d.%d", addr1, addr2, addr3, addr4);

    inetAdrs = inet_addr(inetDotAdrs);

    /* convert command line args */

    if (packetsToSend < 1)
	{
	printf ("Error: no packets to send\n");
	fflush (stdout);
	exit (9);
	}

    /* check if inet address is non-null */

    if (inetAdrs == 0)
        {
        printf ("Error: illegal inet address\n");
        exit (9);
        }

    /* turn off timer if bad arg */

    packetTMO = timeout;
    if (packetTMO < 0)
	packetTMO = 0;

    pktTimeout.tv_sec  = 0;
    pktTimeout.tv_usec = packetTMO * 1000;

    /* open the socket */

    skt = socket (AF_INET, SOCK_DGRAM, 0);
    if (skt < 0)
        {
        printf ("Error: socket() failure\n");
	fflush (stdout);
        exit (2);
        }

    /* prepare our sock addr */

    sktAddrLen = sizeof (struct sockaddr_in);
    (void) memset (&cli_addr, 0, sktAddrLen);           /* clr it */
    cli_addr.sin_family = AF_INET;                      /* fill in family */
    cli_addr.sin_port = 0;                              /* port */


    /* bind our addr to the socket */

    if (bind (skt, (struct sockaddr *) &cli_addr, sktAddrLen) < 0)
        {
        printf ("Error: bind() failure\n");
	fflush (stdout);
        close (skt);
        exit (3);
        }

    /* specify max buffer size on send/receive */

    sktBufSiz = sizeof (buf);         /* increase size of socket buffers */

    if (setsockopt (skt, SOL_SOCKET, SO_SNDBUF, (char *) &sktBufSiz,
        sizeof (sktBufSiz)) < 0)
        {
        close (skt);
        printf ("Error: setsockopt() failure\n");
	fflush (stdout);
        exit (3);
        }

    if (setsockopt (skt, SOL_SOCKET, SO_RCVBUF, (char *) &sktBufSiz,
        sizeof (sktBufSiz)) < 0)
        {
        close (skt);
        printf ("Error: setsockopt() failure\n");
	fflush (stdout);
        exit (4);
        }

    /* prepare server's sock addr */

    (void) memset (&serv_addr, 0, sktAddrLen);          /* clr it */
    serv_addr.sin_family = AF_INET;                     /* fill in family */
    serv_addr.sin_addr.s_addr = inetAdrs;   		/* get IP addr */
    serv_addr.sin_port = htons(IPPORT_ECHO);            /* the ECHO port */

    /*
     * Prepare the outgoing data.
     * The buffer is treated as an array of short integers,
     * each is sequentially numbered, starting from zero.
     */

    pShort = (short *) dataOut;
    for (ix = 0; ix < (MAX_PKT/sizeof (short)); ix++)
        *pShort++ = ix;

    /* Main loop.  Send a packet and wait for it to come back */

    div=packetsToSend/10;
    printf("Sending %d packets\n", packetsToSend);
    fflush (stdout);


    for (packetNum = 0; packetNum < packetsToSend; packetNum++)
        {
        /* print status message */

        if (div > 0)
	    if ((packetNum % div) == (div -1))
	        printf ("%d/10 through UDP echo test\n", ++tenth);
		fflush (stdout);

        curPktSiz = pktSizes [packetNum % N_SIZES];

        /* send a packet */

        cnt = sendto (skt, dataOut, curPktSiz, 0,
		      (struct sockaddr *) &serv_addr, sktAddrLen);

        /* check that entire packet was sent */

        if (cnt != curPktSiz)
            {
            printf ("Error: could not send packet #%d, size %d\n",
		    packetNum, curPktSiz);
	    fflush (stdout);
            exit (4);
            }

        /* expect it back - clear out previously resent packets */

        do {

            FD_ZERO (&readFs);
            FD_SET (skt, &readFs);

            ret = select (skt +1, &readFs, NULL, NULL, &pktTimeout);
            if (ret == 0)
               {
    	       if (resendCnt++ == RESEND)
	           {
	           /* already resent specified times - fail */

                   close (skt);
	           printf ("Error: timed out waiting for packet #%d\n",
	                    packetNum);
                   printf ("UDP echo test failed\n");
                   fflush (stdout);
	           exit (7);
	           }
               else
                   {
	           resendTotal ++;   /* bump counter */
                   printf("Timed out Resending packet:%d trial:%d\n",
                             packetNum, resendCnt);
                   fflush (stdout);
                   cnt = sendto (skt, dataOut, curPktSiz, 0,
	         	      (struct sockaddr *) &serv_addr, sktAddrLen);

                   /* check that entire packet was sent */

                   if (cnt != curPktSiz)
                      {
                      printf ("Error: could not send packet #%d, size %d\n",
		               packetNum, curPktSiz);
	              fflush (stdout);
                      exit (4);
                      }
                   }
               }
            else if (ret == ERROR)
               {
               printf ("Error: select failed\n");
               close (skt);
               exit(9);
               }
            else
               {
               localSktAddrLen = sktAddrLen;
               cnt = recvfrom (skt, dataIn, curPktSiz, 0,
			    (struct sockaddr *) &serv_addr, &localSktAddrLen);
	       }

	} while ((cnt == prevPktSiz) || (cnt == -1));

        resendCnt = 0;

	/* check that packet received is correct size */

        if (cnt != curPktSiz)
            {
            printf ("Error: remote host echoed wrong size in packet #%d\n",
		    packetNum);
	    fflush (stdout);
	    close (skt);
            exit (5);
            }

        /* verify the received data */

        for (iy = curPktSiz; iy--;)
            {
            if (dataIn [iy] != dataOut [iy])
                {
                printf ("Error: echoed data does not match in packet #%d\n",
			packetNum);
		close (skt);
	        fflush (stdout);
                exit (6);
                }
            }

        prevPktSiz = curPktSiz;
        }

    /* print exit messages - test passed */

    printf ("\n\nTotal packets resent: %d\n", resendTotal);
    printf ("UDP echo test passed\n");
    printf ("END OF UDP TEST\n");
    fflush (stdout);
    close (skt);
    exit (0);
    }

/*******************************************************************************
*
* pkIntGet - read an int at a given address
*
* RETURNS: content of address.
*
* SEE ALSO: pkTestSet()
*/
int pkIntGet
   (
   int	*pMem
   )
   {
   return *pMem;
   }

/*******************************************************************************
*
* pkIntSet - store an int at a given address
*
* RETURNS: N/A
*
* SEE ALSO: pkTestGet()
*/
void pkIntSet
   (
   int	*pMem,
   int	val
   )
   {
   *pMem = val;
   }

/*******************************************************************************
*
* pkPing - perform a call to ping()
*
*
* RETURNS: N/A
*
*/
void pkPing
   (
   int	addrByte0,	/* most significant byte of IP address */
   int  addrByte1,
   int  addrByte2,
   int  addrByte3,	/* least sigificant byte of IP address */
   int  numPkts,	/* number of packets to send */
   int	options
   )
   {
   char  buffer[60];

   sprintf(buffer, "%d.%d.%d.%d", addrByte0, addrByte1, addrByte2, addrByte3);
   ping(buffer, numPkts, options);
   }

/*******************************************************************************
*
* pkBusErrorTest - performs a string comparison with an invalid address to cause
*                  bus error exception
*
*
* RETURNS: returns 0 or 1 depending on string compare result otherwise should
*           cause a bus error
*
*/


int pkBusErrorTest
    (
    char * str2
    )
    {
    char * str1 = "This is a sample string" ;

    for(; *str1 == *str2 ; str1++ , str2++)
        if (*str1 == '\0')
        return 0 ;

    return 1 ;

    }

/*****************************************************************************
*
* pkPrintSerialOutput - calls pkPrintTest() given the count.
*
*
* RETURNS: N/A
*
*/

void pkPrintSerialOutput
    (
    int count
    )
    {
    int i;

    for(i=0; i<count; i++)
        {
        pkPrintTest("abcdefghijklmnopqrstuvwxyz" , 1) ;
        }
    }

/*****************************************************************************
*
* pkSerialTestInit - initializes the console settings.
*
*
* RETURNS: N/A
*
*/

void pkSerialTestInit()
    {
    int term1, term2;

    term1 = ioGlobalStdGet(0);
    term2 = ioGlobalStdGet(1);
    (void) ioctl (term1, FIOSETOPTIONS, OPT_TERMINAL);
    (void) ioctl (term2, FIOSETOPTIONS, OPT_TERMINAL);
    }

/*****************************************************************************
*
* pkSerialTest - scans console for input strings and stores them in a global
*                array
*
*
* RETURNS: N/A
*
*/

void pkSerialTest()
    {

    int i=0;

    while (i < 15)
        {
        if (pkSerChanAbort)
            break ;
        scanf("%s",pkSerChanInput[i]) ;
        i++ ;
        }

    }

/*****************************************************************************
*
* pkSerChanOutput - prints the contents of the global array received by
*                   pkSerialTest()
*
*
* RETURNS: N/A
*
*/


void pkSerChanOutput
    (
    int index
    )
    {
    int term;

    term = ioGlobalStdGet(1);

    if (index < 15)
        {
	write(term, pkSerChanInput[index], strlen(pkSerChanInput[index]));
	}
    }

/*****************************************************************************
*
* pkCatastrophicTest - performs catastrophic test given an error address.
*
* This functions connects 'strcmp' routine to the system clock interrupt
* with an invalid address as the argument. This should cause a catastrophic
* error on the target and the target should reboot with an exception.
*
* RETURNS: ERROR if no exception is generated.
*
*/


STATUS pkCatastrophicTest
    (
    UINT32 obErrAdrs
    )
    {

    sysClkDisable();
    if ((sysClkConnect((FUNCPTR) strcmp, obErrAdrs) == ERROR))
        return(ERROR);

    sysClkEnable();

    return(ERROR);
    }


/******************************************************************************
*
* pkCatastrophicClean - clean-up for pkCatastrophicTest() routine.
*
* This functions does clean-up if pkCatastrophicTest() function fails and
* ensures proper operation of system clock.
*
* RETURNS: N/A
*
*/

VOID pkCatastrophicClean ()
    {
    sysClkDisable();
    sysClkConnect((FUNCPTR) tickAnnounce, 0);
    sysClkEnable();
    }

