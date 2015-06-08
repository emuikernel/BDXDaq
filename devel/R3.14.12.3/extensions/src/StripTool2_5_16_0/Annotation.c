/*************************************************************************\
* Copyright (c) 2002 The University of Chicago, as Operator of Argonne
* National Laboratory.
* Copyright (c) 2002 The Regents of the University of California, as
* Operator of Los Alamos National Laboratory.
* This file is distributed subject to a Software License Agreement found
* in the file LICENSE that is included with this distribution.
\*************************************************************************/
/*****************************************************************************
 *
 *   doDragging code taken from medm/medm/utils.c  and modified for StripTool
 *
 *****************************************************************************
*/

#define DEBUG_EVENTS 0
#define GRAB_WINDOW None
#define COLOR_NPIXELS_SIDE 5
#define COLOR_NPIXELS_TOPBOT 1

#include <stdlib.h>
#include <stdio.h>

#include <Xm/MessageB.h>
#include <Xm/Form.h>
#include <Xm/Label.h>
#include <Xm/Text.h>
#include <X11/cursorfont.h>

#include <ellLib.h>

#include "jlAxis.h"
#include "StripGraph.h"
#include "StripMisc.h"

/* ====== Annotation stuff ====== */

typedef struct _STRectangle
{
  double        x, y;             /* in time2dbl(time),value */
  int           rasterX, rasterY;    /* raster */
  Dimension     width, height;    /* raster */
}
STRectangle;

typedef struct _Annotation
{
  ELLNODE         node;
  char*           text;
  StripCurveInfo  *curve;
  STRectangle       box;
  int             numLines;
}
Annotation;

typedef struct _AnnotateDialog
{
  Widget        popupWidget;
  Widget        baseWidget;
  Widget        textWidget;
  XtPointer     newAnnotation;
}
AnnotateDialog;

typedef struct _AnnotationInfo
{
  ELLLIST        *annotationList;
  Annotation     *selectedAnnotation;
  Annotation     *touchedAnnotation;
  int             isModified;
  XFontStruct    *font_info;
  Dimension       boxOffset;
  Cursor          dragCursor;
  Cursor          crosshairCursor;
  AnnotateDialog *ad;
  XtAppContext    app;
  Widget          canvas;
  Widget          shell;
  StripGraph      graph;
  Display *       display;
  StripCurveInfo  *curves; 
}
AnnotationInfo;

char *helpText = 
"StripTool Annotation Help\n\n"
"Create an annotation:\n"
"     Choose the \"Annotate selected curve\" menu item.\n\n"
"Select an annotation:\n"
"     Move the cursor over the annotation then press the left mouse button.\n"
"     A dashed line border is displayed around a selected annotation.\n\n"
"Edit an annotation:\n"
"     Select the annotation then choose the \"Edit selected annotation\"\n"
"     menu item.\n\n"
"Move an annotation:\n"
"     Move the cursor over the annotation then press and hold the middle\n"
"     mouse button.\n\n"
"Delete an annotation:\n"
"     Select the annotation then choose the \"Delete selected annotation\"\n"
"     menu item.\n\n";

void AnnotateDialog_cb  (Widget w, XtPointer client, XtPointer call);
void Annotation_transformRasterValues(AnnotationInfo *ai, int width, int height);
void Annotation_delete(AnnotationInfo *ai, Annotation *annotation);
void Annotation_messagePopup(Widget parent, char * msg);

#define ANNOTATIONINFO
#include <Annotation.h>


/*
 *  draw annotations
 */
