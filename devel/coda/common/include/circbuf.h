
#ifdef Linux
#define HPS_TEST
#endif

/*trying to use big buffers directly*/
#define NEW_READOUT


#include "ttbosio.h"

/* circbuf.h */

/* uncomment following line if do NOT want to send data from ROC to EB */
/*#define ROC_DOES_NOT_SEND 1*/

/* uncomment following line if do NOT want to send data to Building Thread */
/*#define DO_NOT_PUT 1*/

/* uncomment following line if do not want to malloc for every buffer */
#define NOALLOC 1

/* uncomment following line if want fixed (=SEND_BUF_SIZE)
   buffer size over network */
/*#define FIXEDBUFS 1*/

/* uncommect following line if want to use one PMC coprocessor */
#ifndef Linux
#define PMCOFFSET 0xc0000000 /* as seen from PMC board */
#endif


/****************************************************************************/
/* allocate smaller buffers until bigger memory will be avail */

#ifdef Linux

#define MAX_EVENT_LENGTH (NWBOS*4)
#define MAX_EVENT_POOL   400

#define SEND_BUF_SIZE  (3/*10*/ * 1024 * 1024) /*sergey: use 3 for CLAs !!!!!*/
#define TOTAL_RECEIVE_BUF_SIZE  SEND_BUF_SIZE

#else

/* ROL1 buffers */
#define MAX_EVENT_LENGTH (NWBOS*4) /* event buffer length in bytes */
#define MAX_EVENT_POOL   /*400*/200       /* the number of event buffers */

#define SEND_BUF_SIZE  (3 * 1024 * 1024)
#define TOTAL_RECEIVE_BUF_SIZE  SEND_BUF_SIZE

#endif

/* decrease it if have small memory */
#ifdef SunOS_sun4u
#define MAX_ROCS 19 /* must accomodate biggest roc id, not the number of rocs !!! */
/*#define MAX_ROCS 32*/
#else
#ifdef HPS_TEST
#define MAX_ROCS 16
#else
#define MAX_ROCS 32
#endif
#endif


#define NTHREADMAX 7
#define NFIFOMAX   4000 /* the number of events in ET system */
#define NIDMAX     7 /*(NFIFOMAX+NTHREADMAX)*/
#define NCHUNKMAX  400

#ifdef HPS_TEST
#define QSIZE 32
#else
#define QSIZE 16/*8*/ /*sergey: use 16 for CLAS !!! */
#endif

/* big buffer defines */
#define BBIWORDS   0  /* index of buffer length in words */
#define BBIBUFNUM  1  /* index of buffer number */
#define BBIROCID   2  /* index of ROC id */
#define BBIEVENTS  3  /* index of the number of events in buffer */
#define BBIFD      4  /* index of socket descriptor, also used to store 'magic'  */
#define BBIEND     5  /* index of 'end' condition */
#define BBHEAD     6  /* the number of words in big buffer header */
#define BBHEAD_BYTES (BBHEAD*4) /* the number of bytes in big buffer header */


/* minimum number of network buffers required to proceed; if it fells below
warning message will be printed; we may consider pospond execution until
system will recycle buffers (it takes 1 minute) */
#define MINNETBUFS 100


#ifndef VXWORKS

typedef struct circbuf
{
  /* general info */
  int roc;                     /* the ordered number of the ROC (from 0) */
  int rocid;                   /* ROC id (from CODA database) */
  char name[100];              /* name given by user (just info) */
  char parent[100];            /* parent name (just info) */

  /* buffers */
  int write;  /* changed by 'put' only */
  int read;   /* changed by 'get' only */
  void *data[QSIZE];           /* circular buffer of pointers */

  /* locks and conditions */
  pthread_mutex_t read_lock;   /* lock the structure */
  pthread_cond_t read_cond;    /* full <-> not full condition */

  pthread_mutex_t write_lock;  /* lock the structure */
  pthread_cond_t write_cond;   /* empty <-> notempty condition */

  int deleting;                /* are we in a delete somewhere? */

  /* events info */
  int nevents[QSIZE];          /* the number of events left in the buffer */
  unsigned int *evptr1[QSIZE]; /* pointer to first valid event in the buffer */

  /* attachments info */
  int nattach[QSIZE];          /* the number of attachments to the buffer */
  int nbuf[NTHREADMAX];        /* the number of buffers kept by thread */
  int buf1[NTHREADMAX];        /* first buffer kept by thread */

} CIRCBUF;


/* function prototypes for circbuf.c */

#ifdef  __cplusplus
extern "C" {
#endif

CIRCBUF *new_cb(int roc, char *name, char *parent);
void     cb_init(int roc);
void     delete_cb(CIRCBUF **cbp);
int      put_cb_data(CIRCBUF **cbp, void *data);
int      get_cb_data(CIRCBUF **cbp, int id, int chunk,
                     unsigned int *evptr[NCHUNKMAX], int *buflen, int *rocid);
char    *get_cb_name(CIRCBUF *cbp);
int     get_cb_count(CIRCBUF **cbp);

int cb_events_init(CIRCBUF *cba[32]);
int cb_events_get(CIRCBUF *cba[32], int id, int nrocs, int chunk,
                 unsigned int *buf[32][NCHUNKMAX], int evsize[32], int *nphys);

#ifdef  __cplusplus
}
#endif




/* for LINK_support.c and deb_component.c only - temporary here !!! */

typedef struct data_link *DATA_LINK;
typedef struct data_link
{
  char *name;
  char *linkname;   /* for example 'croctest1->EB5' */
  char *parent;
  pthread_t thread;
  int sock;         /* listening socket (bind) */
  int fd;           /* accepted socket (returned by accept()) */
  char host[100];
  int port;
  int thread_exit;
  int bufCnt;
  CIRCBUF *roc_queue;
} DATA_LINK_S;


#endif












