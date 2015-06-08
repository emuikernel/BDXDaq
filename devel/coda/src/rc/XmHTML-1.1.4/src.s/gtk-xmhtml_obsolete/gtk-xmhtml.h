/*****
* gtk-xmhtml.h : XmHTML Widget public header file, Gtk version
*
* This file Version	$Revision: 1.1.1.1 $
*
* Creation date:		Tue Jan 13 20:14:55 GMT+0100 1998
* Last modification: 	$Date: 1998/08/25 17:48:19 $
* By:					$Author: rwm $
* Current State:		$State: Exp $
*
* Author:				newt
*
* Copyright (C) 1994-1998 by Ripley Software Development 
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
* Note:
*	To keep this a little bit organized and manageable, the public header
*	file is split in two parts:
*
*	<gtk-xmhtml/gtk-xmhtml.h>
*		This file, contains all public functions and the widget class
*		pointers;
*	<gtk-xmhtml/gtk-html.h>
*		Contains all structures, enumerations, typedefs and constants
*		referenced by the above functions.
*
*	The file <gtk-xmhtml/gtk-xmhtml-p.h> is private to gtk-xmhtml. You
*	could include it if you want to do some serious debugging.
*
* Thanks:
*	Many, many thanks to Miguel de Icaza <miguel@nuclecu.unam.mx>
*	for porting XmHTML to XmHTML/Gtk!!
*	Federico Mena <federico@nuclecu.unam.mx> for porting the XColorContext
*	code to gdk. It has become a standard part of gdk.
*
*****/
/*****
* $Source: /net/mizar/usr/local/source/coda_source/rc/XmHTML-1.1.4/XmHTML-1.1.4/src/gtk-xmhtml/gtk-xmhtml.h,v $
*****/
/*****
* ChangeLog 
* $Log: gtk-xmhtml.h,v $
* Revision 1.1.1.1  1998/08/25 17:48:19  rwm
*   Initial XmHTML-1.1.4 with RWM fixes.
*
* Revision 1.4  1998/01/07 01:45:35  unammx
* Gtk/XmHTML is ready to be used by the Gnome hackers now!
* Weeeeeee!
*
* This afternoon:
*
* 	- Changes to integrate gtk-xmhtml into an autoconf setup.
*
* 	- Changes to make gtk-xmhtml a library to be used by Gnome
* 	  (simply include <gtk-xmhtml/gtk-xmhtml.h and link
* 	   with -lgtkxmhtml and you are set).
*
* Revision 1.3  1997/12/29 22:16:21  unammx
* This version does:
*
*    - Sync with Koen to version Beta 1.1.2c of the XmHTML widget.
*      Includes various table fixes.
*
*    - Callbacks are now properly checked for the Gtk edition (ie,
*      signals).
*
* Revision 1.2  1997/12/25 01:34:09  unammx
* Good news for the day:
*
*    I have upgraded our XmHTML sources to XmHTML 1.1.1.
*
*    This basically means that we got table support :-)
*
* Still left to do:
*
*    - Set/Get gtk interface for all of the toys in the widget.
*    - Frame support is broken, dunno why.
*    - Form support (ie adding widgets to it)
*
* Miguel.
*
* Revision 1.1  1997/11/28 03:38:55  gnomecvs
* Work in progress port of XmHTML;  No, it does not compile, don't even try -mig
*
*****/ 

#ifndef _gtk_xmhtml_h_
#define _gtk_xmhtml_h_

#include <gdk/gdk.h>
#include <gtk/gtk.h>
#include <gtk/gtkobject.h>

#define WITH_GTK	1
#include <gtk-xmhtml/toolkit.h>

#define XmHTMLVERSION	1
#define XmHTMLREVISION	1
#define XmHTMLUPDATE_LEVEL 2
#define XmHTMLVersion \
	(XmHTMLVERSION * 1000 + XmHTMLREVISION * 100 + XmHTMLUPDATE_LEVEL)

/* used by Imake to get Shared library version numbering */
#ifndef _LIBRARY

#define XmHTMLVERSION_STRING \
	"XmHTML/Gtk Pre-Beta Version 1.1.2d (C)Ripley Software Development"