void  Annotation_draw(Display *display, Window window, GC gc,
                  XRectangle wind_rect, AnnotationInfo *ai,
                  struct timeval *pplotted_t0, struct timeval *pplotted_t1,
                  StripCurveInfo *curves[])
{
  Annotation       *annotation;
  Annotation       *next;
  int               i;
  jlaTransformInfo *transform;
  double            rasterY;
  int               rasterTextX,rasterTextY;
  double            tempY;
  char             *ptr;
  char             *newptr;
  intptr_t          len,diff;
  double            dl,db;
  struct timeval    dll;
  int               fontHeight;
  XGCValues         gcValues;
  int               linewidth = 1;
  StripDataSource   sds;
  struct timeval    sg_t0,sg_t1,sds_t0;
  double            mintime;

  if (!ai) return; 
  if (!ai->annotationList) return; 
  if (!ai->annotationList->count) return; 

  XGetGCValues(display, gc, GCForeground|GCBackground, &gcValues);

  annotation = (Annotation*)ellFirst(ai->annotationList);
  if (!annotation) return;

  while (annotation) {
    transform = NULL;
    next = (Annotation*)ellNext((ELLNODE*)annotation);

    /* Delete the annotation if it's location is out of data range and plot range */
    StripGraph_getattr (ai->graph, STRIPGRAPH_BEGIN_TIME, &sg_t0, 0);
    StripGraph_getattr (ai->graph, STRIPGRAPH_END_TIME, &sg_t1, 0);
    StripGraph_getattr (ai->graph, STRIPGRAPH_DATA_SOURCE, &sds, 0);
    StripDataSource_getattr (sds, SDS_BEGIN_TIME, &sds_t0, 0);
    /* delete if box.x is out of plot range by at least one plot width */
    mintime = time2dbl(&sg_t0) - (time2dbl(&sg_t1) - time2dbl(&sg_t0));
    if (annotation->box.x < mintime && annotation->box.x < time2dbl(&sds_t0)) {
      Annotation_delete(ai, annotation);
      annotation = next;
      continue;
    }

    transform=StripGraph_getTransform(ai->graph, annotation->curve);
    if (!transform) {

      /* Delete the annotation if it's curve was deleted */
      if (annotation->curve) {
        Annotation_delete(ai, annotation);
        annotation = next;
        continue;
      }

      /* annotation has no associated curve */
      /*  locations are rasterized */
      dl = subtract_times(&dll,pplotted_t0,pplotted_t1);
      db = dl/(wind_rect.width - 1);
      annotation->box.rasterY = (int)annotation->box.y;
      annotation->box.rasterX = (int)((annotation->box.x - time2dbl(pplotted_t0)) / db);

    } else {
  
      if (annotation->curve->details->plotstat != STRIPCURVE_PLOTTED) {
        annotation = next;
        continue;
      }

      dl = subtract_times(&dll,pplotted_t0,pplotted_t1);
      db = dl/(wind_rect.width - 1);
  
      /* transform the box location from curve to raster */
      tempY = annotation->box.y;
      jlaTransformValuesRasterized (transform, &tempY, &rasterY, 1);
      annotation->box.rasterY = (int)(wind_rect.height - 1 - rasterY);
      annotation->box.rasterX = (int)((annotation->box.x - time2dbl(pplotted_t0)) / db);
    }

    /* draw a white background rectangle for the annnotation */
    XSetForeground(display, gc, gcValues.background);
    XFillRectangle(display, window, gc, annotation->box.rasterX, annotation->box.rasterY,
          annotation->box.width, annotation->box.height);
    XSetForeground(display, gc, gcValues.foreground);

    /* draw the annotation text lines */
    ptr = annotation->text;
    diff = 0;
    fontHeight = ai->font_info->max_bounds.ascent + ai->font_info->max_bounds.descent;
    rasterTextX = annotation->box.rasterX + COLOR_NPIXELS_SIDE + ai->boxOffset;
    rasterTextY = annotation->box.rasterY + COLOR_NPIXELS_TOPBOT + ai->boxOffset 
                + ai->font_info->max_bounds.ascent;

    XSetForeground(display, gc, gcValues.foreground);

    /* load the font into the graphics context */
    XSetFont(display, gc, ai->font_info->fid);

    for (i=1 ;i<=annotation->numLines;i++) {

      newptr=strchr(ptr,'\n');
      if (!newptr) {
        len = strlen(ptr);
      } else {
        len = (int)(newptr - ptr);
        newptr++;
      }
      XDrawString(display, window, gc, rasterTextX , rasterTextY + diff, ptr,len);
      diff += fontHeight;
      ptr = newptr;
    }
  
    /* draw a color border rectangle around the annnotation */
    XSetLineAttributes (display, gc, linewidth, LineSolid, CapButt, JoinMiter);
    if (annotation->curve) {
      XSetForeground(display, gc, annotation->curve->details->color->xcolor.pixel);
    }
    for (i=0 ;i<COLOR_NPIXELS_SIDE;i++) {
      XDrawRectangle(display, window, gc, annotation->box.rasterX+i+2, 
            annotation->box.rasterY+2,
            annotation->box.width - COLOR_NPIXELS_SIDE - 2, 
            annotation->box.height - 2*COLOR_NPIXELS_TOPBOT - 2);
    }

    /* draw a dashed line rectangle around the selected annnotation */
    XSetForeground(display, gc, gcValues.foreground);
    if (annotation == ai->selectedAnnotation) {
      XSetLineAttributes (display, gc, linewidth, LineOnOffDash, CapButt, JoinMiter);
      XDrawRectangle(display, window, gc, annotation->box.rasterX, 
            annotation->box.rasterY,
            annotation->box.width +1, annotation->box.height);
    }

    annotation = next;
  }
}
 

