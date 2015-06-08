#ifndef lint
static char rcsId[]="$Header: /net/mizar/usr/local/source/coda_source/runControl/Xui/misc.c,v 1.3 1999/10/25 17:31:14 abbottd Exp $";
#endif
/*****
* misc.c : misc. stuff used by the examples.
*
* This file Version	$Revision: 1.3 $
*
* Creation date:		Wed May 29 22:35:32 GMT+0100 1996
* Last modification: 	$Date: 1999/10/25 17:31:14 $
* By:					$Author: abbottd $
* Current State:		$State: Exp $
*
* Author:				newt
*
* Copyright (C) 1994-1997 by Ripley Software Development 
* All Rights Reserved
*
* This file is part the XmHTML Widget Library.
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
* Note: this file has a long history. It's first use was in iEdit, an editor
*       for programmers I started on way back in '94. It then was changed and
*       greatly extended for use with Newt, my master thesis study and was
*       then adapted for use with XntHelp, the help application for both Newt
*       and eXode. And now it is finding its use with XmHTML as well...
*       Good software can be quite reusable ;-)
*****/
/*****
* ChangeLog 
* $Log: misc.c,v $
* Revision 1.3  1999/10/25 17:31:14  abbottd
* Remove Linux WCHAR_T define
*
* Revision 1.2  1998/11/06 20:07:03  timmer
* Linux port wchar_t stuff
*
* Revision 1.1  1998/04/09 17:51:59  heyes
* add
*
* Revision 2.3  1997/10/23 00:28:44  newt
* XmHTML Beta 1.1.0 release
*
* Revision 2.2  1997/05/28 02:14:25  newt
* Moved and adapted from XntHelp, the Newt and eXode help tool.
*
* Revision 2.1  1997/02/11 02:26:07  newt
* removed newt specific typedefs
*
* Revision 2.0  1996/09/19 02:45:20  newt
* Updated for source revision 2.0
*
* Revision 1.7  1996/06/27 03:54:52  newt
* Changes due to the new ntError prototype.
*
* Revision 1.6  1996/01/16  04:05:52  newt
* changed to call to NT_ERROR() to comply with the modified macro: each
* NT_() error macro now requires two arguments.
*
* Revision 1.5  1996/01/13  01:45:46  newt
* replaced all malloc/calloc calls with newt memory routines nt_malloc and
* nt_calloc.
*
* Revision 1.4  1996/01/11  23:54:24  newt
* made much-called routines inline using FAST
*
* Revision 1.3  1995/12/14  07:50:04  newt
* fixed a few typedefs remaining from importing parseFilename and removeSpaces.
*
* Revision 1.2  1995/12/14  02:34:06  newt
* Added the parseFilename and removeSpaces routines.
*
* Revision 1.1  1995/12/05  21:12:09  koen
* Initial revision
*
*****/ 

#include <stdio.h>
#include <string.h>
#include <sys/param.h>
#include <limits.h>
#include <unistd.h>
#include <errno.h>

#include <X11/Xos.h>
#include <X11/cursorfont.h>
#include <Xm/MessageB.h>
#include <XmHTML.h>
#include <stdlib.h>
#include "misc.h"

#ifdef DMALLOC
#include <dmalloc.h>
#endif

/*****
* external function prototypes
*****/
/* can be found in XmHTML */
extern char *my_strcasestr(const char *s1, const char *s2);

/*****
* Local function prototypes
*****/
static int normalizePathname(char *pathname);
static int compressPathname(char *pathname);
static char *nextSlash(char *ptr);
static char *prevSlash(char *ptr);
static int compareThruSlash(char *string1, char *string2);
static void copyThruSlash(char **toString, char **fromString);

/*****
* countStrings: count the number of strings in a string with a given
*	separator.
* In:
*	src: source containing strings to be counted
*	sep: separator used between the strings.
* Returns:
*	the number of strings in src.
*****/
short int
countStrings(char *src, const char sep)
{
	register char *chPtr;
	static short int numStr = 0;

	for(chPtr = &src[0]; *chPtr != '\0'; chPtr++)
	{
		if(*chPtr == sep)
			numStr++;
	}
	/* 
	* if the last character in the given string is not terminated by the
	* given separation character, add one to the counted strings 
	* We always return the total number of separate strings found. The
	* following test will add 1 to the counted strings if the array given
	* is not terminated with a closing separator.
	* As a side effect, it will set numStr to 1 if there is only one string
	* present.
	*/
	if(src[strlen(src)-1] != sep)
		numStr++;
	return(numStr);
}
	
