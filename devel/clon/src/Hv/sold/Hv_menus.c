/*
==================================================================================
The Hv library was developed at CEBAF under contract to the
Department of Energy and is the property of the U.S. Government.

It may be used to develop commercial software, but the Hv
source code and/or compiled modules/libraries may not be sold.

Questions or comments should be directed to heddle@cebaf.gov
==================================================================================
*/

#include "Hv.h"
#include "Hv_xyplot.h"
#include "Hv_undo.h"
#include "Hv_maps.h"

extern      Hv_Item        Hv_hotPlot;

Hv_Widget    Hv_lastItems[Hv_NUMUNDOITEMS];

/* local prototypes */


static void     Hv_CreateUserMenus(void);

static Widget	Hv_CreateHelpPullDown(void);

static void     Hv_SetPopupActivity(Hv_View  View);

static void	Hv_DoQuickZoom(Widget	w,
			       XtPointer	client_data,
			       XtPointer	call_data);

static void     Hv_ToggleZoomResize(Widget	w,
				    XtPointer	client_data,
				    XtPointer	call_data);

static void	Hv_DoHelpMenu(Widget	w,
			      XtPointer	client_data,
			      XtPointer	call_data);

static void     Hv_EditBackgroundColor(Hv_Widget	w);	

static void	Hv_DoPopupMenu(Widget	w,
			       XtPointer	client_data,		
			       XtPointer	call_data);

static void     Hv_DoPopup2AndActionMenu(Widget	w,
					 XtPointer	client_data,
					 XtPointer	call_data);

static void     Hv_ManageMenuItem(Widget menu,
				  Widget w);

static void     Hv_CreatePopup1(void);

static void     Hv_CreatePopup2(void);

static Widget	Hv_CreatePullDown(char	*title);

static Widget   Hv_AddMenuSeparator(Widget	menu);

static Widget   Hv_AddSubMenuItem(String	ItemLabel,
				  Widget     parentmenu,
				  Widget	submenu);

static Widget   Hv_AddMenuToggle(String	        ItemLabel,
				 Widget	        menu,
				 long           ID,
				 XtCallbackProc	CBProcess,
				 Boolean	State,
				 unsigned char  btype,
				 int	        acctype,
				 char	        accchr);

static Widget   Hv_AddMenuItem(String	ItemLabel,
			       Widget	menu,
			       long      ID,
			       XtCallbackProc	CBProcess,
			       int	acctype,
			       char	accchr);		

static Widget	Hv_CreateSubMenu(Widget	parent);

/* variables global for this file */

static  XmFontList  Hv_defaultMenuFontList;

static  Widget    Hv_quitSepItem;
static  Boolean   Hv_userItem;

#define Hv_MAXUSERMENUS        6
static  Widget    Hv_userMenus[Hv_MAXUSERMENUS];
static  Widget    Hv_userPDBtns[Hv_MAXUSERMENUS];
static  XmString  Hv_userMenuTitles[Hv_MAXUSERMENUS];
static  int       Hv_userMenuCount;

static  Hv_Point  plotpp;
static  XEvent    *plotevent;
static  Widget    Hv_poppedUpMenu;


static char *Hv_undoLabels[] = {
  "Last Item Drag",
  "Last Item Rotate",
};

static XtActionsRec menuactions[] = {
  {"Hv_PopdownMenuShell", (XtActionProc)Hv_PopdownMenuShell},
};


/*----- Hv_PopupMenuShell ------*/

void  Hv_PopupMenuShell(Widget        w,
			XButtonPressedEvent *event,
			String        *params,
			Cardinal       *num_params)


{
  Hv_Point    pp;
  
  Hv_SetPoint(&pp, event->x, event->y);
  Hv_WhichView(&Hv_hotView, pp);


  if (Hv_hotView != NULL) {
    Hv_poppedUpMenu = Hv_popupMenu;
    Hv_ViewPopup(Hv_hotView, (XEvent *)event);
  }
  else
    Hv_poppedUpMenu = Hv_popupMenu2;

  XmMenuPosition(Hv_poppedUpMenu, (XButtonPressedEvent *)event);

/* experimental line to try to solve MB3 problem */

  XFlush(Hv_display); /*EXPERIMENTAL*/

  XUngrabPointer(Hv_display, CurrentTime); /*EXPERIMENTAL*/
  XtManageChild(Hv_poppedUpMenu);

/*XGrabPointer(Hv_display, XtWindow(Hv_poppedUpMenu), True, 0, GrabModeAsync, GrabModeAsync, None, None, CurrentTime);*/ /*EXPERIMENTAL*/
  
  XFlush(Hv_display); /*EXPERIMENTAL*/
  XUngrabPointer(Hv_display, CurrentTime); /*EXPERIMENTAL*/
}

/*------- Hv_PopdownMenuShell ------*/

void   Hv_PopdownMenuShell(void)

{
/*
  if (Hv_poppedUpMenu != NULL) {
    XtUnmanageChild(Hv_poppedUpMenu); 
    Hv_poppedUpMenu = NULL;
  }
*/  
}

/*------ Hv_CreateUserMenus -------*/


static void     Hv_CreateUserMenus(void)

{
  XmString    xmst;
  int         i;

  Hv_userMenuCount = 0;

  for (i = 0; i < Hv_MAXUSERMENUS; i++) {
    xmst = XmStringCreate("", XmSTRING_DEFAULT_CHARSET);

    Hv_userMenus[i] = XmCreatePulldownMenu(Hv_menuBar, "pulldown", NULL, 0);
    
    Hv_userPDBtns[i] = XtVaCreateManagedWidget("menubtn", 
					       xmCascadeButtonWidgetClass,  Hv_menuBar,
					       XmNsubMenuId,                Hv_userMenus[i],
					       XmNlabelString,              xmst,
					       NULL);
    XtSetSensitive(Hv_userPDBtns[i], False);
    XmStringFree(xmst);
  }
}

