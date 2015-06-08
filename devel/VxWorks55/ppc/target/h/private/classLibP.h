/* classLibP.h - private object class management library header file */

/* Copyright 1984-1997 Wind River Systems, Inc. */
/*
modification history
--------------------
01d,19may97,jpd  added _ASMLANGUAGE for offsets into OBJ_CLASS.
01c,10dec93,smb  added windview documentation
		 changed helpRtn to instRtn
01b,22sep92,rrr  added support for c++
01a,04jul92,jcf  created.
*/

#ifndef __INCclassLibPh
#define __INCclassLibPh

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _ASMLANGUAGE
#include "vxWorks.h"
#include "vwModNum.h"
#include "memLib.h"
#include "classLib.h"
#include "private/objLibP.h"

/* windview 
 * Instrumented object used in level 1 windview event logging use
 *
 * FUNCPTR             initRtn;        * object initialization routine *
 * FUNCPTR             instRtn;        * object instrument routine *
 *
 */

#define OBJ_INST_RTN	/* include redefinition of obj_class instRtn field */

typedef struct obj_class	/* OBJ_CLASS */
    {
    OBJ_CORE		objCore;	/* object core of class object */
    struct mem_part	*objPartId;	/* memory partition to allocate from */
    unsigned		objSize;	/* size of object */
    unsigned		objAllocCnt;	/* number of allocated objects */
    unsigned		objFreeCnt;	/* number of deallocated objects */
    unsigned		objInitCnt;	/* number of initialized objects */
    unsigned		objTerminateCnt;/* number of terminiated objects */
    int			coreOffset;	/* offset from object start to objCore*/
    FUNCPTR		createRtn;	/* object creation routine */
    FUNCPTR		initRtn;	/* object initialization routine */
    FUNCPTR		destroyRtn;	/* object destroy routine */
    FUNCPTR		showRtn;	/* object show routine */
    FUNCPTR		instRtn;	/* object inst routine */
    } OBJ_CLASS;


/* function declarations */

#if defined(__STDC__) || defined(__cplusplus)

extern CLASS_ID	classCreate (unsigned objectSize, int coreOffset,
			     FUNCPTR createRtn, FUNCPTR initRtn,
			     FUNCPTR destroyRtn);
extern STATUS	classInit (OBJ_CLASS *pObjClass, unsigned objectSize,
			   int coreOffset, FUNCPTR createRtn, FUNCPTR initRtn,
			   FUNCPTR destroyRtn);
extern STATUS	classDestroy (CLASS_ID classId);
extern STATUS	classInstConnect (CLASS_ID classId, FUNCPTR instRtn);
extern STATUS	classShowConnect (CLASS_ID classId, FUNCPTR showRtn);
extern STATUS   classInstrument ( OBJ_CLASS * pObjClass, 
				  OBJ_CLASS * pObjInstClass );


#else	/* __STDC__ */

extern CLASS_ID	classCreate ();
extern STATUS	classInit ();
extern STATUS	classDestroy ();
extern STATUS	classInstConnect ();
extern STATUS	classShowConnect ();
extern STATUS   classInstrument ();

#endif	/* __STDC__ */

#else   /* _ASMLANGUAGE */

#if (CPU_FAMILY == ARM)
#define OBJ_CLASS_initRtn	0x24	/* Offsets into OBJ_CLASS */
#endif

#endif  /* _ASMLANGUAGE */
   

#ifdef __cplusplus
}
#endif

#endif /* __INCclassLibPh */
