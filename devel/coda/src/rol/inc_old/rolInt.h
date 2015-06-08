/* Readout list parameters */

typedef struct rolParameters *rolParam;     
typedef struct rolParameters
{
  char          *name;	         /* name of parent process */
  char          tclName[20];	     /* Tcl name of this list */ 
  char          *listName;	     /* name of this list */
  int            runType;	         /* run type */
  int            runNumber;	     /* run number */
  VOIDFUNCPTR    rol_code;	     /* init procedure */
  int            daproc;	         /* list of function pointers */
  void          *id;		         /* ID of storage used during load */
  int            nounload;	     /* if !=0 module can't be unloaded */
  int            inited;	         /* we have been initialised */
  long          *dabufp;	         /* output  write pointer */
  long          *dabufpi;	         /* input   read  pointer */
  ROL_MEM_PART  *pool;             /* our free memory pool */
  ROL_MEM_PART  *output;	         /* our output */
  ROL_MEM_PART  *input;            /* our input */
  ROL_MEM_PART  *dispatch;         /* used by dispatcher to delay triggers */
  /*Sergey: volatile ???*/ ROL_MEM_PART  *dispQ;   /* dispatcher queue */
  unsigned long  recNb;	         /* count of output buffers processed */
  unsigned long *nevents;          /* number of events taken */
  int           *async_roc;        /* if ne 0 then communication with EB inhibited */
  char          /*Sergey: *usrString*/usrString[32]; /* string passed in download */
  void          *private_storage;  /* private storage */
  int            pid;              /* ROC ID */
  int            poll;             /* to poll or not to poll */
  ROL_MEM_PART  *pool1;		     /* second free memory pool */
  int            doDone;			 /* should we call done in primaryROl ? */
} ROLPARAMS;

/* READOUT LIST PROCEDURES  */

#define DA_INIT_PROC        0
#define DA_DOWNLOAD_PROC    1
#define DA_PRESTART_PROC    2
#define DA_END_PROC         3
#define DA_PAUSE_PROC       4
#define DA_GO_PROC          5
#define DA_POLL_PROC        6
#define DA_DONE_PROC        7
#define DA_REPORT_PROC      8
#define DA_FREE_PROC        9











