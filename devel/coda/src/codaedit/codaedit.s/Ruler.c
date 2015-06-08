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
 *   Ruler widget main program
 *	
 * Author:  Jie Chen , CEBAF Data Acquisition Group
 *
 * Revision History:
 *   $Log: Ruler.c,v $
 *   Revision 1.3  1998/02/03 15:19:47  rwm
 *   Casts and stuff to get rid of compiler warnings.
 *
 *   Revision 1.2  1997/08/25 16:08:09  heyes
 *   fix display problems with background
 *
 *   Revision 1.1.1.1  1996/08/21 19:36:08  heyes
 *   Imported sources
 *
 */
#include <stdio.h>
#include <math.h>
#include <X11/IntrinsicP.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/CoreP.h>
#include "RulerP.h"
#include "Ruler.h"

#define MIN(a,b)  (((a) < (b)) ? (a) : (b))
#define MAX(a,b)  (((a) > (b)) ? (a) : (b))

static void select_marker();
static void Initialize();
static void Redisplay();
static void Resize();
static void Destroy();
static Boolean SetValues();
static void calculate_marker_pos();
static void SetLabelAttributes();

static char defaultTranslations[]="<Btn1Down>:select()";
static XtActionsRec actionsList[] ={
  {"select",   (XtActionProc)select_marker},
};

static XtResource resources[]={
  {XcodaNorientation,XtCOrientation,XtRInt,sizeof(int),
     XtOffset(XcodaRulerWidget,ruler.orientation),XtRString,"0"},
  {XcodaNrealTickerDiv,XtCRealTickerDiv,XtRInt,sizeof(int),
     XtOffset(XcodaRulerWidget,ruler.realTickerDiv),XtRString,"5"},
  {XcodaNworldTickerDiv,XtCWorldTickerDiv,XtRInt,sizeof(int),
     XtOffset(XcodaRulerWidget,ruler.worldTickerDiv),XtRString,"1"},
  {XcodaNminimum,XtCMin,XtRInt,sizeof(int),
     XtOffset(XcodaRulerWidget,ruler.minimum),XtRString,"0"},
  {XcodaNindicatorColor,XtCColor,XtRPixel,sizeof(Pixel),
     XtOffset(XcodaRulerWidget,ruler.indicatorColor),XtRString,"Black"},
  {XcodaNposition,XtCPos,XtRInt,sizeof(int),
     XtOffset(XcodaRulerWidget,ruler.position),XtRString,"0"},
  {XcodaNnumDivBetweenMajor,XtCNumDivBetweenMajor,XtRInt,sizeof(int),
     XtOffset(XcodaRulerWidget,ruler.numDivBetweenMajor),XtRString,"5"},
  {XcodaNtickerColor,XtCColor,XtRPixel,sizeof(Pixel),
     XtOffset(XcodaRulerWidget,ruler.tickerColor),XtRString,"Black"},
  {XcodaNvalueChangedCallback,XtCValueChangedCallback,XtRCallback,
     sizeof(XtCallbackList),
     XtOffset(XcodaRulerWidget,ruler.valueChanged),XtRCallback,NULL},
};
    
XcodaRulerClassRec xcodaRulerClassRec = {
  /* CoreClassPart*/
  {
#ifdef MOTIF
    /* superclass               */ (WidgetClass) & xmPrimitiveClassRec,
#else
    /* superclass               */ (WidgetClass) & widgetClassRec,
#endif
    "Ruler",                        /*class name   */
    sizeof(XcodaRulerRec),             /*widget -size */
    NULL,                           /*class initialize*/
    NULL,                           /*class part initialize*/
    False,                          /*class_inited*/
    Initialize,                     /*initializ method*/
    NULL,                           /*initialize hook*/
    XtInheritRealize,               /*realize inherited*/
    actionsList,                    /*actions*/
    XtNumber(actionsList),          /*number of actions*/
    resources,                      /*resources*/
    XtNumber(resources),            /*number of resources*/
    NULLQUARK,                      /*xrm_class   */
    TRUE,                           /*compress motion*/
    TRUE,                           /*compress exposure*/
    TRUE,                           /*compress enter leave*/
    TRUE,                           /*visible_interest*/
    Destroy,                        /*destroy method*/
    Resize,                         /*resize method*/
    Redisplay,                      /*expose  callback*/
    SetValues,                      /*setvalues*/
    NULL,                           /*set values hook*/
    XtInheritSetValuesAlmost,       /*set values almost*/
    NULL,                           /*get values hook*/
    NULL,                           /*accept focus*/
    XtVersion,                      /*version*/
    NULL,                           /*callback private*/
    defaultTranslations,            /*translation manager table*/
    NULL,                           /*query geometry*/
    NULL,                           /*display accelarator*/
    NULL,                           /*extension*/
  },
#ifdef MOTIF
  {
    /* primitive_class fields   */
    /* border_highlight         */ (XtWidgetProc) _XtInherit,
    /* border_unhighligh        */ (XtWidgetProc) _XtInherit,
    /* translations             */ XtInheritTranslations,
    /* arm_and_activate         */ (XtWidgetProc) _XtInherit,
    /* syn_resources            */ NULL,
    /* num_syn_resources        */ 0,
    /* extension                */ NULL
  },
#endif
/*ruler class fields*/
  {
    0,   /*ignore*/
  }
};
      
