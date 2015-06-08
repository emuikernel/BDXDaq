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

#ifndef _StripMisc
#define _StripMisc

#ifndef NO_X11_HERE /* Albert */

#include <math.h>
#include <time.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>

#include <X11/Intrinsic.h>
#include <X11/Xlib.h>
#include <float.h>
#endif  /* Albert */

/* WIN32 differences */
#ifdef WIN32
/* Hummingbird extra functions including lprintf */
# include <X11/XlibXtra.h>
#if defined(_WIN32) && !defined(_MINGW) && (_MSC_VER < 1800)
/* In MSVC timeval is in winsock.h, winsock2.h, ws2spi.h, nowhere else */
# include <X11/Xwinsock.h>
#else
# include <X11/Xos.h>
#endif
/* _MAX_PATH in stdlib.h for WIN32 */
# define STRIP_PATH_MAX _MAX_PATH
/* Path delimiter is different */
# define STRIP_PATH_DELIMITER ';'
# define STRIP_DIR_DELIMITER_CHAR '\\'
# define STRIP_DIR_DELIMITER_STRING "\\"
#else /* #ifdef WIN32 */
# include <sys/time.h>
/* PATH_MAX may be in limits.h.  Kludge it if not */
# ifndef PATH_MAX
#   define STRIP_PATH_MAX 1024
# else
#   define STRIP_PATH_MAX PATH_MAX
# endif
/* Path delimiter is different */
# define STRIP_PATH_DELIMITER ':'
# define STRIP_DIR_DELIMITER_CHAR '/'
# define STRIP_DIR_DELIMITER_STRING "/"
#endif /* #ifdef WIN32 */

/*
 * this is some serious brain-damage
 * by calling this macro in place of unused arguments in the function
 * headers, then the code will compile without warning or error in
 * either C++ or C.  (C++ complains about unused arguments, C complains
 * about missing arguments)
 */
#if defined(__cplusplus) || defined(C_plusplus)
#  define BOGUS(x)
#else
#  define BOGUS(x)      BOGUS_ARG_ ## x
#endif

#define STRIPCURVE_PENDOWN      1
#define STRIPCURVE_PLOTTED      1

#define ONE_MILLION             1000000.0
#define ONE_THOUSAND            1000.0

#ifndef max
#define max(a,b) ((a)>(b)?(a):(b))
#endif
#ifndef min
#  define min(a,b) ((a)<(b)?(a):(b))
#endif

#ifndef DBL_EPSILON
#  define DBL_EPSILON   2.2204460492503131E-16
#endif

#ifndef ABS
#  define ABS(x)        ((x) < 0? (-(x)) : (x))
#endif

/* ====== Various useful constants initialized by StripMisc_init() ====== */
extern float            vertical_pixels_per_mm;
extern float            horizontal_pixels_per_mm;
extern char             stripHelpPath[STRIP_PATH_MAX];

/* Strip_x_error_code
 *
 *      This value is set by the X error handler (in Strip.c) any time
 *      it is invoked.  In order to catch an error, Synchronize the X
 *      server, set this variable to Success, make an X protocol request,
 *      then check it again.  If changed, then the indicated error has
 *      occurred.  Unsynchronize afterwards!
 */
extern unsigned char    Strip_x_error_code;

/*
 * StripMisc_init
 *
 *      Initializes global constants.
 */
#ifndef NO_X11_HERE /* Albert */
void    StripMisc_init  (Display *, int);       /* display, screen */
#endif /* Albert */

#if 0
/* strip_name
 *
 *      Returns a string describing the application.
 */
char    *strip_name  (void);


/* strip_version
 *
 *      Returns a string describing the current version
 */
char    *strip_version  (void);


/* strip_lib_version
 *
 *      Returns a string describing the linked version of system libraries
 */
char    *strip_lib_version  (void);
#endif


/* ====== Various Time Functions ====== */

/* get_current_time
 *
 *      Stores current time in the supplied buffer.
 */
void            get_current_time        (struct timeval *);

/* time2str
 *
 *      Returns pointer to converted time rep.
 */
