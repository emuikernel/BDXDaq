/* dsmPpc.h - PowerPC disassembler header */

/* Copyright 1984-2003 Wind River Systems, Inc. */

/*
modification history
--------------------
01c,23jan03,pch  Add isel for 440x5 core (440GX)
01b,05dec02,pch  SPR 84100: correctly decode WS field for 4xx tlbre/tlbwe
01a,18sep01,pch  created by merging target/h/arch/ppc/dsmPpcLib.h vn 01o
		 and host/src/tgtsvr/disassembler/dsmPpc.h vn 02j
		 Also added selective handling of processor-specific SPR's
		 and instructions in the host disassembler based on the
		 cpuType of the currently-connected target.

Following are the history entries from the merged files

    07sep01,pch  Improve comments, cleanup; remove MAX_SPR_LEN (no longer used)
    01jun01,dtr  Adding IFORM_VA1B which is needed because input vectors
                 swapped in PIM for two instructions vmaddfp,vnmsubfp.
    29may01,dtr  IFORM_VA_1&2 in the wrong place.
    02may01,kab  Fix duplicate case after merge.
    30apr01,dtr  Correcting merge problems.
    14dec00,s_m  changed IFIELD_WS4 to IFIELD_WS
    09dec00,jrs  Fix WS field.
    07dec00,dtr  Support for Altivec Instruction set.
    05dec00,jrs  Add field type for 405 TLB instructions.
    30nov00,jrs  Add IBM 405 MAC and TLB instructions.
    17nov00,jrs  Add PPC405GP changes.
    20apr98,fle  functions renaming
    03sep97,fle  Modified for being used by the WTX_MEM_DISASSEMBLE service.
    26jul96,tam  added definition for MAX_SPR_LEN, DSTRING_BUF_MAX_SIZE
		 and _IFORM_D_9.
    18jun96,tam  added definition for MAX_SPR_LEN and _IFORM_D_9.
    07jun96,kkk  added endian argument to dsmXXXInst and dsmXXXNbytes
    02feb96,ms   fixed INST_STWU_SP.
    02jan96,elp	 adapted for Tornado.
    13feb95,caf  added sign extension to branch displacement (_IFIELD_BD).
    10feb95,caf  added PPC403 support.
    31jan95,yao  added more macros for branch instructions.
    07nov94,yao  added macros for instructions used by dbgLib and trcLib.
    19aug94,caf  created.
*/

#ifdef	HOST
#include "wtxtypes.h"
#endif	/* HOST */

#ifndef __INCdsmPpch
#define __INCdsmPpch

