/* som_coff.h - HP-PA a.out object module header */

/* Copyright 1993 Wind River Systems, Inc. */

/*
modification history
-------------------
01a,10sep93,yao  written.
*/

#ifndef INCsom_coffh
#define INCsom_coffh

/*
 * VxWorks values for n_type.
 */

#define N_UNDF  0x0             /* undefined */
#define N_ABS   0x2             /* absolute */
#define N_TEXT  0x4             /* text */
#define N_DATA  0x6             /* data */
#define N_BSS   0x8             /* bss */
#define N_COMM  0x12            /* common (internal to ld) */
#define N_FN    0x1f            /* file name symbol */

#define N_EXT   01              /* external bit, or'ed in */
#define N_TYPE  0x1e            /* mask for all the type bits */
#define N_STAB  0xe0            /* if any of these bits set, a SDB entry */


struct sys_clock {
    unsigned int secs;
    unsigned int nanosecs;
    };

union name_pt {
   char		*n_name;
   unsigned int	 n_strx;
   };

typedef struct unwind_table_entry {
   unsigned int startAddr;		/* word 1 */
   unsigned int endAddr;		/* word 2 */
   unsigned int cantUnwind:1;           /* word 3 */
   unsigned int millicode:1;
   unsigned int millicode_save_sr0:1;
   unsigned int regionDescriptor:2;
   unsigned int bunk:22;
   unsigned int saveSP:1;
   unsigned int saveRP:1;
   unsigned int saveMRP:1;
   unsigned int reserved3:1;
   unsigned int cleanupDefined:1;
   unsigned int interruptMarker:2;	/* word 4 */
   unsigned int largeFrame:1;
   unsigned int reserved4:2;
   unsigned int frameSize:27;
   } UNWIND_ENTRY;

#define UNWIND_DISCONTINUOUS    3
#define UNWIND_NORMAL           0

#define symbol_name name.n_name
#define subspace_name name.n_name
#define space_name name.n_name

typedef struct header {
    short int system_id;		/* system id */
    short int a_magic;			/* magic number */
    unsigned int version_id;		/* format version */
    struct sys_clock file_time;		/* timestamp */
    unsigned int entry_space;		/* reserved */
    unsigned int entry_subspace;	/* reserved */
    unsigned int entry_offset;		/* reserved */
    unsigned int aux_header_location;	/* file ptr to aux hdrs */
    unsigned int aux_header_size;	/* size of aux hdrs */
    unsigned int som_length;		/* length of object module */
    unsigned int presumed_dp;		/* reserved */
    unsigned int space_location;	/* file ptr to space dict */
    unsigned int space_total;		/* number of spaces */
    unsigned int subspace_location;	/* file ptr to subspace dict */
    unsigned int subspace_total;	/* number of subspaces */
    unsigned int loader_fixup_location;	/* reserved */
    unsigned int loader_fixup_total;	/* reserved */
    unsigned int space_strings_location;	/* file ptr to space strings */
    unsigned int space_strings_size;	/* size of space strings */
    unsigned int init_array_location;	/* reserved */
    unsigned int init_array_total;	/* reserved */
    unsigned int compiler_location;	/* file ptr to compiler records */
    unsigned int compiler_total;	/* number of compiler records */
    unsigned int symbol_location;	/* file ptr to symbol table */
    unsigned int symbol_total;		/* number of symbols */
    unsigned int fixup_request_location;/* file ptr to fixups */
    unsigned int fixup_request_total;	/* number of fixups */
    unsigned int symbol_strings_location;/* file ptr to symbol strings */
    unsigned int symbol_strings_size;	/* size of to symbol strings */
    unsigned int unloadable_sp_location;/* file ptr to debug info */
    unsigned int unloadable_sp_size;	/* size of to debug info */
    unsigned int checksum;		/* header checksum */
    } SOM_HDR;

struct aux_id {
    unsigned int mandatory : 1;		/* reserved */
    unsigned int copy : 1;		/* reserved */
    unsigned int append : 1;		/* reserved */
    unsigned int ignore : 1;		/* reserved */
    unsigned int reserved : 12;		/* reserved */
    unsigned int type : 16;		/* reserved */
    unsigned int length;      		/* reserved */
    };

/* values for magic number */

#define RELOC_MAGIC     0x106	/* relocatable only */
#define EXEC_MAGIC      0x107	/* normal executable */
#define SHARE_MAGIC     0x108	/* shared executable */

/* values for new and old versions */

#define VERSION_ID      85082112
#define NEW_VERSION_ID  87102412        /* relocatable with new fixups */

