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

#include <Xm/XmP.h>
#include "jlAxisP.h"

#include "rotated.h"
#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <math.h>
#include <time.h>


#define offset(field) XtOffsetOf(AxisRec, field)

#define DEFAULT_MIN_MARGIN      50
#define DEFAULT_MINOR_SIDE      DEFAUILT_MIN_MARGIN
#define DEFAULT_MAJOR_SIDE      150
#define MIN_SIDE                      3
#define BUF_SIZE                      256

static double   DEFAULT_MINVAL          = 0.0;
static double   DEFAULT_MAXVAL          = 100.0;
static double   DEFAULT_LOGEPSILON      = 0;
static char             PLUS_MINUS                    = (char)254;

enum                    TicDivisorNames
{
  One = 0, Two, Five, Ten, NumTicDivisors
};
static double           TicDivisorValues[]      = {1, 2, 5, 10};

enum                    TimeUnitNames
{
  Seconds = 0, Minutes, Hours, Days, Weeks, Months, Years, NumTimeUnits
};
static char             *TimeUnitNameStrings[]  =
{
  "Seconds", "Minutes", "Hours", "Days", "Weeks", "Months", "Years"
};

static double           TimeUnitValues[]        =
{
  1,                    /* seconds */
  60,                   /* minutes */
  60*60,                /* hours */
  24*60*60,             /* days */
  7*24*60*60,           /* weeks */
  30*24*60*60,          /* months */
  365*24*60*60          /* years */
};

/* MaxTimeUnits
 *
 *  The maximum number of intervals for a particular time unit before adjusting
 *  scale to the next.
 */
static int              MaxTimeIntervals[] =
{
  180,                  /* 3 minutes */
  180,                  /* 3 hours */
  72,                   /* 3 days */
  21,                   /* 3 weeks */
  12,                   /* ~3 months */
  36,                   /* 3 years */
  100                   /* a century */
};

/* TimeFormats
 *
 *  Format strings for strftime for corresponding level of detail
 */
static const char *     TimeFormats[] =
{
  "%H:%M:%S\n%b %d, %Y",      /* seconds */
  "%H:%M\n%b %d, %Y",         /* minutes */
  "%H\n%b %d, %Y",            /* hours */
  "%a\n%b %d, %Y",            /* days */
  "%a\n%b %d, %Y",            /* weeks */
  "%b %Y",                    /* months */
  "%Y"                        /* years */
};


/* resources
 */
static XtResource       resources[] =
{
  /* font */
  {
    XmNfont,
    XmCFont,
    XmRFontStruct,
    sizeof (XFontStruct),
    offset (axis.font),
    XmRString,
    (XtPointer)XtDefaultFont
  },

  /* units string */
  {
    XjNunitString,
    XjCUnitString,
    XmRString,
    sizeof (String),
    offset (axis.unit_string),
    XmRImmediate,
    (XtPointer)0
  },

  /* direction */
  {
    XjNdirection,
    XjCDirection,
    XjRAxisDirection,
    sizeof (AxisDirection),
    offset (axis.direction),
    XmRImmediate,
    (XtPointer)XjAXIS_LEFT
  },

  /* value type */
  {
    XjNvalueType,
    XjCValueType,
    XjRAxisValueType,
    sizeof (AxisValueType),
    offset (axis.value_type),
    XmRImmediate,
    (XtPointer)XjAXIS_REAL
  },

  /* transform */
  {
    XjNtransform,
    XjCTransform,
    XjRAxisTransform,
    sizeof (AxisTransform),
    offset (axis.transform),
    XmRImmediate,
    (XtPointer)XjAXIS_LINEAR
  },

  /* log epsilon */
  {
    XjNlogEpsilon,
    XjCLogEpsilon,
    XjRDouble,
    sizeof (double),
    offset (axis.log_epsilon),
    XjRDouble,
    (XtPointer)&DEFAULT_LOGEPSILON
  },

  /* min_val */
  {
    XjNminVal,
    XjCMinVal,
    XjRDouble,
    sizeof (double),
    offset (axis.min_val),
    XjRDouble,
    (XtPointer)&DEFAULT_MINVAL
  },

  /* max_val */
  {
    XjNmaxVal,
    XjCMaxVal,
    XjRDouble,
    sizeof (double),
    offset (axis.max_val),
    XjRDouble,
    (XtPointer)&DEFAULT_MAXVAL
  },

  /* min_pos */
  {
    XjNminPos,
    XjCMinPos,
    XjRAxisEndpointPosition,
    sizeof (AxisEndpointPosition),
    offset (axis.min_pos),
    XmRImmediate,
    (XtPointer)XjAXIS_ORIGIN
  },

  /* max_pos */
  {
    XjNmaxPos,
    XjCMaxPos,
    XjRAxisEndpointPosition,
    sizeof (AxisEndpointPosition),
    offset (axis.max_pos),
    XmRImmediate,
    (XtPointer)XjAXIS_TERMINUS
  },

  /* min_margin */
  {
    XjNminMargin,
    XjCMinMargin,
    XmRDimension,
    sizeof (Dimension),
    offset (axis.min_margin),
    XmRImmediate,
    (XtPointer)DEFAULT_MIN_MARGIN
  },

  /* use_pixmap */
  {
    XjNusePixmap,
    XjCUsePixmap,
    XmRBoolean,
    sizeof (Boolean),
    offset (axis.use_pixmap),
    XmRImmediate,
    (XtPointer)False
  },

  /* text color */
  {
    XjNtextColor,
    XjCTextColor,
    XmRPixel,
    sizeof (Pixel),
    offset (axis.text_color),
    XmRImmediate,
    (XtPointer)XtDefaultForeground
  }, 

  /* ptr min_val */
  {
    XjNpMinVal,
    XjCPMinVal,
    XtRPointer,
    sizeof (double *),
    offset (axis.pmin_val),
    XmRImmediate,
    (double *)0
  },

  /* ptr max_val */
  {
    XjNpMaxVal,
    XjCPMaxVal,
    XtRPointer,
    sizeof (double *),
    offset (axis.pmax_val),
    XmRImmediate,
    (double *)0
  },

  /* ptr log epsilon */
  {
    XjNpLogEpsilon,
    XjCPLogEpsilon,
    XtRPointer,
    sizeof (double *),
    offset (axis.plog_epsilon),
    XmRImmediate,
    (double *)0
  }
};


/* static variables
 */
static Boolean          quarks_initialized = False;
static XrmQuark         QAxisUp, QAxisDown, QAxisLeft, QAxisRight;
static XrmQuark         QAxisOrigin, QAxisTerminus;
static XrmQuark         QAxisLinear, QAxisLog10;
static XrmQuark         QAxisReal, QAxisAbsTime;
static XrmQuark         QAxisRelTimeDates, QAxisRelTimeNumbers;
static char                   buf[BUF_SIZE+1];
 


/* method prototypes
 */
static void           ClassInitialize   (void);

static void     Initialize            (Widget,  /* request */
                                     Widget,    /* new */
                                     ArgList,   /* args */
                                     Cardinal *);       /* num_args */

static void     Redisplay             (Widget,
                                     XEvent *,  /* event */
                                     Region);   /* region */

static void     Destroy         (Widget);

static void     Resize          (Widget);

static Boolean  SetValues             (Widget,        /* old */
                                     Widget,          /* request */
                                     Widget,          /* new */
                                     ArgList,         /* args */
                                     Cardinal *);       /* num_args */

static XtGeometryResult QueryGeometry     (Widget,              
                                           XtWidgetGeometry *,  /* request */
                                           XtWidgetGeometry *); /* reply */

/* action function prototypes
 */
static void           Arm               (Widget,
                                     XEvent *,
                                     String *,        /* params */
                                     Cardinal *);       /* num_params */

/* private function prototypes
 */
static Boolean  ValidateValues  (AxisWidget);
static Boolean  ValidatePositions       (AxisWidget);
static Boolean  ValidateTransform       (AxisWidget);

static int      FindGoodMargin  (AxisWidget);

static void     GetDrawGC               (AxisWidget);
static void     FreeDrawGC              (AxisWidget);
static void     GetTextGC               (AxisWidget);
static void     FreeTextGC              (AxisWidget);
static void     GetEraseGC              (AxisWidget);
static void     FreeEraseGC             (AxisWidget);
static void     GetPixmap               (AxisWidget);
static void     FreePixmap              (AxisWidget);
static void     Draw                    (AxisWidget, Drawable);

/* these all return true if they precipitate a need to redisplay */
static int      ComputeTics             (AxisWidget);
static int      ComputeTimeTics (AxisWidget);
static int      ComputeNormalTics       (AxisWidget);
static int      ComputeLogTics  (AxisWidget);

/* helper functions
 */
static void     find_good_linear_tics   (AxisWidget,    /* for XtAppErrorMsg */
                                     Boolean,         /* nice intervals? */
                                     double,          /* min val */
                                     double,          /* max val */
                                     double *,        /* tic magnitude */
                                     double *,        /* minor divisor */
                                     double *);       /* major divisor */

static void     find_good_log_tics      (AxisWidget,    /* for XtAppErrorMsg */
                                     double *,        /* tic magnitude */
                                     int *,             /* num minor */
                                     int *,             /* num major */
                                     double *,        /* minor divisor */
                                     double *);       /* major divisor */

static void     compute_log_info        (AxisWidget);

static int      get_log_index_value     (AxisWidget,    
                                     int,               /* index */
                                     int *,     /* power */
                                     int *);    /* sign */


static int      get_log_info            (double,        /* min_val */
                                     double,    /* max_val */
                                     double,    /* log_epsilon */
                                     short *,   /* log_epsilon_index */
                                     short *,   /* log_n_powers */
                                     double *,  /* log_delta */
                                     double *); /* log_epsilon_offset*/

static void     transform_values_normalized     (AxisTransform,
                                           AxisValueType,
                                           AxisEndpointPosition,
                                           AxisEndpointPosition,
                                           register double,
                                           register double,
                                           register double,
                                           register double,
                                           register double,
                                           register double *,
                                           register double *,
                                           register int);

static void     untransform_normalized_values     (AxisTransform,
                                           AxisValueType,
                                           AxisEndpointPosition,
                                           AxisEndpointPosition,
                                           register double,
                                           register double,
                                           register double,
                                           register double,
                                           register double,
                                           register double *,
                                           register double *,
                                           register int);

/* converter prototypes
 */
static Boolean  CvtStringToAxisDirection        (Display *,
                                                 XrmValue *,    /* args */
                                                 Cardinal *,    /* n args */
                                                 XrmValue *,    /* from */
                                                 XrmValue *,    /* to */
                                                 XtPointer *);  /* data */
static Boolean  CvtStringToAxisValueType        (Display *,
                                                 XrmValue *,    /* args */
                                                 Cardinal *,    /* n args */
                                                 XrmValue *,    /* from */
                                                 XrmValue *,    /* to */
                                                 XtPointer *);  /* data */
static Boolean  CvtStringToAxisTransform        (Display *,
                                                 XrmValue *,    /* args */
                                                 Cardinal *,    /* n args */
                                                 XrmValue *,    /* from */
                                                 XrmValue *,    /* to */
                                                 XtPointer *);  /* data */
static Boolean  CvtStringToAxisEndpointPosition(Display *,
                                                  XrmValue *,   /* args */
                                                  Cardinal *,   /* n args */
                                                  XrmValue *,   /* from */
                                                  XrmValue *,   /* to */
                                                  XtPointer *); /* data */
