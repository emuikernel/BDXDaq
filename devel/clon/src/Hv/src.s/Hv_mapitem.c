/*	
-----------------------------------------------------------------------
File:     Hv_mapitem.c
==================================================================================

The Hv library was developed at CEBAF under contract to the
Department of Energy and is the property of the U.S. Government.

It may be used to develop commercial software, but the Hv
source code and/or compiled modules/libraries may not be sold.

Questions or comments should be directed to heddle@cebaf.gov
==================================================================================
*/

#include "Hv.h"
#include "Hv_maps.h"


#define  FRAMEMAP           0
#define  FILLMAP            1
#define  FRAMEANDFILLMAP    2
#define  POINTSONLY         3

#define  LIMIT_RECT_DEL 2000
#define  BADPOINTSTEP 7
#define  BADPOINTSKIP 6
#define  MINPOINTDRAW 7

#define  NUMGLOBEPOINTS 360

/* ------ local prototypes ------*/


static void Hv_GlobeClick(Hv_Event  hvevent);

static void Hv_DrawGlobeItem(Hv_Item   Item,
			     Hv_Region  region);

static Hv_Point *Hv_GetGlobePolygon(Hv_View  View,
				    float   xc,
				    float   yc,
				    float   rad,
				    int    n);

static void Hv_DrawTheMaps(Hv_Item   Item,
			   int     mode,
			   Hv_Region  region);

static void Hv_PSClipGlobePolygon(Hv_View  View);

static void Hv_MapAfterOffset(Hv_Item  Item,
			      short   dh,
			      short   dv);

static void Hv_FixMapRegion(Hv_Item     Item);

static void Hv_OffsetMapList(Hv_LocalMap   maps,
			     short      dh,
			     short      dv);



static void Hv_AddMapPoly(Hv_View   View,
			  int     mtype,
			  Hv_Point   **xp,
			  int     npoly,
			  Hv_LocalMap *localmap);

static Hv_LocalMap Hv_AddLocalMapToList(Hv_LocalMap  *map);

static Hv_LocalMap Hv_NewLocalMap(void);

static Hv_Point *Hv_OrthographicMapPointsToPoly(Hv_View   View,
						Hv_Map   map,
						int    *npoly,
						Boolean   ClipIt);

static int Hv_MakeAGoodMap(Hv_View  View,
			   Hv_Map   map,
			   int    ngoodlim,
			   float   **xx,
			   float   **yy);
     
static float Hv_NearestCompassAngle(float  theta);
     
     
static float Hv_ConnectingCompassAngle(float theta1,
				       float theta2);
     
static Hv_Point *Hv_MercatorMapPointsToPoly(Hv_View   View,
					    Hv_Map   map,
					    int    *npoly,
					    Boolean   ClipIt);

static Hv_Point *Hv_MercatorMapPointsToPoly2(Hv_View   View,
					     Hv_Map   map,
					     int    *npoly,
					     Boolean   ClipIt);

static void Hv_DrawOneMap(Hv_LocalMap    map,
			  int        mode,
			  short       fillcolor,
			  short       framecolor);

static void Hv_DestroyMapList(Hv_LocalMap  *maps );

static void Hv_OrthographicGrid(Hv_View  View,
				Hv_Region region);

static void Hv_MercatorGrid(Hv_View  View,
			    Hv_Region region);

static void ReadyLabReg(Hv_Rect  *rect1,
			Hv_Region *reg1,
			short   x,
			short   y,
			short   w,
			short   h);

static Hv_String *GetValStr(float  val,
			    char  *c,
			    short  *w,
			    short  *h,
			    short  color);

Hv_Map                Hv_allMaps = NULL;

extern Boolean        Hv_mapPointIsGood;

Boolean               Hv_alternativeMap = False;
Boolean               Hv_mapPrint = False;

/*------ Hv_GetViewMapData ------*/

Hv_ViewMapData     Hv_GetViewMapData(Hv_View View)

{
  if (View == NULL)
    return NULL;
  else
    return (Hv_ViewMapData)(View->mapdata);
}

/*------ Hv_MallocViewMapData ------*/

void    Hv_MallocViewMapData(Hv_View View)

{
  Hv_ViewMapData     mdata;

  if (View == NULL)
    return;

  if (View->mapdata != NULL) {
    Hv_Println("Warning: attempt to malloc an additional Hv_ViewMapDataRec.");
    return;
  }

  mdata = (Hv_ViewMapData)Hv_Malloc(sizeof(Hv_ViewMapDataRec));

  mdata->lambda0 = (float)(-Hv_DEGTORAD*12.0);
  mdata->deflambda0 = mdata->lambda0;
  mdata->phi1 = (float)(Hv_DEGTORAD*30.0);
  mdata->radius = Hv_DEFAULTRADIUS;

  mdata->landmaps = NULL;
  mdata->watermaps = NULL;
  mdata->bordermaps = NULL;
  mdata->unknownmaps = NULL;
  mdata->specialmaps = NULL;
  mdata->drawpoliticalborders = True;

/* set default map colors */

  mdata->gridcolor = Hv_gray6;
  mdata->gridlabelcolor = Hv_white;
  mdata->waterfill = View->hotrectborderitem->color;
  mdata->waterborder = Hv_dodgerBlue;
  mdata->landfill = Hv_seaGreen;
  mdata->landborder = Hv_tan;
  mdata->projection = Hv_MERCATOR;
  mdata->outerspace = Hv_black;

/* init lat/long grid control */

  mdata->latstep = 15.0;
  mdata->longstep = 15.0;
  mdata->drawgrid = False;
  mdata->rotateearth = False;

  mdata->specialflag = False;
  mdata->specialdraw = NULL;

  mdata->anotherflag = False;
  mdata->anotherdraw = NULL;

  mdata->extraflag = False;
  mdata->extradraw = NULL;

/* now allocate the globe item */

  mdata->GlobeItem = Hv_VaCreateItem(View,
				     Hv_TYPE,        Hv_GLOBEITEM,
				     Hv_DOMAIN,      Hv_INSIDEHOTRECT,
				     Hv_DRAWCONTROL, Hv_INBACKGROUND + Hv_ZOOMABLE + Hv_ALWAYSREDRAW,
				     Hv_SINGLECLICK, Hv_GlobeClick,
				     Hv_USERDRAW,    Hv_DrawGlobeItem,
				     NULL);
    

  View->mapdata = (void *)mdata;
  Hv_FixMapParameters(View);
}

/*------ Hv_DestroyViewMapData ------*/

void    Hv_DestroyViewMapData(Hv_View View)

{
  Hv_ViewMapData    mdata;

  if ((mdata = Hv_GetViewMapData(View)) == NULL)
    return;
}


/*------ Hv_PointIsOnLand -----*/

Boolean Hv_PointIsOnLand(Hv_View   View,
			 Hv_Point  pp)

{
  Hv_Region    landregion;
  Boolean      onland;

  landregion = Hv_LandMapRegion(View);
  onland = Hv_PointInRegion(pp, landregion);
  Hv_DestroyRegion(landregion);
  return  onland;
}


/* ----- Hv_LandMapRegion ----*/

Hv_Region  Hv_LandMapRegion(Hv_View View)

/* MALLOCS and returns a region corresponding to all but the
   smallest land maps */

