#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <string.h>
#include "maps.h"

#define DEBUG
#define FIRSTTOLAST   0
#define LASTTOFIRST   1
#define FIRSTTOFIRST  2
#define LASTTOLAST    3

MapDataPtr   allmaps;
int        Nmin;
int        Nstep;
Boolean    done = False;
char       c;
FILE   *logfile;  



/*----- WriteAsciiMap ----*/

void WriteAsciiMap(map, fp)

MapDataPtr   map;
FILE        *fp;

{
  int  i;
  char  *tstr;

  tstr = (char *)malloc(80);
  fprintf(fp, "   %d   ", map->numpoints);

  strcpy(tstr, map->mapname);


  if (strlen(tstr) > 1) {
    for (i = 0; i < strlen(tstr); i++)
      if (tstr[i] == ' ')
	tstr[i] = '_';
    fprintf(fp, "**%s\n", tstr);
  }
  else
    fprintf(fp, "\n");

  for (i = 0; i < map->numpoints; i++)
    fprintf(fp, "%10f  %10f\n", map->latitude[i]/DEGTORAD, map->longitude[i]/DEGTORAD);
  free(tstr);
}

/* ------- MapReaderInit ------*/

void MapReaderInit()
{
  Nmin = 0;
  Nstep = 1;
}


/* ------- DebugMessage -------*/

void DebugMessage(s)

char   *s;

/*writes a message only if DEBUG is defined*/

{
#ifdef DEBUG
  fprintf(stderr, "%s", s);
#endif
}


/*------- OpenBinaryMapFile ------ */

FILE  *OpenBinaryMapFile(fname, mode)

char *fname;
char *mode;

/* mode should be "rb" or "wb" */

{
  int     status;
  char    line[120];
  FILE   *fp;

  if (!fname) {
    printf("\nBinary Map File Name: ");
    fscanf(stdin, "%s", line);
  }
  else
    strcpy(line, fname);
  
/* try to open it */

  printf("Will attempt to open file: %s\n", line);
  fp = fopen(line, mode);

  if (fp == NULL) {
    printf("Unable to open file: %s\nExiting.\n", line);
    exit(-1);
  }

  return fp;
}

/*------- OpenMapFile ------ */

FILE  *OpenMapFile()


/* primitive query for the mapfile name. In the
   real program, this would be accomplished with
   a Motif File Selection dialog box */

{
  char    fname[80];
  int     status;
  FILE    *mfile;

  printf("\nAscii Map File Name: ");
  scanf("%s", fname);
   
/* try to open it */

  mfile = fopen(fname, "r");

  if (mfile == NULL) {
    printf("Unable to open file: %s\nExiting.\n", fname);
    exit(-1);
  }
  
  return   mfile;
}



/* --------- Usage ---------------*/

void  Usage()

/* called if improper argument list encountered */

{
  printf("\nusage: mapreader\n");
}

/*-------- ProcessCommandLine ----------*/


void  ProcessCommandLine(argc, argv)

int    argc;
char   *argv[];

/* process the command line arguments */

{
  int   c;


  while ((--argc > 0) && (*++argv)[0] == '-')
    while (c = *++argv[0])
      switch (c) {
      default:
	printf("ignoring illegal option: %c\n", c);
	  break;
      }
  
}


/*------ OutOfMemoryCheck -----*/

void OutOfMemoryCheck(ptr, module)

void   *ptr;
char   *module;

{
  if (ptr != NULL)
    return;

  fprintf(stderr, "Out of memory in module %s./nExiting./n", module);
  exit(-1);
}


/*------ CreateMap -------*/

MapDataPtr   CreateMap(name, npoints)

char *name;
unsigned int npoints;

{
  MapDataPtr   tmap;

  tmap = (MapDataPtr)malloc(sizeof(MapData));
  tmap->mapname[0] = '\0';

  OutOfMemoryCheck((void *)tmap, "CreateMap");

  if (tmap == NULL) {
    fprintf(stderr, "Out of memory in CreateMap. Exiting.\n");
    
  }
  strncat(tmap->mapname, name, MAPNAMESIZE-1);
  tmap->numpoints = npoints;

  tmap->latitude  = (float *)malloc(npoints*sizeof(float));
  OutOfMemoryCheck((void *)tmap->latitude, "CreateMap");

  tmap->longitude = (float *)malloc(npoints*sizeof(float));
  OutOfMemoryCheck((void *)tmap->longitude, "CreateMap");
  return  tmap;
}

