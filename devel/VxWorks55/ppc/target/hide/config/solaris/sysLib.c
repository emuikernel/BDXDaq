/* sysLib.c - UNIX-target system dependent library */

/* Copyright 1984-2001 Wind River Systems, Inc. */
#include "copyright_wrs.h"

/*
modification history
--------------------
01f,08jan03,jmp  fixed simulator memory allocation error message (SPR #4027).
01e,08apr02,jmp  added sysBootParams initialization if INCLUDE_NET_INIT and
                 INCLUDE_BOOT_LINE_INIT are not defined (SPR #75207).
02j,08nov01,jmp  no longer use tyCoDrv. added support for PPP on solaris 2.9.
02i,09oct01,hbh  Fixed download of symbol table (SPR #70334).
02h,05oct01,hbh  Fixed initialization of tyCodv (SPR 27291).
02g,09aug01,hbh  Modified solaris_ppp_init() to use sleep() instead of
                 taskDelay() - SPR 29081.
		 Got rid of compilation warnings.
		 Removed warning messages in case NUM_TTY > 1 when PPP
		 not included.
		 Removed ULIP references.
		 Code cleanup.
02f,13may98,dbt  copy sysBootLineMagic in sysBootLine if network is not 
		 initialized to make boot line available to the WDB agent.
		 updated copyright.
02e,21feb97,dat  removed NOMANUAL from sysHwInit2.
02d,06feb97,ms   merged Mark Dapoz's code for PPP networking (sans ULIP).
02c,28may96,ism  moved sysPhysMemTop() above sysHWInit().
02b,22may96,dat  added sysPhysMemTop(), sysBspRev(), removed sysMemBot()
 		 removed sysMemSize. changed sysMemTop().
02a,13jun95,ism  modifications for solaris simulator
01o,21dec94,rhp  man-page formatting tweaks
01n,21feb94,gae  added proc# to tyCo/xterm title. Added sysBusTasClear for HP.
01m,11feb94,gae  fixed HP version of nvram; added sm support.
01l,29jan94,gae  sysNvRam*() checks $HOME if no r/w permission locally.
01k,26jan94,gae  fixed bootline calculation for passFs.
01j,14dec93,gae  moved tyCoDv[] initialization here; fixed linking in of dosFs.
		 made nvram proc# specific, and allowed CR at end of nvram.
		 made aux clock more predictable by deriving from sys clock;
		 made sysclock use virtual and real timer to improve accuracy.
01i,24aug93,gae  documentation tweaks.
01h,30jul93,gae  fixed sysNvRamGet().
01g,08jul93,gae  fixed bootline calculation when not using passFs.
01f,29jan93,gae  1.1-ized.
01e,12aug92,gae  doc tweaks.
01d,05aug92,gae  renamed idle task tIdle.
01c,29jul92,gae  cleanup.
01b,22apr92,gae  WRS revisions.
01a,22feb92,smeg written.
*/

/*
DESCRIPTION
This library provides board-specific routines for the VxWorks simulator.

INCLUDE FILES: sysLib.h

SEE ALSO:
.pG "Configuration"
*/

/* includes */

#include "vxWorks.h"
#include "taskLib.h"
#include "memLib.h"
#include "vxLib.h"
#include "sysLib.h"
#include "config.h"
#include "intLib.h"
#include "iv.h"
#include "string.h"
#include "dosFsLib.h"
#include "stdio.h"
#include "u_Lib.h"
#include "simLib.h"
#include "string.h"
#include "unixSio.h"

/* defines */

#define timerclear(tvp)	(tvp)->tv_sec = (tvp)->tv_usec = 0

#define U_ITIMER_REAL   	0
#define U_ITIMER_VIRTUAL	1
#define U_ITIMER_PROF		2

#define MAX_FD	256 	/* reported value defined in simLib.c file */

#define	SYS_VIRTUAL_TIME 0 /* 1=sys is virtual, 0=strict realtime */
#define	AUX_VIRTUAL_TIME 0 /* 1=aux is virtual, 0=derived from sys */

#define	MILLION	1000000

#define	NV_FILENAME	"%s.nvram%d"
#define	NV_UFILENAME	"%s/%s.nvram%d"

#define OPEN_MODE    	0x0302 /*O_RDWR|O_CREAT|O_TRUNC*/

#define	IPC_RMID	0	/* remove identifier */
#define	IPC_CREAT	0001000	/* create entry if key doesn't exist */

#ifdef	INCLUDE_PPP

/* definitions and structures from Solaris header files */

#define STR             ('S'<<8)
#define I_PUSH          (STR|02)
#define I_SENDFD        (STR|021)

#define FIFO_PATH "/tmp/.asppp.fifo"
#endif /* INCLUDE_PPP */

/* typedefs */

#ifdef INCLUDE_PPP
typedef struct
    {
    u_long  msg;
    char    uname[16];
    } uname_t;

typedef struct
    {
    u_long  msg;
    } restart_t;

typedef struct
    {
    u_long  msg;
    int     debug_level;
    } debug_t;

struct strbuf
    {
    int     maxlen;                 /* no. of bytes in buffer */
    int     len;                    /* no. of bytes returned */
    char    *buf;                   /* pointer to data */
    };

enum fifo_msg_codes
    {
    FIFO_UNAME = 0x200,
    FIFO_RESTART,
    FIFO_DEBUG
    };

