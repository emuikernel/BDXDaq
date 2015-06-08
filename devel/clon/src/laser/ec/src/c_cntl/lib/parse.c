/*
   subroutine for use with laser calibration
   Kevin Giovanetti   Jan 99
*/

/******************************* parse    ************************/

/*
    this program can be found in the library liblaser.a  the array sizes
     are specified here as  parameters

     BUFFER_LENGTH size of the buffer passed to parse for parsing
     DELIM  character that mark the stuff of interest in the buffer  
     MAX_DATA the maximum number of characters expecte of any one type.
     
   the buffer passed to parse contains:
   
   .....garbage... !!format!data!foramt!data...!format!data!!  ...garbage
   .... value = data =.....
   
   the parse routines isolates the delimeted text and returns the values
   int_data    [*n1]
   real_data   [*n2]
   string_data [*n3]
   and the value of the function is the value in the string.
   
   routine is meant to be used with DP_cmd to parse messages returned 
   from camac1.

*/   

#include <laser.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int
parse(
      char  *result,
      int   int_data[MAX_DATA],
      float real_data[MAX_DATA],
      char  string_data[MAX_DATA][80],
      int   *n1,
      int   *n2,
      int   *n3)
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
 int ret_value;
 int nreal=0;
 int nint=0;
 int nstring=0;
 debug=0;


for(j=0; j < 5; j++){string_ptr[j] = string_data[j];} /* use the ptr to load the data */

k=0;   /* k will keep trak of the numger of cahr currently in tem buffer */
j=0;    /*  j will keep track of the postion in the returned data buffer */
         /* look until end of string or end of buffer */


while (   (result[j] != EOS) && ( j <= buffer_size)   )  
 { 
 
       	
    if(debug==1) printf("loop %d  buf=%d \n",j, buffer_size );
    
 
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

   
 if (start_flag != 1) 
    {
    if(debug==1) printf( "\n  error no start delimeters found") ;
    }
    
 if (end_flag != 1) 
    {
    if(debug==1) printf( "\n error no end delimeter found") ;
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
*n1=nint;
*n2=nreal;
*n3=nstring;

 if(debug==1)
 {
  printf(" \n number of real %d \n", nreal);
   for(j=0; j < nreal; j++){printf(" real value %d  = %f \n", j+1 , real_data[j]);}
   
 
   printf(" \n number of int %d \n", nint);
   for(j=0; j < nint; j++){printf(" integer value %d  = %d \n", j+1 , int_data[j]);}
   
   printf(" \n number of strings %d \n", nstring);
   for(j=0; j < nstring; j++){printf(" string value %d  = %s \n", j+1 , string_data[j]);}

   printf(" \n \n value of the function is = %d \n",ret_value);
  }

return ret_value;

}

