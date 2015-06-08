/* regsSimhppa.h - simhppa registers header */

/* Copyright 1993 Wind River Systems, Inc. */

/*
modification history
--------------------
01e,02dec96,mem  Added array to hold os-specific async process state.
01d,05nov96,mem  removed retVal field.
01c,28oct96,mem  changed __ctxRestore prototype, changed extern to IMPORT.
		 removed sig_regs field.
01b,20nov95,mem  expanded to use the full HP-PA register set.
01a,11aug93,gae  from rrr.
*/

#ifndef __INCregsSimhppah
#define __INCregsSimhppah

#ifndef _ASMLANGUAGE

#ifdef __cplusplus
extern "C" {
#endif

/* some common names for registers */
#define reg_fp 		gr[3]
#define reg_pc 		pc
#define reg_npc 	npc
#define reg_psw 	psw
#define reg_arg0	gr[26]
#define reg_dp		gr[27]
#define reg_ret0	gr[28]
#define spReg		gr[30]
#define reg_sp		spReg

typedef struct rset 		/* simhppa Register Set. */
    {
    /* fields which are not strictly registers */
    int		flags;		/* OS flags */
    int		reg_mask;	/* signal mask for the task. */

    /* HP-PA registers */
    int		pc;		/* (pcoqh) pc */
    int		npc;		/* (pcoqt) next pc */
    int		pcsqh;		/* */
    int		pcsqt;		/* */
    int		gr[32];		/* general regs */
    int		sr[8];		/* space regs */
    int		sar;		/* shift amount register (cr11) */
    int		eiem;		/* */
    int		iir;		/* */
    int		isr;		/* */
    int		ior;		/* */
    int		psw;		/* processor status word. */
    int		_goto;		/* */
    int		ccr;		/* control register ccr */
    int		cr0;		/* control register #0 */
    int		cr8;		/* control register #8 */
    int		cr9;		/* control register #9 */
    int		cr12;		/* control register #12 */
    int		cr13;		/* control register #13 */
    int		cr24;		/* control register #24 */
    int		cr25;		/* control register #25 */
    int		cr26;		/* control register #26 */
    int		mpsfu_high;	/* */
    int		mpsfu_low;	/* */
    int		mpsfu_ovflo;	/* */
    int		async;		/* TRUE if saved state is async */
    int		os_state[350];	/* os-specific async state */
    } REG_SET;

#define PC_OFFSET       OFFSET(REG_SET,reg_pc)

#if defined(__STDC__) || defined(__cplusplus)

IMPORT int	sr3RegGet (void);
IMPORT int	dpRegGet (void);
IMPORT int	spRegGet (void);
IMPORT int	vxLDC (void *address);
IMPORT int	__ctxSave (REG_SET *pRegs);
IMPORT int	__ctxWindSave (REG_SET *pRegs, int reschedule);
IMPORT void	__ctxRestore (REG_SET *pRegs);

#else

IMPORT int	sr3RegGet ();
IMPORT int	dpRegGet ();
IMPORT int	spRegGet ();
IMPORT int	vxLDC ();
IMPORT int	__ctxSave ();
IMPORT int	__ctxWindSave ();
IMPORT void	__ctxRestore ();

#endif /* defined(__STDC__) || defined(__cplusplus) */

#ifdef __cplusplus
}
#endif

#endif  /* _ASMLANGUAGE */

#define REG_SET_FLAGS	0x0000
#define REG_SET_MASK	0x0004
#define REG_SET_PC	0x0008
#define REG_SET_NPC	0x000c
#define REG_SET_PCSQH	0x0010
#define REG_SET_PCSQT	0x0014
#define REG_SET_GR0	0x0018
#define REG_SET_SR0	0x0098
#define REG_SET_SAR	0x00b8
#define REG_SET_EIEM	0x00bc
#define REG_SET_IIR	0x00c0
#define REG_SET_ISR	0x00c4
#define REG_SET_IOR	0x00c8
#define REG_SET_PSW	0x00cc
#define REG_SET_GOTO	0x00d0
#define REG_SET_CCR	0x00d4
#define REG_SET_CR0	0x00d8
#define REG_SET_CR8	0x00dc
#define REG_SET_CR9	0x00e0
#define REG_SET_CR12	0x00e4
#define REG_SET_CR13	0x00e8
#define REG_SET_CR24	0x00ec
#define REG_SET_CR25	0x00f0
#define REG_SET_CR26	0x00f4
#define REG_SET_MPSFU_HIGH	0x00f8
#define REG_SET_MPSFU_LOW	0x00fc
#define REG_SET_MPSFU_OVFLO	0x0100
#define REG_SET_ASYNC	0x0104

#endif /* __INCregsSimhppah */
