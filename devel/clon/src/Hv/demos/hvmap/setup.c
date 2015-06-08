/*
----------------------------------------------------
-							
-  File:    setup.c	
-							
-  Summary:						
-           sets up views
-					       
-                     				
-  Revision history:				
-                     				
-  Date       Programmer     Comments		
------------------------------------------------------
-  08/24/95	DPH		Initial Version	
-                                              	
------------------------------------------------------
*/

#include "Hv.h"
#include "Hv_maps.h"
#include "maps.h"


/*------ local prototypes ---------*/

static void         ColorButtonCallback();
static void         ViewDraw();
static void         OffScreenViewDraw();
static void         SimClockStr();
static void         TwoDigitStr();
static int          ModSimTime();
static void         ViewSetup();
static ViewData     InitViewData();
static Hv_Item      AddFeedback();
static Hv_Item      AddOptionButtons();
static void         ProjectionChoiceCB();
static void         SimulationCB();
static void         AddColorButtons();
static Hv_Item      StandardColorButton();

/*------- NewView ----------*/

Hv_View NewView(tag)

short  tag;

{
  Hv_View   View;

  static short     viewcnt = 0;
  short            left, top, width, height;
  float            xmin, xmax, ymin, ymax; 
  float            minw, maxw, minh, maxh;
  int              popupcontrol;
  Hv_FunctionPtr   setup;
  char             tchr[60];


  left = ((viewcnt % 4) + 1)*10;

  if (Hv_root_width > 1025) {
    if (viewcnt < 4) 
      top = Hv_welcomeView->containeritem->area->bottom - 40 + viewcnt*12;
    else
      top = Hv_welcomeView->containeritem->area->top + 4 + (viewcnt % 4)*12;
  }
  else
    top = Hv_welcomeView->containeritem->area->top + 6 + (viewcnt % 4)*12;
  

  viewcnt++;

  xmin = -Hv_PI;
  xmax =  Hv_PI;
  ymin = -2.44;
  ymax =  2.44;

  if (Hv_root_width > 1025)
    width  =  760;
  else
    width = 700;

  minw =    0.0075;
  maxw =    Hv_TWOPI;
  minh =   0.005;
  maxh =   3.94625;
  popupcontrol = Hv_STANDARDPOPUPCONTROL;
  setup = ViewSetup;
  
  sprintf(tchr, "hvmap %d", viewcnt);
  height =  (short)((width*(ymax-ymin))/(xmax-xmin));

/* now actually create the view */

  Hv_VaCreateView(&View,
		  Hv_TAG,            tag,
		  Hv_DRAWCONTROL,    Hv_STANDARDDRAWCONTROL+Hv_SAVEBACKGROUND,
		  Hv_POPUPCONTROL,   popupcontrol,
		  Hv_TITLE,          tchr,
		  Hv_INITIALIZE,     setup,
		  Hv_CUSTOMIZE,      ViewCustomize, 
		  Hv_USERDRAW,       ViewDraw,
		  Hv_OFFSCREENUSERDRAW, OffScreenViewDraw,
		  Hv_FEEDBACK,       ViewFeedback,
		  Hv_LEFT,           left,
		  Hv_TOP,            top,
		  Hv_XMIN,           xmin,
		  Hv_XMAX,           xmax,
		  Hv_YMIN,           ymin,
		  Hv_YMAX,           ymax,
		  Hv_HOTRECTWIDTH,   width,
		  Hv_HOTRECTHEIGHT,  height,
		  Hv_MINZOOMWIDTH,   minw,
		  Hv_MAXZOOMWIDTH,   maxw,
		  Hv_MINZOOMHEIGHT,  minh,
		  Hv_MAXZOOMHEIGHT,  maxh,
		  Hv_SIMPROC,        (void *)SimulationCB,
		  Hv_SIMINTERVAL,    1000,
		  Hv_HOTRECTVMARGIN, 7,
		  Hv_HOTRECTCOLOR,   Hv_blue-2,
		  Hv_MOTIONLESSFB,   True,
		  Hv_MOTIONLESSFBINTERVAL, 500,
		  NULL);
  
  return View;
}

/*-------- ViewSetup ----------*/

static void ViewSetup(View)

Hv_View          View;

/* For Defended, Operating and Scenario (NOT Footprint) */

