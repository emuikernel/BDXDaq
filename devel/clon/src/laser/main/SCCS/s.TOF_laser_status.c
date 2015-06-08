h06705
s 00131/00185/00018
d D 1.2 03/07/21 14:15:05 sergpozd 3 1
c Completely new revision
e
s 00000/00000/00000
d R 1.2 03/03/03 14:04:16 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 laser/sc/s/TOF_laser_status.c
e
s 00203/00000/00000
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
 * TOF_laser_status.c - get lasers statuses.
 *
 *    written by:    Giovanetti, Larson
 *
 *    modified by:   SP, June 2003
 */
E 3
I 1

D 3
/* TOF_laser_status.c - execution of the laser commands giovanetti, Larson */
E 3
I 3
#define INIT 1
E 3

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
D 3

#define INIT 1

E 3
#include "laser.h"

I 3
extern char *rocname[];

E 3
int
main(int argc, char *argv[])
{
D 3
  char exename[111];
  char result[512];
  int j,k,l;
  int no_lasers=4;
  int laser_no[4]={1,2,3,4};
  int iregister[4]={0,0,0,0};
  int oregister[4]={0,0,0,0};
  int bits[16]={0,0,0,0,  0,0,0,0,  0,0,0,0,  0,0,0,0};
  int debug=0;
E 3
I 3
  char str_tmp[200];
  int  flag = 0;
  int  j, no_lasers = 4;
  int  laser_no[4]  = {1,2,3,4};
  int  iregister[5] = {0,0,0,0,0};
  int  oregister[5] = {0,0,0,0,0};
E 3

D 3
  switch(argc)      /* if no arguments, get  parameter from user */
  { 
   case 1: 
     printf("  default is  all lasers  ");
     break;
   case 2:
     sscanf(argv[1],"%d",&laser_no[0]);
     printf("   laser= %d \n",laser_no[0]);
     no_lasers=1;
     break;
   case 3:
     sscanf(argv[1],"%d",&laser_no[0]);
     sscanf(argv[2],"%d",&laser_no[1]);
     printf("  laser= %d, %d \n",laser_no[0],laser_no[1]);
     no_lasers=2;
     break;
   case 4:
     sscanf(argv[1],"%d",&laser_no[0]);
     sscanf(argv[2],"%d",&laser_no[1]);
     sscanf(argv[3],"%d",&laser_no[2]);
     printf("  laser= %d, %d \n",laser_no[0],laser_no[1],laser_no[2]);
     no_lasers=3;
     break;
   case 5:
     sscanf(argv[1],"%d",&laser_no[0]);
     sscanf(argv[2],"%d",&laser_no[1]);
     sscanf(argv[3],"%d",&laser_no[2]);
     sscanf(argv[4],"%d",&laser_no[3]);
     printf("  laser= %d, %d \n",laser_no[0],laser_no[1],laser_no[2],laser_no[3]);
     no_lasers=4;
     break;
   default: printf( "error in no of args=",argc);
     exit;         
   } 
E 3

D 3


/* intialize DP stuff */

 printf("initializing the connection \n");
 DP_cmd_init("clon10");
 printf("initializing done \n");
 


/* 1   command ****************************   */   
      k=0;
      j=0;
      while(j < no_lasers)
	{
	  k=laser_no[j]-1;
	  printf(" first loop j=%d, laser_no=%d \n",j,laser_no[j]);
	  if(!(laser_no[j] == 1 || laser_no[j] ==2 || laser_no[j] ==3 || laser_no[j] ==4))
	    {
	      printf(" illegal laser no = %d only 1,2,3,4 \n",laser_no[j]);
	      break;
	    }

	  sprintf(exename,"exec input(%d)",laser_no[j]);
	 if(debug) printf("%s \n",exename);
	  if(laser_no[j] == 1)
	    {
	      DP_cmd("camac3",exename,result,111); 
	     if(debug) printf(" camac3 laser %d\n",laser_no[j]);
	    }
	  else
	    {
	       DP_cmd("sc-laser1",exename,result,111);
	     if(debug)  printf(" sc-laser1 laser %d\n",laser_no[j]);
	    }
	  if(debug)printf("%s \n",result);
	  iregister[k]=sbin_to_int(result,0);
	  if(debug)printf(" input register =%d\n",iregister[k]);

	  j++;
E 3
I 3
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
E 3
	}
I 3
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
E 3

D 3
      sleep(1);
      j=0;
      while(j < no_lasers)
	{
	  printf(" second loop j=%d, laser_no=%d \n",j,laser_no[j]);
	  if(!(laser_no[j] == 1 || laser_no[j] ==2 || laser_no[j] ==3 || laser_no[j] ==4))
	    {
	      printf(" illegal laser no = %d only 1,2,3,4 \n",laser_no[j]);
	      break;
	    }
	  k=laser_no[j]-1;
	  sprintf(exename,"exec output(%d)",laser_no[j]);
	  if(debug)printf("%s \n",exename);
	  if(laser_no[j] == 1)
	    {
	      DP_cmd("camac3",exename,result,111); 
	     if(debug) printf(" camac3 laser %d\n",laser_no[j]);
	    }
	  else
	    {
	       DP_cmd("sc-laser1",exename,result,111);
	      if(debug) printf(" sc-laser1 laser %d\n",laser_no[j]);
	    }
	  if(debug)printf("%s \n",result);
	  oregister[k]=sbin_to_int(result,0);
	  if(debug)printf("register= %d\n",oregister[k]);
E 3
I 3
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
E 3

I 3
    printf(" output register = 0x%x \n", oregister[laser_no[j]]);
E 3

D 3

	  j++;
	}

      if(debug)printf(" \n\n");
      j=0;
      while(j < no_lasers)
	{
	  if(!(laser_no[j] == 1 || laser_no[j] ==2 || laser_no[j] ==3 || laser_no[j] ==4))
	    {
	      printf(" illegal laser no = %d only 1,2,3,4 \n",laser_no[j]);
	      break;
	    }

	  k=laser_no[j]-1;

	  printf("laser status for laser number = %d \n",k+1);
	  printf("input register %d \n",iregister[k]);
	  
	  if(  bit_TST(status_pwr_bit[k],iregister[k],debug)  )
	    {
	      printf("status indicates laser power on \n");
	    }
	  else
	    {
	      printf("status indicates laser power off \n");
	    }

	  if(  bit_TST(status_rdy_bit[k],iregister[k],debug)  )
	    {
	      printf("status indicates laser ready \n");
	    }
	  else
	    {
	      printf("status indicates laser not yet ready \n");
	    }

	  if(  bit_TST(status_ena_bit[k],iregister[k],debug)  )
	    {
	      printf("status indicates laser enabled \n");
	    }
	  else
	    {
	      printf("status indicates laser disabled \n");
	    }

	  printf("output register %d \n", oregister[k]);

	  if(  bit_TST(contrl_gas_bit[k],oregister[k],debug)  )
	    {
	      printf("gas control line level on \n");
	    }
	  else
	    {
	      printf("gas control line level off \n");
	    }
	  if(  bit_TST(contrl_pwr_bit[k],oregister[k],debug)  )
	    {
	      printf("pwr control line level on \n");
	    }
	  else
	    {
	      printf("pwr control line level off\n");
	    }
	  printf(" enable and disable are pulses so are not available as status\n\n\n");

	  j++;

	}
 
 exit;   
E 3
I 3
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
E 3
}
D 3

E 3

E 1