void Annotation_messagePopup(Widget parent,char *msg)
{
    static Widget popup;
    XmString text,title;

    if (!popup) {
        popup = XmCreateMessageDialog (parent, "message", NULL, 0);
        XtUnmanageChild(XmMessageBoxGetChild(popup,XmDIALOG_CANCEL_BUTTON));
        XtUnmanageChild(XmMessageBoxGetChild(popup,XmDIALOG_HELP_BUTTON));
    }

    text = XmStringCreateLtoR (msg, XmFONTLIST_DEFAULT_TAG);
    title = XmStringCreateLtoR ("StripTool Message Dialog", XmFONTLIST_DEFAULT_TAG);
    XtVaSetValues (popup, XmNmessageString,text, NULL);
    XtVaSetValues(popup,
      XmNdialogTitle, title,
      XmNmessageString, text,
      NULL);
    XmStringFree (text);
    XmStringFree (title);

    XtManageChild (popup);
    XtPopup (XtParent (popup), XtGrabNone);
}


void Annotation_deleteSelected(AnnotationInfo *ai)
{
  Annotation * annotation;

  annotation = ai->selectedAnnotation;
  ai->selectedAnnotation = NULL;
  Annotation_delete(ai,annotation);

  /* refresh the graph */
  /* time to 0 and calling dispatch(). */
  StripGraph_draw (ai->graph, SGCOMPMASK_DATA, (Region *)0);
}


void Annotation_deleteAll(AnnotationInfo *ai)
{
  Annotation * annotation;

  while ( (annotation = (Annotation*)ellFirst(ai->annotationList)) ) {
    Annotation_delete(ai, annotation);
  }
}


void Annotation_delete(AnnotationInfo *ai, Annotation *annotation)
{

  if (annotation == NULL) {
    if (ai->annotationList->count == 0) {
      Annotation_messagePopup(ai->canvas, "There are no annotations.");
    } else {
      Annotation_messagePopup(ai->canvas, "Please select an annotation first.");
    }
  } else {
    ellDelete(ai->annotationList,(ELLNODE*)annotation);
    if (annotation->text) XtFree(annotation->text);
    free(annotation);
  }
}


static Annotation * findTouchedAnnotation(XButtonEvent *event, AnnotationInfo *ai)
{
  Position     x0, y0;
  Annotation * annotation;

  if (!ai || !ai->annotationList || !ai->annotationList->count ) return 0;

  /* Get pointer coordinates */
  x0 = event->x;
  y0 = event->y;

  /* find first annotation touched by pointer */
  annotation = (Annotation*)ellFirst(ai->annotationList);
  while (annotation) {
    if ( x0 >= annotation->box.rasterX  &&
         x0 <= (annotation->box.rasterX + annotation->box.width) &&
         y0 >= annotation->box.rasterY  &&
         y0 <= (annotation->box.rasterY + annotation->box.height) ) {
      break;
    }
    annotation = (Annotation*)ellNext((ELLNODE*)annotation);
  }
  return annotation;
}


int Annotation_select(XButtonEvent *event, AnnotationInfo *ai)
{
  Annotation * touchedAnnotation;

  touchedAnnotation = findTouchedAnnotation(event,ai);
  if (touchedAnnotation == NULL || ((event->button == Button1) &&
      touchedAnnotation == ai->selectedAnnotation) ) {
    ai->selectedAnnotation  = NULL;
    return 0;
  } else {
    ai->selectedAnnotation  = touchedAnnotation;
    return 1;
  }
}

