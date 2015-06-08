
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

#ifndef _Annotation
#define _Annotation

#include <ellLib.h>
#include <X11/Xlib.h>
#include <Xm/Protocols.h>

#include "StripGraph.h"
#include "StripCurve.h"
#include "jlAxis.h"

#ifndef ANNOTATIONINFO
/* ======= Data Types ======= */
typedef void *  AnnotationInfo;
#endif

void  Annotation_draw(Display *display, Window window, GC gc,
                  XRectangle wind_rect, AnnotationInfo *ai,
                  struct timeval *pplotted_t0, struct timeval *pplotted_t1,
                  StripCurveInfo *curves[]);

void Annotation_move(XButtonEvent *event, AnnotationInfo *ai);
void Annotation_deleteSelected(AnnotationInfo *ai);
void AnnotateDialog_popup (AnnotationInfo *ai, XtPointer newAnnotation);
int Annotation_select(XButtonEvent *event, AnnotationInfo *ai);
AnnotationInfo *Annotation_init ( XtAppContext app, Widget canvas, Widget shell, 
         Display *display, void* graph, StripCurveInfo *curves);
void Annotation_deleteAll(AnnotationInfo *ai);
#endif
