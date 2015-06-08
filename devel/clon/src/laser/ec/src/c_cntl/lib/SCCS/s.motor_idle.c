h52953
s 00018/00017/00072
d D 1.2 04/08/11 17:42:22 sergpozd 3 1
c "camac3" --> char *EC_laser_roc="camac1"
e
s 00000/00000/00000
d R 1.2 00/09/21 14:12:41 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 laser/ec/src/c_cntl/lib/motor_idle.c
e
s 00089/00000/00000
d D 1.1 00/09/21 14:12:40 wolin 1 0
c date and time created 00/09/21 14:12:40 by wolin
e
u
U
f e 0
t
T
I 3
/****************************************************
   motor_idle.c
*****************************************************/

E 3
I 1
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


D 3
 
int motor_idle(int debug)
E 3
I 3
int
motor_idle(int debug)
E 3
{
I 3
 int success=0;
 int flag_loop=0;
 int *position;
 int mstat=0;
 int j;
E 3


D 3
int success=0;
int flag_loop=0;

int *position;
int mstat=0;


int j;



E 3
/* *************status blank start ****************************   */   
  
     sprintf(exename,"exec {laser_motor(\"$1\")}");
     if(debug)printf("\n %s ",exename);
D 3
     success = DP_cmd("camac3",exename,result,111);
E 3
I 3
     success = DP_cmd(EC_laser_roc,exename,result,111);
E 3
     if ( success == 0)
       {
       if(debug)printf("%s \n",result);
       ret_value=parse(result, int_data, real_data, string_data,n1,n2,n3);
       if(debug)printf(" value of string %s \n",&string_data[0][3]);
       sscanf(&string_data[0][3],"%x", &mstat);
       if(debug)printf("    status hex = %x ", mstat);
     
     
           
           
        if( BIT00 & mstat  )   {bit00=1;}  else  {bit00=0;}
   	if( BIT01 & mstat  )    {bit01=1;}  else  {bit01=0;}
	if( BIT02 & mstat  )   {bit02=1;}  else  {bit02=0;}
	if( BIT03 & mstat  )   {bit03=1;}  else  {bit03=0;}
	if( BIT04 & mstat  )   {bit04=1;}  else  {bit04=0;}
	if( BIT05 & mstat  )   {bit05=1;}  else  {bit05=0;}
	if( BIT06 & mstat  )   {bit06=1;}  else  {bit06=0;}
	if( BIT07 & mstat  )   {bit07=1;}  else  {bit07=0;}
	if( BIT08 & mstat  )   {bit08=1;}  else  {bit08=0;}
	if( BIT09 & mstat  )   {bit09=1;}  else  {bit09=0;}
	if( BIT10 & mstat  )   {bit10=1;}  else  {bit10=0;}
	if( BIT11 & mstat  )   {bit11=1;}  else  {bit11=0;}
	if( BIT12 & mstat  )   {bit12=1;}  else  {bit12=0;}
   	if( BIT13 & mstat  )   {bit13=1;}  else  {bit13=0;}
        if( BIT14 & mstat  )   {bit14=1;}  else  {bit14=0;}
        if( BIT15 & mstat  )   {bit15=1;}  else  {bit15=0;}
       
	 
	if(debug)printf(" %d%d%d%d %d%d%d%d %d%d%d%d %d%d%d%d \n ",
	bit15,bit14,bit13,bit12,bit11,bit10,bit09,bit08,bit07,bit06,bit05,bit04,bit03,bit02,bit01,bit00);	
	
	motor_pos(position,debug);
        if(bit00  ){printf("\nmotor running %d \n",*position);j=0;}  
        else       {printf("\nmotor idle %d    \n",*position);j=1;}
 
        if(bit01  ){if(debug)printf("limit error\n");}  
        else       {if(debug)printf("no limit error \n");}

        if(bit02  ){if(debug)printf("emergency stop \n");}  
        else       {if(debug)printf("no emergency stop  OK\n");}
    
 	
   	if(bit03 ) {if(debug)printf("command error \n");}  
        else       {if(debug)printf("no command error \n");}
        
        }
    else
        {
         printf("unable to reach controller in motor_idle ");
	 return(-3);
D 3
        } 
             
  return(j); /*  return 1 if idle*/            
E 3
I 3
        }

  return(j); /*  return 1 if idle */

E 3
}
I 3

E 3
E 1