{
  ViewData        viewdata;
  Hv_Item         Sep1, Item, Camera, RightMost, Box, Box2, Choice;
  Hv_Rect        *hr;
  Hv_ViewMapData  mdata;

/* create the viewdata field */

  viewdata = InitViewData(View);
  mdata = Hv_GetViewMapData(View);
/*  Hv_ChangeMapProjection(View, Hv_ORTHOGRAPHIC);*/
  hr = View->hotrect;

/* the standard drawing tools */

  Hv_StandardTools(View,
		   hr->right+8,
		   hr->top-3,
		   Hv_HORIZONTAL,
		   Hv_SHIFTXONRESIZE,
		   Hv_ALLTOOLS,
		   &Camera,
		   &RightMost);

/* a box for some controls */

  Box = Hv_VaCreateItem(View,
			Hv_FILLCOLOR,          Hv_powderBlue,
			Hv_TYPE,               Hv_BOXITEM,
			Hv_LEFT,               Camera->area->left,
			Hv_TOP,                Camera->area->bottom,
			NULL);

/* some option buttons */

  Item = AddOptionButtons(View,
			  Box,
			  Camera->area->left,
			  Camera->area->bottom);

  Box2 = Hv_VaCreateItem(View,
			 Hv_FILLCOLOR,          Hv_cadetBlue,
			 Hv_TYPE,               Hv_BOXITEM,
			 Hv_LEFT,               Box->area->left,
			 Hv_TOP,                Box->area->bottom,
			 Hv_HEIGHT,             200,
			 NULL);

  Item = Hv_VaCreateItem(View,
			 Hv_PARENT,             Box2,
			 Hv_LEFT,               Box2->area->left + 12,
			 Hv_TOP,                Box2->area->top + 8,
			 Hv_TYPE,               Hv_TEXTITEM,
			 Hv_FONT,               Hv_fixed2,
			 Hv_TEXT,
			 "In orthographic projec-\\ntions, use \"shift-click\"\\nto rotate the globe.",
			 NULL);


  Sep1 = Hv_VaCreateItem(View,
			 Hv_PARENT,             Box2,
			 Hv_TYPE,               Hv_SEPARATORITEM,
			 Hv_PLACEMENTGAP,       2,
			 Hv_RELATIVEPLACEMENT,  Hv_POSITIONBELOW,
			 NULL);


  AddColorButtons(Sep1->area->bottom+3, Box2);


/* add the read overlay button */

  viewdata->OverlayButton  = Hv_VaCreateItem(View,
					     Hv_PARENT,            Box2,
					     Hv_TYPE,              Hv_BUTTONITEM,
					     Hv_LEFT,              Box2->area->left + 65,
					     Hv_TOP,               Sep1->area->bottom + 25,
					     Hv_SINGLECLICK,       ReadOverlay,
					     Hv_LABEL,             "\\cRead\\nOverlay",
					     Hv_FONT,              Hv_fixed2,
					     Hv_WIDTH,             65,
					     Hv_HEIGHT,            40,
					     Hv_HOTCOLOR,          Hv_forestGreen,
					     Hv_FILLCOLOR,         Hv_lightSeaGreen,
					     Hv_TEXTCOLOR,         Hv_white,
					     NULL);


  
/* some choice buttons */

  Choice = Hv_VaCreateItem(View,
			   Hv_TYPE,                Hv_CHOICESETITEM,
			   Hv_RELATIVEPLACEMENT,   Hv_POSITIONBELOW,
			   Hv_PLACEMENTITEM,       Box2,
			   Hv_PLACEMENTGAP,        1,
			   Hv_NOWON,               mdata->projection,
			   Hv_FILLCOLOR,           Hv_powderBlue,
			   Hv_ARMCOLOR,            Hv_navyBlue,
			   Hv_COLOR,               Hv_gray10,
			   Hv_FONT,                Hv_fixed2,
			   Hv_OPTION,             "Mercator",
			   Hv_OPTION,             "Orthographic",
			   Hv_SINGLECLICK,         ProjectionChoiceCB,
			   Hv_TITLE,              "Map Projection",
			   NULL);

/* now the feedback */

  Item = AddFeedback(View, Choice);
  Item->area->right = RightMost->area->right;
  Hv_FixRectWH(Item->area);

  Box->area->right = Item->area->right;
  Hv_FixRectWH(Box->area);

  Box2->area->right = Item->area->right;
  Hv_FixRectWH(Box2->area);
  Hv_SeparatorResizeToParent(Sep1);

  Choice->area->right = Item->area->right;
  Hv_FixRectWH(Choice->area);

/* Simulation Clock */

  Item = Hv_VaCreateItem(View,
			 Hv_TYPE,               Hv_BOXITEM,
			 Hv_FILLCOLOR,          Hv_darkBlue,
			 Hv_RELATIVEPLACEMENT,  Hv_POSITIONBELOW,
			 Hv_PLACEMENTGAP,       0,
			 Hv_HEIGHT,             30,
			 Hv_WIDTH,              Item->area->width,
			 Hv_TEXT,               " ",
			 Hv_FONT,               Hv_courierBold17,
			 Hv_TEXTCOLOR,          Hv_aliceBlue,
			 NULL);
  
  viewdata->SimClock = Item;


/* the MAP item */

  Item = Hv_VaCreateItem(View,
			 Hv_TYPE,          Hv_MAPITEM,
			 Hv_DRAWCONTROL,   Hv_INBACKGROUND + Hv_NOPOINTERCHECK + Hv_ZOOMABLE + Hv_ALWAYSREDRAW,
			 NULL);
  

  Hv_CopyRect(Item->area, View->hotrect);
  viewdata->MapItem = Item;
  Hv_SetViewBrickWallItem(viewdata->MapItem);

/* some map colors */


  mdata->waterborder = Hv_black;
  mdata->landfill = Hv_aquaMarine;
  mdata->landborder = Hv_black;
  mdata->gridcolor = Hv_gray4;
  mdata->gridlabelcolor = Hv_yellow;
}