char            *time2str       (struct timeval *);


/* dbl2time
 */
#define dbl2time(t,d) \
(void) \
((t)->tv_sec = (unsigned long)(d), \
 (t)->tv_usec = (unsigned long)(((d) - (t)->tv_sec) * (long)ONE_MILLION))

/* time2dbl
 */
#define time2dbl(t) \
(double)((double)(t)->tv_sec + ((double)(t)->tv_usec / (double)ONE_MILLION))

/* add_times (s=a+b)
 */
#define add_times(s,a,b) \
(void) \
((s)->tv_sec = (a)->tv_sec + (b)->tv_sec + \
 (((a)->tv_usec + (b)->tv_usec) / (unsigned long)ONE_MILLION), \
 (s)->tv_usec = ((a)->tv_usec + (b)->tv_usec) % (long)ONE_MILLION)

/* diff_times (s=a-b)
 */
#define diff_times(s,b,a) \
(void) \
( (s)->tv_sec = (a)->tv_sec, (s)->tv_usec = (a)->tv_usec, \
  ( ((b)->tv_sec > (a)->tv_sec) \
    ? ((s)->tv_sec = 0, (s)->tv_usec = 0) \
    : ((((s)->tv_usec < (b)->tv_usec) \
        ? ((s)->tv_usec += (unsigned long)ONE_MILLION, (s)->tv_sec--) \
        : 0), \
       ( ((s)->tv_sec < (b)->tv_sec) \
         ? ((s)->tv_sec = 0, (s)->tv_usec = 0) \
         : ((s)->tv_sec -= (b)->tv_sec, (s)->tv_usec -= (b)->tv_usec) ))))

/* compare_times
 * returns >0 if a<b, 0 if a=b, and <0 if a<b
 */
#define compare_times(a,b) \
(int) \
(((a)->tv_sec > (b)->tv_sec)? 1 : \
 (((a)->tv_sec < (b)->tv_sec)? -1 : ((a)->tv_usec - (b)->tv_usec)))

/* subtract_times (s=a-b)
 */
#define subtract_times(s,b,a) \
(double) \
((compare_times((a),(b)) >= 0) \
 ? (diff_times((s),(b),(a)), time2dbl((s))) \
 : (diff_times((s),(a),(b)), -time2dbl((s)))) 


#ifndef NO_X11_HERE /* Albert */

/* ====== Various Window Functions ====== */

int     window_isviewable       (Display *, Window);
int     window_ismapped         (Display *, Window);
int     window_isiconic         (Display *, Window);
void    window_map              (Display *, Window);
void    window_unmap            (Display *, Window);

void    MessageBox_popup        (Widget,        /* parent */
                                 Widget *,      /* MessageBox */
                                 int,           /* type: XmDIALOG_XXX */
                                 char *,        /* title */
                                 char *,        /* button label */
                                 char *,        /* message format */
                                 ...);          /* message args */

#endif /* Albert */

int Question_popup(XtAppContext app, Widget wparent, char * question);

/* ====== Miscellaneous ====== */

void    sec2hms (unsigned sec, int *h, int *m, int *s);

char    *dbl2str        (double,        /* value */
                         int,           /* precision: num digits after radix */
                         char[],        /* result buffer */
                         int);          /* max length of converted string */

char    *int2str        (int x, char buf[], int n);

/* basename
 *
 *      Returns the filename portion of a fully qualified path.
 */
char    *basename       (char *);
void History_MessageBox_popup(char *title,char *btn_txt,char *str);

/* General purpose output routine
 * Works with both UNIX and WIN32
 * Uses sprintf to avoid problem with lprintf not handling %f, etc.
 *   (Exceed 5 only) */
void print(const char *fmt, ...);

/* Gets current time and puts it in a static array
 * The calling program should copy it to a safe place
 *   e.g. strcpy(savetime,timestamp()); */
char *timeStamp(void);

/* Function to convert / to \ to avoid inconsistencies in WIN32 */
void convertDirDelimiterToWIN32(char *pathName);

#endif

