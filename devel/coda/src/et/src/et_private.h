/*----------------------------------------------------------------------------*
 *  Copyright (c) 1998        Southeastern Universities Research Association, *
 *                            Thomas Jefferson National Accelerator Facility  *
 *                                                                            *
 *    This software was developed under a United States Government license    *
 *    described in the NOTICE file included as part of this distribution.     *
 *                                                                            *
 *    Author:  Carl Timmer                                                    *
 *             timmer@jlab.org                   Jefferson Lab, MS-12H        *
 *             Phone: (757) 269-5130             12000 Jefferson Ave.         *
 *             Fax:   (757) 269-5800             Newport News, VA 23606       *
 *                                                                            *
 *----------------------------------------------------------------------------*
 *
 * Description:
 *      Header file for ET system
 *
 *----------------------------------------------------------------------------*/

#ifndef __et_private_h
#define __et_private_h

#include <stdlib.h>
#include <stdarg.h>
#include <limits.h>
#include "et.h"

#ifdef	__cplusplus
extern "C" {
#endif

/* Version Number of this ET software package release */
#define ET_VERSION 8      /* treated as an int */
#define ET_MINORVERSION 0 /* treated as an int */

/* Language of ET software package - implementation 
 * varies a bit from language to language.
 */
#define ET_LANG_C     0      /* C    */
#define ET_LANG_CPP   1      /* C++  */
#define ET_LANG_JAVA  2      /* Java */

/*
 * String length of dotted-decimal, ip address string
 * Some systems - but not all - define INET_ADDRSTRLEN
 * ("ddd.ddd.ddd.ddd\0" = 16)
 */
#define ET_IPADDRSTRLEN 16

/*
 * MAXHOSTNAMELEN is defined to be 256 on Solaris and 64 on Linux.
 * We need it to be uniform across all platforms since we transfer
 * this info across the network. Define it to be 256 for everyone.
 */
#define ET_MAXHOSTNAMELEN 256


/******************************************************************
 *      items to handle multiple network addresses or names       *
 ******************************************************************/
/* max # of network addresses/names per host we'll examine */
#define ET_MAXADDRESSES 10

/* In the following structures, 
 * count is the current number of addresses/names
 * addr  is the aray of addresses
 * name  is the array of names
 * tid   is the array of pthread id's used for cancelling threads
 *       started in et_cast_thread
 */
/* struct to handle multiple multicast addresses */
typedef struct et_mcastaddrs_t {
  int       count;
  char	    addr[ET_MAXADDRESSES][ET_IPADDRSTRLEN];
  pthread_t tid[ET_MAXADDRESSES];
} et_mcastaddrs;

/* struct to handle multiple subnet addresses */
typedef struct et_subnets_t {
  int       count;
  char      addr[ET_MAXADDRESSES][ET_IPADDRSTRLEN];
  pthread_t tid[ET_MAXADDRESSES];
} et_subnets;

/* struct to handle multiple network interface addresses */
typedef struct et_ifaddrs_t {
  int       count;
  char      addr[ET_MAXADDRESSES][ET_IPADDRSTRLEN];
  pthread_t tid[ET_MAXADDRESSES];
} et_ifaddrs;

/* struct to handle multiple network interface (host)names */
typedef struct et_ifnames_t {
  int   count;
  char	name[ET_MAXADDRESSES][ET_MAXHOSTNAMELEN];
} et_ifnames;


/****************************************
 * times for heart beating & monitoring *
 ****************************************/
/* 1.6 sec */
#define ET_MON_SEC    1
#define ET_MON_NSEC   600000000
/* 0.5 sec */
#define ET_BEAT_SEC   0
#define ET_BEAT_NSEC  500000000

/* see "Programming with POSIX threads' by Butenhof */
#define err_abort(code,text) do { \
    fprintf (stderr, "%s at \"%s\":%d: %s\n", \
        text, __FILE__, __LINE__, strerror (code)); \
    exit (-1); \
    } while (0)

/* big integer for statistical counting, use 64 bits */
typedef struct et_bigint_t {
  unsigned int  highint;
  unsigned int  lowint;
} et_bigint;

/* max value for heartbeats */
#define ET_HBMODULO UINT_MAX

/* # of extra POSIX threads ET client/producer will start */
#define ET_EXTRA_THREADS 2

/*
 * Maximum # of user attachments/processes on the system.
 * This is used only to allocate space in the system structure
 * for attachment/process housekeeping. The actual maximum #s of
 * attachments & processes are passed in a et_sys_config structure
 * to et_system_start.
 *
 * The number of attachments is the number of "et_station_attach"
 * calls successfully returned - each providing an access to events
 * from a certain station. A unix process may make several attachments
 * to one or more stations on one or more ET systems.
 *
 * The number of processes is just the number of "et_open" calls
 * successfully returned on the same host as the ET system. The
 * system must be one which can allow many processes to share the
 * ET mutexes, thus allowing full access to the shared memory. On
 * Linux this is not possible, meaning that the local user does NOT
 * have the proper access to the shared memory.
 * 
 * Since a unix process may only call et_open once, as things now stand,
 * the number of processes is a count of the number of unix processes
 * on Solaris opened locally.
 */
#define ET_ATTACHMENTS_MAX 110
#define ET_PROCESSES_MAX   ET_ATTACHMENTS_MAX

/* status of an attachment to an ET system */
#define ET_ATT_UNUSED 0		/* attachment # not in use */
#define ET_ATT_ACTIVE 1		/* attached to a station */

/* values to tell attachment to return prematurely from ET routine */
#define ET_ATT_CONTINUE 0	/* situation normal */
#define ET_ATT_QUIT     1	/* return immediately */

/* values telling whether an attachment is blocked in a read or not */
#define ET_ATT_UNBLOCKED 0	/* not blocked */
#define ET_ATT_BLOCKED   1	/* blocked */

/* values telling whether an attachment is in simulated sleep mode or not */
#define ET_ATT_NOSLEEP 0	/* not in sleep mode */
#define ET_ATT_SLEEP   1	/* simultated sleep mode */

