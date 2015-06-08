/****************************************************************************

	MultiList.c

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

#include <stdio.h>
#include <ctype.h>
#include "MultiListP.h"


static char id[] = 
"MultiList widget Copyright(c) May 1992 STNA/France Marc QUINTON";

static char version[] = "MultiList widget version 2.0";

/*===========================================================================*

          D E C L A R A T I O N S    A N D    D E F I N I T I O N S

 *===========================================================================*/

Pixmap XmuCreateStippledPixmap();


#define	FontAscent(f)	((f)->max_bounds.ascent)
#define	FontDescent(f)	((f)->max_bounds.descent)
#define	FontH(f)	(FontAscent(f) + FontDescent(f) + 2)
#define	FontW(f,s)	(XTextWidth(f,s,strlen(s)) + 1)
#define	FontMaxCharW(f)	((f)->max_bounds.rbearing-(f)->min_bounds.lbearing+1)

#ifndef abs
#define abs(a)			((a) < 0 ? -(a) : (a))
#endif

#define max(a,b)		((a) > (b) ? (a) : (b))
#define min(a,b)		((a) < (b) ? (a) : (b))
#define XtStrlen(s)		((s) ? strlen(s) : 0)

#define	TypeAlloc(t,n)		(t *)XtMalloc(sizeof(t) * n)
#define	TypeRealloc(ptr,t,n)	(t *)XtRealloc(ptr,sizeof(t) * n)
#define	StrCopy(s)		strcpy(TypeAlloc(char,strlen(s)+1),s)
#define	StrCopyRetLength(s,lp)	strcpy(TypeAlloc(char,(*lp=(strlen(s)+1))),s)

#define CoreFieldOffset(f)	XtOffset(Widget,core.f)
#define	PrimitiveFieldOffset(f)	XtOffset(MultiListWidget,primitive.f)
#define MultiListFieldOffset(f)	XtOffset(MultiListWidget,multiList.f)



/*===========================================================================*

        I N T E R N A L    P R O C E D U R E    D E C L A R A T I O N S

 *===========================================================================*/

static void Initialize _ARGUMENTS(( MultiListWidget request, 
				    MultiListWidget new ));
static void MultiListRealize _ARGUMENTS(( MultiListWidget mlw, 
					  XtValueMask *valueMask, 
					  XSetWindowAttributes *attrs ));
static void Destroy _ARGUMENTS(( MultiListWidget mlw ));
static void Redisplay _ARGUMENTS(( MultiListWidget mlw, 
				   XEvent *event, Region rectangle_union ));
static XtGeometryResult 
PreferredGeometry _ARGUMENTS(( MultiListWidget mlw, 
			       XtWidgetGeometry *parent_idea, 
			       XtWidgetGeometry *our_idea ));
static void Resize _ARGUMENTS(( MultiListWidget mlw ));
static Boolean SetValues _ARGUMENTS(( MultiListWidget cpl, 
				      MultiListWidget rpl, 
				      MultiListWidget npl ));
static void DestroyOldData _ARGUMENTS(( MultiListWidget mlw ));
static void InitializeNewData _ARGUMENTS(( MultiListWidget mlw ));
static void CreateNewGCs _ARGUMENTS(( MultiListWidget mlw ));
static void RecalcCoords _ARGUMENTS(( MultiListWidget mlw, 
				      Boolean width_changeable,
				      Boolean height_changeable ));
static void NegotiateSizeChange _ARGUMENTS(( MultiListWidget mlw, 
					     Dimension width, 
					     Dimension height ));
static Boolean Layout _ARGUMENTS(( MultiListWidget mlw, 
				   Boolean w_changeable, 
				   Boolean h_changeable, 
				   Dimension *w_ptr, 
				   Dimension *h_ptr ));
static void 
MultiListVertSliderMove _ARGUMENTS(( Widget w, 
				     caddr_t closure, 
				     XmScrollBarCallbackStruct *call_data ));
static void MultiListSetVerticalScrollbar _ARGUMENTS(( MultiListWidget mlw ));
static void MultiListDrawShadow _ARGUMENTS(( MultiListWidget mlw, 
					     Boolean armed ));
static void MultiListSetClipRect _ARGUMENTS(( MultiListWidget mlw ));
static void MultiListUnsetClipRect _ARGUMENTS(( MultiListWidget mlw ));
static void RedrawAll _ARGUMENTS(( MultiListWidget mlw ));
static void RedrawItem _ARGUMENTS(( MultiListWidget mlw, int item_index ));
static void MultiListScroll _ARGUMENTS(( MultiListWidget mlw, int offset ));
static void MultiListRedrawArea _ARGUMENTS(( MultiListWidget mlw, 
					     int x1, int y1, int w, int h ));
static void MultiListRefreshArea _ARGUMENTS(( MultiListWidget mlw, 
					      int x1, int y1, int w, int h ));
static void RedrawRowColumn _ARGUMENTS(( MultiListWidget mlw, 
					 int row, int column ));
static void PixelToRowColumn _ARGUMENTS(( MultiListWidget mlw, int x, int y, 
					  int *row_ptr, int *column_ptr ));
static void RowColumnToPixels _ARGUMENTS(( MultiListWidget mlw, 
					   int row, int col, 
					   int *x_ptr, int *y_ptr, 
					   int *w_ptr, int *h_ptr ));
static Boolean RowColumnToItem _ARGUMENTS(( MultiListWidget mlw, int row, 
					    int column, int *item_ptr ));
static Boolean ItemToRowColumn _ARGUMENTS(( MultiListWidget mlw, 
					    int item_index, 
					    int *row_ptr, int *column_ptr ));
static void MultiListHandleMultiClick _ARGUMENTS(( MultiListWidget mlw, 
						   int item_index ));
static void MultiListDoNotification _ARGUMENTS(( MultiListWidget mlw ));
static void MultiListSet _ARGUMENTS(( MultiListWidget mlw, XEvent *event, 
				      String *params, Cardinal *num_params ));
static void MultiListSetMany _ARGUMENTS(( MultiListWidget mlw, XEvent *event, 
					  String *params, 
					  Cardinal *num_params ));
static void MultiListSetColumn _ARGUMENTS(( MultiListWidget mlw, 
					    XEvent *event, 
					    String *params, 
					    Cardinal *num_params ));
static void MultiListSetAll _ARGUMENTS(( MultiListWidget mlw, 
					 XEvent *event, 
					 String *params, 
					 Cardinal *num_params ));
static void MultiListSelect _ARGUMENTS(( MultiListWidget mlw, 
					 XEvent *event, 
					 String *params, 
					 Cardinal *num_params ));
static void MultiListUnselect _ARGUMENTS(( MultiListWidget mlw, 
					   XEvent *event, 
					   String *params, 
					   Cardinal *num_params ));
static void MultiListUnset _ARGUMENTS(( MultiListWidget mlw, 
					XEvent *event, 
					String *params, 
					Cardinal *num_params ));
static void MultiListUnsetAll _ARGUMENTS(( MultiListWidget mlw, 
					   XEvent *event, 
					   String *params, 
					   Cardinal *num_params ));
static void MultiListUnsetMany _ARGUMENTS(( MultiListWidget mlw, 
					    XEvent *event, 
					    String *params, 
					    Cardinal *num_params ));
static void MultiListToggle _ARGUMENTS(( MultiListWidget mlw, 
					 XEvent *event, 
					 String *params, 
					 Cardinal *num_params ));
static void MultiListToggleMany _ARGUMENTS(( MultiListWidget mlw, 
					     XEvent *event, 
					     String *params, 
					     Cardinal *num_params ));
static void MultiListToggleAll _ARGUMENTS(( MultiListWidget mlw, 
					    XEvent *event, 
					    String *params, 
					    Cardinal *num_params ));
static void MultiListOpen _ARGUMENTS(( MultiListWidget mlw, 
				       XEvent *event, 
				       String *params, 
				       Cardinal *num_params ));
static void MultiListOpenMany _ARGUMENTS(( MultiListWidget mlw, 
					   XEvent *event,
					   String *params, 
					   Cardinal *num_params ));
static void MultiListNotify _ARGUMENTS(( MultiListWidget mlw, 
					 XEvent *event, 
					 String *params, 
					 Cardinal *num_params ));
static void _MultiListHighlightItem _ARGUMENTS(( MultiListWidget mlw, 
						 int item_index ));
static void _MultiListUnhighlightItem _ARGUMENTS(( MultiListWidget mlw, 
						   int item_index ));
static int _MultiListToggleItem _ARGUMENTS(( MultiListWidget mlw, 
					     int item_index ));
static void MultiListBlinkingProc _ARGUMENTS((XtPointer client_data, 
					      XtIntervalId *id));
static int  NumColumnWidths  _ARGUMENTS(( MultiListWidget mlw));
static Dimension TotalWidth  _ARGUMENTS(( MultiListWidget mlw, int col));
static void      ColumnGeometry  _ARGUMENTS(( MultiListWidget mlw, 
					      int col, int* x, int* w));
static int       ColumnByPixel   _ARGUMENTS(( MultiListWidget mlw, int x));
static void      MultiListDrawSeparator _ARGUMENTS(( MultiListWidget mlw,
						     int col,
						     int y1, int y2 ));
static void MultiListDrawSeparatorT _ARGUMENTS ((MultiListWidget mlw,
						 int x,
						 int y1, int y2, int wd));
static void      MultiListEraseSeparator _ARGUMENTS(( MultiListWidget mlw,
						      int x,
						      int y1, 
						      int y2, int wd));
static void MultiListMoveSeparator _ARGUMENTS(( MultiListWidget mlw,
						int x,
						int y1, int y2, int wd));
static void MultiListTrackPointer _ARGUMENTS(( MultiListWidget mlw, 
					       XEvent *event, 
					       String *params, 
					       Cardinal *num_params ));
static int _PixelInsideSeparator _ARGUMENTS((MultiListWidget mlw, 
					    int x, int y));
static void SeparatorGeometry _ARGUMENTS ((MultiListWidget mlw,
			       int ex, int ey,
			       int *x, int* y1, int *y2, int *sepw));
static void SeparatorAuxInfo _ARGUMENTS ((MultiListWidget mlw,
					  int ex, int ey,
					  int *col, int* minx, int* inix));



/*===========================================================================*

        E X T E R N A L    P R O C E D U R E    D E C L A R A T I O N S

 *===========================================================================*/
extern void _XmBackgroundColorDefault();
extern void _XmForegroundColorDefault();


/*===========================================================================*

              R E S O U R C E    I N I T I A L I Z A T I O N

 *===========================================================================*/

static XtResource resources[] =
{

  {XmNwidth, XmCWidth, XmRDimension, sizeof(Dimension),
   CoreFieldOffset(width), XmRString, "0"},
  {XmNheight, XmCHeight, XmRDimension, sizeof(Dimension),
   CoreFieldOffset(height), XmRString, "0"},

  {XmNbackground, XmCBackground, XmRPixel, sizeof (Pixel),
   CoreFieldOffset(background_pixel),
   XmRCallProc, (caddr_t) _XmBackgroundColorDefault},

  {XmNforeground, XmCForeground, XmRPixel, sizeof (Pixel),
   MultiListFieldOffset(foreground), 
   XmRCallProc, (caddr_t) _XmForegroundColorDefault},

  {XmNseparatorWidth, XmCSeparatorWidth, XmRDimension, sizeof (Dimension),
   MultiListFieldOffset(sep_width), XmRImmediate, (caddr_t)0},

  {XmNseparatorType, XmCSeparatorType, XmRSeparatorType,
   sizeof(unsigned char), MultiListFieldOffset(sep_type),
   XmRImmediate, (caddr_t) XmSHADOW_ETCHED_IN},

  {XmNalignment, XmCAlignment, XmRAlignment,
   sizeof(unsigned char), MultiListFieldOffset(alignment),
   XmRImmediate, (XtPointer)XmALIGNMENT_BEGINNING},

  {XmNaltForeground, XmCAltForeground, XmRPixel, sizeof (Pixel),
   MultiListFieldOffset(alt_fg[0]), 
   XmRCallProc, (caddr_t) _XmForegroundColorDefault},

  {XmNaltForeground1, XmCAltForeground1, XmRPixel, sizeof (Pixel),
   MultiListFieldOffset(alt_fg[1]), 
   XmRCallProc, (caddr_t) _XmForegroundColorDefault},

  {XmNaltForeground2, XmCAltForeground2, XmRPixel, sizeof (Pixel),
   MultiListFieldOffset(alt_fg[2]), 
   XmRCallProc, (caddr_t) _XmForegroundColorDefault},

  {XmNaltForeground3, XmCAltForeground3, XmRPixel, sizeof (Pixel),
   MultiListFieldOffset(alt_fg[3]), 
   XmRCallProc, (caddr_t) _XmForegroundColorDefault},

  {XmNaltForeground4, XmCAltForeground4, XmRPixel, sizeof (Pixel),
   MultiListFieldOffset(alt_fg[4]), 
   XmRCallProc, (caddr_t) _XmForegroundColorDefault},

  {XmNaltForeground5, XmCAltForeground5, XmRPixel, sizeof (Pixel),
   MultiListFieldOffset(alt_fg[5]), 
   XmRCallProc, (caddr_t) _XmForegroundColorDefault},

  {XmNaltForeground6, XmCAltForeground6, XmRPixel, sizeof (Pixel),
   MultiListFieldOffset(alt_fg[6]), 
   XmRCallProc, (caddr_t) _XmForegroundColorDefault},

  {XmNaltForeground7, XmCAltForeground7, XmRPixel, sizeof (Pixel),
   MultiListFieldOffset(alt_fg[7]), 
   XmRCallProc, (caddr_t) _XmForegroundColorDefault},

  {XmNhighlightForeground, XmCHForeground, XmRPixel, sizeof(Pixel),
   MultiListFieldOffset(highlight_fg),
   XmRCallProc, (caddr_t) _XmBackgroundColorDefault },

  {XmNhighlightBackground, XmCHBackground, XmRPixel, sizeof(Pixel),
   MultiListFieldOffset(highlight_bg), 
   XmRCallProc, (caddr_t) _XmForegroundColorDefault},

  {XmNcolumnSpacing, XmCSpacing, XmRDimension, sizeof(Dimension),
   MultiListFieldOffset(column_space), XmRImmediate, (caddr_t)8},
  {XmNrowSpacing, XmCSpacing, XmRDimension, sizeof(Dimension),
   MultiListFieldOffset(row_space), XmRImmediate, (caddr_t)0},

  {XmNdefaultColumns, XmCColumns, XmRInt,  sizeof(int),
   MultiListFieldOffset(default_cols), XmRImmediate, (caddr_t)1},
  {XmNforceColumns, XmCColumns, XmRBoolean, sizeof(Boolean),
   MultiListFieldOffset(force_cols), XmRString, (caddr_t) "False"},

  {XmNpasteBuffer, XmCBoolean, XmRBoolean, sizeof(Boolean),
   MultiListFieldOffset(paste), XmRString, (caddr_t) "False"},
  {XmNverticalList, XmCBoolean, XmRBoolean,  sizeof(Boolean),
   MultiListFieldOffset(row_major), XmRString, (caddr_t) "False"},
  {XmNlongest, XmCLongest, XmRInt,  sizeof(int),
   MultiListFieldOffset(longest), XmRImmediate, (caddr_t)0},
  {XmNnumberStrings, XmCNumberStrings, XmRInt,  sizeof(int),
   MultiListFieldOffset(nitems), XmRImmediate, (caddr_t)0},
  {XmNfont,  XmCFont, XmRFontStruct, sizeof(XFontStruct *),
   MultiListFieldOffset(font),XmRString, "XtDefaultFont"},
  {XmNlist, XmCList, XmRPointer, sizeof(char **),
   MultiListFieldOffset(list), XmRString, NULL},
  {XmNsensitiveArray, XmCList, XmRPointer, sizeof(Boolean *),
   MultiListFieldOffset(sensitive_array), XmRString, NULL},
  {XmNpreferredWidths, XmCList, XmRPointer, sizeof(Dimension *),
   MultiListFieldOffset(col_widths), XmRString, NULL},
  {XmNcallback, XmCCallback, XmRCallback, sizeof(caddr_t),
   MultiListFieldOffset(callback), XmRCallback, NULL},
  {XmNmaxSelectable, XmCValue, XmRInt, sizeof(int),
   MultiListFieldOffset(max_selectable), XmRImmediate, (caddr_t) 1},
  {XmNnotifyHighlights, XmCBoolean, XmRBoolean, sizeof(Boolean),
   MultiListFieldOffset(notify_highlights), XmRString, "True"},
  {XmNnotifyUnhighlights, XmCBoolean, XmRBoolean, sizeof(Boolean),
   MultiListFieldOffset(notify_unhighlights), XmRString, "True"},
  {XmNnotifyOpens, XmCBoolean, XmRBoolean, sizeof(Boolean),
   MultiListFieldOffset(notify_opens), XmRString, "True"},

  {XmNclickDelay, XmCClickDelay, XmRInt,  sizeof(int),
   MultiListFieldOffset(click_delay), XmRImmediate, (caddr_t)300},
  {XmNuseMultiClick, XmCUseMultiClick, XmRBoolean, sizeof(Boolean),
   MultiListFieldOffset(use_multi_click), XmRImmediate, (caddr_t) True},

  {XmNcolumnWidth, XmCValue, XmRDimension, sizeof(Dimension),
   MultiListFieldOffset(col_width), XmRImmediate, (caddr_t)0},
  {XmNrowHeight, XmCValue, XmRDimension, sizeof(Dimension),
   MultiListFieldOffset(row_height), XmRImmediate, (caddr_t)0},
  {XmNuseScrollBar, XmCUseScrollBar, XmRBoolean, sizeof(Boolean),
   MultiListFieldOffset(use_scrollBar), XmRString, (caddr_t) "True"},

  /* blinking string */
  {XmNblinkDelay, XmCValue, XmRInt,  sizeof(int),
   MultiListFieldOffset(blink_delay), XmRImmediate, (caddr_t)500},
  {XmNblink, XmCBoolean, XmRBoolean, sizeof(Boolean),
   MultiListFieldOffset(blink), XmRString, "True"},
  {XmNreverseVideo, XmCBoolean, XmRBoolean, sizeof(Boolean),
   MultiListFieldOffset(reverse_video), XmRString, "False"},

  /* selection mechanisme */
  {XmNselectCallback, XmCCallback, XmRCallback, sizeof(caddr_t),
   MultiListFieldOffset(select_callback), XmRCallback, NULL},

  /* separator move mechanisme */
  {XmNsepMoveCallback, XmCCallback, XmRCallback, sizeof(caddr_t),
   MultiListFieldOffset(sep_move_callback), XmRCallback, NULL},


};

/*===========================================================================*

        A C T I O N    A N D    T R A N S L A T I O N    T A B L E S

 *===========================================================================*/

static char defaultTranslations[] =
"	<Btn1Motion>:		SetMany()\n\
Shift<Btn1Down>:	SetMany()\n\
Ctrl<Btn1Down>:		SetAll()\n\
Alt<Btn1Down>:		Select()\n\
Alt<Btn2Down>:		Unselect()\n\
None<Btn1Down>:		Set()\n\
None<Btn2Down>:		Toggle()\n\
Shift<Btn2Down>:	ToggleMany()\n\
<Btn2Motion>:		ToggleMany()\n\
Ctrl<Btn2Down>:		ToggleAll()\n\
None<Btn3Down>:		Unset()\n\
Shift<Btn3Down>:	UnsetMany()\n\
<Btn3Motion>:		UnsetMany()\n\
Ctrl<Btn3Down>:		UnsetAll()\n\
<Motion>:               TrackPointer()\n\
<BtnUp>:		Notify()";




static XtActionsRec actions[] =
{
  {"Notify",	(XtActionProc)MultiListNotify},
  {"Set",		(XtActionProc)MultiListSet},
  {"SetMany",	(XtActionProc)MultiListSetMany},
  {"SetColumn",	(XtActionProc)MultiListSetColumn},
  {"SetAll",	(XtActionProc)MultiListSetAll},
  {"Toggle",	(XtActionProc)MultiListToggle},
  {"ToggleMany",	(XtActionProc)MultiListToggleMany},
  {"ToggleAll",	(XtActionProc)MultiListToggleAll},
  {"Open",	(XtActionProc)MultiListOpen},
  {"OpenMany",	(XtActionProc)MultiListOpenMany},
  {"Unset",	(XtActionProc)MultiListUnset},
  {"UnsetMany",	(XtActionProc)MultiListUnsetMany},
  {"UnsetAll",	(XtActionProc)MultiListUnsetAll},
  {"Select",	(XtActionProc)MultiListSelect},
  {"Unselect",	(XtActionProc)MultiListUnselect},
  {"TrackPointer", (XtActionProc)MultiListTrackPointer},
  {NULL,NULL}
};

/*===========================================================================*

    Callback Functions							
    These are the callback routines for the scrollbar actions.		

 *===========================================================================*/

static XtCallbackRec VSCallBack[] =
{
  {(XtCallbackProc )MultiListVertSliderMove, (caddr_t) NULL},
  {NULL,           (caddr_t) NULL},
};


/*===========================================================================*

                    C L A S S    A L L O C A T I O N

 *===========================================================================*/

MultiListClassRec multiListClassRec =
{
  {/* core class fields   */
    /* superclass		*/	(WidgetClass)&xmPrimitiveClassRec,
	/* class_name		*/	"MultiList",
	/* widget_size		*/	sizeof(MultiListRec),
	/* class_initialize	*/	NULL,
	/* class_part_initialize*/	NULL,
	/* class_inited		*/	FALSE,
	/* initialize		*/	Initialize,
	/* initialize_hook	*/	NULL,
	/* realize		*/	MultiListRealize,
	/* actions		*/	actions,
	/* num_actions		*/	XtNumber(actions),
	/* resources		*/	resources,
	/* resource_count	*/	XtNumber(resources),
	/* xrm_class		*/	NULLQUARK,
	/* compress_motion	*/	TRUE,
	/* compress_exposure	*/	FALSE,
	/* compress_enterleave	*/	TRUE,
	/* visible_interest	*/	FALSE,
	/* destroy		*/	Destroy,
	/* resize		*/	Resize,
	/* expose		*/	Redisplay,
	/* set_values		*/	SetValues,
	/* set_values_hook	*/	NULL,
	/* set_values_almost	*/	XtInheritSetValuesAlmost,
	/* get_values_hook	*/	NULL,
	/* accept_focus	*/	NULL,
	/* version		*/	XtVersion,
	/* callback_private	*/	NULL,
	/* tm_table		*/	defaultTranslations,
	/* query_geometry      */	PreferredGeometry,
	/* display_accelerator */	XtInheritDisplayAccelerator,
	/* extension           */	NULL,
  }, 

  {/* primitive class     */
   /* border highlight    */	NULL /*_XtInherit */,
   /* border unhighlight  */	NULL /*_XtInherit*/,
   /* translations	*/	NULL /*XtInheritTranslations*/,
   /* arm_and_activate	*/	NULL /*_XtInherit*/,
   /* syn resources	*/	NULL,
   /* num_syn_resources	*/	0,
   /* extension		*/	NULL,
  },
  {/* MultiList class fields */
    /* empty		*/	0
  }
};

