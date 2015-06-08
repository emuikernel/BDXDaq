/* ntcontext.h - NT context record header */

/* Copyright 1998-2001 Wind River Systems, Inc. */

/*
modification history
--------------------
01c,13sep01,hbh  Removed BYTE type (SPR 32165)
01b,21jan98,jmb  Added modhist, converted comments to C style.
01a,12jun98,cym  created.
*/

#ifndef __INCntcontexth
#define __INCntcontexth

#ifdef __cplusplus
extern "C" {
#endif

typedef void * HANDLE;
typedef unsigned short int WORD;
typedef unsigned long DWORD;



#define CREATE_SUSPENDED 0x4L

/*
 *  Define the size of the 80387 save area, which is in the context frame.
 */

#define SIZE_OF_80387_REGISTERS      80

/*
 * The following flags control the contents of the CONTEXT structure.
 */

#if !defined(RC_INVOKED)

#define CONTEXT_i386    0x00010000    /* this assumes that i386 and */
#define CONTEXT_i486    0x00010000    /* i486 have identical context records */

/* end_wx86 */

#define CONTEXT_CONTROL         (CONTEXT_i386 | 0x00000001L) /* SS:SP, CS:IP, FLAGS, BP */
#define CONTEXT_INTEGER         (CONTEXT_i386 | 0x00000002L) /* AX, BX, CX, DX, SI, DI */
#define CONTEXT_SEGMENTS        (CONTEXT_i386 | 0x00000004L) /* DS, ES, FS, GS */
#define CONTEXT_FLOATING_POINT  (CONTEXT_i386 | 0x00000008L) /* 387 state */
#define CONTEXT_DEBUG_REGISTERS (CONTEXT_i386 | 0x00000010L) /* DB 0-3,6,7 */

#define CONTEXT_FULL (CONTEXT_CONTROL | CONTEXT_INTEGER |\
                      CONTEXT_SEGMENTS)

/* begin_wx86 */

#endif

typedef struct _FLOATING_SAVE_AREA {
    DWORD   ControlWord;
    DWORD   StatusWord;
    DWORD   TagWord;
    DWORD   ErrorOffset;
    DWORD   ErrorSelector;
    DWORD   DataOffset;
    DWORD   DataSelector;
    char    RegisterArea[SIZE_OF_80387_REGISTERS];
    DWORD   Cr0NpxState;
} FLOATING_SAVE_AREA;

typedef FLOATING_SAVE_AREA *PFLOATING_SAVE_AREA;

/*
 * Context Frame
 *
 *  This frame has a several purposes: 1) it is used as an argument to
 *  NtContinue, 2) is is used to constuct a call frame for APC delivery,
 *  and 3) it is used in the user level thread creation routines.
 *
 *  The layout of the record conforms to a standard call frame.
 */

typedef struct _CONTEXT {

    /*
     * The flags values within this flag control the contents of
     * a CONTEXT record.
     *
     * If the context record is used as an input parameter, then
     * for each portion of the context record controlled by a flag
     * whose value is set, it is assumed that that portion of the
     * context record contains valid context. If the context record
     * is being used to modify a threads context, then only that
     * portion of the threads context will be modified.
     *
     * If the context record is used as an IN OUT parameter to capture
     * the context of a thread, then only those portions of the thread's
     * context corresponding to set flags will be returned.
     *
     * The context record is never used as an OUT only parameter.
     */

    DWORD ContextFlags;

    /*
     * This section is specified/returned if CONTEXT_DEBUG_REGISTERS is
     * set in ContextFlags.  Note that CONTEXT_DEBUG_REGISTERS is NOT
     * included in CONTEXT_FULL.
     */

    DWORD   Dr0;
    DWORD   Dr1;
    DWORD   Dr2;
    DWORD   Dr3;
    DWORD   Dr6;
    DWORD   Dr7;

    /*
     * This section is specified/returned if the
     * ContextFlags word contians the flag CONTEXT_FLOATING_POINT.
     */

    FLOATING_SAVE_AREA FloatSave;

    /*
     * This section is specified/returned if the
     * ContextFlags word contians the flag CONTEXT_SEGMENTS.
     */

    DWORD   SegGs;
    DWORD   SegFs;
    DWORD   SegEs;
    DWORD   SegDs;

    /*
     * This section is specified/returned if the
     * ContextFlags word contians the flag CONTEXT_INTEGER.
     */

    DWORD   Edi;
    DWORD   Esi;
    DWORD   Ebx;
    DWORD   Edx;
    DWORD   Ecx;
    DWORD   Eax;

    /*
     * This section is specified/returned if the
     * ContextFlags word contians the flag CONTEXT_CONTROL.
     */

    DWORD   Ebp;
    DWORD   Eip;
    DWORD   SegCs;              /* MUST BE SANITIZED */
    DWORD   EFlags;             /* MUST BE SANITIZED */
    DWORD   Esp;
    DWORD   SegSs;

} CONTEXT;

typedef CONTEXT *PCONTEXT;

#ifdef __cplusplus
}
#endif

#endif /* __INCntcontexth */
