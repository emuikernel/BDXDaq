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

#ifndef _MULTILIST_H_
#define _MULTILIST_H_

#include <Xm/Xm.h>

/*---------------------------------------------------------------------------*

      R E S O U R C E    D E S C R I P T I O N S    A N D    N O T E S

 *---------------------------------------------------------------------------*/

/*

 Name		     Class		RepType		Default Value
 ----		     -----		-------		-------------
 alignment           Alignment          Unsigned char   XmALIGNMENT_BEGINNING *
 background	     Background		Pixel		XtDefaultBackground
 border		     BorderColor	Pixel		XtDefaultForeground
 borderWidth	     BorderWidth	Dimension	1
 callback            Callback           XtCallbackList  NULL                *1
 columnWidth         Width              Dimension       0                   *10
 columnSpacing       Spacing            Dimension       8
 cursor		     Cursor		Cursor		left_ptr
 defaultColumns      Columns            int             1                   *2
 destroyCallback     Callback		Pointer		NULL 
 font		     Font		XFontStruct*	XtDefaultFont
 forceColumns        Columns            Boolean         False               *2
 foreground	     Foreground		Pixel		XtDefaultForeground
 altForeground       Foregorund         Pixel           XtDefaultForeground *12
 height		     Height		Dimension	0                   *3
 highlightBackground HBackground	Pixel		XtDefaultForeground *4
 highlightForeground HForeground	Pixel		XtDefaultBackground *4
 insensitiveBorder   Insensitive	Pixmap		Gray
 list                List               String *        NULL                *5
 longest             Longest            int             0                   *6
 mappedWhenManaged   MappedWhenManaged	Boolean		True
 maxSelectable       Value              int             1                   *7
 notifyHighlights    Boolean            Boolean         True                *8
 notifyOpens         Boolean            Boolean         True                *8
 notifyUnhighlights  Boolean            Boolean         True                *8
 numberStrings       NumberStrings      int             0                   *5
 pasteBuffer         Boolean            Boolean         False
 preferredWidths     List               Dimentsion *    NULL                *11
 rowHeight           Height             Dimension       0                   *10
 rowSpacing          Spacing            Dimension       2
 sensitive	     Sensitive		Boolean		True
 sensitiveArray      List               Boolean *       NULL                *9
 verticalList        Boolean            Boolean         False
 width		     Width		Dimension	0
 x		     Position		Position	0
 y		     Position		Position	0
 blink               Boolean            Boolean         True
 blinkDelay          Value              int             500 ms
 reverseVideo        Boolean            Boolean         False
 selectedItem        Value              int             0
 selectCallback      Callback           Callback        NULL
 separatorWidth      Width              Dimension       0                  *13
 sepMoveCallback     Callback           Callback        0                  *14

 *1 -  The callback functions are called whenever a highlight, unhighlight,
       or open takes place, and the appropriate notification resource is
       set true.  The callbacks get called with a MultiListReturnStruct pointer
       which points to the data about the action that just occurred.

 *2 -  The defaultColumns resource is used in two cases.  If forceColumns
       is true, the widget will set the number of columns to the value of
       default columns.  If the widget width is unconstrained by the parent
       widgets, the defaultColumns is also used to determine the number of
       columns and the resulting width.  Otherwise, the number of columns
       will be calcultaed based on the current width and will be changed to
       an appropriate value.

 *3 -  If the width or height is set to zero (0), which is the default case,
       then the widget will calculate the size of that dimension to be just
       large enough to hold the contents of the widget.

 *4 -  The highlightForeground and highlightBackground colors specify the
       colors used to highlight the text (foreground) and the surrounding
       Background space of a list item when it is selected (highlighted).
       The default is the reverse of the default foreground and background
       colors.

 *5 -  The list resource is an array of strings (char * array) which tell
       the names of each item of the list.  The number of elements of this
       array are indicated by the resource numberStrings.  If numberStrings
       is set to 0 (the default), then the MultiList widget will count the
       number of strings in the list.  This requires that the list be
       NULL terminated.  If list is NULL, then the widget treats it as an
       empty list.  Once the list is set the list resource is set to NULL,
       so you won't be able to read back the list after it has been set.  The
       widgets copies the strings internally, so the user can free the list
       storage after setting it.

 *6 -  This resource represent the longest string in pixels.  If this
       resource is zero (0), which is the default and probably the value
       most people should use, the longest string length is calculated
       and the resource is updated.

 *7 -  The maxSelectable resource indicates the maximum number of items
       which can be selected at any one time.  In the original Athena
       widget, you could have at most one item selected at a time.  In
       this widget, you can choose how many will be selected at a time.

 *8 -  When an item is highlighted or becomes unhighlighted, or when
       the Open() action is run, a callback can be issued indicating the
       new state of what items are highlighted and what action occurred.
       The notify resources are booleans that enable or disable callbacks
       in various cases.  If you are only interested in being notified
       when the user selects an item, and not when he deselects items,
       then only notifyHighlights needs to be True.

 *9 -  Each item in the MultiList can be made insensitive, so it is printed in
       gray shading and can not be highlighted.  This can be done by
       setting the sensitivity list, which is an array of Booleans which
       indicate whether or not the corresponding item is sensitive (can be
       selected).  If sensitivity list is NULL, all items are sensitive.  The
       widget copies the sensitivity information, so the user can delete the
       sensitivity array storage after setting it.  The widget sets the
       resource to NULL after it has been set, so the user cannot read the
       old list back.

 *10 - These values are intended for reading only.  They indicate the pixel
       width/height of the column/row.

 *11 - Each column in the MultiList can have its prefereed width. This can be
       done using an array of unsigned int to denote the width of each column.
       This resource only works when forceColumn is set.

 *12 - Alternative foreground color which can be used as high light or
       warning visual effect

 *13 - A vertical separator that separates each column : only valid for force
       column is set

 *14 - A callback after moving a separator
*/

