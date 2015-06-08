/*-----------------------------------------------------------------------------
 * Copyright (c) 1991,1992 Southeastern Universities Research Association,
 *                         Continuous Electron Beam Accelerator Facility
 *
 * This software was developed under a United States Government license
 * described in the NOTICE file included as part of this distribution.
 *
 * CEBAF Data Acquisition Group, 12000 Jefferson Ave., Newport News, VA 23606
 * Email: coda@cebaf.gov  Tel: (804) 249-7101  Fax: (804) 249-7363
 *-----------------------------------------------------------------------------
 * 
 * Description:
 *	CODA MixedLabel Widget
 *	
 * Author:  Jie Chen, CEBAF Data Acquisition Group
 *
 * Revision History:
 *   $Log: MixedLabel.c,v $
 *   Revision 1.3  1998/02/03 15:19:46  rwm
 *   Casts and stuff to get rid of compiler warnings.
 *
 *   Revision 1.2  1997/08/25 16:08:07  heyes
 *   fix display problems with background
 *
 *   Revision 1.1.1.1  1996/08/21 19:36:08  heyes
 *   Imported sources
 *
 *	  
 */
#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include <X11/Xos.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "MixedLabel.h"
#include "MixedLabelP.h"

#define streq(a,b) (strcmp((a), (b)) == 0)
#define Offset(field) XtOffsetOf(XcodaMixedLabelRec, mixedLabel.field)

/******************************************************************
 *       Full class record constant                               *
 *****************************************************************/

/* private data */
static XtResource resources[]={
  {XcodaNforeground, XcodaCForeground, XtRPixel, sizeof(Pixel),
     Offset(foreground), XtRString, (XtPointer)XtDefaultForeground},
  {XcodaNtopLabel, XcodaCTopLabel, XtRString, sizeof(String),
     Offset(top_label), XtRString, (XtPointer)NULL},
  {XcodaNbottomLabel, XcodaCBottomLabel, XtRString, sizeof(String),
     Offset(bottom_label), XtRString, (XtPointer)NULL},
  {XcodaNnormalPixmap, XcodaCNormalPixmap,XtRPixmap, sizeof(Pixmap),
     Offset(n_pixmap), XtRImmediate, (XtPointer)None},
  {XcodaNhighLightPixmap, XcodaCHighLightPixmap, XtRPixmap, sizeof(Pixmap),
     Offset(h_pixmap), XtRImmediate, (XtPointer)None},
  {XcodaNresize, XcodaCResize, XtRBoolean, sizeof(Boolean),
     Offset(resize),XtRImmediate, (XtPointer)TRUE},
  {XcodaNfont, XcodaCFont, XtRFontStruct, sizeof(XFontStruct *),
     Offset(font),XtRString, (XtPointer)XtDefaultFont},
  {XcodaNsetHighLight, XcodaCSetHighLight, XtRBoolean, sizeof(Boolean),
     Offset(set), XtRImmediate, (XtPointer)FALSE},
  {XcodaNspace, XcodaCSpace, XtRDimension, sizeof(Dimension),
     Offset(space), XtRImmediate, (XtPointer)2},
  {XcodaNactivateCallback,XcodaCActivateCallback,XtRCallback,sizeof(XtCallbackList),
     Offset(callback), XtRCallback, (XtPointer)NULL},
};
#undef Offset

static char defaultTranslations[]=
  "<Btn1Down> : highlight()      \n\
   <Btn1Up>   : activate() unhighlight()";

static void Highlight(), Activate(), Unhighlight();

static XtActionsRec actions[] = {
  {"highlight",       Highlight},
  {"activate",        Activate},
  {"unhighlight",     Unhighlight},
};

/* forward declarations */
static GC GetInvertedGC();
static void Initialize();
static void Resize();
static void Redisplay();
static Boolean SetValues();
static void Destroy();
static XtGeometryResult QueryGeometry();

