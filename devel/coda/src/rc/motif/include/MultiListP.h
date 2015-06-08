/****************************************************************************

	MultiList.h

	This code is based on Brian Totty MultiList  widget(FWF) and the
        Athena List source which is why the MIT copyright notice appears below.

	May, 1992, Marc QUINTON

	see the Copyright(c) notice below.

 ****************************************************************************/

/* Copyright(c) 1992 STNA/7 95 rue Henri Rochefort 91025 EVRY Cedex FRANCE
 *                        All rights reserved
 * Permission to use, copy, modify and distribute this material for
 * any purpose and without fee is hereby granted, provided that the
 * above copyright notice and this permission notice appear in all
 * copies, and that the name of STNA not be used in advertising
 * or publicity pertaining to this material without the specific,
 * prior written permission of an authorized representative of
 * STNA.
 *
 * STNA MAKES NO REPRESENTATIONS AND EXTENDS NO WARRANTIES, EX-
 * PRESS OR IMPLIED, WITH RESPECT TO THE SOFTWARE, INCLUDING, BUT
 * NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS FOR ANY PARTICULAR PURPOSE, AND THE WARRANTY AGAINST IN-
 * FRINGEMENT OF PATENTS OR OTHER INTELLECTUAL PROPERTY RIGHTS.  THE
 * SOFTWARE IS PROVIDED "AS IS", AND IN NO EVENT SHALL STNA OR
 * ANY OF ITS AFFILIATES BE LIABLE FOR ANY DAMAGES, INCLUDING ANY
 * LOST PROFITS OR OTHER INCIDENTAL OR CONSEQUENTIAL DAMAGES RELAT-
 * ING TO THE SOFTWARE.
 */


/*
 * Copyright 1989 Massachusetts Institute of Technology
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of M.I.T. not be used in advertising or
 * publicity pertaining to distribution of the software without specific,
 * written prior permission.  M.I.T. makes no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 *
 * M.I.T. DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING ALL
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL M.I.T.
 * BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
 * OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN 
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * Original Athena Author:  Chris D. Peterson, MIT X Consortium
 */


#ifndef _MULTILISTP_H_
#define _MULTILISTP_H_

#include <X11/cursorfont.h>
#include <Xm/XmP.h>
#include <Xm/PrimitiveP.h>
#include <Xm/ScrollBar.h>
#include <Xm/ScrolledWP.h>

#include "MultiList.h"


/*---------------------------------------------------------------------------*

     W I D G E T    D A T A    S T R U C T U R E    D E F I N I T I O N S

 *---------------------------------------------------------------------------*/

typedef struct
{
  int		foo;
} MultiListClassPart;

typedef struct _MultiListClassRec
{
  CoreClassPart	core_class;
  XmPrimitiveClassPart	primitive_class;
  MultiListClassPart	multiList_class;
} MultiListClassRec;

extern MultiListClassRec multiListClassRec;