WidgetClass multiListWidgetClass = (WidgetClass)&multiListClassRec;

/*===========================================================================*

                       T O O L K I T    M E T H O D S

 *===========================================================================*/

/*---------------------------------------------------------------------------*

	Initialize()

	This procedure is called by the X toolkit to initialize
	the widget instance.  The hook to this routine is in the
	initialize part of the core part of the class.

 *---------------------------------------------------------------------------*/

/* ARGSUSED */
#if defined (__STDC__) && !defined(_NO_PROTO)
static void Initialize (MultiListWidget request, MultiListWidget new)
#else
static void Initialize (request, new)
     MultiListWidget request;
     MultiListWidget new;
#endif
{
  MultiListWidget mlw;

  mlw = (MultiListWidget)new;
  CreateNewGCs(new);
  InitializeNewData(new);

  /* used by the scrolling manager */
  new->multiList.pixel_offset = 0;

  /* initialize data used by MultiList widget */
  MultiListClipSet(mlw) = False;
  mlw->multiList.vscrollBar = NULL;

  /* timer features */
  mlw->multiList.last_button_time = 0;
  MultiListClickNumber(new) = 0;

  /* blink features */
  new->multiList.blink_on = False;
  new->multiList.num_blinking = 0;
  new->multiList.blink_timer = 0;

  /* cursor features */
  new->multiList.mcursor = XCreateFontCursor(XtDisplay(mlw), 
					     XC_sb_h_double_arrow);

  /* move separator flag */
  new->multiList.move_sep = 0;

  /* Now look at our parent and see if it's a ScrolledWindow.
   * If it is, create the scrollbars and set up all the scrolling stuff.
   */
  if (	(XtClass(XtParent(new)) != xmScrolledWindowWidgetClass)
	|| ( !MultiListUseScrollBar(new)))
    {
      RecalcCoords(new,(MultiListWidth(new) == 0),
		   (MultiListHeight(new) == 0));
      new->multiList.scrolled_win = NULL;
      return;
    }

  new->multiList.scrolled_win = XtParent(new);	

  new->multiList.vscrollBar = XtVaCreateManagedWidget("vscrollBar",
	      xmScrollBarWidgetClass,		new->multiList.scrolled_win,
	      XmNorientation,			XmVERTICAL,
	      XmNincrementCallback,		(XtArgVal) VSCallBack,
	      XmNdecrementCallback,		(XtArgVal) VSCallBack,
	      XmNpageIncrementCallback,	(XtArgVal) VSCallBack,
              XmNpageDecrementCallback,	(XtArgVal) VSCallBack,
	      XmNdragCallback,		(XtArgVal) VSCallBack,
	      NULL);

#if (XmVERSION != 2)
    XtAddCallback ( (Widget) mlw, XmNdestroyCallback, _XmDestroyParentCallback, NULL);
#endif

  RecalcCoords(new, False, False);

  MultiListSetVerticalScrollbar(new);

  XmScrolledWindowSetAreas(
			   new->multiList.scrolled_win,
			   NULL,
			   new->multiList.vscrollBar,
			   (Widget)new);

} /* Initialize */

/*---------------------------------------------------------------*

	MultiListRealize()

	This function is called to realize a ScrolledList widget.

 *---------------------------------------------------------------*/

#if defined (__STDC__) && !defined(_NO_PROTO)
static void MultiListRealize ( MultiListWidget mlw, XtValueMask *valueMask, 
			       XSetWindowAttributes *attrs )
#else
static void MultiListRealize (mlw, valueMask, attrs )
     MultiListWidget mlw;
     XtValueMask *valueMask;
     XSetWindowAttributes *attrs;
#endif
{


  XtCreateWindow((Widget)mlw,InputOutput,(Visual *)CopyFromParent,
		 *valueMask,attrs);

} /* End MultiListRealize */


/*---------------------------------------------------------------------------*

	Destroy()

	This procedure is called by the X toolkit to destroy
	the widget instance.  We need to destroy GC, XtCallbackList,
	XtIntervalId.

 *---------------------------------------------------------------------------*/

#if defined (__STDC__) && !defined(_NO_PROTO)
static void Destroy ( MultiListWidget mlw )
#else
static void Destroy (mlw)
     MultiListWidget mlw;
#endif
{
  int i = 0;
  /* The GCs */
  XtReleaseGC ((Widget)mlw, MultiListEraseGC(mlw));
  XtReleaseGC ((Widget)mlw, MultiListDrawGC(mlw));
  XtReleaseGC ((Widget)mlw, MultiListGrayGC(mlw));
  XtReleaseGC ((Widget)mlw, MultiListHighlightForeGC(mlw));
  XtReleaseGC ((Widget)mlw, MultiListHighlightBackGC(mlw));
  XtReleaseGC ((Widget)mlw, MultiListSepGC(mlw));

  for (i = 0; i < 8; i++) 
    XtReleaseGC ((Widget)mlw, MultiListAltFgGC(mlw, i));


   /* allocated data in the MultiListWidget */
  DestroyOldData(mlw);

  /* Destroy of mlw->multiList.vscrollBar is done by XmNdestroy callback
      which has been set up in Initialize() methode.
      So we don't need to do XtRemoveAllCallbacks for widget mlw->multiList.vscrollBar 
   */

   /* The blinking time-out proc */
  if (mlw->multiList.blink_timer != 0)
    {
      XtRemoveTimeOut(mlw->multiList.blink_timer);
      mlw->multiList.blink_timer = 0;
    }


} /* Destroy() */


/*---------------------------------------------------------------------------*

	Redisplay(mlw,event,rectangle_union)

	This routine redraws the MultiList widget <mlw> based on the exposure
	region requested in <event>.

 *---------------------------------------------------------------------------*/

#if defined (__STDC__) && !defined(_NO_PROTO)
static void Redisplay( MultiListWidget mlw, 
		       XEvent *event, Region rectangle_union )
#else
static void Redisplay(mlw, event, rectangle_union )
     MultiListWidget mlw;
     XEvent *event;
     Region rectangle_union;
#endif
{

#ifdef DEBUG
  /* if(event == NULL)
     printf ("MultiList Redisplay, event = NULL\n");
     else
     printf ("MultiList Redisplay, event_count = %d\n", event->xexpose.count); */
#endif


  if (event == NULL)
    {
      int border;

      border =  ( mlw->primitive.shadow_thickness +
		  mlw->primitive.highlight_thickness);

      XClearWindow(XtDisplay(mlw),XtWindow(mlw));
      MultiListSetClipRect(mlw);

      MultiListRedrawArea(mlw, border, border, 
			  MultiListWidth(mlw),MultiListHeight(mlw));
    }
  else
    {
      MultiListSetClipRect(mlw);
      MultiListRedrawArea(mlw,
			  event->xexpose.x,
			  event->xexpose.y,
			  event->xexpose.width,
			  event->xexpose.height);

    }
  /* restore the clip mask */
  MultiListUnsetClipRect(mlw);
  MultiListDrawShadow(mlw, True);

} /* End Redisplay */

/*---------------------------------------------------------------------------*

	PreferredGeometry(mlw,parent_idea,our_idea)

	This routine is called by the parent to tell us about the
	parent's idea of our width and/or height.  We then suggest
	our preference through <our_idea> and return the information
	to the parent.

 *---------------------------------------------------------------------------*/

#if defined (__STDC__) && !defined(_NO_PROTO)
static XtGeometryResult PreferredGeometry (MultiListWidget mlw, 
					   XtWidgetGeometry *parent_idea, 
					   XtWidgetGeometry *our_idea )
#else
static XtGeometryResult PreferredGeometry (mlw, parent_idea, our_idea )
     MultiListWidget mlw;
     XtWidgetGeometry *parent_idea;
     XtWidgetGeometry *our_idea;
#endif
{
  Dimension nw,nh;
  Boolean parent_wants_w,parent_wants_h,we_changed_size;
    
  /* if (!XtIsRealized(mlw)) 
     return (XtGeometryYes); */

  parent_wants_w = (parent_idea->request_mode) & CWWidth;
  parent_wants_h = (parent_idea->request_mode) & CWHeight;

  if (parent_wants_w)
    nw = parent_idea->width;
  else
    nw = MultiListWidth(mlw);

  if (parent_wants_h)
    nh = parent_idea->height;
  else
    nh = MultiListHeight(mlw);

  our_idea->request_mode = 0;
  if (!parent_wants_w && !parent_wants_h) return(XtGeometryYes);

  if (mlw->multiList.scrolled_win == NULL )
    we_changed_size = Layout(mlw,!parent_wants_w,!parent_wants_h,&nw,&nh);
  else
    {
      /* prefer to resize at our idea */
      we_changed_size = Layout(mlw, False, True, &nw, &nh);
    }

  our_idea->request_mode = (CWWidth | CWHeight);
  our_idea->width = nw;
  our_idea->height = nh;

  if (we_changed_size)
    /* prefer to resize at our idea */
    return(XtGeometryAlmost);
  else
    /* the size is correct, dont change the size */
    return(XtGeometryYes);
} /* End PreferredGeometry */


/*---------------------------------------------------------------------------*

	Resize(mlw)

	This function is called when the widget is being resized.  It
	recalculates the layout of the widget.

 *---------------------------------------------------------------------------*/

#if defined (__STDC__) && !defined(_NO_PROTO)
static void Resize ( MultiListWidget mlw )
#else
static void Resize (mlw )
     MultiListWidget mlw;
#endif
{
  Dimension width,height;
  Boolean size_changed;

  width = MultiListWidth(mlw);
  height = MultiListHeight(mlw);

  size_changed = Layout(mlw,False, False,&width,&height);

  MultiListSetVerticalScrollbar(mlw);


} /* End Resize */


/*---------------------------------------------------------------------------*

        AltGCChanged (cpl, npl, dpl)

	This function checks whether any altGC have been changed
	This is called after pointer 

 *---------------------------------------------------------------------------*/

#if defined (__STDC__) && !defined(_NO_PROTO)
static int AltGCChanged(MultiListWidget cpl, 
			MultiListWidget rpl, 
			MultiListWidget npl)
#else
static int AltGCChanged (cpl, rpl, npl)
     MultiListWidget cpl;
     MultiListWidget rpl;
     MultiListWidget npl;

#endif
{
  int i = 0;

  for (i = 0; i < 8; i++) {
    if (MultiListAltFgGC (cpl, i) != MultiListAltFgGC (npl, i))
      return 1;
  }
  return 0;
}

/*---------------------------------------------------------------------------*

	SetValues(cpl,rpl,npl)

	This routine is called when the user is changing resources.  <cpl>
	is the current widget before the user's changes have been instituted.
	<rpl> includes the original changes as requested by the user.  <npl>
	is the new resulting widget with the requested changes and with all
	superclass changes already made.

 *---------------------------------------------------------------------------*/

#if defined (__STDC__) && !defined(_NO_PROTO)
static Boolean SetValues (MultiListWidget cpl, 
			  MultiListWidget rpl, 
			  MultiListWidget npl)
#else
static Boolean SetValues (cpl, rpl, npl)
     MultiListWidget cpl;
     MultiListWidget rpl;
     MultiListWidget npl;
#endif
{
  Boolean redraw,recalc;
  int i = 0;

  redraw = False;
  recalc = False;

  /* Graphic Context Changes */

  if ((MultiListFG(cpl) != MultiListFG(npl)) ||
      (MultiListBG(cpl) != MultiListBG(npl)) ||
      (MultiListHighlightFG(cpl) != MultiListHighlightFG(npl)) ||
      (MultiListHighlightBG(cpl) != MultiListHighlightBG(npl)) ||
      (MultiListFont(cpl) != MultiListFont(npl)) ||
      AltGCChanged (cpl, rpl, npl) )
    {
      XtDestroyGC(MultiListEraseGC(cpl));
      XtDestroyGC(MultiListDrawGC(cpl));
      XtDestroyGC(MultiListHighlightForeGC(cpl));
      XtDestroyGC(MultiListHighlightBackGC(cpl));
      XtDestroyGC(MultiListGrayGC(cpl));

      for (i = 0; i < 8; i++)
	XtDestroyGC(MultiListAltFgGC(cpl, i));
      
      CreateNewGCs(npl);
      redraw = True;
    }

  /* Changes That Require Data Initialization */

  if ((MultiListList(cpl) != MultiListList(npl)) ||
      (MultiListSensitiveArray(cpl) != MultiListSensitiveArray(npl)) ||
      (MultiListSensitive(cpl) != MultiListSensitive(npl)) ||
      (MultiListAncesSensitive(cpl) != MultiListAncesSensitive(npl)) ||
      (MultiListNumItems(cpl) != MultiListNumItems(npl)) ||
      (MultiListMaxSelectable(cpl) != MultiListMaxSelectable(npl)))
    {
      DestroyOldData(cpl);
      InitializeNewData(npl);
      recalc = True;
      redraw = True;
    }

  /* Changes That Require Recalculating Coordinates */

  if ((MultiListWidth(cpl) != MultiListWidth(npl)) ||
      (MultiListHeight(cpl) != MultiListHeight(npl)) ||
      (MultiListColumnSpace(cpl) != MultiListColumnSpace(npl)) ||
      (MultiListRowSpace(cpl) != MultiListRowSpace(npl)) ||
      (MultiListDefaultCols(cpl) != MultiListDefaultCols(npl)) ||
      (MultiListPrefWidths(cpl) != MultiListPrefWidths(npl)) ||
      (MultiListSepWidth(cpl) != MultiListSepWidth(npl)) ||
      ((MultiListForceCols(cpl) != MultiListForceCols(npl)) &&
       (MultiListNumCols(cpl) != MultiListNumCols(npl))) ||
      (MultiListRowMajor(cpl) != MultiListRowMajor(npl)) ||
      (MultiListFont(cpl) != MultiListFont(npl)) ||
      (MultiListAlignment(cpl) != MultiListAlignment(npl)) ||
      (MultiListLongest(cpl) != MultiListLongest(npl)))
    {
      if(MultiListLongest(cpl) == MultiListLongest(npl))
	MultiListLongest(npl) = 0;
      recalc = True;
      redraw = True;
    }

  if (MultiListColWidth(cpl) != MultiListColWidth(npl))
    {
      XtWarning("columnWidth Resource Is Read-Only");
      MultiListColWidth(npl) = MultiListColWidth(cpl);
    }
  if (MultiListRowHeight(cpl) != MultiListRowHeight(npl))
    {
      XtWarning("rowHeight Resource Is Read-Only");
      MultiListRowHeight(npl) = MultiListRowHeight(cpl);
    }

  if (recalc)
    {


      RecalcCoords(npl,!MultiListWidth(npl),!MultiListHeight(npl));
      MultiListSetVerticalScrollbar(npl);

    }
    
  if (!XtIsRealized((Widget)cpl))
    return(False);
  else
    return(redraw);
} /* End SetValues */

/*===========================================================================*

                  D A T A    I N I T I A L I Z A T I O N

 *===========================================================================*/

/*---------------------------------------------------------------------------*

	DestroyOldData(mlw)

	This routine frees the internal list item array and sets the
	item count to 0.  This is normally done immediately before
	calling InitializeNewData() to rebuild the internal item
	array from new user specified arrays.

 *---------------------------------------------------------------------------*/
#if defined (__STDC__) && !defined(_NO_PROTO)
static void DestroyOldData (MultiListWidget mlw)
#else
static void DestroyOldData (mlw)
     MultiListWidget mlw;
#endif
{
  int i;

  if (MultiListItemArray(mlw) != NULL)	/* Free Old List */
    {
      for (i = 0; i < MultiListNumItems(mlw); i++)
	{
	  XtFree(MultiListItemString(MultiListNthItem(mlw,i)));
	}
      XtFree(MultiListItemArray(mlw));
    }
  if (MultiListSelArray(mlw) != NULL) XtFree(MultiListSelArray(mlw));
  MultiListSelArray(mlw) = NULL;
  MultiListNumSelected(mlw) = 0;
  MultiListItemArray(mlw) = NULL;
  MultiListNumItems(mlw) = 0;
} /* End DestroyOldData */


/*---------------------------------------------------------------------------*

	InitializeNewData(mlw)

	This routine takes a MultiList widget <mlw> and builds up new
	data item tables based on the string list and the sensitivity array.
	All previous data should have already been freed.  If the number
	of items is 0, they will be counted, so the array must be NULL
	terminated.  If the list of strings is NULL, this is treated as
	a list of 0 elements.  If the sensitivity array is NULL, all
	items are treated as sensitive.

	When this routine is done, the string list and sensitivity array
	fields will all be set to NULL, and the widget will not reference
	them again.

 *---------------------------------------------------------------------------*/

#if defined (__STDC__) && !defined(_NO_PROTO)
static void InitializeNewData (MultiListWidget mlw)
#else
static void InitializeNewData (mlw)
     MultiListWidget mlw;
#endif
{
  int i;
  MultiListItem *item;
  String *string_array;

  string_array = MultiListList(mlw);
  if (MultiListNumItems(mlw) == 0)		/* Count The Elements */
    {
      if (MultiListList(mlw) == NULL)	/* No elements */
	{
	  MultiListNumItems(mlw) = 0;
	}
      else
	{
	  for (i = 0; string_array[i] != NULL; i ++);
	  MultiListNumItems(mlw) = i;
	}
    }
  if (MultiListNumItems(mlw) == 0)		/* No Items */
    {
      MultiListItemArray(mlw) = NULL;
    }
  else
    {
      MultiListItemArray(mlw) = TypeAlloc(MultiListItem,MultiListNumItems(mlw));
      for (i = 0; i < MultiListNumItems(mlw); i++)
	{
	  item = MultiListNthItem(mlw,i);
	  if (MultiListSensitiveArray(mlw) == NULL ||
	      (MultiListSensitiveArray(mlw)[i] == True))
	    {
	      MultiListItemSensitive(item) = True;
	    }
	  else
	    {
	      MultiListItemSensitive(item) = False;
	    }
	  MultiListItemString(item) = StrCopy(string_array[i]);
	  MultiListItemHighlighted(item) = False;
	  MultiListItemFgIndex(item) = -1;
	}
    }
  if (MultiListMaxSelectable(mlw) == 0)
    {
      MultiListSelArray(mlw) = NULL;
      MultiListNumSelected(mlw) = 0;
    }
  else
    {
      MultiListSelArray(mlw) = TypeAlloc(int,MultiListMaxSelectable(mlw));
      MultiListNumSelected(mlw) = 0;
    }

  MultiListList(mlw) = NULL;
  MultiListSensitiveArray(mlw) = NULL;
} /* End InitializeNewData */

/*---------------------------------------------------------------------------*

	CreateNewGCs(mlw)

	This routine takes a MultiList widget <mlw> and creates a new set of
	graphic contexts for the widget based on the colors, fonts, etc.
	in the widget.  Any previous GCs are assumed to have already been
	destroyed.

 *---------------------------------------------------------------------------*/
#if defined (__STDC__) && !defined(_NO_PROTO)
static void CreateNewGCs (MultiListWidget mlw)
#else
static void CreateNewGCs (mlw)
     MultiListWidget mlw;
#endif
{
  XGCValues values;
  unsigned int attribs;
  int          i = 0;

  attribs = GCForeground | GCBackground | GCFont | GCFunction;
  values.foreground = MultiListFG(mlw) ^ MultiListBG(mlw);
  values.background = MultiListBG(mlw);
  values.font = MultiListFont(mlw)->fid;
  values.function = GXxor;
  MultiListSepGC(mlw) = XtGetGC((Widget)mlw, attribs, &values);

  attribs = GCForeground | GCBackground | GCFont;
  values.foreground = MultiListFG(mlw);
  values.background = MultiListBG(mlw);
  values.font = MultiListFont(mlw)->fid;
  MultiListDrawGC(mlw) = XtGetGC((Widget)mlw,attribs,&values);

  values.foreground = MultiListBG(mlw);
  MultiListEraseGC(mlw) = XtGetGC((Widget)mlw,attribs,&values);

  values.foreground = MultiListHighlightFG(mlw);
  values.background = MultiListHighlightBG(mlw);
  MultiListHighlightForeGC(mlw) = XtGetGC((Widget)mlw,attribs,&values);

  values.foreground = MultiListHighlightBG(mlw);
  values.background = MultiListHighlightBG(mlw);
  MultiListHighlightBackGC(mlw) = XtGetGC((Widget)mlw,attribs,&values);

  for (i = 0; i < 8; i++) {
    values.foreground = MultiListAltFG(mlw, i);
    values.background = MultiListBG(mlw); 
    values.font = MultiListFont(mlw)->fid;
    MultiListAltFgGC(mlw, i) = XtGetGC ((Widget)mlw, attribs, &values);
  }

  attribs |= GCTile | GCFillStyle;
  values.foreground = MultiListFG(mlw);
  values.background = MultiListBG(mlw);
  values.fill_style = FillTiled;
  values.tile = XmuCreateStippledPixmap(XtScreen(mlw),MultiListFG(mlw),
					MultiListBG(mlw),MultiListDepth(mlw));

  MultiListGrayGC(mlw) = XtGetGC((Widget)mlw,attribs,&values);
} /* End CreateNewGCs */

/*===========================================================================*

        L A Y O U T    A N D    G E O M E T R Y    M A N A G E M E N T

 *===========================================================================*/

/*---------------------------------------------------------------------------*

        RecalcCoords(mlw,width_changeable,height_changeable)

	This routine takes a MultiList widget <mlw> and recalculates
	the coordinates, and item placement based on the current
	width, height, and list of items.  The <width_changeable> and
	<height_changeable> indicate if the width and/or height can
	be arbitrarily set.

	This routine requires that the internal list data be initialized.

 *---------------------------------------------------------------------------*/

#if defined (__STDC__) && !defined(_NO_PROTO)
static void RecalcCoords (MultiListWidget mlw,
			  Boolean width_changeable, 
			  Boolean height_changeable)
#else
static void RecalcCoords (mlw, width_changeable, height_changeable)
     MultiListWidget mlw;
     Boolean width_changeable;
     Boolean height_changeable;
#endif

