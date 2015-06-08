h00791
s 00066/00091/00012
d D 1.2 03/07/21 14:14:49 sergpozd 3 1
c Completely new revision
e
s 00000/00000/00000
d R 1.2 03/03/03 14:04:16 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 laser/sc/s/TOF_laser_on.c
e
s 00103/00000/00000
d D 1.1 03/03/03 14:04:15 boiarino 1 0
c date and time created 03/03/03 14:04:15 by boiarino
e
u
U
f e 0
t
T
I 3
/*
 * TOF_laser_on.c - turn ON lasers.
 *
 *    written by:    Giovanetti, Larson
 *
 *    modified by:   SP, June 2003
 */
E 3
I 1

D 3
/* TOF_laser_on.c - giovanetti, Larson */

E 3
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>

int
main(int argc, char *argv[])
{
D 3
  char exename[111];
  char result[512];
  int j,k,l;
  int no_lasers = 4;
  int laser_no[4] = {1,2,3,4};

  switch(argc)      /* if no arguments, get  parameter from user */
  { 
    case 1: 
      printf("  default is to turn off all lasers\n");
      break;
    case 2:
      sscanf(argv[1],"%d",&laser_no[0]);
      printf("  turn off laser= %d \n",laser_no[0]);
      no_lasers=1;
      break;
    case 3:
      sscanf(argv[1],"%d",&laser_no[0]);
      sscanf(argv[2],"%d",&laser_no[1]);
      printf("  turn off laser= %d, %d \n",laser_no[0],laser_no[1]);
      no_lasers=2;
      break;
    case 4:
      sscanf(argv[1],"%d",&laser_no[0]);
      sscanf(argv[2],"%d",&laser_no[1]);
      sscanf(argv[3],"%d",&laser_no[2]);
      printf("  turn off laser= %d, %d \n",laser_no[0],laser_no[1],laser_no[2]);
      no_lasers=3;
      break;
    case 5:
      sscanf(argv[1],"%d",&laser_no[0]);
      sscanf(argv[2],"%d",&laser_no[1]);
      sscanf(argv[3],"%d",&laser_no[2]);
      sscanf(argv[4],"%d",&laser_no[3]);
      printf("  turn off laser= %d, %d \n",laser_no[0],laser_no[1],laser_no[2],laser_no[3]);
      no_lasers=4;
      break;
    default: printf( "error in no of args=",argc);
      exit(1);         
  } 


  /* intialize DP stuff */
  printf("initializing the connection \n");
  DP_cmd_init("clon10");
  printf("initializing done \n");
 

  /* command */
  k=1;
  j=0;
  while(j < no_lasers)
  {
    if(!(laser_no[j] == 1 || laser_no[j] ==2 || laser_no[j] ==3 || laser_no[j] ==4))
    {
      printf(" illegal laser no = %d only 1,2,3,4 \n",laser_no[j]);
      break;
E 3
I 3
  int flag = 0;
  int j, no_lasers = 4;
  int laser_no[4]  = {1,2,3,4};
  
  /* Check input parameters */
  if(argc > 5) {
    printf("\n Error: Wrong number of inputs. \n");
    flag = 1;
  }
  else if (argc > 1) {
    if(strstr(argv[1],"h") != NULL)  flag = 2;
    else {
      no_lasers = argc-1;
      for(j=0; j<no_lasers; j++) {
	laser_no[j] = atoi(argv[j+1]);
	if(laser_no[j] < 1 || laser_no[j] > 4) {
	  printf("\n Error: Wrong input parameter. \n");
	  flag = 3;
	  j = 10;
	}
      }
E 3
    }
D 3

    sprintf(exename,"exec gas(%d,%d)",laser_no[j],k);
    printf("%s \n",exename);
    if(laser_no[j] == 1)
    {
      DP_cmd("camac3",exename,result,111); 
      printf(" camac3 laser %d\n",laser_no[j]);
    }
    else
    {
      DP_cmd("sc-laser1",exename,result,111);
      printf(" sc-laser1 laser %d\n",laser_no[j]);
    }
    printf("%s \n",result);

    sprintf(exename,"exec pwr(%d,%d)",laser_no[j],k);
    printf("%s \n",exename);
    if(laser_no[j] == 1)
    {
      DP_cmd("camac3",exename,result,111); 
      printf(" camac3 laser %d\n",laser_no[j]);
    }
    else
    {
      DP_cmd("sc-laser1",exename,result,111);
      printf(" sc-laser1 laser %d\n",laser_no[j]);
    }
    printf("%s \n",result);
    j++;
E 3
  }
D 3

  exit(0);   
E 3
I 3
  
  /* Print help message */
  if(flag != 0) {
    printf("\n Usage: TOF_laser_on [n1 [n2] ...] \n");
    printf("        -------------------------- \n");
    printf(" Where \"ni\" - laser number from the list \"1 2 3 4\" \n\n");
    return(-1);
  }
  
  /* Start of getting statuses */
  if(argc > 1) {
    printf("\n Turn on ");
    for(j=0; j<no_lasers; j++) printf("laser#%d ", laser_no[j]);
    printf("\n\n");
  }
  else
    printf("\n Turn on all lasers \n\n");
  
  
  /* Commands execution */
  
  for(j=0; j<no_lasers; j++) {
    gas_ON(laser_no[j]);
    printf(" gas_ON() for laser#%d \n ",laser_no[j]);
    command_print_result();
  }
  
  printf(" Wait for 10 seconds ");
  for(j=0; j<10; j++) { printf("."); fflush(stdout); sleep(1); }
  printf("\n\n");
  
  for(j=0; j<no_lasers; j++) {
    pwr_ON(laser_no[j]);
    printf(" pwr_ON() for laser#%d \n ",laser_no[j]);
    command_print_result();
  }
  
  return(0);
E 3
}
D 3

E 3

E 1