union fifo_msgs
    {
    u_long          msg;
    uname_t         uname;
    restart_t       restart;
    debug_t         debug;
    }; 
#endif /* INCLUDE_PPP */

/* globals */

char vxsim_hostname [80];
char vxsim_cwd [80];

#if	defined(INCLUDE_PPP)
char * vxsim_bootdev = "ppp";		/* boot device is PPP		*/
#elif defined (INCLUDE_PASSFS)
char *vxsim_bootdev = "passDev";
#else	
char * vxsim_bootdev = "";
#endif	/* INCLUDE_PPP */

char * vxsim_ip_name = "vxworks%d";

int	sysCpu      = CPU;             	/* system CPU type */
char 	sysBootLineAdrs[256];		/* no wellknown location */
char *	sysBootLine = sysBootLineAdrs;	/* address of boot line */

char 	sysExcMsgAdrs[256];		/* no wellknown location */
char *	sysExcMsg = sysExcMsgAdrs;	/* catastrophic message area */
int   	sysProcNum;			/* processor number of this CPU */
int   	sysFlags;			/* boot flags */
char  	sysBootHost [BOOT_FIELD_LEN];	/* name of host from which we booted */
char  	sysBootFile [BOOT_FIELD_LEN];	/* name of file from which we booted */

int 	sysHardSqrt = FALSE;            /* Square Root Function Select */

char 	sysBootLineMagicAdrs[256];	/* automagically calculated */
char *	sysBootLineMagic = sysBootLineMagicAdrs;/* address of magic bootline */

char *	sysMemBaseAdrs;			/* start of MALLOC'd memory */
int   	sysMemSize = LOCAL_MEM_SIZE;	/* size of memory, (-r option) */

char *	sysSmAddr = 0x0;		/* SM_ANCHOR_ADRS */
int 	sysShmid;

int ulDebug = 100; /* debug switch for UNIX driver: 0=all, 5=errno, 100=none */
#ifdef	INCLUDE_PPP
int	pppPollPid = 0;
#endif

FUNCPTR sysCacheLibInit = (FUNCPTR) NULL;

/* 
 * For channel 0 (tyCoDv[0]), u_fd field will not be modified after this 
 * initialization and has a value of 0. Channel 1 will be modified only if  
 * PPP is included. 
 * So to know if channel 1 is used or not, we initialize u_fd value to -1 
 * (different from tyCoDv[0].u_fd value which is 0).
 */
 
TYCO_DEV tyCoDv [] =	/* device descriptors */
    {
    {{{{NULL}}}, FALSE, 0, 0},	/* std in/out/err */
    {{{{NULL}}}, FALSE, -1, 0},	/* xterm device */
    };

/* locals */

LOCAL int	sysClkTicksPerSecond	= 20;
LOCAL BOOL 	sysClkRunning        	= FALSE;
LOCAL FUNCPTR 	sysClkRoutine        	= NULL;
LOCAL int 	sysClkArg		= 0;
LOCAL BOOL 	sysClkConnected       	= FALSE;
LOCAL int 	auxClkTicksPerSecond  	= 100;
LOCAL BOOL 	sysAuxClkRunning      	= FALSE;
LOCAL FUNCPTR 	sysAuxClkRoutine   	= NULL;
LOCAL int 	sysAuxClkArg		= 0;
LOCAL BOOL 	auxClkConnected       	= FALSE;

/* function declarations */

extern char *  u_progname;  /* home of executable */
extern char ** u_argvsave;

extern int	PUTMSG();
extern int	PIPE();
extern char * 	rindex();
extern int 	u_shmctl ();
extern int 	u_shmget ();
extern char *	u_shmat ();
extern int 	u_printf ();
extern int 	s_dosTimeHook ();
extern void 	s_userGet ();
extern int 	u_sigsuspendflag;	/* "virtual time" suspended */

/* forward declarations */

LOCAL void sysAuxClkInt ();

#include "sysSerial.c"

/***************************************************************************
*
* sysModel - return model name of the system CPU
*
* Use this routine to find the model name of the system CPU.
* 
* RETURNS: A pointer to a string in the form "<OS> [<machine>]", where
* <OS> represents the operating system name and version number, and
* <machine> describes the host hardware.
*/

char * sysModel (void)
    {
    LOCAL char	name [500] = "";
    char 	sysname [100];
    char 	nodename [100];
    char 	release [100];
    char 	version [100];
    char 	machine [100];

    if (strlen (name) == 0)
	{
	s_uname (sysname, nodename, release, version, machine);
	sprintf (name, "%s %s [%s]", sysname, release, machine);
	}

    return (name);
    }

/***************************************************************************
*
* sysBspRev - return the bsp version/revision identification
*
* This function returns a pointer to a bsp version with the revision.
* for eg. 1.1/<x>. BSP_REV is concatanated to BSP_VERSION to form the
* BSP identification string.
*
* RETURNS: A pointer to the BSP version/revision string.
*/

char * sysBspRev (void)
    {
    return (BSP_VERSION BSP_REV);
    }

