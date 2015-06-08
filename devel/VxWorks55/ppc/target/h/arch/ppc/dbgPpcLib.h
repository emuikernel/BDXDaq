/* dbgPpcLib.h - header file for arch dependent portion of debugger */

/* Copyright 1984-1995 Wind River Systems, Inc. */

/*
modification history
--------------------
01m,14mar02,pch  SPR 74270:  make 440 bh types consistent with 5xx/604/860
01l,23aug01,pch  Separate PPC440 from PPC405, cleanup.
01k,16aug01,pch  Add PPC440 support
01j,30oct00,s_m  separated PPC405 support from PPC403
01i,27oct00,s_m  updated for CPU == PPC405F
01h,25oct00,s_m  renamed PPC405 cpu types
01g,13jun00,alp  Added PPC405 Support
01f,19apr99,zl   added support for PPC509 and PPC555
01e,14oct98,elg  added hardware breakpoints for PPC603 and PPC604
01d,27jul98,elg  added hardware breakpoints
01c,24dec97,dbt  modified for new breakpoint scheme.
01b,24oct95,kdl	 added DBG_TRACE=FALSE for PPC403; added mod hist.
01a,14feb95,yao	 created.
*/

#ifndef __INCdbgPpcLibh
#define __INCdbgPpcLibh

