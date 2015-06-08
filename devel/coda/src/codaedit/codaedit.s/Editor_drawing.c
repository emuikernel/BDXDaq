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
 *	CODA Editor drawing routines for components
 *	
 * Author:  Jie Chen, CEBAF Data Acquisition Group
 *
 * Revision History:
 *   $Log: Editor_drawing.c,v $
 *   Revision 1.20  1998/06/18 12:28:27  heyes
 *   tidy up a lot of thingscd ../cedit
 *
 *   Revision 1.19  1998/02/10 20:35:29  heyes
 *    fix crash on new configuration bug (not checking null string).
 *
 *   Revision 1.18  1997/09/08 15:19:19  heyes
 *   fix dd icon etc
 * 
 *   Revision 1.17  1997/09/05 12:06:44  heyes
 *   almost final
 *
 *   Revision 1.16  1997/09/03 17:54:29  heyes
 *   fix status bar
 *
 *   Revision 1.15  1997/08/29 13:18:23  heyes
 *   remove ref to basename
 *
 *   Revision 1.14  1997/07/23 15:04:03  heyes
 *   increase stability
 *
 *   Revision 1.13  1997/07/22 12:16:31  heyes
 *   need to debug on solaris
 *
 *   Revision 1.12  1997/07/14 13:25:24  heyes
 *   fix strign compare when adding debug
 *
 *   Revision 1.11  1997/07/11 13:13:07  heyes
 *   tweak coloring
 *
 *   Revision 1.10  1997/07/09 19:05:24  heyes
 *   darnit marki
 *
 *   Revision 1.2  1997/06/20 16:59:57  heyes
 *   clean up GUI!
 *
 *   Revision 1.1.1.2  1996/11/05 17:45:06  chen
 *   coda source
 *
 *	  
 */
#include <stdio.h>
#include <string.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <Xm/Xm.h>

#include "Editor_graph.h"
#include "Editor_drawing.h"
#include "Editor_pixmap.h"

#if defined (__STDC__)
void setCompState(char *name,int state);
#else
void setCompState(name,state);
#endif

static char *names[] = {
  "TS",
  "ROC",
  "EB",
  "ANA",
  "EBANA",
  "UT",
  "ER",
  "LOG",
  "SC",
  "UC",
  "RCS",
  "Binary file",
  "Coda file",
  "Debug",
  "Unknown",
  "MON",
  "trash",
  NULL};

static char *sN[] =
{
  "unknown",			/* 0 */
  "booting",			/* 1 */
  "booted",			/* 2 */
  "initing",	         	/* 3 */
  "inited",			/* 4 */
  "loading",    		/* 5 */
  "loaded",			/* 6 */
  "prestarting",		/* 7 */
  "paused",			/* 8 */
  "pausing",			/* 9 */
  "activating",			/* 10 */
  "active",			/* 11 */
  "ending",			/* 12 */
  "verifying",			/* 13 */
  "verified",			/* 14 */
  "terminating",
  "terminated",
  "resuming"
};
#define DA_UNKNOWN        0
#define DA_BOOTING        1
#define DA_BOOTED         2
#define DA_CONFIGURING    3
#define DA_CONFIGURED     4
#define DA_DOWNLOADING    5
#define DA_DOWNLOADED     6
#define DA_PRESTARTING    7
#define DA_PAUSED         8
#define DA_PAUSING        9
#define DA_ACTIVATING     10
#define DA_ACTIVE         11
#define DA_ENDING         12
#define DA_VERIFYING      13
#define DA_VERIFIED       14
#define DA_TERMINATING    15
#define DA_PRESTARTED     16
#define DA_RESUMING       17
#define DA_STATES         18
 
/***********************************************************************
 *      static void draw_arrow(arc, dpy, win, gc, x, y, zoom)          *
 * Description:                                                        *
 *   Draw arrow head                                                   *
 ***********************************************************************/
#if defined (__STDC__)
static void draw_arrow(Arc* arc, 
		       Display* dpy, 
		       Drawable win, 
		       GC gc, 
		       int x, int y, 
		       int zoom, int type)
#else
static void draw_arrow(arc, dpy, win, gc, x, y, zoom,type)
     Arc        *arc;
     Display    *dpy;
     Drawable   win;
     GC         gc;
     int        x, y;
     int        zoom,type;
#endif
{
  int arrow_wd;
  XPoint pp[4];
  
  switch(zoom){
  case 1:
  case 2:
    arrow_wd = 3;
    break;
  case 3:
    arrow_wd = 4;
    break;
  case 4:
    arrow_wd = 6;
    break;
  case 5:
    arrow_wd = 8;
    break;
  case 6:
    arrow_wd = 10;
  default:
    break;
  }
  
  if(type == 0){
    pp[0].x = x;
    pp[0].y = y;
    pp[1].x = x - arrow_wd;
    pp[1].y = y - arrow_wd;
    pp[2].x = x - arrow_wd;
    pp[2].y = y + arrow_wd;
    pp[3].x = x;
    pp[3].y = y;
  }
  else if(type == 1){
    pp[0].x = x;
    pp[0].y = y;
    pp[1].x = x + arrow_wd;
    pp[1].y = y - arrow_wd;
    pp[2].x = x + arrow_wd;
    pp[2].y = y + arrow_wd;
    pp[3].x = x;
    pp[3].y = y;
  }
  else{
    pp[0].x = x;
    pp[0].y = y;
    pp[1].x = x - arrow_wd;
    pp[1].y = y - arrow_wd;
    pp[2].x = x - arrow_wd;
    pp[2].y = y + arrow_wd;
    pp[3].x = x;
    pp[3].y = y;
  }
  XFillPolygon(dpy, win, gc, pp, 4, Convex, CoordModeOrigin);
}

/******************************************************************
 *   static void XFill3dRecTangle( )                              *
 * Description:                                                   *
 *     draw a 3d appearance rectangle                             *
 *****************************************************************/
#if defined (__STDC__)
static void XFill3dRectangle(Display* dpy, 
			     Drawable win, 
			     GC gc, 
			     int* x, int* y, 
			     Dimension* wd, 
			     Dimension* ht, 
			     int num_row, 
			     Pixel fg, Pixel bg, 
			     unsigned short scale,
			     int type)
#else
static void XFill3dRectangle(dpy, win, gc, x, y, wd, ht, num_row, fg, bg, 
			     scale, type)
     Display        *dpy;
     Drawable       win;
     GC             gc;
     int            *x, *y;
     Dimension      *wd, *ht;
     int            num_row;
     Pixel          fg, bg;
     unsigned short scale;
     int            type;