{
  String str;
  Dimension width,height;
  register int i,text_width;

  width = MultiListWidth(mlw);
  height = MultiListHeight(mlw);
  if (MultiListNumItems(mlw) != 0 && MultiListLongest(mlw) == 0)
    {
      for (i = 0; i < MultiListNumItems(mlw); i++)
	{
	  str = MultiListItemString(MultiListNthItem(mlw,i));
	  text_width = FontW(MultiListFont(mlw),str);
	  MultiListLongest(mlw) = max(MultiListLongest(mlw),text_width);
	}
    }
  if (Layout(mlw,width_changeable,height_changeable,&width,&height))
    {
      NegotiateSizeChange(mlw,width,height);
    }
} /* End RecalcCoords */


/*---------------------------------------------------------------------------*

        NegotiateSizeChange(mlw,width,height)

	This routine tries to change the MultiList widget <mlw> to have the
	new size <width> by <height>.  A negotiation will takes place
	to try to change the size.  The resulting size is not necessarily
	the requested size.

 *---------------------------------------------------------------------------*/

#if defined (__STDC__) && !defined(_NO_PROTO)
static void NegotiateSizeChange (MultiListWidget mlw, 
				 Dimension width, Dimension height)
#else
static void NegotiateSizeChange (mlw, width, height)
MultiListWidget mlw;
Dimension width;
Dimension height;
#endif
{
  int attempt_number;
  Boolean w_fixed,h_fixed;
  Dimension *w_ptr,*h_ptr;
	
  XtWidgetGeometry request,reply;

  request.request_mode = CWWidth | CWHeight;
  request.width = width;
  request.height = height;
    
  for (attempt_number = 1; attempt_number <= 3; attempt_number++)
    {
      switch (XtMakeGeometryRequest((Widget)mlw,&request,&reply))
	{
	case XtGeometryYes:
	  return;
	case XtGeometryNo:
	  return;
	case XtGeometryAlmost:
	  switch (attempt_number)
	    {
	    case 1:
	      w_fixed = (request.width != reply.width);
	      h_fixed = (request.height != reply.height);
	      w_ptr = &(reply.width);
	      h_ptr = &(reply.height);
	      Layout(mlw,!w_fixed,!h_fixed,w_ptr,h_ptr);
	      break;
	    case 2:
	      w_ptr = &(reply.width);
	      h_ptr = &(reply.height);
	      Layout(mlw,False,False,w_ptr,h_ptr);
	      break;
	    case 3:
	      return;
	    }
	  break;
	default:
	  XtAppWarning(XtWidgetToApplicationContext((Widget)mlw),
		       "MultiList Widget: Unknown geometry return.");
	  break;
	}
      request = reply;
    }
} /* End NegotiateSizeChange */


/*---------------------------------------------------------------------------*

	Boolean Layout(mlw,w_changeable,h_changeable,w_ptr,h_ptr)

	This routine tries to generate a layout for the MultiList widget
	<mlw>.  The Layout routine is free to arbitrarily set the width
	or height if the corresponding variables <w_changeable> and
	<h_changeable> are set True.  Otherwise the original width or
	height in <w_ptr> and <h_ptr> are used as fixed values.  The
	resulting new width and height are stored back through the
	<w_ptr> and <h_ptr> pointers.  False is returned if no size
	change was done, True is returned otherwise.

 *---------------------------------------------------------------------------*/

#if defined (__STDC__) && !defined(_NO_PROTO)
static Boolean Layout (MultiListWidget mlw, 
		       Boolean w_changeable, 
		       Boolean h_changeable, 
		       Dimension *w_ptr, 
		       Dimension *h_ptr)
#else
static Boolean Layout (mlw, w_changeable, h_changeable, w_ptr, h_ptr)
     MultiListWidget mlw;
     Boolean w_changeable;
     Boolean h_changeable;
     Dimension *w_ptr;
     Dimension *h_ptr;
#endif
{
  Boolean size_changed;
  Dimension primitive_border;
  Dimension  tw = 0;
  int        arraysize = 0;

  primitive_border = 2 * ( mlw->primitive.shadow_thickness +
			   mlw->primitive.highlight_thickness);

  /*
   * If force columns is set, then always use the number
   * of columns specified by default_cols.
   */

  MultiListColWidth(mlw) = MultiListLongest(mlw) + MultiListColumnSpace(mlw);
  MultiListRowHeight(mlw) = FontH(MultiListFont(mlw)) + MultiListRowSpace(mlw);

  if (MultiListForceCols(mlw))
    {
      MultiListNumCols(mlw) = max(MultiListDefaultCols(mlw),1);
      if (MultiListNumItems(mlw) == 0)
	MultiListNumRows(mlw) = 1;
      else
	MultiListNumRows(mlw) = (MultiListNumItems(mlw) - 1) /
	MultiListNumCols(mlw) + 1;
      if (w_changeable)
	{
	  if ((arraysize = NumColumnWidths (mlw)) == 0)
	    *w_ptr = MultiListNumCols(mlw) * MultiListColWidth(mlw) + primitive_border ;
	  else {
	    tw = TotalWidth (mlw, MultiListNumCols (mlw));
	    *w_ptr = tw + primitive_border;
	  }
	  size_changed = True;
	}
      else
	{
	  MultiListColWidth(mlw) = (int)(*w_ptr - primitive_border) / MultiListNumCols(mlw);
	}
      if (h_changeable)
	{
	  *h_ptr = MultiListNumRows(mlw) * MultiListRowHeight(mlw) + primitive_border;
	  size_changed = True;
	}
      return(size_changed);
    }

  /*
   * If both width and height are free to change then use
   * default_cols to determine the number of columns and set
   * the new width and height to just fit the window.
   */

  if (w_changeable && h_changeable)
    {
      MultiListNumCols(mlw) = max(MultiListDefaultCols(mlw),1);
      if (MultiListNumItems(mlw) == 0)
	MultiListNumRows(mlw) = 1;
      else
	MultiListNumRows(mlw) = (MultiListNumItems(mlw) - 1) /
	MultiListNumCols(mlw) + 1;
      if ((arraysize = NumColumnWidths (mlw)) == 0)
	*w_ptr = MultiListNumCols(mlw) * MultiListColWidth(mlw) + primitive_border;
      else {
	tw = TotalWidth (mlw, MultiListNumCols (mlw));
	*w_ptr = tw + primitive_border;	
      }
      *h_ptr = MultiListNumRows(mlw) * MultiListRowHeight(mlw) + primitive_border;
      return(True);
    }

  /*
   * If the width is fixed then use it to determine the
   * number of columns.  If the height is free to move
   * (width still fixed) then resize the height of the
   * widget to fit the current MultiList exactly.
   */

  if (!w_changeable)
    {
      MultiListNumCols(mlw) = (int)(*w_ptr - primitive_border)/ (int)MultiListColWidth(mlw);
      MultiListNumCols(mlw) = max(MultiListNumCols(mlw),1);
      MultiListNumRows(mlw) = (MultiListNumItems(mlw) - 1) /
	MultiListNumCols(mlw) + 1;
      MultiListColWidth(mlw) = (int)(*w_ptr - primitive_border) / MultiListNumCols(mlw);
      if (h_changeable)
	{
	  *h_ptr = MultiListNumRows(mlw) * MultiListRowHeight(mlw) + primitive_border;
	  size_changed = True;
	}
      return(size_changed);
    }

  /*
   * The last case is xfree and !yfree we use the height to
   * determine the number of rows and then set the width to
   * just fit the resulting number of columns.
   */

  if (!h_changeable)
    {
      MultiListNumRows(mlw) = (int) (*h_ptr - primitive_border) / (int) MultiListRowHeight(mlw);
      MultiListNumRows(mlw) = max(MultiListNumRows(mlw),1);
      MultiListNumCols(mlw) = (MultiListNumItems(mlw) - 1) /
	MultiListNumRows(mlw) + 1;
      *w_ptr = MultiListNumCols(mlw) * MultiListColWidth(mlw) + primitive_border;
      return(True);
    }
  return(size_changed);
} /* End Layout */

/*===========================================================================*

                    S C R O L L B A R   S T U F F

 *===========================================================================*/
/*---------------------------------------------------------------------------*

	MultiListVertSliderMove( w ,closure, call_data)

	Callback for the sliderMoved resource of the vertical scrollbar.

 *---------------------------------------------------------------------------*/

#if defined (__STDC__) && !defined(_NO_PROTO)
static void MultiListVertSliderMove (Widget w, caddr_t closure, 
				     XmScrollBarCallbackStruct *call_data)
#else
static void MultiListVertSliderMove (w, closure, call_data)
     Widget w;
     caddr_t closure;
     XmScrollBarCallbackStruct *call_data;
#endif
{
  MultiListWidget mlw;
  int value;

  mlw = (MultiListWidget) (((XmScrolledWindowWidget)w->core.parent)->swindow.WorkWindow);
  value = call_data->value ;

  /* mlw->multiList.pixel_offset = value; */

#ifdef DEBUG
  /* printf("value = %d\n",value); */
#endif
  MultiListScroll(mlw, value);


} /* End MultiListVertSliderMove */

/*---------------------------------------------------------------------------*

	MultiListSetVerticalScrollbar(mlw)

	Set up on an item basis. Min is 0, max is number of rows, origin is
	top_position, extent is number of visible item.

 *---------------------------------------------------------------------------*/

#if defined (__STDC__) && !defined(_NO_PROTO)
static void MultiListSetVerticalScrollbar (MultiListWidget mlw)
#else
static void MultiListSetVerticalScrollbar (mlw)
     MultiListWidget mlw;
#endif
{
  int 		vis_part;	/* visible part in pixels */
  int		max_height, value;	/* setup scrollBar  */


  if (mlw->multiList.vscrollBar == NULL)
    return;

  /* this will be XmNsliderSize */
  vis_part = MultiListHeight(mlw) - 2 * ( mlw->primitive.shadow_thickness +
					  mlw->primitive.highlight_thickness);

  /* this will be XmNmaximum */
  max_height = MultiListRowHeight(mlw) * MultiListNumRows(mlw);

  max_height = max (max_height, vis_part);


  /* setup the scrollbar value, we have to check that 
     XmNvalue + XmNsliderSize <= XmNmaximum
     */

  if ( mlw->multiList.pixel_offset > (max_height - vis_part))
    {
      value = max_height - vis_part;
      mlw->multiList.pixel_offset = value;
    }
  else
    value = mlw->multiList.pixel_offset;

  XtVaSetValues(mlw->multiList.vscrollBar,
		XmNminimum,		0,
		XmNmaximum,		max_height,
		XmNsliderSize,		max(1,vis_part),
		XmNincrement,		MultiListRowHeight(mlw),
		XmNpageIncrement,	max(1,vis_part),
		XmNvalue,		value,
		NULL);
 
} /* End MultiListSetVerticalScrollbar */


/*===========================================================================*

                 P R I M I T I V E   R E D R A W    R O U T I N E S

 *===========================================================================*/

/*---------------------------------------------------------------------------*

	MultiListDrawShadow(mlw)

	This routine redraw a Motif primitive shadaw in the
	MultiList widget <mlw>.

 *---------------------------------------------------------------------------*/

#if defined (__STDC__) && !defined(_NO_PROTO)
static void MultiListDrawShadow (MultiListWidget mlw, Boolean armed)
#else
static void MultiListDrawShadow (mlw, armed)
     MultiListWidget mlw; 
     Boolean armed;
#endif
{

  if (mlw->primitive.shadow_thickness > 0)
    _XmDrawShadow (XtDisplay (mlw), XtWindow (mlw), 
		   (armed
		    ? mlw -> primitive.bottom_shadow_GC
		    : mlw -> primitive.top_shadow_GC),
		   (armed 
		    ? mlw -> primitive.top_shadow_GC 
		    : mlw -> primitive.bottom_shadow_GC), 
		   mlw -> primitive.shadow_thickness,
		   mlw -> primitive.highlight_thickness,
		   mlw -> primitive.highlight_thickness,
		   mlw -> core.width - 2 * mlw->primitive.highlight_thickness,
		   mlw -> core.height - 2 *mlw->primitive.highlight_thickness);
}

/*---------------------------------------------------------------------------*

	MultiListSetClipRect(mlw)

	This routine set a clipping rectangle for the visible area of the
	MultiList widget <mlw> and will mark that a clip rectangle is set
	with the boolean MultiListClipSet(mlw).

	This fonction is called in RedrawRowColumn().
	The clip rectangle is reset in the MultiListUnsetClipRect() fonction.

 *---------------------------------------------------------------------------*/

#if defined (__STDC__) && !defined(_NO_PROTO)
static void MultiListSetClipRect (MultiListWidget mlw)
#else
static void MultiListSetClipRect (mlw)
     MultiListWidget mlw;
#endif
{
  int primitive_border;
  register Position x,y;
  Dimension w,h;
  XRectangle rect;
  int i = 0;

  if(MultiListClipSet(mlw))
    return;

  primitive_border =  mlw->primitive.shadow_thickness +
    mlw->primitive.highlight_thickness;

  x = primitive_border;
  y = primitive_border;
  w = mlw->core.width  - (2 * primitive_border);
  h = mlw->core.height - (2 * primitive_border);
    
  rect.x = 0;
  rect.y = 0;
  rect.width = w;
  rect.height = h;

  if (MultiListEraseGC(mlw))
    XSetClipRectangles(XtDisplay(mlw), MultiListEraseGC(mlw), x, y, 
		       &rect, 1, Unsorted);

  if (MultiListDrawGC(mlw))
    XSetClipRectangles(XtDisplay(mlw), MultiListDrawGC(mlw), x, y, 
		       &rect, 1, Unsorted);

  if (MultiListSepGC(mlw))
    XSetClipRectangles(XtDisplay(mlw), MultiListSepGC(mlw), x, y, 
		       &rect, 1, Unsorted);

  if (MultiListHighlightForeGC(mlw))
    XSetClipRectangles(XtDisplay(mlw), MultiListHighlightForeGC(mlw), x, y, 
		       &rect, 1, Unsorted);

  if (MultiListHighlightBackGC(mlw))
    XSetClipRectangles(XtDisplay(mlw), MultiListHighlightBackGC(mlw), x, y, 
		       &rect, 1, Unsorted);

  if (MultiListGrayGC(mlw))
    XSetClipRectangles(XtDisplay(mlw), MultiListGrayGC(mlw), x, y, 
		       &rect, 1, Unsorted);

  for (i = 0; i < 8; i++)
    if (MultiListAltFgGC(mlw, i))
      XSetClipRectangles(XtDisplay(mlw), MultiListAltFgGC(mlw, i), x, y, 
			 &rect, 1, Unsorted);

  MultiListClipSet(mlw) = True;
    
}
/*---------------------------------------------------------------------------*

	MultiListUnsetClipRect(mlw)

	This routine unset a clipping rectangle.

 *---------------------------------------------------------------------------*/

#if defined (__STDC__) && !defined(_NO_PROTO)
static void MultiListUnsetClipRect (MultiListWidget mlw)
#else
static void MultiListUnsetClipRect (mlw)
     MultiListWidget mlw;
#endif
{
  int i = 0;

  if(!MultiListClipSet(mlw))
    return;


  if (MultiListEraseGC(mlw))
    XSetClipMask(XtDisplay(mlw), MultiListEraseGC(mlw), None);

  if (MultiListDrawGC(mlw))
    XSetClipMask(XtDisplay(mlw), MultiListDrawGC(mlw), None);

  if (MultiListSepGC(mlw))
    XSetClipMask(XtDisplay(mlw), MultiListSepGC(mlw), None);

  if (MultiListHighlightForeGC(mlw))
    XSetClipMask(XtDisplay(mlw), MultiListHighlightForeGC(mlw), None);

  if (MultiListHighlightBackGC(mlw))
    XSetClipMask(XtDisplay(mlw), MultiListHighlightBackGC(mlw), None);

  if (MultiListGrayGC(mlw))
    XSetClipMask(XtDisplay(mlw), MultiListGrayGC(mlw), None);

  for (i = 0; i < 8; i++)
    if (MultiListAltFgGC(mlw, i))
      XSetClipMask(XtDisplay(mlw), MultiListAltFgGC(mlw, i), None);

  MultiListClipSet(mlw) = False;
    
}
/*===========================================================================*

                    R E D R A W    R O U T I N E S

 *===========================================================================*/



/*---------------------------------------------------------------------------*

	RedrawAll(mlw)

	This routine simple calls Redisplay to redraw the entire
	MultiList widget <mlw>.

 *---------------------------------------------------------------------------*/

#if defined (__STDC__) && !defined(_NO_PROTO)
static void RedrawAll (MultiListWidget mlw)
#else
static void RedrawAll (mlw)
     MultiListWidget mlw;
#endif
{
  Redisplay(mlw,NULL,NULL);
} /* End RedrawAll */


/*---------------------------------------------------------------------------*

        MultiListRefresh (Widget w)

	This routine simple calls Redisplay to redraw the entire
	MultiList widget <mlw>.

 *---------------------------------------------------------------------------*/
#if defined (__STDC__) && !defined(_NO_PROTO)
void MultiListRefresh (Widget w)
#else
void MultiListRefresh (w)
     Widget w;
#endif
{
  MultiListWidget mlw = (MultiListWidget)w;
  Redisplay(mlw,NULL,NULL);
} 

/*---------------------------------------------------------------------------*

	RedrawItem(mlw,item_index)

	This routine redraws the item with index <item_index> in the
	MultiList widget <mlw>.  If the item number is bad, nothing is drawn.

 *---------------------------------------------------------------------------*/

#if defined (__STDC__) && !defined(_NO_PROTO)
static void RedrawItem (MultiListWidget mlw, int item_index)
#else
static void RedrawItem (mlw, item_index)
     MultiListWidget mlw; 
     int item_index;
#endif
{
  int row,column;

  if ( MultiListItemIsVisible( (Widget)mlw,item_index) == MULTILIST_NO)
    return;

  if (ItemToRowColumn(mlw,item_index,&row,&column))
    {
      RedrawRowColumn(mlw,row,column);
    }
} /* End RedrawItem */

/*---------------------------------------------------------------------------*

	MultiListScroll(mlw, offset)

	This routine redraw the MultiList widget using offset to simulate
	XtMoveWidget. The offset value is in pixel and comes from scrollbar
	value.

	When the redraw is finished, we store the new offset in pixel_offset
	part of MultiList widget. We do this because we need to use the old
	offset value to do an XCopyArea.

 *---------------------------------------------------------------------------*/

#if defined (__STDC__) && !defined(_NO_PROTO)
static void MultiListScroll (MultiListWidget mlw, int offset)
#else
static void MultiListScroll (mlw, offset)
     MultiListWidget mlw;
     int offset;
#endif
{
  int delta;	/* difference between offset and old offset */
  int primitive_border;
  int x,h,w,y1,y2,y3;

  delta = mlw->multiList.pixel_offset - offset ;

  /* now save the offset value to the MultiList widget */
  mlw->multiList.pixel_offset = offset;

  primitive_border = (	mlw->primitive.shadow_thickness + 
			mlw->primitive.highlight_thickness);
  x = primitive_border;
  w = MultiListWidth(mlw) - 2 * primitive_border;
  h = MultiListHeight(mlw) - 2 * primitive_border;
  if ( abs(delta) > h )
    {
      /* we are scrolling more than MultiList height 
	 so we dont need to move area 
	 */
      y3 = primitive_border;
      delta = h;
    }	
  else 
    {
      if ( delta < 0 )
	{
	  /* we are scrolling down the ScrollBar, we have to 
	     - move the bottom area up delta pixels,
	     - redraw the bottom part of the multiList widget
	     of delta pixels height.
	     */
	  delta = -delta;
	  y1 = primitive_border + delta;		/* source for XCopyArea */
	  y2 = primitive_border;			/* destination for XCopyArea */
	  y3 = primitive_border + h - delta;	/* pos of the area to refresh */
	} else
	  {
	    /* we are scrolling up */
	    y1 = primitive_border;			/* source for XCopyArea */
	    y2 = primitive_border + delta ;		/* destination for XCopyArea */
	    y3 = primitive_border;			/* pos of the area to refresh */
	  }

      /* now we have to move the area */
      XCopyArea(XtDisplay(mlw),XtWindow(mlw),
		XtWindow(mlw), 
		MultiListDrawGC(mlw),
		x,
		y1,
		w,
		h - delta,
		x,
		y2
		);
    } /* else if abs(delta) > h */

  /* and now refresh the new area */
  MultiListRedrawArea(mlw,x,y3,w,delta);	
	
} /* End MultiListScroll */

/*---------------------------------------------------------------------------*

	MultiListRedrawArea(mlw, x,y,width,height)

	This routine redraw the MultiList widget area

 *---------------------------------------------------------------------------*/

#if defined (__STDC__) && !defined(_NO_PROTO)
static void MultiListRedrawArea (MultiListWidget mlw, 
				 int x1, int y1, int w, int h)
#else
static void MultiListRedrawArea (mlw, x1, y1, w, h)
     MultiListWidget mlw;
     int x1;
     int y1;
     int w;
     int h;
#endif
{
  int primitive_border;
  int x2,y2,row,col,ul_row,ul_col,lr_row,lr_col;

  primitive_border =  ( mlw->primitive.shadow_thickness +
			mlw->primitive.highlight_thickness);

  x2 = x1 + w;
  y2 = y1 + h;

  MultiListSetClipRect(mlw);
  XFillRectangle(XtDisplay(mlw),XtWindow(mlw),MultiListEraseGC(mlw),
		 x1,y1,w,h);

  if ( MultiListNumItems(mlw) == 0 ) {
    PixelToRowColumn(mlw,x1,y1,&ul_row,&ul_col);
    PixelToRowColumn(mlw,x2,y2,&lr_row,&lr_col);
#ifdef DEBUG
    printf ("MultiListRedrawArea : %d %d\n",ul_row,lr_row); 
#endif
    lr_row = min(lr_row,MultiListNumRows(mlw) - 1);
    lr_col = min(lr_col,MultiListNumCols(mlw) - 1);

    for (col = ul_col; col <= lr_col; col++)
      {
	if (y1 - primitive_border < 0)
	  y1 = primitive_border;
	if (y2 > MultiListHeight(mlw) - primitive_border)
	  y2 = MultiListHeight(mlw) - primitive_border;
	MultiListDrawSeparator (mlw, col, y1, y2);
      }
    MultiListUnsetClipRect(mlw);
    return;
  }

  PixelToRowColumn(mlw,x1,y1,&ul_row,&ul_col);
  PixelToRowColumn(mlw,x2,y2,&lr_row,&lr_col);
#ifdef DEBUG
  /* printf ("MultiListRedrawArea : %d %d\n",ul_row,lr_row); */
#endif
  lr_row = min(lr_row,MultiListNumRows(mlw) - 1);
  lr_col = min(lr_col,MultiListNumCols(mlw) - 1);

  for (col = ul_col; col <= lr_col; col++)
    {
      if (y1 - primitive_border < 0)
	y1 = primitive_border;
      if (y2 > MultiListHeight(mlw) - primitive_border)
	y2 = MultiListHeight(mlw) - primitive_border;
      MultiListDrawSeparator (mlw, col, y1, y2);
    

      for (row = ul_row; row <= lr_row; row++)
	{
	  RedrawRowColumn(mlw,row,col);
	}
    }
  MultiListUnsetClipRect(mlw);

} /* End MultiListRedrawArea */


