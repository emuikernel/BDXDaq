h11013
s 00032/00032/00075
d D 1.2 04/08/10 18:42:16 sergpozd 3 1
c "camac3" --> char *EC_laser_roc="camac1"
e
s 00000/00000/00000
d R 1.2 00/09/21 14:11:08 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 laser/ec/src/c_cntl/laser_motor_com.c
e
s 00107/00000/00000
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
D 3
 */ 
E 3
I 3
 */

E 3
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <laser.h>
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
D 3
char exename[111];
char result[BUFFER_LENGTH];

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


int debug=0;
int j=0;

char *ptr;
char mid_command_string[80];
char end_command_string[80]="\")}";
char command_string[80]= "{laser_motor(\"$1";
int success=1;


E 3
I 3
 char  exename[111];
 char  result[BUFFER_LENGTH];
 
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
 int   debug=0;
 int   j=0;
 
 char  *ptr;
 char  mid_command_string[80];
 char  end_command_string[80] = "\")}";
 char  command_string[80]     = "{laser_motor(\"$1";
 int   success=1;
 
 
E 3
 n1=&nint;
 n2=&nreal;
 n3=&nstring;
D 3
  
E 3
I 3
 
E 3
/* intialize DP stuff */

 printf("initializing the connection \n");
 DP_cmd_init("clon10");
 printf("initializing done \n");
 
      sprintf(exename,"exec laser_motor_init");
     printf("%s \n",exename);
D 3
     success = DP_cmd("camac3",exename,result,111);
E 3
I 3
     success = DP_cmd(EC_laser_roc,exename,result,111);
E 3
     ret_value = 0;
     
     if ( success == 0)
       {
        printf(" sucessful excecution \n buffer= %s \n",result);
        ret_value= parse(result, int_data, real_data, string_data,n1,n2,n3);
       }
    else
       {
         printf("unable to reach controller "); 
       }     
   
 


 if (argc < 2) 
      {
      /* if no arguments, get the file parameters from user */
      printf("  enter command for motor type \"blank\" for no command :");
      scanf("%s",mid_command_string);   
      }
 else
      {
      /* if arguments, use them as values for freq and number
         (after casting them to the declared types)          */
      sprintf(mid_command_string,"%s",argv[1]);
      }
      
      if( (ptr=strstr(mid_command_string,"blank")) != NULL) {mid_command_string[0]=EOS;}
      strcat(command_string,mid_command_string);
      strcat(command_string,end_command_string);
      
      printf(" =%s\n",command_string);

/* 1   command ****************************   */ 
  
     
     sprintf(exename,"exec %s",command_string);
     printf("%s \n",exename);
D 3
     success = DP_cmd("camac3",exename,result,111);
E 3
I 3
     success = DP_cmd(EC_laser_roc,exename,result,111);
E 3
     ret_value = 0;
     
     if ( success == 0)
       {
 /*       printf(" sucessful excecution \n buffer= %s \n",result);  */
        ret_value= parse(result, int_data, real_data, string_data,n1,n2,n3);
       }
    else
       {
         printf("unable to reach controller "); 
       }     
   
 
   printf(" \n  motor command sent= %s ",mid_command_string);
   printf(" \n    resulting status= %d", int_data[0]);
   printf(" \n     motor response = %s \n", string_data[0]);

}
I 3

E 3
E 1