/* status of a process in regards to an ET system */
#define ET_PROC_CLOSED 0	/* closed to ET systems - no access */
#define ET_PROC_OPEN   1	/* opened to ET system  - access to mapped mem */

/* what a process thinks of its ET system's status */
#define ET_PROC_ETDEAD 0	/* ET system it is connected to is dead */
#define ET_PROC_ETOK   1	/* ET system it is connected to is OK   */

/* certain structures need to be initialized before use */
#define ET_STRUCT_NEW 0		/* struct is newly created and not yet initialized */
#define ET_STRUCT_OK  1		/* struct is initialized and ready for use */

/* values to tell thread to self-destruct or to stick around */
#define ET_THREAD_KEEP 0	/* keep thread around */
#define ET_THREAD_KILL 1	/* remove thread */

/* tells if operating sys can share pthread mutexes between processes */
#define ET_MUTEX_SHARE   0	/* can share mutexes */
#define ET_MUTEX_NOSHARE 1	/* cannot share */

/* is mutex locked or not */
#define ET_MUTEX_UNLOCKED 0
#define ET_MUTEX_LOCKED   1

/*
 * STRUCTURES for the STATIONs:
 * --------------------------
 *  structures          purpose
 * ____________        ________________
 *  et_stat_config	station behavior parameters
 *  et_stat_data	station information
 *  et_event		single event
 *  et_list		list of events
 *  et_station		processes can "attach" to station
 *                      to get and put events
 */

/*
 * et_stat_config: parameters to define a station
 *-----------------------------
 * init		: ET_STRUCT_OK if structure properly initialized, else ET_STRUCT_NEW
 * flow_mode	: ET_STATION_PARALLEL if station part of a group of stations through
 *		: which events flow in parallel instead of through each one.
 *		: ET_STATION_SERIAL is default as events flow through each station.
 * user_mode	: multiple or single process(es) can connect
 * restore_mode	: if process dies, events it read but didn't write can be sent to:
 *		: 1) station's ouput with ET_STATION_RESTORE_OUT,
 *		: 2) station's input with ET_STATION_RESTORE_IN, or
 *		: 3) grand_central station with ET_STATION_RESTORE_GC
 * block_mode	: ET_STATION_BLOCKING means station blocks (accepts every event which
 *		: meets its condition) or
 *		: ET_STATION_NONBLOCKING means it doesn't (keeps some in a cue)
 * prescale	: for blocking,    get every Nth event
 * cue		: for nonblocking, # events desired in input list
 * select_mode  : ET_STATION_SELECT_ALL      - accepts every event (no conditions)
 *		: ET_STATION_SELECT_MATCH    - events' control array must match stations'
 *		:                              select array by predefined rules
 *		: ET_STATION_SELECT_USER     - user supplies lib & func to select events
 *		: ET_STATION_SELECT_RROBIN   - round robin distribution of events to
 *		:                              parallel stations
 *		: ET_STATION_SELECT_EQUALCUE - equal-number-of-events-in-cue distribution
 *		:                              algorithm for parallel stations
 * select	: array of ints for user's event selection
 *
 * for "C" systems, fname & lib are used to dynamically load code
 *
 * fname	: if user is providing a routine to define the
 *		: condition of event selection, this is its name
 * lib		: name of shared lib containing above routine
 *
 * for JAVA systems, a class name is used to dynamically load code
 *
 * classs	: name of JAVA class containing method to define the
 *		: condition of event selection (extra s is NOT misspelling)
 */

/* for event selection function */
typedef int (*ET_SELECT_FUNCPTR) (void *, et_stat_id, et_event *);

typedef struct et_stat_config_t {
  int  init;
  int  flow_mode;
  int  user_mode;
  int  restore_mode;
  int  block_mode;
  int  prescale;
  int  cue;
  int  select_mode;
  int  select[ET_STATION_SELECT_INTS];
  char fname[ET_FUNCNAME_LENGTH];
  char lib[ET_FILENAME_LENGTH];
  char classs[ET_FILENAME_LENGTH];
} et_stat_config;

/*
 * et_stat_data : current state of station
 *-----------------------------
 * status	: active, idle, creating, or unused
 * pid_create	: pid of process that created station
 * nattachments	: # of attachments to this station
 * att		: array in which the unique id# of an attachment
 *		: is the index (=id# if attached, -1 otherwise)
 * lib_handle   : handle for opened shared lib for user defined
 *		: event selection routine
 * func		: pointer to user's event selection routine
 */
 
typedef struct et_stat_data_t {
  int   status;
  int   pid_create;
  int   nattachments;
  int	att[ET_ATTACHMENTS_MAX];
  void *lib_handle;
  ET_SELECT_FUNCPTR func;
} et_stat_data;

/*
 * et_list: station's input or output list of events
 *-----------------------------
 * mutex	: protects linked list when reading & writing
 * cread	: condition var. to notify reader that events are here
 * cnt		: # events in list
 * lasthigh	: place in list of last high priority event
 * events_try	: # of events tried to put in (before prescale)
 * events_in;	: # of events actually put in
 * events_out;	: # of events actually sent out
 * firstevent	: pointer to first event in linked list
 * lastevent	: pointer to last  event in linked list
 */
 
typedef struct et_list_t {
  pthread_mutex_t  mutex;
  pthread_cond_t   cread;
  int              cnt;
  int              lasthigh;
  et_bigint        events_try;
  et_bigint        events_in;
  et_bigint        events_out;
  et_event        *firstevent;
  et_event        *lastevent;
} et_list;

/*
 * et_fix: struct to fix station's input and output linked lists after crash
 *--------------------------------------------------------------
 * first	: pl->firstevent at start of read (NULL if no damage)
 * start	: = 1 at start of write (0 at end)
 * cnt		: pl->cnt at start of read/write
 * num		: num of events intended to read/write
 * call		: =ET_FIX_DUMP if fixing after et_station_(n)dump call or
 *		: =ET_FIX_READ if fixing after et_station_(n)read call
 * eventsin	: value of pl->events_in at start of dump
 */
 
#define ET_FIX_READ 0
#define ET_FIX_DUMP 1

/* struct for fixing input list */
struct et_fixin {
  et_event	*first;
  int		start;
  int		cnt;
  int		num;
  int		call;
  et_bigint	eventsin;
};

