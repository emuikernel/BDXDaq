h47625
s 00052/00020/00050
d D 1.5 03/07/21 16:45:26 sergpozd 6 5
c Completely new revision
e
s 00001/00001/00069
d D 1.4 03/04/25 10:39:02 giovanet 5 4
c one mistake inc instead of int
c 
e
s 00004/00000/00066
d D 1.3 03/04/25 10:35:18 giovanet 4 3
c added the command close_ECport()
c 
e
s 00001/00000/00065
d D 1.2 03/03/03 14:02:08 boiarino 3 1
c first version for 2306 CPU
e
s 00000/00000/00000
d R 1.2 03/02/14 13:02:54 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 vxworks/laser/laser.h
e
s 00065/00000/00000
d D 1.1 03/02/14 13:02:53 boiarino 1 0
c date and time created 03/02/14 13:02:53 by boiarino
e
u
U
f e 0
t
T
I 1

/* laser.h - header file for the laser calibration project */

/* function prototypes */

/* laser.c */
I 3
D 6
void  set_address();
E 3
void  delay(int seconds);
void  gas(int id, int x);
void  pwr(int id, int x);
void  ena(int id, int x);
void  hexbin(short h);
void  output(int id);
void  input(int id);
E 6
I 6
void set_address ();
void delay   (int seconds);
E 6

I 6
void gas (int id, int x);
int  gas_ON  (int laser_number);
int  gas_OFF (int laser_number);

void pwr (int id, int x);
int  pwr_ON  (int laser_number);
int  pwr_OFF (int laser_number);

void ena (int id, int x);
int  TOF_laser_enable  (int laser_number);
int  TOF_laser_disable (int laser_number);

void hexbin  (unsigned short h);
char *output (int laser_number);
char *input  (int laser_number);


E 6
/* motor.c */
D 6
void  integer_to_string(int integer, char *string);
void  copy_string(char *from, char *to);
void  read_buffer(int id);
void  call_position(int id1, int id2);
void  send_command(int id1, int id2, int step);
void  loop_filter(int id);
void  pre_move(int id);
void  null(int id);
int   open_port(int id);
void  close_port(int id);
int   velmex_command(char *raw, int whichPort);
E 6
I 6
int  open_port   (int channel);
int  close_port  (int channel);
int  writeBuffer (int channel, char *buff);
int  readBuffer  (int channel, char *buff, int nbytes);
int  decodBufferChar (char *str, int ch);
void read_buffer (int channel);
E 6

I 6
char get_Velmex_status  (int channel);
int  kill_operation     (int channel);
int  nullify_motors     (int channel);
int  set_limit_switch   (int channel);
int  unset_limit_switch (int channel);

int  set_motor_speed  (int channel, int motor_number, int speed);
int  set_mask_speed   (int channel, int speed);
int  set_filter_speed (int channel, int speed);

int  get_motor_position  (int channel, int motor_number);
int  get_mask_position   (int channel);
int  get_filter_position (int channel);

int  move_mask      (int channel, int step);
int  move_mask_ctrl (int channel, int step);

int  move_filter      (int channel, int step);
int  loop_filter      (int channel);
int  loop_filter_ctrl (int channel);

int  init_set  (int channel);
int  init_ctrl (int channel);
/*--------------------------------------------------*/


E 6
/* calsys_commands.c */
int   cal_motor_init();
int   laser_motor_init();
int   send_mess(char *raw, int whichPort);
int   read_mess(int whichPort);
void  cal_motor(char *mss);
void  laser_motor(char *mss);
void  lpos_open();
void  lpos_A();
void  lpos_B();
void  semix_test();
int   inhibitclear();
int   laser_enable();
int   laser_disable(); 
int   lpoweron();
int   lpoweroff();
int   stepperon();
int   stepperoff();
I 4
D 5
inc   close_ECport();
E 5
I 5
int   close_ECport();
E 5

E 4
char  gason();
void  hp_pulseron();
void  hp_pulseroff();
void  gasoff();

I 4

D 6

E 6
E 4
/* pulser code (?) */
int   get_data();
int   load_pulser();
void  ext_start_on();
void  get_pulser_status();
void  ext_start_off();
void  dataway_start();
void  go_lpulser();
void  go_pulse();
void  stop_lpulser();

int   getstatus();
int   send_test();
int   test_pulser_data();
int   make_pulser_data();
I 6

E 6
E 1