/* structures, constants and other typedefs */
#include <gtk-xmhtml/gtk-html.h>
	
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*****
* Public instance declaration
*****/
typedef struct _GtkXmHTML			*XmHTMLWidget;
typedef struct _GtkXmHTML			GtkXmHTML;
typedef struct _GtkXmHTMLClass		GtkXmHTMLClass;
typedef struct _XmHTMLPart			XmHTMLPart;

#define GTK_XMHTML(obj) \
	GTK_CHECK_CAST (obj, gtk_xmhtml_get_type (), GtkXmHTML)

#define GTK_XMHTML_CLASS(class) \
	 GTK_CHECK_CLASS_CAST (class, gtk_xmhtml_get_type (), GtkXmHTMLClass)

#define GTK_IS_XMHTML(obj) \
	GTK_CHECK_TYPE (obj, gtk_xmhtml_get_type ())

/* For compatibility and consistency functions */
#define XmIsHTML(obj)             GTK_IS_XMHTML(obj)
#define XmHTML(obj)               GTK_XMHTML(obj)

/********    Public Function Declarations    ********/

/* initialize a new XmHTML instance */
extern GtkWidget *gtk_xmhtml_new(void);

/* ?? */
extern void gtk_xmhtml_freeze(GtkXmHTML *html);

/* ?? */
extern void gtk_xmhtml_thaw(GtkXmHTML *html);

/* set a new text in widget */
extern void gtk_xmhtml_source(GtkXmHTML *html, char *source);

/* set string direction: left to right or right to left */
extern void gtk_xmhtml_set_string_direction(GtkXmHTML *html, int direction);

/* set string aligment: left, center, right, justify */
extern void gtk_xmhtml_set_alignment(GtkXmHTML *html, int alignment);

/* set/unset text justification */
extern void gtk_xmhtml_outline(GtkXmHTML *html, int flag);

/* change proportional font */
extern void gtk_xmhtml_set_font_familty(GtkXmHTML *html, char *family,
	char *sizes);

/* change fixed font */
extern void gtk_xmhtml_set_font_familty_fixed(GtkXmHTML *html, char *family,
	char *sizes);

/* change character set */
extern void gtk_xmhtml_set_font_charset(GtkXmHTML *html, char *charset);

/* enable/disable support for BGCOLOR, TEXT,... <BODY> attributes */
extern void gtk_xmhtml_set_allow_body_colors(GtkXmHTML *html, int enable);

/* set/unset anchor highlighting */
extern void gtk_xmhtml_set_hilight_on_enter(GtkXmHTML *html, int flag);

/* change underline style for regular anchors */
extern void gtk_xmhtml_set_anchor_underline_type(GtkXmHTML *html,
	int underline_type);

/* change underline style for visited anchors */
extern void gtk_xmhtml_set_anchor_visited_underline_type (GtkXmHTML *html,
	int underline_type);

/* change underline style for anchors with the target attribute set */
extern void gtk_xmhtml_set_anchor_target_underline_type  (GtkXmHTML *html,
	int underline_type);

/* enable/disable support for the COLOR attribute */
extern void gtk_xmhtml_set_allow_color_switching(GtkXmHTML *html, int flag);

/* enable/disable support for the <FONT FACE=...> attribute */
extern void gtk_xmhtml_set_allow_font_switching(GtkXmHTML *html, int flag);

/* change dithering mode */
extern void gtk_xmhtml_set_dithering(GtkXmHTML *html, XmHTMLDitherType flag);

/* change maximum number of colors. 0 -> take all you have. 256 maximum */
extern void gtk_xmhtml_set_max_image_colors(GtkXmHTML *html, int max_colors);

/* enable/disable image support */
extern void gtk_xmhtml_set_allow_images(GtkXmHTML *html, int flag);

/* modify polling intervals for the Progressive Image Loader */
extern void gtk_xmhtml_set_plc_intervals(GtkXmHTML *html, int min_delay,
	int max_delay, int def_delay);

/* set/modify the default body image. */
extern void gtk_xmhtml_set_def_body_image_url(GtkXmHTML *html, char *url);