/* struct for fixing output list */
struct et_fixout {
  int		start;
  int		cnt;
  int		num;
};

struct et_fix {
  struct et_fixin  in;
  struct et_fixout out;
};

/*
 * et_station:
 *-----------------------------
 * mutex	: mutex used for keeping the linked list of used
 *		: stations in order (for event transfers).
 * num		: unique id # (only if station being used)
 *		: = 0 for first station (grandcentral), = 1 for next
 *		: station in mapped memory, etc.
 * name		: unique name
 * config	: configuration specs
 * data		: status, info
 * list_in	: linked list containing events to read
 * list_out	: linked list containing events to be written
 * conductor	: flag to kill conductor thread when station deleted 
 *		: = ET_THREAD_KILL else ET_THREAD_KEEP
 * fix		: info to repair station's lists after user crash
 * next		: next active or idle station in et station chain,
 *		: Not storing this as a pointer makes for an awkward
 *		: linked list, but it survives mapping the shared
 *		: memory to a different spot.
 *		: For last station this is -1.
 * prev		: previous active or idle station in et station chain,
 *		: For first station this is -1.
 * nextparallel	: if this station is in a group of parallel stations,
 *		: this is a "pointer" to the next parallel station.
 *		: For last station this is -1.
 * prevparallel	: if this station is in a group of parallel stations,
 *		: this is a "pointer" to the previous parallel station.
 *		: For first station this is -1.
 * waslast	: flag which =1 if this station was last one to receive
 *		: an event when using the round-robin selection method
 *		: for a parallel group of stations. (else =0).
 */

/*
 * Use et_stat_id as a user's handle on a station instead
 * of a pointer to the station itself. It allows
 * a fast calculation of a pointer to station.
 */

typedef struct et_station_t {
  pthread_mutex_t       mutex;
  et_stat_id            num;
  char                  name[ET_STATNAME_LENGTH];
  et_stat_config        config;
  et_stat_data          data;
  et_list               list_in;
  et_list               list_out;
  int                   conductor;
  struct et_fix	        fix;
  et_stat_id	        next;
  et_stat_id	        prev;
  et_stat_id	        nextparallel;
  et_stat_id	        prevparallel;
  int                   waslast;
} et_station;

/*
 * et_proc: contains process info
 *----------------------------------------------------------
 * num		: unique index # of this process
 * att		: array in which an element (indexed by the
 *		: unique id# of an attachment owned by this process)
 *		: gives the id# of the attachment and -1 otherwise.
 * nattachments	: # of attachments to an ET system in this process
 * status	: open or connected to ET system (ET_PROC_OPEN),
 *		: or closed/unconnected (ET_PROC_CLOSED)
 * et_status	: knows ET system is dead (ET_PROC_ETDEAD),
 *		: or not (ET_PROC_ETOK)
 * heartbeat	: incremented to tell ET system it's alive
 * pid		: unix process id
 * hbeat_thd_id	: heartbeat thread id
 * hmon_thd_id	: heart monitor thread id
 */
 
struct et_proc {
  et_proc_id    num;
  et_att_id     att[ET_ATTACHMENTS_MAX];
  int		nattachments;
  int           status;
  int		et_status;
  unsigned int  heartbeat;
  pid_t         pid;
  pthread_t     hbeat_thd_id;
  pthread_t     hmon_thd_id;
};

/*
 * et_attach: contains attachment info
 *----------------------------------------------------------
 * num		: unique index # of this attachment
 * proc		: unique index # of process owning this attachment
 * stat		: unique index # of station we're attached to
 * status	: unused or active (ET_ATT_UNUSED, ET_ATT_ACTIVE)
 * blocked	: = ET_ATT_BLOCKED if blocked waiting to read events,
 *		: else = ET_ATT_UNBLOCKED
 * quit		: = ET_ATT_QUIT means return from ET routine
 *		: = ET_ATT_CONTINUE means things are OK
 * sleep	: if = ET_ATT_SLEEP, then attachment is remote and is sleeping
 *		: when getting events. It is simulated by multiple timed waits.
 *		: This causes trouble when waking attachments and must be
 *		: accounted for.
 *		: if = ET_ATT_NOSLEEP, no simulated remote sleeping mode.
 * events_put	: # of events it puts to   station output list
 * events_get	: # of events it gets from station input  list
 * events_dump	: # of events it dumps back into GrandCentral (recycles)
 * events_make	: # of new events it requests
 * pid		: unix process id# of process that owns attachment
 * host		: hostname running process that owns attachment
 */
 
struct et_attach {
  et_att_id   num;
  et_proc_id  proc;
  et_stat_id  stat;
  int         status;
  int         blocked;
  int         quit;
  int         sleep;
  et_bigint   events_put;
  et_bigint   events_get;
  et_bigint   events_dump;
  et_bigint   events_make;
  pid_t       pid;
  char        host[ET_MAXHOSTNAMELEN];
};

/*
 * et_sys_config  : Contains all info necessary to configure
 *		  : an ET system.
 *----------------------------------------------------------
 * init		  : =ET_STRUCT_OK if structure properly initialized
 * nevents	  : total # of events
 * event_size	  : event size in bytes
 * ntemps	  : max # of temporary events allowed (<= nevents)
 * nstations	  : max # of stations allowed
 * nprocesses	  : max # of processes allowed to open ET system
 * nattachments	  : max # of attachments to stations allowed
 * filename	  : name of the ET system file
 * port		  : broad/multicast port # for udp message
 * serverport	  : port # for ET system's tcp server thread
 * subnets	  : list of all local subnet broadcast addrs (dotted-decimal)
 * ifaddrs	  : list of all local network interfaces' addrs (dotted-dec)
 * ifnames	  : list of all interface hostnames corresponding to the IP
 *                : addresses listed in "ifaddrs". May be duplicates as several
 *                : IP addresses may be associated with the same hostname. Thus
 *                : there are "ifaddr.count" number of array members.
 * mcastaddrs	  : list of all multicast addresses to listen on (dotted-dec)
 */
 
