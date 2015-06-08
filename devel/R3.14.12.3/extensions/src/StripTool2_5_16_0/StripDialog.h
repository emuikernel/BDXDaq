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

#ifndef _StripDialog
#define _StripDialog

#include <X11/Intrinsic.h>
#include <stdarg.h>

#include "StripCurve.h"


/* ======= Data Types ======= */
typedef void *  StripDialog;
typedef void    (*SDcallback)   (void *, void *);       /* client, call data */

typedef struct  _SDWindowMenuItem
{
  char          name[64];
  void          *window_id;     /* passed as call data to callback func */
  SDcallback    cb_func;
  void          *cb_data;       /* passed as client data to callback func */
}
SDWindowMenuItem;



/* ======= Attributes ======= */
typedef enum
{
  STRIPDIALOG_SHELL_WIDGET = 1, /* (Widget)                             r- */
  STRIPDIALOG_CONNECT_FUNC,     /* (SDcallback)                         rw */
  STRIPDIALOG_CONNECT_DATA,     /* (void *)                             rw */
  STRIPDIALOG_SHOW_FUNC,        /* (SDcallback)                         rw */
  STRIPDIALOG_SHOW_DATA,        /* (void *)                             rw */
  STRIPDIALOG_CLEAR_FUNC,       /* (SDcallback)                         rw */
  STRIPDIALOG_CLEAR_DATA,       /* (void *)                             rw */
  STRIPDIALOG_DELETE_FUNC,      /* (SDcallback)                         rw */
  STRIPDIALOG_DELETE_DATA,      /* (void *)                             rw */
  STRIPDIALOG_DISMISS_FUNC,     /* (SDcallback)                         rw */
  STRIPDIALOG_DISMISS_DATA,     /* (void *)                             rw */
  STRIPDIALOG_QUIT_FUNC,        /* (SDcallback)                         rw */
  STRIPDIALOG_QUIT_DATA,        /* (void *)                             rw */
  STRIPDIALOG_WINDOW_MENU,      /* (SDWindowMenuItem[])                 r-
                                 * --> must be followed by (int) count
                                 */
  STRIPDIALOG_LAST_ATTRIBUTE
}
StripDialogAttribute;



/* ======= Functions ======= */
/*
 * StripDialog_init
 *
 *      Creates a new strip data structure, setting all values to defaults.
 */
StripDialog     StripDialog_init        (Widget, StripConfig *);


/*
 * StripDialog_delete
 *
 *      Destroys the specified strip chart.
 */
void    StripDialog_delete      (StripDialog);


/*
 * StripDialog_set/getattr
 *
 *      Sets or gets the specified attribute, returning true on success.
 */

int     StripDialog_setattr     (StripDialog, ...);
int     StripDialog_getattr     (StripDialog, ...);


/*
 * StripDialog_popup/down
 *
 */
void    StripDialog_popup       (StripDialog);
void    StripDialog_popdown     (StripDialog);


/*
 * StripDialog_add/removecurve
 *
 *      Adds or deletes the specified StripCurve to/from the list of active
 *      curves.  Returns true on success, false otherwise.
 */
int     StripDialog_addcurve            (StripDialog, StripCurve);
int     StripDialog_removecurve         (StripDialog, StripCurve);


/*
 * StripDialog_add/removesomecurves
 *
 *      Same as add/remove, except a NULL terminated array of curves is
 *      operated upon rather than just a single curve.
 */
int     StripDialog_addsomecurves       (StripDialog, StripCurve[]);
int     StripDialog_removesomecurves    (StripDialog, StripCurve[]);


/*
 * StripDialog_update_curvestat
 */
int     StripDialog_update_curvestat    (StripDialog, StripCurve);

/*
 * StripDialog_isviewable
 *
 *      Returns true if the dialog is viewable, false otherwise.
 */
int     StripDialog_isviewable  (StripDialog);

/*
 * StripDialog_ismapped
 *
 *      Returns true if the dialog window is mapped
 */
int     StripDialog_ismapped    (StripDialog);

/*
 * StripDialog_isiconic
 */
int     StripDialog_isiconic    (StripDialog);

/*
 * StripDialog_reset
 */
void            StripDialog_reset(StripDialog);


void    getwidgetval_min       (StripDialog, int, double *);
void    getwidgetval_max       (StripDialog, int, double *);
void    getwidgetval_precision (StripDialog, int, int *);

#endif
