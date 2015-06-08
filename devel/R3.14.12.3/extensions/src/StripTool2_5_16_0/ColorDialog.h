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

#ifndef _ColorDialog
#define _ColorDialog

#include "cColorManager.h"
#include "StripConfig.h"

#include <Xm/Xm.h>
#include <X11/Xlib.h>

/* ======= Data Types ======= */
typedef void    (*CDcallback)   (void *, cColor *);     /* client, call data */
typedef void *  ColorDialog;

ColorDialog     ColorDialog_init        (Widget,                /* parent */
                                         char *,                /* title */
                                         StripConfig *);
void            ColorDialog_delete      (ColorDialog);
void            ColorDialog_popup       (ColorDialog,
                                         char *,
                                         cColor *,
                                         CDcallback, /* called on OK, Apply */
                                         void *);    /* callback data */
void            ColorDialog_popdown     (ColorDialog);

#endif
