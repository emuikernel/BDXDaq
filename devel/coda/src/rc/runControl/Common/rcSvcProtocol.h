//-----------------------------------------------------------------------------
// Copyright (c) 1994,1995 Southeastern Universities Research Association,
//                         Continuous Electron Beam Accelerator Facility
//
// This software was developed under a United States Government license
// described in the NOTICE file included as part of this distribution.
//
// CEBAF Data Acquisition Group, 12000 Jefferson Ave., Newport News, VA 23606
//       coda@cebaf.gov  Tel: (804) 249-7030     Fax: (804) 249-5800
//-----------------------------------------------------------------------------
//
// Description:
//      CODA RunControl Server Protocol Header File
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: rcSvcProtocol.h,v $
//   Revision 1.5  1999/07/28 19:48:59  rwm
//   Added DARESUME as synchronous command for Pause Button
//
//   Revision 1.4  1998/11/06 18:42:10  timmer
//   Graham's recent rc changes
//
//   Revision 1.3  1997/02/03 13:47:01  heyes
//   add ask command
//
//   Revision 1.2  1996/11/04 16:14:49  chen
//   add options for monitoring components
//
//   Revision 1.1.1.1  1996/10/11 13:39:31  chen
//   run control source
//
//
#ifndef _CODA_RCSVC_PROTOCOL_H
#define _CODA_RCSVC_PROTOCOL_H

/* protocol version number                                          */
#define CODA_RC_PROTOCOL_VERSION 1
#define CODA_RC_MINOR_VERSION    0

/* Caller check version                                             */
#define CODA_RC_VERSION (1000*CODA_RC_PROTOCOL_VERSION + CODA_RC_MINOR_VERSION)

/* rcServer port: server listens at this port for UDP       message */
/* this port will be used only if the port number cannot be found   */
/* through environment variable "CODA_RC_PORT"                      */
#define RC_PORT        9001

/* the following commands are for run control server only           */
#define DAFINDSERVER   3000          /* client looking for server   */

#define MAXCLNT 30                   /* maximum number of clients   */

/* Synchronous command. server and client both use these to notify  */
#define DANULLPROC     ((u_long)0)   /* ping routine                */
#define DACREATE       ((u_long)1)   /* create objects              */
#define DAREMOVE       ((u_long)2)   /* remove all objects          */
#define DADOWNLOAD     ((u_long)3)   /* download all objects        */
#define DAPRESTART     ((u_long)4)   /* prestart all objects        */
#define DAEND          ((u_long)5)   /* end all processes           */
#define DAPAUSE        ((u_long)6)   /* pause all processes         */
#define DAGO           ((u_long)7)   /* activate all processes      */
#define DATERMINATE    ((u_long)8)   /* terminate all processes     */
#define DAREPORT       ((u_long)9)   /* server report msg to clients*/
#define DARESUME       ((u_long)10)  /* resume all objects          */
/* this used to be: */
/* #define DAREADINT      ((u_long)10)  /* obsolete                 */

#define DAREADSTRING   ((u_long)11)  /* obsolete                    */
#define DAREADREAL     ((u_long)12)  /* obsolete                    */
#define DAWRITEINT     ((u_long)13)  /* obsolete                    */
#define DAWRITESTRING  ((u_long)14)  /* obsolete                    */
#define DAWRITEREAL    ((u_long)15)  /* obsolete                    */