/* enable/disable ``buttoned'' anchors */
extern void gtk_xmhtml_set_anchor_buttons(GtkXmHTML *html, int flag);

/* enable/disable and/or modify the default anchor cursor */
extern void gtk_xmhtml_set_anchor_cursor(GtkXmHTML *html, GdkCursor * cursor,
	int flag);

/* line to be shown at the top of the display area */
extern void gtk_xmhtml_set_topline(GtkXmHTML *html, int line);

/* freeze/unfreeze animations */
extern void gtk_xmhtml_set_freeze_animations(GtkXmHTML *html, int flag);

/* get a *pointer* to the source of the currently displayed document */
extern char *gtk_xmhtml_get_source(GtkXmHTML *html);

/* modify screen gamma value */
extern void gtk_xmhtml_set_screen_gamma(GtkXmHTML *html, float gamma);

/* set/modify image loader, gif decoder and progressive loader functions */
extern void gtk_xmhtml_set_image_procs(GtkXmHTML *html,
	XmImageProc image_proc, XmImageGifProc gif_proc,
	XmHTMLGetDataProc get_data, XmHTMLEndDataProc end_data);

/* procedure to be called whenever an HTML 4.0 event is encountered */
extern void gtk_xmhtml_set_event_proc(GtkXmHTML *html,
	XmHTMLEventProc event_proc);

/* enable/modify dithering */
extern void gtk_xmhtml_set_perfect_colors(GtkXmHTML *html, int flag);

/* change uncompress program to use for the builting GIF decoder */
extern void gtk_xmhtml_set_uncompress_command(GtkXmHTML *html, char *cmd);

/* enable/disable strict HTML checking */
extern void gtk_xmhtml_set_strict_checking(GtkXmHTML *html, int flag);

/* tells which warnings you want to get */
extern void gtk_xmhtml_set_bad_html_warnings(GtkXmHTML *html, int flag);

/* propagete document colors to any <FORM> components */
extern void gtk_xmhtml_set_allow_form_coloring(GtkXmHTML *html, int flag);

/* set/unset drawing of bounding boxes around imagemap areas */
extern void gtk_xmhtml_set_imagemap_draw(GtkXmHTML *html, int flag);

/* modify mimetype of document. */
extern void gtk_xmhtml_set_mime_type(GtkXmHTML *html, char *mime_type);

/* set Alpha channel processing (PNG images only) */
extern void gtk_xmhtml_set_alpha_processing(GtkXmHTML *html, int flag);

/* set 24 to 8bit conversion mode */
extern void gtk_xmhtml_set_rgb_conv_mode(GtkXmHTML *html, int val);

/* These ones are used internally: */
extern void *gtk_xmhtml_signal_get_handlers(GtkXmHTML *html, int type);

extern void  gtk_xmhtml_set_geometry(GtkWidget *widget, int x, int y,
	int width, int height);

extern guint gtk_xmhtml_get_type(void);

extern void  gtk_xmhtml_manage(GtkContainer *container, GtkWidget *widget);

enum{
	GTK_ANCHOR_NOLINE,
	GTK_ANCHOR_SINGLE_LINE,
	GTK_ANCHOR_DOUBLE_LINE,
	GTK_ANCHOR_DASHED_LINE,
	GTK_ANCHOR_DOUBLE_DASHED_LINE
};

/* default image loader */
extern XmImageInfo *gtk_xmhtml_image_default_proc(GtkXmHTML *html, String file,
	unsigned char *buf, int size);

/* Return image type */
extern unsigned char gtk_xmhtml_image_get_type(String file, unsigned char *buf,
	int size);

/* returns True if image_default_proc supports JPEG images */
extern Boolean gtk_xmhtml_jpeg_supported(void);

/* returns True if image_default_proc supports PNG images */
extern Boolean gtk_xmhtml_png_supported(void);

/* returns True if image_default_proc supports GZF images */
extern Boolean gtk_xmhtml_gzf_supported(void);

/* Replace image with new_image */
extern XmImageStatus gtk_xmhtml_image_replace(GtkXmHTML *html,
	XmImageInfo *image, XmImageInfo *new_image);