/*----------- Hv_vaCreateMenu ---------*/

void Hv_VaCreateMenu(Hv_Widget *menu,
		     ...)

{
  va_list           ap;
  Hv_AttributeArray attributes;

  *menu = NULL;
  va_start(ap, menu);

/* with new ansi format the menu widget itself is the
   first and only required argument */

  Hv_GetAttributes(ap, attributes, NULL, NULL, NULL, NULL, NULL);

  switch (attributes[Hv_TYPE].s) {
  case Hv_SUBMENUTYPE:
    *menu = Hv_CreateSubMenu((Widget)(attributes[Hv_PARENT].v));
    break;
    
  case Hv_POPUPMENUTYPE:
    *menu = XmCreatePopupMenu(Hv_canvas, "popup", NULL, 0);
    XtVaSetValues(*menu, XmNwhichButton, 3, NULL);
    break;

  case Hv_HELPMENUTYPE:
    *menu = Hv_CreateHelpPullDown();
    break;

  default:
    *menu = Hv_CreatePullDown((char *)(attributes[Hv_TITLE].v));
    break;
  }
}

/*----------- Hv_VaCreateMenuItem ---------*/

Hv_Widget       Hv_VaCreateMenuItem(Hv_Widget menu,
				    ...)

{
  va_list           ap;
  Hv_AttributeArray attributes;
  Widget            menuitem = NULL;

  va_start(ap, menu);

/* Now get the variable arguments */

  Hv_GetAttributes(ap, attributes, NULL, NULL, NULL, NULL, NULL);


  switch (attributes[Hv_TYPE].s) {
  case Hv_SEPARATOR:
    menuitem = Hv_AddMenuSeparator(menu);
    break;
    
  case Hv_TOGGLEMENUITEM:
    menuitem = Hv_AddMenuToggle((char *)(attributes[Hv_LABEL].v), menu, attributes[Hv_ID].l,
				(XtCallbackProc)(attributes[Hv_CALLBACK].fp), (Boolean)(attributes[Hv_STATE].s),
				(unsigned char)(attributes[Hv_MENUTOGGLETYPE].s),
				attributes[Hv_ACCELERATORTYPE].s, attributes[Hv_ACCELERATORCHAR].c);
    break;
    
  case Hv_SUBMENUITEM:
    menuitem = Hv_AddSubMenuItem((char *)(attributes[Hv_LABEL].v), menu, (Widget)(attributes[Hv_SUBMENU].v));
    break;
    
  default:   /* standard type */
    menuitem = Hv_AddMenuItem((char *)(attributes[Hv_LABEL].v), menu, attributes[Hv_ID].l,
			      (XtCallbackProc)(attributes[Hv_CALLBACK].fp),
			      attributes[Hv_ACCELERATORTYPE].s, attributes[Hv_ACCELERATORCHAR].c);
    break;
  }
  
  return  (Hv_Widget)menuitem;
}


/*-------- Hv_SetMenuItemString -------*/

void  Hv_SetMenuItemString(Widget menuitem,
			   char  *str,
			   short  font)

/*sets (or changes) menu item to requested str*/

{
  Arg        arg[2]; /* arguments for widgets */
  XmString   xmst;

  if (font == Hv_DEFAULT) {
    xmst = XmStringCreate(str, XmSTRING_DEFAULT_CHARSET);
    XtSetArg(arg[0], XmNlabelString, xmst);
    XtSetArg(arg[1], XmNfontList, Hv_defaultMenuFontList);
  }
  else {
    xmst = Hv_CreateMotifString(str, font);
    XtSetArg(arg[0], XmNlabelString, xmst);
    XtSetArg(arg[1], XmNfontList, Hv_fontList);
  }

  XtSetValues(menuitem, arg, 2);
  XmStringFree(xmst);
  
}

/* -------- Hv_SetQuickZoomLabel --------*/

void Hv_SetQuickZoomLabel(short   item,
			  char   *str)

{
  if ((item < 0) || (item >= Hv_NUMQUICKZOOM)) {
    fprintf(stderr, "Hv warning: attempt to set quick zoom labe with out of range index.\n");
    return;
  }

  if (Hv_Contained(str, "unused")) {
    Hv_SetMenuItemString(Hv_quickZoomItems[item], "-- Not Used --", Hv_DEFAULT);
    Hv_DisableWidget(Hv_quickZoomItems[item]);
  }
  else {    
    Hv_SetMenuItemString(Hv_quickZoomItems[item], str, Hv_DEFAULT);
    Hv_EnableWidget(Hv_quickZoomItems[item]);
  }
}

/* ------ Hv_CreateMenuBar ------------- */

void Hv_CreateMenuBar(void)

