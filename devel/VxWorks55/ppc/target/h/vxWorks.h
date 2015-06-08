/* vxWorks.h - VxWorks standard definitions header */

/* Copyright 1984-2001 Wind River Systems, Inc. */

/*
modification history
--------------------
03p,27nov01,mrs  Fix to conform to ANSI standard for token pasting.
03o,31oct01,tdl  Check for preexsiting __P def before defining it
03n,18oct01,dat  Chg TOOLCHAIN to TOOL_FAMILY
03m,24sep01,gls  Added definition of __P() macro (SPR #28330)
03l,28sep01,rae  added #define _WRS_VXWORKS_5_X
03k,19sep01,dat  Rework of gnu-diab changes
03j,08sep01,dat  Gnu-Diab infrastructure
03i,20aug01,dat  Gnu-Diab, added #include toolMacros.h
03h,17feb99,mrs  Add C++ support for NULL, (SPR #25079).
03g,25nov97,dat  added BUS_TYPE macros
03f,26nov96,vin  modified BSD to 44.
03e,23aug94,ism  added the MSW() and LSW() macros (SPR#3541)
03d,22sep92,rrr  added support for c++
03c,07sep92,smb  added #undef BSD to remove previous definition
03b,11jul92,smb  moved CPU definitions to types/vxCpu.h
03a,04jul92,jcf  cleaned up.
02q,28jun92,smb  reworked for ANSI lib.
02p,22jun92,rrr  added MIPS CPU_FAMILY
02o,16jun92,jwt  added (CPU_FAMILY = SPARC) for (CPU == SPARClite).
02n,26may92,rrr  the tree shuffle
02m,17mar92,yao  added _STACK_GROWS_DOWN/UP.  added macros ROUND_UP/DOWN,
		 ALIGNED, MEM_ROUND_UP/DOWN, STACK_ROUND_UP/DOWN, MEM_ALIGNED.
		 changed copyright notice.  included arch.h.  moved ffsMSB
		 definitions for 960 to changed BYTE_ORDER to _BYTE_ORDER,
		 BIG/LITTLE_ENDIAN to _BIG/LITTLE_ENDIAN.  arch960.h.  removed
		 conditional include of r3k/arch.h.
02l,09jan91,wmd  conditionalized out ffsMSB inline definition to shut up
 		 ansi warnings.
02k,11nov91,jpb  added definition for R3K CPU_FAMILY.
02j,28oct91,wmd  for lack of better place -  added Intel's 960 ffsMSB
		 inline definition here.
02i,04oct91,rrr  passed through the ansification filter
		  -fixed #else and #endif
		  -changed READ, WRITE and UPDATE to O_RDONLY O_WRONLY and ...
		  -changed copyright notice
02h,25sep91,yao  added CPU32.
02g,10sep91,wmd  added MIPS to conditional for BIG_ENDIAN byte order.
02f,20aug91,ajm  added MIPS_R3k support.
02e,14aug91,del  (intel) added I960KA/KB defines.
02d,29apr91,hdn  added defines and macros for TRON architecture
02c,31mar91,del  added #define of BYTE_ORDER for network etc.
02b,25oct90,dnw  changed to allow NULL to be redefined.
02a,25oct90,shl  combined CPU test for I960KB and I960CA into one.
...deleted pre v2a history - see RCS
*/

#ifndef __INCvxWorksh
#define __INCvxWorksh