/***************************************************************************
*
* sysPhysMemTop - get the address of the top of physical memory
*
* This routine returns the address of the first missing byte of memory,
* which indicates the top of memory.
*
* Normally, the user specifies the amount of physical memory with the
* macro LOCAL_MEM_SIZE in config.h. BSPs that support run-time
* memory sizing do so only if the value of LOCAL_MEM_SIZE is zero.
* If not zero, then LOCAL_MEM_SIZE is assumed to be, and must be, the
* true size of physical memory.
*
* NOTE: Do no adjust LOCAL_MEM_SIZE to reserve memory for application
* use.  See sysMemTop() for more information on reserving memory.
*
* For this simulator BSP, the size of memory is kept in the variable
* sysMemSize and can be changed from the host command line with the
* -r option.  This is unique to the emulator BSPs.
*
* RETURNS: The address of the top of physical memory.
*
* SEE ALSO: sysMemTop()
*/

char * sysPhysMemTop (void)
    {
    LOCAL char * physTop = NULL;

    if (physTop == NULL)
	{
#if LOCAL_MEM_SIZE == 0

	/* If auto-sizing is possible, this would be the spot */
#	error "memory-sizing is not available (LOCAL_MEM_SIZE==0)"		

#else /* LOCAL_MEM_SIZE == 0 */

	/* Don't do autosizing, if size is given */
	
	physTop = (char *)(LOCAL_MEM_LOCAL_ADRS + sysMemSize);

#endif /* LOCAL_MEM_SIZE == 0 */
	}

    return physTop;
    }

/***************************************************************************
*
* sysHwInit - initialize the system hardware
*
* This routine initializes various features of the board.
* It is normally called from usrInit() in usrConfig.c.
*
* NOTE: This routine should not be called directly by the user.
*
* RETURNS: N/A
*/

void sysHwInit (void)
    {
    u_sigsuspendflag = 0;	/* XXX never suspend! */

#ifdef	INCLUDE_DOSFS
    dosFsDateTimeInstall (s_dosTimeHook);
#endif	/* INCLUDE_DOSFS */

    /* For simulators, we get our main memory from a malloc command */
    
    sysMemBaseAdrs = (char *)u_malloc (sysMemSize);

    if (sysMemBaseAdrs == NULL)
        {
	u_printf ("sysHwInit: Failed to allocate VxSim memory (%d bytes).\n",
		  sysMemSize);
        u_exit (-1);
        }

    /* 
     * If mmu tables are used, this is where we would dynamically
     * update the entry describing main memory, using sysPhysMemTop().
     * We must call sysPhysMemTop () at sysHwInit() time to do
     * the memory autosizing if available.
     */
     
    sysPhysMemTop ();

    sysSerialHwInit ();
    }

/***************************************************************************
*
* sysMemTop - get the address of the top of VxWorks memory
*
* This routine returns a pointer to the first byte of memory not
* controlled or used by VxWorks.
*
* The user can reserve memory space by defining the macro USER_RESERVED_MEM
* in config.h.  This routine returns the address of the reserved memory
* area.  The value of USER_RESERVED_MEM is in bytes.
*
* RETURNS: The address of the top of VxWorks memory.
*/

char * sysMemTop (void)
    {
    LOCAL char * memTop = NULL;

    if (memTop == NULL)
	{
	memTop = sysPhysMemTop () - USER_RESERVED_MEM;
	}

    return memTop;
    }

/***************************************************************************
*
* sysToMonitor - transfer control to the ROM monitor
*
* This routine transfers control to the ROM monitor.  Normally, it is called
* only by reboot()--which services ^X--and bus errors at interrupt level.
* However, in some circumstances, the user may wish to introduce a
* <startType> to enable special boot ROM facilities.
*
* RETURNS: Does not return.
*/

STATUS sysToMonitor
    (
    int startType
    )
    {
    int fd;

#ifdef	INCLUDE_PPP
    UNIX_CHAN *pChan = (UNIX_CHAN *) sysSerialChanGet (PPP_TTY);

    u_close (pChan->u_fd);

    if (pppPollPid)
    	{
	/*
	 * A process has been started to pool PPP connection for new packets
	 * (See sysSerial:sysSerialPppPoll()). Before rebooting or exiting
	 * this process must be killed.
	 */
	u_kill (pppPollPid, 9);	/* SIGKILL */
	u_usleep (500000);	/* let some time for the kill to be effective */
	}
#endif	/* INCLUDE_PPP */

    sysClkDisable ();
    sysAuxClkDisable ();
    s_fdint (0, 0);

    u_printf ("%s", &sysExcMsgAdrs); /* print exception message if present */

    for (fd = 3; fd < MAX_FD; fd++)
	(void)u_close (fd);

    if (sysProcNumGet () == 0)
	u_shmctl (sysShmid, IPC_RMID, 0); /* unmap memory */

    if (startType == BOOT_NO_AUTOBOOT)
	u_printf ("sysToMonitor: goodbye.\n");
    else
	{
	u_printf ("sysToMonitor: restarting.\n");
	u_execvp (u_argvsave [0], u_argvsave);
	u_printf ("sysToMonitor: unable to restart.\n");
	}

    u_exit (0);		/* don't return */
    return (ERROR);
    }

/***************************************************************************
*
* sysIntHandler - UNIX SIGINT handler
*/

LOCAL void sysIntHandler
    (
    int ch
    )
    {
    UNIX_CHAN * pChan = (UNIX_CHAN *) sysSerialChanGet(0);

    (*pChan->putRcvChar) (pChan->putRcvArg, ch);
    }

/***************************************************************************
*
* sysClkInt - handle system clock interrupts
*
* This routine handles system clock interrupts.
*/

