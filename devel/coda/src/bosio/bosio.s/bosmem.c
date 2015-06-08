
/* bosmem.c - memory alloc functions */

#include <stdio.h>
#include <stdlib.h>

char *
MALLOC(int size)
{
  char *ptr;
  ptr = malloc(size);
  /*printf("malloc(%08x) %d bytes\n",ptr,size);*/
  return(ptr);
}


void
FREE(void *ptr)
{
  if(ptr == NULL)
  {
    printf("ERROR in FREE, ptr(%08x) == NULL\n",ptr);
  }
  else
  {
    free(ptr);
    /*printf("  free(%08x)\n",ptr);*/
  }

  return;
}
