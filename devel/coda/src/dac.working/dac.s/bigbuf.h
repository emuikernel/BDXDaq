
#ifndef _BIGBUF_
#define _BIGBUF_

/* bigbuf.h */

#define NBIGBUFFERS 16

#define SEND_BUF_MARGIN  (MAX_EVENT_LENGTH + 128)

typedef struct bigbuf
{
  /* buffers */
  int nbufs;                        /* the number of buffers */
  int nbytes;                       /* the size of buffers in bytes */
  int write;                        /* write index, changed by 'put' only */
  int read;                         /* read index, changed by 'get' only */
  unsigned int *data[NBIGBUFFERS];  /* circular buffer of pointers */

  /* locks and conditions */
#ifdef VXWORKS
  SEM_ID read_lock;
  SEM_ID write_lock;
#else
  pthread_mutex_t read_lock;   /* lock the structure */
  pthread_cond_t read_cond;    /* full <-> not full condition */

  pthread_mutex_t write_lock;  /* lock the structure */
  pthread_cond_t write_cond;   /* empty <-> notempty condition */
#endif

  int cleanup;

} BIGBUF;




/* control structure for 'coda_net' and 'coda_proc';
 used to pass info between 'coda_roc'/'coda_proc' and 'coda_net' */
typedef struct bignet
{
  int failure; /* failure flag */
  int doclose; /* thread exit request flag */

  /* for coda_net only */
  int port;
  int socket;

  BIGBUF *gbigBuffer; /* input data buffer */
  BIGBUF *gbigBuffer1; /* output data buffer (NULL for 'coda_net') */

  /* general info */
  int token_interval;
  int rocid;
  char host[128];
  char rolname[256];
  char rolparams[128];
  int proc_on_pmc;
  int net_on_pmc;

  char roc_name[128]; /* CODA name for ROC (for example dc1) */
  char eb_name[128]; /* CODA name for EB (for example EB1) */

} BIGNET;



/* PMC stuff */
#ifdef VXWORKS

/* some following definitions borrowed from dcpu.h; although we can use
our own defs, we'll use same as dcpu.h; in future we may create generic
header file for inter-processor communication and use it everywhere */

/* Commands */
#define DC_CMD_MASK    0xffff
#define DC_DOWNLOAD      0x01
#define DC_PRESTART      0x02
#define DC_GO            0x04
#define DC_END           0x08
#define DC_RESET         0x10

/* States */
#define DC_STATE_MASK  0xffff0000
#define DC_DOWNLOADED     0x10000
#define DC_PRESTARTED     0x20000
#define DC_ACTIVE         0x40000
#define DC_ENDING         0xc0000
#define DC_ENDED          0x80000
#define DC_ERROR       0xff000000
#define DC_IDLE             0

/* keep data pointers aligned to 16-byte */
typedef struct pmc *PMCPtr;
typedef struct pmc
{
  volatile unsigned int  csrr;        /* command/state  register */
  volatile unsigned int  cmd;
  volatile unsigned int  dataReady;
  volatile BIGNET       *bignetptr;

  volatile unsigned int *bufin;
  volatile unsigned int  lenin;

  volatile unsigned int *bufout;
  volatile unsigned int  lenout;

  volatile unsigned int  heartbeat[4];

} PMC;

#endif





/* function prototypes */

BIGBUF       *bb_new(int nbufs, int nbytes);
void          bb_delete(BIGBUF **bbp);
void          bb_cleanup(BIGBUF **bbp);
unsigned int *bb_write(BIGBUF **bbp);
unsigned int *bb_write_current(BIGBUF **bbp);
unsigned int *bb_read(BIGBUF **bbp);

#endif