LOCAL void sysClkInt
    (
    int src	/* 0=ALRM, 1=VTALRM */
    )
    {
    /*
     * When the WIND kernel goes into hard loop, it suspends the entire
     * process and allows ALRM to keep track of time.  Virtual time (VTALRM)
     * does not pass when a process is suspended -- which the WIND kernel
     * does whenever no tasks are runnable.
     */

#if SYS_VIRTUAL_TIME
    if (!u_sigsuspendflag)
	{
	/* process running: count virtual clock ticks, ignore realtime */
	
	if (src == 1)
	    {
	    if (sysClkRoutine != NULL)
		(*sysClkRoutine) (sysClkArg);
#if	!AUX_VIRTUAL_TIME
	if (sysAuxClkRunning)
	    {
	    LOCAL int 	remainderTicks;
	    int 	extraTicks;

	    extraTicks = auxClkTicksPerSecond / sysClkTicksPerSecond;
	    remainderTicks += auxClkTicksPerSecond % sysClkTicksPerSecond;
	    if (remainderTicks > sysClkTicksPerSecond)
		{
		extraTicks++;
		remainderTicks -= sysClkTicksPerSecond;
		}

	    while (extraTicks--)
		sysAuxClkInt ();
	    }
#endif	/* !AUX_VIRTUAL_TIME */
	    }
	}
    else
#endif	/* SYS_VIRTUAL_TIME */

    if (src == 0)
	{
	if (sysClkRoutine != NULL)
	    (*sysClkRoutine) (sysClkArg);
#if	!AUX_VIRTUAL_TIME
	if (sysAuxClkRunning)
	    {
	    LOCAL int	remainderTicks;
	    int 	extraTicks;

	    extraTicks = auxClkTicksPerSecond / sysClkTicksPerSecond;
	    remainderTicks += auxClkTicksPerSecond % sysClkTicksPerSecond;
	    if (remainderTicks > sysClkTicksPerSecond)
		{
		extraTicks++;
		remainderTicks -= sysClkTicksPerSecond;
		}

	    while (extraTicks--)
		sysAuxClkInt ();
	    }
#endif	/* !AUX_VIRTUAL_TIME */
	}
    }

/***************************************************************************
*
* sysHwInit2 - configure and initialize additional system features
*
* This routine connects system interrupts and does any additiona
* configuration necessary. It is called from usrRoot() in usrConfig.c
* after the multitasking kernel has started.
*
* RETURNS: N/A
*/

void sysHwInit2 (void)
    {
    LOCAL int	init = FALSE;
    char 	host_ip [256];
    char	username [80];
    char 	target_ip [16];
    char	gw_ip [19] = "";

    if (init)
	return;

    init = TRUE;

    /* XXX should check what SIGINT char really is */
    
    (void)intConnect ((VOIDFUNCPTR*)IV_INT, sysIntHandler, 0x3); /* ^C */

    (void)intConnect ((VOIDFUNCPTR*)IV_CLOCK1, sysClkInt, 0); /* ALRM */
#if	SYS_VIRTUAL_TIME

    /* when using virtual timer, need realtime for ticks when suspended */

    (void)intConnect ((VOIDFUNCPTR*)IV_CLOCK2, sysClkInt, 1); /* VTARLM */
#endif	/* SYS_VIRTUAL_TIME */

    s_userGet (vxsim_hostname, host_ip, username, vxsim_cwd);

    /* sysProcNum is in BSS and will be zero, unless set from command line */

    sprintf (target_ip, VXSIM_IP_ADDR, sysProcNumGet () + 1);
    sprintf (gw_ip, VXSIM_IP_ADDR, 254);

    /* SPR 70334 : check if u_progname is an absolute or relative path */

    if (u_progname[0] == '/')
	sprintf (sysBootLineMagic, "%s(0,%d)%s:%s h=%s g=%s e=%s u=%s", 
    	     vxsim_bootdev, sysProcNumGet(), vxsim_hostname, u_progname, 
	     host_ip, gw_ip ,target_ip, username);
    else
	sprintf (sysBootLineMagic, "%s(0,%d)%s:%s/%s h=%s g=%s e=%s u=%s", 
    	     vxsim_bootdev, sysProcNumGet(), vxsim_hostname, vxsim_cwd,
	     u_progname, host_ip, gw_ip ,target_ip, username);

#ifndef	INCLUDE_NET_INIT
    /* make core file available to WDB agent */

    strcpy (sysBootLine, sysBootLineMagic);

#ifndef INCLUDE_BOOT_LINE_INIT
    /* sysBootParams init is required by INCLUDE_NET_SYM_TBL */

    bootStringToStruct (sysBootLine, &sysBootParams);
#endif /* !INCLUDE_BOOT_LINE_INIT */
#endif /* !INCLUDE_NET_INIT */

    sysSerialHwInit2 ();
    }

/***************************************************************************
*
* sysClkConnect - connect a routine to the system clock interrupt
*
* This routine specifies the interrupt service routine to be called at each
* clock interrupt.  Normally, it is called from usrRoot() in usrConfig.c to
* connect usrClock() to the system clock interrupt.
*
* RETURN: OK, or ERROR if the routine cannot be connected to the interrupt.
*
* SEE ALSO: intConnect(), usrClock(), sysClkEnable()
*/

