h40072
s 00000/00000/00000
d R 1.2 03/05/23 12:18:00 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 clas/cmon/map.s/map_fillname.c
e
s 00050/00000/00000
d D 1.1 03/05/23 12:17:59 boiarino 1 0
c date and time created 03/05/23 12:17:59 by boiarino
e
u
U
f e 0
t
T
I 1
/* 
        map_fillname.c
         Created           :  7-OCT-1992 by Thom Sulanke
*/
 
#include <stdio.h>                   /* I/O definitions                       */
#include "map_manager.h"
#include "map_internal.h"

/***********************************************************************/
/*                                                                     */
/*   MAP_FILLNAME                                                      */
/*   ------------                                                      */
/*                                                                     */
/*         Created     :  7-OCT-1992    Author : Thom Sulanke          */
/*         Purpose     : fill standard size char array with subsystem, */
/*                       item, etc name.  Also remove trailing blanks  */
/*                       which come from FORTRAN calls, etc            */
/*                                                                     */
/***********************************************************************/
 
void map_fillname( char to[], const char from[] )
 
{
    int i;
    i = 0;

/* copy until null character or standard length is reached. */

    while ( i < MAP_NAMELENGTH-1 && from[i] != '\0' )
    {
        to[i] = from[i];
        ++i;
    }

/* backup over trailing blanks */

    while ( i > 1 && from[i-1] == ' ' )
      {
	--i;
      }

/* pad with null characters */

    while ( i < MAP_NAMELENGTH  )
    {
        to[i] = '\0';
        ++i;
    }
}
E 1
