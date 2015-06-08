/* $Id: SubPlusS.c,v 1.1.1.1 1996/10/10 19:38:06 chen Exp $ */
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


#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/Shell.h>

#include "SubPlusS.h"

/*
** These are routines for creating correctly matched
** subshells.
*/


#if NeedFunctionProtoTypes
Widget XtGetParentShell(Widget _w)
#else
Widget XtGetParentShell(_w)
Widget _w;
#endif /* NeedFunctionProtoTypes */
{
	Widget parent = _w;

	while (parent && !XtIsShell(parent))
		parent = XtParent(parent);

	return parent;
}

#if NeedFunctionProtoTypes
Widget XtPlusCreatePopupShell(String _name, WidgetClass _wc, Widget _parent, ArgList _warg, Cardinal _numWarg)
#else
Widget XtPlusCreatePopupShell(_name, _wc, _parent, _warg, _numWarg)
String _name;
WidgetClass _wc;
Widget _parent;
ArgList _warg;
Cardinal _numWarg;
#endif /* NeedFunctionProtoTypes */
{
	ArgList newWarg;
	Cardinal newNumWarg;
	Widget shellParent, popup;
	int depth;
	Visual *visual;
	Colormap colormap;
	int n, tn;

	newNumWarg = _numWarg + 5;
	newWarg = (ArgList)XtMalloc(sizeof(Arg)*newNumWarg);

	shellParent = XtGetParentShell(_parent);

	n = 0;
	XtSetArg(newWarg[n], XtNdepth, &depth); n++;
	XtSetArg(newWarg[n], XtNvisual, &visual); n++;
	XtSetArg(newWarg[n], XtNcolormap, &colormap); n++;
	XtGetValues(shellParent, newWarg, n);

	n = 0;
	XtSetArg(newWarg[n], XtNdepth, depth); n++;
	XtSetArg(newWarg[n], XtNvisual, visual); n++;
	XtSetArg(newWarg[n], XtNcolormap, colormap); n++;
	XtSetArg(newWarg[n], XtNborderColor, 0); n++;
	XtSetArg(newWarg[n], XtNbackground, 0); n++;

	for(tn=0;n<newNumWarg;n++,tn++)
	{
		newWarg[n].name = _warg[tn].name;
		newWarg[n].value = _warg[tn].value;
	}

	popup = XtCreatePopupShell(_name, _wc, _parent, newWarg, newNumWarg);

	XtFree((void *)newWarg);

	return popup;
}

#if MOTIF
#include <Xm/Xm.h>
#include <Xm/RowColumn.h>
#include <Xm/MenuShell.h>

#if NeedFunctionProtoTypes
static Widget createPlusMenu(Widget _parent, String _name, ArgList _warg, Cardinal _n, char _type);
#else
static Widget createPlusMenu();
#endif /* NeedFunctionProtoTypes */



#if NeedFunctionProtoTypes
Widget XmPlusCreateOptionMenu(Widget _parent, String _name, ArgList _warg, Cardinal _n)
#else
Widget XmPlusCreateOptionMenu(_parent, _name, _warg, _n)
Widget _parent;
String _name;
ArgList _warg;
Cardinal _n;
#endif /* NeedFunctionProtoTypes */
{
	Cardinal newNumWarg;
	ArgList newWarg;
	int n, tn;
	Widget w;
	Colormap colormap;
	int depth;
	Visual *visual;
	Widget shell;
	
	shell = XtGetParentShell(_parent);

	newNumWarg = _n + 9;
	newWarg = (ArgList)XtMalloc(sizeof(Arg)*newNumWarg);

	n = 0;
	XtSetArg(newWarg[n], XtNvisual, &visual); n++;
	XtSetArg(newWarg[n], XtNdepth, &depth); n++;
	XtSetArg(newWarg[n], XtNcolormap, &colormap); n++;
	XtGetValues(shell, newWarg, n);

	n = 0;
	XtSetArg(newWarg[n], XtNvisual, visual); n++;
	XtSetArg(newWarg[n], XtNdepth, depth); n++;
	XtSetArg(newWarg[n], XtNcolormap, colormap); n++;
	XtSetArg(newWarg[n], XtNborderColor, 0); n++;
	XtSetArg(newWarg[n], XtNbackground, 0); n++;
	XtSetArg(newWarg[n], XtNwidth, 5); n++;
	XtSetArg(newWarg[n], XtNheight, 5);	n++;
	XtSetArg(newWarg[n], XmNallowShellResize, TRUE);   n++;
	XtSetArg(newWarg[n], XtNoverrideRedirect, TRUE);   n++;

	for(tn=0;n<newNumWarg;n++,tn++)
	{
		newWarg[n].name = _warg[tn].name;
		newWarg[n].value = _warg[tn].value;
	}

	w = XmCreateOptionMenu(_parent, _name, newWarg, newNumWarg);

	XtFree((void *)newWarg);

	return w;
}

