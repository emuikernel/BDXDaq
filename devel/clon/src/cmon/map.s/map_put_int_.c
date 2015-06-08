/* 
        map_put_int_.c
         Created           :  9-FEB-1993 by Thom Sulanke
*/

#include <map_manager.h>
#include <map_internal.h>
#include <string.h>

/***********************************************************************/
/*                                                                     */
/*   map_put_int_.c                                                    */
/*   ----------                                                        */
/*                                                                     */
/*         Created     :  9-Feb-1993    Author : Thom Sulanke          */
/*         Purpose     :  interface routine to be called from FORTRAN  */
/*                                                                     */
/***********************************************************************/
 
int map_put_int_(char *filename, char *subsystemname,
                  char *itemname, int *arraylength, int *array, 
		  int *firsttime,
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

    return map_put_int(filename_copy,subsystemname_copy,itemname_copy,
		       *arraylength,array,*firsttime);
}
