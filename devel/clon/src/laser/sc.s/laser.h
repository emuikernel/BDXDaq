/*
 * laser.h
 *
 * Some basic defs for TOF_laser
 *
 */

/* maximum size of string returned from tcpClientCmd */
#define BUFFER_LENGTH 10000

/* ELSE_Error if tcpClientCmd return unexpected response */
#define ELSE_Error(roc) \
  else { \
   sprintf(prog_msg,"\n Error: Unexpected response from \"%s\" \n", (roc)); \
   ss_prog(prog_msg); \
   sprintf(prog_msg,"tcpClientCmd return result:\n ---------------------\n %s\n", \
                    command_get_result()); \
					ss_prog(prog_msg); \
   return(-1); \
  }


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


/* function prototypes */

/* TOF_laser_library.c */
int  command_init();
int  command_execute(char *roc, char *cmd);
char *command_get_result();
void command_print_result();

int  gas_ON  (int laser_number);
int  gas_OFF (int laser_number);
int  pwr_ON  (int laser_number);
int  pwr_OFF (int laser_number);
int  TOF_laser_enable  (int laser_number);
int  TOF_laser_disable (int laser_number);
char *output (int laser_number);
char *input  (int laser_number);

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

int  ss_prog     (char *msg_prog);
int  coda_pause  ();
int  coda_resume ();

/* bits.c */
int  bit_TST (int bit, int status);
int  sbin_to_int (char *stringval);

/* init_checks.c */
int  init_checks     (int debug);

/* system_setup.c */
int  system_setup    (int debug);

/* system_check.c */
int  system_check    (int debug);

/* system_shutdown.c */
int  system_shutdown (int debug);

