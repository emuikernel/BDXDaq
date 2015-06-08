/*
----------------------------------------------------
-							
-  File:    menus.c					
-							
-  Summary:						
-            menu routines for ced
-					       
-                     				
-  Revision history:				
-                     				
-  Date       Programmer     Comments		
------------------------------------------------------
-  10/19/94	DPH		Initial Version	
-                                              	
------------------------------------------------------
*/

#include "ced.h"

Hv_Widget      evfromfile;
Hv_Widget      evfromet;
Hv_Widget      evfromdd;
Hv_Widget      evaccumulate;
Hv_Widget      evondemand;
Hv_Widget      evontimer;

/*----- local prototypes ------*/

static void EditColor(Hv_Widget w);

static void PrecountCB(Widget w);

static void  SectorViewQuickZoomResize(Hv_View View,
				       int     choice);

static void  SectorViewQuickZoomOffset(Hv_View View);

static void  SectorViewQuickZoom(Hv_View TheLO,
				 int     choice);

static char  *SectorViewQZLabel(int  sect,
                                 int  n);

static void  SetEventSource(Widget	w,
			    XtPointer  client_data,
			    XtPointer  call_data);	

static void  SetEventWhen(Widget	w,
			  XtPointer  client_data,
			  XtPointer  call_data);	

static char qzbuffer[20];

/*------- color editor variables ---------*/

static Hv_Widget fillcolorlabel0, framecolorlabel0, hitcolorlabel;
static Hv_Widget fillcolorlabel1, framecolorlabel1;
static short     fillcolor0, framecolor0, hitcolor;
static short     fillcolor1, framecolor1;

static Hv_Widget scfillcolorlabel, scframecolorlabel, schitcolorlabel;
static short     scfillcolor, scframecolor, schitcolor;

/* ------- MakeMenus ------- */

void MakeMenus(void)