static char*
nextSlash(char *ptr)
{
	for(; *ptr!='/'; ptr++) 
	{
		if (*ptr == '\0')
		return(NULL);
	}
	return(ptr + 1);
}

static char*
prevSlash(char *ptr)
{
	for(ptr -= 2; *ptr!='/'; ptr--);
	return(ptr + 1);
}

static void 
copyThruSlash(char **toString, char **fromString)
{
	char *to = *toString;
	char *from = *fromString;
	
	while (1) 
	{
		*to = *from;
		if (*from =='\0') 
		{
			*fromString = NULL;
			return;
		}
		if (*from=='/') 
		{
			*toString = to + 1;
			*fromString = from + 1;
			return;
		}
		from++;
		to++;
	}
}

static int 
compareThruSlash(char *string1, char *string2)
{
	while (1) 
	{
		if (*string1 != *string2)
			return(0);
		if (*string1 =='\0' || *string1=='/')
			return(1);
		string1++;
		string2++;
	}
}

static int 
compressPathname(char *pathname)
{
	char *inPtr, *outPtr;

	/* compress out . and .. */
	inPtr = &pathname[1];		/* start after initial / */
	outPtr = &pathname[1];
	while (1) 
	{
		/* if the next component is "../", remove previous component */
		if ((compareThruSlash(inPtr, "../"))!= 0) 
		{
			/* error if already at beginning of string */
			if (outPtr == &pathname[1])
				return(0);
			/* back up outPtr to remove last path name component */
			outPtr = prevSlash(outPtr);
			inPtr = nextSlash(inPtr);
		} 
		else 
			if ((compareThruSlash(inPtr, "./"))!= 0) 
				/* don't copy the component if it's a redundant "./" */
				inPtr = nextSlash(inPtr);
			else
				/* copy the component to outPtr */
				copyThruSlash(&outPtr, &inPtr);
		if (inPtr == NULL)
			return(1);
	}
}

static int 
normalizePathname(char *pathname)
{
	char oldPathname[MAXPATHLEN+1], wd[MAXPATHLEN+1];
	/* If this path is relative, prepend the current working directory */ 
	if (pathname[0] != '/') 
	{
		strcpy(oldPathname, pathname);
		getcwd(wd, MAXPATHLEN+1);
		strcpy(pathname, wd);
		strcat(pathname, "/");
		strcat(pathname, oldPathname);
	}
/* compress out .. and . */
	return(compressPathname(pathname));
}

/***** 
* Decompose a Unix file name into a file name and a path
* This function originally comes from Nedit. 
* Used with kind permission of Mark Edel (Author of Nedit).  
*****/ 
int 
parseFilename(char *fullname, char *filename, char *pathname)
{
	int fullLen = strlen(fullname);
	int i, pathLen, fileLen;

/* find the last slash */
	for (i=fullLen-1; i>=0; i--) 
		if (fullname[i] == '/')
		break;
/* move chars before / (or ] or :) into pathname,& after into filename */
	pathLen = i + 1;
	fileLen = fullLen - pathLen;
	strncpy(pathname, fullname, pathLen);
	pathname[pathLen] = 0;
	strncpy(filename, &fullname[pathLen], fileLen);
	filename[fileLen] = 0;
	return(normalizePathname(pathname));
}

