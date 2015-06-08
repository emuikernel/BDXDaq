/* 
        map_write.c
         Created           :  9-OCT-1992 by Thom Sulanke
*/
 
#include <stdio.h>                   /* I/O definitions                       */
#include <stdlib.h>
#include <unistd.h>
#include <map_manager.h>
#include <map_internal.h>


/***********************************************************************/
/*                                                                     */
/*   MAP_WRITE                                                         */
/*   ---------                                                         */
/*                                                                     */
/*         Created     :  9-OCT-1992    Author : Thom Sulanke          */
/*         Purpose     : add to end of file                            */
/*                                                                     */
/***********************************************************************/
 
int   map_write(const void *chunck, size_t nbytes, int fd, pointer_t *adr, map_conversion_t conversionType)
 
{
 
  pointer_t end_before_write;
  void *swappedData = NULL;

/* set file position to end of file */
  
  if ( (end_before_write = (int)lseek(fd, 0, SEEK_END)) == -1 )
    {
      map_syserror(MAP_SYSTEM_ERROR_IO,
		   "error positioning to write to end of TheMap");
      map_close_map(fd);
      return MAP_SYSTEM_ERROR_IO;
    }

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
  
  if ( write(fd, chunck, nbytes) == -1 )
    {
      map_syserror(MAP_SYSTEM_ERROR_IO,
		   "error writing to end of TheMap");
      map_close_map(fd);
      return MAP_SYSTEM_ERROR_IO;
    }

  if (swappedData != NULL)
    free(swappedData);
  
  *adr = end_before_write;
  return MAP_OK;
    
}
