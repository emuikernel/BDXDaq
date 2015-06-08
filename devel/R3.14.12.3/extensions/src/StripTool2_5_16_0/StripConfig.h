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

#ifndef _StripConfig
#define _StripConfig

#ifndef NO_X11_HERE /* Albert */ 
#include <X11/Xlib.h>
#include <X11/Intrinsic.h>
#endif /* Albert */ 

#include <stdio.h>
#include <stdarg.h>
#include <limits.h>

#ifndef NO_X11_HERE /* Albert */
#include <float.h>
#endif /* Albert */ 

#ifdef WIN32
 /* WIN32 does not have unistd.h */
#else
# include <unistd.h>
#endif

#include "StripDefines.h"
#include "StripMisc.h"
#ifndef NO_X11_HERE /* Albert */ 
#include "cColorManager.h"
#endif /* Albert */ 

extern char *untitled_;

typedef enum _StripScaleType
{
  STRIPSCALE_LINEAR = 0,
  STRIPSCALE_LOG_10,
  STRIPSCALE_NUM_TYPES
}
StripScaleType;

typedef enum _StripGridVisibility
{
  STRIPGRID_NONE = 0,
  STRIPGRID_SOME,
  STRIPGRID_ALL,
  STRIPGRID_NUM_TYPES
}
StripGridVisibility;


/* ======= Default Values ======= */
#define STRIPDEF_TITLE                  untitled_
#define STRIPDEF_TIME_TIMESPAN          STRIP_DEFAULT_TIMESPAN
#define STRIPDEF_TIME_NUM_SAMPLES       7200
#define STRIPDEF_TIME_SAMPLE_INTERVAL   1
#define STRIPDEF_TIME_REFRESH_INTERVAL  1
#define STRIPDEF_COLOR_BACKGROUND_STR   "White"
#define STRIPDEF_COLOR_FOREGROUND_STR   "Black"
#define STRIPDEF_COLOR_GRID_STR         "Grey75"
#define STRIPDEF_COLOR_COLOR1_STR       "Blue"
#define STRIPDEF_COLOR_COLOR2_STR       "OliveDrab"
#define STRIPDEF_COLOR_COLOR3_STR       "Brown"
#define STRIPDEF_COLOR_COLOR4_STR       "CadetBlue"
#define STRIPDEF_COLOR_COLOR5_STR       "Orange"
#define STRIPDEF_COLOR_COLOR6_STR       "Purple"
#define STRIPDEF_COLOR_COLOR7_STR       "Red"
#define STRIPDEF_COLOR_COLOR8_STR       "Gold"
#define STRIPDEF_COLOR_COLOR9_STR       "RosyBrown"
#define STRIPDEF_COLOR_COLOR10_STR      "YellowGreen"
#define STRIPDEF_OPTION_GRID_XON        1
#define STRIPDEF_OPTION_GRID_YON        1
#define STRIPDEF_OPTION_AXIS_YCOLORSTAT 1
#define STRIPDEF_OPTION_GRAPH_LINEWIDTH 2
#define STRIPDEF_CURVE_NAME             "Curve"
#define STRIPDEF_CURVE_EGU              "Undefined"
#define STRIPDEF_CURVE_COMMENT          ""
#define STRIPDEF_CURVE_PRECISION        4
#define STRIPDEF_CURVE_MIN              1e-7
#define STRIPDEF_CURVE_MAX              1e+7
#define STRIPDEF_CURVE_SCALE            STRIPSCALE_LINEAR
#define STRIPDEF_CURVE_PLOTSTAT         STRIPCURVE_PLOTTED
#define STRIPDEF_CURVE_ID               NULL

