#ifndef production
static char rcsId[]="$Header: /net/mizar/usr/local/source/coda_source/rc/XmHTML-1.1.4/XmHTML-1.1.4/src/motif.c,v 1.1.1.1 1998/08/25 17:48:14 rwm Exp $";
#endif
/*****
* motif.c : put_a_description_here
*
* This file Version	$Revision: 1.1.1.1 $
*
* Creation date:		Thu Feb 26 22:33:21 GMT+0100 1998
* Last modification: 	$Date: 1998/08/25 17:48:14 $
* By:					$Author: rwm $
* Current State:		$State: Exp $
*
* Author:				newt
*
* Copyright (C) 1994-1998 by Ripley Software Development 
* All Rights Reserved
*
* This file is part of insert_program_name_here
*
* This library is free software; you can redistribute it and/or
* modify it under the terms of the GNU [Library] General Public
* License as published by the Free Software Foundation; either
* version 2 of the License, or (at your option) any later version.
*
* This library is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
* Library General Public License for more details.
*
* You should have received a copy of the GNU [Library] General Public
* License along with this library; if not, write to the Free
* Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*
*****/
/*****
* ChangeLog 
* $Log: motif.c,v $
* Revision 1.1.1.1  1998/08/25 17:48:14  rwm
*   Initial XmHTML-1.1.4 with RWM fixes.
*
* Revision 1.2  1998/04/27 07:01:23  newt
* Added some more functions for proper tka handling
*
* Revision 1.1  1998/04/04 06:27:24  newt
* Initial Revision
*
*****/ 
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>

#include <Xm/XmP.h>			/* XmField, XmPartOffset and private motif funcs. */
#include <Xm/DrawP.h>		/* Private render functions */
#include <Xm/XmStrDefs.h>	/* For motif XmN macros */
#include <Xm/DrawingA.h>
#include <Xm/ScrollBar.h>
#include <Xm/ScrolledW.h>
#include <Xm/RepType.h>

/* Our private header files */
#include "toolkit.h"
#include XmHTMLPrivateHeader

#include "XmHTMLfuncs.h"

/*** External Function Prototype Declarations ***/

/*** Public Variable Declarations ***/

/*** Private Datatype Declarations ****/

/*** Private Function Prototype Declarations ****/

/*** Private Variable Declarations ***/

/*****
* Name:			DestroyImage
* Return Type: 	void
* Description: 	XDestroyImage is only defined as a macro, which we
*				obviously can't call directly as a function. Instead
*				we define a function that calls the real macro.
* In: 
*	image:		ptr to XImage to be destroyed;
* Returns:
*	nothing.
*****/
static void
DestroyImage(XImage *image)
{
	XDestroyImage(image);
}

#ifdef NO_XM_ILLEGAL_ACCESS
static void
_XmHTMLDrawShadows(Display *dpy, Drawable drawable, GC top_shadow_GC,
	GC bottom_shadow_GC, Position x, Position y, Dimension width,
	Dimension height, Dimension shadow_thick, Byte shadow_type)
{
	switch(shadow_type)
	{
		case XmSHADOW_IN:
			/* top & left border */
			XFillRectangle(dpy, drawable, bottom_shadow_GC, x, y, width, 1);
			XFillRectangle(dpy, drawable, bottom_shadow_GC, x, y, 1, height-1);

			/* bottom & right border */
			XFillRectangle(dpy, drawable, top_shadow_GC, x + 1, y + height - 1,
				width - 1, 1);
			XFillRectangle(dpy, drawable, top_shadow_GC, x - 1, y + 1, 1,
				height - 2); 
			break;
		case XmSHADOW_OUT:
			/* top & left border */
			XFillRectangle(dpy, drawable, top_shadow_GC, x, y, width, 1);
			XFillRectangle(dpy, drawable, top_shadow_GC, x, y, 1, height-1);

			/* bottom & right border */
			XFillRectangle(dpy, drawable, bottom_shadow_GC, x + 1,
				y + height - 1, width - 1, 1);
			XFillRectangle(dpy, drawable, bottom_shadow_GC, x - 1,
				y + 1, 1, height - 2); 
			break;
		default:
			break;
	}
}
#endif

