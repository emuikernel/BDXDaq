/* 
   program written by Kevin Giovanetti, J.Voshell, A.Larson
   for use with laser calibration systems

   giovankl@jmu.edu  Jan 99

  **********************    parse    ***********************

  this program can be found in the library liblaser.a
  the array sizes are specified here as  parameters

  BUFFER_LENGTH - size of the buffer passed to parse for parsing
  DELIM         - separator in the buffer
  MAX_DATA      - maximum number of characters expect of any one type.

  the buffer passed to parse contains:
    ...garbage...!!format!data!foramt!data...!format!data!!...garbage
    ...value = data =...

  the parse routines isolates the delimeted text and returns the values
    int_data    [*n1]
    real_data   [*n2]
    string_data [*n3]
    and the value of the function is the value in the string.

  routine is meant to be used with DP_cmd to parse messages
  returned from camac1.
*/

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