/*---------------------------------------------------------------------------*

	MultiListRefreshArea(mlw, x,y,width,height)

	This routine redraw the MultiList widget area using blinking feature.

 *---------------------------------------------------------------------------*/
#if defined (__STDC__) && !defined(_NO_PROTO)
static void MultiListRefreshArea (MultiListWidget mlw, 
				  int x1, int y1, int w, int h)
#else
static void MultiListRefreshArea (mlw, x1, y1, w, h)
     MultiListWidget mlw;
     int x1;
     int y1;
     int w;
     int h;
#endif
{
  int x2,y2,row,col,ul_row,ul_col,lr_row,lr_col;
  int item_index;
  Boolean has_item;
  MultiListItem *item;

  if (mlw->multiList.blink == False)
    return;

  x2 = x1 + w;
  y2 = y1 + h;

#ifdef DEBUG
  /* printf ("MultiListRefreshArea : %d %d\n",ul_row,lr_row); */
#endif

  MultiListSetClipRect(mlw);

  PixelToRowColumn(mlw,x1,y1,&ul_row,&ul_col);
  PixelToRowColumn(mlw,x2,y2,&lr_row,&lr_col);

  lr_row = min(lr_row,MultiListNumRows(mlw) - 1);
  lr_col = min(lr_col,MultiListNumCols(mlw) - 1);

  for (col = ul_col; col <= lr_col; col++)
    {
      for (row = ul_row; row <= lr_row; row++)
	{
	  has_item = RowColumnToItem(mlw,row,col,&item_index);
	  if (has_item)
	    {
	      item = MultiListNthItem(mlw,item_index);
	      if(MultiListItemBlinking(item) == True)
		RedrawRowColumn(mlw,row,col);
	    }
	}
    }
  MultiListUnsetClipRect(mlw);

} /* End MultiListRefreshArea */


/*---------------------------------------------------------------------------*

	RedrawRowColumn(mlw,row,column)

	This routine paints the item in row/column position <row>,<column>
	on the MultiList widget <mlw>.  If the row/column coordinates are
	outside the widget, nothing is drawn.  If the position is empty,
	blank space is drawn.

 *---------------------------------------------------------------------------*/

#if defined (__STDC__) && !defined(_NO_PROTO)
static void RedrawRowColumn (MultiListWidget mlw, int row, int column)
#else
static void RedrawRowColumn (mlw, row, column)
     MultiListWidget mlw;
     int row;
     int column;
#endif
{
  GC bg_gc,fg_gc, exchange_gc;
  MultiListItem *item;
  int ul_x,ul_y,str_x,str_y,w,h,item_index,has_item,text_h;
  Boolean draw_string = True ;	/* used with blinking */
  int i = 0;
  int dw;

  if (!XtIsRealized((Widget)mlw)) return;
  has_item = RowColumnToItem(mlw,row,column,&item_index);
  RowColumnToPixels(mlw,row,column,&ul_x,&ul_y,&w,&h);

  if (has_item == False)					/* No Item */
    {
      bg_gc = MultiListEraseGC(mlw);
    }
  else
    {
      item = MultiListNthItem(mlw,item_index);
      if (!MultiListItemSensitive(item) ||
	  !MultiListSensitive(mlw))			/* Disabled */
	{
	  bg_gc = MultiListEraseGC(mlw);
	  fg_gc = MultiListGrayGC(mlw);
	}
      else if (MultiListItemHighlighted(item))	/* Selected */
	{
	  bg_gc = MultiListHighlightBackGC(mlw);
	  fg_gc = MultiListHighlightForeGC(mlw);
	}
      else if (MultiListItemFgIndex(item) >= 0 && 
	       MultiListItemFgIndex(item) < 8)
	{
	  i = MultiListItemFgIndex(item);
	  bg_gc = MultiListEraseGC(mlw);
	  fg_gc = MultiListAltFgGC(mlw, i);
	}
      else					/* Normal */
	{
	  bg_gc = MultiListEraseGC(mlw);
	  fg_gc = MultiListDrawGC(mlw);
	}

      if( 	( mlw->multiList.blink ) && 
		(MultiListItemBlinking(item)) &&
		(mlw->multiList.blink_on == True))
	{
	  if (mlw->multiList.reverse_video)
	    {
	      /* exchange foreground and background GC */
	      exchange_gc = bg_gc;
	      bg_gc = fg_gc; fg_gc = exchange_gc;
	    } else
	      {
		/* set foreground = background */
		fg_gc = bg_gc;
		draw_string = False;
	      }
	}
    }

  XFillRectangle(XtDisplay(mlw),XtWindow(mlw),bg_gc,ul_x,ul_y,w,h);

  if ((has_item == True) && (draw_string == True) && 
      (MultiListItemString(item) != NULL)) {
    if (MultiListAlignment(mlw) == XmALIGNMENT_BEGINNING) 
      str_x = ul_x + MultiListColumnSpace(mlw) / 2;
    else if (MultiListAlignment(mlw) == XmALIGNMENT_CENTER) {
      dw = w - FontW(MultiListFont(mlw),MultiListItemString(item)) - 
	MultiListColumnSpace(mlw);
      if (dw < 0)
	dw = 0;
      str_x = ul_x + MultiListColumnSpace(mlw)/2 + dw/2;
    }
    else {
      dw = FontW(MultiListFont(mlw),MultiListItemString(item));
      if (dw + MultiListColumnSpace(mlw) > w)  /* list too long */
	str_x = ul_x + MultiListColumnSpace(mlw) / 2;
      else
	str_x = ul_x +  MultiListColumnSpace(mlw) / 2 + 
	(w - dw - MultiListColumnSpace(mlw));
    }

    text_h = min((int) (FontH(MultiListFont(mlw)) + MultiListRowSpace(mlw)),
		 (int) MultiListRowHeight(mlw));

    str_y = ul_y + FontAscent(MultiListFont(mlw)) + 
      (int) (MultiListRowHeight(mlw) - text_h) / 2;
      
    XDrawString(XtDisplay(mlw),XtWindow(mlw),fg_gc,
		str_x,str_y,MultiListItemString(item),
		strlen(MultiListItemString(item)));

    }
} /* End RedrawRowColumn */
	
/*===========================================================================*

               I T E M    L O C A T I O N    R O U T I N E S

 *===========================================================================*/

/*---------------------------------------------------------------------------*

	void PixelToRowColumn(mlw,x,y,row_ptr,column_ptr)

	This routine takes pixel coordinates <x>, <y> and converts
	the pixel coordinate into a row/column coordinate.  This row/column
	coordinate can then easily be converted into the specific item
	in the list via the function RowColumnToItem().

	If the pixel lies in blank space outside of the items, the
	row & column numbers will be outside of the range of normal
	row & columns numbers, but will correspond to the row & column
	of the item, if an item was actually there.

 *---------------------------------------------------------------------------*/

#if defined (__STDC__) && !defined(_NO_PROTO)
static void PixelToRowColumn (MultiListWidget mlw, 
			      int x, int y, 
			      int *row_ptr, int *column_ptr)
#else
static void PixelToRowColumn (mlw, x, y, row_ptr, column_ptr)
     MultiListWidget mlw;
     int x;
     int y;
     int *row_ptr;
     int *column_ptr;
#endif
{
  int primitive_border;
  Dimension* array = 0;
  int        arraysize = 0;
  int        i = 0;
  Dimension  tw = 0;
  Dimension  ow = 0;

  primitive_border =  ( mlw->primitive.shadow_thickness +
			mlw->primitive.highlight_thickness);

  /* if x or y = 0 and primitive_border > 0 we can use max() because 
     we are computing with signed values.
     */

  x = max(primitive_border, x);
  y = max(primitive_border, y);

  if(MultiListRowHeight(mlw) == 0)
    {
      *row_ptr = 0;
      XtWarning("MultiList widget PixelToRowColumn(): XmNrowHeight = 0");
    }
  else
    *row_ptr =   (y - primitive_border + mlw->multiList.pixel_offset) /
    (int) MultiListRowHeight(mlw);


  if (MultiListColWidth(mlw) == 0)
    {
      *column_ptr = 0;
      XtWarning("MultiList widget PixelToRowColumn(): XmNcolWidth = 0");
    }
  else {
    if ((arraysize = NumColumnWidths (mlw)) == 0)
      *column_ptr = (x - primitive_border) / (int)MultiListColWidth(mlw);
    else 
      *column_ptr = ColumnByPixel (mlw, x - primitive_border);
  }
      

} /* End PixelToRowColumn */

/*---------------------------------------------------------------------------*

	void RowColumnToPixels(mlw,row,col,x_ptr,y_ptr,w_ptr,h_ptr)

	This routine takes a row/column coordinate <row>,<col> and
	converts it into the bounding pixel rectangle which is returned.

 *---------------------------------------------------------------------------*/

#if defined (__STDC__) && !defined(_NO_PROTO)
static void RowColumnToPixels (MultiListWidget mlw, 
			       int row, int col, 
			       int *x_ptr, int *y_ptr, 
			       int *w_ptr, int *h_ptr)
#else
static void RowColumnToPixels (mlw, row, col, x_ptr, y_ptr, w_ptr, h_ptr)
     MultiListWidget mlw;
     int row;
     int col;
     int *x_ptr;
     int *y_ptr;
     int *w_ptr;
     int *h_ptr;
#endif
{
  int primitive_border;
  int arraysize;
  Dimension* array = 0;
  Dimension  tw = 0;
  Dimension  ow = 0;
  int        i = 0;

  primitive_border = ( mlw->primitive.shadow_thickness +
		       mlw->primitive.highlight_thickness);

  if ((arraysize = NumColumnWidths (mlw)) == 0) {
    *x_ptr = col * MultiListColWidth(mlw) + primitive_border;
    *w_ptr = MultiListColWidth(mlw);
  }
  else {
    ColumnGeometry (mlw, col, x_ptr, w_ptr);
    *x_ptr = *x_ptr + primitive_border;
  }

  *y_ptr = row * MultiListRowHeight(mlw) + primitive_border - mlw->multiList.pixel_offset;
  *h_ptr = MultiListRowHeight(mlw);
} /* End RowColumnToPixels */

/*---------------------------------------------------------------------------*

	Boolean RowColumnToItem(mlw,row,column,item_ptr)

	This routine takes a row number <row> and a column number <column>
	and tries to resolve this row and column into the index of the
	item in this position of the MultiList widget <mlw>.  The resulting
	item index is placed through <item_ptr>.  If there is no item at
	this location, False is returned, else True is returned.

 *---------------------------------------------------------------------------*/

#if defined (__STDC__) && !defined(_NO_PROTO)
static Boolean RowColumnToItem (MultiListWidget mlw,
				int row, int column, 
				int *item_ptr)
#else
static Boolean RowColumnToItem (mlw, row, column, item_ptr)
     MultiListWidget mlw;
     int row;
     int column;
     int *item_ptr;
#endif
{
  register int x_stride,y_stride;

  if (row < 0 || row >= MultiListNumRows(mlw) ||
      column < 0 || column >= MultiListNumCols(mlw))
    {
      *item_ptr = -1;
      return(False);
    }
  if (MultiListRowMajor(mlw))
    {
      x_stride = 1;
      y_stride = MultiListNumCols(mlw);
    }
  else
    {
      x_stride = MultiListNumRows(mlw);
      y_stride = 1;
    }
  *item_ptr = row * y_stride + column * x_stride;
  if (*item_ptr >= MultiListNumItems(mlw))
    return(False);
  else
    return(True);
} /* End RowColumnToItem */


/*---------------------------------------------------------------------------*

	Boolean ItemToRowColumn(mlw,item_index,row_ptr,column_ptr)

	This routine takes an item number <item_index> and attempts
	to convert the index into row and column numbers stored through
	<row_ptr> and <column_ptr>.  If the item number does not
	corespond to a valid item, False is returned, else True is
	returned.

 *---------------------------------------------------------------------------*/
#if defined (__STDC__) && !defined(_NO_PROTO)
static Boolean ItemToRowColumn (MultiListWidget mlw, int item_index, 
				int *row_ptr, int *column_ptr)
#else
static Boolean ItemToRowColumn (mlw, item_index, row_ptr, column_ptr)
     MultiListWidget mlw;
     int item_index;
     int *row_ptr;
     int *column_ptr;
#endif
{
  if (item_index < 0 || item_index >= MultiListNumItems(mlw))
    {
      return(False);
    }
  if (MultiListRowMajor(mlw))
    {
      *row_ptr = item_index / MultiListNumCols(mlw);
      *column_ptr = item_index % MultiListNumCols(mlw);
    }
  else
    {
      *row_ptr = item_index % MultiListNumRows(mlw);
      *column_ptr = item_index / MultiListNumRows(mlw);
    }
  return(True);
} /* End ItemToRowColumn */


/*===========================================================================*

                M U L T I - C L I C K   P R O C E D U R E S

 *===========================================================================*/

/*******************************************************************************
 *  Routines for handling Multi Clicks
 *  MultiListIsMultiClickClick() MultiListHandleMultiClick
 *
 ******************************************************************************/

#if defined (__STDC__) && !defined(_NO_PROTO)
static Boolean MultiListIsMultiClick (MultiListWidget mlw ,XEvent * event)
#else
static Boolean MultiListIsMultiClick (mlw, event)
     MultiListWidget mlw;
     XEvent * event;
#endif
{
  Boolean is_multiclick;

  /* we increment click number */
  MultiListClickNumber(mlw)++;

  if((event->xbutton.time - mlw->multiList.last_button_time) < mlw->multiList.click_delay)
    {
      mlw->multiList.last_button_time = event->xbutton.time;
      is_multiclick = TRUE;
    }
  else
    {
      is_multiclick = FALSE;
      MultiListClickNumber(mlw) = 1;
      mlw->multiList.last_button_time = event->xbutton.time;
    }


  return is_multiclick;
}

#if defined (__STDC__) && !defined(_NO_PROTO)
static void MultiListHandleMultiClick (MultiListWidget mlw, int item_index)
#else
static void MultiListHandleMultiClick (mlw, item_index)
     MultiListWidget mlw;
     int item_index;
#endif
{	
  int row,column;
  Boolean item_valid;

  item_valid = ItemToRowColumn(mlw,item_index,&row,&column);

  switch (MultiListClickNumber(mlw))
    {
    case 0:	
    case 1:	
      /* selection of a single item */
      MultiListUnhighlightAll( (Widget) mlw);
      MultiListMostRecentAct(mlw) = MULTILIST_ACTION_SET;
      MultiListHighlightItem( (Widget)mlw, item_index);
      break;
    case 2:
      /* selection of a the column under the mouse */
      if ( item_valid )
	{
	  MultiListHighlightColumn( (Widget)mlw, column);
	}
      break;
    case 3:
      /* selection of max selectable items */
      if ( item_valid )
	{
	  MultiListHighlightAll((Widget) mlw);
	}
      break;
    }
}

/*===========================================================================*

                E V E N T    A C T I O N    H A N D L E R S

 *===========================================================================*/

/*---------------------------------------------------------------------------*

	MultiListSet(mlw,event,params,num_params)

	This function sets a single text item in the MultiList.  Any previously
	selected items will be unselected, even if the user later aborts
	the click.  The item clicked on will be highlighted, and the
	MultiListMostRecentItem(mlw) variable will be set to the item clicked
	on if any, or -1 otherwise.

 *---------------------------------------------------------------------------*/

#if defined (__STDC__) && !defined(_NO_PROTO)
static void MultiListSet (MultiListWidget mlw, 
			  XEvent *event, 
			  String *params, 
			  Cardinal *num_params)
#else
static void MultiListSet (mlw, event, params, num_params)
     MultiListWidget mlw;
     XEvent *event;
     String *params;
     Cardinal *num_params;
#endif
{
  int item_index,row,column;
  Boolean item_valid;
  MultiListItem *item;
  int x, y1, y2, wd;
  int minx, inix, colnum;

  PixelToRowColumn(mlw,event->xbutton.x,event->xbutton.y,&row,&column);

  if (_PixelInsideSeparator (mlw, event->xbutton.x, event->xbutton.y)) {
    SeparatorGeometry (mlw, event->xbutton.x, event->xbutton.y,
		       &x, &y1, &y2, &wd);
    MultiListEraseSeparator (mlw, x, y1, y2, wd);

    /* get aux information for this separator */
    SeparatorAuxInfo (mlw,  event->xbutton.x, event->xbutton.y,
		      &colnum, &minx, &inix);

    MultiListSepX(mlw) = x;
    MultiListSepY1(mlw) = y1;
    MultiListSepY2(mlw) = y2;
    MultiListSepMinX(mlw) = minx;
    MultiListSepIniX(mlw) = inix;
    MultiListSepCol(mlw) = colnum;

    MultiListMoveSeparator (mlw, x, y1, y2, wd);

    /* turn flag of the moving separator */
    MultiListMoveSep(mlw) = 1;
    return;
  }

  item_valid = RowColumnToItem(mlw,row,column,&item_index);

  item = MultiListNthItem(mlw,item_index);

  if (! item_valid) item_index = -1;

  if (MultiListUseMultiClick(mlw))
    {

      /* what is the most recent action */
      MultiListMostRecentAct(mlw) = MULTILIST_ACTION_SET;

      if(MultiListIsMultiClick(mlw, event))
	{
	  /* this is a MultiClick action */
	  MultiListHandleMultiClick(mlw, item_index);
	}
      else
	{
	  /* this is a single click : Highlight the selected item */
	  MultiListUnhighlightAll( (Widget) mlw);
	  MultiListMostRecentAct(mlw) = MULTILIST_ACTION_SET;
	  MultiListHighlightItem( (Widget)mlw, item_index);
	}
    } 

  else if (!MultiListItemSensitive(item))
    return;
  else 	/* no multi-click */
    /* this is a simple set action */
    MultiListHandleMultiClick(mlw,item_index);

} /* End MultiListSet */


/*---------------------------------------------------------------------------*

	MultiListSetMany(mlw,event,params,num_params)

	This function sets many text item in the MultiList.

 *---------------------------------------------------------------------------*/

#if defined (__STDC__) && !defined(_NO_PROTO)
static void MultiListSetMany (MultiListWidget mlw, 
			      XEvent *event, 
			      String *params, 
			      Cardinal *num_params)
#else
static void MultiListSetMany (mlw, event, params, num_params)
     MultiListWidget mlw;
     XEvent *event;
     String *params;
     Cardinal *num_params;
#endif
{
  int click_x,click_y;
  int px;
  int status,item_index,row,column;

#ifdef DEBUG
  /* printf("action SetMany()\n"); */
#endif
  click_x = event->xbutton.x;
  click_y = event->xbutton.y;

  /* check whether we are in the mode of moving separator */
  if (MultiListMoveSep(mlw) == 1) {
    px = MultiListSepX(mlw);
    MultiListMoveSeparator(mlw, px,
			   MultiListSepY1(mlw), MultiListSepY2(mlw),
			   MultiListSepWidth(mlw));

    px = click_x;
    if (px <= MultiListSepMinX(mlw))
      px = MultiListSepMinX(mlw) + MultiListSepWidth(mlw);

    MultiListSepX(mlw) = px;
    MultiListMoveSeparator(mlw, MultiListSepX(mlw),
			   MultiListSepY1(mlw), MultiListSepY2(mlw),
			   MultiListSepWidth(mlw));
    return;
  }

  PixelToRowColumn(mlw,click_x,click_y,&row,&column);

  if (_PixelInsideSeparator (mlw, event->xbutton.x, event->xbutton.y))
    return;

  status = RowColumnToItem(mlw,row,column,&item_index);
  if (status == False) item_index = -1;
  MultiListHighlightItem( (Widget) mlw,item_index);
  MultiListMostRecentAct(mlw) = MULTILIST_ACTION_SET;
  MultiListMostRecentItem(mlw) = item_index;

} /* End MultiListSetMany */

/*---------------------------------------------------------------------------*

	SetColumn(mlw,event,params,num_params)

	This function sets all text items in the column from the MultiList.

 *---------------------------------------------------------------------------*/

#if defined (__STDC__) && !defined(_NO_PROTO)
static void MultiListSetColumn (MultiListWidget mlw, 
				XEvent *event, 
				String *params, 
				Cardinal *num_params)
#else
static void MultiListSetColumn (mlw, event, params, num_params)
     MultiListWidget mlw;
     XEvent *event;
     String *params;
     Cardinal *num_params;
#endif
{

  int click_x,click_y;
  int status,item_index,row,column;

  click_x = event->xbutton.x;
  click_y = event->xbutton.y;
  PixelToRowColumn(mlw,click_x,click_y,&row,&column);

  if (_PixelInsideSeparator (mlw, event->xbutton.x, event->xbutton.y))
    return;

  status = RowColumnToItem(mlw,row,column,&item_index);
  if (status == False) 
    return;

  /* do Highlight the Column */
  MultiListHighlightColumn( (Widget)mlw,column);

} /* End MultiListSetColumn */


/*---------------------------------------------------------------------------*

	MultiListSetAll(mlw,event,params,num_params)

	This function sets all text item in the MultiList.

 *---------------------------------------------------------------------------*/

#if defined (__STDC__) && !defined(_NO_PROTO)
static void MultiListSetAll (MultiListWidget mlw, 
			     XEvent *event, 
			     String *params, 
			     Cardinal *num_params)
#else
static void MultiListSetAll (mlw, event, params, num_params)
     MultiListWidget mlw;
     XEvent *event;
     String *params;
     Cardinal *num_params;
#endif
{

  MultiListHighlightAll( (Widget)mlw);

} /* End MultiListSetAll */


/*---------------------------------------------------------------------------*

	MultiListSelect(mlw,event,params,num_params)

	This function draw a rectangle on the selected item
        and call the selectCallback to notify application.

	The aim of this callback is to let the application to decide what to
	do with the selected item. Do we want to highlight item or to 
        blink it or ...
 *---------------------------------------------------------------------------*/

#if defined (__STDC__) && !defined(_NO_PROTO)
static void MultiListSelect (MultiListWidget mlw, 
			     XEvent *event, 
			     String *params, 
			     Cardinal *num_params)
#else
static void MultiListSelect (mlw, event, params, num_params)
     MultiListWidget mlw;
     XEvent *event;
     String *params;
     Cardinal *num_params;