static Boolean  CvtStringToDouble               (Display *,
                                           XrmValue *,  /* args */
                                           Cardinal *,  /* n args */
                                           XrmValue *,  /* from */
                                           XrmValue *,  /* to */
                                           XtPointer *);        /* data */
                                                         


/* default translations
 */
static char             defaultTranslations[] =
"<Btn1Down>:    Arm(1)  \n"
"<Btn2Down>:    Arm(2)  \n"
"<Btn3Down>:    Arm(3)  \n";

/* actions
 */
static XtActionsRec     actions[] =
{
  {"Arm",       Arm}
};


/* class record
 */
AxisClassRec            axisClassRec = {
  /* core_class fields */
  {
    /* superclass             */    (WidgetClass) &xmPrimitiveClassRec,
    /* class_name             */    "Axis",
    /* widget_size            */    sizeof (AxisRec),
    /* class_initialize       */    ClassInitialize,
    /* class_part_initialize    */      NULL,
    /* class_inited             */      FALSE,
    /* initialize                     */        Initialize,
    /* initialize_hook          */      NULL,
    /* realize                  */      XtInheritRealize,
    /* actions                  */      actions,
    /* num_actions              */      XtNumber (actions),
    /* resources                      */        resources,
    /* num_resources            */      XtNumber (resources),
    /* xrm_class                      */        NULLQUARK,
    /* compress_motion          */      TRUE,
    /* compress_exposure        */      XtExposeCompressMultiple,
    /* compress_enterleave      */      FALSE,
    /* visible_interest         */      FALSE,
    /* destroy                  */      Destroy,
    /* resize                   */      Resize,
    /* expose                   */      Redisplay,
    /* set_values                     */        SetValues,
    /* set_values_hook          */      NULL,
    /* set_values_almost        */      XtInheritSetValuesAlmost,
    /* get_values_hook          */      NULL,
    /* accept_focus             */      NULL,
    /* version                  */      XtVersion,
    /* callback_private         */      NULL,
    /* tm_table                 */      defaultTranslations,
    /* query_geometry           */      QueryGeometry,
    /* display_accelerator      */      XtInheritDisplayAccelerator,
    /* extension                      */        NULL
  },

  /* Primitive class fields */  
  {
    /* border_highlight         */      XmInheritWidgetProc,       
    /* border_unhighlight       */      XmInheritWidgetProc,    
    /* translations             */      XmInheritTranslations,      
    /* arm_and_activate         */      NULL,             
    /* syn resources            */      NULL,           
    /* num_syn_resources        */      0, 
    /* extension                */      NULL,
  },

  /* Axis */
  {
    /* placeholder              */      0,
  },
};

WidgetClass axisWidgetClass = (WidgetClass) &axisClassRec;


/* ========================================================================
 *
 *         >>>>>>>>>>   U S E F U L   M A C R O S   <<<<<<<<<<
 *
 * ========================================================================
 */
/* ABS
 *
 *      Returns absolute value of number.
 */
#if !defined (ABS)
#  define ABS(X)        ((X) < 0? (-(X)) : (X))
#endif


/* MAX
 *
 */
#if !defined (MAX)
#  define MAX(a,b)      ((a) > (b)? (a) : (b))
#endif


/* ROUND
 *
 *      Returns the integer nearest the given (small) number.
 */
#define ROUND(X)        ((X) < 0? (int)((X)-0.5) : (int)((X)+0.5))


/* LENGTH
 *
 *      Returns the length of the widget's side parallel to the axis line.
 */
#define LENGTH(w) \
((((w)->axis.direction == XjAXIS_UP) || ((w)->axis.direction == XjAXIS_DOWN))?\
 (w)->core.width : (w)->core.height)


/* ORIENTATION
 *
 *      Returns XmHORIZONTAL for UP/DOWN widgets, else XmVERTICAL
 */
#define ORIENTATION(w) \
((((w)->axis.direction == XjAXIS_UP) || ((w)->axis.direction == XjAXIS_DOWN))?\
 XmHORIZONTAL : XmVERTICAL)


/* MINPOS
 *
 *      Returns the offset of the axis endpoint corresponding to
 *      the min value.
 */
#define MINPOS(w) \
(((w)->axis.min_pos == XjAXIS_ORIGIN)? 0 : (w)->axis.min_pos)


/* MAXPOS
 *
 *      Returns the offset of the axis endpoint corresponding to
 *      the max value.
 */
#define MAXPOS(w) \
(((w)->axis.max_pos == XjAXIS_TERMINUS)? (LENGTH((w))-1) : (w)->axis.max_pos)


/* ========================================================================
 *
 *         >>>>>>>>>>   W I D G E T   M E T H O D S   <<<<<<<<<<
 *
 * ========================================================================
 */

/* ClassInitialize
 */
static void
ClassInitialize (void)
{
  if (!quarks_initialized)
  {
    /* let's make some quarks! */
    QAxisUp = XrmStringToQuark ("XjAXIS_UP");
    QAxisDown = XrmStringToQuark ("XjAXIS_DOWN");
    QAxisLeft = XrmStringToQuark ("XjAXIS_LEFT");
    QAxisRight = XrmStringToQuark ("XjAXIS_RIGHT");
    QAxisOrigin = XrmStringToQuark ("XjAXIS_ORIGIN");
    QAxisTerminus = XrmStringToQuark ("XjAXIS_TERMINUS");
    QAxisReal = XrmStringToQuark ("XjAXIS_REAL");
    QAxisAbsTime = XrmStringToQuark ("XjAXIS_ABSTIME");
    QAxisRelTimeDates = XrmStringToQuark ("XjAXIS_RELTIME_DATES");
    QAxisRelTimeNumbers = XrmStringToQuark ("XjAXIS_RELTIME_NUMBERS");
    QAxisLinear = XrmStringToQuark ("XjAXIS_LINEAR");
    QAxisLog10 = XrmStringToQuark ("XjAXIS_LOG10");
    quarks_initialized = True;
  }

  /* register type converters */
  XtSetTypeConverter
    (XmRString, XjRAxisDirection, CvtStringToAxisDirection,
     (XtConvertArgList)0, 0, XtCacheNone, 0);
  XtSetTypeConverter
    (XmRString, XjRAxisValueType, CvtStringToAxisValueType,
     (XtConvertArgList)0, 0, XtCacheNone, 0);
  XtSetTypeConverter
    (XmRString, XjRAxisTransform, CvtStringToAxisTransform,
     (XtConvertArgList)0, 0, XtCacheNone, 0);
  XtSetTypeConverter
    (XmRString, XjRAxisEndpointPosition, CvtStringToAxisEndpointPosition,
     (XtConvertArgList)0, 0, XtCacheNone, 0);
  XtSetTypeConverter
    (XmRString, XjRDouble, CvtStringToDouble,
     (XtConvertArgList)0, 0, XtCacheNone, 0);
}


/* Initialize
 */
static void
Initialize      (Widget         treq,
             Widget     tnew,
             ArgList    args,
             Cardinal   *num_args)
{
  AxisWidget nw = (AxisWidget) tnew;

  /* verify that min_val is less than max_val */
  ValidateValues (nw);

  /* Motif Primitive sets width and height to provide enough room for
   * the highlight and shadow around a widget.  A widget that uses these
   * features would *add* its desired dimensions to those set by Primitive.
   */
  if (ORIENTATION(nw) == XmHORIZONTAL)
  {
    nw->core.width = DEFAULT_MAJOR_SIDE;
    nw->core.height = MAX (nw->axis.min_margin, FindGoodMargin (nw));
  }
  else
  {
    nw->core.width = MAX (nw->axis.min_margin, FindGoodMargin (nw));
    nw->core.height = DEFAULT_MAJOR_SIDE;
  }

  /* don't want any border */
  nw->core.border_width = 0;

  /* verify that min and max positions are ok */
  ValidatePositions (nw);

  /* tell Primitive not to allow tabbing to this widget,
   * set shadow thickness to 0, etc. */ 
  XtVaSetValues
    ((Widget)nw,
     XmNtraversalOn,            False,
     XmNshadowThickness,        (Dimension)0,
     XmNhighlightThickness,     (Dimension)0,
     NULL);

  nw->axis.pixmap = 0;
  nw->axis.need_refresh = True;
  
  GetDrawGC (nw);
  GetEraseGC (nw);
  GetTextGC (nw);
  ComputeTics (nw);
}


/* Redisplay
 *
 *      If the event is null, then the function was called
 *      by the widget itself.
 */
static void
Redisplay       (Widget w, XEvent *event, Region region)
{
  AxisWidget    cw = (AxisWidget)w;
  XExposeEvent  *expose = 0;

  if (!XtIsRealized ((Widget)cw)) return;
  if (!cw->axis.pixmap && cw->axis.use_pixmap) GetPixmap (cw);

  /* determine event type */
  if (event) if (event->type == Expose) expose = (XExposeEvent *)event;
    
  /* if no pixmap, draw directly into window */
  if (!cw->axis.pixmap)
  {
    /* If this wasn't an expose event, then the window hasn't
     * been cleared for us */
    if (!expose) XClearWindow (XtDisplay (cw), XtWindow (cw));
    Draw (cw, XtWindow (cw));
    cw->axis.need_refresh = False;
  }

  /* use pixmap */
  else
  {
    /* if the pixmap needs to be updated, do that */
    if (cw->axis.need_refresh) Draw (cw, cw->axis.pixmap);
    cw->axis.need_refresh = False;

    /* if called internally, redraw the whole area */
    if (!expose)
      XCopyArea
        (XtDisplay (cw), cw->axis.pixmap, XtWindow (cw), cw->axis.draw_gc,
         0, 0, cw->core.width, cw->core.height, 0, 0);
                  
    /* otherwise, copy the bounding box for the exposed region
     * from the pixmap */
    else
      XCopyArea
        (XtDisplay (cw), cw->axis.pixmap, XtWindow (cw), cw->axis.draw_gc,
         expose->x, expose->y, expose->width, expose->height,
         expose->x, expose->y);
  }
}