/*------ DestroyMap -------*/

void   DestroyMap(map)

MapDataPtr   *map;

{
  MapDataPtr   tmap;

  tmap = (MapDataPtr)(*map);
  free(tmap->latitude);
  free(tmap->longitude);
  free(tmap);
  *map = NULL;
}


/*------ DestroyMaps -------*/

void   DestroyMaps(map)

MapDataPtr   *map;

/* actually destroys from the given map to end of linked list */

{
  MapDataPtr   tmap;


  tmap = (MapDataPtr)(*map);

  if (tmap->next != NULL)
    DestroyMap(&(tmap->next));

  free(tmap->latitude);
  free(tmap->longitude);
  free(tmap);
  *map = NULL;
}

/*------ DeleteMap ------*/

void  DeleteMap(map)

MapDataPtr  *map;
 
{
  MapDataPtr   prev, next;

  prev = (*map)->prev;
  next = (*map)->next;

  if (prev != NULL)
    prev->next = next;

  if (next != NULL)
    next->prev = prev;

  DestroyMap(map);
}

/*------ ReadBinaryMap -------*/
MapDataPtr  ReadBinaryMap(fp)

FILE      *fp;

{
  MapDataPtr   map;
  char         tnam[MAPNAMESIZE];
  unsigned int np;
  int status1, status2;

  if (fread(&(tnam[0]), 1, MAPNAMESIZE, fp) == 0)
    return NULL;

  if (fread(&np, sizeof(int), 1, fp) == 0)
    return NULL;

  map = CreateMap(tnam, np);

  status1 = fread(map->latitude,  sizeof(float), map->numpoints, fp);
  status2 = fread(map->longitude, sizeof(float), map->numpoints, fp);

/* xmin .... xymax not written */

  if ((status1 == 0) || (status2 == 0)) {
    DestroyMap(&map);
    fprintf(stderr, "Read error in ReadBinaryMap. Exiting.\n");
    exit(-1);
  }

  return  map;
}

/*------ WriteBinaryMap -------*/

void WriteBinaryMap(fp, map)

FILE      *fp;
MapDataPtr map;

{
  fwrite(map->mapname, 1, MAPNAMESIZE, fp);
  fwrite(&(map->numpoints), sizeof(int), 1, fp);
  fwrite(map->latitude,  sizeof(float), map->numpoints, fp);
  fwrite(map->longitude, sizeof(float), map->numpoints, fp);

/* no need to write xmin ... ymax */
}


/*------ GetMapInfo -----*/

void GetMapInfo()

{
  int   totnumpoints = 0;
  int   numpoints;
  short mapcount = 0;
  char line[80];
  int i;
  short ndiscarded = 0;
  short nused = 0;
  FILE   *mfile;

  fprintf(stdout, "information about ascii map\n");
  
  mfile = OpenMapFile();

  while(fgets(line, 80, mfile)) {
    mapcount++;
    sscanf(line, "%d", &numpoints);
    if (numpoints >= Nmin)
      fprintf(stdout, "map %d has %d points\n", mapcount, numpoints);

    if (numpoints < Nmin)
      ndiscarded++;
    else
      nused++;

    totnumpoints += numpoints;

    for (i=0; i<numpoints; i++) {
      fgets(line, 80, mfile);
      if (line == NULL) {
	fprintf(stdout, "Unexpected end of file encountered\nExiting.\n");
        fclose(mfile);
        exit(-1);
      }
    }
  }
  
  fprintf(stderr, "\nUnderstood this mapfile.\nNumber of Maps: %d\nTotal Points: %d\n",
	  mapcount, totnumpoints);

  fprintf(stdout, "total number of maps with <  %d points is %d\n", Nmin, ndiscarded);
  fprintf(stdout, "total number of maps with >= %d points is %d\n", Nmin, nused);

   fclose(mfile);
}

/*------- DoConvertMap -------*/