STATUS sysClkConnect
    (
    FUNCPTR	routine, /* routine called at each system clock interrupt */
    int 	arg	 /* argument with which to call routine */
    )
    {
    if (!sysClkConnected)
	{
	sysHwInit2 ();
	sysClkConnected = TRUE;
	}

    sysClkRoutine = routine;
    sysClkArg     = arg;

    return (OK);
    }

/***************************************************************************
*
* sysClkDisable - turn off system clock interrupts
*
* This routine disables system clock interrupts.
*
* RETURNS: N/A
*
* SEE ALSO: sysClkEnable()
*/

void sysClkDisable (void)
    {
    if (sysClkRunning)
	{
	struct unix_itimerval v;

	timerclear (&v.it_value);
	timerclear (&v.it_interval);
	u_setitimer (U_ITIMER_REAL, (int*)&v, 0);
#if	SYS_VIRTUAL_TIME
	u_setitimer (U_ITIMER_VIRTUAL, (int*)&v, 0);
#endif	/* SYS_VIRTUAL_TIME */

	sysClkRunning = FALSE;
	}
    }

/***************************************************************************
*
* sysClkEnable - turn on system clock interrupts
*
* This routine enables system clock interrupts.
*
* RETURNS: N/A
*
* SEE ALSO: sysClkConnect(), sysClkDisable(), sysClkRateSet()
*/

void sysClkEnable (void)
    {
    /* min. resolution ~20 millisecs or 20,000 microsecs (50hz) */

    if (!sysClkRunning)
	{
	struct unix_itimerval v;
	int ix = MILLION / sysClkTicksPerSecond;

	if (ix <= 0)
	    {
	    u_printf ("sysClkEnable: underflow = %d\n", sysClkTicksPerSecond);
	    ix = 100;
	    }

	v.it_value.tv_sec     = 0;
	v.it_value.tv_usec    = ix;
	v.it_interval.tv_sec  = 0;
	v.it_interval.tv_usec = ix;

	u_setitimer (U_ITIMER_REAL, (int*)&v, 0);
#if	SYS_VIRTUAL_TIME
	u_setitimer (U_ITIMER_VIRTUAL, (int*)&v, 0);
#endif	/* SYS_VIRTUAL_TIME */

	sysClkRunning = TRUE;
	}
    }

/***************************************************************************
*
* sysClkRateGet - get the system clock rate
*
* This routine returns the interrupt rate of the system clock.
*
* RETURNS: The number of ticks per second of the system clock.
*
* SEE ALSO: sysClkEnable(), sysClkRateSet()
*/

int sysClkRateGet (void)
    {
    return (sysClkTicksPerSecond);
    }

/***************************************************************************
*
* sysClkRateSet - set the system clock rate
*
* This routine sets the interrupt rate of the system clock.
* It is called by usrRoot() in usrConfig.c.
*
* RETURNS: OK, or ERROR if the tick rate is invalid or the timer cannot be
* set.
*
* SEE ALSO: sysClkEnable(), sysClkRateGet()
*/

STATUS sysClkRateSet
    (
    int ticksPerSecond	    /* number of clock interrupts per second */
    )
    {
     if (ticksPerSecond < SYS_CLK_RATE_MIN || ticksPerSecond > SYS_CLK_RATE_MAX)
	 return (ERROR);

    sysClkTicksPerSecond = ticksPerSecond;
	     
    if (sysClkRunning)
	{
	sysClkDisable ();
	sysClkEnable ();
	}

    return (OK);
    }

/***************************************************************************
*
* sysAuxClkInt - handle auxiliary clock interrupts
*/

LOCAL void sysAuxClkInt (void)
    {
    if (sysAuxClkRoutine != NULL)
	(*sysAuxClkRoutine) (sysAuxClkArg);
    }

/***************************************************************************
*
* sysAuxClkConnect - connect a routine to the auxiliary clock interrupt
*
* This routine specifies the interrupt service routine to be called at each
* auxiliary clock interrupt.  It does not enable auxiliary clock
* interrupts.
*
* RETURNS: OK, or ERROR if the routine cannot be connected to the interrupt.
*
* SEE ALSO: intConnect(), sysAuxClkEnable()
*/

STATUS sysAuxClkConnect
    (
    FUNCPTR routine,	/* routine called at each auxiliary clock interrupt */
    int     arg		/* argument to auxiliary clock interrupt routine */
    )
    {
    if (!auxClkConnected)
	{
	auxClkConnected  = TRUE;

#if	AUX_VIRTUAL_TIME
	(void)intConnect ((VOIDFUNCPTR*)IV_CLOCK3, sysAuxClkInt, 0); /* PROF */
#endif	/* AUX_VIRTUAL_TIME */
	}

    sysAuxClkRoutine = routine;
    sysAuxClkArg     = arg;

    return (OK);
    }

/***************************************************************************
*
* sysAuxClkDisable - turn off auxiliary clock interrupts
*
* This routine disables auxiliary clock interrupts.
*
* RETURNS: N/A
*
* SEE ALSO: sysAuxClkEnable()
*/

void sysAuxClkDisable (void)
    {
    struct unix_itimerval v;

    timerclear (&v.it_value);
    timerclear (&v.it_interval);
    u_setitimer (U_ITIMER_PROF, (int*)&v, 0);

    sysAuxClkRunning = FALSE;
    }

