/* vxColor.h -  header file for vxColor2.c program */

/* Copyright 1984-1998 Wind River Systems, Inc. */

/*
modification history
--------------------
01f,09apr98,dbt increased stack size for solaris simulator.
01e,15nov95,jcf renamed vxColor2.h to vxColor.h.  sigh.
01d,15nov95,jcf renamed vxColor.h to vxColor2.h.
01c,27sep95,jco re coding convention revision, removed rev field in GNODE.
01b,06sep95,p_m fixed coding convention problems.
01a,03aug95,jco written.
*/

/* WARNING: DEMO_EOLM MUST BE SUPERIOR TO MAX_NODE! If DEMO_EOLM is changed 
 * here, it MUST also be changed in $WIND_BASE/host/src/demo/color/demoHost.tk.
 */

/* defines */

#define	MAX_NODE	100	/* maximum node number in a graph */
#define	MAX_CONNEX	10	/* maximum node connectivity number */
#define	MAX_MSG		1	/* maximum number of message in queue */
#define	MSG_SIZE	sizeof (NODE_ATTRIB) /* message size */
#define	MAX_COLOR	6	/* maximum number of color */
#define	MAX_OUTDEGREE	5	/* maximum node outdegree */
#define	OUTDEGREE_INIT	(MAX_OUTDEGREE + 1)
#define	PAUSE_CLK_RATIO	10 	/* clckRate divisor (N sec for Pause) */
#define	DEMO_EOLM	127	/* connexions file endOfLine marker */
#define	CONT_PRIORITY	200	/* controler's task priority */
#define	NODE_PRIORITY	200	/* node's tasks priority */
#define	NODE_STBL_INIT  -1	/* initial value for stable field */
#define	NODE_UNSTABLE	0	/* unstable value for stable field */
#define	NODE_STABLE	1	/* stable value for stable field */

/*
 * stackSize defines according to cpu type and debug mode boolean 
 */

#if (defined (CPU_FAMILY) && (CPU_FAMILY==MC680X0))
#define	NODE_STACK	500	/* node task stack size */
#define	NODE_STACK_DBG	1000	/* node task stack size for debug */
#define	CONT_STACK	500	/* graphControl task stack size */
#define	CONT_STACK_DBG	1000	/* graphControl stack size for debug */
#endif  /* CPU_FAMILY == MC680X0 */

#if (defined (CPU_FAMILY) && (CPU_FAMILY==SIMSPARCSUNOS))
#define	NODE_STACK	2500	/* node task stack size */
#define	NODE_STACK_DBG	5000	/* node task stack size for debug */
#define	CONT_STACK	2500	/* graphControl task stack size */
#define	CONT_STACK_DBG	5000	/* graphControl stack size for debug */
#endif  /* CPU_FAMILY == SPARC */

#if (defined (CPU_FAMILY) && (CPU_FAMILY==I80X86))
#define	NODE_STACK	500	/* node task stack size */
#define	NODE_STACK_DBG	1000	/* node task stack size for debug */
#define	CONT_STACK	500	/* graphControl task stack size */
#define	CONT_STACK_DBG	1000	/* graphControl stack size for debug */
#endif  /* CPU_FAMILY == I80X86 */

#if (defined (CPU_FAMILY) && (CPU_FAMILY==I960))
#define	NODE_STACK	1000	/* node task stack size */
#define	NODE_STACK_DBG	2000	/* node task stack size for debug */
#define	CONT_STACK	1000	/* graphControl task stack size */
#define	CONT_STACK_DBG	2000	/* graphControl stack size for debug */
#endif  /* CPU_FAMILY == I960 */

#if (defined (CPU_FAMILY) && (CPU_FAMILY==SIMSPARCSOLARIS))
#define	NODE_STACK	5000	/* node task stack size */
#define	NODE_STACK_DBG	10000	/* node task stack size for debug */
#define	CONT_STACK	5000	/* graphControl task stack size */
#define	CONT_STACK_DBG	10000	/* graphControl stack size for debug */
#endif  /* CPU_FAMILY == SIMSPARCSOLARIS */

