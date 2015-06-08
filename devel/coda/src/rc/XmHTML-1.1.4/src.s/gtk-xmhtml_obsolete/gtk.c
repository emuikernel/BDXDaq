#ifndef lint
static char rcsId[]="$Header: /net/mizar/usr/local/source/coda_source/rc/XmHTML-1.1.4/XmHTML-1.1.4/src/gtk-xmhtml/gtk.c,v 1.1.1.1 1998/08/25 17:48:20 rwm Exp $";
#endif
/*****
* gtk.c : put_a_description_here
*
* This file Version	$Revision: 1.1.1.1 $
*
* Creation date:		Fri Feb 27 00:07:58 GMT+0100 1998
* Last modification: 	$Date: 1998/08/25 17:48:20 $
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
*******************************************************************************
*                                                                             *
*                 !!!THIS FILE IS FAR FROM FINISHED!!!                        *
*                                                                             *
*******************************************************************************
*****/
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>

/* Our private header files */
#include "toolkit.h"
#include XmHTMLPrivateHeader

#include "XmHTMLfuncs.h"

/*** External Function Prototype Declarations ***/

/*** Public Variable Declarations ***/

/*** Private Datatype Declarations ****/

/*** Private Function Prototype Declarations ****/

/*** Private Variable Declarations ***/

static GdkFont *curr_font = NULL;

static GdkGC*
gdk_gc_new_wrapper(void *dpy, GdkWindow *window, GdkGCValuesMask values_mask,
	GdkGCValues *values)
{
	return(gdk_gc_new_with_values(window, values, values_mask));
}

static void
gdk_gc_destroy_wrapper(void *dpy, GdkGC *gc)
{
	gdk_gc_destroy(gc);
}

static void
gdk_gc_copy_wrapper(void *dpy, GdkGC *dest, gint flags, GdkGC *src)
{
	gdk_gc_copy(dest, src);
}

static void
gdk_gc_set_function_wrapper(void *dpy, GdkGC *gc, GdkFunction function)
{
	gdk_gc_set_function(gc, function);
}

static void
gdk_gc_set_clip_mask_wrapper(void *dpy, GdkGC *gc, GdkBitmap *mask)
{
	gdk_gc_set_clip_mask(gc, mask);
}

static void
gdk_gc_set_clip_origin_wrapper(void *dpy, GdkGC *gc, gint x, gint y)
{
	gdk_gc_set_clip_origin(gc, x, y);
}

static void
gdk_gc_set_tile_wrapper(void *dpy, GdkGC *gc, GdkPixmap *tile)
{
	gdk_gc_set_tile(gc, tile);
}

static void
gdk_gc_set_ts_origin_wrapper(void *dpy, GdkGC *gc, gint x, gint y)
{
	gdk_gc_set_ts_origin(gc, x, y);
}

static void
gdk_gc_set_fill_wrapper(void *dpy, GdkGC *gc, GdkFill, fill)
{
	gdk_gc_set_fill(gc,fill);
}

static void
gdk_gc_set_font_wrapper(void *dpy, GdkGC *gc, GdkFont *font)
{
	curr_font = font;
	gdk_gc_set_font(gc, font);
}

static void
gdk_gc_set_foreground_wrapper(void *dpy, GdkGC *gc, unsigned long fg)
{
	GdkColor m;
	m.pixel = fg; 
	gdk_gc_set_foreground(gc, &m);
}

static void
gdk_gc_set_background_wrapper(void *dpy, GdkGC *gc, unsigned long bg)
{
	GdkColor m;
	m.pixel = bg; 
	gdk_gc_set_background(gc, &m);
}

static void
gdk_gc_set_line_attributes_wrapper(void *dpy, GdkGC *gc, int thickness,
	int linestyle, int capstyle, int joinstyle)
{
	gdk_gc_set_line_attributes(gc, thickness, linestyle, capstyle, joinstyle);
}

/* Font Allocation functions */
static GdkFont*
gdk_font_load_wrapper(void *dpy, gchar *name)
{
	return(gdk_font_load(name));
}

static void
gdk_font_free_wrapper(void *dpy, GdkFont *font)
{
	gdk_font_unref(font);
}

/* Render functions */

static void
gdk_draw_text_wrapper(void *dpy, GdkWindow *window, GdkGC *gc, int xs,
	int ys, char *text, int len)
{
	gdk_draw_text(window, curr_font, gc, xs, ys, text, len);
}

static void
gdk_draw_line_wrapper(void *dpy, GdkWindow *window, GdkGC *gc, int xs,
	int ys, int xe, int ye)
{
	gdk_draw_line(window, gc, xs, ys, xe, ye);
}