/*****
* Name: 		XMessage
* Return Type: 	void
* Description: 	displays a message dialog
* In: 
*	widget:		parent widget;
*	msg:		message to be displayed;
* Returns:
*	nothing
*****/
void
XMessage(Widget widget, String msg)
{
	static Widget dialog;
	Arg args[2];
	XmString xms;
	
	/* create dialog */
	if(!dialog)
	{
		dialog = XmCreateInformationDialog(widget, "Message", NULL, 0);

		/* remove the cancel and help button */
		XtUnmanageChild(
			XmMessageBoxGetChild(dialog, XmDIALOG_CANCEL_BUTTON));
		XtUnmanageChild(
			XmMessageBoxGetChild(dialog, XmDIALOG_HELP_BUTTON));

		/* set the title */
		XtVaSetValues(XtParent(dialog), XmNtitle, "Message", NULL);
	}
	/* create message */
	xms = XmStringCreateLtoR(msg, XmSTRING_DEFAULT_CHARSET);

	/* set it */
	XtSetArg(args[0], XmNmessageString, xms);
	XtSetArg(args[1], XmNmessageAlignment, XmALIGNMENT_CENTER);
	XtSetValues(dialog, args, 2);

	/* no longer needed, free it */
	XmStringFree(xms);

	/* show it */
	XtManageChild(dialog);
	XtPopup(XtParent(dialog), XtGrabNone);
	XMapRaised(XtDisplay(dialog), XtWindow(XtParent(dialog)));
}

/*****
* Name: 		setBusy
* Return Type: 	void
* Description: 	changes the cursor from or to a stopwatch to indicate we are
*				busy doing something lengthy processing which can't be
*				interrupted.
* In: 
*	state:		True to display the cursor as busy, False to display the
*				normal cursor.
* Returns:
*	nothing.
*****/
void
setBusy(Widget w, Boolean state)
{
	static Boolean busy;
	static Cursor cursor;
	Display *display = XtDisplay(w);

	if(!cursor)
	{
		cursor = XCreateFontCursor(display, XC_watch);
		busy = False;
	}
	if(busy != state)
	{
		busy = state;
		if(busy)
			XDefineCursor(display, XtWindow(w), cursor);
		else
			XUndefineCursor(display, XtWindow(w));
	}
	XFlush(display);
}

/*****
* Follow symbolic links (if any) to translate filename into the name of the
* real file that it represents.  Returns TRUE if the call was successful,
* meaning the links were translated successfully, or the file was not 
* linked to begin with (or there was no file).  Returns false if some
* error prevented the call from determining if there were symbolic links
* to process, or there was an error in processing them.  The error
* can be read from the unix global variable errno.
*****/
Boolean
followSymLinks(String filename)
{
	/*****
	* FIXME
	*
	* readlink doesn't seem to do anything at all on Linux 2.0.27,
	* libc 5.3.12
	*****/
	int cc;
	char buf[1024];

	cc = readlink(filename, buf, 1024);
	if (cc == -1) 
	{
#ifdef __sgi
    		if (errno == EINVAL || errno == ENOENT || errno == ENXIO)
#else
    		if (errno == EINVAL || errno == ENOENT)
#endif
/* no error, just not a symbolic link, or no file */
				return(True);
    		else
				return(False);
	} 
	else
	{
		buf[cc] = '\0';
		strcpy(filename, buf);
		return(True); 
	}
}   

/*****
* Name: 		getMimeType
* Return Type: 	int
* Description: 	make a guess at the mime type of a document by looking at
*				the extension of the given document.
* In: 
*	file:		file for which to get a mime-type;
* Returns:
*	mime type of the given file.
*****/
mimeType
getMimeType(String file)
{
	String chPtr;
	unsigned char img_type;

	if((chPtr = strstr(file, ".")) != NULL)
	{
		String start;

		/* first check if this is plain HTML or not */
		for(start = &file[strlen(file)-1]; *start && *start != '.'; start--);

		if(!strcasecmp(start, ".html") || !strcasecmp(start, ".htm"))
			return(MIME_HTML);
		if(!strcasecmp(start, ".htmlp"))
			return(MIME_HTML_PERFECT);
	}

	/* something else then? */
	/* check if this is an image XmHTML knows of */
	if((img_type = XmHTMLImageGetType(file, NULL, 0)) == IMAGE_ERROR)
		return(MIME_ERR);

	/*****
	* Not an image we know of, get first line in file and see if it's
	* html anyway
	*****/
	if(img_type == IMAGE_UNKNOWN)
	{
		FILE *fp;
		char buf[128];

		/* open file */
		if((fp = fopen(file, "r")) == NULL)
			return(MIME_ERR);

		/* read first line in file */
		if((chPtr = fgets(buf, 128, fp)) == NULL)
		{
			/* close again */
			fclose(fp);
			return(MIME_ERR);
		}
		/* close again */
		fclose(fp);

		/* see if it contains any of these strings */
		if(my_strcasestr(buf, "<!doctype") || my_strcasestr(buf, "<html") ||
			my_strcasestr(buf, "<head") || my_strcasestr(buf, "<body") ||
			my_strcasestr(buf, "<!--"))
			return(MIME_HTML);
		/* we don't know it */
		return(MIME_PLAIN);
	}

	/* known imagetype, but check if support is available */
	if((img_type == IMAGE_JPEG && !XmHTMLImageJPEGSupported()) ||
		(img_type == IMAGE_PNG && !XmHTMLImagePNGSupported()) ||
		(img_type == IMAGE_GZF && !XmHTMLImageGZFSupported())) 
		return(MIME_IMG_UNSUP);

	/* we know this image type */
	return(MIME_IMAGE);
}

