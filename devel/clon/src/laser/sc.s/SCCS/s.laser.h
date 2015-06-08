h44958
s 00004/00004/00091
d D 1.3 08/12/02 20:52:58 boiarino 4 3
c *** empty log message ***
e
s 00079/00040/00016
d D 1.2 03/07/21 14:16:41 sergpozd 3 1
c Completely new revision
e
s 00000/00000/00000
d R 1.2 03/03/03 14:04:20 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 laser/sc/s/laser.h
e
s 00056/00000/00000
d D 1.1 03/03/03 14:04:19 boiarino 1 0
c date and time created 03/03/03 14:04:19 by boiarino
e
u
U
f e 0
t
T
I 3
/*
 * laser.h
 *
 * Some basic defs for TOF_laser
 *
 */
E 3
I 1

D 3
/* some basic defs for TOF_laser */
E 3
I 3
D 4
/* maximum size of string returned from DP_cmd */
E 4
I 4
/* maximum size of string returned from tcpClientCmd */
E 4
#define BUFFER_LENGTH 10000
E 3

I 3
D 4
/* ELSE_Error if DP_cmd return unexpected response */
E 4
I 4
/* ELSE_Error if tcpClientCmd return unexpected response */
E 4
#define ELSE_Error(roc) \
  else { \
   sprintf(prog_msg,"\n Error: Unexpected response from \"%s\" \n", (roc)); \
   ss_prog(prog_msg); \
D 4
   sprintf(prog_msg,"DP_cmd return result:\n ---------------------\n %s\n", \
E 4
I 4
   sprintf(prog_msg,"tcpClientCmd return result:\n ---------------------\n %s\n", \
E 4
                    command_get_result()); \
D 4
   ss_prog(prog_msg); \
E 4
I 4
					ss_prog(prog_msg); \
E 4
   return(-1); \
  }
E 3

D 3
#define EOS     '\0'
#define CR       13
#define LF	 10
#define SPACE    32
#define HTAB      9 
#define GTsym    '>'
#define ETX	  3
#define POUND    '#'
#define CARET    '^'
#define LIMIT    'O'
#define BLANK    ' '
#define DONE     'D'
  
E 3

D 3
/************ for parsing the returned string from DPcommand   */
E 3
I 3
#if defined(INIT)
/* define the input register bits */
 int status_pwr_bit[5]={ 0,   8, 4,  9, 6 };
 int status_rdy_bit[5]={ 0,   9, 6, 10, 7 };
 int status_ena_bit[5]={ 0,  10, 7, 11, 8 };
/* define the output register bits */
 int contrl_gas_bit[5]={ 0,   0, 0, 4, 0 };    /*level*/
 int contrl_pwr_bit[5]={ 0,   1, 1, 5, 1 };    /*level*/
 int contrl_ena_bit[5]={ 0,   2, 2, 6, 2 };    /*pulse*/
 int contrl_dis_bit[5]={ 0,   3, 3, 7, 3 };    /*pulse*/
#elif !defined(INIT)
 extern int status_pwr_bit[5];
 extern int status_rdy_bit[5];
 extern int status_ena_bit[5];
 extern int contrl_gas_bit[5];
 extern int contrl_pwr_bit[5];
 extern int contrl_ena_bit[5];
 extern int contrl_dis_bit[5];
#endif
E 3

D 3
#define BUFFER_LENGTH 10000
E 3

D 3
extern int parseTOF(char *results);
E 3
I 3
/* function prototypes */
E 3

D 3
/*********** fro broadcasting messages using ss_prog  set to one for broadcasting*/
E 3
I 3
/* TOF_laser_library.c */
int  command_init();
int  command_execute(char *roc, char *cmd);
char *command_get_result();
void command_print_result();
E 3

D 3
#if defined(INIT) 
E 3
I 3
int  gas_ON  (int laser_number);
int  gas_OFF (int laser_number);
int  pwr_ON  (int laser_number);
int  pwr_OFF (int laser_number);
int  TOF_laser_enable  (int laser_number);
int  TOF_laser_disable (int laser_number);
char *output (int laser_number);
char *input  (int laser_number);
E 3

D 3
/* define the inout register bits */
 int status_pwr_bit[4]={     8, 4, 9, 6 };
 int status_rdy_bit[4]={     9, 6,10, 7 };   /*laser 2 is 5 hardware 6 gui */
 int status_ena_bit[4]={    10, 7,11, 8 };  
E 3
I 3
int  open_port      (int channel);
int  close_port     (int channel);
int  nullify_motors (int channel);
int  set_mask_speed      (int channel, int speed);
int  set_filter_speed    (int channel, int speed);
int  get_mask_position   (int channel);
int  get_filter_position (int channel);
int  move_mask        (int channel, int step);
int  move_filter      (int channel, int step);
int  loop_filter      (int channel);
int  move_mask_ctrl   (int channel, int step);
int  loop_filter_ctrl (int channel);
int  init_set  (int channel);
int  init_ctrl (int channel);
E 3

D 3
/* define the output register bits  */
 int contrl_gas_bit[4]={     0, 0, 4, 0 };       /*level*/ 
 int contrl_pwr_bit[4]={     1, 1, 5, 1 };       /*level*/ 
 int contrl_ena_bit[4]={     2, 2, 6, 2 };       /*pulse*/   
 int contrl_dis_bit[4]={     3, 3, 7, 3 };       /*pulse*/   
E 3
I 3
int  ss_prog     (char *msg_prog);
int  coda_pause  ();
int  coda_resume ();
E 3

I 3
/* bits.c */
int  bit_TST (int bit, int status);
int  sbin_to_int (char *stringval);
E 3

I 3
/* init_checks.c */
int  init_checks     (int debug);
E 3

I 3
/* system_setup.c */
int  system_setup    (int debug);
E 3

D 3
#elif !defined(INIT)
E 3
I 3
/* system_check.c */
int  system_check    (int debug);
E 3

D 3
 extern int status_pwr_bit[4];
 extern int status_rdy_bit[4];
 extern int status_ena_bit[4];  

 extern int contrl_gas_bit[4];   
 extern int contrl_pwr_bit[4];   
 extern int contrl_ena_bit[4];   
 extern int contrl_dis_bit[4];   

   
#endif 
E 3
I 3
/* system_shutdown.c */
int  system_shutdown (int debug);
E 3

E 1