#endif
{
  XPoint pp[5];
  int       rel_x, rel_y;
  Dimension rel_wd, rel_ht;
  Pixel  b_sh, t_sh;
  
  XcodaGetShadowColor(dpy, win, fg, &b_sh, &t_sh);
  if(scale == 1)
    XFillRectangle(dpy, win, gc, *x, *y, *wd, *ht*num_row);
  else{
    rel_x = *x + scale;
    rel_y = *y + scale;
    rel_wd = *wd - 2*scale;
    rel_ht = (*ht)*num_row - 2*scale;
    
    XFillRectangle(dpy, win, gc, rel_x, rel_y, rel_wd, rel_ht);
    pp[0].x = *x;
    pp[0].y = *y;
    pp[1].x = rel_x;
    pp[1].y = rel_y;
    pp[2].x = pp[1].x;
    pp[2].y = pp[1].y + rel_ht;
    pp[3].x = pp[0].x;
    pp[3].y = pp[0].y + (*ht)*num_row;
    pp[4].x = pp[0].x;
    pp[4].y = pp[0].y;
    if(type == 0)
      XSetForeground(dpy, gc, t_sh);
    else if(type == 1)
      XSetForeground(dpy, gc, b_sh);
    else if(type == 2) /* rubber band unhigh lighted */
      XSetForeground(dpy, gc, t_sh^bg);
    else if(type == 3) /* rubber band high lighted */
      XSetForeground(dpy, gc, b_sh^bg);
    XFillPolygon(dpy, win, gc, pp, 5, Convex, CoordModeOrigin);

    pp[2].x = pp[1].x + rel_wd;
    pp[2].y = pp[1].y;
    pp[3].x = pp[0].x + *wd;
    pp[3].y = pp[0].y;
    XFillPolygon(dpy, win, gc, pp, 5, Convex, CoordModeOrigin);

    pp[0].x = *x + *wd;
    pp[0].y = *y + (*ht)*num_row;
    pp[1].x = pp[0].x - scale;
    pp[1].y = pp[0].y - scale;
    pp[4].x = pp[0].x;
    pp[4].y = pp[0].y;
    if(type == 0)
      XSetForeground(dpy, gc, b_sh);
    else if(type == 1) /* high lighted */
      XSetForeground(dpy, gc, t_sh);
    else if(type == 2) /* rubber band unhighlighted */
      XSetForeground(dpy, gc, b_sh^bg);
    else if(type == 3) /* rubber band high lighted */
      XSetForeground(dpy, gc, t_sh^bg);
    XFillPolygon(dpy, win, gc, pp, 5, Convex, CoordModeOrigin);

    pp[2].x = pp[1].x - rel_wd;
    pp[2].y = pp[1].y;
    pp[3].x = *x;
    pp[3].y = pp[0].y;
    XFillPolygon(dpy, win, gc, pp, 5, Convex, CoordModeOrigin);
    *x = rel_x;
    *y = rel_y;
    *wd = rel_wd;
    *ht = *ht - 2*scale;
  }
}


/**************************************************************
 *     int XTextHeight(XFontStruct *)                         *
 * Description:                                               *
 *    Return text height for a particular font                *
 *************************************************************/
#if defined (__STDC__)
static int XTextHeight(XFontStruct *font)
#else
static int XTextHeight(font)
     XFontStruct *font;
#endif
{
  return (font->max_bounds.ascent + font->max_bounds.descent);
}

/**************************************************************
 *  void draw_original()                                      *
 * Description:                                               *
 *    draw all CODA supported daq components                  *
 *************************************************************/
#if defined (__STDC__)
void draw_original(drawComp* draw_comp, 
		   Display* dpy, 
		   Drawable win, 
		   int x, int y, 
		   Dimension wd, 
		   Dimension ht, 
		   int r, int c)
#else
void draw_original(draw_comp, dpy, win, x, y, wd, ht, r, c)
     drawComp *draw_comp;
     Display *dpy;
     Drawable win;
     int      x, y;
     int      r, c;
     Dimension wd, ht;
