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

#define DEBUG_SCM 0

#include "StripConfig.h"

#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <X11/Xlib.h>
#include <X11/Intrinsic.h>

#if !defined(WIN32) && !defined(linux)
/* This is not an ANSI function and may not be included from string.h.
   Include it here explicitly */
extern char *strdup(const char *);
#endif

/* ====== Configuration File Stuff ====== */
#define STRIPCONFIG_MAJOR_VERSION       1
#define STRIPCONFIG_MINOR_VERSION       2
#define STRIPCONFIG_HEADER_STR          "StripConfig"
#define LEFT_COLUMNWIDTH                30
#define NUMERIC_COLUMNWIDTH             10
#define BUFSIZE                         256

char    *untitled_ = "Untitled";

typedef enum
{
  TITLE = 0,
  FILENAME,
  TIMESPAN,
  NUM_SAMPLES,
  SAMPLE_INTERVAL,
  REFRESH_INTERVAL,
  BACKGROUND,
  FOREGROUND,
  GRID,
  COLOR1,
  COLOR2,
  COLOR3,
  COLOR4,
  COLOR5,
  COLOR6,
  COLOR7,
  COLOR8,
  COLOR9,
  COLOR10,
  GRID_XON,
  GRID_YON,
  AXIS_YCOLORSTAT,
  GRAPH_LINEWIDTH,
  NAME,
  EGU,
  COMMENT,
  PRECISION,
  MIN,
  MAX,
  SCALE,
  PLOTSTAT,
  STRIP,
  TIME,
  COLOR,
  OPTION,
  CURVE,
  SEPARATOR,
  LAST_TOKEN
}
SCFToken;

char    *SCFTokenStr[LAST_TOKEN] =
{
  "Title",
  "Filename",
  "Timespan",
  "NumSamples",
  "SampleInterval",
  "RefreshInterval",
  "Background",
  "Foreground",
  "Grid",
  "Color1",
  "Color2",
  "Color3",
  "Color4",
  "Color5",
  "Color6",
  "Color7",
  "Color8",
  "Color9",
  "Color10",
  "GridXon",
  "GridYon",
  "AxisYcolorStat",
  "GraphLineWidth",
  "Name",
  "Units",
  "Comment",
  "Precision",
  "Min",
  "Max",
  "Scale",
  "PlotStatus",
  "Strip",
  "Time",
  "Color",
  "Option",
  "Curve",
  "."
};

typedef enum
{
  STRIPCFG_BACKGROUND,
  STRIPCFG_FOREGROUND,
  STRIPCFG_GRID,
  STRIPCFG_COLOR1,
  STRIPCFG_COLOR2,
  STRIPCFG_COLOR3,
  STRIPCFG_COLOR4,
  STRIPCFG_COLOR5,
  STRIPCFG_COLOR6,
  STRIPCFG_COLOR7,
  STRIPCFG_COLOR8,
  STRIPCFG_COLOR9,
  STRIPCFG_COLOR10,
  STRIPCFG_LASTIDX
}
StripCfgColorIdx;

static char     *StripCfgColorStr[STRIPCFG_LASTIDX] =
{
  STRIPDEF_COLOR_BACKGROUND_STR,
  STRIPDEF_COLOR_FOREGROUND_STR,
  STRIPDEF_COLOR_GRID_STR,
  STRIPDEF_COLOR_COLOR1_STR,
  STRIPDEF_COLOR_COLOR2_STR,
  STRIPDEF_COLOR_COLOR3_STR,
  STRIPDEF_COLOR_COLOR4_STR,
  STRIPDEF_COLOR_COLOR5_STR,
  STRIPDEF_COLOR_COLOR6_STR,
  STRIPDEF_COLOR_COLOR7_STR,
  STRIPDEF_COLOR_COLOR8_STR,
  STRIPDEF_COLOR_COLOR9_STR,
  STRIPDEF_COLOR_COLOR10_STR
};

/* force the constant variables SCFGMASK_XXX to be initialized */
StripConfigMask SCFGMASK_TIME;
StripConfigMask SCFGMASK_COLOR;
StripConfigMask SCFGMASK_OPTION;
StripConfigMask SCFGMASK_CURVE;
StripConfigMask SCFGMASK_ALL;

void    StripConfig_preinit     (void)
{
  StripConfigMaskElement        elem;

  /* TIME */
  StripConfigMask_clear (&SCFGMASK_TIME);
  for (elem = SCFGMASK_TIME_TIMESPAN;
       elem <= SCFGMASK_TIME_REFRESH_INTERVAL;
       elem++)
    StripConfigMask_set (&SCFGMASK_TIME, elem);

  /* COLOR */
  StripConfigMask_clear (&SCFGMASK_COLOR);
  for (elem = SCFGMASK_COLOR_BACKGROUND;
       elem <= SCFGMASK_COLOR_COLOR10;
       elem++)
    StripConfigMask_set (&SCFGMASK_COLOR, elem);

  /* OPTION */
  StripConfigMask_clear (&SCFGMASK_OPTION);
  for (elem = SCFGMASK_OPTION_GRID_XON;
       elem <= SCFGMASK_OPTION_GRAPH_LINEWIDTH;
       elem++)
    StripConfigMask_set (&SCFGMASK_OPTION, elem);

  /* CURVE */
  StripConfigMask_clear (&SCFGMASK_CURVE);
  for (elem = SCFGMASK_CURVE_NAME;
       elem <= SCFGMASK_CURVE_PLOTSTAT;
       elem++)
    StripConfigMask_set (&SCFGMASK_CURVE, elem);

  /* ALL */
  StripConfigMask_clear (&SCFGMASK_ALL);
  for (elem = SCFGMASK_FIRST_ELEMENT;
       elem <= SCFGMASK_LAST_ELEMENT;
       elem++)
    StripConfigMask_set (&SCFGMASK_ALL, elem);
}

/* ====== Static Function Declarations ====== */
static int              read_oldformat  (StripConfig *,
  FILE *,
  StripConfigMask);

/* StripConfigMask_clear
 *
 */
void    StripConfigMask_clear   (StripConfigMask *pmask)
{
  int   i;

  for (i = 0; i < STRIPCFGMASK_NBYTES; i++)
    pmask->bytes[i] = 0;
}


/* StripConfigMask_test
 *
 *      NOTE: possibly undesirable behavior: all unused bits (extra bits
 *      not corresponding to an attribute value) should be set to 0.
 */
int     StripConfigMask_test    (StripConfigMask *A, StripConfigMask *B)
{
  int   i;

  for (i = 0; i < STRIPCFGMASK_NBYTES; i++)
    if ((A->bytes[i] & B->bytes[i]) != B->bytes[i]) break;

  return (i == STRIPCFGMASK_NBYTES);
}


/* StripConfigMask_and
 *
 */
void    StripConfigMask_and     (StripConfigMask *A, StripConfigMask *B)
{
  int   i;

  for (i = 0; i < STRIPCFGMASK_NBYTES; i++)
    A->bytes[i] &= B->bytes[i];
}


/* StripConfigMask_or
 *
 */
