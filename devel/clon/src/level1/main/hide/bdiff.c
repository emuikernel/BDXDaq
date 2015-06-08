
#ifndef VXWORKS

/* bdiff.c */

#include <stdlib.h>
#include <stdio.h>
#include <netinet/in.h>

FILE *f1;
FILE *f2;

int
main(int argc, char **argv)
{
  unsigned short a, b;
  int count = 0;
  int debug = 0;

  f1 = fopen(argv[1],"r");
  f2 = fopen(argv[2],"r");
  if(argc == 4) debug = 1;

  if(argc < 3)
  {
    printf("Usage: bdiff <arch1> <arch2> -d\n");
    printf("\t -d: Debug. Shows where is different.\n");
    exit(0);
  }

  while(!feof(f1))
  {
    fread(&a,sizeof(a),1,f1);
    fread(&b,sizeof(b),1,f2);
    if(debug)
    {
      if(a != b)
      {
        printf("[%x]: %s:0x%04x %s:0x%04x\n",(2*count),
                argv[1],htons(a),argv[2],htons(b));
      }
    }
    else
    {
      if(a!=b)
      {
        printf("Files are different.\n");
        exit(0);
      }
    }
    count ++;
  }
  if(!debug) printf("Files are equal.\n");

  printf ("Done..\n");
}

#else

void
bdiff_dummy()
{
  return;
}

#endif