{
  Widget   item;

/* we like our menubars to be in the usual place ... along the top of the window.
   This also creates the standard menus */

  XtAppAddActions(Hv_appContext, menuactions, XtNumber(menuactions));
  Hv_menuBar = XmCreateMenuBar(Hv_form, "menubar", NULL, 0);

  XtVaSetValues(Hv_menuBar,
		XmNtopAttachment,   XmATTACH_FORM,
		XmNrightAttachment, XmATTACH_FORM,
		XmNleftAttachment,  XmATTACH_FORM,
		NULL);


  Hv_userItem = False;       /*now adding standard menus */

/* add menubar to form's list of managed widgets */

  XtManageChild(Hv_menuBar);

/* Create standard  Pulldown MenuPanes */
  
  Hv_VaCreateMenu(&Hv_actionMenu, Hv_TITLE, " Action ", NULL);
  Hv_VaCreateMenu(&Hv_viewMenu,   Hv_TITLE, " Views ",  NULL);
  Hv_VaCreateMenu(&Hv_helpMenu,   Hv_TYPE, Hv_HELPMENUTYPE, NULL);

/* add standard items to Action menu */

  item = Hv_VaCreateMenuItem(Hv_actionMenu,
			  Hv_LABEL,           "Refresh Screen",
			  Hv_ID,              Hv_REFRESHSCREEN,
			  Hv_CALLBACK,        Hv_DoPopup2AndActionMenu,
			  Hv_ACCELERATORTYPE, Hv_ALT,
			  Hv_ACCELERATORCHAR, 'R',
			  NULL);

  item = Hv_VaCreateMenuItem(Hv_actionMenu,
			  Hv_LABEL,           "Print Screen...",
			  Hv_ID,              Hv_PRINTSCREEN,
			  Hv_CALLBACK,        Hv_DoPopup2AndActionMenu,
			  Hv_ACCELERATORTYPE, Hv_ALT,
			  Hv_ACCELERATORCHAR, 'P',
			  NULL);


  item = Hv_VaCreateMenuItem(Hv_actionMenu, Hv_TYPE, Hv_SEPARATOR, NULL); 


/* new items in vers 1.0058 the, the configuration handlers */ 

  item = Hv_VaCreateMenuItem(Hv_actionMenu,
			  Hv_LABEL,           "Open Configuration...",
			  Hv_ID,              Hv_OPENCONFIGURATION,
			  Hv_CALLBACK,        Hv_OpenConfiguration,
			  NULL);

  item = Hv_VaCreateMenuItem(Hv_actionMenu,
			  Hv_LABEL,           "Save Configuration...",
			  Hv_ID,              Hv_SAVECONFIGURATION,
			  Hv_CALLBACK,        Hv_SaveConfiguration,
			  NULL);


  Hv_quitSepItem = Hv_VaCreateMenuItem(Hv_actionMenu, Hv_TYPE, Hv_SEPARATOR, NULL); 

  Hv_quitItem = Hv_VaCreateMenuItem(Hv_actionMenu,
				    Hv_LABEL,           "Quit...",
				    Hv_ID,              Hv_QUIT,
				    Hv_CALLBACK,        Hv_DoPopup2AndActionMenu,
				    Hv_ACCELERATORTYPE, Hv_ALT,
				    Hv_ACCELERATORCHAR, 'Q',
				    NULL);
  
/* find the default menu font list */

  XtVaGetValues(item, XmNfontList,  &Hv_defaultMenuFontList, NULL);

/* create the standard  menu items for the Help menu */
  

  item = Hv_VaCreateMenuItem(Hv_helpMenu,
			  Hv_LABEL,           "Help...",
			  Hv_ID,              Hv_STANDARDHELP,
			  Hv_CALLBACK,        Hv_DoHelpMenu,
			  Hv_ACCELERATORTYPE, Hv_ALT,
			  Hv_ACCELERATORCHAR, 'H',
			  NULL);


  item = Hv_VaCreateMenuItem(Hv_helpMenu,
			  Hv_LABEL,           "Show Balloons",
			  Hv_ID,              Hv_BALLOONHELP,
			  Hv_CALLBACK,        Hv_DoHelpMenu,
			  NULL);

  Hv_CreateUserMenus();

  Hv_userItem = True;       /*done adding standard menus */
}

/* ------ Hv_CreatePopups -------*/

void Hv_CreatePopups(void)

{
  Hv_CreatePopup1();
  Hv_CreatePopup2();
}


/* ------- Hv_ViewPopup ------*/

void  Hv_ViewPopup(Hv_View  View,
		   XEvent  *event)

/* set the popup menu to refelect the most common state */

{
  int        zoomresizebit;

  zoomresizebit = Hv_FirstSetBit(Hv_ZOOMRESIZETOGGLE);
  Hv_SetToggleButton(Hv_popupItems[zoomresizebit], Hv_CheckBit(View->drawcontrol, Hv_ZOOMONRESIZE));

/*  XGrabPointer(Hv_display, Hv_mainWindow, False, 0, GrabModeAsync, GrabModeAsync, None, None, CurrentTime); */


/* if it is a plot view, need some further checking */


  if (View->tag == Hv_XYPLOTVIEW) {


    Hv_SetMenuItemString(Hv_popupItems[Hv_FirstSetBit(Hv_CUSTOMIZEVIEW)],
			"Print THIS PLOT ONLY...", Hv_fixed2);

    Hv_SetPoint(&plotpp, event->xbutton.x, event->xbutton.y);
    plotevent = event;

    if ((Hv_hotPlot = Hv_PointInPlotItem(View, plotpp)) == NULL) {
      Hv_ClearBit(&(View->popupcontrol), Hv_CUSTOMIZEVIEW);
      Hv_ClearBit(&(View->popupcontrol), Hv_65PERCENTVIEW);
      Hv_ClearBit(&(View->popupcontrol), Hv_85PERCENTVIEW);
      Hv_ClearBit(&(View->popupcontrol), Hv_110PERCENTVIEW);
      Hv_ClearBit(&(View->popupcontrol), Hv_125PERCENTVIEW);
      Hv_ClearBit(&(View->popupcontrol), Hv_DEFAULTVIEW);
      Hv_ClearBit(&(View->popupcontrol), Hv_VARZOOMVIEW);
    }
    else {
      Hv_SetBit(&(View->popupcontrol), Hv_CUSTOMIZEVIEW);
      Hv_SetBit(&(View->popupcontrol), Hv_65PERCENTVIEW);
      Hv_SetBit(&(View->popupcontrol), Hv_85PERCENTVIEW);
      Hv_SetBit(&(View->popupcontrol), Hv_110PERCENTVIEW);
      Hv_SetBit(&(View->popupcontrol), Hv_125PERCENTVIEW);
      Hv_SetBit(&(View->popupcontrol), Hv_DEFAULTVIEW);
      Hv_SetBit(&(View->popupcontrol), Hv_VARZOOMVIEW);
    }
  }
  else
    Hv_SetMenuItemString(Hv_popupItems[Hv_FirstSetBit(Hv_CUSTOMIZEVIEW)],
			 "Customize...", Hv_fixed2);

  Hv_SetPopupActivity(View);

/* if user quickzoom label customizer routine provided, call it */

  if (Hv_CustomizeQuickZoom != NULL)
    Hv_CustomizeQuickZoom(View);

/* set the menu position to the location of the mouse when the button was pressed */
  
/*  XmMenuPosition(Hv_popupMenu, (XButtonPressedEvent *)event); */

/* popup the menu onto the screen */


/*  XDefineCursor (Hv_display, XtWindow(Hv_popupMenu), Hv_defaultCursor);  */

/*  XtManageChild(Hv_popupMenu); */

/*  XUngrabPointer(Hv_display, CurrentTime); */

}