static AnnotateDialog *AnnotateDialog_build (Widget parent, void *ptr)
{
  AnnotateDialog *ad;

  ad = (AnnotateDialog *)malloc (sizeof (AnnotateDialog));
  if (!ad) return 0;
  
#ifndef WIN32
  ad->popupWidget = XmCreateTemplateDialog (parent, "AnnotateDialog", 0, 0);
#else
  ad->popupWidget = XmCreateMessageDialog (parent, "AnnotateDialog", 0, 0);
#endif
  XtAddCallback (ad->popupWidget, XmNokCallback, AnnotateDialog_cb, 
     (XtPointer)ptr);
  XtAddCallback (ad->popupWidget, XmNcancelCallback, AnnotateDialog_cb, 0);
  XtAddCallback (ad->popupWidget, XmNhelpCallback, AnnotateDialog_cb, 0);

  ad->baseWidget = XtVaCreateManagedWidget
    ("baseForm", xmFormWidgetClass, ad->popupWidget, NULL);

  XtVaCreateManagedWidget ("textLabel", xmLabelWidgetClass, ad->baseWidget, NULL);

  ad->textWidget = XtVaCreateManagedWidget ("editText", xmTextWidgetClass,ad->baseWidget, NULL);

  return ad;
}


void AnnotateDialog_popup (AnnotationInfo *ai, XtPointer newAnnotation)
{
  Annotation           *annotation;
  AnnotateDialog       *ad = ai->ad;
  StripCurveInfo       *curve;

  if (newAnnotation) {

    /* There must be a selected curve */
    StripGraph_getattr (ai->graph, STRIPGRAPH_SELECTED_CURVE, &curve, 0);
    if (!curve) {
      Annotation_messagePopup(ai->canvas, "There is no selected curve.");
      return;
    }

    /* set the text string appropriately */
    XmTextSetString(ad->textWidget,"");

  } else {
    /* set the text string */
    annotation = ai->selectedAnnotation;
    if (annotation && annotation->text) {
      XmTextSetString(ad->textWidget,annotation->text);
      XmTextSetInsertionPosition (ad->textWidget, strlen(annotation->text));
    } else {
      if (ai->annotationList->count == 0) {
        Annotation_messagePopup(ai->canvas, "There are no annotations.");
      } else {
        Annotation_messagePopup(ai->canvas, "Please select an annotation first.");
      }
      return;
    }
  }

  XtVaSetValues
    (ad->popupWidget,
	XmNuserData,       (XtPointer)newAnnotation,
	NULL);

  /* pop it up! */
  XtManageChild (ad->popupWidget);

  XmProcessTraversal(ad->textWidget,XmTRAVERSE_CURRENT);

}

void AnnotateDialog_cb  (Widget w, XtPointer client, XtPointer call)
{
  AnnotationInfo       *ai = (AnnotationInfo *)client;
  XmAnyCallbackStruct  *cbs = (XmAnyCallbackStruct *)call;
  StripCurveInfo       *curve;
  Annotation           *annotation;
  char                 *str,*ptr;
  Position              x0, y0;
  Dimension             width;
  Dimension             height;
  Dimension             boxWidth, boxHeight;
  int                   numLines;
  intptr_t              len;
  int                   textWidth;
  char*                 newptr;
  int                   fontHeight;
  XtPointer             newAnnotation;


  if (cbs->reason == XmCR_OK)
  {
    /* get newAnnotation flag */
    XtVaGetValues (w, XmNuserData, &newAnnotation, NULL);

    /* get annotation text */
    str = XmTextGetString (ai->ad->textWidget);
    if (str == 0) return;
    if (!strlen(str)) return;

    /* determine width of text and number of text lines */
    numLines=1;
    textWidth=0;
    ptr = str;
    while ( TRUE) {  
        newptr=strchr(ptr,'\n');
        if (!newptr) newptr=ptr+strlen(ptr);
        else newptr++;
        len = (intptr_t)(newptr - ptr);
        width = XTextWidth( ai->font_info, ptr, len);
        if ( width > textWidth ) textWidth = width;
        ptr = newptr;
        if ( ptr >= str+strlen(str) ) break;
        numLines++;
    }


    /* determine selected box dimensions */
    fontHeight = ai->font_info->max_bounds.ascent + ai->font_info->max_bounds.descent;
    boxWidth = textWidth + 2*ai->boxOffset + COLOR_NPIXELS_SIDE +2;
    boxHeight = fontHeight*numLines + 2*ai->boxOffset + COLOR_NPIXELS_TOPBOT +2;

    if (!newAnnotation) {

        annotation = ai->selectedAnnotation;
        if (!annotation) return;

        /* Modify existing annotation */

        if (annotation->text) XtFree(annotation->text);
        annotation->text = str;
        annotation->box.width = boxWidth;
        annotation->box.height = boxHeight;
        annotation->numLines = numLines;

        ai->isModified = TRUE;

    } else {

         /* New annotation */

      /* determine x,y of canvas center */
      XtVaGetValues(ai->canvas,
                  XmNwidth,&width,
                  XmNheight,&height,
                  XmNx,&x0,
                  XmNy,&y0,
                  NULL);
      x0 = x0 +(int)(.5*width);
      y0 = y0 +(int)(.5*height);

      /*****************************/
      /*  Need to add arrow code   */
      /*****************************/

      /* Allocate new Annotation */
      annotation = (Annotation*)calloc(1,sizeof(Annotation));
      if (!annotation) return;

      /* determine new annotation box coordinates */
      annotation->box.rasterX = (int)(x0 - boxWidth/2);
      annotation->box.rasterY = (int)(y0 - boxHeight/2);

      StripGraph_getattr (ai->graph, STRIPGRAPH_SELECTED_CURVE, &curve, 0);
      annotation->curve = curve;

      /* Set Annotation fields */
      annotation->text = str;
      annotation->numLines = numLines;
      annotation->box.width = boxWidth;
      annotation->box.height = boxHeight;

      /* Set selected Annotation to new Annotation*/
      ai->selectedAnnotation = annotation;
      ai->isModified = TRUE;

      /* need to calculate curve coordinates */
      Annotation_transformRasterValues(ai,width,height);

      /* Add new Annotation to Annotation list */
      ellAdd(ai->annotationList,(void*)annotation);

    }

    /* refresh the graph */
    /* time to 0 and calling dispatch(). */
    StripGraph_draw (ai->graph, SGCOMPMASK_DATA, (Region *)0);

  }
  if (cbs->reason == XmCR_HELP)
  {
    Annotation_messagePopup(w, helpText);
  }
  return;
}



