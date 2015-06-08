h59087
s 00491/00517/00072
d D 1.3 04/08/10 18:43:22 sergpozd 4 3
c "camac3" --> char *EC_laser_roc="camac1"
e
s 00004/00001/00585
d D 1.2 03/04/25 09:57:58 giovanet 3 1
c I changed strings to string in the include statement and this made
c no difference I then added 
c #define INIT=1  
c and the program compiles and runs\
c 
e
s 00000/00000/00000
d R 1.2 00/09/21 14:11:08 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 laser/ec/src/c_cntl/laser_motor_test.c
e
s 00586/00000/00000
d D 1.1 00/09/21 14:11:07 wolin 1 0
c date and time created 00/09/21 14:11:07 by wolin
e
u
U
f e 0
t
T
I 1
/* dpS execution of the laser commands giovanetti, Larson jan 99
D 4
 */ 
E 4
I 4
 */
E 4
I 3

D 4
#define INIT=1
E 4
I 4
/*  determines if the variables are defined or referenced  */
#define INIT 1
E 4

E 3
#include <stdio.h>
#include <stdlib.h>
D 3
#include <strings.h>
E 3
I 3
#include <strings.h>  /* changed from strings to string */
E 3
#include <laser.h>
#include <time.h>
#include <sys/times.h>
#include <sys/param.h>
#include <stdlib.h>
#include <laser.h>
#include <bit_defs.h>
I 4
#include "EC_laser_ROC.h"
E 4

