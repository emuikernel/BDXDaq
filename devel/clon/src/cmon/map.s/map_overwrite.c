/* 
        map_overwrite.c
         Created           :  9-OCT-1992 by Thom Sulanke
*/
 
#include <stdio.h>                   /* I/O definitions                       */
#include <stdlib.h>
#include <unistd.h>
#include <map_manager.h>
#include <map_internal.h>

/***********************************************************************/
/*                                                                     */
/*   MAP_OVERWRITE                                                     */
/*   ---------                                                         */
/*                                                                     */
/*         Created     :  9-OCT-1992    Author : Thom Sulanke          */
/*         Purpose     : overwrite portion of file                     */
/*                                                                     */
/***********************************************************************/
 
int map_overwrite(const void *chunck, size_t nbytes, int fd, pointer_t offset, map_conversion_t conversionType)
 
{
  void *swappedData = NULL;
 
  /* set file position to offset */
 
    if ( lseek(fd, offset, SEEK_SET) == -1 )
    {
        map_syserror(MAP_SYSTEM_ERROR_IO,
		     "error positioning to overwrite in TheMap");
        map_close_map(fd);
        return MAP_SYSTEM_ERROR_IO;
    }

    /* if needed swap the data endianess */

    if (map_needEndianConversion)
      {
	swappedData = malloc(nbytes);
	
	memcpy(swappedData,chunck,nbytes);
	
	if (map_endianConvert(swappedData,nbytes,conversionType)!=MAP_OK)
	  {
	    map_syserror(MAP_SYSTEM_ERROR_IO,
			 "error during TheMap endian conversion");
	    map_close_map(fd);
	    return MAP_SYSTEM_ERROR_IO;
	  }
	
	chunck = swappedData;
      }

    /* do actual write */
 
    if ( write(fd, chunck, nbytes) != nbytes )
    {
        map_syserror(MAP_SYSTEM_ERROR_IO,
		     "error overwriting in TheMap");
        map_close_map(fd);
        return MAP_SYSTEM_ERROR_IO;
    }

    if (swappedData != NULL)
      free(swappedData);

    return MAP_OK;
 
}