/* ====== Min/Max values for all attributes requiring range checking ====== */
#define STRIPMIN_TIME_TIMESPAN          1
#define STRIPMAX_TIME_TIMESPAN          UINT_MAX
#define STRIPMIN_TIME_NUM_SAMPLES       7200
#define STRIPMAX_TIME_NUM_SAMPLES       65536
#define STRIPMIN_TIME_SAMPLE_INTERVAL   0.01
#define STRIPMAX_TIME_SAMPLE_INTERVAL   DBL_MAX
#define STRIPMIN_TIME_REFRESH_INTERVAL  0.1
#define STRIPMAX_TIME_REFRESH_INTERVAL  DBL_MAX
#define STRIPMIN_OPTION_GRID_XON        STRIPGRID_NONE
#define STRIPMAX_OPTION_GRID_XON        STRIPGRID_ALL
#define STRIPMIN_OPTION_GRID_YON        STRIPGRID_NONE
#define STRIPMAX_OPTION_GRID_YON        STRIPGRID_ALL
#define STRIPMIN_OPTION_GRAPH_LINEWIDTH 0
#define STRIPMAX_OPTION_GRAPH_LINEWIDTH 10
#define STRIPMIN_CURVE_PRECISION        0
#define STRIPMAX_CURVE_PRECISION        12

#define STRIPCONFIG_NUMCOLORS           (STRIP_MAX_CURVES + 3)
#define STRIPCONFIG_MAX_CALLBACKS       10


typedef int     StripConfigMaskElement;
typedef int     StripConfigAttribute;


typedef enum    _StripConfigAttributeEnum
{
  STRIPCONFIG_TITLE = 1,                /* (char *)                     rw */
  STRIPCONFIG_FILENAME,                 /* (char *)                     rw */
  STRIPCONFIG_TIME_TIMESPAN,            /* (unsigned)                   rw */
  STRIPCONFIG_TIME_NUM_SAMPLES,         /* (int)                        rw */
  STRIPCONFIG_TIME_SAMPLE_INTERVAL,     /* (double)                     rw */
  STRIPCONFIG_TIME_REFRESH_INTERVAL,    /* (double)                     rw */
  STRIPCONFIG_COLOR_BACKGROUND,         /* (cColor *)                   r  */
  STRIPCONFIG_COLOR_FOREGROUND,         /* (cColor *)                   r  */
  STRIPCONFIG_COLOR_GRID,               /* (cColor *)                   r  */
  STRIPCONFIG_COLOR_COLOR1,             /* (cColor *)                   r  */
  STRIPCONFIG_COLOR_COLOR2,             /* (cColor *)                   r  */
  STRIPCONFIG_COLOR_COLOR3,             /* (cColor *)                   r  */
  STRIPCONFIG_COLOR_COLOR4,             /* (cColor *)                   r  */
  STRIPCONFIG_COLOR_COLOR5,             /* (cColor *)                   r  */
  STRIPCONFIG_COLOR_COLOR6,             /* (cColor *)                   r  */
  STRIPCONFIG_COLOR_COLOR7,             /* (cColor *)                   r  */
  STRIPCONFIG_COLOR_COLOR8,             /* (cColor *)                   r  */
  STRIPCONFIG_COLOR_COLOR9,             /* (cColor *)                   r  */
  STRIPCONFIG_COLOR_COLOR10,            /* (cColor *)                   r  */
  STRIPCONFIG_OPTION_GRID_XON,          /* (int)                        rw */
  STRIPCONFIG_OPTION_GRID_YON,          /* (int)                        rw */
  STRIPCONFIG_OPTION_AXIS_YCOLORSTAT,   /* (int)                        rw */
  STRIPCONFIG_OPTION_GRAPH_LINEWIDTH,   /* (int)                        rw */

  STRIPCONFIG_TERMINATOR
}
StripConfigAttributeEnum;

#define STRIPCONFIG_FIRST_ATTRIBUTE     1
#define STRIPCONFIG_LAST_ATTRIBUTE      (STRIPCONFIG_TERMINATOR-1)
#define STRIPCONFIG_NUM_ATTRIBUTE       \
(STRIPCONFIG_LAST_ATTRIBUTE - STRIPCONFIG_FIRST_ATTRIBUTE + 1)