/* values for the aux_id.type field */

#define HPUX_AUX_ID	4
#define VERSION_AUX_ID	6
#define COPYRIGHT_AUX_ID 10

typedef struct som_exec_auxhdr {	/* HP-UX auxiliary header */
    struct aux_id som_auxhdr;	/* aux header id */
    long exec_tsize;		/* text size */
    long exec_tmem;		/* start address of text */
    long exec_tfile;		/* file ptr to text */
    long exec_dsize;		/* data size */
    long exec_dmem;		/* start address of data size */
    long exec_dfile;		/* file ptr to data */
    long exec_bsize;		/* bss size */
    long exec_entry;		/* address of entry point */
    long exec_flags;		/* loader flags */
    long exec_bfill;		/* bss initialization value */
    } AUX_HDR;

/* values for exec_flags */

#define TRAP_NIL_PTRS	01

struct user_string_aux_hdr {	/* version string auxiliary header */
    struct aux_id header_id;	/* aux header id */
    unsigned int string_length;	/* strlen(user_string) */
    char user_string[1];	/* user-defined string */
    };

struct copyright_aux_hdr {	/* copyright string auxiliary header */
    struct aux_id header_id;	/* aux header id */
    unsigned int string_length;	/* strlen(user_string) */
    char user_string[1];	/* user-defined string */
    };

/* space dictionary from scnhdr.h */

typedef struct space_dictionary_record {
    union name_pt name;		/* index to space name */
    unsigned int is_loadable: 1;/* space is loadable */
    unsigned int is_defined: 1; /* space is defined within file */
    unsigned int is_private: 1; /* space is not sharable */
    unsigned int reserved: 13; 	/* space is not sharable */
    unsigned int sort_key: 8; 	/* sort key for space */
    unsigned int reserved2: 8; 	/* reserved */
    int          space_number;	/* space index */
    int          subspace_index;/* subspace index */
    unsigned int subspace_quantity;/* number of subspaces in space */
    int          loader_fix_index;/* reserved */
    int          loader_fix_quantity;/* reserved */
    int          int_pointer_index;/* reserved */
    int          int_pointer_quantity;/* reserved */
    } SPACE;

typedef struct subspace_dictionary_record {
    int             space_index;
    unsigned int    access_control_bits: 7; /* access for PDIR entries */
    unsigned int    memory_resident    : 1; /* lock in memory */        
    unsigned int    dup_common         : 1; /* data name clashes allowed  */    
    unsigned int    is_common          : 1; /* subspace is a common block */   
    unsigned int    is_loadable        : 1;
    unsigned int    quadrant           : 2; /* quadrant request */        
    unsigned int    initially_frozen   : 1; /* must be locked into memory
					       when OS is booted */
    unsigned int    is_first           : 1; /* must be first subspace */
    unsigned int    code_only          : 1; /* must contain only code */
    unsigned int    sort_key           : 8; /* subspace sort key */
    unsigned int    replicate_init     : 1; /* init values replicated to 
					       fill subspace_length */
    unsigned int    continuation       : 1; /* subspace is a continuation */
    unsigned int    reserved           : 6;
    int             file_loc_init_value;    /* file location or 
					       initialization value */
    unsigned int    initialization_length;
    unsigned int    subspace_start;         /* starting offset */
    unsigned int    subspace_length;        /* number of bytes defined by 
					     * this subspace */        
    unsigned int    reserved2          :16;   
    unsigned int    alignment          :16; /* alignment required for the
					     * subspace (largest alignment 
					     * requested for any item in the 
					     * subspace) */
    union name_pt   name;                   /* index of subspace name */
    int             fixup_request_index;    /* index into fixup array */
    unsigned int    fixup_request_quantity; /* number of fixup requests */
    } SUBSPACE;
    
#define SBPSZ  sizeof(SUBSPACE)

/*
 * Define constants for names of spaces
 */

#define _SPACE_TEXT	"$TEXT$"
#define _SPACE_PRIVATE	"$PRIVATE$"

/* from syms.h */
typedef struct symbol_dictionary_record {
    unsigned int  hidden           : 1;		/* reserved */
    unsigned int  secondary_def    : 1;		/* symbol type */
    unsigned int  symbol_type      : 6;		/* symbol type */
    unsigned int  symbol_scope     : 4;		/* symbol value */
    unsigned int  check_level      : 3;		/* type checking level */
    unsigned int  must_qualify     : 1;		/* qualifier required */
    unsigned int  initially_frozen : 1;		/* reserved */
    unsigned int  memory_resident  : 1;		/* reserved */
    unsigned int  is_common        : 1;		/* common block */
    unsigned int  dup_common       : 1;		/* COBOL-style common */
    unsigned int  xleast           : 2;		/* reserved */
    unsigned int  arg_reloc        :10;		/* parameter relocation bits */
    union name_pt name;				/* index to symbol name */
    union name_pt qualifier_name; 		/* index to qual name */
    unsigned int  symbol_info;			/* subspace index */
    unsigned int  symbol_value;			/* symbol value */
    } SYMREC;

