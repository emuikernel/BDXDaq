/*
----------------------------------------------------
-							
-  File:    Hv_mapreader.c					
-							
-  Summary:						
-            Collection of routines reading binary maps
-					       
-                     				
-  Revision history:				
-                     				
-  Date       Programmer     Comments		
------------------------------------------------------
-  09/05/95	DPH		Initial Version	
-                                              	
------------------------------------------------------
*/

#include "Hv.h"
#include "Hv_maps.h"

static FILE   *mapfile;

/*------- local prototypes -------*/

static Hv_Map  Hv_CreateMap(char    *name,
			    unsigned int npoints,
			    Hv_Map    parent);

static Hv_Map Hv_ReadBinaryMap(void);

static void Hv_LoadMaps(void);

static int Hv_MapType(char *mname);

/*-------- Hv_GetSpecialMapData -------*/

Hv_SpecialMapData Hv_GetSpecialMapData(Hv_Map map)

{
  if (map == NULL)
	  return NULL;
  else
	  return (Hv_SpecialMapData)(map->data);
}

/*------ Hv_MallocSpecialMapData -------*/

Hv_SpecialMapData Hv_MallocSpecialMapData(void)

{
  Hv_SpecialMapData      smap;

  smap = (Hv_SpecialMapData)Hv_Malloc(sizeof(Hv_SpecialMapDataRec));

  smap->mapname = NULL;
  smap->drawit = True;
  smap->symbol = Hv_RECTSYMBOL;
  smap->color = Hv_black;
  smap->width = 8;
  smap->pointnames = NULL;
  smap->enclosure = NULL;

  return smap;
}



/*------- Hv_MallocMap ------*/

Hv_Map  Hv_MallocMap(unsigned int np)

{
  Hv_Map       map;
  size_t       size;

  map = (Hv_Map)Hv_Malloc(sizeof(Hv_MapRec));

  map->numpoints = np;
  map->latitude  = NULL;
  map->mercatory = NULL;
  map->longitude = NULL;
  map->next = NULL;
  map->data = NULL;
  map->type = Hv_UNKNOWNMAP;

  if (np > 0) {
    size = (size_t)(np*sizeof(float));
    map->longitude = (float *)Hv_Malloc(size);
    map->mercatory = (float *)Hv_Malloc(size);
    map->latitude  = (float *)Hv_Malloc(size);
  }

  return map;
}


/*------- Hv_InitMaps ---------*/

void  Hv_InitMaps(char *fname0)

{
  char   *mname;

  Hv_allMaps = NULL;

/* try to get the map file */

  mapfile = Hv_LookForNamedFile(fname0, Hv_ioPaths, "rb");

  if (!mapfile) {
    if (Hv_Question("Can't find the map file. Do you want to look for it?")) {
      mname = Hv_FileSelect("Please locate the map file.", "*.map*", NULL);
      mapfile = fopen(mname, "rb");
      Hv_Free(mname);
    }
  }

/* if still don't have it, give up */

  if (!mapfile) {
    Hv_Println("Cannot run without maps!\nExiting.");
    exit (-1);
  }
    
  Hv_LoadMaps();
}


/**
 * Hv_DestroyMap
 * @purpose   Destroy a map. Actually destroys from the
 *  given map to end of linked list.
 * @param   map   The map to destroy.
 */

void   Hv_DestroyMap(Hv_Map *map) {


  Hv_Map            tmap;
  int               i, npts;
  Hv_SpecialMapData smap;

  if (map == NULL)
	  return;

  tmap = (Hv_Map)(*map);

  if (tmap->next != NULL)
    Hv_DestroyMap(&(tmap->next));

  if (tmap->type == Hv_SPECIALMAP) {
    smap = Hv_GetSpecialMapData(tmap);

    Hv_Free(smap->mapname);

/* remember pointnames is an array of char pointers */

    if (smap->pointnames != NULL) {
      npts = tmap->numpoints;
      for (i = 0; i < npts; i++)
	Hv_Free(smap->pointnames[i]);
      Hv_Free(smap->pointnames);
    }

    Hv_Free(smap->enclosure);
  } /* end special map */

  if (tmap->data != NULL)
    Hv_Free(tmap->data);
  Hv_Free(tmap->mercatory);
  Hv_Free(tmap->latitude);
  Hv_Free(tmap->longitude);
  Hv_Free(tmap);
  *map = NULL;
}

/*------ Hv_CreateMap -------*/

static Hv_Map  Hv_CreateMap(char    *name,
			    unsigned int npoints,
			    Hv_Map    parent)

{
  Hv_Map   tmap;

  tmap = Hv_MallocMap(npoints);

  tmap->type = Hv_MapType(name);
  tmap->numpoints = npoints;

/* link the list */

  if (parent != NULL)
    parent->next = tmap;


  return  tmap;
}


/*------ Hv_ReadBinaryMap -------*/

static Hv_Map  Hv_ReadBinaryMap(void)

{
  Hv_Map       map;
  char         tnam[25];
  unsigned int np;
  int status1, status2;


  if (fread(&(tnam[0]), 1, 25, mapfile) == 0)
    return NULL;

  if (fread(&np, sizeof(int), 1, mapfile) == 0)
    return NULL;

  map = Hv_CreateMap(tnam, np, NULL);

  status1 = fread(map->latitude,  sizeof(float), map->numpoints, mapfile);
  status2 = fread(map->longitude, sizeof(float), map->numpoints, mapfile);

/* xmin .... xymax not written */

  if ((status1 == 0) || (status2 == 0)) {
    Hv_DestroyMap(&map);
    Hv_Println("Read error in ReadBinaryMap. Exiting.");
    exit(-1);
  }

  return  map;
}

/*------- Hv_LoadMaps --------*/

static void Hv_LoadMaps(void)

{
  Hv_Map      map;
  int         dummy;
  Hv_Map      prevMap = NULL;
  int         i;

  Hv_Println("checking mapfile...");
  rewind(mapfile);

/* read two ints for compatibility with earlier versions */

  if (fread(&dummy, sizeof(int), 1, mapfile) == 0) {
    Hv_Println("Error in mapfile.\nExiting.");
    exit(-1);
  }

  if (fread(&dummy, sizeof(int), 1, mapfile) == 0)  {
    Hv_Println("Error in mapfile.\nExiting.");
    exit(-1);
  }

  while ((map = Hv_ReadBinaryMap()) != NULL) {
    if (prevMap == NULL)
      Hv_allMaps = map;
    else
      prevMap->next = map;
    prevMap = map;

/* pre convert as appropriate for MERCATOR maps */

    for (i = 0; i < (int)(map->numpoints); i++)
/*      map->latitude[i] = Hv_MercatorLatToY(map->latitude[i]); */
      map->mercatory[i] = Hv_MercatorLatToY(map->latitude[i]);
  }

  Hv_Println("Map file understood");
}


/*------- Hv_MapType -------*/

static int Hv_MapType(char *mname)

{
  if (mname == NULL)
    return Hv_UNKNOWNMAP;

  if (Hv_Contained(mname, "Sea") || Hv_Contained(mname, "Lake"))
    return Hv_WATERMAP;
  
  if (Hv_Contained(mname, "Border") || Hv_Contained(mname, "border"))
    return Hv_BORDERMAP;
  
  if (Hv_Contained(mname, "Unknown") || Hv_Contained(mname, "unknown"))
    return Hv_UNKNOWNMAP;
  
  return Hv_LANDMAP;
}