{
  Hv_Widget   item;
  Hv_Widget   EditMenu, EventMenu, OutputMenu;
  char        text[30];

/* create an edit menu */

  Hv_VaCreateMenu(&EditMenu, Hv_TITLE, " Edit", NULL);

  item = Hv_VaCreateMenuItem(EditMenu,
			     Hv_LABEL,            "Drift Chamber Colors & Parameters...",  
			     Hv_CALLBACK,         EditDCData,
			     Hv_ACCELERATORTYPE,  Hv_CTRL,
			     Hv_ACCELERATORCHAR,  'D',
			     NULL);


  item = Hv_VaCreateMenuItem(EditMenu,
			     Hv_LABEL,            "SC, CC, and EC Colors ...",  
			     Hv_CALLBACK,         EditDetectorColors,
			     NULL);


/* create an Event menu  */
  
  
  Hv_VaCreateMenu(&EventMenu, Hv_TITLE, " Events", NULL);

  evfromfile = Hv_VaCreateMenuItem(EventMenu,
				   Hv_LABEL,            "Events From File",  
				   Hv_CALLBACK,         SetEventSource,
				   Hv_TYPE,             Hv_TOGGLEMENUITEM,
				   Hv_MENUTOGGLETYPE,   Hv_ONEOFMANY,
				   Hv_STATE,            (eventsource == CED_FROMFILE),
				   NULL);

  evfromet = Hv_VaCreateMenuItem(EventMenu,
				 Hv_LABEL,            "Events From ET",  
				 Hv_CALLBACK,         SetEventSource,
				 Hv_TYPE,             Hv_TOGGLEMENUITEM,
				 Hv_MENUTOGGLETYPE,   Hv_ONEOFMANY,
				 Hv_STATE,            (eventsource == CED_FROMET),
				 NULL);

#ifdef NO_ET
  Hv_SetSensitivity(evfromet,  False); 
#endif

  item = Hv_VaCreateMenuItem(EventMenu, Hv_TYPE, Hv_SEPARATOR, NULL); 

  item = Hv_VaCreateMenuItem(EventMenu,
			     Hv_LABEL,            "Event Trigger Filter...",  
			     Hv_CALLBACK,         EventFilterDialog,
			     NULL);


  evondemand = Hv_VaCreateMenuItem(EventMenu,
				     Hv_LABEL,            "Events on Demand",  
				     Hv_CALLBACK,         SetEventWhen,
				     Hv_TYPE,             Hv_TOGGLEMENUITEM,
				     Hv_MENUTOGGLETYPE,   Hv_ONEOFMANY,
				     Hv_STATE,            (eventsource == CED_ONDEMAND),
				     Hv_ACCELERATORTYPE,  Hv_ALT,
				     Hv_ACCELERATORCHAR,  'D',
				     NULL);

  sprintf(text, "Event every %d seconds", eventtime);
  evontimer = Hv_VaCreateMenuItem(EventMenu,
				 Hv_LABEL,             text,  
				 Hv_CALLBACK,          SetEventWhen,
				 Hv_TYPE,              Hv_TOGGLEMENUITEM,
				 Hv_STATE,             (eventwhen == CED_ONTIMER),
				 Hv_MENUTOGGLETYPE,    Hv_ONEOFMANY,
				 Hv_ACCELERATORTYPE,   Hv_ALT,
			         Hv_ACCELERATORCHAR,   'T',
				 NULL);


  evintervalmenu = Hv_VaCreateMenuItem(EventMenu,
				       Hv_LABEL,            "Time Interval...",  
				       Hv_CALLBACK,         SetEventTimeInterval,
				       Hv_ACCELERATORTYPE,  Hv_ALT,
				       Hv_ACCELERATORCHAR,  'I',
				       NULL);

 
  evaccumulate = Hv_VaCreateMenuItem(EventMenu,
				     Hv_LABEL,            "Accumulate Events...",  
				     Hv_CALLBACK,         AccumulateDialog,
				     NULL);

  item = Hv_VaCreateMenuItem(EventMenu, Hv_TYPE, Hv_SEPARATOR, NULL); 
  
  item = Hv_VaCreateMenuItem(EventMenu,
			     Hv_LABEL,            "Event Control...",  
			     Hv_CALLBACK,         EventControl,
			     Hv_ACCELERATORCHAR,  'E',
			     NULL);
  
  evopenmenu = Hv_VaCreateMenuItem(EventMenu,
				   Hv_LABEL,            "Open Event File...",  
				   Hv_CALLBACK,         OpenEventCB,
				   Hv_ACCELERATORCHAR,  'O',
				   NULL);
  

  item = Hv_VaCreateMenuItem(EventMenu, Hv_TYPE, Hv_SEPARATOR, NULL); 
  
  evgotomenu = Hv_VaCreateMenuItem(EventMenu,
				   Hv_LABEL,            "GoTo Event...",  
				   Hv_CALLBACK,         GotoEvent,
				   Hv_ACCELERATORTYPE,  Hv_ALT,
				   Hv_ACCELERATORCHAR,  'G',
				   NULL);
  
  evnextmenu = Hv_VaCreateMenuItem(EventMenu,
				   Hv_LABEL,            "Next Event            (F1)",  
				   Hv_CALLBACK,         NextEvent,
				   Hv_ACCELERATORTYPE,  Hv_CTRL,
				   Hv_ACCELERATORCHAR,  'N',
				   NULL);

  evprevmenu = Hv_VaCreateMenuItem(EventMenu,
				   Hv_LABEL,            "Previous Event     (F2)",  
				   Hv_CALLBACK,         PrevEvent,
				   Hv_ACCELERATORTYPE,  Hv_CTRL,
				   Hv_ACCELERATORCHAR,  'P',
				   NULL);

  item = Hv_VaCreateMenuItem(EventMenu, Hv_TYPE, Hv_SEPARATOR, NULL); 

  evfiltermenu = Hv_VaCreateMenuItem(EventMenu,
				     Hv_LABEL,            "Event File Extension...",  
				     Hv_CALLBACK,         SetEventFileFilter,
				     NULL);

  item = Hv_VaCreateMenuItem(EventMenu,
			     Hv_LABEL,             "Precount Events",  
			     Hv_CALLBACK,          PrecountCB,
			     Hv_TYPE,              Hv_TOGGLEMENUITEM,
			     Hv_STATE,             precountEvents,
			     NULL);


  Hv_VaCreateMenu(&OutputMenu, Hv_TITLE, " Output", NULL);

  evoutputmenu = Hv_VaCreateMenuItem(OutputMenu,
				     Hv_LABEL,            "Open File for Writing Events...",  
				     Hv_CALLBACK,         OpenOutputFile,
				     NULL);

  evwritemenu = Hv_VaCreateMenuItem(OutputMenu,
				    Hv_LABEL,            "Write Event",  
				    Hv_CALLBACK,         WriteEvent,
				    Hv_ACCELERATORTYPE,  Hv_ALT,
				    Hv_ACCELERATORCHAR,  'W',
				    NULL);

  evclosemenu = Hv_VaCreateMenuItem(OutputMenu,
				    Hv_LABEL,            "Close Output File",  
				    Hv_CALLBACK,         CloseOutputFile,
				    NULL);


  Hv_SetSensitivity(evaccumulate,  False); 
  Hv_SetSensitivity(evnextmenu,    False); 
  Hv_SetSensitivity(evprevmenu,    False);
  Hv_SetSensitivity(evgotomenu,    False);


/* modify the action menu */

  item = Hv_VaCreateMenuItem(Hv_actionMenu, Hv_TYPE, Hv_SEPARATOR, NULL); 

/* add a "new view" item to the action menu */


  item = Hv_VaCreateMenuItem(Hv_actionMenu,
			     Hv_LABEL,            "New View",  
			     Hv_CALLBACK,         GetNewView,
			     NULL);


/* add a "read field" item to the action menu */

  item = Hv_VaCreateMenuItem(Hv_actionMenu,
			     Hv_LABEL,            "Read CLAS  Magnetic Field...",  
			     Hv_CALLBACK,         ReadField,
			     NULL);

/* add a "read dead wire" item to the action menu */

  item = Hv_VaCreateMenuItem(Hv_actionMenu,
			     Hv_LABEL,            "Read DC Status Map for Run...",  
			     Hv_CALLBACK,         ReadDeadWireFile,
			     NULL);

/* add an "about" to the Help menu */

  item = Hv_VaCreateMenuItem(Hv_helpMenu,
			     Hv_LABEL,            "About ced...",  
			     Hv_CALLBACK,         DoAboutDlog,
			     NULL);

}

