/* wdbRegs.h - header file for register layout as view by the WDB agent */

/* Copyright 1984-2002 Wind River Systems, Inc. */

/*
modification history
--------------------
01i,25apr02,jhw  Added C++ support (SPR 76304).
01h,13jul01,kab  Cleanup for merge to mainline
01g,13apr01,pcs  Add Altivec Support.
01f,15mar01,pcs  Change include filename altiVecLib.h to altivecLib.h
01e,29jan01,dtr  Adding altivec register support.
01e,28feb00,frf  Add SH4 support for T2
01d,31jul98,kab  Added DSP register support.
01d,12feb99,dbt  fixed __STDC__ use.
01c,17dec96,ms	 WDB now uses FP_CONTEXT instead of FPREG_SET (SPR 7654).
01b,25may95,ms	 defined register objects.
01a,03may95,ms   written.
*/

#ifndef __INCwdbRegsh
#define __INCwdbRegsh

#ifdef __cplusplus
extern "C" {
#endif

/* includes */

#include "wdb/dll.h"
#include "wdb/wdb.h"
#include "regs.h"		/* WDB_IU_REGS = VxWorks REG_SET */
#include "fppLib.h"		/* WDB_FPU_REGS = VxWorks FP_CONTEXT */
#include "dspLib.h"		/* WDB_DSP_REGS = VxWorks DSP_CONTEXT */

#ifdef  _WRS_ALTIVEC_SUPPORT
#include "altivecLib.h"
#endif /* _WRS_ALTIVEC_SUPPORT */

/* data types */

#if     CPU_FAMILY==MC680X0
typedef struct WDB_IU_REGS
    {
    REG_SET     regSet;
    int         padding;
    } WDB_IU_REGS;
#else
typedef REG_SET WDB_IU_REGS;
#endif

typedef FP_CONTEXT WDB_FPU_REGS;
typedef DSP_CONTEXT WDB_DSP_REGS;

typedef struct
    {
    dll_t		node;
    WDB_REG_SET_TYPE	regSetType;		/* type of register set */
    void		(*save) (void);		/* save current regs */
    void		(*load) (void);		/* load saved regs */
    void		(*get)  (char ** ppRegs); /* get saved regs */
    void		(*set)  (char *pRegs);	/* change saved regs */
    } WDB_REG_SET_OBJ;

#ifdef  _WRS_ALTIVEC_SUPPORT

typedef ALTIVEC_CONTEXT WDB_ALTIVEC_REGS;

typedef struct
    {
     /*  NOTE:  altivecContext has to be the first element in the struct.
             altivecContext has to be 16 byte aligned. Anyone who uses
             this object should define it as a pointer and memalign to
             get it 16 byte aligned.
     */
    WDB_ALTIVEC_REGS    altivecContext; /* the hardware context */
    WDB_REG_SET_OBJ     regSet;         /* generic register set */
    } ALTIVEC_REG_SET_OBJ;

#endif /* _WRS_ALTIVEC_SUPPORT */


/* function prototypes */

#ifdef  __STDC__
extern  WDB_REG_SET_OBJ * wdbAltivecLibInit (void);
extern	WDB_REG_SET_OBJ * wdbFpLibInit		(void);
extern	WDB_REG_SET_OBJ * wdbDspLibInit		(void);
extern	void              wdbExternRegSetObjAdd (WDB_REG_SET_OBJ *pRegSet);

#else	/* ! __STDC__ */
extern  WDB_REG_SET_OBJ * wdbAltivecLibInit ();
extern	WDB_REG_SET_OBJ * wdbFpLibInit		();
extern	WDB_REG_SET_OBJ * wdbDspLibInit		();
extern	void              wdbExternRegSetObjAdd ();

#endif	/* __STDC__ */

#ifdef __cplusplus
}
#endif

#endif  /* __INCwdbRegsh */
