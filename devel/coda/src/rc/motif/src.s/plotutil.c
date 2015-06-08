/*
 * Copyright (C) 1996 by Rob McMullen
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
 *
 * Author: Rob McMullen <rwmcm@orion.ae.utexas.edu>
 *         http://www.ae.utexas.edu/~rwmcm
 */

#include <stdio.h>
#include <time.h>
#include <X11/Intrinsic.h>
#include <X11/Shell.h>
#include <X11/StringDefs.h>
#include <Xm/Xm.h>
#include <Xm/Label.h>
#include <Xm/TextF.h>
#include <Xm/RowColumn.h>
#include <Xm/Form.h>
#include <Xm/PushB.h>
#include <Xm/DialogS.h>
#include <Xm/MainW.h>
#include <Xm/PanedW.h>

#include "SciPlot.h"
#include "strutil.h"
#include "reader.h"

/* Globals */
static int DialogCount=0;		/* Number of dialogs on screen */


typedef struct {
  char *text;
  int num;
} textnumpair;

textnumpair colors_list[] =
{
  {"blue", 0},
  {"red", 0},
  {"ForestGreen", 0},
  {"DarkGoldenrod", 0},
  {"orange", 0},
  {"magenta", 0},
  {"grey", 0},
  {"purple", 0},
};

#define NUM_COLORS 8

textnumpair marker_list[] =
{
  {"XtMARKER_SQUARE", XtMARKER_SQUARE},
  {"XtMARKER_CIRCLE", XtMARKER_CIRCLE},
  {"XtMARKER_UTRIANG ", XtMARKER_UTRIANGLE},
  {"XtMARKER_DTRIANG ", XtMARKER_DTRIANGLE},
  {"XtMARKER_DIAMOND", XtMARKER_DIAMOND},
  {"XtMARKER_HOURGLA ", XtMARKER_HOURGLASS},
  {"XtMARKER_BOWTIE", XtMARKER_BOWTIE},
  {"XtMARKER_LTRIANG ", XtMARKER_LTRIANGLE},
  {"XtMARKER_RTRIANG", XtMARKER_RTRIANGLE},
  {"XtMARKER_FCIRCLE ", XtMARKER_FCIRCLE},
  {"XtMARKER_FSQUARE", XtMARKER_FSQUARE},
  {"XtMARKER_FUTRIAN", XtMARKER_FUTRIANGLE},
  {"XtMARKER_FDTRIAN ", XtMARKER_FDTRIANGLE},
  {"XtMARKER_FDIAMON ", XtMARKER_FDIAMOND},
  {"XtMARKER_FHOURGL", XtMARKER_FHOURGLASS},
  {"XtMARKER_FBOWTIE", XtMARKER_FBOWTIE},
  {"XtMARKER_FLTRIAN", XtMARKER_FLTRIANGLE},
  {"XtMARKER_FRTRIAN", XtMARKER_FRTRIANGLE}
};

#define NUM_MARKERS 18


/*
 *  Utils
 */
Widget
GetToplevel(Widget w)
{
  Widget p = w;

  while (w != None) {
    p = w;
    w = XtParent(w);
  }
  return p;
}

Widget
GetShell(Widget w)
{
  while (!XtIsShell(w))
    w = XtParent(w);
  return w;
}


typedef struct {
  Widget label1;
  Widget label2;
  Widget label3;
  Widget title;
  Widget xlabel;
  Widget ylabel;
  Widget shell;
  Widget plot;
  Widget text;
} PlotDialogData;

static void
QuitCallback(Widget btn, XtPointer client, XtPointer call)
{
  exit(0);
}

static void
DismissCallback(Widget btn, XtPointer client, XtPointer call)
{
  PlotDialogData *stuff = (PlotDialogData *) client;

  XtPopdown(stuff->shell);
  XtDestroyWidget(stuff->shell);
  free(stuff);
  DialogCount--;
  if (DialogCount == 0)
    exit(0);
}