#endif
{
  int click_x,click_y;
  int status,item_index,row,column;
  MultiListSelectStruct select_value;

  click_x = event->xbutton.x;
  click_y = event->xbutton.y;
  PixelToRowColumn(mlw,click_x,click_y,&row,&column);

  if (_PixelInsideSeparator (mlw, event->xbutton.x, event->xbutton.y))
    return;

  status = RowColumnToItem(mlw,row,column,&item_index);
  if (status == False) 
    return;

  mlw->multiList.selected_item = item_index;

  MultiListMostRecentAct(mlw) = MULTILIST_ACTION_SELECT;

  /* this is a (pre)-selection callback */
  select_value.action = MULTILIST_ACTION_SELECT;
  select_value.item = item_index;
  select_value.event= event;
  select_value.item_value = MultiListNthItem(mlw,item_index);
  XtCallCallbacks((Widget)mlw,XmNselectCallback,(caddr_t)&select_value);

} /* End MultiListSelect */


/*---------------------------------------------------------------------------*

	MultiListUnselect(mlw,event,params,num_params)

	This function un-draw the rectangle to show item selected
        and call the selectCallback to notify application.


 *---------------------------------------------------------------------------*/

#if defined (__STDC__) && !defined(_NO_PROTO)
static void MultiListUnselect (MultiListWidget mlw, 
			       XEvent *event, 
			       String *params, 
			       Cardinal *num_params)
#else
static void MultiListUnselect (mlw, event, params, num_params)
     MultiListWidget mlw;
     XEvent *event;
     String *params;
     Cardinal *num_params;
#endif
{
  int click_x,click_y;
  int status,item_index,row,column;
  MultiListSelectStruct select_value;

  click_x = event->xbutton.x;
  click_y = event->xbutton.y;

  PixelToRowColumn(mlw,click_x,click_y,&row,&column);

  if (_PixelInsideSeparator (mlw, event->xbutton.x, event->xbutton.y))
    return;

  status = RowColumnToItem(mlw,row,column,&item_index);
  if (status == False) 
    return;

  mlw->multiList.selected_item = item_index;
  MultiListMostRecentAct(mlw) = MULTILIST_ACTION_UNSELECT;

  /* this is a (pre)-selection callback */
  select_value.action = MULTILIST_ACTION_UNSELECT;
  select_value.item = item_index;
  select_value.event= event;
  select_value.item_value = MultiListNthItem(mlw,item_index);
  XtCallCallbacks((Widget)mlw,XmNselectCallback,(caddr_t)&select_value);

} /* End MultiListUnselect */


/*---------------------------------------------------------------------------*

	MultiListUnset(mlw,event,params,num_params)

	This function sets a single text item in the MultiList.  If the item
	is already unset, then nothing happens.  Otherwise, the item is
	unset and the selection array and selection count are updated.

 *---------------------------------------------------------------------------*/

#if defined (__STDC__) && !defined(_NO_PROTO)
static void MultiListUnset (MultiListWidget mlw, 
			    XEvent *event, 
			    String *params, 
			    Cardinal *num_params)
#else
static void MultiListUnset (mlw, event, params, num_params)
     MultiListWidget mlw;
     XEvent *event;
     String *params;
     Cardinal *num_params;
#endif
{
  int click_x,click_y;
  int status,item_index,row,column;

  click_x = event->xbutton.x;
  click_y = event->xbutton.y;

  PixelToRowColumn(mlw,click_x,click_y,&row,&column);

  if (_PixelInsideSeparator (mlw, event->xbutton.x, event->xbutton.y))
    return;

  status = RowColumnToItem(mlw,row,column,&item_index);
  if (status == False) item_index = -1;
  MultiListUnhighlightItem( (Widget) mlw,item_index);
  MultiListMostRecentAct(mlw) = MULTILIST_ACTION_UNSET;
  MultiListMostRecentItem(mlw) = item_index;
} /* End MultiListUnset */


/*---------------------------------------------------------------------------*

	UnsetAll(mlw,event,params,num_params)

	This function unsets all text item in the MultiList.

 *---------------------------------------------------------------------------*/

#if defined (__STDC__) && !defined(_NO_PROTO)
static void MultiListUnsetAll (MultiListWidget mlw, 
			       XEvent *event, 
			       String *params, 
			       Cardinal *num_params)
#else
static void MultiListUnsetAll (mlw, event, params, num_params)
     MultiListWidget mlw;
     XEvent *event;
     String *params;
     Cardinal *num_params;
#endif
{

  MultiListUnhighlightAll( (Widget) mlw);
  MultiListMostRecentAct(mlw) = MULTILIST_ACTION_UNSET_ALL;
  MultiListMostRecentItem(mlw) = -1;
} /* End MultiListUnsetAll */


/*---------------------------------------------------------------------------*

	UnsetMany(mlw,event,params,num_params)

	This function unsets many text item in the MultiList.

 *---------------------------------------------------------------------------*/

#if defined (__STDC__) && !defined(_NO_PROTO)
static void MultiListUnsetMany (MultiListWidget mlw, 
				XEvent *event, 
				String *params, 
				Cardinal *num_params)
#else
static void MultiListUnsetMany (mlw, event, params, num_params)
     MultiListWidget mlw;
     XEvent *event;
     String *params; 
     Cardinal *num_params;
#endif
{
  int click_x,click_y;
  int status,item_index,row,column;

  click_x = event->xbutton.x;
  click_y = event->xbutton.y;
  PixelToRowColumn(mlw,click_x,click_y,&row,&column);

  if (_PixelInsideSeparator (mlw, event->xbutton.x, event->xbutton.y))
    return;

  status = RowColumnToItem(mlw,row,column,&item_index);
  if(item_index == MultiListMostRecentItem(mlw)) return;
  if (status == False) item_index = -1;
  MultiListUnhighlightItem( (Widget) mlw,item_index);
  MultiListMostRecentAct(mlw) = MULTILIST_ACTION_UNSET;
  MultiListMostRecentItem(mlw) = item_index;

} /* End MultiListUnsetMany */


/*---------------------------------------------------------------------------*

	Toggle(mlw,event,params,num_params)

	This function toggles a text item in the MultiList, while leaving the
	other selections intact (up to the allowed number of selections).
	The item clicked on will be highlighted, and MultiListMostRecentItem(mlw)
	will be set to the item number clicked on, or -1 if no item was clicked
	on.

 *---------------------------------------------------------------------------*/

#if defined (__STDC__) && !defined(_NO_PROTO)
static void MultiListToggle (MultiListWidget mlw, 
			     XEvent *event, 
			     String *params,
			     Cardinal *num_params)
#else
static void MultiListToggle (mlw, event, params, num_params)
     MultiListWidget mlw;
     XEvent *event;
     String *params;
     Cardinal *num_params;
#endif
{
  int click_x,click_y;
  int status,item_index,row,column;

  click_x = event->xbutton.x;
  click_y = event->xbutton.y;
  PixelToRowColumn(mlw,click_x,click_y,&row,&column);

  if (_PixelInsideSeparator (mlw, event->xbutton.x, event->xbutton.y))
    return;

  status = RowColumnToItem(mlw,row,column,&item_index);
  if (status == False) item_index = -1;
  MultiListMostRecentAct(mlw) = MultiListToggleItem((Widget)mlw,item_index);
  MultiListMostRecentItem(mlw) = item_index;
} /* End MultiListToggle */


/*---------------------------------------------------------------------------*

	ToggleMany(mlw,event,params,num_params)

	This function toggles many text item in the MultiList, while leaving the
	other selections intact (up to the allowed number of selections).
	The item dragged on will be highlighted, and MultiListMostRecentItem(mlw)
	will be set to the item number clicked on, or -1 if no item was clicked
	on.

 *---------------------------------------------------------------------------*/

#if defined (__STDC__) && !defined(_NO_PROTO)
static void MultiListToggleMany (MultiListWidget mlw,
				 XEvent *event, 
				 String *params, 
				 Cardinal *num_params)
#else
static void MultiListToggleMany (mlw, event, params, num_params)
     MultiListWidget mlw;
     XEvent *event;
     String *params;
     Cardinal *num_params;
#endif
{
  int click_x,click_y;
  int status,item_index,row,column;

  click_x = event->xbutton.x;
  click_y = event->xbutton.y;
  PixelToRowColumn(mlw,click_x,click_y,&row,&column);

  if (_PixelInsideSeparator (mlw, event->xbutton.x, event->xbutton.y))
    return;

  status = RowColumnToItem(mlw,row,column,&item_index);

  if (status == False) item_index = -1;

  if(MultiListMostRecentItem(mlw) != item_index)
    MultiListMostRecentAct(mlw) = MultiListToggleItem((Widget)mlw,item_index);
  MultiListMostRecentItem(mlw) = item_index;
} /* End MultiListToggleMany */

/*---------------------------------------------------------------------------*

	ToggleAll(mlw,event,params,num_params)

	This function toggles all text items in the MultiList.

 *---------------------------------------------------------------------------*/

#if defined (__STDC__) && !defined(_NO_PROTO)
static void MultiListToggleAll (MultiListWidget mlw, 
				XEvent *event, 
				String *params, 
				Cardinal *num_params)
#else
static void MultiListToggleAll (mlw, event, params, num_params)
MultiListWidget mlw;
XEvent *event;
String *params;
Cardinal *num_params;
#endif
{
  int i;

  MultiListSetClipRect(mlw);
  for (i = 0; i < MultiListNumItems(mlw); i++)
    {
      _MultiListToggleItem(mlw,i);
      if (MultiListNumSelected(mlw) == MultiListMaxSelectable(mlw)) 
	break;
    }
  MultiListUnsetClipRect(mlw);
} /* End MultiListToggleAll */


/*---------------------------------------------------------------------------*

	MultiListOpen(mlw,event,params,num_params)

	This routine handles the opening (normally double clicking)
	of an item.  All previous selections will be unselected, the
	clicked item will be selected, and the OPEN action will be recorded.

 *---------------------------------------------------------------------------*/

#if defined (__STDC__) && !defined(_NO_PROTO)
static void MultiListOpen (MultiListWidget mlw, 
			   XEvent *event, 
			   String *params, 
			   Cardinal *num_params)
#else
static void MultiListOpen (mlw, event, params, num_params)
     MultiListWidget mlw;
     XEvent *event;
     String *params;
     Cardinal *num_params;
#endif
{
  int click_x,click_y;
  int status,item_index,row,column;

  click_x = event->xbutton.x;
  click_y = event->xbutton.y;
  PixelToRowColumn(mlw,click_x,click_y,&row,&column);

  if (_PixelInsideSeparator (mlw, event->xbutton.x, event->xbutton.y))
    return;

  MultiListUnhighlightAll((Widget)mlw);
  status = RowColumnToItem(mlw,row,column,&item_index);
  if (status == False) item_index = -1;
  MultiListHighlightItem((Widget) mlw,item_index);
  MultiListMostRecentAct(mlw) = MULTILIST_ACTION_OPEN;
  MultiListMostRecentItem(mlw) = item_index;
} /* End MultiListOpen */


/*---------------------------------------------------------------------------*

	OpenMany(mlw,event,params,num_params)

	This routine handles the opening (normally double clicking)
	of an item.  All previous selections will remain selected,
	which is why this routine is called OpenMany.  Many selections
	can be returned on an Open.  The clicked item will be selected,
	and the OPEN action will be recorded.

 *---------------------------------------------------------------------------*/

#if defined (__STDC__) && !defined(_NO_PROTO)
static void MultiListOpenMany (MultiListWidget mlw, 
			       XEvent *event, 
			       String *params, 
			       Cardinal *num_params)
#else
static void MultiListOpenMany (mlw, event, params, num_params)
     MultiListWidget mlw;
     XEvent *event;
     String *params;
     Cardinal *num_params;
#endif
{
  int click_x,click_y;
  int status,item_index,row,column;

  click_x = event->xbutton.x;
  click_y = event->xbutton.y;
  PixelToRowColumn(mlw,click_x,click_y,&row,&column);

  if (_PixelInsideSeparator (mlw, event->xbutton.x, event->xbutton.y))
    return;

  status = RowColumnToItem(mlw,row,column,&item_index);
  if (status == False) item_index = -1;
  MultiListHighlightItem( (Widget) mlw,item_index);
  MultiListMostRecentAct(mlw) = MULTILIST_ACTION_OPEN;
  MultiListMostRecentItem(mlw) = item_index;
} /* End MultiListOpenMany */


/*---------------------------------------------------------------------------*

	MultiListNotify(mlw,event,params,num_params)

	This function performs the Notify action, which is what happens
	when a user releases a button after clicking on an item.  If there
	was no item under the click, or if the item was insensitive, then
	Notify simply returns.  Otherwise, notify notifies the user via
	a callback of the current list of selected items.

	In addition, if the XtNpasteBuffer resource is true and a valid
	sensitive item was clicked on, the name of the last clicked on item
	will be placed in the X cut buffer (buf(0)).

 *---------------------------------------------------------------------------*/

#if defined (__STDC__) && !defined(_NO_PROTO)
static void MultiListNotify (MultiListWidget mlw, 
			     XEvent *event, 
			     String *params, 
			     Cardinal *num_params)
#else
static void MultiListNotify (mlw, event, params, num_params)
     MultiListWidget mlw;
     XEvent *event;
     String *params;
     Cardinal *num_params;
#endif
{
  String string;
  int final_x,final_y;
  int px;
  Boolean item_valid;
  int item_index,string_length,row,column;
  MultiListReturnStruct ret_value;
  MultiListSepMoveStruct movesep_value;
  MultiListItem *item;
  Dimension* array;
  int  arraysize, col;
  int i;

  final_x = event->xbutton.x;
  final_y = event->xbutton.y;

  /* finish dragging the separator */
  if (MultiListMoveSep(mlw) == 1) {
    MultiListMoveSeparator (mlw, MultiListSepX(mlw),
			    MultiListSepY1(mlw), MultiListSepY2(mlw),
			    MultiListSepWidth(mlw));

    px = final_x;
    if (px <= MultiListSepMinX(mlw))
      px = MultiListSepMinX(mlw) + MultiListSepWidth(mlw);

    MultiListSepX(mlw) = px;    
    
    MultiListDrawSeparatorT (mlw, MultiListSepX(mlw),
			     MultiListSepY1(mlw), MultiListSepY2(mlw),
			     MultiListSepWidth(mlw));
    MultiListMoveSep(mlw) = 0;

    /* reset array widths */
    arraysize = NumColumnWidths (mlw);
    array = MultiListPrefWidths (mlw);

    col = MultiListSepCol(mlw);

    /* calculate new column width */
    array[col - 1] = array[col - 1] + px - MultiListSepIniX(mlw);

    /* redraw all */
    RedrawAll (mlw);

    /* call move separator callbacks */
    movesep_value.action = MULTILIST_END_MOVING_SEP;
    movesep_value.event = event;
    movesep_value.num = MultiListDefaultCols (mlw);
    movesep_value.widths = array;
    XtCallCallbacks((Widget)mlw,XmNsepMoveCallback,&movesep_value);
    return;
  }
    

  PixelToRowColumn(mlw,final_x,final_y,&row,&column);

  if (_PixelInsideSeparator (mlw, event->xbutton.x, event->xbutton.y))
    return;

  item_valid = RowColumnToItem(mlw,row,column,&item_index);

  if ( event->xbutton.window != XtWindow(mlw))
    {
      /* On vient d'effectuer un relachement du boutton
	 de la souris en dehors du widget MultiList	
	 */

      MultiListUnhighlightAll( (Widget)mlw);
      if (MultiListNotifyUnsets(mlw))
	{
	  ret_value.action = MULTILIST_ACTION_UNSET;
	  ret_value.item = -1;
	  ret_value.string = NULL;
	  ret_value.num_selected = MultiListNumSelected(mlw);
	  ret_value.selected_items = MultiListSelArray(mlw);
	  ret_value.item_value = NULL;
	  XtCallCallbacks((Widget)mlw,XmNcallback,&ret_value);
	}
      return;
    }

  if (!item_valid ) return;

  item = MultiListNthItem(mlw,item_index);
  if (!MultiListItemSensitive(item))
    return;

  string = MultiListItemString(MultiListNthItem(mlw,item_index));
  string_length = strlen(string);
  if (MultiListMostRecentAct(mlw) == MULTILIST_ACTION_SET)
    {
      if (MultiListPaste(mlw))
	XStoreBytes(XtDisplay(mlw),string,string_length);
    }

  if (((MultiListMostRecentAct(mlw) == MULTILIST_ACTION_SET) &&
       (MultiListNotifySets(mlw) == True)) ||
      ((MultiListMostRecentAct(mlw) == MULTILIST_ACTION_UNSET) &&
       (MultiListNotifyUnsets(mlw) == True)) ||
      ((MultiListMostRecentAct(mlw) == MULTILIST_ACTION_OPEN) &&
       (MultiListNotifyOpens(mlw) == True)))
    {
      ret_value.action = MultiListMostRecentAct(mlw);
      ret_value.item = MultiListMostRecentItem(mlw); 	/* which number */
      ret_value.string = string;
      ret_value.num_selected = MultiListNumSelected(mlw);
      ret_value.selected_items = MultiListSelArray(mlw);
      ret_value.item_value = item;			/* the real value of item */
      XtCallCallbacks((Widget)mlw,XmNcallback,(caddr_t)&ret_value);
    }
} /* End MultiListNotify */

/*===========================================================================*

        U S E R    C A L L A B L E    U T I L I T Y    R O U T I N E S

 *===========================================================================*/

/*---------------------------------------------------------------------------*

	_MultiListHighlightItem(mlw,item_index)

	This is the internal version, with cache to the clip rectangle feature.

 *---------------------------------------------------------------------------*/

#if defined (__STDC__) && !defined(_NO_PROTO)
static void _MultiListHighlightItem (MultiListWidget mlw, int item_index)
#else
static void _MultiListHighlightItem (mlw, item_index)
     MultiListWidget mlw;
     int item_index;
#endif
{
  MultiListItem *item;

  if (MultiListMaxSelectable(mlw) == 0) return;
  if (item_index < 0 || item_index >= MultiListNumItems(mlw))
    {
      MultiListMostRecentItem(mlw) = -1;
      return;
    }
  item = MultiListNthItem(mlw,item_index);
  if (MultiListItemSensitive(item) == False)
    {
      /* Marc QUINTON this should be a ressource ! */
      /* MultiListUnhighlightAll(mlw); */
      return;
    }
  MultiListMostRecentItem(mlw) = item_index;
  if (MultiListItemHighlighted(item) == True) return;
  if (MultiListNumSelected(mlw) == MultiListMaxSelectable(mlw))
    {
      _MultiListUnhighlightItem(mlw,MultiListSelArray(mlw)[0]);
    }
  MultiListItemHighlighted(item) = True;
  MultiListSelArray(mlw)[MultiListNumSelected(mlw)] = item_index;
  ++ MultiListNumSelected(mlw);
  RedrawItem(mlw,item_index);
} /* End _MultiListHighlightItem */

/*---------------------------------------------------------------------------*

	MultiListHighlightItem(w,item_index)

	This routine selects an item with index <item_index> in the
	MultiList widget <mlw>.  If a maximum number of selections is specified
	and exceeded, the earliest selection will be unselected.  If
	<item_index> doesn't correspond to an item the most recently
	clicked item will be set to -1 and this routine will immediately
	return, otherwise the most recently clicked item will be set to the
	current item,

 *---------------------------------------------------------------------------*/

#if defined (__STDC__) && !defined(_NO_PROTO)
void MultiListHighlightItem (Widget w, int item_index)
#else
void MultiListHighlightItem (w, item_index)
     Widget w;
     int item_index;
#endif
{
  MultiListWidget mlw = (MultiListWidget) w;

  MultiListSetClipRect(mlw);
  _MultiListHighlightItem(mlw,item_index);
  MultiListUnsetClipRect(mlw);
	
} /* End MultiListHighlightItem */


/*---------------------------------------------------------------------------*

	MultiListHighlightAll(w)

	This routine highlights all highlightable items in the MultiList
	widget <mlw>, up to the maximum number of allowed highlightable
	items;

 *---------------------------------------------------------------------------*/

#if defined (__STDC__) && !defined(_NO_PROTO)
void MultiListHighlightAll (Widget w)
#else
void MultiListHighlightAll (w)
     Widget w;
#endif
{
  MultiListWidget mlw = (MultiListWidget) w;
  int i;

  MultiListSetClipRect(mlw);
  for (i = 0; i < MultiListNumItems(mlw); i++)
    {
      if (MultiListNumSelected(mlw) == MultiListMaxSelectable(mlw)) 
	break;
      _MultiListHighlightItem(mlw,i);
    }
  MultiListUnsetClipRect(mlw);
} /* End MultiListHighlightAll */


/*---------------------------------------------------------------------------*

	MultiListHighlightColumn(w,column)

	This routine highlights all highlightable items in the MultiList
	widget <mlw>, up to the maximum number of allowed highlightable
	items, in the specified column.

 *---------------------------------------------------------------------------*/

#if defined (__STDC__) && !defined(_NO_PROT)
void MultiListHighlightColumn (Widget w, int column)
#else
void MultiListHighlightColumn (w, column)
     Widget w;
     int column;
#endif
{
  MultiListWidget mlw = (MultiListWidget) w;
  int row, item_number;
  Boolean item_valid;

  MultiListSetClipRect(mlw);
  for (row = 0; row < MultiListNumRows(mlw); row++)
    {
      if (MultiListNumSelected(mlw) == MultiListMaxSelectable(mlw)) break;
      item_valid = RowColumnToItem(mlw,row,column,&item_number);
      if ( item_valid )
	{
	  _MultiListHighlightItem(mlw,item_number);
	}
    }
  MultiListUnsetClipRect(mlw);
} /* End MultiListHighlightColumn */


/*---------------------------------------------------------------------------*

	_MultiListUnhighlightItem(mlw,item_index)

	This is the internal version, with cache to the clip rectangle feature.

 *---------------------------------------------------------------------------*/

#if defined (__STDC__) && !defined(_NO_PROTO)
static void _MultiListUnhighlightItem( MultiListWidget mlw, int item_index)
#else
static void _MultiListUnhighlightItem(mlw,item_index)
     MultiListWidget mlw;
     int item_index;
#endif
{
  int i;
  MultiListItem *item;

  if (MultiListMaxSelectable(mlw) == 0) return;
  if (item_index < 0 || item_index >= MultiListNumItems(mlw)) return;
  item = MultiListNthItem(mlw,item_index);
  if (MultiListItemHighlighted(item) == False) return;
  MultiListItemHighlighted(item) = False;

  for (i = 0; i < MultiListNumSelected(mlw); i++)
    if (MultiListSelArray(mlw)[i] == item_index) break;
  for (i = i + 1; i < MultiListNumSelected(mlw); i++)
    MultiListSelArray(mlw)[i - 1] = MultiListSelArray(mlw)[i];
  -- MultiListNumSelected(mlw);

  RedrawItem(mlw,item_index);
} /* End _MultiListUnhighlightItem */

