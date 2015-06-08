/*----------------------------------------------------------------------------*
 *  Copyright (c) 1991, 1992  Southeastern Universities Research Association, * 
 *                            Continuous Electron Beam Accelerator Facility   *
 *                                                                            *
 *    This software was developed under a United States Government license    *
 *    described in the NOTICE file included as part of this distribution.     *
 *                                                                            *
 * CEBAF Data Acquisition Group, 12000 Jefferson Ave., Newport News, VA 23606 *
 *      heyes@cebaf.gov   Tel: (804) 249-7030    Fax: (804) 249-7363          *
 *----------------------------------------------------------------------------*
 * Discription: follows this header.
 *
 * Author:
 *	Graham Heyes
 *	CEBAF Data Acquisition Group
 *
 *----------------------------------------------------------------------------*/

#ifndef _CODA_OBJ_H
#define	_CODA_OBJ_H

/*
#include "rc.h"
*/

#define DA_MAGIC         0x7030
/* define states possible... */
#define DA_UNKNOWN        0
#define DA_BOOTING        1
#define DA_BOOTED         2
#define DA_CONFIGURING    3
#define DA_CONFIGURED     4
#define DA_DOWNLOADING    5
#define DA_DOWNLOADED     6
#define DA_PRESTARTING    7
#define DA_PAUSED         8
#define DA_PAUSING        9
#define DA_ACTIVATING     10
#define DA_ACTIVE         11
#define DA_ENDING         12
#define DA_VERIFYING      13
#define DA_VERIFIED       14
#define DA_TERMINATING    15
#define DA_PRESTARTED     16
#define DA_RESUMING       17
#define DA_STATES         18

static char *sN[DA_STATES] =
{
  "null",			/* 0 */
  "booting",		/* 1 */
  "booted",			/* 2 */
  "initing",	    /* 3 */
  "inited",			/* 4 */
  "loading",    	/* 5 */
  "loaded",			/* 6 */
  "prestarting",	/* 7 */
  "paused",			/* 8 */
  "pausing",		/* 9 */
  "activating",		/* 10 */
  "active",			/* 11 */
  "ending",			/* 12 */
  "verifying",		/* 13 */
  "verified",		/* 14 */
  "terminating",    /* 15 */
  "terminated",     /* 16 */
  "resuming"        /* 17 */
};

/* Convert a state number into a state name */

#define rcStates(state) (state<=DA_STATES)?(sN[state]):"UNKNOWN"; 

/* define possible state actions for historical reasons */

#define DA_CONFIGURE     DACONFIGURE
#define DA_DOWNLOAD      DADOWNLOAD
#define DA_PRESTART      DAPRESTART
#define DA_END           DAEND
#define DA_PAUSE         DAPAUSE
#define DA_GO            DAGO
#define DA_TERMINATE     DATERMINATE
#define DA_RESET         DARESET
#define DA_REPORT        DAREPORT
#define DA_BECOMEMASTER  DABECOMEMASTER
#define DA_CANCELMASTER  DACANCELMASTER
#define DA_ISMASTER      DAISMASTER
#define DA_READMESSAGES  DAREADMESSAGES

static char *action_Names[] = 
{
  "null",
  "create",
  "remove",
  "download",
  "prestart",
  "end",
  "pause",
  "go",
  "terminate",
  "status",
  "readint",
  "readstring",
  "readreal",
  "writeint",
  "writestring",
  "writereal",
  "modifyint",
  "modifystring",
  "modifyreal",
  "writeevent",
  "configure",
  "reset",
  "becomemaster",
  "cancelmaster",
  "ismaster",
  "sync",
  "readmessages",
  "insertevent",
  "broadcastreply",
  "requestevent",
  "dump",
  NULL
  };

#define DA_CLASSES      12
#define STREAM_SOCKETS   1
#define STREAM_INTERNAL  2
#define STREAM_INPUT     0
#define STREAM_OUTPUT    4

#define TS_CLASS         2
#define ROC_CLASS        3
#define EB_CLASS         4
#define ANA_CLASS        5
#define RCS_CLASS        6

typedef struct rcMsgStruct *rcMsg;
typedef struct rcMsgStruct
{
  long type;
  int action;
  char *p1;
  char *p2;
  char *p3;
} RCMSG;


typedef struct classStruct *objClass;
/*typedef struct classStruct SERVICE;*/
typedef struct classStruct
{
  int *interp_obsolete;		/* Tcl interpreter */
  char *className;		/* name of class */
  char *name;			/* name of this object */
  char *state;			/* name of this object */
  int  codaid;			/* id number of object */
  int  nlongs;			/* number of data words processed */
  int  nevents;			/* number of events processed */

  char *rolNames;
  char *runName;
  int runType;
  int runNumber;

  int fd;			/* who to send data to */

  void *privated;

} objClassStore;

/* _CODA_OBJ_H */
#endif
