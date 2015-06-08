/* prjConfig.c - dynamicaly generated configuration file */


/*
GENERATED: Wed Nov 05 11:58:01 EST 2003
DO NOT EDIT - file is regenerated whenever the project changes.
This file contains the non-BSP system initialization code
for Create a bootable VxWorks image (custom configured).
*/


/* includes */

#include "vxWorks.h"
#include "config.h"
#include "altivecLib.h"
#include "bsdSockLib.h"
#include "bufLib.h"
#include "cacheLib.h"
#include "configNet.h"
#include "cplusLib.h"
#include "dbgLib.h"
#include "drv/pci/pciConfigLib.h"
#include "drv/pci/pciConfigShow.h"
#include "drv/pci/pciHeaderDefs.h"
#include "drv/wdb/wdbEndPktDrv.h"
#include "drv/wdb/wdbVioDrv.h"
#include "end.h"
#include "envLib.h"
#include "eventLib.h"
#include "excLib.h"
#include "fioLib.h"
#include "fppLib.h"
#include "ftpLib.h"
#include "hashLib.h"
#include "hostLib.h"
#include "intLib.h"
#include "ioLib.h"
#include "iosLib.h"
#include "ipProto.h"
#include "loadElfLib.h"
#include "logLib.h"
#include "loginLib.h"
#include "lstLib.h"
#include "m2IfLib.h"
#include "math.h"
#include "memLib.h"
#include "moduleLib.h"
#include "msgQLib.h"
#include "muxLib.h"
#include "muxTkLib.h"
#include "net/mbuf.h"
#include "netBufLib.h"
#include "netDrv.h"
#include "netLib.h"
#include "netinet/if_ether.h"
#include "netinet/ppp/random.h"
#include "pingLib.h"
#include "pipeDrv.h"
#include "private/cplusLibP.h"
#include "private/excLibP.h"
#include "private/ftpLibP.h"
#include "private/funcBindP.h"
#include "private/kernelLibP.h"
#include "private/taskLibP.h"
#include "private/vmLibP.h"
#include "private/workQLibP.h"
#include "pthread.h"
#include "qPriBMapLib.h"
#include "rebootLib.h"
#include "remLib.h"
#include "resolvLib.h"
#include "sched.h"
#include "selectLib.h"
#include "semLib.h"
#include "shellLib.h"
#include "sigLib.h"
#include "sioLib.h"
#include "sntpcLib.h"
#include "spyLib.h"
#include "stdio.h"
#include "string.h"
#include "symLib.h"
#include "symSyncLib.h"
#include "sys/socket.h"
#include "sysLib.h"
#include "sysSymTbl.h"
#include "taskHookLib.h"
#include "taskLib.h"
#include "taskVarLib.h"
#include "tickLib.h"
#include "timexLib.h"
#include "ttyLib.h"
#include "usrConfig.h"
#include "usrLib.h"
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


/* imports */

IMPORT char etext [];                   /* defined by loader */
IMPORT char end [];                     /* defined by loader */
IMPORT char edata [];                   /* defined by loader */


/* BSP_STUBS */



/* configlettes */

#include "sysComms.c"
#include "../../../src/drv/pci/pciConfigShow.c"
#include "SaveRestVR.c"
#include "cplusgnuLang.c"
#include "intrinsics.c"
#include "net/routeSockCfg.c"
#include "net/usrBootLine.c"
#include "net/usrBsdSocket.c"
#include "net/usrDnsCfg.c"
#include "net/usrEndLib.c"
#include "net/usrFtp.c"
#include "net/usrNetBoot.c"
#include "net/usrNetBootUtil.c"
#include "net/usrNetConfigIf.c"
#include "net/usrNetEndBoot.c"
#include "net/usrNetIcmp.c"
#include "net/usrNetIpLib.c"
#include "net/usrNetLib.c"
#include "net/usrNetLoopbackStart.c"
#include "net/usrNetMuxCfg.c"
#include "net/usrNetRemoteCfg.c"
#include "net/usrNetShow.c"
#include "net/usrNetTelnetdCfg.c"
#include "net/usrNfs.c"
#include "net/usrTcp.c"
#include "net/usrUdp.c"
#include "sysClkInit.c"
#include "usrAltivec.c"
#include "usrBanner.c"
#include "usrBreakpoint.c"
#include "usrCache.c"
#include "usrCplus.c"
#include "usrKernel.c"
#include "usrMmuInit.c"
#include "usrScript.c"
#include "usrSecurity.c"
#include "usrSerial.c"
#include "usrStandalone.c"
#include "usrStartup.c"
#include "usrStatTbl.c"
#include "usrWdbAltivec.c"
#include "usrWdbBanner.c"
#include "usrWdbBp.c"
#include "usrWdbCore.c"
#include "usrWdbFpp.c"
#include "usrWdbSys.c"
#include "usrWdbSysFpp.c"
#include "usrWdbTask.c"
#include "usrWdbTaskAltivec.c"
#include "usrWdbTaskFpp.c"
#include "wdbEnd.c"