static Boolean
SetValues       (Widget cur_w,
             Widget     req_w,
             Widget     new_w,
             ArgList    args,
             Cardinal   *num_args)
{
  AxisWidget    cur = (AxisWidget) cur_w;
  AxisWidget    nw = (AxisWidget) new_w;
  Boolean             do_redisplay = False;
  Boolean             do_recompute = False;
  ArgList       arg;
  Cardinal      nargs=*num_args;

  for (arg=args; nargs!=0; nargs--,arg++) {
      if (strcmp(arg->name,XjNpMinVal)==0 && arg->value) 
           nw->axis.min_val=*(double *)arg->value;
      if (strcmp(arg->name,XjNpMaxVal)==0 && arg->value) 
           nw->axis.max_val=*(double *)arg->value;
      if (strcmp(arg->name,XjNpLogEpsilon)==0 && arg->value) 
           nw->axis.log_epsilon=*(double *)arg->value;
  }

  /* new text gc? */
  if ((cur->axis.text_color != nw->axis.text_color) ||
      (cur->axis.font != nw->axis.font))
  {
    FreeTextGC (nw);
    GetTextGC (nw);
    nw->axis.need_refresh = True;
  }

  /* new draw gc? */
  if ((cur->primitive.foreground != nw->primitive.foreground) ||
      (cur->core.background_pixel != nw->core.background_pixel))
  {
    FreeDrawGC (nw);
    GetDrawGC (nw);
    nw->axis.need_refresh = True;
  }

  /* need new erase gc? */
  if (cur->core.background_pixel != nw->core.background_pixel)
  {
    FreeEraseGC (nw);
    GetEraseGC (nw);
    nw->axis.need_refresh = True;
  }

  /* if the units have changed, we need to update the pixmap
   * and update the window contents */
  if (cur->axis.unit_string && nw->axis.unit_string)
    if (strcmp (cur->axis.unit_string, nw->axis.unit_string) != 0)
      nw->axis.need_refresh = True;

  if ((cur->axis.unit_string && !nw->axis.unit_string) ||
      (!cur->axis.unit_string && nw->axis.unit_string))
    nw->axis.need_refresh = True;

  /* start or stop using a pixmap? */
  if (cur->axis.use_pixmap != nw->axis.use_pixmap)
  {
    if (!nw->axis.use_pixmap && cur->axis.pixmap)
      FreePixmap (nw);
    nw->axis.pixmap = 0;
    nw->axis.need_refresh = True;
  }

  /* if the orientation has changed, then we need to update
   * the pixmap. */
  if (cur->axis.direction != nw->axis.direction)
    nw->axis.need_refresh = True;

  /* if the min margin has grown bigger than the current
   * margin, then we need to resize */
  if (cur->axis.min_margin != nw->axis.min_margin)
  {
    if (ORIENTATION (nw) == XmHORIZONTAL)
      nw->core.height = MAX (nw->axis.min_margin, nw->core.height);
    else
      nw->core.width = MAX (nw->axis.min_margin, nw->core.width);
  }

  /* if the dimensions have changed, then we need to get a new
   * pixmap and redraw */
  if ((cur->core.width != nw->core.width) ||
      (cur->core.height != nw->core.height))
  {
    if (nw->axis.pixmap) FreePixmap (nw);
    nw->axis.need_refresh = True;
  }

  /* transform function */
  ValidateTransform (nw);
  if (cur->axis.transform != nw->axis.transform)
  {
    do_recompute = True;
    nw->axis.need_refresh = True;
  }

  /* positions */
  ValidatePositions (nw);
  if ((cur->axis.min_pos != nw->axis.min_pos) ||
      (cur->axis.max_pos != nw->axis.max_pos))
  {
    do_recompute = True;
    nw->axis.need_refresh = True;
    do_redisplay = True;
  }

  /* axis range */
  ValidateValues (nw);
  if ((ABS(cur->axis.min_val - nw->axis.min_val) > DBL_EPSILON) ||
      (ABS(cur->axis.max_val - nw->axis.max_val) > DBL_EPSILON) ||
      cur->axis.log_epsilon != nw->axis.log_epsilon)
  {
    do_recompute = True;
    nw->axis.need_refresh = True;
  }

  /* recompute if necessary */
  if (do_recompute) ComputeTics (nw);

  /* if we return True, then the X Toolkit will clear the widget
   * and invoke Redisplay.  We only want this to happen if we
   * aren't using a pixmap and the widget needs to be refreshed. */
  do_redisplay |= (nw->axis.need_refresh && !nw->axis.use_pixmap);
  if (nw->axis.need_refresh && !do_redisplay)
    Redisplay ((Widget)nw, 0, 0);
  
  return do_redisplay;
}


static void
Destroy         (Widget w)
{
  AxisWidget    cw = (AxisWidget)w;

  FreeDrawGC (cw);
  FreeTextGC (cw);
  FreeEraseGC (cw);
  FreePixmap (cw);
}


static void
Resize          (Widget w)
{
  AxisWidget cw = (AxisWidget)w;

  if (cw->axis.pixmap) FreePixmap (cw);
  ComputeTics (cw);
  cw->axis.need_refresh = 1;
}


static XtGeometryResult
QueryGeometry   (Widget                 w,
                   XtWidgetGeometry     *proposed,
                   XtWidgetGeometry     *answer)
{
  AxisWidget            cw = (AxisWidget)w;
  XtGeometryResult      response;

  answer->request_mode = CWWidth | CWHeight;

  /* ====== Horizontal ====== */
  if (ORIENTATION (cw) == XmHORIZONTAL)
  {
    /* for horizontal widgets, we don't care about width,
     * so accept the proposal if such exists, otherwise
     * suggest the current width */
    if (proposed->request_mode & CWWidth)
      answer->width = proposed->width;
    else answer->width = cw->core.width;

    if (proposed->request_mode & CWHeight)
    {
      if (proposed->height <= cw->axis.min_margin)
        answer->height = cw->axis.min_margin;
    }
    else answer->height = MAX (cw->core.height, cw->axis.min_margin);
  }

  /* ====== Vertical ====== */
  else
  {
    /* for vertical widgets, we don't care about height,
     * so accept the proposal if such exists, otherwise
     * suggest the current height */
    if (proposed->request_mode & CWHeight)
      answer->height = proposed->height;
    else answer->height = cw->core.height;

    if (proposed->request_mode & CWWidth)
    {
      if (proposed->width <= cw->axis.min_margin)
        answer->width = cw->axis.min_margin;
    }
    else answer->width = MAX (cw->core.width, cw->axis.min_margin);
  }

  if ((proposed->request_mode & CWWidth) &&
      (proposed->request_mode & CWHeight) &&
      (proposed->width = answer->width) &&
      (proposed->height = answer->height))
    response = XtGeometryYes;
  else if ((answer->width == cw->core.width) &&
           (answer->height == cw->core.height))
    response = XtGeometryNo;
  else
    response = XtGeometryAlmost;

  return response;
}


/* ========================================================================
 *
 *         >>>>>>>>>>   P R I V A T E   F U N C T I O N S   <<<<<<<<<<
 *
 * ========================================================================
 */

/* FindGoodMargin
 */
static int
FindGoodMargin  (AxisWidget cw)
{
  int   m;
  
  /* for time being, just use max ascent and descent and width.  Later,
   * perhaps determine dimensions depending upon which characters might
   * actually appear */
  if (ORIENTATION(cw) == XmVERTICAL)
  {
    /* max digits in number, sign, radix character */
    m = AXIS_MAX_DIGITS;
  }

  return m = 0;
}


/* GetDrawGC
 */
static void
GetDrawGC       (AxisWidget cw)
{
  XtGCMask      mask;
  XGCValues     values;

  mask = GCForeground | GCBackground;
  values.foreground = cw->primitive.foreground;
  values.background = cw->core.background_pixel;
  cw->axis.draw_gc = XtGetGC ((Widget)cw, mask, &values);
}


/* FreeDrawGC
 */
static void
FreeDrawGC      (AxisWidget cw)
{
  XtReleaseGC ((Widget)cw, cw->axis.draw_gc);
}


/* GetTextGC
 */
static void
GetTextGC       (AxisWidget cw)
{
  XtGCMask      mask;
  XGCValues     values;

  mask = GCForeground | GCFont;
  values.foreground = cw->axis.text_color;
  values.font = cw->axis.font->fid;
  cw->axis.text_gc = XtGetGC ((Widget)cw, mask, &values);
}


/* FreeTextGC
 */
static void
FreeTextGC      (AxisWidget cw)
{
  XtReleaseGC ((Widget)cw, cw->axis.text_gc);
}


/* GetEraseGC
 */
static void
GetEraseGC      (AxisWidget cw)
{
  XtGCMask      mask;
  XGCValues     values;

  mask = GCForeground;
  values.foreground = cw->core.background_pixel;
  cw->axis.erase_gc = XtGetGC ((Widget)cw, mask, &values);
}


/* FreeEraseGC
 */
static void
FreeEraseGC     (AxisWidget cw)
{
  XtReleaseGC ((Widget)cw, cw->axis.erase_gc);
}


/* GetPixmap
 */
static void
GetPixmap       (AxisWidget cw)
{
  if (!XtIsRealized ((Widget)cw)) return;

  cw->axis.pixmap = XCreatePixmap
    (XtDisplay (cw), XtWindow (cw),
     cw->core.width, cw->core.height, cw->core.depth);

  if (!cw->axis.pixmap)
    XtWarningMsg
      ("Resource Unavailable", "GetPixmap",
       "XtToolkitError", "Axis widget is unable to allocate server memory",
       (String *)0, (Cardinal *)0);
}


/* FreePixmap
 */
static void
FreePixmap      (AxisWidget cw)
{
  if (cw->axis.pixmap) XFreePixmap (XtDisplay (cw), cw->axis.pixmap);
  cw->axis.pixmap = 0;
}


/* ValidateValues
 */
static Boolean
ValidateValues          (AxisWidget cw)
{
  Boolean       ret = True;
  
  if ((cw->axis.max_val - cw->axis.min_val) <= DBL_EPSILON)
  {
    XtAppWarningMsg
      (XtWidgetToApplicationContext ((Widget)cw),
       "ValidateValues", "badValue", "XmCMinVal",
       "Axis: minVal is greater than or equal to maxVal",
       (String *)0, (Cardinal *)0);
    cw->axis.min_val = DEFAULT_MINVAL;
    cw->axis.max_val = DEFAULT_MAXVAL;
    ret = False;
  }

  return ret;
}


/* ValidateTransform
 */
static Boolean
ValidateTransform       (AxisWidget cw)
{
  Boolean       ret = True;
  double        e;

  /* verify, if transform is log10, that the min and max values
   * don't both lie within the log_epsilon area */
  if (cw->axis.transform == XjAXIS_LOG10)
  {
    e = pow (10, cw->axis.log_epsilon);
    if ((ABS(cw->axis.min_val) <= ABS(e)) && (ABS(cw->axis.max_val) <= ABS(e)))
    {
      XtAppWarningMsg
        (XtWidgetToApplicationContext ((Widget)cw),
         "ValidateTransform", "badValue", "XmCTransform",
         "Axis: given the current precision, the min and max values\n"
         "      will both fall within the undefined area for the\n"
         "      logarithmic transform.",
         (String *)0, (Cardinal *)0);
      cw->axis.transform = XjAXIS_LINEAR;
      ret = False;
    }
  }

  return ret;
}


/* ValidatePositions
 */
static Boolean
ValidatePositions       (AxisWidget cw)
{
  Boolean       ret = True;

  if (MINPOS(cw) >= MAXPOS(cw))
  {
    XtAppWarningMsg
      (XtWidgetToApplicationContext ((Widget)cw),
       "ValidatePositions", "badValue", "XmCMinPos",
       "Axis: minPos is greater than or equal to maxPos",
       (String *)0, (Cardinal *)0);
    cw->axis.min_pos = XjAXIS_ORIGIN;
    cw->axis.max_pos = XjAXIS_TERMINUS;
    ret = False;
  }

  return ret;
}


/* Draw
 *
 *      Draws current widget state into supplied drawable
 */
