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
 *      CODA Editor's toplevel Xwindow layout
 *	
 * Author: Jie Chen, CEBAF Data Acquisition Group
 *
 * Revision History:
 *   $Log: Editor_widget.c,v $
 *   Revision 1.8  1998/07/13 17:53:59  heyes
 *   remove buttons when env var set...
 *
 *   Revision 1.7  1997/11/25 15:55:43  rwm
 *   Zapped compiler warnings.
 *
 *   Revision 1.6  1997/09/08 15:19:38  heyes
 *   fix dd icon etc
 *
 *   Revision 1.5  1997/08/29 12:25:37  heyes
 *   fixed window positioning
 *
 *   Revision 1.4  1997/07/08 15:30:44  heyes
 *   add stuff
 *
 *   Revision 1.3  1997/06/20 17:00:22  heyes
 *   clean up GUI!
 *
 *   Revision 1.2  1997/06/16 12:24:00  heyes
 *   various fixes and nicities!
 *
 *   Revision 1.1  1997/06/14 17:51:44  heyes
 *   new GUI
 *
 *   Revision 1.1.1.2  1996/11/05 17:45:18  chen
 *   coda source
 *
 *	  
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <Xm/Xm.h>
#include <Xm/Form.h>
#include <Xm/RowColumn.h>
#include <Xm/Separator.h>
#include <Xm/Label.h>
#include <Xm/PushB.h>

#include "MixedLabel.h"

#include "Editor_layout.h"
#include "Editor_pixmap.h"
#include "Editor_graph.h"
#include "Editor_miscinfo.h"
#include "Editor_database.h"
#include "Editor_widget_poll.h"
#include "Editor_cmd_btns.h"
#include "Editor_graph_cmd.h"
#include "Editor_zoom.h"
#include "Editor_icon_box.h"
#include "Editor_drawing.h"

/*  "NetEditor*.drawing_area.background: NavyBlue",
  "NetEditor*.top_ruler.background: NavyBlue",
  "NetEditor*.left_ruler.background: NavyBlue",
  "NetEditor*.top_ruler.foreground: White",
  "NetEditor*.left_ruler.foreground: White",
  "NetEditor*.top_ruler.tickerColor: White",
  "NetEditor*.left_ruler.tickerColor: White",  
  "NetEditor*.edit_popup.*.background:White",
  "NetEditor*.top_ruler.indicatorColor: Red",
  "NetEditor*.left_ruler.indicatorColor: Red",
  "NetEditor*.foreground:     Black",
  "NetEditor*.background:     LightBlue",
*/

static String fallback_resources[]={
  "*.icon.borderWidth: 2",
  "*.scale_red*troughColor: Red",
  "*.scale_green*troughColor: Green",
  "*.scale_blue*troughColor: Blue",
  "*.highlightThickness:0",
  "*.XmRowColumn*spacing: 0",
  "*.selectColor:     red",
  "*.scriptTab.shadowThickness: 2",
  "*.scriptTab.tabWidthPercentage: 0",
  "*.scriptTab.cornerwidth: 4",
  "*.scriptTab.cornerheight: 2",
  "*.scriptTab.textmargin: 4",
  "*.scriptTab.foreground: blue",
  "*.scriptTab.tabcolor: Grey",
  "*.top_ruler.background: NavyBlue",
  "*.left_ruler.background: NavyBlue",
  "*.top_ruler.foreground: White",
  "*.left_ruler.foreground: White",
  "*.top_ruler.tickerColor: White",
  "*.left_ruler.tickerColor: White",  
  "*.edit_popup.*.background:White",
  "*.top_ruler.indicatorColor: Red",
  "*.left_ruler.indicatorColor: Red",
  "*.dialogLabel.fontList: -adobe-helvetica-bold-r-*-*-14-*-*-*-*-*-*-*",
  "*.fontList: -adobe-helvetica-medium-r-*-*-14-*-*-*-*-*-*-*",
  NULL
};

Manager manager;          /* x-window layout manager */
Widget top_form;
Widget left_form, right_form, top_right_form;
Widget menu_bar, sep;
Widget cmd_form, text_rcl;
Widget link_rcl, edit_rcl, op_1, op_2;
Widget icon_box;
Widget temp_label;
Widget zoom_widget;
Widget miscinfo;
Widget graph;

