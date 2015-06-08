/****************************************************
   motor_idle.c
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
motor_idle(int debug)
{
 int success=0;
 int flag_loop=0;
 int *position;
 int mstat=0;
 int j;


/* *************status blank start ****************************   */   
  
     sprintf(exename,"exec {laser_motor(\"$1\")}");
     if(debug)printf("\n %s ",exename);
     success = DP_cmd(EC_laser_roc,exename,result,111);
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
        }

  return(j); /*  return 1 if idle */

}

