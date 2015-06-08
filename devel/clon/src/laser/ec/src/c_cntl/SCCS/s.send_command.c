h64964
s 00060/00255/00026
d D 1.2 04/08/11 11:58:35 sergpozd 3 1
c "camac3" --> char *EC_laser_roc="camac1"
e
s 00000/00000/00000
d R 1.2 00/09/21 14:11:10 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 laser/ec/src/c_cntl/send_command.c
e
s 00281/00000/00000
d D 1.1 00/09/21 14:11:09 wolin 1 0
c date and time created 00/09/21 14:11:09 by wolin
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
I 3
#include <laser.h>
#include "EC_laser_ROC.h"
E 3

D 3
#define BUFFER_LENGTH 512
#define DELIM '!'
#define START_INFO '!!'
#define EOS '\0'
#define MAX_DATA 5


int      int_data[MAX_DATA];
float   real_data[MAX_DATA];
char string_data[MAX_DATA][80];
int nreal=0;
int nint=0;
int nstring=0;
int ret_value;


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
 char  command_string[80];
 int   success=1;
E 3

D 3

int debug=0;
int j=0;
char command_string[80];
int success=1;

E 3
/* intialize DP stuff */

 printf("initializing the connection \n");
 DP_cmd_init("clon10");
 printf("initializing done \n");
 
D 3
  
E 3
I 3
 n1=&nint;
 n2=&nreal;
 n3=&nstring;
E 3

 if (argc < 2) 
D 3
      {
      /* if no arguments, get the file parameters from user */
      printf("  enter command string :");
      scanf("%s",command_string);   
      }
E 3
I 3
   {
     /* if no arguments, get the file parameters from user */
     printf("  enter command string :");
     scanf("%s",command_string);   
   }
E 3
 else
D 3
      {
      /* if arguments, use them as values for freq and number
         (after casting them to the declared types)          */
      sprintf(command_string,"%s",argv[1]);
      }
       


/* 1   command ****************************   */ 
  
E 3
I 3
   {
     /* if arguments, use them as values for freq and number
	(after casting them to the declared types)          */
     sprintf(command_string,"%s",argv[1]);
   }
 
 /* 1   command ****************************   */ 
E 3
     
D 3
     sprintf(exename,"exec %s",command_string);
     printf("%s \n",exename);
     success = DP_cmd("camac3",exename,result,111);
     
     if ( success == 0)
       {
        printf(" sucessful excecution \n buffer= %s \n",result);
        parse(result);
       }
    else
       {
         printf("unable to reach controller "); 
       }     
   
E 3
I 3
 sprintf(exename,"exec %s",command_string);
 printf("%s \n",exename);
 success = DP_cmd(EC_laser_roc,exename,result,111);
 ret_value = 0;
E 3
 
I 3
 if ( success == 0)
   {
     printf(" sucessful excecution \n buffer= %s \n",result);
     ret_value= parse(result, int_data, real_data, string_data,n1,n2,n3);
   }
 else
   {
     printf("unable to reach controller "); 
   }     
E 3
 
D 3
   printf(" \n number of real %d \n", nreal);
   for(j=0; j < nreal; j++){printf(" real value %d  = %f \n", j+1 , real_data[j]);}
   
E 3
I 3
 printf(" \n number of real %d \n", nreal);
 for(j=0; j < nreal; j++){printf(" real value %d  = %f \n", j+1 , real_data[j]);}
E 3
 
D 3
   printf(" \n number of int %d \n", nint);
   for(j=0; j < nint; j++){printf(" integer value %d  = %d \n", j+1 , int_data[j]);}
   
   printf(" \n number of strings %d \n", nstring);
   for(j=0; j < nstring; j++){printf(" string value %d  = %s \n", j+1 , string_data[j]);} 

   printf(" \n \n value of the function is = %d \n",ret_value); 
}


