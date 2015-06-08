/* dsmI86Lib.h - i80x86 disassembler header */

/* Copyright 1984-2001 Wind River Systems, Inc. */

/*
modification history
--------------------
01h,30aug01,hdn  added SIMD, sysenter/exit support.
01g,06may98,fle  added P5 and P6 instructions and facilities
01f,14nov94,hdn  changed D->DISR, W->WFUL, S->SEXT, P->POP, A->AX, I->IMM.
	   +kdl  minor cleanup.
01e,29may94,hdn  removed I80486 conditional.
01d,31aug93,hdn  changed a type of pD, from char to UCHAR.
		 changed a type of 1st parameter, from char to UCHAR.
01c,07jun93,hdn  added support for c++
01b,26mar93,hdn  added new instructions for 486.
01a,23jun92,hdn  written. 
*/

#ifndef __INCdsmI86Libh
#define __INCdsmI86Libh

#ifdef __cplusplus
extern "C" {
#endif

#include "vwModNum.h"


/* dsmLib status codes */

#define S_dsmLib_UNKNOWN_INSTRUCTION	(M_dsmLib | 1)


#define DSM_BUFSIZE16	16
#define DSM_BUFSIZE32	32

typedef struct
    {
    char	*pOpc;		/* Instruction Name */
    int		type;		/* instruction type */
    int		flag;		/* instruction flag */
    UCHAR	flag2;		/* instruction flag2 */
    UCHAR	op0;		/* opcode 0         */
    UCHAR	op1;		/* opcode 1         */
    UCHAR	op2;		/* opcode 2         */
    UCHAR	mask0;		/* mask 0           */
    UCHAR	mask1;		/* mask 1           */
    UCHAR	mask2;		/* mask 2           */
    } INST;

typedef struct 
    {
    INST	*pI;		/* pointer to a structure INST */
    char	*pD;		/* pointer to a binary data */
    char	a32;		/* address size         0,1 */
    char	d32;		/* data size            0,1 */
    char	lenO;		/* opecode length       1,2 */
    char	lenD;		/* displacement length  0,1,2,4 */
    char	lenI;		/* immediate length     0,1,2,4 */
    char	modrm;		/* MODRM                0,1 */
    char	sib;		/* SIB                  0,1 */
    char	w;		/* W                    0,1 */
    char	d;		/* D                    0,1 */
    char	s;		/* S                    0,1 */
    char	reg;		/* register             3 bits, 8 bits */
    char	mod;		/* value of MODRM byte bits 6,7 */
    char	rm;		/* value of MODRM byte bits 0,1,2 */
    char	ss;
    char	index;
    char	base;
    char	mf;
    char	st;
    char	fd;
    } FORMAT_X;

typedef struct 
    {
    char	*pD;			/* opcode data pointer */
    char	*pOpc;			/* instruction name pointer */
    char	*pOpr0;			/* output string 0 */
    char	*pOpr1;			/* output string 1 */
    char	*pOpr2;			/* output string 2 */
    int		len;			/* instruction length */
    int		addr;			/* address operand */
    char	obuf [DSM_BUFSIZE16];	/* opecode buffer */
    char	rbuf [DSM_BUFSIZE16];	/* register buffer */
    char	mbuf [DSM_BUFSIZE32];	/* memory buffer */
    char	temp [DSM_BUFSIZE32];	/* temporary buffer */
    char	ibuf [DSM_BUFSIZE16];	/* immediate buffer */
    char	dbuf [DSM_BUFSIZE16];	/* displacement buffer */
    } FORMAT_Y;

/* bit definitions of flag */

/* jhw: MSB of flag byte is not included in switch statement used
 *       to format the output.
 */

/* MODRM must be defined if REG,MMXREG,XMMREG are defined */

#define SF	0x10000000
#define OP1	0x20000000
#define OP2	0x40000000
#define OP3	0x80000000
#define WFUL    0x01000000      /* operand size bit */
#define SEXT    0x02000000      /* sign extend flag */
#define DISR    0x04000000      /* data direction info encoded in opcode */
#define TTT     0x08000000      /* hack to look for rotate instructions */

#define TTTN    0x00100000      /* opcode least significant byte 
				 * bits 0-4 = condition test field */
#define MF      0x00200000      /* opcode bits 1-2 are = register size info */
#define POP     0x00400000      /* cat a "P" to end of instruction name */

#define IMM     0x00000001
#define I8      0x00000002      /* imm8 byte exists */
#define DIS     0x00000004      /* displacement info */
#define D8      0x00000008      /* displacement info */
#define D16     0x00000010      /* displacement info */
#define AX      0x00000020      /* store into AX register */
#define CL      0x00000040      /* cat "CL" to end of operands */
#define REG     0x00000080      /* modrm bits 3-5 = register specifier */
#define SREG2   0x00000100      /* modrm bits 3-4 = 2 bit segment register */
#define SREG3   0x00000200      /* modrm bits 3-5 = 3 bit segment register */
#define MODRM   0x00000400      /* modrm byte exists */
#define EEE     0x00000800      /* modrm bits 3-5 = control/debug register */
#define REGRM   0x00001000      /* modrm bits 0-2 = register specifier */
#define PORT    0x00002000      /* use a port as an operand */
#define OFFSEL  0x00004000      /* jump or call offset */
#define D16L8   0x00008000      /* displacement info */
#define FD      0x00010000      /* stack element is destination */
#define ST      0x00020000      /* stack element is operand */
#define MMXRM   0x00040000      /* modrm bits 0-2 = MMX register */
#define MMXREG  0x00080000      /* modrm bits 3-5 = MMX register */
#define GG      0x00800000      /* least sign opcode byte 
				 * bits 0-1 = MMX granularity       */

/* bit definitions of flag2 */

#define XMMRM           0x01    /* modrm bits 0-2 = XMM register */
#define XMMREG          0x02    /* modrm bits 3-5 = XMM register */

#define ESC		0xd8	/* for co-processor instructions */

/* instruction types */

#define		itAaa 			 1
#define		itAad       		 2
#define		itAam       		 3
#define		itAas       		 4
#define		itAddItoA   		 5
#define		itAddItoRM  		 6
#define		itAddRMtoRM		 7
#define		itAndItoA   		 8
#define		itAndItoRM  		 9
#define		itAndRMtoRM		10
#define		itArpl     		11
#define		itAsize   		12
#define		itBound   		13
#define		itBsf       		14
#define		itBsr       		15
#define		itBtI        		16
#define		itBtR        		17
#define		itBtcI      		18
#define		itBtcR      		19
#define		itBtrI      		20
#define		itBtrR      		21
#define		itBtsI      		22
#define		itBtsR      		23
#define		itCall     		24
#define		itCallRM   		25
#define		itCallSeg  		26
#define		itCallSegRM		27
#define		itCbw       		28
#define		itCjmp   		29
#define		itClc       		30
#define		itCld       		31
#define		itCli       		32
#define		itClts     		33
#define		itCmc       		34
#define		itCmpIwiA   		35
#define		itCmpIwiRM  		36
#define		itCmpRMwiRM		37
#define		itCmps     		38
#define		itCs         		39
#define		itCset     		40
#define		itCwd       		41
#define		itDaa       		42
#define		itDas       		43
#define		itDecR      		44
#define		itDecRM     		45
#define		itDiv       		46
#define		itDs         		47
#define		itEnter   		48
#define		itEs         		49
#define		itFs         		50
#define		itGs         		51
#define		itHlt       		52
#define		itIdiv     		53
#define		itImulAwiRM		54
#define		itImulRMwiI		55
#define		itImulRwiRM		56
#define		itInF        		57
#define		itInV        		58
#define		itIncR      		59
#define		itIncRM     		60
#define		itIns       		61
#define		itInt       		62
#define		itInt3      		63
#define		itInto     		64
#define		itIret     		65
#define		itJcxz     		66
#define		itJmpD      		67
#define		itJmpRM     		68
#define		itJmpS      		69
#define		itJmpSeg    		70
#define		itJmpSegRM  		71
#define		itLahf     		72
#define		itLar       		73
#define		itLds       		74
#define		itLea       		75
#define		itLeave   		76
#define		itLes       		77
#define		itLfs       		78
#define		itLgdt     		79
#define		itLgs       		80
#define		itLidt     		81
#define		itLldt     		82
#define		itLmsw     		83
#define		itLock     		84
#define		itLods     		85
#define		itLoop     		86
#define		itLoopnz		87
#define		itLoopz   		88
#define		itLsl       		89
#define		itLss       		90
#define		itLtr       		91
#define		itMovAMtoMA 		92
#define		itMovC      		93
#define		itMovD      		94
#define		itMovItoR   		95
#define		itMovItoRM  		96
#define		itMovRMtoMR  		97
#define		itMovRMtoS  		98
#define		itMovRtoRM  		99
#define		itMovStoRM  		100
#define		itMovT      		101
#define		itMovs     		102
#define		itMovsx   		103
#define		itMovzx   		104
#define		itMulAwiRM  		105
#define		itNeg       		106
#define		itNop       		107
#define		itNot       		108
#define		itOrItoA    		109

#define		itOrItoRM   		111

#define		itOrRMtoRM  		113

#define		itOsize   		115
#define		itOutF      		116
#define		itOutV      		117
#define		itOuts     		118
#define		itPopR      		119
#define		itPopRM     		120
#define		itPopS      		121
#define		itPopa     		122
#define		itPopf     		123
#define		itPushI    		124
#define		itPushR    		125
#define		itPushRM   		126
#define		itPushS    		127
#define		itPusha   		128
#define		itPushf   		129
#define		itRNcmps		130
#define		itRNscas		131
#define		itRcmps   		132
#define		itRet       		133
#define		itRetI      		134
#define		itRetSeg    		135
#define		itRetSegI   		136
#define		itRins      		137
#define		itRlods    		138
#define		itRmovs    		139
#define		itRolRMby1		140
#define		itRolRMbyCL		141
#define		itRolRMbyI		142
#define		itRouts    		143
#define		itRscas   		144
#define		itRstos    		145
#define		itSahf     		146
#define		itSbbIfrA   		147
#define		itSbbIfrRM  		148
#define		itSbbRMfrRM		149
#define		itScas     		150
#define		itSgdt     		151
#define		itShldRMbyCL		152
#define		itShldRMbyI		153
#define		itShrdRMbyCL		154
#define		itShrdRMbyI		155
#define		itSmsw     		156
#define		itSs         		157
#define		itStc       		158
#define		itStd       		159
#define		itSti       		160
#define		itStos     		161
#define		itStr      		162

#define		itSubIfrA   		164
#define		itSubIfrRM  		165
#define		itSubRMfrRM		166
#define		itTestIanA 		167
#define		itTestIanRM		168
#define		itTestRManR		169
#define		itWait     		170
#define		itXchgA    		171
#define		itXchgRM   		172
#define		itXlat     		173

/* instructions for 387 */

#define		itF2xm1			174
#define		itFabs			175
#define		itFaddIRM		176
#define		itFaddST		177
#define		itFchs			178
#define		itFclex			179
#define		itFcomIRM		180
#define		itFcomST		181
#define		itFcompIRM		182
#define		itFcompST		183
#define		itFcompp		184
#define		itFcos			185
#define		itFdecstp		186
#define		itFdivIRM		187
#define		itFdivST		188
#define		itFfree			189
#define		itFincstp		190
#define		itFinit			191
#define		itFld1			192
#define		itFldBCDM		193
#define		itFldERM		194
#define		itFldIRM		195
#define		itFldL2E		196
#define		itFldL2T		197
#define		itFldLG2		198
#define		itFldLIM		199
#define		itFldLN2		200
#define		itFldPI			201
#define		itFldST			202
#define		itFldZ			203
#define		itFldcw			204
#define		itFldenv		205
#define		itFmulIRM		206
#define		itFmulST		207
#define		itFnop			208
#define		itFpatan		209
#define		itFprem			210
#define		itFprem1		211
#define		itFptan			212
#define		itFrndint		213
#define		itFrstor		214
#define		itFsave			215
#define		itFscale		216
#define		itFsin			217
#define		itFsincos		218
#define		itFsqrt			219
#define		itFstIRM		220
#define		itFstST			221
#define		itFstcw			222
#define		itFstenv		223
#define		itFstpBCDM		224
#define		itFstpERM		225
#define		itFstpIRM		226
#define		itFstpLIM		227
#define		itFstpST		228
#define		itFstsw			229
#define		itFstswA		230
#define		itFsubIRM		231
#define		itFsubST		232
#define		itFtst			233
#define		itFucom			234
#define		itFucomp		235
#define		itFucompp		236
#define		itFxam			237
#define		itFxch			238
#define		itFxtract		239
#define		itFyl2x			240
#define		itFyl2xp1		241
#define		itFcmovb		242
#define		itFcmove		243
#define		itFcmovbe		244
#define		itFcmovu		245
#define		itFcmovnb		246
#define		itFcmovne		247
#define		itFcmovnbe		248
#define		itFcmovnu		249
#define		itFcomi			250


/* addendum */

#define		itSidt			300
#define		itSldt			301
#define		itVerr			302
#define		itVerw			303
#define		itXorItoRM		304
#define		itAdcItoRM		305
#define		itXorItoA		306
#define		itAdcItoA		307
#define		itXorRMtoRM		308
#define		itAdcRMtoRM		309
#define		itRep			310
#define		itRepNe			311

/* 80486 instructions */

#define		itBswap			400
#define		itCmpxchg		401
#define		itXadd			402
#define		itInvd			403
#define		itWbinvd		404
#define		itInvlpg		405

/* P5 instructions */
#define		itCmpxchg8b		500
#define		itCpuid			501
#define		itRdtsc			502
#define		itRdmsr			503
#define		itWrmsr			504
#define		itRsm			505

/* P6 instructions */

#define		itCmovcc		600
#define		itRdpmc			601
#define		itUd2			602
#define         itSysenter              603
#define         itSysexit               604

/* MMX instructions */

#define		itEmms			700
#define		itMovd			701
#define		itMovq			702
#define		itPackssdw		703
#define		itPacksswb		704
#define		itPackuswb		705
#define		itPadd			706
#define		itPadds			707
#define		itPaddus		708
#define		itPand			709
#define		itPandn			710
#define		itPcmpeq		711
#define		itPcmpgt		712
#define		itPmadd			713
#define		itPmulh			714
#define		itPmull			715
#define		itPor			716
#define		itPsll			717
#define		itPsra			718
#define		itPsrl			719
#define		itPsub			720
#define		itPsubs			721
#define		itPsubus		722
#define		itPunpckh		723
#define		itPunpckl		724
#define		itPxor			725

/* SIMD definitions */
#define		itAddps			800
#define		itAddss			801
#define		itAndnps		802
#define		itAndps			803
#define		itCmpps			804
#define		itCmpss			805
#define		itComiss		806
#define		itCvtpi2ps		807
#define		itCvtps2pi		808
#define		itCvtsi2ss		809
#define		itCvtss2si		810
#define		itCvttps2pi		811
#define		itCvttss2si		812
#define		itDivps			813
#define		itDivss			814
#define		itFxrstor		815
#define		itFxsave		816
#define		itLdmxcsr		817
#define		itMaxps			818
#define		itMaxss			819
#define		itMinps			820
#define		itMinss			821
#define		itMovaps		822
#define		itMovhlps		823
#define		itMovhps		824
#define		itMovlhps		825
#define		itMovlps		826
#define		itMovmskps		827
#define		itMovss			828
#define		itMovups		829
#define		itMulps			830
#define		itMulss			831
#define		itOrps			832
#define		itRcpps			833
#define		itRcpss			834
#define		itRsqrtps		835
#define		itRsqrtss		836
#define		itShufps		837
#define		itSqrtps		838
#define		itSqrtss		839
#define		itStmxcsr		840
#define		itSubps			841
#define		itSubss			842
#define		itUcomiss		843
#define		itUnpckhps		844
#define		itUnpcklps		845
#define		itXorps			846

#define		itPavgb			847
#define		itPavgw			848
#define		itPextrw		849
#define		itPinsrw		850
#define		itPmaxsw		851
#define		itPmaxub		852
#define		itPminsw		853
#define		itPminub		854
#define		itPmovmskb		855
#define		itPmulhuw		856
#define		itPsadbw		857
#define		itPshufw		858

#define		itMaskmovq		859
#define		itMovntps		860
#define		itMovntq		861
#define		itPrefetcht0		862
#define		itPrefetcht1		863
#define		itPrefetcht2		864
#define		itPrefetchnta		865
#define		itSfence		866

/* function declarations */

#if defined (__STDC__) || defined (__cplusplus)

IMPORT	  int	       dsmData (UCHAR *binInst, int address);
IMPORT	  int	       dsmInst (UCHAR *binInst, int address, 
				VOIDFUNCPTR prtAddress);
IMPORT	  int	       dsmNbytes (UCHAR *binInst);

#else

IMPORT	  int	       dsmData ();
IMPORT	  int	       dsmInst ();
IMPORT	  int	       dsmNbytes ();

#endif	/* __STDC__ */

#ifdef __cplusplus
}
#endif

#endif /* __INCdsmI86Libh */
