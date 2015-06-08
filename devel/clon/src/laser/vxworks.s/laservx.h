
/* laservx.h - header file for the laser calibration project (vxworks only) */

/* function prototypes */

/* laser.c */
void set_address ();
void delay   (int seconds);

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


/* motor.c */
int  open_port   (int channel);
int  close_port  (int channel);
int  writeBuffer (int channel, char *buff);
int  readBuffer  (int channel, char *buff, int nbytes);
int  decodBufferChar (char *str, int ch);
void read_buffer (int channel);

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
int   close_ECport();

char  gason();
void  hp_pulseron();
void  hp_pulseroff();
void  gasoff();


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