#endif
{
  Dimension xdiv, ydiv;
  XPoint    pp[10];
  GC        gc = xgc.r_gc;
  int       xx, yy;
  Dimension rec_wd, rec_ht;
  Dimension text_wd, text_ht;
  Dimension cir_wd,cir_ht;
  int       delta_x;
  int       i;
  char *long_name = "UNKNOWN";
  char *short_name = "U";

  draw_comp->width = wd;
  draw_comp->height = ht;
  draw_comp->row = r;
  draw_comp->col = c;
  draw_comp->x = x;
  draw_comp->y = y;

  /* if either x or y is out of viewing region, do nothing */
  if (y < 0 || x < 0)
    return;

  XSetLineAttributes(dpy, gc, 1, LineSolid, CapButt, JoinRound);
  
  xdiv = wd/8;
  ydiv = ht/4;

  if (draw_comp->comp.type < 11 ) {
    long_name = names[draw_comp->comp.type];
    XSetForeground(dpy, gc, xgc.White);
    XFillRectangle(dpy, win,gc,x,y,wd,ht*draw_comp->num_row);
    
    XSetForeground(dpy, gc, xgc.Green);
    
    for(i=0;i<draw_comp->num_row;i++){
      xx = x;
      yy = y + 3*ydiv/2.0 + i*ht;
      if (draw_comp->comp.type > CODA_ROC)
	XFillRectangle(dpy, win, gc, xx, yy, xdiv, ydiv);
      xx = x + wd - xdiv ;
      yy = y + 3*ydiv/2.0 + i*ht;
      XFillRectangle(dpy, win, gc, xx, yy, xdiv, ydiv);
    }

    XSetForeground(dpy, gc, xgc.Black);
    XDrawRectangle(dpy, win,gc,x,y,wd-1,ht*draw_comp->num_row-1);
    
    rec_ht = ht/3.0;
    y -= 1;    
    /*XSetForeground(dpy, gc, xgc.Red);
    XFillRectangle(dpy, win,gc,x,
		   y + ht*draw_comp->num_row - rec_ht,
		   wd-1,
		   rec_ht);
    XSetForeground(dpy, gc, xgc.Black);
    XDrawRectangle(dpy, win,gc,x,
		   y + ht*draw_comp->num_row - rec_ht,
		   wd-1,
		   rec_ht);
		   */
    i = sw_geometry.zoomscale - 1;
    if (i>0) {
      XSetForeground(dpy, gc, xgc.Black);
      XSetFont(dpy, gc, xgc.font[2]->fid);
      yy = xgc.font[2]->max_bounds.ascent + y + 2;
      text_wd = XTextWidth(xgc.font[2], long_name, 3);
      xx = x + wd - text_wd - xgc.font[2]->max_bounds.ascent/2;
      XDrawString(dpy, win, gc, xx, yy, long_name, strlen(long_name));
    }
    {
      ipPort   *port;
      int      i;
      for(i=0;i <draw_comp->num_ports; i++){
	port = &(draw_comp->ip_port[i]);
	(*port->write_name)(port, dpy, win);
      }
      draw_host_name(draw_comp,dpy,win);
      draw_comp_name(draw_comp,dpy,win);
      if (draw_comp->comp.comp_name)
	setCompState(draw_comp->comp.comp_name,-1);
    }
  } else {

    XSetForeground(dpy, gc, xgc.White);
    XFillRectangle(dpy, win,gc,x,y,wd,ht);
    XSetForeground(dpy, gc, xgc.Green);
    for(i=0;i<draw_comp->num_row;i++){
      xx = x;
      yy = y + 3*ydiv/2.0 + i*ht;
      XFillRectangle(dpy, win, gc, xx, yy, xdiv, ydiv);
    }
    if (draw_comp->comp.type == CODA_FILE) {
      XCopyArea(dpy,btn_pixmaps.file,win,gc,0,0,30,38,x+wd-30,y);
    } else if (draw_comp->comp.type == CODA_CODAFILE) {
      XCopyArea(dpy,btn_pixmaps.codaFile,win,gc,0,0,30,38,x+wd-30,y);
    } else if (draw_comp->comp.type == CODA_DEBUG) {
      XCopyArea(dpy,btn_pixmaps.debug,win,gc,0,0,50,61,x+wd-50,y);
    } else if (draw_comp->comp.type == CODA_MON) {
      XCopyArea(dpy,btn_pixmaps.dd,win,gc,0,0,32,32,x+wd-32,y);
    } else if (draw_comp->comp.type == CODA_NONE) {
      XCopyArea(dpy,btn_pixmaps.trash,win,gc,0,0,33,46,x+wd-33,y);
    } else if (draw_comp->comp.type == CODA_UNKNOWN) {
      XCopyArea(dpy,btn_pixmaps.trash,win,gc,0,0,33,48,x+wd-33,y);
    }
    XSetForeground(dpy, gc, xgc.Black);
    XDrawRectangle(dpy, win,gc,x,y,wd-1,ht*draw_comp->num_row-1);

    i = sw_geometry.zoomscale - 1;
    if (i>0) {
      XSetForeground(dpy, gc, xgc.Black);
      XSetFont(dpy, gc, xgc.font[2]->fid);
      if (draw_comp->comp.code[0])
      {
	int j;
	for(j=strlen(draw_comp->comp.code[0]);j>0;j--)
	  if (draw_comp->comp.code[0][j] == '/') {
	    j++;
	    break;
	  }
	long_name = &draw_comp->comp.code[0][j];
	yy = xgc.font[2]->max_bounds.ascent + y + 2;
	xx = x +  xgc.font[2]->max_bounds.ascent/2;
	XDrawString(dpy, win, gc, xx, yy, long_name, strlen(long_name));
      }
      yy = ht - xgc.font[2]->max_bounds.ascent + y + 2;
      xx = x +  xgc.font[2]->max_bounds.ascent/2;
      long_name = names[draw_comp->comp.type];
      XDrawString(dpy, win, gc, xx, yy, long_name, strlen(long_name));

    }
  }  
}


/***************************************************************
 *      void draw_rubber_comp()                                *
 * Description:                                                *
 *    Draw a moving component                                  *
 ***************************************************************/
#if defined (__STDC__)
void draw_rubber_comp(drawComp* draw_comp, 
		      Display* dpy, 
		      Drawable win, 
		      int x, int y, 
		      Dimension wd, 
		      Dimension ht)
#else
void draw_rubber_comp(draw_comp, dpy, win, x, y, wd, ht)
     drawComp *draw_comp;
     Display *dpy;
     Drawable win;
     int      x, y;
     Dimension wd, ht;