static void
LogXCallback(Widget btn, XtPointer client, XtPointer call)
{
  PlotDialogData *stuff = (PlotDialogData *) client;
  Boolean state;

  XtVaGetValues(stuff->plot,
    XtNxLog,        &state,
    NULL);
  state=!state;
  XtVaSetValues(stuff->plot,
    XtNxLog,        state,
    NULL);
}

static void
LogYCallback(Widget btn, XtPointer client, XtPointer call)
{
  PlotDialogData *stuff = (PlotDialogData *) client;
  Boolean state;

  XtVaGetValues(stuff->plot,
    XtNyLog,        &state,
    NULL);
  state=!state;
  XtVaSetValues(stuff->plot,
    XtNyLog,        state,
    NULL);
}

static void
NumbersXCallback(Widget btn, XtPointer client, XtPointer call)
{
  PlotDialogData *stuff = (PlotDialogData *) client;
  Boolean state;

  XtVaGetValues(stuff->plot,
    XtNxAxisNumbers,        &state,
    NULL);
  state=!state;
  XtVaSetValues(stuff->plot,
    XtNxAxisNumbers,        state,
    NULL);
}

static void
NumbersYCallback(Widget btn, XtPointer client, XtPointer call)
{
  PlotDialogData *stuff = (PlotDialogData *) client;
  Boolean state;

  XtVaGetValues(stuff->plot,
    XtNyAxisNumbers,        &state,
    NULL);
  state=!state;
  XtVaSetValues(stuff->plot,
    XtNyAxisNumbers,        state,
    NULL);
}

static void
OriginXCallback(Widget btn, XtPointer client, XtPointer call)
{
  PlotDialogData *stuff = (PlotDialogData *) client;
  Boolean state;

  XtVaGetValues(stuff->plot,
    XtNxOrigin,        &state,
    NULL);
  state=!state;
  XtVaSetValues(stuff->plot,
    XtNxOrigin,        state,
    NULL);
}

static void
OriginYCallback(Widget btn, XtPointer client, XtPointer call)
{
  PlotDialogData *stuff = (PlotDialogData *) client;
  Boolean state;

  XtVaGetValues(stuff->plot,
    XtNyOrigin,        &state,
    NULL);
  state=!state;
  XtVaSetValues(stuff->plot,
    XtNyOrigin,        state,
    NULL);
}

static void
MajorCallback(Widget btn, XtPointer client, XtPointer call)
{
  PlotDialogData *stuff = (PlotDialogData *) client;
  Boolean state;

  XtVaGetValues(stuff->plot,
    XtNdrawMajor,        &state,
    NULL);
  state=!state;
  XtVaSetValues(stuff->plot,
    XtNdrawMajor,        state,
    NULL);
}

static void
MinorCallback(Widget btn, XtPointer client, XtPointer call)
{
  PlotDialogData *stuff = (PlotDialogData *) client;
  Boolean state;

  XtVaGetValues(stuff->plot,
    XtNdrawMinor,        &state,
    NULL);
  state=!state;
  XtVaSetValues(stuff->plot,
    XtNdrawMinor,        state,
    NULL);
}

static void
LegendCallback(Widget btn, XtPointer client, XtPointer call)
{
  PlotDialogData *stuff = (PlotDialogData *) client;
  Boolean state;

  XtVaGetValues(stuff->plot,
    XtNshowLegend,        &state,
    NULL);
  state=!state;
  XtVaSetValues(stuff->plot,
    XtNshowLegend,        state,
    NULL);
}

void
TitleChangeCallback(Widget btn, XtPointer client, XtPointer call)
{
  PlotDialogData *stuff = (PlotDialogData *) client;
  char *ret;

  ret=XmTextFieldGetString(btn);
  XtVaSetValues(stuff->plot,
    XtNplotTitle, ret,
    NULL);
  XtFree(ret);
}

