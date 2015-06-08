#include <stdio.h>

void options(char *processName);

void options(char *processName)
{
  fprintf(stderr,"\nUsage: %s -m<map>\n", processName);
  fprintf(stderr,"\t- wrapper for map_put_float call must use all options\n");
  fprintf(stderr,"\t\t takes into stdin a list of floats separated by whitespace ex:\n");
  fprintf(stderr,"\t\t example:\n");
  fprintf(stderr,"\t\t put_map_float -mTEST.map -stestsub -itestitem -t10 < floatfile\n");
  fprintf(stderr,"Options are:\n");
  fprintf(stderr,"\t[-m<mapfile>]\tMap file to add to\n");
  fprintf(stderr,"\t[-s<subsystem>]\tSubsystem to add to(created if necc.)\n");
  fprintf(stderr,"\t[-i<item>]\tItem to add\n");
  fprintf(stderr,"\t[-t<time>]\ttime/run number to place array at\n");
  exit(0);
}

void main(int argc,char*argv[])
{
  int i, length, time;
  float floatarray[100000];
  char *map, *subsystem, *item, *filename;
  FILE *infile = NULL;

  if(argc != 5)options(argv[0]);
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
      case 't':
	time = atoi(&argv[i][2]);
	break;
      default:
	options(argv[0]);
	break;
      }
    }
  }

  /* read infile */
 
  for(i=0; fscanf(stdin,"%g",&floatarray[i]) != EOF; i++);


  length = i++;
  map_put_float(map, subsystem, item, length, floatarray, time);
  exit(1);
}