#define DAMODIFYINT    ((u_long)16)  /* obsolete                    */
#define DAMODIFYSTRING ((u_long)17)  /* obsolete                    */
#define DAMODIFYREAL   ((u_long)18)  /* obsolete                    */
#define DAWRITEEVENT   ((u_long)19)  /* obsolete                    */
#define DACONFIGURE    ((u_long)20)  /* configure the run type      */
#define DARESET        ((u_long)21)  /* reset all                   */
#define DABECOMEMASTER ((u_long)22)  /* request mastership          */
#define DACANCELMASTER ((u_long)23)  /* give up mastership          */
#define DAISMASTER     ((u_long)24)  /* check mastership            */
#define DASYNC         ((u_long)25)  /* synchronization             */
#define DAREADMESSAGES ((u_long)26)  /* obsolete                    */
#define DAINSERTEVENT  ((u_long)27)  /* insert event                */
#define DABCREPLY      ((u_long)28)  /* broadcast reply             */
#define DAREQEVENT     ((u_long)29)  /* obsolete                    */
#define DADUMP         ((u_long)30)  /* obsolete                    */
#define DAABORT        ((u_long)31)  /* abort                       */
#define DAONLINE       ((u_long)32)  /* server talk to remote procs */
#define DAOFFLINE      ((u_long)33)  /* server is off line          */
#define DACANCELTRAN   ((u_long)34)  /* cancel current transition   */
#define DAAUTOBOOT_INFO ((u_long)35) /* automatic boot components   */
#define DAMONITOR_PARM ((u_long)36)  /* monitor option parms        */


#define DANOMESSAGE    ((u_long)40)  /* client wants no messages    */
#define DASERVMSG      ((u_long)41)  /* Server messages             */

/* load Server side data base                                       */
#define DALOADDBASE    ((u_long)50)  /* load data base              */
#define DADISCONNECT   ((u_long)51)  /* client disconnect           */
#define DASESSION      ((u_long)52)  /* select a session            */
#define DACREATESES    ((u_long)53)  /* create a new session        */
#define DAUNKNOWN      ((u_long)100) /* unknown type for empty data */
#define DATEST         ((u_long)200) /* test server                 */
#define DAZAP          ((u_long)-1)  /* kill all processes          */

/* all monitor on/off operations                                             */
#define DAGET_VAL         ((u_long)2000)      /* get value                   */
#define DASET_VAL         ((u_long)2001)      /* set value                   */
#define DAMONITOR_VAL_ON  ((u_long)2002)      /* start monitor on            */
#define DAMONITOR_VAL_OFF ((u_long)2003)      /* start monitor off           */
#define DACHANGE_STATE    ((u_long)2004)      /* change state of server      */
#define DAREMOVE_VARS     ((u_long)2005)      /* remove dynamic variables    */
#define DAADD_VARS        ((u_long)2006)      /* add dynamic variables       */
#define DAREG_CLIENT_INFO ((u_long)2007)      /* reg client information      */
#define DAADD_ANALOG_VARS ((u_long)2008)      /* add anas log info           */
#define DAREMOVE_ANALOG_VARS ((u_long)2009)   /* invalidate ana log vars     */

/* direct communication */

#define DAASK             ((u_long)3000) /* ask an object to do something */
#define DATELL            ((u_long)3001) /* tell an object to do something */

/* Error Codes */

#define RC__SUCCESS         0
#define RC__RPC_FAILURE    -1
#define RC__NOT_REGISTERED -3
#define RC__NOT_MASTER     -4

/* constants from the DAC    */
#define DA_MAGIC         0x7030
/* define states possible... */
#define DA_DORMANT        0
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
#define DA_NOT_CONNECTED  -1

/* constants for internal use only */
#define MAX_STRING_LEN    40         /* maxumum string len           */
#define MAX_HOSTNAME_LEN  64         /* maximum machine name len     */
#define MAX_NUM_ARGS      8          /* maximum num of arguments for */
                                     /* one message to/from server   */

#define REPORTING_NONE    0          /* message reporting none       */
#define REPORTING_NORMAL  1          /* message reporting normal     */
#define REPORTING_TERSE   2          /* message reporting verbose    */

#define NUM_ATTR_DYN      3          /* number of attributes of      */

                                     /* dynamic variables            */
#ifdef _CODA_2_0_T
#define DYN_ATTR0         "nlongs"   /* attribute name               */
#else
#define DYN_ATTR0         "nlong"    /* attribute name               */
#endif
#define DYN_ATTR1         "nevents"  /* attribute name               */
#define DYN_ATTR2         "status"   /* attribute name               */
#define DYN_ATTR3         "erate"   /* attribute name               */
#define DYN_ATTR4         "drate"   /* attribute name               */

/*sergey*/
#define DYN_ATTR5         "livetime"

#define DYN_ANA_LOG       "log"      /* attribute name log for ANA   */
#endif

