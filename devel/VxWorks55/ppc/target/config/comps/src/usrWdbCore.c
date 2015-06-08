/* usrWdbCore.c - configuration file for the WDB agent's core facilities */

/* Copyright 1997-2002 Wind River Systems, Inc. */

/*
modification history
--------------------
01l,01apr02,jkf  SPR#74251, using bootLib.h macros for array sizes
01k,28feb02,jhw  Modified hasWriteProtect assignment. (SPR 71816).
01j,04oct01,c_c  Added wdbToolName symbol.
01i,06jun01,pch  Fix length of copy to vxBootFile.
01h,09may01,dtr  Addition of wdbTgtHasCoprocessor.
01g,19apr01,kab  Added Altivec support
01f,16nov00,zl   added DSP support.
01e,27apr99,cym  removing wait for simulator IDE signaling
01d,22feb99,cym  increased time before notifying IDE (SPR #23942.)
01c,08jul98,ms   need semTake initialized early if WDB_TASK included
01b,18may98,dbt  integrated latest WDB stuff
01a,21jul97,ms   taken from 01y of usrWdb.c
*/

/*
DESCRIPTION

This library configures the WDB agent's core facilities.
Agent initialization is a separate step.
*/

/* defines */

#define	NUM_MBUFS		5
#define MAX_LEN			BOOT_LINE_SIZE
#define WDB_MAX_SERVICES        50      /* max # of agent services */
#define WDB_POOL_BASE		((char *)(FREE_RAM_ADRS))
#define WDB_NUM_CL_BLKS         5

/* externals */

extern STATUS wdbCommDevInit (WDB_COMM_IF * pCommIf, char ** ppWdbInBuf,
					    char ** ppWdbOutBuf);
#ifdef	INCLUDE_WDB_TASK
static void * vxSemCreate (void);
static STATUS vxSemGive (void * semId);
static STATUS vxSemTake (void * semId, struct timeval * tv);
#endif

/* globals */

uint_t		wdbCommMtu;
int		wdbNumMemRegions = 0;	/* number of extra memory regions */
WDB_MEM_REGION *pWdbMemRegions = NULL;	/* array of regions */
VOIDFUNCPTR	wdbBpSysEnterHook;
VOIDFUNCPTR	wdbBpSysExitHook;
VOIDFUNCPTR	wdbTaskRestartHook;
WDB_RT_IF	wdbRtIf;		/* XXX - move to wdbLib.c */
bool_t		_wdbTgtHasFpp = FALSE;
bool_t		_wdbTgtHasDsp = FALSE;
bool_t		_wdbTgtHasAltivec = FALSE;
bool_t		wdbIsInitialized = FALSE;

/* This two macros transform TOOL define into the string "TOOL" */

#define MKSTR(MACRO) MKSTR_FIRST_PASS(MACRO)
#define MKSTR_FIRST_PASS(MACRO) #MACRO

/* 
 * This symbol "wdbToolName" is used by the tgtsvr to retrieve the name of the
 * tool used to build vxWorks run-time. DO NOT REMOVE !!!
 */

#ifdef	TOOL
const char wdbToolName[]	= MKSTR(TOOL);
#else	/* TOOL */
const char wdbToolName[]	= "Unknown";
#endif	/* TOOL */

/* locals */

LOCAL BUF_POOL wdbMbufPool;
LOCAL BUF_POOL wdbClBlkPool;
LOCAL char	vxBootFile [MAX_LEN];
#ifdef	INCLUDE_KERNEL
LOCAL BOOL	prevKernelState;
#endif	/* INCLUDE_KERNEL */

LOCAL WDB_SVC	wdbSvcArray [WDB_MAX_SERVICES];
LOCAL uint_t	wdbSvcArraySize = WDB_MAX_SERVICES;

/* forward static declarations */

LOCAL void	wdbMbufInit (void);
LOCAL void	wdbRtIfInit (void);
LOCAL STATUS	wdbCommIfInit (void);

#if	CPU==SIMNT
void winIdeNotify (void)
    {
    extern int simUpMutex;
    extern int win_ReleaseMutex(int hMutex);
    win_ReleaseMutex(simUpMutex);
    }
#endif

/******************************************************************************
*
* wdbConfig - configure and initialize the WDB agent.
*
* This routine configures and initializes the WDB agent.
*
* RETURNS :
* OK or ERROR if the communication link cannot be initialized.
*
* NOMANUAL
*/