/*------------- CustomizeQuickZoom ------------*/

void CustomizeQuickZoom(Hv_View View)

{
  ViewData     viewdata;
  int          sect;
  int          i;

  viewdata = GetViewData(View);

/* called EVERYTIME view popup is initiated */

  if (View->tag == SECTOR) {
    sect = GoodSector(View) + 1; /* convert to non C index*/
    Hv_SetQuickZoomLabel(0, SectorViewQZLabel(sect, 1));
    Hv_SetQuickZoomLabel(1, SectorViewQZLabel(sect, 2));
    Hv_SetQuickZoomLabel(2, SectorViewQZLabel(sect, 3));
    Hv_SetQuickZoomLabel(3, SectorViewQZLabel(sect, 4));
    sect += 3;
    Hv_SetQuickZoomLabel(4, SectorViewQZLabel(sect, 1));
    Hv_SetQuickZoomLabel(5, SectorViewQZLabel(sect, 2));
    Hv_SetQuickZoomLabel(6, SectorViewQZLabel(sect, 3));
    Hv_SetQuickZoomLabel(7, SectorViewQZLabel(sect, 4));

    for (i = 8; i < Hv_NUMQUICKZOOM; i++)
      Hv_SetQuickZoomLabel(i, "unused");

  } 
  else {
    Hv_SetQuickZoomLabel(0, "Sector 1");
    Hv_SetQuickZoomLabel(1, "Sector 2");
    Hv_SetQuickZoomLabel(2, "Sector 3");
    Hv_SetQuickZoomLabel(3, "Sector 4");
    Hv_SetQuickZoomLabel(4, "Sector 5");
    Hv_SetQuickZoomLabel(5, "Sector 6");
    for (i = 6; i < Hv_NUMQUICKZOOM; i++)
      Hv_SetQuickZoomLabel(i, "unused");


  }

}

/*----------- DoQuickZoom --------*/

void DoQuickZoom(Hv_Widget   w,
		 Hv_Pointer  clientdata)