#ifdef __cplusplus
extern "C" {
#endif

#include "esf.h"
#include "reg.h"
#ifndef _ASMLANGUAGE
#include "dsmLib.h"
#endif	/* _ASMLANGUAGE */
#include "iv.h"

#define	BREAK_ESF	ESFPPC		/* XXX */

#define DBG_BREAK_INST	0x7fe00008	/* trap 31, 0, 0 */

#ifndef	_PPC_MSR_SE
/*
 * Tell generic code it must simulate single-step using breakpoints.
 * This setting causes wdbDbgArchLib.c to export wdbDbgGetNpc(),
 * and to call wdbDbgTrap() instead of wdbDbgBreakpoint().
 *
 * XXX - PPC4xx have no SE bit in MSR, but do provide an "instruction
 * XXX - completion" debug event which has a similar effect and could
 * XXX - perhaps be used instead of doing this.
 */
#define DBG_NO_SINGLE_STEP	1	/* no trace support */
#endif 	/* _PPC_MSR_SE */

/* definition for HID1 register for 601 */
#define HID1_RUN_MODE(x)	(((x)&0x70000000)>>28)
#define HID1_RUN_NORM
#define HID1_RUN_UNDEF
#define HID1_RUN_UNDEF

/*
 * Set DBG_HARDWARE_BP for CPUs which support hardware breakpoints, and
 * define the breakpoint types.
 *
 * If defined, BRK_INST will be handled by generic code in wdb/wdbBpLib.c
 * and ostool/dbgTaskLib.c.  Other breakpoint types are CPU-specific, and
 * are used primarily in wdbDbgArchLib.c.  The help text and type-display
 * code in dbgArchLib.c should also match the definitions here.
 *
 * BRK_HARDMASK is used to extract the type from bp_flags.  It must be
 * at least large enough to cover all defined types, and must not overlap
 * with BRK_HARDWARE, nor with the flag bits (BP_INSTALLED, BP_HOST, BP_SO,
 * BP_STEP, BP_EVENT) defined in wdb/wdbDbgLib.h.
 */
#if 	(CPU == PPC509)   || (CPU == PPC555) || (CPU == PPC603) || \
        (CPU == PPCEC603) || (CPU == PPC604) || (CPU == PPC860)

#define DBG_HARDWARE_BP	1

#define BRK_INST	0x0	/* hardware instruction breakpoint */
#define BRK_RW		0x1	/* hardware data breakpoint for read or write */
#define BRK_READ	0x2	/* hardware data breakpoint for read */
#define BRK_WRITE	0x3	/* hardware data breakpoint for write */

#define DEFAULT_HW_BP	BRK_RW	/* default hardware breakpoint */

#define BRK_HARDWARE	0x10	/* hardware breakpoint bit */
#define BRK_HARDMASK	0x0f	/* hardware breakpoint mask */

#elif	(CPU == PPC403)

#define DBG_HARDWARE_BP	1

#define BRK_INST	0x0	/* hardware instruction breakpoint */
#define BRK_DATAW1	0x1	/* data breakpoint for write one byte */
#define BRK_DATAR1	0x2	/* data breakpoint for read one byte */
#define BRK_DATARW1	0x3	/* data breakpoint for read/write one byte */
#define BRK_DATAW2	0x5	/* data breakpoint for write 2 bytes */
#define BRK_DATAR2	0x6	/* data breakpoint for read 2 bytes */
#define BRK_DATARW2	0x7	/* data breakpoint for read/write 2 bytes */
#define BRK_DATAW4	0x9	/* data breakpoint for write 4 bytes */
#define BRK_DATAR4	0xA	/* data breakpoint for read 4 bytes */
#define BRK_DATARW4	0xB	/* data breakpoint for read/write 4 bytes */
#define BRK_DATAW16	0xD	/* data breakpoint for write 16 bytes */
#define BRK_DATAR16	0xE	/* data breakpoint for read 16 bytes */
#define BRK_DATARW16	0xF	/* data breakpoint for read/write 16 bytes */

#define DEFAULT_HW_BP	BRK_DATAW1	/* default hardware breakpoint */

#define BRK_HARDWARE	0x10	/* hardware breakpoint bit */
#define BRK_HARDMASK	0x0f	/* hardware breakpoint mask */

#elif	((CPU == PPC405) || (CPU == PPC405F))

#define DBG_HARDWARE_BP	1

#define BRK_INST	0x0	/* hardware instruction breakpoint */
#define BRK_DATAW1	0x1	/* data breakpoint for write one byte */
#define BRK_DATAR1	0x2	/* data breakpoint for read one byte */
#define BRK_DATARW1	0x3	/* data breakpoint for read/write one byte */
#define BRK_DATAW2	0x4	/* data breakpoint for write 2 bytes */
#define BRK_DATAR2	0x5	/* data breakpoint for read 2 bytes */
#define BRK_DATARW2	0x6	/* data breakpoint for read/write 2 bytes */
#define BRK_DATAW4	0x7	/* data breakpoint for write 4 bytes */
#define BRK_DATAR4	0x8	/* data breakpoint for read 4 bytes */
#define BRK_DATARW4	0x9	/* data breakpoint for read/write 4 bytes */
#define BRK_DATAW32	0xA	/* data breakpoint for write cache line size */
#define BRK_DATAR32	0xB	/* data breakpoint for read 16 bytes */
#define BRK_DATARW32	0xC	/* data breakpoint for read/write 16 bytes */

#define DEFAULT_HW_BP	BRK_DATAW1	/* default hardware breakpoint */

#define BRK_HARDWARE	0x10	/* hardware breakpoint bit */
#define BRK_HARDMASK	0x0f	/* hardware breakpoint mask */

#elif	(CPU == PPC440)

#define DBG_HARDWARE_BP	1

#define BRK_INST	0x0	/* hardware instruction breakpoint */
#define BRK_DATARW	0x1	/* data breakpoint for read/write */
#define BRK_DATAR	0x2	/* data breakpoint for read */
#define BRK_DATAW	0x3	/* data breakpoint for write */

/* XXX - 440 has 4 IAC registers (each pair usable as a range), and
 * XXX - 2 DAC registers (usable as a range); also DVC, BRT, ICMP,
 * XXX - IRPT, TRAP, and RET events.  How much of this to support?
 */

#define DEFAULT_HW_BP	BRK_DATAW	/* default hardware breakpoint */

#define BRK_HARDWARE	0x10	/* hardware breakpoint bit */
#define BRK_HARDMASK	0x0f	/* hardware breakpoint mask */

#endif	/* 509 555 603 EC603 604 860 : 403 : 405 405F : 440 */
/* End of hardware breakpoint type definitions */


#if	DBG_HARDWARE_BP

#ifndef _ASMLANGUAGE

/*
 * The DBG_REGS structure specifies the values to be
 * loaded into the hardware breakpoint registers.
 */
typedef struct
    {
#if	((CPU == PPC509) || (CPU == PPC555) || (CPU == PPC860))
    UINT32	cmpa;		/* comparator A value register */
    UINT32	cmpb;		/* comparator B value register */
    UINT32 	cmpc;		/* comparator C value register */
    UINT32	cmpd;		/* comparator D value register */
    UINT32	counta;		/* breakpoint counter value register A */
    UINT32	countb;		/* breakpoint counter value register B */
    UINT32	cmpe;		/* comparator E value register */
    UINT32	cmpf;		/* comparator F value register */
    UINT32	cmpg;		/* comparator G value register */
    UINT32	cmph;		/* comparator H value register */
    UINT32	lctrl1;		/* load/store support comparators control reg */
    UINT32	lctrl2;		/* load/store support AND-OR control register */
    UINT32	ictrl;		/* instruction support control register */
    UINT32	bar;		/* breakpoint address register */

#elif	(CPU == PPC603) || (CPU == PPCEC603) || (CPU == PPC604)
    UINT32	iabr;		/* instruction address breakpoint register */
# if	(CPU == PPC604)
    UINT32	dabr;		/* data address breakpoint register */
    UINT32	dar;		/* data address register */
# endif	/* (CPU == PPC604) */

#elif	(CPU == PPC403)
    UINT32	dbcr;		/* debug control register */
    UINT32	dbsr;		/* debug status register */
    UINT32	dac1;		/* data address compare register 1 */
    UINT32	dac2;		/* data address compare register 2 */
    UINT32	iac1;		/* instruction address compare register 1 */
    UINT32	iac2;		/* instruction address compare register 2 */
    UINT32	msr;		/* machine state register */
#elif   ((CPU == PPC405) || (CPU == PPC405F) || (CPU == PPC440))
    UINT32      dbcr0;          /* debug control register 0 */
    UINT32      dbcr1;          /* debug control register 1 */
# if	(CPU == PPC440)
    UINT32      dbcr2;          /* debug control register 2 */
    UINT32      dbdr;           /* debug data    register */
# endif	/* (CPU == PPC440) */
    UINT32      dbsr;           /* debug status  register */
    UINT32      dac1;           /* data address compare register 1 */
    UINT32      dac2;           /* data address compare register 2 */
    UINT32      iac1;           /* instruction address compare register 1 */
    UINT32      iac2;           /* instruction address compare register 2 */
    UINT32      iac3;           /* instruction address compare register 3 */
    UINT32      iac4;           /* instruction address compare register 4 */
    UINT32      msr;            /* machine state register */
#endif	/* 509 555 860 : 603 EC603 604 : 403 : 405 405F 440 */

    } DBG_REGS;

/* Prototypes for hardware breakpoint register access functions */

#if 	defined(__STDC__) || defined(__cplusplus)

# if	((CPU == PPC509) || (CPU == PPC555) || (CPU == PPC860))
IMPORT	void	dbgCmpaSet (int value);
IMPORT	void	dbgCmpbSet (int value);
IMPORT	void	dbgCmpcSet (int value);
IMPORT	void	dbgCmpdSet (int value);
IMPORT	void	dbgCmpeSet (int value);
IMPORT	void	dbgCmpfSet (int value);
IMPORT	void	dbgLctrl1Set (int value);
IMPORT	void	dbgLctrl2Set (int value);
IMPORT	void	dbgIctrlSet (int value);
IMPORT	int	dbgCmpaGet (void) ;
IMPORT	int	dbgCmpbGet (void) ;
IMPORT	int	dbgCmpcGet (void) ;
IMPORT	int	dbgCmpdGet (void) ;
IMPORT	int	dbgCmpeGet (void) ;
IMPORT	int	dbgCmpfGet (void) ;
IMPORT	int	dbgLctrl1Get (void) ;
IMPORT	int	dbgLctrl2Get (void) ;
IMPORT	int	dbgIctrlGet (void) ;

# elif	(CPU == PPC603) || (CPU == PPCEC603) || (CPU == PPC604)
IMPORT	void	wdbDbgIabrSet (int value);
IMPORT	int	wdbDbgIabrGet (void);
#  if	(CPU == PPC604)
IMPORT	void	wdbDbgDabrSet (int value);
IMPORT	int	wdbDbgDabrGet (void);
IMPORT	int	wdbDbgDarGet (void);
#  endif  /* (CPU == PPC604) */

# elif	(CPU == PPC403)
IMPORT	void	wdbDbgDbcrSet (int value);
IMPORT	int	wdbDbgDbcrGet (void);
IMPORT	void	wdbDbgDbsrSet (int value);
IMPORT	int	wdbDbgDbsrGet (void);
IMPORT	void	wdbDbgDac1Set (int value);
IMPORT	int	wdbDbgDac1Get (void);
IMPORT	void	wdbDbgDac2Set (int value);
IMPORT	int	wdbDbgDac2Get (void);
IMPORT	void	wdbDbgIac1Set (int value);
IMPORT	int	wdbDbgIac1Get (void);
IMPORT	void	wdbDbgIac2Set (int value);
IMPORT	int	wdbDbgIac2Get (void);

# elif   ((CPU == PPC405) || (CPU == PPC405F) || (CPU == PPC440))
IMPORT  void    wdbDbgDbcr0Set (int value);
IMPORT  int     wdbDbgDbcr0Get (void);
IMPORT  void    wdbDbgDbcr1Set (int value);
IMPORT  int     wdbDbgDbcr1Get (void);
#  if	(CPU == PPC440)
IMPORT  void    wdbDbgDbcr2Set (int value);
IMPORT  int     wdbDbgDbcr2Get (void);
IMPORT  void    wdbDbgDbdrSet (int value);
IMPORT  int     wdbDbgDbdrGet (void);
#  endif  /* (CPU == PPC440) */
IMPORT  void    wdbDbgDbsrSet (int value);
IMPORT  int     wdbDbgDbsrGet (void);
IMPORT  void    wdbDbgDac1Set (int value);
IMPORT  int     wdbDbgDac1Get (void);
IMPORT  void    wdbDbgDac2Set (int value);
IMPORT  int     wdbDbgDac2Get (void);
IMPORT  void    wdbDbgIac1Set (int value);
IMPORT  int     wdbDbgIac1Get (void);
IMPORT  void    wdbDbgIac2Set (int value);
IMPORT  int     wdbDbgIac2Get (void);
IMPORT  void    wdbDbgIac3Set (int value);
IMPORT  int     wdbDbgIac3Get (void);
IMPORT  void    wdbDbgIac4Set (int value);
IMPORT  int     wdbDbgIac4Get (void);
# endif	/* 509 555 860 : 603 EC603 604 : 403 : 405 405F 440 */

#else 	/* __STDC__ */

# if	((CPU == PPC509) || (CPU == PPC555) || (CPU == PPC860))
IMPORT	void	dbgCmpaSet ();
IMPORT	void	dbgCmpbSet ();
IMPORT	void	dbgCmpcSet ();
IMPORT	void	dbgCmpdSet ();
IMPORT	void	dbgCmpeSet ();
IMPORT	void	dbgCmpfSet ();
IMPORT	void	dbgLctrl1Set ();
IMPORT	void	dbgLctrl2Set ();
IMPORT	void	dbgIctrlSet ();
IMPORT	int	dbgCmpaGet () ;
IMPORT	int	dbgCmpbGet () ;
IMPORT	int	dbgCmpcGet () ;
IMPORT	int	dbgCmpdGet () ;
IMPORT	int	dbgCmpeGet () ;
IMPORT	int	dbgCmpfGet () ;
IMPORT	int	dbgLctrl1Get () ;
IMPORT	int	dbgLctrl2Get () ;
IMPORT	int	dbgIctrlGet () ;

# elif	(CPU == PPC603) || (CPU == PPCEC603) || (CPU == PPC604)
IMPORT	void	wdbDbgIabrSet ();
IMPORT	int	wdbDbgIabrGet ();
#  if	(CPU == PPC604)
IMPORT	void	wdbDbgDabrSet ();
IMPORT	int	wdbDbgDabrGet ();
IMPORT	int	wdbDbgDarGet ();
#  endif  /* (CPU == PPC604) */

# elif	(CPU == PPC403)
IMPORT	void	wdbDbgDbcrSet ();
IMPORT	int	wdbDbgDbcrGet ();
IMPORT	void	wdbDbgDbsrSet ();
IMPORT	int	wdbDbgDbsrGet ();
IMPORT	void	wdbDbgDac1Set ();
IMPORT	int	wdbDbgDac1Get ();
IMPORT	void	wdbDbgDac2Set ();
IMPORT	int	wdbDbgDac2Get ();
IMPORT	void	wdbDbgIac1Set ();
IMPORT	int	wdbDbgIac1Get ();
IMPORT	void	wdbDbgIac2Set ();
IMPORT	int	wdbDbgIac2Get ();

# elif   ((CPU == PPC405) || (CPU == PPC405F) || (CPU == PPC440))
IMPORT  void    wdbDbgDbcr0Set ();
IMPORT  int     wdbDbgDbcr0Get ();
IMPORT  void    wdbDbgDbcr1Set ();
IMPORT  int     wdbDbgDbcr1Get ();
#  if	(CPU == PPC440)
IMPORT  void    wdbDbgDbcr2Set ();
IMPORT  int     wdbDbgDbcr2Get ();
IMPORT  void    wdbDbgDbdrSet ();
IMPORT  int     wdbDbgDbdrGet ();
#  endif  /* (CPU == PPC440) */
IMPORT  void    wdbDbgDbsrSet ();
IMPORT  int     wdbDbgDbsrGet ();
IMPORT  void    wdbDbgDac1Set ();
IMPORT  int     wdbDbgDac1Get ();
IMPORT  void    wdbDbgDac2Set ();
IMPORT  int     wdbDbgDac2Get ();
IMPORT  void    wdbDbgIac1Set ();
IMPORT  int     wdbDbgIac1Get ();
IMPORT  void    wdbDbgIac2Set ();
IMPORT  int     wdbDbgIac2Get ();
IMPORT  void    wdbDbgIac3Set ();
IMPORT  int     wdbDbgIac3Get ();
IMPORT  void    wdbDbgIac4Set ();
IMPORT  int     wdbDbgIac4Get ();
# endif	/* 509 555 860 : 603 EC603 604 : 403 : 405 405F 440 */

#endif 	/* __STDC__ */

#endif 	/* _ASMLANGUAGE */
#endif 	/* DBG_HARDWARE_BP */

#ifdef __cplusplus
}
#endif

#endif /* __INCdbgPpcLibh */