STATUS wdbConfig (void)
    {
    /* Initialize the agents interface function pointers */

    wdbRtIfInit   ();		/* run-time interface functions */

    if (wdbCommIfInit () == ERROR)	/* communication interface functions */
	{
	if (_func_printErr != NULL)
	    _func_printErr ("wdbConfig: error configuring WDB communication interface\n");
	return (ERROR);
	}

    /* Install some required agent services */

    wdbSvcLibInit (wdbSvcArray, wdbSvcArraySize);
    wdbConnectLibInit ();	/* required agent service */
    wdbMemCoreLibInit ();	/* required agent service */

    wdbIsInitialized = TRUE;	/* mark WDB as initialized */

#if	CPU==SIMNT
    winIdeNotify();
#endif

    return (OK);
    }

/******************************************************************************
*
* wdbExternEnterHook - hook to call when external agent is entered.
*
* NOMANUAL
*/

void wdbExternEnterHook (void)
    {
#ifdef	INCLUDE_KERNEL
    intCnt++;			/* always fake an interrupt context */
    prevKernelState = kernelState;
    kernelState = TRUE;		/* always run in kernel state */
#endif	/* INCLUDE_KERNEL */

#if     (CPU_FAMILY==I80X86)
    sysIntLock ();
#endif  /* CPU_FAMILY==I80X86 */

    if (wdbBpSysEnterHook != NULL)
	wdbBpSysEnterHook ();
    }

/******************************************************************************
*
* wdbExternExitHook - hook to call when the external agent resumes the system.
*
* NOMANUAL
*/

void wdbExternExitHook (void)
    {
#ifdef	INCLUDE_KERNEL
    intCnt--;			/* restore original intCnt value */
    kernelState = prevKernelState; /* restore original kernelState value */
#endif	/* INCLUDE_KERNEL */

#if     (CPU_FAMILY==I80X86)
    intLock ();
    sysIntUnlock ();
#endif  /* CPU_FAMILY==I80X86 */

    if (wdbBpSysExitHook != NULL)
	wdbBpSysExitHook ();
    }

/******************************************************************************
*
* wdbTgtHasFpp - check if the target has fpp support
*/ 

BOOL wdbTgtHasFpp (void)
    {
    return ((BOOL)_wdbTgtHasFpp);
    }

/******************************************************************************
* 
* wdbTgtHasFppSet - call this function to force fpp support
*/ 

void wdbTgtHasFppSet (void)
    {
    _wdbTgtHasFpp = TRUE;
    }


/******************************************************************************
*
* wdbTgtHasAltivec - check if the target has fpp support
*/ 

BOOL wdbTgtHasAltivec (void)
    {
    return ((BOOL)_wdbTgtHasAltivec);
    }

/******************************************************************************
* 
* wdbTgtHasAltivecSet - call this function to force fpp support
*/ 

void wdbTgtHasAltivecSet (void)
    {
    _wdbTgtHasAltivec = TRUE;
    }

/******************************************************************************
*
* wdbTgtHasDsp - check if the target has DSP support
*/ 

BOOL wdbTgtHasDsp (void)
    {
    return ((BOOL)_wdbTgtHasDsp);
    }

/******************************************************************************
* 
* wdbTgtHasDspSet - call this function to force DSP support
*/ 

void wdbTgtHasDspSet (void)
    {
    _wdbTgtHasDsp = TRUE;
    }

/******************************************************************************
* 
* wdbTgtHasCoprocessor - this function's result contains bit fields for each 
* 	co-processor. Coprocessors include floating-point,altivec(PPC) and 
*	dsp(SH).
*/ 

LOCAL UINT32 wdbTgtHasCoprocessor (void)
    {
    UINT32 result;
    result = wdbTgtHasFpp();
#ifdef INCLUDE_ALTIVEC
    result |= (wdbTgtHasAltivec() << WDB_CO_PROC_ALTIVEC);
#endif
#ifdef INCLUDE_DSP
    result |= (wdbTgtHasDsp() << WDB_CO_PROC_DSP);
#endif
    return result;
    }

/******************************************************************************
*
* wdbRtInfoGet - get info on the VxWorks run time system.
*
* NOMANUAL
*/