/*------ AddColorButtons --------*/

static void   AddColorButtons(top, parent)

short      top;
Hv_Item   parent;

{
  ViewData   viewdata;

  Hv_View    View = parent->view;

  viewdata = GetViewData(View);

  viewdata->ColorButtons[0] = Hv_VaCreateItem(View,
					      Hv_PARENT,             parent,
					      Hv_SINGLECLICK,        ColorButtonCallback,
					      Hv_DOUBLECLICK,        ColorButtonCallback,
					      Hv_TYPE,               Hv_COLORBUTTONITEM,
					      Hv_TEXT,               "0",
					      Hv_FONT,               Hv_fixed2,
					      Hv_FILLCOLOR,          Hv_gray12,
					      Hv_LEFT,               parent->area->left + 7,
					      Hv_TOP,                top+6,
					      NULL);

  viewdata->ColorButtons[1] = StandardColorButton(View, parent, "1", Hv_yellow);
  viewdata->ColorButtons[2] = StandardColorButton(View, parent, "2", Hv_red);
  viewdata->ColorButtons[3] = StandardColorButton(View, parent, "3", Hv_wheat);
  viewdata->ColorButtons[4] = StandardColorButton(View, parent, "4", Hv_lightSeaGreen);

  viewdata->ColorButtons[5] = Hv_VaCreateItem(View,
					      Hv_PARENT,             parent,
					      Hv_SINGLECLICK,        ColorButtonCallback,
					      Hv_DOUBLECLICK,        ColorButtonCallback,
					      Hv_TYPE,               Hv_COLORBUTTONITEM,
					      Hv_TEXT,               "5",
					      Hv_FONT,               Hv_fixed2,
					      Hv_FILLCOLOR,          Hv_dodgerBlue,
					      Hv_LEFT,               parent->area->left + 155,
					      Hv_TOP,                top+6,
					      NULL);

  viewdata->ColorButtons[6] = StandardColorButton(View, parent, "6", Hv_mistyRose);
  viewdata->ColorButtons[7] = StandardColorButton(View, parent, "7", Hv_brown);
  viewdata->ColorButtons[8] = StandardColorButton(View, parent, "8", Hv_coral);
  viewdata->ColorButtons[9] = StandardColorButton(View, parent, "9", Hv_gray4);

  parent->area->bottom = viewdata->ColorButtons[4]->area->bottom + 8;
  Hv_FixRectWH(parent->area);
}


/*--------- StandardColorButton --------*/

static Hv_Item StandardColorButton(View, parent, label, color)

