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

#ifndef _jlAxis
#define _jlAxis

#include <Xm/Xm.h>
#include <limits.h>

#define AXIS_MIN_MAJOR_TICS   4
#define AXIS_MAX_MAJOR_TICS   6


/* AXIS_(MIN/MAX)_TICS
 *
 *      Min/max number of tics permitted on axis.
 */
#define AXIS_MIN_TICS            AXIS_MIN_MAJOR_TICS
#define AXIS_MAX_TICS            (AXIS_MAX_MAJOR_TICS * 12)


/* AxisDirection
 *
 *      Specifies how the axis should be drawn.  UP and DOWN indicate
 *      horizontal axes while LEFT and RIGHT indicate vertical.  In each
 *      case, the labels and tic marks will be drawn on the nominal side
 *      of the axis line.
 */
typedef enum    _AxisDirection
{
  XjAXIS_UP, XjAXIS_DOWN, XjAXIS_LEFT, XjAXIS_RIGHT
}
AxisDirection;


/* AxisEndpointPosition
 *
 *      Indicates the offset of an axis line end point, relative to the
 *      beginning of the axis widget.  The default values of XjAXIS_ORIGIN
 *      and XjAXIS_TERMINUS indicate that the axis line should stretch the
 *      entire extent of the widget.  Note that numbers outside of
 *      the range (0, width-1), with the exception of the special values
 *      noted above, will result in clipping.
 *
 *      NB: min_pos must be less than max_pos.
 */
typedef int     AxisEndpointPosition;
#define XjAXIS_ORIGIN           INT_MIN
#define XjAXIS_TERMINUS         INT_MAX


/* AxisValueType
 *
 *      Specifies how the values are interpreted --as real numbers or
 *      as dates.  If dates, then the value is converted to a struct
 *      timeval by extracing the integer and fractional parts.
 *
 *      ABSTIME:                nicest intervals and tic locations chosen.
 *      RELTIME_DATES:          max point is always labeled, and intuitive
 *                              interval widths are chosen, but the labels
 *                              will not fall on intuitive points.
 *      RELTIME_NUMBERS:        max point is always labeled, and intuitive
 *                              interval widths are chosen, and the labels
 *                              will be numbers relative to the max point,
 *                              which is always 0.
 */
typedef enum    _AxisValueType
{
  XjAXIS_REAL = 0,
  XjAXIS_ABSTIME,
  XjAXIS_RELTIME_DATES,
  XjAXIS_RELTIME_NUMBERS
}
AxisValueType;


/* AxisTransform
 *
 *      Specifies what function transforms the values
 *      along the axis interval into points in axis space.
 */
typedef enum    _AxisTransform
{
  XjAXIS_LINEAR = 0, XjAXIS_LOG10
}
AxisTransform;


/* ====== New resource names ======
 */
#define XjNunitString           "unitString"
#define XjNdirection            "direction"
#define XjNtransform            "transform"
#define XjNvalueType            "valueType"
#define XjNlogEpsilon           "logEpsilon"
#define XjNminVal                     "minVal"
#define XjNmaxVal                     "maxVal"
#define XjNminPos                     "minPos"
#define XjNmaxPos                     "maxPos"
#define XjNminMargin            "minMargin"
#define XjNusePixmap            "usePixmap"
#define XjNtextColor            "textColor"
#define XjNpMinVal              "pMinVal"
#define XjNpMaxVal              "pMaxVal"
#define XjNpLogEpsilon          "pLogEpsilon"


/* ====== New resource classes ======
 */
#define XjCUnitString           "UnitString"
#define XjCDirection            "Direction"
#define XjCValueType            "ValueType"
#define XjCTransform            "Transform"
#define XjCLogEpsilon           "LogEpsilon"
#define XjCMinVal                   "MinVal"
#define XjCMaxVal                     "MaxVal"
#define XjCMinPos                     "MinPos"
#define XjCMaxPos                     "MaxPos"
#define XjCMinMargin            "MinMargin"
#define XjCUsePixmap            "UsePixmap"
#define XjCTextColor            "TextColor"
#define XjCPMinVal              "PMinVal"
#define XjCPMaxVal              "PMaxVal"
#define XjCPLogEpsilon          "PLogEpsilon"


/* ====== New resource types ======
 */
#define XjRAxisDirection              "AxisDirection"
#define XjRAxisEndpointPosition "AxisEndpointPosition"
#define XjRAxisValueType              "AxisValueType"
#define XjRAxisTransform              "AxisTransform"
#define XjRDouble                           "Double"


/* ====== Class record constants ====== */

extern WidgetClass                    axisWidgetClass;

typedef struct _AxisClassRec *  AxisWidgetClass;
typedef struct _AxisRec *             AxisWidget;


/* ====== some useful public functions ====== */

