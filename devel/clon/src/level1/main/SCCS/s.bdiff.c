h15945
s 00012/00000/00054
d D 1.3 07/03/20 16:37:52 boiarino 4 3
c *** empty log message ***
e
s 00036/00026/00018
d D 1.2 04/02/03 20:57:25 boiarino 3 1
c debug prints
e
s 00000/00000/00000
d R 1.2 01/10/22 13:45:26 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 vxworks/level1/vmememget/bdiff.c
e
s 00044/00000/00000
d D 1.1 01/10/22 13:45:25 boiarino 1 0
c date and time created 01/10/22 13:45:25 by boiarino
e
u
U
f e 0
t
T
I 3

I 4
#ifndef VXWORKS

E 4
/* bdiff.c */

E 3
I 1
#include <stdlib.h>
#include <stdio.h>
#include <netinet/in.h>

FILE *f1;
FILE *f2;

D 3
int main(int argc, char **argv)
E 3
I 3
int
main(int argc, char **argv)
E 3
{
D 3
  unsigned short a,b;
E 3
I 3
  unsigned short a, b;
E 3
  int count = 0;
  int debug = 0;
D 3
  
E 3
I 3

E 3
  f1 = fopen(argv[1],"r");
  f2 = fopen(argv[2],"r");
D 3
  if (argc==4)
    debug = 1;
  
  if (argc < 3)
E 3
I 3
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
E 3
    {
D 3
      printf("Usage: bdiff <arch1> <arch2> -d\n");
      printf("\t -d: Debug. Shows where is different.\n");
      exit(0);
E 3
I 3
      if(a != b)
      {
        printf("[%x]: %s:0x%04x %s:0x%04x\n",(2*count),
                argv[1],htons(a),argv[2],htons(b));
      }
E 3
    }
D 3
 
  while (!feof(f1))
E 3
I 3
    else
E 3
    {
D 3
      count++;
      fread(&a,sizeof(a),1,f1);
      fread(&b,sizeof(b),1,f2);
      if(debug)
        if (a != b){
          printf("Offset %x: %s:0x%X %s:0x%X\n",(2*count),
                 argv[1],htons(a),argv[2],htons(b));}
      if(!debug)
        if (a!=b){
          printf("Files are different.\n");
          exit(0);}
      
E 3
I 3
      if(a!=b)
      {
        printf("Files are different.\n");
        exit(0);
      }
E 3
    }
D 3
  if (!debug)
    printf("Files are equal.\n");
E 3
I 3
    count ++;
  }
  if(!debug) printf("Files are equal.\n");

E 3
  printf ("Done..\n");
}
I 4

#else

void
bdiff_dummy()
{
  return;
}

#endif
E 4
E 1
