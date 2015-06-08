/*****************************************************************************
*
* vmexLib.h -  library header file
*
******************************************************************************/

/* 
modification history
--------------------
01m,04feb97,jmb  Added support for timer array and removed unused macros.
01l,22jan97,jmb  Get rid of as_return.
01k,22jan97,jmb  Add define of VMEEXEC
01j,22jan97,jmb  undo previous patch.
01i,16jan97,jmb  add some missing flags (only impact on p2vLib).
01h,16oct96,jmb  add variable length msg queue msgs for VMEEXEC
01g,13jun96,jmb  comment clean-up
01f,16may96,jmb  fix ERR_NOASR, change ERR_SSFN, add VMEEXEC macros
01e,08may96,jmb  changed Q_FIFO to Q_FIF to resolve conflict with
		 vxWorks macro.  Added "extern".
01d,02may96,jmb  added miscellaneous flags and error codes
01c,23apr96,jmb  cleanup
01b,11apr96,jmb  reformatted
01a,20mar96,cym  created
*/

/*
DESCRIPTION

This library contains implementations of various PSOS+ kernel routines,
in vxWorks code.  
*/

#ifndef __INCvmexLibh
#define __INCvmexLibh

#ifdef __cplusplus
extern "C" {
#endif

/* Headers/Libraries used by VMEX */

#include "vxWorks.h"
#include "semLib.h"
#include "wdLib.h"

#define VMEEXEC

/*
*   Structures used by multiple library modules
*/

#ifdef VMEEXEC

struct t_date {
    short year;
    char month;
    char day;
};

struct t_time {
    short hour;
    char minute;
    char second;
};

struct time_ds {
    struct t_date date;
    struct t_time time;
    unsigned int ticks;
};

#endif		/* VMEEXEC */

/* Node in local object table */

typedef struct 
    {
    UINT32 name;  		/* 4 character name */
    void *pObject;			/* ptr to object id */
    void *pAux;				/* ptr to auxiliary object info */
    } VMEX_TABLE_NODE ;	

/* Local object table descriptor */

typedef struct 
    {
    char * name;		/* name of table */
    int size;			/* maximum number of nodes in table */
    int count;			/* number of nodes used in table */
    int log2Count;		/* log 2 of count */
    SEM_ID mutexSem;		/* semaphore guarding table modifications */
    BOOL initialized;		/* initialized flag */
    VMEX_TABLE_NODE *nodes;      /* ptr to array of object nodes */
    } VMEX_TABLE;	

/* Timer descriptor */

typedef struct tmNode
    {
    int event;				/* event to send */
    int tid;				/* target task */
    int delay;				/* timer reset in ticks */
    WDOG_ID timer;			/* watchdog timer */
    struct tmNode *next;		/* next timer node */
    struct tmNode *previous;		/* previous timer node */
    } VMEX_TIMER_NODE;                 

/* TCB extension */

typedef struct
    {
    SEM_ID eventWait;		/* semaphore taken by event receiver */
    UINT32 events;		/* event vector */
    UINT32 notepads[16];	/* notepad registers */
    UINT32 ASEvents;		/* ASR event vector */
    UINT32 ASMode;		/* ASR mode */
    FUNCPTR pASR;		/* ASR routine */
#ifdef VMEX_USE_TIMER_ARRAY
    UINT32 timerCount; 		/* Count of armed timers */
#else
    VMEX_TIMER_NODE *pTimerList;	/* List of armed timers */
#endif
    UINT32 initialPriority;	/* initial task priority */
    UINT32 initialMode;		/* initial task mode */
    UINT32 currentMode;		/* current task mode */
    BOOL asrBusy;		/* ASR currently executing flag */
    } VMEX_SHADOW_TCB;			

/* Marker used to indicate that a vxWorks TCB has a vmex extension */

#define VMEX_COOKIE 0x70534f53

/*  API error codes */

#define EOK 		0x00
#define ERR_TIMEOUT 	0x01
#define ERR_SSFN 	0x03 
#define ERR_NODENO 	0x04
#define ERR_OBJDEL 	0x05
#define ERR_OBJID 	0x06
#define ERR_OBJTFULL 	0x08
#define ERR_OBJNF 	0x09
#define ERR_NOSTK 	0x0f
#define ERR_TINYSTK 	0x10
#define ERR_PRIOR 	0x11
#define ERR_ACTIVE 	0x12
#define ERR_SUSP 	0x14
#define ERR_NOTSUSP 	0x15
#define ERR_SETPRI 	0x16
#define ERR_REGNUM 	0x17
#define ERR_RNADDR	0x1B
#define ERR_TINYRN	0x1E
#define ERR_SEGINUSE	0x1F
#define ERR_ZERO	0x20
#define ERR_TOOBIG	0x21
#define ERR_NOSEG	0x22
#define ERR_PTADDR 	0x28
#define ERR_BUFSIZE 	0x29
#define ERR_TINYPT 	0x2A
#define ERR_BUFINUSE 	0x2B
#define ERR_NOBUF 	0x2C
#define ERR_BUFADDR 	0x2D
#define ERR_BUFFREE 	0x2F
#define ERR_MSGSIZ 	0x31
#define ERR_BUFSIZ 	0x32
#define ERR_NOMGB 	0x34
#define ERR_QFULL 	0x35
#define ERR_QKILLD 	0x36
#define ERR_NOMSG 	0x37
#define ERR_TATQDEL 	0x38
#define ERR_MATQDEL 	0x39
#define ERR_VARQ 	0x3a
#define ERR_NOTVARQ 	0x3b
#define ERR_NOEVS 	0x3C 
#define ERR_NOASR 	0x3F
#define ERR_NOSEM 	0x42
#define ERR_SKILLD 	0x43
#define ERR_TATSDEL 	0x44
#define ERR_NOTIME 	0x47
#define ERR_ILLDATE 	0x48
#define ERR_ILLTIME 	0x49
#define ERR_ILLTICKS 	0x4A
#define ERR_NOTIMERS 	0x4B
#define ERR_BADTMID 	0x4C
#define ERR_TOOLATE 	0x4E
/* #define ERR_SSFN 	0xff00 */

#ifdef VMEEXEC

/* Motorola version of macros */

#define E_NOERR 	EOK 
#define E_TIMOUT	ERR_TIMEOUT
#define E_SSFN		ERR_SSFN
#define E_NODENO	ERR_NODENO 
#define E_OBJDEL	ERR_OBJDEL
#define E_OBJID		ERR_OBJID
#define E_OBJFUL	ERR_OBJTFULL
#define E_OBJNF		ERR_OBJNF
#define E_NOSTK		ERR_NOSTK
#define E_SMLSTK 	ERR_TINYSTK 	
#define E_PRIOR		ERR_PRIOR
#define E_ACTIVE 	ERR_ACTIVE 
#define E_SUSP		ERR_SUSP 
#define E_NOSUSP	ERR_NOTSUSP 
#define E_SETPRI	ERR_SETPRI
#define E_REGNUM	ERR_REGNUM 
#define E_RNADDR	ERR_RNADDR
#define E_TINYRN	ERR_TINYRN
#define E_SEGUSE	ERR_SEGINUSE
#define E_ZERO		ERR_ZERO
#define E_TOOBIG	ERR_TOOBIG
#define E_NOSEG		ERR_NOSEG
#define E_PTADDR	ERR_PTADDR 
#define E_BSIZE		ERR_BUFSIZE 
#define E_TINYPT	ERR_TINYPT 
#define E_BUFUSE	ERR_BUFINUSE 
#define E_NOBUF		ERR_NOBUF 
#define E_BUFADR	ERR_BUFADDR 
#define E_BUFFRE	ERR_BUFFREE 
#define E_MSGSIZ	ERR_MSGSIZ 
#define E_BUFSIZ	ERR_BUFSIZ 
#define E_NOMGB		ERR_NOMGB 
#define E_QFULL		ERR_QFULL 
#define E_QKILLD	ERR_QKILLD 	
#define E_NOMSG		ERR_NOMSG 
#define E_TAQDEL	ERR_TATQDEL 
#define E_MAQDEL	ERR_MATQDEL 
#define E_VARQ		ERR_VARQ 
#define E_NTVARQ	ERR_NOTVARQ 
#define E_NOASR		ERR_NOASR
#define E_NOEVS		ERR_NOEVS
#define E_NOSEM		ERR_NOSEM
#define E_SKILLD	ERR_SKILLD
#define E_TASDEL	ERR_TATSDEL 
#define E_NOTIME	ERR_NOTIME 
#define E_ILDATE 	ERR_ILLDATE
#define E_ILTIME	ERR_ILLTIME
#define E_ILTICK	ERR_ILLTICKS
#define E_NOTIMR	ERR_NOTIMERS 
#define E_BDTMID	ERR_BADTMID
#define E_TMLATE	ERR_TOOLATE 

#endif    /* VMEEXEC */

/* message queue bit flags */

#define Q_NOWAIT 	0x01
#define Q_WAIT 		0x00

#define Q_GLOBAL	0x01   /* global queues unsupported */
#define Q_LOCAL 	0x00   /* queues local by default */
#define Q_PRIOR 	0x02
#define Q_FIF		0x00   /* FIFO queue by default */
#define Q_LIMIT		0x04   /* Q_LIMIT must be set */
#define Q_NOLIMIT	0x00   /* unlimited not supported! */
#define Q_PRIBUF	0x08   /* buffer allocation flags ignored */
#define Q_SYSBUF	0x00

/* semaphore bit flags */

#define SM_NOWAIT 	0x01
#define SM_WAIT 	0x00

#define SM_GLOBAL 	0x01
#define SM_LOCAL 	0x00   /* semaphores are local by default */
#define SM_PRIOR 	0x02
#define SM_FIFO 	0x00   /* tasks queue FIFO by default */

/* event bit flags  */

#define EV_NOWAIT 	0x01
#define EV_ANY 		0x02

/* Task mode bits */

#define T_NOPREEMPT 0x1   /* mode bit 0 */
#define T_PREEMPT 0x0     /*  default is pre-emption on */
#define T_TSLICE 0x2      /* mode bit 1 */
#define T_NOTSLICE 0x0    /*  default is round robin off */
#define T_NOASR 0x4       /* mode bit 2 */
#define T_ASR 0x0         /*  default is ASR's enabled */
#define T_SUPV 0x2000     /* mode bit D */
#define T_USER 0x0        /*  default is user mode */

/* Task option bits */

#define T_GLOBAL 0x1      /* option bit 0 -- global tasks unsupported */
#define T_LOCAL 0x0       /*  default is local task */
#define T_FPU 0x2        /* option bit 1 */
#define T_NOFPU 0x0       /*  default is no floating point unit */

/* Fixed block memory manager options */

#define PT_DEL 		0x04
#define PT_NODEL 	0x00
#define PT_PRIOR 	0x02
#define PT_FIFO 	0x00

/* Variable segment memory manager options */

#define RN_DEL 		0x04
#define RN_NODEL 	0x00
#define RN_PRIOR 	0x02
#define RN_FIFO 	0x00
#define RN_WAIT 	0x00
#define RN_NOWAIT 	0x01

#ifdef VMEEXEC

/* Motorola macros for bit fields */

#define LIMIT		Q_LIMIT
#define PRIVATE		Q_PRIBUF
#define	ANY		EV_ANY
#define	NOWAIT		SM_NOWAIT
#define TNOPREEMPT	T_NOPREEMPT
#define TSLICE		T_TSLICE
#define	NOASR		T_NOASR
#define	DISASR		T_NOASR
#define SUPV		T_SUPV
#define EXPORT  	T_GLOBAL
#define FPC 		T_FPU
#define PRIOR		RN_PRIOR
#define OVERRIDE	RN_DEL
/* Semaphores and regions have same nowait flag */
/* #define NOWAIT		RN_NOWAIT  */
#define	U_REG0		0
#define	U_REG1		1
#define	U_REG2		2
#define	U_REG3		3
#define	U_REG4		4
#define	U_REG5		5
#define	U_REG6		6
#define	U_REG7		7
#define	S_REG0		8
#define	S_REG1		9
#define	S_REG2		10
#define	S_REG3		11
#define	S_REG4		12
#define	S_REG5		13
#define	S_REG6		14
#define	S_REG7		15

#endif   /* VMEEXEC */

/* Macro for name argument */

#ifdef VMEEXEC
#define NAME_T(arg) UINT32 arg
#define NAME_TO_INT(arg) arg
#else
#define NAME_T(arg) char arg[4]
#define NAME_TO_INT(arg) * (int *) arg
#endif

/*******************************************************************
*
*  Kernel Service Routines
*/

/* ASR routines */

extern UINT32 as_catch (FUNCPTR ASR, UINT32 mode);
extern UINT32 as_send (UINT32 tid, UINT32 signal);

/* Events routines */

extern UINT32 ev_receive (UINT32 eventin, UINT32 flags, UINT32 timeout, 
                   UINT32 *eventout);
extern UINT32 ev_send (UINT32 tid, UINT32 event);

/* Fixed block memory management routines */

extern UINT32 pt_create (NAME_T(name), void *paddr, void *laddr, UINT32 length, 
		  UINT32 bsize, UINT32 flags, UINT32 *ptid, UINT32 *nbuf);
extern UINT32 pt_delete (UINT32 ptid);
extern UINT32 pt_getbuf (UINT32 ptid, void ** bufaddr);
extern UINT32 pt_ident (NAME_T(name), UINT32 node, UINT32 *ptid);
extern UINT32 pt_retbuf (UINT32 ptid, void *bufaddr);
extern UINT32 pt_sgetbuf (UINT32 ptid, void **paddr, void ** laddr);

/* Fixed length message queue routines */

extern UINT32 q_broadcast (UINT32 qid, UINT32 msg_buf[4], UINT32 *count);
extern UINT32 q_create (NAME_T(name), UINT32 count, UINT32 flags, UINT32 *qid);
extern UINT32 q_delete (UINT32 qid) ;
extern UINT32 q_ident (NAME_T(name), UINT32 node, UINT32 *qid);
extern UINT32 q_receive (UINT32 qid, UINT32 flags, UINT32 timeout, UINT32 msg_buf[4]);
extern UINT32 q_send (UINT32 qid, UINT32 msg_buf[4]);
extern UINT32 q_urgent (UINT32 qid, UINT32 msg_buf[4]);

/* Variable length message queue routines */

extern UINT32 q_vbroadcast (UINT32 qid, void * msg_buf, UINT32 msg_len, UINT32 *count);
extern UINT32 q_vcreate (NAME_T(name), UINT32 flags, UINT32 count, UINT32 maxlength, 
		  UINT32 *qid);
extern UINT32 q_vdelete (UINT32 qid);
extern UINT32 q_vident (NAME_T(name), UINT32 node, UINT32 *qid);
extern UINT32 q_vreceive (UINT32 qid, UINT32 flags, UINT32 timeout, void * msg_buf, 
		   UINT32 buf_len, UINT32 *msg_len);
extern UINT32 q_vsend (UINT32 qid, void * msgbuf, UINT32 msg_len);
extern UINT32 q_vurgent (UINT32 qid, void * msbguf, UINT32 msg_len);

/* Variable segment memory management routines */

extern UINT32 rn_create (NAME_T(name), void *saddr, UINT32 length, UINT32 unit_size, 
		  UINT32 flags, UINT32 *rnid, UINT32 *asiz);
extern UINT32 rn_delete (UINT32 rnid);
extern UINT32 rn_getseg (UINT32 rnid, UINT32 size, UINT32 flags, UINT32 timeout,
		  void ** seg_addr);
extern UINT32 rn_ident (NAME_T(name), UINT32 *rnid);
extern UINT32 rn_retseg (UINT32 rnid, void *seg_addr);

/* Semaphore routines */

extern UINT32 sm_create (NAME_T(name), UINT32 count, UINT32 flags, UINT32 *smid);
extern UINT32 sm_delete (UINT32 smid);
extern UINT32 sm_ident (NAME_T(name), UINT32 node, UINT32 *smid);
extern UINT32 sm_p (UINT32 smid, UINT32 flags, UINT32 timeout);
extern UINT32 sm_v ( UINT32 smid);

/* Task management routines */

extern UINT32 t_create (NAME_T(name), UINT32 priority, UINT32 superstk, UINT32 userstk,
		 UINT32 flags, UINT32 *tid);
extern UINT32 t_delete (UINT32 tid);
extern UINT32 t_getreg (UINT32 tid, UINT32 regnum, UINT32 *regvalue);
extern UINT32 t_ident (NAME_T(name), UINT32 node, UINT32 *tid);
extern UINT32 t_mode (UINT32 mask, UINT32 newmode, UINT32 *oldmode);
extern UINT32 t_start (UINT32 tid, UINT32 mode, void (*entrypt)(), UINT32 args[4]);
extern UINT32 t_restart (UINT32 tid, UINT32 args[4]);
extern UINT32 t_setreg (UINT32 tid, UINT32 regnum, UINT32 regvalue);
extern UINT32 t_setpri (UINT32 tid, UINT32 newprio, UINT32 *oldprio);
extern UINT32 t_suspend (UINT32 tid);
extern UINT32 t_resume (UINT32 tid);

/* Timer routines */

extern UINT32 tm_cancel (UINT32 );
extern UINT32 tm_evafter (UINT32, UINT32, UINT32 *);
extern UINT32 tm_evevery (UINT32, UINT32, UINT32 *);
#ifdef VMEEXEC
extern UINT32 tm_evwhen (struct time_ds * timebuf, UINT32 event, UINT32 *tmid );
#else
extern UINT32 tm_evwhen ( UINT32 date, UINT32 time, UINT32 ticks, UINT32 event, 
		   UINT32 *tmid );
#endif
#ifdef VMEEXEC
extern UINT32 tm_get (struct time_ds * pTimebuf);
#else
extern UINT32 tm_get (UINT32 * date, UINT32 * time, UINT32 * ticks);
#endif
#ifdef VMEEXEC
extern UINT32 tm_set (struct time_ds * pTimebuf);
#else
extern UINT32 tm_set (UINT32 date, UINT32 time, UINT32 ticks);
#endif
extern UINT32 tm_tick();
extern UINT32 tm_wkafter (UINT32);
#ifdef VMEEXEC
extern UINT32 tm_wkwhen (struct time_ds * timebuf);
#else
extern UINT32 tm_wkwhen (UINT32 date, UINT32 time, UINT32 ticks);
#endif

/******************************************************************
*
*
* Utilities  
*/

/* These can be called from the command line to turn the shell 
 * into a vmex task or back to a normal vxWorks task
 */

extern STATUS vmexTCBInit(int tid);
extern void vmexTCBRemove(int tid);

/* Shortcut for spawning a vmex task from the shell using reasonable defaults.
 * Don't need to go though t_create/t_start.
 */

extern int vmexTaskSpawn (FUNCPTR entrypt, int arg1, int arg2, int arg3, 
			 int arg4);

/* Library utility function */

extern VMEX_SHADOW_TCB * vmexTcb( UINT32);

/* Library initialization routines...
 * deleted...initialization is automatic
 *
 * extern void vmexSemInit(); 
 * extern void vmexQInit();
 */

/* Local object table manipulation routines */

extern void *vmexNameToObj (VMEX_TABLE *pTable, UINT32 name);
extern void *vmexObjToShadow (VMEX_TABLE *pTable, void *pObject);
extern void *vmexObjToShadowInt (VMEX_TABLE *pTable, void *pObject);
extern STATUS vmexTblAdd (VMEX_TABLE *pTable, UINT32 name, void *pObject,
    void *shadow);
extern STATUS vmexTblRemove (VMEX_TABLE *pTable, void *pObject);
extern VMEX_TABLE *vmexTblCreate (char * name, VMEX_TABLE_NODE * pNode, int size); 

/* This function is called when the library can't decide what to do
 * about an unsupported argument or option of a service function.
 */

extern void vmexUnsupported (char * fmt, int arg1, int arg2, int arg3, int arg4,
    int arg5, int arg6);

extern STATUS vmexMessage (unsigned int msgcode);


#ifdef __cplusplus
}
#endif

#endif /* __INCvmexLibh */