/* class record declarations */
XcodaMixedLabelClassRec xcodaMixedLabelClassRec = {
  /*core class part */
  {
    /* super class                 */ (WidgetClass)&widgetClassRec,
    /* class_name                  */ "MixedLabel",
    /* widget_size                 */ sizeof(XcodaMixedLabelRec),
    /* class_initialize            */ NULL,
    /* class_part_initialize       */ NULL,
    /* class_initied               */ FALSE,
    /* initialize                  */ Initialize,
    /* initialize_hook             */ NULL,
    /* realize                     */ XtInheritRealize,
    /* actions                     */ actions,
    /* num_actions                 */ XtNumber(actions),
    /* resources                   */ resources,
    /* num_of_resources            */ XtNumber(resources),
    /* xrm_class                   */ NULLQUARK,
    /* compress_motion             */ TRUE,
    /* compress_exposure           */ TRUE,
    /* compress_enterleave         */ TRUE,
    /* visible_interet             */ FALSE,
    /* destroy                     */ Destroy,
    /* resize                      */ Resize,
    /* expose                      */ Redisplay,
    /* set_values                  */ SetValues,
    /* set_values_hook             */ NULL,
    /* set_values_almost           */ XtInheritSetValuesAlmost,
    /* get_values_hook             */ NULL,
    /* accept_focus                */ NULL,
    /* version                     */ XtVersion,
    /* callback offsets            */ NULL,
    /* tm_table                    */ defaultTranslations,
    /* query_geometry              */ QueryGeometry,
    /* display_accelerator         */ XtInheritDisplayAccelerator,
    /* extension                   */ NULL
  },
    /* mixed label class part      */
  {
    /* ignore                      */ 0,
  }				    
};

WidgetClass xcodaMixedLabelWidgetClass = (WidgetClass)&xcodaMixedLabelClassRec;

static GC GetInvertedGC(lw)
     XcodaMixedLabelWidget lw;
{
  XGCValues  values;

  values.foreground = lw->core.background_pixel;
  values.font = lw->mixedLabel.font->fid;

  return XtGetGC((Widget)lw, GCForeground|GCFont, &values);
}

static void Highlight(w, event, params, num_params)
     Widget w;
     XEvent *event;
     String *params;
     Cardinal *num_params;
{
  XcodaMixedLabelWidget lw = (XcodaMixedLabelWidget)w;

  lw->mixedLabel.current_gc = lw->mixedLabel.inverted_gc;
  lw->mixedLabel.inverted = True;
  lw->mixedLabel.set = True;
  if(XtIsRealized(w)){
    XClearWindow(XtDisplay(w), XtWindow(w));
    (*(XtClass(w)->core_class.expose))(w, event, NULL);
  }

/*  XcodaMixedLabelSetHighLightOn(w); */
}


static void Unhighlight(w, event, params, num_params)
     Widget w;
     XEvent *event;
     String *params;
     Cardinal *num_params;
{
  XcodaMixedLabelWidget lw = (XcodaMixedLabelWidget)w;

  lw->mixedLabel.current_gc = lw->mixedLabel.gc;
  lw->mixedLabel.inverted = False;
  lw->mixedLabel.set = False;
  if(XtIsRealized(w)){
    XClearWindow(XtDisplay(w), XtWindow(w));
    (*(XtClass(w)->core_class.expose))(w, event, NULL);
  }

/*  XcodaMixedLabelSetHighLightOff(w);*/
}

static void Activate(w, event, params, num_params)
     Widget w;
     XEvent *event;
     String *params;
     Cardinal *num_params;
{
  XcodaMixedLabelWidget lw = (XcodaMixedLabelWidget)w;

  if(event != NULL && event->type == ButtonRelease &&
     event->xany.window == XtWindow(w)){
    XButtonEvent *b = &event->xbutton;
    if(b->x < 0 || b->y < 0 ||
       b->x >= w->core.width || b->y >= w->core.height)
      return;
  }
  XtCallCallbackList((Widget)lw, lw->mixedLabel.callback, NULL);
}

static Dimension MaximumWidth(a, b, c)
     Dimension a, b ,c;
{
  Dimension temp;

  temp = a;
  if(b > temp)
    temp = b;
  if(c > temp)
    temp = c;

  return temp;
}
    