int hide_controls = 0;

Widget
CodaEditor(Widget toplevel,int withExit)
{
  char   *dbasehost;
  Arg    args[10];
  int    ac;
  Pixel  bg, fg;

  /*dbasehost = 0;*/
  dbasehost = getenv("MYSQL_HOST");

  /* connect to a database server */
  if (connectToDatabase (dbasehost) == NULL)
  {
    fprintf (stderr, "Cannot connect to a CODA database server\n");
    exit(0);/*return;*/
  }

  /* initialize widgets poll */
  init_widget_poll();

  ac = 0;

  XtSetArg (args[ac], XmNshadowThickness, 2); ac++;
  XtSetArg (args[ac], XmNshadowType, XmSHADOW_ETCHED_OUT); ac++;
  top_form = XtCreateWidget("top_form",xmFormWidgetClass,
			    toplevel, args, ac);

/* create all necessary pixmaps */
  ac = 0;
  XtSetArg(args[ac], XmNbackground, &bg); ac++;
  XtSetArg(args[ac], XmNforeground, &fg); ac++;
  XtGetValues(top_form, args, ac); 
  XcodaEditorCreatePixmaps(top_form, bg, fg);
  ac = 0;

/* Create Icon Pixmap for application */
  XtSetArg (args[ac], XmNiconPixmap, btn_pixmaps.icon); ac++;
  XtSetValues (toplevel, args, ac);
  ac = 0;

  if (!hide_controls) {
    XtSetArg(args[ac], XmNtopAttachment, XmATTACH_FORM); ac++;
    XtSetArg(args[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
    XtSetArg(args[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
    menu_bar = XmCreateMenuBar(top_form, "menu_bar", args, ac);
    ac = 0;
    XcodaEditorCreateMenus(menu_bar,withExit);
    XtManageChild(menu_bar);
    
    XtSetArg(args[ac], XmNtopAttachment, XmATTACH_WIDGET); ac++;
    XtSetArg(args[ac], XmNtopWidget,     menu_bar); ac++;
  } else {
    XtSetArg(args[ac], XmNtopAttachment, XmATTACH_FORM); ac++;
    XtSetArg(args[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
    XtSetArg(args[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
    menu_bar = XmCreateMenuBar(top_form, "menu_bar", args, ac);
    XcodaEditorCreateMenus(menu_bar,withExit);
    XtUnmanageChild(menu_bar);
    ac = 0;
    XtSetArg(args[ac], XmNtopAttachment, XmATTACH_FORM); ac++;
    XtSetArg(args[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
    XtSetArg(args[ac], XmNbottomAttachment, XmATTACH_FORM); ac++;
  }
  left_form = XtCreateWidget("left_form", xmFormWidgetClass, top_form, args, ac);
  ac = 0;

    cmd_form = XcodaEditorNewButtons(left_form);

  if (!hide_controls) {
    XtSetArg(args[ac], XmNtopAttachment, XmATTACH_FORM); ac++;
    XtSetArg(args[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
    XtSetArg(args[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
    XtSetValues(cmd_form, args, ac);
    ac = 0;
    
    /* separator to separate top and bottom */
    XtSetArg(args[ac], XmNtopAttachment, XmATTACH_WIDGET); ac++;
    XtSetArg(args[ac], XmNtopWidget, cmd_form); ac++;
    sep = XtCreateManagedWidget("sep",xmSeparatorWidgetClass, left_form, args, ac);
    ac = 0;
    XtSetArg(args[ac], XmNtopAttachment, XmATTACH_WIDGET); ac++;
    XtSetArg(args[ac], XmNtopWidget, sep); ac++;
  } else {
    ac = 0;
    XtUnmanageChild(cmd_form);
    
    XtSetArg(args[ac], XmNtopAttachment, XmATTACH_FORM); ac++;
  }

  zoom_widget = XcodaEditorZoomWidget(left_form, fg, bg);
  XtSetArg(args[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
  XtSetArg(args[ac], XmNleftOffset, 5); ac++;
  XtSetArg(args[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
  XtSetArg(args[ac], XmNrightOffset, 5); ac++;
  XtSetArg(args[ac], XmNbottomAttachment, XmATTACH_FORM); ac++;
  XtSetArg(args[ac], XmNbottomOffset, 5); ac++;
  XtSetValues(zoom_widget, args, ac); 
  ac = 0;

  XtManageChild(left_form);
  /*end of left form */

  XtSetArg(args[ac], XmNtopAttachment, XmATTACH_WIDGET); ac++;
  XtSetArg(args[ac], XmNtopWidget, menu_bar); ac++;
  XtSetArg(args[ac], XmNleftAttachment, XmATTACH_WIDGET); ac++;
  XtSetArg(args[ac], XmNleftWidget, left_form); ac++;
  XtSetArg(args[ac], XmNbottomAttachment, XmATTACH_FORM); ac++;
  XtSetArg(args[ac], XmNorientation, XmVERTICAL); ac++;
  XtSetArg(args[ac], XmNseparatorType, XmSHADOW_ETCHED_OUT); ac++;
  sep = XtCreateManagedWidget("sep", xmSeparatorWidgetClass, top_form,
			      args, ac);
  ac = 0;

  XtSetArg(args[ac], XmNtopAttachment, XmATTACH_WIDGET); ac++;
  XtSetArg(args[ac], XmNtopWidget,     menu_bar); ac++;
  XtSetArg(args[ac], XmNtopOffset,     3); ac++;
  XtSetArg(args[ac], XmNleftAttachment, XmATTACH_WIDGET); ac++;
  XtSetArg(args[ac], XmNleftWidget,    sep); ac++;
  XtSetArg(args[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
  XtSetArg(args[ac], XmNrightOffset, 3); ac++;
  XtSetArg(args[ac], XmNbottomAttachment, XmATTACH_FORM); ac++;
  XtSetArg(args[ac], XmNbottomOffset, 3); ac++;
  right_form = XtCreateWidget("right_form", xmFormWidgetClass, top_form, args, ac);

  ac = 0;

  /* create experiment misc input widgets */
  miscinfo = XcodaEditorCreateInfoWidgets (right_form);
  XtSetArg(args[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
  XtSetArg(args[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
  XtSetArg(args[ac], XmNbottomAttachment, XmATTACH_FORM); ac++;
  XtSetValues(miscinfo, args, ac);

  ac = 0;

  icon_box = XcodaEditorIconBox(right_form);
  if (!hide_controls) {
    XtSetArg(args[ac], XmNtopAttachment, XmATTACH_FORM); ac++;
    XtSetArg(args[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
    XtSetArg(args[ac], XmNbottomAttachment, XmATTACH_WIDGET); ac++;
    XtSetArg(args[ac], XmNbottomWidget, miscinfo); ac++;
    XtSetValues(XtParent(XtParent(icon_box)), args, ac);
    
    ac = 0;  
    XtSetArg(args[ac], XmNleftAttachment, XmATTACH_WIDGET); ac++;
    XtSetArg(args[ac], XmNleftWidget, icon_box); ac++;

  } else {

    XtUnmanageChild(XtParent(XtParent(icon_box)));
    ac = 0;  
    XtSetArg(args[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
  }
  /* bottom right graph widget */
  graph = XcodaEditorDrawingArea(right_form);
  XtSetArg(args[ac], XmNtopAttachment, XmATTACH_WIDGET); ac++;
  XtSetArg(args[ac], XmNtopWidget, sep); ac++;
  XtSetArg(args[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
  XtSetArg(args[ac], XmNbottomAttachment, XmATTACH_WIDGET); ac++;
  XtSetArg(args[ac], XmNbottomWidget, miscinfo); ac++;
  XtSetValues(graph, args, ac);
  ac = 0;


  XtManageChild(right_form);
/*end bottom form*/
  
  XtManageChild(top_form);
  
  /* initially set whole panel locked */
  XcodaEditorDisableInput ();
  {
    char *exp = getenv("EXPID");
    if (exp)
    {
      EditorSelectExp(top_form,exp);
    }
  }
  return top_form;
}
