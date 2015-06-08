/*
---------------------------------------------------------------------------
-
-   	File:     Hv_maps.h
-
-	main header file for the map routines
-
-										
-  Revision history:								
-                     							        
-  Date       Programmer     Comments						
--------------------------------------------------------------------------------
-  08/24/95	DPH		Initial Version					
-------------------------------------------------------------------------------
*/


#ifndef __HVMAPH__
#define __HVMAPH__


/* Map Projections */

#define     Hv_MERCATOR           1
#define     Hv_ORTHOGRAPHIC       2


/* numerical constants */

#define     Hv_DEFAULTRADIUS     2.25

/* map types */

#define     Hv_LANDMAP            0
#define     Hv_WATERMAP           1
#define     Hv_BORDERMAP          2
#define     Hv_UNKNOWNMAP         3
#define     Hv_SPECIALMAP         4
#define     Hv_SINGLEMAP          5

/* min and max heights for map views */

#define    Hv_MERCATORMINH        0.005
#define    Hv_MERCATORMINW        0.0075
#define    Hv_MERCATORMAXH        3.94625
#define    Hv_MERCATORMAXW        6.283185307
#define    Hv_ORTHOGRAPHICMINH    0.005
#define    Hv_ORTHOGRAPHICMINW    0.0075
#define    Hv_ORTHOGRAPHICMAXH    60.0
#define    Hv_ORTHOGRAPHICMAXW    60.0

/* a auxillary data structure for special maps, which
   hold things like lists of cities */

typedef struct hvspecialmapdata  *Hv_SpecialMapData;

typedef struct hvspecialmapdata

{
  Hv_Rect       *enclosure;     /* surrounds the map */
  char          *mapname;       /* overall map name */
  short          drawit;        /* for on/off drawing of special maps */
  short          symbol;        /* symbol of special map */
  short          color;         /* color of symbol of special map */
  short          width;         /* width of symbol */
  char         **pointnames;    /* e.g. city names */
  void          *data;          /* user data */
} Hv_SpecialMapDataRec;


/* Hv_Map holds the actual maps read from binary map file */

typedef struct hvmapdata  *Hv_Map;

typedef struct hvmapdata
{
  Hv_Map         next;          /* to make linked list*/
  int            type;          /* e.g. Hv_LANDMAP */
  unsigned int   numpoints;     /* number of lat-long pairs */
  float         *mercatory;     /* array of latitude points massegsed for mercator */
  float         *latitude;      /* array of latitude points (radians) */
  float         *longitude;     /* array of longitude points (radians) */
  void          *data;          /* auxillary pointer */
} Hv_MapRec;


/* local maps hold the polygon (i.e. pixel coords) of a map.
   There may be more than one local map for each actual map,
   because maps "wrap" around */

typedef struct hvlocalmapdata  *Hv_LocalMap;

typedef struct hvlocalmapdata
{
  int                 type;
  int                 numpoints;
  Hv_Point           *mappoly;
  Hv_LocalMap         next;
} Hv_LocalMapRec;


/* data structure pointed to by mapdata field in each  View */

typedef struct hvviewmapdatarec *Hv_ViewMapData;

typedef struct hvviewmapdatarec
{
  int         projection;          /* mercator, orthographic */
  float       phi1;                /* central latitude for map display */
  float       lambda0;             /* central longitude for map display */

  float       radius;              /* for use in orthographics projections */

/* cache some trig functions */

  float       coslam0, sinlam0, cosphi1, sinphi1;
  float       rcosphi1, rsinphi1;

  Hv_LocalMap landmaps;               /* linked list of land maps */
  Hv_LocalMap watermaps;              /* linked list of water maps (lakes and seas) */
  Hv_LocalMap bordermaps;             /* linked list of border maps */
  Hv_LocalMap unknownmaps;            /* linked list of unknown maps */
  Hv_Map      specialmaps;            /* eg cities -- kept inlat-long form (NOT USED IN DEMO)*/
  Boolean     drawpoliticalborders;   /* draw border maps?  (NOT USED IN DEMO)*/

  Boolean     rotateearth;            /* rotate earth? (orthographics only) */
  Boolean     drawgrid;               /* draw lat-long grid? */
  float       latstep, longstep;      /* gridline spacing */ 

/* map fill and border colors */

  short       gridcolor;
  short       waterfill;
  short       waterborder;
  short       landfill;
  short       landborder;
  short       outerspace;

  Boolean          specialflag;
  Hv_FunctionPtr   specialdraw;

  Boolean          extraflag;
  Hv_FunctionPtr   extradraw;

  Boolean          anotherflag;
  Hv_FunctionPtr   anotherdraw;

  Hv_Item      GlobeItem;
  short        gridlabelcolor;
  float        deflambda0;    /* default central longitude for map display */
} Hv_ViewMapDataRec;

