/**
 * <EM>Deals with windowing system independent (non-gateway) menu routines.</EM>
 * <P>
 * The Hv library was developed at The Thomas Jefferson National
 * Accelerator Facility under contract to the
 * Department of Energy and is the property of they U.S. Government.
 * Partial support came from the National Science Foundation.
 * <P>
 * It may be used to develop commercial software, but the Hv
 * source code and/or compiled modules/libraries may not be sold.
 * <P>
 * Questions or comments should be directed
 * to <a href="mailto:heddle@cebaf.gov">heddle@cebaf.gov</a> <HR>
 */


#include "Hv.h"
#include "Hv_xyplot.h"
#include "Hv_undo.h"
#include "Hv_maps.h"

extern       Hv_Item        Hv_hotPlot;

Hv_Widget    Hv_lastItems[Hv_NUMUNDOITEMS];

/* local prototypes */

static void       Hv_SetPopupActivity(Hv_View  View);

static void	  Hv_DoQuickZoom(Hv_Widget	w,
				 Hv_Pointer	data);

static void       Hv_ToggleZoomResize();

static void	  Hv_DoHelpMenu(Hv_Widget	w,
				Hv_Pointer	data);

static void       Hv_EditBackgroundColor();	

static void	  Hv_DoPopupMenu(Hv_Widget	w,
				 Hv_Pointer	data);

static void       Hv_CreatePopup1(void);

static void       Hv_CreatePopup2(void);

Hv_Widget    Hv_quitSepItem;

/* variables global for this file */

static  Hv_Point     plotpp;
static  Hv_XEvent   *plotevent;

static char *Hv_undoLabels[] = {
  "Last Item Drag",
  "Last Item Rotate",
};


/**
 * Hv_VaCreateMenu    Use variable length arg list of attribute
 * value pairs to create a menu (Generic)
 * @param      menu    pointer to created menu
 * @param      ...     var length list of attribute values
 */

void Hv_VaCreateMenu(Hv_Widget *menu,
		     ...) {

    va_list           ap;
    Hv_AttributeArray attributes;

    *menu = NULL;
    va_start(ap, menu);

/* with new ansi format the menu widget itself is the
   first and only required argument */

    Hv_GetAttributes(ap, attributes, NULL, NULL, NULL, NULL, NULL);

    switch (attributes[Hv_TYPE].s) {
    case Hv_SUBMENUTYPE:
	*menu = Hv_CreateSubmenu((Hv_Widget)(attributes[Hv_PARENT].v));
	break;
    
    case Hv_POPUPMENUTYPE:
	*menu = Hv_CreatePopup(Hv_canvas);
	break;

    case Hv_HELPMENUTYPE:
	*menu = Hv_CreateHelpPullDown();
	break;
    
    default:
	*menu = Hv_CreatePullDown((char *)(attributes[Hv_TITLE].v));
	break;
    }
}

/**
 * Hv_VaCreateMenuItem    Use variable length arg list of attribute
 * value pairs to create a menu item (Generic)
 * @param      menu    the menu parent
 * @param      ...     var length list of attribute values
 */

