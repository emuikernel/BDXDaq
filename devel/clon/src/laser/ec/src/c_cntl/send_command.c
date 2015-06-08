/* dpS execution of the laser commands giovanetti, Larson jan 99
 */

/*  determines if the variables are defined or referenced    */ 
#define INIT 1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <laser.h>
#include "EC_laser_ROC.h"

int
main (int argc, char *argv[])
{
 char  exename[111];
 char  result[BUFFER_LENGTH];
 int   int_data[MAX_DATA];
 int   ret_value;
 float real_data[MAX_DATA];
 char  string_data[MAX_DATA][80];
 int   *n1;
 int   *n2;
 int   *n3;
 int   nreal=0;
 int   nint=0;
 int   nstring=0;
 int   debug=0;
 int   j=0;
 char  command_string[80];
 int   success=1;

/* intialize DP stuff */

 printf("initializing the connection \n");
 DP_cmd_init("clon10");
 printf("initializing done \n");
 
 n1=&nint;
 n2=&nreal;
 n3=&nstring;

 if (argc < 2) 
   {
     /* if no arguments, get the file parameters from user */
     printf("  enter command string :");
     scanf("%s",command_string);   
   }
 else
   {
     /* if arguments, use them as values for freq and number
	(after casting them to the declared types)          */
     sprintf(command_string,"%s",argv[1]);
   }
 
 /* 1   command ****************************   */ 
     
 sprintf(exename,"exec %s",command_string);
 printf("%s \n",exename);
 success = DP_cmd(EC_laser_roc,exename,result,111);
 ret_value = 0;
 
 if ( success == 0)
   {
     printf(" sucessful excecution \n buffer= %s \n",result);
     ret_value= parse(result, int_data, real_data, string_data,n1,n2,n3);
   }
 else
   {
     printf("unable to reach controller "); 
   }     
 
 printf(" \n number of real %d \n", nreal);
 for(j=0; j < nreal; j++){printf(" real value %d  = %f \n", j+1 , real_data[j]);}
 
 
 printf(" \n number of int %d \n", nint);
 for(j=0; j < nint; j++){printf(" integer value %d  = %d \n", j+1 , int_data[j]);}
 
 printf(" \n number of strings %d \n", nstring);
 for(j=0; j < nstring; j++){printf(" string value %d  = %s \n", j+1 , string_data[j]);} 
 
 printf(" \n \n value of the function is = %d \n",ret_value); 

}

