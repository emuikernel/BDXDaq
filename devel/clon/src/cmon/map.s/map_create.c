/* 
   MAP_CREATE.C
   Created           :  1-OCT-1992 by Thom Sulanke
*/
 
#include <stdio.h>                   /* I/O definitions                      */
#ifdef vaxc
#include <file.h>                   /* file and descriptor definitions      */
#else
#include <fcntl.h>
#endif
#include <stdlib.h>
#include <map_manager.h>
#include <map_internal.h>
#include <unistd.h>
#define PERMS 0666   /* RW for owner, group, others */
 

/***********************************************************************/
/*                                                                     */
/*   MAP_CREATE                                                        */
/*   ----------                                                        */
/*                                                                     */
/*         Created     :  1-OCT-1992    Author : Thom Sulanke          */
/*         Purpose     :  create an empty map database                 */
/*                                                                     */
/***********************************************************************/
 
int map_create( const char filename[] )
     
{
  int fd;
  pointer_t head;
  int status;
  nametable_t *table;
  subtableheader_t header;
  int ret;

  map_needEndianConversion = 0; /* create map files in native-endian mode */
  
  /* check if file already exists */
  
  fd = open(filename, O_RDONLY);
  if ( fd != -1 )
    {
      map_warn(MAP_USER_WARN_NOREPEAT,
	       "map_create: TheMap (%s) already exits. NOT overwriting.",
	       filename);
      map_close_map(fd);
      return MAP_USER_WARN_NOREPEAT;
    }
  
  /* create new file */
  
  fd = creat(filename, PERMS);
  
  if ( fd == -1 )
    {
      map_syserror(MAP_SYSTEM_ERROR_OPEN,
		   "map_create: can't create TheMap (%s)",filename);
      return MAP_SYSTEM_ERROR_OPEN;
    }
  
  status = map_close_map(fd);
  if ( status != MAP_OK ) return status;
  
  /* save space in file for pointer to subsystem name table header */

  head = NULL_LOC;

  fd = map_open_rw(filename);

  if ( write(fd, &head, sizeof head) != sizeof head )
    {
      map_syserror(MAP_SYSTEM_ERROR_OPEN,
		   "map_create: can't do first write to TheMap");
      map_close_map(fd);
      return MAP_SYSTEM_ERROR_IO;
    }

  /* create subsystem table header */
  
  header.negversion = -VERSION;
  header.nullname = 0;
  header.table_length = INIT_NAMETABLE_SIZE;
  header.table_used = 0;

  /* add initial subsystem name table */

  table = (nametable_t *) calloc(header.table_length,sizeof(nametable_t));
  ret = map_write(table,header.table_length*sizeof(nametable_t),fd,&header.table,swapnametablearray);
  free(table);
  if ( ret < 0 )
	{
	  map_close_map(fd);
	  return ret;
	}
      
  /* add subsystem table header */
  
  ret = map_write(&header,sizeof header,fd,&head,swapsubtableheader);
  if ( ret < 0 )
    {
      map_close_map(fd);
      return ret;
    }

  /* rewite head */

  status = map_overwrite(&head,sizeof head,fd,ROOT,swaprootpointer);
  if ( status < 0 )
    {
      map_close_map(fd);
      return status;
    }
  
  return map_close_map(fd);
}
