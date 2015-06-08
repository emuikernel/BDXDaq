/* $Id: AppPlusS.h,v 1.1.1.1 1996/10/10 19:38:06 chen Exp $ */
/*
 * Copyright 1994,1995 John L. Cwikla
 *
 * Permission to use, copy, modify, distribute, and sell this software
 * and its documentation for any purpose is hereby granted without fee,
 * provided that the above copyright notice appears in all copies and that
 * both that copyright notice and this permission notice appear in
 * supporting documentation, and that the name of John L. Cwikla or
 * Wolfram Research, Inc not be used in advertising or publicity
 * pertaining to distribution of the software without specific, written
 * prior permission.  John L. Cwikla and Wolfram Research, Inc make no
 * representations about the suitability of this software for any
 * purpose.  It is provided "as is" without express or implied warranty.
 *
 * John L. Cwikla and Wolfram Research, Inc disclaim all warranties with
 * regard to this software, including all implied warranties of
 * merchantability and fitness, in no event shall John L. Cwikla or
 * Wolfram Research, Inc be liable for any special, indirect or
 * consequential damages or any damages whatsoever resulting from loss of
 * use, data or profits, whether in an action of contract, negligence or
 * other tortious action, arising out of or in connection with the use or
 * performance of this software.
 *
 * Author:
 *  John L. Cwikla
 *  X Programmer
 *  Wolfram Research Inc.
 *
 *  cwikla@wri.com
*/

#ifndef _AppPlusShell_h
#define _AppPlusShell_h

#include <X11/Shell.h>

extern WidgetClass appPlusShellWidgetClass;

typedef struct _AppPlusShellRec *AppPlusShellWidget;

#ifndef XtIsAppPlusShell
#define XtIsAppPlusShell(a) (XtIsSubclass(a, appPlusShellWidgetClass))
#endif /* XtIsAppPlusShell */

typedef struct _AppPlusShellCallbackStruct
{
  int reason;
  XEvent *event;
} AppPlusShellCallbackStruct;

#define XtNdata "data"
#define XtCData "Data"
#define XtNsaveYourselfCallback "saveYourselfCallback"
#define XtNvisualClass "visualClass"
#define XtCVisualClass "VisualClass"
#define XtRVisualClass "VisualClass"
#define XtNusePrivateColormap "usePrivateColormap"
#define XtCUsePrivateColormap "UsePrivateColormap"
#define XtNvisualID "visualID"
#define XtCVisualID "VisualID"
#define XtRVisualID "VisualID"
#define XtNallowEditRes "allowEditRes"
#define XtCAllowEditRes "AllowEditRes"
#define XtNapplicationDepth "applicationDepth"
#define XtCApplicationDepth "ApplicationDepth"
#define XtNuseStandardColormaps "useStandardColormaps"
#define XtCUseStandardColormaps "UseStandardColormaps"
#define XtNstandardColormap "standardColormap"
#define XtCStandardColormap "StandardColormap"

#include "XCC.h"

#ifdef __cplusplus
extern "C" {
#endif

#if defined (__STDC__) || defined (__cplusplus)
extern XCC AppPlusShellGetXCC(Widget _w);
#else
extern XCC AppPlusShellGetXCC();
#endif /* NeedFunctionProtoTypes */

#ifdef __cplusplus
};
#endif


#endif /* _AppPlusShell_h */
