/* elfSparc.h - SPARC ABI header file used by ld, ar, as & loaders */

/* Copyright 2003 Wind River Systems, Inc. */

/*
modification history
--------------------
01a,30apr03,jn   Derived from Diab version of elf_sparc.h. 
                 /vobs/rome_diab_lib/dlib/include/include.unx/
		 elf_sparc.h@@/main/1
		 and /.wind_vxw_h/river/target/h/vxWorks.h@@/main/tor2/1 
		 (for elements of Wind River coding standards).
		 Moved definitions of N_* macros here.  
*/

#ifndef __INCelfSparch
#define __INCelfSparch

#ifdef __cplusplus
extern "C" {
#endif

#define EM_ARCH_MACHINE		EM_SPARC     

#ifndef EM_ARCH_MACH_ALT                     /* defaults to EM_ARCH_MACHINE */
#define EM_ARCH_MACH_ALT	EM_ARCH_MACHINE
#endif 


/**************	Exported data, types and macros	*****************/

/*
 * Relocation types
 */

#define R_SPARC_FIRST		0

#define	R_SPARC_NONE		0	/* relocation type */
#define	R_SPARC_8		1
#define	R_SPARC_16		2
#define	R_SPARC_32		3
#define	R_SPARC_DISP8		4
#define	R_SPARC_DISP16		5
#define	R_SPARC_DISP32		6
#define	R_SPARC_WDISP30		7
#define	R_SPARC_WDISP22		8
#define	R_SPARC_HI22		9
#define	R_SPARC_22		10
#define	R_SPARC_13		11
#define	R_SPARC_LO10		12
#define	R_SPARC_GOT10		13
#define	R_SPARC_GOT13		14
#define	R_SPARC_GOT22		15
#define	R_SPARC_PC10		16
#define	R_SPARC_PC22		17
#define	R_SPARC_WPLT30		18
#define	R_SPARC_COPY		19
#define	R_SPARC_GLOB_DAT	20
#define	R_SPARC_JMP_SLOT	21
#define	R_SPARC_RELATIVE	22
#define	R_SPARC_UA32		23


#if CPU==SIMSPARCSOLARIS
/*
 * Simple values for n_type.  VxWorks symbol defines.
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

#define MAX_SCNS        11
#define NO_SCNS         0
#endif /* if CPU==SIMSPARCSOLARIS */

#ifdef __cplusplus
}
#endif

#endif /* __INCelfSparch */

