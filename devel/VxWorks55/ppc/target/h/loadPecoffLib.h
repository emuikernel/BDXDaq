/* loadPecoffLib.h - Pecoff object module header */

/* Copyright 1998-2001 Wind River Systems, Inc. */

/*
modification history
--------------------
01b,23oct01,pad  Added PE section flag extensions. Tidied up a bit.
01a,25mar98,cym  written based on loadCoffLib.h.
*/

#ifndef __INCloadPecoffLibh
#define __INCloadPecoffLibh

#ifdef __cplusplus
extern "C" {
#endif

#ifdef CPU
#include "symLib.h"
#include "moduleLib.h"
#endif

/******************************************************************************
 *
 * This file describes a PECOFF object file.
 *
 ******************************************************************************/

/******************************* DOS EXE HEADER ****************************/
struct dosheader {
    unsigned short e_magic;
    unsigned short e_cblp;
    unsigned short e_cp;
    unsigned short e_crlc;
    unsigned short e_cparhdr;
    unsigned short e_minalloc;
    unsigned short e_maxalloc;
    unsigned short e_ss;
    unsigned short e_sp;
    unsigned short e_csum;
    unsigned short e_ip;
    unsigned short e_cs;
    unsigned short e_lfarlc;
    unsigned short e_ovno;
    unsigned short e_res[4];
    unsigned short e_oemid;
    unsigned short e_oeminfo;
    unsigned short e_res2[10];
    unsigned long  e_lfanew;		/* Offset to the COFF image header */
    };


/******************************* FILE HEADER *******************************/

struct filehdr {
	unsigned short	f_magic;	/* magic number			*/
	unsigned short	f_nscns;	/* number of sections		*/
	long		f_timdat;	/* time & date stamp		*/
	long		f_symptr;	/* file pointer to symtab	*/
	long		f_nsyms;	/* number of symtab entries	*/
	unsigned short	f_opthdr;	/* sizeof(optional hdr)		*/
	unsigned short	f_flags;	/* flags			*/
};

/* Bits for f_flags:
 *	F_RELFLG	relocation info stripped from file
 *	F_EXEC		file is executable (no unresolved external references)
 *	F_LNNO		line numbers stripped from file
 *	F_LSYMS		local symbols stripped from file
 *	F_AR32WR	file has byte ordering of an AR32WR machine (e.g. vax)
 *	F_PIC		file contains position-independent code
 *	F_PID		file contains position-independent data
 *	F_LINKPID	file is suitable for linking w/pos-indep code or data
 */
#define F_RELFLG	0x0001
#define F_EXEC		0x0002
#define F_LNNO		0x0004
#define F_LSYMS		0x0008
#define F_AR32WR	0x0010
#define F_PIC		0x0040
#define F_PID		0x0080
#define F_LINKPID	0x0100

#define F_CCINFO	0x0800

/* Additional magic values from the PE format */

#define IMAGE_NT_SIGNATURE	0x00004550
#define IMAGE_DOS_SIGNATURE	0x5A4D
#define IMAGE_FILE_MACHINE_I386	0x14c

#define	FILHDR	struct filehdr
#define	FILHSZ	sizeof(FILHDR)

#undef  N_TXTOFF                /* undefined for a.out overlap */
#define N_TXTOFF(f) \
    ((f).f_opthdr == 0) ? \
    (FILHSZ + (f).f_nscns * SCNHSZ ) : \
    (FILHSZ + AOUTSZ + (f).f_nscns * SCNHSZ )

/************************* AOUT "OPTIONAL HEADER" *************************/

#define OMAGIC	0407
#define NMAGIC	0410

typedef	struct aouthdr {
	short		magic;	/* type of file				*/
	short		vstamp;	/* version stamp			*/
	unsigned long	tsize;	/* text size in bytes, padded to FW bdry*/
	unsigned long	dsize;	/* initialized data "  "		*/
	unsigned long	bsize;	/* uninitialized data "   "		*/
	unsigned long	entry;	/* entry pt.				*/
	unsigned long	text_start;	/* base of text used for this file */
	unsigned long	data_start;	/* base of data used for this file */
	unsigned long   image_base;	/* offset for the image (add above) */
	unsigned long   section_align;  /* ?? */
	unsigned long   file_align;     /* ?? */
} PEOPTION;

#define PEOPTSZ (sizeof(PEOPTION))


/****************************** SECTION HEADER ******************************/

struct scnhdr {
	char		s_name[8];	/* section name			*/
	long		s_paddr;	/* physical address, aliased s_nlib */
	long		s_vaddr;	/* virtual address		*/
	long		s_size;		/* section size			*/
	long		s_scnptr;	/* file ptr to raw data for section */
	long		s_relptr;	/* file ptr to relocation	*/
	long		s_lnnoptr;	/* file ptr to line numbers	*/
	unsigned short	s_nreloc;	/* number of relocation entries	*/
	unsigned short	s_nlnno;	/* number of line number entries*/
	long		s_flags;	/* flags			*/
};

/*
 * names of "special" sections
 */
#define _TEXT	".text"
#define _DATA	".data"
#define _BSS	".bss"

/* s_flags "type" */
				/* TYPE    ALLOCATED? RELOCATED? LOADED? */
				/* ----    ---------- ---------- ------- */
#define STYP_REG	0x0000	/* regular    yes        yes       yes   */
#define STYP_DSECT	0x0001	/* dummy      no         yes       no    */
#define STYP_NOLOAD	0x0002	/* noload     yes        yes       no    */
#define STYP_GROUP	0x0004	/* grouped  <formed from input sections> */
#define STYP_PAD	0x0008	/* padding    no         no        yes   */
#define STYP_COPY	0x0010	/* copy       no         no        yes   */
#define STYP_INFO	0x0200	/* comment    no         no        no    */

#define STYP_TEXT	0x0020	/* section contains text */
#define STYP_DATA	0x0040	/* section contains data */
#define STYP_BSS	0x0080	/* section contains bss  */
#define STYP_DRECTVE    0x0a00  /* section contains drectve */
#define STYP_LIT	0x8000	/* section contains literal */

#define STYP_DISCARDABLE 0x01000000 /* Section can be discarded ?? */

/*
 * Additional section types specific to the PE format. These are the types
 * actually used by the GNU toolchain along with the STYP_{TEXT|DATA|BSS}
 * types. Their definitions come from gnu/include/coff/pe.h
 */

#define IMAGE_SCN_MEM_DISCARDABLE 0x02000000	/* section can be discarded */
#define IMAGE_SCN_MEM_EXECUTE     0x20000000	/* section is executable */
#define IMAGE_SCN_MEM_READ        0x40000000	/* section is readable */
#define IMAGE_SCN_MEM_WRITE       0x80000000	/* section is writable */


#define	SCNHDR	struct scnhdr
#define	SCNHSZ	sizeof(SCNHDR)


/******************************* LINE NUMBERS *******************************/

/* 1 line number entry for every "breakpointable" source line in a section.
 * Line numbers are grouped on a per function basis; first entry in a function
 * grouping will have l_lnno = 0 and in place of physical address will be the
 * symbol table index of the function name.
 */
struct lineno{
	union {
		long l_symndx;	/* function name symbol index, iff l_lnno == 0*/
		long l_paddr;	/* (physical) address of line number	*/
	} l_addr;
	unsigned short	l_lnno;	/* line number		*/
	char padding[2];	/* force alignment	*/
};

#define	LINENO	struct lineno
#define	LINESZ	sizeof(LINENO)


/********************************** SYMBOLS **********************************/

#define SYMNMLEN	8	/* # characters in a symbol name	*/
#define FILNMLEN	14	/* # characters in a file name		*/
#define DIMNUM		4	/* # array dimensions in auxiliary entry */

struct syment {
	union {
		char	_n_name[SYMNMLEN];	/* old COFF version	*/
		struct {
			long	_n_zeroes;	/* new == 0		*/
			long	_n_offset;	/* offset into string table */
		} _n_n;
		char	*_n_nptr[2];	/* allows for overlaying	*/
	} _n;
	long		n_value;	/* value of symbol		*/
	short		n_scnum;	/* section number		*/
	unsigned short	n_type;		/* type and derived type	*/
	char		n_sclass;	/* storage class		*/
	char		n_numaux;	/* number of aux. entries	*/
};

#define	SYMENT	struct syment
#define	SYMESZ	 sizeof(SYMENT) /* don't use sizeof since some compilers */
				/* may add padding in the structure      */
#define n_name		_n._n_name
#define n_ptr		_n._n_nptr[1]
#define n_zeroes	_n._n_n._n_zeroes
#define n_offset	_n._n_n._n_offset


/*
 * Relocatable symbols have number of the section in which they are defined,
 * or one of the following:
 */
#define N_UNDEF ((short)0)  /* undefined symbol */
#define N_ABS   ((short)-1) /* value of symbol is absolute */
#define N_DEBUG ((short)-2) /* debugging symbol -- value is meaningless */
#define N_TV    ((short)-3) /* indicates symbol needs preload transfer vector */
#define P_TV    ((short)-4) /* indicates symbol needs postload transfer vector*/

/*
 * Symbol storage classes
 */
#define C_EFCN		-1	/* physical end of function	*/
#define C_NULL		0
#define C_AUTO		1	/* automatic variable		*/
#define C_EXT		2	/* external symbol		*/
#define C_STAT		3	/* static			*/
#define C_REG		4	/* register variable		*/
#define C_EXTDEF	5	/* external definition		*/
#define C_LABEL		6	/* label			*/
#define C_ULABEL	7	/* undefined label		*/
#define C_MOS		8	/* member of structure		*/
#define C_ARG		9	/* function argument		*/
#define C_STRTAG	10	/* structure tag		*/
#define C_MOU		11	/* member of union		*/
#define C_UNTAG		12	/* union tag			*/
#define C_TPDEF		13	/* type definition		*/
#define C_USTATIC	14	/* undefined static		*/
#define C_ENTAG		15	/* enumeration tag		*/
#define C_MOE		16	/* member of enumeration	*/
#define C_REGPARM	17	/* register parameter		*/
#define C_FIELD		18	/* bit field			*/
#define C_AUTOARG	19	/* auto argument		*/
#define C_BLOCK		100	/* ".bb" or ".eb"		*/
#define C_FCN		101	/* ".bf" or ".ef"		*/
#define C_EOS		102	/* end of structure		*/
#define C_FILE		103	/* file name			*/
#define C_LINE		104	/* line # reformatted as symbol table entry */
#define C_ALIAS	 	105	/* duplicate tag		*/
#define C_HIDDEN	106	/* ext symbol in dmert public lib */
#define C_SCALL		107	/* Procedure reachable via system call	*/
#define C_LEAFEXT	108	/* Global leaf procedure, "call" via BAL */
#define C_LEAFSTAT	113	/* Static leaf procedure, "call" via BAL */


/*
 * Type of a symbol, in low 5 bits of n_type
 */
#define T_NULL		0
#define T_VOID		1	/* function argument (only used by compiler) */
#define T_CHAR		2	/* character		*/
#define T_SHORT		3	/* short integer	*/
#define T_INT		4	/* integer		*/
#define T_LONG		5	/* long integer		*/
#define T_FLOAT		6	/* floating point	*/
#define T_DOUBLE	7	/* double word		*/
#define T_STRUCT	8	/* structure 		*/
#define T_UNION		9	/* union 		*/
#define T_ENUM		10	/* enumeration 		*/
#define T_MOE		11	/* member of enumeration*/
#define T_UCHAR		12	/* unsigned character	*/
#define T_USHORT	13	/* unsigned short	*/
#define T_UINT		14	/* unsigned integer	*/
#define T_ULONG		15	/* unsigned long	*/
#define T_LNGDBL	16	/* long double		*/

/*
 * derived types, in n_type
 */
#define DT_NON		0	/* no derived type	*/
#define DT_PTR		1	/* pointer		*/
#define DT_FCN		2	/* function		*/
#define DT_ARY		3	/* array		*/

#define N_BTMASK	0x1f
#define N_TMASK		0x60
#define N_BTSHFT	5
#define N_TSHIFT	2

#define BTYPE(x)	((x) & N_BTMASK)

#define ISPTR(x)	(((x) & N_TMASK) == (DT_PTR << N_BTSHFT))
#define ISFCN(x)	(((x) & N_TMASK) == (DT_FCN << N_BTSHFT))
#define ISARY(x)	(((x) & N_TMASK) == (DT_ARY << N_BTSHFT))
#define ISTAG(x)	((x)==C_STRTAG||(x)==C_UNTAG||(x)==C_ENTAG)


#define DECREF(x) ((((x)>>N_TSHIFT)&~N_BTMASK)|((x)&N_BTMASK))

union auxent {
	struct {
		long x_tagndx;	/* str, un, or enum tag indx */
		union {
			struct {
			    unsigned short x_lnno; /* declaration line number */
			    unsigned short x_size; /* str/union/array size */
			} x_lnsz;
			long x_fsize;	/* size of function */
		} x_misc;
		union {
			struct {		/* if ISFCN, tag, or .bb */
			    long x_lnnoptr;	/* ptr to fcn line # */
			    long x_endndx;	/* entry ndx past block end */
			} x_fcn;
			struct {		/* if ISARY, up to 4 dimen. */
			    unsigned short x_dimen[DIMNUM];
			} x_ary;
		} x_fcnary;
		unsigned short x_tvndx;		/* tv index */
	} x_sym;

	union {
		char x_fname[FILNMLEN];
		struct {
			long x_zeroes;
			long x_offset;
		} x_n;
	} x_file;

	struct {
		long x_scnlen;			/* section length */
		unsigned short x_nreloc;	/* # relocation entries */
		unsigned short x_nlinno;	/* # line numbers */
	} x_scn;

	struct {
		long		x_tvfill;	/* tv fill value */
		unsigned short	x_tvlen;	/* length of .tv */
		unsigned short	x_tvran[2];	/* tv range */
	} x_tv;		/* info about .tv section (in auxent of symbol .tv)) */

};

#define	AUXENT	union auxent
#define	AUXESZ	sizeof(AUXENT)

#define _ETEXT	"_etext"

/********************** RELOCATION DIRECTIVES **********************/

struct reloc {
	long r_vaddr;		/* Virtual address of reference */
	long r_symndx;		/* Index into symbol table	*/
	unsigned short r_type;	/* Relocation type		*/
};

#define IMAGE_REL_I386_DIR32	0x0006
#define IMAGE_REL_I386_REL32	0x0014

#define RELOC struct reloc
#define RELSZ sizeof(RELOC)

/* status codes */

#define S_loadLib_FILE_READ_ERROR               (M_loadPecoffLib | 1)
#define S_loadLib_REALLOC_ERROR                 (M_loadPecoffLib | 2)
#define S_loadLib_JMPADDR_ERROR                 (M_loadPecoffLib | 3)
#define S_loadLib_NO_REFLO_PAIR                 (M_loadPecoffLib | 4)
#define S_loadLib_GPREL_REFERENCE               (M_loadPecoffLib | 5)
#define S_loadLib_UNRECOGNIZED_RELOCENTRY       (M_loadPecoffLib | 6)
#define S_loadLib_REFHALF_OVFL                  (M_loadPecoffLib | 7)
#define S_loadLib_FILE_ENDIAN_ERROR             (M_loadPecoffLib | 8)
#define S_loadLib_UNEXPECTED_SYM_CLASS          (M_loadPecoffLib | 9)
#define S_loadLib_UNRECOGNIZED_SYM_CLASS        (M_loadPecoffLib | 10)
#define S_loadLib_HDR_READ                      (M_loadPecoffLib | 11)
#define S_loadLib_OPTHDR_READ                   (M_loadPecoffLib | 12)
#define S_loadLib_SCNHDR_READ                   (M_loadPecoffLib | 13)
#define S_loadLib_READ_SECTIONS                 (M_loadPecoffLib | 14)
#define S_loadLib_LOAD_SECTIONS                 (M_loadPecoffLib | 15)
#define S_loadLib_RELOC_READ                    (M_loadPecoffLib | 16)
#define S_loadLib_SYMHDR_READ                   (M_loadPecoffLib | 17)
#define S_loadLib_EXTSTR_READ                   (M_loadPecoffLib | 18)
#define S_loadLib_EXTSYM_READ                   (M_loadPecoffLib | 19)


/* function declarations */

extern STATUS    loadPecoffInit (void);
extern MODULE_ID ldPecoffModAtSym (int fd, int symFlag, char **ppText,
				   char **ppData, char **ppBss,
				   SYMTAB_ID symTbl);

#ifdef __cplusplus
}
#endif

#endif /* __INCloadPecoffLibh */
