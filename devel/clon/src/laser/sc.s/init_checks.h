/*
 * init_checks.h
 *
 * List of variables that contain status requirement to run TOF_laser
 *
 */

#define MAX_LSR_NO 5

/* INIT section */
#if defined(INIT)
int  debug_value=0;
char configuration[50]="TOF_LASER";
char coda_state[50]="active";
int  bit_number = 10;
int  use_laser[MAX_LSR_NO]={0, 1,1,1,1};

#elif !defined(INIT)
extern  int  debug_value;
extern  char configuration[50];
extern  char coda_state[50];
extern  int  bit_number;
extern  int  use_laser[MAX_LSR_NO];
#endif

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

#elif !defined(INIT)
extern int seq_mask[MAX_LSR_NO];
extern int seq_mpos[MAX_LSR_NO][12];
extern int seq_mspeed[MAX_LSR_NO];
extern int seq_fspeed[MAX_LSR_NO];
extern int seq_floops[MAX_LSR_NO];
#endif

