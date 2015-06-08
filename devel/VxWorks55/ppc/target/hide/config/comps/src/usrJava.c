/* usrJava.c - configuration file for the Personal/Embedded JWorks product */

/* Copyright 1984-1999 Wind River htmlSystems, Inc. */

/*
modification history
--------------------
01m,17jan01,lcs  remove obsolete params for 3.1
01l,10jan01,f_l  modified jvm parameters initailization
01k,04jan01,jak  fix nfsCache for SIMNT
01j,18oct00,jak  remove awt configuration
01i,16oct00,jak  update for pjw31 awt
01h,31aug00,f_l  added nfs support initialization for simsolaris
01g,24jul00,xav  Added javaShutdownDelaySet in javaParamsConfig routine
01f,19jul00,f_l  fixed spr 24637, added function pointer to resolvGetHostByName
01f,03jul00,lcs  Add bootclasspath config option
01e,30jul99,sjw  Call javaGraphicsConfig() externally (SPR 24640)

01e,11oct99,sjw  Merge in latest changes from tor2_0_0.graphics
01d,07may99,dmm  Guard for javaNfsConfig routine needed in case NFS is not
                 selected
01c,05may99,dmm  Adding javaNfsConfig, javaMemConfig and some other routines
01b,04may99,dmm  Complete Cleanup for the New T2 PJWorks Config Scheme
01a,04apr99,dmm  written.
*/

/*
DESCRIPTION
This file configures and initializes both Personal and Embedded JWorks products
The only user callable routine is usrJavaInit().

NOMANUAL
*/

#include <vxWorks.h>
#include <memDrv.h>
#include <private/javaLibP.h>

#if (CPU==SIMSPARCSOLARIS) || (CPU==SIMNT)
    extern nfsCacheSize;
#endif /* CPU==SIMSPARCSOLARIS || CPU==SIMNT */

/* globals */
extern JAVA_LIB_BOUNDS javaStartBounds;
extern JAVA_LIB_BOUNDS javaEndBounds;

JAVA_LIB_SEGS  javaSegList[] =
    { 
    {&javaStartBounds, &javaEndBounds, NULL} ,

    /* this must always be the last entry */
    { NULL, NULL, NULL },
    };

/* This is a temporary workaround until jMemLib.h is in the VxWorks include */
extern STATUS jMemLibInit (void);
extern void   jMemLibFinalize (void);
extern void * jMemAlloc (void * pAddr, size_t size, size_t alignment);
extern void   jMemFree (void * pMem);

/******************************************************************************
*
* javaInstrumentHook - hook to be called when interesting Java events occurr
*
* If WindView facilities have not been selected in the build of
* VxWorks then this compiles to an empty function.  If WindView is
* included then this will call wvEvent() to report the event.  With
* WindView included Java reports user events with the following
* meaning:
*
*	0	MONITOR_CONSTRUCTION	object monitor is being created
*	1	MONITOR_DESTRUCTION	object monitor is being destroyed
*	2	MONITOR_ENTER		object monitor is entered
*	3	MONITOR_EXIT		object monitor is exited
*	4	MONITOR_WAIT		object monitor is being waited on
*	5	MONITOR_NOTIFY		notify of all monitor waiters 
*	6	MONITOR_SIGNAL		notify of single monitor waiter
*
*	7	THREAD_CREATE		creation of thread
*	8	THREAD_DELETE,		deletion of thread
*	9	THREAD_JEXCEPTION	Java exception in thread
*	10	THREAD_SUSPEND		suspend of thread
*	11	THREAD_RESUME		resume of thread
*	12	THREAD_YIELD		thread is yielding
*
*	13	SINGLE			all other threads are locked out
*	14	MULTI			other threads are unlocked
*
*	15	ABORT			java is aborting
*
*/

