/*****
* toolkit.h : XmHTML Motif/Gtk function & data type wrappers
*
* This file Version	$Revision: 1.1.1.1 $
*
* Creation date:		Thu Jan  8 04:32:19 GMT+0100 1998
* Last modification: 	$Date: 1998/08/25 17:48:14 $
* By:					$Author: rwm $
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
* About this file:
*
*	XmHTML is originally an Xt/Motif based Widget. To make porting to
*	other toolkits a bit easier, XmHTML uses a set of defines that allow
*	you to replace all X/Xt/Xm functions used by XmHTML.
*
*	There is one assumption though: that you never include a header containing
*	Xt or Xm specifics. If you do this however, you will need to override
*	a whole bunch of routines, typedefs and constants (don't worry, they
*	are all listed).
*
*****/
/*****
* $Source: /net/mizar/usr/local/source/coda_source/rc/XmHTML-1.1.4/XmHTML-1.1.4/src/toolkit.h,v $
*****/
/*****
* ChangeLog 
* $Log: toolkit.h,v $
* Revision 1.1.1.1  1998/08/25 17:48:14  rwm
*   Initial XmHTML-1.1.4 with RWM fixes.
*
* Revision 1.2  1998/04/27 07:03:52  newt
* more tka stuff
*
* Revision 1.1  1998/04/04 06:27:29  newt
* Initial Revision
*
*****/ 

#ifndef _toolkit_h_
#define _toolkit_h_

/*****
* default mode = X/Motif
* Add a !defined(WITH_XX) if the toolkit is unlisted and add an appropriate
* set of defines after the last toolkit.
*****/
#if !defined(WITH_GTK)

#define XmHTMLPrivateHeader "XmHTMLP.h"
#define HAVE_XCCP_H

/*****
* X types & constants
*****/
#define XXColor						XColor
#define XColormap					Colormap
#define XPixmap						Pixmap
#define XWindow						Window
#define XXImage						XImage
#define XVisual						Visual
#define XGC							GC
#define XXFontStruct				XFontStruct
#define XXEvent						XEvent
#define XXButtonPressedEvent		XButtonPressedEvent
#define XXButtonReleasedEvent		XButtonReleasedEvent

/* byte ordering for this host */
#define XLSBFirst					LSBFirst
#define XMSBFirst					MSBFirst

/*****
* This macro should return a XFontStruct
*****/
#define FontIsXFont(font)			font

#define XTEVENTPROC(f,a,b,c,d) \
	f(a,b,c,d)

/*****
* X Function Wrappers
*****/
#define XXDefaultRootWindow(dpy) \
	DefaultRootWindow(dpy)

#define XXWidthOfScreen(widget) \
	WidthOfScreen(XtScreen((Widget)widget))

#define XXHeightOfScreen(widget) \
	HeightOfScreen(XtScreen((Widget)widget))

#define XXCurrentTime \
	CurrentTime

/*****
* XImage wrappers & access macros.
*****/

#define XXImageData(image) \
	(image->data)

#define XXImageBitsPerPixel(image) \
	(image->bits_per_pixel)

#define XXImageBytesPerLine(image) \
	(image->bytes_per_line)

#define XXImageByteOrder(image) \
	(image->byte_order)

#define XXImageBitmapBitOrder(image) \
	(image->bitmap_bit_order)

/* check support for various combinations of bits per pixel */
#define XXImageCheck2bpp(image) \
	(image->bits_per_pixel == 2)
#define XXImageCheck4bpp(image) \
	(image->bits_per_pixel == 4)
#define XXImageCheck24bpp(image) \
	(image->bits_per_pixel == 24)
#define XXImageCheck32bpp(image) \
	(image->bits_per_pixel == 32)

/*****
* The next two macros should verify whether or not the *parent* of
* the given widget is of the correct class. If not, they should return
* immediatly.
* The first macro just returns, the second macro returns a value.
*****/
#define CHECK_CLASS(widget,class) do { \
	if(XtClass(XtParent(widget)) != class) \
		return; \
}while(0)

#define CHECK_CLASS_WITH_RETURN(widget,class,retval) do { \
	if(XtClass(XtParent(widget)) != class) \
		return(retval); \
}while(0)

