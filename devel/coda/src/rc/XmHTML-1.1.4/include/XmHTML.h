/*****
* XmHTML.h : XmHTML Widget public header file
*
* This file Version	$Revision: 1.1.1.1 $
*
* Creation date:		Tue Nov 19 23:18:37 GMT+0100 1996
* Last modification: 	$Date: 1998/08/25 17:48:03 $
* By:					$Author: rwm $
* Current State:		$State: Exp $
*
* Author:				newt
* (C)Copyright 1995-1996 Ripley Software Development
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
* $Source: /net/mizar/usr/local/source/coda_source/rc/XmHTML-1.1.4/XmHTML-1.1.4/src/XmHTML.h,v $
*****/
/*****
* ChangeLog 
* $Log: XmHTML.h,v $
* Revision 1.1.1.1  1998/08/25 17:48:03  rwm
*   Initial XmHTML-1.1.4 with RWM fixes.
*
* Revision 1.21  1998/04/27 06:56:31  newt
* Updated version number
*
* Revision 1.20  1998/04/04 06:27:53  newt
* XmHTML Beta 1.1.3
*
* Revision 1.19  1997/10/26 23:49:52  newt
* Moved internal symbol defines to XmHTMLP.h
*
* Revision 1.18  1997/10/23 00:24:44  newt
* XmHTML Beta 1.1.0 release
*
* Revision 1.17  1997/08/31 17:32:43  newt
* log edit
*
* Revision 1.16  1997/08/30 00:40:16  newt
* Changed proto's for XmHTMLImageReplace and XmHTMLImageUpdate. They now
* return an XmImageStatus instead of void.
*
* Revision 1.15  1997/08/01 12:54:55  newt
* Progressive image loading changes.
*
* Revision 1.14  1997/05/28 01:41:04  newt
* Added the XmHTMLImageGZFSupported, XmHTMLGIFtoGZF, XmHTMLGifReadOK and
* XmHTMLGifGetDataBlock convienience routines to convert GIF images to an
* alternate format. Added the XmHTMLAllocColor and XmHTMLFreeColor protos for
* easy color allocation. Modified the XmImageCreate routines to use a
* XmImageConfig structure.
*
* Revision 1.13  1997/04/29 14:22:35  newt
* Completely revised due to XmHTMLParserObject integration
*
* Revision 1.12  1997/04/03 05:31:30  newt
* XmHTMLFrameGetChild proto. XmIMAGE_SHARED_DATA option bit added.
*
* Revision 1.11  1997/03/28 07:06:14  newt
* XmNframeCallback, XmHTMLFrameCallbackStruct. XmHTMLParserCallback changes.
*
* Revision 1.10  1997/03/20 08:05:02  newt
* XmHTMLImageFreeImageInfo, XmNrepeatDelay
*
* Revision 1.9  1997/03/11 19:50:27  newt
* Changes in XmImageInfo; grouped and renamed convenience functions
*
* Revision 1.8  1997/03/04 18:45:55  newt
* XmNimagemapBoundingBoxForeground and XmCImagemapDrawBoundingBoxes 
* resources added
*
* Revision 1.7  1997/03/04 00:56:30  newt
* Delayed Image Loading: added the delayed field to the XmImageInfo structure. 
* Removed a number of obsolete defines
*
* Revision 1.6  1997/03/02 23:07:10  newt
* Image/Imagemap related changes. XmHTMLAnchorCallbackStruct changed
*
* Revision 1.5  1997/02/11 02:01:26  newt
* Added the XmNhandleShortTags resource
*
* Revision 1.4  1997/02/04 02:55:57  newt
* added the basefont element
*
* Revision 1.3  1997/01/09 06:55:53  newt
* expanded copyright marker
*
* Revision 1.2  1997/01/09 06:47:58  newt
* New resource: XmNparserCallback and corresponding error structure/defines
*
* Revision 1.1  1996/12/19 02:17:14  newt
* Initial Revision
*
*****/ 
/*****
* Public interfaces are listed at the end of this file
*****/

#ifndef _XmHTML_h_
#define _XmHTML_h_

#define XmHTMLVERSION	1
#define XmHTMLREVISION	1
#define XmHTMLUPDATE_LEVEL 4
#define XmHTMLVersion \
	(XmHTMLVERSION * 1000 + XmHTMLREVISION * 100 + XmHTMLUPDATE_LEVEL)

/* used by Imake to get Shared library version numbering */
#ifndef _LIBRARY

#define XmHTMLVERSION_STRING \
	"XmHTML Beta Version 1.1.4 (C)Ripley Software Development"

/* Required includes */
#include <X11/Intrinsic.h>
#include <Xm/Xm.h>
#include <HTML.h>

_XFUNCPROTOBEGIN

/* XmHTML type defines */
typedef struct _XmHTMLClassRec *XmHTMLWidgetClass;
typedef struct _XmHTMLRec *XmHTMLWidget;

externalref WidgetClass xmHTMLWidgetClass;

/* XmHTML Widget subclassing macro */
#ifndef XmIsHTML
#define XmIsHTML(w)	XtIsSubclass(w, xmHTMLWidgetClass)
#endif /* XmIsHTML */