typedef struct  et_sys_config_t {
  int	          init;
  int	          nevents;
  int	          event_size;
  int	          ntemps;
  int	          nstations;
  int	          nprocesses;
  int	          nattachments;
  char	          filename[ET_FILENAME_LENGTH];
  /* for remote use */
  unsigned short  port;
  unsigned short  serverport;
  et_subnets      subnets;
  et_ifaddrs      ifaddrs;
  et_ifnames      ifnames;
  et_mcastaddrs   mcastaddrs;
} et_sys_config;

/*
 * et_system: contains all ET system information
 *----------------------------------------------------------
 * The next two items are first in the structure so they are the
 * first items in shared memory. The ET version number must be in
 * this same position for each verion of the ET software so
 * comparisons can be made between versions.
 *
 * version	: version # of this ET software release
 * nselects	: current # of selection ints per station (or control ints
 *		: per event)
 *
 * mutex	: protect system  data in changes
 * stat_mutex	: protect station data in changes
 * statadd_mutex: used to add stations one at a time
 * statadd	: cond. var. used to add new stations
 * statdone	: cond. var. used to signal end of station creation
 * tid_hb	: sys heartbeat thread id
 * tid_m	: sys heartmonitor thread id
 * tid_as	: sys add station thread id
 * tid_srv	: ET server tcp thread id
 * tid_mul	: id of thread which spawns broad/multicast listening
 *		: threads
 * asthread	: flag to kill addstat thread (ET_THD_KILL)
 * con_add	: concurrency added to process calling et_system_start
 *
 * pmap		: ptr to mapped mem in ET system's process
 *		: This is here so user-called routines can
 *		: read/write pointers correctly from shared mem
 *
 * heartbeat	: increment to indicate I'm alive
 * hz		: system clock rate
 * mainpid	: unix pid of ET system
 * nstations	: current # of stations idle or active
 * ntemps	: current # of temp events existing
 * nprocesses	: current # of processes in system
 * nattachments	: current # of attachments in system
 * port		: port # for tcp server thread of the ET system
 * host		: host of the ET system
 * proc		: array of info on processes
 * attach	: array of info on attachments
 * stat_head	: head of linked list of used stations (not a pointer)
 * stat_tail	: tail of linked list of used stations (not a pointer)
 *		: Not storing these as pointers makes for an awkward
 *		: linked list, but they survive mapping the shared
 *		: memory to a different spot.
 * config	: parameters used to create ET system
 */
 
typedef struct et_system_t {
  int		   version;
  int		   nselects;
  pthread_mutex_t  mutex;
  pthread_mutex_t  stat_mutex;
  pthread_mutex_t  statadd_mutex;
  pthread_cond_t   statadd;
  pthread_cond_t   statdone;
  pthread_t	   tid_hb;
  pthread_t	   tid_hm;
  pthread_t	   tid_as;
  pthread_t	   tid_srv;
  pthread_t	   tid_mul;
  int		   asthread;
#ifdef sun
  int		   con_add;
#endif
  void		   *pmap;
  unsigned int     heartbeat;
  int		   hz;
  pid_t		   mainpid;
  int		   nstations;
  int		   ntemps;
  int		   nprocesses;
  int		   nattachments;
  unsigned short   port;
  char		   host[ET_MAXHOSTNAMELEN];
  struct et_proc   proc[ET_PROCESSES_MAX];
  struct et_attach attach[ET_ATTACHMENTS_MAX];
  et_stat_id	   stat_head;
  et_stat_id	   stat_tail;
  et_sys_config    config;
} et_system;


/*
 * et_open_config : parameters used to open an ET system
 *-----------------------------
 * init		: =ET_STRUCT_OK if structure properly initialized
 * wait		: =ET_OPEN_WAIT or ET_OPEN_NOWAIT depending on whether
 *		:  user wants to wait for ET system to appear or not.
 * cast		: =ET_BROADCAST for users to discover host & port # of
 *		:  ET system server by broadcasting
 * 		: =ET_MULTICAST for users to discover host & port # of
 *		:  ET system server by multicasting
 * 		: =ET_BROADANDMULTICAST for users to discover host & port # of
 *		:  ET system server by both broad and multicasting
 * 		: =ET_DIRECT when users specify host & port # of ET system
 * ttl		: multicast ttl value (sets scope of multicast)
 * mode		: =ET_HOST_AS_REMOTE or ET_HOST_AS_LOCAL depending on
 *		: whether connections to a local ET system are made as if
 *		: the client were remote (use server) or local (shared mem)
 * debug_default: the default debug level.
 * udpport	: port number for broadcast & direct udp messages
 * multiport	: port number for multicast messages
 * serverport	: port number for ET system's tcp server thread
 * timeout	: max time to wait for ET system to appear if wait=ET_OPEN_WAIT
 * host		: name of host computer that has ET system. Defaults to 
 *		: local host if unset. If domain not included, assumed local.
 *		: May be in dotted-decimal form. If the host name is not known,
 *		: its value may also be ET_HOST_ANYWHERE for an ET system that
 *		: may be local or remote, ET_HOST_REMOTE for an ET system that's
 *		: remote, or ET_HOST_LOCAL for an ET system that is local.
 * policy       : policy to determine which responding ET system to a broad-
 *              : multicast will be chosen as the official respondent.
 *              : = ET_POLICY_ERROR - return error if more than one 
 *              :   responds, else return the single respondent
 *              : = ET_POLICY_FIRST - pick the first to respond
 *              : = ET_POLICY_LOCAL - pick the local system first and
 *              :   if it doesn't respond, the first that does
 * activated    : keep track of which subnets are to be used 
 *              : lowest bit corresponds to first element of "subnets"
 * subnets	: list of all local subnet broadcast addrs (dotted-decimal)
 * mcastaddrs	: list of all multicast addresses (dotted-dec)
 */

typedef struct et_open_config_t {
  int             init;
  int             wait;
  int             cast;
  int             ttl;
  int             mode;
  int             debug_default;
  unsigned short  udpport;
  unsigned short  multiport;
  unsigned short  serverport;
  struct timespec timeout;
  char            host[ET_MAXHOSTNAMELEN];
  int             policy;
  int             activated;
  et_subnets      subnets;
  et_mcastaddrs   mcastaddrs;
} et_open_config;


