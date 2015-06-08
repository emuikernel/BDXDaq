/* a_out.h - a.out object module header */

/* Copyright 1984-1994 Wind River Systems, Inc. */

/*
modification history
--------------------
02d,11oct96,pad  Added M_386 to be in compliance with gcc 2.7
02c,21jul95,pad  updated to C++ symbol types from gcc version 2.6-95q2
02b,01dec94,pad  cleaned up.
02a,28sep94,pad  adaptation to tornado.
*/

/*
This file reflects the a.out format as handled by the Target Server Loader.
*/

#ifndef __INCa_outh
#define __INCa_outh

#ifdef __cplusplus
extern "C" {
#endif

struct exec
    {
#if	defined(HOST_SUN)
    unsigned  a_dynamic:1;     /* has a __DYNAMIC */
    unsigned  a_toolversion:7; /* version of toolset used to create file */
    unsigned  a_machtype: 8;   /* machine type */
    unsigned  a_magic:16;      /* magic number */
#else
    long		a_magic;	/* magic number */
#define a_machtype 	a_magic
#endif	/* HOST_SUN */

    unsigned long	a_text;		/* size of text segment */
    unsigned long	a_data;		/* size of initialized data */
    unsigned long	a_bss;		/* size of uninitialized data */
    unsigned long	a_syms;		/* size of symbol table */
    unsigned long	a_entry;	/* entry point */
    unsigned long	a_trsize;	/* size of text relocation */
    unsigned long	a_drsize;	/* size of data relocation */
    };


/* machine types */

#define M_OLDSUN2       0       /* old sun-2 (or not sun at all) */
#define M_68010         1       /* runs on either 68010 or 68020 */
#define M_68020         2       /* runs only on 68020 */
#define M_SPARC         3       /* runs only on SPARC */
#define M_R3000         4       /* runs only on R3000 */
#define M_386		100	/* runs only on I80X86 */

#define OMAGIC	0x107		/* not write-protected text */
#define NMAGIC  0x108		/* write-protected text */
#define ZMAGIC	0x10b		/* page aligned segments */


/*
 * Macros which take exec structures as arguments and tell whether
 * the file has a reasonable magic number or offsets to text|symbols|strings.
 */

#define	N_OLDOFF(x) \
	((x).a_magic==ZMAGIC ? 0 : sizeof (struct exec))

#define	N_TXTOFF(x) \
	(sizeof (struct exec))

#define	N_DATOFF(x) \
	(N_TXTOFF(x) + (x).a_text)

#define N_SYMOFF(x) \
	(N_OLDOFF(x) + (x).a_text+(x).a_data + (x).a_trsize+(x).a_drsize)

#define	N_STROFF(x) \
	(N_SYMOFF(x) + (x).a_syms)

/* Relocation entry structures */

typedef struct  /* RINFO_68K - 680X0 relocation datum (a_machtype != M_SPARC) */    {
    unsigned    r_address;      /* address which is relocated */
    unsigned    r_symbolnum:24; /* local symbol ordinal */
    unsigned    r_pcrel    : 1; /* was relocated pc relative already */
    unsigned    r_length   : 2; /* 0=byte, 1=word, 2=long */
    unsigned    r_extern   : 1; /* does not include value of sym referenced */
    unsigned    r_baserel  : 1; /* linkage table relative */
    unsigned    r_jmptable : 1; /* pc-relative to jump table */
    unsigned    r_relative : 1; /* relative relocation */
    unsigned               : 1; /* <unused> */
    } RINFO_68K;


typedef enum    /* RTYPE_SPARC - sparc relocation types */
    {
    RELOC_8,        RELOC_16,        RELOC_32,          /* simplest relocs    */
    RELOC_DISP8,    RELOC_DISP16,    RELOC_DISP32,      /* Disp's (pc-rel)    */
    RELOC_WDISP30,  RELOC_WDISP22,                      /* SR word disp's     */
    RELOC_HI22,     RELOC_22,                           /* SR 22-bit relocs   */
    RELOC_13,       RELOC_LO10,                         /* SR 13&10-bit relocs*/
    RELOC_SFA_BASE, RELOC_SFA_OFF13,                    /* SR S.F.A. relocs   */
    RELOC_BASE10,   RELOC_BASE13,    RELOC_BASE22,      /* base_relative pic  */
    RELOC_PC10,     RELOC_PC22,                         /* special pc-rel pic */
    RELOC_JMP_TBL,                                      /* jmp_tbl_rel in pic */
    RELOC_SEGOFF16,                                     /* ShLib offset-in-seg*/
    RELOC_GLOB_DAT, RELOC_JMP_SLOT,  RELOC_RELATIVE     /* rtld relocs        */
    } RTYPE_SPARC;


typedef struct  /* RINFO_SPARC - sparc reloc datum (a_machtype == M_SPARC) */
    {
    unsigned    r_address;      /* relocation addr (offset in segment)*/
    unsigned    r_index   :24;  /* segment index or symbol index      */
    unsigned    r_extern  : 1;  /* if F, r_index==SEG#; if T, SYM idx */
    unsigned              : 2;  /* <unused>                           */
    unsigned    r_type    : 5;  /* type of relocation to perform      */
    long        r_addend;       /* addend for relocation value        */
    } RINFO_SPARC;

typedef struct /* RINFO_MIPS - mips reloc datum (a_machtype == M_R3000) */
    {
    int r_address;              /* address which is relocated */
    unsigned int
    r_symbolnum:24, 		/* local symbol ordinal */
    r_pcrel:1,      		/* was relocated pc relative already */
    r_length:2,     		/* 0=byte, 1=word, 2=long */
    r_extern:1,     		/* does not include value of sym referenced */
    r_type:4;       		/* relocation type */
    } RINFO_MIPS;

/*
 * Format of a symbol table entry; this file is included by <a.out.h>
 * and should be used if you aren't interested the a.out header
 * or relocation information.
 */

struct	nlist
    {
    union
	{
	char	*n_name;	  /* for use when in-core */
	long	n_strx;		  /* index into file string table */
	} n_un;
    unsigned char	n_type;	  /* type flag, i.e. N_TEXT etc; see below */
    char		n_other;  /* unused */
    short		n_desc;	  /* see <stab.h> */
    unsigned long 	n_value;  /* value of this symbol (or sdb offset) */
    };

#define	n_hash	n_desc		  /* used internally by ld */

/*
 * Simple values for n_type.
 */

#define	N_UNDF	0x0		/* undefined */
#define	N_ABS	0x2		/* absolute */
#define	N_TEXT	0x4		/* text */
#define	N_DATA	0x6		/* data */
#define	N_BSS	0x8		/* bss */
#define	N_COMM	0x12		/* common (internal to ld) */
#define N_SETA	0x14		/* Absolute set element symbol */
#define N_SETT	0x16		/* Text set element symbol */
#define N_SETD	0x18		/* Data set element symbol */
#define N_SETB	0x1a		/* Bss set element symbol */
#define	N_FN	0x1f		/* file name symbol */

#define	N_EXT	01		/* external bit, or'ed in for public sym */
#define	N_TYPE	0x1e		/* mask for all the type bits */

/*
 * Sdb entries have some of the N_STAB bits set.
 * These are given in <stab.h>
 */

#define	N_STAB	0xe0		/* if any of these bits set, a SDB entry */

/*
 * The different systems use different names for the same info in some
 * structures.  Make them the same here.
 */

#define A_OUT_HDR       struct exec
#define TEXTSIZE        a_text
#define DATASIZE        a_data
#define BSSSIZE         a_bss
#define TRSIZE          a_trsize
#define DRSIZE          a_drsize

#define SYMENT		struct  nlist

/*
 * Format for namelist values.
 */

#define	N_FORMAT	"%08x"

#ifdef __cplusplus
}
#endif

#endif /* __INCa_outh */