void    StripConfigMask_or      (StripConfigMask *A, StripConfigMask *B)
{
  int   i;

  for (i = 0; i < STRIPCFGMASK_NBYTES; i++)
    A->bytes[i] |= B->bytes[i];
}


/* StripConfigMask_xor
 *
 */
void    StripConfigMask_xor     (StripConfigMask *A, StripConfigMask *B)
{
  int   i;

  for (i = 0; i < STRIPCFGMASK_NBYTES; i++)
    A->bytes[i] ^= B->bytes[i];
}


/* StripConfigMask_intersect
 *
 */
int     StripConfigMask_intersect       (StripConfigMask *A,
  StripConfigMask *B)
{
  int   i;

  for (i = 0; i < STRIPCFGMASK_NBYTES; i++)
    if (A->bytes[i] & B->bytes[i]) break;

  return (i < STRIPCFGMASK_NBYTES);
}



/*
 * StripConfig_init
 *
 */
StripConfig     *StripConfig_init       (cColorManager  scm,
  XVisualInfo            *xvi,
  FILE                   *f,
  StripConfigMask        mask)
{
  StripConfig           *scfg;
  cColor                colors[STRIPCONFIG_NUMCOLORS];
  int                   i, j;
  int                   stat;

  if (!(scfg = (StripConfig *)malloc (sizeof (StripConfig))))
    return 0;

  scfg->scm                     = scm;
  scfg->xvi                     = *xvi;
  scfg->title                   = STRIPDEF_TITLE;
  scfg->filename                = 0;
  
  scfg->Time.timespan           = STRIPDEF_TIME_TIMESPAN;
  scfg->Time.num_samples        = STRIPDEF_TIME_NUM_SAMPLES;
  scfg->Time.sample_interval    = STRIPDEF_TIME_SAMPLE_INTERVAL;
  scfg->Time.refresh_interval   = STRIPDEF_TIME_REFRESH_INTERVAL;

  /* get the default colors */
  cColorManager_build_palette (scfg->scm, 0, CCM_MAX_PALETTE_SIZE);
  for (i = 0; i < STRIPCONFIG_NUMCOLORS; i++)
  {
    /* first try to make a writable color.  If that fails, settle
     * for read-only */
    stat = cColorManager_make_color
      (scfg->scm, &colors[i], StripCfgColorStr[i], CCM_RW_TRY | CCM_MATCH_STR);
    if (!stat)
      stat = cColorManager_make_color
        (scfg->scm, &colors[i], StripCfgColorStr[i], CCM_RO | CCM_MATCH_STR);
    if (!stat)
      fprintf (stderr, "StripConfig_init: unable to make color\n");
    else cColorManager_keep_color (scfg->scm, &colors[i]);
  }
  cColorManager_free_palette (scfg->scm);

  i = 0;
  scfg->Color.background        = colors[i++];
  scfg->Color.foreground        = colors[i++];
  scfg->Color.grid              = colors[i++];
  for (j = 0; j < STRIP_MAX_CURVES; j++)
    scfg->Color.color[j]        = colors[i++];
  
  scfg->Option.grid_xon         = STRIPDEF_OPTION_GRID_XON;
  scfg->Option.grid_yon         = STRIPDEF_OPTION_GRID_YON;
  scfg->Option.axis_ycolorstat  = STRIPDEF_OPTION_AXIS_YCOLORSTAT;
  scfg->Option.graph_linewidth  = STRIPDEF_OPTION_GRAPH_LINEWIDTH;
  
  StripConfigMask_clear (&scfg->UpdateInfo.update_mask);
  
  for (i = 0; i < STRIP_MAX_CURVES; i++)
  {
    sprintf (scfg->Curves.Detail[i].name, "%s%d", STRIPDEF_CURVE_NAME, i);
    scfg->Curves.Detail[i].color = &scfg->Color.color[i];

    StripConfig_reset_details (scfg, &scfg->Curves.Detail[i]);
    scfg->Curves.plot_order[i] = i;
  }
  
  scfg->UpdateInfo.callback_count = 0;
  for (i = 0; i < STRIPCONFIG_MAX_CALLBACKS; i++)
    scfg->UpdateInfo.Callbacks[i].call_func = NULL;
  
  if (f) StripConfig_load (scfg, f, mask);

  return scfg;
}


/* StripConfig_clone
 */
StripConfig     *StripConfig_clone      (StripConfig *original)
{
  StripConfig   *clone;

  if ((clone = (StripConfig *)malloc (sizeof(StripConfig))))
  {
    /* almost everything can be byte copied */
    memcpy (clone, original, sizeof (StripConfig));

    /* a few things can't */
    if (original->title && (original->title != STRIPDEF_TITLE))
      clone->title = strdup (original->title);
    if (original->filename) clone->title = strdup (original->filename);
#if 0
    /* KE: This logic should be checked */
    if (original->scm)
    {
	Display *display = cColorManager_getdisplay(original->scm);
	cColorManager scm = cColorManager_init (display, /*STRIPCONFIG_NUMCOLORS*/ 0);
	clone->scm = scm;
    }
    else
    {
      fprintf (stdout, "StripConfig_clone: cannot initialize color manager.\n");
	clone->scm = NULL;
    }
#endif
  }

#if DEBUG_SCM
  print("StripConfig_clone: original=%x scm=%x\n",original,original->scm);
  print("                   clone=   %x scm=%x\n",clone,clone->scm);
#endif  

  return clone;
}


/*
 * StripConfig_delete
 *
 */
void    StripConfig_delete      (StripConfig *scfg)
{
#if DEBUG_SCM
  print("StripConfig_delete: scfg=%x scm=%x\n",scfg,scfg->scm);
#endif  
  if (scfg->title && (scfg->title != STRIPDEF_TITLE)) free (scfg->title);
  if (scfg->filename) free (scfg->filename);
  if (scfg->scm) cColorManager_delete (scfg->scm);
  free (scfg);
}


/*
 * StripConfig_setattr
 */