/*---------------------------------------------------------------------------*

	MultiListUnhighlightItem(mlw,item_index)

	This routine unselects the item with index <item_index> in the
	MultiList widget <mlw>.  If <item_index> doesn't correspond to a
	selected item, then nothing will happen.  Otherwise, the item
	is unselected and the selection array and count are updated.

 *---------------------------------------------------------------------------*/

#if defined (__STDC__) && !defined(_NO_PROTO)
void MultiListUnhighlightItem(Widget w, int item_index)
#else
void MultiListUnhighlightItem(w,item_index)
     Widget w;
     int item_index;
#endif
{
  MultiListWidget mlw = (MultiListWidget) w;

  MultiListSetClipRect(mlw);
  _MultiListUnhighlightItem(mlw,item_index);
  MultiListUnsetClipRect(mlw);
} /* End MultiListUnhighlightItem */


/*---------------------------------------------------------------------------*

	MultiListUnhighlightAll(w)

	This routine unhighlights all items in the MultiList widget <mlw>.

 *---------------------------------------------------------------------------*/

#if defined (__STDC__) && !defined(_NO_PROTO)
void MultiListUnhighlightAll(Widget w)
#else
void MultiListUnhighlightAll(w)
     Widget w;
#endif
{
  MultiListWidget mlw = (MultiListWidget) w;
  int i;
  MultiListItem *item;

  MultiListSetClipRect(mlw);
  for (i = 0; i < MultiListNumItems(mlw); i++)
    {
      item = MultiListNthItem(mlw,i);
      if (MultiListItemHighlighted(item))
	_MultiListUnhighlightItem(mlw,i);
    }
  MultiListNumSelected(mlw) = 0;
  MultiListUnsetClipRect(mlw);
} /* End MultiListUnhighlightAll */


/*---------------------------------------------------------------------------*

	int _MultiListToggleItem(mlw,item_index)

	This routine highlights the item with index <item_index>
	This is the internal version, with cache to the clip rectangle feature.

 *---------------------------------------------------------------------------*/

#if defined (__STDC__) && !defined(_NO_PROTO)
static int _MultiListToggleItem(MultiListWidget mlw,int item_index)
#else
static int _MultiListToggleItem(mlw,item_index)
     MultiListWidget mlw;
     int item_index;
#endif
{
  MultiListItem *item;

  if (MultiListMaxSelectable(mlw) == 0) return -1;
  if (item_index < 0 || item_index >= MultiListNumItems(mlw)) return -1;
  item = MultiListNthItem(mlw,item_index);
  if (MultiListItemHighlighted(item))
    {
      _MultiListUnhighlightItem(mlw,item_index);
      return(MULTILIST_ACTION_UNSET);
    }
  else
    {
      _MultiListHighlightItem(mlw,item_index);
      return(MULTILIST_ACTION_SET);
    }

} /* End _MultiListToggleItem */

/*---------------------------------------------------------------------------*

	int MultiListToggleItem(w,item_index)

	This routine highlights the item with index <item_index>
	if it is unhighlighted and unhighlights it if it is already
	highlighted.  The action performed by the toggle is returned
	(MULTILIST_ACTION_SET or MULTILIST_ACTION_UNSET).

 *---------------------------------------------------------------------------*/

#if defined (__STDC__) && !defined(_NO_PROTO)
int MultiListToggleItem(Widget w, int item_index)
#else
int MultiListToggleItem(w,item_index)     
     Widget w;
     int item_index;
#endif
{
  MultiListWidget mlw = (MultiListWidget) w;
  int status;

  MultiListSetClipRect(mlw);
  status = _MultiListToggleItem(mlw,item_index);
  MultiListUnsetClipRect(mlw);

  return status;

} /* End MultiListToggleItem */


/*---------------------------------------------------------------------------*

	MultiListReturnStruct *MultiListGetHighlighted(w)

	This routine takes a MultiList widget <mlw> and returns a
	MultiListReturnStruct whose num_selected and selected_items
	fields contain the highlight information.  The action field
	is set to MULTILIST_ACTION_STATUS, and the item_index and string
	fields are invalid.

 *---------------------------------------------------------------------------*/

#if defined (__STDC__) && !defined(_NO_PROTO)
MultiListReturnStruct *MultiListGetHighlighted(Widget w)
#else
MultiListReturnStruct *MultiListGetHighlighted(w)
     Widget w;
#endif
{
  MultiListWidget mlw = (MultiListWidget) w;
  MultiListItem *item;
  static MultiListReturnStruct ret_value;

  ret_value.action = MULTILIST_ACTION_STATUS;
  if (MultiListNumSelected(mlw) == 0)
    {
      ret_value.item = -1;
      ret_value.string = NULL;
    }
  else
    {
      ret_value.item = MultiListSelArray(mlw)[MultiListNumSelected(mlw) - 1];
      item = MultiListNthItem(mlw,ret_value.item);
      ret_value.string = MultiListItemString(item);
    }
  ret_value.num_selected = MultiListNumSelected(mlw);
  ret_value.selected_items = MultiListSelArray(mlw);
  ret_value.item_value = item;

  return(&ret_value);
} /* End MultiListGetHighlighted */


/*---------------------------------------------------------------------------*

	Boolean MultiListIsHighlighted(w,item_index)

	This routine checks if the item with index <item_index>
	is highlighted and returns True or False depending.  If
	<item_index> is invalid, False is returned.

 *---------------------------------------------------------------------------*/

#if defined (__STDC__) && !defined(_NO_PROTO)
Boolean MultiListIsHighlighted(Widget w, int item_index)
#else
Boolean MultiListIsHighlighted(w,item_index)
     Widget w;
     int item_index;
#endif
{
  MultiListWidget mlw = (MultiListWidget) w;
  MultiListItem *item;

  if (item_index < 0 || item_index >= MultiListNumItems(mlw)) return(False);
  item = MultiListNthItem(mlw,item_index);
  return(MultiListItemHighlighted(item));
} /* End MultiListIsHighlighted */


/*---------------------------------------------------------------------------*

	Boolean MultiListGetItemInfo(w,item_index,str_ptr,h_ptr,s_ptr, b_ptr)

	This routine returns the string, highlight status and
	sensitivity information for the item with index <item_index>
	via the pointers <str_ptr>, <h_ptr> and <s_ptr>.  If the item
	index is invalid, False is returned, else True is returned.

 *---------------------------------------------------------------------------*/

#if defined (__STDC__) && !defined(_NO_PROTO)
Boolean MultiListGetItemInfo(Widget w,
			     int    item_index,
			     String *str_ptr,
			     Boolean *h_ptr,
			     Boolean *s_ptr, 
			     Boolean *b_ptr,
			     int *fgindex)
#else
Boolean MultiListGetItemInfo(w,item_index,str_ptr,h_ptr,s_ptr, b_ptr, fgindex)
     Widget w;
     int item_index;
     String *str_ptr;
     Boolean *h_ptr,*s_ptr, *b_ptr;
     int *fgindex;
#endif
{
  MultiListWidget mlw = (MultiListWidget) w;
  MultiListItem *item;

  if (item_index < 0 || item_index >= MultiListNumItems(mlw)) return(False);
  item = MultiListNthItem(mlw,item_index);
  *str_ptr = MultiListItemString(item);
  *h_ptr = MultiListItemHighlighted(item);
  *s_ptr = MultiListItemSensitive(item);
  *b_ptr = MultiListItemBlinking(item);
  *fgindex = MultiListItemFgIndex(item);
  return(True);
} /* End MultiListGetItemInfo */


/*---------------------------------------------------------------------------*

	MultiListSetNewData(w,list,nitems,longest,resize,sensitivity_array)

	This routine will set a new set of strings <list> into the
	MultiList widget <mlw>.  If <resize> is True, the MultiList widget will
	try to resize itself.

 *---------------------------------------------------------------------------*/

#if defined (__STDC__) && !defined(_NO_PROTO)
void MultiListSetNewData (Widget w, 
			  String *list, int nitems, 
			  int longest, 
			  Boolean resize, Boolean *sensitivity_array)
#else
void MultiListSetNewData (w, list, nitems, longest, resize, sensitivity_array)
     Widget w;
     String *list;
     int nitems;
     int longest;
     Boolean resize;
     Boolean *sensitivity_array;
#endif
{
  MultiListWidget mlw = (MultiListWidget) w;

  DestroyOldData(mlw);
  MultiListList(mlw) = list;
  MultiListNumItems(mlw) = max(nitems,0);
  MultiListLongest(mlw) = max(longest,0);
  MultiListSensitiveArray(mlw) = sensitivity_array;
  InitializeNewData(mlw);
  RecalcCoords(mlw,resize,resize);
  MultiListSetVerticalScrollbar(mlw);
  if (XtIsRealized((Widget)mlw)) Redisplay(mlw,NULL,NULL);
} /* End MultiListSetNewData */

/*---------------------------------------------------------------------------*

	MultiListCreateScrolledList(parent, name, args, argCount)

	This routine will create a list inside of a scrolled window.

 *---------------------------------------------------------------------------*/

#if defined (__STDC__) && !defined(_NO_PROTO)
Widget MultiListCreateScrolledList (Widget parent, 
				    char *name, 
				    ArgList args, 
				    int argCount)
#else
Widget MultiListCreateScrolledList (parent, name, args, argCount)
     Widget parent;
     char *name;
     ArgList args;
     int argCount;
#endif
{
  Widget sw, mlw;
  char *s;

  s = XtMalloc(strlen(name) + 3);     /* Name + NULL + "SW" */
  strcpy(s, name);
  strcat(s, "SW");
   
  sw = XtVaCreateManagedWidget(s , 
			       xmScrolledWindowWidgetClass, 	parent,
			       XmNscrollingPolicy,		XmAPPLICATION_DEFINED,
			       XmNvisualPolicy,		XmVARIABLE,
			       XmNscrollBarDisplayPolicy,	XmSTATIC,
			       XmNshadowThickness,		0,
			       NULL);
  XtFree(s);
							       
  mlw = XtCreateWidget( name, multiListWidgetClass, sw, args, argCount);
  return (mlw);                           

} /* End MultiListCreateScrolledList */


/*---------------------------------------------------------------------------*

	MultiListAddItems(w,list,nitems,longest,position,resize,
			sensitivity_array)

	This routine takes a MultiList widget <mlw> and add new data items.  
	If the number of items is 0, they will be counted, so the array must
	be NULL terminated.  If the list of strings is NULL, this is treated
	as a list of 0 elements.

 *---------------------------------------------------------------------------*/

#if defined (__STDC__) && !defined(_NO_PROTO)
void MultiListAddItems (Widget w, 
			String *list, 
			int nitems, 
			int longest, 
			int position, 
			Boolean resize, 
			Boolean *sensitivity_array)
#else
void MultiListAddItems (w, list, nitems, longest, position, resize, sensitivity_array)
     Widget w;
     String *list;
     int nitems;
     int longest;
     int position;
     Boolean resize;
     Boolean *sensitivity_array;
#endif
{
  MultiListWidget mlw = (MultiListWidget) w;
  register int i;
  register MultiListItem *item;
  MultiListItem *items;
  MultiListItem *from, *to;
  int len;
  int max_text_width = 0;
  int text_width;
  String str;

  if (position < 0 || position > MultiListNumItems(mlw))
    return;

  if (nitems == 0)		/* Count The Elements */
    {
      if (list == NULL)	/* No elements */
	{
	  /* just return ! */
	  return;
	}
      else
	{
	  for (i = 0; list[i] != NULL; i++);
	  nitems = i;
	}
    }
  if (nitems == 0)		/* No Items */
    {
      /* one more time just return */
      return;
    }
  else
    {   /* now we have a non null list of nitems */

      /* realloc an new array of items MultiListItem */
      items = TypeRealloc(
			  MultiListItemArray(mlw),
			  MultiListItem,(nitems+MultiListNumItems(mlw)));

      MultiListItemArray(mlw) = items;

      /* we move item ptr from position to (position + nitems) */
      if (position == MultiListNumItems (mlw)) {
	from = MultiListNthItem(mlw,(position));
	to   = MultiListNthItem(mlw,(position + nitems));
	len  = (MultiListNumItems(mlw) - position) * sizeof (MultiListItem);
	bcopy(from, to, len);
      }

      /* fill the new items at position */
      for (i = 0; i < nitems; i++)
	{
	  item = MultiListNthItem(mlw,(i + position));
	  if (sensitivity_array == NULL ||
	      (sensitivity_array[i] == True))
	    {
	      MultiListItemSensitive(item) = True;
	    }
	  else
	    {
	      MultiListItemSensitive(item) = False;
	    }
	  MultiListItemString(item) = StrCopy(list[i]);
	  MultiListItemHighlighted(item) = False;
	  MultiListItemBlinking(item) = False;
	  MultiListItemFgIndex(item) = -1;
			

	  /* look at the longest item */
	  if(longest == 0);
	  {
	    str = MultiListItemString(item);
	    text_width = FontW(MultiListFont(mlw),str);
	    max_text_width = max(max_text_width,text_width);
	  }
	}
    } /* else nitems == 0 */

  /* set up the number of items in the MultiList */
  MultiListNumItems(mlw) += nitems;

  /* if longest is 0, we have calculated the new longest item,
     so update the value to the widget
     else impose the new size of a column in pixels.
     */
  if(longest == 0)
    MultiListLongest(mlw) = max(max_text_width,MultiListLongest(mlw));		
  else
    MultiListLongest(mlw) = longest;

  RecalcCoords(mlw,resize,resize);
  MultiListSetVerticalScrollbar(mlw);
  if (XtIsRealized((Widget)mlw)) Redisplay(mlw,NULL,NULL);
	

} /* End MultiListAddItems */

/*---------------------------------------------------------------------------*

	MultiListAddItems(w,item, position )

	This routine takes a MultiList widget <mlw> and add a single new 
        data item to the list.

 *---------------------------------------------------------------------------*/

#if defined (__STDC__) && !defined(_NO_PROTO)
void MultiListAddItem (Widget w, String item, int position)
#else
void MultiListAddItem (w, item, position)
     Widget w;
     String item;
     int position;
#endif
{
  MultiListWidget mlw = (MultiListWidget) w;

  if (position < 0 || position > MultiListNumItems(mlw))
    return;

  /* (mlw,list,nitems,longest,position,resize,sensitivity_array) */
  MultiListAddItems( (Widget)mlw, &item, 1, 0, position, False, NULL);

} /* End MultiListAddItem */

/*---------------------------------------------------------------------------*

	MultiListDeleteItems(w,position,nitems)

	This routine takes a MultiList widget <mlw> and delete nitems
	starting at the given position.

	The return value is the number of items deleted.

 *---------------------------------------------------------------------------*/

#if defined (__STDC__) && !defined(_NO_PROTO)
int MultiListDeleteItems (Widget w, int position, int nitems)
#else
int MultiListDeleteItems (w, position, nitems)
     Widget w;
     int position;
     int nitems;
#endif
{
  MultiListWidget mlw = (MultiListWidget) w;

  register int i;
  register MultiListItem *item;
  MultiListItem *from, *to;
  int len, num_selected;

  if (position < 0 || position >= MultiListNumItems(mlw)) return 0;

  if (MultiListItemArray(mlw) == NULL)
    return 0;	

  if (nitems < 1)
    return 0;

  /* check that the item number is not greater than 
     MultiListNumItems - position 
     */
  nitems = min(nitems,(MultiListNumItems(mlw) - position));

  /* Free items at position */
  for (i = 0; i < nitems; i++)
    {
      item = MultiListNthItem(mlw,(i + position));
      if(MultiListItemBlinking(item) == True)
	mlw->multiList.num_blinking--;

      XtFree(MultiListItemString(item));
    }

  /* we move items from (position + nitems) to (position) */
  from = MultiListNthItem(mlw,(position + nitems));
  to   = MultiListNthItem(mlw,(position));
  len  = (MultiListNumItems(mlw) - position - nitems)
    *sizeof (MultiListItem);
  bcopy(from, to, len);

  /* realloc an new array of items MultiListItem */
  MultiListItemArray(mlw) = TypeRealloc(
					MultiListItemArray(mlw),
					MultiListItem,(nitems+MultiListNumItems(mlw)));

  /* update the MultiListSelArray of selected items :
     the job consist in removing items from position to (position + nitems)
     that are referenced in the MultiListSelArray
     */
  {
    int from, to;

    from = to = num_selected = 0;
    for ( from = 0; from < MultiListNumSelected(mlw); from++)
      {
	if (( MultiListSelArray(mlw)[from] >= position) &&
	    ( MultiListSelArray(mlw)[from] < (position + nitems)))
	  {
	    /* we are in the interval of deleted items */
	    /* do not copy item */
	  } else
	    if (MultiListSelArray(mlw)[from] < position)
	      {
		/* position is lower than interval of deleted items */
		/* copy selected item as is */
		MultiListSelArray(mlw)[to] = MultiListSelArray(mlw)[from];
		to++; num_selected++;
	      } else
		if (MultiListSelArray(mlw)[from] >= (position + nitems))
		  {
		    /* this is an item after the interval of deleted items */
		    /* move it to the 'to' location and update the index value */
		    MultiListSelArray(mlw)[to] = MultiListSelArray(mlw)[from] - nitems;
		    to++; num_selected++;
		  }
		else
		  XtWarning("widget MultiList : MultiListDeleteItems() we should never go here\n");

      } /* for ( from = 0; from < MultiListMaxSelectable(mlw); from++) */
  }
  /* set up the number of items in the MultiList */
  MultiListNumItems(mlw) -= nitems;

  /* set up the number of selected items */
  MultiListNumSelected(mlw) = num_selected;

  RecalcCoords(mlw, False, False);
  MultiListSetVerticalScrollbar(mlw);
  if (XtIsRealized((Widget)mlw)) Redisplay(mlw,NULL,NULL);

  return nitems;

} /* End MultiListDeleteItems */
/*---------------------------------------------------------------------------*

	MultiListDeleteItem(w,position)

	This routine takes a MultiList widget <mlw> and delete a single item
	at the given position.

 *---------------------------------------------------------------------------*/

#if defined (__STDC__) && !defined(_NO_PROTO)
int MultiListDeleteItem (Widget w, int position)
#else
int MultiListDeleteItem (w, position)
     Widget w;
     int position;
#endif
{
  MultiListWidget mlw = (MultiListWidget) w;

  if (position < 0 || position >= MultiListNumItems(mlw)) return -1;

  return (MultiListDeleteItems((Widget) mlw,position,1));

} /* End MultiListDeleteItem */

/*---------------------------------------------------------------------------*

	MultiListItemIsVisible(mlw, position )

	This routine takes a MultiList widget <mlw> and check that the position
	is visible

	the return value is :
		MULTILIST_NO,
		MULTILIST_YES,
	or	MULTILIST_NEARLY
 *---------------------------------------------------------------------------*/

#if defined (__STDC__) && !defined(_NO_PROTO)
int MultiListItemIsVisible (Widget widget, int position)
#else
int MultiListItemIsVisible (widget, position)
     Widget widget;
     int position;
#endif
{
  MultiListWidget mlw = (MultiListWidget) widget;

  int row,col,x,y, w, h;
  int primitive_border;
  Region region;
  XRectangle rect;
  int return_value;

  if ( ! ItemToRowColumn(mlw,position,&row,&col))
    /* invalid item */
    return False;

  /* we need the pixels position of the item */
  RowColumnToPixels(mlw, row,col,&x,&y,&w,&h);

  /* what is the primitive border */
  primitive_border = 	mlw->primitive.shadow_thickness + 
    mlw->primitive.highlight_thickness;

  /* we use the region primitives from X Lib */
  region = XCreateRegion();

  rect.x = primitive_border;
  rect.y = primitive_border;
  rect.width  = MultiListWidth (mlw) - 2 * primitive_border;
  rect.height = MultiListHeight(mlw) - 2 * primitive_border;

  XUnionRectWithRegion(&rect,region,region);

  switch(XRectInRegion(region, x,y,w,h))
    {
    case RectangleOut:
      return_value = MULTILIST_NO;
      break;

    case RectangleIn:
      return_value = MULTILIST_YES;
      break;

    case RectanglePart:
      return_value = MULTILIST_NEARLY;
      break;

    }

  XDestroyRegion(region);

  return return_value;

} /* End MultiListPosIsVisible */


/*---------------------------------------------------------------------------*

	MultiListItemIsVisibleV(mlw, position )

	This routine takes a MultiList widget <mlw> and check that the position
	is visible in vertical direction

	the return value is :
		MULTILIST_NO,
		MULTILIST_YES,
	or	MULTILIST_NEARLY
 *---------------------------------------------------------------------------*/

#if defined (__STDC__) && !defined(_NO_PROTO)
int MultiListItemIsVisibleV(Widget widget, int position)
#else
int MultiListItemIsVisibleV (widget, position)
     Widget widget;
     int position;
#endif
{
  MultiListWidget mlw = (MultiListWidget) widget;

  int row,col,x,y, w, h;
  int primitive_border;
  XRectangle rect;
  int return_value;

  if ( ! ItemToRowColumn(mlw,position,&row,&col))
    /* invalid item */
    return False;

  /* we need the pixels position of the item */
  RowColumnToPixels(mlw, row,col,&x,&y,&w,&h);

  /* what is the primitive border */
  primitive_border = 	mlw->primitive.shadow_thickness + 
    mlw->primitive.highlight_thickness;

  rect.x = primitive_border;
  rect.y = primitive_border;
  rect.width  = MultiListWidth (mlw) - 2 * primitive_border;
  rect.height = MultiListHeight(mlw) - 2 * primitive_border;

  if (y + h <= rect.y + rect.height)
    return_value = MULTILIST_YES;
  else
    return_value = MULTILIST_NO;

  return return_value;

} /* End MultiListPosIsVisible */


/*---------------------------------------------------------------------------*

	MultiListSetPos(w,position)

	This routine takes a MultiList widget <mlw> and makes the item at the
	given position the fisrt visible position in the list.

	This routine will manage the MultiList widget to insure that the
	item designated by position is visible. If it is possible, this item
	will be placed on the first row.

 *---------------------------------------------------------------------------*/

#if defined (__STDC__) && !defined(_NO_PROTO)
void MultiListSetPos (Widget w, int position)
#else
void MultiListSetPos (w, position)
     Widget w;
     int position;
