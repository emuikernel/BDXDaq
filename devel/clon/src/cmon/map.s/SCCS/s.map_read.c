h63223
s 00000/00000/00000
d R 1.2 03/05/23 12:18:03 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 clas/cmon/map.s/map_read.c
e
s 00058/00000/00000
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
        map_read.c
         Created           :  9-OCT-1992 by Thom Sulanke
*/
 
#include <stdio.h>                   /* I/O definitions                       */
#include <stdlib.h>
#include <unistd.h>

#include "map_manager.h"
#include "map_internal.h"

/***********************************************************************/
/*                                                                     */
/*   MAP_READ                                                          */
/*   --------                                                          */
/*                                                                     */
/*         Created     :  9-OCT-1992    Author : Thom Sulanke          */
/*         Purpose     : read a portion of the map                     */
/*                                                                     */
/***********************************************************************/
 
int map_read(void *chunck, size_t nbytes, int fd, pointer_t offset, map_conversion_t conversionType)
 
{
 
/* set file position */
 
    if ( lseek(fd, offset, SEEK_SET) == -1 )
    {
        map_syserror(MAP_SYSTEM_ERROR_IO,
		     "error positioning to read TheMap");
        map_close_map(fd);
        return MAP_SYSTEM_ERROR_IO;
    }
 
/* do actual read */
 
    if ( read(fd, chunck, nbytes) == -1 )
    {
        map_syserror(MAP_SYSTEM_ERROR_IO,
		     "error reading TheMap");
        map_close_map(fd);
        return MAP_SYSTEM_ERROR_IO;
    }

    if (map_endianConvert(chunck,nbytes,conversionType) != MAP_OK)
      {
        map_syserror(MAP_SYSTEM_ERROR_IO,
		     "error during TheMap endian conversion");
        map_close_map(fd);
        return MAP_SYSTEM_ERROR_IO;
      }

    return MAP_OK;
}

/* end file */
E 1
