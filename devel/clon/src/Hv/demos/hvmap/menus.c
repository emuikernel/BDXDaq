/*
----------------------------------------------------
-							
-  File:    menus.c					
-							
-  Summary:						
-            menu routines
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

#define NUMQZ  12

static char *qzname[NUMQZ];
static float qzminlat[NUMQZ];
static float qzminlong[NUMQZ];
static float qzmaxlat[NUMQZ];
static float qzmaxlong[NUMQZ];
static int   numqz;

static void NewPlotView();
static void StuffQZ();

/*------------ InitQuickZoom ------*/

void InitQuickZoom()

{
  numqz = 0;
  StuffQZ("United States", 19.03, 51.46, -130.84, -65.05);
  StuffQZ("Europe",        35.14, 60.02, -14.77, 45.9);
}

/*--------- StuffQZ ------------*/

static void StuffQZ(name, minlat, maxlat, minlong, maxlong)

char   *name;
float  minlat, maxlat, minlong, maxlong;

{
  if (numqz >= NUMQZ)
    return;

  Hv_InitCharStr(&(qzname[numqz]), name);
  qzminlat[numqz] =  minlat;
  qzmaxlat[numqz] =  maxlat;
  qzminlong[numqz] = minlong;
  qzmaxlong[numqz] = maxlong;

  numqz++;
}


/*------------- CustomizeQuickZoom ------------*/

void CustomizeQuickZoom(View)

Hv_View       View;

{
  int  i;

  for (i = 0; i < numqz; i++)
    Hv_SetQuickZoomLabel(i, qzname[i]);
  
  for (i = numqz; i < NUMQZ; i++)
    Hv_SetQuickZoomLabel(i, "-- not used --");
}


/*----------- DoQuickZoom --------*/

void DoQuickZoom(w, clientdata)

Hv_Widget     w;
Hv_Pointer    clientdata;

{
  int            menuitem;
  float          xmin, ymin, xmax, ymax;

  if ((menuitem = (int)clientdata) >= numqz)
    return;

  Hv_GetGoodMapLimits(Hv_hotView, qzminlat[menuitem], qzmaxlat[menuitem],
	    qzminlong[menuitem], qzmaxlong[menuitem],
	    &xmin, &xmax, &ymin, &ymax, True);
}


/* ------- MakeMenus ------- */

void MakeMenus()


{
  Hv_Widget     item;
  char          *text;

/* create the menu items for the Help menu */


  text = (char *)Hv_Malloc(strlen(Hv_programName) + 20);
  strcpy(text, "About ");
  strcat(text, Hv_programName);
  strcat(text, "...");

  item = Hv_VaCreateMenuItem(Hv_helpMenu,
			     Hv_LABEL,            text,  
			     Hv_CALLBACK,         DoAboutDlog,
			     NULL);
  Hv_Free(text);

/* add a "new view" item to the action menu */

  item = Hv_VaCreateMenuItem(Hv_actionMenu,
			     Hv_LABEL,            "New View",  
			     Hv_CALLBACK,         GetNewView,
			     NULL);


  item = Hv_VaCreateMenuItem(Hv_actionMenu,
			     Hv_LABEL,            "New Plot Canvas",  
			     Hv_CALLBACK,         NewPlotView,
			     NULL);


}

/*----- NewPlotView -----*/

static void NewPlotView()

{
  Hv_NewXYPlotView(NULL, True);
}