WidgetClass xcodaRulerWidgetClass = (WidgetClass)&xcodaRulerClassRec;


static void Initialize(request,new)
     XcodaRulerWidget request,new;
{
  XGCValues values;
  XtGCMask  valueMask;

  if(request->core.width == 0){
    if(request->ruler.orientation == 0 || request->ruler.orientation == 2)
      new->core.width = 10*(new->ruler.realTickerDiv);
    else
      new->core.width = 30;
  }
  if(request->core.height == 0){
    if(request->ruler.orientation == 0 || request->ruler.orientation == 2)
      new->core.height = 30;
    else
      new->core.height = 10*(new->ruler.realTickerDiv);
  }

  if(request->ruler.realTickerDiv <= 0){
    XtWarning("Distance between tickers cannot be less than 0");
    new->ruler.realTickerDiv = 5;
  }

  if(request->ruler.worldTickerDiv ==0){
    XtWarning("Distance between tickers in real world cannot be 0");
    new->ruler.worldTickerDiv = 1;
  }

  if(new->ruler.position < new->ruler.minimum){
/*    XtWarning("Position less than the ruler minimum");*/
    new->ruler.position = new->ruler.minimum;
  }
  
  if(new->ruler.numDivBetweenMajor == 0){
    XtWarning("Cannot have zero distance between major ticker marker");
    new->ruler.numDivBetweenMajor = 5;
  }

  if(new->ruler.orientation == 0 || new->ruler.orientation == 2)
    new->ruler.numTicker = (new->core.width)/(float)(new->ruler.realTickerDiv);
  else
    new->ruler.numTicker = (new->core.height)/(float)(new->ruler.realTickerDiv);

  new->ruler.font = XLoadQueryFont(XtDisplay(new),
				   "-*-*-medium-r-*-*-*-80-*-*-*-*-*-*");
  new->ruler.realNumTicker = new->ruler.numTicker;

/*create GC*/
  valueMask = GCForeground|GCBackground|GCFont; /* for labels*/
  values.foreground = new->ruler.tickerColor;
  values.background = new->core.background_pixel;
  values.font = new->ruler.font->fid;
  new->ruler.tic_GC = XtGetGC((Widget)new,valueMask,&values);

  values.foreground = new->ruler.indicatorColor;
  new->ruler.marker_GC = XtGetGC((Widget)new,valueMask,&values);
  
  valueMask = GCForeground|GCBackground|GCFunction;
  values.function = GXxor;
  values.foreground = new->ruler.indicatorColor ^ new->core.background_pixel;
  values.background = new->core.background_pixel;
  new->ruler.xor_GC = XtGetGC((Widget)new,valueMask,&values);

  Resize(new);
}

static void Destroy(w)
     XcodaRulerWidget w;
{
  XtReleaseGC((Widget)w,w->ruler.xor_GC);
  XtReleaseGC((Widget)w,w->ruler.marker_GC);
  XtReleaseGC((Widget)w,w->ruler.tic_GC);
  XFreeFont(XtDisplay(w),w->ruler.font);
/*  XtRemoveAllCallbacks(w,XcodaNvalueChangedCallback,w->ruler.valueChanged);*/
  XtRemoveAllCallbacks((Widget)w,XcodaNvalueChangedCallback);
}