static void SetWidthAndHeight(lw)
     XcodaMixedLabelWidget lw;
{
  register XFontStruct   *fs = lw->mixedLabel.font;
  Window root;
  int    x,y;
  unsigned int width, height, bw, depth;

  if(lw->mixedLabel.n_pixmap != None){
    if(XGetGeometry(XtDisplay(lw), lw->mixedLabel.n_pixmap, &root,&x,&y,
		    &width,&height,&bw,&depth)){
      lw->mixedLabel.n_pixmap_width = width;
      lw->mixedLabel.n_pixmap_height = height;
      lw->mixedLabel.depth = depth;
    }
  }
  else{
    lw->mixedLabel.n_pixmap_width = 0;
    lw->mixedLabel.n_pixmap_height = 0;
  }
  

  if(lw->mixedLabel.h_pixmap != None){
    if(XGetGeometry(XtDisplay(lw), lw->mixedLabel.h_pixmap, &root,&x,&y,
		    &width,&height,&bw,&depth)){
      lw->mixedLabel.h_pixmap_width = width;
      lw->mixedLabel.h_pixmap_height = height;
      lw->mixedLabel.depth = depth;
    }
  }
  else{
    lw->mixedLabel.h_pixmap_width = 0;
    lw->mixedLabel.h_pixmap_height = 0;
  }

  lw->mixedLabel.top_label_height = lw->mixedLabel.btm_label_height = 
    fs->max_bounds.ascent + fs->max_bounds.descent;

  if(lw->mixedLabel.top_label != NULL){
    lw->mixedLabel.top_label_width =
      XTextWidth(fs, lw->mixedLabel.top_label,
		 strlen(lw->mixedLabel.top_label));
  }
  else
    lw->mixedLabel.top_label_width = 0;

  if(lw->mixedLabel.bottom_label != NULL){
    lw->mixedLabel.btm_label_width =
      XTextWidth(fs, lw->mixedLabel.bottom_label,
		 strlen(lw->mixedLabel.bottom_label));
  }
  else
    lw->mixedLabel.btm_label_width = 0;

  lw->mixedLabel.desired_width = MaximumWidth(lw->mixedLabel.top_label_width,
	  lw->mixedLabel.n_pixmap_width, lw->mixedLabel.btm_label_width);

  lw->mixedLabel.desired_height = lw->mixedLabel.top_label_height + 
          lw->mixedLabel.btm_label_height + 
	  lw->mixedLabel.n_pixmap_height +
          2*lw->mixedLabel.space;

}

/****************************************************************
 *      static void SetXYPosition( )                            *
 * Description:                                                 *
 *     Calculate position of top_label, botoom_label and pixmap *
 ***************************************************************/
