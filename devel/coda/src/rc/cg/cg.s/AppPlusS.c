/* $Id: AppPlusS.c,v 1.2 1998/04/08 18:30:53 heyes Exp $ */
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

#include <X11/IntrinsicP.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/Xmu/Editres.h>

#if HAS_XCC
#include "XCC.h"
#endif /* HAS_XCC */

#include "AppPlusSP.h"

#include <string.h>
#include <ctype.h>


#if MOTIF 
#include <Xm/Xm.h>
#if (XmREVISION >= 2)
#include <Xm/Protocols.h>
#endif
#endif /* MOTIF */

#if NeedFunctionProtoTypes
static XtProc classInitialize(void);
static XtInitProc initialize(AppPlusShellWidget _request, AppPlusShellWidget _new, String *_args, Cardinal *_numArgs);
static XtRealizeProc realize(AppPlusShellWidget _apsw, XtValueMask *_xvm, XSetWindowAttributes *_xswa);
static XtWidgetProc destroy(AppPlusShellWidget _apsw);
static void WMProtoAction(AppPlusShellWidget _apsw, XEvent *_event, String *_parms, Cardinal *_numParms);
static Boolean cvtStringToVisualClass(Display *_display, XrmValuePtr _args,
	Cardinal *_numArgs, XrmValuePtr _from, XrmValuePtr _to, XtPointer *_data);
static Boolean cvtStringToVisualID(Display *_display, XrmValuePtr _args,
	Cardinal *_numArgs, XrmValuePtr _from, XrmValuePtr _to, XtPointer *_data);
#else
static XtProc classInitialize();
static XtInitProc initialize();
static XtRealizeProc realize();
static XtWidgetProc destroy();
static void WMProtoAction();
static Boolean cvtStringToVisualClass();
static Boolean cvtStringToVisualID();
#endif /* NeedFunctionProtoTypes */

#define CORE(a) ((a)->core)
#define APPS(a) ((a)->app_plus_shell)
#define SHELL(a) ((a)->shell)
#define ACLASS(a) ((a)->app_plus_shell_class)
#define VID(a) ((a)->app_plus_shell.visualID)

#if MOTIF & (XmREVISION >= 2)
#define XtCharStringToAtom(_w, _string) XmInternAtom(XtDisplay(_w), _string, FALSE)
#else
#define XtCharStringToAtom(_w, _string) XInternAtom(XtDisplay(_w), _string, FALSE)
#endif /* MOTIF & (XmREVISION >= 2) */

#define DEFAULT -1

#define TheOffset(field) XtOffset(AppPlusShellWidget, app_plus_shell.field)
static XtResource resources[] =
{
	{XtNdata, XtCData, XtRPointer, sizeof(XtPointer),
		TheOffset(data), XtRImmediate, (XtPointer)NULL},
	{XtNsaveYourselfCallback, XtCCallback, XtRCallback, sizeof(XtCallbackList),
		TheOffset(saveYourselfCallback), XtRImmediate, (XtPointer)NULL},
	{XtNvisualClass, XtCVisualClass, XtRVisualClass, sizeof(int),
		TheOffset(visualClass), XtRImmediate, (XtPointer)DEFAULT},
	{XtNusePrivateColormap, XtCUsePrivateColormap, XtRBoolean, sizeof(Boolean),
		TheOffset(usePrivateColormap), XtRImmediate, (XtPointer)FALSE},
	{XtNvisualID, XtCVisualID, XtRVisualID, sizeof(VisualID),
		TheOffset(visualID), XtRImmediate, (XtPointer)0},
	{XtNallowEditRes, XtCAllowEditRes, XtRBoolean, sizeof(Boolean),
		TheOffset(allowEditRes), XtRImmediate, (XtPointer)FALSE},
	{XtNapplicationDepth, XtCApplicationDepth, XtRInt, sizeof(int),
		TheOffset(applicationDepth), XtRImmediate, (XtPointer)DEFAULT},
#if HAS_XCC
	{XtNuseStandardColormaps, XtCUseStandardColormaps, XtRBoolean, sizeof(Boolean),
		TheOffset(useStandardColormaps), XtRImmediate, (XtPointer)TRUE},
	{XtNstandardColormap, XtCStandardColormap, XtRAtom, sizeof(Atom),
		TheOffset(standardColormap), XtRImmediate, (XtPointer)0},
#endif /* HAS_XCC */
};
#undef TheOffset

static char defaultTranslations[] =
"<ClientMessage> WM_PROTOCOLS: WMProtoAction()";
 
static XtActionsRec actions[] =
{
	{"WMProtoAction", (XtActionProc)WMProtoAction},
};

