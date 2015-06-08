/***********************************************************************
   I will assume that the port has been opend and that the laser is on
   I will not check wether these things are ok.
************************************************************************/

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
limit_TST(char* limit, int debug)
{
 int success=0;
 int flag_loop=0;
 int flag_CW_limit=0;
 int flag_CCW_limit=0;
 int flag_CCW=0;
 int flag_CW=0;
 int bit_CCW=1;
 int bit_CW=0;
 int bit_to_test;
 int mstat=0;
 int j;


/************ determine which limit to go to *************/
 if( ( strcmp(limit, "CCW") == 0 ) || ( strcmp(limit, "ccw") == 0 ) )
   { 
     if(debug)printf(" CCW limit requested =%s\n",limit);
     bit_to_test=bit_CCW;
     flag_CCW=1;
     flag_CW=0;
   } 
 else if  ( ( strcmp(limit, "CW") == 0 ) || ( strcmp(limit, "cw") == 0 ) )  
   { 
     if(debug)printf(" CW limit requested =%s\n",limit);
     bit_to_test=bit_CW;
     flag_CW=1;
     flag_CCW=0;
   } 
 else
   {
     printf(" no limit requested error prog= limit_TST \n");
     return(-107);
   } 
/* *************status blank start ****************************   */   
    j=0;
    sprintf(exename,"exec {laser_motor(\"$1CL\")}");
    if(debug)printf("%s \n",exename);
    success = DP_cmd(EC_laser_roc,exename,result,111);
    if(debug)printf("%s \n",result);
    if (success == 0)
      {
       ret_value=parse(result, int_data, real_data, string_data,n1,n2,n3);  
       if(debug)printf(" value of string %s \n",&string_data[0][3]);
       sscanf(&string_data[0][3],"%x", &mstat);
       if(debug)printf("    status hex = %x ", mstat);
       j=bit_TST(bit_to_test, mstat,debug);
       }
    else
      {
       printf("unable to reach controller in limit_TST ");
       return(-3);
      } 
             
  return(j); /*  return 1 if limit reached 0 if not */            

}