/***************************************************************************
*
* sysAuxClkEnable - turn on auxiliary clock interrupts
*
* This routine enables auxiliary clock interrupts.
*
* RETURNS: N/A
*
* SEE ALSO: sysAuxClkConnect(), sysAuxClkDisable(), sysAuxClkRateSet()
*/

void sysAuxClkEnable (void)
    {
    if (!sysAuxClkRunning)
	{
#if	AUX_VIRTUAL_TIME
	struct unix_itimerval	v;
	int 			ix = MILLION / auxClkTicksPerSecond;

        if (ix <= 0)
            {
            u_printf ("sysAuxClkEnable: underflow = %d\n",
	    	      auxClkTicksPerSecond);
            ix = 100;
            }

        v.it_value.tv_sec     = 0;
        v.it_value.tv_usec    = ix;
        v.it_interval.tv_sec  = 0;
        v.it_interval.tv_usec = ix;

        u_setitimer (U_ITIMER_PROF, (int*)&v, 0); /* not exactly accurate */
#endif	/* AUX_VIRTUAL_TIME */

	sysAuxClkRunning = TRUE;
	}
    }

/***************************************************************************
*
* sysAuxClkRateGet - get the auxiliary clock rate
*
* This routine returns the interrupt rate of the auxiliary clock.
*
* RETURNS: The number of ticks per second of the auxiliary clock.
*
* SEE ALSO: sysAuxClkRateSet()
*/

int sysAuxClkRateGet (void)
    {
    return (auxClkTicksPerSecond);
    }

/***************************************************************************
*
* sysAuxClkRateSet - set the auxiliary clock rate
*
* This routine sets the interrupt rate of the auxiliary clock.
* It does not enable auxiliary clock interrupts.
*
* RETURNS: OK, or ERROR if the tick rate is invalid or the timer cannot be set.
*
* SEE ALSO: sysAuxClkEnable(), sysAuxClkRateGet()
*/

STATUS sysAuxClkRateSet
    (
    int ticksPerSecond	    /* number of clock interrupts per second */
    )
    {
    if (ticksPerSecond < AUX_CLK_RATE_MIN || ticksPerSecond > AUX_CLK_RATE_MAX)
	return (ERROR);

    auxClkTicksPerSecond = ticksPerSecond;
	     
    if (sysAuxClkRunning)
	{
	sysAuxClkDisable ();
	sysAuxClkEnable ();
	}

    return (OK);
    }

/***************************************************************************
*
* sysProcNumGet - get processor number
*
* This routine returns the processor number for the target, which is
* set with sysProcNumSet().
*
* RETURNS: The processor number for this target.
*/

int sysProcNumGet (void)
    {
    return (sysProcNum);
    }

/***************************************************************************
*
* sysProcNumSet - set the processor number
*
* This routine sets the processor number for the target.
* Processor numbers should be unique on a host.
*
* NOTE
* The sysProcNumSet() routine is important in identifying the processor
* number when networking with PPP.  The shared memory segment is
* also initialized here.
*
* RETURNS: N/A
*
* SEE ALSO: sysProcNumGet()
*/

void sysProcNumSet
    (
    int procNum		/* processor number */
    )
    {
    sysShmid = u_shmget (0x87654321, SM_MEM_SIZE+SM_OBJ_MEM_SIZE,
			 IPC_CREAT | 0666);

    if (sysShmid < 0)
	u_printf ("sysProcNumSet: unable to get share memory segment\n");
    else
	sysSmAddr = u_shmat (sysShmid, 0, 0);

    sysProcNum = procNum;
    }

/***************************************************************************
*
* sysIntDisable - disable a bus interrupt level
*
* This routine disables a specified bus interrupt level.
*
* RETURNS: OK, or ERROR if <intLevel> is not in the range.
*
* SEE ALSO: sysIntEnable()
*/

STATUS sysIntDisable
    (
    int intLevel	/* interrupt level to disable */
    )
    {
    /* intLevel == pid */

    return (OK);
    }

/***************************************************************************
*
* sysIntEnable - enable a bus interrupt level
*
* This routine enables a specified bus interrupt level.
*
* RETURNS: OK, or ERROR if <intLevel> is not in the range.
*
* SEE ALSO: sysIntDisable()
*/

STATUS sysIntEnable
    (
    int intLevel	/* interrupt level to enable */
    )
    {
    /* intLevel == pid */
    return (OK);
    }

/***************************************************************************
*
* sysBusIntAck - acknowledge a bus interrupt
*
* This routine acknowledges a specified bus interrupt.
*
* NOTE: This routine has no effect.
*
* RETURNS: NULL
*/

int sysBusIntAck
    (
    int intLevel        /* interrupt level to acknowledge */
    )
    {
    return (0);
    }

/***************************************************************************
*
* sysBusIntGen - generate a bus interrupt
*
* This routine generates an bus interrupt for a specified level with a
* specified vector.
*
* RETURNS: OK or ERROR.
*/

STATUS sysBusIntGen
    (
    int level,          /* bus interrupt level to generate          */
    int vector          /* interrupt vector to return (0-255)       */
    )
    {
    /* level == pid */
    return (u_kill (level, vector));
    }

/***************************************************************************
*
* sysMailboxConnect - connect a routine to the mailbox interrupt
*
* This routine specifies the interrupt service routine to be called at each
* mailbox interrupt.
*
* NOTE: This routine has no effect, since the hardware does not support mailbox
* interrupts.
*
* RETURNS: ERROR, since there is no mailbox facility.
*
* SEE ALSO: sysMailboxEnable()
*/