typedef struct
{
  Pixel		foreground;
  Pixel         alt_fg[8];
  Pixel		highlight_fg;
  Pixel		highlight_bg;
  Dimension	column_space;
  Dimension	row_space;
  int		default_cols;
  Boolean	force_cols;
  Boolean	paste;		/* copy to the X selection buffer */
  Boolean	row_major;
  int		longest;
  int		nitems;
  XFontStruct	*font;
  String	*list;
  Cursor        mcursor;
  Boolean	*sensitive_array;
  Dimension     *col_widths;
  
  XtCallbackList  callback;

  int		max_selectable;
  Boolean	notify_highlights;
  Boolean	notify_unhighlights;
  Boolean	notify_opens;
  Dimension	col_width;
  Dimension	row_height;

  Dimension     sep_width;
  unsigned char sep_type;
  unsigned char alignment;
  unsigned char pad0, pad1;
  
  int		nrows;
  int		ncols;
  int		most_recent_clicked_item;
  int		most_recent_action;

  /* for selection mechanisme */
  int		selected_item;
  XtCallbackList  select_callback;
  XtCallbackList  sep_move_callback;
  
  GC		erase_gc;
  GC		draw_gc;
  GC		highlight_bg_gc;
  GC		highlight_fg_gc;
  GC		gray_gc;
  GC            sep_gc;
  GC            alt_fg_gc[8];
  
  MultiListItem	*item_array;
  int		num_selected;
  int		*sel_array;
  
  /* the multi click stuff */
  int           click_delay;
  int           click_number;
  Boolean	use_multi_click;
  Time		last_button_time;
  
  /* the blinking stuff */
  Boolean    	blink;		/* to enable or disable blinking */
  XtIntervalId  blink_timer;
  int           blink_delay;
  int		blink_on;	/* are items highlighted ? */
  int		num_blinking;	/* nb items which are blinking */
  Boolean	reverse_video;	/* blink in reverse video */

  /* rubber line drawing for separator */
  int           x;
  int           y1;
  int           y2;
  /* move separator flag */
  int           move_sep;
  /* minimum position of the separator */
  int           minx;
  /* initial position of the separator */
  int           inix;
  /* column number for  the separator */
  int           colnum;
		

  /* the scrollBar stuff */
  Widget	scrolled_win;
  Widget	vscrollBar;
  Boolean	use_scrollBar;
  int		scrollBar_value;
  int 		pixel_offset;		/* vertical pixel offset */
  Boolean	clip_set;	/* the clip rectangle has been set */
	

} MultiListPart;

typedef struct _MultiListRec
{
  CorePart	core;
  XmPrimitivePart	primitive;
  MultiListPart	multiList;
} MultiListRec;

/*---------------------------------------------------------------------------*

          D A T A    S T R U C T U R E    A C C E S S    M A C R O S

 *---------------------------------------------------------------------------*/

#define	MultiListItemSensitive(i)	((i)->sensitive)
#define	MultiListItemBlinking(i)	((i)->blink)
#define	MultiListItemHighlighted(i)	((i)->highlighted)
#define	MultiListItemString(i)		((i)->string)
#define MultiListItemFgIndex(i)         ((i)->fgi)

#define	InstanceCore(w)			(&((w)->core))
#define	InstancePrimitive(w)		(&((w)->primitive))
#define	InstanceMultiList(w)		(&((w)->multiList))

#define	MultiListWidth(w)		(InstanceCore(w)->width)
#define	MultiListHeight(w)		(InstanceCore(w)->height)
#define	MultiListBG(w)			(InstanceCore(w)->background_pixel)
#define	MultiListSensitive(w)		(InstanceCore(w)->sensitive)
#define	MultiListAncesSensitive(w)	(InstanceCore(w)->ancestor_sensitive)
#define	MultiListDepth(w)		(InstanceCore(w)->depth)