AppPlusShellClassRec appPlusShellClassRec = 
{
	{	/* Core */
		(WidgetClass)&applicationShellClassRec,		 /* superclass */
		"AppPlusShell",								/* class_name */
		sizeof(AppPlusShellRec),					 /* widget_size */
		(XtProc)classInitialize,				 /* class_initialize */
		(XtWidgetClassProc)NULL,	 /* class_part_initialize */
		FALSE,						/* class_init */
		(XtInitProc)initialize,		 /* initialize */
		NULL,						 /* initialize_hook */
		(XtRealizeProc)realize,		 /* realize */
		actions,					/* actions */
		XtNumber(actions),			/* num_actions */
		resources,					 /* resources */
		XtNumber(resources),		 /* num_resouces */
		NULLQUARK,					/* xrm_class */
		FALSE,						 /* compress_motion */
		FALSE,	 					/* compress_exposure */
		FALSE,					 /* compress_enter_leave */
		FALSE,					 /* visibility_interest */
		(XtWidgetProc)destroy,	 /* destroy */
		XtInheritResize,		/* resize */
		XtInheritExpose,		/* expose */
		NULL,				 /* setvalues */
		NULL,				 /* setvalues_hook */
		XtInheritSetValuesAlmost, /* setvalues_almost */
		NULL,						 /* get_values_hook */
		(XtAcceptFocusProc)XtInheritAcceptFocus,	 /* accept_focus */
		XtVersion,							/* version */
		NULL,								 /* callback_private */
		defaultTranslations,				 /* tm_translations */
		XtInheritQueryGeometry,				 /* query_geometry */
		XtInheritDisplayAccelerator,		/* display_accelerator */
		NULL,								 /* extension */
	},
	{	/* Composite */
		XtInheritGeometryManager,
		XtInheritChangeManaged,
		XtInheritInsertChild,
		XtInheritDeleteChild,
		(XtPointer)NULL,
	},
	{	/* Shell */
		0,
	},
	{	/* WindowManagerShell */
		0,
	},
	{	/* VendorShell */
		(XtPointer)NULL,
	},
	{	/* TopLevelShell */
		(XtPointer)NULL,
	},
	{	/* ApplicationShell */
		(XtPointer)NULL,
	},
	{ 	/* AppPlusShell */
		(Colormap)0,
		(Visual *)NULL,
		0,	
		0,
#if HAS_XCC
		NULL,
#endif /* HAS_XCC */
	},
};

WidgetClass appPlusShellWidgetClass = (WidgetClass)&appPlusShellClassRec;

#if DEBUGGING
static char *VisualClassToName(int class)
{
	switch(class)
	{
		case StaticGray:
			return "StaticGray";
		case StaticColor:
			return "StaticColor";
		case GrayScale:
			return "GrayScale";
		case PseudoColor:
			return "PseudoColor";
		case TrueColor:
			return "TrueColor";
		case DirectColor:
			return "DirectColor";
		default:
			return "Unknown";
	}
}
#endif /* DEBUGGING */

#if NeedFunctionProtoTypes
static XtProc classInitialize(void)
#else
static XtProc classInitialize()
#endif /* NeedFunctionProtoTypes */
{
	XtSetTypeConverter(XtRString, XtRVisualClass, cvtStringToVisualClass,
		(XtConvertArgList)NULL, 0,
		XtCacheAll, (XtDestructor)NULL);

	XtSetTypeConverter(XtRString, XtRVisualID, cvtStringToVisualID,
		(XtConvertArgList)NULL, 0,
		XtCacheAll, (XtDestructor)NULL);
}

#if NeedFunctionProtoTypes
static void useDefaults(AppPlusShellWidget _apsw)
#else
static void useDefaults(_apsw)
AppPlusShellWidget _apsw;
#endif /* NeedFunctionProtoTypes */
{
	CORE(_apsw).depth = DefaultDepth(XtDisplay(_apsw), DefaultScreen(XtDisplay(_apsw)));
	SHELL(_apsw).visual = DefaultVisual(XtDisplay(_apsw), DefaultScreen(XtDisplay(_apsw)));
	APPS(_apsw).visualClass = SHELL(_apsw).visual->class;
}