#endif
{
  Dimension xdiv, ydiv;
  XPoint    pp[10];
  GC        gc = xgc.r_gc;
  int       xx, yy;
  Dimension rec_wd, rec_ht;
  Dimension text_wd, text_ht;
  Dimension cir_wd,cir_ht;
  int       delta_x;
  int       i;
  char *long_name = "UNKNOWN";
  char *short_name = "U";

  /* if either x or y is out of viewing region, do nothing */
  if (y < 0 || x < 0)
    return;

  XSetLineAttributes(dpy, gc, 1, LineSolid, CapButt, JoinRound);
  
  xdiv = wd/8;
  ydiv = ht/4;

  XSetForeground(dpy, gc, xgc.Black);
  if (draw_comp->comp.type < 11 ) {
    XSetForeground(dpy, gc, xgc.White);
    XFillRectangle(dpy, win,gc,x,y,wd,ht*draw_comp->num_row);
    
    XSetForeground(dpy, gc, xgc.Green);
    for(i=0;i<draw_comp->num_row;i++){
      xx = x;
      yy = y + 3*ydiv/2.0 + i*ht;
      if (draw_comp->comp.type > CODA_ROC)
	XFillRectangle(dpy, win, gc, xx, yy, xdiv, ydiv);
      xx = x + wd - xdiv ;
      yy = y + 3*ydiv/2.0 + i*ht;
      XFillRectangle(dpy, win, gc, xx, yy, xdiv, ydiv);
    }
    
    XSetForeground(dpy, gc, xgc.Black);
    XDrawRectangle(dpy, win,gc,x,y,wd-1,ht*draw_comp->num_row-1);
    
    rec_ht = ht/3.0;
    y -= 1;
    /*
    XSetForeground(dpy, gc, xgc.Red);
    XFillRectangle(dpy, win,gc,x,
		   y + ht*draw_comp->num_row - rec_ht,
		   wd-1,
		   rec_ht);
    XSetForeground(dpy, gc, xgc.Black);
    XDrawRectangle(dpy, win,gc,x,
		   y + ht*draw_comp->num_row - rec_ht,
		   wd-1,
		   rec_ht);*/
    if (draw_comp->comp.comp_name)
      setCompState(draw_comp->comp.comp_name,-1);

    i = sw_geometry.zoomscale - 1;
    if (i>0) {
      XSetForeground(dpy, gc, xgc.Black);
      XSetFont(dpy, gc, xgc.font[2]->fid);
      long_name = names[draw_comp->comp.type];
      yy = xgc.font[2]->max_bounds.ascent + y + 2;
      text_wd = XTextWidth(xgc.font[2], long_name, 3);
      xx = x + wd - text_wd - xgc.font[2]->max_bounds.ascent/2;
      XDrawString(dpy, win, gc, xx, yy, long_name, strlen(long_name));
    }
    {
      move_host_name(draw_comp,dpy,win, x, y, wd, ht);
      move_comp_name(draw_comp,dpy,win, x, y, wd, ht);
    }
  } else {
    long_name = draw_comp->comp.code[0];
    
    XSetForeground(dpy, gc, xgc.White);
    XFillRectangle(dpy, win,gc,x,y,wd,ht);
    XSetForeground(dpy, gc, xgc.Green);
    for(i=0;i<draw_comp->num_row;i++){
      xx = x;
      yy = y + 3*ydiv/2.0 + i*ht;
      XFillRectangle(dpy, win, gc, xx, yy, xdiv, ydiv);
    }
    if (draw_comp->comp.type == CODA_FILE) {
      XCopyArea(dpy,btn_pixmaps.file,win,gc,0,0,30,38,x+wd-30,y);
    } else if (draw_comp->comp.type == CODA_CODAFILE) {
      XCopyArea(dpy,btn_pixmaps.codaFile,win,gc,0,0,30,38,x+wd-30,y);
    } else if (draw_comp->comp.type == CODA_DEBUG) {
      XCopyArea(dpy,btn_pixmaps.debug,win,gc,0,0,50,61,x+wd-50,y);
    } else if (draw_comp->comp.type == CODA_MON) {
      XCopyArea(dpy,btn_pixmaps.dd,win,gc,0,0,32,32,x+wd-32,y);
    } else if (draw_comp->comp.type == CODA_NONE) {
      XCopyArea(dpy,btn_pixmaps.trash,win,gc,0,0,33,46,x+wd-33,y);
    } else if (draw_comp->comp.type == CODA_UNKNOWN) {
      XCopyArea(dpy,btn_pixmaps.trash,win,gc,0,0,33,48,x+wd-33,y);
    }
    
    XSetForeground(dpy, gc, xgc.Black);
    XDrawRectangle(dpy, win,gc,x,y,wd-1,ht*draw_comp->num_row-1);
    i = sw_geometry.zoomscale - 1;
    if ((i>0) && (long_name)) {
      XSetForeground(dpy, gc, xgc.Black);
      XSetFont(dpy, gc, xgc.font[2]->fid);
      yy = xgc.font[2]->max_bounds.ascent + y + 2;
      xx = x +  xgc.font[2]->max_bounds.ascent/2;
      XDrawString(dpy, win, gc, xx, yy, long_name, strlen(long_name));
      yy = ht - xgc.font[2]->max_bounds.ascent + y + 2;
      xx = x +  xgc.font[2]->max_bounds.ascent/2;
      long_name = names[draw_comp->comp.type];
      XDrawString(dpy, win, gc, xx, yy, long_name, strlen(long_name));
      
    }
    
  }  
}

/****************************************************************
 *           void draw_high_light(p, dpy, win)                  *
 * Description:                                                 *
 *     draw high lighted component                              *
 ***************************************************************/
#if defined (__STDC__)
void draw_high_light(drawComp* p, 
		     Display* dpy, 
		     Drawable win)
#else
void draw_high_light(p, dpy, win)
     drawComp *p;
     Display  *dpy;
     Drawable win;
#endif
{
  GC        gc = xgc.r_gc;
  (*p->draw_original)(p,dpy,win,p->x,p->y,p->width,p->height,
		      p->row, p->col);
  XSetForeground(dpy, gc, xgc.Black);
  XDrawRectangle(dpy, win,gc,p->x + 1,p->y+1,p->width-2,p->height*p->num_row-3);

}


/*******************************************************************
 *              void draw_unhigh_light(p, dpy, win)                *
 * Description:                                                    *
 *    draw original component                                      *
 ******************************************************************/
#if defined (__STDC__)
void draw_unhigh_light(drawComp* p,
		       Display* dpy,
		       Drawable win)
#else
void draw_unhigh_light(p,dpy,win)
     drawComp *p;
     Display  *dpy;
     Drawable win;
#endif
{
  (*p->draw_original)(p,dpy,win,p->x,p->y,p->width,p->height,
		      p->row, p->col);
}


/**************************************************************
 *  void erase_comp()                                         *
 * Description:                                               *
 *    erase a particular component                            *
 *************************************************************/
#if defined (__STDC__)
void erase_comp(drawComp* p, 
		Display* dpy, 
		Drawable win)
#else
void erase_comp(p, dpy, win)
     drawComp *p;
     Display *dpy;
     Drawable win;
#endif
{
  GC        gc = xgc.erase_gc;
  int       x, y;
  int       wd, ht;
  int       width, height;

  wd = p->width;
  ht = p->height;
  x  = p->x;
  y  = p->y;
  width = wd*p->num_col;
  height = ht*p->num_row;
  XSetForeground(dpy, gc, xgc.bg);
  XFillRectangle(dpy, win, gc, x, y, width, height);
  {
    ipPort   *port;
    int      i;
    for(i=0;i <p->num_ports; i++){
      port = &(p->ip_port[i]);
      (*port->erase_name)(port, dpy, win);
    }
    erase_host_name(p,dpy,win);
    erase_comp_name(p,dpy,win);
  }
  {
    drawComp *comp;
    compList *p2;
    
    for(p2 = coda_graph.comp_list_head->next; p2 != coda_graph.comp_list_tail;p2=p2->next){
      comp = p2->draw_comp;
      if (p != comp) {
	if ((x == comp->x) && (y == comp->y)) {
	  draw_original(comp,
			dpy,
			win,
			x,
			y,
			comp->width,
			comp->height,
			comp->row,
			comp->col);
	}
      }
    }
  }
}

/**************************************************************
 *  void setCompState()                                         *
 * Description:                                               *
 *    erase a particular component                            *
 *************************************************************/
#if defined (__STDC__)
void setCompState(char *name,int state)
#else
     void setCompState(name,state)
     char *name;
     int state;