static void SetLabelAttributes(w,ort)
XcodaRulerWidget w;
int              ort;
{
  int v_div,div;
  int i,num_label,real_pos;
  int label_height,label_width,label_len;
  XPoint *ptr;
  XFontStruct *fs = w->ruler.font;

  ptr = w->ruler.label_xy;

  label_height = fs->max_bounds.ascent + fs->max_bounds.descent;
  if(ort == 0 || ort == 2){ /* Horizontal*/
    div = w->ruler.realTickerDiv;
    v_div = w->core.height/20;
    if(w->ruler.numDivBetweenMajor != 0)
      num_label = w->ruler.numTicker/(float)(w->ruler.numDivBetweenMajor);
    else
      num_label = w->ruler.numTicker;
    w->ruler.label_num = num_label;
    for(i=0;i<num_label;i++){
      real_pos = (i+1)*w->ruler.worldTickerDiv + w->ruler.minimum;
      sprintf(w->ruler.label[i],"%d",real_pos);
      label_len = strlen(w->ruler.label[i]);
      label_width = XTextWidth(fs,w->ruler.label[i],label_len);
      ptr->x = (w->ruler.numDivBetweenMajor)*(i+1)*div - label_width/2;
      if(ort == 0)
	ptr++->y = w->core.height/2 - label_height/2;
      else
	ptr++->y = w->core.height/2 + label_height/2;
    }
  }
  else{  /*Vertical*/
    div = w->ruler.realTickerDiv;
    v_div = w->core.width/20;
    if(w->ruler.numDivBetweenMajor != 0)
      num_label = w->ruler.numTicker/(float)(w->ruler.numDivBetweenMajor);
    else
      num_label = w->ruler.numTicker;
    w->ruler.label_num = num_label;
    for(i=0;i<num_label;i++){
      real_pos = (i+1)*w->ruler.worldTickerDiv + w->ruler.minimum;
      sprintf(w->ruler.label[i],"%d",real_pos);
      label_len = strlen(w->ruler.label[i]);
      label_width = XTextWidth(fs,w->ruler.label[i],label_len);
      ptr->y = (w->ruler.numDivBetweenMajor)*(i+1)*div + label_height/2;
      if(ort == 1)
	ptr++->x = w->core.width/4 - label_width/2;
      else
	ptr++->x = 3*w->core.width/4 - label_width/2;
    }
  }

}
      

static void Resize(w)
     XcodaRulerWidget w;
{
  int i,div,v_div;
  XSegment *ptr;

  ptr = w->ruler.segments;

  if(w->ruler.orientation == 0 || w->ruler.orientation == 2){  /*horizontal*/
    div = w->ruler.realTickerDiv;
    w->ruler.numTicker = (w->core.width)/(float)(w->ruler.realTickerDiv);
    w->ruler.realNumTicker = (w->core.width - div*w->ruler.numTicker)/
      (float)div 
      + w->ruler.numTicker;
    if(w->ruler.position > w->ruler.realNumTicker*w->ruler.worldTickerDiv/
       (float)w->ruler.numDivBetweenMajor)
      w->ruler.position = w->ruler.realNumTicker*w->ruler.worldTickerDiv/
       (float)w->ruler.numDivBetweenMajor;	
    v_div = w->core.height/20;
    SetLabelAttributes(w,w->ruler.orientation);

/* calculate real number of ticker*/
    for(i=0;i< w->ruler.realNumTicker;i++)
      {
	if((i+1)%(w->ruler.numDivBetweenMajor) == 0)  /*Major ticker*/
	  {
	    ptr->x2 = ptr->x1 = (i+1)*div;
	    if(w->ruler.orientation==0)	    
	      ptr->y1 = w->core.height/2 + 2;
	    else
	      ptr->y1 = w->core.height/2 - 2;
	    if(w->ruler.orientation==0)
	      ptr++->y2 = w->core.height-1;
	    else
	      ptr++->y2 = 1;
	  }
	else
	  {
	    ptr->x1 = ptr->x2 = (i+1)*div;
	    if(w->ruler.orientation == 0)
	      ptr->y1 = w->core.height - 4;
	    else
	      ptr->y1 = 1;
	    if(w->ruler.orientation == 0)
	      ptr++->y2 = w->core.height - 1;
	    else
	      ptr++->y2 = 4;
	  }
      }
  }
  else{  /*vertical*/
    div = w->ruler.realTickerDiv;
    w->ruler.numTicker = (w->core.height)/(float)(w->ruler.realTickerDiv);
    w->ruler.realNumTicker = (w->core.height - div*w->ruler.numTicker)/(float)div 
      + w->ruler.numTicker;
    if(w->ruler.position > w->ruler.realNumTicker*w->ruler.worldTickerDiv/
       (float)w->ruler.numDivBetweenMajor)       
      w->ruler.position =  w->ruler.realNumTicker*w->ruler.worldTickerDiv/
       (float)w->ruler.numDivBetweenMajor;

    v_div = w->core.width/20;
    SetLabelAttributes(w,w->ruler.orientation);  /*determin label position...*/
    for(i=0;i< w->ruler.realNumTicker;i++)
      {
	if((i+1)%(w->ruler.numDivBetweenMajor) == 0)  /*Major ticker*/
	  {
	    ptr->y1 = ptr->y2 = (i+1)*div;
	    if(w->ruler.orientation == 1)	    
	      ptr->x1 = w->core.width/2 + 2;
	    else
	      ptr->x1 = w->core.width/2 - 2;
	    if(w->ruler.orientation == 1)
	      ptr++->x2 = w->core.width - 1;
	    else
	      ptr++->x2 = 1;
	  }
	else
	  {
	    ptr->y1 = ptr->y2 = (i+1)*div;
	    if(w->ruler.orientation == 1)
	      ptr->x1 = w->core.width - 4;
	    else
	      ptr->x1 = 1;
	    if(w->ruler.orientation == 1)
	      ptr++->x2 = w->core.width - 1;
	    else
	      ptr++->x2 = 4;
	  }
      }
  }
  calculate_marker_pos(w);
}