#ifdef __cplusplus
extern "C" {
#endif

#ifndef	HOST
#include "vwModNum.h"
#endif	/* HOST */

/* typedefs */

typedef struct	/* instruction descriptor */
    {
    char *	name;	/* mnemonic				   */
    UINT32	op;	/* fixed bits				   */
    UINT16	form;	/* form:  major classifier, indexes mask[] */
    UINT16	flags;	/* classification flags:  _IFLAG_*	   */
    } INST;

typedef struct	/* SPR/DCR descriptor */
    {
    int		code;	/* number */
    char *	name;	/* name   */
    } SPR;

/* defines */

#ifdef	HOST
#define _OP(opcd, xo)                   ((UINT32) ((opcd << 26) + (xo << 1)))
#define _VOP(opcd, xo)                  ((UINT32) ((opcd << 26) + xo))
#else	/* HOST */
#define S_dsmLib_UNKNOWN_INSTRUCTION (M_dsmLib | 1)
#define _OP(opcd, xo)		     ((opcd << 26) + (xo << 1))
#define _VOP(opcd, xo)		     ((opcd << 26) + xo)  /* altivec specific */
#endif	/* HOST */

/* instruction forms */

#define _IFORM_I_1      0
#define _IFORM_B_1      1
#define _IFORM_SC_1     2
#define _IFORM_D_1      3
#define _IFORM_D_2      4
#define _IFORM_D_3      5
#define _IFORM_D_4      6
#define _IFORM_D_5      7
#define _IFORM_D_6      8
#define _IFORM_D_7      9
#define _IFORM_D_8      10
#define _IFORM_X_1      11
#define _IFORM_X_2      12
#define _IFORM_X_3      13
#define _IFORM_X_4      14
#define _IFORM_X_5      15
#define _IFORM_X_6      16
#define _IFORM_X_7      17
#define _IFORM_X_8      18
#define _IFORM_X_9      19
#define _IFORM_X_10     20
#define _IFORM_X_11     21
#define _IFORM_X_12     22
#define _IFORM_X_13     23
#define _IFORM_X_14     24
#define _IFORM_X_15     25
#define _IFORM_X_16     26
#define _IFORM_X_17     27
#define _IFORM_X_18     28
#define _IFORM_X_19     29
#define _IFORM_X_20     30
#define _IFORM_X_21     31
#define _IFORM_X_22     32
#define _IFORM_X_23     33
#define _IFORM_X_24     34
#define _IFORM_X_25     35
#define _IFORM_X_26     36
#define _IFORM_X_27     37
#define _IFORM_XL_1     38
#define _IFORM_XL_2     39
#define _IFORM_XL_3     40
#define _IFORM_XL_4     41
#define _IFORM_XFX_1    42
#define _IFORM_XFX_2    43
#define _IFORM_XFX_3    44
#define _IFORM_XFX_4    45
#define _IFORM_XFL_1    46
#define _IFORM_XO_1     47
#define _IFORM_XO_2     48
#define _IFORM_XO_3     49
#define _IFORM_A_1      50
#define _IFORM_A_2      51
#define _IFORM_A_3      52
#define _IFORM_A_4      53
#define _IFORM_M_1      54
#define _IFORM_M_2      55
#define _IFORM_D_9	56
#define _IFORM_400_1	57	/*  mfdcr  (PPC4xx only) */
#define _IFORM_400_2	58	/*  mtdcr  (PPC4xx only) */
#define _IFORM_400_3	59	/*  wrteei (PPC4xx only) */
#define	_IFORM_405_TLB	60	/*  tlbre, tlbwe (PPC405/440 only) */
#define	_IFORM_405_SX	61	/*  tlbsx (PPC405/440 only) */
#define _IFORM_VA_1	62	/*  altivec only */
#define _IFORM_VA_2	63	/*  altivec only */
#define _IFORM_VX_1	64	/*  altivec only */
#define _IFORM_VX_2	65	/*  altivec only */
#define _IFORM_VX_3	66	/*  altivec only */
#define _IFORM_VX_4	67	/*  altivec only */
#define _IFORM_VX_5	68	/*  altivec only */
#define _IFORM_VX_6	69	/*  altivec only */
#define _IFORM_X_28	70	/*  altivec only */
#define _IFORM_X_29	71	/*  altivec only */
#define _IFORM_X_30	72	/*  altivec only */
#define _IFORM_X_31	73	/*  altivec only */
#define _IFORM_X_32	74	/*  altivec only */
#define _IFORM_X_33	75	/*  altivec only */
#define _IFORM_VXR_1	76	/*  altivec only */
#define _IFORM_VA_1B	77	/*  altivec only */
#define _IFORM_M_3	78	/*  isel (440x5/85xx) */

/* instruction classification flags:  append char to opcode if bit is set */

#define _IFLAG_OE	0x0001	/* includes XER bit		'o' */
#define _IFLAG_RC	0x0002	/* includes record bit		'.' */
#define _IFLAG_AA	0x0004	/* includes absolute bit	'a' */
#define _IFLAG_LK	0x0008	/* includes link bit		'l' */
#define _IFLAG_VRC	0x0200	/* includes altivec record bit	'.' */

/* Flags for processor-specific instructions */
#define _IFLAG_601_SPEC 0x0010		/* 601-specific          */
#define _IFLAG_603_SPEC 0x0020		/* 603-specific          */
#define _IFLAG_604_SPEC 0x0040		/* 604-specific          */
#define _IFLAG_403_SPEC 0x0080		/* 403-specific          */
#define	_IFLAG_405_SPEC	0x0100		/* 405-specific          */
#define	_IFLAG_AV_SPEC	0x0400		/* altivec-specific      */
#define	_IFLAG_440_SPEC	0x0800		/* 440-specific          */
#define	_IFLAG_FP_SPEC	0x1000		/* floating point        */

/* shorthand for table construction */
#define	_IFLAG_6XX_SPEC	(_IFLAG_601_SPEC | _IFLAG_603_SPEC | _IFLAG_604_SPEC)
#define	_IFLAG_4XX_SPEC	(_IFLAG_403_SPEC | _IFLAG_405_SPEC | _IFLAG_440_SPEC)

/* TLB handling as in 405 and 440 */
#define	_IFLAG_4XTLB	(_IFLAG_405_SPEC | _IFLAG_440_SPEC)

/* MAC instructions as in 405 and 440 */
#define	_IFLAG_MAC	(_IFLAG_405_SPEC | _IFLAG_440_SPEC)

/* If any are set, instruction is not generic */
#define	_IFLAG_SPEC	(_IFLAG_4XX_SPEC | _IFLAG_6XX_SPEC | _IFLAG_AV_SPEC | \
			 _IFLAG_FP_SPEC)

