#include <stdio.h>

void options(char *processName);

void options(char *processName)
{
  fprintf(stderr,"\nUsage: %s -m<map>\n", processName);
  fprintf(stderr,"\t- wrapper for map_add_item call must use all options ex:\n");
  fprintf(stderr,"\t\t add_map_item -mtest -stestsub -itestitem -l10 -a0\n");
  fprintf(stderr,"Options are:\n");
  fprintf(stderr,"\t[-m<mapfile>]\tMap file to add to\n");
  fprintf(stderr,"\t[-s<subsystem>]\tSubsystem to add to(created if necc.)\n");
  fprintf(stderr,"\t[-i<item>]\tItem to add\n");
  fprintf(stderr,"\t[-l<length>]\tLength of Item (int)\n");
  fprintf(stderr,"\t[-a<arraytype>]\t-a0 = int, -a1 = float, -a2 = string\n");
  exit(0);
}

void main(int argc,char*argv[])
{
  int i, length, arraytype;
  char *map, *subsystem, *item;
  

  if(argc != 6)options(argv[0]);
  for(i=1; i < argc; ++i){
    if( argv[i][0] == '-'){
      switch(argv[i][1]){
      case 'm':
	map = &argv[i][2];
	break;
      case 's':
	subsystem = &argv[i][2];
	break;
      case 'i':
	item = &argv[i][2];
	break;
      case 'l':
	length = atoi(&argv[i][2]);
	break;
      case 'a':
	arraytype = atoi(&argv[i][2]);
	if (arraytype < 0 || arraytype > 2) options(argv[0]);
	break;
      default:
	options(argv[0]);
	break;
      }
    }
  }

  map_add_item(map, subsystem, item, length, arraytype);
  exit(1);
}