/******************************************************************************
*
* usrInit - pre-kernel initialization
*/

void usrInit (int startType)
    {
    sysStart (startType);               /* clear BSS and set up the vector table base address. */
    cacheLibInit (USER_I_CACHE_MODE, USER_D_CACHE_MODE); /* include cache support */
    excExtendedVectors = EXC_EXTENDED_VECTORS_ENABLED; /* Extended 32-bit vector branches to handlers */
    excVecInit ();                      /* exception handling */
    sysHwInit ();                       /* call the BSPs sysHwInit routine during system startup */
    usrCacheEnable ();                  /* optionally enable caches */
    usrKernelInit ();                   /* context switch and interrupt handling. DO NOT REMOVE. */
    }



/******************************************************************************
*
* usrNetProtoInit - Initialize the network protocol stacks
*/

void usrNetProtoInit (void)
    {
    usrBsdSockLibInit();                /* BSD Socket Support */
    routeSockInit();                    /* Allows manipulation of routing table with socket API */
    hostTblInit();                      /* Host Table Support */
    usrIpLibInit();                     /* BSD 4.4 IPv4 */
    udpLibInit (&udpCfgParams);         /* BSD 4.4. UDPv4 */
    udpShowInit();                      /* Initialize the UDP show facility */
    tcpLibInit (&tcpCfgParams);         /* BSD 4.4 TCPv4 */
    tcpShowInit();                      /* Initialize the TCP show facility */
    icmpLibInit (&icmpCfgParams);       /* BSD 4.4 ICMPv4 */
    igmpLibInit();                      /* BSD 4.4 IGMPv4 */
    netLibInit();                       /* creates the network task that runs low-level 			network interface routines in a task context */
    tcpTraceInit ();                    /* BSD 4.4 TCPv4 Debugging */
                                        /* Includes the ARP cache APIs */
    netShowInit();                      /* Initialize the Network show facility */
    }



/******************************************************************************
*
* usrNetworkAddrInit - Initialize the network address for a device
*/

void usrNetworkAddrInit (void)
    {
    usrDhcpcLeaseClean ();              /* Deletes DHCP time values from address field */
    }



/******************************************************************************
*
* usrNetworkDevStart - Attach a network device and start the loopback driver
*/

void usrNetworkDevStart (void)
    {
    usrNetEndDevStart (pDevName, uNum); /* Uses boot parameters to start an END driver */
    usrNetLoopbackStart ();             /* loopback interface for routining to localhost */
    }



/******************************************************************************
*
* usrNetworkBoot - Setup a network device using the boot parameters
*/

void usrNetworkBoot (void)
    {
    usrNetBoot ();                      /* Reads the enet address from the bootline parameters */
    usrNetworkAddrInit ();              /* Initialize the network address for a device */
    usrNetmaskGet ();                   /* Extracts netmask value from address field */
    usrNetDevNameGet ();                /* Gets name from "other" field if booting from disk */
    usrNetworkDevStart ();              /* Attach a network device and start the loopback driver */
    }



/******************************************************************************
*
* usrNetworkAddrCheck - Get an IP address if needed and setup the boot device
*/

void usrNetworkAddrCheck (void)
    {
    usrNetConfig (pDevName, uNum, pTgtName, pAddrString); /* Assigns an IP address and netmask */
    }



/******************************************************************************
*
* usrNetRemoteInit - 
*/

void usrNetRemoteInit (void)
    {
    usrNetHostSetup ();                 /* Route creation and hostname setup */
    usrNetRemoteCreate ();              /* Allows access to file system on boot host */
    usrNfsInit();                       /* Network file system (version 2) client */
    }



/******************************************************************************
*
* usrNetAppInit - 
*/