static void
Draw            (AxisWidget cw, Drawable canvas)
{
  XSegment      line;
  XSegment      segs[2*AXIS_MAX_TICS+1];
  XPoint        *box;
  int           n_segs;
  short         x_mult_tics = 0, y_mult_tics = 0;
  short         x_mult_line = 0, y_mult_line = 0;
  int           x_text_units = 0, y_text_units = 0;
  int           x_text_labels, y_text_labels;
  int           x_adjust, y_adjust;
  int           x_min, x_max;
  int           y_min, y_max;
  int           align_labels = 0, align_units = 0;
  float         angle_units = 0.0;
  int           i, j;
  int           z;
  int           length = 0;

  line.y1=0;
  line.y2=0;
  line.x1=0;
  line.x2=0;

  /* clear the drawable */
  XFillRectangle
    (XtDisplay (cw), canvas, cw->axis.erase_gc,
     0, 0, cw->core.width + 1, cw->core.height + 1);

  /* determine direction dependent info */
  switch (cw->axis.direction)
  {
      case XjAXIS_UP:
        x_mult_tics = 0; y_mult_tics = -1;
        x_mult_line = 1; y_mult_line = 0;
        line.y1 = line.y2 = cw->core.height - 1;
        line.x1 = MINPOS(cw); line.x2 = MAXPOS(cw);
        length = line.x2 - line.x1 + 1;
        x_text_units = line.x1 + length / 2;
        y_text_units = 0;
        align_labels = BCENTRE;
        align_units = TCENTRE;
        angle_units = 0.0;
        break;
        
      case XjAXIS_DOWN:
        x_mult_tics = 0; y_mult_tics = 1;
        x_mult_line = 1; y_mult_line = 0;
        line.y1 = line.y2 = 0;
        line.x1 = MINPOS(cw); line.x2 = MAXPOS(cw);
        length = line.x2 - line.x1 + 1;
        x_text_units = line.x1 + length / 2;
        y_text_units = cw->core.height - 1;
        align_labels = TCENTRE;
        align_units = BCENTRE;
        angle_units = 0.0;
        break;
        
      case XjAXIS_LEFT:
        x_mult_tics = -1; y_mult_tics = 0;
        x_mult_line = 0; y_mult_line = -1;
        line.x1 = line.x2 = cw->core.width - 1;
        line.y1 = cw->core.height - 1 - MINPOS(cw);
        line.y2 = cw->core.height - 1 - MAXPOS(cw);
        length = line.y1 - line.y2 + 1;
        x_text_units = 0;
        y_text_units = line.y1 - length / 2;
        align_labels = MRIGHT;
        align_units = TCENTRE;
        angle_units = 90.0;
        break;
        
      case XjAXIS_RIGHT:
        x_mult_tics = 1; y_mult_tics = 0;
        x_mult_line = 0; y_mult_line = -1;
        line.x1 = line.x2 = 0;
        line.y1 = cw->core.height - 1 - MINPOS(cw);
        line.y2 = cw->core.height - 1 - MAXPOS(cw);
        length = line.y1 - line.y2 + 1;
        x_text_units = cw->core.width - 1;
        y_text_units = line.y1 - length / 2;
        align_labels = MLEFT;
        align_units = BCENTRE;
        angle_units = 90.0;
        break;
  }

  /* generate tic segments and draw labels */
  XRotSetBoundingBoxPad (0);
  for (i = 0, n_segs = 0; i < cw->axis.n_tics; i++)
  {
    /* point for ith tic: transform (0, 1) --> (0, length-1) */
    z = (int)(cw->axis.tic_offsets[i] * (length-1));

    /* if this point is not visible, then forget about it */
    if ((z+MINPOS(cw) < 0) || (z+MINPOS(cw) >= LENGTH(cw)))
      continue;

    segs[n_segs].x1 = line.x1 + (x_mult_line * z);
    segs[n_segs].y1 = line.y1 + (y_mult_line * z);

    /* second point for ith tic */
    segs[n_segs].x2 = segs[n_segs].x1 +
      (cw->axis.tic_lengths[i] * x_mult_tics);
    segs[n_segs].y2 = segs[n_segs].y1 +
      (cw->axis.tic_lengths[i] * y_mult_tics);

    /* if label exists, draw it at end of tic */
    if (*cw->axis.tic_labels[i])
    {
      x_text_labels = segs[n_segs].x2 + (AXIS_LABEL_PAD * x_mult_tics);
      y_text_labels = segs[n_segs].y2 + (AXIS_LABEL_PAD * y_mult_tics);

      /* make sure bottom and top labels don't overrun the
       * widget boundary in the direction parallel to the
       * axis line */
      box = XRotTextExtents
        (XtDisplay (cw), cw->axis.font, 0.0,
         x_text_labels, y_text_labels,
         cw->axis.tic_labels[i], align_labels);

      x_adjust = y_adjust = 0;
      x_min = x_max = box[0].x;
      y_min = y_max = box[0].y;
      for (j = 1; j < 4; j++)
      {
        if (box[j].x < x_min)
          x_min = box[j].x;
        else if (box[j].x > x_max)
          x_max = box[j].x;
        if (box[j].y < y_min)
          y_min = box[j].y;
        else if (box[j].y > y_max)
          y_max = box[j].y;
      }
      free (box);

      if (ORIENTATION(cw) == XmVERTICAL)
      {
        /* if it's of both ends, don't even bother drawing */
        if ((y_min < 0) && (y_max >= cw->core.height))
          continue;
        
        if (y_min < 0)
          y_adjust = -y_min;
        else if (y_max >= cw->core.height)
          y_adjust = cw->core.height - y_max - 1;
      }
      else
      {
        /* if it's of both ends, don't even bother drawing */
        if ((y_min < 0) && (y_max >= cw->core.height))
          continue;
        
        if (x_min < 0)
          x_adjust = -x_min;
        else if (x_max >= cw->core.width)
          x_adjust = cw->core.width - x_max - 1;
      }

#if 0
      /* if the label is off center by too much, then draw a line
       * between it and the tic mark, in order to avoid confusion. */
      if ((ABS(x_adjust) > AXIS_LABEL_MAX_SKEW) ||
          (ABS(y_adjust) > AXIS_LABEL_MAX_SKEW))
      {
        n_segs++;
        segs[n_segs].x1 = segs[n_segs-1].x2;
        segs[n_segs].y1 = segs[n_segs-1].y2;
        segs[n_segs].x2 = x_text_labels + x_adjust;
        segs[n_segs].y2 = y_text_labels + y_adjust;
      }
#endif

      XRotDrawAlignedString
        (XtDisplay (cw), cw->axis.font, 0.0, canvas, cw->axis.text_gc,
         x_text_labels + x_adjust, y_text_labels + y_adjust,
         cw->axis.tic_labels[i], align_labels);
    }

    n_segs++;
  }

  /* draw tics and axis line.  Might as well draw the line at the same
   * time --no sense in making two calls */
  segs[n_segs++] = line;
  XDrawSegments
    (XtDisplay (cw), canvas, cw->axis.draw_gc, segs, n_segs);

  /* units string and magnitude */
  buf[0] = buf[BUF_SIZE] = 0;
  if (cw->axis.unit_string)
    if (*cw->axis.unit_string)
      strncpy (buf, cw->axis.unit_string, BUF_SIZE);
  if (*cw->axis.tic_scale)
    strcat (buf, cw->axis.tic_scale);

  XRotDrawAlignedString
    (XtDisplay (cw), cw->axis.font, angle_units, canvas,
     cw->axis.text_gc, x_text_units, y_text_units,
     buf, align_units);
}


/* ComputeTics
 *
 *      Hands off computation of tic info to the appropriate
 *      function, depending on the prevailing axis transform.
 */
static int
ComputeTics     (AxisWidget cw)
{
  int   ret;
  
  if (cw->axis.value_type == XjAXIS_REAL)
  {
    if (cw->axis.transform == XjAXIS_LINEAR)
      ret = ComputeNormalTics (cw);
    else
    {
      /* need to decide how to deal with the logarithmic scale */
      compute_log_info (cw);
      if (cw->axis.log_n_powers > 1)
        ret = ComputeLogTics (cw);
      else ret = ComputeNormalTics (cw);
    }
  }
  else ret = ComputeTimeTics (cw);

  return ret;
}


/* ComputeTimeTics
 *
 *      Calculates the normalized tic offsets and scale value (seconds,
 *      minutes, hours, days, weeks, years) for the axis.  This value
 *      is chosen such that the tic mark interval widths are easiest
 *      to understand (e.g. 1 minute is easier than 60 seconds).  If
 *      the axis value type is RELTIME_XXX, then the max point of the
 *      axis line will always be labeled, and offsets therefrom will be
 *      labeled appropriately
 */