Hv_View      View;
Hv_Item      parent;
char         *label;
short        color;

{
  Hv_Item       Item;

  Item = Hv_VaCreateItem(View,
			 Hv_PARENT,             parent,
                         Hv_SINGLECLICK,        ColorButtonCallback,
                         Hv_DOUBLECLICK,        ColorButtonCallback,
			 Hv_TYPE,               Hv_COLORBUTTONITEM,
			 Hv_TEXT,               label,
			 Hv_FONT,               Hv_fixed2,
			 Hv_FILLCOLOR,          color,
			 Hv_RELATIVEPLACEMENT,  Hv_POSITIONBELOW,
			 Hv_PLACEMENTGAP,       4,
                         Hv_ALLOWNOCOLOR,       True,
			 NULL);

  return Item;
}

/* ------- AddOptionButtons -----*/

static Hv_Item AddOptionButtons(View, parent, l, t)

Hv_View     View;
Hv_Item     parent;
short       l, t;

{
  Hv_ViewMapData  mdata;
  Hv_Item         Item;
  ViewData        viewdata;

  mdata = Hv_GetViewMapData(View);
  viewdata = GetViewData(View);

  Item = Hv_StandardOptionButton(View, parent, NULL,
				 &(mdata->drawgrid),
				 "Lat-Long Grid",
				 Hv_DIRTYWHENCHANGED);

  Item = Hv_StandardOptionButton(View, parent, Item,
				 &(mdata->rotateearth),
				 "Earth Rotation",
				 0);
  viewdata->RotateItem = Item;

  Item = Hv_StandardOptionButton(View, parent, Item,
				 &(viewdata->showoverlay),
				 "Show Overlay",
				 Hv_DIRTYWHENCHANGED);
  return Item;
}

/*------ AddFeedback ----------*/

static Hv_Item AddFeedback(View, pitem)

Hv_View  View;
Hv_Item  pitem;

{
  Hv_Item        Item;

  Item = Hv_VaCreateItem(View,
			 Hv_TYPE,                Hv_FEEDBACKITEM,
			 Hv_RELATIVEPLACEMENT,   Hv_POSITIONBELOW,
                         Hv_PLACEMENTITEM,       pitem,
                         Hv_FEEDBACKDATACOLS,    18,
			 Hv_PLACEMENTGAP,        0,
			 Hv_FILLCOLOR,           Hv_powderBlue,
			 Hv_FEEDBACKENTRY,       LATLONG, Hv_fixed2, Hv_black, " Lat-Long: ",
			 Hv_FEEDBACKMARGIN,      8,
			 NULL);

/* readjust parent size */

  pitem->area->right = Item->area->right;
  Hv_FixRectWH(pitem->area);

  return Item;
}


/* ----- InitViewData --------*/

static ViewData  InitViewData(View)

Hv_View   View;

/**************************************
  Initialize the view data record carried
  by every view.
***************************************/


{
  ViewData  viewdata;
  int       i;

  Hv_MallocViewMapData(View);
  viewdata = (ViewData)Hv_Malloc(sizeof(ViewDataRec));
  viewdata->simtime = 0;
  viewdata->simstep = 720;   /* seconds */
  viewdata->MapItem = NULL;
  viewdata->RotateItem = NULL;
  viewdata->SimClock = NULL;

  viewdata->OverlayButton = NULL;
  viewdata->OverlayItem = NULL;
  viewdata->data = NULL;
  for (i = 0; i < NUMOVERLAY; i++)
    viewdata->ColorButtons[i] = NULL;

/* initialize the overlay colors */

  viewdata->showoverlay = True;

  View->data = (void *)viewdata;
  return viewdata;
}


/*------- GetViewData --------*/

ViewData GetViewData(View)

Hv_View     View;

{
  return  (ViewData)(View->data);
}


/* ------- ProjectionChoiceCB -------*/

static void ProjectionChoiceCB(hvevent)

Hv_Event    hvevent;

{
  Hv_Item          Item = hvevent->item;
  Hv_View          View = hvevent->view;

  Hv_ChangeMapProjection(View, Hv_HandleChoice(Item));
}

/*------- SimulationCB --------*/

static void SimulationCB(View)

Hv_View   View;