AnnotationInfo *Annotation_init ( XtAppContext app, Widget canvas,
  Widget shell, Display *display, void* graph, StripCurveInfo *curves) 
{
    AnnotationInfo *ai;
    XFontStruct    *font_info;

    ai = (AnnotationInfo*)calloc(1,sizeof(AnnotationInfo));
    ai->annotationList = (ELLLIST*)calloc(1,sizeof(ELLLIST));
    ellInit(ai->annotationList);

    ai->app = app;
    ai->shell = shell;
    ai->canvas = canvas;
    ai->curves = curves;
    ai->display = display;
    ai->graph = graph;

    ai->boxOffset = 3;
    ai->ad = AnnotateDialog_build (shell,ai);

    StripGraph_setattr
      (graph, STRIPGRAPH_ANNOTATION_INFO, ai, 0);

    /* load the font into the Xserver */
    font_info = XLoadQueryFont (display, "*helvetica-bold-r-normal--12*");
    if( !font_info ) {
        printf("Error: couldn't load font\n" );
        return NULL;
    }
    ai->font_info = font_info;

    ai->dragCursor = XCreateFontCursor(display,XC_fleur);

    return ai;
}


/*
 * do dragging  of a rectangle 
 *	RETURNS: boolean indicating whether drag ended in the window
 *	(and hence was valid)
 */