static int
ComputeTimeTics         (AxisWidget cw)
{
  double *  m = TimeUnitValues;
  double    m_minor, m_major;               /* best divisor */
  double    q = 1;
  double    r;
  double    a, b;
  double    delta;
  time_t    a_secs, b_secs, secs;
  struct tm cal = {0, 0};
  int       best;
  int       i;
  int *     tm_field = 0;
  int       interval;
  int *     multiplier = 0;
  int       n_units = 0;

  delta = cw->axis.max_val - cw->axis.min_val;
  for (best = Seconds; best < Years; best++)
    if ((delta / m[best]) <= MaxTimeIntervals[best])
      break;

  /* end points are in terms of best units */
  a = cw->axis.min_val / m[best];
  b = cw->axis.max_val / m[best];
  n_units = (int)(delta / m[best]);


  switch (best)
  {
  case Seconds:
  case Minutes:
    if (n_units <= 5)
    {
      m_major = 1;
      m_minor = 1;
    }
    else if (n_units <= 10)
    {
      m_major = 2;
      m_minor = 1;
    }
    else if (n_units <= 20)
    {
      m_major = 5;
      m_minor = 1;
    }
    else if (n_units <= 40)
    {
      m_major = 10;
      m_minor = 2;
    }
    else if (n_units < 90)
    {
      m_major = 20;
      m_minor = 5;
    }
    else
    {
      m_major = 30;
      m_minor = 5;
    }
    break;

  case Hours:
    if (n_units <= 5)
    {
      m_major = 1;
      m_minor = 1;
    }
    else if (n_units < 12)
    {
      m_major = 2;
      m_minor = 1;
    }
    else if (n_units < 20)
    {
      m_major = 4;
      m_minor = 1;
    }
    else if (n_units < 30)
    {
      m_major = 6;
      m_minor = 1;
    }
    else if (n_units < 60)
    {
      m_major = 12;
      m_minor = 2;
    }
    else
    {
      m_major = 24;
      m_minor = 4;
    }
    break;
  
  case Days:
    if (n_units <= 5)
    {
      m_major = 1;
      m_minor = 1;
    }
    else if (n_units < 12)
    {
      m_major = 2;
      m_minor = 1;
    }
    else if (n_units < 20)
    {
      m_major = 4;
      m_minor = 1;
    }
    else
    {
      m_major = 7;
      m_minor = 1;
    }
    break;
  
  case Weeks:
    if (n_units <= 5)
    {
      m_major = 1;
      m_minor = 1;
    }
    else if (n_units < 12)
    {
      m_major = 2;
      m_minor = 1;
    }
    else
    {
      m_major = 4;
      m_minor = 1;
    }
    break;
  
  case Months:
    if (n_units <= 5)
    {
      m_major = 1;
      m_minor = 1;
    }
    else if (n_units < 10)
    {
      m_major = 2;
      m_minor = 1;
    }
    else if (n_units < 15)
    {
      m_major = 3;
      m_minor = 1;
    }
    else if (n_units < 20)
    {
      m_major = 4;
      m_minor = 1;
    }
    else if (n_units < 30)
    {
      m_major = 6;
      m_minor = 1;
    }
    else
    {
      m_major = 12;
      m_minor = 2;
    }
    break;
  
  case Years:
  default:
    if (n_units <= 5)
    {
      m_major = 1;
      m_minor = 1;
    }
    else if (n_units < 12)
    {
      m_major = 2;
      m_minor = 1;
    }
    else if (n_units < 24)
    {
      m_major = 4;
      m_minor = 1;
    }
    else if (n_units < 30)
    {
      m_major = 5;
      m_minor = 1;
    }
    else if (n_units < 60)
    {
      m_major = 10;
      m_minor = 2;
    }
    else if (n_units < 120)
    {
      m_major = 20;
      m_minor = 5;
    }
    else
    {
      m_major = 50;
      m_minor = 50;
    }
    
    break;
  }

  /* ABSTIME
   *
   *    The tic marks must fall on intuitive points, and the
   *    values must readily present the corresponding dates.
   */
  if (cw->axis.value_type == XjAXIS_ABSTIME)
  {
    /* finding the first tic location is somewhat different for time than
     * for real numbers, since time units don't necessarily occupy
     * consistent intervals (for example, some years have more seconds than
     * others).  So, in order to find the first boundary, truncate the
     * current resolution units in the min value, and then add one.
     */
#if 0
  /* KE: localtime_r is not Posix and is not available on WIN32 */
    a_secs = (time_t)cw->axis.min_val;
    localtime_r (&a_secs, &cal);
#else
    struct tm *ptm;
    
    a_secs = (time_t)cw->axis.min_val;
    ptm = localtime (&a_secs);
    if(ptm) cal = *ptm;
#endif    

    interval = (int)m_minor;
    switch (best)
    {
    case Seconds:
      tm_field = &cal.tm_sec;
      break;
      
    case Minutes:
      cal.tm_sec = 0;
      tm_field = &cal.tm_min;
      break;
      
    case Hours:
      cal.tm_sec = 0;
      cal.tm_min = 0;
      tm_field = &cal.tm_hour;
      break;

    case Days:
      cal.tm_sec = 0;
      cal.tm_min = 0;
      cal.tm_hour = 0;
      cal.tm_isdst = -1;
      tm_field = &cal.tm_mday;
      break;
      
    case Weeks:
      cal.tm_sec = 0;
      cal.tm_min = 0;
      cal.tm_hour = 0;
      cal.tm_isdst = -1;
      cal.tm_mday -= cal.tm_wday;
      tm_field = &cal.tm_mday;
      interval *= 7;
      break;
      
    case Months:
      cal.tm_sec = 0;
      cal.tm_min = 0;
      cal.tm_hour = 0;
      cal.tm_mday = 1;
      cal.tm_isdst = -1;
      tm_field = &cal.tm_mon;
      break;
      
    case Years:
      cal.tm_sec = 0;
      cal.tm_min = 0;
      cal.tm_hour = 0;
      cal.tm_mday = 1;
      cal.tm_mon = 0;
      cal.tm_isdst = -1;
      tm_field = &cal.tm_year;
      break;
    }

    /* move forward to the most intuitive starting tic
     */
    if (mktime (&cal) < a_secs)
    {
      if (best == Weeks) (*tm_field) += 7;
      else
      {
        if (best > Hours) cal.tm_isdst = -1;
        (*tm_field)++;
      }
      mktime (&cal);
    }
    
    if (best == Weeks)
    {
      /* nothing */
    }
    else if (best == Days)
    {
      /* nothing */
    }
    else
    {
      i = (*tm_field) % interval;
      if (i)
      {
        if (best > Hours) cal.tm_isdst = -1;
        (*tm_field) += interval - i;
      }
    }

    /* generate the tics.
     */
    i = 0;
    b_secs = (time_t)cw->axis.max_val;
    secs = mktime (&cal);
    while ((secs <= b_secs) && (i < AXIS_MAX_TICS))
    {
      int is_minor;

      if (best == Days)
      {
        if (m_major == 1) is_minor = 0;
        else if (m_major == 2) is_minor = !(cal.tm_wday % 2);
        else if (m_major == 4)
          is_minor = ((cal.tm_wday != 1) && (cal.tm_wday != 5));
        else is_minor = cal.tm_wday;
      }
      else if (best == Weeks)
      {
        is_minor = i % (int)m_major;
      }
      else is_minor = (*tm_field) % (int)m_major;
      
      cw->axis.tic_offsets[i] = secs;
      cw->axis.tic_lengths[i] = AXIS_MAJOR_TIC_LENGTH;

      if (is_minor)
      {
        *cw->axis.tic_labels[i] = 0;
        cw->axis.tic_lengths[i] /= 2;
      }
      
      else
      {
        strftime (buf, BUF_SIZE, TimeFormats[best], localtime (&secs));
        strncpy (cw->axis.tic_labels[i], buf, AXIS_MAX_LABEL);
        cw->axis.tic_labels[i][AXIS_MAX_LABEL] = 0;
      }

      *tm_field += interval;

      /* some problems with mktime() and dst transitions */
      if (best == Months) cal.tm_isdst = -1;
      
      secs = mktime (&cal);
      i++;
    }

    if ( (best != Weeks) && (interval != 1) )
      multiplier = &interval;
  }

  /* RELTIME
   *
   *    The tic marks must fall on intuitive intervals, though
   *    not necessarily on inuitive points.
   */
  else
  {
    find_good_linear_tics (cw, False, a, b, &q, &m_minor, &m_major);
    interval = (int)(m_major * q);
    if ((cw->axis.value_type != XjAXIS_RELTIME_NUMBERS) && (interval != 1))
      multiplier = &interval;
    
    /* start from greatest time and move backwards */
    r = b;
    i = 0;
    while ((r-a) > -DBL_EPSILON)
    {
      cw->axis.tic_offsets[i] = r*m[best];
      cw->axis.tic_lengths[i] = AXIS_MAJOR_TIC_LENGTH;

      if (fmod (i*m_minor, m_major) > DBL_EPSILON)
      {
        *cw->axis.tic_labels[i] = 0;
        cw->axis.tic_lengths[i] /= 2;
      }
      else
      {
        /* if this is the first tic mark (the one corresponding
         * to the max value), then print out the entire date.
         * If the axis value type is RELTIME_DATES, then we need
         * to draw dates for all major tics, not just the max
         */
        if (cw->axis.value_type == XjAXIS_RELTIME_DATES ||
            ((i == 0) && (cw->axis.value_type == XjAXIS_RELTIME_NUMBERS)))
        {
          /* convert units into seconds */
          secs = (time_t)(r*m[best]);
          strftime (buf, BUF_SIZE, "%H:%M:%S\n%b %d, %Y", localtime (&secs));
        }
        else sprintf (buf, "%d", (int)(r-b));
        
        strncpy (cw->axis.tic_labels[i], buf, AXIS_MAX_LABEL);
        cw->axis.tic_labels[i][AXIS_MAX_LABEL] = 0;
      }

      r -= m_minor * q;
      i++;
    }
  }

  /* only show multiplier if not 1, and not displaying numbers */
  if (multiplier)
    sprintf
      (cw->axis.tic_scale, " (%s x %d)",
       TimeUnitNameStrings[best], *multiplier);
  else
    sprintf
      (cw->axis.tic_scale, " (%s)", TimeUnitNameStrings[best]);

  cw->axis.n_tics = i;
  transform_values_normalized
    (cw->axis.transform, cw->axis.value_type,
     cw->axis.min_pos, cw->axis.max_pos, cw->axis.min_val, cw->axis.max_val,
     cw->axis.log_epsilon, cw->axis.log_epsilon_offset, cw->axis.log_delta,
     cw->axis.tic_offsets, cw->axis.tic_offsets, cw->axis.n_tics);

  return 1;
}
  
 

/* ComputeNormalTics
 *
 *      Calculates the normalized tic offsets (so that when the widget
 *      is resized, the offsets can be mapped to the new pixel length),
 *      labels, and tic magnitude.  Offsets are determined by using
 *      the prevailing transform.
 */
static int
ComputeNormalTics       (AxisWidget cw)
{
  double        m_minor, m_major;               /* best divisor */
  double        p, q;                           /* exponent, magnitude */
  double        a, b;
  double        r, s;
  double        rr;
  double        e;
  int           i;

  find_good_linear_tics
    (cw, True, cw->axis.min_val, cw->axis.max_val, &q, &m_minor, &m_major);

  e = pow (10.0, cw->axis.log_epsilon);

  /* endpoints */
  a = cw->axis.min_val / q;
  b = cw->axis.max_val / q;

  /* r <-- first tic location */
  r = ceil (a / m_minor) * m_minor;

  /* if the tic values contain too many digits, then we'll need
   * to present scaled values for labels, and set the axis scale
   * to the exponential difference between the presented and
   * actual values. */
  if (ABS(r) <= DBL_EPSILON)            /* don't want log of zero! */
    s = log10 (ABS((r+m_minor)*q));
  else s = log10 (ABS(r*q));
  s = floor (s);

  /* p <-- number of powers by which values will be scaled */
  if (ABS(s) >= AXIS_MAX_DIGITS)
    p = s;
  else p = 0.0;

  /* we'll need this later, when displaying the data */
  if (p != 0)
    sprintf (cw->axis.tic_scale, " (x 1e%-d)", (int)p);
  else cw->axis.tic_scale[0] = 0;
  
  /* p <-- scale units, rather than powers */
  p = pow (10.0, p);

  /* generate tic info */
  i = 0;
  rr = r;
  while ((r - b) < DBL_EPSILON)
  {
    /* if major tic, generate label string, otherwise just clear its label
     * and halve its tic length */
    cw->axis.tic_offsets[i] = r*q;
    *cw->axis.tic_labels[i] = 0;

    
    /* if this value is within the epsilon interval for a log plot, then just
     * ignore it. */
    if ( (cw->axis.transform != XjAXIS_LINEAR) &&
              (ABS((r*q) - e) <= DBL_EPSILON) )
    {
      /* do nothing */
    }
    
    /* there are problems with the mod test, owing to cumulative imprecision
     * in floating point numbers.  Rather than test whether the value is
     * "effectively zero", I check that the floating point remainder is
     * less than half the minor tic interval.  This should provide sufficient
     * tolerance to accomodate imprecise numbers without incorrectly
     * identifying them as minor interval boundaries.
     */
    /*
      if (ABS (fmod (r, m_major)) > DBL_EPSILON)
    */
    else if (ABS (fmod (r, m_major)) < (m_minor / 2))
    {
      cw->axis.tic_lengths[i] = AXIS_MAJOR_TIC_LENGTH;
      sprintf (buf, "%-.*g", AXIS_MAX_DIGITS+10, r*(q/p));
      strncpy (cw->axis.tic_labels[i], buf, AXIS_MAX_LABEL);
      cw->axis.tic_labels[i][AXIS_MAX_LABEL] = 0;
    }

    else cw->axis.tic_lengths[i] = AXIS_MINOR_TIC_LENGTH;
    
    i++;
    r = rr + (i * m_minor);
  }

  /* mark epsilon, if necessary */
  if ((cw->axis.transform != XjAXIS_LINEAR) &&
      (cw->axis.log_epsilon_offset >= 0) &&
      (cw->axis.log_epsilon_offset <= 1))
  {
    cw->axis.tic_offsets[i] = cw->axis.log_epsilon_offset;
    cw->axis.tic_lengths[i] = AXIS_MAJOR_TIC_LENGTH+2;
    sprintf (buf, "%c1e%-d", PLUS_MINUS, (int)cw->axis.log_epsilon);
    strncpy (cw->axis.tic_labels[i], buf, AXIS_MAX_LABEL);
    cw->axis.tic_labels[i][AXIS_MAX_LABEL] = 0;
    i++;
  }

  cw->axis.n_tics = (short)i;
  transform_values_normalized
    (cw->axis.transform, cw->axis.value_type,
     cw->axis.min_pos, cw->axis.max_pos, cw->axis.min_val, cw->axis.max_val,
     cw->axis.log_epsilon, cw->axis.log_epsilon_offset, cw->axis.log_delta,
     cw->axis.tic_offsets, cw->axis.tic_offsets, cw->axis.n_tics);

  return 1;
}