{
  Hv_Region        hotrectreg;
  Hv_Region        totreg;
  Hv_Region        polyreg = NULL;
  Hv_Region        tpreg = NULL;
  Hv_Rect          hrect;
  Hv_LocalMap      temp;
  Hv_ViewMapData   mdata;

  if ((mdata = Hv_GetViewMapData(View)) == NULL)
    return NULL;

  Hv_CopyRect(&hrect, View->hotrect);
  Hv_ShrinkRect(&hrect, 1, 1);

  hotrectreg = Hv_RectRegion(&hrect);

  for (temp = mdata->landmaps; temp != NULL; temp = temp->next) 

    if (temp->numpoints >= MINPOINTDRAW) {
      polyreg = Hv_CreateRegionFromPolygon(temp->mappoly, (short)(temp->numpoints));
      Hv_AddRegionToRegion(&tpreg, polyreg);
      Hv_DestroyRegion(polyreg);
   }  /* end if numpoints > */


/* now the unknown maps */

  for (temp = mdata->unknownmaps; temp!=NULL; temp=temp->next)
    if (temp->numpoints >= MINPOINTDRAW) {
      polyreg = Hv_CreateRegionFromPolygon(temp->mappoly, (short)(temp->numpoints));
      Hv_AddRegionToRegion(&tpreg, polyreg);
      Hv_DestroyRegion(polyreg);
   }  /* end if numpoints > */

  totreg = Hv_IntersectRegion(hotrectreg, tpreg);

  Hv_DestroyRegion(hotrectreg);
  Hv_DestroyRegion(tpreg);
  return  totreg;
}

/* ------- Hv_DrawMapItem -------*/

void Hv_DrawMapItem(Hv_Item    Item,
		    Hv_Region     region)

{
  Hv_ViewMapData   mdata;
  Hv_View          View = Item->view;

  if ((mdata = Hv_GetViewMapData(View)) == NULL)
    return;

  Hv_Flush(); 

/* see if we need to build the local maps. We need only test
   for land maps  */

  if (mdata->landmaps == NULL) 
    Hv_BuildLocalMaps(View);

/* special drawing (e.g. elevation) ? */

  if ((mdata->specialflag) && (mdata->specialdraw != NULL)) {
    mdata->specialdraw(Item, region);
    Hv_DrawTheMaps(Item, FRAMEMAP, region);
   }
  else if ((mdata->anotherflag) && (mdata->anotherdraw != NULL)) {
    mdata->anotherdraw(Item, region);
    Hv_DrawTheMaps(Item, FRAMEMAP, region);
  }
  else {
    Hv_DrawTheMaps(Item, FRAMEANDFILLMAP, region);
  }

  if ((mdata->extraflag) && (mdata->extradraw != NULL)) {
    mdata->extradraw(Item, region);
    Hv_DrawTheMaps(Item, FRAMEMAP, region);
  }
  
/****************************************
  Note: it is tempting to put Hv_DrawMapGrid
  here, but since it might call Hv_DrawView
  and this would cause problems if we are
  drawing off screen, we cannot.
*****************************************/

}

/*-------- Hv_DrawOneSpecialMap -------*/

void Hv_DrawOneSpecialMap(Hv_View   View,
			  Hv_Map   map,
			  Hv_Region  region)

{
  int                 i;
  Hv_Point           *xp;
  Hv_SpecialMapData   smap;  

/* set user1 to 0 to avoid drawing this special map */

  if ((map->numpoints == 0) ||
      ((smap = Hv_GetSpecialMapData(map)) == NULL) ||
      ((smap->drawit == 0)))
    return;

  xp = Hv_NewPoints(map->numpoints);
  for (i = 0; i < (int)(map->numpoints); i++)
    Hv_LatLongToLocal(View, &(xp[i].x), &(xp[i].y),
		      map->latitude[i], map->longitude[i]);

 Hv_DrawSymbolsOnPoints(xp,
			map->numpoints,
			smap->width,
			smap->color,
			smap->symbol);

  Hv_PolygonEnclosingRect(smap->enclosure, xp, map->numpoints);
  Hv_Free(xp);
}


/*-------- Hv_InitializeMap ------*/

void Hv_InitializeMap(Hv_Item        Item,
		      Hv_AttributeArray   attributes)

/* called when a map item is created. Also creates the view's
   map data.  */

{
  Hv_View             View = Item->view;
  Hv_ViewMapData      mdata;

  mdata = Hv_GetViewMapData(View);
  if (mdata == NULL)
    Hv_MallocViewMapData(View);

  Item->standarddraw = Hv_DrawMapItem;

  if (attributes[Hv_DRAWCONTROL].i == 0)
    Item->drawcontrol = Hv_NOPOINTERCHECK + Hv_ZOOMABLE + Hv_ALWAYSREDRAW;
  if (attributes[Hv_AFTEROFFSET].v == NULL)
    Item->afteroffset = (Hv_FunctionPtr)Hv_MapAfterOffset;
  if (attributes[Hv_FIXREGION].v == NULL)
    Item->fixregion = Hv_FixMapRegion;


  Item->worldarea = Hv_NewFRect();
  Hv_SetFRect(Item->worldarea, (float)(-3.2), (float)(3.2), (float)(-1.6), (float)(1.6));
  Hv_WorldRectToLocalRect(Item->view, Item->area, Item->worldarea);
}

/*--------- Hv_ChangeMapProjection --------*/

void Hv_ChangeMapProjection(Hv_View  View,
			    int    newprojection)

{
  Hv_ViewMapData     mdata;
  Hv_Item            globe;

  mdata = Hv_GetViewMapData(View);

  if ((mdata == NULL) || (mdata->projection == newprojection))
    return;

/* undo zoom is no possible */

  Hv_Free(View->lastworld);
  View->lastworld = NULL;
  Hv_SetSensitivity(Hv_popupItems[Hv_FirstSetBit(Hv_UNDOZOOM)], False);

  mdata->projection = newprojection;
  globe = mdata->GlobeItem;

  switch (mdata->projection) {
  case Hv_MERCATOR:
    View->minzoomw = (float)Hv_MERCATORMINW;
    View->maxzoomw = (float)Hv_MERCATORMAXW;
    View->minzoomh = (float)Hv_MERCATORMINH;
    View->maxzoomh = (float)Hv_MERCATORMAXH;
    View->hotrectborderitem->color = mdata->waterfill;
    Hv_SetItemDrawControlBit(globe, Hv_NOPOINTERCHECK);
    break;

  case Hv_ORTHOGRAPHIC:
    View->minzoomw = (float)Hv_ORTHOGRAPHICMINW;
    View->maxzoomw = (float)Hv_ORTHOGRAPHICMAXW;
    View->minzoomh = (float)Hv_ORTHOGRAPHICMINH;
    View->maxzoomh = (float)Hv_ORTHOGRAPHICMAXH;

    View->hotrectborderitem->color = mdata->outerspace;
    Hv_ClearItemDrawControlBit(globe, Hv_NOPOINTERCHECK);
    break;
  }

  Hv_DestroyLocalMaps(View);
  Hv_SetViewDirty(View);
  Hv_DrawViewHotRect(View);
}


/*------ Hv_DestroyLocalMaps ------*/

void Hv_DestroyLocalMaps(Hv_View View)

/* destroys the local maps for a given View --  which will force
   them to be recalculated from the global maps. This is needed,
   for example, after a zoom */

/* NOTE: SPECIAL MAPS ARE NOT DESTROYED ! */

{
  Hv_ViewMapData   mdata;

  if ((mdata = Hv_GetViewMapData(View)) == NULL)
    return;

  Hv_DestroyMapList(&(mdata->landmaps));
  Hv_DestroyMapList(&(mdata->bordermaps));
  Hv_DestroyMapList(&(mdata->watermaps));
  Hv_DestroyMapList(&(mdata->unknownmaps));
}

/*-------- Hv_MapPointsToPoly --------*/

Hv_Point *Hv_MapPointsToPoly(Hv_View   View,
			     Hv_Map   map,
			     int    *npoly,
			     Boolean   ClipIt)