{
  int            menuitem;
  float          y0, xmin, ymin, w3, h2;

  menuitem = (int)clientdata;
  
  if (Hv_hotView->tag == SECTOR) {
    SectorViewQuickZoom(Hv_hotView, menuitem+1);
    return;
  }


  else if (Hv_hotView->tag == CALORIMETER) {
    if (menuitem == 0)
      Hv_QuickZoomView(Hv_hotView, -580.0, 0.0, -290.0, 290.0);
    else if (menuitem == 1)
      Hv_QuickZoomView(Hv_hotView, -628.0, 0.0, 0.0, 628.0);
    else if (menuitem == 2)
      Hv_QuickZoomView(Hv_hotView, 0.0, 628.0, 0.0, 628.0);
    else if (menuitem == 3)
      Hv_QuickZoomView(Hv_hotView, 0.0, 580.0, -290.0, 290.0);
    else if (menuitem == 4)
      Hv_QuickZoomView(Hv_hotView, 0.0, 628.0, -628.0, 0.0);
    else if (menuitem == 5)
      Hv_QuickZoomView(Hv_hotView, -628.0, 0.0, -628.0, 0.0);
  }

  else if (Hv_hotView->tag == SCINT) {
    if (menuitem == 0)
      Hv_QuickZoomView(Hv_hotView, -1400, 0.0, -700.0, 700.0);
    else if (menuitem == 1)
      Hv_QuickZoomView(Hv_hotView, -1520.0, 0.0, 0.0, 1520.0);
    else if (menuitem == 2)
      Hv_QuickZoomView(Hv_hotView, 0.0, 1520.0, 0.0, 1520.0);
    else if (menuitem == 3)
      Hv_QuickZoomView(Hv_hotView, 0.0, 1400.0, -700.0, 700.0);
    else if (menuitem == 4)
      Hv_QuickZoomView(Hv_hotView, 0.0, 1520.0, -1520.0, 0.0);
    else if (menuitem == 5)
      Hv_QuickZoomView(Hv_hotView, -1520.0, 0.0, -1520.0, 0.0);
  }

  else if (Hv_hotView->tag == ALLDC) {
    if (menuitem > 5)
      return;

    if (menuitem < 3)
      y0 = -0.025*ALLDCWHEIGHT2;
    else
      y0 = 0.025*ALLDCWHEIGHT2;


    h2 = 1.05*ALLDCWHEIGHT2;
    w3 = 2.0*ALLDCWWIDTH2/3.0;
    ymin = y0 - (menuitem/3)*h2;
    xmin = -ALLDCWWIDTH2 + (menuitem%3)*w3;

    Hv_QuickZoomView(Hv_hotView, xmin, xmin+w3, ymin, ymin+h2);
  }

  else
    return;

}


/*----- SectorViewQZLabel ------*/

static char *SectorViewQZLabel(int  sect,
                                 int  n)

{

  if (n < 4)
    sprintf(qzbuffer, "Sector %1d Region %1d", sect, n);
  else
    sprintf(qzbuffer, "Sector %1d Forward Angles", sect);
  
  return qzbuffer;
}


/*----------- SectorViewQuickZoom -------*/

static void  SectorViewQuickZoom(Hv_View View,
				 int     choice)


/* 
   choice 1 - 8 ->
   reg1, reg, reg3, forward angles (uppersector)
   reg1, reg, reg3, forward angles (lower sector)
*/


{

  Hv_Region  reg1 = NULL;
  Hv_Region  reg2 = NULL;
  Hv_Region  reg3 = NULL;

  reg1 = Hv_CreateRegion();       /* beg for region memory */
  reg2 = Hv_CreateRegion();       /* beg for region memory */
  reg3 = Hv_CreateRegion();       /* beg for region memory */

  reg1 = Hv_GetViewRegion(View);  /* get the old drawing region */
  Hv_EraseRegion(reg1);           /* erase that region */

  Hv_drawingOn = False;
  SectorViewQuickZoomOffset(View);
  SectorViewQuickZoomResize(View, choice);

  reg3 = Hv_GetViewRegion(View);     /* get the new drawing region */

  reg2 = Hv_UnionRegion(reg1, reg3);   /* compute the region affected by the quickzoom */
  Hv_SetClippingRegion(reg2);          /* set that region as the clipping area */
  Hv_SetViewDirty(View);

  Hv_drawingOn = True;
  Hv_UpdateViews(reg2);                /* update the region affected by the quickzoom */

/* free the region memory no longer being used */

  Hv_DestroyRegion(reg1);            
  Hv_DestroyRegion(reg2);
  Hv_DestroyRegion(reg3);
}


/*-------- SectorViewQuickZoomOffset ---------*/

static void  SectorViewQuickZoomOffset(Hv_View View)

/* as a first step in the quick zoom process, move
   the View all the way to the left of the window */

{
  short GAP = 5;
  short newx, oldx, dv, dh;

  oldx = View->local->left;
  newx = GAP;

  dh = newx - oldx;
  dv = 0;

  Hv_OffsetView(View, dh, dv);
}