#endif
{
  compList *p2;
  for(p2 = coda_graph.comp_list_head->next; p2 != coda_graph.comp_list_tail;p2=p2->next){
    drawComp *comp;
    daqComp *daq;
    comp = p2->draw_comp;
    daq = &comp->comp;

    if ((daq->comp_name) && (strcmp(daq->comp_name,name) == 0)) {
      int xx, yy, text_wd, text_ht;
      GC  gc = xgc.r_gc;
      Drawable win = XtWindow(sw_geometry.draw_area);  
  
      int x = comp->x;
      int y = comp->y;
      int ht = comp->height;
      int wd = comp->width;
 
      if ((daq->status <=0) || ( daq->status >=DA_STATES))
	daq->status = 0;

      if ((state <=0) || ( state >=DA_STATES))
	state = daq->status;
      else
	daq->status = state;

      y -= 1;
      x += 1;

      switch (state) {
      case DA_BOOTING:
	XSetForeground(xgc.dpy, xgc.r_gc, xgc.Yellow);
	break;
      case DA_BOOTED:
	XSetForeground(xgc.dpy, xgc.r_gc, xgc.Yellow);
	break;
      case DA_CONFIGURING:
	XSetForeground(xgc.dpy, xgc.r_gc, xgc.Yellow);
	break;
      case DA_CONFIGURED:
	XSetForeground(xgc.dpy, xgc.r_gc, xgc.Yellow);
	break;
      case DA_DOWNLOADED:
	XSetForeground(xgc.dpy, xgc.r_gc, xgc.White);
	break;
      case DA_PRESTARTING:
	XSetForeground(xgc.dpy, xgc.r_gc, xgc.Cyan);
	break;
      case DA_PAUSED:
	XSetForeground(xgc.dpy, xgc.r_gc, xgc.Paused);
	break;
      case DA_PAUSING:
	XSetForeground(xgc.dpy, xgc.r_gc, xgc.White);
	break;
      case DA_ACTIVATING:
	XSetForeground(xgc.dpy, xgc.r_gc, xgc.White);
	break;
      case DA_ACTIVE:
	XSetForeground(xgc.dpy, xgc.r_gc, xgc.Green);
	break;
      case DA_ENDING:
	XSetForeground(xgc.dpy, xgc.r_gc, xgc.Ending);
	break;
      case DA_VERIFYING:
	XSetForeground(xgc.dpy, xgc.r_gc, xgc.White);
	break;
      case DA_VERIFIED:
	XSetForeground(xgc.dpy, xgc.r_gc, xgc.White);
	break;
      case DA_TERMINATING:
	XSetForeground(xgc.dpy, xgc.r_gc, xgc.Red);
	break;
      case DA_PRESTARTED:
	XSetForeground(xgc.dpy, xgc.r_gc, xgc.Paused);
	break;
      case DA_RESUMING:
	XSetForeground(xgc.dpy, xgc.r_gc, xgc.White);
	break;
      case DA_UNKNOWN:
	continue;
      default:
	XSetForeground(xgc.dpy, xgc.r_gc, xgc.Red);
      }

      XFillRectangle(xgc.dpy, win,xgc.r_gc,x,
		     y + ht*comp->num_row - ht/3,
		     wd - 2,
		     ht/3);

      XSetForeground(xgc.dpy, xgc.r_gc, xgc.Black);
      XDrawRectangle(xgc.dpy, win,xgc.r_gc,x,
		     y + ht*comp->num_row - ht/3,
		     wd-2,
		     ht/3);
      
      XSetForeground(xgc.dpy, xgc.r_gc, xgc.Black);
      text_wd = XTextWidth(xgc.font[2], sN[state], 
			   strlen(sN[state]));
      xx = comp->x + (wd - text_wd)/2;
      yy = comp->y + ht*comp->num_row - ht/3 + xgc.font[2]->max_bounds.ascent + 2;
      
      XSetFont(xgc.dpy, xgc.r_gc, xgc.font[2]->fid);

      XDrawString(xgc.dpy, win, xgc.r_gc, xx, yy, sN[state], strlen(sN[state]));
    }
  }
}

/***************************************************************
 *     void draw_comp_name(comp, dpy, win)                     *
 * Description:                                                *
 *     draw component name underneath the block                *
 **************************************************************/
#if defined (__STDC__)
void draw_comp_name(drawComp* comp, 
		    Display *dpy,
		    Drawable win)
#else
void draw_comp_name(comp, dpy, win)
     drawComp *comp;
     Display  *dpy;
     Drawable win;
#endif
{
  int xx, yy, text_wd, text_ht;
  int i = sw_geometry.zoomscale - 1;
  GC  gc = xgc.r_gc;
  daqComp *daq = &(comp->comp);
  
  if(daq->comp_name != NULL){
    XSetForeground(dpy, gc, xgc.Black);
    text_wd = XTextWidth(xgc.font[2], daq->comp_name, 
			 strlen(daq->comp_name));
    /*if(text_wd < comp->width)
      xx = (comp->width - text_wd)/2.0 + comp->x;
      else*/
    xx = comp->x + comp->width/8 + xgc.font[2]->max_bounds.ascent/2;
    yy = comp->y + xgc.font[2]->max_bounds.ascent + 2;
    XSetFont(dpy, gc, xgc.font[2]->fid);
    XDrawString(dpy, win, gc, xx, yy, daq->comp_name, strlen(daq->comp_name));
  }
}

/***************************************************************
 *     void erase_comp_name(comp, dpy, win)                    *
 * Descritpion:                                                *
 *     erase component name                                    *
 **************************************************************/
#if defined (__STDC__)
void erase_comp_name(drawComp* comp, 
		     Display *dpy, 
		     Drawable win)
#else
void erase_comp_name(comp, dpy, win)
     drawComp *comp;
     Display  *dpy;
     Drawable win;
#endif
{
  int xx, yy, text_wd, text_ht;
  int i = sw_geometry.zoomscale - 1;
  GC  gc = xgc.erase_gc;
  daqComp *daq = &(comp->comp);

  if(daq->comp_name != NULL){
    if(i > 0){
      text_ht = XTextHeight(xgc.font[2]) + 4;
      text_wd = XTextWidth(xgc.font[2], daq->comp_name, 
			   strlen(daq->comp_name));

      xx = comp->x + +comp->width/8 + xgc.font[2]->max_bounds.ascent/2;
      yy = comp->y + xgc.font[2]->max_bounds.ascent + 2;
      XSetFont(dpy, gc, xgc.font[2]->fid);
      XDrawString(dpy, win, gc, xx, yy, daq->comp_name, strlen(daq->comp_name));
      /*XFillRectangle(dpy, win, gc, xx, yy, text_wd, text_ht);*/
    }
  }
}