/* converts ntot map points, into a set of
  Hv_Points in the View's local system. For purposes
  of speed, conversions are done "manually" rather than
  via Hv_WorldToLocal etc. routines. In C++, this could
  be accomplshed with "inline" procedures  */

/* CALLING ROUTINE MUST FREE Hv_Points */

{
  Hv_ViewMapData mdata;
  Hv_Point       *xp = NULL;

  if ((mdata = Hv_GetViewMapData(View)) == NULL)
    return NULL;

  switch(mdata->projection) {
  case Hv_MERCATOR:
      if (Hv_alternativeMap)
	  xp = Hv_MercatorMapPointsToPoly2(View, map, npoly, ClipIt);
      else
	  xp = Hv_MercatorMapPointsToPoly(View, map, npoly, ClipIt);
      break;

  case Hv_ORTHOGRAPHIC:
      xp = Hv_OrthographicMapPointsToPoly(View, map, npoly, ClipIt);
      break;

  }

  return xp;
}


/*------   Hv_DrawSpecialMapsOnly ----------*/

void  Hv_DrawSpecialMapsOnly(Hv_View     View,
			     Hv_Region    region)

{
  Hv_ViewMapData  mdata;
  Hv_Map          map;

  if ((mdata = Hv_GetViewMapData(View)) == NULL)
    return;

  for (map = mdata->specialmaps; map != NULL; map = map->next) 
    Hv_DrawOneSpecialMap(View, map, region);
}



/* --------- Hv_DrawMapBordersOnly ---------*/

void Hv_DrawMapBordersOnly(Hv_View     View,
			   Hv_Region    region,
			   Boolean     specialtoo)

{
  Hv_LocalMap        temp;
  Hv_ViewMapData     mdata;
  Hv_Map             map;
  short              bcolor;

  if ((mdata = Hv_GetViewMapData(View)) == NULL)
    return;

  bcolor = mdata->landborder;

/* see if we need to build the local maps. We need only test
   for land maps  */

  if (mdata->landmaps == NULL) 
    Hv_BuildLocalMaps(View);

/* order is important  1) landmaps  2) water maps  3) border maps */

  for (temp = mdata->landmaps; temp!=NULL; temp=temp->next)
    Hv_DrawOneMap(temp, FRAMEMAP, 0, bcolor);

/* now the border maps */

  if (mdata->drawpoliticalborders)
    for (temp = mdata->bordermaps; temp!=NULL; temp=temp->next) 
        Hv_DrawOneMap(temp, FRAMEMAP, 0, bcolor);

/* now the water maps */

  for (temp = mdata->watermaps; temp!=NULL; temp=temp->next)
    Hv_DrawOneMap(temp, FRAMEMAP, 0, mdata->waterborder);

/* now the unknown maps */

  for (temp = mdata->unknownmaps; temp!=NULL; temp=temp->next)
    Hv_DrawOneMap(temp, FRAMEMAP, 0, bcolor);


/* now the special maps  -- which are maintained as "raw" maps*/

  if (specialtoo)
    for (map = mdata->specialmaps; map != NULL; map = map->next) 
      Hv_DrawOneSpecialMap(View, map, region);

}


/*------ Hv_DrawOverlayRect ------*/

void  Hv_DrawOverlayRect(Hv_View View,
			 float  latmin,
			 float  latmax,
			 float  longmin,
			 float  longmax,
			 short  color)

{
  static Hv_Map  map = NULL;
  Hv_LocalMap    localmap = NULL;
  Hv_LocalMap    temp;
  int            npoly;
  Hv_Point       *xp;
  int            i;

  float          tlong, dellong;
  float          tlat, dellat;

  if (map == NULL)
    map = Hv_MallocMap(40);

  dellat  = (float)((latmax  - latmin)/10.0);
  dellong = (float)((longmax - longmin)/10.0);

  map->type = Hv_SINGLEMAP;
  map->latitude[0] = latmin;      map->longitude[0] = longmin;
  map->latitude[10] = latmax;     map->longitude[10] = longmin;
  map->latitude[20] = latmax;     map->longitude[20] = longmax;
  map->latitude[30] = latmin;     map->longitude[30] = longmax;

  tlat  = map->latitude[0];
  tlong = map->longitude[0];

  for (i = 1; i < 10; i++) {
    tlat  = tlat  + dellat;
    tlong = tlong + dellong;
    map->latitude[i]     = tlat;   map->longitude[i]    = map->longitude[0];
    map->latitude[30-i]  = tlat;   map->longitude[30-i] = map->longitude[30];
    map->longitude[10+i] = tlong;  map->latitude[10+i]  = map->latitude[10];
    map->longitude[40-i] = tlong;  map->latitude[40-i]  = map->latitude[0];
  } 

/* pre convert as appropriate for MERCATOR maps */

    for (i = 0; i < 40; i++)
      map->mercatory[i] = Hv_MercatorLatToY(map->latitude[i]);

  while ((xp = Hv_MapPointsToPoly(View, map, &npoly, True)) != NULL) {
    if (npoly > 1)
      Hv_AddMapPoly(View, Hv_SINGLEMAP, &xp, npoly, &localmap);
    else
      Hv_Free(xp);
  }

  if (localmap != NULL) {
    for (temp = localmap; temp != NULL; temp = temp->next)
      Hv_DrawOneMap(temp, FILLMAP, color, Hv_black);
    Hv_DestroyMapList(&localmap);
  }
}

/* ------ Hv_DrawMapGrid ------*/

void Hv_DrawMapGrid(Hv_View   View,
		    Hv_Region  region)

/* this will draw a map grid. The way to use this is to
   the the views "userdraw" field to this function, probably
   at creation with the Hv_USERDRAW attribute */

{
  Hv_ViewMapData mdata;
  Hv_Region      hrreg = NULL;
  Hv_Region      clipreg = NULL;
  Hv_Region      ireg = NULL;
  Hv_Item        globe = NULL;
  Boolean        notmercator;


  if ((mdata = Hv_GetViewMapData(View)) == NULL)
    return;

/* if not mercator, we have some further checking */

  notmercator = (mdata->projection != Hv_MERCATOR);
  if (notmercator) {
    globe = mdata->GlobeItem;
    if ((globe == NULL) || (globe->region == NULL))
      return;
  }


/* this is also where we draw special maps (e.g. cities) 
   to ensure that they are always on top */

  if (mdata->drawgrid == False)
    return;

  hrreg = Hv_ClipHotRect(View, 0); 

  if (region != NULL) {
    ireg = Hv_IntersectRegion(region, hrreg);
    Hv_DestroyRegion(hrreg);
    hrreg = ireg;
	Hv_SetClippingRegion(hrreg);
  }


/* if not mercator, we have some further clipping */

  if (notmercator) {
    clipreg = Hv_IntersectRegion(globe->region, hrreg);
    Hv_SetClippingRegion(clipreg);
  }


  switch (mdata->projection) {
  case Hv_MERCATOR:
    Hv_MercatorGrid(View, hrreg);
    break;

  case Hv_ORTHOGRAPHIC:
    Hv_OrthographicGrid(View, clipreg);
    break;
  }

  
  Hv_DestroyRegion(hrreg);
  Hv_DestroyRegion(clipreg);
  Hv_RestoreClipRegion(region);
}



/*-------- Hv_GlobeClick ---------*/

static void Hv_GlobeClick(Hv_Event hvevent)

/******************************************
  A shift-click on a globe will cause the
  globe to rotate to that point
******************************************/