/* update image */
extern XmImageStatus gtk_xmhtml_image_update(GtkXmHTML *html,
	XmImageInfo *image);

/* release all memory occupied by the images */
extern void gtk_xmhtml_image_free_all_images(GtkXmHTML *html);

/* add an imagemap to a HTML TWidget. */
extern void gtk_xmhtml_image_add_imagemap(GtkXmHTML *html, String image_map);

/* free an XmImageInfo structure */
extern void	gtk_xmhtml_image_free_imageinfo(GtkXmHTML *html, XmImageInfo *info);

/* return the total size of a given XmImageInfo structure */
extern int gtk_xmhtml_get_imageinfo_size(XmImageInfo *info);

/* suspend progressive image loading */
extern void gtk_xmhtml_image_progressive_suspend(GtkXmHTML *html);

/* reactivate progressive image loading */
extern void gtk_xmhtml_image_progressive_continue(GtkXmHTML *html);

/* terminate progressive image loading */
extern void gtk_xmhtml_image_progressive_kill(GtkXmHTML *html);

/* convert a GIF image to a GZF image */
extern Boolean gtk_xmhtml_gif_to_gzf(String infile, unsigned char *buf,
	int size, String outfile);

/*****
* Anchor related convenience functions
* These routines can be used to jump to named anchors.
*****/
/* return the internal id of a named anchor given it's name or -1. */
extern int gtk_xmhtml_anchor_get_id(GtkXmHTML *html, String anchor);

/* scroll to a named anchor, given it's id */
extern void gtk_xmhtml_anchor_get_id(GtkXmHTML *html, int anchor_id);

/* scroll to a named anchor, given it's name */
extern void gtk_xmhtml_anchor_scroll_to_name(GtkXmHTML *html, String anchor);

/*****
* Text related convenience functions
*****/

/* scroll to the requested line number */
extern void gtk_xmhtml_text_scroll_to_line(GtkXmHTML *html, int line);

/* set text into a html TWidget */
extern void gtk_xmhtml_text_set_string(GtkXmHTML *html, String text);

/* return a *pointer* to the original text */
extern String gtk_xmhtml_text_get_source(GtkXmHTML *html);

/* return a copy of the current parser output */
extern String gtk_xmhtml_text_get_string(GtkXmHTML *html);


/*****
* Miscelleneous convenience functions
*****/
/* return the library version number */
extern int gtk_xmhtml_get_version(void);

/* return the URL type of the given href */
extern URLType gtk_xmhtml_get_url_type(String href);

/* return the value of the <TITLE></TITLE> element */
extern String gtk_xmhtml_get_title(GtkXmHTML *html);

/* return an info structure for the specified location */
extern XmHTMLInfoStructure *gtk_xmhtml_xy_to_info(GtkXmHTML *html,
	int x, int y);

/****
* Return the contents of the document head. Returns True when a <head></head>
* section is present in the current document, False if not. When mask_bits
* only contains HeadClear, the given attribute structure is wiped clean and
* this function will return False immediatly.
* The only exception concerns the <!DOCTYPE> tag and the HeadDocType mask bit:
* if this bit is set, the value of this tag is returned whether or not a
* head is present.
****/
extern Boolean gtk_xmhtml_get_head_attributes(GtkXmHTML *html,
	XmHTMLHeadAttributes *head, unsigned char mask_bits);

/* return the TWidget id of a framechild given its name */
extern GtkXmHTML *XmHTMLFrameGetChild(GtkXmHTML *html, String name);

/* force a recomputation of screen layout and trigger a redisplay */
extern void gtk_xmhtml_redisplay(GtkXmHTML *html);

/* return info about the font cache for display of the given TWidget */
extern XmHTMLFontCacheInfo *gtk_xmhtml_get_font_cache_info(GtkXmHTML *html);

/* free the given font cache info */
extern void gtk_xmhtml_free_font_cache_info(XmHTMLFontCacheInfo *info);

#ifdef __cplusplus
}
#endif /* __cplusplus */

/* Don't add anything after this endif! */
#endif /* _gtk_xmhtml_h_ */