/*---------------------------------------------------------------------------*

                    S T R I N G    D E F I N I T I O N S

 *---------------------------------------------------------------------------*/


#define	XmNhighlightForeground	"highlightForeground"
#define	XmNhighlightBackground	"highlightBackground"
#define XmNaltForeground        "altForeground"
#define XmNaltForeground1       "altForeground1"
#define XmNaltForeground2       "altForeground2"
#define XmNaltForeground3       "altForeground3"
#define XmNaltForeground4       "altForeground4"
#define XmNaltForeground5       "altForeground5"
#define XmNaltForeground6       "altForeground6"
#define XmNaltForeground7       "altForeground7"
#define XmNcallback		"callback"
#define XmNcolumnSpacing	"columnSpacing"
#define XmNrowSpacing		"rowSpacing"
#define XmNdefaultColumns	"defaultColumns"
#define XmNforceColumns		"forceColumns"
#define XmNpasteBuffer		"pasteBuffer"
#define XmNverticalList		"verticalList"
#define XmNlongest		"longest"
#define XmNnumberStrings	"numberStrings"
#define XmNlist			"list"
#define	XmNsensitiveArray	"sensitiveArray"
#define	XmNmaxSelectable	"maxSelectable"
#define	XmNnotifyHighlights	"notifyHighlights"
#define	XmNnotifyUnhighlights	"notifyUnhighlights"
#define	XmNnotifyOpens		"notifyOpens"
#define	XmNuseScrollBar		"useScrollBar"
#define	XmNclickDelay		"clickDelay"
#define	XmNuseMultiClick	"useMultiClick"
#define	XmNblink		"blink"
#define	XmNblinkDelay		"blinkDelay"
#define	XmNreverseVideo		"reverseVideo"
#define	XmNselectCallback	"selectCallback"
#define	XmNselectedItem		"selectedItem"

#define	XmNrowHeight		"rowHeight"
#define	XmNcolumnWidth		"columnWidth"
#define XmNseparatorWidth       "separatorWidth"
#define XmNsepMoveCallback      "sepMoveCallback"

#define XmCList			"List"
#define XmCLongest		"Longest"
#define XmCNumberStrings	"NumberStrings"
#define	XmCHForeground		"HForeground"
#define	XmCHBackground		"HBackground"
#define XmCAltForeground        "AltForeground"
#define XmCSeparatorWidth       "SeparatorWidth"
#define XmCAltForeground1       "AltForeground1"
#define XmCAltForeground2       "AltForeground2"
#define XmCAltForeground3       "AltForeground3"
#define XmCAltForeground4       "AltForeground4"
#define XmCAltForeground5       "AltForeground5"
#define XmCAltForeground6       "AltForeground6"
#define XmCAltForeground7       "AltForeground7"
#define	XmCUseScrollBar		"UseScrollBar"
#define	XmCClickDelay		"ClickDelay"
#define	XmCUseMultiClick	"UseMultiClick"

#define XmNpreferredWidths      "preferredWidths"
#define XmCPreferredWidths      "PreferredWidths"

 
	/* Class Record Constants */

extern WidgetClass multiListWidgetClass;

typedef struct _MultiListClassRec *MultiListWidgetClass;
typedef struct _MultiListRec      *MultiListWidget;

/*---------------------------------------------------------------------------*

                     R E T U R N    S T R U C T U R E

 *---------------------------------------------------------------------------*/
#define	MULTILIST_ACTION_SET		0
#define	MULTILIST_ACTION_SET_MANY	1
#define	MULTILIST_ACTION_UNSET		2
#define	MULTILIST_ACTION_UNSET_ALL	3
#define	MULTILIST_ACTION_OPEN		4
#define	MULTILIST_ACTION_OPEN_MANY	5
#define	MULTILIST_ACTION_STATUS		6
#define	MULTILIST_ACTION_SELECT		7
#define	MULTILIST_ACTION_UNSELECT	8
#define	MULTILIST_END_MOVING_SEP	9