/*****
* Name: 		collapseURL
* Return Type: 
* Description:	Copies up to 50 chars from the given url and puts in three
*				dots when the given url is larger than fifty chars.
* In: 
*	url:		url to be collapsed or copied;
* Returns:
*	copied url.
*****/
String
collapseURL(String url)
{
	static char name[51];
	int len;

	if(url == NULL)
		return("");

	len = strlen(url);

	if(len < 50)
	{
		strcpy(name, url);
		name[len] = '\0';	/* NULL terminate */
		return(name);
	}
	/* copy first 11 chars */
	strncpy(name, url, 11);
	/* put in a few dots */
	name[11] = '.';
	name[12] = '.';
	name[13] = '.';
	name[14] = '\0';
	/* copy last 36 chars */
	strcat(name, &url[len - 36]);
	name[50] = '\0';	/* NULL terminate */
	return(name);
}

/*****
* Name: 		loadFile
* Return Type: 	String
* Description: 	loads the contents of the given file.
* In: 
*	filename:	name of the file to load
*	mime_type:		mimetype of file to load, updated upon return.
* Returns:
*	contents of the loaded file.
*****/
String
loadFile(String filename, String *mime_type)
{
	FILE *file;
	int size;
	mimeType mime;
	static String content;
	char buf[1024];

	/* open the given file */
	if((file = fopen(filename, "r")) == NULL)
	{
		sprintf(buf, "%s:\ncannot display: %s", filename, strerror(errno));
		*mime_type = buf;
		return(NULL);
	}

	mime = getMimeType(filename);
	if(mime == MIME_ERR || mime == MIME_IMG_UNSUP)
	{
		if(mime == MIME_ERR)
			sprintf(buf, "%s:\ncannot display, unable to load file.", filename);
		else
			sprintf(buf, "%s:\ncannot display, support for this image type "
				"not present.", filename);
		*mime_type = buf;
		return(NULL);
	}

	if(mime == MIME_HTML)
		*mime_type = "text/html";
	else if(mime == MIME_HTML_PERFECT)
		*mime_type = "text/html-perfect";
	else if(mime == MIME_IMAGE)
		*mime_type = "image/";
	else
		*mime_type = "text/plain";

	/* only load contents of file if we need to load something */
	if(mime == MIME_HTML || mime == MIME_HTML_PERFECT || mime == MIME_PLAIN)
	{
		/* see how large this file is */
		fseek(file, 0, SEEK_END);
		size = ftell(file);
		rewind(file);

		/* allocate a buffer large enough to contain the entire file */
		if((content = malloc(size+1)) == NULL)
		{
			fprintf(stderr, "malloc failed for %i bytes\n", size);
			exit(EXIT_FAILURE);
		}

		/* now read the contents of this file */
		if((fread(content, 1, size, file)) != size)
			printf("Warning: did not read entire file!\n");
		content[size] = '\0';	/* sanity */
	}
	else
		content = strdup(filename);

	fclose(file);

	/* return contents of this file */
	return(content);
}