#endif
{
  MultiListWidget mlw = (MultiListWidget) w;

  int foo, row,col,x,y;
  int primitive_border;
  int old_offset;
  Boolean valid;
  int total_height;
  int vis_part;		/* visible part in pixels */

  valid = ItemToRowColumn(mlw,position,&row,&col);

  if (! valid )
    return;

  /* first we do this to be able to use the RowColumnToPixels function */
  old_offset = mlw->multiList.pixel_offset;
  mlw->multiList.pixel_offset = 0;

  /* the new offset is in y */
  RowColumnToPixels(mlw, row,col,&x,&y,&foo,&foo);

  /* restore the old pixel offset */
  mlw->multiList.pixel_offset = old_offset;

  /* add the primitive width */
  primitive_border = 	mlw->primitive.shadow_thickness + 
    mlw->primitive.highlight_thickness;

  y -= primitive_border;

  if( y == old_offset )
    /* we are already at the position specified by position */
    return;	

  total_height = MultiListNumRows(mlw) * MultiListRowHeight(mlw);
  vis_part = MultiListHeight(mlw) - 2 * ( mlw->primitive.shadow_thickness +
					  mlw->primitive.highlight_thickness);

  /* insure that the new location is coherent with scrollbar */
  y = min(y, (total_height - vis_part));

  /* do the scrolling operation */
  MultiListScroll(mlw, y);

  /* update scrollbar */
  MultiListSetVerticalScrollbar(mlw);

} /* End MultiListSetPos */

/*---------------------------------------------------------------------------*

	MultiListBlinkItem(w,item_index)

	This routine takes a MultiList widget <mlw> and makes the item at the
	given position blink every blinkDelay ( 500ms is default).

	To achieve blinking, we inverse foreground and background.

	Selected items are also blinking but at opposite time in comparision
	with unselected items.

 *---------------------------------------------------------------------------*/

#if defined (__STDC__) && !defined(_NO_PROTO)
void MultiListBlinkItem (Widget w, int item_index)
#else
void MultiListBlinkItem (w, item_index)
     Widget w;
     int item_index;
#endif
{
  MultiListWidget mlw = (MultiListWidget) w;
  MultiListItem  *item;

  if (item_index < 0 || item_index >= MultiListNumItems (mlw))
    return;

  item = MultiListNthItem (mlw, item_index);

  if(MultiListItemBlinking(item) == True)
    /* the selected item is already blinking ! */
    return;

  mlw->multiList.num_blinking++;

  MultiListItemBlinking (item) = True;

  if (mlw->multiList.blink_timer == 0)
    {
      mlw->multiList.blink_timer =
	XtAddTimeOut (mlw->multiList.blink_delay, MultiListBlinkingProc, mlw);
    }

}

/*---------------------------------------------------------------------------*

	MultiListUnblinkItem(w, item_index )

 *---------------------------------------------------------------------------*/

#if defined (__STDC__) && !defined(_NO_PROTO)
void MultiListUnblinkItem (Widget w, int item_index)
#else
void MultiListUnblinkItem (w, item_index)
     Widget w;
     int item_index;
#endif
{
  MultiListWidget mlw = (MultiListWidget) w;
  MultiListItem  *item;
  int row, col;

  if (item_index < 0 || item_index >= MultiListNumItems (mlw))
    return;

  item = MultiListNthItem (mlw, item_index);

  if(MultiListItemBlinking(item) == False)
    /* the selected item is already 'un'blinking ! */
    return;

  MultiListItemBlinking (item) = False;

  /* force redraw of selected item */
  ItemToRowColumn(mlw,item_index,&row,&col);
  RedrawRowColumn(mlw,row,col);

  mlw->multiList.num_blinking--;

}

/*---------------------------------------------------------------------------*

	MultiListBlinkingProc()

 *---------------------------------------------------------------------------*/

#if defined (__STDC__) && !defined(_NO_PROTO)
static void MultiListBlinkingProc (XtPointer client_data, XtIntervalId *id)
#else
static void MultiListBlinkingProc (client_data, id)
     XtPointer client_data;
     XtIntervalId *id;
#endif
{
  MultiListWidget mlw = (MultiListWidget) client_data;

#ifdef DEBUG_1
  printf("MultiList %s : nb items = %d, blinking items = %d\n",
	 XtName(mlw),
	 MultiListNumItems (mlw), mlw->multiList.num_blinking);
#endif

  if ((mlw->multiList.num_blinking == 0))
    {
      mlw->multiList.blink_timer = 0;
      return;
    }

  MultiListRefreshArea(mlw, 0, 0, mlw->core.width, mlw->core.height);

  mlw->multiList.blink_timer=XtAddTimeOut (mlw->multiList.blink_delay, MultiListBlinkingProc, mlw);

  /* if blink state was on set it off */
  mlw->multiList.blink_on = !mlw->multiList.blink_on;

}


/*---------------------------------------------------------------------------*

	int NumColumnWidths (MultiListWidget mlw)

	This routine takes a widget and  return number of column widths
	in the column width array. return 0 if there are none or forceColumn
	is not set

 *---------------------------------------------------------------------------*/

#if defined (__STDC__) && !defined(_NO_PROTO)
static int NumColumnWidths (MultiListWidget mlw)
#else
static int NumColumnWidths (mlw)
     MultiListWidget mlw;
#endif
{
  int i = 0;
  Dimension *array = NULL;

  if (!MultiListForceCols(mlw))
    return 0;

  if ((array = MultiListPrefWidths (mlw)) == NULL)
    return 0;
  
  while (array[i] != 0)
    i++;

  return i;
}


/*---------------------------------------------------------------------------*

	Dimension TotalWidth (MultiListWidget mlw, int numcol)

	This routine takes a widget and  return total width from left to right
	of number of column 'numcol'
 *---------------------------------------------------------------------------*/
#if defined (__STDC__) && !defined(_NO_PROTO)  
static Dimension TotalWidth  (MultiListWidget mlw, int numcol)
#else
static Dimension TotalWidth  (mlw, numcol)
     MultiListWidget mlw; 
     int numcol;
#endif
{
  Dimension *array = NULL;
  Dimension tw = 0;
  Dimension ow = 0;
  int       arraysize = 0;
  int       i = 0;

  arraysize = NumColumnWidths (mlw);
    
  array = MultiListPrefWidths (mlw);
  if (arraysize >= numcol) {
    for (i = 0; i < numcol; i++)
      tw += (array[i] + MultiListSepWidth(mlw) + MultiListColumnSpace(mlw));
  }
  else {
    for (i = 0; i < arraysize; i++) {
      ow = array[i] + MultiListSepWidth(mlw) + MultiListColumnSpace(mlw);
      tw += ow;
    }
    for (i = arraysize; i < numcol; i++)
      tw += ow;
  }

  return tw;
}


/*---------------------------------------------------------------------------*

	Dimension ColumnWidth (MultiListWidget mlw, int col)

	This routine takes a widget and  return width of column 'col'
 *---------------------------------------------------------------------------*/
#if defined (__STDC__) && !defined(_NO_PROTO)  
static void ColumnGeometry (MultiListWidget mlw , int col, int *xptr, int* wptr)
#else
static void ColumnGeometry (mlw , col, xptr, wptr)
     MultiListWidget mlw;
     int col;
     int *xptr;
     int *wptr;
#endif
{
  Dimension* array = NULL;
  Dimension tw = 0;
  Dimension ow = 0;
  int i = 0;
  int arraysize = 0;

  arraysize = NumColumnWidths (mlw);

  array = MultiListPrefWidths (mlw);

  if (arraysize >= col)
    for (i = 0; i < col; i++) {
      tw += (array[i] + MultiListSepWidth(mlw) + MultiListColumnSpace(mlw));
    }
  else {
    for (i = 0; i < arraysize; i++) {
	ow = array[i] + MultiListSepWidth(mlw) + MultiListColumnSpace(mlw);
	tw += ow;
    }
    for (i = arraysize; i < col; i++)
      tw += ow;
  }
  
  *xptr = tw;
    
  /* calculate w_ptr now */
  /* column width needs not include sep width and column space*/
  if (arraysize >= col + 1)
    *wptr = array[col];
  else 
    *wptr = array[arraysize - 1];
}


/*---------------------------------------------------------------------------*

	int ColumnByPixel (MultiListWidget mlw, int x)

	This routine takes a widget and and X pixel value return column number
 *---------------------------------------------------------------------------*/
#if defined (__STDC__) && !defined(_NO_PROTO)  
static int ColumnByPixel (MultiListWidget mlw , int x)
#else
static int ColumnByPixel (mlw , x)
     MultiListWidget mlw;
     int  x;
#endif
{
  Dimension* array = NULL;
  Dimension tw = 0;
  Dimension ow = 0;
  int       i = 0;
  int       arraysize = 0;

  arraysize = NumColumnWidths (mlw);

  array = MultiListPrefWidths (mlw);
  tw = 0;
  if (arraysize >= MultiListDefaultCols (mlw)) {
    for (i = 0; i < arraysize; i++) {
      tw += (array[i] + MultiListSepWidth(mlw) + MultiListColumnSpace(mlw));
      if (x  < tw) 
	return i;
    }
  }
  else {
    for (i = 0; i < arraysize; i++) {
      ow = array[i] + MultiListSepWidth(mlw) + MultiListColumnSpace(mlw);
      tw += ow;
      if (x < tw) 
	return i;
    }
    for (i = arraysize; i < MultiListDefaultCols (mlw); i++) {
      tw += ow;
      if (x < tw) 
	return i;
    }
  }
  
}

#if defined (__STDC__) && !defined(_NO_PROTO)
static void _MultiListSetColor (MultiListWidget mlw, int fgindex,
			       int item_index)
#else
static void _MultiListSetColor (mlw, fgindex, item_index)
     MultiListWidget mlw;
     int fgindex;
     int item_index;
#endif
{
  MultiListItem *item;

  if (item_index < 0 || item_index >= MultiListNumItems(mlw))
    return;

  item = MultiListNthItem(mlw,item_index);

  /* if already selected, do nothing */
  if (MultiListItemHighlighted(item) == True) return;

  /* if insensitive, do nothing */
  if (MultiListItemSensitive(item) == False)
    {
      /* Marc QUINTON this should be a ressource ! */
      /* MultiListUnhighlightAll(mlw); */
      return;
    }

  /* if already in this index, do nothing */
  if (MultiListItemFgIndex(item) == fgindex)
    return;

  MultiListItemFgIndex(item) = fgindex;

  RedrawItem(mlw,item_index);
}

/*---------------------------------------------------------------------------*

	MultiListSetColor (mlw, fgindex, item_index)

	Set color to item in index 'item_index' with foreground index fgindex

 *---------------------------------------------------------------------------*/

#if defined (__STDC__) && !defined(_NO_PROTO)
/*sergey: to make Darwin happy: static*/ void MultiListSetColor (Widget w, int fgindex,
			       int item_index)
#else
static void MultiListSetColor (w, fgindex, item_index)
     Widget w;
     int fgindex;
     int item_index;
#endif
{
  MultiListWidget mlw = (MultiListWidget) w;

  MultiListSetClipRect(mlw);

  _MultiListSetColor (mlw, fgindex, item_index);

  MultiListUnsetClipRect(mlw);
} 


/*---------------------------------------------------------------------------*

	MultiListSetColorColumn (w, fgindex, column)

	This routine set color index in the MultiList
	widget in the specified column. 

 *---------------------------------------------------------------------------*/

#if defined (__STDC__) && !defined(_NO_PROT)
void MultiListSetColorColumn (Widget w, int fgindex, int column)
#else
void MultiListSetColorColumn (w, fgindex, column)
     Widget w;
     int fgindex;
     int column;
#endif
{
  MultiListWidget mlw = (MultiListWidget) w;
  int row, item_number;
  Boolean item_valid;

  MultiListSetClipRect(mlw);
  for (row = 0; row < MultiListNumRows(mlw); row++)
    {
      item_valid = RowColumnToItem(mlw,row,column,&item_number);
      if ( item_valid )
	{
	  _MultiListSetColor(mlw, fgindex, item_number);
	}
    }
  MultiListUnsetClipRect(mlw);
}


/*---------------------------------------------------------------------------*

	MultiListSetColorAll(w, fgindex)

	This routine set color to all items in the MultiList
	widget <mlw> to color index fgindex

 *---------------------------------------------------------------------------*/

#if defined (__STDC__) && !defined(_NO_PROTO)
void MultiListSetColorAll (Widget w, int fgindex)
#else
void MultiListSetColorAll (w, fgindex)
     Widget w;
     int    fgindex;
#endif
{
  MultiListWidget mlw = (MultiListWidget) w;
  int i;

  MultiListSetClipRect(mlw);
  for (i = 0; i < MultiListNumItems(mlw); i++)
    _MultiListSetColor (mlw,fgindex, i);
  
  MultiListUnsetClipRect(mlw);
} 


/*---------------------------------------------------------------------------*

	MultiListAddItemsWithColor(w,list,nitems,longest,position,resize,
	sensitivity_array. fgindex)

	This routine takes a MultiList widget <mlw> and add new data items.  
	If the number of items is 0, they will be counted, so the array must
	be NULL terminated.  If the list of strings is NULL, this is treated
	as a list of 0 elements.

 *---------------------------------------------------------------------------*/

#if defined (__STDC__) && !defined(_NO_PROTO)
void MultiListAddItemsWithColor (Widget w, 
				 String *list, 
				 int nitems, 
				 int longest, 
				 int position, 
				 Boolean resize, 
				 Boolean *sensitivity_array,
				 int fgindex)
#else
void MultiListAddItemsWithColor (w, list, nitems, longest, position, 
				 resize, sensitivity_array, fgindex)
     Widget w;
     String *list;
     int nitems;
     int longest;
     int position;
     Boolean resize;
     Boolean *sensitivity_array;
     int fgindex;
#endif
{
  MultiListWidget mlw = (MultiListWidget) w;
  register int i;
  register MultiListItem *item;
  MultiListItem *items;
  MultiListItem *from, *to;
  int len;
  int max_text_width = 0;
  int text_width;
  String str;

  if (position < 0 || position > MultiListNumItems(mlw))
    return;

  if (nitems == 0)		/* Count The Elements */
    {
      if (list == NULL)	/* No elements */
	{
	  /* just return ! */
	  return;
	}
      else
	{
	  for (i = 0; list[i] != NULL; i++);
	  nitems = i;
	}
    }
  if (nitems == 0)		/* No Items */
    {
      /* one more time just return */
      return;
    }
  else
    {   /* now we have a non null list of nitems */

      /* realloc an new array of items MultiListItem */
      items = TypeRealloc(
			  MultiListItemArray(mlw),
			  MultiListItem,(nitems+MultiListNumItems(mlw)));

      MultiListItemArray(mlw) = items;

      /* we move item ptr from position to (position + nitems) */
      if (position == MultiListNumItems (mlw)) {
	from = MultiListNthItem(mlw,(position));
	to   = MultiListNthItem(mlw,(position + nitems));
	len  = (MultiListNumItems(mlw) - position) * sizeof (MultiListItem);
	bcopy(from, to, len);
      }

      /* fill the new items at position */
      for (i = 0; i < nitems; i++)
	{
	  item = MultiListNthItem(mlw,(i + position));
	  if (sensitivity_array == NULL ||
	      (sensitivity_array[i] == True))
	    {
	      MultiListItemSensitive(item) = True;
	    }
	  else
	    {
	      MultiListItemSensitive(item) = False;
	    }
	  MultiListItemString(item) = StrCopy(list[i]);
	  MultiListItemHighlighted(item) = False;
	  MultiListItemBlinking(item) = False;
	  MultiListItemFgIndex(item) = fgindex;
			

	  /* look at the longest item */
	  if(longest == 0);
	  {
	    str = MultiListItemString(item);
	    text_width = FontW(MultiListFont(mlw),str);
	    max_text_width = max(max_text_width,text_width);
	  }
	}
    } /* else nitems == 0 */

  /* set up the number of items in the MultiList */
  MultiListNumItems(mlw) += nitems;

  /* if longest is 0, we have calculated the new longest item,
     so update the value to the widget
     else impose the new size of a column in pixels.
     */
  if(longest == 0)
    MultiListLongest(mlw) = max(max_text_width,MultiListLongest(mlw));		
  else
    MultiListLongest(mlw) = longest;

  RecalcCoords(mlw,resize,resize);
  MultiListSetVerticalScrollbar(mlw);
  if (XtIsRealized((Widget)mlw)) Redisplay(mlw,NULL,NULL);
	

}


/*---------------------------------------------------------------------------*

	MultiListAddItemWithColor(w,item, position )

	This routine takes a MultiList widget <mlw> and add a single new 
        data item to the list.

 *---------------------------------------------------------------------------*/

#if defined (__STDC__) && !defined(_NO_PROTO)
void MultiListAddItemWithColor (Widget w, String item, int position, int fgcolor)
#else
void MultiListAddItemWithColor (w, item, position, fgcolor)
     Widget w;
     String item;
     int position;
     int fgcolor;
#endif
{
  MultiListWidget mlw = (MultiListWidget) w;

  if (position < 0 || position > MultiListNumItems(mlw))
    return;

  /* (mlw,list,nitems,longest,position,resize,sensitivity_array) */
  MultiListAddItemsWithColor ( (Widget)mlw, &item, 1, 0, position, False, NULL, fgcolor);

} /* End MultiListAddItemWithColor */

/*---------------------------------------------------------------------------*

	_MultiListEraseSeparator (mlw, x, y1, y2 )

	This routine takes a MultiList widget <mlw> and draw separaror
	from (x, y1) t0 (x, y2)

 *---------------------------------------------------------------------------*/
#if defined (__STDC__) && !defined(_NO_PROTO)
static void 
_MultiListEraseSeparator(Display *display, 
			 Drawable win, 
			 GC separator_GC, 
			 Position wx, 
			 Position wy, 
			 Dimension wwidth,	
			 Dimension wheight, 
			 Dimension shadowThickness, 
			 Dimension margin,	
			 unsigned char orientation, 
			 unsigned char separator_type)
#else
static void 
_MultiListEraseSeparator(display, 
			 win, 
			 separator_GC, 
			 wx, 
			 wy, 
			 wwidth,	
			 wheight, 
			 shadowThickness, 
			 margin,	
			 orientation, 
			 separator_type)
     Display *display;
     Drawable win;
     GC separator_GC;
     Position wx;
     Position wy;
     Dimension wwidth;
     Dimension wheight;
     Dimension shadowThickness;
     Dimension margin;
     unsigned char orientation;
     unsigned char separator_type;
#endif
{
    int x,y;
    int x1 = 0,y1 = 0,x2 = 0,y2 = 0,x3 = 0,y3 = 0,x4 = 0,y4 = 0;
    int width, height, i;

#ifdef	DO_FLUSH
    XFlush(display);
#endif

    if (orientation == XmHORIZONTAL)
    {
	x = wx + margin;
	y = wy + wheight/2;
	width = wwidth - 2 * margin;
	if (separator_type  == XmDOUBLE_LINE ||
	    separator_type  == XmDOUBLE_DASHED_LINE) {
	    y2 = y1 = y;
	    y3 = y4 = y+1;
	    x3 = x1 = x;
	    x4 = x2 = x+width;
	}
	else if (separator_type  == XmSHADOW_ETCHED_OUT ||
		 separator_type  == XmSHADOW_ETCHED_IN ||
		 separator_type  == XmSHADOW_ETCHED_IN_DASH ||
		 separator_type  == XmSHADOW_ETCHED_OUT_DASH) {
	    y2 = y1 = y - shadowThickness/2;
	    y3 = y4 = y;
	    x3 = x1 = x;
	    x4 = x2 = x+width;
	}
	else
	{
	    x1 = x;
	    y1 = y;
	    x2 = x+width;
	    y2 = y;
        }
    }
    else
    {
	x = wx + wwidth/2;
	y = wy + margin;
	height = wheight - 2*margin;
	y1 = y+height;
	x1 = x;
	if (separator_type  == XmDOUBLE_LINE ||
	    separator_type  == XmDOUBLE_DASHED_LINE) {
	    y3 = y1 = y;
	    y2 = y4 = y+height;
	    x2 = x1 = x;
	    x4 = x3 = x+1;
        }
	else if (separator_type  == XmSHADOW_ETCHED_OUT ||
		 separator_type  == XmSHADOW_ETCHED_IN ||
		 separator_type  == XmSHADOW_ETCHED_IN_DASH ||
		 separator_type  == XmSHADOW_ETCHED_OUT_DASH){
	    y3 = y1 = y;
	    y2 = y4 = y+height;
	    x2 = x1 = x - shadowThickness/2;
	    x4 = x3 = x;
        }
	else
	{
	    x1 = x;
	    y1 = y;
	    x2 = x;
	    y2 = y+height;
        }
    }

    switch(separator_type)
    {
    case XmSINGLE_LINE:
    	XDrawLine(display, win, separator_GC, x1,y1,x2,y2);
        break;
    case XmSINGLE_DASHED_LINE:
    	XDrawLine(display, win, separator_GC, x1,y1,x2,y2);
	break;
    case XmDOUBLE_LINE:
    	XDrawLine(display, win, separator_GC, x1,y1,x2,y2);
    	XDrawLine(display, win, separator_GC, x3,y3,x4,y4);
	break;
    case XmDOUBLE_DASHED_LINE:
    	XDrawLine(display, win, separator_GC, x1,y1,x2,y2);
    	XDrawLine(display, win, separator_GC, x3,y3,x4,y4);
	break;
    case XmSHADOW_ETCHED_IN:
        if (orientation == XmVERTICAL) {
	    for (i = 0; i < shadowThickness/2; i++) {
		XDrawLine(display, win, separator_GC,
			  x1 + i, y1 + (shadowThickness/2 - i - 1),
			  x2 + i, y2 - (shadowThickness/2 - i - 1));
	    }
	    for (i = 0; i < shadowThickness/2; i++) {
		XDrawLine(display, win, separator_GC,
			  x3 + i, y3 + i,
			  x4 + i, y4 - i);
	    }
	}
	else {
	    for (i = 0; i < shadowThickness/2; i++) {
		XDrawLine(display, win, separator_GC,
			  x1 + (shadowThickness/2 - i - 1), y1 + i,
			  x2 - (shadowThickness/2 - i - 1), y2 + i);
	    }
	    for (i = 0; i < shadowThickness/2; i++) {
		XDrawLine(display, win, separator_GC,
			  x3 + i, y3 + i,
			  x4 - i, y4 + i);
	    }
	}
        break;
    case XmSHADOW_ETCHED_OUT:
	if (orientation == XmVERTICAL) {
	    for (i = 0; i < shadowThickness/2; i++)
		XDrawLine(display, win, separator_GC,
			  x1 + i, y1 + (shadowThickness/2 - i - 1),
			  x2 + i, y2 - (shadowThickness/2 - i - 1));
	    for (i = 0; i < shadowThickness/2; i++)
		XDrawLine(display, win, separator_GC,
			  x3 + i, y3 + i,
			  x4 + i, y4 - i);
	}
	else {
	    for (i = 0; i < shadowThickness/2; i++)
		XDrawLine(display, win, separator_GC,
			  x1 + (shadowThickness/2 - i - 1), y1 + i,
			  x2 - (shadowThickness/2 - i - 1), y2 + i);
	    for (i = 0; i < shadowThickness/2; i++)
		XDrawLine(display, win, separator_GC,
			  x3 + i, y3 + i,
			  x4 - i, y4 + i);
	}
	break;
    case XmSHADOW_ETCHED_IN_DASH:
	XSetLineAttributes(display, separator_GC, 0, LineDoubleDash,
			   CapButt,JoinMiter);
	XSetLineAttributes(display, separator_GC, 0, LineDoubleDash,
			   CapButt,JoinMiter);
        if (orientation == XmVERTICAL) {
	    for (i = 0; i < shadowThickness/2; i++) {
		XDrawLine(display, win, separator_GC,
			  x1 + i, y1 + (shadowThickness/2 - i - 1),
			  x2 + i, y2 - (shadowThickness/2 - i - 1));
	    }
	    for (i = 0; i < shadowThickness/2; i++) {
		XDrawLine(display, win, separator_GC,
			  x3 + i, y3 + i,
			  x4 + i, y4 - i);
	    }
	}
	else {
	    for (i = 0; i < shadowThickness/2; i++) {
		XDrawLine(display, win, separator_GC,
			  x1 + (shadowThickness/2 - i - 1), y1 + i,
			  x2 - (shadowThickness/2 - i - 1), y2 + i);
	    }
	    for (i = 0; i < shadowThickness/2; i++) {
		XDrawLine(display, win, separator_GC,
			  x3 + i, y3 + i,
			  x4 - i, y4 + i);
	    }
	}
	break;
    case XmSHADOW_ETCHED_OUT_DASH:
	if (orientation == XmVERTICAL) {
	    for (i = 0; i < shadowThickness/2; i++)
		XDrawLine(display, win, separator_GC,
			  x1 + i, y1 + (shadowThickness/2 - i - 1),
			  x2 + i, y2 - (shadowThickness/2 - i - 1));
	    for (i = 0; i < shadowThickness/2; i++)
		XDrawLine(display, win, separator_GC,
			  x3 + i, y3 + i,
			  x4 + i, y4 - i);
	}
	else {
	    for (i = 0; i < shadowThickness/2; i++)
		XDrawLine(display, win, separator_GC,
			  x1 + (shadowThickness/2 - i - 1), y1 + i,
			  x2 - (shadowThickness/2 - i - 1), y2 + i);
	    for (i = 0; i < shadowThickness/2; i++)
		XDrawLine(display, win, separator_GC,
			  x3 + i, y3 + i,
			  x4 - i, y4 + i);
	}
	break;
    case XmNO_LINE:
	break;
    default:
    	XDrawLine(display, win, separator_GC, x,y,x1,y1);
	break;
    }
}



