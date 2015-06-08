/* usrWdbBanner.c - print banner to the console */

/* Copyright 1984-2002 Wind River Systems, Inc. */

/*
modification history
--------------------
01f,06may02,sbs  updating copyright
01e,29jan02,g_h  Added WDB_COMM_VTMD as a possible WDB_COMM_TYPE_STR
01d,12sep01,jhw  Added WDB_PIPE as a possible WDB_COMM_TYPE_STR
01c,21aug01,jhw  Added WDB_COMM_TYPE to banner (SPR 9559)
01b,01mar01,zl   updated copyright year.
01b,17jan01,sn   simplify job of host val  
01a,08oct97,ms   taken from usrWdb.c
*/

/*
DESCRIPTION
Print a short banner, like the bootPrintLogo banner
*/

/* Defines */

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
* usrWdbBanner - print the banner information to the Console 
*
* This routine prints out the banner information to the Console. 
*
* RETURNS :
* None 
*
* NOMANUAL
*/
 
void usrWdbBanner (void)
    {
#ifdef	INCLUDE_SHELL_BANNER
    return;
#else
    printf ("\n\n");
    printf ("%23s\n\n",runtimeName);
    printf ("Copyright 1984-2002  Wind River Systems, Inc.\n\n");
    printf ("            CPU: %s\n", sysModel ());
    printf ("   Runtime Name: %s\n", runtimeName);
    printf ("Runtime Version: %s\n", runtimeVersion);
    printf ("    BSP version: " BSP_VERSION BSP_REV "\n");
    printf ("        Created: %s\n", creationDate);
    printf ("  WDB Comm Type: %s\n", WDB_COMM_TYPE_STR );
    printf ("            WDB: %s.\n\n",
            ((wdbRunsExternal () || wdbRunsTasking ()) ?
                 "Ready" : "Agent configuration failed"));
#endif
    }
