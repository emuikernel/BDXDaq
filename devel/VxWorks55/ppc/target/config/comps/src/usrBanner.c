/* usrBanner.c - print the shell banner */

/* Copyright 1984-2001 Wind River Systems, Inc. */

/*
modification history
--------------------
01d,29jan02,g_h  Added WDB_COMM_VTMD as a possible WDB_COMM_TYPE_STR
01c,12sep01,jhw  Added WDB_PIPE as a possible WDB_COMM_TYPE_STR
		 Fixed display alignment of WDB Comm Type.
01b,21aug01,jhw  Added WDB_COMM_TYPE to banner (SPR 9559)
01a,02jun98,ms   written
*/

/*
DESCRIPTION

*/

#ifdef INCLUDE_WDB_COMM_NETWORK
#define WDB_COMM_TYPE_STR "WDB_COMM_NETWORK"
#endif /* INCLUDE_WDB_COMM_NETWORK */

#ifdef INCLUDE_WDB_COMM_SERIAL
#define WDB_COMM_TYPE_STR "WDB_COMM_SERIAL"
#endif /* INCLUDE_WDB_COMM_SERIAL */

#ifdef INCLUDE_WDB_COMM_TYCODRV_5_2
#define WDB_COMM_TYPE_STR "WDB_COMM_TYCODRV_5_2"
#endif /* INCLUDE_WDB_COMM_TYCODRV_5_2 */

#ifdef INCLUDE_WDB_COMM_NETROM
#define WDB_COMM_TYPE_STR "WDB_COMM_NETROM"
#endif /* INCLUDE_WDB_COMM_NETROM */

#ifdef INCLUDE_WDB_COMM_VTMD
#define WDB_COMM_TYPE_STR "WDB_COMM_VTMD"
#endif /* INCLUDE_WDB_COMM_VTMD */

#ifdef INCLUDE_WDB_COMM_END
#define WDB_COMM_TYPE_STR "WDB_COMM_END"
#endif /* INCLUDE_WDB_COMM_END */

#ifdef INCLUDE_WDB_COMM_CUSTOM
#define WDB_COMM_TYPE_STR "WDB_COMM_CUSTOM"
#endif /* INCLUDE_WDB_COMM_CUSTOM */

#ifdef INCLUDE_WDB_COMM_PIPE
#define WDB_COMM_TYPE_STR "WDB_COMM_PIPE"
#endif /* INCLUDE_WDB_COMM_PIPE */

#ifndef WDB_COMM_TYPE_STR
#define WDB_COMM_TYPE_STR "Unknown"
#endif /* WDB_COMM_TYPE_STR */

/******************************************************************************
*
* usrBanner - print the banner information to the Target Shell
*
* This routine prints out the banner information to the Target Shell.
*
* RETURNS :
* None
*
* NOMANUAL
*/
 
void usrBanner (void)
    {
    printLogo ();                               /* print out the banner page */

    printf ("                               ");
    printf ("CPU: %s.  Processor #%d.\n", sysModel (), sysProcNumGet ());
    printf ("                              ");
    printf ("Memory Size: 0x%x.",
		(UINT)(sysMemTop () - (char *)LOCAL_MEM_LOCAL_ADRS));
    printf ("  BSP version " BSP_VERSION BSP_REV ".\n");
#ifdef	INCLUDE_WDB
    printf ("                             WDB Comm Type: %s\n",
    	    WDB_COMM_TYPE_STR );
    printf ("                            WDB: %s.\n",
            ((wdbRunsExternal () || wdbRunsTasking ()) ?
                 "Ready" : "Agent configuration failed") );
#endif
    }

