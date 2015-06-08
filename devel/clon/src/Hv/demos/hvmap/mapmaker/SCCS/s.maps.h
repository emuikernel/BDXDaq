h18543
s 00000/00000/00000
d R 1.2 02/08/26 22:16:08 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 Hv/demos/hvmap/mapmaker/maps.h
e
s 00045/00000/00000
d D 1.1 02/08/26 22:16:07 boiarino 1 0
c date and time created 02/08/26 22:16:07 by boiarino
e
u
U
f e 0
t
T
I 1
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










E 1
