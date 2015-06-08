
#ifndef INCsscanPAUSH
#define INCsscanPAUSH
typedef enum {
	sscanPAUS_Go,
	sscanPAUS_Pause
}sscanPAUS;
#endif /*INCsscanPAUSH*/

#ifndef INCsscanPASMH
#define INCsscanPASMH
typedef enum {
	sscanPASM_Stay,
	sscanPASM_Start_Pos,
	sscanPASM_Prior_Pos,
	sscanPASM_Peak_Pos,
	sscanPASM_Valley_Pos,
	sscanPASM_RisingEdge_Pos,
	sscanPASM_FallingEdge_Pos,
	sscanPASM_COM
}sscanPASM;
#endif /*INCsscanPASMH*/

#ifndef INCsscanP1SMH
#define INCsscanP1SMH
typedef enum {
	sscanP1SM_Linear,
	sscanP1SM_Table,
	sscanP1SM_On_The_Fly
}sscanP1SM;
#endif /*INCsscanP1SMH*/

#ifndef INCsscanP1NVH
#define INCsscanP1NVH
typedef enum {
	sscanP1NV_PV_OK,
	sscanP1NV_No_PV,
	sscanP1NV_PV_NoRead,
	sscanP1NV_PV_xxx,
	sscanP1NV_PV_NoWrite,
	sscanP1NV_PV_yyy,
	sscanP1NV_PV_NC
}sscanP1NV;
#endif /*INCsscanP1NVH*/

#ifndef INCsscanP1ARH
#define INCsscanP1ARH
typedef enum {
	sscanP1AR_Absolute,
	sscanP1AR_Relative
}sscanP1AR;
#endif /*INCsscanP1ARH*/

#ifndef INCsscanNOYESH
#define INCsscanNOYESH
typedef enum {
	sscanNOYES_NO,
	sscanNOYES_YES
}sscanNOYES;
#endif /*INCsscanNOYESH*/

#ifndef INCsscanLINKWAITH
#define INCsscanLINKWAITH
typedef enum {
	sscanLINKWAIT_YES,
	sscanLINKWAIT_NO
}sscanLINKWAIT;
#endif /*INCsscanLINKWAITH*/

#ifndef INCsscanFPTSH
#define INCsscanFPTSH
typedef enum {
	sscanFPTS_No,
	sscanFPTS_Freeze
}sscanFPTS;
#endif /*INCsscanFPTSH*/

#ifndef INCsscanFFOH
#define INCsscanFFOH
typedef enum {
	sscanFFO_Use_F_Flags,
	sscanFFO_Override
}sscanFFO;
#endif /*INCsscanFFOH*/

#ifndef INCsscanFAZEH
#define INCsscanFAZEH
typedef enum {
	sscanFAZE_IDLE,
	sscanFAZE_INIT_SCAN,
	sscanFAZE_BEFORE_SCAN,
	sscanFAZE_BEFORE_SCAN_WAIT,
	sscanFAZE_MOVE_MOTORS,
	sscanFAZE_CHECK_MOTORS,
	sscanFAZE_TRIG_DETCTRS,
	sscanFAZE_READ_DETCTRS,
	sscanFAZE_START_FLY,
	sscanFAZE_RETRACE_MOVE,
	sscanFAZE_RETRACE_WAIT,
	sscanFAZE_AFTER_SCAN_DO,
	sscanFAZE_AFTER_SCAN_WAIT,
	sscanFAZE_SCAN_DONE,
	sscanFAZE_SAVE_DATA_WAIT,
	sscanFAZE_SCAN_PENDING,
	sscanFAZE_TRIG_ARRAY_READ,
	sscanFAZE_ARRAY_READ_WAIT,
	sscanFAZE_PREVIEW,
	sscanFAZE_RECORD_SCALAR_DATA
}sscanFAZE;
#endif /*INCsscanFAZEH*/

#ifndef INCsscanDSTATEH
#define INCsscanDSTATEH
typedef enum {
	sscanDSTATE_UNPACKED,
	sscanDSTATE_TRIG_ARRAY_READ,
	sscanDSTATE_ARRAY_READ_WAIT,
	sscanDSTATE_ARRAY_GET_CALLBACK_WAIT,
	sscanDSTATE_RECORD_ARRAY_DATA,
	sscanDSTATE_SAVE_DATA_WAIT,
	sscanDSTATE_PACKED,
	sscanDSTATE_POSTED
}sscanDSTATE;
#endif /*INCsscanDSTATEH*/

#ifndef INCsscanACQTH
#define INCsscanACQTH
typedef enum {
	sscanACQT_SCALAR,
	sscanACQT_1D_ARRAY
}sscanACQT;
#endif /*INCsscanACQTH*/