/*---------------------------------------------------------------------------*

	_MultiListDrawSeparator (mlw, x, y1, y2 )

	This routine takes a MultiList widget <mlw> and draw separaror
	from (x, y1) t0 (x, y2)

 *---------------------------------------------------------------------------*/
#if defined (__STDC__) && !defined(_NO_PROTO)
static void 
_MultiListDrawSeparator(Display *display, 
			Drawable win, 
			GC top_gc, 
			GC bottom_gc,	
			GC separator_GC, 
			Position wx, 
			Position wy, 
			Dimension wwidth,	
			Dimension wheight, 
			Dimension shadowThickness, 
			Dimension margin,	
			unsigned char orientation, 
			unsigned char separator_type)
#else
static void 
_MultiListDrawSeparator(display, win, top_gc, bottom_gc, separator_GC, 
			wx, wy, 
			wwidth,	wheight, 
			shadowThickness, 
			margin,	
			orientation, 
			separator_type)
     Display *display;
     Drawable win;
     GC top_gc;
     GC bottom_gc;
     GC separator_GC;
     Position wx;
     Position wy;
     Dimension wwidth;
     Dimension wheight;
     Dimension shadowThickness;
     Dimension margin;
     unsigned char orientation;
     unsigned char separator_type;
#endif
{
    int x,y;
    int x1 = 0,y1 = 0,x2 = 0,y2 = 0,x3 = 0,y3 = 0,x4 = 0,y4 = 0;
    int width, height, i;

#ifdef	DO_FLUSH
    XFlush(display);
#endif

    if (orientation == XmHORIZONTAL)
    {
	x = wx + margin;
	y = wy + wheight/2;
	width = wwidth - 2 * margin;
	if (separator_type  == XmDOUBLE_LINE ||
	    separator_type  == XmDOUBLE_DASHED_LINE) {
	    y2 = y1 = y;
	    y3 = y4 = y+1;
	    x3 = x1 = x;
	    x4 = x2 = x+width;
	}
	else if (separator_type  == XmSHADOW_ETCHED_OUT ||
		 separator_type  == XmSHADOW_ETCHED_IN ||
		 separator_type  == XmSHADOW_ETCHED_IN_DASH ||
		 separator_type  == XmSHADOW_ETCHED_OUT_DASH) {
	    y2 = y1 = y - shadowThickness/2;
	    y3 = y4 = y;
	    x3 = x1 = x;
	    x4 = x2 = x+width;
	}
	else
	{
	    x1 = x;
	    y1 = y;
	    x2 = x+width;
	    y2 = y;
        }
    }
    else
    {
	x = wx + wwidth/2;
	y = wy + margin;
	height = wheight - 2*margin;
	y1 = y+height;
	x1 = x;
	if (separator_type  == XmDOUBLE_LINE ||
	    separator_type  == XmDOUBLE_DASHED_LINE) {
	    y3 = y1 = y;
	    y2 = y4 = y+height;
	    x2 = x1 = x;
	    x4 = x3 = x+1;
        }
	else if (separator_type  == XmSHADOW_ETCHED_OUT ||
		 separator_type  == XmSHADOW_ETCHED_IN ||
		 separator_type  == XmSHADOW_ETCHED_IN_DASH ||
		 separator_type  == XmSHADOW_ETCHED_OUT_DASH){
	    y3 = y1 = y;
	    y2 = y4 = y+height;
	    x2 = x1 = x - shadowThickness/2;
	    x4 = x3 = x;
        }
	else
	{
	    x1 = x;
	    y1 = y;
	    x2 = x;
	    y2 = y+height;
        }
    }

    switch(separator_type)
    {
    case XmSINGLE_LINE:
    	XDrawLine(display, win, separator_GC, x1,y1,x2,y2);
        break;
    case XmSINGLE_DASHED_LINE:
    	XDrawLine(display, win, separator_GC, x1,y1,x2,y2);
	break;
    case XmDOUBLE_LINE:
    	XDrawLine(display, win, separator_GC, x1,y1,x2,y2);
    	XDrawLine(display, win, separator_GC, x3,y3,x4,y4);
	break;
    case XmDOUBLE_DASHED_LINE:
    	XDrawLine(display, win, separator_GC, x1,y1,x2,y2);
    	XDrawLine(display, win, separator_GC, x3,y3,x4,y4);
	break;
    case XmSHADOW_ETCHED_IN:
        if (orientation == XmVERTICAL) {
	    for (i = 0; i < shadowThickness/2; i++) {
		XDrawLine(display, win, bottom_gc,
			  x1 + i, y1 + (shadowThickness/2 - i - 1),
			  x2 + i, y2 - (shadowThickness/2 - i - 1));
	    }
	    for (i = 0; i < shadowThickness/2; i++) {
		XDrawLine(display, win, top_gc,
			  x3 + i, y3 + i,
			  x4 + i, y4 - i);
	    }
	}
	else {
	    for (i = 0; i < shadowThickness/2; i++) {
		XDrawLine(display, win, bottom_gc,
			  x1 + (shadowThickness/2 - i - 1), y1 + i,
			  x2 - (shadowThickness/2 - i - 1), y2 + i);
	    }
	    for (i = 0; i < shadowThickness/2; i++) {
		XDrawLine(display, win, top_gc,
			  x3 + i, y3 + i,
			  x4 - i, y4 + i);
	    }
	}
        break;
    case XmSHADOW_ETCHED_OUT:
	if (orientation == XmVERTICAL) {
	    for (i = 0; i < shadowThickness/2; i++)
		XDrawLine(display, win, top_gc,
			  x1 + i, y1 + (shadowThickness/2 - i - 1),
			  x2 + i, y2 - (shadowThickness/2 - i - 1));
	    for (i = 0; i < shadowThickness/2; i++)
		XDrawLine(display, win, bottom_gc,
			  x3 + i, y3 + i,
			  x4 + i, y4 - i);
	}
	else {
	    for (i = 0; i < shadowThickness/2; i++)
		XDrawLine(display, win, top_gc,
			  x1 + (shadowThickness/2 - i - 1), y1 + i,
			  x2 - (shadowThickness/2 - i - 1), y2 + i);
	    for (i = 0; i < shadowThickness/2; i++)
		XDrawLine(display, win, bottom_gc,
			  x3 + i, y3 + i,
			  x4 - i, y4 + i);
	}
	break;
    case XmSHADOW_ETCHED_IN_DASH:
	XSetLineAttributes(display, top_gc, 0, LineDoubleDash,
			   CapButt,JoinMiter);
	XSetLineAttributes(display, bottom_gc, 0, LineDoubleDash,
			   CapButt,JoinMiter);
        if (orientation == XmVERTICAL) {
	    for (i = 0; i < shadowThickness/2; i++) {
		XDrawLine(display, win, bottom_gc,
			  x1 + i, y1 + (shadowThickness/2 - i - 1),
			  x2 + i, y2 - (shadowThickness/2 - i - 1));
	    }
	    for (i = 0; i < shadowThickness/2; i++) {
		XDrawLine(display, win, top_gc,
			  x3 + i, y3 + i,
			  x4 + i, y4 - i);
	    }
	}
	else {
	    for (i = 0; i < shadowThickness/2; i++) {
		XDrawLine(display, win, bottom_gc,
			  x1 + (shadowThickness/2 - i - 1), y1 + i,
			  x2 - (shadowThickness/2 - i - 1), y2 + i);
	    }
	    for (i = 0; i < shadowThickness/2; i++) {
		XDrawLine(display, win, top_gc,
			  x3 + i, y3 + i,
			  x4 - i, y4 + i);
	    }
	}
	break;
    case XmSHADOW_ETCHED_OUT_DASH:
	if (orientation == XmVERTICAL) {
	    for (i = 0; i < shadowThickness/2; i++)
		XDrawLine(display, win, top_gc,
			  x1 + i, y1 + (shadowThickness/2 - i - 1),
			  x2 + i, y2 - (shadowThickness/2 - i - 1));
	    for (i = 0; i < shadowThickness/2; i++)
		XDrawLine(display, win, bottom_gc,
			  x3 + i, y3 + i,
			  x4 + i, y4 - i);
	}
	else {
	    for (i = 0; i < shadowThickness/2; i++)
		XDrawLine(display, win, top_gc,
			  x1 + (shadowThickness/2 - i - 1), y1 + i,
			  x2 - (shadowThickness/2 - i - 1), y2 + i);
	    for (i = 0; i < shadowThickness/2; i++)
		XDrawLine(display, win, bottom_gc,
			  x3 + i, y3 + i,
			  x4 - i, y4 + i);
	}
	break;
    case XmNO_LINE:
	break;
    default:
    	XDrawLine(display, win, separator_GC, x,y,x1,y1);
	break;
    }
}

/*---------------------------------------------------------------------------*

	MultiListEraseSeparator (mlw, x, y1, y2 )

	This routine takes a MultiList widget <mlw> and erase separaror
	at x, y1 to y2

 *---------------------------------------------------------------------------*/
#if defined (__STDC__) && !defined(_NO_PROTO)
static void MultiListEraseSeparator (MultiListWidget mlw,
				     int x,
				     int y1, int y2, int wd)
#else
static void MultiListEraseSeparator (mlw, x, y1, y2, wd)
     MultiListWidget mlw;
     int x;
     int y1;
     int y2;
     int wd;
#endif
{
  /* if separator width is zero, we will do nothing here */
  if (MultiListSepWidth(mlw) == 0)
    return;

  _MultiListEraseSeparator (XtDisplay(mlw), XtWindow (mlw),
			   MultiListEraseGC(mlw),
			   (Position)x,
			   (Position)y1,
			   (Dimension)wd,
			   (Dimension)(y2 - y1),
			   (Dimension)wd,
			   0, XmVERTICAL, MultiListSepType(mlw));
}

  

/*---------------------------------------------------------------------------*

	MultiListDrawSeparator (mlw, col, y1, y2 )

	This routine takes a MultiList widget <mlw> and draw separaror
	at col, y1 to y2

 *---------------------------------------------------------------------------*/
#if defined (__STDC__) && !defined(_NO_PROTO)
static void MultiListDrawSeparator (MultiListWidget mlw,
				    int col,
				    int y1, int y2)
#else
static void MultiListDrawSeparator (mlw, col, y1, y2)
     MultiListWidget mlw;
     int col;
     int y1;
     int y2;
#endif
{
  int colx;
  int colw;

  /* if separator width is zero, we will do nothing here */
  if (MultiListSepWidth(mlw) == 0)
    return;

  ColumnGeometry (mlw , col + 1, &colx, &colw);

  _MultiListDrawSeparator (XtDisplay(mlw), XtWindow (mlw),
			   MultiListTopShadowGC(mlw),
			   MultiListBottomShadowGC(mlw),
			   MultiListSepGC(mlw),
			   (Position)(colx - MultiListSepWidth(mlw)),
			   (Position)y1,
			   (Dimension)(MultiListSepWidth(mlw)),
			   (Dimension)(y2 - y1),
			   MultiListSepWidth(mlw),
			   0, XmVERTICAL, MultiListSepType(mlw));
}


/*---------------------------------------------------------------------------*

	MultiListMoveSeparator (mlw, col, y1, y2 )

	This routine takes a MultiList widget <mlw> and draw separaror
	at col, y1 to y2

 *---------------------------------------------------------------------------*/
#if defined (__STDC__) && !defined(_NO_PROTO)
static void MultiListMoveSeparator (MultiListWidget mlw,
				    int x,
				    int y1, int y2, int wd)
#else
static void MultiListMoveSeparator (mlw, x, y1, y2, wd)
     MultiListWidget mlw;
     int x;
     int y1;
     int y2;
     int wd;
#endif
{
  _MultiListDrawSeparator (XtDisplay(mlw), XtWindow (mlw),
			   MultiListSepGC(mlw),
			   MultiListSepGC(mlw),
			   MultiListSepGC(mlw),
			   (Position)x,
			   (Position)y1,
			   (Dimension)wd,
			   (Dimension)(y2 - y1),
			   (Dimension)wd,
			   0, XmVERTICAL, MultiListSepType(mlw));
}


/*---------------------------------------------------------------------------*

	MultiListDrawSeparatorT (mlw, col, y1, y2 )

	This routine takes a MultiList widget <mlw> and draw separaror
	at col, y1 to y2

 *---------------------------------------------------------------------------*/
#if defined (__STDC__) && !defined(_NO_PROTO)
static void MultiListDrawSeparatorT (MultiListWidget mlw,
				     int x,
				     int y1, int y2, int wd)
#else
static void MultiListDrawSeparatorT (mlw, x, y1, y2, wd)
     MultiListWidget mlw;
     int x;
     int y1;
     int y2;
     int wd;
#endif
{
  _MultiListDrawSeparator (XtDisplay(mlw), XtWindow (mlw),
			   MultiListTopShadowGC(mlw),
			   MultiListBottomShadowGC(mlw),
			   MultiListSepGC(mlw),
			   (Position)x,
			   (Position)y1,
			   (Dimension)wd,
			   (Dimension)(y2 - y1),
			   (Dimension)wd,
			   0, XmVERTICAL, MultiListSepType(mlw));
}

/*---------------------------------------------------------------------------*

        PixelInsideSeparator (x, y)

	This routine will check whether a point (x, y) is inside a separator

 *---------------------------------------------------------------------------*/
#if defined (__STDC__) && !defined(_NO_PROTO)
static int _PixelInsideSeparator (MultiListWidget mlw, 
				 int x, int y)
#else
static int _PixelInsideSeparator (mlw, x, y)
     MultiListWidget mlw;
     int x;
     int y; 
#endif
{
  int primitive_border;
  int col;
  int colx, colw;

  /* separator only works with forceColumn true */
  if (NumColumnWidths (mlw) == 0 || MultiListSepWidth(mlw) == 0)
    return 0;

  primitive_border =  ( mlw->primitive.shadow_thickness +
			mlw->primitive.highlight_thickness);

  /* if x or y = 0 and primitive_border > 0 we can use max() because 
     we are computing with signed values.
     */

  x = max(primitive_border, x);
  y = max(primitive_border, y);

  col = ColumnByPixel (mlw, x - primitive_border);

  ColumnGeometry (mlw , col + 1, &colx, &colw);

  /* separator starts at colx - sepw */
  if (x <= colx && x > colx - MultiListSepWidth(mlw))
    return 1;
  return 0;
}

/*---------------------------------------------------------------------------*

        PixelInsideSeparator (x, y, colptr)

	This routine will check whether a point (x, y) is inside a separator

 *---------------------------------------------------------------------------*/
#if defined (__STDC__) && !defined(_NO_PROTO)
static int PixelInsideSeparator (MultiListWidget mlw, 
				 int x, int y, int *colptr)
#else
static int PixelInsideSeparator (mlw, x, y, colptr)
     MultiListWidget mlw;
     int x;
     int y; 
     int *colptr;
#endif
{
  int primitive_border;
  int colx, colw;

  /* separator only works with forceColumn true */
  if (NumColumnWidths (mlw) == 0 || MultiListSepWidth(mlw) == 0)
    return 0;

  primitive_border =  ( mlw->primitive.shadow_thickness +
			mlw->primitive.highlight_thickness);

  /* if x or y = 0 and primitive_border > 0 we can use max() because 
     we are computing with signed values.
     */

  x = max(primitive_border, x);
  y = max(primitive_border, y);

  *colptr = ColumnByPixel (mlw, x - primitive_border);

  ColumnGeometry (mlw , *colptr + 1, &colx, &colw);

  /* separator starts at colx - sepw/2 */
  if (x <= colx &&  x > colx - MultiListSepWidth(mlw))
    return 1;
  return 0;
}


/*---------------------------------------------------------------------------*

	MultiListTrackPointer (mlw, col, y1, y2 )

	This routine takes a MultiList widget <mlw> and draw separaror
	at col, y1 to y2

 *---------------------------------------------------------------------------*/
#if defined (__STDC__) && !defined(_NO_PROTO)
static void MultiListTrackPointer (MultiListWidget mlw, 
				   XEvent *event, 
				   String *params, 
				   Cardinal *num_params)
#else
static void MultiListTrackPointer (mlw, event, params, num_params)
     MultiListWidget mlw;
     XEvent *event;
     String *params;
     Cardinal *num_params;
#endif
{
  XMotionEvent* mev;
  int col;
  static int _insep = 0;

  /* separator only works with forceColumn true */
  if (NumColumnWidths (mlw) == 0 || MultiListSepWidth(mlw) == 0)
    return;

  mev = (XMotionEvent *)event;
  if (PixelInsideSeparator (mlw, mev->x, mev->y, &col)) {
    if (!_insep) {
      XDefineCursor (XtDisplay (mlw), XtWindow (mlw), MultiListMCursor(mlw));
      _insep = 1;
    }
  }
  else {
    if (_insep) {
      XUndefineCursor (XtDisplay (mlw), XtWindow (mlw));
      _insep = 0;
    }
  }
}


/*---------------------------------------------------------------------------*

        SeparatorGeometry (mlw, ex, ey, x, y1, y2, sepw)

	This routine takes a MultiList widget <mlw> and return a separator
	geometry
	This routine is called after insideSeparator has been checked

 *---------------------------------------------------------------------------*/
#if defined (__STDC__) && !defined(_NO_PROTO)
static void SeparatorGeometry (MultiListWidget mlw,
			       int ex, int ey,
			       int *x, int* y1, int *y2, int *sepw)
#else
static void SeparatorGeometry (mlw, ex, ey, x, y1, y2, sepw)
     MultiListWidget mlw;
     int ex;
     int ey;
     int *x;
     int *y1; 
     int *y2;
     int *sepw;
#endif
{
  int primitive_border;
  int col;
  int colx, colw;


  primitive_border =  ( mlw->primitive.shadow_thickness +
			mlw->primitive.highlight_thickness);

  /* if x or y = 0 and primitive_border > 0 we can use max() because 
     we are computing with signed values.
     */

  ex = max(primitive_border, ex);
  ey = max(primitive_border, ey);

  col = ColumnByPixel (mlw, ex - primitive_border);

  ColumnGeometry (mlw , col + 1, &colx, &colw);

  *x = colx - MultiListSepWidth(mlw);
  *y1 = primitive_border;
  *y2 = MultiListHeight(mlw) - primitive_border;
  *sepw = MultiListSepWidth(mlw);
}


/*---------------------------------------------------------------------------*

        SeparatorAuxInfo (mlw, ex, ey, col, minx, inix)

	This routine takes a MultiList widget <mlw> and return a separator
	information
	This routine is called after insideSeparator has been checked

 *---------------------------------------------------------------------------*/
#if defined (__STDC__) && !defined(_NO_PROTO)
static void SeparatorAuxInfo (MultiListWidget mlw,
			      int ex, int ey,
			      int *col, int* minx, int* inix)
#else
static void SeparatorAuxInfo (mlw,
			      ex, ey,
			      col, minx, inix)
     MultiListWidget mlw;
     int ex;
     int ey;
     int *col;
     int *minx;
     int* inix;
#endif
{
  int primitive_border;
  int colx, colw;
  int tcol;


  primitive_border =  ( mlw->primitive.shadow_thickness +
			mlw->primitive.highlight_thickness);

  /* if x or y = 0 and primitive_border > 0 we can use max() because 
     we are computing with signed values.
     */

  ex = max(primitive_border, ex);
  ey = max(primitive_border, ey);

  tcol = ColumnByPixel (mlw, ex - primitive_border);

  /* column at the left of this separator */
  ColumnGeometry (mlw , tcol, &colx, &colw);
  *minx = colx;
  
  /* current separator's column information */
  ColumnGeometry (mlw , tcol + 1, &colx, &colw);
  *col = tcol + 1;

  *inix = ex;
}
