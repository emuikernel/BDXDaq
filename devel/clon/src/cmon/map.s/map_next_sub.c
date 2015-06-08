/* 
   map_next_sub.c
   Created           : 19-OCT-1992 by Thom Sulanke
*/
 
#include <stdio.h>                   /* I/O definitions                      */
#include <string.h>
#include <stdlib.h>
#include "map_manager.h"
#include "map_internal.h"

/***********************************************************************/
/*                                                                     */
/*   MAP_NEXT_SUB                                                      */
/*   ------------                                                      */
/*                                                                     */
/*         Created     : 19-OCT-1992    Author : Thom Sulanke          */
/*         Purpose     : find the next subsystem after a given         */
/*                             subsystem.  If subsystemname is '*'     */
/*                             return first subsystem                  */
/*                                                                     */
/***********************************************************************/
 
int map_next_sub( const char filename[], const char subsystemname[], 
		 char nextsubsystemname[], int *nitem)
     
{
  int fd ;
  pointer_t pred_loc, succ_loc, header_loc;
  subsystem_t nextsubsystem;
  int status, tindex;
  subtableheader_t header;
  nametable_t *table;
  int ret;

  /* set default for early return */
  
  map_fillname(nextsubsystemname, "*");
  *nitem = 0;
  
  /* open file for read */
  
  fd = map_open_ro(filename);
  if ( fd < 0 )
    {
      map_syserror(fd,
		   "can't open TheMap (%s) for finding next subsystem",
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
  
  if ( header.negversion >= 0 )
    
    /* version 1 */
    
    {
      /* find location in list of entry */
      
      if ( strncmp(subsystemname, "*", MAP_NAMELENGTH) == 0 )
	{
	  pred_loc = ROOT;
	  status = map_read(&succ_loc, sizeof succ_loc, fd, pred_loc, swappointer);
	  if ( status != MAP_OK ) 
	    {
	      map_close_map(fd);
	      return status;
	    }
	  ret = MAP_OK;
	}
      else
	{
	  ret = map_find_same(fd, ROOT, subsystemname, 
				   0, 0, NULL_LOC, &tindex, &succ_loc,
				   &pred_loc);
	}
      if ( ret < 0 )
	{
	  if ( ret == MAP_USER_WARN_NOMATCH ) 
	    map_warn(ret,
		     "no such subsystem %s",
		     subsystemname);
	  map_close_map(fd);
	  return ret;
	}
      else
	{
	  if ( succ_loc != NULL_LOC )
	    {
	      status = map_read(&nextsubsystem, sizeof nextsubsystem, fd, 
				succ_loc,swapsubsystem);
	      if ( status != MAP_OK ) 
		{
		  map_close_map(fd);
		  return status;
		}
	      map_fillname(nextsubsystemname, nextsubsystem.name);

	      if ( nextsubsystem.table == NULL_LOC )
		/* version 1 */
		{		
		  *nitem = 0;
		  pred_loc = nextsubsystem.list;
		  while ( pred_loc != NULL_LOC )
		    {
		      (*nitem)++;
		      status = map_read(&pred_loc,sizeof pred_loc,fd,pred_loc,swappointer);
		      if ( status != MAP_OK ) 
			{
			  map_close_map(fd);
			  return status;
			}
		    }
		}
	      else
		/* version 2 */
		*nitem = nextsubsystem.table_used;
	    }
	}
    }
  else
    
    /* version 2 */
    {
      if ( header.table_used > 0 )
	{
	  table = (nametable_t *) calloc(header.table_used,
					 sizeof(nametable_t));
	  status = map_read(table,header.table_used*sizeof(nametable_t),
			    fd,header.table,swapnametablearray);
	  if ( status != MAP_OK )
	    {
	      free(table);
	      map_close_map(fd);
	      return status;
	    }

	  tindex = 0;  /* tindex will be table index of current subsystem */
	  if ( strncmp(subsystemname, "*", MAP_NAMELENGTH) != 0 )
	    {
	      while ( tindex < header.table_used && 
		     strncmp(subsystemname,table[tindex].name,MAP_NAMELENGTH) >= 0 )
		(tindex)++;
	    }

	  if ( tindex < header.table_used )
	    {
	      map_fillname(nextsubsystemname, table[tindex].name);
	      status = map_read(&nextsubsystem, sizeof nextsubsystem, 
				fd, table[tindex].loc,swapsubsystem);
	      if ( status != MAP_OK ) 
		{
		  free(table);
		  map_close_map(fd);
		  return status;
		}
	      *nitem = nextsubsystem.table_used;
	    }
	  free(table);
	}
      
    }
  
  return map_close_map(fd);
  
}