int     StripConfig_setattr (StripConfig *scfg, ...)
{
  va_list       ap;
  int           attrib;
  int           ret_val = 1;
  union _tmp
  {
    int         i;
    unsigned    u;
    double      d;
    char        *str;
  } tmp;


  va_start (ap, scfg);
  for (attrib = va_arg (ap, StripConfigAttribute);
       (attrib != 0);
       attrib = va_arg (ap, StripConfigAttribute))
  {
    if ((ret_val = ((attrib >= STRIPCONFIG_FIRST_ATTRIBUTE) &&
	(attrib <= STRIPCONFIG_LAST_ATTRIBUTE))))
      switch (attrib)
      {
	case STRIPCONFIG_TITLE:
	  tmp.str = va_arg (ap, char *);
	  if (scfg->title && scfg->title != STRIPDEF_TITLE)
	    free (scfg->title);
	  if (tmp.str) scfg->title = strdup (tmp.str);
	  else scfg->title = STRIPDEF_TITLE;
	  StripConfigMask_set
	    (&scfg->UpdateInfo.update_mask, SCFGMASK_TITLE);
	  break;
            
	case STRIPCONFIG_FILENAME:
	  tmp.str = va_arg (ap, char *);
	  if (scfg->filename) free (scfg->filename);
	  if (tmp.str) scfg->filename = strdup (tmp.str);
	  else scfg->filename = 0;
	  StripConfigMask_set
	    (&scfg->UpdateInfo.update_mask, SCFGMASK_FILENAME);
	  break;
            
	case STRIPCONFIG_TIME_TIMESPAN:
	  tmp.u = va_arg (ap, unsigned);
	  if (tmp.u != scfg->Time.timespan)
	  {
	    tmp.u = max (tmp.u, STRIPMIN_TIME_TIMESPAN);
	    tmp.u = min (tmp.u, STRIPMAX_TIME_TIMESPAN);
	    scfg->Time.timespan = tmp.u;
	    StripConfigMask_set
		(&scfg->UpdateInfo.update_mask, SCFGMASK_TIME_TIMESPAN);
	  }
	  break;
            
	case STRIPCONFIG_TIME_NUM_SAMPLES:
	  tmp.i = va_arg (ap, int);
	  if (tmp.u != scfg->Time.num_samples)
	  {
	    tmp.i = max (tmp.i, STRIPMIN_TIME_NUM_SAMPLES);
	    tmp.i = min (tmp.i, STRIPMAX_TIME_NUM_SAMPLES);
#if 0	    
	    scfg->Time.num_samples = tmp.i;
#else
	    /* KE: The buffer is never lowered in size.  See:
	     * StripDataSource_setattr */
	    scfg->Time.num_samples = max (tmp.i, scfg->Time.num_samples);
#endif	    
	    StripConfigMask_set
		(&scfg->UpdateInfo.update_mask, SCFGMASK_TIME_NUM_SAMPLES);
	  }
	  break;
            
	case STRIPCONFIG_TIME_SAMPLE_INTERVAL:
	  tmp.d = va_arg (ap, double);
	  if (tmp.d != scfg->Time.sample_interval)
	  {
	    tmp.d = max (tmp.d, STRIPMIN_TIME_SAMPLE_INTERVAL);
	    tmp.d = min (tmp.d, STRIPMAX_TIME_SAMPLE_INTERVAL);
	    scfg->Time.sample_interval = tmp.d;
	    StripConfigMask_set
		(&scfg->UpdateInfo.update_mask,
		  SCFGMASK_TIME_SAMPLE_INTERVAL);
	  }
	  break;
            
	case STRIPCONFIG_TIME_REFRESH_INTERVAL:
	  tmp.d = va_arg (ap, double);
	  if (tmp.d != scfg->Time.refresh_interval)
	  {
	    tmp.d = max (tmp.d, STRIPMIN_TIME_REFRESH_INTERVAL);
	    tmp.d = min (tmp.d, STRIPMAX_TIME_REFRESH_INTERVAL);
	    scfg->Time.refresh_interval = tmp.d;
	    StripConfigMask_set
		(&scfg->UpdateInfo.update_mask,
		  SCFGMASK_TIME_REFRESH_INTERVAL);
	  }
	  break;
            
	case STRIPCONFIG_COLOR_BACKGROUND:
	  scfg->Color.background = *(va_arg (ap, cColor *));
	  StripConfigMask_set
	    (&scfg->UpdateInfo.update_mask,
		SCFGMASK_COLOR_BACKGROUND);
	  break;
	case STRIPCONFIG_COLOR_FOREGROUND:
	  scfg->Color.foreground = *(va_arg (ap, cColor *));
	  StripConfigMask_set
	    (&scfg->UpdateInfo.update_mask,
		SCFGMASK_COLOR_FOREGROUND);
	  break;
	case STRIPCONFIG_COLOR_GRID:
	  scfg->Color.grid = *(va_arg (ap, cColor *));
	  StripConfigMask_set
	    (&scfg->UpdateInfo.update_mask,
		SCFGMASK_COLOR_GRID);
	  break;
	case STRIPCONFIG_COLOR_COLOR1:
	  scfg->Color.color[0] = *(va_arg (ap, cColor *));
	  StripConfigMask_set
	    (&scfg->UpdateInfo.update_mask,
		SCFGMASK_COLOR_COLOR1);
	  break;
	case STRIPCONFIG_COLOR_COLOR2:
	  scfg->Color.color[1] = *(va_arg (ap, cColor *));
	  StripConfigMask_set
	    (&scfg->UpdateInfo.update_mask,
		SCFGMASK_COLOR_COLOR2);
	  break;
	case STRIPCONFIG_COLOR_COLOR3:
	  scfg->Color.color[2] = *(va_arg (ap, cColor *));
	  StripConfigMask_set
	    (&scfg->UpdateInfo.update_mask,
		SCFGMASK_COLOR_COLOR3);
	  break;
	case STRIPCONFIG_COLOR_COLOR4:
	  scfg->Color.color[3] = *(va_arg (ap, cColor *));
	  StripConfigMask_set
	    (&scfg->UpdateInfo.update_mask,
		SCFGMASK_COLOR_COLOR4);
	  break;
	case STRIPCONFIG_COLOR_COLOR5:
	  scfg->Color.color[4] = *(va_arg (ap, cColor *));
	  StripConfigMask_set
	    (&scfg->UpdateInfo.update_mask,
		SCFGMASK_COLOR_COLOR5);
	  break;
	case STRIPCONFIG_COLOR_COLOR6:
	  scfg->Color.color[5] = *(va_arg (ap, cColor *));
	  StripConfigMask_set
	    (&scfg->UpdateInfo.update_mask,
		SCFGMASK_COLOR_COLOR6);
	  break;
	case STRIPCONFIG_COLOR_COLOR7:
	  scfg->Color.color[6] = *(va_arg (ap, cColor *));
	  StripConfigMask_set
	    (&scfg->UpdateInfo.update_mask,
		SCFGMASK_COLOR_COLOR7);
	  break;
	case STRIPCONFIG_COLOR_COLOR8:
	  scfg->Color.color[7] = *(va_arg (ap, cColor *));
	  StripConfigMask_set
	    (&scfg->UpdateInfo.update_mask,
		SCFGMASK_COLOR_COLOR8);
	  break;
	case STRIPCONFIG_COLOR_COLOR9:
	  scfg->Color.color[8] = *(va_arg (ap, cColor *));
	  StripConfigMask_set
	    (&scfg->UpdateInfo.update_mask,
		SCFGMASK_COLOR_COLOR9);
	  break;
	case STRIPCONFIG_COLOR_COLOR10:
	  scfg->Color.color[9] = *(va_arg (ap, cColor *));
	  StripConfigMask_set
	    (&scfg->UpdateInfo.update_mask,
		SCFGMASK_COLOR_COLOR10);
	  break;
            
	case STRIPCONFIG_OPTION_GRID_XON:
	  tmp.i = va_arg (ap, int);
	  if (tmp.i != scfg->Option.grid_xon)
	  {
	    tmp.i = max (tmp.i, STRIPMIN_OPTION_GRID_XON);
	    tmp.i = min (tmp.i, STRIPMAX_OPTION_GRID_XON);
	    scfg->Option.grid_xon = tmp.i;
	    StripConfigMask_set
		(&scfg->UpdateInfo.update_mask, SCFGMASK_OPTION_GRID_XON);
	  }
	  break;
            
	case STRIPCONFIG_OPTION_GRID_YON:
	  tmp.i = va_arg (ap, int);
	  if (tmp.i != scfg->Option.grid_yon)
	  {
	    tmp.i = max (tmp.i, STRIPMIN_OPTION_GRID_YON);
	    tmp.i = min (tmp.i, STRIPMAX_OPTION_GRID_YON);
	    scfg->Option.grid_yon = tmp.i;
	    StripConfigMask_set
		(&scfg->UpdateInfo.update_mask, SCFGMASK_OPTION_GRID_YON);
	  }
	  break;
            
	case STRIPCONFIG_OPTION_AXIS_YCOLORSTAT:
	  tmp.i = va_arg (ap, int);
	  if (tmp.i != scfg->Option.axis_ycolorstat)
	  {
	    scfg->Option.axis_ycolorstat = tmp.i;
	    StripConfigMask_set
		(&scfg->UpdateInfo.update_mask,
		  SCFGMASK_OPTION_AXIS_YCOLORSTAT);
	  }
	  break;
            
	case STRIPCONFIG_OPTION_GRAPH_LINEWIDTH:
	  tmp.i = va_arg (ap, int);
	  if (tmp.i != scfg->Option.graph_linewidth)
	  {
	    tmp.i = max (tmp.i, STRIPMIN_OPTION_GRAPH_LINEWIDTH);
	    tmp.i = min (tmp.i, STRIPMAX_OPTION_GRAPH_LINEWIDTH);
	    scfg->Option.graph_linewidth = tmp.i;
	    StripConfigMask_set
		(&scfg->UpdateInfo.update_mask,
		  SCFGMASK_OPTION_GRAPH_LINEWIDTH);
	  }
	  break;
            
	default:
	  fprintf
	    (stderr,
		"StripConfig_setattr: cannot set read-only value, %d\n",
		attrib);
	  ret_val = 0;
	  goto done;
      }
    else break;
  }

  done:
  va_end (ap);
  return ret_val;
}