static void calculate_marker_pos(w)
     XcodaRulerWidget w;
{
  float norm_pos;
  Position x,y;
  int div,v_div;

  if(w->ruler.orientation == 0 || w->ruler.orientation == 2){
    v_div = (w->core.height)/20;
    div = w->ruler.realTickerDiv;
/*calculate marker pos on the ruler*/
    norm_pos = (w->ruler.position - w->ruler.minimum)/
      (float)(w->ruler.worldTickerDiv)*(w->ruler.numDivBetweenMajor);
    
    if(w->ruler.orientation == 0){
      x = (int)((norm_pos)*(w->ruler.realTickerDiv));
      if(w->core.height <= 20)
	y = w->core.height;
      else
	y = w->core.height/2 + 10;
    }
    else{
      x = (int)((norm_pos)*(w->ruler.realTickerDiv));
      if(w->core.height <= 20)
	y = 0;
      else
	y = w->core.height/2 - 10;
    }

    w->ruler.marker_x = x;
    w->ruler.marker_y = y;
    w->ruler.marker_length = 10;
    w->ruler.marker_width = 10;
  }
  else{
    v_div = (w->core.width)/20;
    div = w->ruler.realTickerDiv;
    /*calculate marker pos on the ruler*/
    norm_pos = (w->ruler.position - w->ruler.minimum)/
      (float)(w->ruler.worldTickerDiv)*(w->ruler.numDivBetweenMajor);
    
    if(w->ruler.orientation == 1){
      y = (int)((norm_pos)*div);
      if(w->core.width <= 20)
	x = w->core.width;
      else
	x = w->core.width/2 + 10;
    }
    else{
      y = (int)((norm_pos)*div);
      if(w->core.width <= 20)
	x = 0;
      else
	x = w->core.width/2 - 10;
    }
    
    w->ruler.marker_x = x;
    w->ruler.marker_y = y;
    w->ruler.marker_length = 10;
    w->ruler.marker_width = 10;
  }

}

