h44248
s 00011/00012/00094
d D 1.3 04/08/11 17:41:45 sergpozd 4 3
c "camac3" --> char *EC_laser_roc="camac1"
e
s 00006/00005/00100
d D 1.2 04/08/11 17:23:56 sergpozd 3 1
c "camac3" --> char *EC_laser_roc="camac1"
e
s 00000/00000/00000
d R 1.2 00/09/21 14:12:41 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 laser/ec/src/c_cntl/lib/limit.c
e
s 00105/00000/00000
d D 1.1 00/09/21 14:12:40 wolin 1 0
c date and time created 00/09/21 14:12:40 by wolin
e
u
U
f e 0
t
T
I 1
D 3
/****************************************************** 
E 3
I 3
/***********************************************************************
E 3
   I will assume that the port has been opend and that the laser is on
   I will not check wether these things are ok.
D 3
   ******************************************************/
E 3
I 3
************************************************************************/
E 3

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
I 3
#include "EC_laser_ROC.h"
E 3

D 4
 
int limit(char* limit, int debug)
E 4
I 4

int
limit(char* limit, int debug)
E 4
{
I 4
 int success=0;
 int flag_loop=0;
 int flag_CW_limit=0;
 int flag_CCW_limit=0;
 int flag_CCW=0;
 int flag_CW=0;
 int mstat=0;
 int j;
E 4


D 4
int success=0;
int flag_loop=0;
int flag_CW_limit=0;
int flag_CCW_limit=0;
int flag_CCW=0;
int flag_CW=0;

int mstat=0;
int j;

E 4
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
D 3
	   success = DP_cmd("camac3",exename,result,111);
E 3
I 3
	   success = DP_cmd(EC_laser_roc,exename,result,111);
E 3
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
D 3
	   success = DP_cmd("camac3",exename,result,111);
E 3
I 3
	   success = DP_cmd(EC_laser_roc,exename,result,111);
E 3
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
D 3

E 3
}
I 3

E 3
E 1
