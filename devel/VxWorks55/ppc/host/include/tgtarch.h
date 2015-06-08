/* tgtarch.h - target architecture desciption library header */

/* Copyright 1998-2001 Wind River Systems, Inc. */

/*
modification history
--------------------
01h,28nov01,jhw  Added tgtCpuFamObjDirGet() prototype.
01g,06nov98,c_c  Added close and ioTaskStdGet OS routine names.
01f,10jul98,c_c  Added routine names related to the target shell redirection.
01e,21apr98,c_c  Added a routine to get the arch dir and modified the compiler
                 leading char coding.
01d,17apr98,c_c  Added tgtCpuThumbEnabled routine
01c,10feb98,c_c  Added OS related routines. Modified Init prototype.
01b,09feb98,c_c  added support for the '-f' flag.
01a,22jan98,c_c  written.
*/

#ifndef __INCtgtarchh
#define __INCtgtarchh

#include "wtx.h"
#include "wtxtypes.h"

/* initialize the library with the given CPU number */

extern STATUS targetArchitectureInit 
    (
    WTX_TGT_INFO *	pWtxTgtInfo
    );

/* force to use the given OMF reader */

void tgtOmfNameSet
    (
    const char *        omfName		/* OMF name to set */
    );

/* get the name of a routine which can be used to get a task I/O setting */
extern char *  tgtTaskIOGetRtnNameGet (void);

/* get the name of a routine which can be used to perform I/O redirection */
extern char *  tgtRedirectRtnNameGet (void);

/* get the name of the open routine */
extern char *  tgtOpenRtnNameGet (void);

/* get the name of the close routine */
extern char *  tgtCloseRtnNameGet (void);

/* get the name of the OS running on the target */
extern char *  tgtOSNameGet (void);

/* get the name of the name of the shell task */
extern char *  tgtTargetShellTaskNameGet (void);

/* get the name of the routine used to get a task If from a task name. */
extern char *  tgtTaskNameToIdRtnNameGet (void);

/* get the name of the routine used to redirect the shell I/O */
extern char *  tgtShellOrigStdSetRtnNameGet (void);

/* get the name of the routine used to restart a task */
extern char *  tgtTaskRestartRtnNameGet (void);

/* get the name of the memory allocator routine */
extern char *  tgtAllocRtnNameGet (void);

/* get the name of the semaphore release routine */
extern char *  tgtSemGiveRtnNameGet (void);

/* get the CPU type */
extern int tgtCpuTypeGet (void); 

/* get the CPU Name */
extern char * tgtCpuNameGet (void);

/* get the thumbing status for this CPU */
extern BOOL tgtCpuThumbEnabled (void);

/* get the CPU family */
extern int tgtCpuFamilyGet (void);

/* get the CPU family Object Directory */
extern char * tgtCpuFamObjDirGet (void);

/* get the CPU family Name */
extern char * tgtCpuFamilyNameGet (void);

/* returns the target architecture dir name. */
extern char * tgtArchDirectoryNameGet (void);

/* get the CPU memory aligment */
extern int tgtCpuAlignmentGet (void);

/* returns TRUE if host and target byte order differs */
extern BOOL tgtSwapNeeded (void);

/* get the offset to skip for the checksum of the core file */
extern int tgtCoreFileOffsetGet (void);

/* get the number of OMF supported by the target */
extern int tgtOmfSupportedGet (void);

/* get the first target loader OMF type */
extern char * tgtFirstLoaderOmfTypeGet (void); 

/* returns the leading char for symbol names  (if any) */
extern char  tgtLeadingCharGet (void); 

/* Get the name of the compiler used to build the target core */
extern char * tgtCompilerNameGet (void);

/* Set the name of the compiler used to build the target core */
extern void tgtCompilerNameSet
    (
    char *	compilerName
    );

/* get the next target loader OMF type */
extern char * tgtNextLoaderOmfTypeGet (void); 

/* get the target loader OMF in use */
extern char * tgtLoaderOmfTypeGet (void); 

/* get the target loader name in use */
extern char * tgtLoaderNameGet (void); 

/* get the target loader init routine name*/
extern char * tgtLoaderInitRtnNameGet (void); 

/* get the target loader check routine name*/
extern char * tgtLoaderCheckRtnNameGet (void); 

/* get the target loader management routine name*/
extern char * tgtLoaderManageRtnNameGet (void); 

/* get the target relocator name in use */
extern char * tgtRelocatorNameGet (void); 

/* get the target segment relocator routine name */
extern char * tgtSegRelocatorRtnNameGet (void); 

/* get the target segment verifier routine name */
extern char * tgtModuleVerifyRtnNameGet (void); 

/* get the target relocator library init routine name */
extern char * tgtRelocInitRtnNameGet (void); 

/* get the target disassembler DLL name */
extern char * tgtDisassemblerNameGet (void); 

/* get the target disassembler Print routine name */
extern char * tgtDasmPrintRtnNameGet (void); 

/* get the target disassembler Count routine name */
extern char * tgtDasmCountRtnNameGet (void); 

/* get the C++ loader/unloader DLL name */
extern char * tgtCplusLoaderNameGet (void);

/* get the C++ mangler/demangler DLL name */
extern char * tgtDemanglerNameGet (void);

#endif
