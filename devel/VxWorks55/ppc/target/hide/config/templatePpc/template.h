/* template.h - template board header */

/* Copyright 1984-2002 Wind River Systems, Inc. */

/*
TODO -	Remove the template modification history and begin a new history
	starting with version 01a and growing the history upward with
	each revision.

modification history
--------------------
01d,18apr02,rhe  Added C++ Protection
01c,08apr99,dat  SPR 26491, PCI macros
01b,25aug97.dat  added CPU_TYPE_604R
01a,08jul97,dat  written (from template68k/template.h, ver 01d)
*/

/*
This file contains I/O addresses and related constants for the
template BSP.
*/

#ifndef	INCtemplateh
#define	INCtemplateh

#ifdef __cplusplus
extern "C" {
#endif

#define BUS		NONE		/* no off-board bus interface */
#define N_SIO_CHANNELS	2		/* Number of serial I/O channels */

/* Local I/O address map */

#define	TEMPLATE_SIO_ADRS	((volatile char *) 0xfffe0000)
#define	TEMPLATE_TIMER_ADRS	((volatile char *) 0xfffa0000)
#define BBRAM			((volatile char *) 0xfffb0000)

/* timer constants */

#define	SYS_CLK_RATE_MIN  	3	/* minimum system clock rate */
#define	SYS_CLK_RATE_MAX  	5000	/* maximum system clock rate */
#define	AUX_CLK_RATE_MIN  	3	/* minimum auxiliary clock rate */
#define	AUX_CLK_RATE_MAX  	5000	/* maximum auxiliary clock rate */

/* create a single macro INCLUDE_MMU */

#if defined(INCLUDE_MMU_BASIC) || defined(INCLUDE_MMU_FULL)
#define INCLUDE_MMU
#endif

/* Only one can be selected, FULL overrides BASIC */

#ifdef INCLUDE_MMU_FULL
#   undef INCLUDE_MMU_BASIC
#endif

/* Static interrupt vectors/levels (configurable defs go in config.h) */

#if     FALSE
#define INT_VEC_ABORT           xxx
#define INT_VEC_CLOCK           xxx
...
#define INT_LVL_ABORT           xxx
#define INT_LVL_CLOCK           xxx
...
#endif

#ifdef INCLUDE_PCI
    /* Translate PCI addresses to virtual addresses (master windows) */

#   define	PCI_MEM2LOCAL(x) \
	((void *)((UINT)(x) - PCI_MSTR_MEM_BUS + PCI_MSTR_MEM_LOCAL))

#   define PCI_IO2LOCAL(x) \
	((void *)((UINT)(x) - PCI_MSTR_IO_BUS + PCI_MSTR_IO_LOCAL))

#   define PCI_MEMIO2LOCAL(x) \
	((void *)((UINT)(x) - PCI_MSTR_MEMIO_BUS + PCI_MSTR_MEMIO_LOCAL))

    /* Translate local memory address to PCI address (slave window) */

#   define LOCAL2PCI_MEM(x) \
	((void *)((UINT)(x) - PCI_SLV_MEM_LOCAL + PCI_SLV_MEM_BUS))
#endif /* INCLUDE_PCI */

/*
 * Miscellaneous definitions go here. For example, macro definitions
 * for various devices.
 */

#if CPU==PPC603
#   define WRONG_CPU_MSG "A PPC603 VxWorks image cannot run on a PPC604!\n";
#else
#   define WRONG_CPU_MSG "A PPC604 VxWorks image cannot run on a PPC603!\n";
#endif

#define CPU_TYPE	((vxPvrGet() >> 16) & 0xffff)
#define CPU_TYPE_601	0X01    /* PPC 601 CPU */
#define CPU_TYPE_602	0x02    /* PPC 602 CPU */
#define CPU_TYPE_603	0x03    /* PPC 603 CPU */
#define CPU_TYPE_603E	0x06    /* PPC 603e CPU */
#define CPU_TYPE_603P	0x07    /* PPC 603p CPU */
#define CPU_TYPE_604	0x04    /* PPC 604 CPU */
#define CPU_TYPE_604E	0x09    /* PPC 604e CPU */
#define CPU_TYPE_604R	0x0A    /* PPC 604r CPU */

#ifdef __cplusplus
}
#endif

#endif	/* INCtemplateh */
