h28118
s 00001/00001/00191
d D 1.3 07/11/01 16:33:55 boiarino 4 3
c *** empty log message ***
e
s 00031/00009/00161
d D 1.2 07/11/01 16:32:44 boiarino 3 1
c 
e
s 00000/00000/00000
d R 1.2 01/11/19 18:58:49 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 clas/cmon/recsis.s/dc_maputil.c
e
s 00170/00000/00000
d D 1.1 01/11/19 18:58:48 boiarino 1 0
c date and time created 01/11/19 18:58:48 by boiarino
e
u
U
f e 0
t
T
I 1

/* part of file was removed by Sergey Boyarinov - do not need it ONLINE */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ntypes.h>
#include <bostypes.h>

/*------------prototypes------------------*/

/*--utility functions--*/
int dc_sector(int index);
int dc_region(int index);
int dc_index(int sector, int layer);

/*--Create the DC_GEOM map(only invoked once per experiment!)--*/ 
int create_DC_map(char *map);

/*--retrieve information(from C)--*/
int get_DCalign_map_arrays(char *map, int runno, float xalign[], float yalign[], float zalign[]);
int get_toroid_map_arrays(char *map, int runno, float xt[], float st[]);
int get_DCrot_map_arrays( char *map, int runno,float xrot[], float yrot[], float zrot[]);

/*--FORTRAN callable routine used in RECSIS--*/
int get_dc_map_geom_(int *runno, float xpos[3][6][3], float spos[3][6][3], float *xt, float *st, int ok[3][6]);
int dc_read_map_int_(int *runno, char *map_f, char *item_f, int arr[6][36][192], int ok_sect[6], int maplen_f, int itemlen_f);
int dc_read_map_float_(int *runno, char *map_f, char *item_f, float arr[6][36][192], int ok_sect[6], int maplen_f, int itemlen_f);

/*--write to the map--*/
int put_DCalign_map_arrays(char *map, int runno, float xalign[], float yalign[], float zalign[]);
int put_toroid_map_arrays(char *map, int runno, float xt[], float st[]);
int put_DCrot_map_arrays( char *map, int runno,float xrot[], float yrot[], float zrot[]);


/*--remove geometry for a run--*/
int remove_toroid_geom(char *map, int runno);
int remove_dc_geom(char *map, int runno);

/*--read/write contents of map to/from ascii--*/
void write_dc_text(FILE *fop, int runno, float xt[], float st[], float xalign[], float yalign[], float zalign[], float xrot[], float yrot[], float zrot[]);
void read_dc_text(FILE *fop, int *runno, float xt[], float st[], float xalign[], float yalign[], float zalign[], float xrot[], float yrot[], float zrot[]);

/*--test routine, how to pass arrays from C to FORTRAN--*/
int testmap_(float xpos[3][6][3]);

/*-------source code-------------------------------------------*/

/*remember sector zero in C is really sector 1*/
int
dc_sector(int index){
  return(index%6);
}

/*remember region zero in C is really region 1*/
int
dc_region(int index){
  return(index/6);
}

int
dc_index(int sector, int layer){
  return(layer*6 + sector);
}

int
get_DCalign_map_arrays(char *map, int runno, float xalign[], float yalign[], float zalign[]){
  int firsttime;

  map_get_float(map,"dc", "xalign", 18, xalign, runno, &firsttime);
  if (firsttime < 0 ) return( 0);
  map_get_float(map,"dc", "yalign", 18, yalign, runno, &firsttime);
  if (firsttime < 0 ) return( 0);
  map_get_float(map,"dc", "zalign", 18, zalign, runno, &firsttime);
  if (firsttime < 0 ) return( 0);

  return(1);
}

void
write_dc_text(FILE *fop, int runno, float xt[], float st[], float xalign[],
 float yalign[], float zalign[], float xrot[], float yrot[], float zrot[]){
  int i;

  fprintf(fop, "DC geometry for run: %d\n", runno);
  fprintf(fop, "Toriod align(xyz):  %3.4f  %3.4f  %3.4f\n", xt[0], xt[1], xt[2])
; 
  fprintf(fop, "Toriod rot(xyz):  %3.4f  %3.4f  %3.4f\n", st[0], st[1], st[2]);
  for(i=0; i<18; i++){
    fprintf(fop, "DC align sec:%1d reg:%1d (xyz):  %3.4f  %3.4f  %3.4f\n", (dc_sector(i)+1), (dc_region(i)+1), xalign[i], yalign[i], zalign[i]);
  }
  for(i=0; i<18; i++){
    fprintf(fop, "DC rot sec:%1d reg:%1d (xyz):  %3.4f  %3.4f  %3.4f\n", (dc_sector(i)+1), (dc_region(i)+1), xrot[i], yrot[i], zrot[i]);
  }

}