Hv_Widget       Hv_VaCreateMenuItem(Hv_Widget menu,
				    ...) {
  va_list           ap;
  Hv_AttributeArray attributes;
  Hv_Widget         menuitem = NULL;

  va_start(ap, menu);

/* Now get the variable arguments */

  Hv_GetAttributes(ap, attributes, NULL, NULL, NULL, NULL, NULL);


  switch (attributes[Hv_TYPE].s) {
  case Hv_SEPARATOR:
      menuitem = Hv_AddMenuSeparator(menu);
      break;
    
  case Hv_TOGGLEMENUITEM:
      menuitem = Hv_AddMenuToggle((char *)(attributes[Hv_LABEL].v),
				  menu,
				  attributes[Hv_ID].l,
				  (Hv_CallbackProc)(attributes[Hv_CALLBACK].fp),
				  (Boolean)(attributes[Hv_STATE].s),
				  (unsigned char)(attributes[Hv_MENUTOGGLETYPE].s),
				  attributes[Hv_ACCELERATORTYPE].s, attributes[Hv_ACCELERATORCHAR].c);
    break;
    
  case Hv_SUBMENUITEM:
      menuitem = Hv_AddSubMenuItem((char *)(attributes[Hv_LABEL].v),
				   menu,
				   (Hv_Widget)(attributes[Hv_SUBMENU].v));
    break;
    
  default:   /* standard type */
    menuitem = Hv_AddMenuItem((char *)(attributes[Hv_LABEL].v),
			      menu,
			      attributes[Hv_ID].l,
			      (Hv_CallbackProc)(attributes[Hv_CALLBACK].fp),
			      attributes[Hv_ACCELERATORTYPE].s,
			      attributes[Hv_ACCELERATORCHAR].c);
    break;
  }
  
  return  menuitem;
}



/**
* Hv_SetQuickZoomLabel   Set quick zoom label (generic)
* @param     item    an integer [0.. (Hv_NUMQUICKZOOM-1)] specifying the label
* @param     str     the label
*/