/* ComputeLogTics
 *
 *      Generates the tic mark offsets and labels for the logarithmic
 *      axis.  Also sets log_n_powers and log_epsilon_index.  The former
 *      is the number of powers (of the logarithmic base) contained
 *      within the axis interval; the latter is the index of the power
 *      corresponding to the interval [-log_epsilon, log_epsilon].
 *
 *      Tic placement desiderata:
 *
 *              o the epsilon tic mark, if inside the range, must
 *                be differentiated from the others.
 */
static int
ComputeLogTics  (AxisWidget cw)
{
  double        q;
  double        m_minor, m_major;
  int           n_minor, n_major;
  int           power_a, power_b;
  int           sign_a, sign_b;
  double        a, b, r;
  int           i;

  /* generate good tic intervals */
  find_good_log_tics (cw, &q, &n_minor, &n_major, &m_minor, &m_major);

  m_minor *= q;
  m_major *= q;

  i = 0;        /* tic index */

  /* ====== do positive tics ====== */
  if (cw->axis.log_epsilon_index < cw->axis.log_n_powers)
  {
    /* end points */
    if (cw->axis.log_epsilon_index > 0)
      get_log_index_value (cw, cw->axis.log_epsilon_index, &power_a, &sign_a);
    else get_log_index_value (cw, 0, &power_a, &sign_a);

    get_log_index_value (cw, cw->axis.log_n_powers-1, &power_b, &sign_b);

    a = power_a;
    b = power_b;

    /* r <-- first tic location */
    r = ceil (a / m_minor) * m_minor;

    while ((r - b) < DBL_EPSILON)
    {
      if (r != cw->axis.log_epsilon)
      {
        /* offset of this tic from zero, within (0,1] interval */
        cw->axis.tic_offsets[i] =
          cw->axis.log_epsilon_offset +
          ((r - cw->axis.log_epsilon) / cw->axis.log_delta);
        
        cw->axis.tic_lengths[i] = AXIS_MAJOR_TIC_LENGTH;
        
        /* if major tic, generate label string, otherwise just
         * clear its label and halve its tic length */
        if (ABS (fmod (r, m_major)) <= DBL_EPSILON)
        {
          sprintf (buf, "1e%-d", (int)r);
          strncpy (cw->axis.tic_labels[i], buf, AXIS_MAX_LABEL);
          cw->axis.tic_labels[i][AXIS_MAX_LABEL] = 0;
        }
        else
        {
          *cw->axis.tic_labels[i] = 0;
          cw->axis.tic_lengths[i] /= 2;
        }

        i++;
      }

      r += m_minor;
    }
  }

  /* ====== do negative tics ====== */
  if (cw->axis.log_epsilon_index >= 0)
  {
    /* end points */
    if (cw->axis.log_epsilon_index < cw->axis.log_n_powers - 1)
      get_log_index_value (cw, cw->axis.log_epsilon_index, &power_a, &sign_a);
    else
      get_log_index_value (cw, cw->axis.log_n_powers - 1, &power_a, &sign_a);

    get_log_index_value (cw, 0, &power_b, &sign_b);

    a = power_a;
    b = power_b;

    /* r <-- first tic location */
    r = ceil (a / m_minor) * m_minor;

    while ((r - b) < DBL_EPSILON)
    {
      if (r != cw->axis.log_epsilon)
      {
        /* offset of this tic from zero, within (0,1] interval */
        cw->axis.tic_offsets[i] =
          cw->axis.log_epsilon_offset -
          ((r - cw->axis.log_epsilon) / cw->axis.log_delta);
        
        cw->axis.tic_lengths[i] = AXIS_MAJOR_TIC_LENGTH;

        /* if major tic, generate label string, otherwise just
         * clear its label and halve its tic length */
        if (ABS (fmod (r, m_major)) <= DBL_EPSILON)
        {
          sprintf (buf, "-1e%-d", (int)-r);
          strncpy (cw->axis.tic_labels[i], buf, AXIS_MAX_LABEL);
          cw->axis.tic_labels[i][AXIS_MAX_LABEL] = 0;
        }
        else
        {
          *cw->axis.tic_labels[i] = 0;
          cw->axis.tic_lengths[i] /= 2;
        }
        
        i++;
      }
      
      r += m_minor;
    }
  }

  /* ====== now make the epsilon tic mark if necessary ====== */
  if ((cw->axis.log_epsilon_index >= 0) &&
      (cw->axis.log_epsilon_index < cw->axis.log_n_powers))
  {
    cw->axis.tic_offsets[i] = cw->axis.log_epsilon_offset;
    cw->axis.tic_lengths[i] = AXIS_MAJOR_TIC_LENGTH+2;
    sprintf (buf, "%c1e%-d", PLUS_MINUS, (int)cw->axis.log_epsilon);
    strncpy (cw->axis.tic_labels[i], buf, AXIS_MAX_LABEL);
    cw->axis.tic_labels[i][AXIS_MAX_LABEL] = 0;
    i++;
  }

  cw->axis.tic_scale[0] = 0;
  cw->axis.n_tics = i;

  return 1;
}





/* ========================================================================
 *
 *        >>>>>>>>>>  H E L P E R    F U N C T I O N S  <<<<<<<<<<
 *
 * ========================================================================
 */

/* find_good_linear_tics
 *
 *      Calculates best major and minor tic divisors, magnitude, and count.
 *
 *      Tic placement desiderata:
 *
 *              o there must be at least AXIS_MIN_TICS major tics and
 *                no more than AXIS_MAX_TICS
 *              o maximize tics
 */
static void
find_good_linear_tics   (AxisWidget     cw,
                         Boolean        NiceIntervals,
                         double         min_val,
                         double         max_val,
                         double         *tic_magnitude,
                         double         *minor_divisor,
                         double         *major_divisor)
{
  double *  m = TicDivisorValues;
  int       n[NumTicDivisors];              /* num tics for ith divisor */
  double    m_minor, m_major;               /* best divisor */
  int       n_major;
  double    p, q;                           /* exponent, magnitude */
  double    a, b;
  int       i;

  /* p <-- exponent of magnitude of delta, rounded to nearest integer */
  p = ROUND (log10 (max_val - min_val));
  q = pow (10.0, p-1);

  a = min_val / q;
  b = max_val / q;
    
  /* get num tics for possible interval divisors */
  for (i = One; i <= Ten; i++)
    if (NiceIntervals)
      n[i] = (int)
        (((floor (b/m[i]) * m[i]) - (ceil (a/m[i]) * m[i])) / m[i]) + 1;
    else
      n[i] = (int)((b-a)/m[i]) + 1;

  /* maximize tics, within constraints */
  n_major = 0;
  m_major = 1;
  for (i = One; i <= Ten; i++)
    if ((n[i] > n_major) && (n[i] <= AXIS_MAX_MAJOR_TICS))
    {
      n_major = n[i];
      m_major = m[i];
    }

  /* results */
  m_minor = ((m_major == m[Two])? m_major / 4 : m_major / 5);
  *tic_magnitude = q;
  *minor_divisor = m_minor;
  *major_divisor = m_major;
}


/* find_good_log_tics
 *
 */
static void
find_good_log_tics      (AxisWidget     cw,
                         double         *tic_magnitude,
                         int            *num_minor,
                         int            *num_major,
                         double         *minor_divisor,
                         double         *major_divisor)
{
  double        *m = TicDivisorValues;
  int           n[NumTicDivisors];              /* num tics for ith divisor */
  double        m_minor = 0, m_major = 0;       /* best divisor */
  int           n_minor, n_major;               /* num tics for best div. */
  double        p, q;                           /* exponent, magnitude */
  double        a, b;
  int           x, s;
  int           i;

  /* p <-- exponent of magnitude of delta, rounded to nearest integer */
  p = ROUND (log10 (cw->axis.log_n_powers));
  q = pow (10.0, p-1);

  /* get num tics for possible interval divisors */
  for (i = One; i <= Ten; i++)
  {
    /* ====== positive part ====== */
    if (cw->axis.log_epsilon_index < cw->axis.log_n_powers)
    {
      if (cw->axis.log_epsilon_index > 0)
        get_log_index_value (cw, cw->axis.log_epsilon_index, &x, &s);
      else get_log_index_value (cw, 0, &x, &s);
      a = x;
      get_log_index_value (cw, cw->axis.log_n_powers, &x, &s);
      b = x;
      
      a /= q;
      b /= q;
      
      n[i] = (int)
        (((floor (b/m[i]) * m[i]) - (ceil (a/m[i]) * m[i])) / m[i]) + 1;
    }
    else n[i] = 0;
      
    /* ====== negative part ====== */
    if (cw->axis.log_epsilon_index >= 0)
    {
      if (cw->axis.log_epsilon_index < cw->axis.log_n_powers - 1)
        get_log_index_value (cw, cw->axis.log_epsilon_index, &x, &s);
      else get_log_index_value (cw, cw->axis.log_n_powers - 1, &x, &s);
      a = x;
      get_log_index_value (cw, 0, &x, &s);
      b = x;
      
      a /= q;
      b /= q;
      
      n[i] += (int)
        (((floor (b/m[i]) * m[i]) - (ceil (a/m[i]) * m[i])) / m[i]) + 1;
    }
  }

  
  /* maximize minor tics, within constraints */
  n_minor = AXIS_MIN_TICS-1;
  for (i = One; i <= Ten; i++)
    if ((n[i] > n_minor) && (n[i] <= AXIS_MAX_TICS))
    {
      n_minor = n[i];
      m_minor = m[i];
    }

  /* now find the appropriate major tics */
  n_major = AXIS_MAX_TICS+1;
  for (i = One; i <= Ten; i++)
  {
    /* major tic interval must be greater than, and evenly divisible
     * by, minor tic interval */
    if (m[i] > m_minor)
      if ((n[i] < n_major) && (n[i] >= AXIS_MIN_TICS) &&
          (ABS (fmod (m[i], m_minor)) <= DBL_EPSILON))
      {
        n_major = n[i];
        m_major = m[i];
      }
  }

  /* peculiar situations
   */
  /* no good minor tics --use major tics only */
  if ((n_minor < AXIS_MIN_TICS) && (n_major <= AXIS_MAX_TICS))
  {
    n_minor = n_major;
    m_minor = m_major;
  }
  /* no good major tics --use minor as major */
  else if ((n_minor >= AXIS_MIN_TICS) && (n_major > AXIS_MAX_TICS))
  {
    n_major = n_minor;
    m_major = m_minor;
  }
  /* major and minor tics add up to more than max tics? */
  else if ((n_major + n_minor) > AXIS_MAX_TICS)
  {
    n_minor = n_major;
    m_minor = m_major;
  }
  /* weird situation */
  else if ((n_minor < AXIS_MIN_TICS) && (n_major > AXIS_MAX_TICS))
    XtAppErrorMsg
      (XtWidgetToApplicationContext((Widget)cw),
       "find_good_log_tics", "badProgrammer", "Axis",
       "Axis: an internal algorithmic deficiency has emerged from "
       "its murky depths.",
       (String *)0, (Cardinal *)0);

  /* results */
  *tic_magnitude = q;
  *num_minor = n_minor;
  *num_major = n_major;
  *minor_divisor = m_minor;
  *major_divisor = m_major;
}


