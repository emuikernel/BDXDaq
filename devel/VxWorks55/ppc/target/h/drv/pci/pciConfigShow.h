/* pciConfigShow.h - PCI bus show routines*/

/* Copyright 1984-1998 Wind River Systems, Inc. */

/*
modification history
--------------------
01b,11mar98,tm   renamed to pciConfigShow.h from pciIomapShow.h
01a,04mar98,tm   derived from pciIomapLib.h v01m
*/

#ifndef __INCpciConfigShowh
#define __INCpciConfigShowh

#ifdef __cplusplus
extern "C" {
#endif


#ifndef _ASMLANGUAGE

#if defined(__STDC__) || defined(__cplusplus)

STATUS pciDeviceShow	(int busNo);
STATUS pciHeaderShow	(int busNo, int	deviceNo, int funcNo);
STATUS pciFindDeviceShow(int vendorId, int deviceId, int index);
STATUS pciFindClassShow	(int classCode, int index);

#else	/* __STDC__ */

STATUS pciDeviceShow	();
STATUS pciHeaderShow	();
STATUS pciFindDeviceShow();
STATUS pciFindClassShow	();

#endif	/* __STDC__ */

#endif	/* _ASMLANGUAGE */

#ifdef __cplusplus
}
#endif

#endif /* __INCpciConfigShowh */
