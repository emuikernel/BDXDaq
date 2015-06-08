/* 
   map_put_char.c
   Created           : 12-NOV-1992 by Thom Sulanke
*/

#include <stdio.h>                   /* I/O definitions                      */
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <map_manager.h>
#include <map_internal.h>

/***********************************************************************/
/*                                                                     */
/*   MAP_PUT_CHAR                                                      */
/*   ------------                                                      */
/*                                                                     */
/*         Created     : 12-NOV-1992    Author : Thom Sulanke          */
/*         Purpose     : put array of char values into the map.        */
/*                                                                     */
/***********************************************************************/
 
int map_put_char( const char filename[], const char subsystemname[], 
		 const char itemname[], int length, const char array[], 
		 int atime )
 
{
  int fd ;
  pointer_t item_loc, pred_loc, array_loc;
  item_t item;
  arrayheader_t header, suc_header;
  int status, tindex, suc_time;
  table_t *table;
  pointer_t dummy;

  header.spare[0] = 0;
  
  /* get the current time */
  
  if ( atime == 0 )
    {
      header.time = time(NULL);
    }
  else
    {
      header.time = abs(atime);
    }
  
  /* check arguements */
  
  if ( strlen(subsystemname) > MAP_NAMELENGTH-1 )
    {
      map_error(MAP_USER_ERROR_ARGUMENT,
		"map_put_char: Subsystem name (%s) longer than %d characters.",
		subsystemname,
		MAP_NAMELENGTH-1);
      return MAP_USER_ERROR_ARGUMENT;
    }
  
  if ( strlen(itemname) > MAP_NAMELENGTH-1 )
    {
      map_error(MAP_USER_ERROR_ARGUMENT,
		"map_put_char: Item name (%s) longer than %d characters.",
		itemname,
		MAP_NAMELENGTH-1);
      return MAP_USER_ERROR_ARGUMENT;
    }
  
  /* open file for read and write */
  
  fd = map_open_rw(filename);
  if ( fd < MAP_OK )
    {
      map_syserror(fd,
		   "map_put_char: can't open TheMap (%s) for adding values",
		   filename);
      return fd;
    }
  
  /* find item location and read item */
  
  status = map_find_item(fd,subsystemname,itemname,&item_loc);
  if ( status < 0 )
    {
      map_close_map(fd);
      return status;
    }
  status = map_read(&item,sizeof item,fd,item_loc,swapitem);
  if ( status != MAP_OK )
    {
      map_close_map(fd);
      return status;
    }
  
  /* check array size */
  
  if ( length <= 0 || length > item.length )
    {
      map_error(MAP_USER_ERROR_ARGUMENT,
		"map_put_char: Array length (%d) incorrect for subsystem (%s), item (%s).\n Should be no greater than %d.",
		length,
		subsystemname,
		itemname,
		item.length);
      map_close_map(fd);
      return MAP_USER_ERROR_ARGUMENT;
    }
  header.length = length;
  
  /* check array type */
  
  if ( item.type != 2 )
    {
      map_error(MAP_USER_ERROR_ARGUMENT,
		"map_put_char: Array type (%d) for subsystem (%s), item (%s) \n requires call to different map_put_* routine.",
		item.type,
		subsystemname,
		itemname);
      map_close_map(fd);
      return MAP_USER_ERROR_ARGUMENT;
    }
  
  /* find place for array in list */
  
  status = map_time_pred(fd,item_loc+ITEM_LIST_OFFSET,item,header.time,
			 &tindex,&header.next,&suc_time,&pred_loc);
  if ( status < 0 )
    {
      map_close_map(fd);
      return status;
    }
  
  /* check if sucessor is same time */
  
  if ( suc_time == header.time )
    {
      if ( atime >= 0 )
	{
	  map_warn(MAP_USER_WARN_NOREPLACE,
		    "map_put_char: Values already exist for this time (%d) \n for subsystem (%s), item (%s).",
		    header.time,
		    subsystemname,
		    itemname);
	  map_close_map(fd);
	  return MAP_USER_WARN_NOREPLACE;
	}
      else
	{  
	  /* set succ to its succ to delete it */
	  
	  status = map_read(&suc_header,sizeof suc_header,fd,header.next,swaparrayheader);
	  if ( status != MAP_OK )
	    {
	      map_close_map(fd);
	      return status;
	    }
	  
	  header.next = suc_header.next;
	}
    }
  
  /* fill in entry next pointer */
  
  status = map_write(&header,sizeof header,fd,&array_loc,swaparrayheader);
  if ( status < 0 )
    {
      map_close_map(fd);
      return status;
    }
  
  /* insert entry */
  
  status = map_write(array,header.length*sizeof(char),fd,&dummy,swapchararray);
  if ( status < 0 )
    {
      map_close_map(fd);
      return status;
    }
  
  if ( item.list < 0 )
    
    /* version 2 format */
    /* update table */
      
    {
      if ( item.table_used < item.table_length )
	table = (table_t *) calloc(item.table_used+1,sizeof(table_t));
      else
	table = (table_t *) calloc(item.table_length * TABLE_FACTOR,sizeof(table_t));

      if ( item.table_used > 0 )
	{
	  status = map_read(table,item.table_used*sizeof(table_t),fd,item.table,swaptablearray);
	  if ( status != MAP_OK )
	    {
	      free(table);
	      map_close_map(fd);
	      return status;
	    }
	}
      if ( atime >= 0 || suc_time != header.time )
	{
	  if ( item.table_used-tindex-1 > 0 )
	    memmove(&table[tindex+2],&table[tindex+1],(item.table_used-tindex-1)*sizeof(table[0]));
	  table[tindex+1].time = header.time;
	  table[tindex+1].loc = array_loc;
	  item.table_used++;
	}
      else
	{
	  /* replaced string only change location */
	  
	  table[tindex+1].loc = array_loc;
	}
      if ( item.table_used <= item.table_length )
	{
	  status = map_overwrite(table,item.table_used*sizeof(table_t),fd,item.table,swaptablearray);
	  if ( status != MAP_OK )
	    {
	      free(table);
	      map_close_map(fd);
	      return status;
	    }
	}
      else
	{
	  item.table_length = item.table_length * TABLE_FACTOR;
	  status = map_write(table,item.table_length*sizeof(table_t),fd,
				 &item.table,swaptablearray);
	  if ( status < 0 )
	    {
	      free(table);
	      map_close_map(fd);
	      return status;
	    }
	}
      free(table);
      status = map_overwrite(&item,sizeof item,fd,item_loc,swapitem);
      if ( status != MAP_OK )
	{
	  map_close_map(fd);
	  return status;
	}
    }
  else
    {
      
      /* version 1 format */   
      /* update pointer to new entry */
      
      status = map_overwrite(&array_loc,sizeof array_loc,fd,pred_loc,swappointer);
      if ( status != MAP_OK )
	{
	  map_close_map(fd);
	  return status;
	}
      
    }
  
  /* close the file */
  
  return map_close_map(fd);
  
}
