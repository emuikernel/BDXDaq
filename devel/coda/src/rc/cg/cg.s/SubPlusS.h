/* $Id: SubPlusS.h,v 1.1.1.1 1996/10/10 19:38:06 chen Exp $ */
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


#ifndef _SubPlusS_h
#define _SubPlusS_h

#include <X11/Intrinsic.h>

/*
** These are routines for creating correctly matched
** subshells.
*/


#if NeedFunctionProtoTypes

Widget XtGetParentShell(Widget _w);
Widget XtPlusCreatePopupShell(String _name, WidgetClass _wc, Widget _parent, ArgList _warg, Cardinal _numWarg);

#else

Widget XtGetParentShell();
Widget XtPlusCreatePopupShell();

#endif /* NeedFunctionProtoTypes */

#if MOTIF

#if NeedFunctionProtoTypes
Widget XmPlusCreateOptionMenu(Widget _parent, String _name, ArgList _warg, Cardinal _n);
Widget XmPlusCreatePopupMenu(Widget _parent, String _name, ArgList _warg, Cardinal _n);
Widget XmPlusCreatePulldownMenu(Widget _parent, String _name, ArgList _warg, Cardinal _n);

#else
Widget XmPlusCreateOptionMenu();
Widget XmPlusCreatePopupMenu();
Widget XmPlusCreatePulldownMenu();

#endif /* NeedFunctionProtoTypes */


#endif /* _SubPlusS_h */

#endif /* MOTIF */