/* ------- Hv_CreateHelpPullDown ---------- */

static Widget	Hv_CreateHelpPullDown(void)

{
  XmString      xmst;
  Widget	pdBtn;
  Widget        pd;

  pd = XmCreatePulldownMenu(Hv_menuBar, "pulldown", NULL, 0);
     
  xmst = XmStringCreate(" Help", XmSTRING_DEFAULT_CHARSET);

  pdBtn = XtVaCreateManagedWidget("menubtn", 
				  xmCascadeButtonWidgetClass,  Hv_menuBar,
				  XmNlabelString,              xmst,
				  XmNsubMenuId,                pd,
				  NULL);
    
/* since  a help menu, set resource which will cause it to be placed at extreme right */

  XtVaSetValues(Hv_menuBar, XmNmenuHelpWidget, (XtArgVal)(pdBtn), NULL);
  
  XmStringFree(xmst);
  return pd;
}

/* ----- Hv_SetPopupActivity ------*/

static void Hv_SetPopupActivity(Hv_View View)

{
  int i;
  int checkbit = 1;


  if (View == NULL)
    return;

  for (i = 0; i < Hv_NUMPOPUP; i++) {
      XtSetSensitive(Hv_popupItems[i], Hv_CheckBit(View->popupcontrol, checkbit));
      checkbit *= 2;
  }

/* check the undo menu, which depends on both its bit being set
   and on the presence of "dirty" items */

  Hv_CheckUndoMenu(View);

}

/* ----- Hv_DoQuickZoom ----- */

static void	Hv_DoQuickZoom(Widget	w,
			       XtPointer	client_data,
			       XtPointer	call_data)

/* Dispatch routine for selections from Popup menu */

{

/* doesn't do anything unless user provided a handler */

  if (Hv_QuickZoom != NULL)
    Hv_QuickZoom(w, client_data, call_data);
}


/* ----- Hv_ToggleZoomResize ----- */

static void Hv_ToggleZoomResize(Widget	w,
				XtPointer	client_data,
				XtPointer	call_data)

{
  Hv_ToggleBit(&(Hv_hotView->drawcontrol), Hv_ZOOMONRESIZE);
}

/* ----- Hv_DoHelpMenu ----- */

static void	Hv_DoHelpMenu(Widget	w,
			      XtPointer	client_data,
			      XtPointer	call_data)

/* Dispatch routine for standard selection(s) from Help menu */

{
  int     MenuItem;   /* ID of item selected */

  MenuItem = (int)client_data;    /* this will be the item selected */
  
  switch (MenuItem) {
    
  case Hv_STANDARDHELP:
    Hv_DoHelp();
    break;
    
  case Hv_BALLOONHELP:
    XtUnmanageChild(w);
    Hv_showBalloons = !Hv_showBalloons;
    if (Hv_showBalloons)
      Hv_ChangeLabel(w, "Hide Balloons", Hv_DEFAULT);
    else
      Hv_ChangeLabel(w, "Show Balloons", Hv_DEFAULT);
    XtManageChild(w);
    break;
    
  default:
    break;

  }
}
 
/* ----- Hv_EditBackgroundColor ----- */

static void  Hv_EditBackgroundColor(Hv_Widget w)

{
  Hv_Item    version;

  if (Hv_ModifyColor(Hv_canvas, &Hv_canvasColor,
		     "main window background", False)) {

/* change version string background, if there is one */

    if (Hv_welcomeView != NULL)
      if ((version = Hv_GetItemFromTag(Hv_welcomeView->items, Hv_VERSIONSTRINGTAG)) != NULL) {
      
	version->str->fillcol = Hv_canvasColor;
	if (Hv_canvasColor == Hv_black)
	  version->str->strcol = Hv_white;
      }
    
    
    Hv_ReTileMainWindow();
  }
}
 
/* ----- Hv_DoPopupMenu ----- */

static void	Hv_DoPopupMenu(Widget	w,
			       XtPointer	client_data,		
			       XtPointer	call_data)

/* Dispatch routine for selections from Popup menu */

