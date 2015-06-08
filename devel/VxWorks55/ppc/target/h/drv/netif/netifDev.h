/* netifDev.h - generic network interface device driver header */

/* Copyright 1994-1996 Wind River Systems, Inc. */

/*
modification history
--------------------
01e,11jul96,map  added MBC cluster type.
01d,28may96,dzb  added CPM cluster type.
01c,07feb96,dzb  added NIC_EVB cluster type.
01b,22mar94,dzb  added SLIP cluster type.
01a,13oct93,dzb  created.
*/

/*
DESCRIPTION

This file contains generic header information for network interface device
drivers.
*/

#ifndef __INCnetifDevh
#define __INCnetifDevh

#ifdef __cplusplus
extern "C" {
#endif

#ifndef	_ASMLANGUAGE

/* defines */

/* device specific cluster types - used for buffer loaning */

#define MC_LANCE	((u_char) 2)	/* ln cluster type */
#define MC_BACKPLANE	((u_char) 3)	/* backplane cluster type */
#define MC_EI		((u_char) 4)	/* ei cluster type */
#define MC_QU		((u_char) 5)	/* qu cluster type */
#define MC_SL		((u_char) 6)	/* sl cluster type */
#define MC_NIC_EVB	((u_char) 7)	/* nicEvb cluster type */
#define MC_CPM		((u_char) 8)	/* cpm cluster type */
#define MC_MBC		((u_char) 9)	/* mbe cluster type */

#endif	/* _ASMLANGUAGE */

#ifdef __cplusplus
}
#endif

#endif /* __INCnetifDevh */