STATUS sysMailboxConnect
    (
    FUNCPTR routine,    /* routine called at each mailbox interrupt */
    int     arg         /* argument with which to call routine      */
    )
    {
    return (ERROR);
    }

/***************************************************************************
*
* sysMailboxEnable - enable the mailbox interrupt
*
* This routine enables the mailbox interrupt.
*
* NOTE: This routine has no effect, since the hardware does not support mailbox
* interrupts.
*
* RETURNS: ERROR, since there is no mailbox facility.
*
* SEE ALSO: sysMailboxConnect()
*/

STATUS sysMailboxEnable
    (
    INT8 *mailboxAdrs           /* mailbox address */
    )
    {
    return (ERROR);
    }

/***************************************************************************
*
* sysBusTas - test and set a location across the bus
*
* This routine performs a test-and-set instruction.
*
* NOTE
* This routine is similar to vxTas().
*
* RETURNS:
* TRUE if the previous value had been zero, or FALSE if the value was
* set already.
*
* SEE ALSO: vxTas()
*/

BOOL sysBusTas
    (
    char *adrs          /* address to be tested and set */
    )
    {
    return (vxTas (adrs));
    }

/***************************************************************************
*
* sysNvRamGet - get contents of non-volatile RAM
*
* This routine copies the contents of non-volatile memory into a specified
* string.  The string will be terminated with an EOS.
*
* RETURNS: OK, or ERROR if access is outside the non-volatile RAM range.
*
* SEE ALSO: sysNvRamSet()
*/

STATUS sysNvRamGet
    (
    char *	string,	/* where to copy non-volatile RAM    */
    int 	strLen,	/* maximum number of bytes to copy   */
    int 	offset	/* byte offset into non-volatile RAM */
    )
    {
    char 	filename [200];
    char 	line [NV_RAM_SIZE+1];
    int 	fd;
    int   	len;
    char *	p;
    int 	status = ERROR;

    offset += NV_BOOT_OFFSET;   /* boot line begins at <offset> = 0 */

    if ((offset < 0) || (strLen < 0) || ((offset + strLen) > NV_RAM_SIZE))
	return (ERROR);

    sprintf (filename, NV_FILENAME, u_progname, sysProcNumGet());

    fd = u_open (filename, 0x02 /*O_RDWR*/, 0644);

    if (fd < 0)
	{
	char *home = u_getenv ("HOME");
	if (home != NULL)
	    {
	    sprintf (filename, NV_UFILENAME, home, 
	    	     u_progname, sysProcNumGet());
	    fd = u_open (filename, 0 /*O_RDONLY*/, 0);
	    }
	}

    if (fd > 0)
	{
	if (u_lseek (fd, offset, 0/*SEEK_SET*/) == offset &&
	    (len = u_read (fd, line, sizeof (line))) > 0)
	    {
	    if ((p = rindex (line, '\n')) != NULL)
		*p = EOS;	/* chop off final CR */
	    strncpy (string, line, strLen);
	    string [strLen] = EOS;
	    status = OK;
	    }

	u_close (fd);
	}

    return (status);
    }

/***************************************************************************
*
* sysNvRamSet - write to non-volatile RAM
*
* This routine copies a specified string into non-volatile RAM.
*
* RETURNS: OK, or ERROR if access is outside the non-volatile RAM range.
*
* SEE ALSO: sysNvRamGet()
*/

STATUS sysNvRamSet
    (
    char *	string,	/* string to be copied into non-volatile RAM */
    int 	strLen,	/* maximum number of bytes to copy           */
    int 	offset	/* byte offset into non-volatile RAM         */
    )
    {
    char filename [200];
    char line [NV_RAM_SIZE+1];
    int  fd;
    int	 status = ERROR;

    offset += NV_BOOT_OFFSET;   /* boot line begins at <offset> = 0 */

    if ((offset < 0) || (strLen < 0) || ((offset + strLen) > NV_RAM_SIZE))
	return (ERROR);

    sprintf (filename, NV_FILENAME, u_progname, sysProcNumGet());

    fd = u_open (filename, OPEN_MODE, 0644);

    if (fd < 0)
	{
	char *home = u_getenv ("HOME");
	if (home != NULL)
	    {
	    sprintf (filename, NV_UFILENAME, home, 
	    	     u_progname, sysProcNumGet());
	    fd = u_open (filename, OPEN_MODE, 0644);
	    }
	}

    if (fd > 0)
	{
	strncpy (line, string, NV_RAM_SIZE);

	if (rindex (line, '\n') == NULL)
	    strcat (line, "\n");	/* add final CR */

	strLen = strlen (line);

	if (u_lseek (fd, offset, 0/*SEEK_SET*/) == offset &&
	    u_write (fd, line, strLen) == strLen          &&
	    u_close (fd) == 0)
	    {
	    status = OK;
	    }
	}

    return (status);
    }

/***************************************************************************
*
* sysLocalToBusAdrs - convert a local address to a bus address
*
* This routine gets the bus address that accesses a specified local
* memory address.
*
* NOTE: This routine just returns the same address.  For the simulator
* BSPs we can only simulate a VME bus.  The input address is assumed to be
* the same for the local cpu and the simulated VME bus.
*
* RETURNS: OK, or ERROR if the address space is unknown or the mapping is not
* possible.
*
* SEE ALSO: sysBusToLocalAdrs()
*/