void DoConvertMap()

{
  FILE   *binfp, *mfile;
  char   *tname;
  char   *mname;
  int     numpoints, rednumpoints;
  short   mapcount = 0;
  char    *line;
  int     i, j, k;
  short   ndiscarded = 0;
  short   nused = 0;
  MapDataPtr map;
  float   latitude, longitude;
  char   *special = "**";
  float   prevfirstlat = 1.0e20;
  float   prevlastlat = 1.0e20;
  float   prevfirstlong = 1.0e20;
  float   prevlastlong = 1.0e20;
  float   newfirstlat, newlastlat;
  float   newfirstlong, newlastlong;

  fprintf(stdout, "converting map to binary\n");

  mfile = OpenMapFile();
  binfp = OpenBinaryMapFile(NULL, "wb");

/* write Nmin twice for compatibility with earlier versions*/

  fwrite(&Nmin, sizeof(int), 1, binfp);
  fwrite(&Nmin, sizeof(int), 1, binfp);

/* read the maps from the ascii map file */

  line = (char *)malloc(80);
  mname = (char *)malloc(MAPNAMESIZE+1);

  while(fgets(line, 80, mfile)) {
    mapcount++;
    sscanf(line, "%d", &numpoints);

/* see if a name is present */

    tname = strstr(line, special);
    if (tname != NULL)
      tname += 2;

    fprintf(stdout, "map %d has %d points ", mapcount, numpoints);

/* see if we want to reduce the map size */

    if (Nstep > 1) {
      if ((numpoints % Nstep) == 0)
	rednumpoints = numpoints/Nstep;
      else
	rednumpoints = numpoints/Nstep + 1;
      fprintf(stdout, " -- reduced to  %d points\n", rednumpoints);
    }
    else {
      rednumpoints = numpoints;
      fprintf(stdout, "\n");
    }

/* ignore small maps */

    if (rednumpoints < Nmin) {
      ndiscarded++;
      for (i=0; i<numpoints; i++) {
	fgets(line, 80, mfile);
	if (line == NULL)
	  fprintf(stdout, "Unexpected end of file encountered\nExiting.\n");
      }
    }

    else { /* mumpoints >= Nmin */
      nused++;
      if (tname != NULL) {
	sscanf(tname, "%s", mname);
	for (k = 0; k < strlen(mname); k++)
	  if (mname[k] == '_')
	    mname[k] = ' ';
      }
      else
	sprintf(mname, "Bordermap%-d", nused);
      map = CreateMap(mname, rednumpoints);
      fprintf(stderr, "mname: %s\n", mname);

      j = 0;
      for (i=0; i<numpoints; i++) {
	fgets(line, 80, mfile);
	if (line == NULL)
	  fprintf(stdout, "Unexpected end of file encountered\nExiting.\n");
	else {
	  if ((((i+1) % Nstep) == 0) || (i == (numpoints-1))) {
	    sscanf(line, "%f %f", map->latitude + j, map->longitude + j);

	    if (j == 0) {
	      if ((map->latitude[j] == prevlastlat) && (map->longitude[j] == prevlastlong)) {
		fprintf(logfile, "check for map connectivity at lat long = [%f, %f]\n", prevlastlat, prevlastlong);
	      }
	    }
	    else {
	      if ((map->latitude[j] == map->latitude[j-1]) && (map->longitude[j] == map->longitude[j-1]))
		fprintf(logfile, "check for map DUPLICATE PNTS at lat long = [%f, %f]\n", map->latitude[j], map->longitude[j]);
	    }

	    if (i == (numpoints-1)) {
	      prevlastlat = map->latitude[i];
	      prevlastlong = map->longitude[i];
	    }
	  

	    map->latitude[j]  *= DEGTORAD;
	    map->longitude[j] *= DEGTORAD;
	    j++;
	  }
	}
      }
      WriteBinaryMap(binfp, map);
      DestroyMap(&map);
    } /* end of else for numpoints >= Nmin*/
  }
  
  free(line);
  free(mname);
  fclose(binfp);
  fclose(mfile);
}

/*------- ListCommands -------*/

void ListCommands(c)

char  *c;