/* compute_log_info
 *
 *      Counts the number of powers between min_val and max_val
 *      and determines where the index of log_epsilon resides.
 *      This index is relative to 0, which will always be the
 *      index of the power closest to min_val, within the range.
 */
static void
compute_log_info        (AxisWidget cw)
{
  int stat;
  
  stat = get_log_info
    (cw->axis.min_val, cw->axis.max_val, cw->axis.log_epsilon,
     &cw->axis.log_epsilon_index, &cw->axis.log_n_powers, &cw->axis.log_delta,
     &cw->axis.log_epsilon_offset);

  if (!stat) 
    XtAppErrorMsg
      (XtWidgetToApplicationContext((Widget)cw),
       "compute_log_info", "badProgrammer", "Axis",
       "Axis: unable to calculate log info",
       (String *)0, (Cardinal *)0);
}


static int
get_log_info            (double min_val,
                         double max_val,
                         double log_epsilon,
                         short  *log_epsilon_index,
                         short  *log_n_powers,
                         double *log_delta,
                         double *log_epsilon_offset)
{
  double        a, b, e;
  double        log_a, log_b;
  int           n_a, n_b;

  /* the magnitude ceiling for all undefined values */
  e = pow (10, log_epsilon);
  a = min_val;
  b = max_val;
  
  /* treat everything inside [-e, e] as NaN */
  if ((ABS(a) - e) <= DBL_EPSILON) a = e;
  if ((ABS(b) - e) <= DBL_EPSILON) b = -e;

  if (a >= b)
  {
    fprintf
      (stderr, "get_log_info: min and max values are essentially equa1\n");
    return 0;
  }

  /* compute logarithms once */
  log_a = log10(ABS(a));
  log_b = log10(ABS(b));

  /* we are now guaranteed, mathematically, that |a| >= e,
   * and |b| >= e. */

  /* case 1: e <= a < b
   */
  if ((a - e) >= -DBL_EPSILON)
  {
    n_a = (int)ceil (log_a);
    n_b = (int)floor (log_b);
    *log_epsilon_index = (short)(log_epsilon - n_a);
    *log_n_powers = n_b - n_a + 1;
    *log_delta = (log_b - log_a);
    *log_epsilon_offset = (log_epsilon - log_a) / *log_delta;
  }
  
  /* case 2: a < b <= -e
   */
  else if ((b + e) <= DBL_EPSILON)
  {
    n_a = (int)floor (log_a);
    n_b = (int)ceil (log_b);
    *log_epsilon_index = (short)(n_b - log_epsilon);
    *log_n_powers = n_b - n_a + 1;
    *log_epsilon_index += *log_n_powers - 1;
    *log_delta = (log_a - log_b);
    *log_epsilon_offset = (log_a - log_epsilon) / *log_delta;
  }

  /* case 3: a < -e,  e < b
   */
  else if (((a + e) < -DBL_EPSILON) && ((b - e) > DBL_EPSILON))
  {
    n_a = (int)floor (log_a);
    n_b = (int)floor (log_b);
    *log_epsilon_index = (short)(n_a - log_epsilon);
    *log_n_powers = (short)(n_b + n_a - (log_epsilon * 2) + 1);
    *log_delta = (log_a - log_epsilon) + (log_b - log_epsilon);
    *log_epsilon_offset = (log_a - log_epsilon) / *log_delta;
  }

  /* weird situation */
  else
  {
    fprintf
      (stderr,
       "get_log_info: peculiar floating point rounding condition.\n");
    return 0;
  }

  return 1;
}


/* get_log_index_value
 *
 *      Generates the power and sign of the value corresponding to
 *      the passed-in index, using log_epsilon and log_epsilon_index.
 *      The value of sign will be -1, 0, 1, depending on whether the
 *      resulting value is negative, NaN, or positive.  This is also
 *      the function's return value.
 */
static int
get_log_index_value     (AxisWidget cw, int index, int *power, int *sign)
{
  if (index > cw->axis.log_epsilon_index)
    *sign = 1;
  else if (index < cw->axis.log_epsilon_index)
    *sign = -1;
  else *sign = 0;

  *power = (int)
    (cw->axis.log_epsilon + (*sign * (index - cw->axis.log_epsilon_index)));

  return *sign;
}


/* ========================================================================
 *
 *                   >>>>>>>>>>  A C T I O N S  <<<<<<<<<<
 *
 * ========================================================================
 */

/* Arm
 */
static void
Arm             (Widget w, XEvent *event, String *params, Cardinal *num_params)
{
#if 0
  fprintf (stdout, "Arm (%s)\n", params[0]);
  fflush (stdout);
#endif
}


/* ========================================================================
 *
 *                >>>>>>>>>>  C O N V E R T E R S  <<<<<<<<<<
 *
 * ========================================================================
 */

static Boolean
CvtStringToAxisDirection        (Display        *display,
                               XrmValue *args,
                               Cardinal *n_args,
                               XrmValue *from,
                               XrmValue *to,
                               XtPointer        *data)
{
  static AxisDirection  result;
  Boolean               ret = True;
  XrmQuark              quark;
  
  if (*n_args != 0)
    XtWarningMsg
      ("wrongParameters", "CvtStringToAxisDirection",
       "XtToolkitError", "String to AxisDirection needs no extra arguments",
       (String *)0, (Cardinal *)0);

  quark = XrmStringToQuark ((char *)from->addr);
   
  if (quark == QAxisUp)
    result = XjAXIS_UP;
  else if (quark == QAxisDown)
    result = XjAXIS_DOWN;
  else if (quark == QAxisLeft)
    result = XjAXIS_LEFT;
  else if (quark == QAxisRight)
    result = XjAXIS_RIGHT;
  else {
    XtDisplayStringConversionWarning
      (display, (char *)from->addr, XjRAxisDirection);
    ret = False;
  }

  if (to->addr)
    if (to->size < sizeof(AxisDirection))       /* enough space ? */
      ret = False;
    else *(AxisDirection *)to->addr = result;
  else to->addr = (XtPointer)&result;
  to->size = sizeof (AxisDirection);

  return ret;
}


static Boolean
CvtStringToAxisValueType        (Display        *display,
                               XrmValue *args,
                               Cardinal *n_args,
                               XrmValue *from,
                               XrmValue *to,
                               XtPointer        *data)
{
  static AxisValueType  result;
  Boolean               ret = True;
  XrmQuark              quark;
  
  if (*n_args != 0)
    XtWarningMsg
      ("wrongParameters", "CvtStringToAxisValueType",
       "XtToolkitError", "String to AxisValueType needs no extra arguments",
       (String *)0, (Cardinal *)0);

  quark = XrmStringToQuark ((char *)from->addr);
   
  if (quark == QAxisReal)
    result = XjAXIS_REAL;
  else if (quark == QAxisAbsTime)
    result = XjAXIS_ABSTIME;
  else if (quark == QAxisRelTimeDates)
    result = XjAXIS_RELTIME_DATES;
  else if (quark == QAxisRelTimeNumbers)
    result = XjAXIS_RELTIME_NUMBERS;
  else {
    XtDisplayStringConversionWarning
      (display, (char *)from->addr, XjRAxisValueType);
    ret = False;
  }

  if (to->addr)
    if (to->size < sizeof(AxisValueType))       /* enough space ? */
      ret = False;
    else *(AxisValueType *)to->addr = result;
  else to->addr = (XtPointer)&result;
  to->size = sizeof (AxisValueType);
   
  return ret;
}


static Boolean
CvtStringToAxisTransform        (Display        *display,
                               XrmValue *args,
                               Cardinal *n_args,
                               XrmValue *from,
                               XrmValue *to,
                               XtPointer        *data)
{
  static AxisTransform  result;
  Boolean               ret = True;
  XrmQuark              quark;
  
  if (*n_args != 0)
    XtWarningMsg
      ("wrongParameters", "CvtStringToAxisTransform",
       "XtToolkitError", "String to AxisTransform needs no extra arguments",
       (String *)0, (Cardinal *)0);

  quark = XrmStringToQuark ((char *)from->addr);
   
  if (quark == QAxisLinear)
    result = XjAXIS_LINEAR;
  else if (quark == QAxisLog10)
    result = XjAXIS_LOG10;
  else {
    XtDisplayStringConversionWarning
      (display, (char *)from->addr, XjRAxisTransform);
    ret = False;
  }

  if (to->addr)
    if (to->size < sizeof(AxisTransform))       /* enough space ? */
      ret = False;
    else *(AxisTransform *)to->addr = result;
  else to->addr = (XtPointer)&result;
  to->size = sizeof (AxisTransform);

  return ret;
}


static Boolean
CvtStringToAxisEndpointPosition (Display        *display,
                                     XrmValue   *args,
                                     Cardinal   *n_args,
                                     XrmValue   *from,
                                     XrmValue   *to,
                                     XtPointer  *data)
{
  static AxisEndpointPosition   result;
  Boolean                       ret = True;
  XrmQuark                      quark;

  if (*n_args != 0)
    XtWarningMsg
      ("wrongParameters", "CvtStringToAxisEndpointPosition",
       "XtToolkitError", "String to AxisEndpointPosition needs "
       "no extra arguments",
       (String *)0, (Cardinal *)0);

  quark = XrmStringToQuark ((char *)from->addr);

  if (quark == QAxisOrigin)
    result = XjAXIS_ORIGIN;
  else if (quark == QAxisTerminus)
    result = XjAXIS_TERMINUS;
  else ret = (sscanf(from->addr, "%d", &result) == 1);
  
  if (to->addr)
    if (to->size < sizeof(AxisEndpointPosition))        /* enough space ? */
      ret = False;
    else *(AxisEndpointPosition *)to->addr = result;
  else to->addr = (XtPointer)&result;
  to->size = sizeof (AxisEndpointPosition);

  return ret;
}


static Boolean
CvtStringToDouble       (Display        *display,
                   XrmValue     *args,
                   Cardinal     *n_args,
                   XrmValue     *from,
                   XrmValue     *to,
                   XtPointer    *data)
{
  static double result;
  Boolean       ret = True;

  if (*n_args != 0)
    XtWarningMsg
      ("wrongParameters", "CvtStringToDouble",
       "XtToolkitError", "String to Double needs "
       "no extra arguments",
       (String *)0, (Cardinal *)0);

  ret = (sscanf(from->addr, "%lf", &result) == 1);
  
  if (to->addr)
    if (to->size < sizeof(double))      /* enough space ? */
      ret = False;
    else *(double *)to->addr = result;
  else to->addr = (XtPointer)&result;
  to->size = sizeof (double);

  return ret;
}


/* ========================================================================
 *
 *  >>>>>>>>>>  P U B L I C   U T I L I T Y  F U N C T I O N S  <<<<<<<<<<
 *
 * ========================================================================
 */

/* XjAxisGetMinorTicOffsets
 */
int     XjAxisGetMinorTicOffsets        (Widget w, int *offsets, int n)
{
  AxisWidget    cw = (AxisWidget)w;
  int           i, n_tics;
  int           length = MAXPOS(cw) - MINPOS(cw) + 1;

  for (i = 0, n_tics = 0; (i < cw->axis.n_tics) && (i < n); i++)
    if (cw->axis.tic_lengths[i] < AXIS_MAJOR_TIC_LENGTH)
    {
      *offsets++ = (int)(cw->axis.tic_offsets[i] * (length-1));
      n_tics++;
    }

  return n_tics;
}