void Hv_SetQuickZoomLabel(short   item,
                          char   *str) {
  
  if ((item < 0) || (item >= Hv_NUMQUICKZOOM)) {
    Hv_Println("Hv warning: attempt to set quick zoom label with out of range index.");
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

void Hv_CreateMenuBar(void) {
    Hv_Widget   item;

/* we like our menubars to be in the usual place ... along the top of the window.
   This also creates the standard menus */

  Hv_menuBar = Hv_CreateMainMenuBar(Hv_form);

  Hv_userMenu = False;       /*now adding standard menus */

/* add menubar to form's list of managed widgets */

  Hv_ManageChild(Hv_menuBar);

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

  item = Hv_VaCreateMenuItem(Hv_actionMenu,
			  Hv_LABEL,           "Background Color...",
			  Hv_ID,              Hv_CHANGEBACKGROUND,
			  Hv_CALLBACK,        Hv_EditBackgroundColor,
			  NULL);


  Hv_quitSepItem = Hv_VaCreateMenuItem(Hv_actionMenu, Hv_TYPE, Hv_SEPARATOR, NULL); 

  Hv_quitItem = Hv_VaCreateMenuItem(Hv_actionMenu,
				    Hv_LABEL,           "Quit...",
				    Hv_ID,              Hv_QUIT,
				    Hv_CALLBACK,        Hv_DoPopup2AndActionMenu,
				    Hv_ACCELERATORTYPE, Hv_ALT,
				    Hv_ACCELERATORCHAR, 'Q',
				    NULL);
  
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

  Hv_userMenu = True;       /*done adding standard menus */
}


/**
 * Hv_CreatePopups    Creates the two main popups (generic)
 */



void Hv_CreatePopups(void) {
  Hv_CreatePopup1();
  Hv_CreatePopup2();
}



/**
 * Hv_ViewPopup     Handle a popup invocation on a view (generic)
 * @param       View   The view upon which we popped up
 * @param       event  The actual event
 */

void  Hv_ViewPopup(Hv_View  View,
		   Hv_XEvent  *event) {

/* set the popup menu to refelect the most common state */
    
  int        zoomresizebit;

  if (Hv_modalView != NULL) {
      if (View != Hv_modalView)
	  Hv_SysBeep();
      return;
  }

  zoomresizebit = Hv_FirstSetBit(Hv_ZOOMRESIZETOGGLE);
  Hv_SetToggleButton(Hv_popupItems[zoomresizebit],
		     Hv_CheckBit(View->drawcontrol,Hv_ZOOMONRESIZE));
  
  
/* if it is a plot view, need some further checking */


  if (View->tag == Hv_XYPLOTVIEW) {

      
      Hv_SetMenuItemString(Hv_popupItems[Hv_FirstSetBit(Hv_CUSTOMIZEVIEW)],
			   "Print THIS PLOT ONLY...", Hv_fixed2);
      
      Hv_SetPoint(&plotpp,
		  Hv_GetXEventX(event),
		  Hv_GetXEventY(event));
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
  
}


/**
 * Hv_SetPopupActivitity     Sets sensitivity of popup items (generic)
 * @param    View   the view getting a popup menu
 */

static void Hv_SetPopupActivity(Hv_View View) {
  int i;
  int checkbit = 1;


  if (View == NULL)
    return;

  for (i = 0; i < Hv_NUMPOPUP; i++) {

    if (Hv_popupItems[i] != NULL) {
      Hv_SetSensitivity(Hv_popupItems[i],
        Hv_CheckBit(View->popupcontrol, checkbit));
    }

    checkbit *= 2;
  }

/* check the undo menu, which depends on both its bit being set
   and on the presence of "dirty" items */
  
  Hv_CheckUndoMenu(View);

}


/**
 * Hv_DoQuickZoom   Handle a quick zoom menu selection (generic)
 */

static void	Hv_DoQuickZoom(Hv_Widget	w,
			       Hv_Pointer	data) {

/* Dispatch routine for selections from Popup menu 
   doesn't do anything unless user provided a handler */

  if (Hv_QuickZoom != NULL)
    Hv_QuickZoom(w, data);

}


/**
 * Hv_ToggleZoomResize  Toggles what a resize does visa vis zooming (generic)
 */


static void Hv_ToggleZoomResize() {
    Hv_ToggleBit(&(Hv_hotView->drawcontrol), Hv_ZOOMONRESIZE);
}

/**
 * Hv_DoHelMenu    Handle the help menu   (generic)
 * @param    w       The help menu widget
 * @param    data    Will determine which item
 */


static void	Hv_DoHelpMenu(Hv_Widget	   w,
			      Hv_Pointer   data) {

/* Dispatch routine for standard selection(s) from Help menu */

  int     MenuItem;   /* ID of item selected */

  MenuItem = (int)data;    /* this will be the item selected */
  
  switch (MenuItem) {
    
  case Hv_STANDARDHELP:
    Hv_DoHelp();
    break;
    
  case Hv_BALLOONHELP:
    Hv_UnmanageChild(w);
    Hv_showBalloons = !Hv_showBalloons;
    if (Hv_showBalloons)
      Hv_ChangeLabel(w, "Hide Balloons", Hv_DEFAULT);
    else
      Hv_ChangeLabel(w, "Show Balloons", Hv_DEFAULT);
    Hv_ManageChild(w);
    break;
    
  default:
      break;

  }
}
 

/**
 * Hv_EditBackgroundColor 
 * @purpose Edit background color of main window.
 * @local
 */

static void  Hv_EditBackgroundColor() {
  Hv_Item    version;

  if (Hv_ModifyColor(Hv_canvas,
		     &Hv_canvasColor,
		     "main window background",
		     False)) {

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
 

/**
 * Hv_DoPopupMenu  Dispatch routine for selections from main Popup menu (generic)
 * @param     w    the popup menu widget
 * @param     data    Will determine which item
 */


static void	Hv_DoPopupMenu(Hv_Widget	w,
			       Hv_Pointer	data) {		

  int             MenuItem;   /* ID of item selected */
  float           xmin, xmax, ymin, ymax;
  Hv_ViewMapData  vmdata;


/* If modal dialog is up, do nothing except beep */

  if (Hv_modalView != NULL) {
    if (Hv_hotView != Hv_modalView)
      Hv_SysBeep();
    return;
  }

  Hv_allowMag = False;

  MenuItem = (int)data;    /* this will be the item selected */
  
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
    Hv_PercentZoom(Hv_hotView, (float)0.65);
    break;

  case Hv_85PERCENTVIEW:
    Hv_PercentZoom(Hv_hotView, (float)0.85);
    break;

  case Hv_110PERCENTVIEW:
    Hv_PercentZoom(Hv_hotView, (float)1.1);
    break;

  case Hv_125PERCENTVIEW:
    Hv_PercentZoom(Hv_hotView, (float)1.25);
    break;

  case Hv_VARZOOMVIEW:
    Hv_VariableZoom(Hv_hotView);
    break;

  case Hv_DEFAULTVIEW:
    if (Hv_hotView->tag == Hv_XYPLOTVIEW)  {

/* trick the zoomplot routine into thinking
   this was a shift middle mouse button click */

      Hv_AddModifier(plotevent, Hv_SHIFT);
      Hv_ZoomPlot(Hv_hotView, plotpp, plotevent);
    }
    else
      Hv_RestoreDefaultWorld(Hv_hotView);
    break;

  default:
    break;

  }

  Hv_allowMag = True;

}

/**
 * Hv_DoPopupMenu2AndActionMenu  Dispatch routine for selections from 2nd global Popup menu (generic)
 * @param     w    the 2nd global popup menu widget
 * @param     data    Will determine which item
 */

void Hv_DoPopup2AndActionMenu(Hv_Widget	w,
				     Hv_Pointer	data) {

  int        MenuItem;   /* ID of item selected */
  Hv_View  temp;


/* If modal dialog is up, do nothing except beep */

  if (Hv_modalView != NULL) {
    if (Hv_hotView != Hv_modalView)
      Hv_SysBeep();
    return;
  }

 
  Hv_allowMag = False;

  MenuItem = (int)data;    /* this will be the item selected */
  
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
      if (w == Hv_quitPopupItem)
		  Hv_ShutDown();
      else if (Hv_Question("Do you really want to quit?"))
		  Hv_ShutDown();    /* open the question box with the quit message */
      break;
      
    default:
      break;
    
  }

  Hv_allowMag = True;

}

/**
 * Hv_CreatePopup1     Creates the main view popup (generic)
 */


static void Hv_CreatePopup1(void) {
  int     i;


  Hv_userMenu = False;       /*now adding standard menus */


  Hv_popupMenu = Hv_CreatePopup(Hv_canvas);

  Hv_AddCallback(Hv_canvas,
		 Hv_inputCallback,
		 (Hv_CallbackProc)Hv_Popup1Callback,
		 (Hv_Pointer)Hv_popupMenu);

/* the popup menu for the views has two submenus */

  Hv_VaCreateMenu(&Hv_quickZoomSubmenu,
		  Hv_TYPE,    Hv_SUBMENUTYPE,
		  Hv_PARENT,  Hv_popupMenu,
		  NULL);

  Hv_VaCreateMenu(&Hv_undoSubmenu,
		  Hv_TYPE,    Hv_SUBMENUTYPE,
		  Hv_PARENT,  Hv_popupMenu,
		  NULL);


  for (i = 0; i < Hv_NUMPOPUP; i++)
    Hv_popupItems[i] = NULL; 
  
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
    Hv_LABEL,        "Zoom View When Resized",
    Hv_STATE,        "Hv_OFF",
    Hv_CALLBACK,     Hv_ToggleZoomResize,
    Hv_TAG,          Hv_ZOOMRESIZETOGGLE,
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

  Hv_userMenu = True;       /*done adding standard menus */
}


/**
 * Hv_CreatePopup2     Creates the 2nd main popup (generic)
 */


static void Hv_CreatePopup2(void) {
  Hv_Widget  item;

  Hv_userMenu = False;       /*now adding standard menus */

  Hv_popupMenu2 = Hv_CreatePopup(Hv_canvas);

  Hv_AddCallback(Hv_canvas,
		 Hv_inputCallback,
		 (Hv_CallbackProc)Hv_Popup2Callback,
		 (Hv_Pointer)Hv_popupMenu2);


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

  Hv_userMenu = True;       /*done adding standard menus */
}