#define n_nptr          name.n_name
#define n_offset        name.n_strx
#define q_nptr          qualifier_name.n_name
#define q_offset        qualifier_name.n_strx

/* values for symbol_type */

#define    ST_NULL         0	/* unused symbol entry */
#define    ST_ABSOLUTE     1	/* non-relocatable symbol */
#define    ST_DATA         2	/* data symbol */
#define    ST_CODE         3	/* generic code symbol */
#define    ST_PRI_PROG     4	/* program entry point */
#define    ST_SEC_PROG     5	/* secondary prog entry point */
#define    ST_ENTRY        6	/* procedure entry point */
#define    ST_STORAGE      7	/* storage request */
#define    ST_STUB         8	/* reserved */
#define    ST_MODULE       9	/* Pascal module name */
#define    ST_SYM_EXT     10	/* symbol extension record */
#define    ST_ARG_EXT     11	/* argument extension record */
#define    ST_MILLICODE   12	/* millicode entry point */
#define    ST_PLABEL      13	/* reserved */
#define    ST_OCT_DIS     14	/* reserved */
#define    ST_MILLI_EXT   15	/* reserved */

/* symbol scopes */

#define    SS_UNSAT        0	/* unsatisfied reference */
#define    SS_EXTERNAL     1	/* reserved */
#define    SS_LOCAL        2	/* local symbol */
#define    SS_UNIVERSAL    3	/* global symbol */

/* symbol extension records (for type checking) */

union arg_descriptor {  
    struct 
	{ 
	unsigned int reserved  :3;	/* not used */
	unsigned int packing   :1;	/* reserved */
	unsigned int alignment :4;	/* byte alignment */
	unsigned int mode      :4;	/* not used */
	unsigned int structure :4;	/* use of symbol */
	unsigned int hash      :1;	/* structure of symbol */
	int arg_type           :15;	/* set if arg_type is hashed */
	} arg_desc;
    unsigned int    word;
    };

struct symbol_extension_record 
    {
    unsigned int          type         :8; /* always type SYM_EXT (12) for 
					    * this record */
    unsigned int          max_num_args :8; /* max # of parameters */
    unsigned int          min_num_args :8; /* min # of parameters */
    unsigned int          num_args     :8; /* actual # of parameters  */
    union arg_descriptor  symbol_desc;	   /* first 3 parameters */
    union arg_descriptor  argument_desc[3];
    };

struct argument_desc_array 
    {
    unsigned int            type    : 8; /* always type ARG_EXT (13) for 
					  * this record */
    unsigned int            reserved: 24; /* not used */
    union arg_descriptor    argument_desc[4]; /* nexet 4 parameters */
    };

#define SYMESZ  sizeof(SYMREC)
#define AUXENT  struct symbol_extension_record
#define AUXESZ  sizeof(AUXENT)

/* relocation information */

/*
 * The following declarations are for relocation entries in new-format
 * relocatable object files, produced by compilers on HP-UX Release 3.0
 * and later.  A relocatable object file with this format will have
 * a version number in the file header of NEW_VERSION_ID (see filehdr.h).
 *
 * Relocation entries are a stream of bytes; each subspace (see scnhdr.h)
 * contains a byte offset from the beginning of the fixup table, and a
 * length in bytes of the relocation entries for that subspace.
 * The first byte of each relocation entry is the opcode as described
 * below.  Entries can be from 1 to 12 (currently) bytes long, and
 * describe 0, 1, or more words of data to be relocated.
 */

#define R_NO_RELOCATION   0x00	/* 00-1f:  n words, not relocatable */
#define R_ZEROES          0x20	/* 20-21:  n words, all zero */
#define R_UNINIT          0x22	/* 22-23:  n words, uninitialized */
#define R_RELOCATION      0x24	/* 24:     1 word, relocatable data */
#define R_DATA_ONE_SYMBOL 0x25	/* 25-26:  1 word, data external reference */
#define R_DATA_PLABEL     0x27	/* 27-28:  1 word, data plabel reference */
#define R_SPACE_REF       0x29	/* 29:     1 word, initialized space id */
#define R_REPEATED_INIT   0x2a  /* 2a-2d:  n words, repeated pattern */
				/* 2e-2f:  reserved */