static void
TitleCallback(Widget btn, XtPointer client, XtPointer call)
{
  PlotDialogData *stuff = (PlotDialogData *) client;
  Boolean state;

  XtVaGetValues(stuff->plot,
    XtNshowTitle,        &state,
    NULL);
  state=!state;
  XtVaSetValues(stuff->plot,
    XtNshowTitle,        state,
    NULL);
#ifdef DEBUG
  if (1) {
    char label[256],*txt1,*txt2,*txt3;
    static int count=0;
    
    sprintf(label,"Title #%d",++count);
    XtVaSetValues(stuff->plot,
      XtNplotTitle, label,
      NULL);
    XtVaGetValues(stuff->plot,
      XtNplotTitle, &txt1,
      XtNxLabel, &txt2,
      XtNyLabel, &txt3,
      NULL);
    printf("title=%s\nx label=%s\ny label=%s\n",txt);
#endif
}

void
LabelXChangeCallback(Widget btn, XtPointer client, XtPointer call)
{
  PlotDialogData *stuff = (PlotDialogData *) client;
  char *ret;

  ret=XmTextFieldGetString(btn);
  XtVaSetValues(stuff->plot,
    XtNxLabel, ret,
    NULL);
  XtFree(ret);
}

static void
LabelXCallback(Widget btn, XtPointer client, XtPointer call)
{
  PlotDialogData *stuff = (PlotDialogData *) client;
  Boolean state;

  XtVaGetValues(stuff->plot,
    XtNshowXLabel,        &state,
    NULL);
  state=!state;
  XtVaSetValues(stuff->plot,
    XtNshowXLabel,        state,
    NULL);
#ifdef DEBUG
  if (1) {
    char label[256];
    static int count=0;
    
    sprintf(label,"X Label #%d",++count);
    XtVaSetValues(stuff->plot,
      XtNxLabel, label,
      NULL);
  }
#endif
}

void
LabelYChangeCallback(Widget btn, XtPointer client, XtPointer call)
{
  PlotDialogData *stuff = (PlotDialogData *) client;
  char *ret;

  ret=XmTextFieldGetString(btn);
  XtVaSetValues(stuff->plot,
    XtNyLabel, ret,
    NULL);
  XtFree(ret);
}

static void
LabelYCallback(Widget btn, XtPointer client, XtPointer call)
{
  PlotDialogData *stuff = (PlotDialogData *) client;
  Boolean state;

  XtVaGetValues(stuff->plot,
    XtNshowYLabel,        &state,
    NULL);
  state=!state;
  XtVaSetValues(stuff->plot,
    XtNshowYLabel,        state,
    NULL);
#ifdef DEBUG
  if (1) {
    char label[256];
    static int count=0;
    
    sprintf(label,"Y Label #%d",++count);
    XtVaSetValues(stuff->plot,
      XtNyLabel, label,
      NULL);
  }
#endif
}

static void
PSCallback(Widget btn, XtPointer client, XtPointer call)
{
  PlotDialogData *stuff = (PlotDialogData *) client;
  char *filename;
  
#ifdef EXTRA_FANCY
  char txt[1024],*ptr;

  ptr=XmTextFieldGetString(stuff->label1);
  strcpy(txt,ptr);
  strcat(txt,"\n");
  XtFree(ptr);
  ptr=XmTextFieldGetString(stuff->label2);
  strcat(txt,ptr);
  strcat(txt,"\n\n");
  XtFree(ptr);
  ptr=XmTextFieldGetString(stuff->label3);
  strcat(txt,ptr);
  strcat(txt,"\n");
  XtFree(ptr);
                
  filename = XmTextFieldGetString(stuff->text);
  if (strlen(filename) > 0)
    SciPlotPSCreateFancy(stuff->plot,filename,
      TRUE,txt,NULL);
  XtFree(filename);
#else
  filename = XmTextFieldGetString(stuff->text);
  if (strlen(filename) > 0)
    SciPlotPSCreate(stuff->plot,filename);
  XtFree(filename);
#endif
}


/*
 * Create an plot dialog using Motif widgets
 */
PlotDialogData *
SciPlotDialogInternal(Widget parent, char *title)
{
  Widget paned, box, topbox, botbox, btn, t1;
  PlotDialogData *info;
  char *text;
#ifdef EXTRA_FANCY
  char buf[256];
  time_t  now;
  struct tm *t;
#endif
  
  info = (PlotDialogData *) malloc(sizeof(PlotDialogData));
  DialogCount++;

  text = malloc(strlen(title) + 256);
  sprintf(text, "Plot #%d: %s ", DialogCount, title);

  paned=XmCreateFormDialog(parent,text,NULL,0);
  info->shell=XtParent(paned);
  XtVaSetValues(paned,
    XmNdialogStyle,XmDIALOG_MODELESS,
    XmNnoResize,False,
    XmNautoUnmanage,False,
    NULL);
  
  topbox = box = XtVaCreateManagedWidget("box",
    xmRowColumnWidgetClass,paned,
    XmNleftAttachment, XmATTACH_FORM,
    XmNrightAttachment, XmATTACH_FORM,
    XmNtopAttachment, XmATTACH_FORM,
    XmNpacking,     XmPACK_COLUMN,
    XmNnumColumns,  2,
    XmNorientation, XmVERTICAL,
    NULL);

#ifdef EXTRA_FANCY
  btn = XtVaCreateManagedWidget("Desc 1:",
    xmLabelWidgetClass, box,
    NULL);
  btn = XtVaCreateManagedWidget("Desc 2:",
    xmLabelWidgetClass, box,
    NULL);
  btn = XtVaCreateManagedWidget("Desc 3:",
    xmLabelWidgetClass, box,
    NULL);
#endif
  btn = XtVaCreateManagedWidget("Xlabel:",
    xmLabelWidgetClass, box,
    NULL);
  btn = XtVaCreateManagedWidget("Ylabel:",
    xmLabelWidgetClass, box,
    NULL);
  btn = XtVaCreateManagedWidget("Title:",
    xmLabelWidgetClass, box,
    NULL);
  
#ifdef EXTRA_FANCY
  info->label1 = XtVaCreateManagedWidget("entry",
    xmTextFieldWidgetClass, box,
    XmNskipAdjust, True,
    NULL);
  info->label2 = XtVaCreateManagedWidget("entry",
    xmTextFieldWidgetClass, box,
    XmNskipAdjust, True,
    NULL);
  now=time(NULL);
  t=localtime(&now);
  strftime(buf,256,"%c",t);
  info->label3 = XtVaCreateManagedWidget("entry",
    xmTextFieldWidgetClass, box,
    XmNvalue, buf,
    XmNcursorPosition, 100,
    XmNskipAdjust, True,
    NULL);
#endif
  info->xlabel = XtVaCreateManagedWidget("entry",
    xmTextFieldWidgetClass, box,
    NULL);
  info->ylabel = XtVaCreateManagedWidget("entry",
    xmTextFieldWidgetClass, box,
    NULL);
  info->title = XtVaCreateManagedWidget("entry",
    xmTextFieldWidgetClass, box,
    NULL);
  XtAddCallback(info->xlabel, XmNactivateCallback, LabelXChangeCallback, (XtPointer) info);
  XtAddCallback(info->ylabel, XmNactivateCallback, LabelYChangeCallback, (XtPointer) info);
  XtAddCallback(info->title, XmNactivateCallback, TitleChangeCallback, (XtPointer) info);
  
  box = XtVaCreateManagedWidget("box",
    xmRowColumnWidgetClass,paned,
    XmNleftAttachment,      XmATTACH_FORM,
    XmNrightAttachment,     XmATTACH_FORM,
    XmNbottomAttachment,    XmATTACH_FORM,
    XmNorientation,         XmHORIZONTAL,
    XmNpacking,             XmPACK_TIGHT,
    NULL);
  btn = XtVaCreateManagedWidget("Quit",
    xmPushButtonWidgetClass, box,
    NULL);
  XtAddCallback(btn, XmNactivateCallback, QuitCallback, (XtPointer) NULL);

  btn = XtVaCreateManagedWidget("Dismiss",
    xmPushButtonWidgetClass, box,
    NULL);
  XtAddCallback(btn, XmNactivateCallback, DismissCallback, (XtPointer) info);

  btn = XtVaCreateManagedWidget("Make Postscript",
    xmPushButtonWidgetClass, box,
    NULL);
  sprintf(text, "plot%d.ps", DialogCount);
  t1 = XtVaCreateManagedWidget("entry",
    xmTextFieldWidgetClass, box,
    XmNwidth, 200,
    XmNvalue, text,
    XmNcursorPosition, 100,
    NULL);
  info->text = t1;
  XtAddCallback(btn, XmNactivateCallback, PSCallback, (XtPointer) info);

  botbox = box = XtVaCreateManagedWidget("box",
    xmRowColumnWidgetClass,paned,
    XmNleftAttachment,      XmATTACH_FORM,
    XmNrightAttachment,     XmATTACH_FORM,
    XmNbottomAttachment,    XmATTACH_WIDGET,
    XmNbottomWidget, box,
    XmNorientation,         XmHORIZONTAL,
    XmNpacking,             XmPACK_TIGHT,
    NULL);
  btn = XtVaCreateManagedWidget("Log",
    xmLabelWidgetClass, box,
    NULL);
  btn = XtVaCreateManagedWidget("X",
    xmPushButtonWidgetClass, box,
    NULL);
  XtAddCallback(btn, XmNactivateCallback, LogXCallback, (XtPointer) info);
  btn = XtVaCreateManagedWidget("Y",
    xmPushButtonWidgetClass, box,
    NULL);
  XtAddCallback(btn, XmNactivateCallback, LogYCallback, (XtPointer) info);

  btn = XtVaCreateManagedWidget("Origin",
    xmLabelWidgetClass, box,
    NULL);
  btn = XtVaCreateManagedWidget("X",
    xmPushButtonWidgetClass, box,
    NULL);
  XtAddCallback(btn, XmNactivateCallback, OriginXCallback, (XtPointer) info);
  btn = XtVaCreateManagedWidget("Y",
    xmPushButtonWidgetClass, box,
    NULL);
  XtAddCallback(btn, XmNactivateCallback, OriginYCallback, (XtPointer) info);

  btn = XtVaCreateManagedWidget("Numbers",
    xmLabelWidgetClass, box,
    NULL);
  btn = XtVaCreateManagedWidget("X",
    xmPushButtonWidgetClass, box,
    NULL);
  XtAddCallback(btn, XmNactivateCallback, NumbersXCallback, (XtPointer) info);
  btn = XtVaCreateManagedWidget("Y",
    xmPushButtonWidgetClass, box,
    NULL);
  XtAddCallback(btn, XmNactivateCallback, NumbersYCallback, (XtPointer) info);

  btn = XtVaCreateManagedWidget("Major",
    xmPushButtonWidgetClass, box,
    NULL);
  XtAddCallback(btn, XmNactivateCallback, MajorCallback, (XtPointer) info);
  btn = XtVaCreateManagedWidget("Minor",
    xmPushButtonWidgetClass, box,
    NULL);
  XtAddCallback(btn, XmNactivateCallback, MinorCallback, (XtPointer) info);
  btn = XtVaCreateManagedWidget("Title",
    xmPushButtonWidgetClass, box,
    NULL);
  XtAddCallback(btn, XmNactivateCallback, TitleCallback, (XtPointer) info);
  btn = XtVaCreateManagedWidget("X Label",
    xmPushButtonWidgetClass, box,
    NULL);
  XtAddCallback(btn, XmNactivateCallback, LabelXCallback, (XtPointer) info);
  btn = XtVaCreateManagedWidget("Y Label",
    xmPushButtonWidgetClass, box,
    NULL);
  XtAddCallback(btn, XmNactivateCallback, LabelYCallback, (XtPointer) info);
  btn = XtVaCreateManagedWidget("Legend",
    xmPushButtonWidgetClass, box,
    NULL);
  XtAddCallback(btn, XmNactivateCallback, LegendCallback, (XtPointer) info);

  info->plot = XtVaCreateManagedWidget("plot",
    sciplotWidgetClass, paned,
    XmNleftAttachment, XmATTACH_FORM,
    XmNrightAttachment, XmATTACH_FORM,
    XmNtopAttachment, XmATTACH_WIDGET,
    XmNtopWidget, topbox,
    XmNbottomAttachment, XmATTACH_WIDGET,
    XmNbottomWidget, botbox,
    XtNheight, (XtArgVal) 600,
    XtNwidth, (XtArgVal) 500,
    XtNplotTitle, title,
/*     XtNlabelFont, XtFONT_TIMES|8, */
/*     XtNtitleFont, XtFONT_TIMES|8, */
/*     XtNtitleMargin, 4, */
/*     XtNaxisFont, XtFONT_TIMES|8, */
    NULL);

  free(text);
  XtManageChild(paned);
  XtManageChild(info->shell);
  return info;
}

void
SciPlotDialogInternalPopup(PlotDialogData *info)
{
  char *x,*y,*t;
  
  XtVaGetValues(info->plot,
    XtNplotTitle, &t,
    XtNxLabel, &x,
    XtNyLabel, &y,
    NULL);
  XmTextFieldSetString(info->title,t);
  XmTextFieldSetString(info->xlabel,x);
  XmTextFieldSetString(info->ylabel,y);
  
  XtManageChild(info->shell);
}

Widget
SciPlotDialog(Widget parent, char *title)
{
  PlotDialogData *info;

  info = SciPlotDialogInternal(parent,title);
  return info->plot;
}

void
SciPlotDialogPopup(Widget w)
{
  XtManageChild(XtParent(XtParent(w)));
}

/*
 * Process a text input stream for plot commands
 */
void
ReadDataFile(Widget parent, FILE * fd)
{
  char *loc;
  int count;
  PlotDialogData *working;
  float xlist[10], ylist[10];
  int line[256], linecount;
  Boolean readnext;
  int num, i;

  loc = NULL;
  working = NULL;
  count = getline(fd, loc);
  while (count > 0) {
    if (count > 0) {
      readnext = True;
      upper(field[0]);
      if (strcmp(field[0], "TITLE") == 0 || strcmp(field[0], "NEW") == 0 ) {
          /* Pop up the last dialog if it exists */
        if (working) SciPlotDialogInternalPopup(working);
        
	working = SciPlotDialogInternal(parent, field[1]);
	for (i = 0; i < NUM_COLORS; i++) {
	  colors_list[i].num = SciPlotAllocNamedColor(working->plot, colors_list[i].text);
	}
	linecount = 0;
      }
      else if (strcmp(field[0], "POLAR") == 0) {
	Boolean degflag;

	degflag = True;
	if (count > 1) {
	  upper(field[1]);
	  if (strncmp(field[1], "RAD", 3) == 0)
	    degflag = False;
	}
	XtVaSetValues(working->plot,
	  XtNchartType, XtPOLAR,
	  XtNdegrees, degflag,
	  NULL);
      }
      else if (strcmp(field[0], "XAXIS") == 0) {
	if (working) {
            if (count > 1) XtVaSetValues(working->plot, XtNxLabel, field[1], NULL);
            if (count>2) {
                for (i=2; i<count; i++) {
                    upper(field[i]);
                    if (strcmp(field[i], "LOG") == 0)
                        XtVaSetValues(working->plot, XtNxLog, True, NULL);
                    else if (strcmp(field[i], "NOZERO") == 0)
                        XtVaSetValues(working->plot, XtNxOrigin, False, NULL);
                }
            }
        }
      }
      else if (strcmp(field[0], "YAXIS") == 0) {
	if (working) {
            if (count > 1) XtVaSetValues(working->plot, XtNyLabel, field[1], NULL);
            if (count>2) {
                for (i=2; i<count; i++) {
                    upper(field[i]);
                    if (strcmp(field[i], "LOG") == 0)
                        XtVaSetValues(working->plot, XtNyLog, True, NULL);
                    else if (strcmp(field[i], "NOZERO") == 0)
                        XtVaSetValues(working->plot, XtNyOrigin, False, NULL);
                }
            }
        }
      }
      else if (strcmp(field[0], "LEGEND") == 0) {
	int colorcount, markercount, linestyle;

	if (working && count > 1) {
	  if (count >= 4) {
	    XtVaSetValues(working->plot,
	      XtNxLabel, field[2],
	      XtNyLabel, field[3],
	      NULL);
	  }
	  line[0] = SciPlotListCreateFromFloat(working->plot, 0, NULL, NULL, field[1]);
	  do {
	    count = getline(fd, loc);
	    readnext = False;

	    num = checkfloat(0);
	    if (count > 0 && num) {
	      tofloat(count);
	      xlist[0] = ffield[0];
	      ylist[0] = ffield[1];
	      SciPlotListAddFloat(working->plot, line[0], 1, xlist, ylist);
	    }

	  } while (count > 0 && num);
	  colorcount = linecount % NUM_COLORS;
	  markercount = linecount % NUM_MARKERS;
	  linestyle = -1;
	  SciPlotListSetStyle(working->plot, line[0],
	    colors_list[colorcount].num, marker_list[markercount].num,
	    colors_list[colorcount].num, linestyle);
	  linecount++;
	}

      }
      else if (strcmp(field[0], "LINE") == 0) {
	int colorcount, markercount, linestyle, maxlines;
        Boolean skip;
        
	if (working && count > 1) {
	  for (i = 1; i < count; i++) {
	    line[i] = SciPlotListCreateFromFloat(working->plot, 0, NULL, NULL, field[i]);
	  }
	  maxlines = count;

	  do {
	    count = getline(fd, loc);
	    readnext = False;

	    num = checkfloat(0);
            if (strcmp(field[0],"skip")==0) skip=True;
            else skip=False;
            
	    if (count > 0 && (num || skip)) {
              if (skip) {
                for (i = 1; i < maxlines; i++) {
                  xlist[0] = ylist[0] = SCIPLOT_SKIP_VAL;
                  SciPlotListAddFloat(working->plot, line[i], 1, xlist, ylist);
                }
              }
              else {
                tofloat(count);
                xlist[0] = ffield[0];
                for (i = 1; i < maxlines; i++) {
                  ylist[0] = ffield[i];
                  SciPlotListAddFloat(working->plot, line[i], 1, xlist, ylist);
                }
              }
              
	    }
	  } while (count > 0 && (num || skip));
	  for (i = 1; i < maxlines; i++) {
	    linecount++;
	    colorcount = linecount % NUM_COLORS;
	    markercount = linecount % NUM_MARKERS;
	    linestyle = -1;
	    SciPlotListSetStyle(working->plot, line[i],
	      colors_list[colorcount].num, marker_list[markercount].num,
	      colors_list[colorcount].num, linestyle);
/*             SciPlotListSetMarkerSize(working->plot, line[i], (float) linecount*4); */
	  }
	}

      }
    }
    if (readnext)
      count = getline(fd, loc);
  }
  if (working) SciPlotDialogInternalPopup(working);
}