/********    Public Function Declarations    ********/

/*****
* Convenience/public functions
* There are four categories of them:
* - image related
* - anchor related
* - text related
* - functions that don't fit in any of the above three
*****/

/*****
* Image related convenience functions
*****/
extern XmImageInfo *XmHTMLImageDefaultProc(Widget w, String file,
	unsigned char *buf, int size);

/* Return image type */
extern unsigned char XmHTMLImageGetType(String file, unsigned char *buf,
	int size);

/* returns True if XmHTMLImageDefaultProc supports JPEG images */
extern Boolean XmHTMLImageJPEGSupported(void);

/* returns True if XmHTMLImageDefaultProc supports PNG images */
extern Boolean XmHTMLImagePNGSupported(void);

/* returns True if XmHTMLImageDefaultProc supports GZF images */
extern Boolean XmHTMLImageGZFSupported(void);

/* Replace image with new_image */
extern XmImageStatus XmHTMLImageReplace(Widget w, XmImageInfo *image, 
		XmImageInfo *new_image);

/* update image */
extern XmImageStatus XmHTMLImageUpdate(Widget w, XmImageInfo *image);

/* release all memory occupied by the images */
extern void XmHTMLImageFreeAllImages(Widget w);

/* add an imagemap to a HTML widget. */
extern void XmHTMLImageAddImageMap(Widget w, String image_map);

/* free an XmImageInfo structure */
extern void	XmHTMLImageFreeImageInfo(Widget w, XmImageInfo *info);

/* return the total size of a given XmImageInfo structure */
extern int XmHTMLImageGetImageInfoSize(XmImageInfo *info);

/* suspend progressive image loading */
extern void XmHTMLImageProgressiveSuspend(Widget w);

/* reactivate progressive image loading */
extern void XmHTMLImageProgressiveContinue(Widget w);

/* terminate progressive image loading */
extern void XmHTMLImageProgressiveKill(Widget w);

/*****
* Special image functions
*****/
/*****
* Create and return a XmImage for use other than with XmHTML.
* When width and height are non-zero, the image read is scaled to the specified
* dimensions. 
*****/
extern XmImage *XmImageCreate(Widget w, String file,
	Dimension width, Dimension height, XmImageConfig *config);

/*****
* Create an XmImage from the given XmImageInfo. When the image type is
* IMAGE_UNKNOWN, but the url field represents a local file, this routine
* calls XmImageCreate with the url field as the file argument.
* Only honors the ImageFrameSelect, ImageCreateGC and ImageBackground
* XmImageConfig flag and appropriate fields of that structure.
*****/
extern XmImage *XmImageCreateFromInfo(Widget w, XmImageInfo *info,
	Dimension width, Dimension height, XmImageConfig *config);

/* destroy a XmImage */
extern void XmImageDestroy(XmImage *image);

/*****
* Tiles "dest" with the given XmImage. Please note that "src" *must* have been
* created with the ImageCreateGC flag, otherwise this function does nothing and
* returns -1. Returns 0 upon success.
* Internally, this routine is more or less a combination of XSetTile,
* XSetTSOrigin and XFillRectangle in one.
*
* (UNTESTED)
*****/
extern int XmImageSetBackgroundImage(XmImage *src, Drawable dest, int src_x,
	int src_y, int width, int height, int dest_x, int dest_y);

/*****
* XCopyArea for an XmImage which also takes a possible clipmask into account.
* Please note that "image" *must* have been created with the ImageCreateGC
* flag, otherwise this function does nothing and returns -1.
* Returns 0 upon success.
*
* (UNTESTED)
*****/
extern int XmImageDrawImage(XmImage *image, Drawable dest, int src_x,
	int src_y, int dest_x, int dest_y);

/*****
* Write an image to file. Returns False upon failure.
* Saving an image in the GIF format is *not* possible due to Unisys's
* stupid LZW licensing policy. Exporting an image as PNG, JPEG or GZF is only
* possible if support for the required libraries has been compiled in.
*
* (UNIMPLEMENTED, always returns False)
*****/
extern Boolean XmImageExport(XmImageInfo *info, String file, Dimension width,
	Dimension height, unsigned char type);

/* convert a GIF image to a GZF image */
extern Boolean XmHTMLGIFtoGZF(String infile, unsigned char *buf, int size,
	String outfile);

/*****
* Anchor related convenience functions
* These routines can be used to jump to named anchors.
*****/
/* return the internal id of a named anchor given it's name or -1. */
extern int XmHTMLAnchorGetId(Widget w, String anchor);

/* scroll to a named anchor, given it's id */
extern void XmHTMLAnchorScrollToId(Widget w, int anchor_id);

/* scroll to a named anchor, given it's name */
extern void XmHTMLAnchorScrollToName(Widget w, String anchor);

/*****
* Text related convenience functions
*****/
/* This macro sets the given text into a HTML widget */
#define XmHTMLTextSet(WIDGET,TEXT) XtVaSetValues((WIDGET), \
		XmNvalue, (TEXT), NULL)

/* scroll to the requested line number */
extern void XmHTMLTextScrollToLine(Widget w, int line);