static void Redisplay(w,event,region)
     XcodaRulerWidget w;
     XEvent        *event;
     Region        region;
{
  int   v_div,i;
  int   div;
  XPoint pp[4];

  /* clear old junk */
  XClearWindow (XtDisplay(w), XtWindow(w));

  XSetForeground(XtDisplay(w), w->ruler.tic_GC, w->core.background_pixel);

  XFillRectangle(XtDisplay(w),XtWindow(w) , w->ruler.tic_GC,0,
			 0, w->core.width,
			 w->core.height);

  XSetForeground(XtDisplay(w), w->ruler.tic_GC, w->ruler.tickerColor);

  if(w->ruler.orientation == 0 || w->ruler.orientation == 2){
    v_div = (w->core.height)/20;    
    div = w->ruler.realTickerDiv;
  }
  else{
    v_div = (w->core.width)/20;
    div = w->ruler.realTickerDiv;
  }

  if(w->core.visible){
    /*draw ticker markers*/
    XDrawSegments(XtDisplay(w),XtWindow(w),
		  w->ruler.tic_GC,
		  w->ruler.segments,
		  w->ruler.realNumTicker);

    /*draw base line*/
    if(w->ruler.orientation == 0)
      XDrawLine(XtDisplay(w),XtWindow(w),w->ruler.tic_GC,
		0,w->core.height-1,w->core.width,w->core.height-1);
    else if(w->ruler.orientation == 2)
      XDrawLine(XtDisplay(w),XtWindow(w),w->ruler.tic_GC,
		0,1,w->core.width,1);
    else if(w->ruler.orientation == 1)
      XDrawLine(XtDisplay(w),XtWindow(w),w->ruler.tic_GC,
		w->core.width-1,0,w->core.width-1,w->core.height);      
    else
      XDrawLine(XtDisplay(w),XtWindow(w),w->ruler.tic_GC,
		1,0,1,w->core.height);      

/*draw lables*/
    for(i=0;i<w->ruler.label_num;i++){
      XDrawString(XtDisplay(w),XtWindow(w),w->ruler.tic_GC,
		  w->ruler.label_xy[i].x,w->ruler.label_xy[i].y,
		  w->ruler.label[i],strlen(w->ruler.label[i]));
    }
    /*draw marker*/
    if(w->ruler.orientation == 0){
      pp[0].x = w->ruler.marker_x;
      pp[0].y = w->ruler.marker_y;
      pp[1].x = w->ruler.marker_x - w->ruler.marker_width/2;
      pp[1].y = w->ruler.marker_y - w->ruler.marker_length;
      pp[2].x = w->ruler.marker_x + w->ruler.marker_width/2;
      pp[2].y = pp[1].y;
      pp[3].x = pp[0].x;
      pp[3].y = pp[0].y;
      XDrawLines(XtDisplay(w),XtWindow(w),w->ruler.xor_GC,
		 pp,4,CoordModeOrigin);
    }
    else if(w->ruler.orientation == 2){
      pp[0].x = w->ruler.marker_x;
      pp[0].y = w->ruler.marker_y;
      pp[1].x = w->ruler.marker_x - w->ruler.marker_width/2;
      pp[1].y = w->ruler.marker_y + w->ruler.marker_length;
      pp[2].x = w->ruler.marker_x + w->ruler.marker_width/2;
      pp[2].y = pp[1].y;
      pp[3].x = pp[0].x;
      pp[3].y = pp[0].y;
      XDrawLines(XtDisplay(w),XtWindow(w),w->ruler.xor_GC,
		 pp,4,CoordModeOrigin);
    }
    else if(w->ruler.orientation == 1){
      pp[0].x = w->ruler.marker_x;
      pp[0].y = w->ruler.marker_y;
      pp[1].x = w->ruler.marker_x - w->ruler.marker_length;
      pp[1].y = w->ruler.marker_y - w->ruler.marker_width/2;
      pp[2].y = w->ruler.marker_y + w->ruler.marker_width/2;
      pp[2].x = pp[1].x;
      pp[3].x = pp[0].x;
      pp[3].y = pp[0].y;
      XDrawLines(XtDisplay(w),XtWindow(w),w->ruler.xor_GC,
		 pp,4,CoordModeOrigin);
    }
    else{
      pp[0].x = w->ruler.marker_x;
      pp[0].y = w->ruler.marker_y;
      pp[1].x = w->ruler.marker_x + w->ruler.marker_length;
      pp[1].y = w->ruler.marker_y - w->ruler.marker_width/2;
      pp[2].y = w->ruler.marker_y + w->ruler.marker_width/2;
      pp[2].x = pp[1].x;
      pp[3].x = pp[0].x;
      pp[3].y = pp[0].y;
      XDrawLines(XtDisplay(w),XtWindow(w),w->ruler.xor_GC,
		 pp,4,CoordModeOrigin);
    }

  }
}