#ifdef __cplusplus
extern "C" {
#endif

#if	!defined(NULL)
#if defined __GNUG__
#define NULL (__null)
#else
#if !defined(__cplusplus) && 0
#define NULL ((void*)0)
#else
#define NULL (0)
#endif
#endif
#endif

#if	!defined(EOF) || (EOF!=(-1))
#define EOF		(-1)
#endif

#if	!defined(FALSE) || (FALSE!=0)
#define FALSE		0
#endif

#if	!defined(TRUE) || (TRUE!=1)
#define TRUE		1
#endif


#define NONE		(-1)	/* for times when NULL won't do */
#define EOS		'\0'	/* C string terminator */


/* return status values */

#define OK		0
#define ERROR		(-1)

/* timeout defines */

#define NO_WAIT		0
#define WAIT_FOREVER	(-1)

/* low-level I/O input, output, error fd's */

#define	STD_IN	0
#define	STD_OUT	1
#define	STD_ERR	2

/* modes - must match O_RDONLY/O_WRONLY/O_RDWR in ioLib.h! */

#define VX_READ		0
#define VX_WRITE	1

/* SYSTEM types */

#define V7		1	/* ATT version 7 */
#define SYS_V		2	/* ATT System 5 */
#define BSD_4_2		3	/* Berkeley BSD 4.2 */

/* BUS types */

#define	BUS_TYPE_NONE	    NONE
#define BUS_TYPE_VME	    1
#define BUS_TYPE_MULTIBUS   2
#define BUS_TYPE_PCI	    3

#define VME_BUS		    BUS_TYPE_VME	/* for backward compat. */
#define MULTI_BUS	    BUS_TYPE_MULTIBUS

/* network configuration parameters */

#define	INET		/* include internet protocols */
#undef  BSD             /* remove any previous definition */
#define	BSD	44	/* BSD 4.4 like OS */
#define	BSDDEBUG	/* turn on debug */
#define	GATEWAY		/* tables to be initialized for gateway routing */

/* common macros */

#define MSB(x)	(((x) >> 8) & 0xff)	  /* most signif byte of 2-byte integer */
#define LSB(x)	((x) & 0xff)		  /* least signif byte of 2-byte integer*/
#define MSW(x) (((x) >> 16) & 0xffff) /* most signif word of 2-word integer */
#define LSW(x) ((x) & 0xffff) 		  /* least signif byte of 2-word integer*/

/* swap the MSW with the LSW of a 32 bit integer */
#define WORDSWAP(x) (MSW(x) | (LSW(x) << 16))

#define LLSB(x)	((x) & 0xff)		/* 32bit word byte/word swap macros */
#define LNLSB(x) (((x) >> 8) & 0xff)
#define LNMSB(x) (((x) >> 16) & 0xff)
#define LMSB(x)	 (((x) >> 24) & 0xff)
#define LONGSWAP(x) ((LLSB(x) << 24) | \
		     (LNLSB(x) << 16)| \
		     (LNMSB(x) << 8) | \
		     (LMSB(x)))

#define OFFSET(structure, member)	/* byte offset of member in structure*/\
		((int) &(((structure *) 0) -> member))

#define MEMBER_SIZE(structure, member)	/* size of a member of a structure */\
		(sizeof (((structure *) 0) -> member))

#define NELEMENTS(array)		/* number of elements in an array */ \
		(sizeof (array) / sizeof ((array) [0]))

#define FOREVER	for (;;)

#define max(x, y)	(((x) < (y)) ? (y) : (x))
#define min(x, y)	(((x) < (y)) ? (x) : (y))
#define isascii(c)	((unsigned) (c) <= 0177)
#define toascii(c)	((c) & 0177)


/* storage class specifier definitions */

#define FAST	register
#define IMPORT	extern
#define LOCAL	static

#include "types/vxCpu.h"		/* must come before vxArch.h include */
#include "types/vxArch.h"		/* must come before include of types */
#ifndef _ASMLANGUAGE
#include "types/vxParams.h"		/* must come before include of types */
#include "types/vxTypesBase.h"		/* must come between vxArch/vxTypes */
#include "types/vxTypes.h"
#include "types/vxTypesOld.h"
#endif	/* _ASMLANGUAGE */

#define ROUND_UP(x, align)	(((int) (x) + (align - 1)) & ~(align - 1))
#define ROUND_DOWN(x, align)	((int)(x) & ~(align - 1))
#define ALIGNED(x, align)	(((int)(x) & (align - 1)) == 0)

#define MEM_ROUND_UP(x)		ROUND_UP(x, _ALLOC_ALIGN_SIZE)
#define MEM_ROUND_DOWN(x)	ROUND_DOWN(x, _ALLOC_ALIGN_SIZE)
#define STACK_ROUND_UP(x)	ROUND_UP(x, _STACK_ALIGN_SIZE)
#define STACK_ROUND_DOWN(x)	ROUND_DOWN(x, _STACK_ALIGN_SIZE)
#define MEM_ALIGNED(x)		ALIGNED(x, _ALLOC_ALIGN_SIZE)


/*
 * In case TOOL_FAMILY isn't defined, we shal assume gnu for
 * backward compatibility.  All this is so we can include
 * the toolMacros.h file from the h/tool/$(TOOL_FAMILY) directory
 * path.  This is needed because for host tool building we can't
 * always specify additional include paths for compiler tool
 * builds.
 */

#ifndef TOOL_FAMILY
#   define TOOL_FAMILY gnu
#endif

/* All this to generate a string we can #include */

#define TOOL_HDR_STRINGIFY(x)  #x
#define TOOL_HDR(tc, file) TOOL_HDR_STRINGIFY(tool/tc/file)
#define TOOL_HEADER(file) TOOL_HDR(TOOL_FAMILY,file)

/* Now include the correct header file */

#include TOOL_HEADER(toolMacros.h)

#define _WRS_VXWORKS_5_X

/*
 * Before we insist on defining __P here make sure there
 * aren't any previous occurrences! __P may clash with
 * the compiler's definition of it.
 */

#ifndef __P
#if defined(__STDC__) || defined(__cplusplus)
#define	__P(protos)	protos		/* Use ANSI C proto */
#else
#define	__P(protos)     ()
#endif
#endif

#ifdef __cplusplus
}
#endif

#endif /* __INCvxWorksh */
