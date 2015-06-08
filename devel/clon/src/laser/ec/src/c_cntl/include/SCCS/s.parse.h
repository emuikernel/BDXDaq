h12662
s 00000/00000/00000
d R 1.2 00/09/21 14:12:21 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 laser/ec/src/c_cntl/include/parse.h
e
s 00040/00000/00000
d D 1.1 00/09/21 14:12:20 wolin 1 0
c date and time created 00/09/21 14:12:20 by wolin
e
u
U
f e 0
t
T
I 1
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
E 1