static ToolkitAbstraction*
_CreateMotifToolkitAbstraction(void)
{
	static ToolkitAbstraction *tka;

	tka = (ToolkitAbstraction*)malloc(sizeof(ToolkitAbstraction));

	tka->dpy = NULL;
	tka->win = None;

	/* GC properties */
	tka->fill_style[GC_FILL_SOLID]           = FillSolid;
	tka->fill_style[GC_FILL_TILED]           = FillTiled;
	tka->fill_style[GC_FILL_STIPPLED]        = FillStippled;
	tka->fill_style[GC_FILL_OPAQUE_STIPPLED] = FillOpaqueStippled;

	tka->cap_style[GC_CAP_NOT_LAST]   = CapNotLast;
	tka->cap_style[GC_CAP_BUTT]       = CapButt;
	tka->cap_style[GC_CAP_ROUND]      = CapRound;
	tka->cap_style[GC_CAP_PROJECTING] = CapProjecting;

	tka->line_style[GC_LINE_SOLID]       = LineSolid;
	tka->line_style[GC_LINE_ON_OFF_DASH] = LineOnOffDash;
	tka->line_style[GC_LINE_DOUBLE_DASH] = LineDoubleDash;

	tka->join_style[GC_JOIN_MITER] = JoinMiter;
	tka->join_style[GC_JOIN_ROUND] = JoinRound;
	tka->join_style[GC_JOIN_BEVEL] = JoinBevel;

	tka->coord_mode[GC_COORDMODE_ORIGIN] = CoordModeOrigin;
	tka->coord_mode[GC_COORDMODE_PREVIOUS] = CoordModePrevious;

	/* GC functions */
	tka->gc_func[GC_GXcopy] = GXcopy;

	/* GC functions */
	tka->CreateGC      = XCreateGC;
	tka->FreeGC        = XFreeGC;
	tka->CopyGC        = XCopyGC;
	tka->SetFunction   = XSetFunction;
	tka->SetClipMask   = XSetClipMask;
	tka->SetClipOrigin = XSetClipOrigin;
	tka->SetTile       = XSetTile;
	tka->SetTSOrigin   = XSetTSOrigin;
	tka->SetFillStyle  = XSetFillStyle;
	tka->SetFont       = XSetFont;
	tka->SetForeground = XSetForeground;
	tka->SetBackground = XSetBackground;
	tka->SetLineAttributes = XSetLineAttributes;

	/* Font Allocation functions */
	tka->LoadQueryFont = XLoadQueryFont;
	tka->FreeFont      = XFreeFont;

	/* Cursor & pointer functions */
	tka->UngrabPointer = XUngrabPointer;
	tka->DefineCursor  = XDefineCursor;
	tka->UndefineCursor= XUndefineCursor;
	tka->FreeCursor    = XFreeCursor;

	/* Color functions */
	tka->ParseColor    = XParseColor;
	tka->AllocColor    = XAllocColor;
	tka->QueryColor    = XQueryColor;
	tka->QueryColors   = XQueryColors;
	tka->FreeColors    = XFreeColors;

	/* Pixmap functions */
	tka->CreatePixmap  = XCreatePixmap;
	tka->FreePixmap    = XFreePixmap;
	tka->CreatePixmapFromBitmapData = XCreatePixmapFromBitmapData;

	/* XImage functions */
	tka->CreateImage   = XCreateImage;
	tka->DestroyImage  = DestroyImage;
	tka->PutImage      = XPutImage;

	/* string/text functions */
	tka->TextWidth     = XTextWidth;

	/* Render functions */
	tka->DrawString    = XDrawString;
	tka->DrawLine      = XDrawLine;
	tka->DrawLines     = XDrawLines;
	tka->DrawRectangle = XDrawRectangle;
	tka->FillRectangle = XFillRectangle;
	tka->DrawArc       = XDrawArc;
	tka->FillArc       = XFillArc;

	/* misc. functions */
	tka->CopyArea      = XCopyArea;
	tka->ClearArea     = XClearArea;
	tka->Sync          = XSync;

	/* X Intrinsic wrappers */
	tka->IsRealized      = XtIsRealized;
	tka->IsManaged       = XtIsManaged;
	tka->ManageChild     = XtManageChild;
	tka->UnmanageChild   = XtUnmanageChild;
	tka->MoveWidget      = XtMoveWidget;
	tka->ResizeWidget    = XtResizeWidget;
	tka->ConfigureWidget = XtConfigureWidget;
	tka->SetMappedWhenManaged = XtSetMappedWhenManaged;
	tka->RemoveTimeOut   = XtRemoveTimeOut;
	tka->AddTimeOut      = XtAppAddTimeOut;

	/* Motif Wrappers */
#ifndef NO_XM_ILLEGAL_ACCESS
	tka->DrawShadows     = _XmDrawShadows;
#else
	tka->DrawShadows     = _XmHTMLDrawShadows;
#endif

	return(tka);
}