/* XjAxisGet(Minor/Major)TicOffsets
 *
 *      Fills an array with the tic offsets for the given axis widget,
 *      returning the number of valid entries written in to the array.
 *      The integer offsets are from the min endpoint for the currently
 *      rendered widget.
 */
int     XjAxisGetMajorTicOffsets    (Widget,
                                     int *,             /* array */
                                     int);              /* array elem count */
int     XjAxisGetMinorTicOffsets    (Widget,
                                     int *,             /* array */
                                     int);              /* array elem count */


/* XjAxisTransformValues(Rasterized/Normalized)
 *
 *      Rasterized: Transforms given array of values from
 *        (min, max) --> (minPos, maxPos).
 *
 *      Normalized: Transforms given array of values from
 *        (min, max) --> (0, 1).
 *
 * NB:  Input and output buffers may reference the same array,
 *      with no ill effects, for in-place transform.
 */
void  XjAxisTransformValuesRasterized     (Widget,
                                           double *,  /* input buffer */
                                           double *,  /* result buffer */
                                           int);      /* buffer count */

void  XjAxisTransformValuesNormalized     (Widget,
                                           double *,  /* input buffer */
                                           double *,  /* result buffer */
                                           int);      /* buffer count */


/* XjAxisUntransform(Rasterized/Normalized)Values
 *
 *    Perform inverse transform of above, mapping to a value along the axis.
 *
 * NB:  Input and output buffers may reference the same array,
 *      with no ill effects, for in-place transform.
 */
void  XjAxisTransformValuesRasterized     (Widget,
                                           double *,  /* input buffer */
                                           double *,  /* result buffer */
                                           int);      /* buffer count */

void  XjAxisTransformValuesNormalized     (Widget,
                                           double *,  /* input buffer */
                                           double *,  /* result buffer */
                                           int);      /* buffer count */


/* jlaTransformInfo
 *
 *      This structure contains pre-computed info needed by the following
 *      transform functions.
 */
typedef struct _jlaTransformInfo
{
  AxisTransform         transform;
  AxisValueType         value_type;
  AxisEndpointPosition  min_pos, max_pos;
  double                min_val, max_val;
  double                log_epsilon;
  double                log_delta;
  double                log_epsilon_offset;
  short                 log_n_powers;
} jlaTransformInfo;


/* XjAxisGetTransform
 *
 *      Fills the caller-supplied buffer with transform info for
 *      the given widget.
 */
void    XjAxisGetTransform      (Widget, jlaTransformInfo *);


/* jlaBuildTransform
 *
 *      Builds a transform structure suitable for subsequent calls
 *      to jlaTransform...
 *
 *      The whole point of this facility is to allow the outside world
 *      to use the axis transform algorithms separate from any widget.
 *      This is good for several reasons: it promotes consistency,
 *      centralizes complex code, and avoids redundancy.
 */
int     jlaBuildTransform       (jlaTransformInfo *,    /* result buffer */
                                 AxisTransform,         /* transform */
                                 AxisValueType,         /* value_type */
                                 AxisEndpointPosition,  /* min_pos */
                                 AxisEndpointPosition,  /* max_pos */
                                 double,                /* min_val */
                                 double,                /* max_val */
                                 double);               /* log_epsilon */
                                 

/* jlaTransformValues(Rasterized / Normalized)
 *
 *      Rasterized: Transforms given array of values from
 *        (min, max) --> (minPos, maxPos).
 *
 *      Normalized: Transforms given array of values from
 *        (min, max) --> (0, 1).
 *
 * NB:  Input and output buffers may reference the same array,
 *      with no ill effects, for in-place transform.
 */
void    jlaTransformValuesRasterized    (jlaTransformInfo *,
                                         double *,      /* input buffer */
                                         double *,      /* result buffer */
                                         int);          /* buffer count */

void    jlaTransformValuesNormalized    (jlaTransformInfo *,
                                         double *,      /* input buffer */
                                         double *,      /* result buffer */
                                         int);          /* buffer count */


/* jlaUntransform(Rasterized/Normalized)Values
 *
 *    Perform inverse transform of above, mapping to a value along the axis.
 *
 * NB:  Input and output buffers may reference the same array,
 *      with no ill effects, for in-place transform.
 */
void    jlaUntransformRasterizedValues  (jlaTransformInfo *,
                                         double *,      /* input buffer */
                                         double *,      /* result buffer */
                                         int);          /* buffer count */

void    jlaUntransformNormalizedValues  (jlaTransformInfo *,
                                         double *,      /* input buffer */
                                         double *,      /* result buffer */
                                         int);          /* buffer count */

void    XjAxisUntransformRasterizedValues   (Widget           w,
                                     register double  *x_in,
                                     register double  *x_out,
                                     register int     n);

#endif  /* #ifndef _jlAxis */