static Boolean doDragging(XtAppContext appContext, Widget w, 
  Cursor dragCursor,  XRectangle *po, 
  Position initialX, Position initialY,      /* initial cursor location */
  Position *finalX, Position *finalY)      /* final cursor location */
{
    Display   *display;
    Window     window,rootWindow;
    XEvent     event;
    Dimension  daWidth, daHeight;   /* display width and height */
    Boolean    returnVal = True;
    int        x, y, xOffset, yOffset;
    int        minX, maxX, minY, maxY, groupWidth, groupHeight,
               groupDeltaX0, groupDeltaY0, groupDeltaX1, groupDeltaY1;
    int        nelements;
    GC         xorGC;
    
    /* If no current widget, simply return */
    if(!w) return(False);
    display = XtDisplay(w);
    if(!display)  return(False);
    window = XtWindow(w);
    if(!window)  return(False);

    /* If no rectangle, return */
    if(!po) return False;

    rootWindow=XRootWindowOfScreen(XtScreen(w));
    xorGC = XCreateGC(display,rootWindow,0,NULL);
    /* Eliminate events that we do not handle */
    XSetGraphicsExposures(display,xorGC,False);
    XSetSubwindowMode(display,xorGC,IncludeInferiors);
    XSetFunction(display,xorGC,GXxor);
    XSetForeground(display,xorGC,0);

    XtVaGetValues(w,
        XmNwidth,&daWidth,
        XmNheight,&daHeight,
        NULL);


    xOffset = 0;
    yOffset = 0;

  /* Have all interesting events go to window
   *  Note: ButtonPress and ButtonRelease do not need to be specified
   * KE: If include KeyPressMask get:
   *   BadValue (integer parameter out of range for operation)
   *   but it seems to work OK without it ??? */
    XGrabPointer(display, window, False,
      (unsigned int)(ButtonMotionMask|ButtonReleaseMask),
      GrabModeAsync,GrabModeAsync,GRAB_WINDOW,dragCursor,CurrentTime);

    /* Grab the server to ensure that XORing will be okay */
    XGrabServer(display);

    /* Draw first rectangle and get extents */
    nelements =1;
    XDrawRectangle(display,window, xorGC,
      po->x + xOffset, po->y + yOffset, po->width , po->height);

    minX = po->x;
    maxX = po->x + (int)po->width;
    minY = po->y;
    maxY = po->y + (int)po->height;
#if DEBUG_EVENTS
    print(" minX=%d maxX=%d minY=%d maxY=%d\n\n",
      minX,maxX,minY,maxY);
    print(" po->x=%d po->width=%d po->y=%d po->height =%d\n",
      po->x,po->width,po->y,po->height);
#endif

    /* Check if there was anything selected besides the display */
    if(!nelements) {
        XFreeGC(display,xorGC);
	XUngrabServer(display);
	XUngrabPointer(display,CurrentTime);
	XBell(display,50);
	XFlush(display);
	return False;
    }
    groupWidth = maxX - minX;
    groupHeight = maxY - minY;
  /* How many pixels is the cursor position from the left edge of all objects */
    groupDeltaX0 = initialX - minX;
  /* How many pixels is the cursor position from the top edge of all objects */
    groupDeltaY0 = initialY - minY;
  /* How many pixels is the cursor position from the right edge of all objects */
    groupDeltaX1 = groupWidth - groupDeltaX0;
  /* How many pixels is the cursor position from the bottom edge of all objects */
    groupDeltaY1 = groupHeight - groupDeltaY0;
#if DEBUG_EVENTS > 1
    print(" groupWidth=%d groupHeight=%d groupDeltaX0=%d\n"
      " groupDeltaY0=%d groupDeltaX1=%d groupDeltaY1=%d\n\n",
      groupWidth,groupHeight,groupDeltaX0,
      groupDeltaY0,groupDeltaX1,groupDeltaY1);
#endif
#if DEBUG_EVENTS > 2
    XFreeGC(display,xorGC);
    XUngrabServer(display);
    XUngrabPointer(display,CurrentTime);
    XFlush(display);
    return False;
#endif

/* Loop until the button is released */
    while(TRUE) {
	XtAppNextEvent(appContext, &event);
	switch (event.type) {
	case KeyPress: {
	    XKeyEvent *kevent = (XKeyEvent *)&event;
	    Modifiers modifiers;
	    KeySym keysym;

	    XtTranslateKeycode(display, kevent->keycode, (Modifiers)NULL,
	      &modifiers, &keysym);
#if DEBUG_EVENTS > 1
	    print("doDragging: Type: %d Keysym: %x (osfXK_Cancel=%x) "
	      "Shift: %d Ctrl: %d\n"
	      "  [KeyPress=%d, KeyRelease=%d ButtonPress=%d, ButtonRelease=%d]\n",
	      kevent->type,keysym,osfXK_Cancel,
	      kevent->state&ShiftMask,kevent->state&ControlMask,
	      KeyPress,KeyRelease,ButtonPress,ButtonRelease);     /* In X.h */
#endif
	    if(keysym != osfXK_Cancel) break;
	    returnVal = False;
	  /* Fall through */
	}

	case ButtonRelease:
#if DEBUG_EVENTS > 1
	    print("  ButtonRelease: x=%d y=%d\n",event.xbutton.x,event.xbutton.y);
	    fflush(stdout);
#endif
	  /* Undraw old ones */
	    XDrawRectangle(display, window, xorGC,
		      po->x + xOffset, po->y + yOffset, po->width , po->height);
            XFreeGC(display,xorGC);
	    XUngrabServer(display);
	    XUngrabPointer(display,CurrentTime);
	    XFlush(display);
	    *finalX = initialX + xOffset;
	    *finalY = initialY + yOffset;
#if DEBUG_EVENTS > 1
print("  initialX=%d initialY=%d\n",initialX,initialY);
print("  xOffset=%d yOffset=%d\n",xOffset,yOffset);
print("  finalX=%d finalY=%d\n",*finalX,*finalY);
#endif

	    return (returnVal);	/* return from while(TRUE) */

	case MotionNotify:
#if DEBUG_EVENTS > 4
	    print("  MotionNotify: x=%d y=%d\n",event.xbutton.x,event.xbutton.y);
	    fflush(stdout);
#endif
	  /* Undraw old ones */
	    XDrawRectangle(display, window, xorGC,
		      po->x + xOffset, po->y + yOffset, po->width , po->height);
	  /* Branch depening on snap to grid */
            /* no snap to grid */
	      /* Insure nothing goes outside the display */
		if(event.xmotion.x < groupDeltaX0)
		  x = groupDeltaX0;
		else if(event.xmotion.x > (int)(daWidth-groupDeltaX1))
		  x =  daWidth - groupDeltaX1;
		else
		  x =  event.xmotion.x;
		if(event.xmotion.y < groupDeltaY0)
		  y = groupDeltaY0;
		else if(event.xmotion.y > (int)(daHeight-groupDeltaY1))
		  y =  daHeight - groupDeltaY1;
		else
		  y =  event.xmotion.y;
	  /* Update current coordinates */
	    xOffset = x - initialX;
	    yOffset  = y - initialY;
	  /* Draw new ones */
	    XDrawRectangle(display,window, xorGC,
	      po->x + xOffset, po->y + yOffset, po->width , po->height);
	    break;

	default:
#if DEBUG_EVENTS > 3
	    print("  Default: %s\n",getEventName(event.type));
	    fflush(stdout);
#endif
	    XtDispatchEvent(&event);
	}
    }
}


