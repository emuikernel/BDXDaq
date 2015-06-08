h10149
s 00000/00000/00000
d R 1.2 03/05/23 12:18:01 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 clas/cmon/map.s/map_find_same.c
e
s 00104/00000/00000
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
   map_find_same.c
   Created           : 16-OCT-1992 by Thom Sulanke
*/

static const char sccsid[] = "@(#)"__FILE__"\t4.2\tCreated 5/5/97 17:47:57, \tcompiled "__DATE__;
 
#include <stdio.h>                   /* I/O definitions                      */
#include <stdlib.h>
#include <string.h>
#include "map_manager.h"
#include "map_internal.h"

/***********************************************************************/
/*                                                                     */
/*   MAP_FIND_SAME                                                     */
/*   -------------                                                     */
/*                                                                     */
/*         Created     : 16-OCT-1992    Author : Thom Sulanke          */
/*         Purpose     : find location of name in file.  If name is    */
/*             		 not in file, return MAP_USER_WARN_NOMATCH     */
/*                                                                     */
/***********************************************************************/
 
int map_find_same(int fd, pointer_t pointer, const char name[], 
		  int table_length, int table_used, 
		  pointer_t table_loc, int *tindex, pointer_t *succ_loc,
		  pointer_t *adr)
     
{
  pointer_t same_loc;
  name_t entry;
  int status;
  nametable_t *table;
  
  if ( table_loc == NULL_LOC )

    /* version 1 */
    
    {
      *tindex = -2;
      *succ_loc = NULL_LOC;
      status = map_read(&same_loc,sizeof same_loc,fd,pointer,swappointer);
      if ( status != MAP_OK ) return status;
      while ( same_loc != NULL_LOC )
	{
	  status = map_read(&entry,sizeof entry,fd,same_loc,swapname);
	  if ( status != MAP_OK ) return status;
	  *succ_loc = entry.next;
	  if ( strncmp(name,entry.name,MAP_NAMELENGTH) == 0 )
	    break;
	  same_loc = entry.next;
	}  /* While */
      
    }
  else

    /* version 2 */
    
    { 
      *tindex = -1;
      *succ_loc = NULL_LOC;
      same_loc  = NULL_LOC;
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
	  *tindex = 0;
	  while ( *tindex < table_used && strncmp(name,table[*tindex].name,MAP_NAMELENGTH) != 0 )
	    (*tindex)++;
	  if ( *tindex < table_used )
	    same_loc = table[*tindex].loc;

	  if ( *tindex+1 < table_used )
	    *succ_loc = table[*tindex+1].loc;

	  if ( *tindex == table_used )
	    *tindex = -1;

	  free(table);
	}
      
    }

  if ( same_loc == NULL_LOC )
    return MAP_USER_WARN_NOMATCH;
  else{
    *adr = same_loc;
    return MAP_OK;
  }

}






E 1