/* StripConfigMaskElement
 *
 *      Individual elements which can be set or unset in a StripConfigMask
 *      data structure.  There is a one-to-one correspondance between
 *      StripConfigAttribute values and the related StripConfigMaskElement
 *      values.
 */
typedef enum    _StripConfigMaskElementEnum
{
  SCFGMASK_TITLE                        = STRIPCONFIG_TITLE,
  SCFGMASK_FILENAME                     = STRIPCONFIG_FILENAME,

  /* time */
  SCFGMASK_TIME_TIMESPAN                = STRIPCONFIG_TIME_TIMESPAN,
  SCFGMASK_TIME_NUM_SAMPLES             = STRIPCONFIG_TIME_NUM_SAMPLES,
  SCFGMASK_TIME_SAMPLE_INTERVAL         = STRIPCONFIG_TIME_SAMPLE_INTERVAL,
  SCFGMASK_TIME_REFRESH_INTERVAL        = STRIPCONFIG_TIME_REFRESH_INTERVAL,

  /* color */
  SCFGMASK_COLOR_BACKGROUND             = STRIPCONFIG_COLOR_BACKGROUND,
  SCFGMASK_COLOR_FOREGROUND             = STRIPCONFIG_COLOR_FOREGROUND,
  SCFGMASK_COLOR_GRID                   = STRIPCONFIG_COLOR_GRID,
  SCFGMASK_COLOR_COLOR1                 = STRIPCONFIG_COLOR_COLOR1,
  SCFGMASK_COLOR_COLOR2                 = STRIPCONFIG_COLOR_COLOR2,
  SCFGMASK_COLOR_COLOR3                 = STRIPCONFIG_COLOR_COLOR3,
  SCFGMASK_COLOR_COLOR4                 = STRIPCONFIG_COLOR_COLOR4,
  SCFGMASK_COLOR_COLOR5                 = STRIPCONFIG_COLOR_COLOR5,
  SCFGMASK_COLOR_COLOR6                 = STRIPCONFIG_COLOR_COLOR6,
  SCFGMASK_COLOR_COLOR7                 = STRIPCONFIG_COLOR_COLOR7,
  SCFGMASK_COLOR_COLOR8                 = STRIPCONFIG_COLOR_COLOR8,
  SCFGMASK_COLOR_COLOR9                 = STRIPCONFIG_COLOR_COLOR9,
  SCFGMASK_COLOR_COLOR10                = STRIPCONFIG_COLOR_COLOR10,

  /* options */
  SCFGMASK_OPTION_GRID_XON              = STRIPCONFIG_OPTION_GRID_XON,
  SCFGMASK_OPTION_GRID_YON              = STRIPCONFIG_OPTION_GRID_YON,
  SCFGMASK_OPTION_AXIS_YCOLORSTAT       = STRIPCONFIG_OPTION_AXIS_YCOLORSTAT,
  SCFGMASK_OPTION_GRAPH_LINEWIDTH       = STRIPCONFIG_OPTION_GRAPH_LINEWIDTH,

  /* curves */
  SCFGMASK_CURVE_NAME,
  SCFGMASK_CURVE_EGU,
  SCFGMASK_CURVE_COMMENT,
  SCFGMASK_CURVE_PRECISION,
  SCFGMASK_CURVE_MIN,
  SCFGMASK_CURVE_MAX,
  SCFGMASK_CURVE_SCALE,
  SCFGMASK_CURVE_PLOTSTAT,

  SCFGMASK_TERMINATOR
}
StripConfigMaskElementEnum;

#define SCFGMASK_FIRST_ELEMENT  1
#define SCFGMASK_LAST_ELEMENT   (SCFGMASK_TERMINATOR-1)
#define SCFGMASK_NUM_ELEMENTS   \
(SCFGMASK_LAST_ELEMENT - SCFGMASK_FIRST_ELEMENT + 1)



