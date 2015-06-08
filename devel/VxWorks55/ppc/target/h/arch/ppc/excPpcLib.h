/* excPpcLib.h - PowerPC exception library header */

/* Copyright 1984-1998 Wind River Systems, Inc. */

/*
modification history
--------------------
01r,13nov01,yvp  Changed _EXC_NEW_OFF_FIT to 0x01180 to make room for extended
                 vectors.
01q,27aug01,pch  Only define _EXC_INFO_* if the corresponding members of
                 EXC_INFO exist; utilize IVPR/IVOR where available.
01p,15aug01,pch  Add PPC440; cleanup
01o,30nov00,s_m  fixed bus error handling for 405
01n,25oct00,s_m  renamed PPC405 cpu types
01m,13oct00,sm   removed bear and besr from EXC_INFO for 405 since it doesn't
                 have these regs
01l,30aug00,sm   added cases for PPC405 and PPC405F
01k,12mar99,zl   added PowerPC 509 and PowerPC 555 support
01j,18aug98,tpr  added PowerPC EC 603 support.
01i,06aug97,tam  added macro _EXC_NEW_OFF_PIT and _EXC_NEW_OFF_FIT.
01h,20mar97,tam  added prototype for excIntCrtConnect (PPC403 specific).
01g,26feb97,tam  added support for the 403GC and 403GCX cpus.
01f,20apr96,tpr  added excConnect(), excIntConnect(),excVecSet(),excVecGet(),
		 excVecBaseSet() and excVecBaseGet() prototype.
01e,04mar96,tam  added PPC403 support.
01d,14feb96,tpr  added PPC604 support.
01c,09feb95,yao  added excCrtStub () for PPC403.
01b,07nov94,yao  changed PPC403GA to PPC403.  cleanup.
01a,07jul94,yao  written.
*/

#ifndef __INCexcPcLibh
#define __INCexcPcLibh

#ifdef __cplusplus
extern "C" {
#endif

/* exception information valid bits */

#define _EXC_INFO_VEC	0x001	/* vector offset valid */
#define _EXC_INFO_CIA	0x002	/* curent exception address(pc) valid */
#define _EXC_INFO_MSR	0x004	/* machine status register valid */

#if	((CPU == PPC403) || (CPU == PPC405) || (CPU == PPC405F) || \
	 (CPU == PPC440))
#define _EXC_INFO_DEAR	0x008	/* data exception address register valid */
#else	/* CPU == PPC4xx */
#define _EXC_INFO_DAR	0x008	/* data access register valid */
#endif	/* CPU == PPC4xx */

#define _EXC_INFO_XER	0x010	/* fixed point register valid */
#define _EXC_INFO_CR	0x020	/* condition register valid */

#if	((CPU == PPC403) || (CPU == PPC405))
#define _EXC_INFO_BEAR	0x040	/* bus error address register valid */
#define _EXC_INFO_BESR	0x080	/* bus error syndrome register valid */
#elif	(CPU == PPC405F)
	/* This setting of _EXC_INFO_BEAR is a change from that used in
	 * the 405F release, where it was the same as _EXC_INFO_FPCSR
	 * (clearly broken).
	 */
#define _EXC_INFO_FPCSR	0x040	/* fp status register valid */
#define _EXC_INFO_BEAR	0x080	/* bus error address register valid */
#elif	(CPU == PPC440)
	/* Just padding for the 440, so far. */
#else	/* CPU == PPC4xx */
#define _EXC_INFO_FPCSR	0x040	/* fp status register valid */
#define _EXC_INFO_DSISR	0x080	/* data storage interrupt status reg valid */
#endif	/* CPU == PPC4xx */

#define _EXC_INFO_NIA	0x8000	/* pc saved in cia is nia */

#define _EXC_INFO_DEFAULT	(_EXC_INFO_VEC|_EXC_INFO_CIA|_EXC_INFO_MSR| \
				 _EXC_INFO_CR)

#ifndef _ASMLANGUAGE

typedef struct
    {
    UINT32	valid;		/* 0x118: indicators that following fields
				 *        are valid */
    UINT32	vecOff;		/* 0x11c: vector offset */
    _RType	cia;		/* 0x120: save/restore reg0 - exception
				 *        address */
    _RType	msr;		/* 0x124/0x128: save/restore register1 - msr */
# ifdef	_EXC_INFO_DEAR
    _RType	dear;		/* data exception address register */
# else	/* _EXC_INFO_DEAR */
    _RType	dar;		/* 0x128/0x130: data access register */
# endif	/* _EXC_INFO_DEAR */
    UINT32      cr;		/* 0x12c/0x134: condition register */
    UINT32	xer;		/* 0x130/0x138: fixed point exception reg */
# if	((CPU == PPC403) || (CPU == PPC405))
    UINT32	bear;		/* bus error address register */
    UINT32	besr;		/* bus error syndrome register */
    /* XXX - handling of bus errors is implementation specific on the PPC405 */
# elif	(CPU == PPC405F)
    UINT32	fpcsr;		/* floating point status reg */
    UINT32	bear;
# elif	(CPU == PPC440)
    UINT32	pad3;		/* future: use for fpcsr if a 440 with FPU ? */
    UINT32	pad4;
# else	/* CPU == PPC4xx */
    UINT32	fpcsr;		/* 0x134/0x13c: floating point status reg */
    UINT32	dsisr;		/* 0x138/0x140: data storage interrupt
				 * status reg */
# endif	/* CPU == PPC4xx */
    } EXC_INFO;