void wdbRtInfoGet
    (
    WDB_RT_INFO *	pRtInfo
    )
    {
    pRtInfo->rtType	= WDB_RT_VXWORKS;
    pRtInfo->rtVersion	= vxWorksVersion;
    pRtInfo->cpuType	= CPU;
    pRtInfo->hasCoprocessor	= wdbTgtHasCoprocessor();
#ifdef INCLUDE_PROTECT_TEXT
    pRtInfo->hasWriteProtect	= (vmLibInfo.pVmTextProtectRtn != NULL);
#else
    pRtInfo->hasWriteProtect	= FALSE;
#endif /* INCLUDE_PROTECT_TEXT */
    pRtInfo->pageSize   = VM_PAGE_SIZE_GET();
    pRtInfo->endian	= _BYTE_ORDER;
    pRtInfo->bspName	= sysModel();

    pRtInfo->bootline	= vxBootFile;

    pRtInfo->memBase	= (TGT_ADDR_T)(LOCAL_MEM_LOCAL_ADRS);
    pRtInfo->memSize	= (int)sysMemTop() - (int)LOCAL_MEM_LOCAL_ADRS;
    pRtInfo->numRegions	= wdbNumMemRegions;
    pRtInfo->memRegion	= pWdbMemRegions;

    pRtInfo->hostPoolBase = (TGT_ADDR_T)WDB_POOL_BASE;
    pRtInfo->hostPoolSize = WDB_POOL_SIZE;

    }

/******************************************************************************
*
* vxReboot - reboot the system.
*
*/

LOCAL void vxReboot (void)
    {
#ifdef	INCLUDE_KERNEL
    reboot (0);
#else	/* INCLUDE_KERNEL */
    (void) sysToMonitor (0);
#endif	/* INCLUDE_KERNEL */
    }

/******************************************************************************
*
* vxMemProtect - protect a region of memory.
*/

LOCAL STATUS vxMemProtect
    (
    char * addr,
    u_int  nBytes,
    bool_t protect		/* TRUE = protect, FALSE = unprotect */
    )
    {
    return (VM_STATE_SET (NULL, addr, nBytes, VM_STATE_MASK_WRITABLE, 
                     (protect ? VM_STATE_WRITABLE_NOT : VM_STATE_WRITABLE)));
    }

/******************************************************************************
*
* vxExcHookAdd -
*/

LOCAL void (*vxExcHook)();

LOCAL int vxExcHookWrapper (int vec, char *pESF, WDB_IU_REGS *pRegs)
    {
    WDB_CTX	context;

#ifdef	INCLUDE_KERNEL
    if (INT_CONTEXT() || wdbIsNowExternal() || (taskIdCurrent == 0))
	context.contextType = WDB_CTX_SYSTEM;
    else
	context.contextType = WDB_CTX_TASK;
    context.contextId	= (int)taskIdCurrent;
#else	/* INCLUDE_KERNEL */
    context.contextType = WDB_CTX_SYSTEM;
    context.contextId   = -1;
#endif	/* INCLUDE_KERNEL */

    (*vxExcHook)(context, vec, pESF, pRegs);

    if (wdbTaskRestartHook != NULL)
	wdbTaskRestartHook ();

    return (FALSE);
    }

/******************************************************************************
*
* vxExcHookAdd - add an excpetion hook
*/

LOCAL void vxExcHookAdd
    (
    void	(*hook)()
    )
    {
    vxExcHook = hook;

    _func_excBaseHook = vxExcHookWrapper;
    }

/******************************************************************************
*
* wdbRtIfInit - Initialize pointers to the VxWorks routines.
*/

LOCAL void wdbRtIfInit ()
    {
    int 	ix = 0;

    bzero ((char *)&wdbRtIf, sizeof (WDB_RT_IF));

    wdbRtIf.rtInfoGet		= wdbRtInfoGet;
    wdbRtIf.reboot		= vxReboot;
    wdbRtIf.cacheTextUpdate	= (void (*)())cacheLib.textUpdateRtn;
    wdbRtIf.memProtect  	= vxMemProtect;
    wdbRtIf.memProbe		= (STATUS (*)())vxMemProbe;
    wdbRtIf.excHookAdd		= vxExcHookAdd;

#ifdef	INCLUDE_WDB_TASK
    wdbRtIf.semCreate   = vxSemCreate;
    wdbRtIf.semGive     = vxSemGive;
    wdbRtIf.semTake     = vxSemTake;
#endif

    /* first check if boot line is empty (eg : no network) */

    if (*sysBootLine != EOS)
	{
	for (ix = 0; ix < MAX_LEN; ix ++)
	    {
	    if (*(sysBootLine + ix) == ')')
		{
		ix++;
		break;
		}
	    }

	/* Copy the bootline following the ')' to vxBootFile */
	strncpy (vxBootFile, sysBootLine + ix, sizeof(vxBootFile));

	/* Truncate vxBootFile at the first space */
	for (ix = 0; ix < MAX_LEN - 1; ix ++)
	    {
	    if (*(vxBootFile + ix) == ' ')
		break;
	    }
	}

    *(vxBootFile + ix) = '\0';

    wdbInstallRtIf (&wdbRtIf);
    }



