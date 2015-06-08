#include <stdio.h>
#include <stdlib.h>

int
main()
{
  unsigned int vmeaddr;
  char *arg2 = "0xfa100000";

  vmeaddr = strtoll(arg2, (char **)NULL, 0);

  printf(">%s< 0x%08x\n",arg2,vmeaddr);

  exit(0);
}
