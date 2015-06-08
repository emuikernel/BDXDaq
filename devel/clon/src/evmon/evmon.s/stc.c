
/* Sergey Boyarinov: copied from offline ST package */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* some definitions from offline st.h file */
#define ST_TUBES 6
#define VALUES 0
#define UNCERTAINTIES 1
#define TDC_T0 0
#define TDC_T1 1
#define TDC_T2 2
#define WALK0 3
#define WALK1 4
#define WALK2 5
#define M0 6
#define LAMBDA 7
#define VEFF_LEG_NEAR 8
#define VEFF_LEG_FAR 9
#define VEFF_NOSE_NEAR 10
#define VEFF_NOSE_FAR 11
#define PEDESTAL 12


int
st_read_map(char *map,int runno,char *subsystem, int index,
		float StCal_values[6][13],float StCal_uncs[6][13])
{
  float values[6],uncertainties[6];
  int i, firsttime;

  map_get_float(map,subsystem,"value",ST_TUBES,values, runno, &firsttime);
  map_get_float(map,subsystem,"unc",ST_TUBES,uncertainties,runno,&firsttime);
  for(i=0;i<6;i++){
    StCal_values[i][index]=values[i];
    StCal_uncs[i][index]=uncertainties[i];
  }
}

int
stcal_read_map_(int *runno,float StCal_values[6][13],float StCal_uncs[6][13])
{
  char *dir, map[128];
  float st2tof,offsets[6],side2side[3],pair2pair[3];
  float veff_leg_near[6],veff_leg_far[6];
  float veff_nose_near[6],veff_nose_far[6];
  int i,firsttime;

  dir = getenv("CLAS_PARMS");
  if(dir==NULL)
  {
    printf("stcal_read_map_: ERROR: env var CLAS_PARMS does not defined\n");
    return(0);
  }

  sprintf(map,"%s/Maps/ST_CALIB.map",dir); 

  st_read_map(map,*runno,"pedestals",PEDESTAL,StCal_values,StCal_uncs);
  st_read_map(map,*runno,"T0",TDC_T0,StCal_values,StCal_uncs);
  st_read_map(map,*runno,"T1",TDC_T1,StCal_values,StCal_uncs);
  st_read_map(map,*runno,"T2",TDC_T2,StCal_values,StCal_uncs);
  st_read_map(map,*runno,"W0",WALK0,StCal_values,StCal_uncs);
  st_read_map(map,*runno,"W1",WALK1,StCal_values,StCal_uncs);
  st_read_map(map,*runno,"W2",WALK2,StCal_values,StCal_uncs);
  st_read_map(map,*runno,"M0",M0,StCal_values,StCal_uncs);
  st_read_map(map,*runno,"LAMBDA",LAMBDA,StCal_values,StCal_uncs);

  map_get_float(map,"veff_leg","near",6,veff_leg_near,*runno,&firsttime);
  map_get_float(map,"veff_leg","far",6,veff_leg_far,*runno,&firsttime);
  map_get_float(map,"veff_nose","near",6,veff_nose_near,*runno,&firsttime);
  map_get_float(map,"veff_leg","far",6,veff_nose_far,*runno,&firsttime); 
  map_get_float(map,"st2tof","value",1,&st2tof,*runno,&firsttime);
  map_get_float(map,"delta_T","value",6,offsets,*runno,&firsttime);

  for (i=0;i<6;i++){
    StCal_values[i][VEFF_LEG_NEAR]=veff_leg_near[i];
    StCal_values[i][VEFF_LEG_FAR]=veff_leg_far[i];
    StCal_values[i][VEFF_NOSE_NEAR]=veff_nose_near[i];
    StCal_values[i][VEFF_NOSE_FAR]=veff_nose_far[i];  
    StCal_values[i][TDC_T0]-=offsets[i]+st2tof;
    /* printf("%f %f\n",offsets[i],StCal_values[i][TDC_T0]); */
   }
 fprintf(stderr,"Reading map file %s for run %d.\n",map,*runno);
}








/*Sergey: temporary from libutil*/

/* 
*  duplicates fortran string, removes trailing blanks and inserts ending NULL 
*/

char *
strdupf(const char *fortran_string, int len) 
{
  char *s;
  int len_occ;

  /* find occupied length, not including trailing blanks */
  for(len_occ=len;(len_occ>0)&&(fortran_string[len_occ-1]==' ');len_occ--) ;

  s=(char *)malloc(len_occ+1);
  strncpy(s,fortran_string,(size_t)len_occ);
  s[len_occ]='\0';
  
  return(s);
}


/* C++ linker cannot find 'rindex' on Linux */
int
myrindex_(const char *str, int *c)
{
  int ret;
  char *ptr;

  ptr = strrchr(str,*c);

  if(ptr == NULL) ret = 0;
  else            ret = (ptr - str) + 1;
  /*
  printf("rindex: ret=%d (0x%08x 0x%08x)\n",ret,str,ptr);
  */
  return(ret);
}