#define	MultiListFG(w)			(InstancePrimitive(w)->foreground)
#define	MultiListAltFG(w, n)		(InstanceMultiList(w)->alt_fg[n])
#define MultiListSepWidth(w)            (InstanceMultiList(w)->sep_width)
#define MultiListSepType(w )            (InstanceMultiList(w)->sep_type)
#define MultiListAlignment(w )          (InstanceMultiList(w)->alignment)
#define	MultiListHighlightFG(w)		(InstanceMultiList(w)->highlight_fg)
#define	MultiListHighlightBG(w)		(InstanceMultiList(w)->highlight_bg)
#define	MultiListColumnSpace(w)		(InstanceMultiList(w)->column_space)
#define	MultiListRowSpace(w)		(InstanceMultiList(w)->row_space)
#define	MultiListDefaultCols(w)		(InstanceMultiList(w)->default_cols)
#define	MultiListForceCols(w)		(InstanceMultiList(w)->force_cols)
#define	MultiListPaste(w)		(InstanceMultiList(w)->paste)
#define	MultiListRowMajor(w)		(InstanceMultiList(w)->row_major)
#define	MultiListLongest(w)		(InstanceMultiList(w)->longest)
#define	MultiListNumItems(w)		(InstanceMultiList(w)->nitems)
#define	MultiListFont(w)		(InstanceMultiList(w)->font)
#define MultiListMCursor(w)             (InstanceMultiList(w)->mcursor)
#define	MultiListList(w)		(InstanceMultiList(w)->list)
#define	MultiListSensitiveArray(w)	(InstanceMultiList(w)->sensitive_array)
#define MultiListPrefWidths(w)          (InstanceMultiList(w)->col_widths)
#define	MultiListCallback(w)		(InstanceMultiList(w)->callback)
#define	MultiListMaxSelectable(w)	(InstanceMultiList(w)->max_selectable)
#define	MultiListNotifySets(w)		(InstanceMultiList(w)->notify_highlights)
#define	MultiListNotifyUnsets(w)	(InstanceMultiList(w)->notify_unhighlights)
#define	MultiListNotifyOpens(w)		(InstanceMultiList(w)->notify_opens)

#define	MultiListColWidth(w)		(InstanceMultiList(w)->col_width)
#define	MultiListRowHeight(w)		(InstanceMultiList(w)->row_height)
#define	MultiListNumRows(w)		(InstanceMultiList(w)->nrows)
#define	MultiListNumCols(w)		(InstanceMultiList(w)->ncols)
#define	MultiListMostRecentItem(w)	(InstanceMultiList(w)->most_recent_clicked_item)
#define	MultiListMostRecentAct(w)	(InstanceMultiList(w)->most_recent_action)
#define	MultiListEraseGC(w)		(InstanceMultiList(w)->erase_gc)
#define	MultiListDrawGC(w)		(InstanceMultiList(w)->draw_gc)
#define	MultiListSepGC(w)		(InstanceMultiList(w)->sep_gc)
#define	MultiListHighlightForeGC(w)	(InstanceMultiList(w)->highlight_fg_gc)
#define	MultiListHighlightBackGC(w)	(InstanceMultiList(w)->highlight_bg_gc)
#define	MultiListGrayGC(w)		(InstanceMultiList(w)->gray_gc)
#define	MultiListAltFgGC(w,n)		(InstanceMultiList(w)->alt_fg_gc[n])
#define MultiListTopShadowGC(w)         (InstancePrimitive(w)->top_shadow_GC)
#define MultiListBottomShadowGC(w)      (InstancePrimitive(w)->bottom_shadow_GC)
#define	MultiListItemArray(w)		(InstanceMultiList(w)->item_array)
#define	MultiListNthItem(w,n)		(&(MultiListItemArray(w)[n]))
#define	MultiListSelArray(w)		(InstanceMultiList(w)->sel_array)
#define	MultiListNumSelected(w)		(InstanceMultiList(w)->num_selected)

#define	MultiListUseScrollBar(w)	(InstanceMultiList(w)->use_scrollBar)

#define	MultiListClickDelay(w)		(InstanceMultiList(w)->click_delay)
#define	MultiListClickNumber(w)		(InstanceMultiList(w)->click_number)
#define	MultiListUseMultiClick(w)	(InstanceMultiList(w)->use_multi_click)
#define	MultiListClipSet(w)		(InstanceMultiList(w)->clip_set)

#define MultiListSepX(w)                (InstanceMultiList(w)->x)
#define MultiListSepY1(w)               (InstanceMultiList(w)->y1)
#define MultiListSepY2(w)               (InstanceMultiList(w)->y2)
#define MultiListMoveSep(w)             (InstanceMultiList(w)->move_sep)
#define MultiListSepMinX(w)             (InstanceMultiList(w)->minx)
#define MultiListSepIniX(w)             (InstanceMultiList(w)->inix)
#define MultiListSepCol(w)              (InstanceMultiList(w)->colnum)

#endif