/******************************************************************************
*
* wdbCommIfInit - Initialize the agent's communction interface
*
* RETURNS : OK or error if we can't initialize the communication interface.
*
* NOMANUAL
*/

LOCAL STATUS wdbCommIfInit ()
    {
    static WDB_XPORT	wdbXport;
    static WDB_COMM_IF	wdbCommIf;
    char *		pWdbInBuf;
    char *		pWdbOutBuf;

    wdbMbufInit ();

    /* initialize the agent communication interface device */

    if (wdbCommDevInit (&wdbCommIf, &pWdbInBuf, &pWdbOutBuf) == ERROR)
	return (ERROR);

    /*
     * Install the agents communication interface and RPC transport handle.
     * Currently only one agent will be active at a time, so both
     * agents can share the same communication interface and XPORT handle.
     */

    wdbRpcXportInit  (&wdbXport, &wdbCommIf, pWdbInBuf, pWdbOutBuf, wdbCommMtu);
    wdbInstallCommIf (&wdbCommIf, &wdbXport);

    return (OK);
    }

/******************************************************************************
*
* wdbMbufInit - initialize the agent's mbuf memory allocator.
*
* wdbMbufLib manages I/O buffers for the agent since the agent
* can't use malloc().
*
* If the agent is ever hooked up to a network driver that uses standard
* MGET/MFREE for mbuf managment, then the routines wdbMBufAlloc()
* and wdbMBufFree() below should be changed accordingly.
*/ 

LOCAL void wdbMbufInit (void)
    {
    static struct mbuf mbufs[NUM_MBUFS];
    static CL_BLK      wdbClBlks [WDB_NUM_CL_BLKS];
    bufPoolInit (&wdbMbufPool, (char *)mbufs, NUM_MBUFS, sizeof (struct mbuf));
    bufPoolInit (&wdbClBlkPool, (char *)wdbClBlks, WDB_NUM_CL_BLKS,
                 sizeof (CL_BLK));
    }

/******************************************************************************
*
* wdbMbufAlloc - allocate an mbuf
*
* RETURNS: a pointer to an mbuf, or NULL on error.
*/ 

struct mbuf *	wdbMbufAlloc (void)
    {
    struct mbuf * pMbuf;
    CL_BLK_ID     pClBlk;

    pMbuf = (struct mbuf *)bufAlloc (&wdbMbufPool);

    if (pMbuf == NULL)
        return (NULL);

    pClBlk = (CL_BLK_ID) bufAlloc (&wdbClBlkPool);

    if (pClBlk == NULL)
        {
        wdbMbufFree (pMbuf);
        return (NULL);
        }

    pMbuf->m_next       = NULL;
    pMbuf->m_nextpkt    = NULL;
    pMbuf->m_flags      = 0;
    pMbuf->pClBlk       = pClBlk;
    return (pMbuf);
    }

/******************************************************************************
*
* wdbMbufFree - free an mbuf
*/ 

void wdbMbufFree
    (
    struct mbuf *	pMbuf		/* mbuf chain to free */
    )
    {
    /* if it is a cluster, see if we need to perform a callback */

    if (pMbuf->m_flags & M_EXT)
        {
        if (--(pMbuf->m_extRefCnt) <= 0)
            {
            if (pMbuf->m_extFreeRtn != NULL)
                {
                (*pMbuf->m_extFreeRtn) (pMbuf->m_extArg1, pMbuf->m_extArg2, pMbuf->m_extArg3);
                }
            /* free the cluster blk */
            bufFree (&wdbClBlkPool, (char *) pMbuf->pClBlk);
            }
        }
#if 1
    bufFree (&wdbMbufPool, (char *)pMbuf);
#endif
    }


