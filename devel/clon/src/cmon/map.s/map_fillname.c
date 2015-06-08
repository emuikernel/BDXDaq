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
