/*******************************************************

unix_master.c
   code written by K. Giovanetti

This code controls the hardware setup of the EC laser.
The code will be started from the DAQ so that the laser 
hardware can be set for a laser calibration. General functions are:

 setup laser
 check daq and trigger state
 broadcast progress and status
 broadcast events for the data stream

 ***********************************************/

/**************

 - dont adequately check the values for the pulser 9/2/99 
 
 - add a reboot command when there is an error connecting 10/23/99
   p[robaly should chage this so that the I use system commans as
   in the TOF system.
********/


/*  determines if the variables are defined or referenced    */ 
/* by defining init varialbes are intialized in the .h files */

#define INIT 1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <laser.h>
#include <time.h>
#include <sys/times.h>
#include <sys/param.h>
#include <stdlib.h>
#include <laser.h>
#include <bit_defs.h>
#include "EC_laser_ROC.h"

/* for smartsockets */
#include <ssinclude.h>

/* prototype the locally defined functions */
int reboot_camac1();

time_t tp_reboot_start;
 
int tmptmp;
/* prototype system commands */
int system(const char *string);

int main (int argc, char *argv[])
{

/* list of variables that need to be true =1 to do a run */
int flag_all_init=0; 
int flag_clon10_init=0;
int flag_config=0;
int flag_coda_go = 0;
int flag_camac1_ok = 0;
int flag_bit_11 =0;
int flag_laser_ok =0;
int flag_laser_power=0;
int flag_laser_ready=0;
int flag_laser_enable=0;
int flag_pulser_ready=0;
int flag_pulser_loaded=0;
int flag_pulser_data=0;
int flag_pulser_on=0;
int flag_error=0;
int flag_filter=0;
int flag_CW_limit=0;
int flag_CCW_limit=0;
int flag_motor_moving=0;
int flag_setting=1;


int flag_pulser_type=0;
int flag_hp_pulser=0;
int flag_jor_pulser=1;
int hp_time_on = 60;
int flag_hp_ready=0;
int flag_RT_broadcast=1;

/* for the ss sockets to tape routine BOSwrite */
T_INT4 tape_event_buffer[100];
T_INT4 no_of_events=0;
T_INT4 hardware_status_reg;
T_INT4 software_status_reg;
T_STR tmpstr[50];
T_INT4 tmpval;

int debug_um=0;

char camac_state[50]="booted";
char configuration[50]="E1_PROD";
char coda_state[50]="active"; /* possible: active configured */
char prog_msg[BUFFER_LENGTH];  /*message buffer */
int mstat; /*  laser motor status put here */

int numberOfPulses=300;
int period_10usec=10000;
float freq;

int trigger_bits;
int bit_number = 11;

float real_no;

/*   varialbes for the dp_cmd  command, returned buffer */
char exename[111];
char result[BUFFER_LENGTH];

/* variables for parsing buffer   */
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

/* varaible for doing the read form file  */
FILE *file_handle ;
int counter=0;
int next_char;
int flag_search=0;
char *input_filename;
char word[150];
int flag_init_start=0;
int flag_procedure_start=0;
int flag_init_end=0;
int flag_procedure_end=0;
int flag_eol=0;
int flag_eof=0;
int line_count;

int numberOfsettings=0;
int setting_counter=0;
int setting[500];
int wait_setting[500];

/* 
  tp is the integer number of seconds since 1/1/1970 
  via program Thu Jun  4 10:57:38 1998 numerical time= 896972258
*/
 
 time_t tp;

 int interim=30;

/*   
  this is the number of seconds to wait sleep(interim); 
  uses the abve time to do wait 
*/

int debug=0;
int j=0;
char command_string[80];
int success=1;

/*********  init variables  ***************/
 n1=&nint;
 n2=&nreal;
 n3=&nstring;
       
 nopairs=0; /*  variable for the status buffer */

 /**************************get time *****************/
 tp=time(NULL);
 printf("start time= %s(seconds since 1970= %d)  \n\n ",ctime(&tp),tp);


 /* try to connect with RT server */
 if(flag_RT_broadcast)
       {
  	if(!ss_init("clasprod",0))
  	  {	
  	   printf("  RT init error "); 
  	  }
  	else 
  	 {
  	  sprintf(prog_msg," succesful connection to RTserver "); 
  	  ss_prog(prog_msg,flag_RT_broadcast);
  	  sleep(1);
  	 }
       }	 

 


 /************** read configuration file to set initial values    ********/
 /* input_filename = "/home/clasrun/jmu/c_cntl/config/configuration.txt"; */

 	 input_filename = "/usr/local/clas/parms/EC_config/configuration.txt";

	 if ( !(file_handle = fopen(input_filename,"r"))) /* returns !Null if can't open file */	  
	 {
	       sprintf(prog_msg,"ERROR: Could not open %s",input_filename);
	       ss_prog(prog_msg,flag_RT_broadcast);
	 	exit(1);
	 }
	 sprintf(prog_msg,"searching for BEGIN_INIT \n");
	 ss_prog(prog_msg,flag_RT_broadcast);
	 flag_search=0;
	 line_count=1;
	 flag_init_start=0;
	 while (!flag_search && !flag_eof && !flag_init_end) 
	 { 
	        counter=0;
	        flag_eol=0; 
	        while(!flag_eol && !flag_eof )
	        {
	 	     if(  (next_char=fgetc(file_handle)) < 0){flag_eof=1;}
	 	     word[counter]=next_char;
		     /* putchar(next_char);  */
		     if(word[counter] == SPACE || word[counter] == HTAB){ word[counter]=NULL;}
		     if(word[counter] == CR || word[counter]== LF ) 
		           {
		           flag_eol=1; 
		           word[counter]=NULL;
		           } 
		      counter++;     
		 }  
	 	if (strstr(word,"BEGIN_INIT") != NULL) 
	 	{
	 	       sprintf(prog_msg,"found BEGIN_INIT\n");
	 	       ss_prog(prog_msg,flag_RT_broadcast);
	 		flag_init_start=1;
	 	}	 	
	 	if (strstr(word,"END_INIT") != NULL) 
	 	{
	 	       sprintf(prog_msg,"found END_INIT\n");
	 	       ss_prog(prog_msg,flag_RT_broadcast);
	 	       flag_init_end=1;
	 	}
	 	if(flag_init_start && !flag_init_end)
	 	{
	 	switch(line_count++)
	 	   {
	 	   case 2:  sprintf(configuration,"%s",word);
	 	            sprintf(prog_msg," setting config file = %s\n",configuration);
	 	            ss_prog(prog_msg,flag_RT_broadcast);
	 	    	    break; 
	 	   case 3: sscanf(word,"%d", &bit_number);
	 	           sprintf(prog_msg," setting the trigger bit requirement to %d \n", bit_number);
	 	           ss_prog(prog_msg,flag_RT_broadcast);
	 	           break; 
	 	   case 4: sprintf(coda_state,"%s",word);
	 	           sprintf(prog_msg," expected coda state = %s \n", coda_state);
	 	           ss_prog(prog_msg,flag_RT_broadcast);
	 	           break;
	 	   case 5: sscanf(word,"%f", &real_no);
	 	   	   period_10usec=(int)(100000.0/real_no); 
	 	           sprintf(prog_msg," freq and period(10usec) = %f %d \n", real_no, period_10usec);
	 	           ss_prog(prog_msg,flag_RT_broadcast);
	 	           break;
	 	   case 6: sscanf(word,"%d", &numberOfPulses);
	 	           sprintf(prog_msg," setting number of pulses to %d \n", numberOfPulses);
	 	           ss_prog(prog_msg,flag_RT_broadcast);
	 	           break; 
	 	   case 7: sscanf(word,"%d", &interim);
	 	           sprintf(prog_msg," wait time between intialization attempts %d \n",interim);
	 	           ss_prog(prog_msg,flag_RT_broadcast);
	 	           break; 
	 	            	  
	 	   case 8: sscanf(word,"%d", &flag_pulser_type);
	 	           sprintf(prog_msg," pulser used hp=1, Jorway=0,  value= %d \n",flag_pulser_type);
	 	           ss_prog(prog_msg,flag_RT_broadcast);
	 	             if(flag_pulser_type == 0) { 
	 	                  flag_jor_pulser=1; 
	 	                  flag_hp_pulser=0;
	 	                  sprintf(prog_msg," jorway pulser \n");
	 	                  ss_prog(prog_msg,flag_RT_broadcast);
	 	                  }
	 	              else { 
	 	                  flag_jor_pulser=0; 
	 	                  flag_hp_pulser=1;
	 	                  sprintf(prog_msg," hp pulser \n");
	 	                  ss_prog(prog_msg,flag_RT_broadcast);
	 	                  }       
	 	           break; 	 	   
	 	  default: sprintf(prog_msg,"def \n");
	 	           ss_prog(prog_msg,flag_RT_broadcast);
	 	            break;
	 	   }                  
	 	}
	 		 	 	
	 }	



 *tmpstr="config_file_read" ;
 add_status(tmpstr,1);
  *tmpstr="done" ;
 add_status(tmpstr,0);

 ss_status(); 
 	
 sprintf(prog_msg," send the done status word don=0 \n");
 ss_prog(prog_msg,flag_RT_broadcast); 
 
 
/****************************************************/
/**********  execute a systems command    *********/

  sprintf(exename,"status");
  success=system(exename);
  if ( success == 0)
       {
        sprintf(prog_msg," execute program or system command, %s \n",exename);
        ss_prog(prog_msg,flag_RT_broadcast); 
       }
    else
       {
         sprintf(prog_msg,"unable to exectue command, %s \n ", exename); 
         ss_prog(prog_msg,flag_RT_broadcast);
       }     
/***************************************************/
 sprintf(prog_msg,"waiting till camac1 returns = %s \n",camac_state);
 ss_prog(prog_msg,flag_RT_broadcast);
 sprintf(prog_msg,"waiting till coda returns = %s \n",coda_state);
 ss_prog(prog_msg,flag_RT_broadcast);
 sprintf(prog_msg,"waiting till trigger returns = %s \n",configuration);
 ss_prog(prog_msg,flag_RT_broadcast);
 
while(flag_all_init == 0)
{
    /**************** intialize DP stuff **************/
     if(flag_clon10_init == 0)
      {
        sprintf(prog_msg,"initializing the connection with clon10 \n");
        ss_prog(prog_msg,flag_RT_broadcast);
        success=DP_cmd_init("clon10");
 
        if ( success == 0)
            {
             sprintf(prog_msg,"initializing done     success= %d \n\n",success);
             ss_prog(prog_msg,flag_RT_broadcast);
             flag_clon10_init = 1; 
            }
        else
            {
              sprintf(prog_msg,"initializing failure      success= %d \n\n",success);
              ss_prog(prog_msg,flag_RT_broadcast);
              flag_clon10_init = 0;  
            } 
      }



        /******** camac1 status ****************************   */     
   if(flag_camac1_ok == 0)
      {
        sprintf(exename,"status");
        sprintf(prog_msg,"\n\n%s \n",exename);
        ss_prog(prog_msg,flag_RT_broadcast);
        success = DP_cmd(EC_laser_roc,exename,result,111);
      
        if ( success == 0)
         {
          sprintf(prog_msg," sucessful excecution \n buffer= %s \n",result);
          ss_prog(prog_msg,flag_RT_broadcast);
          if(strstr(result,camac_state) != NULL) 
             {flag_camac1_ok=1; 
              sprintf(prog_msg," camac1 in booted state crate OK \n\n");
              ss_prog(prog_msg,flag_RT_broadcast);
             }
          else 
             {
              sprintf(prog_msg," camac1 reached but not in booted state \n\n");
              ss_prog(prog_msg,flag_RT_broadcast);
              flag_camac1_ok=0;
             }     
         }
       else
         {
           sprintf(prog_msg,"unable to reach controller ");
           ss_prog(prog_msg,flag_RT_broadcast);
	   reboot_camac1();
           flag_camac1_ok=0; 
         }     
      }

     /******** gason becomes the disable for the monitor trigger ****************************   */     

     
        sprintf(exename,"exec gason");
        sprintf(prog_msg,"%s \n",exename);
        ss_prog(prog_msg,flag_RT_broadcast);
        success = DP_cmd(EC_laser_roc,exename,result,111);
        ret_value = 0;
     
        if ( success == 0)
          {
           sprintf(prog_msg," sucessful excecution \n buffer= %s \n",result); 
           ss_prog(prog_msg,flag_RT_broadcast);
          }
        else
          {
           sprintf(prog_msg,"unable to reach controller "); 
           ss_prog(prog_msg,flag_RT_broadcast);
	   reboot_camac1();
          }
              
    /******** laser power on ****************************   */     
    if(flag_laser_power == 0)
      {
        sprintf(exename,"exec lpoweron");
        sprintf(prog_msg,"%s \n",exename);
        ss_prog(prog_msg,flag_RT_broadcast);
        success = DP_cmd(EC_laser_roc,exename,result,111);
        ret_value = 0;
     
        if ( success == 0)
          {
           sprintf(prog_msg," sucessful excecution \n buffer= %s \n",result); 
           ss_prog(prog_msg,flag_RT_broadcast);
          }
        else
          {
           sprintf(prog_msg,"unable to reach controller "); 
           ss_prog(prog_msg,flag_RT_broadcast);
	   reboot_camac1();
          }
       }       
    /******** laser enable ****************************   */     
    if(flag_laser_power != 0 && flag_laser_ready != 0 && flag_laser_enable == 0)
      {
        sprintf(exename,"exec laser_enable");
        sprintf(prog_msg,"%s \n",exename);
        ss_prog(prog_msg,flag_RT_broadcast);
        success = DP_cmd(EC_laser_roc,exename,result,111);
        ret_value = 0;
     
        if ( success == 0)
          {
           sprintf(prog_msg," sucessful excecution \n buffer= %s \n",result);
           ss_prog(prog_msg,flag_RT_broadcast); 
          }
        else
          {
           sprintf(prog_msg,"unable to reach controller "); 
           ss_prog(prog_msg,flag_RT_broadcast);
	   reboot_camac1();
          }
       }       

    /******** filter intialize ****************************   */     
    if(flag_laser_power != 0 && flag_filter == 0 )
      {
        sprintf(exename,"exec laser_motor_init");
        sprintf(prog_msg,"%s \n",exename);
        ss_prog(prog_msg,flag_RT_broadcast);
        success = DP_cmd(EC_laser_roc,exename,result,111);
        ret_value = 0;
        
         sleep(1);
          
        sprintf(exename,"exec {laser_motor(\"$19\")}");
        sprintf(prog_msg,"%s \n",exename);
        ss_prog(prog_msg,flag_RT_broadcast);
        DP_cmd(EC_laser_roc,exename,result,111);
        sprintf(prog_msg,"%s \n",result);
        ss_prog(prog_msg,flag_RT_broadcast);
 
     
        if ( success == 0)
          {
           sprintf(prog_msg," sucessful excecution \n buffer= %s \n",result);
           ss_prog(prog_msg,flag_RT_broadcast);
           if (strstr(result,">") != NULL) flag_filter=1;
            
          }
        else
          {
           sprintf(prog_msg,"unable to reach controller ");
           ss_prog(prog_msg,flag_RT_broadcast);
	   reboot_camac1(); 
          }
          
          
       }       
     /******************  get coda state *********************/
    if(flag_coda_go == 0)
      {  
	sprintf(prog_msg," test EB coda should be in the state= %s \n",coda_state) ;
	ss_prog(prog_msg,flag_RT_broadcast);
         sprintf(prog_msg,"waiting till camac1 returns = %s \n",camac_state);
	 ss_prog(prog_msg,flag_RT_broadcast);
	 sprintf(prog_msg,"waiting till coda returns = %s \n",coda_state);
	 ss_prog(prog_msg,flag_RT_broadcast);
	 sprintf(prog_msg,"waiting till trigger returns = %s \n",configuration);
	 ss_prog(prog_msg,flag_RT_broadcast);     	
        DP_cmd("EB1","EB1 status",result,111);
      	sprintf(prog_msg,"  event builder is %s \n",result);
      	ss_prog(prog_msg,flag_RT_broadcast);
      	if(strstr(result,coda_state) != NULL) 
             {flag_coda_go=1; 
              sprintf(prog_msg," coda found in the go state OK \n\n");
              ss_prog(prog_msg,flag_RT_broadcast);
             }
          else 
             {
              sprintf(prog_msg," coda in another transistion current state= %s \n",result);
              ss_prog(prog_msg,flag_RT_broadcast);
              sprintf(prog_msg," coda should be in the state= %s \n",coda_state);
              ss_prog(prog_msg,flag_RT_broadcast);
              flag_coda_go=0;
             } 
      }
      /******************  get configuraqtion  **************/
      if(flag_config ==0)
       {	
       	DP_cmd("clastrig2","clastrig2 cget -config",result,111);
        sprintf(prog_msg,"%s \n",result);
        ss_prog(prog_msg,flag_RT_broadcast);
        if(strstr(result,configuration) != NULL) 
             {flag_config=1; 
              sprintf(prog_msg," trigger indicate the correct configuration OK \n\n");
              ss_prog(prog_msg,flag_RT_broadcast);
             }
          else 
             {
              sprintf(prog_msg," trigger indicates wrong configuration not %s \n\n", configuration);
              ss_prog(prog_msg,flag_RT_broadcast);
              flag_config=0;
             } 
       }
      /******************  get trigger bit  **************/
      if(flag_bit_11 == 0)
       {
        DP_cmd("clastrig2","exec ts_control",result,111);	
        trigger_bits=strtoul(result,NULL, 16);  
        sprintf(prog_msg,"%x \n", trigger_bits);
        ss_prog(prog_msg,flag_RT_broadcast);
        flag_bit_11 = trigger_bits >> bit_number & 0x1;
       /* shift right bit_num times and compare with 1*/
        sprintf(prog_msg,"flag_bit_11 = %d \n",flag_bit_11);
        ss_prog(prog_msg,flag_RT_broadcast);
       }
       
       
     /* *********** pulser ****************************  */ 
  
 if(flag_jor_pulser == 1)  
 {
     if(flag_pulser_data ==0)
     {
         sprintf(prog_msg," set number of pulses =%d \n",numberOfPulses);
         ss_prog(prog_msg,flag_RT_broadcast);
         flag_pulser_data =1 ;
         sprintf(exename,"exec numberOfPulses=%d",numberOfPulses);
         sprintf(prog_msg,"%s \n",exename);
         ss_prog(prog_msg,flag_RT_broadcast);
         success = DP_cmd(EC_laser_roc,exename,result,111);
         sprintf(exename,"exec numberOfPulses");
         sprintf(prog_msg,"%s \n",exename);
         ss_prog(prog_msg,flag_RT_broadcast);
         success = DP_cmd(EC_laser_roc,exename,result,111);
         
        if ( success == 0)
          {
           ret_value= parse(result, int_data, real_data, string_data,&nint,&nreal,&nstring); 
 	   sprintf(prog_msg,"CURRENT VALUE for number of pulses= %d  : software status\n",ret_value);
 	   ss_prog(prog_msg,flag_RT_broadcast);
          }
        else
          {
            sprintf(prog_msg,"unable to reach controller \n ");
            ss_prog(prog_msg,flag_RT_broadcast);
	   reboot_camac1();
           flag_pulser_data=0; 
          }     

        sprintf(exename,"exec period_10usec=%d",period_10usec);
        success = DP_cmd(EC_laser_roc,exename,result,111);
        sprintf(exename,"exec period_10usec");
        success = DP_cmd(EC_laser_roc,exename,result,111);
 
     
     
        if ( success == 0)
          {
           ret_value= parse(result, int_data, real_data, string_data,n1,n2,n3); 
           freq=(100000.0/(float) ret_value);
 	   sprintf(prog_msg,"CURRENT VALUE for freq= %f  : software status\n",freq);
 	   ss_prog(prog_msg,flag_RT_broadcast);
          }
       else
          {
            sprintf(prog_msg,"unable to reach controller \n ");
            ss_prog(prog_msg,flag_RT_broadcast);
	    reboot_camac1();
            flag_pulser_data=0; 
          }
     sprintf(exename,"exec make_pulser_data");
     sprintf(prog_msg,"%s \n",exename);
     ss_prog(prog_msg,flag_RT_broadcast);
     success = DP_cmd(EC_laser_roc,exename,result,111);
      if ( success == 0)
       {
        sprintf(prog_msg," sucessful excecution \n buffer= %s \n",result);
        ss_prog(prog_msg,flag_RT_broadcast);
        ret_value= parse(result, int_data, real_data, string_data,n1,n2,n3);
       }
     else 
        { 
        sprintf(prog_msg, "unable to generate pulser data \n");
        ss_prog(prog_msg,flag_RT_broadcast);
        flag_pulser_data=0;
        }
                         
     }
          
      if(flag_pulser_data != 0   && flag_pulser_loaded == 0)
      {     
       sprintf(exename,"exec load_pulser");
       sprintf(prog_msg,"%s \n",exename);
       ss_prog(prog_msg,flag_RT_broadcast);
       success = DP_cmd(EC_laser_roc,exename,result,111);
      if ( success == 0)
       {
        sprintf(prog_msg," sucessful excecution \n buffer= %s \n",result);
        ss_prog(prog_msg,flag_RT_broadcast);
        ret_value= parse(result, int_data, real_data, string_data,n1,n2,n3);
        if(ret_value == 0) { flag_pulser_loaded=1;}
        }
      else 
       { 
         sprintf(prog_msg, "unable to generate pulser data \n");
         ss_prog(prog_msg,flag_RT_broadcast);
         flag_pulser_loaded=0;
        }  
      }
 }
 else 
 { 
     hp_time_on = numberOfPulses*.1;
     sprintf(prog_msg, " hp pulser time on = %d \n", hp_time_on);
     ss_prog(prog_msg,flag_RT_broadcast);
 }    
     
     /*************** end command  ****************/
      
       

    
     
     /******************  get status of hardware ****************************   */ 
  
     
     sprintf(exename,"exec getstatus");
     sprintf(prog_msg,"%s \n",exename);
     ss_prog(prog_msg,flag_RT_broadcast);
     success = DP_cmd(EC_laser_roc,exename,result,111);
     ret_value = 0;
     
     if ( success == 0)
       {
        sprintf(prog_msg," sucessful excecution \n buffer= %s \n",result);
        ss_prog(prog_msg,flag_RT_broadcast);
        ret_value= parse(result, int_data, real_data, string_data,n1,n2,n3);
	hardware_status_reg=ret_value;

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
        
	if(bit00  ){sprintf(prog_msg,"\nLASER POWER ON: hardware status \n");
	            ss_prog(prog_msg,flag_RT_broadcast);
	            flag_laser_power=1; }  
        else       {sprintf(prog_msg,"\nLASER POWER OFF: hardware status \n");
       		    ss_prog(prog_msg,flag_RT_broadcast);
                    flag_laser_power=0;}
    
 	if(bit01  ){sprintf(prog_msg,"LASER ON READY: hardware status \n");
 		     ss_prog(prog_msg,flag_RT_broadcast);
 	            flag_laser_ready=1;}  
        else
         {  flag_laser_ready=0;
            if(bit00  ) {sprintf(prog_msg,"LASER NOT YET READY(5 min wait): hardware status \n");
                         ss_prog(prog_msg,flag_RT_broadcast);}
            else        {sprintf(prog_msg,"LASER NOT READY - NO POWER: hardware status \n");
                         ss_prog(prog_msg,flag_RT_broadcast);}
         } 
        
   	if(bit02 ) {sprintf(prog_msg,"LASER ENABLED: hardware status \n");
   	            ss_prog(prog_msg,flag_RT_broadcast);
   	            flag_laser_enable=1;}  
        else       {sprintf(prog_msg,"LASER DISABLED: hardware status \n");
                    ss_prog(prog_msg,flag_RT_broadcast);
                    flag_laser_enable=0;}

   	if(bit05  ){sprintf(prog_msg,"STEPPER POWER ON: hardware status \n");
   	            ss_prog(prog_msg,flag_RT_broadcast);}  
        else       {sprintf(prog_msg,"STEPPER POWER OFF: hardware status \n");
                    ss_prog(prog_msg,flag_RT_broadcast);}

       }
    else
       {
         sprintf(prog_msg,"unable to reach controller reboot request ");
         ss_prog(prog_msg,flag_RT_broadcast);
	 reboot_camac1(); 
       }  
   

    /*********************************************/ 

   
   flag_laser_ok =flag_laser_power  &&
                  flag_laser_ready  &&
                  flag_laser_enable       ;

   if(flag_jor_pulser==1)
      {
        flag_pulser_ready = flag_pulser_loaded && 
                            flag_pulser_data;
      }    
   else
      {
      flag_pulser_ready = 1;
      } 

     
   flag_all_init =  flag_clon10_init && 
                    flag_coda_go     && 
                    flag_camac1_ok   && 
                    flag_bit_11      && 
                    flag_laser_ok    &&
                    flag_config      &&
                    flag_filter      &&
                    flag_pulser_ready    ;
 
 *tmpstr="config_file_read" ;
 add_status(tmpstr,1);
 *tmpstr="clon10_connection" ;
 tmpval=flag_clon10_init;
 add_status(tmpstr,tmpval);
 *tmpstr="coda_ready" ;
 tmpval=flag_coda_go;
 add_status(tmpstr,tmpval);
 *tmpstr="camac1_connection" ;
 tmpval=flag_camac1_ok;
 add_status(tmpstr,tmpval);
 *tmpstr="trigger_bit_11_set" ;
 tmpval=flag_bit_11;
 add_status(tmpstr,tmpval);
 *tmpstr="laser_power_on" ;
 tmpval=flag_laser_power;
 add_status(tmpstr,tmpval);
 *tmpstr="laser_ready" ;
 tmpval=flag_laser_ready;
 add_status(tmpstr,tmpval);
 *tmpstr="laser_enabled" ;
 tmpval=flag_laser_enable;
 add_status(tmpstr,tmpval);
 *tmpstr="coda_config_ok" ;
 tmpval=flag_config;
 add_status(tmpstr,tmpval); 
 *tmpstr="laser_filter_ready" ;
 tmpval=flag_filter;
 add_status(tmpstr,tmpval);
 *tmpstr="pulser_ready" ;
 tmpval=flag_pulser_ready;
 add_status(tmpstr,tmpval);
 *tmpstr="jorway_pulser" ;
 tmpval=flag_jor_pulser;
 add_status(tmpstr,tmpval);
 *tmpstr="pulser_loaded" ;
 tmpval=flag_pulser_loaded;
 add_status(tmpstr,tmpval);
 *tmpstr="number_of_pulses" ;
 tmpval=numberOfPulses;
 add_status(tmpstr,tmpval);
 *tmpstr="pulser_frequency" ;
 tmpval=freq;
 add_status(tmpstr,tmpval); 
                    
 ss_status();

    sprintf(prog_msg," end of intialization loop laser ready= %d, init= %d \n\n",flag_laser_ok, flag_all_init);                
     ss_prog(prog_msg,flag_RT_broadcast);
    sprintf(prog_msg," clon10      = %d \n",flag_clon10_init);
    ss_prog(prog_msg,flag_RT_broadcast);
    sprintf(prog_msg," camac1      = %d \n",flag_camac1_ok);
    ss_prog(prog_msg,flag_RT_broadcast);
    sprintf(prog_msg," coda st ok  = %d \n",flag_coda_go);
    ss_prog(prog_msg,flag_RT_broadcast);
    sprintf(prog_msg," trigger bit = %d \n",flag_bit_11);
    ss_prog(prog_msg,flag_RT_broadcast);
    sprintf(prog_msg," laser       = %d \n",flag_laser_ok);
    ss_prog(prog_msg,flag_RT_broadcast);
    sprintf(prog_msg," config      = %d \n",flag_config);
    ss_prog(prog_msg,flag_RT_broadcast);
    sprintf(prog_msg," filter      = %d \n",flag_filter);
    ss_prog(prog_msg,flag_RT_broadcast);
    sprintf(prog_msg," pulser      = %d \n",flag_pulser_ready);
    ss_prog(prog_msg,flag_RT_broadcast);

    /****************** sleep for 30 seconds *****************************/
     sleep(interim);                             
}

sprintf(prog_msg," finished intitialization ");
ss_prog(prog_msg,flag_RT_broadcast);
/******* send 1200 event to the event stream ****/ 
 tp=time(NULL);
 tape_event_buffer[0]=1200;
 tape_event_buffer[1]=tp;
 tape_event_buffer[2]=hardware_status_reg;
 tape_event_buffer[3]=flag_all_init;
 no_of_events=1;
 boswrite(tape_event_buffer,no_of_events);
 
 sprintf(prog_msg," broadcast a 1200 event for tape ");
 ss_prog(prog_msg,flag_RT_broadcast);

/******************************************************/

 /************** read configuration file to set execute procedure    ********/
        line_count=0;
        flag_search=0;

	 while (!flag_search && !flag_eof && !flag_procedure_end) 
	 { 
	        counter=0;
	        flag_eol=0; 
	        while(!flag_eol && !flag_eof )
	        {
	 	     if(  (next_char=fgetc(file_handle)) < 0){flag_eof=1;}
	 	     word[counter]=next_char;
		     /* putchar(next_char);  */
		     if(word[counter] == SPACE || word[counter] == HTAB){ word[counter]=NULL;}
		     if(word[counter] == CR || word[counter]== LF ) 
		           {
		           flag_eol=1; 
		           word[counter]=NULL;
		           } 
		      counter++;     
		 }  
	 	if (strstr(word,"BEGIN_PROCEDURE") != NULL) 
	 	{
	 	       sprintf(prog_msg,"found BEGIN_PROCEDURE\n");
	 	       ss_prog(prog_msg,flag_RT_broadcast);
	 		flag_procedure_start=1;
	 	}	 	
	 	if (strstr(word,"END_PROCEDURE") != NULL) 
	 	{
	 	       sprintf(prog_msg,"found END_PROCEDURE\n");
	 	       ss_prog(prog_msg,flag_RT_broadcast);
	 		flag_procedure_end=1;
	 	}
	 	if(flag_procedure_start && !flag_procedure_end)
	 	{
	 	switch(line_count++)
	 	   {
                   case 0: sprintf(prog_msg," processing procedure steps\n");
                           ss_prog(prog_msg,flag_RT_broadcast);
                           break;
	 	   case 1: sscanf(word,"%d", &numberOfsettings);
	 	           sprintf(prog_msg," number of settings in the procedure= %d \n", numberOfsettings);
	 	           ss_prog(prog_msg,flag_RT_broadcast);
	 	           setting_counter=0;
	 	           break; 
	 	  
	 	            	  
	 	   default: sprintf(prog_msg,"get setting %d \n", (setting_counter+1));
	 	            ss_prog(prog_msg,flag_RT_broadcast);
	 	            if(setting_counter >= numberOfsettings){
	 	                sprintf(prog_msg," error too many settings found");
	 	                ss_prog(prog_msg,flag_RT_broadcast);
	 	                break;}
	 	            if (flag_setting){
	 	                sprintf(prog_msg,"get setting \n");
	 	                ss_prog(prog_msg,flag_RT_broadcast);
	 	                sscanf(word,"%d", &setting[setting_counter]); 
	 	                flag_setting=0;
	 	                }
			    else {
			          sprintf(prog_msg,"get wait \n");
			          ss_prog(prog_msg,flag_RT_broadcast);
			          sscanf(word,"%d", &wait_setting[setting_counter]);
			          flag_setting=1; 
			          setting_counter++;
			          }
	 	           
	 	            break;
	 	   }                  
	 	}
	 		 	 	
	 }	

/********* file read begin data recording ******************************/

 record_monitors(flag_RT_broadcast,20);

  j=0; 
   do{ 
    if(setting[j] == 9999 )
    {    
     sprintf(prog_msg," FOUND 9999 sequence \n");
     ss_prog(prog_msg,flag_RT_broadcast);
     
       sleep(wait_setting[j]);

      record_monitors(flag_RT_broadcast,20);
    }
    else
    {
    sprintf(prog_msg,"_______________ go to CCW __________________8 \n");
    ss_prog(prog_msg,flag_RT_broadcast);

    sprintf(exename,"exec {laser_motor(\"$18\")}");
     sprintf(prog_msg,"%s \n",exename);
     ss_prog(prog_msg,flag_RT_broadcast);
     DP_cmd(EC_laser_roc,exename,result,111);
     sprintf(prog_msg,"%s \n",result);
     ss_prog(prog_msg,flag_RT_broadcast);
      sleep(1);
    flag_CCW_limit=0;  
    while(flag_CCW_limit==0)
       {
       /************************  CL *****************************/
       sprintf(exename,"exec {laser_motor(\"$1CL\")}");
       if(debug_um){sprintf(prog_msg,"%s \n",exename);
                   ss_prog(prog_msg,flag_RT_broadcast);}
       DP_cmd(EC_laser_roc,exename,result,111);
       if(debug_um){sprintf(prog_msg,"%s \n",result);
                    ss_prog(prog_msg,flag_RT_broadcast);}
       ret_value=parse(result, int_data, real_data, string_data,n1,n2,n3);
       if(debug_um){sprintf(prog_msg," value of string %s \n",&string_data[0][3]);
                    ss_prog(prog_msg,flag_RT_broadcast);}
       sscanf(&string_data[0][3],"%x", &mstat);
       sprintf(prog_msg," status = %x \n", mstat);
       ss_prog(prog_msg,flag_RT_broadcast);
     
     
           
           
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
       
	 
	if(debug_um){sprintf(prog_msg," %d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d \n ",
	bit15,bit14,bit13,bit12,bit11,bit10,bit09,bit08,bit07,bit06,bit05,bit04,bit03,bit02,bit01,bit00);
	ss_prog(prog_msg,flag_RT_broadcast);}	

	if(bit00  ){if(debug_um){sprintf(prog_msg,"\nCW limit reached \n");ss_prog(prog_msg,flag_RT_broadcast);} 
	            flag_CW_limit=1;
	            
	           }  
        else       {if(debug_um){sprintf(prog_msg,"\nNO CW limit \n"); ss_prog(prog_msg,flag_RT_broadcast); }
                    flag_CW_limit=0;
                    
                   }
 
        if(bit01  ){if(debug_um){sprintf(prog_msg,"CCW limit reached\n"); ss_prog(prog_msg,flag_RT_broadcast);}               
                    flag_CCW_limit=1;
                    }  
        else       {sprintf(prog_msg,"no CCW limt \n");
                   flag_CCW_limit=0;
                   ss_prog(prog_msg,flag_RT_broadcast);
                   }
 
 *tmpstr="CCWlimit" ;
 tmpval=flag_CCW_limit;
 add_status(tmpstr,tmpval);
 *tmpstr="CWlimit" ;
 tmpval=flag_CW_limit;
 add_status(tmpstr,tmpval);
 *tmpstr="filter_moving" ;
 tmpval=0;
 add_status(tmpstr,tmpval);
	
         /*****************************   CL end **************/
 
        }

  sprintf(prog_msg," CCW limit reached \n\n");
  ss_prog(prog_msg,flag_RT_broadcast);
  sleep(1);

  
  
  
      sprintf(prog_msg," start at setting %d: filter = %d",(j+1), setting[j]);
      ss_prog(prog_msg,flag_RT_broadcast);
     sprintf(exename,"exec {laser_motor(\"$12%d\")}",setting[j]);
     sprintf(prog_msg,"%s \n",exename);
     ss_prog(prog_msg,flag_RT_broadcast);
     DP_cmd(EC_laser_roc,exename,result,111);
     sprintf(prog_msg,"%s \n",result);
     ss_prog(prog_msg,flag_RT_broadcast);
     sleep(1);
     sprintf(exename,"exec {laser_motor(\"$14\")}");
     sprintf(prog_msg,"%s \n",exename);
     ss_prog(prog_msg,flag_RT_broadcast);
     DP_cmd(EC_laser_roc,exename,result,111);
     sprintf(prog_msg,"%s \n",result);
     ss_prog(prog_msg,flag_RT_broadcast);
  
     flag_motor_moving=1;
     while(flag_motor_moving == 1)
       {
       /* *************status blank start ****************************   */   
  
     sprintf(exename,"exec {laser_motor(\"$1\")}");
     if(debug_um){sprintf(prog_msg,"%s \n",exename);
                  ss_prog(prog_msg,flag_RT_broadcast);}
     DP_cmd(EC_laser_roc,exename,result,111);
     if(debug_um){sprintf(prog_msg,"%s \n",result);
                  ss_prog(prog_msg,flag_RT_broadcast);}
     ret_value=parse(result, int_data, real_data, string_data,n1,n2,n3);
     if(debug_um){sprintf(prog_msg," value of string %s \n",&string_data[0][3]);
                  ss_prog(prog_msg,flag_RT_broadcast);}
     sscanf(&string_data[0][3],"%x", &mstat);
     if(debug_um){sprintf(prog_msg," status = %x \n", mstat);
                  ss_prog(prog_msg,flag_RT_broadcast);}
     
     
           
           
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
       
	 
	if(debug_um){sprintf(prog_msg," %d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d \n ",
	bit15,bit14,bit13,bit12,bit11,bit10,bit09,bit08,bit07,bit06,bit05,bit04,bit03,bit02,bit01,bit00);
	             ss_prog(prog_msg,flag_RT_broadcast);}	
	
     
      if(bit00  ){sprintf(prog_msg,"\nmotor running \n");
                  flag_motor_moving=1;
                  ss_prog(prog_msg,flag_RT_broadcast);
                  }  
        else     {
                  if(debug_um)
                     {sprintf(prog_msg,"\nmotor idle\n");
                      ss_prog(prog_msg,flag_RT_broadcast);}
                  flag_motor_moving=0;
                    
                  }
 
        if(bit01  ){if(debug_um){sprintf(prog_msg,"limit error\n");
                   ss_prog(prog_msg,flag_RT_broadcast);}
                   }  
        else       {if(debug_um){sprintf(prog_msg,"no limit error \n");
                    ss_prog(prog_msg,flag_RT_broadcast);}
                    }

        if(bit02  ){if(debug_um){sprintf(prog_msg,"emergency stop \n");
                   ss_prog(prog_msg,flag_RT_broadcast);} }  
        else       {if(debug_um){sprintf(prog_msg,"no emergency stop  OK\n");
                   ss_prog(prog_msg,flag_RT_broadcast);}}
    
 	
   	if(bit03 ) {if(debug_um){sprintf(prog_msg,"command error \n");
   	            ss_prog(prog_msg,flag_RT_broadcast);}}  
        else       {if(debug_um){sprintf(prog_msg,"no command error \n");
                    ss_prog(prog_msg,flag_RT_broadcast);}}

/*************** status blank end *************************/
 *tmpstr="CCWlimit" ;
 tmpval=0;
 add_status(tmpstr,tmpval);
 *tmpstr="CWlimit" ;
 tmpval=0;
 add_status(tmpstr,tmpval);
 *tmpstr="filter_moving" ;
 tmpval=flag_motor_moving;
 add_status(tmpstr,tmpval);
 *tmpstr="filter_setting" ;
 tmpval=setting[j];
 add_status(tmpstr,tmpval);
 

       }
       sprintf(prog_msg," reached destination ");
       ss_prog(prog_msg,flag_RT_broadcast);
       sprintf(prog_msg," \n\n start pulser aftert %d \n ", wait_setting[j]);
       ss_prog(prog_msg,flag_RT_broadcast);
       
 /**************************get time *****************/
 tp=time(NULL);
 sprintf(prog_msg," before wait= %s(seconds since 1970= %d)  \n\n ",ctime(&tp),tp);
 ss_prog(prog_msg,flag_RT_broadcast);

 *tmpstr="CCWlimit" ;
 tmpval=0;
 add_status(tmpstr,tmpval);
 *tmpstr="CWlimit" ;
 tmpval=0;
 add_status(tmpstr,tmpval);
 *tmpstr="filter_moving" ;
 tmpval=flag_motor_moving;
 add_status(tmpstr,tmpval);
 *tmpstr="filter_setting" ;
 tmpval=setting[j];
 add_status(tmpstr,tmpval);
  *tmpstr="wait_setting" ;
 tmpval=wait_setting[j];
 add_status(tmpstr,tmpval);
 *tmpstr="asleep" ;
 tmpval=1;
 add_status(tmpstr,tmpval);
 ss_status();     

       sleep(wait_setting[j]);

 *tmpstr="asleep" ;
 tmpval=0;
 add_status(tmpstr,tmpval);
 ss_status();     

/******* send 1200 1100 event to the event stream ****/ 
 tp=time(NULL);
 tape_event_buffer[0]=1201;
 tape_event_buffer[1]=tp;
 tape_event_buffer[2]=hardware_status_reg;
 tape_event_buffer[3]=flag_all_init;

 tape_event_buffer[4]=1100;
 tape_event_buffer[5]=setting[j];
 tape_event_buffer[6]=mstat;
 tape_event_buffer[7]=0;
 no_of_events=2;
 boswrite(tape_event_buffer,no_of_events);

 sprintf(prog_msg," broadcast a 1200 1100 event for tape ");
 ss_prog(prog_msg,flag_RT_broadcast);

/******************************************************/

 sleep(10); 




 /**************************get time *****************/
 tp=time(NULL);
 sprintf(prog_msg,"after = %s(seconds since 1970= %d)  \n\n ",ctime(&tp),tp);
 ss_prog(prog_msg,flag_RT_broadcast);
      
       sprintf(exename,"exec dataway_start");
       sprintf(prog_msg,"%s \n",exename);
       ss_prog(prog_msg,flag_RT_broadcast);
       success=1;
       success = DP_cmd(EC_laser_roc,exename,result,111);
      if ( success == 0)
       {
        sprintf(prog_msg," sucessful excecution \n buffer= %s \n",result);
        ss_prog(prog_msg,flag_RT_broadcast);
        ret_value= parse(result, int_data, real_data, string_data,n1,n2,n3);
       }
     else 
        { 
        sprintf(prog_msg, "unable to start pulser \n");
        ss_prog(prog_msg,flag_RT_broadcast);
        }  
        
    flag_pulser_on=1;   

    while(flag_pulser_on==1)
     {
       sleep(10);
        sprintf(prog_msg, "checking pulser status \n");
        ss_prog(prog_msg,flag_RT_broadcast);
     sleep(1);
     sprintf(exename,"exec get_pulser_status");
     if(debug_um){sprintf(prog_msg,"%s \n",exename);ss_prog(prog_msg,flag_RT_broadcast);}
     success = DP_cmd(EC_laser_roc,exename,result,111);
     ret_value = 0;
     
     if ( success == 0)
       {
        if(debug){sprintf(prog_msg," sucessful excecution \n buffer= %s \n",result);
                 ss_prog(prog_msg,flag_RT_broadcast);}
        ret_value= parse(result, int_data, real_data, string_data,n1,n2,n3);
       }
    else
       {
         sprintf(prog_msg,"unable to reach controller \n ");
         ss_prog(prog_msg,flag_RT_broadcast);
	 reboot_camac1(); 
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
        
        if(debug_um) {sprintf(prog_msg,"\n\n current value of status $register = %d,  ",int_data[0]);
                      ss_prog(prog_msg,flag_RT_broadcast);}
        
	if(debug_um) {sprintf(prog_msg," %d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d \n\n ",
	bit15,bit14,bit13,bit12,bit11,bit10,bit09,bit08,bit07,bit06,bit05,bit04,bit03,bit02,bit01,bit00);
	              ss_prog(prog_msg,flag_RT_broadcast);}	

        
	if(bit00  ){if(debug_um){sprintf(prog_msg,"\nPULSER OUTPUT ENABLED: hardware status \n");ss_prog(prog_msg,flag_RT_broadcast);}  }
        else       {if(debug_um){sprintf(prog_msg,"\nPULSER OUTPUT DISABLED: hardware status \n");ss_prog(prog_msg,flag_RT_broadcast);}}
    
        if(bit01 ) {if(debug_um){sprintf(prog_msg,"NO PULSER INHIBIT: hardware status \n");ss_prog(prog_msg,flag_RT_broadcast);}  }
        else       {if(debug_um){sprintf(prog_msg,"PULSER INHIBIT: hardware status \n");ss_prog(prog_msg,flag_RT_broadcast);}}

        if(bit02 ) {if(debug_um){sprintf(prog_msg,"PULSER START ENABLED: hardware status \n");ss_prog(prog_msg,flag_RT_broadcast);}  }
        else       {if(debug_um){sprintf(prog_msg,"PULSER START DISABLED: hardware status \n");ss_prog(prog_msg,flag_RT_broadcast);}}

        if(bit03 ) {{sprintf(prog_msg,"PULSER EXECUTING SEQUENCE: hardware status \n"); flag_pulser_on=1;ss_prog(prog_msg,flag_RT_broadcast);}}  
        else       {{sprintf(prog_msg,"PULSER INACTIVE: hardware status \n");flag_pulser_on=0;ss_prog(prog_msg,flag_RT_broadcast);}}

        if(bit04 ) {if(debug_um){sprintf(prog_msg,"PULSER IN HOLD STATE: hardware status \n");ss_prog(prog_msg,flag_RT_broadcast);}  }
        else       {if(debug_um){sprintf(prog_msg,"PULSER NOT HOLDING: hardware status \n");ss_prog(prog_msg,flag_RT_broadcast);}}
	
        
   	if(bit05 ) {if(debug_um){sprintf(prog_msg,"PULSER CLOCK 10usec STEPS: hardware status \n");ss_prog(prog_msg,flag_RT_broadcast);}}  
        else       {if(debug_um){sprintf(prog_msg,"PULSER CLOCK 1usec STEPS: hardware status \n");ss_prog(prog_msg,flag_RT_broadcast);}}

   	if(bit06  ){if(debug_um){sprintf(prog_msg,"PULSER CLOCK EXTERNAL: hardware status \n");ss_prog(prog_msg,flag_RT_broadcast);}  }
        else       {if(debug_um){sprintf(prog_msg,"PULSER CLOCK INTERNAL: hardware status \n");ss_prog(prog_msg,flag_RT_broadcast);}}

/*************** end command  ****************/      
     }

/*  at this point the sequence of pyuulses has ended  ****/
/*  you are still looping through settings **************/

/*  insert the pause-resume code    */


/* **************** for sync  pause run */
sprintf(exename,"exec pause_run");
sprintf(prog_msg,"%s \n",exename);
ss_prog(prog_msg,flag_RT_broadcast);
success = DP_cmd("clastrig2",exename,result,111);
ret_value = 0;
if ( success == 0)
          {
           sprintf(prog_msg," sucessful excecution \n buffer= %s \n",result); 
           ss_prog(prog_msg,flag_RT_broadcast);
          }
        else
          {
           sprintf(prog_msg,"unable to reach controller "); 
           ss_prog(prog_msg,flag_RT_broadcast);
	   reboot_camac1();
          }
 sleep(10);
   /*****   resume run *******/
sprintf(exename,"exec resume_run");
sprintf(prog_msg,"%s \n",exename);
ss_prog(prog_msg,flag_RT_broadcast);
success = DP_cmd("clastrig2",exename,result,111);
ret_value = 0;
if ( success == 0)
          {
           sprintf(prog_msg," sucessful excecution \n buffer= %s \n",result); 
           ss_prog(prog_msg,flag_RT_broadcast);
          }
        else
          {
           sprintf(prog_msg,"unable to reach controller "); 
           ss_prog(prog_msg,flag_RT_broadcast);
	   reboot_camac1();
          }
/***************************************/


/***************************************/
     sprintf(prog_msg,"pulser activity stopped\n");
     ss_prog(prog_msg,flag_RT_broadcast);

     /******* send 1200 1100 event to the event stream ****/ 
     tp=time(NULL);
     tape_event_buffer[0]=1202;
     tape_event_buffer[1]=tp;
     tape_event_buffer[2]=hardware_status_reg;
     tape_event_buffer[3]=flag_all_init;
 
     tape_event_buffer[4]=1100;
     tape_event_buffer[5]=setting[j];
     tape_event_buffer[6]=mstat;
     tape_event_buffer[7]=0;
     no_of_events=2;
     boswrite(tape_event_buffer,no_of_events);

     sprintf(prog_msg," broadcast a 1200 1100 event for tape ");
     ss_prog(prog_msg,flag_RT_broadcast);

/******************************************************/
   }
     j++;
    } while(j < numberOfsettings);
    
    record_monitors(flag_RT_broadcast,20);

     sprintf(exename,"exec lpoweroff");
     sprintf(prog_msg,"%s \n",exename);
     ss_prog(prog_msg,flag_RT_broadcast);
     success = DP_cmd(EC_laser_roc,exename,result,111);
 
     *tmpstr="done" ;
     add_status(tmpstr,1);
     ss_status();
     sprintf(prog_msg," ALL DONE exiting the control program \n");
     ss_prog(prog_msg,flag_RT_broadcast);
     ss_prog(prog_msg,flag_RT_broadcast);
     ss_prog(prog_msg,flag_RT_broadcast);
     ss_prog(prog_msg,flag_RT_broadcast);
     ss_prog(prog_msg,flag_RT_broadcast);
 
     
     
     TipcSrvDestroy(T_IPC_SRV_CONN_NONE);  
exit;
}
/**********************end of program********************************/



int reboot_camac1()
{
time_t tp1=0;
char prog_msg1[BUFFER_LENGTH];
int flag_RT_broadcast1=1;
char exename1[111];
int success1=1;

/**********  execute a systems command    *********/
 tp1=time(NULL);

 if((tp1-tp_reboot_start) > 120)
   {
     sprintf(prog_msg1," rebooting camac1 ");
     ss_prog(prog_msg1,flag_RT_broadcast1);
 
     sprintf(exename1,"roc_reboot camac1");
     success1=system(exename1);
     if ( success1 == 0)
       {
	 tp_reboot_start=time(NULL);
	 sprintf(prog_msg1," request -- %s  -- return with no error  \n",exename1);
	 ss_prog(prog_msg1,flag_RT_broadcast1);
       }
     else
       {
         sprintf(prog_msg1,"request -- %s  -- return with no error ", exename1); 
         ss_prog(prog_msg1,flag_RT_broadcast1);
	 return(104);
       }
   }
 else
   {
     sprintf(prog_msg1," reboot camac1 request not completed. Need to wait for 2 minutes ");
     ss_prog(prog_msg1,flag_RT_broadcast1);
   }

/***************************************************/
 return(0);
}



int record_monitors(int flag_RT_broadcast,int length)
{
time_t tp;
T_INT4 tape_event_buffer[100];
T_INT4 no_of_events=0;
char prog_msg[BUFFER_LENGTH];  /*message buffer */
char exename[111];
int success=1;
char result[BUFFER_LENGTH];
int ret_value;
 
/******* send 1201 100 event to the event stream ****/ 
 tp=time(NULL);
 tape_event_buffer[0]=1201;
 tape_event_buffer[1]=tp;
 tape_event_buffer[2]=0;
 tape_event_buffer[3]=0;

 tape_event_buffer[4]=1100;
 tape_event_buffer[5]=9999;
 tape_event_buffer[6]=length;
 tape_event_buffer[7]=0;
 no_of_events=2;
 boswrite(tape_event_buffer,no_of_events);

 sprintf(prog_msg," broadcast a 1200 1100 event for tape ");
 ss_prog(prog_msg,flag_RT_broadcast);
 sprintf(prog_msg," setting up for 300 secs of monitors ");
 ss_prog(prog_msg,flag_RT_broadcast);


/******************************************************/
 sleep(10); 
/******** gason becomes the disable for the monitor trigger ****************************   */         
        sprintf(exename,"exec gasoff");
        sprintf(prog_msg,"%s \n",exename);
        ss_prog(prog_msg,flag_RT_broadcast);
        success = DP_cmd(EC_laser_roc,exename,result,111);
        ret_value = 0;
     
        if ( success == 0)
          {
           sprintf(prog_msg," sucessful excecution \n buffer= %s \n",result); 
           ss_prog(prog_msg,flag_RT_broadcast);
          }
        else
          {
           sprintf(prog_msg,"unable to reach controller "); 
           ss_prog(prog_msg,flag_RT_broadcast);
	   reboot_camac1();
          }
/***************************************************************/

 /**************************get time *****************/
 tp=time(NULL);
 sprintf(prog_msg,"after = %s(seconds since 1970= %d)  \n\n ",ctime(&tp),tp);
 ss_prog(prog_msg,flag_RT_broadcast);
 sleep(length);


/******** gason becomes the disable for the monitor trigger ****************************   */     

     
        sprintf(exename,"exec gason");
        sprintf(prog_msg,"%s \n",exename);
        ss_prog(prog_msg,flag_RT_broadcast);
        success = DP_cmd(EC_laser_roc,exename,result,111);
        ret_value = 0;
     
        if ( success == 0)
          {
           sprintf(prog_msg," sucessful excecution \n buffer= %s \n",result); 
           ss_prog(prog_msg,flag_RT_broadcast);
          }
        else
          {
           sprintf(prog_msg,"unable to reach controller "); 
           ss_prog(prog_msg,flag_RT_broadcast);
	   reboot_camac1();
          }
/***************************************************************/

/***************************************/
     sprintf(prog_msg," monitor source data recording stopped\n");
     ss_prog(prog_msg,flag_RT_broadcast);

     /******* send 1202 1100 event to the event stream ****/ 
     tp=time(NULL);
     tape_event_buffer[0]=1202;
     tape_event_buffer[1]=tp;
     tape_event_buffer[2]=0;
     tape_event_buffer[3]=0;
 
     tape_event_buffer[4]=1100;
     tape_event_buffer[5]=9999;
     tape_event_buffer[6]=length;
     tape_event_buffer[7]=0;
     no_of_events=2;
     
     boswrite(tape_event_buffer,no_of_events);

     sprintf(prog_msg," broadcast a 1202 1100 event for tape ");
     ss_prog(prog_msg,flag_RT_broadcast);

/******************************************************/


return(0);

}

