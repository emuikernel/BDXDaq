/*************************************************************************\
* Copyright (c) 2002 The University of Chicago, as Operator of Argonne
*     National Laboratory.
* Copyright (c) 2002 The Regents of the University of California, as
*     Operator of Los Alamos National Laboratory.
* EPICS BASE Versions 3.13.7
* and higher are distributed subject to a Software License Agreement found
* in file LICENSE that is included with this distribution. 
\*************************************************************************/
/*epicsExit.h*/
#ifndef epicsExith
#define epicsExith
#include <shareLib.h>

#ifdef __cplusplus
extern "C" {
#endif

epicsShareFunc void epicsShareAPI epicsExit(int status);
epicsShareFunc void epicsShareAPI epicsExitCallAtExits(void);
epicsShareFunc int epicsShareAPI epicsAtExit(
                 void (*epicsExitFunc)(void *arg),void *arg);

epicsShareFunc void epicsShareAPI epicsExitCallAtThreadExits(void);
epicsShareFunc int epicsShareAPI epicsAtThreadExit(
                 void (*epicsExitFunc)(void *arg),void *arg);


#ifdef __cplusplus
}
#endif

#endif /*epicsExith*/