{
  Hv_ViewMapData    mdata;
  Boolean           shifted;
  Hv_View           View = hvevent->view;
  float             lambda0, phi1;

  shifted = Hv_CheckBit(hvevent->modifiers, Hv_SHIFT);

  if ((mdata = Hv_GetViewMapData(View)) == NULL)
    return;
  
  if (mdata->projection == Hv_MERCATOR)
    return;
  
  if (!shifted)
    return;

  Hv_LocalToLatLong(View,
		    hvevent->where.x,
		    hvevent->where.y,
		    &phi1,
		    &lambda0);

  Hv_ChangeMapCenter(View, phi1, lambda0);
  Hv_DestroyLocalMaps(View);
  Hv_SetViewDirty(View);
  Hv_DrawViewHotRect(View);
}


/**
 * Hv_DrawGlobeItem
 * @purpose   Draws GlobeItems, used in orthographic
 * map projections.
 * @param    Item    The GlobeItem.
 * @param    region  The clip region.
 * @local
 */

static void Hv_DrawGlobeItem(Hv_Item   Item,
							 Hv_Region  region) {

  Hv_View           View = Item->view;
  Hv_ViewMapData    mdata;
  float             fx, fy;
  Hv_Point          *poly;


  if ((mdata = Hv_GetViewMapData(View)) == NULL)
    return;

  if (mdata->projection == Hv_MERCATOR)
    return;

  Hv_DestroyRegion(Item->region);

/* get the globe center and polygon */

  Hv_LatLongToXY(View, &fx, &fy, mdata->phi1, mdata->lambda0);
  poly = Hv_GetGlobePolygon(View, fx, fy, mdata->radius, NUMGLOBEPOINTS);

  Item->region = Hv_CreateRegionFromPolygon(poly, (short)NUMGLOBEPOINTS);

  Hv_FillPolygon(poly, NUMGLOBEPOINTS, True, mdata->waterfill, mdata->waterborder);
  Hv_Free(poly);
}

/*--------- Hv_GetGlobePolygon -------*/

static Hv_Point *Hv_GetGlobePolygon(Hv_View  View,
				    float   xc,
				    float   yc,
				    float   rad,
				    int    n)

/* after testing this will probably become a more general routine */

{
  Hv_Point    *poly;
  int         i;
  float       theta, dtheta;
  float       x, y;

  if ((View == NULL) || (n < 5))
    return NULL;

  poly = Hv_NewPoints(n);
  if (poly == NULL)
    return NULL;
  
  dtheta  = (float)(Hv_TWOPI/(n-1));

  for (i = 0; i < n; i++) {
    theta = i*dtheta;
    x = (float)(xc + rad*cos(theta));
    y = (float)(yc + rad*sin(theta));
    Hv_WorldToLocal(View, x, y, &(poly[i].x), &(poly[i].y));
  }

  return poly;
}


/**
 * Hv_DrawTheMaps
 * @purpose
 * @param Item
 * @param mode
 * @param region
 * @local
 */

static void Hv_DrawTheMaps(Hv_Item   Item,
			   int     mode,
			   Hv_Region  region) {

  Hv_View          View = Item->view;
  Hv_LocalMap      temp;
  Hv_ViewMapData   mdata;
  Boolean          notmercator;
  Hv_Item          globe;
  Hv_Region        cliprgn;

  mdata = Hv_GetViewMapData(View);

/* extra clipping for non mercators */

  notmercator = (mdata->projection != Hv_MERCATOR);

  if (notmercator) {
    globe = mdata->GlobeItem;
    if ((globe == NULL) || (globe->region == NULL))
      return;

    if (Hv_inPostscriptMode)
      Hv_PSClipGlobePolygon(View);
    else {
      if (region == NULL)
	Hv_SetClippingRegion(globe->region);
      else {
	cliprgn = Hv_IntersectRegion(region, globe->region);
	Hv_SetClippingRegion(cliprgn);
	Hv_DestroyRegion(cliprgn);
      }
    }  /* non ps mode */
  }
  

/* order is important:
   1) landmaps
   2) border maps
   3) water maps
   4) unknown maps */

  for (temp = mdata->landmaps; temp != NULL; temp = temp->next)
    Hv_DrawOneMap(temp, mode, mdata->landfill, mdata->landborder);

/* now the border maps */

  if (mdata->drawpoliticalborders)
    for (temp = mdata->bordermaps; temp != NULL; temp = temp->next)
      Hv_DrawOneMap(temp, FRAMEMAP, mdata->landfill, mdata->landborder);
  
/* now the water maps */
  
  for (temp = mdata->watermaps; temp != NULL; temp = temp->next)
    Hv_DrawOneMap(temp, mode, mdata->waterfill, mdata->waterborder);
  
/* now the unknown maps */

  for (temp = mdata->unknownmaps; temp != NULL; temp = temp->next)
    Hv_DrawOneMap(temp, mode, mdata->landfill, mdata->landborder);
  
  
  if (notmercator)
    Hv_RestoreClipRegion(region);

}

/*----------- Hv_PSClipGlobePolygon ------*/

static void  Hv_PSClipGlobePolygon(Hv_View View)

{
  float             fx, fy;
  Hv_ViewMapData    mdata;
  Hv_Point          *poly;
  Hv_Point          *pspoly;


  if ((mdata = Hv_GetViewMapData(View)) == NULL)
    return;

  if (mdata->projection == Hv_MERCATOR)
    return;

/* get the globe center and polygon -- convert to ps polygon */

  Hv_LatLongToXY(View, &fx, &fy, mdata->phi1, mdata->lambda0);
  poly = Hv_GetGlobePolygon(View, fx, fy, mdata->radius, NUMGLOBEPOINTS);
  pspoly = Hv_NewPoints(NUMGLOBEPOINTS);
  Hv_LocalPolygonToPSLocalPolygon(poly, pspoly, NUMGLOBEPOINTS);
  Hv_PSClipPolygon(pspoly, NUMGLOBEPOINTS);

  Hv_Free(poly);
  Hv_Free(pspoly);
}


/*-------- Hv_MapAfterOffset --------*/

static void Hv_MapAfterOffset(Hv_Item  Item,
			      short   dh,
			      short   dv)

/* called after an offset, e.g. entire view was moved */

{
  Hv_View            View = Item->view;
  Hv_ViewMapData     mdata;

  if (Hv_offsetDueToScroll)
    Hv_DestroyLocalMaps(View);
  else {   /* go through all the maps, ofsetting each map polygon.*/
    if ((mdata = Hv_GetViewMapData(View)) == NULL)
      return;
    Hv_OffsetMapList(mdata->landmaps,    dh, dv);
    Hv_OffsetMapList(mdata->bordermaps,  dh, dv);
    Hv_OffsetMapList(mdata->watermaps,   dh, dv);
    Hv_OffsetMapList(mdata->unknownmaps, dh, dv);
  }

}

/*-------- Hv_FixMapRegion -------*/

static void Hv_FixMapRegion(Hv_Item Item)

{

/* This routine will be called after a zoom */

	Hv_Println("destroying local maps");
  Hv_DestroyLocalMaps(Item->view);
}


/*------- Hv_OffsetMapList -------*/

static void Hv_OffsetMapList(Hv_LocalMap   maps,
			     short      dh,
			     short      dv)

{
  Hv_LocalMap    temp;

  for (temp = maps; temp != NULL; temp = temp->next)
    Hv_OffsetPolygon(temp->mappoly, temp->numpoints, dh, dv);
}

/* ------- Hv_BuildLocalMaps ------*/

void Hv_BuildLocalMaps(Hv_View View)

/* converts global maps into local polynomials */