/***************************************************************
 *     void move_comp_name(comp, dpy, win,x ,y, wd ,ht)        *
 * Descritpion:                                                *
 *     rubber band moving component name                       *
 **************************************************************/
#if defined (__STDC__)
void move_comp_name(drawComp* comp, 
		    Display *dpy, 
		    Drawable win, 
		    int x, int y,
		    Dimension wd, 
		    Dimension ht)
#else
void move_comp_name(comp, dpy, win, x, y, wd, ht)
     drawComp  *comp;
     Display   *dpy;
     Drawable  win;
     int       x, y;
     Dimension wd, ht;
#endif
{
  int xx, yy, text_wd, text_ht;
  int i = sw_geometry.zoomscale - 1;
  GC  gc = xgc.xor_gc;
  daqComp *daq = &(comp->comp);

  if(daq->comp_name != NULL){
    if(i > 0){
      XSetForeground(dpy, gc, xgc.Black);
      text_ht = XTextHeight(xgc.font[2]);
      text_wd = XTextWidth(xgc.font[2], daq->comp_name, 
			   strlen(daq->comp_name));
      xx = x + comp->width/8 + xgc.font[2]->max_bounds.ascent/2;
      yy = y + xgc.font[2]->max_bounds.ascent + 2;
      XSetFont(dpy, gc, xgc.font[2]->fid);
      XDrawString(dpy, win, gc, xx, yy, daq->comp_name, strlen(daq->comp_name));      
    }
  }
}

/*********************************************************************
 *       void draw_host_name (drawComp, dpy, win)                    *
 * Description:                                                      *
 *     Draw A component host name to the top of component            *
 ********************************************************************/
#if defined (__STDC__)
void draw_host_name(drawComp* comp, 
		    Display* dpy, 
		    Drawable win)
#else
void draw_host_name(comp, dpy, win)
     drawComp *comp;
     Display  *dpy;
     Drawable win;
#endif
{
  int xx, yy, text_wd, text_ht;
  int i = sw_geometry.zoomscale - 1;
  GC  gc = xgc.r_gc;
  daqComp *daq = &(comp->comp);
  
  if(daq->node_name != NULL){
    if(i > 0){
      XSetForeground(dpy, gc, xgc.Black);
      text_wd = XTextWidth(xgc.font[2], daq->node_name, 
			   strlen(daq->node_name));

      xx = comp->x + comp->width/8 +xgc.font[2]->max_bounds.ascent/2;
      yy = comp->y + xgc.font[2]->max_bounds.ascent*2 + 4;
      XSetFont(dpy, gc, xgc.font[2]->fid);
      XDrawString(dpy, win, gc, xx, yy, daq->node_name, 
		  strlen(daq->node_name));
    }
  }
}

/***************************************************************
 *     void erase_host_name(comp, dpy, win)                    *
 * Descritpion:                                                *
 *     erase component host name                               *
 **************************************************************/
#if defined (__STDC__)
void erase_host_name(drawComp* comp, 
		     Display* dpy, 
		     Drawable win)
#else
void erase_host_name(comp, dpy, win)
     drawComp *comp;
     Display  *dpy;
     Drawable win;
#endif
{
  int xx, yy, text_wd, text_ht;
  int i = sw_geometry.zoomscale - 1;
  GC  gc = xgc.erase_gc;
  daqComp *daq = &(comp->comp);

  if(daq->node_name != NULL){
    if(i > 0){
      text_ht = XTextHeight (xgc.font[2]);
      text_wd = XTextWidth(xgc.font[2], daq->node_name, 
			   strlen(daq->node_name));

      xx = comp->x + comp->width/8 + xgc.font[2]->max_bounds.ascent/2;
      yy = comp->y + xgc.font[2]->max_bounds.ascent*2 + 4;
      /*XFillRectangle(dpy, win, gc, xx, yy, text_wd, text_ht);*/
    }
  }
}

/***************************************************************
 *     void move_host_name(comp, dpy, win,x ,y, wd ,ht)        *
 * Descritpion:                                                *
 *     rubber band moving component name                       *
 **************************************************************/
#if defined (__STDC__)
void move_host_name(drawComp* comp, 
		    Display* dpy, 
		    Drawable win, 
		    int x, int y, 
		    Dimension wd, 
		    Dimension ht)
#else
void move_host_name(comp, dpy, win, x, y, wd, ht)
     drawComp  *comp;
     Display   *dpy;
     Drawable  win;
     int       x, y;
     Dimension wd, ht;
#endif
{
  int xx, yy, text_wd, text_ht;
  int i = sw_geometry.zoomscale - 1;
  GC  gc = xgc.xor_gc;
  daqComp *daq = &(comp->comp);

  if(daq->node_name != NULL){
    if(i > 0){
      XSetForeground(dpy, gc, xgc.Black);
      text_wd = XTextWidth(xgc.font[2], daq->node_name, 
			   strlen(daq->node_name));

      xx = x + comp->width/8 + xgc.font[2]->max_bounds.ascent/2;
      yy = y + xgc.font[2]->max_bounds.ascent*2 + 4;
      XSetFont(dpy, gc, xgc.font[2]->fid);
      XDrawString(dpy, win, gc, xx, yy, daq->node_name, 
		  strlen(daq->node_name));      
    }
  }
}

      
/***************************************************************
 *     void arm_ip_port(port, dpy, win)                        *
 * Description:                                                *
 *    draw high_lighted ip_port                                *
 ***************************************************************/
#if defined (__STDC__)
void arm_ip_port(ipPort* port, 
		 Display* dpy, 
		 Drawable win)
#else
void arm_ip_port(port, dpy, win)
     ipPort *port;
     Display *dpy;
     Drawable win;
#endif
{
  GC gc = xgc.r_gc;
  int       xx, yy;
  Dimension wd, ht;

  if(port->left_selected){
    xx = port->left_x;
    yy = port->left_y;
    wd = port->width;
    ht = port->height;
    XSetForeground(dpy, gc, xgc.Black);
    XFillRectangle(dpy, win, gc, xx, yy, wd, ht);
  }
  else if(port->right_selected){
    xx = port->right_x;
    yy = port->right_y;
    wd = port->width;
    ht = port->height;
    XSetForeground(dpy, gc, xgc.Black);
    XFillRectangle(dpy, win, gc, xx, yy, wd, ht);    
  }
  else
    ;
}

