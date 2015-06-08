/*
 * map_swapEndian.c
*/

#include <stdio.h>
#include <assert.h>

#include "map_manager.h"
#include "map_internal.h"

#define SWAPUINT32(w) ((w) = (((w)&0xff)<<24)|(((w)&0xff00)<<8)|(((w)&0xff0000)>>8)|(((w)&0xff000000)>>24))

#define SWAPINT(p) SWAPUINT32(p)
#define SWAPPTR(p) SWAPUINT32(p)

int map_needEndianConversion = 0;

int map_endianConvert(void *chunck, size_t nbytes, map_conversion_t conversionType)
{
  /* use the root pointer to determine the endianness of the Map file */

  if (conversionType == swaprootpointer)
    {
      unsigned int ptr,swappedptr;
      int diff;
      unsigned int udiff;

      ptr = *(unsigned int *)chunck;

      swappedptr = ptr;
      SWAPPTR(swappedptr);

      diff = ptr - swappedptr;
      udiff = ptr - swappedptr;

      if (ptr < swappedptr)
	map_needEndianConversion = 0;
      else
	map_needEndianConversion = 1;

#ifdef UNDEF
      fprintf(stderr,"ptr: 0x%x, swappedptr: 0x%x, need conversion: %d, diff: %d, udiff: %d\n",
	      ptr,swappedptr,map_needEndianConversion,diff,udiff);
#endif

      conversionType = swappointer;
    }

  if (!map_needEndianConversion)
    return MAP_OK;

    switch (conversionType)
      {
      default:
	fprintf(stderr,"map_endianConvert: Unknown conversion type [%d]\n",conversionType);
        return MAP_SYSTEM_ERROR_IO;

      case swaperror:
	fprintf(stderr,"map_endianConvert: Fatal error during endian conversion type [%d]\n",conversionType);
        return MAP_SYSTEM_ERROR_IO;

      case swappointer:
	{
	  pointer_t *dst;
	  assert(nbytes == sizeof(*dst));
	  assert(nbytes == 4);
	  dst = (void*) chunck;
	  SWAPPTR(*dst);
	}
	break;

      case swapsubtableheader:
	{
	  subtableheader_t *dst;
	  assert(nbytes == sizeof(*dst));
	  dst = (void*)chunck;

	  SWAPINT(dst->negversion);
	  SWAPINT(dst->nullname);
	  SWAPINT(dst->table_length);
	  SWAPINT(dst->table_used);
	  SWAPPTR(dst->table);
	}
	break;

      case swapname:
	{
	  name_t *dst;
	  assert(nbytes == sizeof(*dst));
	  dst = (void*)chunck;

	  SWAPPTR(dst->next);
	}
	break;

      case swaparrayheader:
	{
	  arrayheader_t *dst;
	  assert(nbytes == sizeof(*dst));
	  dst = (void*)chunck;

	  SWAPPTR(dst->next);
	  SWAPINT(dst->time);
	  SWAPINT(dst->length);
	  SWAPINT(dst->spare[0]);
	}
	break;

      case swapnametablearray:
	{
	  int i,count,leftover;
	  nametable_t *dst;
	  count = nbytes/sizeof(*dst);
	  leftover = nbytes - count*sizeof(*dst);
	  assert(leftover == 0);
	  dst = (void*)chunck;

	  for (i=0; i<count;  i++)
	    SWAPPTR(dst[i].loc);
	}
	break;

      case swaptablearray:
	{
	  int i,count,leftover;
	  table_t *dst;
	  count = nbytes/sizeof(*dst);
	  leftover = nbytes - count*sizeof(*dst);
	  assert(leftover == 0);
	  dst = (void*)chunck;

	  for (i=0; i<count;  i++)
	    {
	      SWAPINT(dst[i].time);
	      SWAPPTR(dst[i].loc);
	    }
	}
	break;

      case swapchararray:
	/* do nothing */
	break;

      case swapintarray:
      case swapfloatarray:
	{
	  int i,count,leftover;
	  int *dst;
	  count = nbytes/sizeof(*dst);
	  leftover = nbytes - count*sizeof(*dst);
	  assert(leftover == 0);
	  dst = (void*)chunck;

	  for (i=0; i<count;  i++)
	    SWAPINT(dst[i]);
	}
	break;

      case swapsubsystem:
	{
	  subsystem_t *dst;
	  assert(nbytes == sizeof(*dst));
	  dst = (void*)chunck;

	  SWAPPTR(dst->next);
	  SWAPPTR(dst->list);
	  SWAPPTR(dst->table);
	  SWAPINT(dst->table_length);
	  SWAPINT(dst->table_used);
	}
	break;

      case swapitem:
	{
	  item_t *dst;
	  assert(nbytes == sizeof(*dst));
	  dst = (void*)chunck;

	  SWAPPTR(dst->next);
	  SWAPPTR(dst->list);
	  SWAPINT(dst->length);
	  SWAPINT(dst->type);
	  SWAPPTR(dst->table);
	  SWAPINT(dst->table_length);
	  SWAPINT(dst->table_used);
	  SWAPINT(dst->spare[0]);
	}
	break;

	
      }

    return MAP_OK;
}
 
/* end file */
