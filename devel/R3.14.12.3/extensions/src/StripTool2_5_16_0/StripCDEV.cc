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

extern "C" {
#include "StripDAQ.h"
}

#include <cdevSystem.h>
#include <cdevRequestObject.h>
#include <cdevCallback.h>
#include <cdevData.h>
#include <math.h>

#define CDEV_POLL_PERIOD        0.3

#define MAX_BUF_LEN             256
#define DEFAULT_ATTR            "VAL"

#define DEF_DEVATTR_UNITS       "undefined"
#define DEF_DEVATTR_PRECISION   4
#define DEF_DEVATTR_DISPLO      -1.0
#define DEF_DEVATTR_DISPHI      1.0

typedef enum
{
  DEVATTR_VALUE = 0,
  DEVATTR_UNITS,
  DEVATTR_PRECISION,
  DEVATTR_DISPLO,
  DEVATTR_DISPHI,
  DEVATTR_COUNT
}
DevAttr;

static char     *DevAttrStr[DEVATTR_COUNT] =
{
  "value",
  "units",
  "precision",
  "displayLow",
  "displayHigh"
};

typedef struct _DeviceData
{
  cdevCallback          *cb;
  char                  buf[MAX_BUF_LEN];
  char                  *dev, *attr;
  int                   tag;
  double                value;
  struct _StripDAQInfo  *this_;
}
DeviceData;
  

typedef struct _StripDAQInfo
{
  Strip         strip;
  DeviceData    dev_data[STRIP_MAX_CURVES];
} StripDAQInfo;
      

/* ====== Prototypes ====== */
static void     fd_callback     (int, int, void *);
static void     work_callback   (XtPointer, int *, XtInputId *);
static void     poll_callback   (XtPointer, XtIntervalId *);
static void     data_callback   (int,
                                 void *,
                                 cdevRequestObject &,
                                 cdevData &);
static double   get_value       (void *);

/*
 * StripDAQ_initialize
 */
extern "C" StripDAQ        StripDAQ_initialize     (Strip strip)
{
  cdevSystem    &system = cdevSystem::defaultSystem();
  StripDAQInfo  *scd = NULL;
  int           status;
  int           fd[MAX_BUF_LEN];
  int           i;

  if ((scd = (StripDAQInfo *)calloc (sizeof (StripDAQInfo), 1)) != NULL)
  {
    scd->strip = strip;
    for (i = 0; i < STRIP_MAX_CURVES; i++)
    {
      scd->dev_data[i].cb = 0;
      scd->dev_data[i].tag = -1;
      scd->dev_data[i].this_ = scd;
    }

    system.setThreshold (CDEV_SEVERITY_ERROR);

    i = MAX_BUF_LEN;
    status = system.getFd (fd, i);
    if (status == CDEV_SUCCESS)
    {
      for (i--; i >= 0; i--)
        Strip_addfd (scd->strip, fd[i], work_callback, (XtPointer)scd);
      status = system.addFdChangedCallback (fd_callback, scd);
      if (status == CDEV_SUCCESS)
        system.flush();
      poll_callback (scd->strip, (XtIntervalId *)0);
    }
      
    if (status != CDEV_SUCCESS)
    {
      free (scd);
      scd = NULL;
    }
  }
  
  return (StripDAQ)scd;
}


/*
 * StripDAQ_terminate
 */
extern "C" void    StripDAQ_terminate      (StripDAQ the_scd)
{
  StripDAQInfo  *scd = (StripDAQInfo *)the_scd;
  int           i;
  
  for (i = 0; i < STRIP_MAX_CURVES; i++)
  {
    if (scd->dev_data[i].cb)
      delete scd->dev_data[i].cb;
  }
  free (scd);
}


/*
 * StripDAQ_request_connect
 */