/***************************************************************
 *     void high_light_input_port(port, dpy, win)              *
 * Description:                                                *
 *    draw high_lighted input ip_port                          *
 ***************************************************************/
#if defined (__STDC__)
void high_light_input_port(ipPort* port, 
			   Display* dpy, 
			   Drawable win)
#else
void high_light_input_port(port, dpy, win)
     ipPort *port;
     Display *dpy;
     Drawable win;
#endif
{
  GC gc = xgc.r_gc;
  int       xx, yy;
  Dimension wd, ht;

  xx = port->left_x;
  yy = port->left_y;
  wd = port->width;
  ht = port->height;
  XSetForeground(dpy, gc, xgc.Red);
  XFillRectangle(dpy, win, gc, xx, yy, wd, ht);
  
  xx = port->right_x;
  yy = port->right_y;
  wd = port->width;
  ht = port->height;
  XSetForeground(dpy, gc, xgc.Red);
  XFillRectangle(dpy, win, gc, xx, yy, wd, ht);    
}


/***************************************************************
 *     void high_light_output_port(port, dpy, win)             *
 * Description:                                                *
 *    draw high_lighted output ip_port                         *
 ***************************************************************/
#if defined (__STDC__)
void high_light_output_port(ipPort* port, 
			    Display* dpy, 
			    Drawable win)
#else
void high_light_output_port(port, dpy, win)
     ipPort *port;
     Display *dpy;
     Drawable win;
#endif
{
  GC gc = xgc.r_gc;
  int       xx, yy;
  Dimension wd, ht;

  xx = port->left_x;
  yy = port->left_y;
  wd = port->width;
  ht = port->height;
  XSetForeground(dpy, gc, xgc.Cyan);
  XFillRectangle(dpy, win, gc, xx, yy, wd, ht);

  xx = port->right_x;
  yy = port->right_y;
  wd = port->width;
  ht = port->height;
  XSetForeground(dpy, gc, xgc.Cyan);
  XFillRectangle(dpy, win, gc, xx, yy, wd, ht);    
  
}

/***************************************************************
 *         void disarm_ip_port(port, dpy, win)                 *
 * Description:                                                *
 *     draw disarmed ip_port                                   *
 ***************************************************************/
#if defined (__STDC__)
void disarm_ip_port(ipPort* port, 
		    Display* dpy, 
		    Drawable win)
#else
void disarm_ip_port(port, dpy, win)
     ipPort *port;
     Display *dpy;
     Drawable win;
#endif
{
  GC gc = xgc.r_gc;
  int       xx, yy;
  Dimension wd, ht;

  xx = port->left_x;
  yy = port->left_y;
  wd = port->width;
  ht = port->height;
  XSetForeground(dpy, gc, xgc.Green);
  XFillRectangle(dpy, win, gc, xx, yy, wd, ht);
  port->left_selected = 0;

  
  xx = port->right_x;
  yy = port->right_y;
  wd = port->width;
  ht = port->height;
  XSetForeground(dpy, gc, xgc.Green);
  XFillRectangle(dpy, win, gc, xx, yy, wd, ht);    
  port->right_selected = 0;
}

/*********************************************************************
 *       void draw_port_name(port, dpy, win)                         *
 * Description:                                                      *
 *     Draw port name to the right side of the port                  *
 ********************************************************************/
#if defined (__STDC__)
void draw_port_name(ipPort* port, Display* dpy, Drawable win)
#else
void draw_port_name(port, dpy, win)
     ipPort   *port;
     Display  *dpy;
     Drawable win;
#endif
{
  int xx, yy, text_height;
  int i = sw_geometry.zoomscale - 1;
  GC  gc = xgc.r_gc;
  daqComp *daq = port->comp_ptr;

  XSetForeground(dpy, gc, xgc.White);
  xx = port->right_x + port->width + 2;
  yy = port->right_y - 2;
  
  if(port->ip_addr != NULL){
    XSetFont(dpy, gc, xgc.font[2]->fid);
    XDrawString(dpy, win, gc, xx, yy, port->ip_addr, 
		strlen(port->ip_addr));
  }
}

/********************************************************************
 *       void erase_port_name(port, dpy, win)                       *
 * Description:                                                     *
 *     erase old name from old position                             *
 *******************************************************************/
#if defined (__STDC__)
void erase_port_name(ipPort* port, Display* dpy, Drawable win)
#else
void erase_port_name(port, dpy, win)
     ipPort  *port;
     Display *dpy;
     Drawable win;
#endif
{
  int xx, yy, text_ht, text_wd;
  int i = sw_geometry.zoomscale - 1;
  GC  gc = xgc.erase_gc;
  daqComp *daq = port->comp_ptr;

  if(port->ip_addr != NULL){
    text_ht = XTextHeight(xgc.font[2])+ 2;
    text_wd = XTextWidth(xgc.font[2],port->ip_addr, strlen(port->ip_addr));
    xx = port->right_x + 2 + port->width;
    yy = port->right_y - 2 - text_ht;
    /*XFillRectangle(dpy, win, gc, xx, yy, text_wd, text_ht);*/
    XSetFont(dpy, gc, xgc.font[2]->fid);
    XDrawString(dpy, win, gc, xx, yy, port->ip_addr, 
		strlen(port->ip_addr));
  }  
}

/*********************************************************************
 *       void draw_rubber_name(port, dpy, win, x, y, wd, ht)         *
 * Description:                                                      *
 *    Draw movable port name. x, y are the top left corner           *
 ********************************************************************/
#if defined (__STDC__)
void draw_rubber_name(ipPort* port, Display* dpy, 
		      Drawable win, 
		      int x, int y, 
		      Dimension wd, Dimension ht)
#else
void draw_rubber_name(port, dpy, win, x, y, wd, ht)
     ipPort   *port;
     Display  *dpy;
     Drawable  win;
     int       x, y;
     Dimension wd, ht;
#endif
{
  Dimension xdiv, ydiv;
  unsigned short  i = sw_geometry.zoomscale - 1;
  GC  gc = xgc.xor_gc;
  int xx, yy;
  daqComp *daq = port->comp_ptr; 

  xdiv = wd/8.0;
  ydiv = ht/8.0;
  xx = x + wd - 2*xdiv;
  yy = y + 3*ydiv - 2;
  
  xx = x + port->width + 2;
  yy = y - 2;
  
  XSetForeground(dpy, gc, xgc.Black);
  if(port->ip_addr != NULL)
    XSetFont(dpy, gc, xgc.font[2]->fid);
  XDrawString(dpy, win, gc, xx, yy, port->ip_addr, 
	      strlen(port->ip_addr));
}