#if NeedFunctionProtoTypes
static Boolean validVisualID(AppPlusShellWidget _apsw, XVisualInfo *_visualInfo)
#else
static Boolean validVisualID(_apsw, _visualInfo)
AppPlusShellWidget _apsw;
XVisualInfo *_visualInfo;
#endif /* NeedFunctionProtoTypes */
{
	XVisualInfo *vinfos, vtemp;
	int vitems;

	if (VID(_apsw) == 0)
		return FALSE;

	vtemp.visualid = VID(_apsw);

	vinfos = XGetVisualInfo(XtDisplay(_apsw), VisualIDMask, &vtemp, &vitems);

	if (vinfos != NULL)
	{
/*
** There better ONLY BE ONE!
*/
		_visualInfo->visual = vinfos[0].visual;
		_visualInfo->depth = vinfos[0].depth;
		_visualInfo->class = vinfos[0].class;

		XFree((char *)vinfos);
		return TRUE;
	}
	else
		return FALSE;
}

#if NeedFunctionProtoTypes
static XtInitProc initialize(AppPlusShellWidget _request, AppPlusShellWidget _new, String *_args, Cardinal *_numArgs)
#else
static XtInitProc initialize(_request, _new, _args, _numArgs)
AppPlusShellWidget _request;
AppPlusShellWidget _new;
String *_args;
Cardinal *_numArgs;
#endif /* NeedFunctionProtoTypes */
{
	Display *display;
	int screenNumber;
	Visual *defVisual;
	XVisualInfo visualInfo;
	AppPlusShellWidgetClass aswc;

	if (APPS(_new).allowEditRes)
		XtAddEventHandler((Widget)_new, (EventMask)0, TRUE, _XEditResCheckMessages , NULL);

	aswc = (AppPlusShellWidgetClass)appPlusShellWidgetClass;

	/*	if (ACLASS(aswc).colormap == 0)*/
	if (1) 
	{
#if DEBUGGING
		printf("Creating Visual, colormap, etc...\n");
#endif /* DEBUGGING */

		display = XtDisplay(_new);
		screenNumber = DefaultScreen(display);
		defVisual = DefaultVisual(display, screenNumber);

/*
** Highest priority is given to a specified visual id.
*/
		if (validVisualID(_new, &visualInfo))
		{
				SHELL(_new).visual = visualInfo.visual;
				CORE(_new).depth = visualInfo.depth;
				APPS(_new).visualClass = visualInfo.class;
		}
		else
		if ( (APPS(_new).applicationDepth == DEFAULT) && (APPS(_new).visualClass == DEFAULT))
		{
		  useDefaults(_new);
		}
		else
		{
			if (APPS(_new).applicationDepth == DEFAULT)
				CORE(_new).depth = DefaultDepth(display, screenNumber);
			else
				CORE(_new).depth = APPS(_new).applicationDepth;

			if (APPS(_new).visualClass == DEFAULT)
				APPS(_new).visualClass = defVisual->class;
/*
** Try for match.
*/
			if (XMatchVisualInfo(display, screenNumber, CORE(_new).depth, 
				APPS(_new).visualClass, &visualInfo) != 0)
			{
				SHELL(_new).visual = visualInfo.visual;
				CORE(_new).depth = visualInfo.depth;
				APPS(_new).visualClass = visualInfo.class;
			}
			else
			{
				XVisualInfo visTemplate;
				XVisualInfo *visReturn;
				int numVis, n;

				n = 0;
				visReturn = (XVisualInfo *)NULL;

/*
** Next see if we can find a depth that they have set.
*/

				if (APPS(_new).applicationDepth != DEFAULT)
				{
					visTemplate.depth = CORE(_new).depth;
					visReturn = XGetVisualInfo(display, VisualDepthMask, &visTemplate, &numVis);

/*
** If numVis is greater than 1 you may want to have it pick your favorite visual here. I
** don't think this is really necessary though, since the user still has control with
** the XtNvisualClass to get the one he/she really wants.
*/
				}

				if (visReturn == (XVisualInfo *)NULL)
				{
/*
** Nope.  Now find the best depth at this visual class
*/
					visTemplate.class = APPS(_new).visualClass;
					visReturn = XGetVisualInfo(display, VisualClassMask, &visTemplate, &numVis);

					if (visReturn != (XVisualInfo *)NULL)
					{
						int i;
						n = 0;
						for (i=1;i<numVis;i++)
							if (visReturn[i].depth > visReturn[n].depth)
								n = i;
					}
				}

/*
** Nope. Use defaults.
*/
				if (visReturn == (XVisualInfo *)NULL)
					useDefaults(_new);
				else
				{
					SHELL(_new).visual =	visReturn[n].visual;
					CORE(_new).depth = visReturn[n].depth;
					APPS(_new).visualClass = visReturn[n].class;
					XtFree((char *)visReturn);
				}
			}
		}

#if HAS_XCC
		ACLASS(aswc).xcc = XCCCreate(_new, SHELL(_new).visual, APPS(_new).usePrivateColormap, 
					     APPS(_new).useStandardColormaps, APPS(_new).standardColormap, &CORE(_new).colormap);
#else


		if ( (SHELL(_new).visual->visualid == defVisual->visualid) && !APPS(_new).usePrivateColormap)
		{
			CORE(_new).colormap =   DefaultColormap(display, screenNumber);
#if DEBUGGING
			printf("Using default visual, depth, and colormap\n");
#endif /* DEBUGGING */
		}
		else
		{
			CORE(_new).colormap = XCreateColormap(display, RootWindow(display, screenNumber), 
				SHELL(_new).visual, AllocNone);
#if DEBUGGING
			printf("Created colormap\n");
#endif /* DEBUGGING */
		}
#endif /* HAS_XCC */


		ACLASS(aswc).visual = SHELL(_new).visual;
		ACLASS(aswc).colormap = CORE(_new).colormap;
		ACLASS(aswc).visualClass = APPS(_new).visualClass;
		ACLASS(aswc).applicationDepth = CORE(_new).depth;
        }
	else
	{
#if DEBUGGING
		printf("Using already created visual, colormap, etc...\n");
#endif /* DEBUGGING */
		SHELL(_new).visual = ACLASS(aswc).visual;
		CORE(_new).colormap = ACLASS(aswc).colormap;
		APPS(_new).visualClass = ACLASS(aswc).visualClass;
		CORE(_new).depth = ACLASS(aswc).applicationDepth;
	}


	APPS(_new).numberProtos = 0;
	APPS(_new).saveYourself[(APPS(_new).numberProtos)++] = XtCharStringToAtom((Widget)_new, "WM_DELETE_WINDOW");
}