/*****
* This macro evaluates whether or not a callback is installed.
*****/
#define CHECK_CALLBACK(widget,func,signaltype) \
	((widget)->html.func)

/*****
* Widget internal access wrappers
*****/
#define HTMLW(html)	XmHTMLWidget html
#define WIDGET(w)	Widget w

/* Main access method. *REQUIRES* a variable of html to be known */
#define HTML_ATTR(field)	((XmHTMLWidget)html)->html.field
#define CORE_ATTR(field)	((Widget)html)->core.field
#define MGR_ATTR(field)		((XmHTMLWidget)html)->manager.field

/* subclass access methods */
#define ATTR_CORE(widget,field) \
	(widget)->core.field

#define ATTR_MGR(widget,field) \
	(widget)->manager.field

#define ATTR_HTML(widget,field) \
	((XmHTMLWidget)widget)->html.field

/* widely used subclass properties */

/* get default XmHTMLFont */
#define GetDefaultFont(widget) \
	ATTR_HTML(widget,default_font)

/* get XFont used by the default XmHTMLFont */
#define GetDefaultXFont(widget) \
	ATTR_HTML(widget,default_font->xfont)

#define GetBottomShadowGC(widget) \
	ATTR_MGR(widget,bottom_shadow_GC)

#define GetTopShadowGC(widget) \
	ATTR_MGR(widget,top_shadow_GC)

#define GetHighlightGC(widget) \
	ATTR_MGR(widget,highlight_GC)

#define GetHighlightColor(widget) \
	ATTR_MGR(widget,highlight_color)

#define GetForegroundColor(widget) \
	ATTR_MGR(widget,foreground)

#define GetBackgroundColor(widget) \
	ATTR_CORE(widget,background_pixel)

#define GetCoreWidth(widget) \
	ATTR_CORE(widget,width)

#define GetCoreHeight(widget) \
	ATTR_CORE(widget,height)

#define GetCoreX(widget) \
	ATTR_CORE(widget,x)

#define GetCoreY(widget) \
	ATTR_CORE(widget,y)

#define GetCoreColormap(widget) \
	ATTR_CORE(widget,colormap)

/* get the visual used by a widget */
#define GetVisual(widget, dest) do { \
	XtVaGetValues((widget),XmNvisual, &dest, NULL); \
	/* get parent visual if current widget doesn't have one. This will */ \
	/* *always* return a valid visual */ \
	if(dest == NULL) \
		visual = XCCGetParentVisual(widget); \
}while(0)

#define GetVisualDepth(widget) \
	ATTR_HTML(widget,xcc->visualInfo->depth)

#define GetVisualMapEntries(visual) \
	visual->map_entries

#define WidgetRepaint(widget) \
	_XmHTMLClearArea((widget), 0, 0, GetCoreWidth(widget), \
		GetCoreHeight(widget))

#define ForceWidgetRepaint(widget) do{ \
	WidgetRepaint(widget); \
	XSync(XtDisplay((Widget)(widget)), True); \
} while (0)

#else

/* gtk toolkit? */
#if defined(WITH_GTK)

#include <gtk/gtk.h>
#include <gdk/gdkx.h>

/* Private Header to be included for Gtk */
#define XmHTMLPrivateHeader "gtk-xmhtml-p.h"

/* No illegal xt access 'cause we're not using xt at all! */
#ifndef NO_XLIB_ILLEGAL_ACCESS
#define NO_XLIB_ILLEGAL_ACCESS 1
#endif

#define XXEvent					GdkEvent
#define XXButtonPressedEvent	GdkEventButton
#define XXButtonReleasedEvent	GdkEventButton

#include <X11/Xlib.h>

/*
* XPoint and GdkPoint are equivalent, xpoint has short int and gdkpoint
* has gint16
*/

typedef GdkFont     XXFontStruct;
typedef GdkCursor   *XCursor;
typedef GdkVisual   XVisual;
typedef GdkImage    XXImage;
typedef GdkColormap *XColormap;
typedef GdkPixmap   *XPixmap;
typedef GdkWindow   *XWindow;
typedef GdkDrawable *XDrawable;
typedef GdkGC       *XGC;
typedef GdkAtom     *XAtom;
typedef GdkColor    XXColor;
typedef XVisualInfo XXVisualInfo;

