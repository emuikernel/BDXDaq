/*****
* misc.h : routines shared by a number of the example programs
*
* This file Version	$Revision: 1.1 $
*
* Creation date:		Sun Nov  2 22:17:14 GMT+0100 1997
* Last modification: 	$Date: 1998/04/09 17:51:50 $
* By:					$Author: heyes $
* Current State:		$State: Exp $
*
* Author:				newt
*
* Copyright (C) 1994-1997 by Ripley Software Development 
* All Rights Reserved
*
* This file is part of the XmHTML Widget Library
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
* $Source: /net/mizar/usr/local/source/coda_source/runControl/Xui/misc.h,v $
*****/
/*****
* ChangeLog 
* $Log: misc.h,v $
* Revision 1.1  1998/04/09 17:51:50  heyes
* add
*
*****/ 

#ifndef _misc_h_
#define _misc_h_

/* mime types getMimeType knows about */
typedef enum{
	MIME_HTML = 0,		/* text/html */
	MIME_HTML_PERFECT,	/* text/html-perfect */
	MIME_IMAGE,			/* image/whatever */
	MIME_PLAIN,			/* text/plain/unknown */
	MIME_IMG_UNSUP,		/* unsupported image type */
	MIME_ERR			/* some error occured */
}mimeType;

/* determine the mime type of the given file */
extern mimeType getMimeType(String file);

/*****
* Load a file and return it's contents. Return value must be freed by the
* caller. Returns a mime-description string in mime_type
*****/
extern String loadFile(String filename, String *mime_type);

/* copy and possibly collapse a URL to a string with at most 50 chars */
String collapseURL(String url);

/* Returns True if filename is a symbolic link */
extern Boolean followSymLinks(String filename);

/*****
* Display a Busy cursor when state is True and removes it when state is
* False
*****/
extern void setBusy(Widget w, Boolean state);

/* display a messagebox with the string msg */
extern void XMessage(Widget widget, String msg);

/* Decompose a Unix file name into a file name and a path */ 
int parseFilename(char *fullname, char *filename, char *pathname);

/* Don't add anything after this endif! */
#endif /* _misc_h_ */