{
  int             MenuItem;   /* ID of item selected */
  float           xmin, xmax, ymin, ymax;
  Hv_ViewMapData  vmdata;

  MenuItem = (int)client_data;    /* this will be the item selected */
  
  switch (MenuItem) {
    
  case Hv_REFRESHVIEW:
    Hv_SetViewDirty(Hv_hotView);
    Hv_DrawView(Hv_hotView, NULL);
    break;

  case Hv_UNDOZOOM:
    vmdata = Hv_GetViewMapData(Hv_hotView);

    if (Hv_hotView->lastworld != NULL) {

/* use the existence of mapdata as a sign
that this is a map view, for which lastworld
actually stores lats and longs  (for Mercator) */


      if ((Hv_hotView->mapdata != NULL) && (vmdata != NULL) && (vmdata->projection == Hv_MERCATOR))
	Hv_GetGoodMapLimits(Hv_hotView,
			    Hv_hotView->lastworld->xmin,
			    Hv_hotView->lastworld->xmax,
			    Hv_hotView->lastworld->ymin,
			    Hv_hotView->lastworld->ymax,
			    &xmin, &xmax, &ymin, &ymax, True);
      else
	Hv_QuickZoomView(Hv_hotView,
			 Hv_hotView->lastworld->xmin,
			 Hv_hotView->lastworld->xmax,
			 Hv_hotView->lastworld->ymin,
			 Hv_hotView->lastworld->ymax);
    }
    break;

/* note: for plot views, the next item is really "print this plot only */

  case Hv_CUSTOMIZEVIEW:  /*if customize routine, call it*/
    if (Hv_hotView != NULL) {
      if (Hv_hotView->tag == Hv_XYPLOTVIEW) {
	if (Hv_hotPlot != NULL) {
	  Hv_printThisPlotOnly = Hv_hotPlot;
	  Hv_SendViewToFront(Hv_hotView);
	  Hv_psWholeScreen = False;
	  Hv_PrinterSetup(Hv_hotView);
	  Hv_printThisPlotOnly = NULL;
	}
      } 
      
      else if (Hv_hotView->customize)
	Hv_hotView->customize(Hv_hotView);
    }
    break;
    
  case Hv_SENDBACKVIEW:
    Hv_SendViewToBack(Hv_hotView);
    break;

  case Hv_DELETEVIEW:
    if (Hv_Question("Are you sure you want to delete this view from memory?"))
      Hv_FreeView(Hv_hotView);  /* open the question window with the delete mess */
    break;

  case Hv_PRINTVIEW:
    Hv_SendViewToFront(Hv_hotView);
    Hv_psWholeScreen = False;
    Hv_PrinterSetup(Hv_hotView);
    break;

  case Hv_65PERCENTVIEW:
    Hv_PercentZoom(Hv_hotView, 0.65);
    break;

  case Hv_85PERCENTVIEW:
    Hv_PercentZoom(Hv_hotView, 0.85);
    break;

  case Hv_110PERCENTVIEW:
    Hv_PercentZoom(Hv_hotView, 1.1);
    break;

  case Hv_125PERCENTVIEW:
    Hv_PercentZoom(Hv_hotView, 1.25);
    break;

  case Hv_VARZOOMVIEW:
    Hv_VariableZoom(Hv_hotView);
    break;

  case Hv_DEFAULTVIEW:
    if (Hv_hotView->tag == Hv_XYPLOTVIEW)  {

/* trick the zoomplot routine into thinking
   this was a shift middle mouse button click */

      plotevent->xbutton.state += ShiftMask;
      Hv_ZoomPlot(Hv_hotView, plotpp, plotevent);
    }
    else
      Hv_RestoreDefaultWorld(Hv_hotView);
    break;

  default:
    break;

  }
}

/* ---- Hv_DoPopup2AndActionMenu -----*/

static void Hv_DoPopup2AndActionMenu(Widget	w,
				     XtPointer	client_data,
				     XtPointer	call_data)

{
  int        MenuItem;   /* ID of item selected */
  Hv_View  temp;

  MenuItem = (int)client_data;    /* this will be the item selected */
  
  switch (MenuItem) {

    case Hv_REFRESHSCREEN:
      for (temp = Hv_views.first; temp != NULL; temp = temp->next) {
	Hv_SetViewDirty(temp);
	Hv_DrawView(temp, NULL);
      }
      break;

    case Hv_PRINTSCREEN:
      Hv_psWholeScreen = True;     /* tells us to capture whole screen, not just one View */
      Hv_PrinterSetup(NULL);
      break;


    case Hv_QUIT:
      if (Hv_libMode)
	Hv_libDone = True;
      else if (w == Hv_quitPopupItem)
	exit(0);
      else if (Hv_Question("Do you really want to quit?"))
	exit(0);    /* open the question box with the quit message */
      break;
      
    default:
      break;
    
  }
}

/*------ Hv_ManageMenuItem -------*/

static void   Hv_ManageMenuItem(Hv_Widget menu,
				Widget w)

/* when adding to some standard menus, rearrange some
   items. For example, when adding to the Action menu,
   make sure that the last two items are always a separator
   and Quit */

{
  if (!Hv_userItem) {
    XtManageChild(w);
    return;
  }

  if (menu == Hv_actionMenu) {
    XtUnmanageChild(Hv_quitSepItem);
    XtUnmanageChild(Hv_quitItem);
    XtDestroyWidget(Hv_quitSepItem);
    XtDestroyWidget(Hv_quitItem);
    XtManageChild(w);

    Hv_userItem = False;

    Hv_quitSepItem = Hv_VaCreateMenuItem(Hv_actionMenu, Hv_TYPE, Hv_SEPARATOR, NULL); 

    Hv_quitItem = Hv_VaCreateMenuItem(Hv_actionMenu,
				      Hv_LABEL,           "Quit...",
				      Hv_ID,              Hv_QUIT,
				      Hv_CALLBACK,        Hv_DoPopup2AndActionMenu,
				      Hv_ACCELERATORTYPE, Hv_ALT,
				      Hv_ACCELERATORCHAR, 'Q',
				      NULL);
    Hv_userItem = True;
  }
  else
    XtManageChild(w);

}



/* ----- Hv_CreatePopup1 -----*/

static void Hv_CreatePopup1(void)