#ifndef INCsscanACQMH
#define INCsscanACQMH
typedef enum {
	sscanACQM_NORMAL,
	sscanACQM_ACC,
	sscanACQM_ADD
}sscanACQM;
#endif /*INCsscanACQMH*/
#ifndef INCsscanH
#define INCsscanH
#include "epicsTypes.h"
#include "link.h"
#include "epicsMutex.h"
#include "ellLib.h"
#include "epicsTime.h"
typedef struct sscanRecord {
	char		name[61];	/* Record Name */
	char		desc[41];	/* Descriptor */
	char		asg[29];	/* Access Security Group */
	epicsEnum16	scan;	/* Scan Mechanism */
	epicsEnum16	pini;	/* Process at iocInit */
	epicsInt16	phas;	/* Scan Phase */
	epicsInt16	evnt;	/* Event Number */
	epicsInt16	tse;	/* Time Stamp Event */
	DBLINK		tsel;	/* Time Stamp Link */
	epicsEnum16	dtyp;	/* Device Type */
	epicsInt16	disv;	/* Disable Value */
	epicsInt16	disa;	/* Disable */
	DBLINK		sdis;	/* Scanning Disable */
	epicsMutexId	mlok;	/* Monitor lock */
	ELLLIST		mlis;	/* Monitor List */
	epicsUInt8	disp;	/* Disable putField */
	epicsUInt8	proc;	/* Force Processing */
	epicsEnum16	stat;	/* Alarm Status */
	epicsEnum16	sevr;	/* Alarm Severity */
	epicsEnum16	nsta;	/* New Alarm Status */
	epicsEnum16	nsev;	/* New Alarm Severity */
	epicsEnum16	acks;	/* Alarm Ack Severity */
	epicsEnum16	ackt;	/* Alarm Ack Transient */
	epicsEnum16	diss;	/* Disable Alarm Sevrty */
	epicsUInt8	lcnt;	/* Lock Count */
	epicsUInt8	pact;	/* Record active */
	epicsUInt8	putf;	/* dbPutField process */
	epicsUInt8	rpro;	/* Reprocess  */
	struct asgMember *asp;	/* Access Security Pvt */
	struct putNotify *ppn;	/* addr of PUTNOTIFY */
	struct putNotifyRecord *ppnr;	/* pputNotifyRecord */
	struct scan_element *spvt;	/* Scan Private */
	struct rset	*rset;	/* Address of RSET */
	struct dset	*dset;	/* DSET address */
	void		*dpvt;	/* Device Private */
	struct dbRecordType *rdes;	/* Address of dbRecordType */
	struct lockRecord *lset;	/* Lock Set */
	epicsEnum16	prio;	/* Scheduling Priority */
	epicsUInt8	tpro;	/* Trace Processing */
	char bkpt;	/* Break Point */
	epicsUInt8	udf;	/* Undefined */
	epicsTimeStamp	time;	/* Time */
	DBLINK		flnk;	/* Forward Process Link */
	epicsFloat64	vers;	/* Code Version */
	epicsFloat64	val;	/* Value Field */
	char		smsg[40];	/* Record State Msg */
	epicsEnum16	cmnd;	/* Command Field */
	epicsUInt8	alrt;	/* Operator Alert */
	void *           rpvt;	/* Ptr to Pvt Struct */
	epicsInt32	mpts;	/* Max # of Points */
	epicsInt16	exsc;	/* Execute Scan */
	epicsInt16	xsc;	/* Internal execScan */
	epicsUInt8	pxsc;	/* Previous XScan */
	epicsUInt8	busy;	/* Scan in progress */
	epicsUInt8	kill;	/* Abort right now */
	epicsInt16	wait;	/* Wait for client(s) */
	epicsInt16	wcnt;	/* Wait count) */
	epicsInt16	awct;	/* Auto WCNT */
	epicsInt16	wtng;	/* Waiting for client(s) */
	epicsInt16	await;	/* Waiting for data-storage client */
	epicsEnum16	aawait;	/* AutoWait for data-storage client */
	epicsInt16	data;	/* Scan data ready */
	epicsInt16	refd;	/* Reference detector */
	epicsInt32	npts;	/* Number of Points */
	epicsEnum16	fpts;	/* Freeze Num of Points */
	epicsEnum16	ffo;	/* Freeze Flag Override */
	epicsInt32	cpt;	/* Current Point */
	epicsInt32	bcpt;	/* Bufferred Current Point */
	epicsInt32	dpt;	/* Desired Point */
	epicsInt32	pcpt;	/* Point ofLast Posting */
	epicsEnum16	pasm;	/* After Scan Mode */
	epicsTimeStamp	tolp;	/* Time of Last Posting */
	epicsTimeStamp	tlap;	/* Time of Last Array Posting */
	epicsFloat32	atime;	/* Array post time period */
	char		p1pv[40];	/* Positioner 1 PV Name */
	char		p2pv[40];	/* Positioner 2 PV Name */
	char		p3pv[40];	/* Positioner 3 PV Name */
	char		p4pv[40];	/* Positioner 4 PV Name */
	char		r1pv[40];	/* P1 Readback  PV Name */
	char		r2pv[40];	/* P2 Readback  PV Name */
	char		r3pv[40];	/* P3 Readback  PV Name */
	char		r4pv[40];	/* P4 Readback  PV Name */
	char		d01pv[40];	/* Detector PV Name */
	char		d02pv[40];	/* Detector PV Name */
	char		d03pv[40];	/* Detector PV Name */
	char		d04pv[40];	/* Detector PV Name */
	char		d05pv[40];	/* Detector PV Name */
	char		d06pv[40];	/* Detector PV Name */
	char		d07pv[40];	/* Detector PV Name */
	char		d08pv[40];	/* Detector PV Name */
	char		d09pv[40];	/* Detector PV Name */
	char		d10pv[40];	/* Detector PV Name */
	char		d11pv[40];	/* Detector PV Name */
	char		d12pv[40];	/* Detector PV Name */
	char		d13pv[40];	/* Detector PV Name */
	char		d14pv[40];	/* Detector PV Name */
	char		d15pv[40];	/* Detector PV Name */
	char		d16pv[40];	/* Detector PV Name */
	char		d17pv[40];	/* Detector PV Name */
	char		d18pv[40];	/* Detector PV Name */
	char		d19pv[40];	/* Detector PV Name */
	char		d20pv[40];	/* Detector PV Name */
	char		d21pv[40];	/* Detector PV Name */
	char		d22pv[40];	/* Detector PV Name */
	char		d23pv[40];	/* Detector PV Name */
	char		d24pv[40];	/* Detector PV Name */
	char		d25pv[40];	/* Detector PV Name */
	char		d26pv[40];	/* Detector PV Name */
	char		d27pv[40];	/* Detector PV Name */
	char		d28pv[40];	/* Detector PV Name */
	char		d29pv[40];	/* Detector PV Name */
	char		d30pv[40];	/* Detector PV Name */
	char		d31pv[40];	/* Detector PV Name */
	char		d32pv[40];	/* Detector PV Name */
	char		d33pv[40];	/* Detector PV Name */
	char		d34pv[40];	/* Detector PV Name */
	char		d35pv[40];	/* Detector PV Name */
	char		d36pv[40];	/* Detector PV Name */
	char		d37pv[40];	/* Detector PV Name */
	char		d38pv[40];	/* Detector PV Name */
	char		d39pv[40];	/* Detector PV Name */
	char		d40pv[40];	/* Detector PV Name */
	char		d41pv[40];	/* Detector PV Name */
	char		d42pv[40];	/* Detector PV Name */
	char		d43pv[40];	/* Detector PV Name */
	char		d44pv[40];	/* Detector PV Name */
	char		d45pv[40];	/* Detector PV Name */
	char		d46pv[40];	/* Detector PV Name */
	char		d47pv[40];	/* Detector PV Name */
	char		d48pv[40];	/* Detector PV Name */
	char		d49pv[40];	/* Detector PV Name */
	char		d50pv[40];	/* Detector PV Name */
	char		d51pv[40];	/* Detector PV Name */
	char		d52pv[40];	/* Detector PV Name */
	char		d53pv[40];	/* Detector PV Name */
	char		d54pv[40];	/* Detector PV Name */
	char		d55pv[40];	/* Detector PV Name */
	char		d56pv[40];	/* Detector PV Name */
	char		d57pv[40];	/* Detector PV Name */
	char		d58pv[40];	/* Detector PV Name */
	char		d59pv[40];	/* Detector PV Name */
	char		d60pv[40];	/* Detector PV Name */
	char		d61pv[40];	/* Detector PV Name */
	char		d62pv[40];	/* Detector PV Name */
	char		d63pv[40];	/* Detector PV Name */
	char		d64pv[40];	/* Detector PV Name */
	char		d65pv[40];	/* Detector PV Name */
	char		d66pv[40];	/* Detector PV Name */
	char		d67pv[40];	/* Detector PV Name */
	char		d68pv[40];	/* Detector PV Name */
	char		d69pv[40];	/* Detector PV Name */
	char		d70pv[40];	/* Detector PV Name */
	char		t1pv[40];	/* Trigger 1    PV Name */
	char		t2pv[40];	/* Trigger 2    PV Name */
	char		t3pv[40];	/* Trigger 3    PV Name */
	char		t4pv[40];	/* Trigger 4    PV Name */
	char		a1pv[40];	/* Array-read trigger 1 PV Name */
	char		bspv[40];	/* Before Scan  PV Name */
	char		aspv[40];	/* After Scan   PV Name */
	epicsEnum16	bswait;	/* Wait for completion? */
	epicsEnum16	aswait;	/* Wait for completion? */
	epicsEnum16	p1nv;	/* P1  PV Status */
	epicsEnum16	p2nv;	/* P2  PV Status */
	epicsEnum16	p3nv;	/* P3  PV Status */
	epicsEnum16	p4nv;	/* P4  PV Status */
	epicsEnum16	r1nv;	/* R1  PV Status */
	epicsEnum16	r2nv;	/* R2  PV Status */
	epicsEnum16	r3nv;	/* R3  PV Status */
	epicsEnum16	r4nv;	/* R4  PV Status */
	epicsEnum16	d01nv;	/* PV Status */
	epicsEnum16	d02nv;	/* PV Status */
	epicsEnum16	d03nv;	/* PV Status */
	epicsEnum16	d04nv;	/* PV Status */
	epicsEnum16	d05nv;	/* PV Status */
	epicsEnum16	d06nv;	/* PV Status */
	epicsEnum16	d07nv;	/* PV Status */
	epicsEnum16	d08nv;	/* PV Status */
	epicsEnum16	d09nv;	/* PV Status */
	epicsEnum16	d10nv;	/* PV Status */
	epicsEnum16	d11nv;	/* PV Status */
	epicsEnum16	d12nv;	/* PV Status */
	epicsEnum16	d13nv;	/* PV Status */
	epicsEnum16	d14nv;	/* PV Status */
	epicsEnum16	d15nv;	/* PV Status */
	epicsEnum16	d16nv;	/* PV Status */
	epicsEnum16	d17nv;	/* PV Status */
	epicsEnum16	d18nv;	/* PV Status */
	epicsEnum16	d19nv;	/* PV Status */
	epicsEnum16	d20nv;	/* PV Status */
	epicsEnum16	d21nv;	/* PV Status */
	epicsEnum16	d22nv;	/* PV Status */
	epicsEnum16	d23nv;	/* PV Status */
	epicsEnum16	d24nv;	/* PV Status */
	epicsEnum16	d25nv;	/* PV Status */
	epicsEnum16	d26nv;	/* PV Status */
	epicsEnum16	d27nv;	/* PV Status */
	epicsEnum16	d28nv;	/* PV Status */
	epicsEnum16	d29nv;	/* PV Status */
	epicsEnum16	d30nv;	/* PV Status */
	epicsEnum16	d31nv;	/* PV Status */
	epicsEnum16	d32nv;	/* PV Status */
	epicsEnum16	d33nv;	/* PV Status */
	epicsEnum16	d34nv;	/* PV Status */
	epicsEnum16	d35nv;	/* PV Status */
	epicsEnum16	d36nv;	/* PV Status */
	epicsEnum16	d37nv;	/* PV Status */
	epicsEnum16	d38nv;	/* PV Status */
	epicsEnum16	d39nv;	/* PV Status */
	epicsEnum16	d40nv;	/* PV Status */
	epicsEnum16	d41nv;	/* PV Status */
	epicsEnum16	d42nv;	/* PV Status */
	epicsEnum16	d43nv;	/* PV Status */
	epicsEnum16	d44nv;	/* PV Status */
	epicsEnum16	d45nv;	/* PV Status */
	epicsEnum16	d46nv;	/* PV Status */
	epicsEnum16	d47nv;	/* PV Status */
	epicsEnum16	d48nv;	/* PV Status */
	epicsEnum16	d49nv;	/* PV Status */
	epicsEnum16	d50nv;	/* PV Status */
	epicsEnum16	d51nv;	/* PV Status */
	epicsEnum16	d52nv;	/* PV Status */
	epicsEnum16	d53nv;	/* PV Status */
	epicsEnum16	d54nv;	/* PV Status */
	epicsEnum16	d55nv;	/* PV Status */
	epicsEnum16	d56nv;	/* PV Status */
	epicsEnum16	d57nv;	/* PV Status */
	epicsEnum16	d58nv;	/* PV Status */
	epicsEnum16	d59nv;	/* PV Status */
	epicsEnum16	d60nv;	/* PV Status */
	epicsEnum16	d61nv;	/* PV Status */
	epicsEnum16	d62nv;	/* PV Status */
	epicsEnum16	d63nv;	/* PV Status */
	epicsEnum16	d64nv;	/* PV Status */
	epicsEnum16	d65nv;	/* PV Status */
	epicsEnum16	d66nv;	/* PV Status */
	epicsEnum16	d67nv;	/* PV Status */
	epicsEnum16	d68nv;	/* PV Status */
	epicsEnum16	d69nv;	/* PV Status */
	epicsEnum16	d70nv;	/* PV Status */
	epicsEnum16	t1nv;	/* T1  PV Status */
	epicsEnum16	t2nv;	/* T2  PV Status */
	epicsEnum16	t3nv;	/* T3  PV Status */
	epicsEnum16	t4nv;	/* T4  PV Status */
	epicsEnum16	a1nv;	/* A1  PV Status */
	epicsEnum16	bsnv;	/* BeforeScan PV Status */
	epicsEnum16	asnv;	/* After Scan PV Status */
	epicsFloat64	p1pp;	/* P1 Previous Position */
	epicsFloat64	p1cv;	/* P1 Current Value */
	epicsFloat64	p1dv;	/* P1 Desired Value */
	epicsFloat64	p1lv;	/* P1 Last Value Posted */
	epicsFloat64	p1sp;	/* P1 Start Position */
	epicsFloat64	p1si;	/* P1 Step Increment */
	epicsFloat64	p1ep;	/* P1 End Position */
	epicsFloat64	p1cp;	/* P1 Center Position */
	epicsFloat64	p1wd;	/* P1 Scan Width */
	epicsFloat64	r1cv;	/* P1 Readback Value */
	epicsFloat64	r1lv;	/* P1 Rdbk Last Val Pst */
	epicsFloat64	r1dl;	/* P1 Readback Delta */
	epicsFloat64	p1hr;	/* P1 High Oper Range */
	epicsFloat64	p1lr;	/* P1 Low  Oper Range */
	double *         p1pa;	/* P1 Step Array */
	double *         p1ra;	/* P1 Readback Array */
	double *         p1ca;	/* P1 Current Readback Array */
	epicsEnum16	p1fs;	/* P1 Freeze Start Pos */
	epicsEnum16	p1fi;	/* P1 Freeze Step Inc */
	epicsEnum16	p1fe;	/* P1 Freeze End Pos */
	epicsEnum16	p1fc;	/* P1 Freeze Center Pos */
	epicsEnum16	p1fw;	/* P1 Freeze Width */
	epicsEnum16	p1sm;	/* P1 Step Mode */
	epicsEnum16	p1ar;	/* P1 Absolute/Relative */
	char		p1eu[16];	/* P1 Engineering Units */
	epicsInt16	p1pr;	/* P1 Display Precision */
	epicsFloat64	p2pp;	/* P2 Previous Position */
	epicsFloat64	p2cv;	/* P2 Current Value */
	epicsFloat64	p2dv;	/* P2 Desired Value */
	epicsFloat64	p2lv;	/* P2 Last Value Posted */
	epicsFloat64	p2sp;	/* P2 Start Position */
	epicsFloat64	p2si;	/* P2 Step Increment */
	epicsFloat64	p2ep;	/* P2 End Position */
	epicsFloat64	p2cp;	/* P2 Center Position */
	epicsFloat64	p2wd;	/* P2 Scan Width */
	epicsFloat64	r2cv;	/* P2 Readback Value */
	epicsFloat64	r2lv;	/* P2 Rdbk Last Val Pst */
	epicsFloat64	r2dl;	/* P2 Readback Delta */
	epicsFloat64	p2hr;	/* P2 High Oper Range */
	epicsFloat64	p2lr;	/* P2 Low  Oper Range */
	double *         p2pa;	/* P2 Step Array */
	double *         p2ra;	/* P2 Readback Array */
	double *         p2ca;	/* P2 Current Readback Array */
	epicsEnum16	p2fs;	/* P2 Freeze Start Pos */
	epicsEnum16	p2fi;	/* P2 Freeze Step Inc */
	epicsEnum16	p2fe;	/* P2 Freeze End Pos */
	epicsEnum16	p2fc;	/* P2 Freeze Center Pos */
	epicsEnum16	p2fw;	/* P2 Freeze Width */
	epicsEnum16	p2sm;	/* P2 Step Mode */
	epicsEnum16	p2ar;	/* P2 Absolute/Relative */
	char		p2eu[16];	/* P2 Engineering Units */
	epicsInt16	p2pr;	/* P2 Display Precision */
	epicsFloat64	p3pp;	/* P3 Previous Position */
	epicsFloat64	p3cv;	/* P3 Current Value */
	epicsFloat64	p3dv;	/* P3 Desired Value */
	epicsFloat64	p3lv;	/* P3 Last Value Posted */
	epicsFloat64	p3sp;	/* P3 Start Position */
	epicsFloat64	p3si;	/* P3 Step Increment */
	epicsFloat64	p3ep;	/* P3 End Position */
	epicsFloat64	p3cp;	/* P3 Center Position */
	epicsFloat64	p3wd;	/* P3 Scan Width */
	epicsFloat64	r3cv;	/* P3 Readback Value */
	epicsFloat64	r3lv;	/* P3 Rdbk Last Val Pst */
	epicsFloat64	r3dl;	/* P3 Readback Delta */
	epicsFloat64	p3hr;	/* P3 High Oper Range */
	epicsFloat64	p3lr;	/* P3 Low  Oper Range */
	double *         p3pa;	/* P3 Step Array */
	double *         p3ra;	/* P3 Readback Array */
	double *         p3ca;	/* P3 Current Readback Array */
	epicsEnum16	p3fs;	/* P3 Freeze Start Pos */
	epicsEnum16	p3fi;	/* P3 Freeze Step Inc */
	epicsEnum16	p3fe;	/* P3 Freeze End Pos */
	epicsEnum16	p3fc;	/* P3 Freeze Center Pos */
	epicsEnum16	p3fw;	/* P3 Freeze Width */
	epicsEnum16	p3sm;	/* P3 Step Mode */
	epicsEnum16	p3ar;	/* P3 Absolute/Relative */
	char		p3eu[16];	/* P3 Engineering Units */
	epicsInt16	p3pr;	/* P3 Display Precision */
	epicsFloat64	p4pp;	/* P4 Previous Position */
	epicsFloat64	p4cv;	/* P4 Current Value */
	epicsFloat64	p4dv;	/* P4 Desired Value */
	epicsFloat64	p4lv;	/* P4 Last Value Posted */
	epicsFloat64	p4sp;	/* P4 Start Position */
	epicsFloat64	p4si;	/* P4 Step Increment */
	epicsFloat64	p4ep;	/* P4 End Position */
	epicsFloat64	p4cp;	/* P4 Center Position */
	epicsFloat64	p4wd;	/* P4 Scan Width */
	epicsFloat64	r4cv;	/* P4 Readback Value */
	epicsFloat64	r4lv;	/* P4 Rdbk Last Val Pst */
	epicsFloat64	r4dl;	/* P4 Readback Delta */
	epicsFloat64	p4hr;	/* P4 High Oper Range */
	epicsFloat64	p4lr;	/* P4 Low  Oper Range */
	double *         p4pa;	/* P4 Step Array */
	double *         p4ra;	/* P4 Readback Array */
	double *         p4ca;	/* P4 Current Readback Array */
	epicsEnum16	p4fs;	/* P4 Freeze Start Pos */
	epicsEnum16	p4fi;	/* P4 Freeze Step Inc */
	epicsEnum16	p4fe;	/* P4 Freeze End Pos */
	epicsEnum16	p4fc;	/* P4 Freeze Center Pos */
	epicsEnum16	p4fw;	/* P4 Freeze Width */
	epicsEnum16	p4sm;	/* P4 Step Mode */
	epicsEnum16	p4ar;	/* P4 Absolute/Relative */
	char		p4eu[16];	/* P4 Engineering Units */
	epicsInt16	p4pr;	/* P4 Display Precision */
	epicsFloat64	d01hr;	/* High Oper Range */
	epicsFloat64	d01lr;	/* Low  Oper Range */
	float *          d01da;	/* Data Array */
	float *          d01ca;	/* Current Data Array */
	epicsFloat32	d01cv;	/* Current Value */
	epicsFloat32	d01lv;	/* Last Value Posted */
	epicsUInt32	d01ne;	/* # of Elements/Pt */
	char		d01eu[16];	/* Engineering Units */
	epicsInt16	d01pr;	/* Display Precision */
	epicsFloat64	d02hr;	/* High Oper Range */
	epicsFloat64	d02lr;	/* Low  Oper Range */
	float *          d02da;	/* Data Array */
	float *          d02ca;	/* Current Data Array */
	epicsFloat32	d02cv;	/* Current Value */
	epicsFloat32	d02lv;	/* Last Value Posted */
	epicsUInt32	d02ne;	/* # of Elements/Pt */
	char		d02eu[16];	/* Engineering Units */
	epicsInt16	d02pr;	/* Display Precision */
	epicsFloat64	d03hr;	/* High Oper Range */
	epicsFloat64	d03lr;	/* Low  Oper Range */
	float *          d03da;	/* Data Array */
	float *          d03ca;	/* Current Data Array */
	epicsFloat32	d03cv;	/* Current Value */
	epicsFloat32	d03lv;	/* Last Value Posted */
	epicsUInt32	d03ne;	/* # of Elements/Pt */
	char		d03eu[16];	/* Engineering Units */
	epicsInt16	d03pr;	/* Display Precision */
	epicsFloat64	d04hr;	/* High Oper Range */
	epicsFloat64	d04lr;	/* Low  Oper Range */
	float *          d04da;	/* Data Array */
	float *          d04ca;	/* Current Data Array */
	epicsFloat32	d04cv;	/* Current Value */
	epicsFloat32	d04lv;	/* Last Value Posted */
	epicsUInt32	d04ne;	/* # of Elements/Pt */
	char		d04eu[16];	/* Engineering Units */
	epicsInt16	d04pr;	/* Display Precision */
	epicsFloat64	d05hr;	/* High Oper Range */
	epicsFloat64	d05lr;	/* Low  Oper Range */
	float *          d05da;	/* Data Array */
	float *          d05ca;	/* Current Data Array */
	epicsFloat32	d05cv;	/* Current Value */
	epicsFloat32	d05lv;	/* Last Value Posted */
	epicsUInt32	d05ne;	/* # of Elements/Pt */
	char		d05eu[16];	/* Engineering Units */
	epicsInt16	d05pr;	/* Display Precision */
	epicsFloat64	d06hr;	/* High Oper Range */
	epicsFloat64	d06lr;	/* Low  Oper Range */
	float *          d06da;	/* Data Array */
	float *          d06ca;	/* Current Data Array */
	epicsFloat32	d06cv;	/* Current Value */
	epicsFloat32	d06lv;	/* Last Value Posted */
	epicsUInt32	d06ne;	/* # of Elements/Pt */
	char		d06eu[16];	/* Engineering Units */
	epicsInt16	d06pr;	/* Display Precision */
	epicsFloat64	d07hr;	/* High Oper Range */
	epicsFloat64	d07lr;	/* Low  Oper Range */
	float *          d07da;	/* Data Array */
	float *          d07ca;	/* Current Data Array */
	epicsFloat32	d07cv;	/* Current Value */
	epicsFloat32	d07lv;	/* Last Value Posted */
	epicsUInt32	d07ne;	/* # of Elements/Pt */
	char		d07eu[16];	/* Engineering Units */
	epicsInt16	d07pr;	/* Display Precision */
	epicsFloat64	d08hr;	/* High Oper Range */
	epicsFloat64	d08lr;	/* Low  Oper Range */
	float *          d08da;	/* Data Array */
	float *          d08ca;	/* Current Data Array */
	epicsFloat32	d08cv;	/* Current Value */
	epicsFloat32	d08lv;	/* Last Value Posted */
	epicsUInt32	d08ne;	/* # of Elements/Pt */
	char		d08eu[16];	/* Engineering Units */
	epicsInt16	d08pr;	/* Display Precision */
	epicsFloat64	d09hr;	/* High Oper Range */
	epicsFloat64	d09lr;	/* Low  Oper Range */
	float *          d09da;	/* Data Array */
	float *          d09ca;	/* Current Data Array */
	epicsFloat32	d09cv;	/* Current Value */
	epicsFloat32	d09lv;	/* Last Value Posted */
	epicsUInt32	d09ne;	/* # of Elements/Pt */
	char		d09eu[16];	/* Engineering Units */
	epicsInt16	d09pr;	/* Display Precision */
	epicsFloat64	d10hr;	/* High Oper Range */
	epicsFloat64	d10lr;	/* Low  Oper Range */
	float *          d10da;	/* Data Array */
	float *          d10ca;	/* Current Data Array */
	epicsFloat32	d10cv;	/* Current Value */
	epicsFloat32	d10lv;	/* Last Value Posted */
	epicsUInt32	d10ne;	/* # of Elements/Pt */
	char		d10eu[16];	/* Engineering Units */
	epicsInt16	d10pr;	/* Display Precision */
	epicsFloat64	d11hr;	/* High Oper Range */
	epicsFloat64	d11lr;	/* Low  Oper Range */
	float *          d11da;	/* Data Array */
	float *          d11ca;	/* Current Data Array */
	epicsFloat32	d11cv;	/* Current Value */
	epicsFloat32	d11lv;	/* Last Value Posted */
	epicsUInt32	d11ne;	/* # of Elements/Pt */
	char		d11eu[16];	/* Engineering Units */
	epicsInt16	d11pr;	/* Display Precision */
	epicsFloat64	d12hr;	/* High Oper Range */
	epicsFloat64	d12lr;	/* Low  Oper Range */
	float *          d12da;	/* Data Array */
	float *          d12ca;	/* Current Data Array */
	epicsFloat32	d12cv;	/* Current Value */
	epicsFloat32	d12lv;	/* Last Value Posted */
	epicsUInt32	d12ne;	/* # of Elements/Pt */
	char		d12eu[16];	/* Engineering Units */
	epicsInt16	d12pr;	/* Display Precision */
	epicsFloat64	d13hr;	/* High Oper Range */
	epicsFloat64	d13lr;	/* Low  Oper Range */
	float *          d13da;	/* Data Array */
	float *          d13ca;	/* Current Data Array */
	epicsFloat32	d13cv;	/* Current Value */
	epicsFloat32	d13lv;	/* Last Value Posted */
	epicsUInt32	d13ne;	/* # of Elements/Pt */
	char		d13eu[16];	/* Engineering Units */
	epicsInt16	d13pr;	/* Display Precision */
	epicsFloat64	d14hr;	/* High Oper Range */
	epicsFloat64	d14lr;	/* Low  Oper Range */
	float *          d14da;	/* Data Array */
	float *          d14ca;	/* Current Data Array */
	epicsFloat32	d14cv;	/* Current Value */
	epicsFloat32	d14lv;	/* Last Value Posted */
	epicsUInt32	d14ne;	/* # of Elements/Pt */
	char		d14eu[16];	/* Engineering Units */
	epicsInt16	d14pr;	/* Display Precision */
	epicsFloat64	d15hr;	/* High Oper Range */
	epicsFloat64	d15lr;	/* Low  Oper Range */
	float *          d15da;	/* Data Array */
	float *          d15ca;	/* Current Data Array */
	epicsFloat32	d15cv;	/* Current Value */
	epicsFloat32	d15lv;	/* Last Value Posted */
	epicsUInt32	d15ne;	/* # of Elements/Pt */
	char		d15eu[16];	/* Engineering Units */
	epicsInt16	d15pr;	/* Display Precision */
	epicsFloat64	d16hr;	/* High Oper Range */
	epicsFloat64	d16lr;	/* Low  Oper Range */
	float *          d16da;	/* Data Array */
	float *          d16ca;	/* Current Data Array */
	epicsFloat32	d16cv;	/* Current Value */
	epicsFloat32	d16lv;	/* Last Value Posted */
	epicsUInt32	d16ne;	/* # of Elements/Pt */
	char		d16eu[16];	/* Engineering Units */
	epicsInt16	d16pr;	/* Display Precision */
	epicsFloat64	d17hr;	/* High Oper Range */
	epicsFloat64	d17lr;	/* Low  Oper Range */
	float *          d17da;	/* Data Array */
	float *          d17ca;	/* Current Data Array */
	epicsFloat32	d17cv;	/* Current Value */
	epicsFloat32	d17lv;	/* Last Value Posted */
	epicsUInt32	d17ne;	/* # of Elements/Pt */
	char		d17eu[16];	/* Engineering Units */
	epicsInt16	d17pr;	/* Display Precision */
	epicsFloat64	d18hr;	/* High Oper Range */
	epicsFloat64	d18lr;	/* Low  Oper Range */
	float *          d18da;	/* Data Array */
	float *          d18ca;	/* Current Data Array */
	epicsFloat32	d18cv;	/* Current Value */
	epicsFloat32	d18lv;	/* Last Value Posted */
	epicsUInt32	d18ne;	/* # of Elements/Pt */
	char		d18eu[16];	/* Engineering Units */
	epicsInt16	d18pr;	/* Display Precision */
	epicsFloat64	d19hr;	/* High Oper Range */
	epicsFloat64	d19lr;	/* Low  Oper Range */
	float *          d19da;	/* Data Array */
	float *          d19ca;	/* Current Data Array */
	epicsFloat32	d19cv;	/* Current Value */
	epicsFloat32	d19lv;	/* Last Value Posted */
	epicsUInt32	d19ne;	/* # of Elements/Pt */
	char		d19eu[16];	/* Engineering Units */
	epicsInt16	d19pr;	/* Display Precision */
	epicsFloat64	d20hr;	/* High Oper Range */
	epicsFloat64	d20lr;	/* Low  Oper Range */
	float *          d20da;	/* Data Array */
	float *          d20ca;	/* Current Data Array */
	epicsFloat32	d20cv;	/* Current Value */
	epicsFloat32	d20lv;	/* Last Value Posted */
	epicsUInt32	d20ne;	/* # of Elements/Pt */
	char		d20eu[16];	/* Engineering Units */
	epicsInt16	d20pr;	/* Display Precision */
	epicsFloat64	d21hr;	/* High Oper Range */
	epicsFloat64	d21lr;	/* Low  Oper Range */
	float *          d21da;	/* Data Array */
	float *          d21ca;	/* Current Data Array */
	epicsFloat32	d21cv;	/* Current Value */
	epicsFloat32	d21lv;	/* Last Value Posted */
	epicsUInt32	d21ne;	/* # of Elements/Pt */
	char		d21eu[16];	/* Engineering Units */
	epicsInt16	d21pr;	/* Display Precision */
	epicsFloat64	d22hr;	/* High Oper Range */
	epicsFloat64	d22lr;	/* Low  Oper Range */
	float *          d22da;	/* Data Array */
	float *          d22ca;	/* Current Data Array */
	epicsFloat32	d22cv;	/* Current Value */
	epicsFloat32	d22lv;	/* Last Value Posted */
	epicsUInt32	d22ne;	/* # of Elements/Pt */
	char		d22eu[16];	/* Engineering Units */
	epicsInt16	d22pr;	/* Display Precision */
	epicsFloat64	d23hr;	/* High Oper Range */
	epicsFloat64	d23lr;	/* Low  Oper Range */
	float *          d23da;	/* Data Array */
	float *          d23ca;	/* Current Data Array */
	epicsFloat32	d23cv;	/* Current Value */
	epicsFloat32	d23lv;	/* Last Value Posted */
	epicsUInt32	d23ne;	/* # of Elements/Pt */
	char		d23eu[16];	/* Engineering Units */
	epicsInt16	d23pr;	/* Display Precision */
	epicsFloat64	d24hr;	/* High Oper Range */
	epicsFloat64	d24lr;	/* Low  Oper Range */
	float *          d24da;	/* Data Array */
	float *          d24ca;	/* Current Data Array */
	epicsFloat32	d24cv;	/* Current Value */
	epicsFloat32	d24lv;	/* Last Value Posted */
	epicsUInt32	d24ne;	/* # of Elements/Pt */
	char		d24eu[16];	/* Engineering Units */
	epicsInt16	d24pr;	/* Display Precision */
	epicsFloat64	d25hr;	/* High Oper Range */
	epicsFloat64	d25lr;	/* Low  Oper Range */
	float *          d25da;	/* Data Array */
	float *          d25ca;	/* Current Data Array */
	epicsFloat32	d25cv;	/* Current Value */
	epicsFloat32	d25lv;	/* Last Value Posted */
	epicsUInt32	d25ne;	/* # of Elements/Pt */
	char		d25eu[16];	/* Engineering Units */
	epicsInt16	d25pr;	/* Display Precision */
	epicsFloat64	d26hr;	/* High Oper Range */
	epicsFloat64	d26lr;	/* Low  Oper Range */
	float *          d26da;	/* Data Array */
	float *          d26ca;	/* Current Data Array */
	epicsFloat32	d26cv;	/* Current Value */
	epicsFloat32	d26lv;	/* Last Value Posted */
	epicsUInt32	d26ne;	/* # of Elements/Pt */
	char		d26eu[16];	/* Engineering Units */
	epicsInt16	d26pr;	/* Display Precision */
	epicsFloat64	d27hr;	/* High Oper Range */
	epicsFloat64	d27lr;	/* Low  Oper Range */
	float *          d27da;	/* Data Array */
	float *          d27ca;	/* Current Data Array */
	epicsFloat32	d27cv;	/* Current Value */
	epicsFloat32	d27lv;	/* Last Value Posted */
	epicsUInt32	d27ne;	/* # of Elements/Pt */
	char		d27eu[16];	/* Engineering Units */
	epicsInt16	d27pr;	/* Display Precision */
	epicsFloat64	d28hr;	/* High Oper Range */
	epicsFloat64	d28lr;	/* Low  Oper Range */
	float *          d28da;	/* Data Array */
	float *          d28ca;	/* Current Data Array */
	epicsFloat32	d28cv;	/* Current Value */
	epicsFloat32	d28lv;	/* Last Value Posted */
	epicsUInt32	d28ne;	/* # of Elements/Pt */
	char		d28eu[16];	/* Engineering Units */
	epicsInt16	d28pr;	/* Display Precision */
	epicsFloat64	d29hr;	/* High Oper Range */
	epicsFloat64	d29lr;	/* Low  Oper Range */
	float *          d29da;	/* Data Array */
	float *          d29ca;	/* Current Data Array */
	epicsFloat32	d29cv;	/* Current Value */
	epicsFloat32	d29lv;	/* Last Value Posted */
	epicsUInt32	d29ne;	/* # of Elements/Pt */
	char		d29eu[16];	/* Engineering Units */
	epicsInt16	d29pr;	/* Display Precision */
	epicsFloat64	d30hr;	/* High Oper Range */
	epicsFloat64	d30lr;	/* Low  Oper Range */
	float *          d30da;	/* Data Array */
	float *          d30ca;	/* Current Data Array */
	epicsFloat32	d30cv;	/* Current Value */
	epicsFloat32	d30lv;	/* Last Value Posted */
	epicsUInt32	d30ne;	/* # of Elements/Pt */
	char		d30eu[16];	/* Engineering Units */
	epicsInt16	d30pr;	/* Display Precision */
	epicsFloat64	d31hr;	/* High Oper Range */
	epicsFloat64	d31lr;	/* Low  Oper Range */
	float *          d31da;	/* Data Array */
	float *          d31ca;	/* Current Data Array */
	epicsFloat32	d31cv;	/* Current Value */
	epicsFloat32	d31lv;	/* Last Value Posted */
	epicsUInt32	d31ne;	/* # of Elements/Pt */
	char		d31eu[16];	/* Engineering Units */
	epicsInt16	d31pr;	/* Display Precision */
	epicsFloat64	d32hr;	/* High Oper Range */
	epicsFloat64	d32lr;	/* Low  Oper Range */
	float *          d32da;	/* Data Array */
	float *          d32ca;	/* Current Data Array */
	epicsFloat32	d32cv;	/* Current Value */
	epicsFloat32	d32lv;	/* Last Value Posted */
	epicsUInt32	d32ne;	/* # of Elements/Pt */
	char		d32eu[16];	/* Engineering Units */
	epicsInt16	d32pr;	/* Display Precision */
	epicsFloat64	d33hr;	/* High Oper Range */
	epicsFloat64	d33lr;	/* Low  Oper Range */
	float *          d33da;	/* Data Array */
	float *          d33ca;	/* Current Data Array */
	epicsFloat32	d33cv;	/* Current Value */
	epicsFloat32	d33lv;	/* Last Value Posted */
	epicsUInt32	d33ne;	/* # of Elements/Pt */
	char		d33eu[16];	/* Engineering Units */
	epicsInt16	d33pr;	/* Display Precision */
	epicsFloat64	d34hr;	/* High Oper Range */
	epicsFloat64	d34lr;	/* Low  Oper Range */
	float *          d34da;	/* Data Array */
	float *          d34ca;	/* Current Data Array */
	epicsFloat32	d34cv;	/* Current Value */
	epicsFloat32	d34lv;	/* Last Value Posted */
	epicsUInt32	d34ne;	/* # of Elements/Pt */
	char		d34eu[16];	/* Engineering Units */
	epicsInt16	d34pr;	/* Display Precision */
	epicsFloat64	d35hr;	/* High Oper Range */
	epicsFloat64	d35lr;	/* Low  Oper Range */
	float *          d35da;	/* Data Array */
	float *          d35ca;	/* Current Data Array */
	epicsFloat32	d35cv;	/* Current Value */
	epicsFloat32	d35lv;	/* Last Value Posted */
	epicsUInt32	d35ne;	/* # of Elements/Pt */
	char		d35eu[16];	/* Engineering Units */
	epicsInt16	d35pr;	/* Display Precision */
	epicsFloat64	d36hr;	/* High Oper Range */
	epicsFloat64	d36lr;	/* Low  Oper Range */
	float *          d36da;	/* Data Array */
	float *          d36ca;	/* Current Data Array */
	epicsFloat32	d36cv;	/* Current Value */
	epicsFloat32	d36lv;	/* Last Value Posted */
	epicsUInt32	d36ne;	/* # of Elements/Pt */
	char		d36eu[16];	/* Engineering Units */
	epicsInt16	d36pr;	/* Display Precision */
	epicsFloat64	d37hr;	/* High Oper Range */
	epicsFloat64	d37lr;	/* Low  Oper Range */
	float *          d37da;	/* Data Array */
	float *          d37ca;	/* Current Data Array */
	epicsFloat32	d37cv;	/* Current Value */
	epicsFloat32	d37lv;	/* Last Value Posted */
	epicsUInt32	d37ne;	/* # of Elements/Pt */
	char		d37eu[16];	/* Engineering Units */
	epicsInt16	d37pr;	/* Display Precision */
	epicsFloat64	d38hr;	/* High Oper Range */
	epicsFloat64	d38lr;	/* Low  Oper Range */
	float *          d38da;	/* Data Array */
	float *          d38ca;	/* Current Data Array */
	epicsFloat32	d38cv;	/* Current Value */
	epicsFloat32	d38lv;	/* Last Value Posted */
	epicsUInt32	d38ne;	/* # of Elements/Pt */
	char		d38eu[16];	/* Engineering Units */
	epicsInt16	d38pr;	/* Display Precision */
	epicsFloat64	d39hr;	/* High Oper Range */
	epicsFloat64	d39lr;	/* Low  Oper Range */
	float *          d39da;	/* Data Array */
	float *          d39ca;	/* Current Data Array */
	epicsFloat32	d39cv;	/* Current Value */
	epicsFloat32	d39lv;	/* Last Value Posted */
	epicsUInt32	d39ne;	/* # of Elements/Pt */
	char		d39eu[16];	/* Engineering Units */
	epicsInt16	d39pr;	/* Display Precision */
	epicsFloat64	d40hr;	/* High Oper Range */
	epicsFloat64	d40lr;	/* Low  Oper Range */
	float *          d40da;	/* Data Array */
	float *          d40ca;	/* Current Data Array */
	epicsFloat32	d40cv;	/* Current Value */
	epicsFloat32	d40lv;	/* Last Value Posted */
	epicsUInt32	d40ne;	/* # of Elements/Pt */
	char		d40eu[16];	/* Engineering Units */
	epicsInt16	d40pr;	/* Display Precision */
	epicsFloat64	d41hr;	/* High Oper Range */
	epicsFloat64	d41lr;	/* Low  Oper Range */
	float *          d41da;	/* Data Array */
	float *          d41ca;	/* Current Data Array */
	epicsFloat32	d41cv;	/* Current Value */
	epicsFloat32	d41lv;	/* Last Value Posted */
	epicsUInt32	d41ne;	/* # of Elements/Pt */
	char		d41eu[16];	/* Engineering Units */
	epicsInt16	d41pr;	/* Display Precision */
	epicsFloat64	d42hr;	/* High Oper Range */
	epicsFloat64	d42lr;	/* Low  Oper Range */
	float *          d42da;	/* Data Array */
	float *          d42ca;	/* Current Data Array */
	epicsFloat32	d42cv;	/* Current Value */
	epicsFloat32	d42lv;	/* Last Value Posted */
	epicsUInt32	d42ne;	/* # of Elements/Pt */
	char		d42eu[16];	/* Engineering Units */
	epicsInt16	d42pr;	/* Display Precision */
	epicsFloat64	d43hr;	/* High Oper Range */
	epicsFloat64	d43lr;	/* Low  Oper Range */
	float *          d43da;	/* Data Array */
	float *          d43ca;	/* Current Data Array */
	epicsFloat32	d43cv;	/* Current Value */
	epicsFloat32	d43lv;	/* Last Value Posted */
	epicsUInt32	d43ne;	/* # of Elements/Pt */
	char		d43eu[16];	/* Engineering Units */
	epicsInt16	d43pr;	/* Display Precision */
	epicsFloat64	d44hr;	/* High Oper Range */
	epicsFloat64	d44lr;	/* Low  Oper Range */
	float *          d44da;	/* Data Array */
	float *          d44ca;	/* Current Data Array */
	epicsFloat32	d44cv;	/* Current Value */
	epicsFloat32	d44lv;	/* Last Value Posted */
	epicsUInt32	d44ne;	/* # of Elements/Pt */
	char		d44eu[16];	/* Engineering Units */
	epicsInt16	d44pr;	/* Display Precision */
	epicsFloat64	d45hr;	/* High Oper Range */
	epicsFloat64	d45lr;	/* Low  Oper Range */
	float *          d45da;	/* Data Array */
	float *          d45ca;	/* Current Data Array */
	epicsFloat32	d45cv;	/* Current Value */
	epicsFloat32	d45lv;	/* Last Value Posted */
	epicsUInt32	d45ne;	/* # of Elements/Pt */
	char		d45eu[16];	/* Engineering Units */
	epicsInt16	d45pr;	/* Display Precision */
	epicsFloat64	d46hr;	/* High Oper Range */
	epicsFloat64	d46lr;	/* Low  Oper Range */
	float *          d46da;	/* Data Array */
	float *          d46ca;	/* Current Data Array */
	epicsFloat32	d46cv;	/* Current Value */
	epicsFloat32	d46lv;	/* Last Value Posted */
	epicsUInt32	d46ne;	/* # of Elements/Pt */
	char		d46eu[16];	/* Engineering Units */
	epicsInt16	d46pr;	/* Display Precision */
	epicsFloat64	d47hr;	/* High Oper Range */
	epicsFloat64	d47lr;	/* Low  Oper Range */
	float *          d47da;	/* Data Array */
	float *          d47ca;	/* Current Data Array */
	epicsFloat32	d47cv;	/* Current Value */
	epicsFloat32	d47lv;	/* Last Value Posted */
	epicsUInt32	d47ne;	/* # of Elements/Pt */
	char		d47eu[16];	/* Engineering Units */
	epicsInt16	d47pr;	/* Display Precision */
	epicsFloat64	d48hr;	/* High Oper Range */
	epicsFloat64	d48lr;	/* Low  Oper Range */
	float *          d48da;	/* Data Array */
	float *          d48ca;	/* Current Data Array */
	epicsFloat32	d48cv;	/* Current Value */
	epicsFloat32	d48lv;	/* Last Value Posted */
	epicsUInt32	d48ne;	/* # of Elements/Pt */
	char		d48eu[16];	/* Engineering Units */
	epicsInt16	d48pr;	/* Display Precision */
	epicsFloat64	d49hr;	/* High Oper Range */
	epicsFloat64	d49lr;	/* Low  Oper Range */
	float *          d49da;	/* Data Array */
	float *          d49ca;	/* Current Data Array */
	epicsFloat32	d49cv;	/* Current Value */
	epicsFloat32	d49lv;	/* Last Value Posted */
	epicsUInt32	d49ne;	/* # of Elements/Pt */
	char		d49eu[16];	/* Engineering Units */
	epicsInt16	d49pr;	/* Display Precision */
	epicsFloat64	d50hr;	/* High Oper Range */
	epicsFloat64	d50lr;	/* Low  Oper Range */
	float *          d50da;	/* Data Array */
	float *          d50ca;	/* Current Data Array */
	epicsFloat32	d50cv;	/* Current Value */
	epicsFloat32	d50lv;	/* Last Value Posted */
	epicsUInt32	d50ne;	/* # of Elements/Pt */
	char		d50eu[16];	/* Engineering Units */
	epicsInt16	d50pr;	/* Display Precision */
	epicsFloat64	d51hr;	/* High Oper Range */
	epicsFloat64	d51lr;	/* Low  Oper Range */
	float *          d51da;	/* Data Array */
	float *          d51ca;	/* Current Data Array */
	epicsFloat32	d51cv;	/* Current Value */
	epicsFloat32	d51lv;	/* Last Value Posted */
	epicsUInt32	d51ne;	/* # of Elements/Pt */
	char		d51eu[16];	/* Engineering Units */
	epicsInt16	d51pr;	/* Display Precision */
	epicsFloat64	d52hr;	/* High Oper Range */
	epicsFloat64	d52lr;	/* Low  Oper Range */
	float *          d52da;	/* Data Array */
	float *          d52ca;	/* Current Data Array */
	epicsFloat32	d52cv;	/* Current Value */
	epicsFloat32	d52lv;	/* Last Value Posted */
	epicsUInt32	d52ne;	/* # of Elements/Pt */
	char		d52eu[16];	/* Engineering Units */
	epicsInt16	d52pr;	/* Display Precision */
	epicsFloat64	d53hr;	/* High Oper Range */
	epicsFloat64	d53lr;	/* Low  Oper Range */
	float *          d53da;	/* Data Array */
	float *          d53ca;	/* Current Data Array */
	epicsFloat32	d53cv;	/* Current Value */
	epicsFloat32	d53lv;	/* Last Value Posted */
	epicsUInt32	d53ne;	/* # of Elements/Pt */
	char		d53eu[16];	/* Engineering Units */
	epicsInt16	d53pr;	/* Display Precision */
	epicsFloat64	d54hr;	/* High Oper Range */
	epicsFloat64	d54lr;	/* Low  Oper Range */
	float *          d54da;	/* Data Array */
	float *          d54ca;	/* Current Data Array */
	epicsFloat32	d54cv;	/* Current Value */
	epicsFloat32	d54lv;	/* Last Value Posted */
	epicsUInt32	d54ne;	/* # of Elements/Pt */
	char		d54eu[16];	/* Engineering Units */
	epicsInt16	d54pr;	/* Display Precision */
	epicsFloat64	d55hr;	/* High Oper Range */
	epicsFloat64	d55lr;	/* Low  Oper Range */
	float *          d55da;	/* Data Array */
	float *          d55ca;	/* Current Data Array */
	epicsFloat32	d55cv;	/* Current Value */
	epicsFloat32	d55lv;	/* Last Value Posted */
	epicsUInt32	d55ne;	/* # of Elements/Pt */
	char		d55eu[16];	/* Engineering Units */
	epicsInt16	d55pr;	/* Display Precision */
	epicsFloat64	d56hr;	/* High Oper Range */
	epicsFloat64	d56lr;	/* Low  Oper Range */
	float *          d56da;	/* Data Array */
	float *          d56ca;	/* Current Data Array */
	epicsFloat32	d56cv;	/* Current Value */
	epicsFloat32	d56lv;	/* Last Value Posted */
	epicsUInt32	d56ne;	/* # of Elements/Pt */
	char		d56eu[16];	/* Engineering Units */
	epicsInt16	d56pr;	/* Display Precision */
	epicsFloat64	d57hr;	/* High Oper Range */
	epicsFloat64	d57lr;	/* Low  Oper Range */
	float *          d57da;	/* Data Array */
	float *          d57ca;	/* Current Data Array */
	epicsFloat32	d57cv;	/* Current Value */
	epicsFloat32	d57lv;	/* Last Value Posted */
	epicsUInt32	d57ne;	/* # of Elements/Pt */
	char		d57eu[16];	/* Engineering Units */
	epicsInt16	d57pr;	/* Display Precision */
	epicsFloat64	d58hr;	/* High Oper Range */
	epicsFloat64	d58lr;	/* Low  Oper Range */
	float *          d58da;	/* Data Array */
	float *          d58ca;	/* Current Data Array */
	epicsFloat32	d58cv;	/* Current Value */
	epicsFloat32	d58lv;	/* Last Value Posted */
	epicsUInt32	d58ne;	/* # of Elements/Pt */
	char		d58eu[16];	/* Engineering Units */
	epicsInt16	d58pr;	/* Display Precision */
	epicsFloat64	d59hr;	/* High Oper Range */
	epicsFloat64	d59lr;	/* Low  Oper Range */
	float *          d59da;	/* Data Array */
	float *          d59ca;	/* Current Data Array */
	epicsFloat32	d59cv;	/* Current Value */
	epicsFloat32	d59lv;	/* Last Value Posted */
	epicsUInt32	d59ne;	/* # of Elements/Pt */
	char		d59eu[16];	/* Engineering Units */
	epicsInt16	d59pr;	/* Display Precision */
	epicsFloat64	d60hr;	/* High Oper Range */
	epicsFloat64	d60lr;	/* Low  Oper Range */
	float *          d60da;	/* Data Array */
	float *          d60ca;	/* Current Data Array */
	epicsFloat32	d60cv;	/* Current Value */
	epicsFloat32	d60lv;	/* Last Value Posted */
	epicsUInt32	d60ne;	/* # of Elements/Pt */
	char		d60eu[16];	/* Engineering Units */
	epicsInt16	d60pr;	/* Display Precision */
	epicsFloat64	d61hr;	/* High Oper Range */
	epicsFloat64	d61lr;	/* Low  Oper Range */
	float *          d61da;	/* Data Array */
	float *          d61ca;	/* Current Data Array */
	epicsFloat32	d61cv;	/* Current Value */
	epicsFloat32	d61lv;	/* Last Value Posted */
	epicsUInt32	d61ne;	/* # of Elements/Pt */
	char		d61eu[16];	/* Engineering Units */
	epicsInt16	d61pr;	/* Display Precision */
	epicsFloat64	d62hr;	/* High Oper Range */
	epicsFloat64	d62lr;	/* Low  Oper Range */
	float *          d62da;	/* Data Array */
	float *          d62ca;	/* Current Data Array */
	epicsFloat32	d62cv;	/* Current Value */
	epicsFloat32	d62lv;	/* Last Value Posted */
	epicsUInt32	d62ne;	/* # of Elements/Pt */
	char		d62eu[16];	/* Engineering Units */
	epicsInt16	d62pr;	/* Display Precision */
	epicsFloat64	d63hr;	/* High Oper Range */
	epicsFloat64	d63lr;	/* Low  Oper Range */
	float *          d63da;	/* Data Array */
	float *          d63ca;	/* Current Data Array */
	epicsFloat32	d63cv;	/* Current Value */
	epicsFloat32	d63lv;	/* Last Value Posted */
	epicsUInt32	d63ne;	/* # of Elements/Pt */
	char		d63eu[16];	/* Engineering Units */
	epicsInt16	d63pr;	/* Display Precision */
	epicsFloat64	d64hr;	/* High Oper Range */
	epicsFloat64	d64lr;	/* Low  Oper Range */
	float *          d64da;	/* Data Array */
	float *          d64ca;	/* Current Data Array */
	epicsFloat32	d64cv;	/* Current Value */
	epicsFloat32	d64lv;	/* Last Value Posted */
	epicsUInt32	d64ne;	/* # of Elements/Pt */
	char		d64eu[16];	/* Engineering Units */
	epicsInt16	d64pr;	/* Display Precision */
	epicsFloat64	d65hr;	/* High Oper Range */
	epicsFloat64	d65lr;	/* Low  Oper Range */
	float *          d65da;	/* Data Array */
	float *          d65ca;	/* Current Data Array */
	epicsFloat32	d65cv;	/* Current Value */
	epicsFloat32	d65lv;	/* Last Value Posted */
	epicsUInt32	d65ne;	/* # of Elements/Pt */
	char		d65eu[16];	/* Engineering Units */
	epicsInt16	d65pr;	/* Display Precision */
	epicsFloat64	d66hr;	/* High Oper Range */
	epicsFloat64	d66lr;	/* Low  Oper Range */
	float *          d66da;	/* Data Array */
	float *          d66ca;	/* Current Data Array */
	epicsFloat32	d66cv;	/* Current Value */
	epicsFloat32	d66lv;	/* Last Value Posted */
	epicsUInt32	d66ne;	/* # of Elements/Pt */
	char		d66eu[16];	/* Engineering Units */
	epicsInt16	d66pr;	/* Display Precision */
	epicsFloat64	d67hr;	/* High Oper Range */
	epicsFloat64	d67lr;	/* Low  Oper Range */
	float *          d67da;	/* Data Array */
	float *          d67ca;	/* Current Data Array */
	epicsFloat32	d67cv;	/* Current Value */
	epicsFloat32	d67lv;	/* Last Value Posted */
	epicsUInt32	d67ne;	/* # of Elements/Pt */
	char		d67eu[16];	/* Engineering Units */
	epicsInt16	d67pr;	/* Display Precision */
	epicsFloat64	d68hr;	/* High Oper Range */
	epicsFloat64	d68lr;	/* Low  Oper Range */
	float *          d68da;	/* Data Array */
	float *          d68ca;	/* Current Data Array */
	epicsFloat32	d68cv;	/* Current Value */
	epicsFloat32	d68lv;	/* Last Value Posted */
	epicsUInt32	d68ne;	/* # of Elements/Pt */
	char		d68eu[16];	/* Engineering Units */
	epicsInt16	d68pr;	/* Display Precision */
	epicsFloat64	d69hr;	/* High Oper Range */
	epicsFloat64	d69lr;	/* Low  Oper Range */
	float *          d69da;	/* Data Array */
	float *          d69ca;	/* Current Data Array */
	epicsFloat32	d69cv;	/* Current Value */
	epicsFloat32	d69lv;	/* Last Value Posted */
	epicsUInt32	d69ne;	/* # of Elements/Pt */
	char		d69eu[16];	/* Engineering Units */
	epicsInt16	d69pr;	/* Display Precision */
	epicsFloat64	d70hr;	/* High Oper Range */
	epicsFloat64	d70lr;	/* Low  Oper Range */
	float *          d70da;	/* Data Array */
	float *          d70ca;	/* Current Data Array */
	epicsFloat32	d70cv;	/* Current Value */
	epicsFloat32	d70lv;	/* Last Value Posted */
	epicsUInt32	d70ne;	/* # of Elements/Pt */
	char		d70eu[16];	/* Engineering Units */
	epicsInt16	d70pr;	/* Display Precision */
	epicsFloat32	t1cd;	/* T1 Cmnd */
	epicsFloat32	t2cd;	/* T2 Cmnd */
	epicsFloat32	t3cd;	/* T3 Cmnd */
	epicsFloat32	t4cd;	/* T4 Cmnd */
	epicsFloat32	a1cd;	/* A1 Cmnd */
	epicsFloat32	bscd;	/* Before Scan Cmnd */
	epicsFloat32	ascd;	/* After Scan Cmnd */
	epicsEnum16	paus;	/* Go/Pause control */
	epicsEnum16	lpau;	/* Last value of Go/Pause */
	epicsFloat32	pdly;	/* Positioner-settling delay */
	epicsFloat32	ddly;	/* Detector-settling delay */
	epicsFloat32	rdly;	/* Pause resume delay */
	epicsEnum16	faze;	/* Scan phase */
	epicsEnum16	acqm;	/* Acquisition mode */
	epicsEnum16	acqt;	/* Acquisition type */
	epicsEnum16	dstate;	/* Data state */
	epicsInt32	copyto;	/* Copy Last Pt Thru */
} sscanRecord;
#define sscanRecordNAME	0
#define sscanRecordDESC	1
#define sscanRecordASG	2
#define sscanRecordSCAN	3
#define sscanRecordPINI	4
#define sscanRecordPHAS	5
#define sscanRecordEVNT	6
#define sscanRecordTSE	7
#define sscanRecordTSEL	8
#define sscanRecordDTYP	9
#define sscanRecordDISV	10
#define sscanRecordDISA	11
#define sscanRecordSDIS	12
#define sscanRecordMLOK	13
#define sscanRecordMLIS	14
#define sscanRecordDISP	15
#define sscanRecordPROC	16
#define sscanRecordSTAT	17
#define sscanRecordSEVR	18
#define sscanRecordNSTA	19
#define sscanRecordNSEV	20
#define sscanRecordACKS	21
#define sscanRecordACKT	22
#define sscanRecordDISS	23
#define sscanRecordLCNT	24
#define sscanRecordPACT	25
#define sscanRecordPUTF	26
#define sscanRecordRPRO	27
#define sscanRecordASP	28
#define sscanRecordPPN	29
#define sscanRecordPPNR	30
#define sscanRecordSPVT	31
#define sscanRecordRSET	32
#define sscanRecordDSET	33
#define sscanRecordDPVT	34
#define sscanRecordRDES	35
#define sscanRecordLSET	36
#define sscanRecordPRIO	37
#define sscanRecordTPRO	38
#define sscanRecordBKPT	39
#define sscanRecordUDF	40
#define sscanRecordTIME	41
#define sscanRecordFLNK	42
#define sscanRecordVERS	43
#define sscanRecordVAL	44
#define sscanRecordSMSG	45
#define sscanRecordCMND	46
#define sscanRecordALRT	47
#define sscanRecordRPVT	48
#define sscanRecordMPTS	49
#define sscanRecordEXSC	50
#define sscanRecordXSC	51
#define sscanRecordPXSC	52
#define sscanRecordBUSY	53
#define sscanRecordKILL	54
#define sscanRecordWAIT	55
#define sscanRecordWCNT	56
#define sscanRecordAWCT	57
#define sscanRecordWTNG	58
#define sscanRecordAWAIT	59
#define sscanRecordAAWAIT	60
#define sscanRecordDATA	61
#define sscanRecordREFD	62
#define sscanRecordNPTS	63
#define sscanRecordFPTS	64
#define sscanRecordFFO	65
#define sscanRecordCPT	66
#define sscanRecordBCPT	67
#define sscanRecordDPT	68
#define sscanRecordPCPT	69
#define sscanRecordPASM	70
#define sscanRecordTOLP	71
#define sscanRecordTLAP	72
#define sscanRecordATIME	73
#define sscanRecordP1PV	74
#define sscanRecordP2PV	75
#define sscanRecordP3PV	76
#define sscanRecordP4PV	77
#define sscanRecordR1PV	78
#define sscanRecordR2PV	79
#define sscanRecordR3PV	80
#define sscanRecordR4PV	81
#define sscanRecordD01PV	82
#define sscanRecordD02PV	83
#define sscanRecordD03PV	84
#define sscanRecordD04PV	85
#define sscanRecordD05PV	86
#define sscanRecordD06PV	87
#define sscanRecordD07PV	88
#define sscanRecordD08PV	89
#define sscanRecordD09PV	90
#define sscanRecordD10PV	91
#define sscanRecordD11PV	92
#define sscanRecordD12PV	93
#define sscanRecordD13PV	94
#define sscanRecordD14PV	95
#define sscanRecordD15PV	96
#define sscanRecordD16PV	97
#define sscanRecordD17PV	98
#define sscanRecordD18PV	99
#define sscanRecordD19PV	100
#define sscanRecordD20PV	101
#define sscanRecordD21PV	102
#define sscanRecordD22PV	103
#define sscanRecordD23PV	104
#define sscanRecordD24PV	105
#define sscanRecordD25PV	106
#define sscanRecordD26PV	107
#define sscanRecordD27PV	108
#define sscanRecordD28PV	109
#define sscanRecordD29PV	110
#define sscanRecordD30PV	111
#define sscanRecordD31PV	112
#define sscanRecordD32PV	113
#define sscanRecordD33PV	114
#define sscanRecordD34PV	115
#define sscanRecordD35PV	116
#define sscanRecordD36PV	117
#define sscanRecordD37PV	118
#define sscanRecordD38PV	119
#define sscanRecordD39PV	120
#define sscanRecordD40PV	121
#define sscanRecordD41PV	122
#define sscanRecordD42PV	123
#define sscanRecordD43PV	124
#define sscanRecordD44PV	125
#define sscanRecordD45PV	126
#define sscanRecordD46PV	127
#define sscanRecordD47PV	128
#define sscanRecordD48PV	129
#define sscanRecordD49PV	130
#define sscanRecordD50PV	131
#define sscanRecordD51PV	132
#define sscanRecordD52PV	133
#define sscanRecordD53PV	134
#define sscanRecordD54PV	135
#define sscanRecordD55PV	136
#define sscanRecordD56PV	137
#define sscanRecordD57PV	138
#define sscanRecordD58PV	139
#define sscanRecordD59PV	140
#define sscanRecordD60PV	141
#define sscanRecordD61PV	142
#define sscanRecordD62PV	143
#define sscanRecordD63PV	144
#define sscanRecordD64PV	145
#define sscanRecordD65PV	146
#define sscanRecordD66PV	147
#define sscanRecordD67PV	148
#define sscanRecordD68PV	149
#define sscanRecordD69PV	150
#define sscanRecordD70PV	151
#define sscanRecordT1PV	152
#define sscanRecordT2PV	153
#define sscanRecordT3PV	154
#define sscanRecordT4PV	155
#define sscanRecordA1PV	156
#define sscanRecordBSPV	157
#define sscanRecordASPV	158
#define sscanRecordBSWAIT	159
#define sscanRecordASWAIT	160
#define sscanRecordP1NV	161
#define sscanRecordP2NV	162
#define sscanRecordP3NV	163
#define sscanRecordP4NV	164
#define sscanRecordR1NV	165
#define sscanRecordR2NV	166
#define sscanRecordR3NV	167
#define sscanRecordR4NV	168
#define sscanRecordD01NV	169
#define sscanRecordD02NV	170
#define sscanRecordD03NV	171
#define sscanRecordD04NV	172
#define sscanRecordD05NV	173
#define sscanRecordD06NV	174
#define sscanRecordD07NV	175
#define sscanRecordD08NV	176
#define sscanRecordD09NV	177
#define sscanRecordD10NV	178
#define sscanRecordD11NV	179
#define sscanRecordD12NV	180
#define sscanRecordD13NV	181
#define sscanRecordD14NV	182
#define sscanRecordD15NV	183
#define sscanRecordD16NV	184
#define sscanRecordD17NV	185
#define sscanRecordD18NV	186
#define sscanRecordD19NV	187
#define sscanRecordD20NV	188
#define sscanRecordD21NV	189
#define sscanRecordD22NV	190
#define sscanRecordD23NV	191
#define sscanRecordD24NV	192
#define sscanRecordD25NV	193
#define sscanRecordD26NV	194
#define sscanRecordD27NV	195
#define sscanRecordD28NV	196
#define sscanRecordD29NV	197
#define sscanRecordD30NV	198
#define sscanRecordD31NV	199
#define sscanRecordD32NV	200
#define sscanRecordD33NV	201
#define sscanRecordD34NV	202
#define sscanRecordD35NV	203
#define sscanRecordD36NV	204
#define sscanRecordD37NV	205
#define sscanRecordD38NV	206
#define sscanRecordD39NV	207
#define sscanRecordD40NV	208
#define sscanRecordD41NV	209
#define sscanRecordD42NV	210
#define sscanRecordD43NV	211
#define sscanRecordD44NV	212
#define sscanRecordD45NV	213
#define sscanRecordD46NV	214
#define sscanRecordD47NV	215
#define sscanRecordD48NV	216
#define sscanRecordD49NV	217
#define sscanRecordD50NV	218
#define sscanRecordD51NV	219
#define sscanRecordD52NV	220
#define sscanRecordD53NV	221
#define sscanRecordD54NV	222
#define sscanRecordD55NV	223
#define sscanRecordD56NV	224
#define sscanRecordD57NV	225
#define sscanRecordD58NV	226
#define sscanRecordD59NV	227
#define sscanRecordD60NV	228
#define sscanRecordD61NV	229
#define sscanRecordD62NV	230
#define sscanRecordD63NV	231
#define sscanRecordD64NV	232
#define sscanRecordD65NV	233
#define sscanRecordD66NV	234
#define sscanRecordD67NV	235
#define sscanRecordD68NV	236
#define sscanRecordD69NV	237
#define sscanRecordD70NV	238
#define sscanRecordT1NV	239
#define sscanRecordT2NV	240
#define sscanRecordT3NV	241
#define sscanRecordT4NV	242
#define sscanRecordA1NV	243
#define sscanRecordBSNV	244
#define sscanRecordASNV	245
#define sscanRecordP1PP	246
#define sscanRecordP1CV	247
#define sscanRecordP1DV	248
#define sscanRecordP1LV	249
#define sscanRecordP1SP	250
#define sscanRecordP1SI	251
#define sscanRecordP1EP	252
#define sscanRecordP1CP	253
#define sscanRecordP1WD	254
#define sscanRecordR1CV	255
#define sscanRecordR1LV	256
#define sscanRecordR1DL	257
#define sscanRecordP1HR	258
#define sscanRecordP1LR	259
#define sscanRecordP1PA	260
#define sscanRecordP1RA	261
#define sscanRecordP1CA	262
#define sscanRecordP1FS	263
#define sscanRecordP1FI	264
#define sscanRecordP1FE	265
#define sscanRecordP1FC	266
#define sscanRecordP1FW	267
#define sscanRecordP1SM	268
#define sscanRecordP1AR	269
#define sscanRecordP1EU	270
#define sscanRecordP1PR	271
#define sscanRecordP2PP	272
#define sscanRecordP2CV	273
#define sscanRecordP2DV	274
#define sscanRecordP2LV	275
#define sscanRecordP2SP	276
#define sscanRecordP2SI	277
#define sscanRecordP2EP	278
#define sscanRecordP2CP	279
#define sscanRecordP2WD	280
#define sscanRecordR2CV	281
#define sscanRecordR2LV	282
#define sscanRecordR2DL	283
#define sscanRecordP2HR	284
#define sscanRecordP2LR	285
#define sscanRecordP2PA	286
#define sscanRecordP2RA	287
#define sscanRecordP2CA	288
#define sscanRecordP2FS	289
#define sscanRecordP2FI	290
#define sscanRecordP2FE	291
#define sscanRecordP2FC	292
#define sscanRecordP2FW	293
#define sscanRecordP2SM	294
#define sscanRecordP2AR	295
#define sscanRecordP2EU	296
#define sscanRecordP2PR	297
#define sscanRecordP3PP	298
#define sscanRecordP3CV	299
#define sscanRecordP3DV	300
#define sscanRecordP3LV	301
#define sscanRecordP3SP	302
#define sscanRecordP3SI	303
#define sscanRecordP3EP	304
#define sscanRecordP3CP	305
#define sscanRecordP3WD	306
#define sscanRecordR3CV	307
#define sscanRecordR3LV	308
#define sscanRecordR3DL	309
#define sscanRecordP3HR	310
#define sscanRecordP3LR	311
#define sscanRecordP3PA	312
#define sscanRecordP3RA	313
#define sscanRecordP3CA	314
#define sscanRecordP3FS	315
#define sscanRecordP3FI	316
#define sscanRecordP3FE	317
#define sscanRecordP3FC	318
#define sscanRecordP3FW	319
#define sscanRecordP3SM	320
#define sscanRecordP3AR	321
#define sscanRecordP3EU	322
#define sscanRecordP3PR	323
#define sscanRecordP4PP	324
#define sscanRecordP4CV	325
#define sscanRecordP4DV	326
#define sscanRecordP4LV	327
#define sscanRecordP4SP	328
#define sscanRecordP4SI	329
#define sscanRecordP4EP	330
#define sscanRecordP4CP	331
#define sscanRecordP4WD	332
#define sscanRecordR4CV	333
#define sscanRecordR4LV	334
#define sscanRecordR4DL	335
#define sscanRecordP4HR	336
#define sscanRecordP4LR	337
#define sscanRecordP4PA	338
#define sscanRecordP4RA	339
#define sscanRecordP4CA	340
#define sscanRecordP4FS	341
#define sscanRecordP4FI	342
#define sscanRecordP4FE	343
#define sscanRecordP4FC	344
#define sscanRecordP4FW	345
#define sscanRecordP4SM	346
#define sscanRecordP4AR	347
#define sscanRecordP4EU	348
#define sscanRecordP4PR	349
#define sscanRecordD01HR	350
#define sscanRecordD01LR	351
#define sscanRecordD01DA	352
#define sscanRecordD01CA	353
#define sscanRecordD01CV	354
#define sscanRecordD01LV	355
#define sscanRecordD01NE	356
#define sscanRecordD01EU	357
#define sscanRecordD01PR	358
#define sscanRecordD02HR	359
#define sscanRecordD02LR	360
#define sscanRecordD02DA	361
#define sscanRecordD02CA	362
#define sscanRecordD02CV	363
#define sscanRecordD02LV	364
#define sscanRecordD02NE	365
#define sscanRecordD02EU	366
#define sscanRecordD02PR	367
#define sscanRecordD03HR	368
#define sscanRecordD03LR	369
#define sscanRecordD03DA	370
#define sscanRecordD03CA	371
#define sscanRecordD03CV	372
#define sscanRecordD03LV	373
#define sscanRecordD03NE	374
#define sscanRecordD03EU	375
#define sscanRecordD03PR	376
#define sscanRecordD04HR	377
#define sscanRecordD04LR	378
#define sscanRecordD04DA	379
#define sscanRecordD04CA	380
#define sscanRecordD04CV	381
#define sscanRecordD04LV	382
#define sscanRecordD04NE	383
#define sscanRecordD04EU	384
#define sscanRecordD04PR	385
#define sscanRecordD05HR	386
#define sscanRecordD05LR	387
#define sscanRecordD05DA	388
#define sscanRecordD05CA	389
#define sscanRecordD05CV	390
#define sscanRecordD05LV	391
#define sscanRecordD05NE	392
#define sscanRecordD05EU	393
#define sscanRecordD05PR	394
#define sscanRecordD06HR	395
#define sscanRecordD06LR	396
#define sscanRecordD06DA	397
#define sscanRecordD06CA	398
#define sscanRecordD06CV	399
#define sscanRecordD06LV	400
#define sscanRecordD06NE	401
#define sscanRecordD06EU	402
#define sscanRecordD06PR	403
#define sscanRecordD07HR	404
#define sscanRecordD07LR	405
#define sscanRecordD07DA	406
#define sscanRecordD07CA	407
#define sscanRecordD07CV	408
#define sscanRecordD07LV	409
#define sscanRecordD07NE	410
#define sscanRecordD07EU	411
#define sscanRecordD07PR	412
#define sscanRecordD08HR	413
#define sscanRecordD08LR	414
#define sscanRecordD08DA	415
#define sscanRecordD08CA	416
#define sscanRecordD08CV	417
#define sscanRecordD08LV	418
#define sscanRecordD08NE	419
#define sscanRecordD08EU	420
#define sscanRecordD08PR	421
#define sscanRecordD09HR	422
#define sscanRecordD09LR	423
#define sscanRecordD09DA	424
#define sscanRecordD09CA	425
#define sscanRecordD09CV	426
#define sscanRecordD09LV	427
#define sscanRecordD09NE	428
#define sscanRecordD09EU	429
#define sscanRecordD09PR	430
#define sscanRecordD10HR	431
#define sscanRecordD10LR	432
#define sscanRecordD10DA	433
#define sscanRecordD10CA	434
#define sscanRecordD10CV	435
#define sscanRecordD10LV	436
#define sscanRecordD10NE	437
#define sscanRecordD10EU	438
#define sscanRecordD10PR	439
#define sscanRecordD11HR	440
#define sscanRecordD11LR	441
#define sscanRecordD11DA	442
#define sscanRecordD11CA	443
#define sscanRecordD11CV	444
#define sscanRecordD11LV	445
#define sscanRecordD11NE	446
#define sscanRecordD11EU	447
#define sscanRecordD11PR	448
#define sscanRecordD12HR	449
#define sscanRecordD12LR	450
#define sscanRecordD12DA	451
#define sscanRecordD12CA	452
#define sscanRecordD12CV	453
#define sscanRecordD12LV	454
#define sscanRecordD12NE	455
#define sscanRecordD12EU	456
#define sscanRecordD12PR	457
#define sscanRecordD13HR	458
#define sscanRecordD13LR	459
#define sscanRecordD13DA	460
#define sscanRecordD13CA	461
#define sscanRecordD13CV	462
#define sscanRecordD13LV	463
#define sscanRecordD13NE	464
#define sscanRecordD13EU	465
#define sscanRecordD13PR	466
#define sscanRecordD14HR	467
#define sscanRecordD14LR	468
#define sscanRecordD14DA	469
#define sscanRecordD14CA	470
#define sscanRecordD14CV	471
#define sscanRecordD14LV	472
#define sscanRecordD14NE	473
#define sscanRecordD14EU	474
#define sscanRecordD14PR	475
#define sscanRecordD15HR	476
#define sscanRecordD15LR	477
#define sscanRecordD15DA	478
#define sscanRecordD15CA	479
#define sscanRecordD15CV	480
#define sscanRecordD15LV	481
#define sscanRecordD15NE	482
#define sscanRecordD15EU	483
#define sscanRecordD15PR	484
#define sscanRecordD16HR	485
#define sscanRecordD16LR	486
#define sscanRecordD16DA	487
#define sscanRecordD16CA	488
#define sscanRecordD16CV	489
#define sscanRecordD16LV	490
#define sscanRecordD16NE	491
#define sscanRecordD16EU	492
#define sscanRecordD16PR	493
#define sscanRecordD17HR	494
#define sscanRecordD17LR	495
#define sscanRecordD17DA	496
#define sscanRecordD17CA	497
#define sscanRecordD17CV	498
#define sscanRecordD17LV	499
#define sscanRecordD17NE	500
#define sscanRecordD17EU	501
#define sscanRecordD17PR	502
#define sscanRecordD18HR	503
#define sscanRecordD18LR	504
#define sscanRecordD18DA	505
#define sscanRecordD18CA	506
#define sscanRecordD18CV	507
#define sscanRecordD18LV	508
#define sscanRecordD18NE	509
#define sscanRecordD18EU	510
#define sscanRecordD18PR	511
#define sscanRecordD19HR	512
#define sscanRecordD19LR	513
#define sscanRecordD19DA	514
#define sscanRecordD19CA	515
#define sscanRecordD19CV	516
#define sscanRecordD19LV	517
#define sscanRecordD19NE	518
#define sscanRecordD19EU	519
#define sscanRecordD19PR	520
#define sscanRecordD20HR	521
#define sscanRecordD20LR	522
#define sscanRecordD20DA	523
#define sscanRecordD20CA	524
#define sscanRecordD20CV	525
#define sscanRecordD20LV	526
#define sscanRecordD20NE	527
#define sscanRecordD20EU	528
#define sscanRecordD20PR	529
#define sscanRecordD21HR	530
#define sscanRecordD21LR	531
#define sscanRecordD21DA	532
#define sscanRecordD21CA	533
#define sscanRecordD21CV	534
#define sscanRecordD21LV	535
#define sscanRecordD21NE	536
#define sscanRecordD21EU	537
#define sscanRecordD21PR	538
#define sscanRecordD22HR	539
#define sscanRecordD22LR	540
#define sscanRecordD22DA	541
#define sscanRecordD22CA	542
#define sscanRecordD22CV	543
#define sscanRecordD22LV	544
#define sscanRecordD22NE	545
#define sscanRecordD22EU	546
#define sscanRecordD22PR	547
#define sscanRecordD23HR	548
#define sscanRecordD23LR	549
#define sscanRecordD23DA	550
#define sscanRecordD23CA	551
#define sscanRecordD23CV	552
#define sscanRecordD23LV	553
#define sscanRecordD23NE	554
#define sscanRecordD23EU	555
#define sscanRecordD23PR	556
#define sscanRecordD24HR	557
#define sscanRecordD24LR	558
#define sscanRecordD24DA	559
#define sscanRecordD24CA	560
#define sscanRecordD24CV	561
#define sscanRecordD24LV	562
#define sscanRecordD24NE	563
#define sscanRecordD24EU	564
#define sscanRecordD24PR	565
#define sscanRecordD25HR	566
#define sscanRecordD25LR	567
#define sscanRecordD25DA	568
#define sscanRecordD25CA	569
#define sscanRecordD25CV	570
#define sscanRecordD25LV	571
#define sscanRecordD25NE	572
#define sscanRecordD25EU	573
#define sscanRecordD25PR	574
#define sscanRecordD26HR	575
#define sscanRecordD26LR	576
#define sscanRecordD26DA	577
#define sscanRecordD26CA	578
#define sscanRecordD26CV	579
#define sscanRecordD26LV	580
#define sscanRecordD26NE	581
#define sscanRecordD26EU	582
#define sscanRecordD26PR	583
#define sscanRecordD27HR	584
#define sscanRecordD27LR	585
#define sscanRecordD27DA	586
#define sscanRecordD27CA	587
#define sscanRecordD27CV	588
#define sscanRecordD27LV	589
#define sscanRecordD27NE	590
#define sscanRecordD27EU	591
#define sscanRecordD27PR	592
#define sscanRecordD28HR	593
#define sscanRecordD28LR	594
#define sscanRecordD28DA	595
#define sscanRecordD28CA	596
#define sscanRecordD28CV	597
#define sscanRecordD28LV	598
#define sscanRecordD28NE	599
#define sscanRecordD28EU	600
#define sscanRecordD28PR	601
#define sscanRecordD29HR	602
#define sscanRecordD29LR	603
#define sscanRecordD29DA	604
#define sscanRecordD29CA	605
#define sscanRecordD29CV	606
#define sscanRecordD29LV	607
#define sscanRecordD29NE	608
#define sscanRecordD29EU	609
#define sscanRecordD29PR	610
#define sscanRecordD30HR	611
#define sscanRecordD30LR	612
#define sscanRecordD30DA	613
#define sscanRecordD30CA	614
#define sscanRecordD30CV	615
#define sscanRecordD30LV	616
#define sscanRecordD30NE	617
#define sscanRecordD30EU	618
#define sscanRecordD30PR	619
#define sscanRecordD31HR	620
#define sscanRecordD31LR	621
#define sscanRecordD31DA	622
#define sscanRecordD31CA	623
#define sscanRecordD31CV	624
#define sscanRecordD31LV	625
#define sscanRecordD31NE	626
#define sscanRecordD31EU	627
#define sscanRecordD31PR	628
#define sscanRecordD32HR	629
#define sscanRecordD32LR	630
#define sscanRecordD32DA	631
#define sscanRecordD32CA	632
#define sscanRecordD32CV	633
#define sscanRecordD32LV	634
#define sscanRecordD32NE	635
#define sscanRecordD32EU	636
#define sscanRecordD32PR	637
#define sscanRecordD33HR	638
#define sscanRecordD33LR	639
#define sscanRecordD33DA	640
#define sscanRecordD33CA	641
#define sscanRecordD33CV	642
#define sscanRecordD33LV	643
#define sscanRecordD33NE	644
#define sscanRecordD33EU	645
#define sscanRecordD33PR	646
#define sscanRecordD34HR	647
#define sscanRecordD34LR	648
#define sscanRecordD34DA	649
#define sscanRecordD34CA	650
#define sscanRecordD34CV	651
#define sscanRecordD34LV	652
#define sscanRecordD34NE	653
#define sscanRecordD34EU	654
#define sscanRecordD34PR	655
#define sscanRecordD35HR	656
#define sscanRecordD35LR	657
#define sscanRecordD35DA	658
#define sscanRecordD35CA	659
#define sscanRecordD35CV	660
#define sscanRecordD35LV	661
#define sscanRecordD35NE	662
#define sscanRecordD35EU	663
#define sscanRecordD35PR	664
#define sscanRecordD36HR	665
#define sscanRecordD36LR	666
#define sscanRecordD36DA	667
#define sscanRecordD36CA	668
#define sscanRecordD36CV	669
#define sscanRecordD36LV	670
#define sscanRecordD36NE	671
#define sscanRecordD36EU	672
#define sscanRecordD36PR	673
#define sscanRecordD37HR	674
#define sscanRecordD37LR	675
#define sscanRecordD37DA	676
#define sscanRecordD37CA	677
#define sscanRecordD37CV	678
#define sscanRecordD37LV	679
#define sscanRecordD37NE	680
#define sscanRecordD37EU	681
#define sscanRecordD37PR	682
#define sscanRecordD38HR	683
#define sscanRecordD38LR	684
#define sscanRecordD38DA	685
#define sscanRecordD38CA	686
#define sscanRecordD38CV	687
#define sscanRecordD38LV	688
#define sscanRecordD38NE	689
#define sscanRecordD38EU	690
#define sscanRecordD38PR	691
#define sscanRecordD39HR	692
#define sscanRecordD39LR	693
#define sscanRecordD39DA	694
#define sscanRecordD39CA	695
#define sscanRecordD39CV	696
#define sscanRecordD39LV	697
#define sscanRecordD39NE	698
#define sscanRecordD39EU	699
#define sscanRecordD39PR	700
#define sscanRecordD40HR	701
#define sscanRecordD40LR	702
#define sscanRecordD40DA	703
#define sscanRecordD40CA	704
#define sscanRecordD40CV	705
#define sscanRecordD40LV	706
#define sscanRecordD40NE	707
#define sscanRecordD40EU	708
#define sscanRecordD40PR	709
#define sscanRecordD41HR	710
#define sscanRecordD41LR	711
#define sscanRecordD41DA	712
#define sscanRecordD41CA	713
#define sscanRecordD41CV	714
#define sscanRecordD41LV	715
#define sscanRecordD41NE	716
#define sscanRecordD41EU	717
#define sscanRecordD41PR	718
#define sscanRecordD42HR	719
#define sscanRecordD42LR	720
#define sscanRecordD42DA	721
#define sscanRecordD42CA	722
#define sscanRecordD42CV	723
#define sscanRecordD42LV	724
#define sscanRecordD42NE	725
#define sscanRecordD42EU	726
#define sscanRecordD42PR	727
#define sscanRecordD43HR	728
#define sscanRecordD43LR	729
#define sscanRecordD43DA	730
#define sscanRecordD43CA	731
#define sscanRecordD43CV	732
#define sscanRecordD43LV	733
#define sscanRecordD43NE	734
#define sscanRecordD43EU	735
#define sscanRecordD43PR	736
#define sscanRecordD44HR	737
#define sscanRecordD44LR	738
#define sscanRecordD44DA	739
#define sscanRecordD44CA	740
#define sscanRecordD44CV	741
#define sscanRecordD44LV	742
#define sscanRecordD44NE	743
#define sscanRecordD44EU	744
#define sscanRecordD44PR	745
#define sscanRecordD45HR	746
#define sscanRecordD45LR	747
#define sscanRecordD45DA	748
#define sscanRecordD45CA	749
#define sscanRecordD45CV	750
#define sscanRecordD45LV	751
#define sscanRecordD45NE	752
#define sscanRecordD45EU	753
#define sscanRecordD45PR	754
#define sscanRecordD46HR	755
#define sscanRecordD46LR	756
#define sscanRecordD46DA	757
#define sscanRecordD46CA	758
#define sscanRecordD46CV	759
#define sscanRecordD46LV	760
#define sscanRecordD46NE	761
#define sscanRecordD46EU	762
#define sscanRecordD46PR	763
#define sscanRecordD47HR	764
#define sscanRecordD47LR	765
#define sscanRecordD47DA	766
#define sscanRecordD47CA	767
#define sscanRecordD47CV	768
#define sscanRecordD47LV	769
#define sscanRecordD47NE	770
#define sscanRecordD47EU	771
#define sscanRecordD47PR	772
#define sscanRecordD48HR	773
#define sscanRecordD48LR	774
#define sscanRecordD48DA	775
#define sscanRecordD48CA	776
#define sscanRecordD48CV	777
#define sscanRecordD48LV	778
#define sscanRecordD48NE	779
#define sscanRecordD48EU	780
#define sscanRecordD48PR	781
#define sscanRecordD49HR	782
#define sscanRecordD49LR	783
#define sscanRecordD49DA	784
#define sscanRecordD49CA	785
#define sscanRecordD49CV	786
#define sscanRecordD49LV	787
#define sscanRecordD49NE	788
#define sscanRecordD49EU	789
#define sscanRecordD49PR	790
#define sscanRecordD50HR	791
#define sscanRecordD50LR	792
#define sscanRecordD50DA	793
#define sscanRecordD50CA	794
#define sscanRecordD50CV	795
#define sscanRecordD50LV	796
#define sscanRecordD50NE	797
#define sscanRecordD50EU	798
#define sscanRecordD50PR	799
#define sscanRecordD51HR	800
#define sscanRecordD51LR	801
#define sscanRecordD51DA	802
#define sscanRecordD51CA	803
#define sscanRecordD51CV	804
#define sscanRecordD51LV	805
#define sscanRecordD51NE	806
#define sscanRecordD51EU	807
#define sscanRecordD51PR	808
#define sscanRecordD52HR	809
#define sscanRecordD52LR	810
#define sscanRecordD52DA	811
#define sscanRecordD52CA	812
#define sscanRecordD52CV	813
#define sscanRecordD52LV	814
#define sscanRecordD52NE	815
#define sscanRecordD52EU	816
#define sscanRecordD52PR	817
#define sscanRecordD53HR	818
#define sscanRecordD53LR	819
#define sscanRecordD53DA	820
#define sscanRecordD53CA	821
#define sscanRecordD53CV	822
#define sscanRecordD53LV	823
#define sscanRecordD53NE	824
#define sscanRecordD53EU	825
#define sscanRecordD53PR	826
#define sscanRecordD54HR	827
#define sscanRecordD54LR	828
#define sscanRecordD54DA	829
#define sscanRecordD54CA	830
#define sscanRecordD54CV	831
#define sscanRecordD54LV	832
#define sscanRecordD54NE	833
#define sscanRecordD54EU	834
#define sscanRecordD54PR	835
#define sscanRecordD55HR	836
#define sscanRecordD55LR	837
#define sscanRecordD55DA	838
#define sscanRecordD55CA	839
#define sscanRecordD55CV	840
#define sscanRecordD55LV	841
#define sscanRecordD55NE	842
#define sscanRecordD55EU	843
#define sscanRecordD55PR	844
#define sscanRecordD56HR	845
#define sscanRecordD56LR	846
#define sscanRecordD56DA	847
#define sscanRecordD56CA	848
#define sscanRecordD56CV	849
#define sscanRecordD56LV	850
#define sscanRecordD56NE	851
#define sscanRecordD56EU	852
#define sscanRecordD56PR	853
#define sscanRecordD57HR	854
#define sscanRecordD57LR	855
#define sscanRecordD57DA	856
#define sscanRecordD57CA	857
#define sscanRecordD57CV	858
#define sscanRecordD57LV	859
#define sscanRecordD57NE	860
#define sscanRecordD57EU	861
#define sscanRecordD57PR	862
#define sscanRecordD58HR	863
#define sscanRecordD58LR	864
#define sscanRecordD58DA	865
#define sscanRecordD58CA	866
#define sscanRecordD58CV	867
#define sscanRecordD58LV	868
#define sscanRecordD58NE	869
#define sscanRecordD58EU	870
#define sscanRecordD58PR	871
#define sscanRecordD59HR	872
#define sscanRecordD59LR	873
#define sscanRecordD59DA	874
#define sscanRecordD59CA	875
#define sscanRecordD59CV	876
#define sscanRecordD59LV	877
#define sscanRecordD59NE	878
#define sscanRecordD59EU	879
#define sscanRecordD59PR	880
#define sscanRecordD60HR	881
#define sscanRecordD60LR	882
#define sscanRecordD60DA	883
#define sscanRecordD60CA	884
#define sscanRecordD60CV	885
#define sscanRecordD60LV	886
#define sscanRecordD60NE	887
#define sscanRecordD60EU	888
#define sscanRecordD60PR	889
#define sscanRecordD61HR	890
#define sscanRecordD61LR	891
#define sscanRecordD61DA	892
#define sscanRecordD61CA	893
#define sscanRecordD61CV	894
#define sscanRecordD61LV	895
#define sscanRecordD61NE	896
#define sscanRecordD61EU	897
#define sscanRecordD61PR	898
#define sscanRecordD62HR	899
#define sscanRecordD62LR	900
#define sscanRecordD62DA	901
#define sscanRecordD62CA	902
#define sscanRecordD62CV	903
#define sscanRecordD62LV	904
#define sscanRecordD62NE	905
#define sscanRecordD62EU	906
#define sscanRecordD62PR	907
#define sscanRecordD63HR	908
#define sscanRecordD63LR	909
#define sscanRecordD63DA	910
#define sscanRecordD63CA	911
#define sscanRecordD63CV	912
#define sscanRecordD63LV	913
#define sscanRecordD63NE	914
#define sscanRecordD63EU	915
#define sscanRecordD63PR	916
#define sscanRecordD64HR	917
#define sscanRecordD64LR	918
#define sscanRecordD64DA	919
#define sscanRecordD64CA	920
#define sscanRecordD64CV	921
#define sscanRecordD64LV	922
#define sscanRecordD64NE	923
#define sscanRecordD64EU	924
#define sscanRecordD64PR	925
#define sscanRecordD65HR	926
#define sscanRecordD65LR	927
#define sscanRecordD65DA	928
#define sscanRecordD65CA	929
#define sscanRecordD65CV	930
#define sscanRecordD65LV	931
#define sscanRecordD65NE	932
#define sscanRecordD65EU	933
#define sscanRecordD65PR	934
#define sscanRecordD66HR	935
#define sscanRecordD66LR	936
#define sscanRecordD66DA	937
#define sscanRecordD66CA	938
#define sscanRecordD66CV	939
#define sscanRecordD66LV	940
#define sscanRecordD66NE	941
#define sscanRecordD66EU	942
#define sscanRecordD66PR	943
#define sscanRecordD67HR	944
#define sscanRecordD67LR	945
#define sscanRecordD67DA	946
#define sscanRecordD67CA	947
#define sscanRecordD67CV	948
#define sscanRecordD67LV	949
#define sscanRecordD67NE	950
#define sscanRecordD67EU	951
#define sscanRecordD67PR	952
#define sscanRecordD68HR	953
#define sscanRecordD68LR	954
#define sscanRecordD68DA	955
#define sscanRecordD68CA	956
#define sscanRecordD68CV	957
#define sscanRecordD68LV	958
#define sscanRecordD68NE	959
#define sscanRecordD68EU	960
#define sscanRecordD68PR	961
#define sscanRecordD69HR	962
#define sscanRecordD69LR	963
#define sscanRecordD69DA	964
#define sscanRecordD69CA	965
#define sscanRecordD69CV	966
#define sscanRecordD69LV	967
#define sscanRecordD69NE	968
#define sscanRecordD69EU	969
#define sscanRecordD69PR	970
#define sscanRecordD70HR	971
#define sscanRecordD70LR	972
#define sscanRecordD70DA	973
#define sscanRecordD70CA	974
#define sscanRecordD70CV	975
#define sscanRecordD70LV	976
#define sscanRecordD70NE	977
#define sscanRecordD70EU	978
#define sscanRecordD70PR	979
#define sscanRecordT1CD	980
#define sscanRecordT2CD	981
#define sscanRecordT3CD	982
#define sscanRecordT4CD	983
#define sscanRecordA1CD	984
#define sscanRecordBSCD	985
#define sscanRecordASCD	986
#define sscanRecordPAUS	987
#define sscanRecordLPAU	988
#define sscanRecordPDLY	989
#define sscanRecordDDLY	990
#define sscanRecordRDLY	991
#define sscanRecordFAZE	992
#define sscanRecordACQM	993
#define sscanRecordACQT	994
#define sscanRecordDSTATE	995
#define sscanRecordCOPYTO	996
#endif /*INCsscanH*/
#ifdef GEN_SIZE_OFFSET
#ifdef __cplusplus
extern "C" {
#endif
#include <epicsExport.h>
static int sscanRecordSizeOffset(dbRecordType *pdbRecordType)
{
    sscanRecord *prec = 0;
  pdbRecordType->papFldDes[0]->size=sizeof(prec->name);
  pdbRecordType->papFldDes[0]->offset=(short)((char *)&prec->name - (char *)prec);
  pdbRecordType->papFldDes[1]->size=sizeof(prec->desc);
  pdbRecordType->papFldDes[1]->offset=(short)((char *)&prec->desc - (char *)prec);
  pdbRecordType->papFldDes[2]->size=sizeof(prec->asg);
  pdbRecordType->papFldDes[2]->offset=(short)((char *)&prec->asg - (char *)prec);
  pdbRecordType->papFldDes[3]->size=sizeof(prec->scan);
  pdbRecordType->papFldDes[3]->offset=(short)((char *)&prec->scan - (char *)prec);
  pdbRecordType->papFldDes[4]->size=sizeof(prec->pini);
  pdbRecordType->papFldDes[4]->offset=(short)((char *)&prec->pini - (char *)prec);
  pdbRecordType->papFldDes[5]->size=sizeof(prec->phas);
  pdbRecordType->papFldDes[5]->offset=(short)((char *)&prec->phas - (char *)prec);
  pdbRecordType->papFldDes[6]->size=sizeof(prec->evnt);
  pdbRecordType->papFldDes[6]->offset=(short)((char *)&prec->evnt - (char *)prec);
  pdbRecordType->papFldDes[7]->size=sizeof(prec->tse);
  pdbRecordType->papFldDes[7]->offset=(short)((char *)&prec->tse - (char *)prec);
  pdbRecordType->papFldDes[8]->size=sizeof(prec->tsel);
  pdbRecordType->papFldDes[8]->offset=(short)((char *)&prec->tsel - (char *)prec);
  pdbRecordType->papFldDes[9]->size=sizeof(prec->dtyp);
  pdbRecordType->papFldDes[9]->offset=(short)((char *)&prec->dtyp - (char *)prec);
  pdbRecordType->papFldDes[10]->size=sizeof(prec->disv);
  pdbRecordType->papFldDes[10]->offset=(short)((char *)&prec->disv - (char *)prec);
  pdbRecordType->papFldDes[11]->size=sizeof(prec->disa);
  pdbRecordType->papFldDes[11]->offset=(short)((char *)&prec->disa - (char *)prec);
  pdbRecordType->papFldDes[12]->size=sizeof(prec->sdis);
  pdbRecordType->papFldDes[12]->offset=(short)((char *)&prec->sdis - (char *)prec);
  pdbRecordType->papFldDes[13]->size=sizeof(prec->mlok);
  pdbRecordType->papFldDes[13]->offset=(short)((char *)&prec->mlok - (char *)prec);
  pdbRecordType->papFldDes[14]->size=sizeof(prec->mlis);
  pdbRecordType->papFldDes[14]->offset=(short)((char *)&prec->mlis - (char *)prec);
  pdbRecordType->papFldDes[15]->size=sizeof(prec->disp);
  pdbRecordType->papFldDes[15]->offset=(short)((char *)&prec->disp - (char *)prec);
  pdbRecordType->papFldDes[16]->size=sizeof(prec->proc);
  pdbRecordType->papFldDes[16]->offset=(short)((char *)&prec->proc - (char *)prec);
  pdbRecordType->papFldDes[17]->size=sizeof(prec->stat);
  pdbRecordType->papFldDes[17]->offset=(short)((char *)&prec->stat - (char *)prec);
  pdbRecordType->papFldDes[18]->size=sizeof(prec->sevr);
  pdbRecordType->papFldDes[18]->offset=(short)((char *)&prec->sevr - (char *)prec);
  pdbRecordType->papFldDes[19]->size=sizeof(prec->nsta);
  pdbRecordType->papFldDes[19]->offset=(short)((char *)&prec->nsta - (char *)prec);
  pdbRecordType->papFldDes[20]->size=sizeof(prec->nsev);
  pdbRecordType->papFldDes[20]->offset=(short)((char *)&prec->nsev - (char *)prec);
  pdbRecordType->papFldDes[21]->size=sizeof(prec->acks);
  pdbRecordType->papFldDes[21]->offset=(short)((char *)&prec->acks - (char *)prec);
  pdbRecordType->papFldDes[22]->size=sizeof(prec->ackt);
  pdbRecordType->papFldDes[22]->offset=(short)((char *)&prec->ackt - (char *)prec);
  pdbRecordType->papFldDes[23]->size=sizeof(prec->diss);
  pdbRecordType->papFldDes[23]->offset=(short)((char *)&prec->diss - (char *)prec);
  pdbRecordType->papFldDes[24]->size=sizeof(prec->lcnt);
  pdbRecordType->papFldDes[24]->offset=(short)((char *)&prec->lcnt - (char *)prec);
  pdbRecordType->papFldDes[25]->size=sizeof(prec->pact);
  pdbRecordType->papFldDes[25]->offset=(short)((char *)&prec->pact - (char *)prec);
  pdbRecordType->papFldDes[26]->size=sizeof(prec->putf);
  pdbRecordType->papFldDes[26]->offset=(short)((char *)&prec->putf - (char *)prec);
  pdbRecordType->papFldDes[27]->size=sizeof(prec->rpro);
  pdbRecordType->papFldDes[27]->offset=(short)((char *)&prec->rpro - (char *)prec);
  pdbRecordType->papFldDes[28]->size=sizeof(prec->asp);
  pdbRecordType->papFldDes[28]->offset=(short)((char *)&prec->asp - (char *)prec);
  pdbRecordType->papFldDes[29]->size=sizeof(prec->ppn);
  pdbRecordType->papFldDes[29]->offset=(short)((char *)&prec->ppn - (char *)prec);
  pdbRecordType->papFldDes[30]->size=sizeof(prec->ppnr);
  pdbRecordType->papFldDes[30]->offset=(short)((char *)&prec->ppnr - (char *)prec);
  pdbRecordType->papFldDes[31]->size=sizeof(prec->spvt);
  pdbRecordType->papFldDes[31]->offset=(short)((char *)&prec->spvt - (char *)prec);
  pdbRecordType->papFldDes[32]->size=sizeof(prec->rset);
  pdbRecordType->papFldDes[32]->offset=(short)((char *)&prec->rset - (char *)prec);
  pdbRecordType->papFldDes[33]->size=sizeof(prec->dset);
  pdbRecordType->papFldDes[33]->offset=(short)((char *)&prec->dset - (char *)prec);
  pdbRecordType->papFldDes[34]->size=sizeof(prec->dpvt);
  pdbRecordType->papFldDes[34]->offset=(short)((char *)&prec->dpvt - (char *)prec);
  pdbRecordType->papFldDes[35]->size=sizeof(prec->rdes);
  pdbRecordType->papFldDes[35]->offset=(short)((char *)&prec->rdes - (char *)prec);
  pdbRecordType->papFldDes[36]->size=sizeof(prec->lset);
  pdbRecordType->papFldDes[36]->offset=(short)((char *)&prec->lset - (char *)prec);
  pdbRecordType->papFldDes[37]->size=sizeof(prec->prio);
  pdbRecordType->papFldDes[37]->offset=(short)((char *)&prec->prio - (char *)prec);
  pdbRecordType->papFldDes[38]->size=sizeof(prec->tpro);
  pdbRecordType->papFldDes[38]->offset=(short)((char *)&prec->tpro - (char *)prec);
  pdbRecordType->papFldDes[39]->size=sizeof(prec->bkpt);
  pdbRecordType->papFldDes[39]->offset=(short)((char *)&prec->bkpt - (char *)prec);
  pdbRecordType->papFldDes[40]->size=sizeof(prec->udf);
  pdbRecordType->papFldDes[40]->offset=(short)((char *)&prec->udf - (char *)prec);
  pdbRecordType->papFldDes[41]->size=sizeof(prec->time);
  pdbRecordType->papFldDes[41]->offset=(short)((char *)&prec->time - (char *)prec);
  pdbRecordType->papFldDes[42]->size=sizeof(prec->flnk);
  pdbRecordType->papFldDes[42]->offset=(short)((char *)&prec->flnk - (char *)prec);
  pdbRecordType->papFldDes[43]->size=sizeof(prec->vers);
  pdbRecordType->papFldDes[43]->offset=(short)((char *)&prec->vers - (char *)prec);
  pdbRecordType->papFldDes[44]->size=sizeof(prec->val);
  pdbRecordType->papFldDes[44]->offset=(short)((char *)&prec->val - (char *)prec);
  pdbRecordType->papFldDes[45]->size=sizeof(prec->smsg);
  pdbRecordType->papFldDes[45]->offset=(short)((char *)&prec->smsg - (char *)prec);
  pdbRecordType->papFldDes[46]->size=sizeof(prec->cmnd);
  pdbRecordType->papFldDes[46]->offset=(short)((char *)&prec->cmnd - (char *)prec);
  pdbRecordType->papFldDes[47]->size=sizeof(prec->alrt);
  pdbRecordType->papFldDes[47]->offset=(short)((char *)&prec->alrt - (char *)prec);
  pdbRecordType->papFldDes[48]->size=sizeof(prec->rpvt);
  pdbRecordType->papFldDes[48]->offset=(short)((char *)&prec->rpvt - (char *)prec);
  pdbRecordType->papFldDes[49]->size=sizeof(prec->mpts);
  pdbRecordType->papFldDes[49]->offset=(short)((char *)&prec->mpts - (char *)prec);
  pdbRecordType->papFldDes[50]->size=sizeof(prec->exsc);
  pdbRecordType->papFldDes[50]->offset=(short)((char *)&prec->exsc - (char *)prec);
  pdbRecordType->papFldDes[51]->size=sizeof(prec->xsc);
  pdbRecordType->papFldDes[51]->offset=(short)((char *)&prec->xsc - (char *)prec);
  pdbRecordType->papFldDes[52]->size=sizeof(prec->pxsc);
  pdbRecordType->papFldDes[52]->offset=(short)((char *)&prec->pxsc - (char *)prec);
  pdbRecordType->papFldDes[53]->size=sizeof(prec->busy);
  pdbRecordType->papFldDes[53]->offset=(short)((char *)&prec->busy - (char *)prec);
  pdbRecordType->papFldDes[54]->size=sizeof(prec->kill);
  pdbRecordType->papFldDes[54]->offset=(short)((char *)&prec->kill - (char *)prec);
  pdbRecordType->papFldDes[55]->size=sizeof(prec->wait);
  pdbRecordType->papFldDes[55]->offset=(short)((char *)&prec->wait - (char *)prec);
  pdbRecordType->papFldDes[56]->size=sizeof(prec->wcnt);
  pdbRecordType->papFldDes[56]->offset=(short)((char *)&prec->wcnt - (char *)prec);
  pdbRecordType->papFldDes[57]->size=sizeof(prec->awct);
  pdbRecordType->papFldDes[57]->offset=(short)((char *)&prec->awct - (char *)prec);
  pdbRecordType->papFldDes[58]->size=sizeof(prec->wtng);
  pdbRecordType->papFldDes[58]->offset=(short)((char *)&prec->wtng - (char *)prec);
  pdbRecordType->papFldDes[59]->size=sizeof(prec->await);
  pdbRecordType->papFldDes[59]->offset=(short)((char *)&prec->await - (char *)prec);
  pdbRecordType->papFldDes[60]->size=sizeof(prec->aawait);
  pdbRecordType->papFldDes[60]->offset=(short)((char *)&prec->aawait - (char *)prec);
  pdbRecordType->papFldDes[61]->size=sizeof(prec->data);
  pdbRecordType->papFldDes[61]->offset=(short)((char *)&prec->data - (char *)prec);
  pdbRecordType->papFldDes[62]->size=sizeof(prec->refd);
  pdbRecordType->papFldDes[62]->offset=(short)((char *)&prec->refd - (char *)prec);
  pdbRecordType->papFldDes[63]->size=sizeof(prec->npts);
  pdbRecordType->papFldDes[63]->offset=(short)((char *)&prec->npts - (char *)prec);
  pdbRecordType->papFldDes[64]->size=sizeof(prec->fpts);
  pdbRecordType->papFldDes[64]->offset=(short)((char *)&prec->fpts - (char *)prec);
  pdbRecordType->papFldDes[65]->size=sizeof(prec->ffo);
  pdbRecordType->papFldDes[65]->offset=(short)((char *)&prec->ffo - (char *)prec);
  pdbRecordType->papFldDes[66]->size=sizeof(prec->cpt);
  pdbRecordType->papFldDes[66]->offset=(short)((char *)&prec->cpt - (char *)prec);
  pdbRecordType->papFldDes[67]->size=sizeof(prec->bcpt);
  pdbRecordType->papFldDes[67]->offset=(short)((char *)&prec->bcpt - (char *)prec);
  pdbRecordType->papFldDes[68]->size=sizeof(prec->dpt);
  pdbRecordType->papFldDes[68]->offset=(short)((char *)&prec->dpt - (char *)prec);
  pdbRecordType->papFldDes[69]->size=sizeof(prec->pcpt);
  pdbRecordType->papFldDes[69]->offset=(short)((char *)&prec->pcpt - (char *)prec);
  pdbRecordType->papFldDes[70]->size=sizeof(prec->pasm);
  pdbRecordType->papFldDes[70]->offset=(short)((char *)&prec->pasm - (char *)prec);
  pdbRecordType->papFldDes[71]->size=sizeof(prec->tolp);
  pdbRecordType->papFldDes[71]->offset=(short)((char *)&prec->tolp - (char *)prec);
  pdbRecordType->papFldDes[72]->size=sizeof(prec->tlap);
  pdbRecordType->papFldDes[72]->offset=(short)((char *)&prec->tlap - (char *)prec);
  pdbRecordType->papFldDes[73]->size=sizeof(prec->atime);
  pdbRecordType->papFldDes[73]->offset=(short)((char *)&prec->atime - (char *)prec);
  pdbRecordType->papFldDes[74]->size=sizeof(prec->p1pv);
  pdbRecordType->papFldDes[74]->offset=(short)((char *)&prec->p1pv - (char *)prec);
  pdbRecordType->papFldDes[75]->size=sizeof(prec->p2pv);
  pdbRecordType->papFldDes[75]->offset=(short)((char *)&prec->p2pv - (char *)prec);
  pdbRecordType->papFldDes[76]->size=sizeof(prec->p3pv);
  pdbRecordType->papFldDes[76]->offset=(short)((char *)&prec->p3pv - (char *)prec);
  pdbRecordType->papFldDes[77]->size=sizeof(prec->p4pv);
  pdbRecordType->papFldDes[77]->offset=(short)((char *)&prec->p4pv - (char *)prec);
  pdbRecordType->papFldDes[78]->size=sizeof(prec->r1pv);
  pdbRecordType->papFldDes[78]->offset=(short)((char *)&prec->r1pv - (char *)prec);
  pdbRecordType->papFldDes[79]->size=sizeof(prec->r2pv);
  pdbRecordType->papFldDes[79]->offset=(short)((char *)&prec->r2pv - (char *)prec);
  pdbRecordType->papFldDes[80]->size=sizeof(prec->r3pv);
  pdbRecordType->papFldDes[80]->offset=(short)((char *)&prec->r3pv - (char *)prec);
  pdbRecordType->papFldDes[81]->size=sizeof(prec->r4pv);
  pdbRecordType->papFldDes[81]->offset=(short)((char *)&prec->r4pv - (char *)prec);
  pdbRecordType->papFldDes[82]->size=sizeof(prec->d01pv);
  pdbRecordType->papFldDes[82]->offset=(short)((char *)&prec->d01pv - (char *)prec);
  pdbRecordType->papFldDes[83]->size=sizeof(prec->d02pv);
  pdbRecordType->papFldDes[83]->offset=(short)((char *)&prec->d02pv - (char *)prec);
  pdbRecordType->papFldDes[84]->size=sizeof(prec->d03pv);
  pdbRecordType->papFldDes[84]->offset=(short)((char *)&prec->d03pv - (char *)prec);
  pdbRecordType->papFldDes[85]->size=sizeof(prec->d04pv);
  pdbRecordType->papFldDes[85]->offset=(short)((char *)&prec->d04pv - (char *)prec);
  pdbRecordType->papFldDes[86]->size=sizeof(prec->d05pv);
  pdbRecordType->papFldDes[86]->offset=(short)((char *)&prec->d05pv - (char *)prec);
  pdbRecordType->papFldDes[87]->size=sizeof(prec->d06pv);
  pdbRecordType->papFldDes[87]->offset=(short)((char *)&prec->d06pv - (char *)prec);
  pdbRecordType->papFldDes[88]->size=sizeof(prec->d07pv);
  pdbRecordType->papFldDes[88]->offset=(short)((char *)&prec->d07pv - (char *)prec);
  pdbRecordType->papFldDes[89]->size=sizeof(prec->d08pv);
  pdbRecordType->papFldDes[89]->offset=(short)((char *)&prec->d08pv - (char *)prec);
  pdbRecordType->papFldDes[90]->size=sizeof(prec->d09pv);
  pdbRecordType->papFldDes[90]->offset=(short)((char *)&prec->d09pv - (char *)prec);
  pdbRecordType->papFldDes[91]->size=sizeof(prec->d10pv);
  pdbRecordType->papFldDes[91]->offset=(short)((char *)&prec->d10pv - (char *)prec);
  pdbRecordType->papFldDes[92]->size=sizeof(prec->d11pv);
  pdbRecordType->papFldDes[92]->offset=(short)((char *)&prec->d11pv - (char *)prec);
  pdbRecordType->papFldDes[93]->size=sizeof(prec->d12pv);
  pdbRecordType->papFldDes[93]->offset=(short)((char *)&prec->d12pv - (char *)prec);
  pdbRecordType->papFldDes[94]->size=sizeof(prec->d13pv);
  pdbRecordType->papFldDes[94]->offset=(short)((char *)&prec->d13pv - (char *)prec);
  pdbRecordType->papFldDes[95]->size=sizeof(prec->d14pv);
  pdbRecordType->papFldDes[95]->offset=(short)((char *)&prec->d14pv - (char *)prec);
  pdbRecordType->papFldDes[96]->size=sizeof(prec->d15pv);
  pdbRecordType->papFldDes[96]->offset=(short)((char *)&prec->d15pv - (char *)prec);
  pdbRecordType->papFldDes[97]->size=sizeof(prec->d16pv);
  pdbRecordType->papFldDes[97]->offset=(short)((char *)&prec->d16pv - (char *)prec);
  pdbRecordType->papFldDes[98]->size=sizeof(prec->d17pv);
  pdbRecordType->papFldDes[98]->offset=(short)((char *)&prec->d17pv - (char *)prec);
  pdbRecordType->papFldDes[99]->size=sizeof(prec->d18pv);
  pdbRecordType->papFldDes[99]->offset=(short)((char *)&prec->d18pv - (char *)prec);
  pdbRecordType->papFldDes[100]->size=sizeof(prec->d19pv);
  pdbRecordType->papFldDes[100]->offset=(short)((char *)&prec->d19pv - (char *)prec);
  pdbRecordType->papFldDes[101]->size=sizeof(prec->d20pv);
  pdbRecordType->papFldDes[101]->offset=(short)((char *)&prec->d20pv - (char *)prec);
  pdbRecordType->papFldDes[102]->size=sizeof(prec->d21pv);
  pdbRecordType->papFldDes[102]->offset=(short)((char *)&prec->d21pv - (char *)prec);
  pdbRecordType->papFldDes[103]->size=sizeof(prec->d22pv);
  pdbRecordType->papFldDes[103]->offset=(short)((char *)&prec->d22pv - (char *)prec);
  pdbRecordType->papFldDes[104]->size=sizeof(prec->d23pv);
  pdbRecordType->papFldDes[104]->offset=(short)((char *)&prec->d23pv - (char *)prec);
  pdbRecordType->papFldDes[105]->size=sizeof(prec->d24pv);
  pdbRecordType->papFldDes[105]->offset=(short)((char *)&prec->d24pv - (char *)prec);
  pdbRecordType->papFldDes[106]->size=sizeof(prec->d25pv);
  pdbRecordType->papFldDes[106]->offset=(short)((char *)&prec->d25pv - (char *)prec);
  pdbRecordType->papFldDes[107]->size=sizeof(prec->d26pv);
  pdbRecordType->papFldDes[107]->offset=(short)((char *)&prec->d26pv - (char *)prec);
  pdbRecordType->papFldDes[108]->size=sizeof(prec->d27pv);
  pdbRecordType->papFldDes[108]->offset=(short)((char *)&prec->d27pv - (char *)prec);
  pdbRecordType->papFldDes[109]->size=sizeof(prec->d28pv);
  pdbRecordType->papFldDes[109]->offset=(short)((char *)&prec->d28pv - (char *)prec);
  pdbRecordType->papFldDes[110]->size=sizeof(prec->d29pv);
  pdbRecordType->papFldDes[110]->offset=(short)((char *)&prec->d29pv - (char *)prec);
  pdbRecordType->papFldDes[111]->size=sizeof(prec->d30pv);
  pdbRecordType->papFldDes[111]->offset=(short)((char *)&prec->d30pv - (char *)prec);
  pdbRecordType->papFldDes[112]->size=sizeof(prec->d31pv);
  pdbRecordType->papFldDes[112]->offset=(short)((char *)&prec->d31pv - (char *)prec);
  pdbRecordType->papFldDes[113]->size=sizeof(prec->d32pv);
  pdbRecordType->papFldDes[113]->offset=(short)((char *)&prec->d32pv - (char *)prec);
  pdbRecordType->papFldDes[114]->size=sizeof(prec->d33pv);
  pdbRecordType->papFldDes[114]->offset=(short)((char *)&prec->d33pv - (char *)prec);
  pdbRecordType->papFldDes[115]->size=sizeof(prec->d34pv);
  pdbRecordType->papFldDes[115]->offset=(short)((char *)&prec->d34pv - (char *)prec);
  pdbRecordType->papFldDes[116]->size=sizeof(prec->d35pv);
  pdbRecordType->papFldDes[116]->offset=(short)((char *)&prec->d35pv - (char *)prec);
  pdbRecordType->papFldDes[117]->size=sizeof(prec->d36pv);
  pdbRecordType->papFldDes[117]->offset=(short)((char *)&prec->d36pv - (char *)prec);
  pdbRecordType->papFldDes[118]->size=sizeof(prec->d37pv);
  pdbRecordType->papFldDes[118]->offset=(short)((char *)&prec->d37pv - (char *)prec);
  pdbRecordType->papFldDes[119]->size=sizeof(prec->d38pv);
  pdbRecordType->papFldDes[119]->offset=(short)((char *)&prec->d38pv - (char *)prec);
  pdbRecordType->papFldDes[120]->size=sizeof(prec->d39pv);
  pdbRecordType->papFldDes[120]->offset=(short)((char *)&prec->d39pv - (char *)prec);
  pdbRecordType->papFldDes[121]->size=sizeof(prec->d40pv);
  pdbRecordType->papFldDes[121]->offset=(short)((char *)&prec->d40pv - (char *)prec);
  pdbRecordType->papFldDes[122]->size=sizeof(prec->d41pv);
  pdbRecordType->papFldDes[122]->offset=(short)((char *)&prec->d41pv - (char *)prec);
  pdbRecordType->papFldDes[123]->size=sizeof(prec->d42pv);
  pdbRecordType->papFldDes[123]->offset=(short)((char *)&prec->d42pv - (char *)prec);
  pdbRecordType->papFldDes[124]->size=sizeof(prec->d43pv);
  pdbRecordType->papFldDes[124]->offset=(short)((char *)&prec->d43pv - (char *)prec);
  pdbRecordType->papFldDes[125]->size=sizeof(prec->d44pv);
  pdbRecordType->papFldDes[125]->offset=(short)((char *)&prec->d44pv - (char *)prec);
  pdbRecordType->papFldDes[126]->size=sizeof(prec->d45pv);
  pdbRecordType->papFldDes[126]->offset=(short)((char *)&prec->d45pv - (char *)prec);
  pdbRecordType->papFldDes[127]->size=sizeof(prec->d46pv);
  pdbRecordType->papFldDes[127]->offset=(short)((char *)&prec->d46pv - (char *)prec);
  pdbRecordType->papFldDes[128]->size=sizeof(prec->d47pv);
  pdbRecordType->papFldDes[128]->offset=(short)((char *)&prec->d47pv - (char *)prec);
  pdbRecordType->papFldDes[129]->size=sizeof(prec->d48pv);
  pdbRecordType->papFldDes[129]->offset=(short)((char *)&prec->d48pv - (char *)prec);
  pdbRecordType->papFldDes[130]->size=sizeof(prec->d49pv);
  pdbRecordType->papFldDes[130]->offset=(short)((char *)&prec->d49pv - (char *)prec);
  pdbRecordType->papFldDes[131]->size=sizeof(prec->d50pv);
  pdbRecordType->papFldDes[131]->offset=(short)((char *)&prec->d50pv - (char *)prec);
  pdbRecordType->papFldDes[132]->size=sizeof(prec->d51pv);
  pdbRecordType->papFldDes[132]->offset=(short)((char *)&prec->d51pv - (char *)prec);
  pdbRecordType->papFldDes[133]->size=sizeof(prec->d52pv);
  pdbRecordType->papFldDes[133]->offset=(short)((char *)&prec->d52pv - (char *)prec);
  pdbRecordType->papFldDes[134]->size=sizeof(prec->d53pv);
  pdbRecordType->papFldDes[134]->offset=(short)((char *)&prec->d53pv - (char *)prec);
  pdbRecordType->papFldDes[135]->size=sizeof(prec->d54pv);
  pdbRecordType->papFldDes[135]->offset=(short)((char *)&prec->d54pv - (char *)prec);
  pdbRecordType->papFldDes[136]->size=sizeof(prec->d55pv);
  pdbRecordType->papFldDes[136]->offset=(short)((char *)&prec->d55pv - (char *)prec);
  pdbRecordType->papFldDes[137]->size=sizeof(prec->d56pv);
  pdbRecordType->papFldDes[137]->offset=(short)((char *)&prec->d56pv - (char *)prec);
  pdbRecordType->papFldDes[138]->size=sizeof(prec->d57pv);
  pdbRecordType->papFldDes[138]->offset=(short)((char *)&prec->d57pv - (char *)prec);
  pdbRecordType->papFldDes[139]->size=sizeof(prec->d58pv);
  pdbRecordType->papFldDes[139]->offset=(short)((char *)&prec->d58pv - (char *)prec);
  pdbRecordType->papFldDes[140]->size=sizeof(prec->d59pv);
  pdbRecordType->papFldDes[140]->offset=(short)((char *)&prec->d59pv - (char *)prec);
  pdbRecordType->papFldDes[141]->size=sizeof(prec->d60pv);
  pdbRecordType->papFldDes[141]->offset=(short)((char *)&prec->d60pv - (char *)prec);
  pdbRecordType->papFldDes[142]->size=sizeof(prec->d61pv);
  pdbRecordType->papFldDes[142]->offset=(short)((char *)&prec->d61pv - (char *)prec);
  pdbRecordType->papFldDes[143]->size=sizeof(prec->d62pv);
  pdbRecordType->papFldDes[143]->offset=(short)((char *)&prec->d62pv - (char *)prec);
  pdbRecordType->papFldDes[144]->size=sizeof(prec->d63pv);
  pdbRecordType->papFldDes[144]->offset=(short)((char *)&prec->d63pv - (char *)prec);
  pdbRecordType->papFldDes[145]->size=sizeof(prec->d64pv);
  pdbRecordType->papFldDes[145]->offset=(short)((char *)&prec->d64pv - (char *)prec);
  pdbRecordType->papFldDes[146]->size=sizeof(prec->d65pv);
  pdbRecordType->papFldDes[146]->offset=(short)((char *)&prec->d65pv - (char *)prec);
  pdbRecordType->papFldDes[147]->size=sizeof(prec->d66pv);
  pdbRecordType->papFldDes[147]->offset=(short)((char *)&prec->d66pv - (char *)prec);
  pdbRecordType->papFldDes[148]->size=sizeof(prec->d67pv);
  pdbRecordType->papFldDes[148]->offset=(short)((char *)&prec->d67pv - (char *)prec);
  pdbRecordType->papFldDes[149]->size=sizeof(prec->d68pv);
  pdbRecordType->papFldDes[149]->offset=(short)((char *)&prec->d68pv - (char *)prec);
  pdbRecordType->papFldDes[150]->size=sizeof(prec->d69pv);
  pdbRecordType->papFldDes[150]->offset=(short)((char *)&prec->d69pv - (char *)prec);
  pdbRecordType->papFldDes[151]->size=sizeof(prec->d70pv);
  pdbRecordType->papFldDes[151]->offset=(short)((char *)&prec->d70pv - (char *)prec);
  pdbRecordType->papFldDes[152]->size=sizeof(prec->t1pv);
  pdbRecordType->papFldDes[152]->offset=(short)((char *)&prec->t1pv - (char *)prec);
  pdbRecordType->papFldDes[153]->size=sizeof(prec->t2pv);
  pdbRecordType->papFldDes[153]->offset=(short)((char *)&prec->t2pv - (char *)prec);
  pdbRecordType->papFldDes[154]->size=sizeof(prec->t3pv);
  pdbRecordType->papFldDes[154]->offset=(short)((char *)&prec->t3pv - (char *)prec);
  pdbRecordType->papFldDes[155]->size=sizeof(prec->t4pv);
  pdbRecordType->papFldDes[155]->offset=(short)((char *)&prec->t4pv - (char *)prec);
  pdbRecordType->papFldDes[156]->size=sizeof(prec->a1pv);
  pdbRecordType->papFldDes[156]->offset=(short)((char *)&prec->a1pv - (char *)prec);
  pdbRecordType->papFldDes[157]->size=sizeof(prec->bspv);
  pdbRecordType->papFldDes[157]->offset=(short)((char *)&prec->bspv - (char *)prec);
  pdbRecordType->papFldDes[158]->size=sizeof(prec->aspv);
  pdbRecordType->papFldDes[158]->offset=(short)((char *)&prec->aspv - (char *)prec);
  pdbRecordType->papFldDes[159]->size=sizeof(prec->bswait);
  pdbRecordType->papFldDes[159]->offset=(short)((char *)&prec->bswait - (char *)prec);
  pdbRecordType->papFldDes[160]->size=sizeof(prec->aswait);
  pdbRecordType->papFldDes[160]->offset=(short)((char *)&prec->aswait - (char *)prec);
  pdbRecordType->papFldDes[161]->size=sizeof(prec->p1nv);
  pdbRecordType->papFldDes[161]->offset=(short)((char *)&prec->p1nv - (char *)prec);
  pdbRecordType->papFldDes[162]->size=sizeof(prec->p2nv);
  pdbRecordType->papFldDes[162]->offset=(short)((char *)&prec->p2nv - (char *)prec);
  pdbRecordType->papFldDes[163]->size=sizeof(prec->p3nv);
  pdbRecordType->papFldDes[163]->offset=(short)((char *)&prec->p3nv - (char *)prec);
  pdbRecordType->papFldDes[164]->size=sizeof(prec->p4nv);
  pdbRecordType->papFldDes[164]->offset=(short)((char *)&prec->p4nv - (char *)prec);
  pdbRecordType->papFldDes[165]->size=sizeof(prec->r1nv);
  pdbRecordType->papFldDes[165]->offset=(short)((char *)&prec->r1nv - (char *)prec);
  pdbRecordType->papFldDes[166]->size=sizeof(prec->r2nv);
  pdbRecordType->papFldDes[166]->offset=(short)((char *)&prec->r2nv - (char *)prec);
  pdbRecordType->papFldDes[167]->size=sizeof(prec->r3nv);
  pdbRecordType->papFldDes[167]->offset=(short)((char *)&prec->r3nv - (char *)prec);
  pdbRecordType->papFldDes[168]->size=sizeof(prec->r4nv);
  pdbRecordType->papFldDes[168]->offset=(short)((char *)&prec->r4nv - (char *)prec);
  pdbRecordType->papFldDes[169]->size=sizeof(prec->d01nv);
  pdbRecordType->papFldDes[169]->offset=(short)((char *)&prec->d01nv - (char *)prec);
  pdbRecordType->papFldDes[170]->size=sizeof(prec->d02nv);
  pdbRecordType->papFldDes[170]->offset=(short)((char *)&prec->d02nv - (char *)prec);
  pdbRecordType->papFldDes[171]->size=sizeof(prec->d03nv);
  pdbRecordType->papFldDes[171]->offset=(short)((char *)&prec->d03nv - (char *)prec);
  pdbRecordType->papFldDes[172]->size=sizeof(prec->d04nv);
  pdbRecordType->papFldDes[172]->offset=(short)((char *)&prec->d04nv - (char *)prec);
  pdbRecordType->papFldDes[173]->size=sizeof(prec->d05nv);
  pdbRecordType->papFldDes[173]->offset=(short)((char *)&prec->d05nv - (char *)prec);
  pdbRecordType->papFldDes[174]->size=sizeof(prec->d06nv);
  pdbRecordType->papFldDes[174]->offset=(short)((char *)&prec->d06nv - (char *)prec);
  pdbRecordType->papFldDes[175]->size=sizeof(prec->d07nv);
  pdbRecordType->papFldDes[175]->offset=(short)((char *)&prec->d07nv - (char *)prec);
  pdbRecordType->papFldDes[176]->size=sizeof(prec->d08nv);
  pdbRecordType->papFldDes[176]->offset=(short)((char *)&prec->d08nv - (char *)prec);
  pdbRecordType->papFldDes[177]->size=sizeof(prec->d09nv);
  pdbRecordType->papFldDes[177]->offset=(short)((char *)&prec->d09nv - (char *)prec);
  pdbRecordType->papFldDes[178]->size=sizeof(prec->d10nv);
  pdbRecordType->papFldDes[178]->offset=(short)((char *)&prec->d10nv - (char *)prec);
  pdbRecordType->papFldDes[179]->size=sizeof(prec->d11nv);
  pdbRecordType->papFldDes[179]->offset=(short)((char *)&prec->d11nv - (char *)prec);
  pdbRecordType->papFldDes[180]->size=sizeof(prec->d12nv);
  pdbRecordType->papFldDes[180]->offset=(short)((char *)&prec->d12nv - (char *)prec);
  pdbRecordType->papFldDes[181]->size=sizeof(prec->d13nv);
  pdbRecordType->papFldDes[181]->offset=(short)((char *)&prec->d13nv - (char *)prec);
  pdbRecordType->papFldDes[182]->size=sizeof(prec->d14nv);
  pdbRecordType->papFldDes[182]->offset=(short)((char *)&prec->d14nv - (char *)prec);
  pdbRecordType->papFldDes[183]->size=sizeof(prec->d15nv);
  pdbRecordType->papFldDes[183]->offset=(short)((char *)&prec->d15nv - (char *)prec);
  pdbRecordType->papFldDes[184]->size=sizeof(prec->d16nv);
  pdbRecordType->papFldDes[184]->offset=(short)((char *)&prec->d16nv - (char *)prec);
  pdbRecordType->papFldDes[185]->size=sizeof(prec->d17nv);
  pdbRecordType->papFldDes[185]->offset=(short)((char *)&prec->d17nv - (char *)prec);
  pdbRecordType->papFldDes[186]->size=sizeof(prec->d18nv);
  pdbRecordType->papFldDes[186]->offset=(short)((char *)&prec->d18nv - (char *)prec);
  pdbRecordType->papFldDes[187]->size=sizeof(prec->d19nv);
  pdbRecordType->papFldDes[187]->offset=(short)((char *)&prec->d19nv - (char *)prec);
  pdbRecordType->papFldDes[188]->size=sizeof(prec->d20nv);
  pdbRecordType->papFldDes[188]->offset=(short)((char *)&prec->d20nv - (char *)prec);
  pdbRecordType->papFldDes[189]->size=sizeof(prec->d21nv);
  pdbRecordType->papFldDes[189]->offset=(short)((char *)&prec->d21nv - (char *)prec);
  pdbRecordType->papFldDes[190]->size=sizeof(prec->d22nv);
  pdbRecordType->papFldDes[190]->offset=(short)((char *)&prec->d22nv - (char *)prec);
  pdbRecordType->papFldDes[191]->size=sizeof(prec->d23nv);
  pdbRecordType->papFldDes[191]->offset=(short)((char *)&prec->d23nv - (char *)prec);
  pdbRecordType->papFldDes[192]->size=sizeof(prec->d24nv);
  pdbRecordType->papFldDes[192]->offset=(short)((char *)&prec->d24nv - (char *)prec);
  pdbRecordType->papFldDes[193]->size=sizeof(prec->d25nv);
  pdbRecordType->papFldDes[193]->offset=(short)((char *)&prec->d25nv - (char *)prec);
  pdbRecordType->papFldDes[194]->size=sizeof(prec->d26nv);
  pdbRecordType->papFldDes[194]->offset=(short)((char *)&prec->d26nv - (char *)prec);
  pdbRecordType->papFldDes[195]->size=sizeof(prec->d27nv);
  pdbRecordType->papFldDes[195]->offset=(short)((char *)&prec->d27nv - (char *)prec);
  pdbRecordType->papFldDes[196]->size=sizeof(prec->d28nv);
  pdbRecordType->papFldDes[196]->offset=(short)((char *)&prec->d28nv - (char *)prec);
  pdbRecordType->papFldDes[197]->size=sizeof(prec->d29nv);
  pdbRecordType->papFldDes[197]->offset=(short)((char *)&prec->d29nv - (char *)prec);
  pdbRecordType->papFldDes[198]->size=sizeof(prec->d30nv);
  pdbRecordType->papFldDes[198]->offset=(short)((char *)&prec->d30nv - (char *)prec);
  pdbRecordType->papFldDes[199]->size=sizeof(prec->d31nv);
  pdbRecordType->papFldDes[199]->offset=(short)((char *)&prec->d31nv - (char *)prec);
  pdbRecordType->papFldDes[200]->size=sizeof(prec->d32nv);
  pdbRecordType->papFldDes[200]->offset=(short)((char *)&prec->d32nv - (char *)prec);
  pdbRecordType->papFldDes[201]->size=sizeof(prec->d33nv);
  pdbRecordType->papFldDes[201]->offset=(short)((char *)&prec->d33nv - (char *)prec);
  pdbRecordType->papFldDes[202]->size=sizeof(prec->d34nv);
  pdbRecordType->papFldDes[202]->offset=(short)((char *)&prec->d34nv - (char *)prec);
  pdbRecordType->papFldDes[203]->size=sizeof(prec->d35nv);
  pdbRecordType->papFldDes[203]->offset=(short)((char *)&prec->d35nv - (char *)prec);
  pdbRecordType->papFldDes[204]->size=sizeof(prec->d36nv);
  pdbRecordType->papFldDes[204]->offset=(short)((char *)&prec->d36nv - (char *)prec);
  pdbRecordType->papFldDes[205]->size=sizeof(prec->d37nv);
  pdbRecordType->papFldDes[205]->offset=(short)((char *)&prec->d37nv - (char *)prec);
  pdbRecordType->papFldDes[206]->size=sizeof(prec->d38nv);
  pdbRecordType->papFldDes[206]->offset=(short)((char *)&prec->d38nv - (char *)prec);
  pdbRecordType->papFldDes[207]->size=sizeof(prec->d39nv);
  pdbRecordType->papFldDes[207]->offset=(short)((char *)&prec->d39nv - (char *)prec);
  pdbRecordType->papFldDes[208]->size=sizeof(prec->d40nv);
  pdbRecordType->papFldDes[208]->offset=(short)((char *)&prec->d40nv - (char *)prec);
  pdbRecordType->papFldDes[209]->size=sizeof(prec->d41nv);
  pdbRecordType->papFldDes[209]->offset=(short)((char *)&prec->d41nv - (char *)prec);
  pdbRecordType->papFldDes[210]->size=sizeof(prec->d42nv);
  pdbRecordType->papFldDes[210]->offset=(short)((char *)&prec->d42nv - (char *)prec);
  pdbRecordType->papFldDes[211]->size=sizeof(prec->d43nv);
  pdbRecordType->papFldDes[211]->offset=(short)((char *)&prec->d43nv - (char *)prec);
  pdbRecordType->papFldDes[212]->size=sizeof(prec->d44nv);
  pdbRecordType->papFldDes[212]->offset=(short)((char *)&prec->d44nv - (char *)prec);
  pdbRecordType->papFldDes[213]->size=sizeof(prec->d45nv);
  pdbRecordType->papFldDes[213]->offset=(short)((char *)&prec->d45nv - (char *)prec);
  pdbRecordType->papFldDes[214]->size=sizeof(prec->d46nv);
  pdbRecordType->papFldDes[214]->offset=(short)((char *)&prec->d46nv - (char *)prec);
  pdbRecordType->papFldDes[215]->size=sizeof(prec->d47nv);
  pdbRecordType->papFldDes[215]->offset=(short)((char *)&prec->d47nv - (char *)prec);
  pdbRecordType->papFldDes[216]->size=sizeof(prec->d48nv);
  pdbRecordType->papFldDes[216]->offset=(short)((char *)&prec->d48nv - (char *)prec);
  pdbRecordType->papFldDes[217]->size=sizeof(prec->d49nv);
  pdbRecordType->papFldDes[217]->offset=(short)((char *)&prec->d49nv - (char *)prec);
  pdbRecordType->papFldDes[218]->size=sizeof(prec->d50nv);
  pdbRecordType->papFldDes[218]->offset=(short)((char *)&prec->d50nv - (char *)prec);
  pdbRecordType->papFldDes[219]->size=sizeof(prec->d51nv);
  pdbRecordType->papFldDes[219]->offset=(short)((char *)&prec->d51nv - (char *)prec);
  pdbRecordType->papFldDes[220]->size=sizeof(prec->d52nv);
  pdbRecordType->papFldDes[220]->offset=(short)((char *)&prec->d52nv - (char *)prec);
  pdbRecordType->papFldDes[221]->size=sizeof(prec->d53nv);
  pdbRecordType->papFldDes[221]->offset=(short)((char *)&prec->d53nv - (char *)prec);
  pdbRecordType->papFldDes[222]->size=sizeof(prec->d54nv);
  pdbRecordType->papFldDes[222]->offset=(short)((char *)&prec->d54nv - (char *)prec);
  pdbRecordType->papFldDes[223]->size=sizeof(prec->d55nv);
  pdbRecordType->papFldDes[223]->offset=(short)((char *)&prec->d55nv - (char *)prec);
  pdbRecordType->papFldDes[224]->size=sizeof(prec->d56nv);
  pdbRecordType->papFldDes[224]->offset=(short)((char *)&prec->d56nv - (char *)prec);
  pdbRecordType->papFldDes[225]->size=sizeof(prec->d57nv);
  pdbRecordType->papFldDes[225]->offset=(short)((char *)&prec->d57nv - (char *)prec);
  pdbRecordType->papFldDes[226]->size=sizeof(prec->d58nv);
  pdbRecordType->papFldDes[226]->offset=(short)((char *)&prec->d58nv - (char *)prec);
  pdbRecordType->papFldDes[227]->size=sizeof(prec->d59nv);
  pdbRecordType->papFldDes[227]->offset=(short)((char *)&prec->d59nv - (char *)prec);
  pdbRecordType->papFldDes[228]->size=sizeof(prec->d60nv);
  pdbRecordType->papFldDes[228]->offset=(short)((char *)&prec->d60nv - (char *)prec);
  pdbRecordType->papFldDes[229]->size=sizeof(prec->d61nv);
  pdbRecordType->papFldDes[229]->offset=(short)((char *)&prec->d61nv - (char *)prec);
  pdbRecordType->papFldDes[230]->size=sizeof(prec->d62nv);
  pdbRecordType->papFldDes[230]->offset=(short)((char *)&prec->d62nv - (char *)prec);
  pdbRecordType->papFldDes[231]->size=sizeof(prec->d63nv);
  pdbRecordType->papFldDes[231]->offset=(short)((char *)&prec->d63nv - (char *)prec);
  pdbRecordType->papFldDes[232]->size=sizeof(prec->d64nv);
  pdbRecordType->papFldDes[232]->offset=(short)((char *)&prec->d64nv - (char *)prec);
  pdbRecordType->papFldDes[233]->size=sizeof(prec->d65nv);
  pdbRecordType->papFldDes[233]->offset=(short)((char *)&prec->d65nv - (char *)prec);
  pdbRecordType->papFldDes[234]->size=sizeof(prec->d66nv);
  pdbRecordType->papFldDes[234]->offset=(short)((char *)&prec->d66nv - (char *)prec);
  pdbRecordType->papFldDes[235]->size=sizeof(prec->d67nv);
  pdbRecordType->papFldDes[235]->offset=(short)((char *)&prec->d67nv - (char *)prec);
  pdbRecordType->papFldDes[236]->size=sizeof(prec->d68nv);
  pdbRecordType->papFldDes[236]->offset=(short)((char *)&prec->d68nv - (char *)prec);
  pdbRecordType->papFldDes[237]->size=sizeof(prec->d69nv);
  pdbRecordType->papFldDes[237]->offset=(short)((char *)&prec->d69nv - (char *)prec);
  pdbRecordType->papFldDes[238]->size=sizeof(prec->d70nv);
  pdbRecordType->papFldDes[238]->offset=(short)((char *)&prec->d70nv - (char *)prec);
  pdbRecordType->papFldDes[239]->size=sizeof(prec->t1nv);
  pdbRecordType->papFldDes[239]->offset=(short)((char *)&prec->t1nv - (char *)prec);
  pdbRecordType->papFldDes[240]->size=sizeof(prec->t2nv);
  pdbRecordType->papFldDes[240]->offset=(short)((char *)&prec->t2nv - (char *)prec);
  pdbRecordType->papFldDes[241]->size=sizeof(prec->t3nv);
  pdbRecordType->papFldDes[241]->offset=(short)((char *)&prec->t3nv - (char *)prec);
  pdbRecordType->papFldDes[242]->size=sizeof(prec->t4nv);
  pdbRecordType->papFldDes[242]->offset=(short)((char *)&prec->t4nv - (char *)prec);
  pdbRecordType->papFldDes[243]->size=sizeof(prec->a1nv);
  pdbRecordType->papFldDes[243]->offset=(short)((char *)&prec->a1nv - (char *)prec);
  pdbRecordType->papFldDes[244]->size=sizeof(prec->bsnv);
  pdbRecordType->papFldDes[244]->offset=(short)((char *)&prec->bsnv - (char *)prec);
  pdbRecordType->papFldDes[245]->size=sizeof(prec->asnv);
  pdbRecordType->papFldDes[245]->offset=(short)((char *)&prec->asnv - (char *)prec);
  pdbRecordType->papFldDes[246]->size=sizeof(prec->p1pp);
  pdbRecordType->papFldDes[246]->offset=(short)((char *)&prec->p1pp - (char *)prec);
  pdbRecordType->papFldDes[247]->size=sizeof(prec->p1cv);
  pdbRecordType->papFldDes[247]->offset=(short)((char *)&prec->p1cv - (char *)prec);
  pdbRecordType->papFldDes[248]->size=sizeof(prec->p1dv);
  pdbRecordType->papFldDes[248]->offset=(short)((char *)&prec->p1dv - (char *)prec);
  pdbRecordType->papFldDes[249]->size=sizeof(prec->p1lv);
  pdbRecordType->papFldDes[249]->offset=(short)((char *)&prec->p1lv - (char *)prec);
  pdbRecordType->papFldDes[250]->size=sizeof(prec->p1sp);
  pdbRecordType->papFldDes[250]->offset=(short)((char *)&prec->p1sp - (char *)prec);
  pdbRecordType->papFldDes[251]->size=sizeof(prec->p1si);
  pdbRecordType->papFldDes[251]->offset=(short)((char *)&prec->p1si - (char *)prec);
  pdbRecordType->papFldDes[252]->size=sizeof(prec->p1ep);
  pdbRecordType->papFldDes[252]->offset=(short)((char *)&prec->p1ep - (char *)prec);
  pdbRecordType->papFldDes[253]->size=sizeof(prec->p1cp);
  pdbRecordType->papFldDes[253]->offset=(short)((char *)&prec->p1cp - (char *)prec);
  pdbRecordType->papFldDes[254]->size=sizeof(prec->p1wd);
  pdbRecordType->papFldDes[254]->offset=(short)((char *)&prec->p1wd - (char *)prec);
  pdbRecordType->papFldDes[255]->size=sizeof(prec->r1cv);
  pdbRecordType->papFldDes[255]->offset=(short)((char *)&prec->r1cv - (char *)prec);
  pdbRecordType->papFldDes[256]->size=sizeof(prec->r1lv);
  pdbRecordType->papFldDes[256]->offset=(short)((char *)&prec->r1lv - (char *)prec);
  pdbRecordType->papFldDes[257]->size=sizeof(prec->r1dl);
  pdbRecordType->papFldDes[257]->offset=(short)((char *)&prec->r1dl - (char *)prec);
  pdbRecordType->papFldDes[258]->size=sizeof(prec->p1hr);
  pdbRecordType->papFldDes[258]->offset=(short)((char *)&prec->p1hr - (char *)prec);
  pdbRecordType->papFldDes[259]->size=sizeof(prec->p1lr);
  pdbRecordType->papFldDes[259]->offset=(short)((char *)&prec->p1lr - (char *)prec);
  pdbRecordType->papFldDes[260]->size=sizeof(prec->p1pa);
  pdbRecordType->papFldDes[260]->offset=(short)((char *)&prec->p1pa - (char *)prec);
  pdbRecordType->papFldDes[261]->size=sizeof(prec->p1ra);
  pdbRecordType->papFldDes[261]->offset=(short)((char *)&prec->p1ra - (char *)prec);
  pdbRecordType->papFldDes[262]->size=sizeof(prec->p1ca);
  pdbRecordType->papFldDes[262]->offset=(short)((char *)&prec->p1ca - (char *)prec);
  pdbRecordType->papFldDes[263]->size=sizeof(prec->p1fs);
  pdbRecordType->papFldDes[263]->offset=(short)((char *)&prec->p1fs - (char *)prec);
  pdbRecordType->papFldDes[264]->size=sizeof(prec->p1fi);
  pdbRecordType->papFldDes[264]->offset=(short)((char *)&prec->p1fi - (char *)prec);
  pdbRecordType->papFldDes[265]->size=sizeof(prec->p1fe);
  pdbRecordType->papFldDes[265]->offset=(short)((char *)&prec->p1fe - (char *)prec);
  pdbRecordType->papFldDes[266]->size=sizeof(prec->p1fc);
  pdbRecordType->papFldDes[266]->offset=(short)((char *)&prec->p1fc - (char *)prec);
  pdbRecordType->papFldDes[267]->size=sizeof(prec->p1fw);
  pdbRecordType->papFldDes[267]->offset=(short)((char *)&prec->p1fw - (char *)prec);
  pdbRecordType->papFldDes[268]->size=sizeof(prec->p1sm);
  pdbRecordType->papFldDes[268]->offset=(short)((char *)&prec->p1sm - (char *)prec);
  pdbRecordType->papFldDes[269]->size=sizeof(prec->p1ar);
  pdbRecordType->papFldDes[269]->offset=(short)((char *)&prec->p1ar - (char *)prec);
  pdbRecordType->papFldDes[270]->size=sizeof(prec->p1eu);
  pdbRecordType->papFldDes[270]->offset=(short)((char *)&prec->p1eu - (char *)prec);
  pdbRecordType->papFldDes[271]->size=sizeof(prec->p1pr);
  pdbRecordType->papFldDes[271]->offset=(short)((char *)&prec->p1pr - (char *)prec);
  pdbRecordType->papFldDes[272]->size=sizeof(prec->p2pp);
  pdbRecordType->papFldDes[272]->offset=(short)((char *)&prec->p2pp - (char *)prec);
  pdbRecordType->papFldDes[273]->size=sizeof(prec->p2cv);
  pdbRecordType->papFldDes[273]->offset=(short)((char *)&prec->p2cv - (char *)prec);
  pdbRecordType->papFldDes[274]->size=sizeof(prec->p2dv);
  pdbRecordType->papFldDes[274]->offset=(short)((char *)&prec->p2dv - (char *)prec);
  pdbRecordType->papFldDes[275]->size=sizeof(prec->p2lv);
  pdbRecordType->papFldDes[275]->offset=(short)((char *)&prec->p2lv - (char *)prec);
  pdbRecordType->papFldDes[276]->size=sizeof(prec->p2sp);
  pdbRecordType->papFldDes[276]->offset=(short)((char *)&prec->p2sp - (char *)prec);
  pdbRecordType->papFldDes[277]->size=sizeof(prec->p2si);
  pdbRecordType->papFldDes[277]->offset=(short)((char *)&prec->p2si - (char *)prec);
  pdbRecordType->papFldDes[278]->size=sizeof(prec->p2ep);
  pdbRecordType->papFldDes[278]->offset=(short)((char *)&prec->p2ep - (char *)prec);
  pdbRecordType->papFldDes[279]->size=sizeof(prec->p2cp);
  pdbRecordType->papFldDes[279]->offset=(short)((char *)&prec->p2cp - (char *)prec);
  pdbRecordType->papFldDes[280]->size=sizeof(prec->p2wd);
  pdbRecordType->papFldDes[280]->offset=(short)((char *)&prec->p2wd - (char *)prec);
  pdbRecordType->papFldDes[281]->size=sizeof(prec->r2cv);
  pdbRecordType->papFldDes[281]->offset=(short)((char *)&prec->r2cv - (char *)prec);
  pdbRecordType->papFldDes[282]->size=sizeof(prec->r2lv);
  pdbRecordType->papFldDes[282]->offset=(short)((char *)&prec->r2lv - (char *)prec);
  pdbRecordType->papFldDes[283]->size=sizeof(prec->r2dl);
  pdbRecordType->papFldDes[283]->offset=(short)((char *)&prec->r2dl - (char *)prec);
  pdbRecordType->papFldDes[284]->size=sizeof(prec->p2hr);
  pdbRecordType->papFldDes[284]->offset=(short)((char *)&prec->p2hr - (char *)prec);
  pdbRecordType->papFldDes[285]->size=sizeof(prec->p2lr);
  pdbRecordType->papFldDes[285]->offset=(short)((char *)&prec->p2lr - (char *)prec);
  pdbRecordType->papFldDes[286]->size=sizeof(prec->p2pa);
  pdbRecordType->papFldDes[286]->offset=(short)((char *)&prec->p2pa - (char *)prec);
  pdbRecordType->papFldDes[287]->size=sizeof(prec->p2ra);
  pdbRecordType->papFldDes[287]->offset=(short)((char *)&prec->p2ra - (char *)prec);
  pdbRecordType->papFldDes[288]->size=sizeof(prec->p2ca);
  pdbRecordType->papFldDes[288]->offset=(short)((char *)&prec->p2ca - (char *)prec);
  pdbRecordType->papFldDes[289]->size=sizeof(prec->p2fs);
  pdbRecordType->papFldDes[289]->offset=(short)((char *)&prec->p2fs - (char *)prec);
  pdbRecordType->papFldDes[290]->size=sizeof(prec->p2fi);
  pdbRecordType->papFldDes[290]->offset=(short)((char *)&prec->p2fi - (char *)prec);
  pdbRecordType->papFldDes[291]->size=sizeof(prec->p2fe);
  pdbRecordType->papFldDes[291]->offset=(short)((char *)&prec->p2fe - (char *)prec);
  pdbRecordType->papFldDes[292]->size=sizeof(prec->p2fc);
  pdbRecordType->papFldDes[292]->offset=(short)((char *)&prec->p2fc - (char *)prec);
  pdbRecordType->papFldDes[293]->size=sizeof(prec->p2fw);
  pdbRecordType->papFldDes[293]->offset=(short)((char *)&prec->p2fw - (char *)prec);
  pdbRecordType->papFldDes[294]->size=sizeof(prec->p2sm);
  pdbRecordType->papFldDes[294]->offset=(short)((char *)&prec->p2sm - (char *)prec);
  pdbRecordType->papFldDes[295]->size=sizeof(prec->p2ar);
  pdbRecordType->papFldDes[295]->offset=(short)((char *)&prec->p2ar - (char *)prec);
  pdbRecordType->papFldDes[296]->size=sizeof(prec->p2eu);
  pdbRecordType->papFldDes[296]->offset=(short)((char *)&prec->p2eu - (char *)prec);
  pdbRecordType->papFldDes[297]->size=sizeof(prec->p2pr);
  pdbRecordType->papFldDes[297]->offset=(short)((char *)&prec->p2pr - (char *)prec);
  pdbRecordType->papFldDes[298]->size=sizeof(prec->p3pp);
  pdbRecordType->papFldDes[298]->offset=(short)((char *)&prec->p3pp - (char *)prec);
  pdbRecordType->papFldDes[299]->size=sizeof(prec->p3cv);
  pdbRecordType->papFldDes[299]->offset=(short)((char *)&prec->p3cv - (char *)prec);
  pdbRecordType->papFldDes[300]->size=sizeof(prec->p3dv);
  pdbRecordType->papFldDes[300]->offset=(short)((char *)&prec->p3dv - (char *)prec);
  pdbRecordType->papFldDes[301]->size=sizeof(prec->p3lv);
  pdbRecordType->papFldDes[301]->offset=(short)((char *)&prec->p3lv - (char *)prec);
  pdbRecordType->papFldDes[302]->size=sizeof(prec->p3sp);
  pdbRecordType->papFldDes[302]->offset=(short)((char *)&prec->p3sp - (char *)prec);
  pdbRecordType->papFldDes[303]->size=sizeof(prec->p3si);
  pdbRecordType->papFldDes[303]->offset=(short)((char *)&prec->p3si - (char *)prec);
  pdbRecordType->papFldDes[304]->size=sizeof(prec->p3ep);
  pdbRecordType->papFldDes[304]->offset=(short)((char *)&prec->p3ep - (char *)prec);
  pdbRecordType->papFldDes[305]->size=sizeof(prec->p3cp);
  pdbRecordType->papFldDes[305]->offset=(short)((char *)&prec->p3cp - (char *)prec);
  pdbRecordType->papFldDes[306]->size=sizeof(prec->p3wd);
  pdbRecordType->papFldDes[306]->offset=(short)((char *)&prec->p3wd - (char *)prec);
  pdbRecordType->papFldDes[307]->size=sizeof(prec->r3cv);
  pdbRecordType->papFldDes[307]->offset=(short)((char *)&prec->r3cv - (char *)prec);
  pdbRecordType->papFldDes[308]->size=sizeof(prec->r3lv);
  pdbRecordType->papFldDes[308]->offset=(short)((char *)&prec->r3lv - (char *)prec);
  pdbRecordType->papFldDes[309]->size=sizeof(prec->r3dl);
  pdbRecordType->papFldDes[309]->offset=(short)((char *)&prec->r3dl - (char *)prec);
  pdbRecordType->papFldDes[310]->size=sizeof(prec->p3hr);
  pdbRecordType->papFldDes[310]->offset=(short)((char *)&prec->p3hr - (char *)prec);
  pdbRecordType->papFldDes[311]->size=sizeof(prec->p3lr);
  pdbRecordType->papFldDes[311]->offset=(short)((char *)&prec->p3lr - (char *)prec);
  pdbRecordType->papFldDes[312]->size=sizeof(prec->p3pa);
  pdbRecordType->papFldDes[312]->offset=(short)((char *)&prec->p3pa - (char *)prec);
  pdbRecordType->papFldDes[313]->size=sizeof(prec->p3ra);
  pdbRecordType->papFldDes[313]->offset=(short)((char *)&prec->p3ra - (char *)prec);
  pdbRecordType->papFldDes[314]->size=sizeof(prec->p3ca);
  pdbRecordType->papFldDes[314]->offset=(short)((char *)&prec->p3ca - (char *)prec);
  pdbRecordType->papFldDes[315]->size=sizeof(prec->p3fs);
  pdbRecordType->papFldDes[315]->offset=(short)((char *)&prec->p3fs - (char *)prec);
  pdbRecordType->papFldDes[316]->size=sizeof(prec->p3fi);
  pdbRecordType->papFldDes[316]->offset=(short)((char *)&prec->p3fi - (char *)prec);
  pdbRecordType->papFldDes[317]->size=sizeof(prec->p3fe);
  pdbRecordType->papFldDes[317]->offset=(short)((char *)&prec->p3fe - (char *)prec);
  pdbRecordType->papFldDes[318]->size=sizeof(prec->p3fc);
  pdbRecordType->papFldDes[318]->offset=(short)((char *)&prec->p3fc - (char *)prec);
  pdbRecordType->papFldDes[319]->size=sizeof(prec->p3fw);
  pdbRecordType->papFldDes[319]->offset=(short)((char *)&prec->p3fw - (char *)prec);
  pdbRecordType->papFldDes[320]->size=sizeof(prec->p3sm);
  pdbRecordType->papFldDes[320]->offset=(short)((char *)&prec->p3sm - (char *)prec);
  pdbRecordType->papFldDes[321]->size=sizeof(prec->p3ar);
  pdbRecordType->papFldDes[321]->offset=(short)((char *)&prec->p3ar - (char *)prec);
  pdbRecordType->papFldDes[322]->size=sizeof(prec->p3eu);
  pdbRecordType->papFldDes[322]->offset=(short)((char *)&prec->p3eu - (char *)prec);
  pdbRecordType->papFldDes[323]->size=sizeof(prec->p3pr);
  pdbRecordType->papFldDes[323]->offset=(short)((char *)&prec->p3pr - (char *)prec);
  pdbRecordType->papFldDes[324]->size=sizeof(prec->p4pp);
  pdbRecordType->papFldDes[324]->offset=(short)((char *)&prec->p4pp - (char *)prec);
  pdbRecordType->papFldDes[325]->size=sizeof(prec->p4cv);
  pdbRecordType->papFldDes[325]->offset=(short)((char *)&prec->p4cv - (char *)prec);
  pdbRecordType->papFldDes[326]->size=sizeof(prec->p4dv);
  pdbRecordType->papFldDes[326]->offset=(short)((char *)&prec->p4dv - (char *)prec);
  pdbRecordType->papFldDes[327]->size=sizeof(prec->p4lv);
  pdbRecordType->papFldDes[327]->offset=(short)((char *)&prec->p4lv - (char *)prec);
  pdbRecordType->papFldDes[328]->size=sizeof(prec->p4sp);
  pdbRecordType->papFldDes[328]->offset=(short)((char *)&prec->p4sp - (char *)prec);
  pdbRecordType->papFldDes[329]->size=sizeof(prec->p4si);
  pdbRecordType->papFldDes[329]->offset=(short)((char *)&prec->p4si - (char *)prec);
  pdbRecordType->papFldDes[330]->size=sizeof(prec->p4ep);
  pdbRecordType->papFldDes[330]->offset=(short)((char *)&prec->p4ep - (char *)prec);
  pdbRecordType->papFldDes[331]->size=sizeof(prec->p4cp);
  pdbRecordType->papFldDes[331]->offset=(short)((char *)&prec->p4cp - (char *)prec);
  pdbRecordType->papFldDes[332]->size=sizeof(prec->p4wd);
  pdbRecordType->papFldDes[332]->offset=(short)((char *)&prec->p4wd - (char *)prec);
  pdbRecordType->papFldDes[333]->size=sizeof(prec->r4cv);
  pdbRecordType->papFldDes[333]->offset=(short)((char *)&prec->r4cv - (char *)prec);
  pdbRecordType->papFldDes[334]->size=sizeof(prec->r4lv);
  pdbRecordType->papFldDes[334]->offset=(short)((char *)&prec->r4lv - (char *)prec);
  pdbRecordType->papFldDes[335]->size=sizeof(prec->r4dl);
  pdbRecordType->papFldDes[335]->offset=(short)((char *)&prec->r4dl - (char *)prec);
  pdbRecordType->papFldDes[336]->size=sizeof(prec->p4hr);
  pdbRecordType->papFldDes[336]->offset=(short)((char *)&prec->p4hr - (char *)prec);
  pdbRecordType->papFldDes[337]->size=sizeof(prec->p4lr);
  pdbRecordType->papFldDes[337]->offset=(short)((char *)&prec->p4lr - (char *)prec);
  pdbRecordType->papFldDes[338]->size=sizeof(prec->p4pa);
  pdbRecordType->papFldDes[338]->offset=(short)((char *)&prec->p4pa - (char *)prec);
  pdbRecordType->papFldDes[339]->size=sizeof(prec->p4ra);
  pdbRecordType->papFldDes[339]->offset=(short)((char *)&prec->p4ra - (char *)prec);
  pdbRecordType->papFldDes[340]->size=sizeof(prec->p4ca);
  pdbRecordType->papFldDes[340]->offset=(short)((char *)&prec->p4ca - (char *)prec);
  pdbRecordType->papFldDes[341]->size=sizeof(prec->p4fs);
  pdbRecordType->papFldDes[341]->offset=(short)((char *)&prec->p4fs - (char *)prec);
  pdbRecordType->papFldDes[342]->size=sizeof(prec->p4fi);
  pdbRecordType->papFldDes[342]->offset=(short)((char *)&prec->p4fi - (char *)prec);
  pdbRecordType->papFldDes[343]->size=sizeof(prec->p4fe);
  pdbRecordType->papFldDes[343]->offset=(short)((char *)&prec->p4fe - (char *)prec);
  pdbRecordType->papFldDes[344]->size=sizeof(prec->p4fc);
  pdbRecordType->papFldDes[344]->offset=(short)((char *)&prec->p4fc - (char *)prec);
  pdbRecordType->papFldDes[345]->size=sizeof(prec->p4fw);
  pdbRecordType->papFldDes[345]->offset=(short)((char *)&prec->p4fw - (char *)prec);
  pdbRecordType->papFldDes[346]->size=sizeof(prec->p4sm);
  pdbRecordType->papFldDes[346]->offset=(short)((char *)&prec->p4sm - (char *)prec);
  pdbRecordType->papFldDes[347]->size=sizeof(prec->p4ar);
  pdbRecordType->papFldDes[347]->offset=(short)((char *)&prec->p4ar - (char *)prec);
  pdbRecordType->papFldDes[348]->size=sizeof(prec->p4eu);
  pdbRecordType->papFldDes[348]->offset=(short)((char *)&prec->p4eu - (char *)prec);
  pdbRecordType->papFldDes[349]->size=sizeof(prec->p4pr);
  pdbRecordType->papFldDes[349]->offset=(short)((char *)&prec->p4pr - (char *)prec);
  pdbRecordType->papFldDes[350]->size=sizeof(prec->d01hr);
  pdbRecordType->papFldDes[350]->offset=(short)((char *)&prec->d01hr - (char *)prec);
  pdbRecordType->papFldDes[351]->size=sizeof(prec->d01lr);
  pdbRecordType->papFldDes[351]->offset=(short)((char *)&prec->d01lr - (char *)prec);
  pdbRecordType->papFldDes[352]->size=sizeof(prec->d01da);
  pdbRecordType->papFldDes[352]->offset=(short)((char *)&prec->d01da - (char *)prec);
  pdbRecordType->papFldDes[353]->size=sizeof(prec->d01ca);
  pdbRecordType->papFldDes[353]->offset=(short)((char *)&prec->d01ca - (char *)prec);
  pdbRecordType->papFldDes[354]->size=sizeof(prec->d01cv);
  pdbRecordType->papFldDes[354]->offset=(short)((char *)&prec->d01cv - (char *)prec);
  pdbRecordType->papFldDes[355]->size=sizeof(prec->d01lv);
  pdbRecordType->papFldDes[355]->offset=(short)((char *)&prec->d01lv - (char *)prec);
  pdbRecordType->papFldDes[356]->size=sizeof(prec->d01ne);
  pdbRecordType->papFldDes[356]->offset=(short)((char *)&prec->d01ne - (char *)prec);
  pdbRecordType->papFldDes[357]->size=sizeof(prec->d01eu);
  pdbRecordType->papFldDes[357]->offset=(short)((char *)&prec->d01eu - (char *)prec);
  pdbRecordType->papFldDes[358]->size=sizeof(prec->d01pr);
  pdbRecordType->papFldDes[358]->offset=(short)((char *)&prec->d01pr - (char *)prec);
  pdbRecordType->papFldDes[359]->size=sizeof(prec->d02hr);
  pdbRecordType->papFldDes[359]->offset=(short)((char *)&prec->d02hr - (char *)prec);
  pdbRecordType->papFldDes[360]->size=sizeof(prec->d02lr);
  pdbRecordType->papFldDes[360]->offset=(short)((char *)&prec->d02lr - (char *)prec);
  pdbRecordType->papFldDes[361]->size=sizeof(prec->d02da);
  pdbRecordType->papFldDes[361]->offset=(short)((char *)&prec->d02da - (char *)prec);
  pdbRecordType->papFldDes[362]->size=sizeof(prec->d02ca);
  pdbRecordType->papFldDes[362]->offset=(short)((char *)&prec->d02ca - (char *)prec);
  pdbRecordType->papFldDes[363]->size=sizeof(prec->d02cv);
  pdbRecordType->papFldDes[363]->offset=(short)((char *)&prec->d02cv - (char *)prec);
  pdbRecordType->papFldDes[364]->size=sizeof(prec->d02lv);
  pdbRecordType->papFldDes[364]->offset=(short)((char *)&prec->d02lv - (char *)prec);
  pdbRecordType->papFldDes[365]->size=sizeof(prec->d02ne);
  pdbRecordType->papFldDes[365]->offset=(short)((char *)&prec->d02ne - (char *)prec);
  pdbRecordType->papFldDes[366]->size=sizeof(prec->d02eu);
  pdbRecordType->papFldDes[366]->offset=(short)((char *)&prec->d02eu - (char *)prec);
  pdbRecordType->papFldDes[367]->size=sizeof(prec->d02pr);
  pdbRecordType->papFldDes[367]->offset=(short)((char *)&prec->d02pr - (char *)prec);
  pdbRecordType->papFldDes[368]->size=sizeof(prec->d03hr);
  pdbRecordType->papFldDes[368]->offset=(short)((char *)&prec->d03hr - (char *)prec);
  pdbRecordType->papFldDes[369]->size=sizeof(prec->d03lr);
  pdbRecordType->papFldDes[369]->offset=(short)((char *)&prec->d03lr - (char *)prec);
  pdbRecordType->papFldDes[370]->size=sizeof(prec->d03da);
  pdbRecordType->papFldDes[370]->offset=(short)((char *)&prec->d03da - (char *)prec);
  pdbRecordType->papFldDes[371]->size=sizeof(prec->d03ca);
  pdbRecordType->papFldDes[371]->offset=(short)((char *)&prec->d03ca - (char *)prec);
  pdbRecordType->papFldDes[372]->size=sizeof(prec->d03cv);
  pdbRecordType->papFldDes[372]->offset=(short)((char *)&prec->d03cv - (char *)prec);
  pdbRecordType->papFldDes[373]->size=sizeof(prec->d03lv);
  pdbRecordType->papFldDes[373]->offset=(short)((char *)&prec->d03lv - (char *)prec);
  pdbRecordType->papFldDes[374]->size=sizeof(prec->d03ne);
  pdbRecordType->papFldDes[374]->offset=(short)((char *)&prec->d03ne - (char *)prec);
  pdbRecordType->papFldDes[375]->size=sizeof(prec->d03eu);
  pdbRecordType->papFldDes[375]->offset=(short)((char *)&prec->d03eu - (char *)prec);
  pdbRecordType->papFldDes[376]->size=sizeof(prec->d03pr);
  pdbRecordType->papFldDes[376]->offset=(short)((char *)&prec->d03pr - (char *)prec);
  pdbRecordType->papFldDes[377]->size=sizeof(prec->d04hr);
  pdbRecordType->papFldDes[377]->offset=(short)((char *)&prec->d04hr - (char *)prec);
  pdbRecordType->papFldDes[378]->size=sizeof(prec->d04lr);
  pdbRecordType->papFldDes[378]->offset=(short)((char *)&prec->d04lr - (char *)prec);
  pdbRecordType->papFldDes[379]->size=sizeof(prec->d04da);
  pdbRecordType->papFldDes[379]->offset=(short)((char *)&prec->d04da - (char *)prec);
  pdbRecordType->papFldDes[380]->size=sizeof(prec->d04ca);
  pdbRecordType->papFldDes[380]->offset=(short)((char *)&prec->d04ca - (char *)prec);
  pdbRecordType->papFldDes[381]->size=sizeof(prec->d04cv);
  pdbRecordType->papFldDes[381]->offset=(short)((char *)&prec->d04cv - (char *)prec);
  pdbRecordType->papFldDes[382]->size=sizeof(prec->d04lv);
  pdbRecordType->papFldDes[382]->offset=(short)((char *)&prec->d04lv - (char *)prec);
  pdbRecordType->papFldDes[383]->size=sizeof(prec->d04ne);
  pdbRecordType->papFldDes[383]->offset=(short)((char *)&prec->d04ne - (char *)prec);
  pdbRecordType->papFldDes[384]->size=sizeof(prec->d04eu);
  pdbRecordType->papFldDes[384]->offset=(short)((char *)&prec->d04eu - (char *)prec);
  pdbRecordType->papFldDes[385]->size=sizeof(prec->d04pr);
  pdbRecordType->papFldDes[385]->offset=(short)((char *)&prec->d04pr - (char *)prec);
  pdbRecordType->papFldDes[386]->size=sizeof(prec->d05hr);
  pdbRecordType->papFldDes[386]->offset=(short)((char *)&prec->d05hr - (char *)prec);
  pdbRecordType->papFldDes[387]->size=sizeof(prec->d05lr);
  pdbRecordType->papFldDes[387]->offset=(short)((char *)&prec->d05lr - (char *)prec);
  pdbRecordType->papFldDes[388]->size=sizeof(prec->d05da);
  pdbRecordType->papFldDes[388]->offset=(short)((char *)&prec->d05da - (char *)prec);
  pdbRecordType->papFldDes[389]->size=sizeof(prec->d05ca);
  pdbRecordType->papFldDes[389]->offset=(short)((char *)&prec->d05ca - (char *)prec);
  pdbRecordType->papFldDes[390]->size=sizeof(prec->d05cv);
  pdbRecordType->papFldDes[390]->offset=(short)((char *)&prec->d05cv - (char *)prec);
  pdbRecordType->papFldDes[391]->size=sizeof(prec->d05lv);
  pdbRecordType->papFldDes[391]->offset=(short)((char *)&prec->d05lv - (char *)prec);
  pdbRecordType->papFldDes[392]->size=sizeof(prec->d05ne);
  pdbRecordType->papFldDes[392]->offset=(short)((char *)&prec->d05ne - (char *)prec);
  pdbRecordType->papFldDes[393]->size=sizeof(prec->d05eu);
  pdbRecordType->papFldDes[393]->offset=(short)((char *)&prec->d05eu - (char *)prec);
  pdbRecordType->papFldDes[394]->size=sizeof(prec->d05pr);
  pdbRecordType->papFldDes[394]->offset=(short)((char *)&prec->d05pr - (char *)prec);
  pdbRecordType->papFldDes[395]->size=sizeof(prec->d06hr);
  pdbRecordType->papFldDes[395]->offset=(short)((char *)&prec->d06hr - (char *)prec);
  pdbRecordType->papFldDes[396]->size=sizeof(prec->d06lr);
  pdbRecordType->papFldDes[396]->offset=(short)((char *)&prec->d06lr - (char *)prec);
  pdbRecordType->papFldDes[397]->size=sizeof(prec->d06da);
  pdbRecordType->papFldDes[397]->offset=(short)((char *)&prec->d06da - (char *)prec);
  pdbRecordType->papFldDes[398]->size=sizeof(prec->d06ca);
  pdbRecordType->papFldDes[398]->offset=(short)((char *)&prec->d06ca - (char *)prec);
  pdbRecordType->papFldDes[399]->size=sizeof(prec->d06cv);
  pdbRecordType->papFldDes[399]->offset=(short)((char *)&prec->d06cv - (char *)prec);
  pdbRecordType->papFldDes[400]->size=sizeof(prec->d06lv);
  pdbRecordType->papFldDes[400]->offset=(short)((char *)&prec->d06lv - (char *)prec);
  pdbRecordType->papFldDes[401]->size=sizeof(prec->d06ne);
  pdbRecordType->papFldDes[401]->offset=(short)((char *)&prec->d06ne - (char *)prec);
  pdbRecordType->papFldDes[402]->size=sizeof(prec->d06eu);
  pdbRecordType->papFldDes[402]->offset=(short)((char *)&prec->d06eu - (char *)prec);
  pdbRecordType->papFldDes[403]->size=sizeof(prec->d06pr);
  pdbRecordType->papFldDes[403]->offset=(short)((char *)&prec->d06pr - (char *)prec);
  pdbRecordType->papFldDes[404]->size=sizeof(prec->d07hr);
  pdbRecordType->papFldDes[404]->offset=(short)((char *)&prec->d07hr - (char *)prec);
  pdbRecordType->papFldDes[405]->size=sizeof(prec->d07lr);
  pdbRecordType->papFldDes[405]->offset=(short)((char *)&prec->d07lr - (char *)prec);
  pdbRecordType->papFldDes[406]->size=sizeof(prec->d07da);
  pdbRecordType->papFldDes[406]->offset=(short)((char *)&prec->d07da - (char *)prec);
  pdbRecordType->papFldDes[407]->size=sizeof(prec->d07ca);
  pdbRecordType->papFldDes[407]->offset=(short)((char *)&prec->d07ca - (char *)prec);
  pdbRecordType->papFldDes[408]->size=sizeof(prec->d07cv);
  pdbRecordType->papFldDes[408]->offset=(short)((char *)&prec->d07cv - (char *)prec);
  pdbRecordType->papFldDes[409]->size=sizeof(prec->d07lv);
  pdbRecordType->papFldDes[409]->offset=(short)((char *)&prec->d07lv - (char *)prec);
  pdbRecordType->papFldDes[410]->size=sizeof(prec->d07ne);
  pdbRecordType->papFldDes[410]->offset=(short)((char *)&prec->d07ne - (char *)prec);
  pdbRecordType->papFldDes[411]->size=sizeof(prec->d07eu);
  pdbRecordType->papFldDes[411]->offset=(short)((char *)&prec->d07eu - (char *)prec);
  pdbRecordType->papFldDes[412]->size=sizeof(prec->d07pr);
  pdbRecordType->papFldDes[412]->offset=(short)((char *)&prec->d07pr - (char *)prec);
  pdbRecordType->papFldDes[413]->size=sizeof(prec->d08hr);
  pdbRecordType->papFldDes[413]->offset=(short)((char *)&prec->d08hr - (char *)prec);
  pdbRecordType->papFldDes[414]->size=sizeof(prec->d08lr);
  pdbRecordType->papFldDes[414]->offset=(short)((char *)&prec->d08lr - (char *)prec);
  pdbRecordType->papFldDes[415]->size=sizeof(prec->d08da);
  pdbRecordType->papFldDes[415]->offset=(short)((char *)&prec->d08da - (char *)prec);
  pdbRecordType->papFldDes[416]->size=sizeof(prec->d08ca);
  pdbRecordType->papFldDes[416]->offset=(short)((char *)&prec->d08ca - (char *)prec);
  pdbRecordType->papFldDes[417]->size=sizeof(prec->d08cv);
  pdbRecordType->papFldDes[417]->offset=(short)((char *)&prec->d08cv - (char *)prec);
  pdbRecordType->papFldDes[418]->size=sizeof(prec->d08lv);
  pdbRecordType->papFldDes[418]->offset=(short)((char *)&prec->d08lv - (char *)prec);
  pdbRecordType->papFldDes[419]->size=sizeof(prec->d08ne);
  pdbRecordType->papFldDes[419]->offset=(short)((char *)&prec->d08ne - (char *)prec);
  pdbRecordType->papFldDes[420]->size=sizeof(prec->d08eu);
  pdbRecordType->papFldDes[420]->offset=(short)((char *)&prec->d08eu - (char *)prec);
  pdbRecordType->papFldDes[421]->size=sizeof(prec->d08pr);
  pdbRecordType->papFldDes[421]->offset=(short)((char *)&prec->d08pr - (char *)prec);
  pdbRecordType->papFldDes[422]->size=sizeof(prec->d09hr);
  pdbRecordType->papFldDes[422]->offset=(short)((char *)&prec->d09hr - (char *)prec);
  pdbRecordType->papFldDes[423]->size=sizeof(prec->d09lr);
  pdbRecordType->papFldDes[423]->offset=(short)((char *)&prec->d09lr - (char *)prec);
  pdbRecordType->papFldDes[424]->size=sizeof(prec->d09da);
  pdbRecordType->papFldDes[424]->offset=(short)((char *)&prec->d09da - (char *)prec);
  pdbRecordType->papFldDes[425]->size=sizeof(prec->d09ca);
  pdbRecordType->papFldDes[425]->offset=(short)((char *)&prec->d09ca - (char *)prec);
  pdbRecordType->papFldDes[426]->size=sizeof(prec->d09cv);
  pdbRecordType->papFldDes[426]->offset=(short)((char *)&prec->d09cv - (char *)prec);
  pdbRecordType->papFldDes[427]->size=sizeof(prec->d09lv);
  pdbRecordType->papFldDes[427]->offset=(short)((char *)&prec->d09lv - (char *)prec);
  pdbRecordType->papFldDes[428]->size=sizeof(prec->d09ne);
  pdbRecordType->papFldDes[428]->offset=(short)((char *)&prec->d09ne - (char *)prec);
  pdbRecordType->papFldDes[429]->size=sizeof(prec->d09eu);
  pdbRecordType->papFldDes[429]->offset=(short)((char *)&prec->d09eu - (char *)prec);
  pdbRecordType->papFldDes[430]->size=sizeof(prec->d09pr);
  pdbRecordType->papFldDes[430]->offset=(short)((char *)&prec->d09pr - (char *)prec);
  pdbRecordType->papFldDes[431]->size=sizeof(prec->d10hr);
  pdbRecordType->papFldDes[431]->offset=(short)((char *)&prec->d10hr - (char *)prec);
  pdbRecordType->papFldDes[432]->size=sizeof(prec->d10lr);
  pdbRecordType->papFldDes[432]->offset=(short)((char *)&prec->d10lr - (char *)prec);
  pdbRecordType->papFldDes[433]->size=sizeof(prec->d10da);
  pdbRecordType->papFldDes[433]->offset=(short)((char *)&prec->d10da - (char *)prec);
  pdbRecordType->papFldDes[434]->size=sizeof(prec->d10ca);
  pdbRecordType->papFldDes[434]->offset=(short)((char *)&prec->d10ca - (char *)prec);
  pdbRecordType->papFldDes[435]->size=sizeof(prec->d10cv);
  pdbRecordType->papFldDes[435]->offset=(short)((char *)&prec->d10cv - (char *)prec);
  pdbRecordType->papFldDes[436]->size=sizeof(prec->d10lv);
  pdbRecordType->papFldDes[436]->offset=(short)((char *)&prec->d10lv - (char *)prec);
  pdbRecordType->papFldDes[437]->size=sizeof(prec->d10ne);
  pdbRecordType->papFldDes[437]->offset=(short)((char *)&prec->d10ne - (char *)prec);
  pdbRecordType->papFldDes[438]->size=sizeof(prec->d10eu);
  pdbRecordType->papFldDes[438]->offset=(short)((char *)&prec->d10eu - (char *)prec);
  pdbRecordType->papFldDes[439]->size=sizeof(prec->d10pr);
  pdbRecordType->papFldDes[439]->offset=(short)((char *)&prec->d10pr - (char *)prec);
  pdbRecordType->papFldDes[440]->size=sizeof(prec->d11hr);
  pdbRecordType->papFldDes[440]->offset=(short)((char *)&prec->d11hr - (char *)prec);
  pdbRecordType->papFldDes[441]->size=sizeof(prec->d11lr);
  pdbRecordType->papFldDes[441]->offset=(short)((char *)&prec->d11lr - (char *)prec);
  pdbRecordType->papFldDes[442]->size=sizeof(prec->d11da);
  pdbRecordType->papFldDes[442]->offset=(short)((char *)&prec->d11da - (char *)prec);
  pdbRecordType->papFldDes[443]->size=sizeof(prec->d11ca);
  pdbRecordType->papFldDes[443]->offset=(short)((char *)&prec->d11ca - (char *)prec);
  pdbRecordType->papFldDes[444]->size=sizeof(prec->d11cv);
  pdbRecordType->papFldDes[444]->offset=(short)((char *)&prec->d11cv - (char *)prec);
  pdbRecordType->papFldDes[445]->size=sizeof(prec->d11lv);
  pdbRecordType->papFldDes[445]->offset=(short)((char *)&prec->d11lv - (char *)prec);
  pdbRecordType->papFldDes[446]->size=sizeof(prec->d11ne);
  pdbRecordType->papFldDes[446]->offset=(short)((char *)&prec->d11ne - (char *)prec);
  pdbRecordType->papFldDes[447]->size=sizeof(prec->d11eu);
  pdbRecordType->papFldDes[447]->offset=(short)((char *)&prec->d11eu - (char *)prec);
  pdbRecordType->papFldDes[448]->size=sizeof(prec->d11pr);
  pdbRecordType->papFldDes[448]->offset=(short)((char *)&prec->d11pr - (char *)prec);
  pdbRecordType->papFldDes[449]->size=sizeof(prec->d12hr);
  pdbRecordType->papFldDes[449]->offset=(short)((char *)&prec->d12hr - (char *)prec);
  pdbRecordType->papFldDes[450]->size=sizeof(prec->d12lr);
  pdbRecordType->papFldDes[450]->offset=(short)((char *)&prec->d12lr - (char *)prec);
  pdbRecordType->papFldDes[451]->size=sizeof(prec->d12da);
  pdbRecordType->papFldDes[451]->offset=(short)((char *)&prec->d12da - (char *)prec);
  pdbRecordType->papFldDes[452]->size=sizeof(prec->d12ca);
  pdbRecordType->papFldDes[452]->offset=(short)((char *)&prec->d12ca - (char *)prec);
  pdbRecordType->papFldDes[453]->size=sizeof(prec->d12cv);
  pdbRecordType->papFldDes[453]->offset=(short)((char *)&prec->d12cv - (char *)prec);
  pdbRecordType->papFldDes[454]->size=sizeof(prec->d12lv);
  pdbRecordType->papFldDes[454]->offset=(short)((char *)&prec->d12lv - (char *)prec);
  pdbRecordType->papFldDes[455]->size=sizeof(prec->d12ne);
  pdbRecordType->papFldDes[455]->offset=(short)((char *)&prec->d12ne - (char *)prec);
  pdbRecordType->papFldDes[456]->size=sizeof(prec->d12eu);
  pdbRecordType->papFldDes[456]->offset=(short)((char *)&prec->d12eu - (char *)prec);
  pdbRecordType->papFldDes[457]->size=sizeof(prec->d12pr);
  pdbRecordType->papFldDes[457]->offset=(short)((char *)&prec->d12pr - (char *)prec);
  pdbRecordType->papFldDes[458]->size=sizeof(prec->d13hr);
  pdbRecordType->papFldDes[458]->offset=(short)((char *)&prec->d13hr - (char *)prec);
  pdbRecordType->papFldDes[459]->size=sizeof(prec->d13lr);
  pdbRecordType->papFldDes[459]->offset=(short)((char *)&prec->d13lr - (char *)prec);
  pdbRecordType->papFldDes[460]->size=sizeof(prec->d13da);
  pdbRecordType->papFldDes[460]->offset=(short)((char *)&prec->d13da - (char *)prec);
  pdbRecordType->papFldDes[461]->size=sizeof(prec->d13ca);
  pdbRecordType->papFldDes[461]->offset=(short)((char *)&prec->d13ca - (char *)prec);
  pdbRecordType->papFldDes[462]->size=sizeof(prec->d13cv);
  pdbRecordType->papFldDes[462]->offset=(short)((char *)&prec->d13cv - (char *)prec);
  pdbRecordType->papFldDes[463]->size=sizeof(prec->d13lv);
  pdbRecordType->papFldDes[463]->offset=(short)((char *)&prec->d13lv - (char *)prec);
  pdbRecordType->papFldDes[464]->size=sizeof(prec->d13ne);
  pdbRecordType->papFldDes[464]->offset=(short)((char *)&prec->d13ne - (char *)prec);
  pdbRecordType->papFldDes[465]->size=sizeof(prec->d13eu);
  pdbRecordType->papFldDes[465]->offset=(short)((char *)&prec->d13eu - (char *)prec);
  pdbRecordType->papFldDes[466]->size=sizeof(prec->d13pr);
  pdbRecordType->papFldDes[466]->offset=(short)((char *)&prec->d13pr - (char *)prec);
  pdbRecordType->papFldDes[467]->size=sizeof(prec->d14hr);
  pdbRecordType->papFldDes[467]->offset=(short)((char *)&prec->d14hr - (char *)prec);
  pdbRecordType->papFldDes[468]->size=sizeof(prec->d14lr);
  pdbRecordType->papFldDes[468]->offset=(short)((char *)&prec->d14lr - (char *)prec);
  pdbRecordType->papFldDes[469]->size=sizeof(prec->d14da);
  pdbRecordType->papFldDes[469]->offset=(short)((char *)&prec->d14da - (char *)prec);
  pdbRecordType->papFldDes[470]->size=sizeof(prec->d14ca);
  pdbRecordType->papFldDes[470]->offset=(short)((char *)&prec->d14ca - (char *)prec);
  pdbRecordType->papFldDes[471]->size=sizeof(prec->d14cv);
  pdbRecordType->papFldDes[471]->offset=(short)((char *)&prec->d14cv - (char *)prec);
  pdbRecordType->papFldDes[472]->size=sizeof(prec->d14lv);
  pdbRecordType->papFldDes[472]->offset=(short)((char *)&prec->d14lv - (char *)prec);
  pdbRecordType->papFldDes[473]->size=sizeof(prec->d14ne);
  pdbRecordType->papFldDes[473]->offset=(short)((char *)&prec->d14ne - (char *)prec);
  pdbRecordType->papFldDes[474]->size=sizeof(prec->d14eu);
  pdbRecordType->papFldDes[474]->offset=(short)((char *)&prec->d14eu - (char *)prec);
  pdbRecordType->papFldDes[475]->size=sizeof(prec->d14pr);
  pdbRecordType->papFldDes[475]->offset=(short)((char *)&prec->d14pr - (char *)prec);
  pdbRecordType->papFldDes[476]->size=sizeof(prec->d15hr);
  pdbRecordType->papFldDes[476]->offset=(short)((char *)&prec->d15hr - (char *)prec);
  pdbRecordType->papFldDes[477]->size=sizeof(prec->d15lr);
  pdbRecordType->papFldDes[477]->offset=(short)((char *)&prec->d15lr - (char *)prec);
  pdbRecordType->papFldDes[478]->size=sizeof(prec->d15da);
  pdbRecordType->papFldDes[478]->offset=(short)((char *)&prec->d15da - (char *)prec);
  pdbRecordType->papFldDes[479]->size=sizeof(prec->d15ca);
  pdbRecordType->papFldDes[479]->offset=(short)((char *)&prec->d15ca - (char *)prec);
  pdbRecordType->papFldDes[480]->size=sizeof(prec->d15cv);
  pdbRecordType->papFldDes[480]->offset=(short)((char *)&prec->d15cv - (char *)prec);
  pdbRecordType->papFldDes[481]->size=sizeof(prec->d15lv);
  pdbRecordType->papFldDes[481]->offset=(short)((char *)&prec->d15lv - (char *)prec);
  pdbRecordType->papFldDes[482]->size=sizeof(prec->d15ne);
  pdbRecordType->papFldDes[482]->offset=(short)((char *)&prec->d15ne - (char *)prec);
  pdbRecordType->papFldDes[483]->size=sizeof(prec->d15eu);
  pdbRecordType->papFldDes[483]->offset=(short)((char *)&prec->d15eu - (char *)prec);
  pdbRecordType->papFldDes[484]->size=sizeof(prec->d15pr);
  pdbRecordType->papFldDes[484]->offset=(short)((char *)&prec->d15pr - (char *)prec);
  pdbRecordType->papFldDes[485]->size=sizeof(prec->d16hr);
  pdbRecordType->papFldDes[485]->offset=(short)((char *)&prec->d16hr - (char *)prec);
  pdbRecordType->papFldDes[486]->size=sizeof(prec->d16lr);
  pdbRecordType->papFldDes[486]->offset=(short)((char *)&prec->d16lr - (char *)prec);
  pdbRecordType->papFldDes[487]->size=sizeof(prec->d16da);
  pdbRecordType->papFldDes[487]->offset=(short)((char *)&prec->d16da - (char *)prec);
  pdbRecordType->papFldDes[488]->size=sizeof(prec->d16ca);
  pdbRecordType->papFldDes[488]->offset=(short)((char *)&prec->d16ca - (char *)prec);
  pdbRecordType->papFldDes[489]->size=sizeof(prec->d16cv);
  pdbRecordType->papFldDes[489]->offset=(short)((char *)&prec->d16cv - (char *)prec);
  pdbRecordType->papFldDes[490]->size=sizeof(prec->d16lv);
  pdbRecordType->papFldDes[490]->offset=(short)((char *)&prec->d16lv - (char *)prec);
  pdbRecordType->papFldDes[491]->size=sizeof(prec->d16ne);
  pdbRecordType->papFldDes[491]->offset=(short)((char *)&prec->d16ne - (char *)prec);
  pdbRecordType->papFldDes[492]->size=sizeof(prec->d16eu);
  pdbRecordType->papFldDes[492]->offset=(short)((char *)&prec->d16eu - (char *)prec);
  pdbRecordType->papFldDes[493]->size=sizeof(prec->d16pr);
  pdbRecordType->papFldDes[493]->offset=(short)((char *)&prec->d16pr - (char *)prec);
  pdbRecordType->papFldDes[494]->size=sizeof(prec->d17hr);
  pdbRecordType->papFldDes[494]->offset=(short)((char *)&prec->d17hr - (char *)prec);
  pdbRecordType->papFldDes[495]->size=sizeof(prec->d17lr);
  pdbRecordType->papFldDes[495]->offset=(short)((char *)&prec->d17lr - (char *)prec);
  pdbRecordType->papFldDes[496]->size=sizeof(prec->d17da);
  pdbRecordType->papFldDes[496]->offset=(short)((char *)&prec->d17da - (char *)prec);
  pdbRecordType->papFldDes[497]->size=sizeof(prec->d17ca);
  pdbRecordType->papFldDes[497]->offset=(short)((char *)&prec->d17ca - (char *)prec);
  pdbRecordType->papFldDes[498]->size=sizeof(prec->d17cv);
  pdbRecordType->papFldDes[498]->offset=(short)((char *)&prec->d17cv - (char *)prec);
  pdbRecordType->papFldDes[499]->size=sizeof(prec->d17lv);
  pdbRecordType->papFldDes[499]->offset=(short)((char *)&prec->d17lv - (char *)prec);
  pdbRecordType->papFldDes[500]->size=sizeof(prec->d17ne);
  pdbRecordType->papFldDes[500]->offset=(short)((char *)&prec->d17ne - (char *)prec);
  pdbRecordType->papFldDes[501]->size=sizeof(prec->d17eu);
  pdbRecordType->papFldDes[501]->offset=(short)((char *)&prec->d17eu - (char *)prec);
  pdbRecordType->papFldDes[502]->size=sizeof(prec->d17pr);
  pdbRecordType->papFldDes[502]->offset=(short)((char *)&prec->d17pr - (char *)prec);
  pdbRecordType->papFldDes[503]->size=sizeof(prec->d18hr);
  pdbRecordType->papFldDes[503]->offset=(short)((char *)&prec->d18hr - (char *)prec);
  pdbRecordType->papFldDes[504]->size=sizeof(prec->d18lr);
  pdbRecordType->papFldDes[504]->offset=(short)((char *)&prec->d18lr - (char *)prec);
  pdbRecordType->papFldDes[505]->size=sizeof(prec->d18da);
  pdbRecordType->papFldDes[505]->offset=(short)((char *)&prec->d18da - (char *)prec);
  pdbRecordType->papFldDes[506]->size=sizeof(prec->d18ca);
  pdbRecordType->papFldDes[506]->offset=(short)((char *)&prec->d18ca - (char *)prec);
  pdbRecordType->papFldDes[507]->size=sizeof(prec->d18cv);
  pdbRecordType->papFldDes[507]->offset=(short)((char *)&prec->d18cv - (char *)prec);
  pdbRecordType->papFldDes[508]->size=sizeof(prec->d18lv);
  pdbRecordType->papFldDes[508]->offset=(short)((char *)&prec->d18lv - (char *)prec);
  pdbRecordType->papFldDes[509]->size=sizeof(prec->d18ne);
  pdbRecordType->papFldDes[509]->offset=(short)((char *)&prec->d18ne - (char *)prec);
  pdbRecordType->papFldDes[510]->size=sizeof(prec->d18eu);
  pdbRecordType->papFldDes[510]->offset=(short)((char *)&prec->d18eu - (char *)prec);
  pdbRecordType->papFldDes[511]->size=sizeof(prec->d18pr);
  pdbRecordType->papFldDes[511]->offset=(short)((char *)&prec->d18pr - (char *)prec);
  pdbRecordType->papFldDes[512]->size=sizeof(prec->d19hr);
  pdbRecordType->papFldDes[512]->offset=(short)((char *)&prec->d19hr - (char *)prec);
  pdbRecordType->papFldDes[513]->size=sizeof(prec->d19lr);
  pdbRecordType->papFldDes[513]->offset=(short)((char *)&prec->d19lr - (char *)prec);
  pdbRecordType->papFldDes[514]->size=sizeof(prec->d19da);
  pdbRecordType->papFldDes[514]->offset=(short)((char *)&prec->d19da - (char *)prec);
  pdbRecordType->papFldDes[515]->size=sizeof(prec->d19ca);
  pdbRecordType->papFldDes[515]->offset=(short)((char *)&prec->d19ca - (char *)prec);
  pdbRecordType->papFldDes[516]->size=sizeof(prec->d19cv);
  pdbRecordType->papFldDes[516]->offset=(short)((char *)&prec->d19cv - (char *)prec);
  pdbRecordType->papFldDes[517]->size=sizeof(prec->d19lv);
  pdbRecordType->papFldDes[517]->offset=(short)((char *)&prec->d19lv - (char *)prec);
  pdbRecordType->papFldDes[518]->size=sizeof(prec->d19ne);
  pdbRecordType->papFldDes[518]->offset=(short)((char *)&prec->d19ne - (char *)prec);
  pdbRecordType->papFldDes[519]->size=sizeof(prec->d19eu);
  pdbRecordType->papFldDes[519]->offset=(short)((char *)&prec->d19eu - (char *)prec);
  pdbRecordType->papFldDes[520]->size=sizeof(prec->d19pr);
  pdbRecordType->papFldDes[520]->offset=(short)((char *)&prec->d19pr - (char *)prec);
  pdbRecordType->papFldDes[521]->size=sizeof(prec->d20hr);
  pdbRecordType->papFldDes[521]->offset=(short)((char *)&prec->d20hr - (char *)prec);
  pdbRecordType->papFldDes[522]->size=sizeof(prec->d20lr);
  pdbRecordType->papFldDes[522]->offset=(short)((char *)&prec->d20lr - (char *)prec);
  pdbRecordType->papFldDes[523]->size=sizeof(prec->d20da);
  pdbRecordType->papFldDes[523]->offset=(short)((char *)&prec->d20da - (char *)prec);
  pdbRecordType->papFldDes[524]->size=sizeof(prec->d20ca);
  pdbRecordType->papFldDes[524]->offset=(short)((char *)&prec->d20ca - (char *)prec);
  pdbRecordType->papFldDes[525]->size=sizeof(prec->d20cv);
  pdbRecordType->papFldDes[525]->offset=(short)((char *)&prec->d20cv - (char *)prec);
  pdbRecordType->papFldDes[526]->size=sizeof(prec->d20lv);
  pdbRecordType->papFldDes[526]->offset=(short)((char *)&prec->d20lv - (char *)prec);
  pdbRecordType->papFldDes[527]->size=sizeof(prec->d20ne);
  pdbRecordType->papFldDes[527]->offset=(short)((char *)&prec->d20ne - (char *)prec);
  pdbRecordType->papFldDes[528]->size=sizeof(prec->d20eu);
  pdbRecordType->papFldDes[528]->offset=(short)((char *)&prec->d20eu - (char *)prec);
  pdbRecordType->papFldDes[529]->size=sizeof(prec->d20pr);
  pdbRecordType->papFldDes[529]->offset=(short)((char *)&prec->d20pr - (char *)prec);
  pdbRecordType->papFldDes[530]->size=sizeof(prec->d21hr);
  pdbRecordType->papFldDes[530]->offset=(short)((char *)&prec->d21hr - (char *)prec);
  pdbRecordType->papFldDes[531]->size=sizeof(prec->d21lr);
  pdbRecordType->papFldDes[531]->offset=(short)((char *)&prec->d21lr - (char *)prec);
  pdbRecordType->papFldDes[532]->size=sizeof(prec->d21da);
  pdbRecordType->papFldDes[532]->offset=(short)((char *)&prec->d21da - (char *)prec);
  pdbRecordType->papFldDes[533]->size=sizeof(prec->d21ca);
  pdbRecordType->papFldDes[533]->offset=(short)((char *)&prec->d21ca - (char *)prec);
  pdbRecordType->papFldDes[534]->size=sizeof(prec->d21cv);
  pdbRecordType->papFldDes[534]->offset=(short)((char *)&prec->d21cv - (char *)prec);
  pdbRecordType->papFldDes[535]->size=sizeof(prec->d21lv);
  pdbRecordType->papFldDes[535]->offset=(short)((char *)&prec->d21lv - (char *)prec);
  pdbRecordType->papFldDes[536]->size=sizeof(prec->d21ne);
  pdbRecordType->papFldDes[536]->offset=(short)((char *)&prec->d21ne - (char *)prec);
  pdbRecordType->papFldDes[537]->size=sizeof(prec->d21eu);
  pdbRecordType->papFldDes[537]->offset=(short)((char *)&prec->d21eu - (char *)prec);
  pdbRecordType->papFldDes[538]->size=sizeof(prec->d21pr);
  pdbRecordType->papFldDes[538]->offset=(short)((char *)&prec->d21pr - (char *)prec);
  pdbRecordType->papFldDes[539]->size=sizeof(prec->d22hr);
  pdbRecordType->papFldDes[539]->offset=(short)((char *)&prec->d22hr - (char *)prec);
  pdbRecordType->papFldDes[540]->size=sizeof(prec->d22lr);
  pdbRecordType->papFldDes[540]->offset=(short)((char *)&prec->d22lr - (char *)prec);
  pdbRecordType->papFldDes[541]->size=sizeof(prec->d22da);
  pdbRecordType->papFldDes[541]->offset=(short)((char *)&prec->d22da - (char *)prec);
  pdbRecordType->papFldDes[542]->size=sizeof(prec->d22ca);
  pdbRecordType->papFldDes[542]->offset=(short)((char *)&prec->d22ca - (char *)prec);
  pdbRecordType->papFldDes[543]->size=sizeof(prec->d22cv);
  pdbRecordType->papFldDes[543]->offset=(short)((char *)&prec->d22cv - (char *)prec);
  pdbRecordType->papFldDes[544]->size=sizeof(prec->d22lv);
  pdbRecordType->papFldDes[544]->offset=(short)((char *)&prec->d22lv - (char *)prec);
  pdbRecordType->papFldDes[545]->size=sizeof(prec->d22ne);
  pdbRecordType->papFldDes[545]->offset=(short)((char *)&prec->d22ne - (char *)prec);
  pdbRecordType->papFldDes[546]->size=sizeof(prec->d22eu);
  pdbRecordType->papFldDes[546]->offset=(short)((char *)&prec->d22eu - (char *)prec);
  pdbRecordType->papFldDes[547]->size=sizeof(prec->d22pr);
  pdbRecordType->papFldDes[547]->offset=(short)((char *)&prec->d22pr - (char *)prec);
  pdbRecordType->papFldDes[548]->size=sizeof(prec->d23hr);
  pdbRecordType->papFldDes[548]->offset=(short)((char *)&prec->d23hr - (char *)prec);
  pdbRecordType->papFldDes[549]->size=sizeof(prec->d23lr);
  pdbRecordType->papFldDes[549]->offset=(short)((char *)&prec->d23lr - (char *)prec);
  pdbRecordType->papFldDes[550]->size=sizeof(prec->d23da);
  pdbRecordType->papFldDes[550]->offset=(short)((char *)&prec->d23da - (char *)prec);
  pdbRecordType->papFldDes[551]->size=sizeof(prec->d23ca);
  pdbRecordType->papFldDes[551]->offset=(short)((char *)&prec->d23ca - (char *)prec);
  pdbRecordType->papFldDes[552]->size=sizeof(prec->d23cv);
  pdbRecordType->papFldDes[552]->offset=(short)((char *)&prec->d23cv - (char *)prec);
  pdbRecordType->papFldDes[553]->size=sizeof(prec->d23lv);
  pdbRecordType->papFldDes[553]->offset=(short)((char *)&prec->d23lv - (char *)prec);
  pdbRecordType->papFldDes[554]->size=sizeof(prec->d23ne);
  pdbRecordType->papFldDes[554]->offset=(short)((char *)&prec->d23ne - (char *)prec);
  pdbRecordType->papFldDes[555]->size=sizeof(prec->d23eu);
  pdbRecordType->papFldDes[555]->offset=(short)((char *)&prec->d23eu - (char *)prec);
  pdbRecordType->papFldDes[556]->size=sizeof(prec->d23pr);
  pdbRecordType->papFldDes[556]->offset=(short)((char *)&prec->d23pr - (char *)prec);
  pdbRecordType->papFldDes[557]->size=sizeof(prec->d24hr);
  pdbRecordType->papFldDes[557]->offset=(short)((char *)&prec->d24hr - (char *)prec);
  pdbRecordType->papFldDes[558]->size=sizeof(prec->d24lr);
  pdbRecordType->papFldDes[558]->offset=(short)((char *)&prec->d24lr - (char *)prec);
  pdbRecordType->papFldDes[559]->size=sizeof(prec->d24da);
  pdbRecordType->papFldDes[559]->offset=(short)((char *)&prec->d24da - (char *)prec);
  pdbRecordType->papFldDes[560]->size=sizeof(prec->d24ca);
  pdbRecordType->papFldDes[560]->offset=(short)((char *)&prec->d24ca - (char *)prec);
  pdbRecordType->papFldDes[561]->size=sizeof(prec->d24cv);
  pdbRecordType->papFldDes[561]->offset=(short)((char *)&prec->d24cv - (char *)prec);
  pdbRecordType->papFldDes[562]->size=sizeof(prec->d24lv);
  pdbRecordType->papFldDes[562]->offset=(short)((char *)&prec->d24lv - (char *)prec);
  pdbRecordType->papFldDes[563]->size=sizeof(prec->d24ne);
  pdbRecordType->papFldDes[563]->offset=(short)((char *)&prec->d24ne - (char *)prec);
  pdbRecordType->papFldDes[564]->size=sizeof(prec->d24eu);
  pdbRecordType->papFldDes[564]->offset=(short)((char *)&prec->d24eu - (char *)prec);
  pdbRecordType->papFldDes[565]->size=sizeof(prec->d24pr);
  pdbRecordType->papFldDes[565]->offset=(short)((char *)&prec->d24pr - (char *)prec);
  pdbRecordType->papFldDes[566]->size=sizeof(prec->d25hr);
  pdbRecordType->papFldDes[566]->offset=(short)((char *)&prec->d25hr - (char *)prec);
  pdbRecordType->papFldDes[567]->size=sizeof(prec->d25lr);
  pdbRecordType->papFldDes[567]->offset=(short)((char *)&prec->d25lr - (char *)prec);
  pdbRecordType->papFldDes[568]->size=sizeof(prec->d25da);
  pdbRecordType->papFldDes[568]->offset=(short)((char *)&prec->d25da - (char *)prec);
  pdbRecordType->papFldDes[569]->size=sizeof(prec->d25ca);
  pdbRecordType->papFldDes[569]->offset=(short)((char *)&prec->d25ca - (char *)prec);
  pdbRecordType->papFldDes[570]->size=sizeof(prec->d25cv);
  pdbRecordType->papFldDes[570]->offset=(short)((char *)&prec->d25cv - (char *)prec);
  pdbRecordType->papFldDes[571]->size=sizeof(prec->d25lv);
  pdbRecordType->papFldDes[571]->offset=(short)((char *)&prec->d25lv - (char *)prec);
  pdbRecordType->papFldDes[572]->size=sizeof(prec->d25ne);
  pdbRecordType->papFldDes[572]->offset=(short)((char *)&prec->d25ne - (char *)prec);
  pdbRecordType->papFldDes[573]->size=sizeof(prec->d25eu);
  pdbRecordType->papFldDes[573]->offset=(short)((char *)&prec->d25eu - (char *)prec);
  pdbRecordType->papFldDes[574]->size=sizeof(prec->d25pr);
  pdbRecordType->papFldDes[574]->offset=(short)((char *)&prec->d25pr - (char *)prec);
  pdbRecordType->papFldDes[575]->size=sizeof(prec->d26hr);
  pdbRecordType->papFldDes[575]->offset=(short)((char *)&prec->d26hr - (char *)prec);
  pdbRecordType->papFldDes[576]->size=sizeof(prec->d26lr);
  pdbRecordType->papFldDes[576]->offset=(short)((char *)&prec->d26lr - (char *)prec);
  pdbRecordType->papFldDes[577]->size=sizeof(prec->d26da);
  pdbRecordType->papFldDes[577]->offset=(short)((char *)&prec->d26da - (char *)prec);
  pdbRecordType->papFldDes[578]->size=sizeof(prec->d26ca);
  pdbRecordType->papFldDes[578]->offset=(short)((char *)&prec->d26ca - (char *)prec);
  pdbRecordType->papFldDes[579]->size=sizeof(prec->d26cv);
  pdbRecordType->papFldDes[579]->offset=(short)((char *)&prec->d26cv - (char *)prec);
  pdbRecordType->papFldDes[580]->size=sizeof(prec->d26lv);
  pdbRecordType->papFldDes[580]->offset=(short)((char *)&prec->d26lv - (char *)prec);
  pdbRecordType->papFldDes[581]->size=sizeof(prec->d26ne);
  pdbRecordType->papFldDes[581]->offset=(short)((char *)&prec->d26ne - (char *)prec);
  pdbRecordType->papFldDes[582]->size=sizeof(prec->d26eu);
  pdbRecordType->papFldDes[582]->offset=(short)((char *)&prec->d26eu - (char *)prec);
  pdbRecordType->papFldDes[583]->size=sizeof(prec->d26pr);
  pdbRecordType->papFldDes[583]->offset=(short)((char *)&prec->d26pr - (char *)prec);
  pdbRecordType->papFldDes[584]->size=sizeof(prec->d27hr);
  pdbRecordType->papFldDes[584]->offset=(short)((char *)&prec->d27hr - (char *)prec);
  pdbRecordType->papFldDes[585]->size=sizeof(prec->d27lr);
  pdbRecordType->papFldDes[585]->offset=(short)((char *)&prec->d27lr - (char *)prec);
  pdbRecordType->papFldDes[586]->size=sizeof(prec->d27da);
  pdbRecordType->papFldDes[586]->offset=(short)((char *)&prec->d27da - (char *)prec);
  pdbRecordType->papFldDes[587]->size=sizeof(prec->d27ca);
  pdbRecordType->papFldDes[587]->offset=(short)((char *)&prec->d27ca - (char *)prec);
  pdbRecordType->papFldDes[588]->size=sizeof(prec->d27cv);
  pdbRecordType->papFldDes[588]->offset=(short)((char *)&prec->d27cv - (char *)prec);
  pdbRecordType->papFldDes[589]->size=sizeof(prec->d27lv);
  pdbRecordType->papFldDes[589]->offset=(short)((char *)&prec->d27lv - (char *)prec);
  pdbRecordType->papFldDes[590]->size=sizeof(prec->d27ne);
  pdbRecordType->papFldDes[590]->offset=(short)((char *)&prec->d27ne - (char *)prec);
  pdbRecordType->papFldDes[591]->size=sizeof(prec->d27eu);
  pdbRecordType->papFldDes[591]->offset=(short)((char *)&prec->d27eu - (char *)prec);
  pdbRecordType->papFldDes[592]->size=sizeof(prec->d27pr);
  pdbRecordType->papFldDes[592]->offset=(short)((char *)&prec->d27pr - (char *)prec);
  pdbRecordType->papFldDes[593]->size=sizeof(prec->d28hr);
  pdbRecordType->papFldDes[593]->offset=(short)((char *)&prec->d28hr - (char *)prec);
  pdbRecordType->papFldDes[594]->size=sizeof(prec->d28lr);
  pdbRecordType->papFldDes[594]->offset=(short)((char *)&prec->d28lr - (char *)prec);
  pdbRecordType->papFldDes[595]->size=sizeof(prec->d28da);
  pdbRecordType->papFldDes[595]->offset=(short)((char *)&prec->d28da - (char *)prec);
  pdbRecordType->papFldDes[596]->size=sizeof(prec->d28ca);
  pdbRecordType->papFldDes[596]->offset=(short)((char *)&prec->d28ca - (char *)prec);
  pdbRecordType->papFldDes[597]->size=sizeof(prec->d28cv);
  pdbRecordType->papFldDes[597]->offset=(short)((char *)&prec->d28cv - (char *)prec);
  pdbRecordType->papFldDes[598]->size=sizeof(prec->d28lv);
  pdbRecordType->papFldDes[598]->offset=(short)((char *)&prec->d28lv - (char *)prec);
  pdbRecordType->papFldDes[599]->size=sizeof(prec->d28ne);
  pdbRecordType->papFldDes[599]->offset=(short)((char *)&prec->d28ne - (char *)prec);
  pdbRecordType->papFldDes[600]->size=sizeof(prec->d28eu);
  pdbRecordType->papFldDes[600]->offset=(short)((char *)&prec->d28eu - (char *)prec);
  pdbRecordType->papFldDes[601]->size=sizeof(prec->d28pr);
  pdbRecordType->papFldDes[601]->offset=(short)((char *)&prec->d28pr - (char *)prec);
  pdbRecordType->papFldDes[602]->size=sizeof(prec->d29hr);
  pdbRecordType->papFldDes[602]->offset=(short)((char *)&prec->d29hr - (char *)prec);
  pdbRecordType->papFldDes[603]->size=sizeof(prec->d29lr);
  pdbRecordType->papFldDes[603]->offset=(short)((char *)&prec->d29lr - (char *)prec);
  pdbRecordType->papFldDes[604]->size=sizeof(prec->d29da);
  pdbRecordType->papFldDes[604]->offset=(short)((char *)&prec->d29da - (char *)prec);
  pdbRecordType->papFldDes[605]->size=sizeof(prec->d29ca);
  pdbRecordType->papFldDes[605]->offset=(short)((char *)&prec->d29ca - (char *)prec);
  pdbRecordType->papFldDes[606]->size=sizeof(prec->d29cv);
  pdbRecordType->papFldDes[606]->offset=(short)((char *)&prec->d29cv - (char *)prec);
  pdbRecordType->papFldDes[607]->size=sizeof(prec->d29lv);
  pdbRecordType->papFldDes[607]->offset=(short)((char *)&prec->d29lv - (char *)prec);
  pdbRecordType->papFldDes[608]->size=sizeof(prec->d29ne);
  pdbRecordType->papFldDes[608]->offset=(short)((char *)&prec->d29ne - (char *)prec);
  pdbRecordType->papFldDes[609]->size=sizeof(prec->d29eu);
  pdbRecordType->papFldDes[609]->offset=(short)((char *)&prec->d29eu - (char *)prec);
  pdbRecordType->papFldDes[610]->size=sizeof(prec->d29pr);
  pdbRecordType->papFldDes[610]->offset=(short)((char *)&prec->d29pr - (char *)prec);
  pdbRecordType->papFldDes[611]->size=sizeof(prec->d30hr);
  pdbRecordType->papFldDes[611]->offset=(short)((char *)&prec->d30hr - (char *)prec);
  pdbRecordType->papFldDes[612]->size=sizeof(prec->d30lr);
  pdbRecordType->papFldDes[612]->offset=(short)((char *)&prec->d30lr - (char *)prec);
  pdbRecordType->papFldDes[613]->size=sizeof(prec->d30da);
  pdbRecordType->papFldDes[613]->offset=(short)((char *)&prec->d30da - (char *)prec);
  pdbRecordType->papFldDes[614]->size=sizeof(prec->d30ca);
  pdbRecordType->papFldDes[614]->offset=(short)((char *)&prec->d30ca - (char *)prec);
  pdbRecordType->papFldDes[615]->size=sizeof(prec->d30cv);
  pdbRecordType->papFldDes[615]->offset=(short)((char *)&prec->d30cv - (char *)prec);
  pdbRecordType->papFldDes[616]->size=sizeof(prec->d30lv);
  pdbRecordType->papFldDes[616]->offset=(short)((char *)&prec->d30lv - (char *)prec);
  pdbRecordType->papFldDes[617]->size=sizeof(prec->d30ne);
  pdbRecordType->papFldDes[617]->offset=(short)((char *)&prec->d30ne - (char *)prec);
  pdbRecordType->papFldDes[618]->size=sizeof(prec->d30eu);
  pdbRecordType->papFldDes[618]->offset=(short)((char *)&prec->d30eu - (char *)prec);
  pdbRecordType->papFldDes[619]->size=sizeof(prec->d30pr);
  pdbRecordType->papFldDes[619]->offset=(short)((char *)&prec->d30pr - (char *)prec);
  pdbRecordType->papFldDes[620]->size=sizeof(prec->d31hr);
  pdbRecordType->papFldDes[620]->offset=(short)((char *)&prec->d31hr - (char *)prec);
  pdbRecordType->papFldDes[621]->size=sizeof(prec->d31lr);
  pdbRecordType->papFldDes[621]->offset=(short)((char *)&prec->d31lr - (char *)prec);
  pdbRecordType->papFldDes[622]->size=sizeof(prec->d31da);
  pdbRecordType->papFldDes[622]->offset=(short)((char *)&prec->d31da - (char *)prec);
  pdbRecordType->papFldDes[623]->size=sizeof(prec->d31ca);
  pdbRecordType->papFldDes[623]->offset=(short)((char *)&prec->d31ca - (char *)prec);
  pdbRecordType->papFldDes[624]->size=sizeof(prec->d31cv);
  pdbRecordType->papFldDes[624]->offset=(short)((char *)&prec->d31cv - (char *)prec);
  pdbRecordType->papFldDes[625]->size=sizeof(prec->d31lv);
  pdbRecordType->papFldDes[625]->offset=(short)((char *)&prec->d31lv - (char *)prec);
  pdbRecordType->papFldDes[626]->size=sizeof(prec->d31ne);
  pdbRecordType->papFldDes[626]->offset=(short)((char *)&prec->d31ne - (char *)prec);
  pdbRecordType->papFldDes[627]->size=sizeof(prec->d31eu);
  pdbRecordType->papFldDes[627]->offset=(short)((char *)&prec->d31eu - (char *)prec);
  pdbRecordType->papFldDes[628]->size=sizeof(prec->d31pr);
  pdbRecordType->papFldDes[628]->offset=(short)((char *)&prec->d31pr - (char *)prec);
  pdbRecordType->papFldDes[629]->size=sizeof(prec->d32hr);
  pdbRecordType->papFldDes[629]->offset=(short)((char *)&prec->d32hr - (char *)prec);
  pdbRecordType->papFldDes[630]->size=sizeof(prec->d32lr);
  pdbRecordType->papFldDes[630]->offset=(short)((char *)&prec->d32lr - (char *)prec);
  pdbRecordType->papFldDes[631]->size=sizeof(prec->d32da);
  pdbRecordType->papFldDes[631]->offset=(short)((char *)&prec->d32da - (char *)prec);
  pdbRecordType->papFldDes[632]->size=sizeof(prec->d32ca);
  pdbRecordType->papFldDes[632]->offset=(short)((char *)&prec->d32ca - (char *)prec);
  pdbRecordType->papFldDes[633]->size=sizeof(prec->d32cv);
  pdbRecordType->papFldDes[633]->offset=(short)((char *)&prec->d32cv - (char *)prec);
  pdbRecordType->papFldDes[634]->size=sizeof(prec->d32lv);
  pdbRecordType->papFldDes[634]->offset=(short)((char *)&prec->d32lv - (char *)prec);
  pdbRecordType->papFldDes[635]->size=sizeof(prec->d32ne);
  pdbRecordType->papFldDes[635]->offset=(short)((char *)&prec->d32ne - (char *)prec);
  pdbRecordType->papFldDes[636]->size=sizeof(prec->d32eu);
  pdbRecordType->papFldDes[636]->offset=(short)((char *)&prec->d32eu - (char *)prec);
  pdbRecordType->papFldDes[637]->size=sizeof(prec->d32pr);
  pdbRecordType->papFldDes[637]->offset=(short)((char *)&prec->d32pr - (char *)prec);
  pdbRecordType->papFldDes[638]->size=sizeof(prec->d33hr);
  pdbRecordType->papFldDes[638]->offset=(short)((char *)&prec->d33hr - (char *)prec);
  pdbRecordType->papFldDes[639]->size=sizeof(prec->d33lr);
  pdbRecordType->papFldDes[639]->offset=(short)((char *)&prec->d33lr - (char *)prec);
  pdbRecordType->papFldDes[640]->size=sizeof(prec->d33da);
  pdbRecordType->papFldDes[640]->offset=(short)((char *)&prec->d33da - (char *)prec);
  pdbRecordType->papFldDes[641]->size=sizeof(prec->d33ca);
  pdbRecordType->papFldDes[641]->offset=(short)((char *)&prec->d33ca - (char *)prec);
  pdbRecordType->papFldDes[642]->size=sizeof(prec->d33cv);
  pdbRecordType->papFldDes[642]->offset=(short)((char *)&prec->d33cv - (char *)prec);
  pdbRecordType->papFldDes[643]->size=sizeof(prec->d33lv);
  pdbRecordType->papFldDes[643]->offset=(short)((char *)&prec->d33lv - (char *)prec);
  pdbRecordType->papFldDes[644]->size=sizeof(prec->d33ne);
  pdbRecordType->papFldDes[644]->offset=(short)((char *)&prec->d33ne - (char *)prec);
  pdbRecordType->papFldDes[645]->size=sizeof(prec->d33eu);
  pdbRecordType->papFldDes[645]->offset=(short)((char *)&prec->d33eu - (char *)prec);
  pdbRecordType->papFldDes[646]->size=sizeof(prec->d33pr);
  pdbRecordType->papFldDes[646]->offset=(short)((char *)&prec->d33pr - (char *)prec);
  pdbRecordType->papFldDes[647]->size=sizeof(prec->d34hr);
  pdbRecordType->papFldDes[647]->offset=(short)((char *)&prec->d34hr - (char *)prec);
  pdbRecordType->papFldDes[648]->size=sizeof(prec->d34lr);
  pdbRecordType->papFldDes[648]->offset=(short)((char *)&prec->d34lr - (char *)prec);
  pdbRecordType->papFldDes[649]->size=sizeof(prec->d34da);
  pdbRecordType->papFldDes[649]->offset=(short)((char *)&prec->d34da - (char *)prec);
  pdbRecordType->papFldDes[650]->size=sizeof(prec->d34ca);
  pdbRecordType->papFldDes[650]->offset=(short)((char *)&prec->d34ca - (char *)prec);
  pdbRecordType->papFldDes[651]->size=sizeof(prec->d34cv);
  pdbRecordType->papFldDes[651]->offset=(short)((char *)&prec->d34cv - (char *)prec);
  pdbRecordType->papFldDes[652]->size=sizeof(prec->d34lv);
  pdbRecordType->papFldDes[652]->offset=(short)((char *)&prec->d34lv - (char *)prec);
  pdbRecordType->papFldDes[653]->size=sizeof(prec->d34ne);
  pdbRecordType->papFldDes[653]->offset=(short)((char *)&prec->d34ne - (char *)prec);
  pdbRecordType->papFldDes[654]->size=sizeof(prec->d34eu);
  pdbRecordType->papFldDes[654]->offset=(short)((char *)&prec->d34eu - (char *)prec);
  pdbRecordType->papFldDes[655]->size=sizeof(prec->d34pr);
  pdbRecordType->papFldDes[655]->offset=(short)((char *)&prec->d34pr - (char *)prec);
  pdbRecordType->papFldDes[656]->size=sizeof(prec->d35hr);
  pdbRecordType->papFldDes[656]->offset=(short)((char *)&prec->d35hr - (char *)prec);
  pdbRecordType->papFldDes[657]->size=sizeof(prec->d35lr);
  pdbRecordType->papFldDes[657]->offset=(short)((char *)&prec->d35lr - (char *)prec);
  pdbRecordType->papFldDes[658]->size=sizeof(prec->d35da);
  pdbRecordType->papFldDes[658]->offset=(short)((char *)&prec->d35da - (char *)prec);
  pdbRecordType->papFldDes[659]->size=sizeof(prec->d35ca);
  pdbRecordType->papFldDes[659]->offset=(short)((char *)&prec->d35ca - (char *)prec);
  pdbRecordType->papFldDes[660]->size=sizeof(prec->d35cv);
  pdbRecordType->papFldDes[660]->offset=(short)((char *)&prec->d35cv - (char *)prec);
  pdbRecordType->papFldDes[661]->size=sizeof(prec->d35lv);
  pdbRecordType->papFldDes[661]->offset=(short)((char *)&prec->d35lv - (char *)prec);
  pdbRecordType->papFldDes[662]->size=sizeof(prec->d35ne);
  pdbRecordType->papFldDes[662]->offset=(short)((char *)&prec->d35ne - (char *)prec);
  pdbRecordType->papFldDes[663]->size=sizeof(prec->d35eu);
  pdbRecordType->papFldDes[663]->offset=(short)((char *)&prec->d35eu - (char *)prec);
  pdbRecordType->papFldDes[664]->size=sizeof(prec->d35pr);
  pdbRecordType->papFldDes[664]->offset=(short)((char *)&prec->d35pr - (char *)prec);
  pdbRecordType->papFldDes[665]->size=sizeof(prec->d36hr);
  pdbRecordType->papFldDes[665]->offset=(short)((char *)&prec->d36hr - (char *)prec);
  pdbRecordType->papFldDes[666]->size=sizeof(prec->d36lr);
  pdbRecordType->papFldDes[666]->offset=(short)((char *)&prec->d36lr - (char *)prec);
  pdbRecordType->papFldDes[667]->size=sizeof(prec->d36da);
  pdbRecordType->papFldDes[667]->offset=(short)((char *)&prec->d36da - (char *)prec);
  pdbRecordType->papFldDes[668]->size=sizeof(prec->d36ca);
  pdbRecordType->papFldDes[668]->offset=(short)((char *)&prec->d36ca - (char *)prec);
  pdbRecordType->papFldDes[669]->size=sizeof(prec->d36cv);
  pdbRecordType->papFldDes[669]->offset=(short)((char *)&prec->d36cv - (char *)prec);
  pdbRecordType->papFldDes[670]->size=sizeof(prec->d36lv);
  pdbRecordType->papFldDes[670]->offset=(short)((char *)&prec->d36lv - (char *)prec);
  pdbRecordType->papFldDes[671]->size=sizeof(prec->d36ne);
  pdbRecordType->papFldDes[671]->offset=(short)((char *)&prec->d36ne - (char *)prec);
  pdbRecordType->papFldDes[672]->size=sizeof(prec->d36eu);
  pdbRecordType->papFldDes[672]->offset=(short)((char *)&prec->d36eu - (char *)prec);
  pdbRecordType->papFldDes[673]->size=sizeof(prec->d36pr);
  pdbRecordType->papFldDes[673]->offset=(short)((char *)&prec->d36pr - (char *)prec);
  pdbRecordType->papFldDes[674]->size=sizeof(prec->d37hr);
  pdbRecordType->papFldDes[674]->offset=(short)((char *)&prec->d37hr - (char *)prec);
  pdbRecordType->papFldDes[675]->size=sizeof(prec->d37lr);
  pdbRecordType->papFldDes[675]->offset=(short)((char *)&prec->d37lr - (char *)prec);
  pdbRecordType->papFldDes[676]->size=sizeof(prec->d37da);
  pdbRecordType->papFldDes[676]->offset=(short)((char *)&prec->d37da - (char *)prec);
  pdbRecordType->papFldDes[677]->size=sizeof(prec->d37ca);
  pdbRecordType->papFldDes[677]->offset=(short)((char *)&prec->d37ca - (char *)prec);
  pdbRecordType->papFldDes[678]->size=sizeof(prec->d37cv);
  pdbRecordType->papFldDes[678]->offset=(short)((char *)&prec->d37cv - (char *)prec);
  pdbRecordType->papFldDes[679]->size=sizeof(prec->d37lv);
  pdbRecordType->papFldDes[679]->offset=(short)((char *)&prec->d37lv - (char *)prec);
  pdbRecordType->papFldDes[680]->size=sizeof(prec->d37ne);
  pdbRecordType->papFldDes[680]->offset=(short)((char *)&prec->d37ne - (char *)prec);
  pdbRecordType->papFldDes[681]->size=sizeof(prec->d37eu);
  pdbRecordType->papFldDes[681]->offset=(short)((char *)&prec->d37eu - (char *)prec);
  pdbRecordType->papFldDes[682]->size=sizeof(prec->d37pr);
  pdbRecordType->papFldDes[682]->offset=(short)((char *)&prec->d37pr - (char *)prec);
  pdbRecordType->papFldDes[683]->size=sizeof(prec->d38hr);
  pdbRecordType->papFldDes[683]->offset=(short)((char *)&prec->d38hr - (char *)prec);
  pdbRecordType->papFldDes[684]->size=sizeof(prec->d38lr);
  pdbRecordType->papFldDes[684]->offset=(short)((char *)&prec->d38lr - (char *)prec);
  pdbRecordType->papFldDes[685]->size=sizeof(prec->d38da);
  pdbRecordType->papFldDes[685]->offset=(short)((char *)&prec->d38da - (char *)prec);
  pdbRecordType->papFldDes[686]->size=sizeof(prec->d38ca);
  pdbRecordType->papFldDes[686]->offset=(short)((char *)&prec->d38ca - (char *)prec);
  pdbRecordType->papFldDes[687]->size=sizeof(prec->d38cv);
  pdbRecordType->papFldDes[687]->offset=(short)((char *)&prec->d38cv - (char *)prec);
  pdbRecordType->papFldDes[688]->size=sizeof(prec->d38lv);
  pdbRecordType->papFldDes[688]->offset=(short)((char *)&prec->d38lv - (char *)prec);
  pdbRecordType->papFldDes[689]->size=sizeof(prec->d38ne);
  pdbRecordType->papFldDes[689]->offset=(short)((char *)&prec->d38ne - (char *)prec);
  pdbRecordType->papFldDes[690]->size=sizeof(prec->d38eu);
  pdbRecordType->papFldDes[690]->offset=(short)((char *)&prec->d38eu - (char *)prec);
  pdbRecordType->papFldDes[691]->size=sizeof(prec->d38pr);
  pdbRecordType->papFldDes[691]->offset=(short)((char *)&prec->d38pr - (char *)prec);
  pdbRecordType->papFldDes[692]->size=sizeof(prec->d39hr);
  pdbRecordType->papFldDes[692]->offset=(short)((char *)&prec->d39hr - (char *)prec);
  pdbRecordType->papFldDes[693]->size=sizeof(prec->d39lr);
  pdbRecordType->papFldDes[693]->offset=(short)((char *)&prec->d39lr - (char *)prec);
  pdbRecordType->papFldDes[694]->size=sizeof(prec->d39da);
  pdbRecordType->papFldDes[694]->offset=(short)((char *)&prec->d39da - (char *)prec);
  pdbRecordType->papFldDes[695]->size=sizeof(prec->d39ca);
  pdbRecordType->papFldDes[695]->offset=(short)((char *)&prec->d39ca - (char *)prec);
  pdbRecordType->papFldDes[696]->size=sizeof(prec->d39cv);
  pdbRecordType->papFldDes[696]->offset=(short)((char *)&prec->d39cv - (char *)prec);
  pdbRecordType->papFldDes[697]->size=sizeof(prec->d39lv);
  pdbRecordType->papFldDes[697]->offset=(short)((char *)&prec->d39lv - (char *)prec);
  pdbRecordType->papFldDes[698]->size=sizeof(prec->d39ne);
  pdbRecordType->papFldDes[698]->offset=(short)((char *)&prec->d39ne - (char *)prec);
  pdbRecordType->papFldDes[699]->size=sizeof(prec->d39eu);
  pdbRecordType->papFldDes[699]->offset=(short)((char *)&prec->d39eu - (char *)prec);
  pdbRecordType->papFldDes[700]->size=sizeof(prec->d39pr);
  pdbRecordType->papFldDes[700]->offset=(short)((char *)&prec->d39pr - (char *)prec);
  pdbRecordType->papFldDes[701]->size=sizeof(prec->d40hr);
  pdbRecordType->papFldDes[701]->offset=(short)((char *)&prec->d40hr - (char *)prec);
  pdbRecordType->papFldDes[702]->size=sizeof(prec->d40lr);
  pdbRecordType->papFldDes[702]->offset=(short)((char *)&prec->d40lr - (char *)prec);
  pdbRecordType->papFldDes[703]->size=sizeof(prec->d40da);
  pdbRecordType->papFldDes[703]->offset=(short)((char *)&prec->d40da - (char *)prec);
  pdbRecordType->papFldDes[704]->size=sizeof(prec->d40ca);
  pdbRecordType->papFldDes[704]->offset=(short)((char *)&prec->d40ca - (char *)prec);
  pdbRecordType->papFldDes[705]->size=sizeof(prec->d40cv);
  pdbRecordType->papFldDes[705]->offset=(short)((char *)&prec->d40cv - (char *)prec);
  pdbRecordType->papFldDes[706]->size=sizeof(prec->d40lv);
  pdbRecordType->papFldDes[706]->offset=(short)((char *)&prec->d40lv - (char *)prec);
  pdbRecordType->papFldDes[707]->size=sizeof(prec->d40ne);
  pdbRecordType->papFldDes[707]->offset=(short)((char *)&prec->d40ne - (char *)prec);
  pdbRecordType->papFldDes[708]->size=sizeof(prec->d40eu);
  pdbRecordType->papFldDes[708]->offset=(short)((char *)&prec->d40eu - (char *)prec);
  pdbRecordType->papFldDes[709]->size=sizeof(prec->d40pr);
  pdbRecordType->papFldDes[709]->offset=(short)((char *)&prec->d40pr - (char *)prec);
  pdbRecordType->papFldDes[710]->size=sizeof(prec->d41hr);
  pdbRecordType->papFldDes[710]->offset=(short)((char *)&prec->d41hr - (char *)prec);
  pdbRecordType->papFldDes[711]->size=sizeof(prec->d41lr);
  pdbRecordType->papFldDes[711]->offset=(short)((char *)&prec->d41lr - (char *)prec);
  pdbRecordType->papFldDes[712]->size=sizeof(prec->d41da);
  pdbRecordType->papFldDes[712]->offset=(short)((char *)&prec->d41da - (char *)prec);
  pdbRecordType->papFldDes[713]->size=sizeof(prec->d41ca);
  pdbRecordType->papFldDes[713]->offset=(short)((char *)&prec->d41ca - (char *)prec);
  pdbRecordType->papFldDes[714]->size=sizeof(prec->d41cv);
  pdbRecordType->papFldDes[714]->offset=(short)((char *)&prec->d41cv - (char *)prec);
  pdbRecordType->papFldDes[715]->size=sizeof(prec->d41lv);
  pdbRecordType->papFldDes[715]->offset=(short)((char *)&prec->d41lv - (char *)prec);
  pdbRecordType->papFldDes[716]->size=sizeof(prec->d41ne);
  pdbRecordType->papFldDes[716]->offset=(short)((char *)&prec->d41ne - (char *)prec);
  pdbRecordType->papFldDes[717]->size=sizeof(prec->d41eu);
  pdbRecordType->papFldDes[717]->offset=(short)((char *)&prec->d41eu - (char *)prec);
  pdbRecordType->papFldDes[718]->size=sizeof(prec->d41pr);
  pdbRecordType->papFldDes[718]->offset=(short)((char *)&prec->d41pr - (char *)prec);
  pdbRecordType->papFldDes[719]->size=sizeof(prec->d42hr);
  pdbRecordType->papFldDes[719]->offset=(short)((char *)&prec->d42hr - (char *)prec);
  pdbRecordType->papFldDes[720]->size=sizeof(prec->d42lr);
  pdbRecordType->papFldDes[720]->offset=(short)((char *)&prec->d42lr - (char *)prec);
  pdbRecordType->papFldDes[721]->size=sizeof(prec->d42da);
  pdbRecordType->papFldDes[721]->offset=(short)((char *)&prec->d42da - (char *)prec);
  pdbRecordType->papFldDes[722]->size=sizeof(prec->d42ca);
  pdbRecordType->papFldDes[722]->offset=(short)((char *)&prec->d42ca - (char *)prec);
  pdbRecordType->papFldDes[723]->size=sizeof(prec->d42cv);
  pdbRecordType->papFldDes[723]->offset=(short)((char *)&prec->d42cv - (char *)prec);
  pdbRecordType->papFldDes[724]->size=sizeof(prec->d42lv);
  pdbRecordType->papFldDes[724]->offset=(short)((char *)&prec->d42lv - (char *)prec);
  pdbRecordType->papFldDes[725]->size=sizeof(prec->d42ne);
  pdbRecordType->papFldDes[725]->offset=(short)((char *)&prec->d42ne - (char *)prec);
  pdbRecordType->papFldDes[726]->size=sizeof(prec->d42eu);
  pdbRecordType->papFldDes[726]->offset=(short)((char *)&prec->d42eu - (char *)prec);
  pdbRecordType->papFldDes[727]->size=sizeof(prec->d42pr);
  pdbRecordType->papFldDes[727]->offset=(short)((char *)&prec->d42pr - (char *)prec);
  pdbRecordType->papFldDes[728]->size=sizeof(prec->d43hr);
  pdbRecordType->papFldDes[728]->offset=(short)((char *)&prec->d43hr - (char *)prec);
  pdbRecordType->papFldDes[729]->size=sizeof(prec->d43lr);
  pdbRecordType->papFldDes[729]->offset=(short)((char *)&prec->d43lr - (char *)prec);
  pdbRecordType->papFldDes[730]->size=sizeof(prec->d43da);
  pdbRecordType->papFldDes[730]->offset=(short)((char *)&prec->d43da - (char *)prec);
  pdbRecordType->papFldDes[731]->size=sizeof(prec->d43ca);
  pdbRecordType->papFldDes[731]->offset=(short)((char *)&prec->d43ca - (char *)prec);
  pdbRecordType->papFldDes[732]->size=sizeof(prec->d43cv);
  pdbRecordType->papFldDes[732]->offset=(short)((char *)&prec->d43cv - (char *)prec);
  pdbRecordType->papFldDes[733]->size=sizeof(prec->d43lv);
  pdbRecordType->papFldDes[733]->offset=(short)((char *)&prec->d43lv - (char *)prec);
  pdbRecordType->papFldDes[734]->size=sizeof(prec->d43ne);
  pdbRecordType->papFldDes[734]->offset=(short)((char *)&prec->d43ne - (char *)prec);
  pdbRecordType->papFldDes[735]->size=sizeof(prec->d43eu);
  pdbRecordType->papFldDes[735]->offset=(short)((char *)&prec->d43eu - (char *)prec);
  pdbRecordType->papFldDes[736]->size=sizeof(prec->d43pr);
  pdbRecordType->papFldDes[736]->offset=(short)((char *)&prec->d43pr - (char *)prec);
  pdbRecordType->papFldDes[737]->size=sizeof(prec->d44hr);
  pdbRecordType->papFldDes[737]->offset=(short)((char *)&prec->d44hr - (char *)prec);
  pdbRecordType->papFldDes[738]->size=sizeof(prec->d44lr);
  pdbRecordType->papFldDes[738]->offset=(short)((char *)&prec->d44lr - (char *)prec);
  pdbRecordType->papFldDes[739]->size=sizeof(prec->d44da);
  pdbRecordType->papFldDes[739]->offset=(short)((char *)&prec->d44da - (char *)prec);
  pdbRecordType->papFldDes[740]->size=sizeof(prec->d44ca);
  pdbRecordType->papFldDes[740]->offset=(short)((char *)&prec->d44ca - (char *)prec);
  pdbRecordType->papFldDes[741]->size=sizeof(prec->d44cv);
  pdbRecordType->papFldDes[741]->offset=(short)((char *)&prec->d44cv - (char *)prec);
  pdbRecordType->papFldDes[742]->size=sizeof(prec->d44lv);
  pdbRecordType->papFldDes[742]->offset=(short)((char *)&prec->d44lv - (char *)prec);
  pdbRecordType->papFldDes[743]->size=sizeof(prec->d44ne);
  pdbRecordType->papFldDes[743]->offset=(short)((char *)&prec->d44ne - (char *)prec);
  pdbRecordType->papFldDes[744]->size=sizeof(prec->d44eu);
  pdbRecordType->papFldDes[744]->offset=(short)((char *)&prec->d44eu - (char *)prec);
  pdbRecordType->papFldDes[745]->size=sizeof(prec->d44pr);
  pdbRecordType->papFldDes[745]->offset=(short)((char *)&prec->d44pr - (char *)prec);
  pdbRecordType->papFldDes[746]->size=sizeof(prec->d45hr);
  pdbRecordType->papFldDes[746]->offset=(short)((char *)&prec->d45hr - (char *)prec);
  pdbRecordType->papFldDes[747]->size=sizeof(prec->d45lr);
  pdbRecordType->papFldDes[747]->offset=(short)((char *)&prec->d45lr - (char *)prec);
  pdbRecordType->papFldDes[748]->size=sizeof(prec->d45da);
  pdbRecordType->papFldDes[748]->offset=(short)((char *)&prec->d45da - (char *)prec);
  pdbRecordType->papFldDes[749]->size=sizeof(prec->d45ca);
  pdbRecordType->papFldDes[749]->offset=(short)((char *)&prec->d45ca - (char *)prec);
  pdbRecordType->papFldDes[750]->size=sizeof(prec->d45cv);
  pdbRecordType->papFldDes[750]->offset=(short)((char *)&prec->d45cv - (char *)prec);
  pdbRecordType->papFldDes[751]->size=sizeof(prec->d45lv);
  pdbRecordType->papFldDes[751]->offset=(short)((char *)&prec->d45lv - (char *)prec);
  pdbRecordType->papFldDes[752]->size=sizeof(prec->d45ne);
  pdbRecordType->papFldDes[752]->offset=(short)((char *)&prec->d45ne - (char *)prec);
  pdbRecordType->papFldDes[753]->size=sizeof(prec->d45eu);
  pdbRecordType->papFldDes[753]->offset=(short)((char *)&prec->d45eu - (char *)prec);
  pdbRecordType->papFldDes[754]->size=sizeof(prec->d45pr);
  pdbRecordType->papFldDes[754]->offset=(short)((char *)&prec->d45pr - (char *)prec);
  pdbRecordType->papFldDes[755]->size=sizeof(prec->d46hr);
  pdbRecordType->papFldDes[755]->offset=(short)((char *)&prec->d46hr - (char *)prec);
  pdbRecordType->papFldDes[756]->size=sizeof(prec->d46lr);
  pdbRecordType->papFldDes[756]->offset=(short)((char *)&prec->d46lr - (char *)prec);
  pdbRecordType->papFldDes[757]->size=sizeof(prec->d46da);
  pdbRecordType->papFldDes[757]->offset=(short)((char *)&prec->d46da - (char *)prec);
  pdbRecordType->papFldDes[758]->size=sizeof(prec->d46ca);
  pdbRecordType->papFldDes[758]->offset=(short)((char *)&prec->d46ca - (char *)prec);
  pdbRecordType->papFldDes[759]->size=sizeof(prec->d46cv);
  pdbRecordType->papFldDes[759]->offset=(short)((char *)&prec->d46cv - (char *)prec);
  pdbRecordType->papFldDes[760]->size=sizeof(prec->d46lv);
  pdbRecordType->papFldDes[760]->offset=(short)((char *)&prec->d46lv - (char *)prec);
  pdbRecordType->papFldDes[761]->size=sizeof(prec->d46ne);
  pdbRecordType->papFldDes[761]->offset=(short)((char *)&prec->d46ne - (char *)prec);
  pdbRecordType->papFldDes[762]->size=sizeof(prec->d46eu);
  pdbRecordType->papFldDes[762]->offset=(short)((char *)&prec->d46eu - (char *)prec);
  pdbRecordType->papFldDes[763]->size=sizeof(prec->d46pr);
  pdbRecordType->papFldDes[763]->offset=(short)((char *)&prec->d46pr - (char *)prec);
  pdbRecordType->papFldDes[764]->size=sizeof(prec->d47hr);
  pdbRecordType->papFldDes[764]->offset=(short)((char *)&prec->d47hr - (char *)prec);
  pdbRecordType->papFldDes[765]->size=sizeof(prec->d47lr);
  pdbRecordType->papFldDes[765]->offset=(short)((char *)&prec->d47lr - (char *)prec);
  pdbRecordType->papFldDes[766]->size=sizeof(prec->d47da);
  pdbRecordType->papFldDes[766]->offset=(short)((char *)&prec->d47da - (char *)prec);
  pdbRecordType->papFldDes[767]->size=sizeof(prec->d47ca);
  pdbRecordType->papFldDes[767]->offset=(short)((char *)&prec->d47ca - (char *)prec);
  pdbRecordType->papFldDes[768]->size=sizeof(prec->d47cv);
  pdbRecordType->papFldDes[768]->offset=(short)((char *)&prec->d47cv - (char *)prec);
  pdbRecordType->papFldDes[769]->size=sizeof(prec->d47lv);
  pdbRecordType->papFldDes[769]->offset=(short)((char *)&prec->d47lv - (char *)prec);
  pdbRecordType->papFldDes[770]->size=sizeof(prec->d47ne);
  pdbRecordType->papFldDes[770]->offset=(short)((char *)&prec->d47ne - (char *)prec);
  pdbRecordType->papFldDes[771]->size=sizeof(prec->d47eu);
  pdbRecordType->papFldDes[771]->offset=(short)((char *)&prec->d47eu - (char *)prec);
  pdbRecordType->papFldDes[772]->size=sizeof(prec->d47pr);
  pdbRecordType->papFldDes[772]->offset=(short)((char *)&prec->d47pr - (char *)prec);
  pdbRecordType->papFldDes[773]->size=sizeof(prec->d48hr);
  pdbRecordType->papFldDes[773]->offset=(short)((char *)&prec->d48hr - (char *)prec);
  pdbRecordType->papFldDes[774]->size=sizeof(prec->d48lr);
  pdbRecordType->papFldDes[774]->offset=(short)((char *)&prec->d48lr - (char *)prec);
  pdbRecordType->papFldDes[775]->size=sizeof(prec->d48da);
  pdbRecordType->papFldDes[775]->offset=(short)((char *)&prec->d48da - (char *)prec);
  pdbRecordType->papFldDes[776]->size=sizeof(prec->d48ca);
  pdbRecordType->papFldDes[776]->offset=(short)((char *)&prec->d48ca - (char *)prec);
  pdbRecordType->papFldDes[777]->size=sizeof(prec->d48cv);
  pdbRecordType->papFldDes[777]->offset=(short)((char *)&prec->d48cv - (char *)prec);
  pdbRecordType->papFldDes[778]->size=sizeof(prec->d48lv);
  pdbRecordType->papFldDes[778]->offset=(short)((char *)&prec->d48lv - (char *)prec);
  pdbRecordType->papFldDes[779]->size=sizeof(prec->d48ne);
  pdbRecordType->papFldDes[779]->offset=(short)((char *)&prec->d48ne - (char *)prec);
  pdbRecordType->papFldDes[780]->size=sizeof(prec->d48eu);
  pdbRecordType->papFldDes[780]->offset=(short)((char *)&prec->d48eu - (char *)prec);
  pdbRecordType->papFldDes[781]->size=sizeof(prec->d48pr);
  pdbRecordType->papFldDes[781]->offset=(short)((char *)&prec->d48pr - (char *)prec);
  pdbRecordType->papFldDes[782]->size=sizeof(prec->d49hr);
  pdbRecordType->papFldDes[782]->offset=(short)((char *)&prec->d49hr - (char *)prec);
  pdbRecordType->papFldDes[783]->size=sizeof(prec->d49lr);
  pdbRecordType->papFldDes[783]->offset=(short)((char *)&prec->d49lr - (char *)prec);
  pdbRecordType->papFldDes[784]->size=sizeof(prec->d49da);
  pdbRecordType->papFldDes[784]->offset=(short)((char *)&prec->d49da - (char *)prec);
  pdbRecordType->papFldDes[785]->size=sizeof(prec->d49ca);
  pdbRecordType->papFldDes[785]->offset=(short)((char *)&prec->d49ca - (char *)prec);
  pdbRecordType->papFldDes[786]->size=sizeof(prec->d49cv);
  pdbRecordType->papFldDes[786]->offset=(short)((char *)&prec->d49cv - (char *)prec);
  pdbRecordType->papFldDes[787]->size=sizeof(prec->d49lv);
  pdbRecordType->papFldDes[787]->offset=(short)((char *)&prec->d49lv - (char *)prec);
  pdbRecordType->papFldDes[788]->size=sizeof(prec->d49ne);
  pdbRecordType->papFldDes[788]->offset=(short)((char *)&prec->d49ne - (char *)prec);
  pdbRecordType->papFldDes[789]->size=sizeof(prec->d49eu);
  pdbRecordType->papFldDes[789]->offset=(short)((char *)&prec->d49eu - (char *)prec);
  pdbRecordType->papFldDes[790]->size=sizeof(prec->d49pr);
  pdbRecordType->papFldDes[790]->offset=(short)((char *)&prec->d49pr - (char *)prec);
  pdbRecordType->papFldDes[791]->size=sizeof(prec->d50hr);
  pdbRecordType->papFldDes[791]->offset=(short)((char *)&prec->d50hr - (char *)prec);
  pdbRecordType->papFldDes[792]->size=sizeof(prec->d50lr);
  pdbRecordType->papFldDes[792]->offset=(short)((char *)&prec->d50lr - (char *)prec);
  pdbRecordType->papFldDes[793]->size=sizeof(prec->d50da);
  pdbRecordType->papFldDes[793]->offset=(short)((char *)&prec->d50da - (char *)prec);
  pdbRecordType->papFldDes[794]->size=sizeof(prec->d50ca);
  pdbRecordType->papFldDes[794]->offset=(short)((char *)&prec->d50ca - (char *)prec);
  pdbRecordType->papFldDes[795]->size=sizeof(prec->d50cv);
  pdbRecordType->papFldDes[795]->offset=(short)((char *)&prec->d50cv - (char *)prec);
  pdbRecordType->papFldDes[796]->size=sizeof(prec->d50lv);
  pdbRecordType->papFldDes[796]->offset=(short)((char *)&prec->d50lv - (char *)prec);
  pdbRecordType->papFldDes[797]->size=sizeof(prec->d50ne);
  pdbRecordType->papFldDes[797]->offset=(short)((char *)&prec->d50ne - (char *)prec);
  pdbRecordType->papFldDes[798]->size=sizeof(prec->d50eu);
  pdbRecordType->papFldDes[798]->offset=(short)((char *)&prec->d50eu - (char *)prec);
  pdbRecordType->papFldDes[799]->size=sizeof(prec->d50pr);
  pdbRecordType->papFldDes[799]->offset=(short)((char *)&prec->d50pr - (char *)prec);
  pdbRecordType->papFldDes[800]->size=sizeof(prec->d51hr);
  pdbRecordType->papFldDes[800]->offset=(short)((char *)&prec->d51hr - (char *)prec);
  pdbRecordType->papFldDes[801]->size=sizeof(prec->d51lr);
  pdbRecordType->papFldDes[801]->offset=(short)((char *)&prec->d51lr - (char *)prec);
  pdbRecordType->papFldDes[802]->size=sizeof(prec->d51da);
  pdbRecordType->papFldDes[802]->offset=(short)((char *)&prec->d51da - (char *)prec);
  pdbRecordType->papFldDes[803]->size=sizeof(prec->d51ca);
  pdbRecordType->papFldDes[803]->offset=(short)((char *)&prec->d51ca - (char *)prec);
  pdbRecordType->papFldDes[804]->size=sizeof(prec->d51cv);
  pdbRecordType->papFldDes[804]->offset=(short)((char *)&prec->d51cv - (char *)prec);
  pdbRecordType->papFldDes[805]->size=sizeof(prec->d51lv);
  pdbRecordType->papFldDes[805]->offset=(short)((char *)&prec->d51lv - (char *)prec);
  pdbRecordType->papFldDes[806]->size=sizeof(prec->d51ne);
  pdbRecordType->papFldDes[806]->offset=(short)((char *)&prec->d51ne - (char *)prec);
  pdbRecordType->papFldDes[807]->size=sizeof(prec->d51eu);
  pdbRecordType->papFldDes[807]->offset=(short)((char *)&prec->d51eu - (char *)prec);
  pdbRecordType->papFldDes[808]->size=sizeof(prec->d51pr);
  pdbRecordType->papFldDes[808]->offset=(short)((char *)&prec->d51pr - (char *)prec);
  pdbRecordType->papFldDes[809]->size=sizeof(prec->d52hr);
  pdbRecordType->papFldDes[809]->offset=(short)((char *)&prec->d52hr - (char *)prec);
  pdbRecordType->papFldDes[810]->size=sizeof(prec->d52lr);
  pdbRecordType->papFldDes[810]->offset=(short)((char *)&prec->d52lr - (char *)prec);
  pdbRecordType->papFldDes[811]->size=sizeof(prec->d52da);
  pdbRecordType->papFldDes[811]->offset=(short)((char *)&prec->d52da - (char *)prec);
  pdbRecordType->papFldDes[812]->size=sizeof(prec->d52ca);
  pdbRecordType->papFldDes[812]->offset=(short)((char *)&prec->d52ca - (char *)prec);
  pdbRecordType->papFldDes[813]->size=sizeof(prec->d52cv);
  pdbRecordType->papFldDes[813]->offset=(short)((char *)&prec->d52cv - (char *)prec);
  pdbRecordType->papFldDes[814]->size=sizeof(prec->d52lv);
  pdbRecordType->papFldDes[814]->offset=(short)((char *)&prec->d52lv - (char *)prec);
  pdbRecordType->papFldDes[815]->size=sizeof(prec->d52ne);
  pdbRecordType->papFldDes[815]->offset=(short)((char *)&prec->d52ne - (char *)prec);
  pdbRecordType->papFldDes[816]->size=sizeof(prec->d52eu);
  pdbRecordType->papFldDes[816]->offset=(short)((char *)&prec->d52eu - (char *)prec);
  pdbRecordType->papFldDes[817]->size=sizeof(prec->d52pr);
  pdbRecordType->papFldDes[817]->offset=(short)((char *)&prec->d52pr - (char *)prec);
  pdbRecordType->papFldDes[818]->size=sizeof(prec->d53hr);
  pdbRecordType->papFldDes[818]->offset=(short)((char *)&prec->d53hr - (char *)prec);
  pdbRecordType->papFldDes[819]->size=sizeof(prec->d53lr);
  pdbRecordType->papFldDes[819]->offset=(short)((char *)&prec->d53lr - (char *)prec);
  pdbRecordType->papFldDes[820]->size=sizeof(prec->d53da);
  pdbRecordType->papFldDes[820]->offset=(short)((char *)&prec->d53da - (char *)prec);
  pdbRecordType->papFldDes[821]->size=sizeof(prec->d53ca);
  pdbRecordType->papFldDes[821]->offset=(short)((char *)&prec->d53ca - (char *)prec);
  pdbRecordType->papFldDes[822]->size=sizeof(prec->d53cv);
  pdbRecordType->papFldDes[822]->offset=(short)((char *)&prec->d53cv - (char *)prec);
  pdbRecordType->papFldDes[823]->size=sizeof(prec->d53lv);
  pdbRecordType->papFldDes[823]->offset=(short)((char *)&prec->d53lv - (char *)prec);
  pdbRecordType->papFldDes[824]->size=sizeof(prec->d53ne);
  pdbRecordType->papFldDes[824]->offset=(short)((char *)&prec->d53ne - (char *)prec);
  pdbRecordType->papFldDes[825]->size=sizeof(prec->d53eu);
  pdbRecordType->papFldDes[825]->offset=(short)((char *)&prec->d53eu - (char *)prec);
  pdbRecordType->papFldDes[826]->size=sizeof(prec->d53pr);
  pdbRecordType->papFldDes[826]->offset=(short)((char *)&prec->d53pr - (char *)prec);
  pdbRecordType->papFldDes[827]->size=sizeof(prec->d54hr);
  pdbRecordType->papFldDes[827]->offset=(short)((char *)&prec->d54hr - (char *)prec);
  pdbRecordType->papFldDes[828]->size=sizeof(prec->d54lr);
  pdbRecordType->papFldDes[828]->offset=(short)((char *)&prec->d54lr - (char *)prec);
  pdbRecordType->papFldDes[829]->size=sizeof(prec->d54da);
  pdbRecordType->papFldDes[829]->offset=(short)((char *)&prec->d54da - (char *)prec);
  pdbRecordType->papFldDes[830]->size=sizeof(prec->d54ca);
  pdbRecordType->papFldDes[830]->offset=(short)((char *)&prec->d54ca - (char *)prec);
  pdbRecordType->papFldDes[831]->size=sizeof(prec->d54cv);
  pdbRecordType->papFldDes[831]->offset=(short)((char *)&prec->d54cv - (char *)prec);
  pdbRecordType->papFldDes[832]->size=sizeof(prec->d54lv);
  pdbRecordType->papFldDes[832]->offset=(short)((char *)&prec->d54lv - (char *)prec);
  pdbRecordType->papFldDes[833]->size=sizeof(prec->d54ne);
  pdbRecordType->papFldDes[833]->offset=(short)((char *)&prec->d54ne - (char *)prec);
  pdbRecordType->papFldDes[834]->size=sizeof(prec->d54eu);
  pdbRecordType->papFldDes[834]->offset=(short)((char *)&prec->d54eu - (char *)prec);
  pdbRecordType->papFldDes[835]->size=sizeof(prec->d54pr);
  pdbRecordType->papFldDes[835]->offset=(short)((char *)&prec->d54pr - (char *)prec);
  pdbRecordType->papFldDes[836]->size=sizeof(prec->d55hr);
  pdbRecordType->papFldDes[836]->offset=(short)((char *)&prec->d55hr - (char *)prec);
  pdbRecordType->papFldDes[837]->size=sizeof(prec->d55lr);
  pdbRecordType->papFldDes[837]->offset=(short)((char *)&prec->d55lr - (char *)prec);
  pdbRecordType->papFldDes[838]->size=sizeof(prec->d55da);
  pdbRecordType->papFldDes[838]->offset=(short)((char *)&prec->d55da - (char *)prec);
  pdbRecordType->papFldDes[839]->size=sizeof(prec->d55ca);
  pdbRecordType->papFldDes[839]->offset=(short)((char *)&prec->d55ca - (char *)prec);
  pdbRecordType->papFldDes[840]->size=sizeof(prec->d55cv);
  pdbRecordType->papFldDes[840]->offset=(short)((char *)&prec->d55cv - (char *)prec);
  pdbRecordType->papFldDes[841]->size=sizeof(prec->d55lv);
  pdbRecordType->papFldDes[841]->offset=(short)((char *)&prec->d55lv - (char *)prec);
  pdbRecordType->papFldDes[842]->size=sizeof(prec->d55ne);
  pdbRecordType->papFldDes[842]->offset=(short)((char *)&prec->d55ne - (char *)prec);
  pdbRecordType->papFldDes[843]->size=sizeof(prec->d55eu);
  pdbRecordType->papFldDes[843]->offset=(short)((char *)&prec->d55eu - (char *)prec);
  pdbRecordType->papFldDes[844]->size=sizeof(prec->d55pr);
  pdbRecordType->papFldDes[844]->offset=(short)((char *)&prec->d55pr - (char *)prec);
  pdbRecordType->papFldDes[845]->size=sizeof(prec->d56hr);
  pdbRecordType->papFldDes[845]->offset=(short)((char *)&prec->d56hr - (char *)prec);
  pdbRecordType->papFldDes[846]->size=sizeof(prec->d56lr);
  pdbRecordType->papFldDes[846]->offset=(short)((char *)&prec->d56lr - (char *)prec);
  pdbRecordType->papFldDes[847]->size=sizeof(prec->d56da);
  pdbRecordType->papFldDes[847]->offset=(short)((char *)&prec->d56da - (char *)prec);
  pdbRecordType->papFldDes[848]->size=sizeof(prec->d56ca);
  pdbRecordType->papFldDes[848]->offset=(short)((char *)&prec->d56ca - (char *)prec);
  pdbRecordType->papFldDes[849]->size=sizeof(prec->d56cv);
  pdbRecordType->papFldDes[849]->offset=(short)((char *)&prec->d56cv - (char *)prec);
  pdbRecordType->papFldDes[850]->size=sizeof(prec->d56lv);
  pdbRecordType->papFldDes[850]->offset=(short)((char *)&prec->d56lv - (char *)prec);
  pdbRecordType->papFldDes[851]->size=sizeof(prec->d56ne);
  pdbRecordType->papFldDes[851]->offset=(short)((char *)&prec->d56ne - (char *)prec);
  pdbRecordType->papFldDes[852]->size=sizeof(prec->d56eu);
  pdbRecordType->papFldDes[852]->offset=(short)((char *)&prec->d56eu - (char *)prec);
  pdbRecordType->papFldDes[853]->size=sizeof(prec->d56pr);
  pdbRecordType->papFldDes[853]->offset=(short)((char *)&prec->d56pr - (char *)prec);
  pdbRecordType->papFldDes[854]->size=sizeof(prec->d57hr);
  pdbRecordType->papFldDes[854]->offset=(short)((char *)&prec->d57hr - (char *)prec);
  pdbRecordType->papFldDes[855]->size=sizeof(prec->d57lr);
  pdbRecordType->papFldDes[855]->offset=(short)((char *)&prec->d57lr - (char *)prec);
  pdbRecordType->papFldDes[856]->size=sizeof(prec->d57da);
  pdbRecordType->papFldDes[856]->offset=(short)((char *)&prec->d57da - (char *)prec);
  pdbRecordType->papFldDes[857]->size=sizeof(prec->d57ca);
  pdbRecordType->papFldDes[857]->offset=(short)((char *)&prec->d57ca - (char *)prec);
  pdbRecordType->papFldDes[858]->size=sizeof(prec->d57cv);
  pdbRecordType->papFldDes[858]->offset=(short)((char *)&prec->d57cv - (char *)prec);
  pdbRecordType->papFldDes[859]->size=sizeof(prec->d57lv);
  pdbRecordType->papFldDes[859]->offset=(short)((char *)&prec->d57lv - (char *)prec);
  pdbRecordType->papFldDes[860]->size=sizeof(prec->d57ne);
  pdbRecordType->papFldDes[860]->offset=(short)((char *)&prec->d57ne - (char *)prec);
  pdbRecordType->papFldDes[861]->size=sizeof(prec->d57eu);
  pdbRecordType->papFldDes[861]->offset=(short)((char *)&prec->d57eu - (char *)prec);
  pdbRecordType->papFldDes[862]->size=sizeof(prec->d57pr);
  pdbRecordType->papFldDes[862]->offset=(short)((char *)&prec->d57pr - (char *)prec);
  pdbRecordType->papFldDes[863]->size=sizeof(prec->d58hr);
  pdbRecordType->papFldDes[863]->offset=(short)((char *)&prec->d58hr - (char *)prec);
  pdbRecordType->papFldDes[864]->size=sizeof(prec->d58lr);
  pdbRecordType->papFldDes[864]->offset=(short)((char *)&prec->d58lr - (char *)prec);
  pdbRecordType->papFldDes[865]->size=sizeof(prec->d58da);
  pdbRecordType->papFldDes[865]->offset=(short)((char *)&prec->d58da - (char *)prec);
  pdbRecordType->papFldDes[866]->size=sizeof(prec->d58ca);
  pdbRecordType->papFldDes[866]->offset=(short)((char *)&prec->d58ca - (char *)prec);
  pdbRecordType->papFldDes[867]->size=sizeof(prec->d58cv);
  pdbRecordType->papFldDes[867]->offset=(short)((char *)&prec->d58cv - (char *)prec);
  pdbRecordType->papFldDes[868]->size=sizeof(prec->d58lv);
  pdbRecordType->papFldDes[868]->offset=(short)((char *)&prec->d58lv - (char *)prec);
  pdbRecordType->papFldDes[869]->size=sizeof(prec->d58ne);
  pdbRecordType->papFldDes[869]->offset=(short)((char *)&prec->d58ne - (char *)prec);
  pdbRecordType->papFldDes[870]->size=sizeof(prec->d58eu);
  pdbRecordType->papFldDes[870]->offset=(short)((char *)&prec->d58eu - (char *)prec);
  pdbRecordType->papFldDes[871]->size=sizeof(prec->d58pr);
  pdbRecordType->papFldDes[871]->offset=(short)((char *)&prec->d58pr - (char *)prec);
  pdbRecordType->papFldDes[872]->size=sizeof(prec->d59hr);
  pdbRecordType->papFldDes[872]->offset=(short)((char *)&prec->d59hr - (char *)prec);
  pdbRecordType->papFldDes[873]->size=sizeof(prec->d59lr);
  pdbRecordType->papFldDes[873]->offset=(short)((char *)&prec->d59lr - (char *)prec);
  pdbRecordType->papFldDes[874]->size=sizeof(prec->d59da);
  pdbRecordType->papFldDes[874]->offset=(short)((char *)&prec->d59da - (char *)prec);
  pdbRecordType->papFldDes[875]->size=sizeof(prec->d59ca);
  pdbRecordType->papFldDes[875]->offset=(short)((char *)&prec->d59ca - (char *)prec);
  pdbRecordType->papFldDes[876]->size=sizeof(prec->d59cv);
  pdbRecordType->papFldDes[876]->offset=(short)((char *)&prec->d59cv - (char *)prec);
  pdbRecordType->papFldDes[877]->size=sizeof(prec->d59lv);
  pdbRecordType->papFldDes[877]->offset=(short)((char *)&prec->d59lv - (char *)prec);
  pdbRecordType->papFldDes[878]->size=sizeof(prec->d59ne);
  pdbRecordType->papFldDes[878]->offset=(short)((char *)&prec->d59ne - (char *)prec);
  pdbRecordType->papFldDes[879]->size=sizeof(prec->d59eu);
  pdbRecordType->papFldDes[879]->offset=(short)((char *)&prec->d59eu - (char *)prec);
  pdbRecordType->papFldDes[880]->size=sizeof(prec->d59pr);
  pdbRecordType->papFldDes[880]->offset=(short)((char *)&prec->d59pr - (char *)prec);
  pdbRecordType->papFldDes[881]->size=sizeof(prec->d60hr);
  pdbRecordType->papFldDes[881]->offset=(short)((char *)&prec->d60hr - (char *)prec);
  pdbRecordType->papFldDes[882]->size=sizeof(prec->d60lr);
  pdbRecordType->papFldDes[882]->offset=(short)((char *)&prec->d60lr - (char *)prec);
  pdbRecordType->papFldDes[883]->size=sizeof(prec->d60da);
  pdbRecordType->papFldDes[883]->offset=(short)((char *)&prec->d60da - (char *)prec);
  pdbRecordType->papFldDes[884]->size=sizeof(prec->d60ca);
  pdbRecordType->papFldDes[884]->offset=(short)((char *)&prec->d60ca - (char *)prec);
  pdbRecordType->papFldDes[885]->size=sizeof(prec->d60cv);
  pdbRecordType->papFldDes[885]->offset=(short)((char *)&prec->d60cv - (char *)prec);
  pdbRecordType->papFldDes[886]->size=sizeof(prec->d60lv);
  pdbRecordType->papFldDes[886]->offset=(short)((char *)&prec->d60lv - (char *)prec);
  pdbRecordType->papFldDes[887]->size=sizeof(prec->d60ne);
  pdbRecordType->papFldDes[887]->offset=(short)((char *)&prec->d60ne - (char *)prec);
  pdbRecordType->papFldDes[888]->size=sizeof(prec->d60eu);
  pdbRecordType->papFldDes[888]->offset=(short)((char *)&prec->d60eu - (char *)prec);
  pdbRecordType->papFldDes[889]->size=sizeof(prec->d60pr);
  pdbRecordType->papFldDes[889]->offset=(short)((char *)&prec->d60pr - (char *)prec);
  pdbRecordType->papFldDes[890]->size=sizeof(prec->d61hr);
  pdbRecordType->papFldDes[890]->offset=(short)((char *)&prec->d61hr - (char *)prec);
  pdbRecordType->papFldDes[891]->size=sizeof(prec->d61lr);
  pdbRecordType->papFldDes[891]->offset=(short)((char *)&prec->d61lr - (char *)prec);
  pdbRecordType->papFldDes[892]->size=sizeof(prec->d61da);
  pdbRecordType->papFldDes[892]->offset=(short)((char *)&prec->d61da - (char *)prec);
  pdbRecordType->papFldDes[893]->size=sizeof(prec->d61ca);
  pdbRecordType->papFldDes[893]->offset=(short)((char *)&prec->d61ca - (char *)prec);
  pdbRecordType->papFldDes[894]->size=sizeof(prec->d61cv);
  pdbRecordType->papFldDes[894]->offset=(short)((char *)&prec->d61cv - (char *)prec);
  pdbRecordType->papFldDes[895]->size=sizeof(prec->d61lv);
  pdbRecordType->papFldDes[895]->offset=(short)((char *)&prec->d61lv - (char *)prec);
  pdbRecordType->papFldDes[896]->size=sizeof(prec->d61ne);
  pdbRecordType->papFldDes[896]->offset=(short)((char *)&prec->d61ne - (char *)prec);
  pdbRecordType->papFldDes[897]->size=sizeof(prec->d61eu);
  pdbRecordType->papFldDes[897]->offset=(short)((char *)&prec->d61eu - (char *)prec);
  pdbRecordType->papFldDes[898]->size=sizeof(prec->d61pr);
  pdbRecordType->papFldDes[898]->offset=(short)((char *)&prec->d61pr - (char *)prec);
  pdbRecordType->papFldDes[899]->size=sizeof(prec->d62hr);
  pdbRecordType->papFldDes[899]->offset=(short)((char *)&prec->d62hr - (char *)prec);
  pdbRecordType->papFldDes[900]->size=sizeof(prec->d62lr);
  pdbRecordType->papFldDes[900]->offset=(short)((char *)&prec->d62lr - (char *)prec);
  pdbRecordType->papFldDes[901]->size=sizeof(prec->d62da);
  pdbRecordType->papFldDes[901]->offset=(short)((char *)&prec->d62da - (char *)prec);
  pdbRecordType->papFldDes[902]->size=sizeof(prec->d62ca);
  pdbRecordType->papFldDes[902]->offset=(short)((char *)&prec->d62ca - (char *)prec);
  pdbRecordType->papFldDes[903]->size=sizeof(prec->d62cv);
  pdbRecordType->papFldDes[903]->offset=(short)((char *)&prec->d62cv - (char *)prec);
  pdbRecordType->papFldDes[904]->size=sizeof(prec->d62lv);
  pdbRecordType->papFldDes[904]->offset=(short)((char *)&prec->d62lv - (char *)prec);
  pdbRecordType->papFldDes[905]->size=sizeof(prec->d62ne);
  pdbRecordType->papFldDes[905]->offset=(short)((char *)&prec->d62ne - (char *)prec);
  pdbRecordType->papFldDes[906]->size=sizeof(prec->d62eu);
  pdbRecordType->papFldDes[906]->offset=(short)((char *)&prec->d62eu - (char *)prec);
  pdbRecordType->papFldDes[907]->size=sizeof(prec->d62pr);
  pdbRecordType->papFldDes[907]->offset=(short)((char *)&prec->d62pr - (char *)prec);
  pdbRecordType->papFldDes[908]->size=sizeof(prec->d63hr);
  pdbRecordType->papFldDes[908]->offset=(short)((char *)&prec->d63hr - (char *)prec);
  pdbRecordType->papFldDes[909]->size=sizeof(prec->d63lr);
  pdbRecordType->papFldDes[909]->offset=(short)((char *)&prec->d63lr - (char *)prec);
  pdbRecordType->papFldDes[910]->size=sizeof(prec->d63da);
  pdbRecordType->papFldDes[910]->offset=(short)((char *)&prec->d63da - (char *)prec);
  pdbRecordType->papFldDes[911]->size=sizeof(prec->d63ca);
  pdbRecordType->papFldDes[911]->offset=(short)((char *)&prec->d63ca - (char *)prec);
  pdbRecordType->papFldDes[912]->size=sizeof(prec->d63cv);
  pdbRecordType->papFldDes[912]->offset=(short)((char *)&prec->d63cv - (char *)prec);
  pdbRecordType->papFldDes[913]->size=sizeof(prec->d63lv);
  pdbRecordType->papFldDes[913]->offset=(short)((char *)&prec->d63lv - (char *)prec);
  pdbRecordType->papFldDes[914]->size=sizeof(prec->d63ne);
  pdbRecordType->papFldDes[914]->offset=(short)((char *)&prec->d63ne - (char *)prec);
  pdbRecordType->papFldDes[915]->size=sizeof(prec->d63eu);
  pdbRecordType->papFldDes[915]->offset=(short)((char *)&prec->d63eu - (char *)prec);
  pdbRecordType->papFldDes[916]->size=sizeof(prec->d63pr);
  pdbRecordType->papFldDes[916]->offset=(short)((char *)&prec->d63pr - (char *)prec);
  pdbRecordType->papFldDes[917]->size=sizeof(prec->d64hr);
  pdbRecordType->papFldDes[917]->offset=(short)((char *)&prec->d64hr - (char *)prec);
  pdbRecordType->papFldDes[918]->size=sizeof(prec->d64lr);
  pdbRecordType->papFldDes[918]->offset=(short)((char *)&prec->d64lr - (char *)prec);
  pdbRecordType->papFldDes[919]->size=sizeof(prec->d64da);
  pdbRecordType->papFldDes[919]->offset=(short)((char *)&prec->d64da - (char *)prec);
  pdbRecordType->papFldDes[920]->size=sizeof(prec->d64ca);
  pdbRecordType->papFldDes[920]->offset=(short)((char *)&prec->d64ca - (char *)prec);
  pdbRecordType->papFldDes[921]->size=sizeof(prec->d64cv);
  pdbRecordType->papFldDes[921]->offset=(short)((char *)&prec->d64cv - (char *)prec);
  pdbRecordType->papFldDes[922]->size=sizeof(prec->d64lv);
  pdbRecordType->papFldDes[922]->offset=(short)((char *)&prec->d64lv - (char *)prec);
  pdbRecordType->papFldDes[923]->size=sizeof(prec->d64ne);
  pdbRecordType->papFldDes[923]->offset=(short)((char *)&prec->d64ne - (char *)prec);
  pdbRecordType->papFldDes[924]->size=sizeof(prec->d64eu);
  pdbRecordType->papFldDes[924]->offset=(short)((char *)&prec->d64eu - (char *)prec);
  pdbRecordType->papFldDes[925]->size=sizeof(prec->d64pr);
  pdbRecordType->papFldDes[925]->offset=(short)((char *)&prec->d64pr - (char *)prec);
  pdbRecordType->papFldDes[926]->size=sizeof(prec->d65hr);
  pdbRecordType->papFldDes[926]->offset=(short)((char *)&prec->d65hr - (char *)prec);
  pdbRecordType->papFldDes[927]->size=sizeof(prec->d65lr);
  pdbRecordType->papFldDes[927]->offset=(short)((char *)&prec->d65lr - (char *)prec);
  pdbRecordType->papFldDes[928]->size=sizeof(prec->d65da);
  pdbRecordType->papFldDes[928]->offset=(short)((char *)&prec->d65da - (char *)prec);
  pdbRecordType->papFldDes[929]->size=sizeof(prec->d65ca);
  pdbRecordType->papFldDes[929]->offset=(short)((char *)&prec->d65ca - (char *)prec);
  pdbRecordType->papFldDes[930]->size=sizeof(prec->d65cv);
  pdbRecordType->papFldDes[930]->offset=(short)((char *)&prec->d65cv - (char *)prec);
  pdbRecordType->papFldDes[931]->size=sizeof(prec->d65lv);
  pdbRecordType->papFldDes[931]->offset=(short)((char *)&prec->d65lv - (char *)prec);
  pdbRecordType->papFldDes[932]->size=sizeof(prec->d65ne);
  pdbRecordType->papFldDes[932]->offset=(short)((char *)&prec->d65ne - (char *)prec);
  pdbRecordType->papFldDes[933]->size=sizeof(prec->d65eu);
  pdbRecordType->papFldDes[933]->offset=(short)((char *)&prec->d65eu - (char *)prec);
  pdbRecordType->papFldDes[934]->size=sizeof(prec->d65pr);
  pdbRecordType->papFldDes[934]->offset=(short)((char *)&prec->d65pr - (char *)prec);
  pdbRecordType->papFldDes[935]->size=sizeof(prec->d66hr);
  pdbRecordType->papFldDes[935]->offset=(short)((char *)&prec->d66hr - (char *)prec);
  pdbRecordType->papFldDes[936]->size=sizeof(prec->d66lr);
  pdbRecordType->papFldDes[936]->offset=(short)((char *)&prec->d66lr - (char *)prec);
  pdbRecordType->papFldDes[937]->size=sizeof(prec->d66da);
  pdbRecordType->papFldDes[937]->offset=(short)((char *)&prec->d66da - (char *)prec);
  pdbRecordType->papFldDes[938]->size=sizeof(prec->d66ca);
  pdbRecordType->papFldDes[938]->offset=(short)((char *)&prec->d66ca - (char *)prec);
  pdbRecordType->papFldDes[939]->size=sizeof(prec->d66cv);
  pdbRecordType->papFldDes[939]->offset=(short)((char *)&prec->d66cv - (char *)prec);
  pdbRecordType->papFldDes[940]->size=sizeof(prec->d66lv);
  pdbRecordType->papFldDes[940]->offset=(short)((char *)&prec->d66lv - (char *)prec);
  pdbRecordType->papFldDes[941]->size=sizeof(prec->d66ne);
  pdbRecordType->papFldDes[941]->offset=(short)((char *)&prec->d66ne - (char *)prec);
  pdbRecordType->papFldDes[942]->size=sizeof(prec->d66eu);
  pdbRecordType->papFldDes[942]->offset=(short)((char *)&prec->d66eu - (char *)prec);
  pdbRecordType->papFldDes[943]->size=sizeof(prec->d66pr);
  pdbRecordType->papFldDes[943]->offset=(short)((char *)&prec->d66pr - (char *)prec);
  pdbRecordType->papFldDes[944]->size=sizeof(prec->d67hr);
  pdbRecordType->papFldDes[944]->offset=(short)((char *)&prec->d67hr - (char *)prec);
  pdbRecordType->papFldDes[945]->size=sizeof(prec->d67lr);
  pdbRecordType->papFldDes[945]->offset=(short)((char *)&prec->d67lr - (char *)prec);
  pdbRecordType->papFldDes[946]->size=sizeof(prec->d67da);
  pdbRecordType->papFldDes[946]->offset=(short)((char *)&prec->d67da - (char *)prec);
  pdbRecordType->papFldDes[947]->size=sizeof(prec->d67ca);
  pdbRecordType->papFldDes[947]->offset=(short)((char *)&prec->d67ca - (char *)prec);
  pdbRecordType->papFldDes[948]->size=sizeof(prec->d67cv);
  pdbRecordType->papFldDes[948]->offset=(short)((char *)&prec->d67cv - (char *)prec);
  pdbRecordType->papFldDes[949]->size=sizeof(prec->d67lv);
  pdbRecordType->papFldDes[949]->offset=(short)((char *)&prec->d67lv - (char *)prec);
  pdbRecordType->papFldDes[950]->size=sizeof(prec->d67ne);
  pdbRecordType->papFldDes[950]->offset=(short)((char *)&prec->d67ne - (char *)prec);
  pdbRecordType->papFldDes[951]->size=sizeof(prec->d67eu);
  pdbRecordType->papFldDes[951]->offset=(short)((char *)&prec->d67eu - (char *)prec);
  pdbRecordType->papFldDes[952]->size=sizeof(prec->d67pr);
  pdbRecordType->papFldDes[952]->offset=(short)((char *)&prec->d67pr - (char *)prec);
  pdbRecordType->papFldDes[953]->size=sizeof(prec->d68hr);
  pdbRecordType->papFldDes[953]->offset=(short)((char *)&prec->d68hr - (char *)prec);
  pdbRecordType->papFldDes[954]->size=sizeof(prec->d68lr);
  pdbRecordType->papFldDes[954]->offset=(short)((char *)&prec->d68lr - (char *)prec);
  pdbRecordType->papFldDes[955]->size=sizeof(prec->d68da);
  pdbRecordType->papFldDes[955]->offset=(short)((char *)&prec->d68da - (char *)prec);
  pdbRecordType->papFldDes[956]->size=sizeof(prec->d68ca);
  pdbRecordType->papFldDes[956]->offset=(short)((char *)&prec->d68ca - (char *)prec);
  pdbRecordType->papFldDes[957]->size=sizeof(prec->d68cv);
  pdbRecordType->papFldDes[957]->offset=(short)((char *)&prec->d68cv - (char *)prec);
  pdbRecordType->papFldDes[958]->size=sizeof(prec->d68lv);
  pdbRecordType->papFldDes[958]->offset=(short)((char *)&prec->d68lv - (char *)prec);
  pdbRecordType->papFldDes[959]->size=sizeof(prec->d68ne);
  pdbRecordType->papFldDes[959]->offset=(short)((char *)&prec->d68ne - (char *)prec);
  pdbRecordType->papFldDes[960]->size=sizeof(prec->d68eu);
  pdbRecordType->papFldDes[960]->offset=(short)((char *)&prec->d68eu - (char *)prec);
  pdbRecordType->papFldDes[961]->size=sizeof(prec->d68pr);
  pdbRecordType->papFldDes[961]->offset=(short)((char *)&prec->d68pr - (char *)prec);
  pdbRecordType->papFldDes[962]->size=sizeof(prec->d69hr);
  pdbRecordType->papFldDes[962]->offset=(short)((char *)&prec->d69hr - (char *)prec);
  pdbRecordType->papFldDes[963]->size=sizeof(prec->d69lr);
  pdbRecordType->papFldDes[963]->offset=(short)((char *)&prec->d69lr - (char *)prec);
  pdbRecordType->papFldDes[964]->size=sizeof(prec->d69da);
  pdbRecordType->papFldDes[964]->offset=(short)((char *)&prec->d69da - (char *)prec);
  pdbRecordType->papFldDes[965]->size=sizeof(prec->d69ca);
  pdbRecordType->papFldDes[965]->offset=(short)((char *)&prec->d69ca - (char *)prec);
  pdbRecordType->papFldDes[966]->size=sizeof(prec->d69cv);
  pdbRecordType->papFldDes[966]->offset=(short)((char *)&prec->d69cv - (char *)prec);
  pdbRecordType->papFldDes[967]->size=sizeof(prec->d69lv);
  pdbRecordType->papFldDes[967]->offset=(short)((char *)&prec->d69lv - (char *)prec);
  pdbRecordType->papFldDes[968]->size=sizeof(prec->d69ne);
  pdbRecordType->papFldDes[968]->offset=(short)((char *)&prec->d69ne - (char *)prec);
  pdbRecordType->papFldDes[969]->size=sizeof(prec->d69eu);
  pdbRecordType->papFldDes[969]->offset=(short)((char *)&prec->d69eu - (char *)prec);
  pdbRecordType->papFldDes[970]->size=sizeof(prec->d69pr);
  pdbRecordType->papFldDes[970]->offset=(short)((char *)&prec->d69pr - (char *)prec);
  pdbRecordType->papFldDes[971]->size=sizeof(prec->d70hr);
  pdbRecordType->papFldDes[971]->offset=(short)((char *)&prec->d70hr - (char *)prec);
  pdbRecordType->papFldDes[972]->size=sizeof(prec->d70lr);
  pdbRecordType->papFldDes[972]->offset=(short)((char *)&prec->d70lr - (char *)prec);
  pdbRecordType->papFldDes[973]->size=sizeof(prec->d70da);
  pdbRecordType->papFldDes[973]->offset=(short)((char *)&prec->d70da - (char *)prec);
  pdbRecordType->papFldDes[974]->size=sizeof(prec->d70ca);
  pdbRecordType->papFldDes[974]->offset=(short)((char *)&prec->d70ca - (char *)prec);
  pdbRecordType->papFldDes[975]->size=sizeof(prec->d70cv);
  pdbRecordType->papFldDes[975]->offset=(short)((char *)&prec->d70cv - (char *)prec);
  pdbRecordType->papFldDes[976]->size=sizeof(prec->d70lv);
  pdbRecordType->papFldDes[976]->offset=(short)((char *)&prec->d70lv - (char *)prec);
  pdbRecordType->papFldDes[977]->size=sizeof(prec->d70ne);
  pdbRecordType->papFldDes[977]->offset=(short)((char *)&prec->d70ne - (char *)prec);
  pdbRecordType->papFldDes[978]->size=sizeof(prec->d70eu);
  pdbRecordType->papFldDes[978]->offset=(short)((char *)&prec->d70eu - (char *)prec);
  pdbRecordType->papFldDes[979]->size=sizeof(prec->d70pr);
  pdbRecordType->papFldDes[979]->offset=(short)((char *)&prec->d70pr - (char *)prec);
  pdbRecordType->papFldDes[980]->size=sizeof(prec->t1cd);
  pdbRecordType->papFldDes[980]->offset=(short)((char *)&prec->t1cd - (char *)prec);
  pdbRecordType->papFldDes[981]->size=sizeof(prec->t2cd);
  pdbRecordType->papFldDes[981]->offset=(short)((char *)&prec->t2cd - (char *)prec);
  pdbRecordType->papFldDes[982]->size=sizeof(prec->t3cd);
  pdbRecordType->papFldDes[982]->offset=(short)((char *)&prec->t3cd - (char *)prec);
  pdbRecordType->papFldDes[983]->size=sizeof(prec->t4cd);
  pdbRecordType->papFldDes[983]->offset=(short)((char *)&prec->t4cd - (char *)prec);
  pdbRecordType->papFldDes[984]->size=sizeof(prec->a1cd);
  pdbRecordType->papFldDes[984]->offset=(short)((char *)&prec->a1cd - (char *)prec);
  pdbRecordType->papFldDes[985]->size=sizeof(prec->bscd);
  pdbRecordType->papFldDes[985]->offset=(short)((char *)&prec->bscd - (char *)prec);
  pdbRecordType->papFldDes[986]->size=sizeof(prec->ascd);
  pdbRecordType->papFldDes[986]->offset=(short)((char *)&prec->ascd - (char *)prec);
  pdbRecordType->papFldDes[987]->size=sizeof(prec->paus);
  pdbRecordType->papFldDes[987]->offset=(short)((char *)&prec->paus - (char *)prec);
  pdbRecordType->papFldDes[988]->size=sizeof(prec->lpau);
  pdbRecordType->papFldDes[988]->offset=(short)((char *)&prec->lpau - (char *)prec);
  pdbRecordType->papFldDes[989]->size=sizeof(prec->pdly);
  pdbRecordType->papFldDes[989]->offset=(short)((char *)&prec->pdly - (char *)prec);
  pdbRecordType->papFldDes[990]->size=sizeof(prec->ddly);
  pdbRecordType->papFldDes[990]->offset=(short)((char *)&prec->ddly - (char *)prec);
  pdbRecordType->papFldDes[991]->size=sizeof(prec->rdly);
  pdbRecordType->papFldDes[991]->offset=(short)((char *)&prec->rdly - (char *)prec);
  pdbRecordType->papFldDes[992]->size=sizeof(prec->faze);
  pdbRecordType->papFldDes[992]->offset=(short)((char *)&prec->faze - (char *)prec);
  pdbRecordType->papFldDes[993]->size=sizeof(prec->acqm);
  pdbRecordType->papFldDes[993]->offset=(short)((char *)&prec->acqm - (char *)prec);
  pdbRecordType->papFldDes[994]->size=sizeof(prec->acqt);
  pdbRecordType->papFldDes[994]->offset=(short)((char *)&prec->acqt - (char *)prec);
  pdbRecordType->papFldDes[995]->size=sizeof(prec->dstate);
  pdbRecordType->papFldDes[995]->offset=(short)((char *)&prec->dstate - (char *)prec);
  pdbRecordType->papFldDes[996]->size=sizeof(prec->copyto);
  pdbRecordType->papFldDes[996]->offset=(short)((char *)&prec->copyto - (char *)prec);
    pdbRecordType->rec_size = sizeof(*prec);
    return(0);
}
epicsExportRegistrar(sscanRecordSizeOffset);
#ifdef __cplusplus
}
#endif
#endif /*GEN_SIZE_OFFSET*/
