h17608
s 00000/00000/00000
d R 1.2 02/08/26 22:12:17 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 Hv/demos/hvmap/init.c
e
s 00138/00000/00000
d D 1.1 02/08/26 22:12:16 boiarino 1 0
c date and time created 02/08/26 22:12:16 by boiarino
e
u
U
f e 0
t
T
I 1
/*
----------------------------------------------------
-							
-  File:    init.c				
-							
-  Summary:						
-            Primary Initialization
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

/* ------ local prototypes -------*/

static void	  InitControls();
static void       FinalInit();
static void       WindowTile();
static void       AddLogo();
static void       DrawTile();

/*------ Init() ----*/

void Init()

{

/* set the background color */

  Hv_canvasColor = Hv_lightSeaGreen;

  WindowTile();
  MakeMenus();
  InitControls();
  AddLogo();
  InitQuickZoom();
  FinalInit();
}

/*-------- WindowTile -----------*/

static void WindowTile()

/* this sets the size of the rectangle that is tiled
   on the background */

{
  Hv_tileX = 200;
  Hv_tileY = 80;
  Hv_TileDraw = DrawTile;
}

/*------- DrawTile --------------*/

static void DrawTile()

{
  Hv_Rect    rect;

  if ((Hv_mainWindow == 0) || (Hv_display == NULL))
    return;

  Hv_SetRect(&rect, 0, 0, (short)Hv_tileX, (short)Hv_tileY);
  Hv_FillRect(&rect, Hv_canvasColor);

  DrawLogo(Hv_tileX, Hv_tileY);
}



/*----- FinalInit -------*/

static void FinalInit()

{
  Hv_View  view;

  Hv_AddPath("./mapmaker");  /*add to search path */
  Hv_InitMaps("hv.maps");       /* initialize maps */

/* play some color game for the highlighting */

  Hv_AdjustAdjacentColors(Hv_lightSeaGreen, 6000);
  Hv_AdjustAdjacentColors(Hv_skyBlue, -6000);
  Hv_CopyColor(Hv_red+1, Hv_red);

/* initial Views */

  view = NewView(MAPVIEW);
}

/* ----- InitControls -----*/

static void  InitControls()

/* Initialize the variables that define the state and determine flow */

{ 

/* set specialized user functions for various actions */

  Hv_CustomizeQuickZoom = CustomizeQuickZoom;   /* to modify labels */
  Hv_QuickZoom = DoQuickZoom;                   /* to handle selection */
}

/*--------- AddLogo ---------*/

static void AddLogo()

{
  Hv_Item     Item, verstr;
  Hv_Rect     *area;

  verstr = Hv_GetItemFromTag(Hv_welcomeView->items, Hv_VERSIONSTRINGTAG);
  area = verstr->area;

  Item = Hv_VaCreateItem(Hv_welcomeView,
			 Hv_TYPE,              Hv_USERITEM,
			 Hv_USERDRAW,          DrawLogoItem,
			 Hv_DOMAIN,            Hv_OUTSIDEHOTRECT,
			 Hv_LEFT,              area->right + 7,
			 Hv_TOP,               area->top - 7,
			 Hv_HEIGHT,            65,
			 Hv_WIDTH,             150,
			 NULL);

  Hv_AutosizeView(Hv_welcomeView);
}

E 1