int
get_toroid_map_arrays(char *map, int runno, float xt[],float st[]){
  int firsttime;

  map_get_float(map,"toroid", "align", 3, xt, runno, &firsttime);
  if (firsttime < 0 ) return( 0);
  map_get_float(map,"toroid", "rot", 3, st, runno, &firsttime);
  if (firsttime < 0 ) return( 0);
  return(1);
}

int
get_DCrot_map_arrays( char *map, int runno,float xrot[], float yrot[], float zrot[]){
  int firsttime;

  map_get_float(map,"dc", "xrot", 18, xrot, runno, &firsttime);
  if (firsttime < 0 ) return( 0);
  map_get_float(map,"dc", "yrot", 18, yrot, runno, &firsttime);
  if (firsttime < 0 ) return( 0);
  map_get_float(map,"dc", "zrot", 18, zrot, runno, &firsttime);
  if (firsttime < 0 ) return( 0);

  return(1);
}

int
D 3
get_dc_map_geom_(int *runno, float xpos[3][6][3], float spos[3][6][3], float *xt, float *st, int ok[3][6]){
E 3
I 3
get_dc_map_geom_(int *runno, float xpos[3][6][3], float spos[3][6][3], float *xt, float *st, int ok[3][6])
{
  FILE *fd;
E 3
  int sec, layer;
  float xalign[18], yalign[18], zalign[18];
  float xrot[18], yrot[18], zrot[18];
  char *dir, *map, def_map[100];

D 3
  dir = getenv("CLAS_PARMS");
E 3
I 3
  if((dir = (char *)getenv("CLON_PARMS")) == NULL)
  {
    printf("stlib: CLON_PARMS not defined !!!\n");
    exit(0);
  }

E 3
  sprintf(def_map,"%s/Maps/DC_GEOM.map",dir);
I 3
  if((fd=fopen(def_map,"r")) != NULL)
  {
D 4
    printf("dc_maputil: reading calibration map >%s< for run # %d\n",def_map,run);
E 4
I 4
    printf("dc_maputil: reading calibration map >%s< for run # %d\n",def_map,*runno);
E 4
    fclose(fd);
  }
  else
  {
    printf("dc_maputil: error opening map file >%s< - exit\n",def_map);
    exit(0);
  }

E 3
  map = &def_map[0];

D 3

E 3
  if (get_DCalign_map_arrays(map, *runno, xalign, yalign, zalign) && 
D 3
      get_DCrot_map_arrays(map, *runno, xrot, yrot, zrot) ){
E 3
I 3
      get_DCrot_map_arrays(map, *runno, xrot, yrot, zrot) )
  {
E 3
    /*set ok equal to one - map load successful*/
D 3
    for(sec=0; sec<6; sec++){
E 3
I 3
    for(sec=0; sec<6; sec++)
    {
E 3
      for(layer=0; layer<3; layer++)
D 3
	ok[layer][sec] = 1;
E 3
I 3
	    ok[layer][sec] = 1;
E 3
    }
D 3
  } else {
E 3
I 3
  }
  else
  {
E 3
    /*set ok equal to zero - map load failed*/
D 3
    for(sec=0; sec<6; sec++){
E 3
I 3
    for(sec=0; sec<6; sec++)
    {
E 3
      for(layer=0; layer<3; layer++)
D 3
	ok[layer][sec] = 0;
E 3
I 3
	    ok[layer][sec] = 0;
E 3
    }
  }

  get_toroid_map_arrays(map, *runno, xt, st);
  
  for(layer=0; layer<3; layer++){
    for(sec=0; sec<6; sec++) {
      xpos[layer][sec][0] = xalign[dc_index(sec, layer)]; 
      xpos[layer][sec][1] = yalign[dc_index(sec, layer)]; 
      xpos[layer][sec][2] = zalign[dc_index(sec, layer)]; 
      
      spos[layer][sec][0] = xrot[dc_index(sec, layer)]; 
      spos[layer][sec][1] = yrot[dc_index(sec, layer)]; 
      spos[layer][sec][2] = zrot[dc_index(sec, layer)]; 
      
    }
  }
  
  return(ok[0][0]);
}




E 1
