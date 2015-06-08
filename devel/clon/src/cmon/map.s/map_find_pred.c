/* 
   map_find_pred.c
   Created           :  8-OCT-1992 by Thom Sulanke
*/
 
#include <stdio.h>                   /* I/O definitions                       */
#include <string.h>
#include <map_manager.h> 
#include <map_internal.h>
#include <stdlib.h>

/***********************************************************************/
/*                                                                     */
/*   MAP_FIND_PRED                                                     */
/*   -------------                                                     */
/*                                                                     */
/*         Created     :  8-OCT-1992    Author : Thom Sulanke          */
/*         Purpose     : find location of predeccessor to name in file */
/*                                                                     */
/***********************************************************************/
 
int map_find_pred( int fd, pointer_t pointer, const char name[],
		  int table_length, int table_used, pointer_t table_loc,
		  int *tindex, pointer_t *succ_loc, pointer_t *adr)
     
{
  pointer_t pred_loc, next_loc;
  name_t entry;
  int status;
  nametable_t *table;
  
  if ( table_loc == NULL_LOC )

    /* version 1 */

    {
      *tindex = -2;
      pred_loc = pointer;
      status = map_read(&next_loc,sizeof next_loc,fd,pointer,swappointer);
      if ( status != MAP_OK ) return status;
      *succ_loc = next_loc;
      
      while ( next_loc != NULL_LOC )
	{
	  status = map_read(&entry,sizeof entry,fd,next_loc,swapname);
	  if ( status != MAP_OK ) return status;
	  if ( strncmp(name,entry.name,MAP_NAMELENGTH) <= 0 )
	    break;
	  pred_loc = next_loc;
	  next_loc = entry.next;
	  *succ_loc = entry.next;
	}  /* While */
      
      *adr = pred_loc;
      return MAP_OK;
    }
  else

    /* version 2 */
    
    {
      *tindex = -1;
      *succ_loc = NULL_LOC;
      pred_loc = NULL_LOC;
      if ( table_used > 0 )
 	{
	  table = (nametable_t *) calloc(table_used,sizeof(nametable_t));
	  status = map_read(table,table_used*sizeof(nametable_t),fd,table_loc,swapnametablearray);
	  if ( status != MAP_OK )
	    {
	      free(table);
	      map_close_map(fd);
	      return status;
	    }
	  while ( *tindex < table_used-1 && strncmp(name,table[*tindex+1].name,MAP_NAMELENGTH) > 0 )
	    (*tindex)++;
	  if ( *tindex > -1 )
	    pred_loc = table[*tindex].loc;

	  if ( *tindex != table_used-1 )
	    *succ_loc = table[*tindex+1].loc;
	  free(table);
	}
      *adr = pred_loc;
      return MAP_OK;
    }

}
