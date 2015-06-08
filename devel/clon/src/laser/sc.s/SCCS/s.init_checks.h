h62098
s 00038/00055/00009
d D 1.2 03/07/21 14:16:27 sergpozd 3 1
c Completely new revision
e
s 00000/00000/00000
d R 1.2 03/03/03 14:04:20 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 laser/sc/s/init_checks.h
e
s 00064/00000/00000
d D 1.1 03/03/03 14:04:19 boiarino 1 0
c date and time created 03/03/03 14:04:19 by boiarino
e
u
U
f e 0
t
T
I 1
D 3
/* ********************
 * the program has to test the statuss several devices and processes
 * the vaibles that contain the status requirements are here.
E 3
I 3
/*
 * init_checks.h
E 3
 *
D 3
 *********************************/
E 3
I 3
 * List of variables that contain status requirement to run TOF_laser
 *
 */
E 3

D 3
/*********** fro broadcasting messages using ss_prog  set to one for broadcasting*/
E 3
I 3
#define MAX_LSR_NO 5
E 3

D 3
#define MAX_SEQ_NO 25
E 3
I 3
/* INIT section */
#if defined(INIT)
int  debug_value=0;
char configuration[50]="TOF_LASER";
char coda_state[50]="active";
int  bit_number = 10;
int  use_laser[MAX_LSR_NO]={0, 1,1,1,1};
E 3

D 3
#if defined(INIT) 
 char camac3_state[50]="booted";
 char sc_laser1_state[50]="booted";
 char configuration[50]="E1_PROD";
 char coda_state[50]="active"; /* possible: active configured */ 
 int bit_number = 10;
 int debug_value=0; 
 int freq_hz=15;
 int numberOfPulses=-1;
 int interim=1;
 int use_laser[4]={1,1,1,1};
 int use_camac3=1;
 int use_sc_laser1=1;
 int no_filters[4]={1,1,1,1};  /* not currentl read in but the number of filters may be > 1 in future */

E 3
#elif !defined(INIT)
D 3
 extern  char camac3_state[50];
 extern  char sc_laser1_state[50];
 extern  char configuration[50];
 extern  char coda_state[50]; /* possible: active configured */
 extern  int bit_number; 
 extern  int debug_value;
 extern  int freq_hz;
 extern  int numberOfPulses;
 extern  int interim;
 extern  int use_laser[4];
 extern  int use_camac3;
 extern  int use_sc_laser1;
 extern  int no_filters[4];
#endif 
E 3
I 3
extern  int  debug_value;
extern  char configuration[50];
extern  char coda_state[50];
extern  int  bit_number;
extern  int  use_laser[MAX_LSR_NO];
#endif
E 3

D 3
/*****************   procedure section ie what we do  ****************/
#if defined(INIT) 
E 3
I 3
/* PROCEDURE section */
#if defined(INIT)
int seq_mask[MAX_LSR_NO]     = {0, 0,0,0,0};
int seq_mpos[MAX_LSR_NO][12] = {
  {0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
};
int seq_mspeed[MAX_LSR_NO]   = {0, 200,200,200,200};
int seq_fspeed[MAX_LSR_NO]   = {0, 30,30,30,30};
int seq_floops[MAX_LSR_NO]   = {0, 2,2,2,2};
E 3

D 3
  int seq_no=4;
  int seq_laser[MAX_SEQ_NO]={1,2,3,4};
  int seq_filter[MAX_SEQ_NO]={1,1,1,1};
  int seq_loops[MAX_SEQ_NO]={1,1,1,1};
  int seq_speed[MAX_SEQ_NO]={30,30,30,30};
  int current_seq=0;

E 3
#elif !defined(INIT)
D 3

  extern int seq_no;
  extern int seq_laser[MAX_SEQ_NO];
  extern int seq_filter[MAX_SEQ_NO];
  extern int seq_loops[MAX_SEQ_NO];
  extern int seq_speed[MAX_SEQ_NO];
  extern int current_seq;

#endif 

extern int init_checks(int debug);
E 3
I 3
extern int seq_mask[MAX_LSR_NO];
extern int seq_mpos[MAX_LSR_NO][12];
extern int seq_mspeed[MAX_LSR_NO];
extern int seq_fspeed[MAX_LSR_NO];
extern int seq_floops[MAX_LSR_NO];
#endif
E 3

E 1