{
  printf("\nPossible Commands (first char sufficient):\n\n");
  printf("\tb*inary file check\n");
  printf("\tc*onvert mapfile\n");
  printf("\ti*nformation on mapfile\n");
  printf("\tm*odify parameters\n");
  printf("\to*rder maps -> new ascii file\n");
  printf("\tq*uit\n");
  printf("\tw*rite parameters\n");

  while (!isalpha(*c = getchar()))
    ;

  while (getchar() != '\n')
    ;
}


/* ------ WriteParameters ------ */

void WriteParameters(fp)

FILE   *fp;

/* just dump the parameters for diagnostic purposes */

{
  int i;

  fprintf(fp, "---- parameters\n\n");
  fprintf(fp, "Nmin   \t%d\tmaps with fewer points are not converted\n", Nmin);
  fprintf(fp, "Nstep  \t%d\ttake evry Nstep points\n", Nstep);
}

/*------- BinaryFileCheck --------*/

void BinaryFileCheck()

{
  FILE   *binfp;
  MapDataPtr map;
  int    dummy;

  binfp = OpenBinaryMapFile(NULL, "rb");

  fread(&Nmin,    sizeof(int), 1, binfp);
  fread(&dummy,   sizeof(int), 1, binfp);

  while ((map = ReadBinaryMap(binfp)) != NULL) {
    fprintf(stdout, "map: %s has %d points\n", map->mapname, map->numpoints);
    DestroyMap(&map);
  }

  fprintf(stdout, "This map file used Nmin = %d\n", Nmin);
  fclose(binfp);
}

/*------ ModifyParameter --------*/


void ModifyParameter()

/* modify an input parameter */

{
  int choice;
  Boolean  changed = True;

  printf("\n\nEnter the number corresponding to the parameter you want to modify:\n\n");
  printf("0) \tCANCEL- NO MODIFICATION\n");
  printf("1)\tNmin      \tmaps with fewer points are not converted\n");
  printf("2)\tNstep     \ttake every Nstep points\n");
  
  scanf("%d", &choice);

  switch(choice)  {
    
  case 1:
    printf("old value for Nmin: %d\nEnter new value:\n", Nmin);
    scanf("%d", &Nmin);
    break;

  case 2:
    printf("old value for Nstep: %d\nEnter new value:\n", Nstep);
    scanf("%d", &Nstep);
    break;
    
  default:
    changed = False;
    break;

  }

  if (changed) {
    fprintf(stdout, "\nCURRENT INPUT PARAMETERS:\n");
    WriteParameters(stdout);
  }
}



/*------- LoadMaps --------*/

static void LoadMaps(mapfile)

FILE   *mapfile;

{
  int        i;
  MapDataPtr map;
  int dummy;
  MapDataPtr  prevMap = NULL;
  int  NminMap;

  fprintf(stdout, "checking mapfile...");
  rewind(mapfile);

/* read two ints for compatibility with earlier versions */

  if (fread(&NminMap, sizeof(int), 1, mapfile) == 0) {
    fprintf(stderr, "Error in mapfile.\nExiting.\n");
    exit(-1);
  }

  if (fread(&dummy, sizeof(int), 1, mapfile) == 0)  {
    fprintf(stderr, "Error in mapfile.\nExiting.\n");
    exit(-1);
  }


  while ((map = ReadBinaryMap(mapfile)) != NULL) {
    if (prevMap == NULL)
      allmaps = map;
    else {
      prevMap->next = map;
      map->prev = prevMap;
    }
    prevMap = map;
  }


  fprintf(stderr, "Map file looks O.K. (Nmin =  %d)\n", NminMap);
}


/*------- SwitchMaps ------*/

void  SwitchMaps(map1, map2)

MapDataPtr  map1, map2;

{
  MapData  temp;

  temp = *map2;

  strcpy(map2->mapname, map1->mapname);
  map2->numpoints = map1->numpoints;
  map2->latitude = map1->latitude;
  map2->longitude = map1->longitude;
  map2->user1 = map1->user1;
  map2->user2 = map1->user2;

  strcpy(map1->mapname, temp.mapname);
  map1->numpoints = temp.numpoints;
  map1->latitude = temp.latitude;
  map1->longitude = temp.longitude;
  map1->user1 = temp.user1;
  map1->user2 = temp.user2;
}


