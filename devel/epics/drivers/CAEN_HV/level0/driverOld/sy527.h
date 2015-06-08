
/* sy527.h - EPICS driver support for CAEN SY527 HV mainframe through v288 CAENET board */

/* data structures */

#define MAX_NAME  81
#define MAX_SLOT  16
#define MAX_CHAN  48
#define MAX_PARAM 100

#define MAX_HVPS        2 /*was 256, reduce temporary*/
#define CAENHV_OK       0
#define CAENHV_SYSERR   1

#define LINKTYPE_TCPIP              0
#define LINKTYPE_RS232              1
#define LINKTYPE_CAENET             2

#define PARAM_TYPE_NUMERIC          0
#define PARAM_TYPE_ONOFF            1
#define PARAM_TYPE_CHSTATUS         2
#define PARAM_TYPE_BDSTATUS         3

typedef struct thread
{
  int threadid;
  struct timespec   start;
  struct timespec   end;
} THREAD;


typedef struct channel
{
  char          name[MAX_NAME];
  float         fval[MAX_PARAM];
  unsigned long lval[MAX_PARAM];
  int           setflag[MAX_PARAM]; /* if 1, need to write */
} CHANNEL;

typedef struct board
{
  int           nchannels;
  char          modelname[MAX_NAME];
  char          description[MAX_NAME];
  int           sernum;
  int           relmax;
  int           relmin;
  int           nparams;
  char          parnames[MAX_PARAM][MAX_NAME];
  unsigned long partypes[MAX_PARAM];
  CHANNEL       channel[MAX_CHAN];
  int           setflag; /* if 1, need to write */
} BOARD;

typedef struct sys
{
  int   id;
  char  name[MAX_NAME];
  int   nslots;
  BOARD board[MAX_SLOT];
  int   setflag; /* if 1, need to write */
} HV;



/* functions */

int
sy527Measure2Demand(unsigned int id, unsigned int board);
int
sy527GetBoard(unsigned int id, unsigned int board);
int
sy527SetBoard(unsigned int id, unsigned int board);
int
sy527GetMap(unsigned int id);
int
sy527PrintMap(unsigned int id);
void *
sy527MainframeThread(void *arg);
int
sy527Init();
int
sy527Start(unsigned id, char *ip_address);
int
sy527Stop(unsigned id);

int
sy527SetMainframeOnOff(unsigned int id, unsigned int on_off);

int
sy527GetMainframeStatus(unsigned int id, int *active, int *onoff, int *alarm);
int
sy527SetChannelDemandVoltage(unsigned int id, unsigned int board,
                              unsigned int chan, float u);
float
sy527GetChannelDemandVoltage(unsigned int id, unsigned int board,
                              unsigned int chan);
int
sy527SetChannelMaxVoltage(unsigned int id, unsigned int board,
                           unsigned int chan, float u);
float
sy527GetChannelMaxVoltage(unsigned int id, unsigned int board,
                           unsigned int chan);
int
sy527SetChannelMaxCurrent(unsigned int id, unsigned int board,
                           unsigned int chan, float u);
float
sy527GetChannelMaxCurrent(unsigned int id, unsigned int board,
                           unsigned int chan);
float
sy527GetChannelMeasuredVoltage(unsigned int id, unsigned int board,
                                unsigned int chan);
float
sy527GetChannelMeasuredCurrent(unsigned int id, unsigned int board,
                                unsigned int chan);
int
sy527SetChannelRampUp(unsigned int id, unsigned int board, unsigned int chan,
                       float u);
float
sy527GetChannelRampUp(unsigned int id, unsigned int board, unsigned int chan);
int
sy527SetChannelRampDown(unsigned int id, unsigned int board,
                         unsigned int chan, float u);
float
sy527GetChannelRampDown(unsigned int id, unsigned int board,
                         unsigned int chan);
int
sy527SetChannelOnOff(unsigned int id, unsigned int board,
                      unsigned int chan, unsigned int u);
unsigned int
sy527GetChannelOnOff(unsigned int id, unsigned int board,
                      unsigned int chan);
int
sy527SetChannelEnableDisable(unsigned int id, unsigned int board,
                              unsigned int chan, unsigned int u);
unsigned int
sy527GetChannelEnableDisable(unsigned int id, unsigned int board,
                              unsigned int chan);
unsigned int
sy527GetChannelStatus(unsigned int id, unsigned int board,
                       unsigned int chan);