{
  Hv_ViewMapData  mdata;
  Hv_Map          temp;
  Hv_Point        *xp;
  int             npoly;

  if ((mdata = Hv_GetViewMapData(View)) == NULL)
    return;

  if (mdata->landmaps != NULL) {
    Hv_Println("Warning! attempted to build over existing local maps!");
    return;
  }

/* loop over all raw maps (stored in the link list named "allmaps".
   add maps with more than one point */

  for (temp = Hv_allMaps; temp != NULL; temp=temp->next) {
    while ((xp = Hv_MapPointsToPoly(View, temp, &npoly, True)) != NULL) {
      if (npoly > 1)
        Hv_AddMapPoly(View, temp->type, &xp, npoly, NULL);
      else
	Hv_Free(xp);
    }

  }  /* end loop over maps */

}

/*----- Hv_AddMapPoly ------*/

static void Hv_AddMapPoly(Hv_View   View,
			  int     mtype,
			  Hv_Point   **xp,
			  int     npoly,
			  Hv_LocalMap *localmap)

{
  Hv_LocalMap      temp;
  Hv_ViewMapData   mdata;
  int              i;
  Hv_Point         *tp;
  Hv_Rect          rect;
  Hv_Point         hrc;

  int              visstep = 5;

  if ((mdata = Hv_GetViewMapData(View)) == NULL)
    return;

/* if it is not visible, don't bother */

  for (i = 0; i < npoly; i = i + visstep) {
    tp = (*xp) + i;
    if (Hv_PointInRect(*tp, View->hotrect))
      goto isvisible;
  }

/* a final check that the map doesn't encompass the hotrect */

  Hv_PolygonEnclosingRect(&rect, *xp, npoly);
  Hv_SetPoint(&hrc,
	  (short)((View->hotrect->left + View->hotrect->right)/2),
	  (short)((View->hotrect->top + View->hotrect->bottom)/2));

  if (Hv_PointInRect(hrc, &rect))
    goto isvisible; 

  Hv_Free(*xp);
  *xp = NULL;
  return;

 isvisible:


  switch (mtype) {
  case Hv_LANDMAP:
    temp = Hv_AddLocalMapToList(&(mdata->landmaps));
    break;

  case Hv_WATERMAP:
    temp = Hv_AddLocalMapToList(&(mdata->watermaps));
    break;

  case Hv_BORDERMAP:
    temp = Hv_AddLocalMapToList(&(mdata->bordermaps));
    break;

  case Hv_UNKNOWNMAP:
    temp = Hv_AddLocalMapToList(&(mdata->unknownmaps));
    break;
    
  case Hv_SINGLEMAP:
    temp = Hv_AddLocalMapToList(localmap);
    break;

  default:
    Hv_Println("Unknown map type encountered. Exiting.");
    exit(-1);
  }

  temp->numpoints = npoly;
  temp->type = mtype;
  temp->mappoly = (*xp);
  temp->next = NULL;
}


/*------ Hv_AddLocalMapToList ------*/

static Hv_LocalMap Hv_AddLocalMapToList(Hv_LocalMap *map)

{
  Hv_LocalMap   temp;

  if (*map == NULL) {
    *map =  Hv_NewLocalMap();
    temp = *map;
  }
  else {
    for (temp = *map; temp->next != NULL; temp = temp->next)
      ;
    temp->next =  Hv_NewLocalMap();
    temp = temp->next;
  }
  return temp;
}


/*--------- Hv_NewLocalMap ----------*/

static Hv_LocalMap Hv_NewLocalMap(void) 

{
  return  (Hv_LocalMap)Hv_Malloc(sizeof(Hv_LocalMapRec));
}


/* -------- Hv_OrthographicMapPointsToPoly -------*/

static Hv_Point *Hv_OrthographicMapPointsToPoly(Hv_View   View,
						Hv_Map   map,
						int    *npoly,
						Boolean   ClipIt)

/***********************************************************
  This takes the map and converts it to a pixel based
  polygon of points "npoly".
************************************************************/

{
  int            i;
  Hv_Point       *xp, *yp, *pp;
  float          fx, fy;
  float          rx, ry, xx, yy;   /*for manual conversion */
  Hv_FRect       *world;
  Hv_Rect        *local;
  static int     Pass = 1;
  int            ngood = 0;
  int            ngoodlim;

  short          oldx = -32765;
  short          oldy = -32765;

  float          *goodx = NULL;
  float          *goody = NULL;

  
/* the multiple passes are really needed fo the Mercator
   projection. Here the seccond pass is just a dummy
   that returns immediately */

  if (Pass == 2) {
    Pass = 1;
    return NULL;
  }
  Pass = 2;

/* call the routine that takes the map and converts
  in into a "good" map, returning the number of
  "good" points */

  if (map->type == Hv_BORDERMAP)
    ngoodlim = 3;
  else if (map->type == Hv_SINGLEMAP)
    ngoodlim = 1;
  else
    ngoodlim = 7;

  ngood = Hv_MakeAGoodMap(View, map, ngoodlim, &goodx, &goody);

  if (Hv_mapPrint)
      Hv_Println("np = %d ngood = %d", map->numpoints, ngood);

  if (ngood == 0) {
    Pass = 1;
    return NULL;
  }

/* if we made it here, we can start converting to 
   a local polygon */

  local = View->local;
  world = View->world;
  rx = ((float)local->width)/world->width;
  ry = ((float)local->height)/world->height;
  
  xp = Hv_NewPoints(ngood);
  pp = xp;
  *npoly = 0;


  for (i = 0; i < ngood; i++) {

    fx = goodx[i];
    fy = goody[i];
    xx = local->left + rx*(fx - world->xmin);
    yy = local->top  + ry*(world->ymax - fy);
      
    pp->x = Hv_InRange(xx);
    pp->y = Hv_InRange(yy);

    if ((pp->x != oldx) || (pp->y != oldy)) { 
      oldx = pp->x;
      oldy = pp->y;
      (*npoly)++;
      pp++;
    } /* unique point */
  }   /* end of main for loop */

  Hv_Free(goodx);
  Hv_Free(goody);

  
/* compress xp and return */

  if (*npoly == 0) {
    Pass = 1;
    Hv_Free(xp);
    xp =  NULL;
  }  
  else if (*npoly != ngood) {
    yp = Hv_NewPoints(*npoly);
    memcpy((void *)yp, (void *)xp, (size_t)((*npoly)*sizeof(Hv_Point))); 
    Hv_Free(xp);
    xp =  yp;
  }
  
  return xp;

}


/*---------- Hv_MakeAGoodMap -----------*/

static int Hv_MakeAGoodMap(Hv_View  View,
			   Hv_Map   map,
			   int    ngoodlim,
			   float   **xx,
			   float   **yy)

/***************************************************
  converts a map to a "good" map by removing the
  "bad" points (those that aren't visible) and adding
  "connectors" so that the polygons behave properly
  near the borders.
****************************************************/