/* set text into a html widget */
extern void XmHTMLTextSetString(Widget w, String text);

/*****
* same as XmHTMLTextSetString with one fundamental difference: text doesn't
* have to be NULL terminated. The size of the input string is instead
* given by len. If text is NULL or len is 0, the current contents are
* cleared.
*****/
extern void XmHTMLTextSetStringWithLength(Widget w, String text, size_t len);

/* return a *pointer* to the original text */
extern String XmHTMLTextGetSource(Widget w);

/* return a copy of the current parser output */
extern String XmHTMLTextGetString(Widget w);

/*****
* Return a formatted copy of the current widget contents
* (UNIMPLEMENTED, always returns NULL)
*****/
extern String XmHTMLTextGetFormatted(Widget w, unsigned char papertype,
	XmHTMLPaperSize *paperdef, unsigned char type, unsigned char PSoptions);

extern String XmHTMLTextGetSelection(Widget w);

extern void XmHTMLTextClearSelection(Widget w, Time time);

extern Boolean XmHTMLTextSetSelection(Widget w, XmHTMLTextPosition first,
	XmHTMLTextPosition last, Time time);

extern void XmHTMLTextSetHighlight(Widget w, XmHTMLTextPosition first,
	XmHTMLTextPosition last, XmHighlightMode mode);

extern Boolean XmHTMLTextCopy(Widget w, Time time);

extern Boolean XmHTMLTextShowPosition(Widget w, XmHTMLTextPosition position);

extern Boolean XmHTMLTextCopy(Widget w, Time time);

/* text search functions */
extern XmHTMLTextFinder XmHTMLTextFinderCreate(Widget w);

extern void XmHTMLTextFinderDestroy(XmHTMLTextFinder finder);

extern Boolean XmHTMLTextFinderSetPattern(XmHTMLTextFinder finder,
	String to_find);

extern Boolean XmHTMLTextFinderSetPatternFlags(XmHTMLTextFinder finder,
	int flags, Boolean ignore_case, XmHTMLDirection direction);

/* return the id of the last known regex error */
extern int XmHTMLTextFinderGetError(XmHTMLTextFinder finder);

/* same as above but now returns a string which must be freed by caller */
extern String XmHTMLTextFinderGetErrorString(XmHTMLTextFinder finder);

extern XmHTMLRegexStatus XmHTMLTextFindString(Widget w,
	XmHTMLTextFinder finder);

extern void XmHTMLTextFinderReset(XmHTMLTextFinder finder);

extern Boolean XmHTMLTextFindToPosition(Widget w,
	XmHTMLTextFinder finder, XmHTMLTextPosition *start,
	XmHTMLTextPosition *end);

/*****
* Miscelleneous convenience functions
*****/
/* return the library version number */
extern int XmHTMLGetVersion(void);

/* return the URL type of the given href */
extern URLType XmHTMLGetURLType(String href);

/* return the value of the <TITLE></TITLE> element */
extern String XmHTMLGetTitle(Widget w);

/* return an info structure for the specified location */
extern XmHTMLInfoStructure *XmHTMLXYToInfo(Widget w, int x, int y);

/* return document structure (list of images & hyperlinks) */
extern XmHTMLDocumentInfo *XmHTMLGetDocumentInfo(Widget w);

/* free a document structure */
extern void XmHTMLFreeDocumentInfo(XmHTMLDocumentInfo *doc_info);

/****
* Return the contents of the document head. Returns True when a <head></head>
* section is present in the current document, False if not. When mask_bits
* only contains HeadClear, the given attribute structure is wiped clean and
* this function will return False immediatly.
* The only exception concerns the <!DOCTYPE> tag and the HeadDocType mask bit:
* if this bit is set, the value of this tag is returned whether or not a
* head is present.
****/
extern Boolean XmHTMLGetHeadAttributes(Widget w, XmHTMLHeadAttributes *head,
	unsigned char mask_bits);

/* return the widget id of a framechild given its name */
extern Widget XmHTMLFrameGetChild(Widget w, String name);

/* Create a HTML widget if parent is not null and no subclass of XmGadget */
extern Widget XmCreateHTML(Widget parent, String name, ArgList arglist, 
	Cardinal argcount);

/* force a recomputation of screen layout and trigger a redisplay */
extern void XmHTMLRedisplay(Widget w);

/* return info about the font cache for display of the given widget */
extern XmHTMLFontCacheInfo *XmHTMLGetFontCacheInfo(Widget w);

/* free the given font cache info */
extern void XmHTMLFreeFontCacheInfo(XmHTMLFontCacheInfo *info);

/*
* Allocate given color (symbolic name of rgb triplet) using the widget's
* colormap. Works with the XmNmaxImageColors resource.
*/
extern Pixel XmHTMLAllocColor(Widget w, String color, Pixel def_pixel);

/* free a color allocated with XmHTMLAllocColor */
extern void XmHTMLFreeColor(Widget w, Pixel pixel);

_XFUNCPROTOEND

#endif /* _LIBRARY */

/* Don't add anything after this endif! */
#endif /* _XmHTML_h_ */
