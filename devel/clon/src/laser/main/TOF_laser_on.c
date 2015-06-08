/*
 * TOF_laser_on.c - turn ON lasers.
 *
 *    written by:    Giovanetti, Larson
 *
 *    modified by:   SP, June 2003
 */

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>

int
main(int argc, char *argv[])
{
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
    }
  }
  
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
}