#define R_PCREL_CALL      0x30	/* 30-3d:  1 word, pc-relative call */
				/* 3e-3f:  reserved */
#define R_ABS_CALL        0x40	/* 40-4d:  1 word, absolute call */
				/* 4e-4f:  reserved */
#define R_DP_RELATIVE     0x50	/* 50-72:  1 word, dp-relative load/store */
				/* 73-77:  reserved */
#define R_DLT_REL         0x78	/* 78-79:  1 word, dlt-relative load/store */
#define R_CODE_ONE_SYMBOL 0x80  /* 80-a2:  1 word, relocatable code */
				/* a3-ad:  reserved */
#define R_MILLI_REL       0xae  /* ae-af:  1 word, millicode-relative branch */
#define R_CODE_PLABEL     0xb0  /* b0-b1:  1 word, code plabel reference */
#define R_BREAKPOINT      0xb2	/* b2:     1 word, statement breakpoint */
#define R_ENTRY           0xb3	/* b3-b4:  procedure entry */
#define R_ALT_ENTRY       0xb5	/* b5:     alternate exit */
#define R_EXIT            0xb6	/* b6:     procedure exit */
#define R_BEGIN_TRY       0xb7	/* b7:     start of try block */
#define R_END_TRY         0xb8	/* b8-ba:  end of try block */
#define R_BEGIN_BRTAB     0xbb	/* bb:     start of branch table */
#define R_END_BRTAB       0xbc	/* bc:     end of branch table */
#define R_STATEMENT       0xbd	/* bd-bf:  statement number */
#define R_DATA_EXPR       0xc0	/* c0:     1 word, relocatable data expr */
#define R_CODE_EXPR       0xc1	/* c1:     1 word, relocatable code expr */
#define R_FSEL            0xc2	/* c2:     F' override */
#define R_LSEL            0xc3	/* c3:     L'/LD'/LS'/LR' override */
#define R_RSEL            0xc4	/* c4:     R'/RD'/RS'/RR' override */
#define R_N_MODE          0xc5	/* c5:     set L'/R' mode */
#define R_S_MODE          0xc6	/* c6:     set LS'/RS' mode */
#define R_D_MODE          0xc7	/* c7:     set LD'/RD' mode */
#define R_R_MODE          0xc8	/* c8:     set LR'/RR' mode */
#define R_DATA_OVERRIDE   0xc9  /* c9-cd:  get data from fixup area */
#define R_TRANSLATED      0xce  /* ce:     toggle translated mode */
#define R_AUX_UNWIND      0xcf  /* cf:     auxiliary unwind (proc begin) */
#define R_COMP1           0xd0	/* d0:     arbitrary expression */

#define     R_PUSH_PCON1      0x00      /* 00-3f:  positive constant */
#define     R_PUSH_DOT        0x40
#define     R_MAX             0x41
#define     R_MIN             0x42
#define     R_ADD             0x43
#define     R_SUB             0x44
#define     R_MULT            0x45
#define     R_DIV             0x46
#define     R_MOD             0x47
#define     R_AND             0x48
#define     R_OR              0x49
#define     R_XOR             0x4a
#define     R_NOT             0x4b
                                        /* 4c-5f:  reserved */
#define     R_LSHIFT          0x60      /* 60-7f:  shift count (0-variable) */
#define     R_ARITH_RSHIFT    0x80      /* 80-9f:  shift count (0-variable) */
#define     R_LOGIC_RSHIFT    0xa0      /* a0-bf:  shift count (0-variable) */
#define     R_PUSH_NCON1      0xc0      /* c0-ff:  negative constant */

#define R_COMP2           0xd1	/* d1:     arbitrary expression */
#define     R_PUSH_PCON2      0x00      /* 00-7f:  positive constant */
#define     R_PUSH_SYM        0x80
                                        /* 81:     reserved */
#define     R_PUSH_PLABEL     0x82      /* 82-83:  plabel */
                                        /* 84-bf:  reserved */
#define     R_PUSH_NCON2      0xc0      /* c0-ff:  negative constant */

#define R_COMP3           0xd2	/* d2:     arbitrary expression */
#define     R_PUSH_PROC       0x00      /* 00-01:  procedure */
#define     R_PUSH_CONST      0x02      /* 02:     constant */
                                        /* 03-ff:  reserved */

