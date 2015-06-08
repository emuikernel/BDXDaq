/*
---------------------------------------------------------------------------
-
-   	File:     maps.h
-
-	main header file for the maps demo
-
-										
-  Revision history:								
-                     							        
-  Date       Programmer     Comments						
--------------------------------------------------------------------------------
-  08/24/95	DPH		Initial Version					
-------------------------------------------------------------------------------
*/


#ifndef __MAPH__
#define __MAPH__

#define  BUTTONWIDTH       40
#define  BIGBUTTONWIDTH    60
#define  BUTTONHEIGHT      28
#define  BIGBUTTONHEIGHT   20

#define  NUMOVERLAY         10

/* feedback tags */

#define     LATLONG            1

/* View Types */

#define     MAPVIEW  1001

/* overlay data structure */

typedef struct overlay  *Overlay;

typedef struct overlay
{
  float    dellat, dellong;   /*spacing in degrees*/
  int      npts;              /*number of points */
  float   *latitude;
  float   *longitude;
  int     *value;              /* data 0 -- 8 at each point */
} OverlayRec;



/* data structure pointed to by data field in each  View */

typedef struct viewdatarec *ViewData;

typedef struct viewdatarec
{
  unsigned long   simtime;                /* sim time in seconds */
  int             simstep;                /* simstep in seconds */ 
  Boolean         showoverlay;            /* showoverlaydata */

  Hv_Item         MapItem;
  Hv_Item         RotateItem;
  Hv_Item         SimClock;
  Hv_Item         OverlayButton;
  Hv_Item         OverlayItem;
  Hv_Item         ColorButtons[NUMOVERLAY];
  Overlay         data;

} ViewDataRec;


/*------ overlay.c ------*/

extern void     ReadOverlay();
extern Overlay  MallocOverlay();
extern void     DestroyOverlay();
extern void     DrawOverlay();

/*------ init.c ------*/

extern  void          Init();

/*------ menus.c ------*/

extern  void          MakeMenus();
extern  void          CustomizeQuickZoom();
extern  void          DoQuickZoom();
extern  void          InitQuickZoom();

/*------ setup.c ------*/

extern  Hv_View       NewView();
extern  ViewData      GetViewData();

/*------ dlogs.c ------*/

extern void           DoAboutDlog();
extern void           GetNewView();

/*------ logo.c -------*/

extern void           DrawLogo();
extern void           DrawLogoItem();
extern void           ViewCustomize();

/*-------- feedback.c ----------*/

extern void           ViewFeedback();
#endif



