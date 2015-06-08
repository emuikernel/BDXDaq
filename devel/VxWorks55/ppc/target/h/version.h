/* version.h - VxWorks version information */

/* Copyright 1984-1998 Wind River Systems, Inc. */

/*
modification history
--------------------
03c,25mar03,sfp  update for CP-1 release: VxWorks 5.5.1
03b,12sep01,fmk  change release to 5.5
03a,17jan01,sn   simplify job of host val  
03a,08dec00,cmf  changed release to 5.4.2
02z,08jul98,ms   changed release to 5.4.
02y,02oct96,p_m  changed release to 5.3.1.
                 removed pre 02r history.
02x,15mar96,p_m  changed release to 5.3 for everything except 960 K.
02w,06mar96,dat  removed length macros, no longer needed
02v,23jan96,tpr  changed release to 5.3-PowerPC-beta
02t,06jun95,caf  changed release to 5.2-PowerPC-beta.
02u,25oct95,ms	 changed release to 5.3
02t,21jun95,ms	 changed release to 5.3-beta
02s,29mar95,kdl  changed release to 5.2.
02r,12nov94,dvs  changed release to 5.2-beta
*/

#ifndef __INCversionh
#define __INCversionh

#ifdef __cplusplus
extern "C" {
#endif

#define RUNTIME_NAME "VxWorks"
#define RUNTIME_VERSION "5.5.1"
#define VXWORKS_VERSION RUNTIME_NAME RUNTIME_VERSION

IMPORT char * creationDate;
IMPORT char * runtimeVersion;
IMPORT char * runtimeName;
IMPORT char * vxWorksVersion;

#ifdef __cplusplus
}
#endif

#endif /* __INCversionh */
