/* loadElfLib.h - object module dependant loader library header */

/* Copyright 1996-2001 Wind River Systems, Inc. */

/*
modification history
--------------------
01b,05sep01,jn   changing ARM to ELF and created loadElfLibP.h - 
                 moved utility macros and private type and structure
		 definitions to loadElfLibP.h
01a,20jun96,dbt  written from /host/include/loadelf.h v01c
		 and /host/include/elfppc.h v01c
*/

#ifndef __INCloadElfLibh
#define __INCloadElfLibh

#ifdef __cplusplus
extern "C" {
#endif

#include "vwModNum.h"
#include "elf.h"

#include "private/loadElfLibP.h"

/* Defines */

#define MAX_SEC_LEN     256

#define SDA_SCN_TYPE    unsigned char
#define NOT_SDA_SCN     0
#define SDA_SCN         1
#define SDA2_SCN        2

/* status codes */

#define S_loadElfLib_HDR_READ			(M_loadElfLib | 1)
#define S_loadElfLib_HDR_ERROR			(M_loadElfLib | 2)
#define S_loadElfLib_PHDR_MALLOC		(M_loadElfLib | 3)
#define S_loadElfLib_PHDR_READ			(M_loadElfLib | 4)
#define S_loadElfLib_SHDR_MALLOC		(M_loadElfLib | 5)
#define S_loadElfLib_SHDR_READ			(M_loadElfLib | 6)
#define S_loadElfLib_READ_SECTIONS		(M_loadElfLib | 7)
#define S_loadElfLib_LOAD_SECTIONS		(M_loadElfLib | 8)
#define S_loadElfLib_LOAD_PROG			(M_loadElfLib | 9)
#define S_loadElfLib_SYMTAB			(M_loadElfLib | 10)
#define S_loadElfLib_RELA_SECTION		(M_loadElfLib | 11)
#define S_loadElfLib_SCN_READ			(M_loadElfLib | 12)
#define S_loadElfLib_SDA_MALLOC			(M_loadElfLib | 13)
#define S_loadElfLib_SST_READ			(M_loadElfLib | 15)

#define S_loadElfLib_JMPADDR_ERROR		(M_loadElfLib | 20)
#define S_loadElfLib_GPREL_REFERENCE		(M_loadElfLib | 21)
#define S_loadElfLib_UNRECOGNIZED_RELOCENTRY	(M_loadElfLib | 22)
#define S_loadElfLib_RELOC			(M_loadElfLib | 23)
#define S_loadElfLib_UNSUPPORTED		(M_loadElfLib | 24)
#define S_loadElfLib_REL_SECTION		(M_loadElfLib | 25)

typedef enum    /* RTYPE_SPARC - sparc relocation types */
    {
    RELOC_8,        RELOC_16,        RELOC_32,          /* simplest relocs */
    RELOC_DISP8,    RELOC_DISP16,    RELOC_DISP32,      /* Disp's (pc-rel) */
    RELOC_WDISP30,  RELOC_WDISP22,                      /* SR word disp's */
    RELOC_HI22,     RELOC_22,                           /* SR 22-bit relocs */
    RELOC_13,       RELOC_LO10,                         /* SR 13&10-bit relocs*/
    RELOC_SFA_BASE, RELOC_SFA_OFF13,                    /* SR S.F.A. relocs */
    RELOC_BASE10,   RELOC_BASE13,    RELOC_BASE22,      /* base_relative pic */
    RELOC_PC10,     RELOC_PC22,                         /* special pc-rel pic */
    RELOC_JMP_TBL,                                      /* jmp_tbl_rel in pic */
    RELOC_SEGOFF16,                                     /* ShLib offset-in-seg*/
    RELOC_GLOB_DAT, RELOC_JMP_SLOT,  RELOC_RELATIVE     /* rtld relocs */
    } RTYPE_SPARC;

/* function declarations */

extern STATUS loadElfInit (void);

#ifdef __cplusplus
}
#endif

#endif /* __INCloadElfLibh */
