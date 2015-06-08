/****************************************************
   $1R resets the PLR
*****************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <laser.h>
#include <time.h>
#include <sys/times.h>
#include <sys/param.h>
#include <stdlib.h>
#include <laser.h>
#include <bit_defs.h>
#include <parse.h>
#include "EC_laser_ROC.h"


int
motor_INIT(int debug)
{
 int success=0;
 int flag_loop=0;
 int mstat=0;
 int j;


/* ************set status to bidirectional ***************************   */   
     sprintf(exename,"exec {laser_motor(\"$1EC1\")}");
     if(debug)printf("\n %s ",exename);
     success = DP_cmd(EC_laser_roc,exename,result,111);
     if ( success == 0)
       {
       if(debug)printf("%s \n",result);
       ret_value=parse(result, int_data, real_data, string_data,n1,n2,n3);
       if(debug)printf(" value of string %s \n",&string_data[0][0]);
       if (strstr(string_data[0],">") != NULL) 
	 {
	   if(debug)printf("    found > motor ok in motor_INIT \n");
	 }
       else
	 {
	   if(debug) printf(" bad response from motor in prog motor_INIT \n");
	   return(-2);
	 }
        }
    else
        {
         printf("unable to reach controller in program motor_INIT ");
	 return(-3);
        } 
 /* ************clock to 2 clck (default) set for certainty **********/   
     sprintf(exename,"exec {laser_motor(\"$1ED1\")}");
     if(debug)printf("\n %s ",exename);
     success = DP_cmd(EC_laser_roc,exename,result,111);
     if ( success == 0)
       {
        if(debug)printf("%s \n",result);
       ret_value=parse(result, int_data, real_data, string_data,n1,n2,n3);
       if(debug)printf(" value of string %s \n",&string_data[0][0]);
       if (strstr(string_data[0],">") != NULL) 
	 {
	   if(debug)printf("    found > motor o kin motor_INIT \n");
	 }
       else
	 {
	   if(debug) printf(" bad response from motor in motor_INIT \n");
	   return(-2);
	 }

        }
    else
        {
         printf("unable to reach controller in program motor_INIT ");
	 return(-3);
        } 

     printf(" move to CCW limit \n ");
     limit("CCW",debug);
     printf("reset motor PLR to 0 \n ");

/* ************reset the location **********/   
     sprintf(exename,"exec {laser_motor(\"$1R\")}");
     if(debug)printf("\n %s ",exename);
     success = DP_cmd(EC_laser_roc,exename,result,111);
     if ( success == 0)
       {
        if(debug)printf("%s \n",result);
       ret_value=parse(result, int_data, real_data, string_data,n1,n2,n3);
       if(debug)printf(" value of string %s \n",&string_data[0][0]);
       if (strstr(string_data[0],">") != NULL) 
	 {
	   if(debug)printf("    found > motor ok in motor_INIT \n");
	 }
       else
	 {
	   if(debug) printf(" bad response from motor in motor_INIT \n");
	   return(-2);
	 }

        }
    else
        {
         printf("unable to reach controller in motor_INIT");
	 return(-3);
        }

  return(1); /*  return 1 okay */

}