/*
 * et_id: Contains pointers to key mem locations, config info,
 *	: status info, node locality and remote node info.
 *	: It is essentially an ET system id. User needs one
 *	: for each ET system in use.
 *----------------------------------------------------------
 * init		: =ET_STRUCT_OK if structure properly initialized
 * lang		: language this ET system was written in:
 *		: ET_LANG_C, ET_LANG_CPP, or ET_LANG_JAVA
 * alive	: is system alive? 1 = yes, 0 = no
 * proc		: unique process id# for processes connected to an
 *		: ET system. It's an index into data stored in the
 *		: "et_system" struct. For the et_id returned by a
 *		: call to "et_system_start", this id is -1, This is
 *		: because it refers to the system itself and is not
 *		: "connected" to the ET system.
 * offset	: byte offset between pmap for et & user's processes
 * race		: flag used to eliminate race conditions
 * cleanup	: flag used to warn certain routines (such as et_station_detach and
 *		: et_restore_events) that they are being executed by the main ET system
 *		: process because the ET system is cleaning up after an ET consumer's crash.
 *		: This is meant to distinguish use by the ET system server when executing
 *		: such routines on behalf of a remote client. This prevents mutexes from
 *		: being grabbed during cleanup and temp events from being unlinked during
 *		: normal usage.
 * debug	: level of desired printed output
 * nevents	: total number of events in ET system
 * esize	: size in bytes of events in ET system
 * version	: version # of this ET software release
 * nselects	: current # of selection ints per station (or control ints
 *		: per event)
 * share	: =ET_MUTEX_SHARE if operating system can share mutexes
 *		: between processes, =ET_MUTEX_NOSHARE otherwise
 * memsize	: total size of shared memory in bytes - used when to unmap
 *		: mmapped file when it's already been deleted.
 *
 * for REMOTE client use:
 * locality	: =ET_LOCAL if process is on same machine as ET system,
 *		: =ET_REMOTE if process is on another machine
 *		: =ET_LOCAL_LINUX if process is on same Linux machine as ET
 * sockfd	: client's socket connection to ET server
 * endian	: endian of client's node (ET_ENDIAN_BIG or
 *		: ET_ENDIAN_LITTLE in et_network.h)
 * systemendian	: endian of ET system's node
 * iov_max	: iovmax of client's node
 * port		: port # for tcp server thread of the ET system
 * ethost	: host of the ET system
 *
 * pmap		: pointer to start of mapped memory
 * sys		: ptr to et_system structure
 * stats	: ptr to start of et_station structures
 * histogram	: ptr to histogram data
 * events	: ptr to start of et_event structures
 * data		: ptr to event data
 * grandcentral	: ptr to grandcentral station
 * mutex	: for thread-safe remote communications
 */
 
typedef struct  et_id_t {
  int			init;
  int                   lang;
  int			alive;
  et_proc_id		proc;
  int			offset;
  int			race;
  int			cleanup;
  int			debug;
  int			nevents;
  int			esize;
  int			version;
  int			nselects;
  int			share;
  size_t		memsize;
  /* for REMOTE use */
  int			locality;
  int			sockfd;
  int			endian;
  int			systemendian;
  int			iov_max;
  unsigned short	port;
  char			ethost[ET_MAXHOSTNAMELEN];
  /******************/
  void			*pmap;
  et_system		*sys;
  et_station		*stats;
  int			*histogram;
  et_event		*events;
  char			*data;
  et_station		*grandcentral;
  pthread_mutex_t       mutex;
} et_id;

/*
 * et_mem: Contains info stored at front of mapped memory
 *----------------------------------------------------------
 * This structure must not be changed in size as it will
 * cause differences in the memory location of the version
 * number (in et_system stucture). This version number is
 * used to check the compatiblity of ET systems and users.
 *
 * totalsize	: total size of mapped memory (mapped
 *		: memory must be allocated in pages).
 * usedsize	: desired size of mapped memory given as arg
 *		: to et_mem_create.
 */

typedef struct  et_mem_t {
  size_t	totalsize;
  size_t	usedsize;
} et_mem;

/****************************
 *       REMOTE STUFF       *
 ****************************/

/* "table" of values representing ET commands for tcp/ip */

/* for operating systems that cannot share mutexes (Linux) */
#define  ET_NET_EV_GET_L	0	/* et_event_get */
#define  ET_NET_EVS_GET_L	1	/* et_events_get */
#define  ET_NET_EV_PUT_L	2	/* et_event_put */
#define  ET_NET_EVS_PUT_L	3	/* et_events_put */
#define  ET_NET_EV_NEW_L	4	/* et_event_new */
#define  ET_NET_EVS_NEW_L	5	/* et_events_new */
#define  ET_NET_EV_DUMP_L	6	/* et_event_dump */
#define  ET_NET_EVS_DUMP_L	7	/* et_events_dump */

/* for fully remote systems */
#define  ET_NET_EV_GET		20	/* et_event_get */
#define  ET_NET_EVS_GET		21	/* et_events_get */
#define  ET_NET_EV_PUT		22	/* et_event_put */
#define  ET_NET_EVS_PUT		23	/* et_events_put */
#define  ET_NET_EV_NEW		24	/* et_event_new */
#define  ET_NET_EVS_NEW		25	/* et_events_new */
#define  ET_NET_EV_DUMP		26	/* et_event_dump */
#define  ET_NET_EVS_DUMP	27	/* et_events_dump */

#define  ET_NET_ALIVE		40	/* et_alive */
#define  ET_NET_WAIT		41	/* et_wait_for_alive */
#define  ET_NET_CLOSE		42	/* et_close */
#define  ET_NET_FCLOSE		43	/* et_forcedclose */
#define  ET_NET_WAKE_ATT	44	/* et_wakeup_attachment */
#define  ET_NET_WAKE_ALL	45	/* et_wakeup_all */
 
#define  ET_NET_STAT_ATT	60	/* et_station_attach */
#define  ET_NET_STAT_DET	61	/* et_station_detach */
#define  ET_NET_STAT_CRAT	62	/* et_station_create_at */
#define  ET_NET_STAT_RM		63	/* et_station_remove */
#define  ET_NET_STAT_SPOS	64	/* et_station_setposition */
#define  ET_NET_STAT_GPOS	65	/* et_station_getposition */