static void SetXYPosition(lw)
     XcodaMixedLabelWidget lw;
{
  if(lw->mixedLabel.set){
    lw->mixedLabel.desired_width = MaximumWidth(lw->mixedLabel.top_label_width,
	lw->mixedLabel.h_pixmap_width, lw->mixedLabel.btm_label_width);
    lw->mixedLabel.desired_height = lw->mixedLabel.top_label_height + 
        lw->mixedLabel.btm_label_height + 
	lw->mixedLabel.h_pixmap_height +
        2*lw->mixedLabel.space;
  }
  else{
    lw->mixedLabel.desired_width = MaximumWidth(lw->mixedLabel.top_label_width,
	lw->mixedLabel.n_pixmap_width, lw->mixedLabel.btm_label_width);
    lw->mixedLabel.desired_height = lw->mixedLabel.top_label_height + 
        lw->mixedLabel.btm_label_height + 
	lw->mixedLabel.n_pixmap_height +
        2*lw->mixedLabel.space;
  }

  if (lw->mixedLabel.top_label_width < lw->mixedLabel.desired_width){
    lw->mixedLabel.top_label_x = 0 +
      (lw->mixedLabel.desired_width - lw->mixedLabel.top_label_width)/2.0;
  }
  else
    lw->mixedLabel.top_label_x = 0;
  lw->mixedLabel.top_label_y = 0 + lw->mixedLabel.font->max_bounds.ascent;

  if(lw->mixedLabel.set){ /* highLight Pixmap */
    if (lw->mixedLabel.h_pixmap_width < lw->mixedLabel.desired_width){
      lw->mixedLabel.h_pixmap_x = 0 +
	(lw->mixedLabel.desired_width - lw->mixedLabel.h_pixmap_width)/2.0;
    }
    else
      lw->mixedLabel.h_pixmap_x = 0;
    lw->mixedLabel.h_pixmap_y = lw->mixedLabel.top_label_height
      + lw->mixedLabel.space;
  }
  else{
    if (lw->mixedLabel.n_pixmap_width < lw->mixedLabel.desired_width){
      lw->mixedLabel.n_pixmap_x = 0 +
	(lw->mixedLabel.desired_width - lw->mixedLabel.n_pixmap_width)/2.0;
    }
    else
      lw->mixedLabel.n_pixmap_x = 0;
    lw->mixedLabel.n_pixmap_y = lw->mixedLabel.top_label_height
      + lw->mixedLabel.space;
  }

  if (lw->mixedLabel.btm_label_width < lw->mixedLabel.desired_width){
    lw->mixedLabel.btm_label_x = 0 +
      (lw->mixedLabel.desired_width - lw->mixedLabel.btm_label_width)/2.0;
  }
  else
    lw->mixedLabel.btm_label_x = 0;

  if(lw->mixedLabel.set)
    lw->mixedLabel.btm_label_y = lw->mixedLabel.top_label_height
      + lw->mixedLabel.h_pixmap_height 
      + 2*lw->mixedLabel.space
      + lw->mixedLabel.font->max_bounds.ascent;
  else
    lw->mixedLabel.btm_label_y = lw->mixedLabel.top_label_height
      + lw->mixedLabel.n_pixmap_height 
      + 2*lw->mixedLabel.space
      + lw->mixedLabel.font->max_bounds.ascent;
}


static void GetNormalGC(lw)
     XcodaMixedLabelWidget lw;
{
  XGCValues values;

  values.foreground = lw->mixedLabel.foreground;
  values.background = lw->core.background_pixel;
  values.font = lw->mixedLabel.font->fid;

  lw->mixedLabel.gc = XtGetGC(
	 (Widget)lw,
	 (unsigned) GCForeground| GCBackground |GCFont,
         &values);
}

/****************************************************************
 *      static void Initialize ( )                              *
 * Description:                                                 *
 *     Initialize all data structure elements                   *
 ***************************************************************/
static void Initialize(request, new, args, num_args)
     Widget request, new;
     ArgList args;
     Cardinal *num_args;
{
  XcodaMixedLabelWidget lw = (XcodaMixedLabelWidget) new;

/* if no top label and bottom label specified , use empty label */
  if(lw->mixedLabel.top_label == NULL)
    lw->mixedLabel.top_label = XtNewString("   ");
  else
    lw->mixedLabel.top_label = XtNewString(lw->mixedLabel.top_label);

  if(lw->mixedLabel.bottom_label == NULL)
    lw->mixedLabel.bottom_label = XtNewString("   ");
  else
    lw->mixedLabel.bottom_label = XtNewString(lw->mixedLabel.bottom_label);

  GetNormalGC(lw);
  lw->mixedLabel.inverted_gc = GetInvertedGC(lw);
  lw->mixedLabel.current_gc = lw->mixedLabel.gc;
  lw->mixedLabel.inverted = False;
  
  SetWidthAndHeight(lw);

  lw->core.height = lw->mixedLabel.desired_height;
  lw->core.width = lw->mixedLabel.desired_width;

/*  SetXYPosition(lw);*/
}

/***************************************************************
 *          static void Redisplay( )                            *
 * Description:                                                 *
 *     Repaint the widget window upon exposure event            *
 ***************************************************************/