#define R_PREV_FIXUP      0xd3	/* d3-d6:  apply previous fixup again */
#define R_SEC_STMT        0xd7	/* d7:     secondary statement number */
				/* d8-df:  reserved */
#define R_RESERVED        0xe0	/* e0-ff:  reserved for compiler/linker */


/*
 * Loader fixups
 *
 * These are produced by the linker only when the -HK option is used.
 * They indicate initialized pointers in the data space that contain
 * the addresses of other locations in the data space, so that those
 * pointers can be relocated at load time if the data is loaded at
 * some address other than that specified at link time.  (References
 * from code to data are always relative to the dp register (r27),
 * and will not need to be relocated.)  These are needed primarily
 * for the MPE XL operating system, but may be useful for some
 * dynamic loading applications on HP-UX.
 */

struct loader_fixup {
        unsigned int fixup_type;
        unsigned int space_index;   /* index of space to fix up         */
        unsigned int space_offset;  /* offset at which to patch         */
        int 	     constant;	    /* constant used to patch the space */
};

/* loader fixup types */

#define LD_SPACE_REF 0  /* space reference fixup */
#define LD_DATA_REF  1  /* data reference fixup  */
#define LD_XRT_REF   2  /* XRT reference fixup  */

#define LDRFIX struct loader_fixup
#define LDRFIXSZ sizeof(LDRFIX)


/*
 * The following definitions are for old-format relocatable object
 * files, and for absolute fixups (e_abs) in all executable files.
 * Old relocatable object files were produced by compilers before
 * HP-UX Release 3.0 on the Series 800, and the file header contains
 * the version number VERSION_ID (see filehdr.h).
 *
 * Absolute fixups are produced by the linker only when the -HF
 * option is specified.  They record the locations of all absolute
 * branches (LDIL/BLE) in the code, so that code can be relocated
 * without being completely relinked.  They are primarily useful
 * only on MPE XL, but may be useful in dynamic loading applications
 * on HP-UX.
 */

typedef struct fixup_request_record {
        unsigned int need_data_ref  : 1;
        unsigned int arg_reloc      : 10;
        unsigned int expression_type: 5;  /* type of expr. providing value    */
	unsigned int exec_level     : 2;  /* execution level at point of call */
        unsigned int fixup_format   : 6;  /* inst. or data format             */
        unsigned int fixup_field    : 8;  /* field from fixup value           */
        unsigned int subspace_offset;     /* offset of word to be fixed up    */
        unsigned int symbol_index_one;    /* sequence # of symbol             */
        unsigned int symbol_index_two;    /* sequence # of second symbol      */
        int          fixup_constant;      /* constant part           	      */
} FIXUP;

/* field selectors */

#define e_fsel  0    /* F'  : no change           	     */
#define e_lssel 1    /* LS' : if (bit 21) then add 0x800
                              arithmetic shift right 11 bits */
#define e_rssel 2    /* RS' : Sign extend from bit 21        */
#define e_lsel  3    /* L'  : Arithmetic shift right 11 bits */
#define e_rsel  4    /* R'  : Set bits 0-20 to zero          */
#define e_ldsel 5    /* LD' : Add 0x800, arithmetic shift
                              right 11 bits          	     */
#define e_rdsel 6    /* RD' : Set bits 0-20 to one           */
#define e_lrsel 7    /* LR' : L' with "rounded" constant     */
#define e_rrsel 8    /* RR' : R' with "rounded" constant     */

/* expression types */

#define e_one         0      /* label + constant                   */
#define e_two         1      /* label - label + constant           */
#define e_pcrel       2      /* label - subspace offset + constant */
#define e_con         3      /* constant                           */
#define e_plabel      7      /* plabel for label                   */
#define e_abs	     18      /* absolute, 1st sym index is address */

/* fixup formats */

#define i_exp14 0      /* 14 bit signed long displacement     */
#define i_exp21 1      /* 21 bit signed immediate             */
#define i_exp11 2      /* signed 11 bit immediate             */
#define i_rel17 3      /* 19 bit signed displacement, discard */
                       /* 2 low order bits to make 17 bits    */
#define i_rel12 4      /* 14 bit signed displacement, discard */
                       /* 2 low order bits to make 12 bits    */
#define i_data  5      /* whole word                          */
#define i_none  6      /* no expression in this word          */
#define i_abs17 7      /* same as i_rel17                     */
#define i_milli 8      /* i_abs17 plus SR bits                */
#define i_break 9      /* conditionally replace NOP w/ BREAK  */

#define RELSZ sizeof(FIXUP)

#endif 	/* INCsom_coffh */