#define  ET_NET_STAT_ISAT	80	/* et_station_isattached */
#define  ET_NET_STAT_EX		81	/* et_station_exists */
#define  ET_NET_STAT_SSW	82	/* et_station_setselectwords */
#define  ET_NET_STAT_GSW	83	/* et_station_getselectwords */
#define  ET_NET_STAT_LIB	84	/* et_station_getlib */
#define  ET_NET_STAT_FUNC	85	/* et_station_getfunction */
#define  ET_NET_STAT_CLASS	86	/* et_station_getclass */
   
#define  ET_NET_STAT_GATTS	100	/* et_station_getattachments */
#define  ET_NET_STAT_STATUS	101	/* et_station_getstatus */
#define  ET_NET_STAT_INCNT	102	/* et_station_getinputcount */
#define  ET_NET_STAT_OUTCNT	103	/* et_station_getoutputcount */
#define  ET_NET_STAT_GBLOCK	104	/* et_station_getblock */
#define  ET_NET_STAT_GUSER	105	/* et_station_getuser */
#define  ET_NET_STAT_GRESTORE	106	/* et_station_getrestore */
#define  ET_NET_STAT_GPRE	107	/* et_station_getprescale */
#define  ET_NET_STAT_GCUE	108	/* et_station_getcue */
#define  ET_NET_STAT_GSELECT	109	/* et_station_getselect */

#define  ET_NET_STAT_SBLOCK	115	/* et_station_getblock */
#define  ET_NET_STAT_SUSER	116	/* et_station_getuser */
#define  ET_NET_STAT_SRESTORE	117	/* et_station_getrestore */
#define  ET_NET_STAT_SPRE	118	/* et_station_getprescale */
#define  ET_NET_STAT_SCUE	119	/* et_station_getcue */

#define  ET_NET_ATT_PUT		130	/* et_att_getput */
#define  ET_NET_ATT_GET		131	/* et_att_getget */
#define  ET_NET_ATT_DUMP	132	/* et_att_getdump */
#define  ET_NET_ATT_MAKE	133	/* et_att_getmake */

#define  ET_NET_SYS_TMP		150	/* et_system_gettemps */
#define  ET_NET_SYS_TMPMAX	151	/* et_system_gettempsmax */
#define  ET_NET_SYS_STAT	152	/* et_system_getstations */
#define  ET_NET_SYS_STATMAX	153	/* et_system_getstationsmax */
#define  ET_NET_SYS_PROC	154	/* et_system_getprocs */
#define  ET_NET_SYS_PROCMAX	155	/* et_system_getprocsmax */
#define  ET_NET_SYS_ATT		156	/* et_system_getattachments */
#define  ET_NET_SYS_ATTMAX	157	/* et_system_getattsmax */
#define  ET_NET_SYS_HBEAT	158	/* et_system_getheartbeat */
#define  ET_NET_SYS_PID		159	/* et_system_getpid */

#define  ET_NET_SYS_DATA	170	/* send ET system data */
#define  ET_NET_SYS_HIST	171	/* send ET histogram data */


/* struct for passing data from system to network threads */
typedef struct et_netthread_t {
  et_id *id;			      /* system id */
  et_sys_config *config;	      /* system configuration */
  int  cast;			      /* broad or multicast */
  char host[ET_MAXHOSTNAMELEN];       /* host assoc with this address */
  char listenaddr[ET_IPADDRSTRLEN];   /* interface, broadcast, or multicast address
                                      (dot-decimal) being bound to socket so it
				      can receive udp packets on the address */
  char returnaddr[ET_IPADDRSTRLEN];   /* interface address (dot-decimal) being
                                      returned to client along with hostname */
} et_netthread;

/****************************
 *       BRIDGE STUFF       *
 ****************************/
 
/*
 * et_bridge_config : parameters used to bridge ET systems,
 *		    : (transfer events between 2 systems)
 *-----------------------------
 * init		: =ET_STRUCT_OK if structure properly initialized
 * mode_from	: =ET_SLEEP, ET_TIMED, or ET_ASYNC for getting events
 *		: from the "from" ET system
 * mode_to	: =ET_SLEEP, ET_TIMED, or ET_ASYNC for getting new events
 *		: from the "to" ET system into which the "from" events go.
 * chunk_from	: # of events to get from the "from" ET system at one time.
 * chunk_to	: # of new events to get from the "to" ET system at one time.
 * timeout_from	: max time to wait when getting events from the "from" ET system
 * timeout_to	: max time to wait when getting new events from the "to" ET system
 * func		: pointer to function which takes a pointer to an event as
 *		: an argument, swaps the data, and returns ET_ERROR if there's
 *		: a problem & ET_OK if not.
 */

typedef struct et_bridge_config_t {
  int             init;
  int             mode_from;
  int             mode_to;
  int             chunk_from;
  int             chunk_to ;
  struct timespec timeout_from;
  struct timespec timeout_to;
  ET_SWAP_FUNCPTR func;		/* see et.h */
} et_bridge_config;

/****************************
 *    END BRIDGE STUFF      *
 ****************************/

/* macros to switch ptrs from et space to user space & vice versa */
#define ET_PEVENT2USR(p, offset) ((et_event *)((char *)(p) + (offset)))
#define ET_PEVENT2ET(p, offset)  ((et_event *)((char *)(p) - (offset)))
#define ET_PSTAT2USR(p, offset)  ((et_station *)((char *)(p) + (offset)))
#define ET_PSTAT2ET(p, offset)   ((et_station *)((char *)(p) - (offset)))
#define ET_PDATA2USR(p, offset)  ((void *)((char *)(p) + (offset)))
#define ET_PDATA2ET(p, offset)   ((void *)((char *)(p) - (offset)))

/***********************/
/* Extern declarations */
/***********************/

/* mutex related */
extern void et_station_lock(et_system *sys);
extern void et_station_unlock(et_system *sys);

