
/*  level2lookupaker.c - program to generate level2 lookup tables */

#include <stdio.h>

main(int argc, char *argv[])
{
  char file_in[256], file_out[256];

  if(argc!=2)
  {
    printf("must be 1 argument: file name WITHOUT extension (presume '*.trg' in current directory)\n");
    exit(0);
  }

  strncpy(file_in, argv[1], 255);
  strcat(file_in, ".trg");
  strncpy(file_out, argv[1], 255);
  strcat(file_out, ".lut");
  printf("file_in >%s<   file_out >%s<\n",file_in,file_out);

  level2LUTSetup(file_in, file_out);
}