#if NeedFunctionProtoTypes
static XtRealizeProc realize(AppPlusShellWidget _apsw, XtValueMask *_xvm, XSetWindowAttributes *_xswa)
#else
static XtRealizeProc realize(_apsw, _xvm, _xswa)
AppPlusShellWidget _apsw;
XtValueMask *_xvm;
XSetWindowAttributes *_xswa;
#endif /* NeedFunctionProtoTypes */
{
	_xswa->colormap = CORE(_apsw).colormap;
	_xswa->background_pixel = _xswa->border_pixel = 0;
	*_xvm |= CWColormap | CWBorderPixel | CWBackPixel;

	(*appPlusShellWidgetClass->core_class.superclass->core_class.realize)((Widget)_apsw, _xvm, _xswa);

	XSetWindowColormap(XtDisplay(_apsw), XtWindow(_apsw), CORE(_apsw).colormap);

#if 0 /* HMMM */
	XSetWMColormapWindows(XtDisplay(_apsw), XtWindow(_apsw), &XtWindow(_apsw), 1);
#endif /* 0 */

#if MOTIF & (XmREVISION >= 2)
	XmAddWMProtocols((Widget)_apsw, APPS(_apsw).saveYourself, APPS(_apsw).numberProtos);
#else
	(void)XSetWMProtocols(XtDisplay(_apsw), XtWindow(_apsw), APPS(_apsw).saveYourself, APPS(_apsw).numberProtos);
#endif /* MOTIF & (XmREVISION >= 2) */
}

#if NeedFunctionProtoTypes
static XtWidgetProc destroy(AppPlusShellWidget _apsw)
#else
static XtWidgetProc destroy(_apsw)
AppPlusShellWidget _apsw;
#endif /* NeedFunctionProtoTypes */
{
	XtRemoveAllCallbacks((Widget)_apsw, XtNsaveYourselfCallback);
}

#if NeedFunctionProtoTypes
static void WMProtoAction(AppPlusShellWidget _apsw, XEvent *_event, String *_parms, Cardinal *_numParms)
#else
static void WMProtoAction(_apsw, _event, _parms, _numParms)
AppPlusShellWidget _apsw;
XEvent *_event;
String *_parms;
Cardinal *_numParms;
#endif /* NeedFunctionProtoTypes */
{
	Atom proto;
	int i;

#if DEBUGGING
	printf("proto action\n");
#endif /* DEBUGGING */
 
	if (_event->type != ClientMessage)
		return;

	proto = (Atom)_event->xclient.data.l[0];

	for(i=0;i<APPS(_apsw).numberProtos;i++)
		if (APPS(_apsw).saveYourself[i] == proto)
		{
			XtCallCallbacks((Widget)_apsw, XtNsaveYourselfCallback, _event);
			break;
		}
}

