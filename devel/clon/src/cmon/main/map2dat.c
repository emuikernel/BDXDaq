
/* map2dat.c - converts *.map file to *.dat file */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX 1000

void
main(int argc, char **argv)
{
  FILE *fd;
  int nrecords, nitems, itype, narray;
  char subsystem[16], item[16];

  int length;
  char nextsubsystem[16], nextitem[16], type[10];
  int length_[MAX];
  char nextsubsystem_[MAX][16], nextitem_[MAX][16], type_[MAX][10];

  int i, j, run, k;
  char map[1000], dat[1000], mapname[1000], *dir;
  int ibuf[1000000];
  float fbuf[1000000];

  if(argc != 3)
  {
    printf("Usage: map2dat <mapname> run_number\n");
    printf("       where <mapname> is TAG_CALIB etc\n");
    printf("       All files located in CLON_PARMS area\n");
    exit(0);
  }

  strcpy(mapname,argv[1]);
  run = atoi(argv[2]);
  printf("map2dat: mapname >%s< run=%d\n",mapname,run);

  if((dir = (char *)getenv("CLON_PARMS")) == NULL)
  {
    printf("map2dat: CLON_PARMS not defined !!!\n");
    exit(0);
  }

  sprintf(map,"%s/cmon/tglib/%s.map",dir,mapname);
  sprintf(dat,"%s/cmon/tglib/%s.dat",dir,mapname);
  printf("map2dat: reading map file >%s< for run # %d\n",map,run);

  if((fd=fopen(dat,"w")) != NULL)
  {
    map_next_sub(map, "*", nextsubsystem, &nitems);
    nrecords = 0;
    while(nextsubsystem[0] != '*')
    {
      printf("\tSubsystem: %s,\tnitems: %d\n", nextsubsystem, nitems);  
      sprintf(item, "*");
      for(i=0; i< nitems; i++)
      {
        map_next_item(map,nextsubsystem,item,nextitem,&length,&itype,&narray);
        switch(itype)
        {
          case 0:
            sprintf(type,"int");
            break;
          case 1:
            sprintf(type,"float");
            break;
          case 2:
            sprintf(type,"char");
            break;
        }
        printf("\t\tItem: %s,\tlength: %d,\ttype: %s,\tnarray:%d\n",
          nextitem, length, type, narray);
        sprintf(item,"%s",nextitem);

        fprintf(fd,"%s %s %s %d %s\n",
          mapname, nextsubsystem, nextitem, length, type);		
        nrecords ++;
      }
	  
      sprintf(subsystem,"%s",nextsubsystem);
      map_next_sub(map, subsystem, nextsubsystem, &nitems);
	  
    }
    fclose(fd);
  }

  /*map_get_float(mapname,nextsubsystem,nextitem,length,fbuf,run,&k);*/

  /*
  map_get_float(map,"tag_t","slope_left",61,buf,run,&k);
  map_get_float(map,"tag_t","slope_right",61,buf,run,&k);
  map_get_float(map,"tag_t","dt_left",61,buf,run,&k);
  map_get_float(map,"tag_t","dt_right",61,buf,run,&k);
  map_get_float(map,"tag_t","ci",121,buf,run,&k);

  map_get_float(map,"tag_e","slope",1,buf,run,&k);
  map_get_float(map,"tag_e","dt",384,buf,run,&k);
  */

  exit(0);
}