static void
gdk_draw_lines_wrapper(void *dpy, GdkWindow *window, GdkGC *gc,
	GdkPoint *points, gint npoints, gint coordmode)
{
	gdk_draw_polygon(window, gc, FALSE, points, npoints);
}

static void
gdk_draw_rectangle_wrapper(void *dpy, GdkWindow *window, GdkGC *gc,
	gint xs, gint ys, gint width, gint height)
{
	gdk_draw_rectangle(window, gc, FALSE, xs, ys, width, height);
}

static void
gdk_fill_rectangle_wrapper(void *dpy, GdkWindow *window, GdkGC *gc,
	gint xs, gint ys, gint width, gint height)
{
	gdk_draw_rectangle(window, gc, TRUE, xs, ys, width, height);
}

static void
gdk_draw_arc_wrapper(void *dpy, GdkWindow *window, GdkGC *gc, gint xs,
	gint ys, gint width, gint height, gint angle1, gint angle2)
{
	gdk_draw_arc(window, gc, FALSE, xs, ys, width, height, angle1, angle2);
}

static void
gdk_fill_arc_wrapper(void *dpy, GdkWindow *window, GdkGC *gc, gint xs,
	gint ys, gint width, gint height, gint angle1, gint angle2)
{
	gdk_draw_arc(window, gc, TRUE, xs, ys, width, height, angle1, angle2);
}

static void
gdk_query_colors_wrapper(void *dpy, GdkColormap *colormap,
	GdkColor *colors, gint ncolors)
{
	XColor *xcolors;
	gint    i;

	xcolors = g_new(XColor, ncolors);
	for (i = 0; i < ncolors; i++)
		xcolors[i].pixel = colors[i].pixel;

	XQueryColors(gdk_display, GDK_COLORMAP_XCOLORMAP(colormap), xcolors,
		ncolors);

	for (i = 0; i < ncolors; i++) {
		colors[i].red   = xcolors[i].red;
		colors[i].green = xcolors[i].green;
		colors[i].blue  = xcolors[i].blue;
	}
	g_free(xcolors);
}

static void
gdk_query_color_wrapper(void *dpy, GdkColormap *colormap,
	GdkColor *color)
{
	gdk_query_colors_wrapper(dpy, colormap, color, 1);
}

