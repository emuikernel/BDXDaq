h06100
s 00000/00000/00000
d R 1.2 03/05/23 12:18:03 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 clas/cmon/map.s/map_time_pred.c
e
s 00105/00000/00000
d D 1.1 03/05/23 12:18:02 boiarino 1 0
c date and time created 03/05/23 12:18:02 by boiarino
e
u
U
f e 0
t
T
I 1
/* 
        map_time_pred.c
         Created           : 16-NOV-1992 by Thom Sulanke
*/
 
#include <stdio.h>                   /* I/O definitions                       */
#include <stdlib.h>
#include "map_manager.h"
#include "map_internal.h"

/***********************************************************************/
/*                                                                     */
/*   MAP_TIME_PRED                                                     */
/*   -------------                                                     */
/*                                                                     */
/*         Created     : 16-NOV-1992    Author : Thom Sulanke          */
/*         Purpose     : find predessor (in list, successor in time)   */
/*                          of array of values, also return index of   */
/*                          predessor. An index of -1 indicates no     */
/*                          predessor. An index of -2 indicates no     */
/*                          table.                                     */
/*                                                                     */
/***********************************************************************/
 
int map_time_pred( int fd, pointer_t pointer, item_t item, int atime, 
		  int *tindex, pointer_t *suc_loc, int *suc_time,
		  pointer_t *adr)
 
{
    pointer_t pred_loc, next_loc;
    arrayheader_t entry;
    int status;
    table_t *table;
    
    if ( item.list < 0 )

/* use table */

      {
	if ( item.table_used == 0 )
	  *tindex = -1;
	else
	  {
	    table = (table_t *) calloc(item.table_used,sizeof(table_t));
	    status = map_read(table,item.table_used*sizeof(table_t),fd,item.table,swaptablearray);
	    if ( status != MAP_OK )
	      {
		free(table);
		map_close_map(fd);
		return status;
	      }
	    *tindex = -1;
	    while ( *tindex < (item.table_used)-1 && table[*tindex+1].time > atime )
	      (*tindex)++;
	  }

	if ( *tindex == -1 )
	  pred_loc = pointer;	
	else
	  pred_loc = table[*tindex].loc;
	
	if ( *tindex == item.table_used-1 )
	  {
	    *suc_loc = NULL_LOC;
	    *suc_time = 0;
	  }
	else
	  {
	    *suc_loc = table[*tindex+1].loc;
	    *suc_time = table[*tindex+1].time;
	  }
	if ( item.table_used != 0 )
	  free(table);
      }

    else

/* use linked list */

      {
	*tindex = -2;
	pred_loc = pointer;
	next_loc = item.list;
	
	while ( next_loc != NULL_LOC )
	  {
	    status = map_read(&entry,sizeof entry,fd,next_loc,swaparrayheader);
	    if ( status != MAP_OK )
	      {
		map_close_map(fd);
		return status;
	      }
	    if ( entry.time <= atime )
	      break;
	    pred_loc = next_loc;
	    next_loc = entry.next;
	  }  /* While */
	*suc_loc = next_loc;
	*suc_time = entry.time;
      }
   
    *adr = pred_loc;
    return MAP_OK;
    
  }
E 1
