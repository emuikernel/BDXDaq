h56684
s 00001/00000/00030
d D 1.2 03/04/17 16:45:23 boiarino 3 1
c *** empty log message ***
e
s 00000/00000/00000
d R 1.2 70/01/01 00:00:02 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 clas/bosio/bosio.s/bosmem.c
e
s 00030/00000/00000
d D 1.1 00/08/10 11:10:03 boiarino 1 0
c date and time created 00/08/10 11:10:03 by boiarino
e
u
U
f e 0
t
T
I 1

/* bosmem.c - memory alloc functions */

I 3
#include <stdio.h>
E 3
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
E 1
