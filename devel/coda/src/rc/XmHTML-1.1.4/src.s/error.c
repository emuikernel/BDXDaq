#ifndef production
static char rcsId[]="$Header: /net/mizar/usr/local/source/coda_source/rc/XmHTML-1.1.4/XmHTML-1.1.4/src/error.c,v 1.4 1998/09/17 19:27:38 rwm Exp $";
#endif
/*****
* error.c : XmHTML warning/error functions
*
* This file Version	$Revision: 1.4 $
*
* Creation date:		Wed Jan 29 19:30:25 GMT+0100 1997
* Last modification: 	$Date: 1998/09/17 19:27:38 $
* By:					$Author: rwm $
* Current State:		$State: Exp $
*
* Author:				newt
*
* Copyright (C) 1994-1997 by Ripley Software Development 
* All Rights Reserved
*
* This file is part of the XmHTML Widget Library.
*
* This library is free software; you can redistribute it and/or
* modify it under the terms of the GNU Library General Public
* License as published by the Free Software Foundation; either
* version 2 of the License, or (at your option) any later version.
*
* This library is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
* Library General Public License for more details.
*
* You should have received a copy of the GNU Library General Public
* License along with this library; if not, write to the Free
* Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*
*****/
/*****
* ChangeLog 
* $Log: error.c,v $
* Revision 1.4  1998/09/17 19:27:38  rwm
* Nix debug prints and vsprintf.
*
* Revision 1.3  1998/09/16 12:45:43  rwm
* Ignore around vsprintf problem.
*
* Revision 1.9  1998/04/27 06:59:08  newt
* tka stuff
*
* Revision 1.8  1998/04/04 06:28:05  newt
* XmHTML Beta 1.1.3
*
* Revision 1.7  1997/08/31 17:33:22  newt
* log edit
*
* Revision 1.6  1997/08/30 00:48:26  newt
* Modified all protos: there are now two different versions of each function:
* one for debugging and one for production builds.
*
* Revision 1.5  1997/08/01 12:59:10  newt
* Added _XmHTMLBadParent
*
* Revision 1.4  1997/04/29 14:25:39  newt
* Moved dependency on a XmHTML widget out.
*
* Revision 1.3  1997/03/20 08:08:33  newt
* LynxOS changes
*
* Revision 1.2  1997/03/02 23:16:45  newt
* moved all html widget declarations between #ifdef DEBUG/#endif pairs
*
* Revision 1.1  1997/02/11 01:59:21  newt
* Initial Revision
*
*****/ 
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdlib.h>

#ifdef __STDC__
#include <stdarg.h>
#else
#include <varargs.h>
#endif

#include <stdio.h>	/* must follow stdarg or varargs on LynxOS */

#include "toolkit.h"
#include XmHTMLPrivateHeader
#include "XmHTMLfuncs.h"

/*** External Function Prototype Declarations ***/

/*** Public Variable Declarations ***/

/*** Private Datatype Declarations ****/

/*** Private Function Prototype Declarations ****/

/*** Private Variable Declarations ***/
#ifdef DEBUG
#define XM_BUF_LEN 8192
static char buf[XM_BUF_LEN];
static char loc[128];
#else
#define XM_BUF_LEN 8192
static char buf[XM_BUF_LEN];
#endif

/* default string that gets appended when a fatal error occurs */
static String authors = "    Please contact the XmHTML author at "
	"ripley@xs4all.nl.";

static String err_fmt = "\n    Name: %s\n    Class: %s\n    ";

/* undefine these to prevent compiler errors */
#undef _XmHTMLWarning
#undef _XmHTMLError

/*****
* Name: 		__XmHTMLWarning
* Return Type: 	void
* Description: 	displays a warning message to stderr
* In: 
*	w:			widget;
*	module:		source file name;
*	line:		line number information;
*	routine:	routine name;
*	fmt:		message to display;
*	...:		extra args to fmt;
* Returns:
*	nothing
* Note:
*	the module, line and routine args are only used when DEBUG was defined
*	during compilation.
*****/

void
#ifdef __STDC__
# ifdef DEBUG
__XmHTMLWarning(Widget w, String module, int line, String routine, 
	String fmt, ...) 