void Annotation_move(XButtonEvent *event, AnnotationInfo *ai)
{
  Position     finalX=0, finalY=0;      /* final cursor location */
  XRectangle   rec;
  Annotation  *annotation;
  int          status;
  Dimension    width;
  Dimension    height;
  
  annotation = ai->selectedAnnotation;
  rec.x = (short)annotation->box.rasterX;
  rec.y = (short)annotation->box.rasterY;
  rec.width = (unsigned short)annotation->box.width;
  rec.height = (unsigned short)annotation->box.height;

  status = doDragging(ai->app, ai->canvas, ai->dragCursor, &rec,
      event->x, event->y, &finalX, &finalY);

  if (!status) return;

  /* set annotation box raster coordinates */
  annotation->box.rasterX = finalX - (event->x - annotation->box.rasterX); 
  annotation->box.rasterY = finalY - (event->y - annotation->box.rasterY); 

  XtVaGetValues(ai->canvas,
      XmNwidth,&width,
      XmNheight,&height,
      NULL);

  /* need to calculate curve coordinates */
  Annotation_transformRasterValues(ai,width,height);
}

void Annotation_transformRasterValues(AnnotationInfo *ai, int width, int height)
{
  struct timeval        t0,t1,dll;
  double                dl,db;
  double                inY, outY;
  jlaTransformInfo     *transform = NULL;
  Annotation           *annotation;

  annotation = ai->selectedAnnotation;
  annotation->box.x = 0;
  annotation->box.y = 0;

  /* transform rasterX */
  StripGraph_getattr (ai->graph, STRIPGRAPH_END_TIME, &t1, 0);
  StripGraph_getattr (ai->graph, STRIPGRAPH_BEGIN_TIME, &t0, 0);
  dl = subtract_times(&dll,&t0,&t1);
  db = dl/(width - 1);
  annotation->box.x = annotation->box.rasterX*db + time2dbl(&t0);

  transform=StripGraph_getTransform(ai->graph, annotation->curve);
  if (transform) {
    /* transform raster location to a curve value */
    inY = height - 1 - annotation->box.rasterY;
    outY = 0.0;
    jlaUntransformRasterizedValues (transform, &inY, &outY, (int)1);
    annotation->box.y = outY;
  }
}

