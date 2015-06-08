#include <stdio.h>

void options(char *processName);

void options(char *processName)
{
  fprintf(stderr,"\nUsage: %s <map>\n", processName);
  fprintf(stderr,"\t- wrapper for map_create call\n");
  exit(0);
}

void main(int argc,char*argv[])
{
  int i;
  char *map;

  if(argc != 2)options(argv[0]);
  map = &argv[1][0];

  map_create(map);
  exit(1);
}