extern "C" int     StripDAQ_request_connect        (StripCurve curve, void *the_scd)
{
  StripDAQInfo          *scd = (StripDAQInfo *)the_scd;
  DeviceData            *dd;
  cdevSystem            &system = cdevSystem::defaultSystem();
  cdevRequestObject     *request;
  cdevData              data;
  int                   i;
  int                   status;
  int                   ret_val;
  char                  msg_buf[MAX_BUF_LEN];
  int                   tag;

  for (i = 0; i < STRIP_MAX_CURVES; i++)
    if (scd->dev_data[i].cb == 0)
      break;

  if (ret_val = (i < STRIP_MAX_CURVES))
  {
    dd = &scd->dev_data[i];
    StripCurve_setattr (curve, STRIPCURVE_FUNCDATA, dd, 0);
      
    /* parse string designating the requested value for device/attribute */
    strcpy(dd->buf, (char *)StripCurve_getattr_val (curve, STRIPCURVE_NAME));
    for (dd->dev = dd->attr = dd->buf;
         (*dd->attr != '\0') && (*dd->attr != '.');
         dd->attr++);
    if (*dd->attr == '.')
    {
      *dd->attr = '\0'; /* separate the device from the attribute */
      dd->attr++;               /* point at the attribute string */
    }
    else dd->attr = DEFAULT_ATTR;

    sprintf (msg_buf, "get %s", dd->attr);

    /* set the device context to return useful attributes */
    for (i = 0; i < DEVATTR_COUNT; i++)
    {
      status = cdevData::tagC2I (DevAttrStr[i], &tag);
      if (status == CDEV_SUCCESS)
        data.insert (tag, 1);
    }
      
    /* set up the monitor */
    request = cdevRequestObject::attachPtr (dd->dev, msg_buf);
    if (request) {
      dd->cb = new cdevCallback (data_callback, curve);
      dd->tag = -1;
      request->device().setContext (data);
      status = request->sendCallback (0, *dd->cb);
      ret_val = (status == CDEV_SUCCESS);
      system.flush();
    }
    else ret_val = 0;
  }

  return ret_val;
}


/*
 * StripDAQ_request_disconnect
 */
extern "C" int     StripDAQ_request_disconnect     (StripCurve curve, void *)
{
  DeviceData            *dd;
  cdevSystem            &system = cdevSystem::defaultSystem();
  cdevRequestObject     *request;
  int                   status;
  int                   ret_val;
  char                  msg_buf[MAX_BUF_LEN];

  dd = (DeviceData *)StripCurve_getattr_val (curve, STRIPCURVE_FUNCDATA);

  /* this will happen if a non-CDEV curve is submitted for disconnect */
  if (!dd) return 1;

  if (dd->cb != 0)
  {
    sprintf (msg_buf, "monitorOff %s", dd->attr);
    request = cdevRequestObject::attachPtr (dd->dev, msg_buf);
    if (request) {
      status = request->sendCallback (0, *dd->cb);
      system.flush();
      delete dd->cb;            // this seems like a bad idea!
    }
    dd->cb = 0;
    dd->tag = -1;
    ret_val = 1;
  }
  else ret_val = 1;

  return ret_val;
}


/*
 * fd_callback
 *
 *      Add new file descriptors to select upon.
 *      Remove old file descriptors from selection.
 */
static void     fd_callback     (int fd, int opened, void *data)
{
  StripDAQInfo  *scd = (StripDAQInfo *)data;
  if (opened)
    Strip_addfd (scd->strip, fd, work_callback, (XtPointer)scd);
  else Strip_clearfd (scd->strip, fd);
}


/*
 * work_callback
 *
 *      Gives control to cdev for a while.
 */
static void     work_callback           (XtPointer      BOGUS(data),
                                         int            *fd,
                                         XtInputId      *BOGUS(id))
{
  cdevSystem::defaultSystem().pend(*fd);
}


/*
 * poll_callback
 *
 *      Gives control to cdev for a while.
 */
static void     poll_callback           (XtPointer arg, XtIntervalId *)
{
  Strip strip = (Strip)arg;
  cdevSystem::defaultSystem().poll();
  Strip_addtimeout (strip, CDEV_POLL_PERIOD, poll_callback, (XtPointer)strip);
}


/*
 * data_callback
 */