/* default values */
#ifndef NODE_STACK
#define	NODE_STACK	2500	/* node task stack size */
#define	NODE_STACK_DBG	5000	/* node task stack size for debug */
#define	CONT_STACK	2500	/* graphControl task stack size */
#define	CONT_STACK_DBG	5000	/* graphControl stack size for debug */
#endif /* not def NODE_STACK */


#if (defined (CPU_FAMILY) && (CPU_FAMILY==I960) && (defined __GNUC__))
#pragma	align 1                 /* tell gcc960 not to optimize alignments */
#endif  /* CPU_FAMILY==I960 */

/* typedefs */

typedef struct ids /* IDS */
    {
    INT32	tid;		/* task id */
    INT32	nid;		/* node id */
    } IDS;			/* task/node id	*/

typedef struct node_attrib /* NODE_ATTRIB */
    {
    INT32	color;		/* node color */
    INT32	Xvalue;		/* node Xvalue */
    } NODE_ATTRIB;		/* node attributes */

typedef struct connect_info /* CONNECT_INFO */
    {
    INT32	dir;		/* direction: +=to connected -=from connected */
    IDS		tnid;		/* connected node tid & nid */
    NODE_ATTRIB	att;		/* connected node attributes */
    MSG_Q_ID	wMQId;		/* write (toNeighbor) mesg queue id */
    MSG_Q_ID	rMQId;		/* read  (toNeighbor) mesg queue id */
    } CONNECT_INFO;		/* connected node info */

typedef struct gnode_st /* GNODE */ 
    {
    INT32		stable;			/* color stability flag	*/
    IDS			tnid;			/* node tid & nid */
    NODE_ATTRIB		att;			/* node col & Xval */
    INT32		pc;			/* node Prog Counter */
    INT32		oD;			/* node outDegree */
    INT32		cNum;			/* node connexions # */
    CONNECT_INFO	cArray [MAX_CONNEX];	/* node connexions array */
    struct gnode_st *	pNext;			/* pointer to pNext gnode */
    } GNODE;					/* global node info */

typedef struct control /* CONTROL */
    {
    char        steadyState;            /* flag on if graph is stable */
    char        controlAlive;           /* flag on if control task is alive */
    char	coloringStop;           /* flag on when stop requested */
    char        pauseOn;                /* flag on when pause requested */
    INT32       graphControlId;         /* graphControl task Id */
    INT32       nodeFailure;            /* flag on if a node task in error */
    SEM_ID	nodeJobDoneSem;		/* synchro for controler */
    SEM_ID      createAckSem;           /* Ack for each node creation */
    SEM_ID	restartSem;		/* StartColoring and control synchro */
    SEM_ID	connexionsDoneSem;	/* connexions established synchro */
    SEM_ID	controlerFlushSem;	/* controler flush semaphore */
    SEM_ID      dataBaseMutex;          /* data base mutex semaphore */
    } CONTROL;				/* global control info */

typedef struct data_base /* DATA_BASE */
    {
    CONTROL	cT;			/* control info block */
    INT32	nNum;			/* graph node #	*/
    GNODE *	pFirstGnode;		/* pointer to first gnode */
    } DATA_BASE;			/* global graph info */	

/* turn off alignment requirement for i960 cpu family */

#if (defined (CPU_FAMILY) && (CPU_FAMILY==I960) && (defined __GNUC__))
#pragma align 0
#endif  /* CPU_FAMILY==I960 */

/* forward declarations defining the demo GUI API */

INT32	graphColorUpdate (DATA_BASE * pdB, char * colorMemBlkAdd);
INT32	graphInit (INT32 nodNumber, char * colorMemBlkAdd, BOOL debugMode);
INT32	graphStartColoring (DATA_BASE * pdB);
INT32 	graphStop (DATA_BASE * pdB);
