/* dpS execution of the laser commands giovanetti, Larson
 */

#define INIT 1

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <time.h>
#include <sys/times.h>
#include <sys/param.h>
#include <stdlib.h>
#include <bit_defs.h>
#include <laser.h>
#include "EC_laser_ROC.h"

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

/* 
  tp is the integer number of seconds since 1/1/1970 
  via program Thu Jun  4 10:57:38 1998 numerical time= 896972258
*/
 
 time_t tp;
 time_t start_t;
 time_t stop_t;
 time_t dif_t;
 int repeat_count=1;
 int repeat_wait=0;
 int cont_flag=0;
   
int
main()
{
 char  exename[111];
 char  result[BUFFER_LENGTH];
 int   data;
 float freq;
 long  period_10usec;
 int   numberOfPulses;
 
/**********  init variables  ***************/
 n1=&nint;
 n2=&nreal;
 n3=&nstring;
 
/* intialize DP stuff */

 printf("initializing the connection \n");
 DP_cmd_init("clon10");
 printf("initializing done \n");
 
 /*
 printf("debug_pulser (1=on 0=off)=?");
 scanf("%d,\n",&data);

 
     printf(" there is a limited amount of data to be passed sometimes debug crashes program");
     */
 data=0;
     sprintf(exename,"exec debug_pulser=%d",data);
     printf("%s \n",exename);
     DP_cmd(EC_laser_roc,exename,result,111);
     printf("%s \n",result);

     /*
 printf(" frequency of pulser=?");
 scanf("%f,\n",&freq);
 */
 freq=10.0;
 printf(" using freq= %f \n",freq);
 period_10usec= (long) 100000./freq;
 printf(" the period in 10 usec steps is %ld \n",period_10usec);

 
 
     sprintf(exename,"exec period_10usec=%ld",period_10usec);
     printf("%s \n",exename);
      DP_cmd(EC_laser_roc,exename,result,111);
     printf("%s \n",result);
     /*
 printf(" number of pulses=?");
 scanf("%d,\n",&numberOfPulses);
 */
numberOfPulses=300;
 printf(" using number of pulses = %d \n",numberOfPulses);
 
 
 
     sprintf(exename,"exec numberOfPulses=%d",numberOfPulses);
     printf("%s \n",exename);
      DP_cmd(EC_laser_roc,exename,result,111);
     printf("%s \n",result);

/* 1   command ****************************   */   
  
     sprintf(exename,"exec make_pulser_data");
     printf("%s \n",exename);
     DP_cmd(EC_laser_roc,exename,result,111);
     printf("%s \n",result);

/*2 load data ********************************* */
 
      sprintf(exename,"exec load_pulser");
      printf("%s \n",exename); 
      DP_cmd(EC_laser_roc,exename,result,111);
      printf("%s \n",result);
      /*      
 printf(" continuous 1= cont, 0= reapeat mode =?");
 
 
 scanf("%d,\n",&cont_flag);
 */
cont_flag=0;
 

 if(cont_flag)
 	{
   	printf(" continuous mode be sure cabe from cycl complete is in place \n");
        sprintf(exename,"exec ext_start_on");
        printf("%s \n",exename); 
        DP_cmd(EC_laser_roc,exename,result,111);
        printf("%s \n",result);
        repeat_wait=0;
        repeat_count=1;
        }
   else 	
   	{
	  /*
	    printf(" number of cycles=?");
	    scanf("%d,\n",&repeat_count);
	  */
	  /*
	    repeat_count=100;
	  */
	  repeat_count=5;
	  printf(" repeating= %d \n",repeat_count);
	  /*
	    printf(" seconds to wait in between =?");
	    scanf("%d,\n",&repeat_wait);
	  */
	repeat_wait=1;
 	printf(" wait %d seconds between cycles \n",repeat_wait);
        }
        
 while(repeat_count >0 ){
       	printf("Number of times to loop =%d  \n",repeat_count);
    	repeat_count--;     
 	/**************************get time *****************/
 	tp=time(NULL);
 	printf("start time= %s(seconds since 1970= %d)  \n\n ",ctime(&tp),tp);
	 start_t=tp;
 
      	sprintf(exename,"exec dataway_start");
     	 printf("%s \n",exename); 
      	DP_cmd(EC_laser_roc,exename,result,111);
      	printf("%s \n",result); 
      
      	bit03=1;
       printf(" start pulser wait until done \n",result);
            
      	while(bit03){
      
      		sprintf(exename,"exec get_pulser_status");
/*      	printf("%s \n",exename); */
      		DP_cmd(EC_laser_roc,exename,result,111);
/*      	printf("%s \n",result); */
      
      		ret_value= parse(result, int_data, real_data, string_data,n1,n2,n3);
    
/*     		printf("int data %d \n",int_data[0]); */
     	  
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
/*      	printf("bit3 %d \n",bit03);   */   
        	sleep(1);
 	}

        
	 /**************************get time *****************/
 	tp=time(NULL);
 	printf("start time= %s(seconds since 1970= %d)  \n\n ",ctime(&tp),tp);
 	stop_t=tp;      
 	dif_t=stop_t-start_t;       
 	printf("total pulser on time= %d seconds  \n\n ",dif_t);
 	sleep(repeat_wait);
	}
             
}