D 4
int main()
E 4
I 4
int
main()
E 4
{
D 4
char exename[111];
char result[BUFFER_LENGTH];
int mstat=0;
E 4
I 4
 char exename[111];
 char result[BUFFER_LENGTH];
 int mstat=0;
 int j;
E 4

D 4
int j;
E 4
/* variables for parsing buffer   */
I 4
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
 int   success=0;
E 4

D 4
int int_data[MAX_DATA];
int ret_value;
float   real_data[MAX_DATA];
char string_data[MAX_DATA][80];
int *n1;
int *n2;
int *n3;
int nreal=0;
int nint=0;
int nstring=0;

int success=0;

E 4
/**********  init variables  ***************/
 n1=&nint;
 n2=&nreal;
 n3=&nstring;
 

/* intialize DP stuff */

 printf("initializing the connection \n");
 DP_cmd_init("clon10");
 printf("initializing done \n");
I 4
 
 sprintf(exename,"exec getstatus");
 printf("%s \n",exename);
 success = DP_cmd(EC_laser_roc,exename,result,111);
 ret_value = 0;
 
 if ( success == 0)
   {
     printf(" sucessful excecution \n buffer= %s \n",result);
E 4
     
D 4
     sprintf(exename,"exec getstatus");
     printf("%s \n",exename);
     success = DP_cmd("camac3",exename,result,111);
     ret_value = 0;
E 4
I 4
     ret_value=parse(result, int_data, real_data, string_data,n1,n2,n3); 
E 4
     
D 4
     if ( success == 0)
       {
        printf(" sucessful excecution \n buffer= %s \n",result);
        
        ret_value=parse(result, int_data, real_data, string_data,n1,n2,n3); 
        
        
           
        if( BIT00 & ret_value  )   {bit00=1;}  else  {bit00=0;}
   	if( BIT01 & ret_value  )   {bit01=1;}  else  {bit01=0;}
	if( BIT02 & ret_value  )   {bit02=1;}  else  {bit02=0;}
	if( BIT03 & ret_value  )   {bit03=1;}  else  {bit03=0;}
	if( BIT04 & ret_value  )   {bit04=1;}  else  {bit04=0;}
	if( BIT05 & ret_value  )   {bit05=1;}  else  {bit05=0;}
	if( BIT06 & ret_value  )   {bit06=1;}  else  {bit06=0;}
	if( BIT07 & ret_value  )   {bit07=1;}  else  {bit07=0;}
	if( BIT08 & ret_value  )   {bit08=1;}  else  {bit08=0;}
	if( BIT09 & ret_value  )   {bit09=1;}  else  {bit09=0;}
	if( BIT10 & ret_value  )   {bit10=1;}  else  {bit10=0;}
	if( BIT11 & ret_value  )   {bit11=1;}  else  {bit11=0;}
	if( BIT12 & ret_value  )   {bit12=1;}  else  {bit12=0;}
   	if( BIT13 & ret_value  )   {bit13=1;}  else  {bit13=0;}
        if( BIT14 & ret_value  )   {bit14=1;}  else  {bit14=0;}
        if( BIT15 & ret_value  )   {bit15=1;}  else  {bit15=0;}
       
	if(bit00  ){printf("\nLASER POWER ON: hardware status \n");
	           }  
        else       {printf("\nLASER POWER OFF: hardware status \n");
        	   sprintf(exename,"exec lpoweron");
                   printf(" pwer on wait 30 secs   %s \n",exename);
                   DP_cmd("camac3",exename,result,111);
                   printf("%s \n",result);
    
                   sleep(5);
                    }
    
 	if(bit01  ){printf("LASER ON READY: hardware status \n");
 	    }  
        else
           { 
            if(bit00  ) {printf("LASER NOT YET READY(5 min wait): hardware status \n");}
            else        {printf("LASER NOT READY - NO POWER: hardware status \n");}
           } 
        
   	if(bit02 ) {printf("LASER ENABLED: hardware status \n");
   	            }  
        else       {printf("LASER DISABLED: hardware status \n");
                    }
E 4
I 4
     
     if( BIT00 & ret_value  )   {bit00=1;}  else  {bit00=0;}
     if( BIT01 & ret_value  )   {bit01=1;}  else  {bit01=0;}
     if( BIT02 & ret_value  )   {bit02=1;}  else  {bit02=0;}
     if( BIT03 & ret_value  )   {bit03=1;}  else  {bit03=0;}
     if( BIT04 & ret_value  )   {bit04=1;}  else  {bit04=0;}
     if( BIT05 & ret_value  )   {bit05=1;}  else  {bit05=0;}
     if( BIT06 & ret_value  )   {bit06=1;}  else  {bit06=0;}
     if( BIT07 & ret_value  )   {bit07=1;}  else  {bit07=0;}
     if( BIT08 & ret_value  )   {bit08=1;}  else  {bit08=0;}
     if( BIT09 & ret_value  )   {bit09=1;}  else  {bit09=0;}
     if( BIT10 & ret_value  )   {bit10=1;}  else  {bit10=0;}
     if( BIT11 & ret_value  )   {bit11=1;}  else  {bit11=0;}
     if( BIT12 & ret_value  )   {bit12=1;}  else  {bit12=0;}
     if( BIT13 & ret_value  )   {bit13=1;}  else  {bit13=0;}
     if( BIT14 & ret_value  )   {bit14=1;}  else  {bit14=0;}
     if( BIT15 & ret_value  )   {bit15=1;}  else  {bit15=0;}
     
     if(bit00  ){printf("\nLASER POWER ON: hardware status \n");
     }  
     else       {printf("\nLASER POWER OFF: hardware status \n");
     sprintf(exename,"exec lpoweron");
     printf(" pwer on wait 30 secs   %s \n",exename);
     DP_cmd(EC_laser_roc,exename,result,111);
     printf("%s \n",result);
     
     sleep(5);
     }
     
     if(bit01  ){printf("LASER ON READY: hardware status \n");
     }  
     else
       { 
	 if(bit00  ) {printf("LASER NOT YET READY(5 min wait): hardware status \n");}
	 else        {printf("LASER NOT READY - NO POWER: hardware status \n");}
       } 
     
     if(bit02 ) {printf("LASER ENABLED: hardware status \n");
     }  
     else       {printf("LASER DISABLED: hardware status \n");
     }
     
     if(bit05  ){printf("STEPPER POWER ON: hardware status \n");}  
     else       {printf("STEPPER POWER OFF: hardware status \n");}
     
   }
 else
   {
     printf("unable to reach controller "); 
   }     
 
/*********************************************/ 
E 4

D 4
   	if(bit05  ){printf("STEPPER POWER ON: hardware status \n");}  
        else       {printf("STEPPER POWER OFF: hardware status \n");}

       }
    else
       {
         printf("unable to reach controller "); 
       }     
   
    /*********************************************/ 

E 4
 
/* 1   command ****************************   */   
D 4
  
     sprintf(exename,"exec laser_motor_init");
     printf("%s \n",exename);
     DP_cmd("camac3",exename,result,111);
     printf("%s \n",result);
    
     sleep(1);


E 4
 
D 4




E 4
I 4
 sprintf(exename,"exec laser_motor_init");
 printf("%s \n",exename);
 DP_cmd(EC_laser_roc,exename,result,111);
 printf("%s \n",result);
 
 sleep(1);
 
 
E 4
/********************************9 start *******/     
D 4
    sprintf(exename,"exec {laser_motor(\"$19\")}");
     printf("%s \n",exename);
     DP_cmd("camac3",exename,result,111);
     printf("%s \n",result);
     ret_value=parse(result, int_data, real_data, string_data,n1,n2,n3);
     printf(" value of string %s \n",&string_data[0][3]);
     sscanf(&string_data[0][3],"%d", &mstat);
     printf(" status = %d \n", mstat);
     
     
           
           
        if( BIT00 & mstat  )   {bit00=1;}  else  {bit00=0;}
   	if( BIT01 & mstat  )   {bit01=1;}  else  {bit01=0;}
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
       
	 
	printf(" %d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d \n ",
	bit15,bit14,bit13,bit12,bit11,bit10,bit09,bit08,bit07,bit06,bit05,bit04,bit03,bit02,bit01,bit00);	
	
                 
	if(bit00  ){printf("\ncommunication error \n");}  
        else       {printf("\nno communication errors \n");}
E 4
I 4
 sprintf(exename,"exec {laser_motor(\"$19\")}");
 printf("%s \n",exename);
 DP_cmd(EC_laser_roc,exename,result,111);
 printf("%s \n",result);
 ret_value=parse(result, int_data, real_data, string_data,n1,n2,n3);
 printf(" value of string %s \n",&string_data[0][3]);
 sscanf(&string_data[0][3],"%d", &mstat);
 printf(" status = %d \n", mstat);
E 4
 
D 4
        if(bit01  ){printf("limit error\n");}  
        else       {printf("no limit error \n");}

        if(bit02  ){printf("emergency stop \n");}  
        else       {printf("no emergency stop  OK\n");}
    
 	
   	if(bit03 ) {printf("command error \n");}  
        else       {printf("no command error \n");}

   	if(bit04  ){ 
   	             if(bit05){printf("error both bits 4 and 5 on \n");}
   	             else     {printf("Mode 1 operation closed loop\n");} 
   	            }  
        else       {
                    if(bit05){printf("Mode 2 operation servo  \n");}
   	             else     {printf("Mode 0 operation  open loop\n");} 
                   }
 		
   	if(bit06  ){printf("Stall detected \n");}  
        else       {printf("no stall \n");}

   	if(bit07  ){printf("motor 2 selected \n");}  
        else       {printf("motor 1 selected \n");}
         
        

E 4
I 4
 
 if( BIT00 & mstat  )   {bit00=1;}  else  {bit00=0;}
 if( BIT01 & mstat  )   {bit01=1;}  else  {bit01=0;}
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
 
 
 printf(" %d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d \n ",
	bit15,bit14,bit13,bit12,bit11,bit10,bit09,bit08,
	bit07,bit06,bit05,bit04,bit03,bit02,bit01,bit00);	
 
 
 if(bit00  ){printf("\ncommunication error \n");}  
 else       {printf("\nno communication errors \n");}
 
 if(bit01  ){printf("limit error\n");}  
 else       {printf("no limit error \n");}
 
 if(bit02  ){printf("emergency stop \n");}  
 else       {printf("no emergency stop  OK\n");}
 
 
 if(bit03 ) {printf("command error \n");}  
 else       {printf("no command error \n");}
 
 if(bit04  ){ 
   if(bit05){printf("error both bits 4 and 5 on \n");}
   else     {printf("Mode 1 operation closed loop\n");} 
 }  
 else       {
   if(bit05){printf("Mode 2 operation servo  \n");}
   else     {printf("Mode 0 operation  open loop\n");} 
 }
 
 if(bit06  ){printf("Stall detected \n");}  
 else       {printf("no stall \n");}
 
 if(bit07  ){printf("motor 2 selected \n");}  
 else       {printf("motor 1 selected \n");}
 
 
 
E 4
/*************** 9 end command  ****************/ 
D 4

printf("_______________ go to CCW __________________ 8 \n");

E 4
 
D 4
    sprintf(exename,"exec {laser_motor(\"$18\")}");
     printf("%s \n",exename);
     DP_cmd("camac3",exename,result,111);
     printf("%s \n",result);

     
   j=0; 
   do{  
E 4
I 4
 printf("_______________ go to CCW __________________ 8 \n");
E 4
 
 
D 4
/* *************status blank start ****************************   */   
  
     sprintf(exename,"exec {laser_motor(\"$1\")}");
     printf("%s \n",exename);
     DP_cmd("camac3",exename,result,111);
     printf("%s \n",result);
        ret_value=parse(result, int_data, real_data, string_data,n1,n2,n3);
     printf(" value of string %s \n",&string_data[0][3]);
     sscanf(&string_data[0][3],"%d", &mstat);
     printf(" status = %d \n", mstat);
     
     
           
           
        if( BIT00 & mstat  )   {bit00=1;}  else  {bit00=0;}
   	if( BIT01 & mstat  )   {bit01=1;}  else  {bit01=0;}
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
       
	 
	printf(" %d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d \n ",
	bit15,bit14,bit13,bit12,bit11,bit10,bit09,bit08,bit07,bit06,bit05,bit04,bit03,bit02,bit01,bit00);	
	
     
      if(bit00  ){printf("\nmotor running \n");}  
        else       {printf("\nmotor idle\n");}
E 4
I 4
 sprintf(exename,"exec {laser_motor(\"$18\")}");
 printf("%s \n",exename);
 DP_cmd(EC_laser_roc,exename,result,111);
 printf("%s \n",result);
E 4
 
D 4
        if(bit01  ){printf("limit error\n");}  
        else       {printf("no limit error \n");}
E 4
I 4
 
 j=0; 
 do{  
   
   
/* *************status blank start ****************************   */   
   
   sprintf(exename,"exec {laser_motor(\"$1\")}");
   printf("%s \n",exename);
   DP_cmd(EC_laser_roc,exename,result,111);
   printf("%s \n",result);
   ret_value=parse(result, int_data, real_data, string_data,n1,n2,n3);
   printf(" value of string %s \n",&string_data[0][3]);
   sscanf(&string_data[0][3],"%d", &mstat);
   printf(" status = %d \n", mstat);
   
   
   if( BIT00 & mstat  )   {bit00=1;}  else  {bit00=0;}
   if( BIT01 & mstat  )   {bit01=1;}  else  {bit01=0;}
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
   
   
   printf(" %d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d \n ",
	  bit15,bit14,bit13,bit12,bit11,bit10,bit09,bit08,
	  bit07,bit06,bit05,bit04,bit03,bit02,bit01,bit00);	
   
   
   if(bit00  ){printf("\nmotor running \n");}  
   else       {printf("\nmotor idle\n");}
   
   if(bit01  ){printf("limit error\n");}  
   else       {printf("no limit error \n");}
   
   if(bit02  ){printf("emergency stop \n");}  
   else       {printf("no emergency stop  OK\n");}
   
   
   if(bit03 ) {printf("command error \n");}  
   else       {printf("no command error \n");}
E 4

D 4
        if(bit02  ){printf("emergency stop \n");}  
        else       {printf("no emergency stop  OK\n");}
    
 	
   	if(bit03 ) {printf("command error \n");}  
        else       {printf("no command error \n");}

E 4
/*************** status blank end *************************/

/************************  CL *****************************/
D 4
    sprintf(exename,"exec {laser_motor(\"$1CL\")}");
     printf("%s \n",exename);
     DP_cmd("camac3",exename,result,111);
     printf("%s \n",result);
     ret_value=parse(result, int_data, real_data, string_data,n1,n2,n3);
     printf(" value of string %s \n",&string_data[0][3]);
     sscanf(&string_data[0][3],"%d", &mstat);
     printf(" status = %d \n", mstat);
     
     
           
           
        if( BIT00 & mstat  )   {bit00=1;}  else  {bit00=0;}
   	if( BIT01 & mstat  )   {bit01=1;}  else  {bit01=0;}
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
       
	 
	printf(" %d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d \n ",
	bit15,bit14,bit13,bit12,bit11,bit10,bit09,bit08,bit07,bit06,bit05,bit04,bit03,bit02,bit01,bit00);	
E 4
I 4
   sprintf(exename,"exec {laser_motor(\"$1CL\")}");
   printf("%s \n",exename);
   DP_cmd(EC_laser_roc,exename,result,111);
   printf("%s \n",result);
   ret_value=parse(result, int_data, real_data, string_data,n1,n2,n3);
   printf(" value of string %s \n",&string_data[0][3]);
   sscanf(&string_data[0][3],"%d", &mstat);
   printf(" status = %d \n", mstat);
E 4

D 4
	if(bit00  ){printf("\nClW limit reached \n");}  
        else       {printf("\nNO CW limit \n");}
 
        if(bit01  ){printf("CCW limit reached\n");}  
        else       {printf("no CCW limt \n");}
E 4
I 4
   
   if( BIT00 & mstat  )   {bit00=1;}  else  {bit00=0;}
   if( BIT01 & mstat  )   {bit01=1;}  else  {bit01=0;}
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
   
   
   printf(" %d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d \n ",
	  bit15,bit14,bit13,bit12,bit11,bit10,bit09,bit08,
	  bit07,bit06,bit05,bit04,bit03,bit02,bit01,bit00);	
   
   if(bit00  ){printf("\nClW limit reached \n");}  
   else       {printf("\nNO CW limit \n");}
   
   if(bit01  ){printf("CCW limit reached\n");}  
   else       {printf("no CCW limt \n");}
   
   if(bit04  ){printf("ORGLS reached\n");}  
   else       {printf("no ORGLS\n");}
   
   
   if(bit05 ) {printf("ALM em stop on \n");}  
   else       {printf("no ALM \n");}
   
   
   if(bit06  ){printf("EZ \n");}  
   else       {printf("no EZ \n");}
E 4

D 4
        if(bit04  ){printf("ORGLS reached\n");}  
        else       {printf("no ORGLS\n");}
    
 	
   	if(bit05 ) {printf("ALM em stop on \n");}  
        else       {printf("no ALM \n");}

   	
   	if(bit06  ){printf("EZ \n");}  
        else       {printf("no EZ \n");}
         
        
	
E 4
I 4
   
E 4
/*****************************   CL end **************/

/********************************9 start *******/     
D 4
    sprintf(exename,"exec {laser_motor(\"$19\")}");
     printf("%s \n",exename);
     DP_cmd("camac3",exename,result,111);
     printf("%s \n",result);
     ret_value=parse(result, int_data, real_data, string_data,n1,n2,n3);
     printf(" value of string %s \n",&string_data[0][3]);
     sscanf(&string_data[0][3],"%d", &mstat);
     printf(" status = %d \n", mstat);
     
     
           
           
        if( BIT00 & mstat  )   {bit00=1;}  else  {bit00=0;}
   	if( BIT01 & mstat  )   {bit01=1;}  else  {bit01=0;}
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
       
	 
	printf(" %d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d \n ",
	bit15,bit14,bit13,bit12,bit11,bit10,bit09,bit08,bit07,bit06,bit05,bit04,bit03,bit02,bit01,bit00);	
	
                 
	if(bit00  ){printf("\ncommunication error \n");}  
        else       {printf("\nno communication errors \n");}
 
        if(bit01  ){printf("limit error\n");}  
        else       {printf("no limit error \n");}
E 4
I 4
   sprintf(exename,"exec {laser_motor(\"$19\")}");
   printf("%s \n",exename);
   DP_cmd(EC_laser_roc,exename,result,111);
   printf("%s \n",result);
   ret_value=parse(result, int_data, real_data, string_data,n1,n2,n3);
   printf(" value of string %s \n",&string_data[0][3]);
   sscanf(&string_data[0][3],"%d", &mstat);
   printf(" status = %d \n", mstat);
   
   
   if( BIT00 & mstat  )   {bit00=1;}  else  {bit00=0;}
   if( BIT01 & mstat  )   {bit01=1;}  else  {bit01=0;}
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
   
   
   printf(" %d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d \n ",
	  bit15,bit14,bit13,bit12,bit11,bit10,bit09,bit08,
	  bit07,bit06,bit05,bit04,bit03,bit02,bit01,bit00);	
   
   
   if(bit00  ){printf("\ncommunication error \n");}  
   else       {printf("\nno communication errors \n");}
   
   if(bit01  ){printf("limit error\n");}  
   else       {printf("no limit error \n");}
   
   if(bit02  ){printf("emergency stop \n");}  
   else       {printf("no emergency stop  OK\n");}
   
   
   if(bit03 ) {printf("command error \n");}  
   else       {printf("no command error \n");}
   
   if(bit04  ){ 
     if(bit05){printf("error both bits 4 and 5 on \n");}
     else     {printf("Mode 1 operation closed loop\n");} 
   }  
   else       {
     if(bit05){printf("Mode 2 operation servo  \n");}
     else     {printf("Mode 0 operation  open loop\n");} 
   }
   
   if(bit06  ){printf("Stall detected \n");}  
   else       {printf("no stall \n");}
   
   if(bit07  ){printf("motor 2 selected \n");}  
   else       {printf("motor 1 selected \n");}
E 4

D 4
        if(bit02  ){printf("emergency stop \n");}  
        else       {printf("no emergency stop  OK\n");}
    
 	
   	if(bit03 ) {printf("command error \n");}  
        else       {printf("no command error \n");}

   	if(bit04  ){ 
   	             if(bit05){printf("error both bits 4 and 5 on \n");}
   	             else     {printf("Mode 1 operation closed loop\n");} 
   	            }  
        else       {
                    if(bit05){printf("Mode 2 operation servo  \n");}
   	             else     {printf("Mode 0 operation  open loop\n");} 
                   }
 		
   	if(bit06  ){printf("Stall detected \n");}  
        else       {printf("no stall \n");}

   	if(bit07  ){printf("motor 2 selected \n");}  
        else       {printf("motor 1 selected \n");}
         
        

E 4
I 4
   
E 4
/*************** 9 end command  ****************/ 
         
I 4
   if(j==1) sleep(30);
   j++;
 } while(j < 3);
E 4
 
D 4
    if(j==1) sleep(30);
     j++;
    } while(j < 3);

     
  
E 4
 
D 4
printf("_______________ go to CW __________________7 \n");

    sprintf(exename,"exec {laser_motor(\"$17\")}");
     printf("%s \n",exename);
     DP_cmd("camac3",exename,result,111);
     printf("%s \n",result);
      
     
   j=0; 
   do{  
E 4
I 4
 printf("_______________ go to CW __________________7 \n");
E 4
 
I 4
 sprintf(exename,"exec {laser_motor(\"$17\")}");
 printf("%s \n",exename);
 DP_cmd(EC_laser_roc,exename,result,111);
 printf("%s \n",result);
E 4
 
D 4
/* *************status blank start ****************************   */   
  
     sprintf(exename,"exec {laser_motor(\"$1\")}");
     printf("%s \n",exename);
     DP_cmd("camac3",exename,result,111);
     printf("%s \n",result);
        ret_value=parse(result, int_data, real_data, string_data,n1,n2,n3);
     printf(" value of string %s \n",&string_data[0][3]);
     sscanf(&string_data[0][3],"%d", &mstat);
     printf(" status = %d \n", mstat);
     
     
           
           
        if( BIT00 & mstat  )   {bit00=1;}  else  {bit00=0;}
   	if( BIT01 & mstat  )   {bit01=1;}  else  {bit01=0;}
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
       
	 
	printf(" %d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d \n ",
	bit15,bit14,bit13,bit12,bit11,bit10,bit09,bit08,bit07,bit06,bit05,bit04,bit03,bit02,bit01,bit00);	
	
     
      if(bit00  ){printf("\nmotor running \n");}  
        else       {printf("\nmotor idle\n");}
E 4
 
D 4
        if(bit01  ){printf("limit error\n");}  
        else       {printf("no limit error \n");}

        if(bit02  ){printf("emergency stop \n");}  
        else       {printf("no emergency stop  OK\n");}
    
 	
   	if(bit03 ) {printf("command error \n");}  
        else       {printf("no command error \n");}

E 4
I 4
 j=0; 
 do{  
   
/**************status blank start ****************************   */   
   
   sprintf(exename,"exec {laser_motor(\"$1\")}");
   printf("%s \n",exename);
   DP_cmd(EC_laser_roc,exename,result,111);
   printf("%s \n",result);
   ret_value=parse(result, int_data, real_data, string_data,n1,n2,n3);
   printf(" value of string %s \n",&string_data[0][3]);
   sscanf(&string_data[0][3],"%d", &mstat);
   printf(" status = %d \n", mstat);
   
   
   if( BIT00 & mstat  )   {bit00=1;}  else  {bit00=0;}
   if( BIT01 & mstat  )   {bit01=1;}  else  {bit01=0;}
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
   
   
   printf(" %d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d \n ",
	  bit15,bit14,bit13,bit12,bit11,bit10,bit09,bit08,
	  bit07,bit06,bit05,bit04,bit03,bit02,bit01,bit00);	
   
   
   if(bit00  ){printf("\nmotor running \n");}  
   else       {printf("\nmotor idle\n");}
   
   if(bit01  ){printf("limit error\n");}  
   else       {printf("no limit error \n");}
   
   if(bit02  ){printf("emergency stop \n");}  
   else       {printf("no emergency stop  OK\n");}
   
   
   if(bit03 ) {printf("command error \n");}  
   else       {printf("no command error \n");}
   
E 4
/*************** status blank end *************************/

/************************  CL *****************************/
D 4
    sprintf(exename,"exec {laser_motor(\"$1CL\")}");
     printf("%s \n",exename);
     DP_cmd("camac3",exename,result,111);
     printf("%s \n",result);
     ret_value=parse(result, int_data, real_data, string_data,n1,n2,n3);
     printf(" value of string %s \n",&string_data[0][3]);
     sscanf(&string_data[0][3],"%d", &mstat);
     printf(" status = %d \n", mstat);
     
     
           
           
        if( BIT00 & mstat  )   {bit00=1;}  else  {bit00=0;}
   	if( BIT01 & mstat  )   {bit01=1;}  else  {bit01=0;}
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
       
	 
	printf(" %d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d \n ",
	bit15,bit14,bit13,bit12,bit11,bit10,bit09,bit08,bit07,bit06,bit05,bit04,bit03,bit02,bit01,bit00);	

	if(bit00  ){printf("\nClW limit reached \n");}  
        else       {printf("\nNO CW limit \n");}
 
        if(bit01  ){printf("CCW limit reached\n");}  
        else       {printf("no CCW limt \n");}

        if(bit04  ){printf("ORGLS reached\n");}  
        else       {printf("no ORGLS\n");}
    
 	
   	if(bit05 ) {printf("ALM em stop on \n");}  
        else       {printf("no ALM \n");}

   	
   	if(bit06  ){printf("EZ \n");}  
        else       {printf("no EZ \n");}
         
        
	
E 4
I 4
   sprintf(exename,"exec {laser_motor(\"$1CL\")}");
   printf("%s \n",exename);
   DP_cmd(EC_laser_roc,exename,result,111);
   printf("%s \n",result);
   ret_value=parse(result, int_data, real_data, string_data,n1,n2,n3);
   printf(" value of string %s \n",&string_data[0][3]);
   sscanf(&string_data[0][3],"%d", &mstat);
   printf(" status = %d \n", mstat);
   
   
   if( BIT00 & mstat  )   {bit00=1;}  else  {bit00=0;}
   if( BIT01 & mstat  )   {bit01=1;}  else  {bit01=0;}
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
   
   
   printf(" %d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d \n ",
	  bit15,bit14,bit13,bit12,bit11,bit10,bit09,bit08,
	  bit07,bit06,bit05,bit04,bit03,bit02,bit01,bit00);	
   
   if(bit00  ){printf("\nClW limit reached \n");}  
   else       {printf("\nNO CW limit \n");}
   
   if(bit01  ){printf("CCW limit reached\n");}  
   else       {printf("no CCW limt \n");}
   
   if(bit04  ){printf("ORGLS reached\n");}  
   else       {printf("no ORGLS\n");}
   
   
   if(bit05 ) {printf("ALM em stop on \n");}  
   else       {printf("no ALM \n");}
   
   
   if(bit06  ){printf("EZ \n");}  
   else       {printf("no EZ \n");}
   
   
E 4
/*****************************   CL end **************/
D 4

E 4
I 4
   
E 4
/********************************9 start *******/     
D 4
    sprintf(exename,"exec {laser_motor(\"$19\")}");
     printf("%s \n",exename);
     DP_cmd("camac3",exename,result,111);
     printf("%s \n",result);
     ret_value=parse(result, int_data, real_data, string_data,n1,n2,n3);
     printf(" value of string %s \n",&string_data[0][3]);
     sscanf(&string_data[0][3],"%d", &mstat);
     printf(" status = %d \n", mstat);
E 4
I 4
   sprintf(exename,"exec {laser_motor(\"$19\")}");
   printf("%s \n",exename);
   DP_cmd(EC_laser_roc,exename,result,111);
   printf("%s \n",result);
   ret_value=parse(result, int_data, real_data, string_data,n1,n2,n3);
   printf(" value of string %s \n",&string_data[0][3]);
   sscanf(&string_data[0][3],"%d", &mstat);
   printf(" status = %d \n", mstat);
E 4
     
D 4
     
           
           
        if( BIT00 & mstat  )   {bit00=1;}  else  {bit00=0;}
   	if( BIT01 & mstat  )   {bit01=1;}  else  {bit01=0;}
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
       
	 
	printf(" %d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d \n ",
	bit15,bit14,bit13,bit12,bit11,bit10,bit09,bit08,bit07,bit06,bit05,bit04,bit03,bit02,bit01,bit00);	
	
                 
	if(bit00  ){printf("\ncommunication error \n");}  
        else       {printf("\nno communication errors \n");}
 
        if(bit01  ){printf("limit error\n");}  
        else       {printf("no limit error \n");}

        if(bit02  ){printf("emergency stop \n");}  
        else       {printf("no emergency stop  OK\n");}
    
 	
   	if(bit03 ) {printf("command error \n");}  
        else       {printf("no command error \n");}

   	if(bit04  ){ 
   	             if(bit05){printf("error both bits 4 and 5 on \n");}
   	             else     {printf("Mode 1 operation closed loop\n");} 
   	            }  
        else       {
                    if(bit05){printf("Mode 2 operation servo  \n");}
   	             else     {printf("Mode 0 operation  open loop\n");} 
                   }
 		
   	if(bit06  ){printf("Stall detected \n");}  
        else       {printf("no stall \n");}

   	if(bit07  ){printf("motor 2 selected \n");}  
        else       {printf("motor 1 selected \n");}
         
E 4
I 4
   
   if( BIT00 & mstat  )   {bit00=1;}  else  {bit00=0;}
   if( BIT01 & mstat  )   {bit01=1;}  else  {bit01=0;}
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
   
   
   printf(" %d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d \n ",
	  bit15,bit14,bit13,bit12,bit11,bit10,bit09,bit08,
	  bit07,bit06,bit05,bit04,bit03,bit02,bit01,bit00);	
   
   
   if(bit00  ){printf("\ncommunication error \n");}  
   else       {printf("\nno communication errors \n");}
   
   if(bit01  ){printf("limit error\n");}  
   else       {printf("no limit error \n");}
   
   if(bit02  ){printf("emergency stop \n");}  
   else       {printf("no emergency stop  OK\n");}
   
   
   if(bit03 ) {printf("command error \n");}  
   else       {printf("no command error \n");}
   
   if(bit04  ){ 
     if(bit05){printf("error both bits 4 and 5 on \n");}
     else     {printf("Mode 1 operation closed loop\n");} 
   }  
   else       {
     if(bit05){printf("Mode 2 operation servo  \n");}
     else     {printf("Mode 0 operation  open loop\n");} 
   }
   
   if(bit06  ){printf("Stall detected \n");}  
   else       {printf("no stall \n");}
   
   if(bit07  ){printf("motor 2 selected \n");}  
   else       {printf("motor 1 selected \n");}
E 4
        

/*************** 9 end command  ****************/ 
         
D 4
 
      if(j==1) sleep(30);
     j++;
    } while(j < 3);
E 4
I 4
   if(j==1) sleep(30);
   j++;
 } while(j < 3);
E 4

D 4
     
 
 
    
E 4
}
D 4

E 4

E 1