typedef	struct
{
  Boolean		sensitive;
  Boolean		highlighted;
  int                   fgi;    /* -1, use default, 0: alt_fg0: etc */
  String		string;
  Boolean		blink;
} MultiListItem;

typedef struct _MultiListReturnStruct
{
  int action;
  int item;
  String string;
  int num_selected;
  int *selected_items;
  MultiListItem *item_value;
  
} MultiListReturnStruct;

typedef struct _MultiListSelectStruct
{
  int 		action;	/* one of MULTILIST_ACTION_SELECT or UNSELECT */
  int 		item;
  MultiListItem 	*item_value;
  XEvent		*event;
} MultiListSelectStruct;

typedef struct _MultiListSepMoveStruct
{
  int           action;
  XEvent        *event;
  int           num;
  Dimension     *widths;
}MultiListSepMoveStruct;

/* used by MultiList question procedures */
#define	MULTILIST_NO			0
#define	MULTILIST_YES			1
#define MULTILIST_NEARLY		2


/*---------------------------------------------------------------------------*

                     U T I L I T Y    R O U T I N E S

 *---------------------------------------------------------------------------*/

/* Changes to suppress ANSI warnings January 96  (from XRN) */
#ifndef _ARGUMENTS
#if defined (__cplusplus) || (defined(__STDC__) && !defined(_NO_PROTO))
#define _ARGUMENTS(arglist) arglist
#else
#define _ARGUMENTS(arglist) ()
#endif
#endif


#if defined (__cplusplus)
extern "C" {
#endif

extern void MultiListRefresh _ARGUMENTS ((Widget w ));
extern void MultiListHighlightItem _ARGUMENTS(( Widget w, int item_index ));
extern void MultiListHighlightAll _ARGUMENTS(( Widget w ));
extern void MultiListHighlightColumn _ARGUMENTS(( Widget w, int column ));
extern void MultiListUnhighlightItem _ARGUMENTS(( Widget w, int item_index ));
extern void MultiListUnhighlightAll _ARGUMENTS(( Widget w ));
extern void MultiListSetColor _ARGUMENTS (( Widget w, int index, 
					    int item_index));
extern void MultiListSetColorColumn _ARGUMENTS((Widget w, int fgindex,
						int column));
extern void MultiListSetColorAll _ARGUMENTS((Widget w, int fgindex));
extern int MultiListToggleItem _ARGUMENTS(( Widget w, int item_index ));
extern MultiListReturnStruct *MultiListGetHighlighted _ARGUMENTS(( Widget w ));
extern Boolean MultiListIsHighlighted _ARGUMENTS(( Widget w, int item_index ));
extern Boolean MultiListGetItemInfo _ARGUMENTS(( Widget w, int item_index, 
						 String *str_ptr, Boolean *h_ptr, 
						 Boolean *s_ptr, Boolean *b_ptr, 
						 int *fgindex));
extern int  MultiListItemIsVisible _ARGUMENTS(( Widget w, int item_index ));
extern int  MultiListItemIsVisibleV _ARGUMENTS(( Widget w, int item_index ));
extern void MultiListSetNewData _ARGUMENTS(( Widget w, String *list, 
					     int nitems, int longest, 
					     Boolean resize, 
					     Boolean *sensitivity_array ));
extern Widget MultiListCreateScrolledList _ARGUMENTS(( Widget parent, char *name, 
						       ArgList args, int argCount ));
extern void MultiListAddItems _ARGUMENTS(( Widget w, String *list, 
					   int nitems, int longest, int position, 
					   Boolean resize, Boolean *sensitivity_array ));
extern void MultiListAddItem _ARGUMENTS(( Widget w, String item, int position ));
extern void MultiListAddItemWithColor _ARGUMENTS(( Widget w, String item, 
						   int position, int fgindex));
extern void MultiListAddItemsWithColor _ARGUMENTS(( Widget w, String *list, 
						    int nitems, int longest, int position, 
						    Boolean resize, 
						    Boolean *sensitivity_array,
						    int fgindex));
extern int MultiListDeleteItems _ARGUMENTS(( Widget w, int position, int nitems ));
extern int MultiListDeleteItem _ARGUMENTS(( Widget w, int position ));
extern int MultiListItemIsVisible _ARGUMENTS(( Widget w, int position ));
extern void MultiListSetPos _ARGUMENTS(( Widget w, int position ));
extern void MultiListBlinkItem _ARGUMENTS(( Widget w, int item_index ));
extern void MultiListUnblinkItem _ARGUMENTS(( Widget w, int item_index ));

#if defined (__cplusplus)
};
#endif


#endif