#endif 	/* _ASMLANGUAGE */

#define _EXC_CODE_SIZE	16	/* size of exc code */

#define EXC_VEC_LOW	0x00000	/* lowest vector entry */
#define EXC_VEC_HIGH	0x02fff	/* highest vector entry */

#define _PPC_EXC_VEC_BASE_LOW	0x00000000
#define _PPC_EXC_VEC_BASE_HIGH	0xfff00000

/* exception vector offset */

#ifdef	IVPR
/*
 * This processor implements the Book E Interrupt Vector Prefix Register
 * and Interrupt Vector Offset Registers, instead of the Exception Vector
 * Prefix Register used (with fixed offsets) in older designs.  The new
 * architecture enables us to locate exception vectors "anywhere".
 */
#define	_EXC_OFF_CRTL		0x0100	/* Critical Input */
#define	_EXC_OFF_MACH		0x0200	/* Machine Check */
#define	_EXC_OFF_DATA		0x0300	/* Data Storage */
#define	_EXC_OFF_INST		0x0400	/* Instruction Storage */
#define	_EXC_OFF_INTR		0x0500	/* External Input */
#define	_EXC_OFF_ALIGN		0x0600	/* Alignment */
#define	_EXC_OFF_PROG		0x0700	/* Program */
#define	_EXC_OFF_FPU		0x0800	/* Floating Point Unavailable */
#define	_EXC_OFF_SYSCALL	0x0900	/* System Call */
#define	_EXC_OFF_APU		0x0a00	/* Auxiliary Processor Unavailable */
#define	_EXC_OFF_DECR		0x0b00	/* Decrementer */
#define	_EXC_OFF_FIT		0x0c00	/* Fixed Interval Timer */
#define	_EXC_OFF_WD		0x0d00	/* Watchdog Timer */
#define	_EXC_OFF_DATA_MISS	0x0e00	/* Data TLB Error */
#define	_EXC_OFF_INST_MISS	0x0f00	/* Instruction TLB Error */
#define	_EXC_OFF_DBG		0x1000	/* Debug exception */

/* Mappings between vector names and corresponding IVORs, for excALib.s */
#define	IVOR0_VAL   _EXC_OFF_CRTL	/* Critical Input */
#define	IVOR1_VAL   _EXC_OFF_MACH	/* Machine Check */
#define	IVOR2_VAL   _EXC_OFF_DATA	/* Data Storage */
#define	IVOR3_VAL   _EXC_OFF_INST	/* Instruction Storage */
#define	IVOR4_VAL   _EXC_OFF_INTR	/* External Input */
#define	IVOR5_VAL   _EXC_OFF_ALIGN	/* Alignment */
#define	IVOR6_VAL   _EXC_OFF_PROG	/* Program */
#define	IVOR7_VAL   _EXC_OFF_FPU	/* Floating Point Unavailable */
#define	IVOR8_VAL   _EXC_OFF_SYSCALL	/* System Call */
#define	IVOR9_VAL   _EXC_OFF_APU	/* Auxiliary Processor Unavailable */
#define	IVOR10_VAL  _EXC_OFF_DECR	/* Decrementer */
#define	IVOR11_VAL  _EXC_OFF_FIT	/* Fixed Interval Timer */
#define	IVOR12_VAL  _EXC_OFF_WD		/* Watchdog Timer */
#define	IVOR13_VAL  _EXC_OFF_DATA_MISS	/* Data TLB Error */
#define	IVOR14_VAL  _EXC_OFF_INST_MISS	/* Instruction TLB Error */
#define	IVOR15_VAL  _EXC_OFF_DBG	/* Debug exception */

#else	/* IVPR */