static GdkPixmap
gdk_pixmap_new_wrapper(void *dpy, GdkWindow *win, gint width, gint height,
	gint depth)
{
	return(gdk_pixmap_new(win, width, height, depth);
}

static void
gdk_pixmap_unref_wrapper(void *dpy, GdkPixmap *pixmap)
{
	gdk_pixmap_unref(pixmap);
}

static GdkPixmap*
gdk_pixmap_create_from_data_wrapper(void *dpy, GdkWindow *win,
	guchar *data, guint width, guint height, gulong fg, gulong bg,
	guint depth)
{
	GdkColor fg_color, bg_color;
	fg_color.pixel = fg;
	bg_color.pixel = bg;

	return(gdk_pixmap_create_from_data(win, data, width, height, depth,
		&fg_color, &bg_color));
}

/* XImage functions */
static GdkImage*
gdk_image_new_wrapper(void *dpy, GdkVisual *visual, guint depth, gint format,
	gint offset, gchar *data, guint width, guint height, gint bitmap_pad,
	gint bytes_per_line)
{
	return(gdk_image_new(GDK_IMAGE_FASTEST, visual, width, height));
}

static void
gdk_draw_image_wrapper(void *dpy, GdkDrawable *drawable, GdkGC *gc,
	GdkImage *image, gint src_x, gint src_y, gint dest_x, gint dest_y,
	guint width, guint height)
{
	gdk_draw_image(drawable, gc, image, src_x, src_y, dest_x, dest_y,
		(gint)width, (gint)height);
}

/*****
* X Library Intrinsics function wrappers
*****/

static Boolean
gtk_widget_realized_wrapper(GdkWidget *w)
{
	return(GTK_WIDGET_REALIZED (GTK_WIDGET (w)));
}


static Boolean
gtk_widget_managed_wrapper(GdkWidget *w)
{
	return(GTK_WIDGET_MAPPED (GTK_WIDGET (w)));
}

static void
gtk_widget_move_wrapper(GdkWidget *w, gint x, gint y)
{
	if(GTK_WIDGET_NO_WINDOW (w) && GTK_WIDGET_MAPPED (w) &&
		((w->allocation.x != x) || (w->allocation.y != y)) &&
		(w->allocation.width != 0 && w->allocation.height != 0))
		gdk_window_clear_area(w->window,
			w->allocation.x, w->allocation.y,
			w->allocation.width, w->allocation.height);

	if(GTK_WIDGET_REALIZED (w) && !GTK_WIDGET_NO_WINDOW (w))
	{
		gdk_window_move(w->window, x, y);
		gtk_widget_show(w);
	}
}

static void
gtk_widget_resize_wrapper(GdkWidget *w, gint width, gint height)
{
	GtkAllocation alloc;

	/* keep current position */
	alloc.x = w->allocation.x;
	alloc.y = w->allocation.y;
	alloc.width  = width;
	alloc.height = height;

	gtk_widget_size_allocate(w, &alloc);
}

static void
gtk_widget_configure_wrapper(GdkWidget *w, gint x, gint h, gint width,
	gint height, gint border_width)
{
	GtkAllocation alloc;

	alloc.x = x;
	alloc.y = y;
	alloc.width  = width;
	alloc.height = height;

	/* gdk/gtk doesn't care about borderwidth */

	gtk_widget_size_allocate(w, &alloc);
}

static int
gtk_widget_mapped_when_managed_wrapper(GtkWidget *w, Boolean map_state)
{
	if(map_state)
	{
		if(GTK_WIDGET_MAPPED (w))
			return(0);
		gtk_widget_show(w);
	}
	else
	{
		if(!GTK_WIDGET_MAPPED (w))
			return(0);
		gtk_widget_hide(w);
	}
	return(1);
}

static ToolkitAbstraction*
_CreateGtkToolkitAbstraction(void)
{
	static ToolkitAbstraction *tka;

	tka = (ToolkitAbstraction*)malloc(sizeof(ToolkitAbstraction));

	/* GC properties */
	tka->fill_style[GC_FILL_SOLID]           = GDK_SOLID;
	tka->fill_style[GC_FILL_TILED]           = GDK_TILED;
	tka->fill_style[GC_FILL_STIPPLED]        = GDK_STIPPLED;
	tka->fill_style[GC_FILL_OPAQUE_STIPPLED] = GDK_OPAQUE_STIPPLED;

	tka->cap_style[GC_CAP_NOT_LAST]   = GDK_CAP_NOT_LAST;
	tka->cap_style[GC_CAP_BUTT]       = GDK_CAP_BUTT;
	tka->cap_style[GC_CAP_ROUND]      = GDK_CAP_ROUND;
	tka->cap_style[GC_CAP_PROJECTING] = GDK_CAP_PROJECTING;

	tka->line_style[GC_LINE_SOLID]       = GDK_LINE_SOLID;
	tka->line_style[GC_LINE_ON_OFF_DASH] = GDK_LINE_ON_OFF_DASH;
	tka->line_style[GC_LINE_DOUBLE_DASH] = GDK_LINE_DOUBLE_DASH;

	tka->join_style[GC_JOIN_MITER] = GDK_JOIN_MITER;
	tka->join_style[GC_JOIN_ROUND] = GDK_JOIN_ROUND;
	tka->join_style[GC_JOIN_BEVEL] = GDK_JOIN_BEVEL;

	/* GC functions */
	tka->gc_func[GC_GXcopy] = GDK_COPY;

	/* GdkGC functions */
	tka->CreateGC      = gdk_gc_new_wrapper;
	tka->FreeGC        = gdk_gc_destroy_wrapper;
	tka->CopyGC        = gdk_gc_copy_wrapper;
	tka->SetFunction   = gdk_gc_set_function_wrapper;
	tka->SetClipMask   = gdk_gc_set_clipmask_wrapper;
	tka->SetClipOrigin = gdk_gc_set_clip_origin_wrapper;
	tka->SetTile       = gdk_gc_set_tile_wrapper;
	tka->SetTSOrigin   = gdk_gc_set_ts_origin_wrapper;
	tka->SetFillStyle  = gdk_gc_set_fill_wrapper;
	tka->SetFont       = gdk_gc_set_font_wrapper;
	tka->SetForeground = gdk_gc_set_foreground_wrapper;
	tka->SetBackground = gdk_gc_set_background_wrapper;
	tka->SetLineAttributes = gdk_gc_set_line_attributes_wrapper;

	/* Font Allocation functions */
	tka->LoadQueryFont = gdk_font_load_wrapper;
	tka->FreeFont      = gdk_font_free_wrapper;

	/* Cursor & pointer functions */
	tka->UngrabPointer = gdk_pointer_ungrab_wrapper;
	tka->DefineCursor  = gdk_window_set_cursor_wrapper;
	tka->UndefineCursor= gdk_window_unset_cursor_wrapper;
	tka->FreeCursor    = gdk_cursor_destroy_wrapper;

	/* Color functions */
	tka->ParseColor    = gdk_color_parse_wrapper;
	tka->AllocColor    = gdk_color_alloc_wrapper;
	tka->QueryColor    = gdk_query_color_wrapper;
	tka->QueryColors   = gdk_query_colors_wrapper;
	tka->FreeColors    = gdk_free_colors_wrapper;

	/* Pixmap functions */
	tka->CreatePixmap  = gdk_pixmap_new_wrapper;
	tka->FreePixmap    = gdk_pixmap_unref_wrapper;
	tka->CreatePixmapFromBitmapData = gdk_pixmap_create_from_data_wrapper;

	/* XImage functions */
	tka->CreateImage   = gdk_image_new_wrapper;
	tka->DestroyImage  = gdk_image_destroy;
	tka->PutImage      = gdk_draw_image_wrapper;

	/* string/text functions */
	tka->TextWidth     = gdk_text_width;

	/* Render functions */
	tka->DrawString    = gdk_draw_text_wrapper;
	tka->DrawLine      = gdk_draw_line_wrapper;
	tka->DrawLines     = gdk_draw_lines_wrapper;
	tka->DrawRectangle = gdk_draw_rectangle_wrapper;
	tka->FillRectangle = gdk_fill_rectangle_wrapper;
	tka->DrawArc       = gdk_draw_arc_wrapper;
	tka->FillArc       = gdk_fill_arc_wrapper;

	/* misc. functions */
	tka->CopyArea      = gdk_window_copy_area_wrapper;
	tka->ClearArea     = gdk_clear_area_wrapper;
	tka->Sync          = gdk_flush_wrapper;

	/* X Intrinsic wrappers */
	tka->IsRealized      = gtk_widget_realized_wrapper;
	tka->IsManaged       = gtk_widget_managed_wrapper;
	tka->ManageChild     = gtk_widget_show;
	tka->UnmanageChild   = gtk_widget_hide;
	tka->MoveWidget      = gtk_widget_move_wrapper;
	tka->ResizeWidget    = gtk_widget_resize_wrapper;
	tka->ConfigureWidget = gtk_widget_configure_wrapper;
	tka->SetMappedWhenManaged = gtk_widget_mapped_when_managed_wrapper;

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
	return(_CreateGtkToolkitAbstraction(void));
}

void
_XmHTMLToolkitAbstractionSetWindow(ToolkitAbstraction *tka, Drawable drawable)
{
	tka->win = drawable;
}

void
_XmHTMLToolkitAbstractionSetDisplay(ToolkitAbstraction *tka, void *dpy)
{
	tka->dpy = GDK_DISPLAY();
}

/*****
* Color computation
*****/

/*****
* This color shading method is taken from gtkstyle.c.  Some
* modifications made by me - Federico
*****/

static void
rgb_to_hls (gdouble *r, gdouble *g, gdouble *b)
{
	gdouble min;
	gdouble max;
	gdouble red;
	gdouble green;
	gdouble blue;
	gdouble h, l, s;
	gdouble delta;

	red = *r;
	green = *g;
	blue = *b;

	if (red > green) {
		if (red > blue)
			max = red;
		else
			max = blue;

		if (green < blue)
			min = green;
		else
			min = blue;
	} else {
		if (green > blue)
			max = green;
		else
			max = blue;

		if (red < blue)
			min = red;
		else
			min = blue;
	}

	l = (max + min) / 2;
	s = 0;
	h = 0;

	if (max != min) {
		if (l <= 0.5)
			s = (max - min) / (max + min);
		else
			s = (max - min) / (2 - max - min);

		delta = max -min;
		if (red == max)
			h = (green - blue) / delta;
		else if (green == max)
			h = 2 + (blue - red) / delta;
		else if (blue == max)
			h = 4 + (red - green) / delta;

		h *= 60;
		if (h < 0.0)
			h += 360;
	}

	*r = h;
	*g = l;
	*b = s;
}

static void
hls_to_rgb (gdouble *h, gdouble *l, gdouble *s)
{
	gdouble hue;
	gdouble lightness;
	gdouble saturation;
	gdouble m1, m2;
	gdouble r, g, b;

	lightness = *l;
	saturation = *s;

	if (lightness <= 0.5)
		m2 = lightness * (1 + saturation);
	else
		m2 = lightness + saturation - lightness * saturation;
	m1 = 2 * lightness - m2;

	if (saturation == 0) {
		*h = lightness;
		*l = lightness;
		*s = lightness;
	} else {
		hue = *h + 120;
		while (hue > 360)
			hue -= 360;
		while (hue < 0)
			hue += 360;

		if (hue < 60)
			r = m1 + (m2 - m1) * hue / 60;
		else if (hue < 180)
			r = m2;
		else if (hue < 240)
			r = m1 + (m2 - m1) * (240 - hue) / 60;
		else
			r = m1;

		hue = *h;
		while (hue > 360)
			hue -= 360;
		while (hue < 0)
			hue += 360;

		if (hue < 60)
			g = m1 + (m2 - m1) * hue / 60;
		else if (hue < 180)
			g = m2;
		else if (hue < 240)
			g = m1 + (m2 - m1) * (240 - hue) / 60;
		else
			g = m1;

		hue = *h - 120;
		while (hue > 360)
			hue -= 360;
		while (hue < 0)
			hue += 360;

		if (hue < 60)
			b = m1 + (m2 - m1) * hue / 60;
		else if (hue < 180)
			b = m2;
		else if (hue < 240)
			b = m1 + (m2 - m1) * (240 - hue) / 60;
		else
			b = m1;

		*h = r;
		*l = g;
		*s = b;
	}
}

static void
shade_color(GdkColor *a, GdkColor *b, gdouble k)
{
	gdouble red;
	gdouble green;
	gdouble blue;

	/* Special case for black, so that it looks pretty... */

	if ((a->red == 0) && (a->green == 0) && (a->blue == 0)) {
		a->red = 32768;
		a->green = 32768;
		a->blue = 32768;
	}

	red = (gdouble) a->red / 65535.0;
	green = (gdouble) a->green / 65535.0;
	blue = (gdouble) a->blue / 65535.0;

	rgb_to_hls (&red, &green, &blue);

	green *= k;

	if (green > 1.0)
		green = 1.0;
	else if (green < 0.0)
		green = 0.0;

	blue *= k;

	if (blue > 1.0)
		blue = 1.0;
	else if (blue < 0.0)
		blue = 0.0;

	hls_to_rgb (&red, &green, &blue);

	b->red = (int) (red * 65535.0 + 0.5);
	b->green = (int) (green * 65535.0 + 0.5);
	b->blue = (int) (blue * 65535.0 + 0.5);
}

#define HIGHLIGHT_MULT 0.7
#define LIGHT_MULT     1.5
#define DARK_MULT      0.5

static void
my_get_colors(GdkColormap *colormap, gulong background, gulong *top,
	gulong *bottom, gulong *highlight, ToolkitAbstraction *tka)
{
	GdkColor cbackground;
	GdkColor ctop, cbottom, chighlight;

	/*****
	* I think this should use a ColorContext instead of allocating colors
	* itself... - Federico
	*****/

	/* FIXME: The colors that are allocated here are never freed.
	 * I think we can save the pixel values in static variables so that we
	 * can call gdk_colors_free() on the next invocation of the function.
	 */

	cbackground.pixel = background;
	tka->QueryColors(colormap, &cbackground, 1);

	if (top) {
		shade_color(&cbackground, &ctop, LIGHT_MULT);
		gdk_color_alloc(colormap, &ctop);
		*top = ctop.pixel;
	}

	if (bottom) {
		shade_color(&cbackground, &cbottom, DARK_MULT);
		gdk_color_alloc(colormap, &cbottom);
		*bottom = cbottom.pixel;
	}

	if (highlight) {
		shade_color(&cbackground, &chighlight, HIGHLIGHT_MULT);
		gdk_color_alloc(colormap, &chighlight);
		*highlight = chighlight.pixel;
	}
}

static void
set_widget_colors(GtkXmHTML *html, gulong *top, gulong *bottom,
	gulong *highlight)
{
	GdkColor c;
	
	if (top) {
		c.pixel = *top;
		gdk_gc_set_foreground(html->top_shadow_gc, &c);
	}

	if (bottom) {
		c.pixel = *bottom;
		gdk_gc_set_foreground(html->bottom_shadow_gc, &c);
	}

	if (highlight) {
		c.pixel = *highlight;
		gdk_gc_set_foreground(html->highlight_gc, &c);
		html->highlight_color = *highlight;
	}
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
		gulong top, bottom, highlight;
		my_get_colors(gtk_widget_get_colormap(GTK_WIDGET(html)),
			html->html.body_bg, &top, &bottom, &highlight, html->html.tka);
		set_widget_colors(html, &top, &bottom, &highlight);
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
	if(html->html.gc != NULL)
	{
		gulong highlight;

		my_get_colors(gtk_widget_get_colormap(GTK_WIDGET(html)),i
			html->html.body_bg, NULL, NULL, &highlight, html->html.tka);
		set_widget_colors(html, NULL, NULL, &highlight);
	}
}