static void     data_callback   (int                    status,
                                 void                   *arg,
                                 cdevRequestObject      &,
                                 cdevData               &data)
{
  StripCurve            curve = (StripCurve)arg;
  DeviceData            *dd;
  cdevSystem            &system = cdevSystem::defaultSystem();
  cdevRequestObject     *request;
  double                lo, hi;
  int                   p;
  char                  msg_buf[MAX_BUF_LEN];
  cdevData              temp_dat;

  union _val {
    int         i;
    double      d;
    char        *s;
  } val;

  /* first, get the DeviceData structure from the curve */
  dd = (DeviceData *)StripCurve_getattr_val (curve, STRIPCURVE_FUNCDATA);

  if (status == CDEV_DISCONNECTED)
  {
    Strip_setwaiting (dd->this_->strip, curve);
  }
  else if (status == CDEV_SUCCESS || status == CDEV_RECONNECTED)
  {
    /* if this is the first callback, gather some useful info */
    if (dd->tag < 0)
    {
      cdevData::tagC2I (DevAttrStr[DEVATTR_VALUE], &dd->tag);
        
      /* get the value */
      data.get (dd->tag, &dd->value);
        
      /* get context info for curve */
      /* units */
      if (!StripCurve_getstat (curve, STRIPCURVE_EGU_SET))
      {
        status = data.get (DevAttrStr[DEVATTR_UNITS], &val.s);
        if (status != CDEV_SUCCESS)
          StripCurve_setattr (curve, STRIPCURVE_EGU, DEF_DEVATTR_UNITS, 0);
        else
        {
          StripCurve_setattr (curve, STRIPCURVE_EGU, val.s, 0);
          free (val.s);
        }
      }
        
      /* precision */
      if (!StripCurve_getstat (curve, STRIPCURVE_PRECISION_SET))
      {
        status = data.get (DevAttrStr[DEVATTR_PRECISION], &val.i);
        if (status != CDEV_SUCCESS)
          val.i = DEF_DEVATTR_PRECISION;
        StripCurve_setattr (curve, STRIPCURVE_PRECISION, val.i, 0);
      }
        
        
      lo = floor (dd->value - fabs (dd->value));
      hi = ceil (dd->value + fabs (dd->value));
      if (lo == hi)
      {
        lo = DEF_DEVATTR_DISPLO;
        hi = DEF_DEVATTR_DISPHI;
      }
        
      /* make sure that the diplay range will include the current value */
      p = *(int *)StripCurve_getattr_val (curve, STRIPCURVE_PRECISION);
      while ((hi - lo) < (1.0 / (double)pow ((double)10, p)))
      {
        hi *= 10.0;
        lo /= 10.0;
        fprintf (stdout, "hi: %f, lo: %f\n", hi, lo);
      }
        
      /* display low */
      if (!StripCurve_getstat (curve, STRIPCURVE_MIN_SET))
      {
        status = data.get (DevAttrStr[DEVATTR_DISPLO], &val.d);
        if (status != CDEV_SUCCESS)
          val.d = lo;
        StripCurve_setattr (curve, STRIPCURVE_MIN, val.d, 0);
      }
        
      /* display high */
      if (!StripCurve_getstat (curve, STRIPCURVE_MAX_SET))
      {
        status = data.get (DevAttrStr[DEVATTR_DISPHI], &val.d);
        if (status != CDEV_SUCCESS)
          val.d = (hi > lo? hi : ceil (lo + 2*lo));
        StripCurve_setattr (curve, STRIPCURVE_MAX, val.d, 0);
      }
        
      /* now initiate the monitor */
      sprintf (msg_buf, "monitorOn %s", dd->attr);
      request = cdevRequestObject::attachPtr (dd->dev, msg_buf);
      if (request) {
        delete dd->cb;
        dd->cb = new cdevCallback (data_callback, curve);
          
        /* set the device context to return useful attributes */
        temp_dat.insert (DevAttrStr[DEVATTR_VALUE], 1);
          
        request->device().setContext (temp_dat);
        request->sendCallback (0, *dd->cb);
      }
      system.flush();
    }

    /* now get the value */
    data.get (dd->tag, &dd->value);

#if 0
    fprintf
      (stdout,
       "StripDAQ, data_callback(): %s.%s = %12.8f\n",
       dd->dev, dd->attr, dd->value);
    fflush (stdout);
#endif
      
    if (StripCurve_getstat (curve, STRIPCURVE_WAITING))
    {
      StripCurve_setattr
        (curve, STRIPCURVE_SAMPLEFUNC, get_value, 0);
      Strip_setconnected (dd->this_->strip, curve);
    }
  }
}


/*
 * get_value
 *
 *      Returns the current value specified by the CurveData passed in.
 */
static double   get_value       (void *data)
{
  DeviceData    *dd = (DeviceData *)data;

  return dd->value;
}