#define _EXC_OFF_RES0	 	0x00000		/* reserved */
#define _EXC_OFF_RESET	 	0x00100		/* system reset */
#define _EXC_OFF_MACH	 	0x00200		/* machine check */
#define _EXC_OFF_DATA	 	0x00300		/* data access */
#define _EXC_OFF_INST	 	0x00400		/* instruction access */
#define _EXC_OFF_INTR	 	0x00500		/* external interrupt */
#define _EXC_OFF_ALIGN	 	0x00600		/* alignment */
#define _EXC_OFF_PROG	 	0x00700		/* program */
#define _EXC_OFF_FPU	 	0x00800		/* floating point unavailable */
#define _EXC_OFF_DECR	 	0x00900		/* decrementer */
#define _EXC_OFF_RES1	 	0x00a00		/* reserved */
#define _EXC_OFF_RES2	 	0x00b00		/* reserved */
#define _EXC_OFF_SYSCALL 	0x00c00		/* system call */
#define _EXC_OFF_TRACE	 	0x00d00		/* trace */
#define _EXC_OFF_RES3	 	0x00e00		/* reserved */
						/* exception for PPC601) */

# if	(CPU == PPC509)
#undef  _EXC_OFF_DATA
#undef  _EXC_OFF_INST
#undef  _EXC_OFF_RES3
#define _EXC_OFF_FPA	 	0x00e00	/* floating point assist */
#define _EXC_OFF_SW_EMUL	0x01000	/* Software Emulation */
#define _EXC_OFF_DATA_BKPT	0x01c00	/* Data Breakpoint */
#define _EXC_OFF_INST_BKPT	0x01d00	/* Instruction Breakpoint */
#define	_EXC_OFF_PERI_BKPT	0x01e00	/* Maskable Dev. Port or
					   Peripheral Breakpoint */
#define _EXC_OFF_NM_DEV_PORT	0x01f00	/* Non Maskable develop. port */
# endif	/* (CPU == PPC509) */

# if	(CPU == PPC555)
#undef  _EXC_OFF_DATA
#undef  _EXC_OFF_INST
#undef  _EXC_OFF_RES3
#define _EXC_OFF_FPA		0x00e00	/* floating point assist */
#define _EXC_OFF_SW_EMUL	0x01000	/* Software Emulation */
#define _EXC_OFF_IPE		0x01300 /* Instruction protection error */
#define _EXC_OFF_DPE		0x01400 /* Data protection error */
#define _EXC_OFF_DATA_BKPT	0x01c00	/* Data Breakpoint */
#define _EXC_OFF_INST_BKPT	0x01d00	/* Instruction Breakpoint */
#define	_EXC_OFF_PERI_BKPT	0x01e00	/* Maskable Dev. Port or */
					/* Peripheral Breakpoint */
#define _EXC_OFF_NM_DEV_PORT	0x01f00	/* Non Maskable develop. port */
# endif	/* (CPU == PPC555) */

# if	(CPU == PPC601)
#undef	_EXC_OFF_RES1
#define _EXC_OFF_IOERR	 	0x00a00	/* I/O controller interface error */
#undef 	_EXC_OFF_TRACE
#define _EXC_OFF_RES4	 	0x00d00		/* reserved for mpc601 */
#define _EXC_OFF_RUN_TRACE 	0x02000		/* run-mode/trace exception */
# endif	/* (CPU == PPC601) */

# if	((CPU == PPC603) || (CPU == PPCEC603))
#define _EXC_OFF_INST_MISS	0x01000	/* instruction translation miss */
#define _EXC_OFF_LOAD_MISS	0x01100	/* data load translation miss */
#define _EXC_OFF_STORE_MISS	0x01200	/* data store translation miss */
# endif	/* ((CPU == PPC603) || (CPU == PPCEC603)) */

# if	((CPU == PPC603) || (CPU == PPCEC603) || (CPU == PPC604))
#define _EXC_OFF_INST_BRK	0x01300 /* instruction breakpoint exception */
#define _EXC_OFF_SYS_MNG	0x01400	/* system management exception */
# endif	/* ((CPU == PPC603) || (CPU == PPCEC603) || (CPU == PPC604)) */

# if	((CPU == PPC403) || (CPU == PPC405) || (CPU == PPC405F))
#undef 	_EXC_OFF_RESET
#undef  _EXC_OFF_DATA
#undef 	_EXC_OFF_DECR
#undef 	_EXC_OFF_TRACE

#define _EXC_OFF_CRTL	 	0x00100	/* critical interrupt */
#define _EXC_OFF_PROT	 	0x00300	/* protection violation exception */
#define _EXC_OFF_PIT		0x01000	/* programmable interval timer */
#define _EXC_OFF_FIT		0x01010	/* fixed interval timer */
#define _EXC_OFF_WD		0x01020	/* watchdog timer */
#define _EXC_OFF_DBG		0x02000	/* debug exception */
#define _EXC_OFF_DATA_MISS	0x01100	/* data translation miss */
#define _EXC_OFF_INST_MISS	0x01200	/* instruction translation miss */

/*
 * the FIT and PIT excConnectCode stub won't fit at their default location
 * so we need to put them where we've got some available space and jump
 * to it from_EXC_OFF_PIT and _EXC_OFF_FIT.
 */