static void Redisplay(w, event, region)
     Widget w;
     XEvent *event;
     Region region;
{
  XcodaMixedLabelWidget lw = (XcodaMixedLabelWidget) w;
  GC gc;

  gc = lw->mixedLabel.current_gc;

  SetXYPosition(lw);
/* if inverted fill widget with foreground color */
  if(lw->mixedLabel.inverted) {
    XFillRectangle(XtDisplay(w),XtWindow(w),lw->mixedLabel.gc,
		   0,0,lw->core.width,lw->core.height);
  } else {

    XFillRectangle(XtDisplay(w),XtWindow(w),lw->mixedLabel.inverted_gc,
		   0,0,lw->core.width,lw->core.height);
  }

  XDrawString(XtDisplay(w), XtWindow(w), gc, lw->mixedLabel.top_label_x,
	      lw->mixedLabel.top_label_y, lw->mixedLabel.top_label,
	      strlen(lw->mixedLabel.top_label));

  if(lw->mixedLabel.set){ /* highlight pixmap */
    if(lw->mixedLabel.depth == 1){
      XCopyPlane(XtDisplay(w), lw->mixedLabel.h_pixmap, XtWindow(w), gc, 0, 0, 
		 lw->mixedLabel.h_pixmap_width, lw->mixedLabel.h_pixmap_height,
		 lw->mixedLabel.h_pixmap_x, lw->mixedLabel.h_pixmap_y,
		 1L);
    }
    else{
      XCopyArea(XtDisplay(w), lw->mixedLabel.h_pixmap, XtWindow(w), gc, 0, 0, 
		 lw->mixedLabel.h_pixmap_width, lw->mixedLabel.h_pixmap_height,
		 lw->mixedLabel.h_pixmap_x, lw->mixedLabel.h_pixmap_y);
    }
  }
  else{
    if(lw->mixedLabel.depth == 1){
      XCopyPlane(XtDisplay(w), lw->mixedLabel.n_pixmap, XtWindow(w), gc, 0, 0, 
		 lw->mixedLabel.n_pixmap_width, lw->mixedLabel.n_pixmap_height,
		 lw->mixedLabel.n_pixmap_x, lw->mixedLabel.n_pixmap_y,
		 1L);
    }
    else{
      XCopyArea(XtDisplay(w), lw->mixedLabel.n_pixmap, XtWindow(w), gc, 0, 0, 
		 lw->mixedLabel.n_pixmap_width, lw->mixedLabel.n_pixmap_height,
		 lw->mixedLabel.n_pixmap_x, lw->mixedLabel.n_pixmap_y);
    }
  }
		       
  XDrawString(XtDisplay(w), XtWindow(w), gc, lw->mixedLabel.btm_label_x,
	      lw->mixedLabel.btm_label_y, lw->mixedLabel.bottom_label,
	      strlen(lw->mixedLabel.bottom_label));  

}  

/***************************************************************************
 *       static Boolean SetValues()                                        *
 * Description:                                                            *
 *    whether widget needs redisplay when you set new resources            *
 **************************************************************************/