void usrNetAppInit (void)
    {
    rpcInit ();                         /* Remote Procedure Call system */
    rlogInit();                         /* Allows remote access to target */
    usrTelnetdStart ();                 /* Supports remote login using telnet protocol */
    usrSecurity ();                     /* rlogin and telnet password protection */
    usrFtpInit();                       /* File Transfer Protocol (FTP) library */
    sntpcInit (SNTP_PORT);              /* Simple network time protocol client */
    pingLibInit();                      /* ICMP (ping) client */
    usrDnsInit (DNS_DEBUG);             /* Domain name service client */
    }



/******************************************************************************
*
* usrNetworkInit - Initialize the network subsystem
*/

void usrNetworkInit (void)
    {
    usrNetLibInit ();                   /* network buffer creation and device support */
    usrNetProtoInit ();                 /* Initialize the network protocol stacks */
    usrMuxLibInit ();                   /* network driver to protocol multiplexer */
    usrEndLibInit();                    /* Support for network devices using MUX/END interface */
                                        /* This should always be included for backward compatibility */
    usrNetworkBoot ();                  /* Setup a network device using the boot parameters */
    usrNetworkAddrCheck ();             /* Get an IP address if needed and setup the boot device */
    usrNetRemoteInit ();                /* initialize network remote I/O access */
    usrNetAppInit ();                   /* initialize network application protocols */
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
    wdbAltivecInit ();                  /* Initialize WDB AltiVec debug support */
    wdbTaskAltivecInit ();              /* task mode AltiVec debug support */
    usrWdbBanner ();                    /* print banner to console after the agent is initialized */
    symSyncLibInit ();                  /* keep tgtsvr and target symbols tables synchronized. */
    }



/******************************************************************************
*
* usrShellInit - the target shell
*/

void usrShellInit (void)
    {
    dbgInit ();                         /* breakpoints and stack tracer on target. Not needed for remote debugging with tornado. */
    usrBanner ();                       /* display the WRS banner on startup */
    usrStartupScript (sysBootParams.startupScript); /* shell startup script */
    shellInit (SHELL_STACK_SIZE, TRUE); /* target shell */
    }



/******************************************************************************
*
* usrShowInit - enable object show routines
*/

void usrShowInit (void)
    {
    taskShowInit ();                    /* task show routine */
    memShowInit ();                     /* memory show routine */
    taskHookShowInit ();                /* task hook show routine */
    semShowInit ();                     /* semaphore show routine */
    msgQShowInit ();                    /* message queue show routine */
    symShowInit ();                     /* symbol table show routine */
    fppShowInit ();                     /* task floating point registers */
    stdioShowInit ();                   /* stdio show routine */
    altivecShowInit ();                 /* task AltiVec registers */
    }



/******************************************************************************
*
* usrToolsInit - software development tools
*/

void usrToolsInit (void)
    {
    spyLibInit ();                      /* utility to measure per-task CPU utilization */
    timexInit ();                       /* utility to measure function execution time */
    moduleLibInit ();                   /* support library for the target-based loader. */
    loadElfInit ();                     /* ELF loader */
    usrStandaloneInit ();               /* prefered method if not booting from the network. */
    usrStatTblInit ();                  /* table of error strings for perror() */
    usrWdbInit ();                      /* the WDB target agent */
    usrShellInit ();                    /* the target shell */
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
    pthreadLibInit ();	                 /* POSIX threads */
    usrAltivecInit();                   /* AltiVec register save/restore routines */
    }



/******************************************************************************
*
* usrIosCoreInit - core I/O system
*/

void usrIosCoreInit (void)
    {
    mathHardInit ();                    /* hardware fpp support */
    usrBootLineParse (BOOT_LINE_ADRS);  /* parse some boot device configuration info  */
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
    usrMmuInit ();                      /* basic MMU component */
    sysClkInit ();                      /* System clock component */
    selectInit (NUM_FILES);             /* select */
    usrIosCoreInit ();                  /* core I/O system */
    usrKernelExtraInit ();              /* extended kernel facilities */
    usrIosExtraInit ();                 /* extended I/O system */
    usrNetworkInit ();                  /* Initialize the network subsystem */
    selTaskDeleteHookAdd ();            /* install select task delete hook */
    usrToolsInit ();                    /* software development tools */
    cplusCtorsLink ();                  /* run compiler generated initialization functions at system startup */
    usrCplusLibInit ();                 /* Basic support for C++ applications */
    cplusDemanglerInit ();              /* support library for target shell and loader: provides human readable forms of C++ identifiers */
    usrAppInit ();                      /* call usrAppInit() (in your usrAppInit.c project file) after startup. */
    }