/* StripConfigMask
 *
 *      Structure containing info bits.  Must be accessed through
 *      utility functions.
 */
#define STRIPCFGMASK_NBYTES     \
((SCFGMASK_NUM_ELEMENTS + CHAR_BIT) / CHAR_BIT)

typedef struct _StripConfigMask
{
  char  bytes[STRIPCFGMASK_NBYTES];
}
StripConfigMask;

/* StripConfigMask_clear
 *
 *      Zeroes out all bits.
 */
void    StripConfigMask_clear   (StripConfigMask *);


/* StripConfigMask_set
 *
 *      Sets the specified bit high.
 */
#define StripConfigMask_set(pmask, elem) \
((pmask)->bytes[((elem)-SCFGMASK_FIRST_ELEMENT)/CHAR_BIT] |= \
 (1 << (((elem)-SCFGMASK_FIRST_ELEMENT)%CHAR_BIT)))

/* StripConfigMask_unset
 *
 *      Sets the specified bit low.
 */
#define StripConfigMask_unset(pmask, elem) \
((pmask)->bytes[((elem)-SCFGMASK_FIRST_ELEMENT)/CHAR_BIT] &= \
 ~(1 << (((elem)-SCFGMASK_FIRST_ELEMENT)%CHAR_BIT)))


/* StripConfigMask_stat
 *
 *      Returns the status of the specified bit.
 */
#define StripConfigMask_stat(pmask, elem) \
((pmask)->bytes[((elem)-SCFGMASK_FIRST_ELEMENT)/CHAR_BIT] & \
 (1 << (((elem)-SCFGMASK_FIRST_ELEMENT)%CHAR_BIT)))


/* StripConfigMask_test
 *
 *      Takes two masks, A and B.  Returns true if B AND A == B.
 */
int     StripConfigMask_test    (StripConfigMask *A, StripConfigMask *B);


/* StripConfigMask_and
 *
 *      A = A AND B
 */
void    StripConfigMask_and     (StripConfigMask *A, StripConfigMask *B);


/* StripConfigMask_or
 *
 *      A = A OR B
 */
void    StripConfigMask_or      (StripConfigMask *A, StripConfigMask *B);


/* StripConfigMask_xor
 *
 *      A = A XOR B
 */
void    StripConfigMask_xor     (StripConfigMask *A, StripConfigMask *B);


/* StripConfigMask_intersect
 *
 *      returns {A AND B} != {}
 */
int     StripConfigMask_intersect (StripConfigMask *A, StripConfigMask *B);


/* STRIPCFGMASK_XXX
 *
 *      Predefined Masks comprised of related sets of elements.
 */
extern StripConfigMask  SCFGMASK_TIME;
extern StripConfigMask  SCFGMASK_COLOR;
extern StripConfigMask  SCFGMASK_OPTION;
extern StripConfigMask  SCFGMASK_CURVE;
extern StripConfigMask  SCFGMASK_ALL;




/* ======= Data Types ======= */
typedef void    (*StripConfigUpdateFunc) (StripConfigMask, void *);
  
#ifndef NO_X11_HERE /* Albert */ 
typedef struct _StripCurveDetail
{
  char                  name[STRIP_MAX_NAME_CHAR+1];
  char                  egu[STRIP_MAX_EGU_CHAR+1];
  char                  comment[STRIP_MAX_COMMENT_CHAR+1];
  int                   precision;
  double                min, max;
  int                   scale;
  int                   plotstat;
  short                 valid;
  cColor                *color;
  void                  *id;
  StripConfigMask       update_mask;    /* fields updated since last update */
  StripConfigMask       set_mask;       /* fields without default values */
}
StripCurveDetail;