/*****
* Byte ordering used by images
*****/
#define XLSBFirst						GDK_LSB_FIRST
#define XMSBFirst						GDK_MSB_FIRST

/*****
* Provide (or override) a number of Xt typedefs
*****/
#ifdef _XtIntrinsic_h

/* <X11/Intrinsic.h> was included..., define instead of typedef */

#define Widget			GtkWidget*
#define WidgetList		GtkWidget**
#define XtAppContext	gpointer
#define XtCallbackList	GList
#define XtIntervalId	gint
#define XtPointer		gpointer
#define String			char*
#define Boolean			gboolean
#define Cardinal		guint
#define Dimension		gushort
#define Position		gshort

#else
typedef GtkWidget		*Widget;
typedef GtkWidget		**WidgetList;
typedef gpointer		XtAppContext;
typedef GList			XtCallbackList;
typedef gint			XtIntervalId;
typedef gpointer		XtPointer;
typedef char			*String;
typedef gboolean		Boolean
typedef guint			Cardinal;
typedef gushort			Dimension;
typedef gshort			Position;

#endif

/* and for proper definition in gtk-xmhtml-p.h */
typedef gpointer		GdkAppContext;

#define XTEVENTPROC(f,a,b,c,d) \
	f(a, b)

/*****
* This macro should return a XFontStruct
*****/
#define FontIsXFont(font)	((XFontStruct *)(((GdkFontPrivate *)font)->xfont))

/*****
* X Function Wrappers
*****/

#define XXDefaultRootWindow(dpy) \
	GDK_ROOT_PARENT()

#define XXWidthOfScreen(widget) \
	gdk_screen_width ()

#define XXHeightOfScreen(widget) \
	gdk_screen_height ()

#define XXCurrentTime \
	GDK_CURRENT_TIME

/*****
* XImage wrappers & access macros.
*****/

#define XXImageData(image) \
	(image->mem)

/* GdkImage has bytes per pixel, not bits per pixel */
#define XXImageBitsPerPixel(image) \
	(image->bpp*8)

#define XXImageBytesPerLine(image) \
	(image->bpl)

#define XXImageByteOrder(image) \
	(image->byte_order)

#define XXImageBitmapBitOrder(image) \
	(((GdkImagePrivate*)image)->ximage->bitmap_bit_order)

/*****
* check support for various combinations of bits per pixel
* FIXME:
* Gdk misses support for 2 and 4 bits per pixel.
*****/
#define XXImageCheck2bpp(image) \
	FALSE
#define XXImageCheck4bpp(image) \
	FALSE
#define XXImageCheck24bpp(image) \
	(image->bpp == 3)
#define XXImageCheck32bpp(image) \
	(image->bpp == 4)

/*****
* Xt Function wrappers
*****/

#define XtName(w)				"(unknown)"
#define ClassName(w)			gtk_widget_get_name(w)

#define XtAppWarning(CTX,MSG)	fputs(MSG, stderr)

#define XtWarning(MSG)			fputs(MSG, stderr)

#define XtAppError(CTX,MSG) do{ fputs(MSG,stderr) ; exit(1); }while(0)

#define XtError(MSG) do{ fputs(MSG,stderr) ; exit(1); }while(0)

/* gdk doesn't have the concept of Application Contexts */
#define XtWidgetToApplicationContext(widget) NULL

/*****
* Motif Wrappers
*****/
/*****
* In Motif, XmUpdateDisplay causes all events that are still left on the
* queue to be flushed *AND* to wait until all requests have been delivered.
* I guess an XFlush followed by an XSync will do the same (sort of).
*****/
#define XmUpdateDisplay(widget) do { \
	gdk_flush(); \
}while(0)

/*****
* Unused by gtk.
*****/
#define CHECK_CLASS(widget,class) /* no-op */

#define CHECK_CLASS_WITH_RETURN(widget,class,retval)	/* no-op */