#if NeedFunctionProtoTypes
Widget XmPlusCreatePopupMenu(Widget _parent, String _name, ArgList _warg, Cardinal _n)
#else
Widget XmPlusCreatePopupMenu(_parent, _name, _warg, _n)
Widget _parent;
String _name;
ArgList _warg;
Cardinal _n;
#endif /* NeedFunctionProtoTypes */
{
	return createPlusMenu(_parent, _name, _warg, _n, XmMENU_POPUP);
}

#if NeedFunctionProtoTypes
Widget XmPlusCreatePulldownMenu(Widget _parent, String _name, ArgList _warg, Cardinal _n)
#else
Widget XmPlusCreatePulldownMenu(_parent, _name, _warg, _n)
Widget _parent;
String _name;
ArgList _warg;
Cardinal _n;
#endif /* NeedFunctionProtoTypes */
{
	return createPlusMenu(_parent, _name, _warg, _n, XmMENU_PULLDOWN);
}

#if NeedFunctionProtoTypes
static Widget createPlusMenu(Widget _parent, String _name, ArgList _warg, Cardinal _n, char _type)
#else
static Widget createPlusMenu(_parent, _name, _warg, _n, _type)
Widget _parent;
String _name;
ArgList _warg;
Cardinal _n;
int _type;
#endif /* NeedFunctionProtoTypes */
{
	Cardinal newNumWarg;
	ArgList newWarg;
	int n, tn;
	Widget w, mp;
	Colormap colormap;
	int depth;
	Visual *visual;
	Widget shell;
	char b[200];

	shell = XtGetParentShell(_parent);

	newNumWarg = _n + 10;
	newWarg = (ArgList)XtMalloc(sizeof(Arg)*newNumWarg);

	n = 0;
	XtSetArg(newWarg[n], XtNvisual, &visual); n++;
	XtSetArg(newWarg[n], XtNdepth, &depth); n++;
	XtSetArg(newWarg[n], XtNcolormap, &colormap); n++;
	XtGetValues(shell, newWarg, n);

	n = 0;
	XtSetArg(newWarg[n], XtNvisual, visual); n++;
	XtSetArg(newWarg[n], XtNdepth, depth); n++;
	XtSetArg(newWarg[n], XtNcolormap, colormap); n++;
	XtSetArg(newWarg[n], XtNborderColor, 0); n++;
	XtSetArg(newWarg[n], XtNbackground, 0); n++;
	XtSetArg(newWarg[n], XtNwidth, 5); n++;
	XtSetArg(newWarg[n], XtNheight, 5);	n++;
	XtSetArg(newWarg[n], XmNallowShellResize, TRUE);   n++;
	XtSetArg(newWarg[n], XtNoverrideRedirect, TRUE);   n++;

	for(tn=0;n<newNumWarg;n++,tn++)
	{
		newWarg[n].name = _warg[tn].name;
		newWarg[n].value = _warg[tn].value;
	}

	sprintf(b, "popup_%s", _name);
	mp = XtCreatePopupShell(b, xmMenuShellWidgetClass, _parent, newWarg, n);

	n = 0;
	XtSetArg(newWarg[n], XmNrowColumnType, _type); n++;
	w = XtCreateWidget(_name, xmRowColumnWidgetClass, mp, newWarg, n);

	XtFree((void *)newWarg);

	return w;
}

#endif /* MOTIF */
