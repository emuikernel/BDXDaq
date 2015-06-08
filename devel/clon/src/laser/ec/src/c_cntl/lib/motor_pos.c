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
motor_pos(int *position, int debug)
{
 int success=0;
 int flag_loop=0;
 int mstat=0;
 int j;


/* *************status blank start ****************************   */   
  
     sprintf(exename,"exec {laser_motor(\"$16\")}");
     if(debug)printf("\n %s ",exename);
     success = DP_cmd(EC_laser_roc,exename,result,111);
     if ( success == 0)
       {
       if(debug)printf("%s \n",result);
       ret_value=parse(result, int_data, real_data, string_data,n1,n2,n3);
       if(debug)printf(" value of string %s \n",&string_data[0][3]);
       sscanf(&string_data[0][3],"%d", &mstat);
       if(debug)printf("   position = %d ", mstat);
        }
    else
        {
         printf("unable to reach controller in motor_pos ");
	 return(-3);
        }
  *position=mstat;

  return(1); /*  return 1 if idle */

}