{
  ViewData         viewdata = GetViewData(View);
  char             text[60];
  Hv_ViewMapData   mdata = Hv_GetViewMapData(View);
  float            fract;
  Hv_Region        hrrgn;
  Boolean          earthrotate;

/* simtime is in SECONDS */

  viewdata->simtime += viewdata->simstep;

  if (Hv_ViewIsVisible(View)) {
    SimClockStr(View, text);
    Hv_UpdateBoxItem(viewdata->SimClock, text);
  }

/* earth rotation ? */

  if (mdata == NULL)
    return;

  hrrgn = Hv_ClipHotRect(View, 1);

  earthrotate = Hv_GetOptionButton(viewdata->RotateItem);
  
  if (earthrotate) {
    fract = ((float)viewdata->simstep)/86400.0;
    mdata->lambda0 -= Hv_TWOPI*fract;
    
    if (mdata->lambda0 < -Hv_PI)
      mdata->lambda0 += Hv_TWOPI;
    
    mdata->coslam0 = cos(mdata->lambda0);
    mdata->sinlam0 = sin(mdata->lambda0);
    
    Hv_DestroyLocalMaps(View);
    Hv_SaveViewBackground(View, hrrgn);
    Hv_RestoreViewBackground(View);
    Hv_DrawNonBackgroundItems(View, hrrgn);
  }

  Hv_DestroyRegion(hrrgn);
  
}

/*------- SetSimClock ------*/

static void SimClockStr(View, text)

Hv_View     View;
char        *text;

{
  int day;
  int hr;
  int min;
  int sec;
  ViewData    viewdata;
  unsigned long stime;

  char        daystr[10], hrstr[5], minstr[5], secstr[5];

  viewdata = GetViewData(View);

  stime = viewdata->simtime;

  day = ModSimTime(&stime, 86400);
  hr  = ModSimTime(&stime, 3600);
  min = ModSimTime(&stime, 60);
  sec = (int)stime;

  TwoDigitStr(day, daystr);
  TwoDigitStr(hr,  hrstr);
  TwoDigitStr(min, minstr);
  TwoDigitStr(sec, secstr);

  sprintf(text, "Day %s   %s:%s:%s", daystr, hrstr, minstr, secstr);
}

/*------ TwoDigitStr ----*/

static void TwoDigitStr(val, st)

int   val;
char  *st;

{
  if (val < 10) {
    st[0] = '0';
    st[1] = '0' + val;
    st[2] = '\0';
  }
  else
    sprintf(st, "%-2d", val);

}

/*----- ModSimTime  -----*/

static int ModSimTime(stime, iv)

unsigned long    *stime;
int              iv;

{
  int   val;

  val = (int)((*stime)/iv);

  if (val > 0)
    (*stime) -= (val*iv);

  return val;
}


/*-------- ViewDraw -------*/

static void ViewDraw(View, region)

Hv_View    View;
Hv_Region  region;

{
  Hv_ViewMapData     mdata;

  if (Hv_drawingOffScreen)
    return;
  
  mdata = Hv_GetViewMapData(View);
  
  if (mdata->drawgrid == True)
    Hv_DrawMapGrid(View, region);
}


/*-------- OffScreenViewDraw -------*/

static void OffScreenViewDraw(View, region)

Hv_View    View;
Hv_Region  region;

{
  Hv_ViewMapData     mdata;
  ViewData           viewdata;
  
  viewdata = GetViewData(View);  
  mdata = Hv_GetViewMapData(View);

  if (viewdata->showoverlay)
    DrawOverlay(View, region);

  if (mdata->drawgrid == True)
    Hv_DrawMapGrid(View, region);
}


/*------- ColorButtonCallback -----*/

static void    ColorButtonCallback(hvevent)

Hv_Event       hvevent;

{
  Hv_View    View = hvevent->view;
  Hv_Item    Item = hvevent->item;
  ViewData   viewdata;
  short      oldcolor, newcolor;

  viewdata = GetViewData(View);
  oldcolor = Hv_GetColorButtonColor(Item);
  Hv_EditColorButtonItem(hvevent);
  newcolor = Hv_GetColorButtonColor(Item);

  if (oldcolor == newcolor)
    return;

  if ((viewdata->showoverlay) && (viewdata->data != NULL)) {
    Hv_SetViewDirty(View);
    Hv_DrawViewHotRect(View);
  }
}