/*
 * StripConfig_getattr
 */
int     StripConfig_getattr (StripConfig *scfg, ...)
{
  va_list       ap;
  int           attrib;
  int           ret_val = 1;

  va_start (ap, scfg);
  for (attrib = va_arg (ap, StripConfigAttribute);
       (attrib != 0);
       attrib = va_arg (ap, StripConfigAttribute))
  {
    if ((ret_val = ((attrib >= STRIPCONFIG_FIRST_ATTRIBUTE) &&
	(attrib <= STRIPCONFIG_LAST_ATTRIBUTE))))
      switch (attrib)
      {
	case STRIPCONFIG_TITLE:
	  *(va_arg (ap, char **)) = scfg->title;
	  break;
	case STRIPCONFIG_FILENAME:
	  *(va_arg (ap, char **)) = scfg->filename;
	  break;
	case STRIPCONFIG_TIME_TIMESPAN:
	  *(va_arg (ap, unsigned *)) = scfg->Time.timespan;
	  break;
	case STRIPCONFIG_TIME_NUM_SAMPLES:
	  *(va_arg (ap, int *)) = scfg->Time.num_samples;
	  break;
	case STRIPCONFIG_TIME_SAMPLE_INTERVAL:
	  *(va_arg (ap, double *)) = scfg->Time.sample_interval;
	  break;
	case STRIPCONFIG_TIME_REFRESH_INTERVAL:
	  *(va_arg (ap, double *)) = scfg->Time.refresh_interval;
	  break;
	case STRIPCONFIG_COLOR_BACKGROUND:
	  *(va_arg (ap, cColor **)) = &scfg->Color.background;
	  break;
	case STRIPCONFIG_COLOR_FOREGROUND:
	  *(va_arg (ap, cColor **)) = &scfg->Color.foreground;
	  break;
	case STRIPCONFIG_COLOR_GRID:
	  *(va_arg (ap, cColor **)) = &scfg->Color.grid;
	  break;
	case STRIPCONFIG_COLOR_COLOR1:
	  *(va_arg (ap, cColor **)) = &scfg->Color.color[0];
	  break;
	case STRIPCONFIG_COLOR_COLOR2:
	  *(va_arg (ap, cColor **)) = &scfg->Color.color[1];
	  break;
	case STRIPCONFIG_COLOR_COLOR3:
	  *(va_arg (ap, cColor **)) = &scfg->Color.color[2];
	  break;
	case STRIPCONFIG_COLOR_COLOR4:
	  *(va_arg (ap, cColor **)) = &scfg->Color.color[3];
	  break;
	case STRIPCONFIG_COLOR_COLOR5:
	  *(va_arg (ap, cColor **)) = &scfg->Color.color[4];
	  break;
	case STRIPCONFIG_COLOR_COLOR6:
	  *(va_arg (ap, cColor **)) = &scfg->Color.color[5];
	  break;
	case STRIPCONFIG_COLOR_COLOR7:
	  *(va_arg (ap, cColor **)) = &scfg->Color.color[6];
	  break;
	case STRIPCONFIG_COLOR_COLOR8:
	  *(va_arg (ap, cColor **)) = &scfg->Color.color[7];
	  break;
	case STRIPCONFIG_COLOR_COLOR9:
	  *(va_arg (ap, cColor **)) = &scfg->Color.color[8];
	  break;
	case STRIPCONFIG_COLOR_COLOR10:
	  *(va_arg (ap, cColor **)) = &scfg->Color.color[9];
	  break;
	case STRIPCONFIG_OPTION_GRID_XON:
	  *(va_arg (ap, int *)) = scfg->Option.grid_xon;
	  break;
	case STRIPCONFIG_OPTION_GRID_YON:
	  *(va_arg (ap, int *)) = scfg->Option.grid_yon;
	  break;
	case STRIPCONFIG_OPTION_AXIS_YCOLORSTAT:
	  *(va_arg (ap, int *)) = scfg->Option.axis_ycolorstat;
	  break;
	case STRIPCONFIG_OPTION_GRAPH_LINEWIDTH:
	  *(va_arg (ap, int *)) = scfg->Option.graph_linewidth;
	  break;
      }
    else break;
  }

  va_end (ap);
  return ret_val;
}


/*
 * StripConfig_write
 *
 */
