h49667
s 00044/00043/00010
d D 1.2 04/08/10 16:18:34 sergpozd 3 1
c *** empty log message ***
e
s 00000/00000/00000
d R 1.2 00/09/21 14:12:21 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 laser/ec/src/c_cntl/include/laser.h
e
s 00053/00000/00000
d D 1.1 00/09/21 14:12:20 wolin 1 0
c date and time created 00/09/21 14:12:20 by wolin
e
u
U
f e 0
t
T
I 1
/* 
D 3
program written by kevin giovanetti, J Voshell, A larson for use with
laser calibration systems
giovankl@jmu.edu
E 3
I 3
   program written by Kevin Giovanetti, J.Voshell, A.Larson
   for use with laser calibration systems
E 3

D 3
Jan 99 (Y2k-1)
E 3
I 3
   giovankl@jmu.edu  Jan 99
E 3

D 3
 */
 
/******************************* parse    ************************/
  
/*
    this program can be found in the library liblaser.a  the array sizes
     are specified here as  parameters
E 3
I 3
  **********************    parse    ***********************
E 3

D 3
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
   from camac3.
E 3
I 3
  this program can be found in the library liblaser.a
  the array sizes are specified here as  parameters
E 3

D 3
*/   
E 3
I 3
  BUFFER_LENGTH - size of the buffer passed to parse for parsing
  DELIM         - separator in the buffer
  MAX_DATA      - maximum number of characters expect of any one type.
E 3

D 3
#define BUFFER_LENGTH 2048
#define DELIM '!'
#define START_INFO '!!'
#define EOS '\0'
#define MAX_DATA 5
#define CR       13
#define LF	 10
#define SPACE    32
#define HTAB      9 
#define GTsym       '>'
#define ETX	         3
E 3
I 3
  the buffer passed to parse contains:
    ...garbage...!!format!data!foramt!data...!format!data!!...garbage
    ...value = data =...
E 3

I 3
  the parse routines isolates the delimeted text and returns the values
    int_data    [*n1]
    real_data   [*n2]
    string_data [*n3]
    and the value of the function is the value in the string.
E 3

D 3
   
extern int parse(char *result, int int_data[MAX_DATA], float real_data[MAX_DATA],char string_data[MAX_DATA][80],int *n1,int *n2,int *n3);
E 3
I 3
  routine is meant to be used with DP_cmd to parse messages
  returned from camac1.
*/
E 3

D 3
/*******************************     ************************/
E 3
I 3
#define BUFFER_LENGTH  2048
#define DELIM          '!'
#define START_INFO     '!!'
#define EOS            '\0'
#define MAX_DATA       5
#define CR             13
#define LF             10
#define SPACE          32
#define HTAB           9
#define GTsym          '>'
#define ETX            3


/* function prototype */

extern int parse (
		  char  *result,
		  int   int_data[MAX_DATA],
		  float real_data[MAX_DATA],
		  char  string_data[MAX_DATA][80],
		  int   *n1,
		  int   *n2,
		  int   *n3
		  );
E 3

E 1
