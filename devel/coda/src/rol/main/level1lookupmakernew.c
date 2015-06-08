
/* level1lookupakernew.c - program to generate level1 lookup tables */

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

  level1LUTSetupNew(file_in);
}
