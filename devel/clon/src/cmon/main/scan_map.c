#include <stdio.h>

void options(char *processName);
int print_timeline(const char map[], const char subsystem[], const char item[], int length, int type);

void options(char *processName)
{
  fprintf(stderr,"\nUsage: %s [-t] [-h] <map>\n", processName);
  fprintf(stderr,"\t- lists all items and subsystems in map file\n");
  fprintf(stderr,"\t\t-t\tDraw a timeline for each item\n");
  fprintf(stderr,"\t\t-h\tPrint this message\n");
  exit(0);
}

void main(int argc,char*argv[])
{
  int i, nmaps,nitems, length, type, narray, timeline=0;
  char *map;
  char nextsubsystem[16], subsystem[16], nextitem[16], item[16];
  char array_type[10];

  if(argc == 1)
    options(argv[0]);
  for(i=1; i< argc; ++i){
    if( argv[i][0] == '-'){
      switch(argv[i][1]){
      case 'h':
	options(argv[0]);
	break;
      case 't':
        timeline = 1; /*draw a timeline*/
	map_log_mess(0, 0); /*turn off all error messages*/
	break;
      default:
	options(argv[0]);
	break;
      }
    }
  }


  for (nmaps = 1;nmaps < argc; ++nmaps) {
    map = &argv[nmaps][0];
    if (map[0] != '-') {
      int index = 0;
      
      fprintf(stdout,"\n");
      
      fprintf(stdout,"Map: %s\n",map);
      
      map_next_sub(map, "*", nextsubsystem, &nitems);
      while(nextsubsystem[0] != '*'){
	fprintf(stdout,"\tSubsystem: %s,\tnitems: %d\n", nextsubsystem, nitems);  
	sprintf(item, "*");
	for(i=0; i< nitems; i++){
	  map_next_item(map, nextsubsystem, item, nextitem, &length, &type, &narray);
	  switch(type){
	  case 0:
	    sprintf(array_type,"int");
	    break;
	  case 1:
	    sprintf(array_type,"float");
	    break;
	  case 2:
	    sprintf(array_type,"char");
	    break;
	  }
	  fprintf(stdout,"\t\tItem: %s,\tlength: %d,\ttype: %s,\tnarray:%d\n", nextitem, length, array_type, narray);  
	  if (timeline) print_timeline(map, nextsubsystem, nextitem, length, type);
	  sprintf(item,"%s",nextitem);
	}
	sprintf(subsystem,"%s",nextsubsystem);
	map_next_sub(map, subsystem, nextsubsystem, &nitems);
      }
    }
  }
  exit(1);
}

int print_timeline(const char map[], const char subsystem[], const char item[], int length, int type){
  int index = 0, dummy[10000];
  int currenttime = 999999;
  int firsttime = 0;
  int olength;

  fprintf(stdout,"\t\t INF <--");
  while(firsttime > -1 && currenttime != 0){
    switch(type){
    case  0:
      map_get_int(map, subsystem, item, length, dummy,currenttime, &firsttime); 
      break;
    case 1:
      map_get_float(map, subsystem, item, length, (float*)dummy,currenttime, &firsttime);
      break;
    case 2:
      map_get_char(map, subsystem, item, length, (char*)dummy,currenttime, &firsttime, &olength);
      break;
    }
    if (firsttime > 0)
      fprintf(stdout," %d <-- ", firsttime);
    currenttime=firsttime - 1;
    index++;
  }
  
  fprintf(stdout, " UNDEF\n");
}