/* instruction fields */

#define _IFIELD_AA(x)	((0x00000002 & x) >>  1)    /* absolute address      */
#define _IFIELD_BD(x)	((0x00008000 & x) ? (0xffff0000 | (x & ~3)) : \
                                            (0x0000fffc & x))
                                                    /* ^ signed branch displ */
#define _IFIELD_BI(x)   ((0x001f0000 & x) >> 16)    /* branch condition      */
#define _IFIELD_BO(x)   ((0x03e00000 & x) >> 21)    /* branch options        */
#define _IFIELD_CRBA(x) ((0x001f0000 & x) >> 16)    /* source CR bit         */
#define _IFIELD_CRBB(x) ((0x0000f800 & x) >> 11)    /* source CR bit         */
#define _IFIELD_CRBD(x) ((0x03e00000 & x) >> 21)    /* dest [FPS]CR bit      */
#define _IFIELD_CRFD(x) ((0x03800000 & x) >> 23)    /* dest [FPS]CR field    */
#define _IFIELD_CRFS(x) ((0x001c0000 & x) >> 18)    /* source [FPS]CR field  */
#define _IFIELD_CRM(x)  ((0x000ff000 & x) >> 12)    /* CR field mask         */
#define _IFIELD_D(x)    ((0x0000ffff & x))	    /* ^ signed 16b integer  */
#define _IFIELD_D_S(x)	((0x00008000 & x) ? (0xffff0000 | x) : \
					    (0x0000ffff & x))
						    /* ^ signed 16b integer  */
#define _IFIELD_FM(x)   ((0x01fe0000 & x) >> 17)    /* FPSCR field mask      */
#define _IFIELD_FRA(x)  ((0x001f0000 & x) >> 16)    /* source FPR            */
#define _IFIELD_FRB(x)  ((0x0000f800 & x) >> 11)    /* source FPR            */
#define _IFIELD_FRC(x)  ((0x000007c0 & x) >>  6)    /* source FPR            */
#define _IFIELD_FRD(x)  ((0x03e00000 & x) >> 21)    /* dest FPR              */
#define _IFIELD_FRS(x)  ((0x03e00000 & x) >> 21)    /* source FPR            */
#define _IFIELD_IMM(x)  ((0x0000f000 & x) >> 12)    /* data for FPSCR        */
#define _IFIELD_L(x)    ((0x00200000 & x) >> 21)    /* 64-bit flag (ignored) */
#define _IFIELD_LI(x)	((0x02000000 & x) ? ((0xf6000000 | x) & ~3) : \
                                            ((0x03fffffc & x) & ~3))
                                                    /* ^ signed 24b integer  */
#define _IFIELD_LK(x)   ((0x00000001 & x) >>  0)    /* link                  */
#define _IFIELD_MB(x)   ((0x000007c0 & x) >>  6)    /* mask begin (split)    */
#define _IFIELD_ME(x)   ((0x0000003e & x) >>  1)    /* mask end (split)      */
#define _IFIELD_NB(x)   ((0x0000f800 & x) >> 11)    /* number of bytes       */
#define _IFIELD_OE(x)   ((0x00000400 & x) >> 10)    /* enable setting OV, SO */
#define _IFIELD_OPCD(x) ((0xfc000000 & x) >> 26)    /* primary opcode        */
#define _IFIELD_RA(x)   ((0x001f0000 & x) >> 16)    /* source or dest GPR    */
#define _IFIELD_VA(x)   ((0x001f0000 & x) >> 16)    /* source VR    */

