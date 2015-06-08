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

#define DEBUG_CONNECTING 0


#include "StripCurve.h"

StripCurve      StripCurve_init (StripConfig *scfg)
{
  StripCurveInfo        *sc;

  if ((sc = (StripCurveInfo *)malloc (sizeof (StripCurveInfo))) != NULL)
  {
    sc->scfg                    = scfg;
    sc->id                      = 0;
    sc->details                 = 0;
    sc->func_data               = 0;
    sc->get_value               = 0;
    sc->status                  = 0;
  }

  return (StripCurve)sc;
}


void            StripCurve_delete       (StripCurve the_sc)
{
  free (the_sc);
}


int             StripCurve_setattr      (StripCurve the_sc, ...)
{
  va_list               ap;
  int                   attrib;
  StripCurveInfo        *sc = (StripCurveInfo *)the_sc;
  int                   ret_val = 1;
  
#if DEBUG_CONNECTING
  print("%s StripCurve_setattr\"  %s\n",
    timeStamp(), sc->details->name);
#endif	
  
  va_start (ap, the_sc);
  for (attrib = va_arg (ap, StripCurveAttribute);
       (attrib != 0);
       attrib = va_arg (ap, StripCurveAttribute))
  {
    if ((ret_val = ((attrib > 0) &&
	(attrib < STRIPCURVE_LAST_ATTRIBUTE))))
    {
#if DEBUG_CONNECTING
	print("*** StripCurve_setattr: attrib=%d\n", attrib);
#endif	
      switch (attrib)
      {
	case STRIPCURVE_NAME:
	  strcpy (sc->details->name, va_arg (ap, char *));
	  StripConfigMask_set
	    (&sc->details->update_mask, SCFGMASK_CURVE_NAME);
	  StripConfigMask_set
	    (&sc->scfg->UpdateInfo.update_mask, SCFGMASK_CURVE_NAME);
	  break;
	  
	case STRIPCURVE_EGU:
	  strcpy (sc->details->egu, va_arg (ap, char *));
	  StripConfigMask_set
	    (&sc->details->update_mask, SCFGMASK_CURVE_EGU);
	  StripConfigMask_set
	    (&sc->scfg->UpdateInfo.update_mask, SCFGMASK_CURVE_EGU);
	  StripCurve_setstat(sc, STRIPCURVE_EGU_SET);
	  break;
	  
	case STRIPCURVE_COMMENT:
	  strcpy (sc->details->comment, va_arg (ap, char *));
	  StripConfigMask_set
	    (&sc->details->update_mask, SCFGMASK_CURVE_COMMENT);
	  StripConfigMask_set
	    (&sc->scfg->UpdateInfo.update_mask, SCFGMASK_CURVE_COMMENT);
	  StripCurve_setstat (sc, STRIPCURVE_COMMENT_SET);
	  break;
	  
	case STRIPCURVE_PRECISION:
	  sc->details->precision = va_arg (ap, int);
	  sc->details->precision =
	    max ( sc->details->precision, STRIPMIN_CURVE_PRECISION);
	  sc->details->precision =
	    min ( sc->details->precision, STRIPMAX_CURVE_PRECISION);
	  StripConfigMask_set
	    (&sc->details->update_mask, SCFGMASK_CURVE_PRECISION);
	  StripConfigMask_set
	    (&sc->scfg->UpdateInfo.update_mask, SCFGMASK_CURVE_PRECISION);
	  StripCurve_setstat (sc, STRIPCURVE_PRECISION_SET);
	  break;
	  
	case STRIPCURVE_MIN:
	  sc->details->min = va_arg (ap, double);
	  StripConfigMask_set
	    (&sc->details->update_mask, SCFGMASK_CURVE_MIN);
	  StripConfigMask_set
	    (&sc->scfg->UpdateInfo.update_mask, SCFGMASK_CURVE_MIN);
	  StripCurve_setstat (sc, STRIPCURVE_MIN_SET);
	  break;
	  
	case STRIPCURVE_MAX:
	  sc->details->max = va_arg (ap, double);
	  StripConfigMask_set
	    (&sc->details->update_mask, SCFGMASK_CURVE_MAX);
	  StripConfigMask_set
	    (&sc->scfg->UpdateInfo.update_mask, SCFGMASK_CURVE_MAX);
	  StripCurve_setstat (sc, STRIPCURVE_MAX_SET);
	  break;
	  
	case STRIPCURVE_SCALE:
	  sc->details->scale = va_arg (ap, int);
	  StripConfigMask_set
	    (&sc->details->update_mask, SCFGMASK_CURVE_SCALE);
	  StripConfigMask_set
	    (&sc->scfg->UpdateInfo.update_mask, SCFGMASK_CURVE_SCALE);
	  break;
	  
	case STRIPCURVE_PLOTSTAT:
	  sc->details->plotstat = va_arg (ap, int);
	  StripConfigMask_set
	    (&sc->details->update_mask, SCFGMASK_CURVE_PLOTSTAT);
	  StripConfigMask_set
	    (&sc->scfg->UpdateInfo.update_mask, SCFGMASK_CURVE_PLOTSTAT);
	  break;
	  
	case STRIPCURVE_COLOR:
	  sc->details->color = va_arg (ap, cColor *);
	  break;
	  
	case STRIPCURVE_FUNCDATA:
	  sc->func_data = va_arg (ap, void *);
	  break;
	  
	case STRIPCURVE_SAMPLEFUNC:
	  sc->get_value = va_arg (ap, StripCurveSampleFunc);
	  break;
	  
      }
    }
    else break;
  }
  
  va_end (ap);
#if DEBUG_CONNECTING
  print("*** StripCurve_setattr: Ending\n",
    timeStamp());
#endif	
  return ret_val;
}