static Boolean SetValues(current, request, new, args, num_args)
     Widget current, request, new;
     ArgList args;
     Cardinal num_args;
{
  XcodaMixedLabelWidget oldlw = (XcodaMixedLabelWidget)current;
  XcodaMixedLabelWidget reqlw = (XcodaMixedLabelWidget)request;
  XcodaMixedLabelWidget newlw = (XcodaMixedLabelWidget)new;
  Boolean redisplay = False;
  Boolean was_resized = False;

#define NE(field) (oldlw->field != newlw->field)

/* if toplabel has been set to NULL, change to "   " */
  if(newlw->mixedLabel.top_label == NULL){
    newlw->mixedLabel.top_label = XtNewString("   ");
  }
  if(newlw->mixedLabel.bottom_label == NULL){
    newlw->mixedLabel.bottom_label = XtNewString("   ");
  }
  
  if(NE(mixedLabel.top_label)){
    XtFree((char *)oldlw->mixedLabel.top_label);
    newlw->mixedLabel.top_label = XtNewString(newlw->mixedLabel.top_label);
    was_resized = True;
  }
  if(NE(mixedLabel.bottom_label)){
    XtFree((char *)oldlw->mixedLabel.bottom_label);
    newlw->mixedLabel.bottom_label = XtNewString(newlw->mixedLabel.bottom_label);
    was_resized = True;
  }
  
  if(NE(mixedLabel.set))was_resized = True;

  if(NE(mixedLabel.n_pixmap) && !newlw->mixedLabel.set)was_resized = True;

  if(NE(mixedLabel.h_pixmap) && newlw->mixedLabel.set)was_resized = True;

  if(NE(mixedLabel.font))was_resized = True;

  SetWidthAndHeight(newlw);
  SetXYPosition(newlw);

  if(was_resized && newlw->mixedLabel.resize){
    newlw->core.width = newlw->mixedLabel.desired_width;
    newlw->core.height = newlw->mixedLabel.desired_height;
  }

  if(NE(mixedLabel.foreground) || NE(core.background_pixel)
     || NE(mixedLabel.font->fid)){
    XtReleaseGC(new, oldlw->mixedLabel.gc);
    GetNormalGC(newlw);
    redisplay = True;
  }
  
  return was_resized || redisplay;
#undef NE
}

/***********************************************************************
 *          static void Destroy()                                      *
 * Description:                                                        *
 *     release all information and memory                              *
 **********************************************************************/
static void Destroy(w)
     Widget w;
{
  XcodaMixedLabelWidget lw = (XcodaMixedLabelWidget)w;

  XtFree(lw->mixedLabel.top_label);
  XtFree(lw->mixedLabel.bottom_label);
  XtReleaseGC(w, lw->mixedLabel.gc);
}

/*********************************************************************
 *         static XtGeometryResult QueryGeometry()                   *
 * Description:                                                      *
 *     Query geometry to see whether we have resize or not           *
 ********************************************************************/
static XtGeometryResult QueryGeometry(w,intended, preferred)
     Widget w;
     XtWidgetGeometry *intended, *preferred;
{
  register XcodaMixedLabelWidget lw = (XcodaMixedLabelWidget)w;

  preferred->request_mode = CWWidth | CWHeight;
  preferred->width = lw->mixedLabel.desired_width;
  preferred->height = lw->mixedLabel.desired_height;
  
  if( (intended->request_mode & (CWWidth | CWHeight)
     == (CWWidth | CWHeight)) && intended->width == preferred->width &
     intended->height == preferred->height)
    return XtGeometryYes;
  else if (preferred->width == w->core.width &&
	   preferred->height == w->core.height)
    return XtGeometryNo;
  else
    return XtGeometryAlmost;
}

/********************************************************************
 *            static void Resize()                                  *
 * Description:                                                     *
 *     Resize handler                                               *
 *******************************************************************/
static void Resize(w)
     Widget w;
{
  XcodaMixedLabelWidget lw = (XcodaMixedLabelWidget)w;

  if(XtIsRealized(w)){
    XClearWindow(XtDisplay(w), XtWindow(w));
    (*(XtClass(w)->core_class.expose))(w,
       (XEvent *)NULL, (Region) NULL);
  }
}


/*******************************************************************
 *       some supporting routings                                  *
 ******************************************************************/
Boolean XcodaMixedLabelIsHighLighted(w)
     Widget w;
{
  XcodaMixedLabelWidget lw = (XcodaMixedLabelWidget)w;
  Boolean ret;

  ret = lw->mixedLabel.set;

  return ret;
}
  
void XcodaMixedLabelSetHighLightOn(w)
     Widget w;
{
  Arg args[1];

  if (XcodaMixedLabelIsHighLighted(w))
    return;
  else{
    XtSetArg(args[0], XcodaNsetHighLight, True);
    XtSetValues(w, args, 1);
  }
}
    
void XcodaMixedLabelSetHighLightOff(w)
     Widget w;
{
  Arg args[1];

  if (!XcodaMixedLabelIsHighLighted(w))
    return;
  else{
    XtSetArg(args[0], XcodaNsetHighLight, False);
    XtSetValues(w, args, 1);
  }
}
  