#define _IFIELD_RB(x)   ((0x0000f800 & x) >> 11)    /* source GPR            */
#define	_IFIELD_WS(x)	((0x0000f800 & x) >> 11)   /* 4xx tlbre/tlbwe WS fld */
#define _IFIELD_VB(x)   ((0x0000f800 & x) >> 11)    /* source VR             */
#define _IFIELD_VC(x)   ((0x000007c0 & x) >>  6)    /* source VR             */
#define _IFIELD_RC(x)   ((0x00000001 & x) >>  0)    /* record                */
#define _IFIELD_VRC(x)  ((0x00000400 & x) >> 10)    /* record                */
#define _IFIELD_RD(x)   ((0x03e00000 & x) >> 21)    /* dest GPR              */
#define _IFIELD_VD(x)   ((0x03e00000 & x) >> 21)    /* destination VR	     */
#define _IFIELD_RS(x)   ((0x03e00000 & x) >> 21)    /* source GPR            */
#define _IFIELD_VS(x)   ((0x03e00000 & x) >> 21)    /* source VR	     */
#define _IFIELD_SH(x)   ((0x0000f800 & x) >> 11)    /* shift amount (split)  */
#define _IFIELD_VSH(x)  ((0x000003c0 & x) >>  6)   /* shift amount for vector */
#define _IFIELD_SIMM(x) ((0x0000ffff & x))
#define _IFIELD_SIMM_S(x) ((0x00008000 & x) ? (0xffff0000 | x) : \
                                            (0x0000ffff & x))
                                                    /* ^ signed 16b integer  */
#define _IFIELD_VSIMM(x) ((0x00100000 & x) ? (0xffffffe0 | (x>>16)) : \
                                            (0x0000001f & (x>>16)))
                                                    /* ^ signed 16b integer  */
#define _IFIELD_SPR(x)  (((0x001f0000 & x) >> 16) |   \
                         ((0x0000f800 & x) >>  6))  /* mtspr, mfspr (split)  */
#define _IFIELD_SR(x)   ((0x000f0000 & x) >> 16)    /* segment register      */
#define _IFIELD_TBR(x)  ((0x001ff800 & x) >> 11)    /* TBL, TBU (split)      */
#define _IFIELD_TO(x)   ((0x03e00000 & x) >> 21)    /* trap conditions       */
#define _IFIELD_UIMM(x) ((0x0000ffff & x) >>  0)    /* unsigned 16b integer  */
#define _IFIELD_VUIMM(x) ((0x001f0000 & x)>> 16)    /* unsigned 5b integer   */

#define _IFIELD_XO_1(x) ((0x000007fe & x) >>  1)    /* extended op 1 (21-30) */
#define _IFIELD_XO_2(x) ((0x000003fe & x) >>  1)    /* extended op 2 (22-30) */
#define _IFIELD_XO_3(x) ((0x0000003e & x) >>  1)    /* extended op 3 (26-30) */

#define _IFIELD_STRM(x) ((0x00600000 & x) >> 21)    /* Data stream ID */

#define INST_B         	_OP(18, 0)
#define INST_BC        	_OP(16, 0)
#define INST_BCCTR      _OP(19, 528)
#define INST_BCLR      	_OP(19, 16)
#define INST_BL         (_OP(18, 0) | 1)
#define INST_BCL        (_OP(16, 0) | 1)
#define INST_B_MASK     0xfc000000
#define INST_BL_MASK    0xfc000001
#define INST_BCR_MASK	0xfc00fffe
#define INST_BCLRL	0x4c000021
#define INST_BCLRL_MASK	0xfc00ffff

#define INST_MFLR       0x7c0802a6
#define INST_MTLR       0x7c0803a6
#define INST_ADDI_SP    0x38210000
#define INST_STWU_SP    0x94210000
#define INST_LWZ_SP     0x80010000
#define INST_LWZU_SP    0x84010000

#define INST_MTSPR_MASK 0xfc1fffff
#define INST_HIGH_MASK  0xffff0000
#define INST_BCLR_MASK  0xfc00ffff

#ifdef	HOST
/* function declarations */

extern int dsmPpcInstGet
    (
    UINT32 *	binInst,		/* Pointer to the instruction        */
    int		endian,			/* endianness of data in buffer      */
    TGT_ADDR_T	address, 		/* Address prepended to instruction  */
    VOIDFUNCPTR	prtAddress,		/* printing function address         */
    char *	pString,		/* where to write disassembled inst. */
    BOOL32	printAddr,		/* if address has to be appened      */
    BOOL32	printOpcodes		/* if opcodes have to be appened     */
    );

extern int dsmPpcInstSizeGet
    (
    UINT32 *	binInst,	/* pointer to the instruction                */
    int		endian		/* endianness of data in buffer              */
    );
#endif	/* HOST */

#ifdef __cplusplus
}
#endif

#endif /* __INCdsmPpch */
