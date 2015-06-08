/* ifLib.h - network interface library header */

/* Copyright 1984 - 2001 Wind River Systems, Inc. */

/*
modification history
--------------------
01e,10oct01,rae  merge from truestack ver 01j, base 01d
01d,06oct98,ham  added ifAddrAdd.
01c,02sep93,elh  changed param name to ifunit
01b,22sep92,rrr  added support for c++
01a,04jul92,jcf  cleaned up.
01b,26may92,rrr  the tree shuffle
01a,10dec91,gae  written.
	   +rrr
*/

#ifndef __INCifLibh
#define __INCifLibh

#ifdef __cplusplus
extern "C" {
#endif


/* function declarations */

#if defined(__STDC__) || defined(__cplusplus)
extern STATUS   ifAddrAdd (char *interfaceName, char *interfaceAddress,
                           char *broadcastAddress, int subnetMask);
extern STATUS 	ifAddrSet (char *interfaceName, char *interfaceAddress);
extern STATUS 	ifAddrDelete (char *interfaceName, char *interfaceAddress);
extern STATUS 	ifAddrGet (char *interfaceName, char *interfaceAddress);
extern STATUS 	ifBroadcastSet (char *interfaceName, char *broadcastAddress);
extern STATUS 	ifBroadcastGet (char *interfaceName, char *broadcastAddress);
extern STATUS 	ifDstAddrSet (char *interfaceName, char *dstAddress);
extern STATUS 	ifDstAddrGet (char *interfaceName, char *dstAddress);
extern STATUS 	ifMaskSet (char *interfaceName, int netMask);
extern STATUS 	ifMaskGet (char *interfaceName, int *netMask);
extern STATUS 	ifFlagChange (char *interfaceName, int flags, BOOL on);
extern STATUS 	ifFlagSet (char *interfaceName, int flags);
extern STATUS 	ifFlagGet (char *interfaceName, int *flags);
extern STATUS 	ifMetricSet (char *interfaceName, int metric);
extern STATUS 	ifMetricGet (char *interfaceName, int *pMetric);
extern int 	ifRouteDelete (char *ifName, int unit);
extern struct 	ifnet *ifunit (char *ifname);
extern struct	ifnet *ifIndexToIfpNoLock (int ifIndex);
extern struct	ifnet *ifIndexToIfp (int ifIndex);
extern unsigned short	ifNameToIfIndex (char *ifname);
extern STATUS	ifIndexToIfName (unsigned short ifIndex, char *ifname);
extern STATUS	ifUnnumberedSet(char *, char *, char *, char *);
extern int 	ifAllRoutesDelete (char *, int);
#else
extern STATUS   ifAddrAdd ();
extern STATUS 	ifAddrSet ();
extern STATUS 	ifAddrDelete ();
extern STATUS 	ifAddrGet ();
extern STATUS 	ifBroadcastSet ();
extern STATUS 	ifBroadcastGet ();
extern STATUS 	ifDstAddrSet ();
extern STATUS 	ifDstAddrGet ();
extern STATUS 	ifMaskSet ();
extern STATUS 	ifMaskGet ();
extern STATUS 	ifFlagChange ();
extern STATUS 	ifFlagSet ();
extern STATUS 	ifFlagGet ();
extern STATUS 	ifMetricSet ();
extern STATUS 	ifMetricGet ();
extern int 	ifRouteDelete ();
extern struct 	ifnet *ifunit ();
extern struct	ifnet *ifIndexToIfpNoLock ();
extern struct	ifnet *ifIndexToIfp ();
extern unsigned short	ifNameToIfIndex ();
extern STATUS	ifIndexToIfName ();
extern STATUS	ifUnnumberedSet ();
extern int 	ifAllRoutesDelete ();

#endif	/* __STDC__ */
#ifdef __cplusplus
}
#endif

#endif /* __INCifLibh */