{
  int     i;


  Hv_userItem = False;       /*now adding standard menus */

  Hv_VaCreateMenu(&Hv_popupMenu, Hv_TYPE, Hv_POPUPMENUTYPE, NULL);

/* the popup menu for the views has two submenus */

  Hv_VaCreateMenu(&Hv_quickZoomSubmenu,
		  Hv_TYPE,    Hv_SUBMENUTYPE,
		  Hv_PARENT,  Hv_popupMenu,
		  NULL);

  Hv_VaCreateMenu(&Hv_undoSubmenu,
		  Hv_TYPE,    Hv_SUBMENUTYPE,
		  Hv_PARENT,  Hv_popupMenu,
		  NULL);


/* WARNING: the popup items are defined bitwise */


  Hv_popupItems[Hv_FirstSetBit(Hv_REFRESHVIEW)] =
    Hv_VaCreateMenuItem(Hv_popupMenu,
			Hv_LABEL,           "Refresh View",
			Hv_ID,              Hv_REFRESHVIEW,
			Hv_CALLBACK,        Hv_DoPopupMenu,
			NULL);
  
  
  Hv_popupItems[Hv_FirstSetBit(Hv_CUSTOMIZEVIEW)] =
    Hv_VaCreateMenuItem(Hv_popupMenu,
			Hv_LABEL,           "Customize...",
			Hv_ID,              Hv_CUSTOMIZEVIEW,
			Hv_CALLBACK,        Hv_DoPopupMenu,
			NULL);


  Hv_popupItems[Hv_FirstSetBit(Hv_SENDBACKVIEW)] =
    Hv_VaCreateMenuItem(Hv_popupMenu,
			Hv_LABEL,           "Send to Back  [Sh-MB1]",
			Hv_ID,              Hv_SENDBACKVIEW,
			Hv_CALLBACK,        Hv_DoPopupMenu,
			NULL);

  Hv_popupItems[Hv_FirstSetBit(Hv_DELETEVIEW)] =
    Hv_VaCreateMenuItem(Hv_popupMenu,
			Hv_LABEL,           "Delete View...",
			Hv_ID,              Hv_DELETEVIEW,
			Hv_CALLBACK,        Hv_DoPopupMenu,
			NULL);
  
  Hv_popupItems[Hv_FirstSetBit(Hv_PRINTVIEW)] =
    Hv_VaCreateMenuItem(Hv_popupMenu,
			Hv_LABEL,           "Print View...",
			Hv_ID,              Hv_PRINTVIEW,
			Hv_CALLBACK,        Hv_DoPopupMenu,
			NULL);


  Hv_popupItems[Hv_FirstSetBit(Hv_SEPVIEW)] =
    Hv_VaCreateMenuItem(Hv_popupMenu, Hv_TYPE, Hv_SEPARATOR, NULL);

  Hv_popupItems[Hv_FirstSetBit(Hv_UNDOVIEW)] =
    Hv_VaCreateMenuItem(Hv_popupMenu,
			Hv_TYPE,            Hv_SUBMENUITEM,
			Hv_SUBMENU,         Hv_undoSubmenu,
			Hv_LABEL,           "Undo",
			NULL);

  Hv_popupItems[Hv_FirstSetBit(Hv_QUICKZOOM)] =
    Hv_VaCreateMenuItem(Hv_popupMenu,
			Hv_TYPE,            Hv_SUBMENUITEM,
			Hv_SUBMENU,         Hv_quickZoomSubmenu,
			Hv_LABEL,           "Quick Zoom",
			NULL);
  

  Hv_popupItems[Hv_FirstSetBit(Hv_65PERCENTVIEW)] =
    Hv_VaCreateMenuItem(Hv_popupMenu,
			Hv_LABEL,           "Zoom In (65%)",
			Hv_ID,              Hv_65PERCENTVIEW,
			Hv_CALLBACK,        Hv_DoPopupMenu,
			NULL);


  Hv_popupItems[Hv_FirstSetBit(Hv_85PERCENTVIEW)] =
    Hv_VaCreateMenuItem(Hv_popupMenu,
			Hv_LABEL,           "Zoom In (85%)",
			Hv_ID,              Hv_85PERCENTVIEW,
			Hv_CALLBACK,        Hv_DoPopupMenu,
			NULL);
  
  Hv_popupItems[Hv_FirstSetBit(Hv_110PERCENTVIEW)] =
    Hv_VaCreateMenuItem(Hv_popupMenu,
			Hv_LABEL,           "Zoom Out (110%)",
			Hv_ID,              Hv_110PERCENTVIEW,
			Hv_CALLBACK,        Hv_DoPopupMenu,
			NULL);

  Hv_popupItems[Hv_FirstSetBit(Hv_125PERCENTVIEW)] =
    Hv_VaCreateMenuItem(Hv_popupMenu,
			Hv_LABEL,           "Zoom Out (125%)",
			Hv_ID,              Hv_125PERCENTVIEW,
			Hv_CALLBACK,        Hv_DoPopupMenu,
			NULL);
  
  Hv_popupItems[Hv_FirstSetBit(Hv_VARZOOMVIEW)] =
    Hv_VaCreateMenuItem(Hv_popupMenu,
			Hv_LABEL,           "Variable Zoom...",
			Hv_ID,              Hv_VARZOOMVIEW,
			Hv_CALLBACK,        Hv_DoPopupMenu,
			NULL);
  

  Hv_popupItems[Hv_FirstSetBit(Hv_DEFAULTVIEW)] =
    Hv_VaCreateMenuItem(Hv_popupMenu,
			Hv_LABEL,           "Default World  [Sh-MB2]",
			Hv_ID,              Hv_DEFAULTVIEW,
			Hv_CALLBACK,        Hv_DoPopupMenu,
			NULL);

  Hv_popupItems[Hv_FirstSetBit(Hv_UNDOZOOM)] =
    Hv_VaCreateMenuItem(Hv_popupMenu,
			Hv_LABEL,           "Undo Last Zoom",
			Hv_ID,              Hv_UNDOZOOM,
			Hv_CALLBACK,        Hv_DoPopupMenu,
			NULL);

  
  Hv_popupItems[Hv_FirstSetBit(Hv_SEPVIEW2)] =
    Hv_VaCreateMenuItem(Hv_popupMenu, Hv_TYPE, Hv_SEPARATOR, NULL);

  Hv_popupItems[Hv_FirstSetBit(Hv_ZOOMRESIZETOGGLE)] =
    Hv_VaCreateMenuItem(Hv_popupMenu,
			Hv_TYPE,         Hv_TOGGLEMENUITEM,
			Hv_TITLE,        "Zoom View When Resized",
			Hv_STATE,        "Hv_OFF",
			Hv_CALLBACK,     Hv_ToggleZoomResize,
			Hv_TAG,          Hv_ZOOMRESIZETOGGLE,

/* undo items */
									   NULL);
  for (i = 0; i < Hv_NUMUNDOITEMS; i++)
    Hv_lastItems[i] = Hv_VaCreateMenuItem(Hv_undoSubmenu,
					  Hv_LABEL,           Hv_undoLabels[i],
					  Hv_ID,              (long)i,
					  Hv_CALLBACK,        Hv_DoUndo,
					  NULL);

/* now quick zoom */

  for (i = 0; i < Hv_NUMQUICKZOOM; i++)
    Hv_quickZoomItems[i] = Hv_VaCreateMenuItem(Hv_quickZoomSubmenu,
					       Hv_LABEL,           " ",
					       Hv_ID,              (long)i,
					       Hv_CALLBACK,        Hv_DoQuickZoom,
					       NULL);

  Hv_userItem = True;       /*done adding standard menus */
}


