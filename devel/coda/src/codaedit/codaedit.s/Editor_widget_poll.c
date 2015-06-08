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
 *	xcefdmp all widgets are kept here
 *	
 * Author:  Jie Chen
 *          CEBAF Data Acquisition Group
 *
 * Revision History:
 *   $Log: Editor_widget_poll.c,v $
 *   Revision 1.1.1.1  1996/08/21 19:36:03  heyes
 *   Imported sources
 *
 *	  
 */
#include <stdio.h>
#include <string.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <Xm/Xm.h>

#include "Ruler.h"

#include "Editor_graph.h"

#define MAX_NUM_WIDGETS 100

static Widget widget_poll[MAX_NUM_WIDGETS];
static int    num_widgets = 0;

#if defined (__STDC__)
void init_widget_poll (void)
#else
void init_widget_poll ()
#endif
{
  int i = 0;

  for (i = 0; i < MAX_NUM_WIDGETS; i++)
    widget_poll[i] = 0;
}

#if defined (__STDC__)
void add_widget_to_poll (Widget w)
#else
void add_widget_to_poll (w)
Widget w;
#endif
{
  if (num_widgets >= MAX_NUM_WIDGETS - 1){
    fprintf(stderr," Cannot hold any more widgets, exit \n");
    exit (1);
  }
  widget_poll[num_widgets++] = w;
}

#if defined (__STDC__)
void change_color(Pixel col, Screen* scr,int type)
#else
void change_color(col,scr,type)
Pixel   col;
Screen  *scr;
int     type;  /* 0 for foreground, 1 for background */
#endif
{
  XGCValues gcv;
  Arg arg[10];
  int ac = 0, i;
  Pixel unused;
  Pixel fg, bg;
  Pixel fg_ret, top_shadow, bot_shadow, sel_color;

  if (type == 1){
    bg = col;
    fg = xgc.fg;
    XmGetColors (scr, DefaultColormapOfScreen (scr), bg,
		 &fg_ret, &top_shadow, &bot_shadow, &sel_color);

    XtSetArg (arg[ac], XmNbackground, bg); ac++;
    XtSetArg (arg[ac], XmNtopShadowColor, top_shadow); ac++;
    XtSetArg (arg[ac], XmNbottomShadowColor, bot_shadow); ac++;
    XtSetArg (arg[ac], XmNselectColor, sel_color); ac++;
    XtSetArg (arg[ac], XmNarmColor, sel_color); ac++;
    XtSetArg (arg[ac], XmNborderColor, fg_ret); ac++;
    for (i = 0; i < num_widgets; i++)
      XtSetValues (widget_poll[i], arg, ac);
    ac = 0;
  }
  else{
    fg = col;
    bg = xgc.bg;

    XtSetArg (arg[ac], XmNforeground, fg); ac++;
    XtSetArg (arg[ac], XcodaNindicatorColor, fg); ac++;
    XtSetArg (arg[ac], XcodaNtickerColor, fg); ac++;
    for (i = 0; i < num_widgets; i++)
      XtSetValues (widget_poll[i], arg, ac);
    ac = 0;
  }    
  /* change background pixmap and all the GC */
  XFreeGC (xgc.dpy, xgc.erase_gc);
  XFreeGC (xgc.dpy, xgc.r_gc);
  XFreeGC (xgc.dpy, xgc.hl_gc);
  XFreeGC (xgc.dpy, xgc.xor_gc);
  
  xgc.NavyBlue = bg;
  xgc.bg = bg;
  xgc.fg = fg;
    
  gcv.foreground = bg;
  gcv.background = bg;
  xgc.erase_gc = XCreateGC(xgc.dpy, xgc.root_w,GCForeground|GCBackground,
			   &gcv);

  gcv.foreground = fg;
  xgc.r_gc = XCreateGC(xgc.dpy, xgc.root_w,GCForeground|GCBackground,
		       &gcv);

  gcv.foreground = bg;
  gcv.background = fg;
  xgc.hl_gc = XCreateGC(xgc.dpy, xgc.root_w,GCForeground|GCBackground,
			&gcv);

  gcv.foreground = fg^bg;
  gcv.background = bg;
  gcv.function = GXxor;
  xgc.xor_gc = XCreateGC(xgc.dpy, xgc.root_w, GCFunction|GCForeground|
			 GCBackground,&gcv);
    
  XcodaEditorBgPixmap (sw_geometry.draw_area);
}

#if defined (__STDC__)
void saveColor (XColor* col, int type)
#else
void saveColor (col, type)
XColor *col;
int    type;
#endif
{
  char *home;
  char fullname[128], line[128];
  char file_lines[10][128];
  int  num_lines = 0, i = 0;
  FILE *fd;
  extern char *getenv();

  home = getenv ("HOME");
  if (!home)
    return;
  strcpy (fullname, home);
  strcat (fullname, "/.netEditor");
  
  fd = fopen (fullname,"r");
  if (fd){
    while (!feof(fd)){
      line[0] = '\0';
      fgets(line, sizeof(line), fd);
      strcpy (file_lines[i++], line);
    }
    num_lines = i;
  }
  fclose (fd);

  fd = fopen (fullname, "w");
  if (fd == NULL){
    fprintf(stderr,"Cannot open file %s \n",fullname);
    exit (1);
  }
  if (type == 1){
    fprintf(fd,"background       rgb:%x/%x/%x\n",
	    col->red, col->green, col->blue);
    for (i = 0; i < num_lines; i++){
      if (strstr(file_lines[i], "background") == 0)
	fprintf(fd, "%s", file_lines[i]);
    }
  }
  else{
    for (i = 0; i < num_lines; i++){
      if (strstr(file_lines[i], "foreground") == 0)
	fprintf(fd, "%s", file_lines[i]);    
    }
    fprintf(fd,"foreground       rgb:%x/%x/%x\n",
	    col->red, col->green, col->blue);
  }
  fclose (fd);
}