/*------- OrderMaps -------*/

void OrderMaps()

{
  FILE   *bfp; /* binary file */
  FILE   *fp;  /* new ascii file */
  char   *fname;
  MapDataPtr  map, map1, map2, next1, next2;
  int    i, j, nummaps;
  Boolean   same;

  fname = (char *)malloc(80);

  printf("\nBinary File Name: ");
  fscanf(stdin, "%s", fname);
  bfp = fopen(fname, "rb");
  free(fname);
  if (bfp == NULL) {
    printf("Unable to open file.\nExiting.\n");
    exit(-1);
  }

  fname = (char *)malloc(80);

  printf("\nOrdered Ascii File Name: ");
  fscanf(stdin, "%s", fname);
  fp = fopen(fname, "w");
  free(fname);
  if (fp == NULL) {
    printf("Unable to open file.\nExiting.\n");
    exit(-1);
  }

  LoadMaps(bfp);

  fprintf(stderr, "sorting...");

/* bubble sort -- time is not important */


  nummaps = 0;
  for(map1 = allmaps; map1 != NULL; map1 = map1->next)
    nummaps++;

  fprintf(stderr, "number of maps = %d\n", nummaps);


  for(map1 = allmaps; map1->next != NULL; map1 = map1->next) {
    for(map2 = map1->next; map2 != NULL; map2 = map2->next) {
      if (map2->numpoints > map1->numpoints) {
	SwitchMaps(map1, map2);
      }
    }
  }

  fprintf(stderr, "done\n");

/* checking for duplicates */

  fprintf(stderr, "checking for duplicates...");

  for(map1 = allmaps; map1->next != NULL; map1 = map1->next) {
    map2 = map1->next;
    while ((map2 != NULL) && (map1->numpoints == map2->numpoints)) {
      if (map1->latitude[0] == map2->latitude[0]) 
	if (map1->longitude[0] == map2->longitude[0]) {
	  for (i = 1; i < map1->numpoints; i++) {
	    same = ((map1->latitude[i] == map2->latitude[i]) && (map1->longitude[i] == map2->longitude[i]));
	    if (!same)
	      break;
	  }

	  if (same) {
	    fprintf(logfile, "FIXED DUPLICATE MAPS %s and %s with %d points\n",map1->mapname, map2->mapname, map1->numpoints);
	    map = map2->prev;
	    DeleteMap(&map2);
	    map2 = map;
	  }
	}
      
      map2 = map2->next;
    }
  }

  fprintf(stderr, "done\n");

  fprintf(stderr, "writing...");

  for(map = allmaps; map != NULL; map = map->next)
    WriteAsciiMap(map, fp);

  fprintf(stderr, "done\n");

  DestroyMaps(&allmaps);
  fclose(fp);
  fclose(bfp);
}


/*------- HandleCommand -------*/

void HandleCommand(c)

char   c;

{
  int   n;

  switch (c) {

  case 'b':
  case 'B':
    BinaryFileCheck();
    break;

  case 'c':
  case 'C':
    DoConvertMap();
    break;

  case 'i':
  case 'I':
    GetMapInfo();
    break;

  case 'm':
  case 'M':
    ModifyParameter();
    break;

  case 'o':
  case 'O':
    OrderMaps();
    break;

  case 'q':
  case 'Q':
    done = True;
    break;

  case 'w':
  case 'W':
    WriteParameters(stdout);
    break;

  default:
    printf("\nDo not understand command starting with '%c'\n", c);
    break;
  }
}

/*-------- main ----------*/

main(argc, argv)

int    argc;
char   *argv[];


{

#ifdef __ultrix
  fprintf(stdout, "\n** running under ULTRIX **\n");
#elif hpux
  fprintf(stdout, "\n** running under HPUX **\n");
#elif __sgi
  fprintf(stdout, "Running under IRIX\n");
#endif

  logfile = fopen("logfile", "w");
  MapReaderInit();

/* process the command line */

  ProcessCommandLine(argc, argv);

  while (!done) {
    ListCommands(&c);
    HandleCommand(c);
  }



  fclose(logfile);
}








