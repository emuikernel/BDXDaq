/* 
        map_next_item.c
         Created           : 11-NOV-1992 by Thom Sulanke
*/
 
#include <stdio.h>                   /* I/O definitions                       */
#include <string.h>
#include <stdlib.h>
#include "map_manager.h"
#include "map_internal.h"

/***********************************************************************/
/*                                                                     */
/*   MAP_NEXT_ITEM                                                     */
/*   -------------                                                     */
/*                                                                     */
/*         Created     : 11-NOV-1992    Author : Thom Sulanke          */
/*         Purpose     : find the next item after a given              */
/*                             item.  If itemname is '*' return        */
/*                             first item                              */
/*                                                                     */
/***********************************************************************/
 
int map_next_item( const char filename[], const char subsystemname[], 
		  const char itemname[], char nextitemname[], int *length, 
		  int *type, int *narray)
     
{
  int fd ;
  pointer_t pred_loc, succ_loc, subsystem_loc, header_loc;
  item_t nextitem;
  subsystem_t subsystem;
  int status, tindex;
  subtableheader_t header;
  nametable_t *table;
  int ret;

  /* set default for early return */
  
  map_fillname(nextitemname,  "*");
  *length = 0;
  *type = 0;
  *narray = 0;
  
  /* open file for read */
  
  fd = map_open_ro(filename);
  if ( fd < 0 )
    {
      map_syserror(fd,
		   "can't open TheMap (%s) for finding next item",
		   filename);
      return fd;
    }
  
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
  
  /* find location of subsystem */
      
  if ( header.negversion >= 0 )
    
    /* version 1 header */
    
    ret = map_find_same(fd, ROOT, subsystemname, 
			0, 0, NULL_LOC, &tindex, &succ_loc,
			&subsystem_loc);
  
  else
    
    /* version 2 header */
    
    ret = map_find_same(fd, NULL_LOC, subsystemname, 
			header.table_length, header.table_used, 
			header.table, &tindex, &succ_loc,
			&subsystem_loc);

  if ( ret < 0 )
    {
      if ( ret == MAP_USER_WARN_NOMATCH ) 
	map_warn(ret,
		 "no such subsystem %s",
		 subsystemname);
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
    
    /* version 1 subsystem */
    
    {
      
      /* find location in list of entry */
      
      if ( strncmp(itemname, "*", MAP_NAMELENGTH) == 0 )
	{
	  pred_loc = subsystem_loc + SUBSYSTEM_LIST_OFFSET;
	  succ_loc = subsystem.list;
	  if ( status != MAP_OK ) 
	    {
	      map_close_map(fd);
	      return status;
	    }
	  ret = 0;
	}
      else
	{
	  ret = map_find_same(fd, subsystem_loc+SUBSYSTEM_LIST_OFFSET, 
				   itemname, 
				   0, 0, NULL_LOC, &tindex, &succ_loc,
				   &pred_loc);
	}
      if ( ret < 0 )
	{
	  if ( ret == MAP_USER_WARN_NOMATCH ) 
	    map_warn(ret,
		     "no such item (%s) in subsystem (%s)", 
		     itemname,
		     subsystemname);
	  map_close_map(fd);
	  return ret;
	}
      else
	{
	  if ( succ_loc != NULL_LOC )
	    {
	      status = map_read(&nextitem, sizeof nextitem, fd, succ_loc, swapitem);
	      if ( status != MAP_OK ) 
		{
		  map_close_map(fd);
		  return status;
		}
	      map_fillname(nextitemname, nextitem.name);
	      *length = nextitem.length;
	      *type = nextitem.type;
	      if ( nextitem.table_length == 0 )
		
		/* version 1 item */
		/* loop through linked list */
		
		{
		  *narray = 0;
		  pred_loc = nextitem.list;
		  while ( pred_loc != NULL_LOC )
		    {
		      (*narray)++;
		      status = map_read(&pred_loc,sizeof pred_loc,fd,pred_loc, swappointer);
		      if ( status != MAP_OK ) 
			{
			  map_close_map(fd);
			  return status;
			}
		    }
		}
	      else
		
		/* version 2 item */
		/* if table use its size */
		
		*narray = nextitem.table_used;
	    }
	}
    }
  else
    
    /* version 2 subsystem */

    if ( subsystem.table_used > 0 )
      {
	table = (nametable_t *) calloc(subsystem.table_used,
				       sizeof(nametable_t));
	status = map_read(table,subsystem.table_used*sizeof(nametable_t),
			  fd,subsystem.table, swapnametablearray);
	if ( status != MAP_OK )
	  {
	    free(table);
	    map_close_map(fd);
	    return status;
	  }
	tindex = 0;  /* tindex will be table index of current item */
	if ( strncmp(itemname, "*", MAP_NAMELENGTH) != 0 )
	  {
	    while ( tindex < subsystem.table_used && 
		   strncmp(itemname,table[tindex].name,MAP_NAMELENGTH) >= 0 )
	      (tindex)++;
	  }
	if ( tindex < subsystem.table_used )
	  {
	    map_fillname(nextitemname, table[tindex].name);
	    status = map_read(&nextitem, sizeof nextitem, fd,
			      table[tindex].loc, swapitem);
	    if ( status != MAP_OK ) 
	      {
		free(table);
		map_close_map(fd);
		return status;
	      }
	    *length = nextitem.length;
	    *type = nextitem.type;
	    *narray = nextitem.table_used;
	  }
	free(table);
      }
  
  return map_close_map(fd);
  
}







