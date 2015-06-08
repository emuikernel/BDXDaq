h43177
s 00000/00000/00000
d R 1.2 03/05/23 12:18:01 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 clas/cmon/map.s/map_get_flo_.c
e
s 00060/00000/00000
d D 1.1 03/05/23 12:18:00 boiarino 1 0
c date and time created 03/05/23 12:18:00 by boiarino
e
u
U
f e 0
t
T
I 1
/* 
        map_get_float_.c
         Created           :  9-FEB-1993 by Thom Sulanke
*/

#include "map_manager.h"
#include "map_internal.h"
#include <string.h>

/***********************************************************************/
/*                                                                     */
/*   map_get_float_.c                                                  */
/*   ----------------                                                  */
/*                                                                     */
/*         Created     :  9-Feb-1993    Author : Thom Sulanke          */
/*         Purpose     :  interface routine to be called from FORTRAN  */
/*                                                                     */
/***********************************************************************/
 
int map_get_float_(char *filename, char *subsystemname,
                  char *itemname, int *arraylength, float *array, 
		  int *atime, int *firsttime,
	          int filename_len, int subsystemname_len, int itemname_len)
 
{
    char filename_copy[MAP_FILENAMELENGTH];
    char subsystemname_copy[MAP_NAMELENGTH];
    char itemname_copy[MAP_NAMELENGTH];
    
    if ( filename_len > MAP_FILENAMELENGTH-1 ){
      map_error(MAP_USER_ERROR_ARGUMENT,
		"MAP_REM_ARR: filename longer (%d) than %d characters.",
		filename_len,
		MAP_FILENAMELENGTH-1);
      return MAP_USER_ERROR_ARGUMENT;
    }
    
    if ( subsystemname_len > MAP_NAMELENGTH-1 ){
      map_error(MAP_USER_ERROR_ARGUMENT,
		"MAP_REM_ARR: subsystemname longer (%d) than %d characters.",
		subsystemname_len,
		MAP_NAMELENGTH-1);
      return MAP_USER_ERROR_ARGUMENT;
    }
    
    if ( itemname_len > MAP_NAMELENGTH-1 ){
      map_error(MAP_USER_ERROR_ARGUMENT,
		"MAP_REM_ARR: itemname longer (%d) than %d characters.",
		itemname_len,
		MAP_NAMELENGTH-1);
      return MAP_USER_ERROR_ARGUMENT;
    }
    
    map_conv_Fstring(filename_copy, filename, filename_len);
    map_conv_Fstring(subsystemname_copy, subsystemname, subsystemname_len);
    map_conv_Fstring(itemname_copy, itemname, itemname_len);

    return map_get_float(filename_copy,subsystemname_copy,itemname_copy,
			 *arraylength,array,*atime,firsttime);
}
E 1