# else
__XmHTMLWarning(Widget w, String fmt, ...) 
# endif
{
    va_list arg_list;
    va_start(arg_list, fmt);

#else /* ! __STDC__ */
# ifdef DEBUG
__XmHTMLWarning(w, module, line, routine, fmt, va_alist)
# else
__XmHTMLWarning(w, fmt, va_alist)
# endif
    Widget w;
# ifdef DEBUG
	String module;
	int line;
	String routine;
#endif
    String fmt;
    va_dcl
{
	va_alist arg_list;
    va_start(arg_list);
#endif /* __STDC__ */

    memset(buf, 0, XM_BUF_LEN);

#ifdef DEBUG
	if(w && debug_disable_warnings)
		return;
	sprintf(loc, "\n    (%s, %s, line %i)\n", module, routine, line);
#endif

	if(w)
	{
	  char *pbuf = &buf[0];

/* 	  printf("&buf = %X.\n", buf); */
/* 	  printf(" buf = %s.\n", buf); */
/* 	  printf(" buflen = %d.\n", strlen(buf)); */
		sprintf(buf, err_fmt, XtName(w), XtClass(w)->core_class.class_name);
/* 	  printf(" buflen = %d.\n", strlen(buf)); */
	  pbuf += strlen(buf);
/* 		vsprintf(pbuf, fmt, arg_list); */
		va_end(arg_list);
#ifdef DEBUG
		strcat(buf, loc);
#else
		strcat(buf, "\n");
#endif
	    XtAppWarning(XtWidgetToApplicationContext(w), buf);
	}
	else
	{
/* 	  printf("&buf = %X.\n", buf); */
/* 	  printf(" buf = %s.\n", buf); */
/* 	  printf(" buflen = %d.\n", strlen(buf)); */

/* 		vsprintf(buf, fmt, arg_list); */
		va_end(arg_list);
#ifdef DEBUG
		strcat(buf, loc);
#else
		strcat(buf, "\n");
#endif
		XtWarning(buf);
	}
	_XmHTMLDebugMirrorToFile((buf));
}

/*****
* Name: 		__XmHTMLError
* Return Type: 	void
* Description: 	displays an error message on stderr and exits.
* In: 
*	w:			widget;
*	module:		source file name;
*	line:		line information;
*	routine:	routine name;
*	fmt:		message to display;
*	...:		args to message;
* Returns:
*	nothing.
* Note:
*	the module, line and routine args are only used when DEBUG was defined
*	during compilation.
*****/
void
#ifdef __STDC__
#ifdef DEBUG
__XmHTMLError(Widget w, String module, int line, String routine,
	String fmt, ...) 
#else
__XmHTMLError(Widget w, String fmt, ...) 
#endif
{
    va_list arg_list;
    va_start(arg_list, fmt);

    memset(buf, 0, XM_BUF_LEN);

#else /* ! __STDC__ */
#ifdef DEBUG
__XmHTMLError(w, module, line, routine, fmt, va_list)
#else
__XmHTMLError(w, fmt, va_alist) 
#endif
    Widget w;
#ifdef DEBUG
	String module;
	int line;
	String routine;
#endif
    String fmt;
    va_dcl
{
	va_alist arg_list;
    va_start(arg_list);
#endif /* __STDC__ */

#ifdef DEBUG
	if(w && debug_disable_warnings)
		return;
	sprintf(loc, "\n    (%s, %s, line %i)\n", module, routine, line);
#endif

	if(w)
	{
		sprintf(buf, err_fmt, XtName(w), XtClass(w)->core_class.class_name);
/* 	  printf("&buf = %X.\n", buf); */
/* 	  printf(" buf = %s.\n", buf); */
/* 	  printf(" buflen = %d.\n", strlen(buf)); */

/* 		vsprintf(buf+strlen(buf), fmt, arg_list); */
		va_end(arg_list);
#ifdef DEBUG
		strcat(buf, loc);
#else
		strcat(buf, "\n");
#endif
		strcat(buf, authors);
	    XtAppError(XtWidgetToApplicationContext(w), buf);
	}
	else
	{
/* 	  printf("&buf = %X.\n", buf); */
/* 	  printf(" buf = %s.\n", buf); */
/* 	  printf(" buflen = %d.\n", strlen(buf)); */

/* 		vsprintf(buf, fmt, arg_list); */
		va_end(arg_list);
#ifdef DEBUG
		strcat(buf, loc);
#else
		strcat(buf, "\n");
#endif
		strcat(buf, authors);
		XtError(buf);
	}
	_XmHTMLDebugMirrorToFile((buf));
	exit(EXIT_FAILURE);
}

/*****
* Name: 		_XmHTMLAllocError
* Return Type: 	void
* Description: 	displays an error message on stderr and exits.
* In: 
*	w:			widget
*	module:		source file name
*	routine:	routine name
*	func:		function used for allocation
*	size:		size for which allocation was attempted.
* Returns:
*	nothing.
*****/
void
_XmHTMLAllocError(Widget w, char *module, char *routine, char *func, int size)
{
	if(w)
	{
		sprintf(buf, "\n    Name: %s\n    Class: %s\n    "
			"cannot continue: %s failed for %i bytes.\n    (%s, %s)\n", 
			XtName(w), XtClass(w)->core_class.class_name, func, size, module,
			routine); 
	    XtAppError(XtWidgetToApplicationContext(w), buf);
	}
	else
	{
		sprintf(buf, "cannot continue: %s failed for %i bytes.\n"
			"    (%s, %s)\n", func, size, module, routine);
		XtError(buf);
	}
	_XmHTMLDebugMirrorToFile((buf));
	exit(EXIT_FAILURE);
}

/*****
* Name:			__XmHTMLBadParent
* Return Type: 	void
* Description: 	default warning message for a call to a public routine and
*				the widget argument is either NULL or not of class XmHTML.
* In: 
*	w:			offending Widget id;
*	src_file:	source file where function is found;
*	func:		function in which error occured.
* Returns:
*	nothing.
*****/
void
#ifdef DEBUG
__XmHTMLBadParent(Widget w, String src_file, int line, String func)
#else
__XmHTMLBadParent(Widget w, String func)
#endif
{
#ifdef DEBUG
	__XmHTMLWarning(w, src_file, line, func, XMHTML_MSG_34,
#else
	__XmHTMLWarning(w, XMHTML_MSG_34,
#endif
		(w ? "Invalid parent" : "NULL parent"), func);
}
