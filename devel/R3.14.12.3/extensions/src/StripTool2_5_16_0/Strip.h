/*************************************************************************\
* Copyright (c) 1994-2004 The University of Chicago, as Operator of Argonne
* National Laboratory.
* Copyright (c) 1997-2003 Southeastern Universities Research Association,
* as Operator of Thomas Jefferson National Accelerator Facility.
* Copyright (c) 1997-2002 Deutches Elektronen-Synchrotron in der Helmholtz-
* Gemelnschaft (DESY).
* This file is distributed subject to a Software License Agreement found
* in the file LICENSE that is included with this distribution. 
\*************************************************************************/

#ifndef _Strip
#define _Strip

#include "StripDefines.h"
#include "StripMisc.h"
#ifndef NO_X11_HERE /* Albert */
#include "StripConfig.h"
#endif /* Albert */
#include "StripCurve.h"


/* ======= Data Types ======= */
typedef void *  Strip;
typedef int     (*StripCallback)        (StripCurve, void *);

/* ======= Attributes ======= */
typedef enum
{
  STRIP_CONNECT_FUNC = 1,       /* (StripCallback)                      rw */
  STRIP_CONNECT_DATA,           /* (void *)                             rw */
  STRIP_DISCONNECT_FUNC,        /* (StripCallback)                      rw */
  STRIP_DISCONNECT_DATA,        /* (void *)                             rw */
  STRIP_QUIT_FUNC,              /* (StripCallback)                      rw */
  STRIP_QUIT_DATA,              /* (void *)                             rw */
  STRIP_DAQ,                    /* (void *)                             rw */
  STRIP_LAST_ATTRIBUTE
}
StripAttribute;



/* ======= Functions ======= */
/*
 * Strip_init
 *
 *      Creates a new strip data structure, setting all values to defaults.
 *      The first two arguments are the command line specs.  The last
 *      argument is the FILE pointer to which all error/status messages are
 *      written.
 */
Strip   Strip_init      (int    *argc,
                         char   *argv[],
                         FILE   *logfile);


/*
 * Strip_delete
 *
 *      Destroys the specified strip chart.
 */
void    Strip_delete    (Strip);


/*
 * Strip_set/getattr
 *
 *      Sets or gets the specified attribute, returning true on success.
 */
int     Strip_setattr   (Strip, ...);
int     Strip_getattr   (Strip, ...);


/*
 * Strip_newcurve
 *
 *      Retrieves a new curve object.
 */
StripCurve      Strip_getcurve  (Strip);


/*
 * Strip_freecurve
 *
 *      Destroys the specified curve, removing all internal references to it.
 *      If the curve is connected to a data source, it is first disconnected.
 */
void    Strip_freecurve         (Strip, StripCurve);


/*
 * Strip_freesomecurves
 *
 *      Same as freecurve, except a null-terminated array of curves is
 *      operated upon.
 */
void    Strip_freesomecurves    (Strip, StripCurve[]);


/*
 * Strip_addfd
 *
 *      Manages the specified file descriptor.  Calls the specified event
 *      handler with the given data when activity occurs on the file
 *      descriptor.
 */
#ifndef NO_X11_HERE /* Albert */
int     Strip_addfd     (Strip,
                         int,                   /* file descriptor */
                         XtInputCallbackProc,   /* callback function */
                         XtPointer);            /* callback data */


/*
 * Strip_addtimeout
 *
 *      Calls the given callback once the specified time span has passed.
 *      If the timeout cannot be registered, false is returned.
 */
XtIntervalId    Strip_addtimeout        (Strip,
                                         double,                /* seconds */
                                         XtTimerCallbackProc,   /* function */
                                         XtPointer);            /* data */

#endif /* Albert */
/*
 * Strip_clearfd
 *
 *      Unmanages the specified file descriptor.
 */
int     Strip_clearfd   (Strip, int);                   /* file descriptor */


/*
 * Strip_go
 *
 *      Causes the strip chart to draw itself and start plotting available
 *      data.  If any curves are connected then the graph pops up, otherwise
 *      the dialog box pops up.
 *
 *      This function must be called before any of the following strip control
 *      functions: Strip_print()
 */
void    Strip_go        (Strip);


/*
 * Strip_connectcurve
 *
 *      Calls the user-specified connection callback, and sets internal
 *      status indicators.  This function should be called rather than calling
 *      the user specified routine directly.
 *
 *      To allow for asynchronous connection requests, the client must also
 *      call Strip_setconnected() to inform the Strip object that the
 *      curve has been connected.  If the connection is established within
 *      the client callback, then Strip_setconnected() should be called
 *      from there, otherwise it should be called once the actual physical
 *      connection has been established.
 *
 */
int     Strip_connectcurve      (Strip, StripCurve);


/*
 * Strip_setconnected
 *
 *      Informs the Strip object that the specified curve is ready for
 *      plotting.
 *
 *      After this function is called, data will start being gathered for the
 *      StripCurve.  Whether or not the data are plotted depends on the
 *      curve's plot status and/or pen status attribute values.
 */
void    Strip_setconnected      (Strip, StripCurve);


#ifndef PEND_DESCRIPTION
/*
 * Strip_setdescconnected
 *
 *      Informs the Strip object that description for the specified curve
 *      is ready for plotting.
 *
 */
void    Strip_setdescconnected      (Strip, StripCurve);
#endif

/*
 * Strip_setwating
 *
 *      Informs the Strip object that the specified curve is waiting to
 *      reestablish a link to its data source.
 */
void    Strip_setwaiting        (Strip, StripCurve);


/*
 * Strip_disconnectcurve
 *
 *      Like Strip_connectcurve.  Calls user-supplied disconnect callback, and
 *      sets internal status indicators.
 */
int     Strip_disconnectcurve   (Strip, StripCurve);

/*
 * Strip_clear
 *
 *      Frees all curves and clears the graph.
 */
void    Strip_clear     (Strip);


/*
 * Strip_dump
 *
 *      Dumps the data buffer to specified file.
 */
int     Strip_dumpdata  (Strip, char *);


/*
 * Strip_writeconfig
 *
 *      Writes the current configuration to the specified stdio stream at the
 *      current location in the stream.  See StripConfig.h for info on the
 *      output format.
 */
int     Strip_writeconfig       (Strip, FILE *, StripConfigMask, char *);


/*
 * Strip_readconfig
 *
 *      Scans the specified file, searching for configuration information
 *      written by Strip_writeconfig().  All found attributes which fit the
 *      specified mask are then pushed into the current configuration.  See
 *      StripConfig.h for more info.
 */
int     Strip_readconfig        (Strip, FILE *, StripConfigMask, char *);

/* Should put descriptions here as for others */
void    Strip_refresh  (Strip the_strip);
int     Strip_auto_scale (Strip the_strip);
void    Strip_handlexerrors (void);  
void    Strip_ignorexerrors (void);  

/*
 * callBrowser
 *
 *      Calls the default browser with the given url and bookmark
 */
int callBrowser(Display *dpy, char *url, char *bookmark);

#endif  /* #ifndef _Strip */
