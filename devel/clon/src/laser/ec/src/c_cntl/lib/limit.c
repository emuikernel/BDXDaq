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
limit(char* limit, int debug)
{
 int success=0;
 int flag_loop=0;
 int flag_CW_limit=0;
 int flag_CCW_limit=0;
 int flag_CCW=0;
 int flag_CW=0;
 int mstat=0;
 int j;


/************ determine which limit to go to *************/
 if( ( strcmp(limit, "CCW") == 0 ) || ( strcmp(limit, "ccw") == 0 ) )
   { 
     if(debug)printf(" CCW limit requested =%s\n",limit);
     flag_CCW=1;
     flag_CW=0;
   } 
 else if  ( ( strcmp(limit, "CW") == 0 ) || ( strcmp(limit, "cw") == 0 ) )  
   { 
     if(debug)printf(" CW limit requested =%s\n",limit);
     flag_CW=1;
     flag_CCW=0;
   } 
 else
   {
     printf(" no limit requested error \n");
     return(-106);
   }
 
 switch(flag_CW)
   {
   case 0:
     {
       while( flag_CCW_limit == 0)
	 {
	   sprintf(exename,"exec {laser_motor(\"$18\")}");
	   if(debug)printf("%s \n",exename);
	   success = DP_cmd(EC_laser_roc,exename,result,111);
	   if (success == 0)
	     {
	    if(debug)printf("%s \n",result);
	    while(!motor_idle(debug)) { }
	    if(debug)printf(" going CCW motor stopped\n");
	    flag_CCW_limit=limit_TST("CCW",debug);   
	     }
	   else
	     {
	       printf("unable to reach controller in program limit");
	       return(-3);
	     }
	 } 
       return(1);
     }
   case 1:
     {
       while( flag_CW_limit == 0)
	 {
	   sprintf(exename,"exec {laser_motor(\"$17\")}");
	   if(debug)printf("%s \n",exename);
	   success = DP_cmd(EC_laser_roc,exename,result,111);
	   if (success == 0)
	     {
	    if(debug)printf("%s \n",result);
	    while(!motor_idle(debug)) { }
	    if(debug)printf(" going CW motor stopped\n");
	    flag_CW_limit=limit_TST("CW",debug);   
	     }
	   else
	     {
	       printf("unable to reach controller in program limit ");
	       return(-3);
	     }
	 }
       return(1);
     }
   default: 
     {
       printf(" error wrong flag value \n");
       return(-105);
     }
   }
}

