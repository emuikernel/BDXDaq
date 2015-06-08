/* regsSimsolaris.h - simsolaris registers header */

/* Copyright 1995 Wind River Systems, Inc. */

/*
modification history
--------------------
01c,30dec97,dbt  added common names for registers
01b,27oct95,ism  cleanup
01a,12jun95,ism  written
*/

#ifndef __INCregsSimsolarish
#define __INCregsSimsolarish

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _ASMLANGUAGE

#define NGREG 19
#define SPARC_MAXREGWINDOW  31      /* max windows in SPARC arch. */

#define REG_PSR (0)
#define REG_PC  (1)
#define REG_nPC (2)
#define REG_Y   (3)
#define REG_G1  (4)
#define REG_G2  (5)
#define REG_G3  (6)
#define REG_G4  (7)
#define REG_G5  (8)
#define REG_G6  (9)
#define REG_G7  (10)
#define REG_O0  (11)
#define REG_O1  (12)
#define REG_O2  (13)
#define REG_O3  (14)
#define REG_O4  (15)
#define REG_O5  (16)
#define REG_O6  (17)
#define REG_O7  (18)

typedef struct solaris_ucontext  /* solaris-required context structure    */
    {
    unsigned long             uc_flags;
    struct solaris_ucontext   *uc_link;
    unsigned long             uc_sigmask[4];

    struct
        {
        char        *ss_sp;
        int         ss_size;
        int         ss_flags;
        } uc_stack;

    struct
        {
        int         gregs[NGREG];

        struct
            {
            int     wbcnt;
            int     *spbuf[SPARC_MAXREGWINDOW];
            struct
		{
                int rw_local[8];
                int rw_in[8];
        	} wbuf[SPARC_MAXREGWINDOW];
            } *gwins; /* POSSIBLE pointer to register windows */

        struct
	    {
    	    union
		{                           /* FPU floating point regs */
                unsigned    fpu_regs[32];   /* 32 singles */
                double      fpu_dregs[16];  /* 16 doubles */
                } fpu_fr;

    	    void           *fpu_q;          /* ptr to array of FQ entries */
    	    unsigned        fpu_fsr;        /* FPU status register */
    	    unsigned char   fpu_qcnt;       /* # of entries in saved FQ */
    	    unsigned char   fpu_q_entrysize;/* # of bytes per FQ entry */
    	    unsigned char   fpu_en;         /* flag signifying fpu in use */
            } fpregs;

        long        filler[21];
        } uc_mcontext;

    long                   uc_filler[23];

    } solaris_ucontext_t;

typedef struct		/* sparcsim Register Set */
    {
    void *reg_pc;	/* pc to retore */
    void *reg_npc;	/* next pc to restore */
    int   reg_psr;	/* psr to restore */
    int   reg_wbcnt;	/* number of outstanding windows (always 0!) */
    int   reg_tbr;	/* not used for sim */
    int   reg_y;
    int   reg_onstack;	/* sigstack state to restore (instead of pCpContext) */
    int   reg_mask[4];	/* signal mask to restore (instead of pAsrSet) */
    int   reg_global[8];
    int   reg_out[8];
    int   reg_local[8];
    int   reg_in[8];
    union
	{
	int solaris_context[112]; /* Solaris 2.4 u_context=448 bytes */
	solaris_ucontext_t aris;
	} sol;
    } REG_SET;

/* some common names for registers */

#undef pc
#define pc	reg_pc
#define reg_sp	reg_out[6]
#define reg_fp	reg_in[6]

#undef spReg
#define spReg	reg_out[6]

#define REG_SET_PC      0x00            /* Program Counter */
#define REG_SET_NPC     0x04            /* Next Program Counter */
#define REG_SET_PSR     0x08            /* Processor Status Register */
#define REG_SET_WIM     0x0C            /* Window Invalid Mask */
#define REG_SET_TBR     0x10            /* Trap Base Register */
#define REG_SET_Y       0x14            /* Y Register */
/*#define REG_SET_pCP     0x18           / * Coprocessor Pointer */
/*#define REG_SET_pASR    0x1C           / * Pointer to ASRs (%asr1 - %asr31) */

#define REG_SET_GLOBALS 0x2C            /* Global Registers (%R0  - %R7)  */
#define REG_SET_OUTS    0x4C            /* Out Registers    (%R8  - %R15) */
#define REG_SET_LOCALS  0x6C            /* Local Registers  (%R16 - %R23) */
#define REG_SET_INS     0x8C            /* In Registers     (%R24 - %R31) */

#define REG_SET_GLOBAL(n)   (REG_SET_GLOBALS + (4 * (n)))
#define REG_SET_OUT(n)      (REG_SET_OUTS + (4 * (n)))
#define REG_SET_LOCAL(n)    (REG_SET_LOCALS + (4 * (n)))
#define REG_SET_IN(n)       (REG_SET_INS + (4 * (n)))

#define pREG_COPROCESSOR(n)	(4 * (n))
#define pREG_ASR(n)      	(4 * (n))

#define PC_OFFSET	REG_SET_PC	/* referenced by pc() in usrLib */

#endif  /* _ASMLANGUAGE */

#ifdef __cplusplus
}
#endif

#endif /* __INCregsSimsolarish */