/* ----- Hv_CreatePopup2 -----*/

static void Hv_CreatePopup2(void)

{
  Widget  item;

  Hv_userItem = False;       /*now adding standard menus */

  Hv_VaCreateMenu(&Hv_popupMenu2, Hv_TYPE, Hv_POPUPMENUTYPE, NULL);


  item = Hv_VaCreateMenuItem(Hv_popupMenu2,
			  Hv_LABEL,           "Refresh Screen",
			  Hv_ID,              Hv_REFRESHSCREEN,
			  Hv_CALLBACK,        Hv_DoPopup2AndActionMenu,
			  NULL);

  item = Hv_VaCreateMenuItem(Hv_popupMenu2,
			  Hv_LABEL,           "Print Screen...",
			  Hv_ID,              Hv_PRINTSCREEN,
			  Hv_CALLBACK,        Hv_DoPopup2AndActionMenu,
			  NULL);

  item = Hv_VaCreateMenuItem(Hv_popupMenu2, Hv_TYPE, Hv_SEPARATOR, NULL); 

  Hv_quitPopupItem = Hv_VaCreateMenuItem(Hv_popupMenu2,
					 Hv_LABEL,           "Quit...",
					 Hv_ID,              Hv_QUIT,
					 Hv_CALLBACK,        Hv_DoPopup2AndActionMenu,
					 NULL);

  Hv_userItem = True;       /*done adding standard menus */
}


/* ------- Hv_CreatePullDown ---------- */

static Widget	Hv_CreatePullDown(char *title)

{
  XmString      xmst;
  Widget	pdBtn;
  Widget        pd;

  xmst = XmStringCreate(title, XmSTRING_DEFAULT_CHARSET);

  if (Hv_userItem) {
    if (Hv_userMenuCount >= Hv_MAXUSERMENUS) {
      fprintf(stderr, "Hv warning: cannot create more than %2d additional menus\n", Hv_MAXUSERMENUS);
      pd = NULL;
    }
    else {
      Hv_userMenuTitles[Hv_userMenuCount] = XmStringCreate(title, XmSTRING_DEFAULT_CHARSET);
      pd = Hv_userMenus[Hv_userMenuCount];
      XtVaSetValues(Hv_userPDBtns[Hv_userMenuCount], XmNlabelString, xmst, NULL);
      XtSetSensitive(Hv_userPDBtns[Hv_userMenuCount], True);
      Hv_userMenuCount++;
    }
  }
  
  else {
    pd = XmCreatePulldownMenu(Hv_menuBar, "pulldown", NULL, 0);
    pdBtn = XtVaCreateManagedWidget("menubtn", 
				    xmCascadeButtonWidgetClass,  Hv_menuBar,
				    XmNlabelString,              xmst,
				    XmNsubMenuId,                pd,
				    NULL);

  }

  XmStringFree(xmst);
  return pd;
}


/* ------- Hv_AddMenuSeparator -------- */

static Widget Hv_AddMenuSeparator(Hv_Widget menu)

/* add a separator bar to a menu */

{
  Widget		w;		/* returned widget */

  if (menu == NULL)
    return NULL;

  w = (Widget)XmCreateSeparatorGadget(menu, "sep", NULL, 0);
  Hv_ManageMenuItem(menu, w);
  return w;
}

/* ------ Hv_AddSubMenuItem --------- */

static Widget Hv_AddSubMenuItem(String	ItemLabel,
				Widget     parentmenu,
				Widget	submenu)

/* add a sub (hierarchical) menu to a parent menu */

{
  Widget	w;		/* returned widget */
  XmString      xmst;

  w = (Widget)XmCreateCascadeButtonGadget(parentmenu, "submenuitem", NULL, 0);

  xmst = XmStringCreate(ItemLabel, XmSTRING_DEFAULT_CHARSET);
  XtVaSetValues(w,
		XmNlabelString, xmst,
		XmNsubMenuId, submenu,
		NULL);
  

  XmStringFree(xmst);
  Hv_ManageMenuItem(parentmenu, w);
  return w;
}


/* ------Hv_AddMenuToggle --------- */

