h45552
s 00025/00022/00623
d D 1.2 04/08/11 14:28:36 sergpozd 3 1
c "camac3" --> char *EC_laser_roc="camac1"
e
s 00000/00000/00000
d R 1.2 00/09/21 14:11:11 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 laser/ec/src/c_cntl/status.c
e
s 00645/00000/00000
d D 1.1 00/09/21 14:11:10 wolin 1 0
c date and time created 00/09/21 14:11:10 by wolin
e
u
U
f e 0
t
T
I 1
/* dpS execution of the laser commands giovanetti, Larson jan 99
 */
D 3
 
 /*  determines if the variables are defined or referenced    */ 
#define INIT=1
 
E 3
I 3

 /*  determines if the variables are defined or referenced    */
#define INIT 1

E 3
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <laser.h>
#include <bit_defs.h>
I 3
#include "EC_laser_ROC.h"
E 3


D 3
int main (int argc, char *argv[])
E 3
I 3
int
main (int argc, char *argv[])
E 3
{
char exename[111];
char result[BUFFER_LENGTH];

int period_10usec;
int numberOfPulses;
float freq; 
int exit_value=0;

D 3

E 3
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

/***********  define the software status  ****************/
/*     12   11       10         9      8      7     6        5      4   3   2    1     0 
      gas-hppulser-lmotinit-calinit-get_d-loadp-extcylce-steppower-make-NA-len-lready-lpow   */

int soft_status =0;   

int debug=0;
int j=0;
char command_string[80];
int success=1;

/* intialize DP stuff */

 printf("initializing the connection \n");
 DP_cmd_init("clon10");
 printf("initializing done \n\n");
 
 n1=&nint;
 n2=&nreal;
 n3=&nstring;
  
  
  
    
/***************************
    
    int soft_status_lpower=0;  -- power on =1 power off =0 
    int soft_status_laser=0;   -- laser enabled =1 laser disabled =0 
    int soft_status_gas=0;     -- gas on =1 gass off =0 
    int soft_status_hp_pul=0;  -- hp pulser status 1=on (register state only)
    int soft_status_stepper=0; -- stepper on =1 off =0 
    int soft_status_pul_get=0; -- get data  sets it to 1 
    int soft_status_pul_make=0; -- make data  sets it to 1
    int soft_status_pul_ld=0;  -- load sets to 1 
    int soft_status_pul_ext=0; -- on for external start 
    int soft_status_vme_reg=0; -- last value of vme status register 
    int soft_status_l_motor=0; -- 0 not initialized  1 init 
    int soft_status_c_motor=0; --  0 cal motor nit init
    
 

********************/ 
 /* ***********  command ****************************  */ 
  
     
     sprintf(exename,"exec soft_status_gas");
     if(debug)printf("%s \n",exename);
D 3
     success = DP_cmd("camac3",exename,result,111);
E 3
I 3
     success = DP_cmd(EC_laser_roc,exename,result,111);
E 3
     ret_value = 0;
     
     if ( success == 0)
       {
        if(debug)printf(" sucessful excecution \n buffer= %s \n",result);
        ret_value= parse(result, int_data, real_data, string_data,n1,n2,n3);
       }
    else
       {
         printf("unable to reach controller \n "); 
       }     
   
 	if(ret_value) 
 	{
 	printf("(currently irrelevant gas always on) GAS ON: software status\n");
 	soft_status=soft_status + BIT12 ;
 	}
 	else
 	{
 	printf("(currently irrelevant gas always on) GAS OFF: software status\n");
 	}

/*************** end command  ****************/ 

 
 /* ***********  command ****************************  */ 
  
     
     sprintf(exename,"exec soft_status_hp_pul");
     if(debug)printf("%s \n",exename);
D 3
     success = DP_cmd("camac3",exename,result,111);
E 3
I 3
     success = DP_cmd(EC_laser_roc,exename,result,111);
E 3
     ret_value = 0;
     
     if ( success == 0)
       {
        if(debug)printf(" sucessful excecution \n buffer= %s \n",result);
        ret_value= parse(result, int_data, real_data, string_data,n1,n2,n3);
       }
    else
       {
         printf("unable to reach controller \n "); 
       }     
   
 	if(ret_value) 
 	{
 	printf("(currently not used) HP ON: software status\n");
 	soft_status=soft_status + BIT11 ;
 	}
 	else
 	{
 	printf("(currently not used) HP OFF: software status\n");
 	}

/*************** end command  ****************/ 

  printf(" \n\n relevant status information follows previous not currently used \n\n" ); 
  /* ***********  command ****************************  */ 
  
     
     sprintf(exename,"exec soft_status_lpower");
     if(debug)printf("%s \n",exename);
D 3
     success = DP_cmd("camac3",exename,result,111);
E 3
I 3
     success = DP_cmd(EC_laser_roc,exename,result,111);
E 3
     ret_value = 0;
     
     if ( success == 0)
       {
        if(debug)printf(" sucessful excecution \n buffer= %s \n",result);
        ret_value= parse(result, int_data, real_data, string_data,n1,n2,n3);
       }
    else
       {
         printf("unable to reach controller \n"); 
       }     
   
 	if(ret_value) 
 	{
 	printf("LASER POWER ON: software status only\n");
 	soft_status=soft_status + BIT00 ;
 	soft_status=soft_status + BIT01 ;
 	}
 	else
 	{
 	printf("LASER POWER OFF: software status only\n");
 	}

/*************** end command  ****************/ 
 /* ***********  command ****************************  */ 
  
     
     sprintf(exename,"exec soft_status_laser");
     if(debug)printf("%s \n",exename);
D 3
     success = DP_cmd("camac3",exename,result,111);
E 3
I 3
     success = DP_cmd(EC_laser_roc,exename,result,111);
E 3
     ret_value = 0;
     
     if ( success == 0)
       {
        if(debug)printf(" sucessful excecution \n buffer= %s \n",result);
        ret_value= parse(result, int_data, real_data, string_data,n1,n2,n3);
       }
    else
       {
         printf("unable to reach controller \n "); 
       }     
   
 	if(ret_value) 
 	{
 	printf("LASER ENABLED: software status only on\n");
 	soft_status=soft_status + BIT02 ;
 	}
 	else
 	{
 	printf("LASER DISABLED: software status only on\n");
 	}

/*************** end command  ****************/ 
 
  /* ***********  command ****************************  */ 
  
     
     sprintf(exename,"exec soft_status_stepper");
     if(debug)printf("%s \n",exename);
D 3
     success = DP_cmd("camac3",exename,result,111);
E 3
I 3
     success = DP_cmd(EC_laser_roc,exename,result,111);
E 3
     ret_value = 0;
     
     if ( success == 0)
       {
        if(debug)printf(" sucessful excecution \n buffer= %s \n",result);
        ret_value= parse(result, int_data, real_data, string_data,n1,n2,n3);
       }
    else
       {
         printf("unable to reach controller \n "); 
       }     
   
 	if(ret_value) 
 	{
 	printf("STEPPER ON: software status\n");
 	soft_status=soft_status + BIT05 ;
 	}
 	else
 	{
 	printf("STEPPER OFF: software status\n");
 	}

/*************** end command  ****************/ 
  /* ***********  command ****************************  */ 
  
     
     sprintf(exename,"exec soft_status_pul_get");
     if(debug)printf("%s \n",exename);
D 3
     success = DP_cmd("camac3",exename,result,111);
E 3
I 3
     success = DP_cmd(EC_laser_roc,exename,result,111);
E 3
     ret_value = 0;
     
     if ( success == 0)
       {
        if(debug)printf(" sucessful excecution \n buffer= %s \n",result);
        ret_value= parse(result, int_data, real_data, string_data,n1,n2,n3);
       }
    else
       {
         printf("unable to reach controller \n "); 
       }     
   
 	if(ret_value) 
 	{
 	printf("PULSER DATA LOADED TO ROC: software status\n");
 	soft_status=soft_status + BIT08 ;
 	}
 	else
 	{
 	printf("NO PULSER DATA ON ROC: software status\n");
 	}

/*************** end command  ****************/ 
  /* ***********  command ****************************  */ 
  
     
     sprintf(exename,"exec soft_status_pul_make");
     if(debug)printf("%s \n",exename);
D 3
     success = DP_cmd("camac3",exename,result,111);
E 3
I 3
     success = DP_cmd(EC_laser_roc,exename,result,111);
E 3
     ret_value = 0;
     
     if ( success == 0)
       {
        if(debug)printf(" sucessful excecution \n buffer= %s \n",result);    
       }
    else
       {
         printf("unable to reach controller \n "); 
       }     
   
 	if(ret_value) 
 	{
 	printf("PULSER DATA CREATED BY ROC: software status\n");
 	soft_status=soft_status + BIT04 ;
 	}
 	else
 	{
 	printf("NO GENERATED PULSER DATA ON ROC: software status\n");
 	}

/*************** end command  ****************/ 

  /* ***********  command ****************************  */ 
  
     
     sprintf(exename,"exec numberOfPulses");
     if(debug)printf("%s \n",exename);
D 3
     success = DP_cmd("camac3",exename,result,111);
E 3
I 3
     success = DP_cmd(EC_laser_roc,exename,result,111);
E 3
     ret_value = 0;
     
     if ( success == 0)
       {
        if(debug)printf(" sucessful excecution \n buffer= %s \n",result);
        numberOfPulses= parse(result, int_data, real_data, string_data,n1,n2,n3); 
 	printf("CURRENT VALUE for number of pulses= %d  : software status\n",numberOfPulses);
       }
    else
       {
         printf("unable to reach controller \n "); 
       }     
   
 

/*************** end command  ****************/ 
  /* ***********  command ****************************  */ 
  
     
     sprintf(exename,"exec period_10usec");
     if(debug)printf("%s \n",exename);
D 3
     success = DP_cmd("camac3",exename,result,111);
E 3
I 3
     success = DP_cmd(EC_laser_roc,exename,result,111);
E 3
     ret_value = 0;
     
     if ( success == 0)
       {
        if(debug)printf(" sucessful excecution \n buffer= %s \n",result);
        period_10usec= parse(result, int_data, real_data, string_data,n1,n2,n3); 
        freq=(100000.0/(float) period_10usec);
 	printf("CURRENT VALUE for freq= %f  : software status\n",freq);
       }
    else
       {
         printf("unable to reach controller \n "); 
       }     
   
 

/*************** end command  ****************/ 
 /* ***********  command ****************************  */ 
  
     
     sprintf(exename,"exec soft_status_pul_ld");
     if(debug)printf("%s \n",exename);
D 3
     success = DP_cmd("camac3",exename,result,111);
E 3
I 3
     success = DP_cmd(EC_laser_roc,exename,result,111);
E 3
     ret_value = 0;
     
     if ( success == 0)
       {
        if(debug)printf(" sucessful excecution \n buffer= %s \n",result);
        ret_value= parse(result, int_data, real_data, string_data,n1,n2,n3);
       }
    else
       {
         printf("unable to reach controller \n "); 
       }     
   
 	if(ret_value) 
 	{
 	printf("PULSER DATA LOADED TO PULSE: software status\n");
 	soft_status=soft_status + BIT07 ;
 	}
 	else
 	{
 	printf("NO PULSER DATA LOADED TO PULSE: software status \n");
 	}

/*************** end command  ****************/ 
 /* ***********  command ****************************  */ 
  
     
     sprintf(exename,"exec soft_status_pul_ext");
     if(debug)printf("%s \n",exename);
D 3
     success = DP_cmd("camac3",exename,result,111);
E 3
I 3
     success = DP_cmd(EC_laser_roc,exename,result,111);
E 3
     ret_value = 0;
     
     if ( success == 0)
       {
        if(debug)printf(" sucessful excecution \n buffer= %s \n",result);
        ret_value= parse(result, int_data, real_data, string_data,n1,n2,n3);
       }
    else
       {
         printf("unable to reach controller \n "); 
       }     
   
 	if(ret_value) 
 	{
 	printf("PULSER RECYCLE ON: softwarestatus\n");
 	soft_status=soft_status + BIT06 ;
 	}
 	else
 	{
 	printf("PULSER RECYCLE OFF: softwarestatus\n");
 	}

/*************** end command  ****************/ 
  
 /* ***********  command ****************************  */ 
  
     
     sprintf(exename,"exec soft_status_l_motor");
     if(debug)printf("%s \n",exename);
D 3
     success = DP_cmd("camac3",exename,result,111);
E 3
I 3
     success = DP_cmd(EC_laser_roc,exename,result,111);
E 3
     ret_value = 0;
     
     if ( success == 0)
       {
        if(debug)printf(" sucessful excecution \n buffer= %s \n",result);
        ret_value= parse(result, int_data, real_data, string_data,n1,n2,n3);
       }
    else
       {
         printf("unable to reach controller \n "); 
       }     
   
 	if(ret_value) 
 	{
 	printf("LASER PORT INITIALIZED: software status\n");
 	soft_status=soft_status + BIT10 ;
 	}
 	else
 	{
 	printf("LASER PORT NOT INITIALIZED: software status\n");
 	}

/*************** end command  ****************/ 
  /* ***********  command ****************************  */ 
  
     
     sprintf(exename,"exec soft_status_c_motor");
     if(debug)printf("%s \n",exename);
D 3
     success = DP_cmd("camac3",exename,result,111);
E 3
I 3
     success = DP_cmd(EC_laser_roc,exename,result,111);
E 3
     ret_value = 0;
     
     if ( success == 0)
       {
        if(debug)printf(" sucessful excecution \n buffer= %s \n",result);
        ret_value= parse(result, int_data, real_data, string_data,n1,n2,n3);
       }
    else
       {
         printf("unable to reach controller \n "); 
       }     
   
 	if(ret_value) 
 	{
 	printf("CAL PORT INITIALIZED: software status\n");
 	soft_status=soft_status + BIT09 ;
 	}
 	else
 	{
 	printf("CAL PORT NOT INITIALIZED: software status\n");
 	}

/*************** end command  ****************/ 
/* ***********  command ****************************  */ 
  
     
     sprintf(exename,"exec soft_status_vme_reg");
     if(debug)printf("%s \n",exename);
D 3
     success = DP_cmd("camac3",exename,result,111);
E 3
I 3
     success = DP_cmd(EC_laser_roc,exename,result,111);
E 3
     ret_value = 0;
     
     if ( success == 0)
       {
        if(debug)printf(" sucessful excecution \n buffer= %s \n",result);
        ret_value= parse(result, int_data, real_data, string_data,n1,n2,n3);
       }
    else
       {
         printf("unable to reach controller \n "); 
       }  
          
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
        

        
 	 printf(" \n previous value of VME register   = %d,  ", ret_value );
 	
  	 
	printf(" %d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d \n ",
	bit15,bit14,bit13,bit12,bit11,bit10,bit09,bit08,bit07,bit06,bit05,bit04,bit03,bit02,bit01,bit00);	
	

/*************** end command  ****************/ 

        if( BIT00 & soft_status  )   {bit00=1;}  else  {bit00=0;}
   	if( BIT01 & soft_status  )   {bit01=1;}  else  {bit01=0;}
	if( BIT02 & soft_status  )   {bit02=1;}  else  {bit02=0;}
	if( BIT03 & soft_status  )   {bit03=1;}  else  {bit03=0;}
	if( BIT04 & soft_status  )   {bit04=1;}  else  {bit04=0;}
	if( BIT05 & soft_status  )   {bit05=1;}  else  {bit05=0;}
	if( BIT06 & soft_status  )   {bit06=1;}  else  {bit06=0;}
	if( BIT07 & soft_status  )   {bit07=1;}  else  {bit07=0;}
	if( BIT08 & soft_status  )   {bit08=1;}  else  {bit08=0;}
	if( BIT09 & soft_status  )   {bit09=1;}  else  {bit09=0;}
	if( BIT10 & soft_status  )   {bit10=1;}  else  {bit10=0;}
	if( BIT11 & soft_status  )   {bit11=1;}  else  {bit11=0;}
	if( BIT12 & soft_status  )   {bit12=1;}  else  {bit12=0;}
   	if( BIT13 & soft_status  )   {bit13=1;}  else  {bit13=0;}
        if( BIT14 & soft_status  )   {bit14=1;}  else  {bit14=0;}
        if( BIT15 & soft_status  )   {bit15=1;}  else  {bit15=0;}
        

        
        
 	 printf("value of software status word    = %d,  ", soft_status);
 	
  	 
	printf(" %d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d \n ",
	bit15,bit14,bit13,bit12,bit11,bit10,bit09,bit08,bit07,bit06,bit05,bit04,bit03,bit02,bit01,bit00);	
	

  /* ***********  command ****************************  */ 
  
     
     sprintf(exename,"exec getstatus");
     if(debug)printf("%s \n",exename);
D 3
     success = DP_cmd("camac3",exename,result,111);
E 3
I 3
     success = DP_cmd(EC_laser_roc,exename,result,111);
E 3
     ret_value = 0;
     
     if ( success == 0)
       {
        if(debug)printf(" sucessful excecution \n buffer= %s \n",result);
        ret_value= parse(result, int_data, real_data, string_data,n1,n2,n3);
       }
    else
       {
         printf("unable to reach controller \n "); 
       }     
   
      printf("current value of status register = %d,   0000%s\n\n ",ret_value, string_data[0]);
      
    
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
        
	if(bit00  ){printf("\nLASER POWER ON: hardware status \n"); exit_value=1;}  
        else       {printf("\nLASER POWER OFF: hardware status \n");exit_value=0;}
    
 	if(bit01  ){printf("LASER ON READY: hardware status \n");}  
        else
         {  if(bit00  ) {printf("LASER NOT YET READY(5 min wait): hardware status \n");}
            else        {printf("LASER NOT READY - NO POWER: hardware status \n");}
         } 
        
   	if(bit02 ) {printf("LASER ENABLED: hardware status \n");}  
        else       {printf("LASER DISABLED: hardware status \n");}

   	if(bit05  ){printf("STEPPER POWER ON: hardware status \n");}  
        else       {printf("STEPPER POWER OFF: hardware status \n");}

/*************** end command  ****************/ 
 /* ***********  command ****************************  */ 
  
     
     sprintf(exename,"exec get_pulser_status");
     if(debug)printf("%s \n",exename);
D 3
     success = DP_cmd("camac3",exename,result,111);
E 3
I 3
     success = DP_cmd(EC_laser_roc,exename,result,111);
E 3
     ret_value = 0;
     
     if ( success == 0)
       {
        if(debug)printf(" sucessful excecution \n buffer= %s \n",result);
        ret_value= parse(result, int_data, real_data, string_data,n1,n2,n3);
       }
    else
       {
         printf("unable to reach controller \n "); 
       }     
   
 	      
    
        if( BIT00 & int_data[0]  )   {bit00=1;}  else  {bit00=0;}
   	if( BIT01 & int_data[0]  )   {bit01=1;}  else  {bit01=0;}
	if( BIT02 & int_data[0]  )   {bit02=1;}  else  {bit02=0;}
	if( BIT03 & int_data[0]  )   {bit03=1;}  else  {bit03=0;}
	if( BIT04 & int_data[0]  )   {bit04=1;}  else  {bit04=0;}
	if( BIT05 & int_data[0]  )   {bit05=1;}  else  {bit05=0;}
	if( BIT06 & int_data[0]  )   {bit06=1;}  else  {bit06=0;}
	if( BIT07 & int_data[0]  )   {bit07=1;}  else  {bit07=0;}
	if( BIT08 & int_data[0]  )   {bit08=1;}  else  {bit08=0;}
	if( BIT09 & int_data[0]  )   {bit09=1;}  else  {bit09=0;}
	if( BIT10 & int_data[0]  )   {bit10=1;}  else  {bit10=0;}
	if( BIT11 & int_data[0]  )   {bit11=1;}  else  {bit11=0;}
	if( BIT12 & int_data[0]  )   {bit12=1;}  else  {bit12=0;}
   	if( BIT13 & int_data[0]  )   {bit13=1;}  else  {bit13=0;}
        if( BIT14 & int_data[0]  )   {bit14=1;}  else  {bit14=0;}
        if( BIT15 & int_data[0]  )   {bit15=1;}  else  {bit15=0;}
        
        printf("\n\n current value of status register = %d,  ",int_data[0]);
	printf(" %d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d \n\n ",
	bit15,bit14,bit13,bit12,bit11,bit10,bit09,bit08,bit07,bit06,bit05,bit04,bit03,bit02,bit01,bit00);	

        
	if(bit00  ){printf("\nPULSER OUTPUT ENABLED: hardware status \n");}  
        else       {printf("\nPULSER OUTPUT DISABLED: hardware status \n");}
    
        if(bit01 ) {printf("NO PULSER INHIBIT: hardware status \n");}  
        else       {printf("PULSER INHIBIT: hardware status \n");}

        if(bit02 ) {printf("PULSER START ENABLED: hardware status \n");}  
        else       {printf("PULSER START DISABLED: hardware status \n");}

        if(bit03 ) {printf("PULSER EXECUTING SEQUENCE: hardware status \n");}  
        else       {printf("PULSER INACTIVE: hardware status \n");}

        if(bit04 ) {printf("PULSER IN HOLD STATE: hardware status \n");}  
        else       {printf("PULSER NOT HOLDING: hardware status \n");}
	
        
   	if(bit05 ) {printf("PULSER CLOCK 10usec STEPS: hardware status \n");}  
        else       {printf("PULSER CLOCK 1usec STEPS: hardware status \n");}

   	if(bit06  ){printf("PULSER CLOCK EXTERNAL: hardware status \n");}  
        else       {printf("PULSER CLOCK INTERNAL: hardware status \n");}

/*************** end command  ****************/ 


printf(" exiting with status=%d \n",exit_value);
exit(exit_value); 
I 3

E 3
}
I 3

E 3
E 1