int     StripConfig_write       (StripConfig            *scfg,
  FILE                   *f,
  StripConfigMask        mask)
{
  StripConfigMaskElement        elem;
  int                           i, j = 0;
  char                          cbuf[BUFSIZE], fbuf[BUFSIZE], num_buf[BUFSIZE];
  char                          *p;
  cColor                        *pcolor;

  fprintf
    (f,
	"%-*s%d.%d\n",
	LEFT_COLUMNWIDTH,
	STRIPCONFIG_HEADER_STR,
	STRIPCONFIG_MAJOR_VERSION,
	STRIPCONFIG_MINOR_VERSION);

  for (i = 0; i < SCFGMASK_NUM_ELEMENTS; i++)
  {
    elem = SCFGMASK_FIRST_ELEMENT + i;

    if (StripConfigMask_stat (&mask, elem))
    {
      p = cbuf;
      sprintf (p, "%s", SCFTokenStr[STRIP]);
      p += strlen (p);

      /* Time */
      if (StripConfigMask_stat (&SCFGMASK_TIME, elem))
      {
        sprintf
          (p, "%s%s%s%s",
		SCFTokenStr[SEPARATOR], SCFTokenStr[TIME],
		SCFTokenStr[SEPARATOR], SCFTokenStr[i]);
        sprintf
          (fbuf, "%-*s", LEFT_COLUMNWIDTH, cbuf);

        switch (elem)
        {
	  case SCFGMASK_TIME_TIMESPAN:
	    fprintf (f, "%s%u\n", fbuf, scfg->Time.timespan);
	    break;
	  case SCFGMASK_TIME_NUM_SAMPLES:
	    fprintf (f, "%s%d\n", fbuf, scfg->Time.num_samples);
	    break;
	  case SCFGMASK_TIME_SAMPLE_INTERVAL:
	    fprintf (f, "%s%f\n", fbuf, scfg->Time.sample_interval);
	    break;
	  case SCFGMASK_TIME_REFRESH_INTERVAL:
	    fprintf (f, "%s%f\n", fbuf, scfg->Time.refresh_interval);
	    break;
        }
      }

      /* Color */
      else if (StripConfigMask_stat (&SCFGMASK_COLOR, elem))
      {
        sprintf
          (p, "%s%s%s%s",
		SCFTokenStr[SEPARATOR], SCFTokenStr[COLOR],
		SCFTokenStr[SEPARATOR], SCFTokenStr[i]);
        sprintf
          (fbuf, "%-*s", LEFT_COLUMNWIDTH, cbuf);

        /* remember that n = (1 << i), that i is a SFCToken,
         * and that we have a mapping from token -> attribute */
        StripConfig_getattr (scfg, (StripConfigAttribute)i+1, &pcolor, 0);
        fprintf
          (f, "%s%-*hu%-*hu%-*hu\n",
		fbuf,
		NUMERIC_COLUMNWIDTH, pcolor->xcolor.red,
		NUMERIC_COLUMNWIDTH, pcolor->xcolor.green,
		NUMERIC_COLUMNWIDTH, pcolor->xcolor.blue);
      }

      /* Option */
      else if (StripConfigMask_stat (&SCFGMASK_OPTION, elem))
      {
        sprintf
          (p, "%s%s%s%s",
		SCFTokenStr[SEPARATOR], SCFTokenStr[OPTION],
		SCFTokenStr[SEPARATOR], SCFTokenStr[i]);
        sprintf
          (fbuf, "%-*s", LEFT_COLUMNWIDTH, cbuf);

        switch (elem)
        {
	  case SCFGMASK_OPTION_GRID_XON:
	    j = scfg->Option.grid_xon;
	    break;
	  case SCFGMASK_OPTION_GRID_YON:
	    j = scfg->Option.grid_yon;
	    break;
	  case SCFGMASK_OPTION_AXIS_YCOLORSTAT:
	    j = scfg->Option.axis_ycolorstat;
	    break;
	  case SCFGMASK_OPTION_GRAPH_LINEWIDTH:
	    j = scfg->Option.graph_linewidth;
	    break;
        }
        fprintf (f, "%s%d\n", fbuf, j);
      }

      /* Curves */
      else if (StripConfigMask_stat (&SCFGMASK_CURVE, elem))
      {
        sprintf
          (p, "%s%s",
		SCFTokenStr[SEPARATOR],
		SCFTokenStr[CURVE]);

        for (j = 0; j < STRIP_MAX_CURVES; j++)
        {
          if (scfg->Curves.Detail[j].id == NULL)
            continue;
          sprintf
            (fbuf, "%s%s%d%s%s",
		  cbuf, SCFTokenStr[SEPARATOR],
		  j, SCFTokenStr[SEPARATOR],
		  SCFTokenStr[i]);
          switch (elem)
          {
	    case SCFGMASK_CURVE_NAME:
		fprintf
		  (f, "%-*s%s\n",
		    LEFT_COLUMNWIDTH, fbuf,
		    scfg->Curves.Detail[j].name);
		break;
	    case SCFGMASK_CURVE_EGU:
		if (StripConfigMask_stat
		  (&scfg->Curves.Detail[j].set_mask, SCFGMASK_CURVE_EGU))
		  fprintf
		    (f, "%-*s%s\n",
			LEFT_COLUMNWIDTH, fbuf, scfg->Curves.Detail[j].egu);
		break;
	    case SCFGMASK_CURVE_COMMENT:
		if (StripConfigMask_stat
		  (&scfg->Curves.Detail[j].set_mask, SCFGMASK_CURVE_COMMENT))
		  fprintf
		    (f, "%-*s%s\n",
			LEFT_COLUMNWIDTH, fbuf, scfg->Curves.Detail[j].comment);
		break;
	    case SCFGMASK_CURVE_PRECISION:
		if (StripConfigMask_stat
		  (&scfg->Curves.Detail[j].set_mask, SCFGMASK_CURVE_PRECISION))
		  fprintf
		    (f, "%-*s%d\n",
			LEFT_COLUMNWIDTH, fbuf, scfg->Curves.Detail[j].precision);
		break;
	    case SCFGMASK_CURVE_MIN:
		if (StripConfigMask_stat
		  (&scfg->Curves.Detail[j].set_mask, SCFGMASK_CURVE_MIN))
		{
		  dbl2str
		    (scfg->Curves.Detail[j].min,
			scfg->Curves.Detail[j].precision,
			num_buf, 31);
		  fprintf (f, "%-*s%s\n", LEFT_COLUMNWIDTH, fbuf, num_buf);
		}
		break;
	    case SCFGMASK_CURVE_MAX:
		if (StripConfigMask_stat
		  (&scfg->Curves.Detail[j].set_mask, SCFGMASK_CURVE_MAX))
		{
		  dbl2str
		    (scfg->Curves.Detail[j].max,
			scfg->Curves.Detail[j].precision,
			num_buf, 31);
		  fprintf (f, "%-*s%s\n", LEFT_COLUMNWIDTH, fbuf, num_buf);
		  break;
		}
	    case SCFGMASK_CURVE_SCALE:
		fprintf
		  (f, "%-*s%d\n",
		    LEFT_COLUMNWIDTH, fbuf,
		    scfg->Curves.Detail[j].scale);
		break;
	    case SCFGMASK_CURVE_PLOTSTAT:
		fprintf
		  (f, "%-*s%d\n",
		    LEFT_COLUMNWIDTH, fbuf,
		    scfg->Curves.Detail[j].plotstat);
		break;
          }
        }
      }
    }
  }

  return 1;
}


/*
 * StripConfig_load
 *
 */