STATUS javaInstrumentHook
    (
    const char * eventName,	/* event being logged, e.g. monitor enter */
    void *	pData,		/* associated data, e.g. monitor id */
    int		evtType		/* event type code - see instrument_md.h */
    )
    {
#ifdef INCLUDE_WINDVIEW
    /* Pass it over to WindView */
    return wvEvent ((event_t) evtType, (char *) &pData, sizeof (void *));
#else
    return OK;
#endif /* INCLUDE_WINDVIEW */
    }

/*******************************************************************************
*
* usrJavaInit - configure and initialize Personal/Embedded JWorks
*
* This routine configures and initializes both Personal and Embedded JWorks
* products.
*
* RETURNS:
* OK, or
* ERROR if there is a problem in initializing the JWorks products.
*
* NOMANUAL
*/

STATUS javaParamsConfig ()
    {
    javaHomeSet (JAVA_HOME);
    javaShutdownDelaySet (JAVA_SHUTDOWN_DELAY);
    javaClassPathSet (JAVA_CLASS_PATH);
    javaBootClassPathSet (JAVA_BOOT_CLASS_PATH);
    if (JAVA_USER_HOME == NULL)
    {
    	javaUserHomeSet (JAVA_HOME);
    }
    else
    {
    	javaUserHomeSet (JAVA_USER_HOME);
    }
    if (JAVA_USER_DIR == NULL)
    {
    	javaUserDirSet (JAVA_HOME);
    }
    else
    {
    	javaUserDirSet (JAVA_USER_DIR);
    }
    javaUserNameSet (JAVA_USER_NAME);
    javaFileSeparatorSet (JAVA_FILE_SEPARATOR);
    javaJNIArgInitHookSet (JAVA_JNI_ARG_INIT_HOOK);

    javaNativeStackSizeSet (JAVA_NATIVE_STACK_SIZE);
    javaMaxHeapSizeSet (JAVA_MAX_HEAP_SIZE);

    javaStackCheckSet (JAVA_STACK_CHECK);
    javaInversionSafeSet (JAVA_INVERSION_SAFE);
    javaMaxPrioritySet (JAVA_MAX_PRIORITY);
    javaLoadLibraryPathSet (JAVA_LOAD_LIBRARY_PATH);
    javaThreadInitHookSet (JAVA_THREAD_INIT_HOOK);
    javaInstrumentHookSet (JAVA_INSTRUMENT_HOOK_ROUTINE);

#if defined (INCLUDE_WINDVIEW)
    javaInstrumentSet (TRUE);
#else
    javaInstrumentSet (FALSE);
#endif /* INCLUDE_WINDVIEW */

    javaAbortOnExceptionSet (JAVA_ABORT_ON_EXCEPTION);
    javaRestartableSet(JAVA_RESTARTABLE);
    javaMemInterfaceSet (jMemLibInit, jMemLibFinalize,
			   jMemAlloc, jMemFree);

    javaPathSeparatorSet (JAVA_PATH_SEPARATOR);

    /* one day there may be a way for this to fail */
    return OK;
    }

/* Flag related to the Java VM configuration */
BOOL javaConfigDone = FALSE;

/******************************************************************************
*
* usrJavaInit - configure and initialize the Java interpreter.
*
* RETURNS: N/A
*/
STATUS usrJavaInit (void)
    {
    STATUS err;

    /* Init error to okay */
    err = OK;

    if (javaConfigDone == TRUE) return err;

#if defined(JAVA_USER_PRE_CONFIG)
    JAVA_USER_PRE_CONFIG;
#endif

    if (err != OK)
	{
	/* User init code reported error - do nothing */
	}

    if (javaParamsConfig () != OK)
	{
	err = ERROR;
	}

#if defined(JAVA_FILE_CONFIG_NFS)
    if (javaNfsConfig () != OK)
	{
	err = ERROR;
	}
#endif
    
    if (javaMemDrvConfig () != OK)
	{
	err = ERROR;
	}

#if defined(JAVA_USER_POST_CONFIG)
    JAVA_USER_POST_CONFIG;
#endif

    javaConfigDone = TRUE;
	
    return err;
    }