{
  Hv_ViewMapData mdata;
  int            i, j, jm1, np;
  float          *x, *y;
  Boolean        *good;
  int            ngood, nextgood;
  int            nreq;
  int            firstgood = -1;
  float          *goodx, *goody;
  float          r2, theta, ctheta1, ctheta2, cctheta;

  if ((mdata = Hv_GetViewMapData(View)) == NULL)
    return 0;

  np = map->numpoints;

/* allocate a temporary array for the x's and y's */

  x = (float *)Hv_Malloc(np*sizeof(float));
  y = (float *)Hv_Malloc(np*sizeof(float));
  good = (Boolean *)Hv_Malloc(np*sizeof(Boolean));

  ngood = 0;
  nreq = 0;   /* number needed in the "good" arrays */
  
  for (i = 0; i < np; i++) {

/* must undo the mercator y, and count the number of good points */

/*    lat = Hv_MercatorYToLat(map->latitude[i]); */
    Hv_LatLongToXY(View, &(x[i]), &(y[i]),
		   map->latitude[i], map->longitude[i]);

    good[i] = Hv_mapPointIsGood;
    if (good[i]) {
      if (firstgood < 0)
	firstgood = i;

      ngood++;
      nreq++;

    } /* end good */
    else { /* bad */
      if ((i > 0) && (good[i-1]))  /* means previous point was good */
	nreq += 5;   /* for "connector */

    }  /* end bad */
  }  /* end for loop */

/* maybe its already time to give up */

  if (ngood < ngoodlim) {
    Hv_Free(x);
    Hv_Free(y);
    Hv_Free(good);
    return 0;
  }

  
  r2 = (float)(2.0 * mdata->radius);

  i = firstgood;
  j = 0;
  goodx = (float *)Hv_Malloc(nreq*sizeof(float));
  goody = (float *)Hv_Malloc(nreq*sizeof(float));

step2:

  while (good[i]) {
    goodx[j] = x[i];
    goody[j] = y[i];

    j++;
    i++;
    if (i >= np)
      goto eod;
  }

/* we have run into a bad point (i). We must make a 5 point "connctor":
   point 1: the previous good point scaled to sit on edge of globe
   point 2: a distant "compass" point
   point 3: another compass point for connecting pts 2 and 4
   point 4: a compass point for the next good point
   point 5: the next good point scaled to sit on the border
 */

  jm1 = j - 1;  /* last "good" point saved */

/* 1st point of connector can be filled in now */

  if (Hv_mapPrint)
      Hv_Println("jm1 = %d y = %f x = %f", jm1, y[jm1], x[jm1]); 

  theta = (float)(atan2(goody[jm1], goodx[jm1]));
  goodx[j] = (float)(mdata->radius*cos(theta));
  goody[j] = (float)(mdata->radius*sin(theta));

  if (Hv_mapPrint)
      Hv_Println("\nj = %d BP1 [%f, %f] -> [%f, %f]",
		 j, goodx[jm1], goody[jm1], goodx[j], goody[j]);
  j++;

/* second point can be filled in now */

  ctheta1 = Hv_NearestCompassAngle(theta);
  goodx[j] = (float)(r2*cos(ctheta1));
  goody[j] = (float)(r2*sin(ctheta1));

  if (Hv_mapPrint)
      Hv_Println("j = %d CP1 [%f, %f]",
		 j, goodx[j], goody[j]); 
  j++;

  
/* now need the next good point (note: point "i" is bad) */

  nextgood = i;
  while (!good[i]) {
    i++;
    if (i >= np) {
      nextgood = firstgood;
      goto connect;
    }
    nextgood = i;
  }
  

/* next block is where we go to complete the connector */

 connect:

  theta = (float)(atan2(y[nextgood], x[nextgood]));
  goodx[j+2] = (float)(mdata->radius*cos(theta));
  goody[j+2] = (float)(mdata->radius*sin(theta));

  ctheta2 = Hv_NearestCompassAngle(theta);
  goodx[j+1] = (float)(r2*cos(ctheta2));
  goody[j+1] = (float)(r2*sin(ctheta2));

  cctheta = Hv_ConnectingCompassAngle(ctheta1, ctheta2);
  goodx[j] = (float)(r2*cos(cctheta));
  goody[j] = (float)(r2*sin(cctheta));

  j += 3;

/* if nextgood is firstgood, we have ended on a string of bads
   and made a connection to the first good. Thus we
   are done. Otherwise, we are starting another string of "goods" */

  if (nextgood != firstgood)
    goto step2;
  else
      if (Hv_mapPrint)
	  Hv_Println("ended on a bad string.\n\n");
 

/* next is the "end of data" block */

 eod:

  if (j != nreq)
    Hv_Println("error in Hv_MakeAGoodMap j = %d   nreq = %d", j, nreq);

/* if here, we have sufficient godd points to proceed */

  Hv_Free(x);
  Hv_Free(y);
  Hv_Free(good);
  *xx = goodx;
  *yy = goody;

  return nreq;
}


/*--------- Hv_NearestCompassAngle ----------*/

static float Hv_NearestCompassAngle(float theta)

/*************************************
note only positive angles are returned 
***************************************/


{

  if ((theta > -Hv_PIOVER4) && (theta <= Hv_PIOVER4))
    return 0.0;
  if ((theta > Hv_PIOVER4) && (theta <= Hv_THREEPIOVER4))
    return (float)Hv_PIOVER2;
  if ((theta > -Hv_THREEPIOVER4) && (theta <= -Hv_PIOVER4))
    return (float)Hv_THREEPIOVER2;
  return (float)Hv_PI;
}

/*--------- Hv_ConnectingCompassAngle ----------*/

static float Hv_ConnectingCompassAngle(float theta1,
				       float theta2)

{
  float  cctheta;
  float  tmin, tmax;

  cctheta = (float)(0.5*(theta1 + theta2));

/* correct for the one weird case where one
   angle is 0, the other is 270. We want it
   to return 315 instead of 135. */

  tmin = Hv_fMin(theta1, theta2);
  tmax = Hv_fMax(theta1, theta2);

  if ((fabs(tmax - Hv_THREEPIOVER2) < 0.1) && (fabs(tmin) < 0.1)) {
    cctheta = (float)(-Hv_PIOVER4);
  }
  
  return cctheta;
}

static Boolean CheckInRange(float xx, 
							float yy, short *sx, short *sy, 
							int badcnt, 
							float rad) {
	static float rsq = -1.0;
	double theta;

	*sx = Hv_InRange(xx);
	*sy = Hv_InRange(yy);

	if (rsq < 0.0) {
		rsq = rad*rad;
	}

	if ((fabs(xx) < rad) && (fabs(yy) < rad))
		return False;

    if ((xx*xx + yy*yy) < rsq) {
		return False;
	}

	if ((badcnt % BADPOINTSTEP) != 0)
		return True;

	theta = atan2(yy, xx);
	*sx = (short)(rad*cos(theta));
	*sy = (short)(rad*sin(theta));
/*	Hv_Println("xx, yy = [%f, %f]     sx, sy = [%d %d]", xx, yy, *sx, *sy);*/
	return True;
}


/* -------- Hv_MercatorMapPointsToPoly -------*/