int     StripConfig_load        (StripConfig            *scfg,
  FILE                   *f,
  StripConfigMask        mask)
{
  StripConfig                   *clone;
  char                          fbuf[BUFSIZE], ebuf[BUFSIZE];
  char                          *p, *ptok, *pval, *ptmp;
  int                           i;
  int                           token, token_min = 0, token_max = 0;
  StripConfigMaskElement        elem;
  int                           curve_idx;
  cColor                        *pcolor, color;
  long                          foffset;
  int                           ret;
  int                           minor_version, major_version;
  int                           old = 0, error = 0;
  union _tmp {
    unsigned    u;
    int         i;
    double      d;
    char        buf[128];
  } tmp;

  if (!(clone = StripConfig_clone (scfg))) return 0;

  /* first line */
  foffset = ftell (f);
  if (!fgets (fbuf, BUFSIZE, f)) {
    error = 1;
    goto finish;
  }

  /* check header */
  if (sscanf(fbuf, "%s %d.%d", ebuf, &major_version, &minor_version) != 3)
  {
    old = 1;
    fseek (f, foffset, SEEK_SET);
    error = !read_oldformat (clone, f, mask);
  }
  else if (strcmp (ebuf, STRIPCONFIG_HEADER_STR) != 0)
  {
    old = 1;
    fseek (f, foffset, SEEK_SET);
    error = !read_oldformat (clone, f, mask);
  }
  else if ((major_version > STRIPCONFIG_MAJOR_VERSION) ||
    ((major_version == STRIPCONFIG_MAJOR_VERSION) &&
	(minor_version > STRIPCONFIG_MINOR_VERSION)))
  {
    fprintf
      (stderr,
	  "StripConfig_load(): can't read configuration file version %d.%d\n"
	  "                    I only know about version %d.%d and lower!\n",
	  major_version, minor_version,
	  STRIPCONFIG_MAJOR_VERSION, STRIPCONFIG_MINOR_VERSION);
  }
  else while (fgets (fbuf, BUFSIZE, f))
  {
    strcpy (ebuf, fbuf);
      
    /* first split the input line into the attribute token and the
     * attribute value */
      
    p = ptok = fbuf;
    while (!isspace ((int)*p)) p++;
    *p = 0;

    p++;
    while (isspace ((int)*p)) p++;
    pval = p;

    /* now parse the token string to get a token value */
    if ((ret = ((p = strtok (ptok, SCFTokenStr[SEPARATOR])) != NULL)))
      ret = (strcmp (p, SCFTokenStr[STRIP]) == 0);
    if (!ret) continue;         /* ignore unknown token */

    if ((ret = ((p = strtok (NULL, SCFTokenStr[SEPARATOR])) != NULL)))
    {
      for (token = TIME; token <= CURVE; token++)
        if ((ret = (strcmp (p, SCFTokenStr[token]) == 0)))
          break;
    }
    if (!ret) continue;         /* ignore unknown token */

    switch (token)
    {
    case TIME:
	token_min = TIMESPAN;
	token_max = REFRESH_INTERVAL;
	break;
    case COLOR:
	token_min = BACKGROUND;
	token_max = COLOR10;
	break;
    case OPTION:
	token_min = GRID_XON;
	token_max = GRAPH_LINEWIDTH;
	break;
    case CURVE:
	token_min = NAME;
	token_max = PLOTSTAT;
	/* must read the curve index */
	if ((ret = ((p = strtok (NULL, SCFTokenStr[SEPARATOR])) != NULL)))
	  if ((ret = sscanf (p, "%d", &curve_idx) == 1))
	    ret = (curve_idx >= 0) && (curve_idx <= STRIP_MAX_CURVES);
	if (!ret) {
	  fprintf (stderr, "StripConfig_load: bad curve index, \"%s\"\n", p);
	  fprintf (stderr, "==> %s\n", ebuf);
	  error = 1;
	  goto finish;
	}
	break;
    }

    /* the next token should be the the last in the attribute so the
     * separator will be whitespace */
    if ((ret = ((p = strtok (NULL, " \t")) != NULL)))
    {
      for (token = token_min; token <= token_max; token++)
        if ((ret = (strcmp (p, SCFTokenStr[token]) == 0)))
          break;
    }

    if (!ret)
    {
      fprintf
        (stderr, "StripConfig_load: unknown or obsolete attribute (ignore)\n");
      fprintf (stderr, "==> %s\n", ebuf);
      continue;
    }

    /* If here, then the attribute token is valid and now known.  So now
     * load the value if the attribute is not masked off */
    if (!StripConfigMask_stat (&mask, SCFGMASK_FIRST_ELEMENT + token))
      continue;
      
    switch (token)
    {
    case TIMESPAN:
	if ((ret = (sscanf (pval, "%ud", &tmp.u) == 1)))
	  StripConfig_setattr (clone, STRIPCONFIG_TIME_TIMESPAN, tmp.u, 0);
	break;
          
    case NUM_SAMPLES:
	if ((ret = (sscanf (pval, "%d", &tmp.i) == 1)))
	  StripConfig_setattr
	    (clone, STRIPCONFIG_TIME_NUM_SAMPLES, tmp.i, 0);
	break;
          
    case SAMPLE_INTERVAL:
	if ((ret = (sscanf (pval, "%lf", &tmp.d) == 1)))
	  StripConfig_setattr
	    (clone, STRIPCONFIG_TIME_SAMPLE_INTERVAL, tmp.d, 0);
	break;
          
    case REFRESH_INTERVAL:
	if ((ret = (sscanf (pval, "%lf", &tmp.d) == 1)))
	  StripConfig_setattr
	    (clone, STRIPCONFIG_TIME_REFRESH_INTERVAL, tmp.d, 0);
	break;
          
    case BACKGROUND:
    case FOREGROUND:
    case GRID:
    case COLOR1:
    case COLOR2:
    case COLOR3:
    case COLOR4:
    case COLOR5:
    case COLOR6:
    case COLOR7:
    case COLOR8:
    case COLOR9:
    case COLOR10:
	StripConfig_getattr
	  (clone, (StripConfigAttribute)token+1, &pcolor, 0);
	ret =
	  (sscanf
	    (pval, "%hu%hu%hu",
		&pcolor->xcolor.red, &pcolor->xcolor.green, &pcolor->xcolor.blue)
	    == 3);
	break;
          
    case GRID_XON:
	ret = (sscanf (pval, "%d", &clone->Option.grid_xon) == 1);
	if ((clone->Option.grid_xon < STRIPGRID_NONE) ||
	  (clone->Option.grid_xon > STRIPGRID_ALL))
	  clone->Option.grid_xon = STRIPGRID_SOME;
	break;
          
    case GRID_YON:
	ret = (sscanf (pval, "%d", &clone->Option.grid_yon) == 1);
	if ((clone->Option.grid_yon < STRIPGRID_NONE) ||
	  (clone->Option.grid_yon > STRIPGRID_ALL))
	  clone->Option.grid_yon = STRIPGRID_SOME;
	break;
          
    case AXIS_YCOLORSTAT:
	ret = (sscanf (pval, "%d", &clone->Option.axis_ycolorstat) == 1);
	break;
          
    case GRAPH_LINEWIDTH:
	ret = (sscanf (pval, "%d", &clone->Option.graph_linewidth) == 1);
	break;
          
    case NAME:
	ret =
	  (sscanf (pval, "%s", clone->Curves.Detail[curve_idx].name) == 1);
	break;
          
    case EGU:
	/* may be empty */
	clone->Curves.Detail[curve_idx].egu[0] = 0;
	sscanf (pval, "%s", clone->Curves.Detail[curve_idx].egu);
	break;
          
    case COMMENT:
	ptmp = clone->Curves.Detail[curve_idx].comment;
	*ptmp = 0;
	for (i = 0; i < STRIP_MAX_COMMENT_CHAR; i++)
	  if (*pval) *ptmp++ = *pval++;
	  else break;

	/* remove trailing whitespace */
	while (ptmp > clone->Curves.Detail[curve_idx].comment)
	{
	  ptmp--;
	  if (isspace ((int)*ptmp)) *ptmp = 0;
	  else break;
	}
	ret = (ptmp >= clone->Curves.Detail[curve_idx].comment);
	break;
          
    case PRECISION:
	ret = (sscanf (pval, "%d", &tmp.i) == 1);
	if (ret)
	{
	  clone->Curves.Detail[curve_idx].precision =
	    max (tmp.i, STRIPMIN_CURVE_PRECISION);
	  clone->Curves.Detail[curve_idx].precision =
	    min (tmp.i, STRIPMAX_CURVE_PRECISION);
	}
	break;
          
    case MIN:
	ret =
	  (sscanf (pval, "%lf", &clone->Curves.Detail[curve_idx].min) == 1);
	break;
          
    case MAX:
	ret =
	  (sscanf (pval, "%lf", &clone->Curves.Detail[curve_idx].max) == 1);
	break;

    case SCALE:
	ret =
	  (sscanf (pval, "%d", &clone->Curves.Detail[curve_idx].scale) == 1);
	if (ret)
	  ret =
	    (clone->Curves.Detail[curve_idx].scale == STRIPSCALE_LINEAR ||
		clone->Curves.Detail[curve_idx].scale == STRIPSCALE_LOG_10);
	break;

    case PLOTSTAT:
	ret =
	  (sscanf (pval, "%d", &clone->Curves.Detail[curve_idx].plotstat)
	    == 1);
	break;
          
    default:
	fprintf
	  (stderr,
	    "StripConfig_load: unknown or obsolete attribute (ignore)\n");
	fprintf (stderr, "==> %s\n", pval);
	continue;
    }

    if (ret)
    {
      elem = SCFGMASK_FIRST_ELEMENT + token;
      StripConfigMask_set (&clone->UpdateInfo.update_mask, elem);

      if (StripConfigMask_stat (&SCFGMASK_CURVE, elem))
      {
        StripConfigMask_set
          (&clone->Curves.Detail[curve_idx].update_mask, elem);
        StripConfigMask_set
          (&clone->Curves.Detail[curve_idx].set_mask, elem);
      }
    }
    else
    {
      fprintf (stderr, "StripConfig_load: bad value specification\n");
      fprintf (stderr, "==> %s\n", ebuf);
      error = 1;
      goto finish;
    }
  }
  
  finish:

  /* if we have some good config info, then copy it into the current
   * config.  Update colors if necessary (if any new colors were
   * loaded, then the rgb values will reflect the desired value,
   * while the pixel will specify the previous value). */
  if (!error)
  {
    /* if NumSamples wasn't specified, then set it to be the time span
     * divided by the sample interval */
    if (!StripConfigMask_stat
	(&clone->UpdateInfo.update_mask, STRIPCONFIG_TIME_NUM_SAMPLES))
    {
      tmp.d = ceil((double)clone->Time.timespan / clone->Time.sample_interval);
      clone->Time.num_samples = (int)tmp.d;
      StripConfigMask_set
        (&clone->UpdateInfo.update_mask, STRIPCONFIG_TIME_NUM_SAMPLES);
    }

    /* update colors if necessary */
    if (!old &&
	StripConfigMask_intersect (&SCFGMASK_COLOR, &mask) &&
	StripConfigMask_intersect
	(&SCFGMASK_COLOR, &clone->UpdateInfo.update_mask))
    {
      cColorManager_build_palette (scfg->scm, 0, CCM_MAX_PALETTE_SIZE);

      for (i = STRIPCONFIG_COLOR_BACKGROUND;
           i <= STRIPCONFIG_COLOR_COLOR10;
           i++)
      {
        StripConfig_getattr (scfg, (StripConfigAttribute)i, &pcolor, 0);
        color = *pcolor;
        StripConfig_getattr (clone, (StripConfigAttribute)i, &pcolor, 0);

        if (color.writable)
        {
          /* if unable to update color, revert to old */
          if (!cColorManager_change_color (clone->scm, pcolor))
            *pcolor = color;
        }
        else
        {
          ret = cColorManager_make_color
            (clone->scm, pcolor, 0, CCM_RW_TRY | CCM_MATCH_RGB);
          if (!ret)
            ret = cColorManager_make_color
              (clone->scm, pcolor, 0, CCM_RO | CCM_MATCH_RGB);

          if (ret)
          {
            cColorManager_keep_color (clone->scm, pcolor);
            cColorManager_free_color (clone->scm, &color);
          }
          else *pcolor = color;
        }
      }
      
      cColorManager_free_palette (scfg->scm);
    }
    
    /* copy */
    if (scfg->title && (scfg->title != STRIPDEF_TITLE)) free (scfg->title);
    if (scfg->filename) free (scfg->filename);
    memcpy (scfg, clone, sizeof (StripConfig));
    if (clone->title && (scfg->title != STRIPDEF_TITLE))
      scfg->title = strdup (clone->title);
    if (clone->filename) scfg->filename = strdup (clone->filename);
  }

  /* set the scm to NULL in the clone so it won't be freed (the scfg
     is using it) */
  clone->scm = NULL;
  StripConfig_delete (clone);
  return !error;
}


