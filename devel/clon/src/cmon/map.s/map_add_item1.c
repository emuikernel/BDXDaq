/* 
   map_add_item1.c
   Created           :  15-Nov-1993 by Thom Sulanke
*/

static const char sccsid[] = "@(#)"__FILE__"\t4.2\tCreated 5/5/97 17:47:50, \tcompiled "__DATE__;
 
#include <stdio.h>                   /* I/O definitions                      */
#include <string.h>
#include <stdlib.h>
#include <map_manager.h>
#include <map_internal.h>
 

/***********************************************************************/
/*                                                                     */
/*   MAP_ADD_ITEM1                                                     */
/*   -------------                                                     */
/*                                                                     */
/*         Created     :  15-NOV-1993    Author : Thom Sulanke         */
/*         Purpose     :  add a new item to map (internal version)     */
/*                                                                     */
/***********************************************************************/
 
int map_add_item1( const char filename[], const char subsystemname[], 
		  const char itemname[], int length, int type, int table_size )
{
  int fd ;
  pointer_t pred_loc, succ_loc, item_loc, subsystem_loc, header_loc;
  item_t item, successor;
  int status;
  int tindex;
  nametable_t *nametable;
  subtableheader_t header;
  subsystem_t subsystem;
  table_t *table;
  int ret;

  /* check arguments */
  
  if ( strlen(subsystemname) > MAP_NAMELENGTH-1 )
    {
      map_error(MAP_USER_ERROR_ARGUMENT,
		"map_add_item: Subsystem name (%s) longer than %d characters.",
		subsystemname, MAP_NAMELENGTH-1);
      return MAP_USER_ERROR_ARGUMENT;
    }
  
  if ( strlen(itemname) > MAP_NAMELENGTH-1 )
    {
      map_error(MAP_USER_ERROR_ARGUMENT,
		"map_add_item: Item name (%s) longer than %d characters.",
		itemname, MAP_NAMELENGTH-1);
      return MAP_USER_ERROR_ARGUMENT;
    }
  
  if ( length < 1 )
    {
      map_error(MAP_USER_ERROR_ARGUMENT,
		"map_add_item: Array length (%d) out of range for subsystem (%s), item (%s).",
		length, subsystemname, itemname);
      return MAP_USER_ERROR_ARGUMENT;
    }
  
  if ( type < 0 || type > 2 )
    {
      map_error(MAP_USER_ERROR_ARGUMENT,
		"map_add_item: Array type (%d) out of range for subsystem (%s), item (%s).",
		type, subsystemname, itemname);
      return MAP_USER_ERROR_ARGUMENT;
    }
  
  /* open file for read and write */
  
  fd = map_open_rw(filename);
  if ( fd < MAP_OK )
    {
      map_syserror(fd,
		   "map_add_item: can't open (for read/write) TheMap (%s).",
		   filename);
      return fd;
    }
  
  /* add subsystem */
  
  status = map_add_sub(fd,subsystemname,INIT_NAMETABLE_SIZE);
  if ( status != MAP_OK ) 
    {
      map_close_map(fd);
      return status;
    }
  
  /* fill entry name */
  
  item.spare[0] = 0;
  map_fillname(item.name,itemname);
  item.list = -VERSION;
  item.length = length;
  item.type = type;
  
  /* find location in list for entry */
  
  status = map_read(&header_loc,sizeof header_loc,fd,ROOT,swaprootpointer);
  if ( status != MAP_OK )
    {
      map_close_map(fd);
      return status;
    }
  
  status = map_read(&header,sizeof header,fd,header_loc,swapsubtableheader);
  if ( status != MAP_OK )
    {
      map_close_map(fd);
      return status;
    }
  
  if ( header.negversion >= 0 )
    ret = map_find_same(fd,ROOT,subsystemname,
				  0,0,NULL_LOC,&tindex,&succ_loc,
				  &subsystem_loc);
  else
    ret = map_find_same(fd,NULL_LOC,subsystemname,
				  header.table_length,
				  header.table_used,header.table,
				  &tindex,&succ_loc,&subsystem_loc);

  if ( ret < 0 )
    {
      map_close_map(fd);
      return ret;
    }

  status = map_read(&subsystem,sizeof subsystem,fd,subsystem_loc,swapsubsystem);
  if ( status != MAP_OK )
    {
      map_close_map(fd);
      return status;
    }
  
  if ( subsystem.list >= 0 )
    ret = map_find_pred(fd,subsystem_loc+SUBSYSTEM_LIST_OFFSET,
			     item.name,0,0,NULL_LOC,&tindex,&succ_loc,&pred_loc);
  else
    ret = map_find_pred(fd,NULL_LOC,item.name,subsystem.table_length,
			     subsystem.table_used,subsystem.table,&tindex,
			     &succ_loc,&pred_loc);
  if ( ret < 0 )
    {
      map_close_map(fd);
      return ret;
    }

  if ( succ_loc != NULL_LOC )
    {
      status = map_read(&successor,sizeof successor,fd,succ_loc,swapitem);
      if ( status != MAP_OK )
	{
	  map_close_map(fd);
	  return status;
	}
      if ( strncmp(item.name, successor.name, MAP_NAMELENGTH) == 0 )
        {
	  map_warn(MAP_USER_WARN_NOREPEAT,
		   "map_add_item: item (%s) already exists for subsystem (%s)",
		   item.name,
		   subsystemname);
	  map_close_map(fd);
	  return MAP_USER_WARN_NOREPEAT;
        }
    }
  
  /* fill in entry next pointer */
  
  if ( subsystem.list >= 0 )
    /* version 1 */
    item.next = succ_loc;
  else
    /* version 2 */
    item.next = -VERSION;

  /* create initial time table */
  
  item.table_length = table_size;
  table = (table_t *) calloc(item.table_length,sizeof(table_t));
  ret = map_write(table,sizeof(table_t)*item.table_length,fd,&item.table,swaptablearray);
  free(table);
  if ( ret < 0 )
    {
      map_close_map(fd);
      return ret;
    }
  item.table_used = 0;
  
  /* insert entry */
  
  ret = map_write(&item,sizeof item,fd,&item_loc,swapitem);
  if ( ret < 0 )
    {
      map_close_map(fd);
      return ret;
    }
  
  if ( subsystem.list >= 0 )

    /* version 1 */

    {
      /* update pointer to new entry */
      
      status = map_overwrite(&item_loc,sizeof item_loc,fd,pred_loc,swappointer);
      if ( status != MAP_OK )
	{
	  map_close_map(fd);
	  return status;
	}
    }
  else

    /* version 2 */
    /* add name to table */
    
    {
      if ( subsystem.table_used < subsystem.table_length )
	nametable = (nametable_t *) calloc(subsystem.table_used+1,sizeof(nametable_t));
      else /* we will extend the table: need to allocate more space. CO 26-Jan-1995 */
	nametable = (nametable_t *) calloc(subsystem.table_length * TABLE_FACTOR,sizeof(nametable_t));

      if ( subsystem.table_used > 0 )
	{
	  status = map_read(nametable,subsystem.table_used*sizeof(nametable_t),
			    fd,subsystem.table,swapnametablearray);
	  if ( status != MAP_OK )
	    {
	      free(nametable);
	      map_close_map(fd);
	      return status;
	    }
	}
      if ( subsystem.table_used-tindex-1 > 0 )
	memmove(&nametable[tindex+2],&nametable[tindex+1],
		(subsystem.table_used-tindex-1)*sizeof(nametable_t));
      memcpy(nametable[tindex+1].name,item.name,MAP_NAMELENGTH);
      nametable[tindex+1].loc = item_loc;
      subsystem.table_used++;
      if ( subsystem.table_used <= subsystem.table_length )
	{
	  status = map_overwrite(nametable,
				 subsystem.table_used*sizeof(nametable_t),
				 fd,subsystem.table,
				 swapnametablearray);
	  if ( status != MAP_OK )
	    {
	      free(nametable);
	      map_close_map(fd);
	      return status;
	    }
	}
      else
	{
	  subsystem.table_length = subsystem.table_length * TABLE_FACTOR;
	  ret = map_write(nametable,
			  subsystem.table_length*sizeof(nametable_t),fd,
			  &subsystem.table,
			  swapnametablearray);
	  if ( ret < 0 )
	    {
	      free(nametable);
	      map_close_map(fd);
	      return ret;
	    }
	}
      free(nametable);
      status = map_overwrite(&subsystem,sizeof subsystem,fd,subsystem_loc,swapsubsystem);
      if ( status != MAP_OK )
	{
	  map_close_map(fd);
	  return status;
	}
    }
  
  /* close the file */
  
  return map_close_map(fd);
  
}