typedef struct _StripConfig
{
  cColorManager                 scm;
  XVisualInfo                   xvi;
  char                          *title;
  char                          *filename;
  FILE                          *logfile;
#if 0
/* KE: Not used, not available on WIN32 */
  struct passwd                 user;
#endif
  struct _Time {
    unsigned                    timespan;
    int                         num_samples;
    double                      sample_interval;
    double                      refresh_interval;
  } Time;

  struct _Color {
    cColor                      background;
    cColor                      foreground;
    cColor                      grid;
    cColor                      color[STRIP_MAX_CURVES];
  } Color;

  struct _Option {
    int                         grid_xon;
    int                         grid_yon;
    int                         axis_ycolorstat;
    int                         graph_linewidth;
  } Option;

  struct _Curves {
    StripCurveDetail            Detail[STRIP_MAX_CURVES];
    int                         plot_order[STRIP_MAX_CURVES];
  } Curves;

  struct _UpdateInfo {
    StripConfigMask             update_mask;
    struct _Callbacks {
      StripConfigMask           call_event;
      StripConfigUpdateFunc     call_func;
      void                      *call_data;
    } Callbacks[STRIPCONFIG_MAX_CALLBACKS];
    int callback_count;
  } UpdateInfo;
}
StripConfig;

#endif /* Albert */ 
/*
 * StripConfig_preinit
 *
 *      Initializes global constants (SCFGMASK_TIME, etc.)
 */
void    StripConfig_preinit     (void);



/*
 * StripConfig_init
 *
 *      Allocates storage for a StripConfig structure and initializes it
 *      with default values.  Then reads config info from the specified
 *      stdio stream, if it's not null.  See StripConfig_load() below for
 *      specifics on how the file is read.
 */
#ifndef NO_X11_HERE /* Albert */ 
StripConfig     *StripConfig_init       (cColorManager,
                                         XVisualInfo *,
                                         FILE *,
                                         StripConfigMask);

/* StripConfig_clone
 *
 *      Creates and returns a copy of the specified config object.
 */
StripConfig     *StripConfig_clone      (StripConfig *);


/*
 * StripConfig_delete
 *
 *      Deallocates storage for the StripConfig structure.
 */
void    StripConfig_delete      (StripConfig *);


/*
 * StripConfig_set/getattr
 *
 *      Sets/gets the specified attributes, returning true on success.
 */
int     StripConfig_setattr     (StripConfig *, ...);
int     StripConfig_getattr     (StripConfig *, ...);


/*
 * StripConfig_write
 *
 *      Attempts to write configuration info to the specified stdio stream.
 *      Writing is begun at the current file pointer.  Output format is:
 *      <ascii-attribute-name><whitespace><ascii-attribute-value><end-of-line>
 *      for each selected attribute.  The mask specifies which attributes to
 *      write to the file.
 */
int     StripConfig_write       (StripConfig *, FILE *, StripConfigMask);


/*
 * StripConfig_load
 *
 *      Attempts to read configuration info from the specified stdio stream.
 *      Reading is begun at the current file pointer and continues until an
 *      end-of-file is encoutered.  Any unrecognized lines are ignored.
 *      The mask specifies which attributes to load from the file.
 */
int     StripConfig_load        (StripConfig *, FILE *, StripConfigMask);


/*
 * StripConfig_addcallback
 *
 *      Causes all update event callbacks which match the mask to be called.
 */
int     StripConfig_addcallback (StripConfig *,
                                 StripConfigMask,
                                 StripConfigUpdateFunc, /* callback function */
                                 void *);               /* client data */


/*
 * StripConfig_update
 *
 *      Causes all pending update event callbacks which match the
 *      mask to be called.
 */
void    StripConfig_update      (StripConfig *, StripConfigMask);



/*
 * StripConfig_reset_details
 *
 *      Given a StripCurveDetail pointer, intializes all fields
 *      to defaults.
 */
void    StripConfig_reset_details       (StripConfig *, StripCurveDetail *);
#endif /* Albert */
#endif
