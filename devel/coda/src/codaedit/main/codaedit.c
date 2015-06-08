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
 *   $Log: Editor_toplevel.c,v $
 *   Revision 1.14  1999/10/25 14:46:31  rwm
 *   Change type of include to conventional symbol.
 *
 *   Revision 1.13  1998/09/01 14:28:38  heyes
 *   add -embed option
 *
 *   Revision 1.11  1998/09/01 13:56:05  heyes
 *   add -embed option
 *
 *   Revision 1.10  1998/07/13 17:53:58  heyes
 *   remove buttons when env var set...
 *
 *   Revision 1.9  1998/06/18 12:28:32  heyes
 *   tidy up a lot of thingscd ../cedit
 *
 *   Revision 1.8  1997/11/04 20:35:10  heyes
 *   add redirect
 *
 *   Revision 1.7  1997/10/15 16:18:18  heyes
 *   embed tools in runcontrol
 *
 *   Revision 1.6  1997/10/13 15:24:43  heyes
 *   embedded windows
 *
 *   Revision 1.5  1997/08/29 12:25:36  heyes
 *   fixed window positioning
 *
 *   Revision 1.4  1997/08/25 16:08:06  heyes
 *   fix display problems with background
 *
 *   Revision 1.3  1997/06/16 12:23:59  heyes
 *   various fixes and nicities!
 *
 *   Revision 1.2  1997/06/14 17:50:39  heyes
 *   new GUI
 *
 *   Revision 1.1.1.2  1996/11/05 17:45:18  chen
 *   coda source
 *
 *	  
 */
#include <stdio.h>
#include <string.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <Xm/Xm.h>
#include <Xm/Form.h>
#include <Xm/RowColumn.h>
#include <Xm/Separator.h>
#include <Xm/Label.h>
#include <Xm/PushB.h>
#ifdef USE_CREG
#include "codaRegistry.h"
#endif
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
#include "./xpm_icon/stone.xpm"
#include "XcodaXpm.h"

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
  
  "NetEditor*.icon.borderWidth: 2",
  /*"NetEditor*.backgroundPixmap: bg_pixmap1",*/
  "NetEditor*.highlightThickness:0",
  "NetEditor*.XmRowColumn*spacing: 0",
  "NetEditor*.selectColor:     red",
  "NetEditor*.scriptTab.shadowThickness: 2",
  "NetEditor*.scriptTab.tabWidthPercentage: 0",
  "NetEditor*.scriptTab.cornerwidth: 4",
  "NetEditor*.scriptTab.cornerheight: 2",
  "NetEditor*.scriptTab.textmargin: 4",
  
  "NetEditor*.foreground:     white",
  "NetEditor*.background:     gray20",
  "NetEditor*.topShadowColor:                   gray",
  "NetEditor*.bottomShadowColor:                black",
  "NetEditor*.borderColor:                      gray25",

  "NetEditor*.scale_red*troughColor: Red",
  "NetEditor*.scale_green*troughColor: Green",
  "NetEditor*.scale_blue*troughColor: Blue",
  "NetEditor*.scriptTab.foreground: blue",
  "NetEditor*.scriptTab.tabcolor: Grey",
  "NetEditor*.top_ruler.background: gray",
  "NetEditor*.left_ruler.background: gray",
  "NetEditor*.top_ruler.indicatorColor: Red",
  "NetEditor*.left_ruler.indicatorColor: Red",

  "NetEditor*.dialogLabel.fontList: -*-helvetica-bold-r-*-*-14-*-*-*-*-*-*-*",
  "NetEditor*.fontList: -*-helvetica-medium-r-*-*-14-*-*-*-*-*-*-*",
  NULL
};

extern int hide_controls;

Manager manager;          /* x-window layout manager */
XtAppContext app_context;
Widget toplevel;

int
Xhandler(Widget w,XtPointer p, XEvent *e,Boolean *b)
{
  if (e->type == DestroyNotify) {
    printf("CEDIT:X window was destroyed\n");
    exit(0);
  }
  return 0;
}

void
messageHandler(char *message)
{
  switch (message[0]) {
  case 'c':
    EditorSelectConfig(&message[2]);
    break;
  case 'e':
    EditorSelectExp(toplevel,&message[2]);
    break;
  case 's':
    {
      int state;
      char name[50];
      sscanf(&message[2],"%d %s",&state, name);
      setCompState(name,state);
    }
    break;

  default:
  printf("unknown message : %s\n",message);
  
  }
}

main(int argc, char **argv)
{
  Arg    args[10];
  int    ac,ix;
  Pixmap bg_pixmap;
  XImage *image;

  for (ix=1;ix < argc;ix++)
  {
    if (argv[ix] &&  (strcmp(argv[ix],"-noedit") == 0))
    {
      hide_controls = 1;
    }
  }
  ac = 0;
  XtSetArg(args[ac], XmNtitle, "net_Editor"); ac++;
  XtSetArg(args[ac], XmNiconName,"net_Editor"); ac++;
  XtSetArg(args[ac], XmNoverrideRedirect,True); ac++;
  toplevel = XtAppInitialize(&app_context,"NetEditor", NULL, 0, &argc, argv,
			     fallback_resources, args, ac);
  
  bg_pixmap =  XcodaCreatePixmapFromXpm(toplevel,stone_xpm, 1);
  image = XGetImage(XtDisplay(toplevel), bg_pixmap, 0, 0, 159, 160, AllPlanes, XYPixmap);
  
  XmInstallImage (image, "bg_pixmap1");
  
  {
    Window parent;
    Widget w;
    char cmd[100];
    parent = 0;

#ifdef USE_CREG
    for(ix=1; ix<argc; ix++)
    {
      if (argv[ix] &&  (strcmp(argv[ix],"-embed") == 0))
      {
		printf("wwwwwwwwwwwwwwwwwwwww CREG wwwwwwwwwwwwwww\n");
        parent = CODAGetAppWindow(XtDisplay(toplevel),"cedit_WINDOW");
      }
    }

    if (parent)
    {
      ac = 0;
      XtSetArg(args[ac], XmNx,3000); ac++;
      XtSetValues (toplevel, args, ac);
      XtRealizeWidget(toplevel);
      XWithdrawWindow(XtDisplay(toplevel), XtWindow(toplevel),0);

      sprintf(cmd,"r:0x%08x 0x%08x",XtWindow(toplevel),parent);      
      coda_Send(XtDisplay(toplevel),"RUNCONTROL",cmd);
      CodaEditor(toplevel,1);
      codaSendInit(toplevel,"CEDIT");
      codaRegisterMsgCallback(messageHandler);
      XtAddEventHandler(toplevel,StructureNotifyMask, False, Xhandler, NULL);
    }
    else
#else
	printf("uuuuuuuuuuuuuuu NO CREG uuuuuuuuuuuuuuuuuuu\n");
#endif
    {
      ac = 0;
      XtSetArg(args[ac], XmNoverrideRedirect,False); ac++;
      XtSetValues (toplevel, args, ac);

      CodaEditor(toplevel,0);
      XtRealizeWidget(toplevel);
    }
  }

  XtAppMainLoop(app_context);
}
