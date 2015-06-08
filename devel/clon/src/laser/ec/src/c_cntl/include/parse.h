/* variables for parsing buffer   */
#if defined(INIT) 

     int int_data[MAX_DATA];
     int ret_value;
     float   real_data[MAX_DATA];
     char string_data[MAX_DATA][80];

     int nreal;
     int nint;
     int nstring;

     int *n1=&nint; 
     int *n2=&nreal;
     int *n3=&nstring;
     
     char exename[111];
     char result[BUFFER_LENGTH];
     
 
#elif !defined(INIT) 
     

     extern int int_data[MAX_DATA];
     extern int ret_value;
     extern float   real_data[MAX_DATA];
     extern char string_data[MAX_DATA][80];

     extern int nreal;
     extern int nint;
     extern int nstring;

     extern int *n1;
     extern int *n2;
     extern int *n3;

     extern char exename[111];
     extern char result[BUFFER_LENGTH];
 
#endif