static Widget Hv_AddMenuToggle(String	      ItemLabel,
			       Widget         menu,
			       long           ID,
			       XtCallbackProc CBProcess,
			       Boolean	      State,
			       unsigned char  btype,
			       int	      acctype,
			       char	      accchr)

/* add a toggle button item to a menu 

String		ItemLabel	 text for menu item 
Widget		menu		 menu parent of this item 
long            ID               the menu ID 
XtCallbackProc	CBProcess	 callback function 
Boolean		State		 initial state 
int		acctype		 type of accelerator 
char		accchr		 accelerator character 

*/

{
  Arg		args[10];	    /* Args are used to set/retrieve properties of resources*/
  int		n = 0;		    /* argument counter */
  Widget	w;		    /* returned widget */
  char	        *str1;		    /* test string */
  char  	str2[11];		    /* test string */
  XmString      xmst1 = NULL;
  XmString      xmst2 = NULL;
  char          *acstr;

/* set the resources for the toggle item */

  xmst1 = XmStringCreate(ItemLabel, XmSTRING_DEFAULT_CHARSET);
  XtSetArg(args[n], XmNlabelString, xmst1);	n++;
  XtSetArg(args[n], XmNset, State);	        n++;
  XtSetArg(args[n], XmNindicatorSize, 14);	n++;
  XtSetArg(args[n], XmNindicatorType, btype);	n++;
  XtSetArg(args[n], XmNvisibleWhenOff, True);	n++;


/* see if an accelerator is requested */

  if ((accchr != ' ') && (acctype != Hv_NONE)) {
    
    acstr = (char *)Hv_Malloc(2);
    acstr[0] = accchr;
    acstr[1] = '\0';


    switch (acctype) {
    case Hv_ALT:
      str1 = (char *) Hv_Malloc(strlen("Alt/") + 3);
      strcpy(str1, "Alt/");

      strcpy(str2, "Mod1<Key> ");
      break;

/* default case: CTRL */

    default: 
      str1 = (char *) Hv_Malloc(strlen("Ctrl/") + 3);
      strcpy(str1,"Ctrl/");
      
      strcpy(str2, "Ctrl<Key> ");
      break;
      
    }

    strcat(str1, acstr);
    Hv_Free(acstr);

    xmst2 = XmStringCreate(str1, XmSTRING_DEFAULT_CHARSET);
    XtSetArg(args[n], XmNacceleratorText, xmst2); n++;
    
    str2[9]  = accchr;
    str2[10] = '\0';

    XtSetArg(args[n], XmNaccelerator, str2);    n++;
    Hv_Free(str1);
  }
  
  w = XmCreateToggleButtonGadget(menu, "toggleitem", args, n);
  XtAddCallback(w, XmNvalueChangedCallback, CBProcess, (XtPointer)ID);
  
  XmStringFree(xmst1);
  if (xmst2 != NULL)
    XmStringFree(xmst2);

  Hv_ManageMenuItem(menu, w);
  return w;
}


/* ----- Hv_AddMenuItem ------- */

static Widget Hv_AddMenuItem(String	ItemLabel,
			     Widget	menu,
			     long      ID,
			     XtCallbackProc	CBProcess,
			     int	acctype,
			     char	accchr)		

/* adds a standard item to a menu 

String		ItemLabel	text for menu item
Widget		menu		menu parent of this item
long            ID              the menu ID
XtCallbackProc	CBProcess	callback function
int		acctype		type of accelerator
char		accchr		accelerator character

*/

{
  Arg		args[10];	   /* Args are used to set/retrieve properties of resources*/
  int		n = 0;		   /* argument counter */
  char		*str1;		   /* test string */
  char  	str2[11];	   /* test string */
  Widget	w;		   /* returned widget */
  XmString      xmst1 = NULL;
  XmString      xmst2 = NULL;
  char          acstr[2];

  if (menu == NULL)
    return NULL;

/* now set the resources for the menuitem */

  xmst1 = XmStringCreate(ItemLabel, XmSTRING_DEFAULT_CHARSET);
  XtSetArg(args[n], XmNlabelString, xmst1);	n++;

/* see if an accelerator is requested */


  if ((accchr != ' ') && (acctype != Hv_NONE)) {
    switch (acctype) {
    case Hv_ALT:
      str1 = (char *) Hv_Malloc(strlen("Alt/") + 3);
      strcpy(str1,"Alt/");
      
      strcpy(str2, "Mod1<Key> ");
      break;
      
/* default case: CTRL */
	  
    default: 
      str1 = (char *) Hv_Malloc(strlen("Ctrl/") + 3);
      strcpy(str1,"Ctrl/");
      
      strcpy(str2, "Ctrl<Key> ");
      break;
    }
      
    acstr[0] = accchr;
    acstr[1] = '\0';
    strcat(str1, acstr);

    xmst2 = XmStringCreate(str1, XmSTRING_DEFAULT_CHARSET);
    XtSetArg(args[n], XmNacceleratorText, xmst2); n++;
    
    str2[9]  = accchr;
    str2[10] = '\0';

    XtSetArg(args[n], XmNaccelerator, str2);    n++;
    Hv_Free(str1);
  }
  
  w = XmCreatePushButtonGadget(menu, "menuitem", args, n);
  XtAddCallback(w, XmNactivateCallback, CBProcess, (XtPointer)ID);
  
  
  XmStringFree(xmst1);
  if (xmst2 != NULL)
    XmStringFree(xmst2);

  Hv_ManageMenuItem(menu, w);
  return w;
}


/* ------- Hv_CreateSubMenu ---------- */

static Widget	Hv_CreateSubMenu(Hv_Widget parent)

{
  if (parent == NULL)
    return NULL;
  return  XmCreatePulldownMenu(parent, "popup", NULL, 0);
}