/* XjAxisGetMajorTicOffsets
 */
int     XjAxisGetMajorTicOffsets        (Widget w, int *offsets, int n)
{
  AxisWidget    cw = (AxisWidget)w;
  int           i, n_tics;
  int           length = MAXPOS(cw) - MINPOS(cw) + 1;

  for (i = 0, n_tics = 0; (i < cw->axis.n_tics) && (i < n); i++)
    if (cw->axis.tic_lengths[i] >= AXIS_MAJOR_TIC_LENGTH)
    {
      *offsets++ = (int)(cw->axis.tic_offsets[i] * (length-1));
      n_tics++;
    }

  return n_tics;
}


/* XjAxisTransformValuesNormalized
 */
void
XjAxisTransformValuesNormalized   (Widget           w,
                                   register double  *x_in,
                                   register double  *x_out,
                                   register int     n)
{
  AxisWidget      cw = (AxisWidget)w;
  
  transform_values_normalized
    (cw->axis.transform, cw->axis.value_type,
     MINPOS(cw), MAXPOS(cw), cw->axis.min_val, cw->axis.max_val,
     cw->axis.log_epsilon, cw->axis.log_epsilon_offset, cw->axis.log_delta,
     x_in, x_out, n);
}


/* XjAxisTransformValuesRasterized
 */
void
XjAxisTransformValuesRasterized   (Widget           w,
                                   register double  *x_in,
                                   register double  *x_out,
                                   register int     n)
{
  AxisWidget      cw = (AxisWidget)w;
  register int    length = MAXPOS(cw) - MINPOS(cw) + 1;

  transform_values_normalized
    (cw->axis.transform, cw->axis.value_type,
     MINPOS(cw), MAXPOS(cw), cw->axis.min_val, cw->axis.max_val,
     cw->axis.log_epsilon, cw->axis.log_epsilon_offset, cw->axis.log_delta,
     x_in, x_out, n);

  while (n > 0)
  {
    *x_out *= length - 1;
    n--; x_out++;
  }
}


/* XjAxisUntransformNormalizedValues
 */
void
XjAxisUntransformNormalizedValues   (Widget           w,
                                     register double  *x_in,
                                     register double  *x_out,
                                     register int     n)
{
  AxisWidget      cw = (AxisWidget)w;
  
  untransform_normalized_values
    (cw->axis.transform, cw->axis.value_type,
     MINPOS(cw), MAXPOS(cw), cw->axis.min_val, cw->axis.max_val,
     cw->axis.log_epsilon, cw->axis.log_epsilon_offset, cw->axis.log_delta,
     x_in, x_out, n);
}


/* XjAxisUntransformRasterizedValues
 */
void
XjAxisUntransformRasterizedValues   (Widget           w,
                                     register double  *x_in,
                                     register double  *x_out,
                                     register int     n)
{
  AxisWidget      cw = (AxisWidget)w;
  register int    length = MAXPOS(cw) - MINPOS(cw) + 1;
  register int    m = n;

  /* first normalize, then untransform */
  while (m > 0)
  {
    *x_in /= length - 1;
    m--; x_in++;
  }
  x_in -= n;

  untransform_normalized_values
    (cw->axis.transform, cw->axis.value_type,
     MINPOS(cw), MAXPOS(cw), cw->axis.min_val, cw->axis.max_val,
     cw->axis.log_epsilon, cw->axis.log_epsilon_offset, cw->axis.log_delta,
     x_in, x_out, n);
}


/* XjAxisGetTRansform
 */
void
XjAxisGetTransform      (Widget w, jlaTransformInfo *t)
{
  AxisWidget    cw = (AxisWidget)w;

  t->transform = cw->axis.transform;
  t->value_type = cw->axis.value_type;
  t->min_pos = MINPOS(cw);
  t->max_pos = MAXPOS(cw);
  t->min_val = cw->axis.min_val;
  t->max_val = cw->axis.max_val;
  t->log_epsilon = cw->axis.log_epsilon;
  t->log_delta = cw->axis.log_delta;
  t->log_epsilon_offset = cw->axis.log_epsilon_offset;
  t->log_n_powers = cw->axis.log_n_powers;
}


int
jlaBuildTransform       (jlaTransformInfo       *t,
                   AxisTransform                transform,
                   AxisValueType                value_type,
                   AxisEndpointPosition min_pos,
                   AxisEndpointPosition max_pos,
                   double                       min_val,
                   double                       max_val,
                   double                       log_epsilon)
{
  int   ret = 1;
  short dummy;
  
  t->transform = transform;
  t->value_type = value_type;
  t->min_pos = min_pos;
  t->max_pos = max_pos;
  t->min_val = min_val;
  t->max_val = max_val;
  t->log_epsilon = log_epsilon;

  if (transform != XjAXIS_LINEAR)
    ret = get_log_info
      (min_val, max_val, log_epsilon,
       &dummy,
       &t->log_n_powers,
       &t->log_delta,
       &t->log_epsilon_offset);

  return ret;
}
                         


/* jlaTransformValuesNormalized
 */
void
jlaTransformValuesNormalized    (jlaTransformInfo       *t,
                               register double  *x_in,
                               register double  *x_out,
                               register int             n)
{
  transform_values_normalized
    (t->transform, t->value_type,
     t->min_pos, t->max_pos, t->min_val, t->max_val,
     t->log_epsilon, t->log_epsilon_offset, t->log_delta,
     x_in, x_out, n);
}


/* jlaTransformValuesRasterized
 */
void
jlaTransformValuesRasterized    (jlaTransformInfo       *t,
                               register double  *x_in,
                               register double  *x_out,
                               register int             n)
{
  register int  length = t->max_pos - t->min_pos + 1;

  transform_values_normalized
    (t->transform, t->value_type,
     t->min_pos, t->max_pos, t->min_val, t->max_val,
     t->log_epsilon, t->log_epsilon_offset, t->log_delta,
     x_in, x_out, n);

  while (n > 0)
  {
    *x_out *= length - 1;
    n--; x_out++;
  }
}


/* jlaUntransformNormalizedValues
 */
void
jlaUntransformNormalizedValues(jlaTransformInfo       *t,
                               register double        *x_in,
                               register double        *x_out,
                               register int           n)
{
  untransform_normalized_values
    (t->transform, t->value_type,
     t->min_pos, t->max_pos, t->min_val, t->max_val,
     t->log_epsilon, t->log_epsilon_offset, t->log_delta,
     x_in, x_out, n);
}


/* jlaUntransformRasterizedValues
 */
void
jlaUntransformRasterizedValues(jlaTransformInfo       *t,
                               register double        *x_in,
                               register double        *x_out,
                               register int           n)
{
  register int    length = t->max_pos - t->min_pos + 1;
  register int    m = n;

  /* first normalize, then untransform */
  while (m > 0)
  {
    *x_in /= length - 1;
    m--; x_in++;
  }
  x_in -= n;

  untransform_normalized_values
    (t->transform, t->value_type,
     t->min_pos, t->max_pos, t->min_val, t->max_val,
     t->log_epsilon, t->log_epsilon_offset, t->log_delta,
     x_in, x_out, n);
}


static void
transform_values_normalized     (AxisTransform          transform,
                               AxisValueType            value_type,
                               AxisEndpointPosition     min_pos,
                               AxisEndpointPosition     max_pos,
                               register double  min_val,
                               register double  max_val,
                               register double  log_epsilon,
                               register double  log_epsilon_offset,
                               register double  log_delta,
                               register double  *x_in,
                               register double  *x_out,
                               register int             n)
{
  register double       log_x;

  /* linear real values, or time values */
  if ((transform == XjAXIS_LINEAR) || (value_type != XjAXIS_REAL))
    while (n > 0)
    {
      *x_out = (*x_in - min_val) / (max_val - min_val);
      n--; x_in++; x_out++;
    }

  /* logarithmic real values */
  else
    while (n > 0)
    {
      log_x = (ABS(*x_in) <= DBL_EPSILON? log_epsilon : log10 (ABS(*x_in)));
      
      /* special case: x is in (-e, +e) */
      if ((log_x - log_epsilon) <= DBL_EPSILON)
        *x_out = log_epsilon_offset;
      
      /* x positive */
      else if (*x_in > 0)
        *x_out = log_epsilon_offset + ((log_x - log_epsilon) / log_delta);
      
      /* x negative */
      else
        *x_out = log_epsilon_offset - ((log_x - log_epsilon) / log_delta);

      n--; x_in++; x_out++;
    }
}


#if 0
static void
untransform_normalized_values (AxisTransform          transform,
                               AxisValueType          value_type,
                               AxisEndpointPosition   min_pos,
                               AxisEndpointPosition   max_pos,
                               register double        min_val,
                               register double        max_val,
                               register double        log_epsilon,
                               register double        log_epsilon_offset,
                               register double        log_delta,
                               register double        *x_in,
                               register double        *x_out,
                               register int           n)
{
  register double       log_x;

  /* linear real values, or time values */
  if ((transform == XjAXIS_LINEAR) || (value_type != XjAXIS_REAL))
    while (n > 0)
    {
      *x_out = (*x_in * (max_val - min_val)) + min_val;
      n--; x_in++; x_out++;
    }

  /* logarithmic real values */
  else
    while (n > 0)
    {
      log_x = (ABS(*x_in) <= DBL_EPSILON? log_epsilon : log10 (ABS(*x_in)));

      /* special case: x is in (-e, +e) */
      if (*x_in == log_epsilon_offset)
        *x_out = 0.0;
      
      /* x positive */
      else if (*x_in > log_epsilon_offset)
        *x_out =
          pow (10.0, (*x_in * log_delta) - log_epsilon_offset + log_epsilon);
      
      /* x negative */
      else
        *x_out = 
          pow (10.0, (*x_in * log_delta) + log_epsilon_offset + log_epsilon);

      n--; x_in++; x_out++;
    }
}
#endif


static void
untransform_normalized_values (AxisTransform          transform,
                               AxisValueType          value_type,
                               AxisEndpointPosition   min_pos,
                               AxisEndpointPosition   max_pos,
                               register double        min_val,
                               register double        max_val,
                               register double        log_epsilon,
                               register double        log_epsilon_offset,
                               register double        log_delta,
                               register double        *x_in,
                               register double        *x_out,
                               register int           n)
{
  double log_max, log_min;
  register double       log_x;

  /* linear real values, or time values */
  if ((transform == XjAXIS_LINEAR) || (value_type != XjAXIS_REAL))
    while (n > 0)
    {
      *x_out = (*x_in * (max_val - min_val)) + min_val;
      n--; x_in++; x_out++;
    }

  /* logarithmic real values */
  else {
    log_max = (ABS(max_val) <= DBL_EPSILON? log_epsilon : log10 (ABS(max_val)));
    log_min = (ABS(min_val) <= DBL_EPSILON? log_epsilon : log10 (ABS(min_val)));
    if (max_val < 0. ) log_max = -log_max;
    if (min_val < 0. ) log_min = -log_min;
    while (n > 0)
    {
      log_x = *x_in * (log_max - log_min) + log_min;
      *x_out = pow (10.0 ,ABS(log_x));
      if ( log_x < 0. ) *x_out = *x_out * -1.0;
      n--; x_in++; x_out++;
    }
  }
}

