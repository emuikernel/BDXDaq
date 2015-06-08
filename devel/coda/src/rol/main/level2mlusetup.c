
/*  level2mlusetup.c - program to load level2 MLU module */

#include <stdio.h>

main(int argc, char *argv[])
{
  char file_in[256];

  if(argc!=2)
  {
    printf("must be 1 argument: file name WITHOUT extension (presume '*.trg' in current directory)\n");
    exit(0);
  }

  strncpy(file_in, argv[1], 255);
  strcat(file_in, ".trg");
  printf("file_in >%s<\n",file_in);

  /*level2MLUSetup("/usr/clas/parms/trigger/config/cosmic/level2oppositesectors.trg");*/
  level2MLUSetup(file_in);
}
