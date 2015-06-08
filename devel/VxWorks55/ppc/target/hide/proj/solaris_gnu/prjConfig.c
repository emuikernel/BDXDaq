/* prjConfig.c - dynamicaly generated configuration file */


/*
GENERATED: Tue Oct 01 00:18:02 PDT 2002
DO NOT EDIT - file is regenerated whenever the project changes.
This file contains the non-BSP system initialization code
for Create a bootable VxWorks image (custom configured).
*/


/* includes */

#include "vxWorks.h"
#include "config.h"
#include "bufLib.h"
#include "cacheLib.h"
#include "cplusLib.h"
#include "drv/wdb/wdbPipePktDrv.h"
#include "drv/wdb/wdbVioDrv.h"
#include "envLib.h"
#include "eventLib.h"
#include "excLib.h"
#include "fioLib.h"
#include "hashLib.h"
#include "intLib.h"
#include "ioLib.h"
#include "iosLib.h"
#include "logLib.h"
#include "lstLib.h"
#include "math.h"
#include "memLib.h"
#include "msgQLib.h"
#include "pipeDrv.h"
#include "private/funcBindP.h"
#include "private/kernelLibP.h"
#include "private/seqDrvP.h"
#include "private/taskLibP.h"
#include "private/trgLibP.h"
#include "private/vmLibP.h"
#include "private/workQLibP.h"
#include "private/wvFileUploadPathLibP.h"
#include "private/wvTsfsUploadPathLibP.h"
#include "private/wvUploadPathP.h"
#include "qPriBMapLib.h"
#include "rBuffLib.h"
#include "rebootLib.h"
#include "selectLib.h"
#include "semLib.h"
#include "sigLib.h"
#include "sioLib.h"
#include "stdio.h"
#include "string.h"
#include "symLib.h"
#include "sysLib.h"
#include "taskHookLib.h"
#include "taskLib.h"
#include "taskVarLib.h"
#include "tickLib.h"
#include "timexLib.h"
#include "trgLib.h"
#include "ttyLib.h"
#include "usrConfig.h"
#include "version.h"
#include "vxLib.h"
#include "wdLib.h"
#include "wdb/wdb.h"
#include "wdb/wdbBpLib.h"
#include "wdb/wdbCommIfLib.h"
#include "wdb/wdbLib.h"
#include "wdb/wdbLibP.h"
#include "wdb/wdbMbufLib.h"
#include "wdb/wdbRegs.h"
#include "wdb/wdbRpcLib.h"
#include "wdb/wdbRtIfLib.h"
#include "wdb/wdbSvcLib.h"
#include "wdb/wdbUdpLib.h"
#include "wdb/wdbVioLib.h"
#include "wvLib.h"
#include "wvTmrLib.h"


/* imports */

IMPORT char etext [];                   /* defined by loader */
IMPORT char end [];                     /* defined by loader */
IMPORT char edata [];                   /* defined by loader */


/* BSP_STUBS */



/* configlettes */

#include "sysComms.c"
#include "intrinsics.c"
#include "sysClkInit.c"
#include "usrBreakpoint.c"
#include "usrCache.c"
#include "usrKernel.c"
#include "usrPassFs.c"
#include "usrSerial.c"
#include "usrStartup.c"
#include "usrWdbBanner.c"
#include "usrWdbBp.c"
#include "usrWdbCore.c"
#include "usrWdbFpp.c"
#include "usrWdbSys.c"
#include "usrWdbSysFpp.c"
#include "usrWdbTask.c"
#include "usrWdbTaskFpp.c"
#include "usrWindview.c"
#include "usrWvFileUploadPath.c"
#include "usrWvTsfsUploadPath.c"
#include "wdbPipe.c"


/******************************************************************************
*
* usrInit - pre-kernel initialization
*/

void usrInit (int startType)
    {
    sysStart (startType);               /* clear BSS and set up the vector table base address. */
    cacheLibInit (USER_I_CACHE_MODE, USER_D_CACHE_MODE); /* include cache support */
    excVecInit ();                      /* exception handling */
    sysHwInit ();                       /* call the BSPs sysHwInit routine during system startup */
    usrCacheEnable ();                  /* optionally enable caches */
    wvLibInit ();                       /* low-level kernel instrumentation needed by windview */
    usrKernelInit ();                   /* context switch and interrupt handling. DO NOT REMOVE. */
    }



/******************************************************************************
*
* usrWdbInit - the WDB target agent
*/