/*-------- SectorViewQuickZoomResize ---------*/

static void  SectorViewQuickZoomResize(Hv_View View,
				       int     choice)

/* 
   choice 1 - 8 ->
   reg1, reg, reg3, forward angles (uppersector)
   reg1, reg, reg3, forward angles (lower sector)
*/


{
  short       GAP = 5;
  float       apr;
  short       dh, dv;
  short       new_hr_width, new_hr_height;
  Hv_FRect    world;
  Hv_Rect     *hotrect = View->hotrect;

  switch(choice) {
  case 1:
    Hv_SetFRect(&world, -82.0,  104.0,  8.0,  73.0);
    break;

  case 2:
    Hv_SetFRect(&world, -168.25, 265.49,  24.13, 200.66);
    break;

  case 3:
    Hv_SetFRect(&world, -260.0, 410.0,  30.0, 315.0);
    break;

  case 4:
    Hv_SetFRect(&world,  35.6,  407.9, 10.9, 309.2);
    break;

  case 5:
    Hv_SetFRect(&world, -82.0, 104.0, -73.0, -8.0);
    break;

  case 6:
    Hv_SetFRect(&world, -168.25, 265.49, -200.66, -24.13);
    break;

  case 7:
    Hv_SetFRect(&world, -260.0, 410.0, -315.0, -30.0);
    break;

  case 8:
    Hv_SetFRect(&world, 35.6, 407.9, -309.2, -10.9);
    break;
  }

  
  dh = (Hv_canvasRect.width - 2*GAP) - View->local->width;
  apr = world.width/world.height;      /* aspect ratio*/


  new_hr_width  = hotrect->width + dh;
  new_hr_height = (short)(((float)new_hr_width)/apr);
  dv = new_hr_height - hotrect->height;

  Hv_ResizeView(View, dh, dv);
  Hv_QuickZoomView(View, world.xmin, world.xmax, world.ymin, world.ymax);

/*  Hv_GetTemporaryWorld(&world, View->hotrect, View->world, View->local); */
}


/* ---- SetEventSource -----*/

static void SetEventSource(Widget	w,
			      XtPointer	client_data,
			      XtPointer	call_data)	

{
  eventsource = -1;

  if (w == evfromfile)
    eventsource = CED_FROMFILE;
  else if (w == evfromet) {
#ifndef NO_ET
      if (!etisok) {
	  
	  init_et_(etsession, ETSTATION, ETQSIZE, ETPRESCALE);

	  connect_et();

	  if (!etisok) {
	      Hv_Warning("Connection to ET failed.");
  	  }

      }



    eventsource = CED_FROMET;
#endif
  }


  else {
    fprintf(stderr, "Error: eventsource = %d\n", eventsource);
    exit(0);
  }
  
  EventButtonSelectability();
}


/*------ PrecountCB --------*/

static void PrecountCB(Widget w) {
    precountEvents = !precountEvents;
}


/* ---- SetEventWhen -----*/

static void SetEventWhen(Widget	w,
			 XtPointer	client_data,
			 XtPointer	call_data)	

{
  eventwhen = -1;

  if (w == evondemand) {
      eventwhen = CED_ONDEMAND;
      StopEventTimer();
  }
  else if (w == evontimer) {
      eventwhen = CED_ONTIMER;
      StartEventTimer();
  }
  else {
      fprintf(stderr, "Error: eventwhen = %d\n", eventwhen);
      exit(0);
  }
  
  EventButtonSelectability();
}


/*---------- EditDetectorColors -----------*/

void EditDetectorColors()
    
