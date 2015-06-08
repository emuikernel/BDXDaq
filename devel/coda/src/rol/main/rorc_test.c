#include <stdio.h>

#ifdef LINUX
int
main()
{
  printf("Reading one event\n");
  rorcGetEvent();
  exit(0);
}
#endif