static Boolean SetValues(current,request,new)
     XcodaRulerWidget current,request,new;
{
  XGCValues values;
  XtGCMask  valueMask;
  Boolean   redraw = False;
  Boolean   redraw_marker = False;
  XPoint    pp[4];
  int i,div,v_div;
  XSegment *ptr;


  /*make sure new ruler Widget is resonable*/
  /*check to see the orientation resource*/
  if(new->ruler.orientation != current->ruler.orientation)
    ;

/* check to see color resources*/
  if(new->ruler.indicatorColor != current->ruler.indicatorColor ||
     new->core.background_pixel != current->core.background_pixel){
    if(new->ruler.orientation == 0){
      pp[0].x = current->ruler.marker_x;
      pp[0].y = current->ruler.marker_y;
      pp[1].x = current->ruler.marker_x - current->ruler.marker_width/2;
      pp[1].y = current->ruler.marker_y - current->ruler.marker_length;
      pp[2].x = current->ruler.marker_x + current->ruler.marker_width/2;
      pp[2].y = pp[1].y;
      pp[3].x = pp[0].x;
      pp[3].y = pp[0].y;
      XDrawLines(XtDisplay(current),XtWindow(current),
		 current->ruler.xor_GC,
		 pp,4,CoordModeOrigin);
    }
    else if(new->ruler.orientation == 2){
      pp[0].x = current->ruler.marker_x;
      pp[0].y = current->ruler.marker_y;
      pp[1].x = current->ruler.marker_x - current->ruler.marker_width/2;
      pp[1].y = current->ruler.marker_y + current->ruler.marker_length;
      pp[2].x = current->ruler.marker_x + current->ruler.marker_width/2;
      pp[2].y = pp[1].y;
      pp[3].x = pp[0].x;
      pp[3].y = pp[0].y;
      XDrawLines(XtDisplay(current),XtWindow(current),
		 current->ruler.xor_GC,
		 pp,4,CoordModeOrigin);
    }
    else if(new->ruler.orientation == 1){
      pp[0].x = current->ruler.marker_x;
      pp[0].y = current->ruler.marker_y;
      pp[1].y = current->ruler.marker_y - current->ruler.marker_width/2;
      pp[1].x = current->ruler.marker_x - current->ruler.marker_length;
      pp[2].y = current->ruler.marker_y + current->ruler.marker_width/2;
      pp[2].x = pp[1].x;
      pp[3].x = pp[0].x;
      pp[3].y = pp[0].y;
      XDrawLines(XtDisplay(current),XtWindow(current),
		 current->ruler.xor_GC,
		 pp,4,CoordModeOrigin);
    }
    else{
      pp[0].x = current->ruler.marker_x;
      pp[0].y = current->ruler.marker_y;
      pp[1].y = current->ruler.marker_y - current->ruler.marker_width/2;
      pp[1].x = current->ruler.marker_x + current->ruler.marker_length;
      pp[2].y = current->ruler.marker_y + current->ruler.marker_width/2;
      pp[2].x = pp[1].x;
      pp[3].x = pp[0].x;
      pp[3].y = pp[0].y;
      XDrawLines(XtDisplay(current),XtWindow(current),
		 current->ruler.xor_GC,
		 pp,4,CoordModeOrigin);
    }
    valueMask = GCForeground | GCBackground;
    values.foreground = new->ruler.indicatorColor;
    values.background = new->core.background_pixel;
    XtReleaseGC((Widget)new,new->ruler.marker_GC);
    valueMask = GCForeground | GCBackground;
    new->ruler.marker_GC = XtGetGC((Widget)new,valueMask,&values);
    values.foreground = new->ruler.tickerColor;
    values.background = new->core.background_pixel;
    XtReleaseGC((Widget)new,new->ruler.tic_GC);
    new->ruler.tic_GC = XtGetGC((Widget)new,valueMask,&values);
    valueMask = GCForeground|GCBackground|GCFunction;
    values.background = new->core.background_pixel;
    values.foreground = new->ruler.indicatorColor^new->core.background_pixel; 
    values.function = GXxor;
    XtReleaseGC((Widget)new,new->ruler.xor_GC);
    new->ruler.xor_GC = XtGetGC((Widget)new,valueMask,&values);
    redraw = False;
  }
  
  if(new->ruler.tickerColor != current->ruler.tickerColor){
    valueMask = GCForeground | GCBackground;
    values.foreground = new->ruler.tickerColor;
    values.background = new->core.background_pixel;
    XtReleaseGC((Widget)new,new->ruler.tic_GC);
    new->ruler.tic_GC = XtGetGC((Widget)new,valueMask,&values);
    redraw = True;
  }

/* if distance between tickers has been changed, redraw eveerything*/
  if(new->ruler.realTickerDiv != current->ruler.realTickerDiv ||
     new->ruler.worldTickerDiv != current->ruler.worldTickerDiv ||
     new->ruler.numDivBetweenMajor != current->ruler.numDivBetweenMajor){
    redraw = True;
    ptr = new->ruler.segments;
    if(new->ruler.orientation == 0 || new->ruler.orientation == 2){  /*horizontal*/
      div = new->ruler.realTickerDiv;
      new->ruler.numTicker = (new->core.width)/(new->ruler.realTickerDiv); 
      v_div = new->core.height/20;

      /* calculate real number of ticker*/
      new->ruler.realNumTicker = (new->core.width - div*new->ruler.numTicker)/div 
	+ new->ruler.numTicker;


      SetLabelAttributes(new,new->ruler.orientation);
      for(i=0;i< new->ruler.realNumTicker;i++)
	{
	  if((i+1)%(new->ruler.numDivBetweenMajor) == 0)  /*Major ticker*/
	    {
	      ptr->x1 = ptr->x2 = (i+1)*div;
	      if(new->ruler.orientation==0)
		ptr->y1 = new->core.height/2 - 2;
	      else
		ptr->y1 = new->core.height/2 + 2;
	      if(new->ruler.orientation==0)
		ptr++->y2 = new->core.height-1;
	      else
		ptr++->y2 = 1;
	    }
	  else
	    {
	      ptr->x1 = ptr->x2 = (i+1)*div;
	      if(new->ruler.orientation == 0)
		ptr->y1 = new->core.height - 4;
	      else
		ptr->y1 = 1;
	      if(new->ruler.orientation == 0)
		ptr++->y2 = new->core.height - 1;
	      else
		ptr++->y2 = 4;
	    }
	}
    }
    else{  /*vertical*/
      div = new->ruler.realTickerDiv;
      new->ruler.numTicker = (new->core.height)/(float)(new->ruler.realTickerDiv);  /*distance between tic*/
      v_div = new->core.width/20;
      
      new->ruler.realNumTicker = (new->core.height - div*new->ruler.numTicker)/div 
	+ new->ruler.numTicker;

      SetLabelAttributes(new,new->ruler.orientation);
      for(i=0;i< new->ruler.realNumTicker;i++)
	{
	  if((i+1)%(new->ruler.numDivBetweenMajor) == 0)  /*Major ticker*/
	    {
	      ptr->y1 = ptr->y2 = (i+1)*div;
	      if(new->ruler.orientation == 1)
		ptr->x1 = new->core.width/2 - 2;
	      else
		ptr->x1 = new->core.width/2 + 2;
	      if(new->ruler.orientation == 1)
		ptr++->x2 = new->core.width - 1;
	      else
		ptr++->x2 = 1;
	    }
	  else
	    {
	      ptr->y1 = ptr->y2 = (i+1)*div;
	      if(new->ruler.orientation == 1)
		ptr->x1 = new->core.width - 4;
	      else
		ptr->x1 = 1;
	      if(new->ruler.orientation == 1)
		ptr++->x2 = new->core.width - 1;
	      else
		ptr++->x2 = 4;
	    }
	}
    }
  }

  if(new->ruler.minimum != current->ruler.minimum){
      redraw = True;
      calculate_marker_pos(new);
      SetLabelAttributes(new,new->ruler.orientation);
    }

/* if marker changed position or min and max have been changed*/
  if(new->ruler.position != current->ruler.position){
    calculate_marker_pos(new);
    redraw_marker = True;
  }

/* if only the marker needs to be redrawn*/
  if(redraw_marker && ! redraw &&
     XtIsRealized((Widget)new) && new->core.visible){
    if(new->ruler.orientation == 0){
      pp[0].x = current->ruler.marker_x;
      pp[0].y = current->ruler.marker_y;
      pp[1].x = current->ruler.marker_x - current->ruler.marker_width/2;
      pp[1].y = current->ruler.marker_y - current->ruler.marker_length;
      pp[2].x = current->ruler.marker_x + current->ruler.marker_width/2;
      pp[2].y = pp[1].y;
      pp[3].x = pp[0].x;
      pp[3].y = pp[0].y;
      XDrawLines(XtDisplay(current),XtWindow(current),
		 current->ruler.xor_GC,
		 pp,4,CoordModeOrigin);
      
      pp[0].x = new->ruler.marker_x;
      pp[0].y = new->ruler.marker_y;
      pp[1].x = new->ruler.marker_x - new->ruler.marker_width/2;
      pp[1].y = new->ruler.marker_y - new->ruler.marker_length;
      pp[2].x = new->ruler.marker_x + new->ruler.marker_width/2;
      pp[2].y = pp[1].y;
      pp[3].x = pp[0].x;
      pp[3].y = pp[0].y;
      XDrawLines(XtDisplay(new),XtWindow(new),
		 new->ruler.xor_GC,
		 pp,4,CoordModeOrigin);
    }
    else if(new->ruler.orientation == 2){
      pp[0].x = current->ruler.marker_x;
      pp[0].y = current->ruler.marker_y;
      pp[1].x = current->ruler.marker_x - current->ruler.marker_width/2;
      pp[1].y = current->ruler.marker_y + current->ruler.marker_length;
      pp[2].x = current->ruler.marker_x + current->ruler.marker_width/2;
      pp[2].y = pp[1].y;
      pp[3].x = pp[0].x;
      pp[3].y = pp[0].y;
      XDrawLines(XtDisplay(current),XtWindow(current),
		 current->ruler.xor_GC,
		 pp,4,CoordModeOrigin);
      
      pp[0].x = new->ruler.marker_x;
      pp[0].y = new->ruler.marker_y;
      pp[1].x = new->ruler.marker_x - new->ruler.marker_width/2;
      pp[1].y = new->ruler.marker_y + new->ruler.marker_length;
      pp[2].x = new->ruler.marker_x + new->ruler.marker_width/2;
      pp[2].y = pp[1].y;
      pp[3].x = pp[0].x;
      pp[3].y = pp[0].y;
      XDrawLines(XtDisplay(new),XtWindow(new),new->ruler.xor_GC,
		 pp,4,CoordModeOrigin);
    }
    else if(new->ruler.orientation == 1){
      pp[0].x = current->ruler.marker_x;
      pp[0].y = current->ruler.marker_y;
      pp[1].y = current->ruler.marker_y - current->ruler.marker_width/2;
      pp[1].x = current->ruler.marker_x - current->ruler.marker_length;
      pp[2].y = current->ruler.marker_y + current->ruler.marker_width/2;
      pp[2].x = pp[1].x;
      pp[3].x = pp[0].x;
      pp[3].y = pp[0].y;
      XDrawLines(XtDisplay(current),XtWindow(current),
		 current->ruler.xor_GC,
		 pp,4,CoordModeOrigin);
      
      pp[0].x = new->ruler.marker_x;
      pp[0].y = new->ruler.marker_y;
      pp[1].y = new->ruler.marker_y - new->ruler.marker_width/2;
      pp[1].x = new->ruler.marker_x - new->ruler.marker_length;
      pp[2].y = new->ruler.marker_y + new->ruler.marker_width/2;
      pp[2].x = pp[1].x;
      pp[3].x = pp[0].x;
      pp[3].y = pp[0].y;
      XDrawLines(XtDisplay(new),XtWindow(new),new->ruler.xor_GC,
		 pp,4,CoordModeOrigin);
    }
    else{
      pp[0].x = current->ruler.marker_x;
      pp[0].y = current->ruler.marker_y;
      pp[1].y = current->ruler.marker_y - current->ruler.marker_width/2;
      pp[1].x = current->ruler.marker_x + current->ruler.marker_length;
      pp[2].y = current->ruler.marker_y + current->ruler.marker_width/2;
      pp[2].x = pp[1].x;
      pp[3].x = pp[0].x;
      pp[3].y = pp[0].y;
      XDrawLines(XtDisplay(current),XtWindow(current),
		 current->ruler.xor_GC,
		 pp,4,CoordModeOrigin);
      
      pp[0].x = new->ruler.marker_x;
      pp[0].y = new->ruler.marker_y;
      pp[1].y = new->ruler.marker_y - new->ruler.marker_width/2;
      pp[1].x = new->ruler.marker_x + new->ruler.marker_length;
      pp[2].y = new->ruler.marker_y + new->ruler.marker_width/2;
      pp[2].x = pp[1].x;
      pp[3].x = pp[0].x;
      pp[3].y = pp[0].y;
      XDrawLines(XtDisplay(new),XtWindow(new),new->ruler.xor_GC,
		 pp,4,CoordModeOrigin);
    }

  }
  return(redraw);
}

static void select_marker(w,event,args,n_args)
     XcodaRulerWidget w;
     XEvent        *event;
     char          *args[];
     int           n_args;
{
  Position pos;
  int      real_pos;
  XcodaRulerCallbackStruct cb;

  pos = w->ruler.position;
  if(event->type == ButtonPress ||
     event->type == MotionNotify){
    if(w->ruler.orientation == 0 || w->ruler.orientation == 2){ /*horizontal*/
      pos = event->xbutton.x;
      real_pos = w->ruler.minimum + pos/(float)((w->ruler.realTickerDiv)*
	         (w->ruler.numDivBetweenMajor))*w->ruler.worldTickerDiv;
    }
    else
      {
	pos = event->xbutton.y;
	real_pos = w->ruler.minimum + pos/(float)((w->ruler.realTickerDiv)*
	         (w->ruler.numDivBetweenMajor))*w->ruler.worldTickerDiv;

      }
  }

/*invoke the callback, report the position in the call_data struct*/
  cb.reason = Xcoda_SELECTED;
  cb.event = event;
  cb.position = real_pos;
  XtCallCallbacks((Widget)w,XcodaNvalueChangedCallback,&cb);  /*old X11R3*/
}