static Hv_Point *Hv_MercatorMapPointsToPoly(Hv_View   View,
											Hv_Map   map,
											int    *npoly,
											Boolean   ClipIt) {

/* makes three passes: (to take into account the possibility
   that the map falls off one side of the Mercator projection
   and shows up on the other side:

   1) pass 1 -> polygon for all on one side

   2) pass 2 -> polygon for all points on other side, if any 

   3) pass 3 -> resets and returns NULL */


  Hv_Point      *xp, *yp, *pp;
  float          oldfx;
  float          fx;
  short          oldx = -32765;
  short          oldy = -32765;
  Boolean        useit = True;
  int            i;
  Hv_Rect        LimitRect;
  int            badcnt = 0;

  float          rx, ry, xx, yy;   /*for manual conversion */
  Hv_FRect      *world;
  Hv_Rect       *local;
  Hv_ViewMapData mdata;

  Boolean       outsidelimitrect;

  static int Pass = 1;
  static int ChangeSidePoint = 0;

  if ((mdata = Hv_GetViewMapData(View)) == NULL)
    return NULL;

  Hv_CopyRect(&LimitRect, View->hotrect);
  Hv_ShrinkRect(&LimitRect, -LIMIT_RECT_DEL, -LIMIT_RECT_DEL);

/* reset if we are on the third pass, or if we are on the second
   pass but there are no "other side" points to pick up */

  if ((Pass > 2) || ((Pass == 2) && (ChangeSidePoint < 1))) {
    Pass = 1;
    ChangeSidePoint = 0;
    return NULL;
  }

  xp = Hv_NewPoints(map->numpoints);
  pp = xp;
  *npoly = 0;

/* use oldfx to watch for map changing from one side to the other */

  oldfx = Hv_MercatorLongToX(mdata->lambda0, map->longitude[ChangeSidePoint]);

  local = View->local;
  world = View->world;
  rx = ((float)local->width)/world->width;
  ry = ((float)local->height)/world->height;

  for (i = ChangeSidePoint; i < (int)(map->numpoints); i++) {
    fx = Hv_MercatorLongToX(mdata->lambda0, map->longitude[i]);

/* arbitrarily use Hv_PIOVER2 to indicate too big x step */

    useit = (fabs(fx - oldfx) < Hv_PIOVER2);

/* note: we stored the mercator y value rather than the latitude */

    if (useit) {
		xx = local->left + rx*(fx - world->xmin);
		yy = local->top  + ry*(world->ymax - map->mercatory[i]);

		outsidelimitrect = CheckInRange(xx, yy, 
			&(pp->x), &(pp->y), 
			badcnt, 
			3000.0);

/* only use this point if it is different */

		if (useit) {
			if ((pp->x == oldx) && (pp->y == oldy)) {
				useit = False;
			}
		}

/* dont use all the bad points */

		if (useit && ClipIt && outsidelimitrect) {
			useit = ((badcnt % BADPOINTSTEP) == 0);
			badcnt++;
			i += BADPOINTSKIP;
		}
		
		if (useit) {
			oldx = pp->x;
			oldy = pp->y;
			pp++;
			(*npoly)++;
		}

		oldfx = fx;
    }  /*stop of useit*/
	
    else {   /* don't use it*/
		if (ChangeSidePoint < 1)
			ChangeSidePoint = i;
    }
	
  }   /* end of for i loop */
  
  Pass++;

/* compress xp and return */

  if (*npoly == (int)(map->numpoints))
	  return xp;
  else {
	  yp = Hv_NewPoints(*npoly);
	  memcpy((void *)yp, (void *)xp, (size_t)((*npoly)*sizeof(Hv_Point))); 
	  Hv_Free(xp);
	  return yp;
  }
}


/* -------- Hv_MercatorMapPointsToPoly2 -------*/

static Hv_Point *Hv_MercatorMapPointsToPoly2(Hv_View   View,
					     Hv_Map   map,
					     int    *npoly,
					     Boolean   ClipIt)


{
  Hv_Point      *xp, *yp, *pp;
  float          oldfx;
  float          fx;
  short          oldx = -32765;
  short          oldy = -32765;
  Boolean        useit = True;
  int            i;
  Hv_Rect        LimitRect;
  int            badcnt = 0;

  float          rx, ry, xx, yy;   /*for manual conversion */
  Hv_FRect      *world;
  Hv_Rect       *local;
  Hv_ViewMapData mdata;

  static int done = False;
  static int ChangeSidePoint = 0;

  if ((mdata = Hv_GetViewMapData(View)) == NULL)
    return NULL;

  Hv_CopyRect(&LimitRect, View->hotrect);
  Hv_ShrinkRect(&LimitRect, -LIMIT_RECT_DEL, -LIMIT_RECT_DEL);


  if (done) {
    done = False;
    ChangeSidePoint = 0;
    return NULL;
  }

  xp = Hv_NewPoints(map->numpoints);
  pp = xp;
  *npoly = 0;

/* use oldfx to watch for map changing from one side to the other */

  oldfx = Hv_MercatorLongToX(mdata->lambda0, map->longitude[ChangeSidePoint]);

  local = View->local;
  world = View->world;
  rx = ((float)local->width)/world->width;
  ry = ((float)local->height)/world->height;


  for (i = ChangeSidePoint; i < (int)(map->numpoints); i++) {
    fx = Hv_MercatorLongToX(mdata->lambda0, map->longitude[i]);

/* arbitrarily use Hv_PIOVER2 to indicate too big x step */

    useit = (fabs(fx - oldfx) < Hv_PIOVER2);

/* note: we stored the mercator y value rather than the latitude */

    if (useit) {
      xx = local->left + rx*(fx - world->xmin);
      yy = local->top  + ry*(world->ymax - map->mercatory[i]);
      
      pp->x = Hv_InRange(xx);
      pp->y = Hv_InRange(yy);

/* only use this point if it is different */

      useit = ((pp->x != oldx) || (pp->y != oldy));

/* dont use all the bad points */

      if (useit && ClipIt && !Hv_PointInRect(*pp, &LimitRect)) {
	useit = ((badcnt % BADPOINTSTEP) == 0);
	badcnt++;
	i += BADPOINTSKIP;
      }

      if (useit) {
	oldx = pp->x;
	oldy = pp->y;
	pp++;
	(*npoly)++;
      }

      oldfx = fx;
    }  /*stop of useit*/

    else {   /* don't use it*/
	ChangeSidePoint = i;
	break;
    }

    if (i >= (int)(map->numpoints)-1) {
	done = True;
    }
  }   /* end of for i loop */


/* compress xp and return */

  if (*npoly == (int)(map->numpoints))
    return xp;
  else {
    yp = Hv_NewPoints(*npoly);
    memcpy((void *)yp, (void *)xp, (size_t)((*npoly)*sizeof(Hv_Point))); 
    Hv_Free(xp);
    return yp;
  }
}


/* ------- Hv_DrawOneMap ------*/

static void Hv_DrawOneMap(Hv_LocalMap    map,
			  int        mode,
			  short       fillcolor,
			  short       framecolor)

{
  if (Hv_inPostscriptMode && Hv_printAllColorsBlack) {
    fillcolor = Hv_white;
    framecolor = Hv_black;
  }

  switch (mode) {
  case FRAMEMAP:
    Hv_DrawLines(map->mappoly, (short)(map->numpoints), framecolor);
    break;

  case FILLMAP:
    if ((map->numpoints < MINPOINTDRAW) && (map->type != Hv_SINGLEMAP))
      return;
    Hv_FillPolygon(map->mappoly, (short)(map->numpoints), False, fillcolor, framecolor);
    break;

  case FRAMEANDFILLMAP:
    if ((map->numpoints < MINPOINTDRAW) && (map->type != Hv_SINGLEMAP))
      return;
    Hv_FillPolygon(map->mappoly, (short)(map->numpoints), True, fillcolor, framecolor);
    break;

  case POINTSONLY:
    Hv_DrawPoints(map->mappoly, map->numpoints, Hv_yellow);
    break;

  }

}

/*------- Hv_DestroyMapList -------*/

static void Hv_DestroyMapList(Hv_LocalMap   *maps)

/* NOTE: This destroys LOCAL maps, not the actual maps */

{
  Hv_LocalMap    temp, temp2;

  for (temp = *maps; temp != NULL; temp = temp2) {
    temp2 = temp->next;
    Hv_Free(temp->mappoly);
    Hv_Free(temp);
  }

  *maps = NULL;
}


#define  OGRIDSTEP 80

/*----- Hv_OrthographicGrid --------*/

static void Hv_OrthographicGrid(Hv_View    View,
				Hv_Region  region)

