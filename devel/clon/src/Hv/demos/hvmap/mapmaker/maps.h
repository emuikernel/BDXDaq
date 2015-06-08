/* 
---------------------------------------------------------------------------
-
-   	File:     maps.h

-										
-  Date       Programmer     Comments						
--------------------------------------------------------------------------------
-  7/01/94	DPH		Initial Version					
*/

#ifndef __MAPS
#define __MAPS

#define   DEGTORAD     0.0174533
#define   MAPNAMESIZE  25
 
typedef enum {False, True} Boolean;

/* mapdata holds a continuous map */

typedef struct mapdata  *MapDataPtr;

typedef struct mapdata
{
  char           mapname[MAPNAMESIZE];   /* name of map */
  unsigned int   numpoints;     /* number of lat-long pairs */
  float         *latitude;      /* array of latitude points */
  float         *longitude;     /* array of longitude points */
  short         user1;          /* for whatever */
  short         user2;          /* for whatever */
  MapDataPtr    next;
  MapDataPtr    prev;
} MapData;
#endif