{
    static Widget           dialog = NULL;
    Widget                  dummy, rowcol, rc;
    int                     answer;
    Hv_View                 temp;
    
    if (!dialog) {
	Hv_VaCreateDialog(&dialog, Hv_TITLE, " SC, CC, and EC Colors ", NULL);
	rowcol = Hv_DialogColumn(dialog, 0);
	dummy = Hv_StandardLabel(rowcol, "NOTE: These changes are applied to ALL Views!", 0);
	dummy = Hv_SimpleDialogSeparator(rowcol);
	dummy = Hv_StandardLabel(rowcol, " ", 0);

/* shower counters*/

	dummy = Hv_StandardLabel(rowcol, "Shower Counter Colors", -1);
	rc = Hv_DialogTable(rowcol, 3, 2);
	fillcolorlabel0  = Hv_SimpleColorLabel(rc, "inner fill color ",
					       (Hv_FunctionPtr)EditColor);
	framecolorlabel0 = Hv_SimpleColorLabel(rc, "inner frame color ",
					       (Hv_FunctionPtr)EditColor);
	fillcolorlabel1  = Hv_SimpleColorLabel(rc, "outer fill color ",
					       (Hv_FunctionPtr)EditColor);
	framecolorlabel1 = Hv_SimpleColorLabel(rc, "outer frame color ",
					       (Hv_FunctionPtr)EditColor);
	hitcolorlabel   = Hv_SimpleColorLabel(rc, " LAC hit color ",
					      (Hv_FunctionPtr)EditColor);
	

	
	dummy = Hv_SimpleDialogSeparator(rowcol);

/* scints */

	dummy = Hv_StandardLabel(rowcol, "Scintillator Colors", -1);
	rc = Hv_DialogTable(rowcol, 1, 3);
	scfillcolorlabel  = Hv_SimpleColorLabel(rc, "fill color ", EditColor);
	scframecolorlabel = Hv_SimpleColorLabel(rc, "frame color ", EditColor);
	schitcolorlabel   = Hv_SimpleColorLabel(rc, "hit color ", EditColor);

	dummy = Hv_StandardActionButtons(rowcol, 120, Hv_OKBUTTON + Hv_CANCELBUTTON + Hv_APPLYBUTTON);
    }
    
/* the dialog has been created */
    
    framecolor0 = ecframecolor[0];
    fillcolor0  = ecfillcolor[0];
    framecolor1 = ecframecolor[1];
    fillcolor1  = ecfillcolor[1];
    hitcolor    = ec1hitcolor;

    Hv_ChangeLabelColors(fillcolorlabel0,  -1, fillcolor0);
    Hv_ChangeLabelColors(framecolorlabel0, -1, framecolor0);
    Hv_ChangeLabelColors(fillcolorlabel1,  -1, fillcolor1);
    Hv_ChangeLabelColors(framecolorlabel1, -1, framecolor1);
    Hv_ChangeLabelColors(hitcolorlabel,   -1, hitcolor);

    scfillcolor  = scintfillcolor;
    scframecolor = scintframecolor;
    schitcolor   = scinthitcolor;

    Hv_ChangeLabelColors(scfillcolorlabel,  -1, scfillcolor);
    Hv_ChangeLabelColors(scframecolorlabel, -1, scframecolor);
    Hv_ChangeLabelColors(schitcolorlabel,   -1, schitcolor);

    
    while ((answer = Hv_DoDialog(dialog, NULL)) > 0) {

	ecframecolor[0] = framecolor0;
	ecfillcolor[0]  = fillcolor0;
	ecframecolor[1] = framecolor1;
	ecfillcolor[1]  = fillcolor1;
	ec1hitcolor     = hitcolor;

	scintfillcolor  = scfillcolor;
	scintframecolor = scframecolor;
	scinthitcolor   = schitcolor;

/* redraw all views */

	for (temp = Hv_views.first;  temp != NULL;  temp = temp->next) {
	    Hv_SetViewDirty(temp);
	    Hv_DrawViewHotRect(temp); 
	}

	if (answer != Hv_APPLY)
	    break;
    }
    
}


/* ------ EdiColor --- */

static void EditColor(Hv_Widget w)
    
{
  if (w == fillcolorlabel0) 
    Hv_ModifyColor(w, &fillcolor0, "EC Inner Fill Color", False);
  else if (w == framecolorlabel0)
    Hv_ModifyColor(w, &framecolor0, "EC Inner Frame Color", False);
  if (w == fillcolorlabel1) 
    Hv_ModifyColor(w, &fillcolor1, "EC Outer Fill Color", False);
  else if (w == framecolorlabel1)
    Hv_ModifyColor(w, &framecolor1, "EC Outer Frame Color", False);
  else if (w == hitcolorlabel)
    Hv_ModifyColor(w, &hitcolor, "LAC Hit Color", False);


  else if (w == scfillcolorlabel)
    Hv_ModifyColor(w, &scfillcolor, "Scint Fill Color", False);
  else if (w == scframecolorlabel)
    Hv_ModifyColor(w, &scframecolor, "Scint Frame Color", False);
  else if (w == schitcolorlabel)
    Hv_ModifyColor(w, &schitcolor, "Scint Hit Color", False);
  

}


