h05783
s 00123/00128/00046
d D 1.2 04/08/11 16:44:58 sergpozd 3 1
c *** empty log message ***
e
s 00000/00000/00000
d R 1.2 00/09/21 14:11:12 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 laser/ec/src/c_cntl/trigger_status.c
e
s 00174/00000/00000
d D 1.1 00/09/21 14:11:11 wolin 1 0
c date and time created 00/09/21 14:11:11 by wolin
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
 
E 3
I 3

E 3
 /*  determines if the variables are defined or referenced    */ 
D 3
#define INIT=1
E 3
I 3
#define INIT 1
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

D 3
int main(int argc, char *argv[])
E 3
I 3
int
main(int argc, char *argv[])
E 3
{
D 3
char exename[111];
char result[BUFFER_LENGTH];
int mstat=0;
E 3
I 3
 char exename[111];
 char result[BUFFER_LENGTH];
 int  mstat=0;
 int  debug=0;
 int  j;
E 3

D 3
int debug=0;

int j;
E 3
/* variables for parsing buffer   */
I 3
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
 int   flag_loop=0;
E 3

D 3
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
E 3

D 3
int success=0;
int flag_loop=0;

E 3
/**********  init variables  ***************/
 n1=&nint;
 n2=&nreal;
 n3=&nstring;
D 3
 
  if (argc < 2)      /* if no arguments, get  parameter from user */
      { 
      printf("  enter debug= 1, or 0 1 means print:  ");
      scanf("%d",&debug);
      if(debug)printf(" debug =%d\n",debug);
  
      }
E 3
I 3

 if (argc < 2)      /* if no arguments, get  parameter from user */
   {
     printf("  enter debug= 1, or 0 1 means print:  ");
     scanf("%d",&debug);
     if(debug)printf(" debug =%d\n",debug);
   }
E 3
 else                   /* if arguments, use them         */
D 3
      {
      
        sscanf(argv[1],"%d",&debug);
      } 
      
         
      if(debug)printf(" debug =%d\n",debug);
E 3
I 3
   {
     sscanf(argv[1],"%d",&debug);
   }
E 3

I 3
 if(debug)printf(" debug =%d\n",debug);
E 3

D 3
/* intialize DP stuff */
E 3

I 3
/* intialize DP stuff */
E 3
 printf("initializing the connection \n");
 DP_cmd_init("clon10");
 printf("initializing done \n");
 flag_loop=1;  

/************************  command ***************************/

D 3
     sprintf(exename,"exec ts_regs");
     printf("\n %s \n",exename);
     success = DP_cmd("clastrig2",exename,result,111);
     
     if ( success == 0)
       {

        printf(" All trigger registers \n");

        printf(" sucessful excecution \n buffer= %s \n",result);        
        
       }
    else
       {
         printf("unable to reach controller "); 
       }     
   
    /*********************************************/ 
E 3
I 3
 sprintf(exename,"exec ts_regs");
 printf("\n %s \n",exename);
 success = DP_cmd("clastrig2",exename,result,111);
 
 if ( success == 0)
   {
     printf(" All trigger registers \n");
     printf(" sucessful excecution \n buffer= %s \n",result);        
   }
 else
   {
     printf("unable to reach controller "); 
   }     
 
/*********************************************/ 
E 3
  

D 3
 /************************  command 1  ***************************/
E 3
I 3
/************************  command 1  ***************************/
E 3

D 3
     sprintf(exename,"exec ts_cstatus");
     printf("\n %s \n",exename);
     success = DP_cmd("clastrig2",exename,result,111);
E 3
I 3
 sprintf(exename,"exec ts_cstatus");
 printf("\n %s \n",exename);
 success = DP_cmd("clastrig2",exename,result,111);
 
 if ( success == 0)
   {
     ret_value=strtoul(result,NULL, 16);
     printf(" Trigger status register = %x \n", ret_value);
E 3
     
D 3
     if ( success == 0)
       {
	ret_value=strtoul(result,NULL, 16);
        printf(" Trigger status register = %x \n", ret_value);

        if(debug)printf(" sucessful excecution \n buffer= %s \n",result);        
        bit_SET32(ret_value, debug);
	printf(" %d%d%d%d %d%d%d%d %d%d%d%d %d%d%d%d %d%d%d%d %d%d%d%d %d%d%d%d %d%d%d%d \n ",
	bit31,bit30,bit29,bit28,bit27,bit26,bit25,bit24,bit23,bit22,bit21,bit20,bit19,bit18,bit17,bit16,	      
	bit15,bit14,bit13,bit12,bit11,bit10,bit09,bit08,bit07,bit06,bit05,bit04,bit03,bit02,bit01,bit00);
	if (bit00) {printf("trigger in GO mode \n");}
	else  {printf("trigger in Paused mode \n");}
   
       }
    else
       {
         printf("unable to reach controller "); 
       }     
   
    /*********************************************/ 
 /************************  command 1  ***************************/
     sprintf(exename,"exec ts_rocenable");
     printf("\n %s \n",exename);
     success = DP_cmd("clastrig2",exename,result,111);
E 3
I 3
     if(debug)printf(" sucessful excecution \n buffer= %s \n",result);        
     bit_SET32(ret_value, debug);
     printf(" %d%d%d%d %d%d%d%d %d%d%d%d %d%d%d%d %d%d%d%d %d%d%d%d %d%d%d%d %d%d%d%d \n ",
	    bit31,bit30,bit29,bit28,bit27,bit26,bit25,bit24,
	    bit23,bit22,bit21,bit20,bit19,bit18,bit17,bit16,	      
	    bit15,bit14,bit13,bit12,bit11,bit10,bit09,bit08,
	    bit07,bit06,bit05,bit04,bit03,bit02,bit01,bit00);
     if (bit00) {printf("trigger in GO mode \n");}
     else  {printf("trigger in Paused mode \n");}
E 3
     
D 3
     if ( success == 0)
       {
	ret_value=strtoul(result,NULL, 16);
        printf(" Rocs currently enabled = %x \n", ret_value);

        if(debug)printf(" sucessful excecution \n buffer= %s \n",result);        
        bit_SET32(ret_value, debug);
	printf(" %d%d%d%d %d%d%d%d %d%d%d%d %d%d%d%d %d%d%d%d %d%d%d%d %d%d%d%d %d%d%d%d \n ",
	bit31,bit30,bit29,bit28,bit27,bit26,bit25,bit24,bit23,bit22,bit21,bit20,bit19,bit18,bit17,bit16,	      
	bit15,bit14,bit13,bit12,bit11,bit10,bit09,bit08,bit07,bit06,bit05,bit04,bit03,bit02,bit01,bit00);	
E 3
I 3
   }
 else
   {
     printf("unable to reach controller "); 
   }     
 
/*********************************************/ 
/************************  command 1  ***************************/
 sprintf(exename,"exec ts_rocenable");
 printf("\n %s \n",exename);
 success = DP_cmd("clastrig2",exename,result,111);
 
 if ( success == 0)
   {
     ret_value=strtoul(result,NULL, 16);
     printf(" Rocs currently enabled = %x \n", ret_value);
     
     if(debug)printf(" sucessful excecution \n buffer= %s \n",result);        
     bit_SET32(ret_value, debug);
     printf(" %d%d%d%d %d%d%d%d %d%d%d%d %d%d%d%d %d%d%d%d %d%d%d%d %d%d%d%d %d%d%d%d \n ",
	    bit31,bit30,bit29,bit28,bit27,bit26,bit25,bit24,
	    bit23,bit22,bit21,bit20,bit19,bit18,bit17,bit16,	      
	    bit15,bit14,bit13,bit12,bit11,bit10,bit09,bit08,
	    bit07,bit06,bit05,bit04,bit03,bit02,bit01,bit00);	
     
   }
 else
   {
     printf("unable to reach controller "); 
   }     
E 3
   
D 3
       }
    else
       {
         printf("unable to reach controller "); 
       }     
   
    /*********************************************/ 
 /************************  command ***************************/
     sprintf(exename,"exec ts_control");
     printf("\n %s \n",exename);
     success = DP_cmd("clastrig2",exename,result,111);
E 3
I 3
/*********************************************/ 
/************************  command ***************************/
 sprintf(exename,"exec ts_control");
 printf("\n %s \n",exename);
 success = DP_cmd("clastrig2",exename,result,111);
 
 if ( success == 0)
   {
     ret_value=strtoul(result,NULL, 16);
     printf(" Trigger bits enabled = %x \n", ret_value);
E 3
     
D 3
     if ( success == 0)
       {
	ret_value=strtoul(result,NULL, 16);
        printf(" Trigger bits enabled = %x \n", ret_value);
E 3
I 3
     if(debug)printf(" sucessful excecution \n buffer= %s \n",result);        
     bit_SET32(ret_value, debug);
     
     printf(" %d%d%d%d %d%d%d%d %d%d%d%d %d%d%d%d %d%d%d%d %d%d%d%d %d%d%d%d %d%d%d%d \n ",
	    bit31,bit30,bit29,bit28,bit27,bit26,bit25,bit24,
	    bit23,bit22,bit21,bit20,bit19,bit18,bit17,bit16,	      
	    bit15,bit14,bit13,bit12,bit11,bit10,bit09,bit08,
	    bit07,bit06,bit05,bit04,bit03,bit02,bit01,bit00);	
     if (bit11) {printf("ENABLED  EC claibration triggers accepted \n");}
     else  {printf("DISABlED  NO EC claibration triggers accepted \n");}
E 3

D 3
        if(debug)printf(" sucessful excecution \n buffer= %s \n",result);        
        bit_SET32(ret_value, debug);
 
	printf(" %d%d%d%d %d%d%d%d %d%d%d%d %d%d%d%d %d%d%d%d %d%d%d%d %d%d%d%d %d%d%d%d \n ",
	bit31,bit30,bit29,bit28,bit27,bit26,bit25,bit24,bit23,bit22,bit21,bit20,bit19,bit18,bit17,bit16,	      
	bit15,bit14,bit13,bit12,bit11,bit10,bit09,bit08,bit07,bit06,bit05,bit04,bit03,bit02,bit01,bit00);	
  	if (bit11) {printf("ENABLED  EC claibration triggers accepted \n");}
	else  {printf("DISABlED  NO EC claibration triggers accepted \n");}
   
       }
    else
       {
         printf("unable to reach controller "); 
       }     
   
    /*********************************************/  
E 3
I 3
   }
 else
   {
     printf("unable to reach controller "); 
   }
E 3

D 3
 
    
}
E 3
I 3
/*********************************************/
E 3

I 3
}
E 3

E 1