void usrWdbInit (void)
    {
    wdbConfig ();                       /* software agent to support the tornado tools */
    wdbMemLibInit ();                   /* read/write target memory */
    wdbSysModeInit ();                  /* A breakpoint stops the entire operating system. */
    wdbTaskModeInit ();                 /* A breakpoint stops one task, while others keep running. */
    wdbEventLibInit ();                 /* asynchronous event handling needed for breakpoints etc. */
    wdbEvtptLibInit ();                 /* support library for breakpoints and other asynchonous events. */
    wdbDirectCallLibInit ();            /* call arbitrary functions directly from WDB */
    wdbCtxLibInit ();                   /* create/delete/manipulate tasks */
    wdbRegsLibInit ();                  /* get/set registers */
    wdbGopherLibInit ();                /* information gathering language used by many tools */
    wdbCtxExitLibInit ();               /* ability to notify the host when a task exits */
    wdbExcLibInit ();                   /* notify the host when an exception occurs */
    wdbFuncCallLibInit ();              /* asynchronous function calls */
    wdbVioLibInit ();                   /* low-level virtual I/O handling */
    wdbVioDrv ("/vio");                 /* vxWorks driver for accessing virtual I/O */
    usrWdbBp ();                        /* core breakpoint library */
    wdbTaskBpLibInit ();                /* task-mode breakpoint library */
    wdbCtxStartLibInit ();              /* ability to notify the host when a task starts */
    wdbUserEvtLibInit ();               /* ability to send user events to the host */
    wdbFppInit ();                      /* WDB hardware fpp support */
    wdbTaskFppInit ();                  /* task mode fpp debug support */
    wdbSysFppInit ();                   /* system mode mode fpp debug support */
    usrWdbBanner ();                    /* print banner to console after the agent is initialized */
    }



/******************************************************************************
*
* usrWindviewInit - 
*/

void usrWindviewInit (void)
    {
    windviewConfig ();                  /* initialize and control event logging */
    wvTmrRegister ((UINTFUNCPTR) seqStamp,					(UINTFUNCPTR) seqStampLock,					(FUNCPTR)     seqEnable,					(FUNCPTR)     seqDisable,					(FUNCPTR)     seqConnect,					(UINTFUNCPTR) seqPeriod,					(UINTFUNCPTR) seqFreq); /* no timestamping */
    rBuffLibInit ();                    /* windview 2.0 ring of buffers for event logging  */
    wdbTsfsDrv ("/tgtsvr");             /* virtual file system based on the WDB agent */
    usrWvTsfsUploadPathInit ();         /* initialize path for the upload through TSFS socket  */
    usrWvFileUploadPathInit ();         /* initialize path for the upload to file  */
    }



/******************************************************************************
*
* usrShowInit - enable object show routines
*/

void usrShowInit (void)
    {
    taskShowInit ();                    /* task show routine */
    msgQShowInit ();                    /* message queue show routine */
    }



/******************************************************************************
*
* usrToolsInit - software development tools
*/

void usrToolsInit (void)
    {
    timexInit ();                       /* utility to measure function execution time */
    trgInit ();                         /* triggering for system and user events */
    usrWdbInit ();                      /* the WDB target agent */
    usrWindviewInit ();                 /* usrWindviewInit */
    usrShowInit ();                     /* enable object show routines */
    }



/******************************************************************************
*
* usrKernelCoreInit - core kernel facilities
*/

void usrKernelCoreInit (void)
    {
    eventLibInit ();                    /* VxWorks events */
    semBLibInit ();                     /* binary semaphores */
    semMLibInit ();                     /* mutex semaphores */
    semCLibInit ();                     /* counting semaphores */
    msgQLibInit ();                     /* message queues */
    wdLibInit ();                       /* watchdog timers */
    taskHookInit ();                    /* user callouts on task creation/deletion/context switch */
    }



/******************************************************************************
*
* usrKernelExtraInit - extended kernel facilities
*/

void usrKernelExtraInit (void)
    {
    hashLibInit ();                     /* hash library */
    symLibInit ();                      /* symbol table */
    envLibInit (ENV_VAR_USE_HOOKS);     /* environment variables */
    sigInit ();                         /* signals */
    }



/******************************************************************************
*
* usrIosCoreInit - core I/O system
*/

void usrIosCoreInit (void)
    {
    mathHardInit ();                    /* hardware fpp support */
    iosInit (NUM_DRIVERS, NUM_FILES, "/null"); /* IO system */
    ttyDrv ();                          /* terminal driver */
    usrSerialInit ();                   /* SIO component */
    }



/******************************************************************************
*
* usrIosExtraInit - extended I/O system
*/

void usrIosExtraInit (void)
    {
    excShowInit ();                     /* exception show routines */
    excInit ();                         /* miscellaneous support task */
    logInit (consoleFd, MAX_LOG_MSGS);  /* message logging */
    pipeDrv ();                         /* pipes */
    stdioInit ();                       /* buffered IO library */
    fioLibInit ();                      /* formatting for printf, scanf, etc. */
    floatInit ();                       /* allow printf and others to format floats correctly */
    usrPassFsInit ();                   /* direct access to host filesystem */
    }



/******************************************************************************
*
* usrRoot - entry point for post-kernel initialization
*/

void usrRoot (char *pMemPoolStart, unsigned memPoolSize)
    {
    usrKernelCoreInit ();               /* core kernel facilities */
    memInit (pMemPoolStart, memPoolSize); /* full featured memory allocator */
    memPartLibInit (pMemPoolStart, memPoolSize); /* core memory partition manager */
    sysClkInit ();                      /* System clock component */
    selectInit (NUM_FILES);             /* select */
    usrIosCoreInit ();                  /* core I/O system */
    usrKernelExtraInit ();              /* extended kernel facilities */
    usrIosExtraInit ();                 /* extended I/O system */
    selTaskDeleteHookAdd ();            /* install select task delete hook */
    usrToolsInit ();                    /* software development tools */
    cplusCtorsLink ();                  /* run compiler generated initialization functions at system startup */
    usrAppInit ();                      /* call usrAppInit() (in your usrAppInit.c project file) after startup. */
    }