#if NeedFunctionProtoTypes
static void toLower(char *_str1, char *_str2, int _length)
#else
static void toLower(_str1, _str2, _length)
char *_str1;
char *_str2;
int _length;
#endif /* NeedFunctionProtoTypes */
{
	int i;
	char *ptr;

	for(ptr=_str1,i=0;(ptr!=NULL) && (i<_length);ptr++,i++)
		*(_str2+i) = tolower(*ptr);
}

#if NeedFunctionProtoTypes
static Boolean cvtStringToVisualClass(Display *_display, XrmValuePtr _args,
	Cardinal *_numArgs, XrmValuePtr _from, XrmValuePtr _to, XtPointer *_data)
#else
static Boolean cvtStringToVisualClass(_display, _args, _numArgs, _from, _to, _data)
Display *_display;
XrmValuePtr _args;
Cardinal *_numArgs;
XrmValuePtr _from;
XrmValuePtr _to;
XtPointer *_data;
#endif /* NeedFunctionProtoTypes */
{
	char *lower;
	static int vClass;
	Boolean badConversion = FALSE;

	if (*_numArgs != 0)
	{
		XtAppWarningMsg(XtDisplayToApplicationContext(_display), "CvtStringToVisualClass", "wrongParamaters",
		"ResourceError",
		"cvtStringToVisualClass needs no arguments.",
		(String *)NULL, (Cardinal *)NULL);
	}

	lower = XtNewString(_from->addr);
	toLower(_from->addr, lower, strlen(_from->addr));

	vClass = (int)DEFAULT;

	if (!strncmp(lower, "staticgray", 10))
		vClass = StaticGray;
	else
	if (!strncmp(lower, "staticcolor", 11))
		vClass = StaticColor;
	else
	if (!strncmp(lower, "pseudocolor", 11))
		vClass = PseudoColor;
	else
	if (!strncmp(lower, "grayscale", 9))
		vClass = GrayScale;
	else
	if (!strncmp(lower, "truecolor", 9))
		vClass = TrueColor;
	else
	if (!strncmp(lower, "directcolor", 11))
		vClass = DirectColor;
	else
		badConversion = TRUE;

	XtFree(lower);

	if (badConversion)
		XtDisplayStringConversionWarning(_display, _from->addr, XtRVisualClass);
	else
	{
		if (_to->addr == NULL)
			_to->addr = (XtPointer)&vClass;
		else
		if (_to->size < sizeof(int))
			badConversion = TRUE;
		else
			*(int *)_to->addr = vClass;
		_to->size = sizeof(int);
	}

	return !badConversion;
}


#if NeedFunctionProtoTypes
static Boolean cvtStringToVisualID(Display *_display, XrmValuePtr _args,
		Cardinal *_numArgs, XrmValuePtr _from, XrmValuePtr _to, XtPointer *_data)
#else
static Boolean cvtStringToVisualID(_display, _args, _numArgs, _from, _to, _data)
Display *_display;
XrmValuePtr _args;
Cardinal *_numArgs;
XrmValuePtr _from;
XrmValuePtr _to;
XtPointer *_data;
#endif /* NeedFunctionProtoTypes */
{
		static VisualID value;
		Boolean badConversion;

		if (*_numArgs != 0)
		{
				XtAppWarningMsg(XtDisplayToApplicationContext(_display), "CvtStringToVisualID", "wrongParamaters",
				"ResourceError",
				"CvtStringToVisualID needs no arguments.",
				(String *)NULL, (Cardinal *)NULL);
		}

		badConversion = FALSE;

		value	= (VisualID)strtol(_from->addr, (char **)NULL, 0);

		if (_to->addr == NULL)
				_to->addr = (XtPointer)&value;
		else
		if (_to->size < sizeof(VisualID))
				badConversion = TRUE;
		else
				*(VisualID *)_to->addr = value;
				_to->size = sizeof(VisualID);

		return !badConversion;
}

#if HAS_XCC

#if NeedFunctionProtoTypes
XCC AppPlusShellGetXCC(Widget _w)
#else
XCC AppPlusShellGetXCC(_w)
Widget _w;
#endif /* NeedFunctionProtoTypes */
{
	AppPlusShellWidgetClass aswc;
	aswc = (AppPlusShellWidgetClass)appPlusShellWidgetClass;

	if (aswc->core_class.class_inited)
		return ACLASS(aswc).xcc;
	else
	{
		XtAppWarning(XtWidgetToApplicationContext(_w),
			"Attempting to access XCC when AppPlusShell is uninitialized!\n");
		return (XCC)NULL;
	}
}

#endif /* HAS_XCC */