/*
 * StripConfig_addcallback
 *
 */
int     StripConfig_addcallback (StripConfig            *scfg,
  StripConfigMask        mask,
  StripConfigUpdateFunc  func,
  void                   *data)
{
  int   i;
  int   ret;

  if ((ret = (scfg->UpdateInfo.callback_count < STRIPCONFIG_MAX_CALLBACKS)))
  {
    i = scfg->UpdateInfo.callback_count;
      
    scfg->UpdateInfo.Callbacks[i].call_func = func;
    scfg->UpdateInfo.Callbacks[i].call_data = data;
    scfg->UpdateInfo.Callbacks[i].call_event = mask;

    scfg->UpdateInfo.callback_count++;
  }
  
  return ret;
}


/*
 * StripConfig_update
 *
 */
void    StripConfig_update      (StripConfig *scfg, StripConfigMask mask)
{
  int                   i;
  StripConfigMask       tmp_mask;

  /* does the specified mask intersect with the mask of currently
   * pending updates? */
  if (StripConfigMask_intersect (&scfg->UpdateInfo.update_mask, &mask))
  {
    /* for each registered callback, if its trigger mask
     * intersects with the passed-in mask, invoke it */
    for (i = 0; i < scfg->UpdateInfo.callback_count; i++)
      if (scfg->UpdateInfo.Callbacks[i].call_func != NULL)
      {
        if (StripConfigMask_intersect
	    (&mask, &scfg->UpdateInfo.Callbacks[i].call_event))
        {
          tmp_mask = scfg->UpdateInfo.Callbacks[i].call_event;
          StripConfigMask_and (&tmp_mask, &mask);
          
          scfg->UpdateInfo.Callbacks[i].call_func
            (tmp_mask, scfg->UpdateInfo.Callbacks[i].call_data);
        }
      }

    /* clear the pending status for the masked updates just
     * handled */
    StripConfigMask_or (&scfg->UpdateInfo.update_mask, &mask);
    StripConfigMask_xor (&scfg->UpdateInfo.update_mask, &mask);
    for (i = 0; i < STRIP_MAX_CURVES; i++)
    {
      /* clear the update_mask bits for each curve by xor'ing that
       * variable with the current mask.  In order to make sure that
       * no stray bits get set on (e.g. if one of the curves didn't
       * have one of the bits in mask turned on, then the xor operation
       * would turn it on), first or the mask with the curve's update
       * mask, then xor.  This is equivalent to anding ~mask */
      StripConfigMask_or (&scfg->Curves.Detail[i].update_mask, &mask);
      StripConfigMask_xor (&scfg->Curves.Detail[i].update_mask, &mask);
    }
  }
}


