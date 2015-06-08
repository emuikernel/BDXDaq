#include "vmalloc.h"
main()
{
  int ix;
  char *p;
  for (ix=0;ix<10000000;ix++) {
	p = malloc(1000);
	free(p);
  }
}
