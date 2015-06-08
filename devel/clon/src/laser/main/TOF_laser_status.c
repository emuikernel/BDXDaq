/*
 * TOF_laser_status.c - get lasers statuses.
 *
 *    written by:    Giovanetti, Larson
 *
 *    modified by:   SP, June 2003
 */

#define INIT 1

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include "laser.h"

extern char *rocname[];

int
main(int argc, char *argv[])
{
  char str_tmp[200];
  int  flag = 0;
  int  j, no_lasers = 4;
  int  laser_no[4]  = {1,2,3,4};
  int  iregister[5] = {0,0,0,0,0};
  int  oregister[5] = {0,0,0,0,0};


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
    printf("\n Usage: TOF_laser_status [n1 [n2] ...] \n");
    printf("        ------------------------------ \n");
    printf(" Where \"ni\" - laser number from the list \"1 2 3 4\" \n\n");
    return(-1);
  }
  
  
  /* Start of getting statuses */
  if(argc > 1) {
    printf("\n Get statuses for ");
    for(j=0; j<no_lasers; j++) printf("laser#%d ", laser_no[j]);
    printf("\n\n");
  }
  else
    printf("\n Get statuses for all lasers \n\n");
  
  
  /* Get input registers */
  printf(" get input registers \n");
  for(j=0; j<no_lasers; j++) {
    sprintf(str_tmp,input(laser_no[j]));
    if(strstr(str_tmp,"ERROR") == NULL) {
      iregister[laser_no[j]] = sbin_to_int(str_tmp);
    }
    else {
      printf("\n Error: Unexpected response from \"%s\"\n",
	     rocname[laser_no[j]]);
      return(-1);
    }
  }
  
  /* Get output registers */
  printf(" get output registers \n\n");
  for(j=0; j<no_lasers; j++) {
    sprintf(str_tmp,output(laser_no[j]));
    if(strstr(str_tmp,"ERROR") == NULL) {
      oregister[laser_no[j]] = sbin_to_int(str_tmp);
    }
    else {
      printf("\n Error: Unexpected response from \"%s\"\n",
	     rocname[laser_no[j]]);
      return(-1);
    }
  }
  
  /* Determine statuses and control lines states */
  for(j=0; j<no_lasers; j++) {
    printf(" laser status for laser number = %d \n", laser_no[j]);
    printf(" input register  = 0x%x \n", iregister[laser_no[j]]);

    if( bit_TST(status_pwr_bit[laser_no[j]],iregister[laser_no[j]]) )
      {
	printf("   status indicates laser power on \n");
      }
    else
      {
	printf("   status indicates laser power off \n");
      }
    if( bit_TST(status_rdy_bit[laser_no[j]],iregister[laser_no[j]]) )
      {
	printf("   status indicates laser ready \n");
      }
    else
      {
	printf("   status indicates laser not yet ready \n");
      }
    if( bit_TST(status_ena_bit[laser_no[j]],iregister[laser_no[j]]) )
      {
	printf("   status indicates laser enabled \n");
      }
    else
      {
	printf("   status indicates laser disabled \n");
      }

    printf(" output register = 0x%x \n", oregister[laser_no[j]]);

    if( bit_TST(contrl_gas_bit[laser_no[j]],oregister[laser_no[j]]) )
      {
	printf("   gas control line level on \n");
      }
    else
      {
	printf("   gas control line level off \n");
      }
    if( bit_TST(contrl_pwr_bit[laser_no[j]],oregister[laser_no[j]]) )
      {
	printf("   pwr control line level on \n");
      }
    else
      {
	printf("   pwr control line level off\n");
      }
    printf(" enable and disable are pulses (not available as status) \n\n");
  }
  
  return(0);
}