{
  float          tlat, tlong;
  short          xt, yt;
  Hv_ViewMapData mdata;
  float          dellong, dellat;
  float          lambda0;
  int            i, j;
  Hv_Point       pp[OGRIDSTEP];

  if ((mdata = Hv_GetViewMapData(View)) == NULL)
    return;

/* first the latlines */

  tlat = -90.0;
  dellong = (float)(359.99/(OGRIDSTEP-1));
  lambda0 = Hv_RADTODEG*mdata->lambda0;

  while (tlat < 90.001) {
    j = 0;
    tlong = (float)(lambda0 - 180.0);
    
    for (i = 0; i < OGRIDSTEP; i++) {
      tlong += dellong;
      
/*      tlong = Hv_fMin(179.99, Hv_fMax(-179.99, tlong)); */

      Hv_LatLongToLocal(View, &xt, &yt, Hv_DEGTORAD*tlat, Hv_DEGTORAD*tlong);
      
      if (Hv_mapPointIsGood) {
	Hv_SetPoint(&(pp[j]), xt, yt);
	j++;
      }
    }

    if (j > 1)
      Hv_DrawLines(pp, (short)j, mdata->gridcolor);
    
    
    tlat += mdata->latstep;
  }

/* longitude lines */

  tlong = -180.0;
  dellat = (float)(180.0/(OGRIDSTEP-1));
  
  while (tlong < 180.0) {

    j = 0;  /*keeps track of "good" points */

    for (i = 0; i < OGRIDSTEP; i++) {
      tlat = (float)(-90.0 + i*dellat);
      Hv_LatLongToLocal(View, &xt, &yt, Hv_DEGTORAD*tlat, Hv_DEGTORAD*tlong);

      if (Hv_mapPointIsGood) {
	Hv_SetPoint(&(pp[j]), xt, yt);
	j++;
      }
    }

    if (j > 1)
      Hv_DrawLines(pp, (short)j, mdata->gridcolor);
    
    
    tlong += mdata->longstep;
  }
  
}

#undef OGRIDSTEP

/*----- Hv_MercatorGrid --------*/

static void Hv_MercatorGrid(Hv_View   View,
			    Hv_Region region)

{
  float          latmax = (float)85.01;
  float          longmax = (float)180.01;
  float          longmin = (float)(-179.01);
  float          tlat, tlong;
  short          xt, yt, xs, ys;
  Hv_Rect        hr;
  Hv_String      *hvstr;
  short          w, h;
  Hv_Region      reg1 = NULL;
  Hv_Region      reg2 = NULL;
  Hv_Rect        rect1;
  Hv_ViewMapData mdata;


  if ((mdata = Hv_GetViewMapData(View)) == NULL)
    return;

  Hv_CopyRect(&hr, View->hotrect);

/* first the latlines */

  tlat = 0.0;
  while (tlat < latmax) {
    Hv_LatLongToLocal(View, &xt, &yt, Hv_DEGTORAD*tlat, 0.0);
    if (yt < hr.top)
      break;

    if (yt < hr.bottom) {
      Hv_DrawLine(hr.left, yt, hr.right, yt, mdata->gridcolor);

/* now the label (scrolling requires special care) */

      hvstr = GetValStr(tlat, "N", &w, &h, mdata->gridlabelcolor);

      xs = hr.right-w;
      ys = yt-h/2;

      if ((Hv_scrollDH != 0) && (!Hv_drawingOffScreen))
	ReadyLabReg(&rect1, &reg1, xs, ys, w, h);
      else
	Hv_SimpleDrawString(xs, ys,  hvstr);
      Hv_DestroyString(hvstr);
      
    }
    tlat += mdata->latstep;
  }
  
  tlat = -mdata->latstep;
  while (tlat > -latmax) {
    Hv_LatLongToLocal(View, &xt, &yt, Hv_DEGTORAD*tlat, 0.0);
    if (yt > hr.bottom)
      break;
    
    if (yt > hr.top) {
      Hv_DrawLine(hr.left, yt, hr.right, yt, mdata->gridcolor);

/* label (scrolling requires special care) */

      hvstr = GetValStr(tlat, "S", &w, &h, mdata->gridlabelcolor);
      xs = hr.right-w;
      ys = yt-h/2;

      if ((Hv_scrollDH != 0) && (!Hv_drawingOffScreen))
	ReadyLabReg(&rect1, &reg1, xs, ys, w, h);
      else
	Hv_SimpleDrawString(xs, ys,  hvstr);
      Hv_DestroyString(hvstr);

    }
    tlat -= mdata->latstep;
  }

/* longitude lines */

  tlong = 0.0;
  while ((tlong - mdata->longstep) > longmin)
    tlong -= mdata->longstep;
  
  while (tlong < longmax) {
    Hv_LatLongToLocal(View, &xt, &yt, 0.0, Hv_DEGTORAD*tlong);
    if ((xt < hr.right) && (xt > hr.left)) {
      Hv_DrawLine(xt, hr.top, xt, hr.bottom, mdata->gridcolor);

/* label (scrolling requires special care) */

      if ((tlong > 179.9) && (tlong < 180.01))
	hvstr = GetValStr(tlong, " ", &w, &h, mdata->gridlabelcolor);
      else if (tlong < 0.0)
	hvstr = GetValStr(tlong, "W", &w, &h, mdata->gridlabelcolor);
      else
	hvstr = GetValStr(tlong, "E", &w, &h, mdata->gridlabelcolor);
      xs = xt-w/2;
      ys = hr.bottom-h-1;

      if ((Hv_scrollDV != 0) && (!Hv_drawingOffScreen))
	ReadyLabReg(&rect1, &reg1, xs, ys, w, h);
      else
	Hv_SimpleDrawString(xs, ys,  hvstr);
      Hv_DestroyString(hvstr);
      
    }
    tlong += mdata->longstep;
  }

  if ((reg1 != NULL) && (!Hv_drawingOffScreen)) {
    reg2 = Hv_IntersectRegion(reg1, region);
    Hv_scrollDH = 0; 
    Hv_scrollDV = 0; 
    Hv_DrawView(View, reg2);
    Hv_DestroyRegion(reg1);
    Hv_DestroyRegion(reg2);
  }
}


/*------- ReadyLabReg ------*/

static void ReadyLabReg(Hv_Rect  *rect1,
			Hv_Region *reg1,
			short   x,
			short   y,
			short   w,
			short   h)

{
  Hv_SetRect(rect1, x, y, w, h);
  if (*reg1 == NULL)
    *reg1 = Hv_RectRegion(rect1);
  else
    Hv_UnionRectWithRegion(rect1, *reg1);

  Hv_OffsetRect(rect1, Hv_scrollDH, Hv_scrollDV);
  Hv_UnionRectWithRegion(rect1, *reg1);
}

/*------- GetValStr -------*/

static Hv_String *GetValStr(float  val,
			    char  *c,
			    short  *w,
			    short  *h,
			    short  color)

{
  Hv_String     *hvstr;
  char          text[20];

  val = (float)fabs(val);
  

  if (val < 0.01)
    strcpy(text, "0");
  else if (val < 10.0)
    sprintf(text, "%-1.0f%s", val, c);
  else if (val < 100.0)
    sprintf(text, "%-2.0f%s", val, c);
  else
    sprintf(text, "%-3.0f%s", val, c);
   

  hvstr = Hv_CreateString(text);
  hvstr->font = Hv_fixed;
  hvstr->strcol = color;
  Hv_StringDimension(hvstr, w, h);
  return hvstr;
}

#undef BADPOINTSTEP
#undef LIMIT_RECT_DEL
#undef BADPOINTSKIP

#undef  FRAMEMAP
#undef  FILLMAP 
#undef  FRAMEANDFILLMAP
#undef  POINTSONLY
#undef  LANDMAP
#undef  WATERMAP
#undef  BORDERMAP
#undef  MINPOINTDRAW

#undef  NUMGLOBEPOINTS