int             StripCurve_getattr      (StripCurve the_sc, ...)
{
  va_list               ap;
  int                   attrib;
  StripCurveInfo        *sc = (StripCurveInfo *)the_sc;
  int                   ret_val = 1;

  va_start (ap, the_sc);
  for (attrib = va_arg (ap, StripCurveAttribute);
       (attrib != 0);
       attrib = va_arg (ap, StripCurveAttribute))
  {
    if ((ret_val = ((attrib > 0) && (attrib < STRIPCURVE_LAST_ATTRIBUTE))))
      switch (attrib)
      {
	case STRIPCURVE_NAME:
	  *(va_arg (ap, char **)) = sc->details->name;
	  break;
	case STRIPCURVE_EGU:
	  *(va_arg (ap, char **)) = sc->details->egu;
	  break;
	case STRIPCURVE_COMMENT:
	  *(va_arg (ap, char **)) = sc->details->comment;
	  break;
	case STRIPCURVE_PRECISION:
	  *(va_arg (ap, int *)) = sc->details->precision;
	  break;
	case STRIPCURVE_MIN:
	  *(va_arg (ap, double *)) = sc->details->min;
	  break;
	case STRIPCURVE_MAX:
	  *(va_arg (ap, double *)) = sc->details->max;
	  break;
	case STRIPCURVE_SCALE:
	  *(va_arg (ap, int *)) = sc->details->scale;
	  break;
	case STRIPCURVE_PLOTSTAT:
	  *(va_arg (ap, int *)) = sc->details->plotstat;
	  break;
	case STRIPCURVE_COLOR:
	  *(va_arg (ap, cColor **)) = sc->details->color;
	  break;
	case STRIPCURVE_FUNCDATA:
	  *(va_arg (ap, void **)) = sc->func_data;
	  break;
	case STRIPCURVE_SAMPLEFUNC:
	  *(va_arg (ap, StripCurveSampleFunc *)) = sc->get_value;
	  break;
      }
    else break;
  }
  va_end (ap);
  return ret_val;
}


void    StripCurve_update       (StripCurve the_sc)
{
  StripCurveInfo        *sc = (StripCurveInfo *)the_sc;

  StripConfig_update (sc->scfg, SCFGMASK_CURVE);
}  


/*
 * StripCurve_set/getattr_val
 */
