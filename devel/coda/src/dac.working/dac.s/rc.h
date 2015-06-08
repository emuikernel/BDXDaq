/*----------------------------------------------------------------------------*
 *  Copyright (c) 1991, 1992  Southeastern Universities Research Association, * 
 *                            Continuous Electron Beam Accelerator Facility   *
 *                                                                            *
 *    This software was developed under a United States Government license    *
 *    described in the NOTICE file included as part of this distribution.     *
 *                                                                            *
 * CEBAF Data Acquisition Group, 12000 Jefferson Ave., Newport News, VA 23606 *
 *      heyes@cebaf.gov   Tel: (804) 249-7030    Fax: (804) 249-7363          *
 *----------------------------------------------------------------------------*
 * Discription: follows this header.
 *
 * Author:
 *	Graham Heyes
 *	CEBAF Data Acquisition Group
 *
 * Revision History:
 *      $Log: rc.h,v $
 *      Revision 1.3  1998/05/27 13:45:08  heyes
 *      add message Q to ROC, improve EB stability on EBD transition
 *
 *      Revision 1.1.1.1  1996/08/21 19:18:58  heyes
 *      Imported sources
 *
*	  Revision 1.1  94/05/04  13:23:52  13:23:52  heyes (Graham Heyes)
*	  Initial revision
*	  
*	  Revision 1.1  94/03/15  11:54:07  11:54:07  heyes (Graham Heyes)
*	  Initial revision
*	  
*	  Revision 1.1  93/10/27  08:48:38  08:48:38  heyes (Graham Heyes)
*	  Initial revision
*	  
 *	  Revision 1.13  93/08/16  10:07:12  10:07:12  heyes (Graham Heyes)
 *	  found checked out...
 *	  
 *	  Revision 1.12  93/03/16  14:04:46  14:04:46  heyes (Graham Heyes)
 *	  reinsert for coda 1.2
 *	  
 *	  Revision 1.11  1992/11/06  19:40:06  quarrie
 *	  Add DAREADMESSAGES etc.
 *
 *	  Revision 1.10  1992/11/05  18:04:00  heyes
 *	  ing states added correctly
 *
 *	  Revision 1.9  1992/10/22  19:15:43  heyes
 *	  1.1.3
 *
 *	  Revision 1.8  1992/09/09  13:32:16  quarrie
 *	  Added more Actions
 *
 *	  Revision 1.7  1992/09/01  13:34:16  quarrie
 *	  Revert to versionn 1.5 for coda1.1.3
 *
 *	  Revision 1.5  1992/07/29  19:01:11  heyes
 *	  fixed camac read
 *
 *	  Revision 1.4  1992/06/29  15:49:02  heyes
 *	  made consistent with obj.h
 *
 *	  Revision 1.3  1992/06/25  19:49:12  heyes
 *	  Add Mastership Procedure Codes and Error Codes
 *
 *	  Revision 1.2  1992/06/25  19:01:28  heyes
 *	  insert into RCS for first time
 *
 *	  Revision 1.1  1992/06/25  18:30:36  heyes
 *	  Initial revision
 *
 *
 *----------------------------------------------------------------------------*/
#ifndef _RC_INCLUDED_
#define _RC_INCLUDED_

#define MAXCLNT 30

#define DANULLPROC     ((u_long)0)
#define DACREATE       ((u_long)1)
#define DAREMOVE       ((u_long)2)
#define DADOWNLOAD     ((u_long)3)
#define DAPRESTART     ((u_long)4)
#define DAEND          ((u_long)5)
#define DAPAUSE        ((u_long)6)
#define DAGO           ((u_long)7)
#define DATERMINATE    ((u_long)8)
#define DAREPORT       ((u_long)9)
#define DAREADINT      ((u_long)10)
#define DAREADSTRING   ((u_long)11)
#define DAREADREAL     ((u_long)12)
#define DAWRITEINT     ((u_long)13)
#define DAWRITESTRING  ((u_long)14)
#define DAWRITEREAL    ((u_long)15)

#define DAMODIFYINT    ((u_long)16)
#define DAMODIFYSTRING ((u_long)17)
#define DAMODIFYREAL   ((u_long)18)
#define DAWRITEEVENT   ((u_long)19)
#define DACONFIGURE    ((u_long)20)
#define DARESET        ((u_long)21)
#define DABECOMEMASTER ((u_long)22)
#define DACANCELMASTER ((u_long)23)
#define DAISMASTER     ((u_long)24)
#define DASYNC         ((u_long)25)
#define DAREADMESSAGES ((u_long)26)
#define DAINSERTEVENT  ((u_long)27)
#define DABCREPLY      ((u_long)28)
#define DAREQEVENT     ((u_long)29)
#define DADUMP         ((u_long)30)
#define DAZAP          ((u_long)-1)

#define DA_ACTIONS       30

/* Error Codes */

#define RC__SUCCESS         0
#define RC__RPC_FAILURE    -1
#define RC__NOT_REGISTERED -3
#define RC__NOT_MASTER     -4

typedef struct rp {
  int a ;
  int b ;
} rpStruct;

typedef struct rp *runparameters;
typedef struct rp rp;

typedef struct reti {
  int a;
  int b;
} retiStruct;

typedef struct arg_rs *retrs;

struct arg_rs {
	int value;
	char *name;
} arg_rsStruct;

typedef struct arg_rs arg_rs;

typedef struct retr {
  int a;
  float b;
} retrStruct;

typedef struct argw_f {
  char *name;
  float value;
} argw_fStruct;

typedef struct argw_i {
  char *name;
  int value;
} argw_iStruct;

typedef struct argw_s {
  char *name;
  char *value;
} argw_sStruct;

extern int xdr_record();
extern int xdr_event();
extern int xdr_rp();
extern int xdr_ri();
extern int xdr_arg_rs();
extern int xdr_retrs();
extern int xdr_rr();
extern int xdr_wf();
extern int xdr_wi();
extern int xdr_ws();

extern int *dacreate();
extern int *daremove();
extern int *dadownload();
extern int *daprestart();
extern int *daend();
extern int *dapause();
extern int *dago();
extern int *daterminate();
extern int *dareport();
extern int daWriteEvent();
extern int rcConnect();
extern struct reti *dareadint();
extern retrs *dareadstring();
extern int daSync();
extern char *rcStates();
extern int *dareqevent();
extern int lastContext();
extern int restoreContext();
extern int daHandleSpy();


/******************************************/
/* heartbeat functions (coda_component.c) */

#define HB_MAX  1/*4*/  /* the number of monitored threads */

#define HB_ROL  0  /* from ROL */
#define HB_TCP  1  /* from TCP */
#define HB_PMC  2  /* from PMC */

int checkHeartBeats();
int resetHeartBeats();
int setHeartBeat(int, int, int);

#endif