/* ====== Static Functions ====== */
static int      read_oldformat  (StripConfig            *scfg,
  FILE                   *f,
  StripConfigMask        mask)
{
  char          fbuf[BUFSIZE];
  char          *pattr, *pval;
  int           curve_idx = -1;
  union         _tmp
  {
    unsigned    u;
    double      d;
    char        buf[BUFSIZE];
  } tmp;
  int           ret = 0;
  
  while (fgets (fbuf, BUFSIZE, f) != NULL)
  {
    if ((pattr = strtok (fbuf, " \t")) != NULL)
      if ((pval = strtok (NULL, " \t")) != NULL)
      {
        if (strcmp (pattr, "SAMPLEFREQUENCY") == 0)
        {
          if (StripConfigMask_stat (&mask, SCFGMASK_TIME_SAMPLE_INTERVAL))
            if (sscanf (pval, "%lf", &tmp.d) == 1)
            {
              StripConfig_setattr
                (scfg, STRIPCONFIG_TIME_SAMPLE_INTERVAL, tmp.d, 0);
              StripConfigMask_set
                (&scfg->UpdateInfo.update_mask,
			SCFGMASK_TIME_SAMPLE_INTERVAL);
                      
              /* make sample frequency default update frequency */
              StripConfig_setattr
                (scfg, STRIPCONFIG_TIME_REFRESH_INTERVAL, tmp.d, 0);
              StripConfigMask_set
                (&scfg->UpdateInfo.update_mask,
			SCFGMASK_TIME_REFRESH_INTERVAL);
                      
              ret = 1;
            }
        }
            
        else if (strcmp (pattr, "TIMESPAN") == 0)
        {
          if (StripConfigMask_stat (&mask, SCFGMASK_TIME_TIMESPAN))
            if (sscanf (pval, "%ud", &tmp.u) == 1)
            {
              StripConfig_setattr
                (scfg, STRIPCONFIG_TIME_TIMESPAN, tmp.u, 0);
              StripConfigMask_set
                (&scfg->UpdateInfo.update_mask, SCFGMASK_TIME_TIMESPAN);
              ret = 1;
            }
        }

        else if (strcmp (pattr, "CHANNEL") == 0)
        {
          if (StripConfigMask_stat (&mask, SCFGMASK_CURVE_NAME))
            if (sscanf (pval, "%s", tmp.buf) == 1)
            {
              curve_idx++;
              strcpy (scfg->Curves.Detail[curve_idx].name, tmp.buf);
              StripConfigMask_set
                (&scfg->UpdateInfo.update_mask, SCFGMASK_CURVE_NAME);
              StripConfigMask_set
		    (&scfg->Curves.Detail[curve_idx].update_mask,
			SCFGMASK_CURVE_NAME);
              StripConfigMask_set
		    (&scfg->Curves.Detail[curve_idx].set_mask,
			SCFGMASK_CURVE_NAME);                 
              ret = 1;
            }
        }

        else if (strcmp (pattr, "MAXIMUM") == 0)
        {
          if (StripConfigMask_stat (&mask, SCFGMASK_CURVE_MAX))
            if ((curve_idx >= 0) && (curve_idx < STRIP_MAX_CURVES))
            {
              if (sscanf (pval, "%lf", &tmp.d) == 1)
              {
                scfg->Curves.Detail[curve_idx].max = tmp.d;
                StripConfigMask_set
                  (&scfg->UpdateInfo.update_mask, SCFGMASK_CURVE_MAX);
                StripConfigMask_set
                  (&scfg->Curves.Detail[curve_idx].update_mask,
			  SCFGMASK_CURVE_MAX);
                StripConfigMask_set
                  (&scfg->Curves.Detail[curve_idx].set_mask,
			  SCFGMASK_CURVE_MAX);               
                ret = 1;
              }
            }
        }

        else if (strcmp (pattr, "MINIMUM") == 0)
        {
          if (StripConfigMask_stat (&mask, SCFGMASK_CURVE_MIN))
            if ((curve_idx >= 0) && (curve_idx < STRIP_MAX_CURVES))
            {
              if (sscanf (pval, "%lf", &tmp.d) == 1)
              {
                scfg->Curves.Detail[curve_idx].min = tmp.d;
                StripConfigMask_set
                  (&scfg->UpdateInfo.update_mask, SCFGMASK_CURVE_MIN);
                StripConfigMask_set
                  (&scfg->Curves.Detail[curve_idx].update_mask,
			  SCFGMASK_CURVE_MIN);
                StripConfigMask_set
                  (&scfg->Curves.Detail[curve_idx].set_mask,
			  SCFGMASK_CURVE_MIN);               
                ret = 1;
              }
            }
        }
      }
  }

  return ret;
}


/*
 * StripConfig_reset_details
 */
void    StripConfig_reset_details       (StripConfig *scfg,
  StripCurveDetail *detail)
{
  strcpy (detail->egu, STRIPDEF_CURVE_EGU);
  strcpy (detail->comment, STRIPDEF_CURVE_COMMENT);
  detail->precision     = STRIPDEF_CURVE_PRECISION;
  detail->min           = STRIPDEF_CURVE_MIN;
  detail->max           = STRIPDEF_CURVE_MAX;
  detail->scale         = STRIPDEF_CURVE_SCALE;
  detail->plotstat      = STRIPDEF_CURVE_PLOTSTAT;
  detail->id            = STRIPDEF_CURVE_ID;
  
  StripConfigMask_clear (&detail->update_mask);
  StripConfigMask_clear (&detail->set_mask);
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
