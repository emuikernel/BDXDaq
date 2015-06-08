/* usrNetLib.c - network subsystem initialization */

/* Copyright 1992 - 2003 Wind River Systems, Inc. */

/*
modification history
--------------------
01g,20mar03,wap  move m2If64BitCounters to funcBind (SPR #86776)
01f,17jan03,wap  Only reference m2If64BitCounters when including
                 INCLUDE_MIB2_IF component.
01e,13jan03,vvv  merged from branch wrn_velocecp, ver01e (SPR #83330, 83342)
01d,23jan02,vvv  added arpMaxEntries
01c,09oct01,rae  merge from truestack (SPR #70476)
01b,27apr99,dat  SPR 27032, IP_MAX_UNITS was not effective
01a,05dec97,spm  added DHCP code review modifications
*/

/*
DESCRIPTION
This file is used to configure and initialize the VxWorks networking support.
The file is included in the project configuration file when INCLUDE_NET_SETUP 
is defined. It creates the network memory pools and the device control 
structure used by both END and BSD style drivers. It also initializes the 
interface index library.


NOMANUAL
*/

/* network buffers configuration */

/*
 * mBlk, clBlk configuration table for network stack data pool.
 * Only used for data transfer in the network stack.
 */
M_CL_CONFIG mClBlkConfig = 
    {
    /* 
    no. mBlks		no. clBlks	memArea		memSize
    -----------		----------	-------		-------
    */
    NUM_NET_MBLKS, 	NUM_CL_BLKS,	NULL, 		0
    };

/*
 * network stack data cluster pool configuration table
 * Only used for data transfer in the network stack.
 */
CL_DESC clDescTbl [] = 		
    {
    /* 
    clusterSize		num		memArea		memSize
    -----------		----		-------		-------
    */
    {64,		NUM_64,		NULL,		0},
    {128,		NUM_128,	NULL,		0},
    {256,		NUM_256,	NULL,		0},
    {512,		NUM_512,	NULL,		0},
    {1024,		NUM_1024,	NULL,		0},
    {2048,		NUM_2048,	NULL,		0}
    }; 
int clDescTblNumEnt = (NELEMENTS(clDescTbl));

/*
 * mBlk, clBlk configuration table for network stack system pool.
 * Used for network stack system structures such as routes, sockets,
 * protocol control blocks, interface addresses, mulitcast addresses,
 * and multicast routing entries.
 */
M_CL_CONFIG sysMclBlkConfig = 
    {
    /* 
    no. mBlks		no. clBlks		memArea		memSize
    -----------		----------		-------		-------
    */
    NUM_SYS_MBLKS, 	NUM_SYS_CL_BLKS,	NULL, 		0
    };

/*
 * network stack system cluster pool configuration table
 * Used for network stack system structures such as routes, sockets,
 * protocol control blocks, interface addresses, mulitcast addresses,
 * and multicast routing entries.
 */
CL_DESC sysClDescTbl [] = 		
    {
    /* 
    clusterSize		num		memArea		memSize
    -----------		----		-------		-------
    */
    {64,		NUM_SYS_64,	NULL,		0},
    {128,		NUM_SYS_128,	NULL,		0},
    {256,		NUM_SYS_256,	NULL,		0},
    {512,		NUM_SYS_512,	NULL,		0},
    }; 
int sysClDescTblNumEnt = (NELEMENTS(sysClDescTbl));

IP_DRV_CTRL        ipDrvCtrl[IP_MAX_UNITS];
int                ipMaxUnits = IP_MAX_UNITS;
IMPORT int         arpMaxEntries;

void usrNetLibInit ()
    {
    ifIndexLibInit ();
    arpMaxEntries = ARP_MAX_ENTRIES;
#ifdef INCLUDE_MIB2_IF
    m2If64BitCounters  = MIB2IF_64BIT_COUNTERS;
#endif /* INCLUDE_MIB2_IF */
    }