STATUS sysLocalToBusAdrs
    (
    int		adrsSpace,  /* bus address space in which busAdrs resides */
    char *	localAdrs,  /* local address to convert                   */
    char **	pBusAdrs    /* where to return bus address                */
    )
    {
    if ((int)localAdrs < (int)LOCAL_MEM_LOCAL_ADRS || localAdrs >= sysMemTop ())
        return (ERROR); 
    
    *pBusAdrs = localAdrs;

    return (OK); 
    }

/***************************************************************************
*
* sysBusToLocalAdrs - convert a bus address to a local address
*
* This routine gets the local address that accesses a specified bus address.
*
* NOTE: This routine just returns the same address.  For the simulator
* BSPs we can only simulate a VME bus.  The input address is assumed to be
* a local cpu address, not a real VME address.
*
* RETURNS: OK, or ERROR if the address space is unknown or the mapping is not
* possible.
*
* SEE ALSO: sysLocalToBusAdrs()
*/

STATUS sysBusToLocalAdrs
    (
    int		adrsSpace,  /* bus address space in which busAdrs resides */
    char *	busAdrs,    /* bus address to convert                     */
    char **	pLocalAdrs  /* where to return local address              */
    )
    {
    *pLocalAdrs = (char *) busAdrs;
    return (OK);
    }

#ifdef	INCLUDE_PPP

/***************************************************************************
*
* solaris_ppp_init - intialize a FIFO for use with solaris PPP
*/

STATUS solaris_ppp_init
    (
    int ppp_fd
    )
    {
    char            	buf[256];
    char            	name[64];       /* user name string */
    struct strbuf   	data;
    register int    	fd;             /* FIFO to aspppd */
    union fifo_msgs *	fifo_msg;
    int             	retries;

    /*
     * Simple protocol, send:
     * user name and trailing null
     * file descriptor (could be stdin or stdout) that remote end
     * connected on
     */

    sprintf (name, "vxsim%d", sysProcNumGet());

    /*
     * sometimes we need to let asppd clean up a previous connection
     * so retry the open a few times
     */

    for (retries=0, fd=-1; retries < 4 && fd < 0; retries++)
	if ((fd = u_open (FIFO_PATH, O_WRONLY, 0)) < 0)
	    u_system ("sleep 1");

    if (fd < 0)
	{
        u_printf ("Can't open %s\n", FIFO_PATH);
        return (ERROR);
        }

#if FALSE
    /* set a high debug level in the Solaris PPP driver */

    data.buf = buf;
    data.len = sizeof (debug_t);
    data.maxlen = sizeof (debug_t);
    fifo_msg = (union fifo_msgs *) data.buf;
    fifo_msg->msg = FIFO_DEBUG;
    fifo_msg->debug.debug_level = 42;

    if (PUTMSG (fd, NULL, &data, 0) < 0)
	{
	u_printf ("Can't send name to aspppd\n");
	u_close (fd);
	return (ERROR);
	}
#endif

    data.buf = buf;
    data.len = sizeof (uname_t);
    fifo_msg = (union fifo_msgs *) data.buf;
    fifo_msg->msg = FIFO_UNAME;
    strcpy (fifo_msg->uname.uname, name);

    if (PUTMSG (fd, NULL, &data, 0) < 0)
	{
	u_printf ("Can't send name to aspppd\n");
	u_close (fd);
        return (ERROR);
	}

    if (u_ioctl (fd, I_SENDFD, (void *)ppp_fd) < 0)
	{
	u_printf ("Can't send local fd to aspppd\n");
	u_close (fd);
	return (ERROR);
	}

    return (OK);
    }

/***************************************************************************
*
* sysPPPOpen - create a pipe and connect to PPP
*
* This routine will create a UNIX pipe and connect one end of it to 
* the Solaris PPP driver.  This is done by making one end of the pipe
* look like a terminal and then pasing it on to the asynchronous PPP 
* link manager (aspppd).  aspppd will then negotitate the PPP connection
* and install pass the pipe on to the kernel.  The other end of the
* pipe is returned to vxWorks to be installed as a device which is
* connected to the PPP network driver.
*
* RETURNS:
*    -1 if the creation of the pipe fails, otherwise the UNIX file
*    descriptor ir returned.
*
* INTERNAL
* This code is highly non-portable and subject to changes made to
* how Solaris implements the aspppd. A more portable approach would be to
* use a /dev/pty device and connect it to the dial in port using
* a standard UNIX service such as aspppls, /etc/uucp/Dialers, etc.
* On the other hand, it works fine for now.
*/

int sysPPPOpen (void)
    {
    int fd[2];

    if (PIPE(fd) < 0)
        return(-1);
    u_ioctl (fd[1], I_PUSH, "ptem");
    u_ioctl (fd[1], I_PUSH, "ldterm");
    u_ioctl (fd[1], I_PUSH, "ttcompat");
    if (solaris_ppp_init (fd[1]) == ERROR)
	{
	u_printf ("solaris_ppp_init failed\n");
        u_close (fd[0]);
        u_close (fd[1]);
        return (ERROR);
	}

    return (fd[0]);
    }

#endif	/* INCLUDE_PPP */