#ifdef __cplusplus
extern "C"
{
#endif


extern Boolean   Hv_alternativeMap;
extern Boolean   Hv_mapPrint;
extern float     Hv_RADEARTH;


/*------ Hv_mapmath.c -----*/

extern  Boolean Hv_LatLongIsVisible(Hv_View View,
				    float  latitude,
				    float  longitude);

extern void Hv_MakeGCLine(float lat1,
			  float long1,
			  float lat2,
			  float long2,
			  int   np,
			  float *latitude,
			  float *longitude);


extern float Hv_GetAzimuth(float lat1,
			   float long1,
			   float lat2,
			   float long2);

extern void Hv_GetAzimuthAndGCD(float lat1,
			   float long1,
			   float lat2,
			   float long2,
			   float *azimuth,
			   float *gcd);

void Hv_OffsetLatLong(float latitude,
                    float longitude,
                    float range,
                    float azimuth,
                    float *newlat,
                    float *newlong);

extern void Hv_GetGoodMapLimits(Hv_View View,
			 float  latmin,
			 float  latmax,
			 float  longmin,
			 float  longmax,
			 float  *xmin,
			 float  *xmax,
			 float  *ymin,
			 float  *ymax,
			 Boolean qz);

extern void Hv_LatLongToLocal(Hv_View  View,
		       short    *x,
		       short    *y,
		       float    latitude,
		       float    longitude);

extern void Hv_LocalToLatLong(Hv_View  View,
		       short    x,
		       short    y,
		       float    *latitude,
		       float    *longitude);

extern void Hv_XYToLatLong(Hv_View View,
		    float  x,
		    float  y,
		    float  *latitude,
		    float  *longitude);

extern void Hv_WorldToLatLong(Hv_View View,
			      float  x,
			      float  y,
			      float  *latitude,
			      float  *longitude);

extern void Hv_LatLongToXY(Hv_View View,
		    float  *x,
		    float  *y,
		    float  latitude,
		    float  longitude);

extern void Hv_LatLongToWorld(Hv_View View,
			      float  *x,
			      float  *y,
			      float  latitude,
			      float  longitude);

extern void Hv_ChangeMapCenter(Hv_View  View,
			       float    phi1,
			       float    lambda0);

extern void Hv_FixMapParameters(Hv_View View);

extern float Hv_MercatorYToLat(float  y);

extern float Hv_MercatorLatToY(float lat);

extern float Hv_MercatorXToLong(float  lambda0,
			 float  x);

extern float Hv_MercatorLongToX(float  lambda0,
			 float  longitude);

extern float Hv_GCDistance(float lat1,
		    float long1,
		    float lat2,
		    float long2);

/*------ Hv_mapitem.c ------*/

extern Hv_ViewMapData   Hv_GetViewMapData(Hv_View   View);

extern void  Hv_MallocViewMapData(Hv_View   View);

extern void  Hv_DestroyViewMapData(Hv_View   View);

extern Boolean Hv_PointIsOnLand(Hv_View   View,
			 Hv_Point  pp);

extern Hv_Region Hv_LandMapRegion(Hv_View  View);

extern void Hv_DrawMapItem(Hv_Item    Item,
		    Hv_Region     region);

extern void Hv_DrawOneSpecialMap(Hv_View   View,
			  Hv_Map   map,
			  Hv_Region  region);

extern void Hv_InitializeMap(Hv_Item        Item,
		      Hv_AttributeArray   attributes);

extern void Hv_ChangeMapProjection(Hv_View  View,
			    int    newprojection);

extern void Hv_DestroyLocalMaps(Hv_View  View);

extern void Hv_BuildLocalMaps(Hv_View  View);

extern Hv_Point *Hv_MapPointsToPoly(Hv_View   View,
			     Hv_Map   map,
			     int    *npoly,
			     Boolean   ClipIt);

extern void  Hv_DrawSpecialMapsOnly(Hv_View     View,
			     Hv_Region    region);

extern void Hv_DrawMapBordersOnly(Hv_View     View,
			   Hv_Region    region,
			   Boolean     specialtoo);

extern void  Hv_DrawOverlayRect(Hv_View View,
			 float  latmin,
			 float  latmax,
			 float  longmin,
			 float  longmax,
			 short  color);

extern void Hv_DrawMapGrid(Hv_View   View,
		    Hv_Region  region);



/*------ Hv_mapreader.c ------*/

extern Hv_SpecialMapData Hv_GetSpecialMapData(Hv_Map  map);

extern Hv_SpecialMapData Hv_MallocSpecialMapData(void);

extern Hv_Map Hv_MallocMap(unsigned int np);

extern void Hv_InitMaps(char *fname0);

extern void  Hv_DestroyMap(Hv_Map  *map);

/*-------- Hv_worldpolygon.c -----*/

extern void Hv_DrawPolygonMap(Hv_Item    Item,
			      Hv_Region  region,
			      Hv_Map     map,
			      Boolean    dounion,
			      short      color,
			      short      fillcolor,
			      short      pattern);

extern void Hv_GetPolygonMap(Hv_Item              Item,
			     Hv_WorldPolygonData *wp);


/*-------- Hv_worldwedge.c -----*/


extern void Hv_SetWedgeMapPoints(Hv_View View,
				 Hv_Map  map,
				 float latitude,
				 float longitude,
				 float radinner,
				 float radouter,
				 float azim1,
				 float azim2,
				 int   mode);

extern void Hv_DrawWedgeMap(Hv_View    View,
			    Hv_Item    Item,
			    Hv_Region  region,
			    Hv_Map     map,
			    Boolean    dounion,
			    short      color,
			    short      fillcolor,
			    short      pattern);

/* one specific global variable */

extern  Hv_Map         Hv_allMaps;

#ifdef __cplusplus
}
#endif

#endif