#if defined(JAVA_FILE_CONFIG_NFS)

/******************************************************************************
*
* javaNfsConfig - mount NFS file system(s) for Java VM
*
* This routine will mount the file system JAVA_NFS_HOST_PATH from
* JAVA_NFS_HOST to the target file system JAVA_HOME.  If JAVA_NFS_HOST_ADDR
* is defined then it will first add the mount host name to the host name
* table with the IP address string given by JAVA_NFS_HOST_ADDR e.g.
* "127.0.0.1"
*
* RETURNS: OK on success, ERROR otherwise
*/

STATUS javaNfsConfig (void)
    {
	char *host;

	if (JAVA_NFS_HOST == NULL) 
	{
		host = (char *) &sysBootHost;		
	}
	else
	{
		host = JAVA_NFS_HOST;
	}
	
	if (JAVA_NFS_HOST_ADDR != NULL)
	{
		if (hostAdd (host, JAVA_NFS_HOST_ADDR) != OK &&
        		errno != S_hostLib_HOST_ALREADY_ENTERED)
			{
				fprintf (stderr, "Failed to add %s, %s to host table\n",
		 			host, JAVA_NFS_HOST_ADDR);
				return ERROR;
			}
#if (CPU==SIMSPARCSOLARIS) || (CPU==SIMNT)
	/* override default nfs cache size (spr 33253) */
	nfsCacheSize = 512;
#endif /* CPU==SIMSPARCSOLARIS || CPU==SIMNT*/

	} /* JAVA_NFS_HOST_ADDR */

    /* Mount NFS file systems for Java classes */
    if (nfsMount (host, JAVA_NFS_HOST_PATH, JAVA_HOME) != OK)
	{
	fprintf (stderr, "Failed to mount %s from %s:%s\n", 
		 JAVA_HOME,
		 host, 
		 JAVA_NFS_HOST_PATH);
	return ERROR;
	}

    return OK;
    }

#endif

/******************************************************************************
*
* javaMemDrvConfig - create a memDrv device containing class data
*
*
* RETURNS: OK on success, ERROR otherwise
*/
STATUS javaMemDrvConfig (void)
    {
    STATUS err;

    err = OK;

#if defined(JAVA_MEM_DRV_NAME) && defined(JAVA_MEM_DRV_ADDRESS) && \
    defined(JAVA_MEM_DRV_SIZE)

    /* Initialize the device driver - doesn't hurt to repeat */
    memDrv ();

    /* Now create the device using data array above */
    if (memDevCreate (JAVA_MEM_DRV_NAME, 
		      (char *) JAVA_MEM_DRV_ADDRESS, 
		      JAVA_MEM_DRV_SIZE) != OK)
	{
	fprintf (stderr, "Failed to create %ld byte memDrv device %s at %p",
		 JAVA_MEM_DRV_SIZE, JAVA_MEM_DRV_NAME, JAVA_MEM_DRV_ADDRESS);
	err = ERROR;
	}
    else
	{
	struct stat statBuf;

	if (stat (JAVA_MEM_DRV_NAME, & statBuf) == ERROR)
	    {
	    fprintf (stderr, 
		     "Failed to stat file %s, memDrv driver may be old\n",
		     JAVA_MEM_DRV_NAME);
	    err = ERROR;
	    }
	}
#endif /* JAVA_MEM_DRV_DATA */
    return err;
    }    


/* Fix for spr 24637 . Set function pointer to vxWorks' resolvGetHostByName 
 * routine if DNS is included */
#ifdef INCLUDE_DNS_RESOLVER
FUNCPTR _pJavaResolvGetHostByName = resolvGetHostByName;
#else 
FUNCPTR _pJavaResolvGetHostByName = NULL;
#endif /* INCLUDE_DNS_RESOLVER */