#define _EXC_NEW_OFF_PIT	0x01080	/* programmable interval timer */
#define _EXC_NEW_OFF_FIT	0x01180	/* fixed interval timer */
# endif	/* CPU == PPC40x */

#endif	/* IVPR */

#ifndef	_EXC_OFF_TRACE
/* Processors having no Trace exception handle single step differently */

#define BRANCH_MASK             0xF0000000
#define OP_BRANCH               0x40000000
#define BCCTR_MASK              0xFC0007FE
#define BCLR_MASK               0xFC0007FE
#define AA_MASK                 0x00000002

#define BO_NB_BIT               5               /* nb bits of BO field    */
#define CR_NB_BIT               32              /* nb bits of CR register */

#define _OP(opcd, xo)           ((opcd << 26) + (xo << 1))

#define INST_BCCTR              _OP(19, 528)    /* BCCTR instruction opcode */
#define INST_BCLR               _OP(19, 16)     /* BCLR instruction opcode */

#define _IFIELD_LI(x)           ((0x02000000 & x) ? ((0xf6000000 | x) & ~3) : \
                                                    ((0x03fffffc & x) & ~3))
#define _IFIELD_BD(x)           ((0x00008000 & x) ? (0xffff0000 | (x & ~3)) : \
                                                    (0x0000fffc & x))
                                                /* ^ signed branch displ */
#define _IFIELD_BO(x)           ((0x03e00000 & x) >> 21)  /* branch options */
#define _IFIELD_BI(x)           ((0x001f0000 & x) >> 16)  /* branch condition */

#define _REG32_BIT(reg, no)     (0x00000001 & (reg >> (no)))
                        /* value of no th bit of the 32 bits register reg */
                        /* bit 0 is the LSB */
#endif	/* _EXC_OFF_TRACE */

/* SRR1 bit definition for program exception */

#define _EXC_PROG_SRR1_FPU	0x00100000	/* floating-point enabled */
#define _EXC_PROG_SRR1_ILL	0x00080000	/* illegal instruction */
#define _EXC_PROG_SRR1_PRIV	0x00040000	/* privileged instruction */
#define _EXC_PROG_SRR1_TRAP	0x00020000	/* trap exception */
#define _EXC_PROG_SRR1_NIA	0x00010000	/* SRR0 has nia (else cia) */

#ifdef	_PPC_MSR_CE
/*
 * This processor implements "critical" exceptions,
 * which use different SRR's than other exceptions.
 * Abstract the names of those registers, since they are
 * different in the 440 than in previous 4xx processors.
 */
# ifdef	CSRR0
#  define	CRIT_SAVE_PC	CSRR0
# else	/* CSRR0 */
#  define	CRIT_SAVE_PC	SRR2
# endif	/* CSRR0 */

# ifdef	CSRR1
#  define	CRIT_SAVE_MSR	CSRR1
# else	/* CSRR1 */
#  define	CRIT_SAVE_MSR	SRR3
# endif	/* CSRR1 */

#endif	/* _PPC_MSR_CE */

#ifndef _ASMLANGUAGE

/* variable declarations */

extern FUNCPTR  excExcepHook;   /* add'l rtn to call when exceptions occur */

/* function declarations */

# if defined(__STDC__) || defined(__cplusplus)

extern STATUS		excConnect (VOIDFUNCPTR *, VOIDFUNCPTR);
extern STATUS  		excIntConnect (VOIDFUNCPTR *, VOIDFUNCPTR);
extern void		excVecSet (FUNCPTR *, FUNCPTR);
extern FUNCPTR		excVecGet (FUNCPTR *);
extern void		excVecBaseSet (FUNCPTR *);
extern FUNCPTR *	excVecBaseGet (void);

#  ifdef _PPC_MSR_CE
extern STATUS		excCrtConnect (VOIDFUNCPTR *, VOIDFUNCPTR);
extern STATUS		excIntCrtConnect (VOIDFUNCPTR *, VOIDFUNCPTR);
extern STATUS  		excIntConnectTimer (VOIDFUNCPTR *, VOIDFUNCPTR);
#  endif  /* _PPC_MSR_CE */

# else	/* __STDC__ */

extern STATUS		excConnect ();
extern STATUS		excIntConnect ();
extern void		excVecSet ();
extern FUNCPTR		excVecGet ();
extern void		excVecBaseSet ();
extern FUNCPTR *	excVecBaseGet ();

#  ifdef _PPC_MSR_CE
extern STATUS		excCrtConnect ();
extern STATUS		excIntCrtConnect ();
extern STATUS		excIntConnectTimer ();
#  endif  /* _PPC_MSR_CE */

# endif	/* __STDC__ */

#endif /* _ASMLANGUAGE */

#ifdef __cplusplus
}
#endif

#endif /* __INCexcPpcLibh */
