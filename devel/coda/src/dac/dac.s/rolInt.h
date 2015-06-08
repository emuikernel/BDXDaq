/* Readout list parameters */
/* default values all zero, cleaned up in roc_component.c in Download */

typedef struct rolParameters *rolParam;     
typedef struct rolParameters
{
  char          *name;	           /* name of parent process */
  char           tclName[20];	   /* Tcl name of this list */ 
  char          *listName;	       /* name of this list */
  int            runType;	       /* run type */
  int            runNumber;	       /* run number */
  VOIDFUNCPTR    rol_code;	       /* init procedure */
  int            daproc;	       /* list of function pointers */
  void          *id;		       /* ID of storage used during load */
  int            nounload;	       /* if !=0 module can't be unloaded */
  int            inited;	       /* we have been initialised */
  int           *dabufp;	       /* output  write pointer */
  int           *dabufpi;	       /* input   read  pointer */

  unsigned int   recNb;	           /* count of output buffers processed */
  unsigned int  *nevents;          /* number of events taken */
  int            bla_rol_sync_flag;    /* communication flag between ROL1 and rols_loop */
  char           usrString[32];     /* string passed in download */
  void          *private_storage;  /* private storage */
  int            user_storage[2];  /* user storage */
  int            pid;              /* ROC ID */
  int            poll;             /* to poll or not to poll */

  int            doDone;		   /* should we call 'done' in ROL1 ? */
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