extern void et_llist_lock(et_list *pl);
extern void et_llist_unlock(et_list *pl);

extern void et_system_lock(et_system *sys);
extern void et_system_unlock(et_system *sys);

extern void et_transfer_lock(et_station *ps);
extern void et_transfer_unlock(et_station *ps);
extern void et_transfer_lock_all(et_id *id);
extern void et_transfer_unlock_all(et_id *id);

extern void et_tcp_lock(et_id *id);
extern void et_tcp_unlock(et_id *id);

extern int  et_mutex_locked(pthread_mutex_t *pmutex);

extern int  et_repair_station(et_id *id, et_stat_id stat_id);
extern int  et_repair_gc(et_id *id);

/* initialization */
extern void et_init_station(et_station *ps);
extern void et_init_llist(et_list *pl);

extern void et_init_event(et_event *pe);
extern void et_init_event_(et_event *pe);

extern void et_init_process(et_system *sys, et_proc_id id);
extern void et_init_attachment(et_system *sys, et_att_id id);

extern void et_init_histogram(et_id *id);
extern void et_init_stats_att(et_system *sys, et_att_id id);
extern void et_init_stats_allatts(et_system *sys);
extern void et_init_stats_station(et_station *ps);
extern void et_init_stats_allstations(et_id *id);
extern void et_init_stats_all(et_id *id);

extern int  et_id_init(et_sys_id* id);
extern void et_id_destroy(et_sys_id id);

/* read and write */
extern int  et_station_write(et_id *id,  et_stat_id stat_id, et_event  *pe);
extern int  et_station_nwrite(et_id *id, et_stat_id stat_id, et_event *pe[], int num);

extern int  et_station_read(et_id *id,  et_stat_id stat_id, et_event **pe, 
			    int _mode, et_att_id att, struct timespec *time);
extern int  et_station_nread(et_id *id, et_stat_id stat_id, et_event *pe[], int mode,
			     et_att_id att, struct timespec *time, int num, int *nread);
extern int  et_station_dump(et_id *id, et_event *pe);
extern int  et_station_ndump(et_id *id, et_event *pe[], int num);

extern int  et_llist_read(et_list *pl, et_event **pe);
extern int  et_llist_write(et_id *id, et_list *pl, et_event **pe, int num);
extern int  et_llist_write_gc(et_id *id, et_event **pe, int num);

extern int  et_restore_events(et_id *id, et_att_id att, et_stat_id stat_id);
extern void et_flush_events(et_id *id, et_att_id att, et_stat_id stat_id);

/* mmap/memory functions */
extern int   et_mem_create(const char *name, size_t memsize, void **pmemory);
extern void *et_mem_attach(const char *name);
extern int   et_mem_unmap(const char *name, void *pmem);
extern int   et_mem_remove(const char *name, void *pmem);
extern int   et_mem_size(const char *name, size_t *totalsize, size_t *usedsize);

extern void *et_temp_create(const char *name, size_t size);
extern void *et_temp_attach(const char *name, size_t size);
extern int   et_temp_remove(const char *name, void *pmem, size_t size);

/* remote routines */
extern int  etr_event_new(et_sys_id id, et_att_id att, et_event **ev,
		int wait, struct timespec *deltatime, int size);
extern int  etr_events_new(et_sys_id id, et_att_id att, et_event *evs[],
		int wait, struct timespec *deltatime, int size, int num, int *nread);
extern int  etr_event_get(et_sys_id id, et_att_id att, et_event **ev,
		int wait, struct timespec *deltatime);
extern int  etr_events_get(et_sys_id id, et_att_id att, et_event *evs[],
		int wait, struct timespec *deltatime, int num, int *nread);
extern int  etr_event_put(et_sys_id id, et_att_id att, et_event *ev);
extern int  etr_events_put(et_sys_id id, et_att_id att, et_event *evs[], int num);
extern int  etr_event_dump(et_sys_id id, et_att_id att, et_event *ev);
extern int  etr_events_dump(et_sys_id id, et_att_id att, et_event *evs[], int num);

extern int  etr_open(et_sys_id *id, const char *et_filename, et_openconfig openconfig);
extern int  etr_close(et_sys_id id);
extern int  etr_forcedclose(et_sys_id id);
extern int  etr_alive(et_sys_id id);
extern int  etr_wait_for_alive(et_sys_id id);
extern int  etr_wakeup_attachment(et_sys_id id, et_att_id att);
extern int  etr_wakeup_all(et_sys_id id, et_stat_id stat_id);

extern int  etr_station_create_at(et_sys_id id, et_stat_id *stat_id,
		 const char *stat_name, et_statconfig sconfig,
		 int position, int parallelposition);
extern int  etr_station_create(et_sys_id id, et_stat_id *stat_id,
		 const char *stat_name, et_statconfig sconfig);
extern int  etr_station_remove(et_sys_id id, et_stat_id stat_id);
extern int  etr_station_attach(et_sys_id id, et_stat_id stat_id, et_att_id *att);
extern int  etr_station_detach(et_sys_id id, et_att_id att);
extern int  etr_station_setposition(et_sys_id id, et_stat_id stat_id, int position,
                                    int parallelposition);
extern int  etr_station_getposition(et_sys_id id, et_stat_id stat_id, int *position,
                                    int *parallelposition);
extern int  etr_station_isattached(et_sys_id id, et_stat_id stat_id, et_att_id att);
extern int  etr_station_exists(et_sys_id id, et_stat_id *stat_id, const char *stat_name);

extern int  etr_station_getattachments(et_sys_id id, et_stat_id stat_id, int *numatts);
extern int  etr_station_getstatus(et_sys_id id, et_stat_id stat_id, int *status);
extern int  etr_station_getstatus(et_sys_id id, et_stat_id stat_id, int *status);
extern int  etr_station_getinputcount(et_sys_id id, et_stat_id stat_id, int *cnt);
extern int  etr_station_getoutputcount(et_sys_id id, et_stat_id stat_id, int *cnt);
extern int  etr_station_getselect(et_sys_id id, et_stat_id stat_id, int *select);
extern int  etr_station_getlib(et_sys_id id, et_stat_id stat_id, char *lib);
extern int  etr_station_getclass(et_sys_id id, et_stat_id stat_id, char *classs);
extern int  etr_station_getfunction(et_sys_id id, et_stat_id stat_id, char *function);