/*********************************************************************
 *       void draw_original_arc(arc, dpy, win)                       *
 * Description:                                                      *
 *     Draw a directed arc from one port to another                  *
 *********************************************************************/
#if defined (__STDC__)
void draw_original_arc(Arc* arc, Display* dpy, Drawable win)
#else
void draw_original_arc(arc, dpy, win)
     Arc      *arc;
     Display  *dpy;
     Drawable win;
#endif
{
  GC     gc = xgc.r_gc;

  XSetForeground(dpy, gc, xgc.Red);
  if(arc->type == IP_CONNECTION){
    if(sw_geometry.zoomscale < 4)
      XSetLineAttributes(dpy, gc, 1, LineSolid, CapButt,
			 JoinRound);
    else
      XSetLineAttributes(dpy, gc, 2, LineSolid, CapButt,
			 JoinRound);
  }
  else{
    if(sw_geometry.zoomscale < 4)
      XSetLineAttributes(dpy, gc, 1, LineOnOffDash, CapButt,
			 JoinRound);
    else
      XSetLineAttributes(dpy, gc, 2, LineOnOffDash, CapButt,
			 JoinRound);
  }
  XDrawLines(dpy, win, gc, arc->edge_point, 6, CoordModeOrigin);
  draw_arrow(arc, dpy, win, gc, arc->edge_point[5].x, arc->edge_point[5].y,
	     sw_geometry.zoomscale, arc->arrow_type);
}

/*********************************************************************
 *       void draw_rubber_arc(arc, dpy, win)                         *
 * Description:                                                      *
 *     Draw a moving arc                                             *
 *********************************************************************/
#if defined (__STDC__)
void draw_rubber_arc(Arc* arc, Display* dpy, 
		     Drawable win, int x, int y)
#else
void draw_rubber_arc(arc, dpy, win, x, y)
     Arc      *arc;
     Display  *dpy;
     Drawable win;
     int      x, y;
#endif
{
  GC     gc = xgc.xor_gc;
  int    xx, yy;
  
  XSetForeground(dpy, gc, xgc.Cyan^xgc.NavyBlue);

  if(arc->type == IP_CONNECTION){
    if(sw_geometry.zoomscale < 4)
      XSetLineAttributes(dpy, gc, 1, LineSolid, CapButt,
			 JoinRound);
    else
      XSetLineAttributes(dpy, gc, 2, LineSolid, CapButt,
			 JoinRound);
  }
  else{
    if(sw_geometry.zoomscale < 4)
      XSetLineAttributes(dpy, gc, 1, LineOnOffDash, CapButt,
			 JoinRound);
    else
      XSetLineAttributes(dpy, gc, 2, LineOnOffDash, CapButt,
			 JoinRound);
  }

  if(arc->edge_point[0].x >= arc->edge_point[6].x){
    if(x >= arc->edge_point[0].x)
      xx = arc->edge_point[0].x - 10;
    else if(x <= arc->edge_point[6].x)
      xx = arc->edge_point[6].x + 10;
    else 
      xx = x;
  }
  else{
    if(x >= arc->edge_point[6].x)
      xx = arc->edge_point[6].x - 10;
    else if(x <= arc->edge_point[0].x)
      xx = arc->edge_point[0].x + 10;
    else
      xx = x;
  }
  XDrawLine(dpy, win, gc, xx, arc->edge_point[1].y,
	    xx, arc->edge_point[2].y);
  XDrawLine(dpy, win, gc, arc->edge_point[0].x, arc->edge_point[0].y,
	    xx, arc->edge_point[0].y);
  XDrawLine(dpy, win, gc, arc->edge_point[3].x, arc->edge_point[3].y,
	    xx, arc->edge_point[3].y);
}

/***********************************************************************
 *          void draw_high_light_arc(arc, dpy, win)                    *
 * Description:                                                        *
 *     High light a particuler arc                                     *
 **********************************************************************/
#if defined (__STDC__)
void draw_high_light_arc(Arc* arc, Display* dpy, Drawable win)
#else
void draw_high_light_arc(arc, dpy, win)
     Arc     *arc;
     Display *dpy;
     Drawable win;
#endif
{
  GC     gc = xgc.r_gc;
  
  XSetForeground(dpy, gc, xgc.Red);
  if(arc->type == IP_CONNECTION){
    if(sw_geometry.zoomscale < 4)
      XSetLineAttributes(dpy, gc, 1, LineSolid, CapButt,
			 JoinRound);
    else
      XSetLineAttributes(dpy, gc, 2, LineSolid, CapButt,
			 JoinRound);
  }
  else{
    if(sw_geometry.zoomscale < 4)
      XSetLineAttributes(dpy, gc, 1, LineOnOffDash, CapButt,
			 JoinRound);
    else
      XSetLineAttributes(dpy, gc, 2, LineOnOffDash, CapButt,
			 JoinRound);
  }
  XDrawLines(dpy, win, gc, arc->edge_point, 6, CoordModeOrigin);
  draw_arrow(arc, dpy, win, gc, arc->edge_point[3].x, arc->edge_point[3].y,
	     sw_geometry.zoomscale, arc->arrow_type);
}  


/***********************************************************************
 *      void erase_arc(arc, dpy, win)                                  *
 * Description:                                                        *
 *     erase arc by using background                                   * 
 **********************************************************************/
#if defined (__STDC__)
void erase_arc(Arc* arc, Display* dpy, Drawable win)
#else
void erase_arc(arc, dpy, win)
     Arc *arc;
     Display *dpy;
     Drawable win;
#endif
{
  GC gc = xgc.erase_gc;

  if(arc->type == IP_CONNECTION){
    if(sw_geometry.zoomscale < 4)
      XSetLineAttributes(dpy, gc, 1, LineSolid, CapButt,
			 JoinRound);
    else
      XSetLineAttributes(dpy, gc, 2, LineSolid, CapButt,
			 JoinRound);
  }
  else{
    if(sw_geometry.zoomscale < 4)
      XSetLineAttributes(dpy, gc, 1, LineOnOffDash, CapButt,
			 JoinRound);
    else
      XSetLineAttributes(dpy, gc, 2, LineOnOffDash, CapButt,
			 JoinRound);
  }
  XDrawLines(dpy, win, gc, arc->edge_point, 6, CoordModeOrigin);
  draw_arrow(arc, dpy, win, gc, arc->edge_point[3].x, arc->edge_point[3].y,
	     sw_geometry.zoomscale, arc->arrow_type);
}