/*****
* Name:
* Return Type:
* Description:
* In:
*
* Returns:
*
*****/
ToolkitAbstraction*
_XmHTMLCreateToolkitAbstraction(void)
{
	return(_CreateMotifToolkitAbstraction());
}

void
_XmHTMLDestroyToolkitAbstraction(ToolkitAbstraction *tka)
{
	free(tka);
}

void
_XmHTMLToolkitAbstractionSetDrawable(ToolkitAbstraction *tka, Drawable drawable)
{
	tka->win = drawable;
}

void
_XmHTMLToolkitAbstractionSetDisplay(ToolkitAbstraction *tka, Widget w)
{
	tka->dpy = XtDisplay(w);
}

/*****
* Name: 		_XmHTMLRecomputeColors
* Return Type: 	void
* Description: 	computes new values for top and bottom shadows and the
*				highlight color based on the current background color.
* In: 
*	html:		XmHTMLWidget id;
* Returns:
*	nothing.
*****/
void
_XmHTMLRecomputeColors(XmHTMLWidget html) 
{
	/* 
	* We can only compute the colors when we have a GC. If we don't
	* have a GC, the widget is not yet realized. Use managers defaults
	* then.
	*/
	if(html->html.gc != NULL)
	{
		Pixel top = None, bottom = None, highlight = None;
		Arg args[3];

		XmGetColors(XtScreen((Widget)html), html->core.colormap,
			html->html.body_bg, NULL, &top, &bottom, &highlight);
		XtSetArg(args[0], XmNtopShadowColor, top);
		XtSetArg(args[1], XmNbottomShadowColor, bottom);
		XtSetArg(args[2], XmNhighlightColor, highlight);
		XtSetValues((Widget)html, args, 3);
	}
}

/*****
* Name: 		_XmHTMLRecomputeHighlightColor
* Return Type: 	void
* Description: 	computes the select color based upon the given color.
* In: 
*	html:		XmHTMLWidget id;
* Returns:
*	nothing.
*****/
void
_XmHTMLRecomputeHighlightColor(XmHTMLWidget html, Pixel bg_color) 
{
	/* 
	* We can only compute the colors when we have a GC. If we don't
	* have a GC, the widget is not yet realized. Use managers defaults
	* then.
	*/
	if(html->html.gc != NULL)
	{
		Pixel highlight = None;
		Arg args[1];

		XmGetColors(XtScreen((Widget)html), html->core.colormap,
			bg_color, NULL, NULL, NULL, &highlight);
		XtSetArg(args[0], XmNhighlightColor, highlight);
		XtSetValues((Widget)html, args, 1);
	}
}

/*****
* Name:			_XmHTMLRecomputeShadowColors
* Return Type: 	void
* Description: 	recomputes the top and bottom shadow colors based on the
*				given *foreground* color
* In: 
*	html:		XmHTMLWidget id;
*	base:		base color to base computation on.
* Returns:
*	Nothing, but the GC's from Manager are updated to reflect the change.
*****/
void
_XmHTMLRecomputeShadowColors(XmHTMLWidget html, Pixel base) 
{
	/* 
	* We can only compute the colors when we have a GC. If we don't
	* have a GC, the widget is not yet realized. Use managers defaults
	* then.
	*/
	if(html->html.gc != NULL)
	{
		Pixel top = None, bottom = None;
		Arg args[2];

		XmGetColors(XtScreen((Widget)html), html->core.colormap,
			base, NULL, &top, &bottom, NULL);
		XtSetArg(args[0], XmNtopShadowColor, top);
		XtSetArg(args[1], XmNbottomShadowColor, bottom);
		XtSetValues((Widget)html, args, 2);
	}
}
