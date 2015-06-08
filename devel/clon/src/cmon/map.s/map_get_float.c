/* 
        map_get_float.c
         Created           : 16-NOV-1992 by Thom Sulanke
*/
 
#include <stdio.h>                   /* I/O definitions                       */
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include "map_manager.h"
#include "map_internal.h"

/***********************************************************************/
/*                                                                     */
/*   MAP_GET_FLOAT                                                     */
/*   --------------                                                    */
/*                                                                     */
/*         Created     : 16-NOV-1992    Author : Thom Sulanke          */
/*         Purpose     : get array of float values from the map.  if no*/
/*                          values were valid at atime, vtime is set to*/
/*                          -1 and array is undefined.  vtime is set to*/
/*                          -2 if there is an error.                   */
/*         Input:    filename: the map file name                       */
/*                     subsystemname: subsystem name                   */
/*                     itemname: item name                             */
/*                     length: length of array                         */
/*                     atime: time when values were valid              */
/*                             (0 for current time)                    */
/*         Output:    array: array of values                           */
/*                     vtime: time when values were first valid        */
/***********************************************************************/
 
int map_get_float( const char filename[], const char subsystemname[], 
		  const char itemname[], int length, float array[], int atime , 
		  int *vtime )
     
{
  int fd ;
  pointer_t item_loc, pred_loc, succ_loc;
  item_t item;
  int status, succ_time;
  int tindex;    /* unused */
  int ctry;
  
  /* set error value in case of early return */
  
  *vtime = -2;

  /* get the current time */
  
  if ( atime == 0 )
    {
      atime = time(NULL);
    }
  
  /* check arguements */
  
  if ( strlen(subsystemname) > MAP_NAMELENGTH-1 )
    {
      map_error(MAP_USER_ERROR_ARGUMENT,
		"map_get_float: Subsystem name (%s) longer than %d characters.",
		subsystemname,
		MAP_NAMELENGTH-1);
      return MAP_USER_ERROR_ARGUMENT;
    }
  
  if ( strlen(itemname) > MAP_NAMELENGTH-1 )
    {
      map_error(MAP_USER_ERROR_ARGUMENT,
		"map_get_float: Item name (%s) longer than %d characters.",
		itemname,
		MAP_NAMELENGTH-1);
      return MAP_USER_ERROR_ARGUMENT;
    }
  
  /* open file for read */
  /* first time try without file locking. If nomatch or nofind errors occur,
     try again with file locking.  It is possible that if another process
     is updating a table that the last entry in the table might not be
     used and thus a nomatch or nofind.  */
  
  ctry = 1;
 tryagain:
  if ( ctry == 1 )
    fd = map_open_ro(filename);
  else
    {
      map_warn(MAP_USER_WARN_NOFIND,
	       "map_get_float: Retrying for time %d, subsystem (%s), item (%s).\n",
	       atime,
	       subsystemname,
	       itemname);
      fd = map_open_ro_lock(filename);
    }
  
  if ( fd < 0 )
    {
      map_syserror(fd,
		   "map_get_float: can't open TheMap (%s).",
		   filename);
      return fd;
    }
  
  /* find item location and read item */
  
  status = map_find_item(fd,subsystemname,itemname,&item_loc);
  if ( status < 0 )
    {
      map_close_map(fd);
      if ( ctry == 1 && status == MAP_USER_WARN_NOMATCH )
	{
	  ctry = 2;
	  goto tryagain;
	}
      return status;
    }
  status = map_read(&item,sizeof item,fd,item_loc,swapitem);
  if ( status != MAP_OK ) 
    {
      map_close_map(fd);
      return status;
    }
  
  /* check array size */
  
  if ( length != item.length )
    {
      map_error(MAP_USER_ERROR_ARGUMENT,
		"map_get_float: Array length (%d) incorrect for subsystem (%s),\n item (%s).  Should be %d.\n",
		length,
		subsystemname,
		itemname,
		item.length);
      map_close_map(fd);
      return MAP_USER_ERROR_ARGUMENT;
    }
  
  /* check array type */
  
  if ( item.type != 1 )
    {
      map_error(MAP_USER_ERROR_ARGUMENT,
		"map_get_float: Array type (%d) for subsystem (%s), item (%s) \n requires call to different map_get_* routine.",
		item.type,
		subsystemname,
		itemname);
      map_close_map(fd);
      return MAP_USER_ERROR_ARGUMENT;
    }
  
  /* check if any values */
  
  if ( (item.list == NULL_LOC) || (item.list < 0 && item.table_used == 0 ))
    {
      map_warn(MAP_USER_WARN_NOFIND,
	       "map_get_float: No values for subsystem (%s), item (%s).\n",
	       subsystemname,
	       itemname);
      *vtime = -1;
      map_close_map(fd);
      return MAP_USER_WARN_NOFIND;
    }  /* If */
  
  /* find place for array in list */
  
  status = map_time_pred(fd,item_loc+ITEM_LIST_OFFSET,item,atime,&tindex,&succ_loc,&succ_time,&pred_loc);
  if ( status < 0 ) 
    {
      map_close_map(fd);
      return status;
    }
  
  if ( succ_loc == NULL_LOC )
    {
      map_close_map(fd);
      if ( ctry == 1 )
	{
	  ctry = 2;
	  goto tryagain;
	}
      
      map_warn(MAP_USER_WARN_NOFIND,
	       "map_get_float: No values at time %d for subsystem (%s), item (%s).\n",
	       atime,
	       subsystemname,
	       itemname);
      *vtime = -1;
      return MAP_USER_WARN_NOFIND;
    }
  
  /* extract time and values */
  
  *vtime = succ_time;
  
  status = map_read(array,length*sizeof(float),fd,succ_loc+ARRAY_VALUES_OFFSET,swapfloatarray);
  if ( status != MAP_OK ) 
    {
      map_close_map(fd);
      return status;
    }
  
  /* close the file */
  
  return map_close_map(fd);
  
}