/*****
* This macro evaluates whether or not a signal is installed.
*****/
#define CHECK_CALLBACK(widget,func,signaltype) \
	(gtk_xmhtml_signal_get_handlers \
		(widget, gtk_xmhtml_signals [GTK_XMHTML_##signaltype]))

/*****
* Widget internal access wrappers
*****/
#define HTMLW(html)	XmHTMLWidget html
#define WIDGET(w)	GtkWidget *w

/* subclass access methods */
#define ATTR_CORE(widget,field) \
	(GTK_WIDGET((widget))->allocation).(field)

#define ATTR_MGR(widget,field) \
	(GTK_WIDGET((widget)))->style->(field)

#define ATTR_HTML(widget,field) \
	(GTK_HTML((widget)))->html.(field)

/* widely used subclass properties */

/* get default XmHTMLFont */
#define GetDefaultFont(widget) \
	ATTR_HTML(widget,default_font)

/* get XFont used by the default XmHTMLFont */
#define GetDefaultXFont(widget) \
	((XFontStruct*)((GdkFontPrivate*)((GetDefaultFont(widget))->xfont))

#define GetBottomShadowGC(widget) \
	ATTR_MGR(widget, dark_gc [GTK_STATE_NORMAL])

#define GetTopShadowGC(widget) \
	ATTR_MGR(widget, light_gc [GTK_STATE_NORMAL])

#define GetHighlightGC(widget) \
	ATTR_MGR(widget, bg_gc [GTK_STATE_PRELIGHT])

#define GetHighlightColor(widget) \
	ATTR_MGR(widget, bg [GTK_STATE_PRELIGHT].pixel)

#define GetForegroundColor(widget) \
	ATTR_MGR(widget, fg [GTK_STATE_NORMAL].pixel)

#define GetBackgroundColor(widget) 
	ATTR_MGR(widget, bg [GTK_STATE_NORMAL].pixel)

#define GetCoreWidth(widget) \
	ATTR_CORE(widget,width)

#define GetCoreHeight(widget) \
	ATTR_CORE(widget,height)

#define GetCoreX(widget) \
	ATTR_CORE(widget,x)

#define GetCoreY(widget) \
	ATTR_CORE(widget,y)

#define GetCoreColormap(widget) \
	gtk_widget_get_colormap (GTK_WIDGET (widget))

#define GetVisual(widget, dest) \
	dest = gtk_widget_get_visual (widget)

#define GetVisualDepth(widget) \
	ATTR_HTML(widget,xcc->visual->depth)

#define GetVisualMapEntries(visual) \
	GDK_VISUAL_XVISUAL(visual)->map_entries

#define WidgetRepaint(widget) \
	_XmHTMLClearArea((widget), 0, 0, GetCoreWidth(widget), \
		GetCoreHeight(widget))

#define ForceWidgetRepaint(widget) do { \
	WidgetRepaint(widget); \
	gtk_widget_draw (GTK_WIDGET (widget), NULL); \
	gdk_flush(); \
}while (0)

/*****
* XColorContext Wrappers
*****/

#define XCCCreate(w,v,c) \
	gdk_color_context_new (v, c)

#define XCCFree(c) \
	gdk_color_context_free (c)

#define XCCGetDepth(c) \
	(c)->visual->depth

#define XCCGetNumColors(c) \
	(c)->num_colors

#define XCCGetParentVisual(w) \
	gtk_widget_get_visual(w)

#define XCCGetPixels(cc,r,g,b,n,co,a) \
	gdk_color_context_get_pixels (cc,r,g,b,n,co,a)

#define XCCGetPixelsIncremental(cc,r,g,b,n,u,co,na) do{ \
	gdk_color_context_get_pixels_incremental (cc,r,g,b,n,u,co,na); \
}while (0)

#define XCCAddPalette(c,p,n) \
	gdk_color_context_add_palette (c,p,n)

#define XCCInitDither(cc) \
	gdk_color_context_init_dither (cc)

#define XCCGetIndexFromPalette(cc,r,g,b,f) \
	gdk_color_context_get_index_from_palette(cc,r,g,b,f)

#define XCCFreeDither(cc) \
	gdk_color_context_free_dither (cc)

typedef GdkColorContextDither XCCDither;

#endif /* WITH_GTK */

/*****
* Add a set of defines for your toolkit here
*****/

#endif /* outer wrapper */

/* Don't add anything after this endif! */
#endif /* _toolkit_h_ */