void    *StripCurve_getattr_val (StripCurve the_sc, StripCurveAttribute attrib)
{
  StripCurveInfo        *sc = (StripCurveInfo *)the_sc;
  
  switch (attrib)
  {
  case STRIPCURVE_NAME:
    return (void *)sc->details->name;
  case STRIPCURVE_EGU:
    return (void *)sc->details->egu;
  case STRIPCURVE_COMMENT:
    return (void *)sc->details->comment;
  case STRIPCURVE_PRECISION:
    return (void *)&sc->details->precision;
  case STRIPCURVE_MIN:
    return (void *)&sc->details->min;
  case STRIPCURVE_MAX:
    return (void *)&sc->details->max;
  case STRIPCURVE_SCALE:
    return (void *)&sc->details->scale;
  case STRIPCURVE_PLOTSTAT:
    return (void *)&sc->details->plotstat;
  case STRIPCURVE_COLOR:
    return (void *)sc->details->color;
  case STRIPCURVE_FUNCDATA:
    return (void *)sc->func_data;
  case STRIPCURVE_SAMPLEFUNC:
    return (void *)sc->get_value;
  default:
    return NULL;
  }
}


/*
 * StripCurve_setstat
 */
void            StripCurve_setstat      (StripCurve     the_sc,
                                         unsigned       stat)
{
  StripCurveInfo        *sc = (StripCurveInfo *)the_sc;
  int                   i;

#if DEBUG_CONNECTING
  if(stat & STRIPCURVE_WAITING) {
    print("%s  StripCurve_setstat:STRIPCURVE_WAITING\"  %s\n",
	timeStamp(), sc->details->name);
  }
  if(stat & STRIPCURVE_CONNECTED) {
    print("%s  StripCurve_setstat:STRIPCURVE_CONNECTED\"  %s\n",
	timeStamp(), sc->details->name);
  }
#endif  

  /* translate status bits into StripConfigMaskElement values and
   * update set_mask appropriately */
  if (sc->details)
    for (i = 0; (i + SCIDX_EGU_SET) < SCIDX_COUNT; i++)
      if (stat & (1 << (i + SCIDX_EGU_SET)))
        StripConfigMask_set (&sc->details->set_mask, SCFGMASK_CURVE_EGU + i);

  /* update the status */
  sc->status |= stat;
}

/*
 * StripCurve_getstat
 */
StripCurveStatus        StripCurve_getstat      (StripCurve     the_sc,
                                                 unsigned       stat)
{
  unsigned long         ret;
  StripCurveInfo        *sc = (StripCurveInfo *)the_sc;
  int                   i;
  

  /* need to translate StripConfigMaskElement values from the
   * set_mask into StripCurveStatus bits to combine with the
   * existing status bits to produce the return value. */
  ret = sc->status;

  if (sc->details)
    for (i = 0; (i + SCIDX_EGU_SET) < SCIDX_COUNT; i++)
      if (StripConfigMask_stat
          (&sc->details->set_mask, SCFGMASK_CURVE_EGU + i))
        ret |= (1 << (SCIDX_EGU_SET + i));
  
  return (StripCurveStatus)(ret & stat);
}

/*
 * StripCurve_clearstat
 */
void            StripCurve_clearstat    (StripCurve     the_sc,
                                         unsigned       stat)
{
  StripCurveInfo        *sc = (StripCurveInfo *)the_sc;
  int                   i;

#if DEBUG_CONNECTING
  if(stat & STRIPCURVE_WAITING) {
    print("%s  StripCurve_clearstat:STRIPCURVE_WAITING\"  %s\n",
	timeStamp(), sc->details->name);
  }
  if(stat & STRIPCURVE_CONNECTED) {
    print("%s  StripCurve_clearstat:STRIPCURVE_CONNECTED\"  %s\n",
	timeStamp(), sc->details->name);
  }
#endif
      
  /* translate status bits into StripConfigMaskElement values and
   * update set_mask appropriately */
  if (sc->details)
    for (i = 0; (i + SCIDX_EGU_SET) < SCIDX_COUNT; i++)
      if (stat & (1 << (i + SCIDX_EGU_SET)))
        StripConfigMask_unset (&sc->details->set_mask, SCFGMASK_CURVE_EGU + i);

  /* update the status */
  sc->status &= ~stat;
}

/* **************************** Emacs Editing Sequences ***************** */
/* Local Variables: */
/* tab-width: 6 */
/* c-basic-offset: 2 */
/* c-comment-only-line-offset: 0 */
/* c-indent-comments-syntactically-p: t */
/* c-label-minimum-indentation: 1 */
/* c-file-offsets: ((substatement-open . 0) (label . 2) */
/* (brace-entry-open . 0) (label .2) (arglist-intro . +) */
/* (arglist-cont-nonempty . c-lineup-arglist) ) */
/* End: */