extern int  etr_station_getblock(et_sys_id id, et_stat_id stat_id, int *block);
extern int  etr_station_setblock(et_sys_id id, et_stat_id stat_id, int  block);
extern int  etr_station_getuser(et_sys_id id, et_stat_id stat_id, int *user);
extern int  etr_station_setuser(et_sys_id id, et_stat_id stat_id, int  user);
extern int  etr_station_getrestore(et_sys_id id, et_stat_id stat_id, int *restore);
extern int  etr_station_setrestore(et_sys_id id, et_stat_id stat_id, int  restore);
extern int  etr_station_getprescale(et_sys_id id, et_stat_id stat_id, int *prescale);
extern int  etr_station_setprescale(et_sys_id id, et_stat_id stat_id, int  prescale);
extern int  etr_station_getcue(et_sys_id id, et_stat_id stat_id, int *cue);
extern int  etr_station_setcue(et_sys_id id, et_stat_id stat_id, int  cue);
extern int  etr_station_getselectwords(et_sys_id id, et_stat_id stat_id, int select[]);
extern int  etr_station_setselectwords(et_sys_id id, et_stat_id stat_id, int select[]);

extern int  etr_system_gettemps (et_sys_id id, int *temps);
extern int  etr_system_gettempsmax (et_sys_id id, int *tempsmax);
extern int  etr_system_getstations (et_sys_id id, int *stations);
extern int  etr_system_getstationsmax (et_sys_id id, int *stationsmax);
extern int  etr_system_getprocs (et_sys_id id, int *procs);
extern int  etr_system_getprocsmax (et_sys_id id, int *procsmax);
extern int  etr_system_getattachments (et_sys_id id, int *atts);
extern int  etr_system_getattsmax (et_sys_id id, int *attsmax);
extern int  etr_system_getheartbeat (et_sys_id id, int *heartbeat);
extern int  etr_system_getpid (et_sys_id id, int *pid);
/* end remote routines */

/* attachment rountines */
extern int etr_attach_geteventsput(et_sys_id id, et_att_id att_id,
                                   int *highint, int *lowint);
extern int etr_attach_geteventsget(et_sys_id id, et_att_id att_id,
                                   int *highint, int *lowint);
extern int etr_attach_geteventsdump(et_sys_id id, et_att_id att_id,
                                    int *highint, int *lowint);
extern int etr_attach_geteventsmake(et_sys_id id, et_att_id att_id,
                                    int *highint, int *lowint);
/* end of attachment routines */

/* local no-share (linux) routines */
extern int  etn_open(et_sys_id *id, const char *filename, et_openconfig openconfig);
extern int  etn_close(et_sys_id id);
extern int  etn_forcedclose(et_sys_id id);
extern int  etn_alive(et_sys_id id);
extern int  etn_wait_for_alive(et_sys_id id);
extern int  etn_event_new(et_sys_id id, et_att_id att, et_event **ev,
                 int mode, struct timespec *deltatime, int size);
extern int  etn_events_new(et_sys_id id, et_att_id att, et_event *evs[],
                 int mode, struct timespec *deltatime, int size, int num, int *nread);
extern int  etn_event_get(et_sys_id id, et_att_id att, et_event **ev,
                 int mode, struct timespec *deltatime);
extern int  etn_events_get(et_sys_id id, et_att_id att, et_event *evs[],
                 int mode, struct timespec *deltatime, int num, int *nread);
extern int  etn_event_put(et_sys_id id, et_att_id att, et_event *ev);
extern int  etn_events_put(et_sys_id id, et_att_id att, et_event *evs[], int num);
extern int  etn_event_dump(et_sys_id id, et_att_id att, et_event *ev);
extern int  etn_events_dump(et_sys_id id, et_att_id att, et_event *evs[], int num);
/* end of local linux routines */

/* local routines (Sun) */
extern int  etl_open(et_sys_id *id, const char *filename, et_openconfig openconfig);
extern int  etl_close(et_sys_id id);
extern int  etl_forcedclose(et_sys_id id);
extern int  etl_alive(et_sys_id id);
extern int  etl_wait_for_alive(et_sys_id id);
extern int  et_wait_for_system(et_sys_id id, struct timespec *timeout,
				const char *etname);
extern int  et_look(et_sys_id *id, const char *filename);
extern int  et_unlook(et_sys_id id);
/* end local routines */

/* server/network routines */
extern void *et_cast_thread(void *arg);
extern void *et_netserver(void *arg);
extern int   et_findserver(const char *etname, char *ethost, unsigned short *port,
		           et_open_config *config);
extern int   et_responds(const char *etname);
extern int   et_findlocality(const char *filename, et_openconfig openconfig);
extern int   et_netinfo (et_ifnames *ifnames, et_ifaddrs *ifaddrs, et_subnets *nets);
/* end server/network routines */
 
/* 64bit integers */
extern et_bigint    et_bigint_add(et_bigint bi, unsigned int add);
extern et_bigint    et_bigint_sub(et_bigint bi, unsigned int sub);
extern int          et_bigint_equal(et_bigint bi_1, et_bigint bi_2);
extern double       et_bigint_diff(et_bigint bi_1,  et_bigint bi_2);
extern unsigned int et_bigint_mod(et_bigint bi, unsigned int mod);
extern void         et_bigint_init(et_bigint *bi);
extern void         et_bigint_print(et_bigint bi, const char *format);
extern void         et_bigint_string(et_bigint bi, char *string);
/* end 64 bit */

/* station configuration checks */
extern int  et_station_config_check(et_id *id, et_stat_config *sc);
extern int  et_station_compare_parallel(et_id *id, et_stat_config *group,
            et_stat_config *config);
/* end station configuration checks */

/* error logging */
#ifdef WITH_DALOGMSG
  #define et_logmsg daLogMsg
  extern void daLogMsg (char *sev,char *fmt,...);
#else
  extern void et_logmsg (char *sev, char *fmt, ...);
#endif

#ifdef	__cplusplus
}
#endif

#endif