int parse(char *result)
{
char type_char[1];
short int  type_int;
int j=0 ;
int k=0 ;
int type_flag;
int start_flag=0;
int end_flag=0;
int data_flag=0;
int debug=0;
char *ret_val_ptr;
char *string_ptr[MAX_DATA];
char tmp_buf[BUFFER_LENGTH];
int buffer_size=BUFFER_LENGTH;

     
debug=0;

for(j=0; j < 5; j++){string_ptr[j] = string_data[j];} /* use the ptr to load the data */

k=0;   /* k will keep trak of the numger of cahr currently in tem buffer */
j=0;    /*  j will keep track of the postion in the returned data buffer */
         /* look until end of string or end of buffer */


while (   (result[j] != EOS) && ( j <= buffer_size)   )  
 { 
E 3
 
D 3
       	
    if(debug==1) printf("loop %d  buf=%d \n",j, buffer_size );
    
E 3
I 3
 printf(" \n number of int %d \n", nint);
 for(j=0; j < nint; j++){printf(" integer value %d  = %d \n", j+1 , int_data[j]);}
E 3
 
D 3
 /* ************** check for start *********************  */
    
    if( result[j]==DELIM && result[j+1]==DELIM && start_flag==0)
      {
      start_flag=1;
      data_flag=1;
      type_flag=1;
      if(debug==1) printf("  found start \n");
      j++; 
      j++;  /* now pointing at the start of no of args */
      strcpy(tmp_buf, &result[j]);
      k=0;      
      }
      
/* *****************look for end of data */

    if ( result[j]==DELIM && result[j+1]==DELIM && start_flag==1) 
      {
      tmp_buf[k]=EOS ;
      if(debug==1) printf(" end of message found \n" ) ;
      end_flag=1;
      data_flag=0;
      
      if(k == 0) printf(" variable lenght is zero ");
      if(debug==1) printf(" getting number \n");
   
    switch (type_int)
        {
          case 'R': 
          case 'r': 
                    sscanf(tmp_buf,"%f", &real_data[nreal]);
                    if(debug==1) printf(" real found = %f  \n", real_data[nreal]);
                    nreal++; 
                    break;
           
          case 'I': 
          case 'i':          
                    sscanf(tmp_buf,"%d", &int_data[nreal]);
                    if(debug==1) printf(" int found = %d \n", int_data[nint]);
                    nint++; 
                    break;
          
          case 'S': 
          case 's':
                    sprintf(string_ptr[nstring],"%s",tmp_buf);
                    if(debug==1) printf(" string found = %s  \n", string_data[nstring]); 
                    nstring++;;
                    break;
          default: printf(" error unknown type \n");
        } 

      } 
E 3
I 3
 printf(" \n number of strings %d \n", nstring);
 for(j=0; j < nstring; j++){printf(" string value %d  = %s \n", j+1 , string_data[j]);} 
E 3
 
D 3
 /*********  check for type ******************* */
    
    if (result[j]==DELIM && data_flag == 1 && type_flag == 1 )
      {
      tmp_buf[k]=EOS;
      if(k != 1) printf(" type variable too long ");
      if(debug==1) printf(" type= %s ", tmp_buf);
      sprintf(type_char,"%s",tmp_buf); 
      if(debug==1) printf(" expecting %s  value \n",type_char);
      type_int=type_char[0];
      k=0;
      j++;
      type_flag=0;
      strcpy(tmp_buf, &result[j]);
      } 
      
  /*    get number   ******************  */
  
  if (result[j]==DELIM && data_flag == 1  && type_flag == 0 )
      {
      tmp_buf[k]=EOS;
      if(k == 0) printf(" variable lenght is zero ");
      if(debug==1) printf(" getting number \n");
   
    switch (type_int)
        {
          case 'R': 
          case 'r': 
                    sscanf(tmp_buf,"%f", &real_data[nreal]);
                    if(debug==1) printf(" real found = %f  \n", real_data[nreal]);
                    nreal++; 
                    break;
           
          case 'I': 
          case 'i':          
                    sscanf(tmp_buf,"%d", &int_data[nint]);
                    if(debug==1) printf(" int found = %d \n", int_data[nint]);
                    nint++; 
                    break;
          
          case 'S': 
          case 's':
                    sprintf(string_ptr[nstring],"%s",tmp_buf);
                    if(debug==1) printf(" string found = %s  \n", string_data[nstring]); 
                    nstring++;;
                    break;
          default: printf(" error unknown type \n");
        } 
       
      k=0;
      j++;
      type_flag=1;
      strcpy(tmp_buf, &result[j]);
      
     
      }      
      
      
          
    j++ ;   /* j follows the data in main buffer */
    k++ ;   /* k follows the data in the tmp buffer */
    
    if(debug==1) printf("end of loop %d  buf=%d \n",j, buffer_size );
    
   if (debug==1)
   {
      if(   (result[j] != EOS) && ( j <= buffer_size) ) 
       {
       printf(" statement true ");
       putchar(result[j]);
       }
      else
       {
       printf(" statement false");
       }
   }  
     
    
  }
E 3
I 3
 printf(" \n \n value of the function is = %d \n",ret_value); 
E 3

D 3
   
 if (start_flag != 1) 
    {
    printf( "\n  error no start delimeters found") ;
    }
    
 if (end_flag != 1) 
    {
    printf( "\n error no end delimeter found") ;
    }


ret_value=11111;
ret_val_ptr = strstr(result,"value =");
if(ret_val_ptr != NULL )
{
   strcpy(tmp_buf, (ret_val_ptr+7));
   ret_val_ptr= strstr(tmp_buf," ="); 
   *ret_val_ptr= EOS;
   sscanf(tmp_buf,"%d", &ret_value);
}  
E 3
}
I 3

E 3
E 1
