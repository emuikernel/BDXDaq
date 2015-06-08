/* dsmSimsparcLib.h - simsparc disassembler library header */

/* Copyright 1984-1995 Wind River Systems, Inc. */
/*
modification history
--------------------
01a,07jun95,ism  derived from simsparc
*/

#ifndef __INCdsmSimsparcLibh
#define __INCdsmSimsparcLibh

#ifdef __cplusplus
extern "C" {
#endif

#include "vwModNum.h"


/* dsmLib status codes */

#define S_dsmLib_UNKNOWN_INSTRUCTION	(M_dsmLib | 1)


/* instruction fields */

/* instruction fields: opcode masks */

#define	OP		0xC0000000	/* general opcode */
#define	OP2		0x01C00000	/* op2 opcode */
#define	OP3		0x01F80000	/* op3 opcode */
#define	OPFC		0x00003FE0	/* floating point or other coprocessor opcode */

/* instruction fields: register masks */

#define	RD		0x3E000000	/* destination register */
#define	RS1		0x0007C000	/* source register 1 */
#define	RS2		0x0000001F	/* source register 2 */

/* instruction fields: constant masks */

#define	DISP30		0x3FFFFFFF	/* 30-bit sign-extended word displacement */
#define	DISP22		0x003FFFFF	/* 22-bit sign-extended word displacement */
#define	DISP22_SIGN	0x00200000	/* 22-bit sign */
#define	CONST22		0x003FFFFF	/* 22-bit constant value (structure size) */
#define	IMM22		0x003FFFFF	/* 22-bit immediate value (result of %hi) */
#define	SIMM13		0x00001FFF	/* 13-bit sign-extended immediate value */
#define	SIMM13_SIGN	0x00001000	/* 13-bit sign */
#define	LOBITS		0x000003FF	/* 10-bit immediate value (result of %lo) */
#define	TRAP_NUMBER	0x0000007F	/* trap number */

/* instruction fields: miscellaneous masks */

#define	A		0x20000000	/* annul bit */
#define	COND		0x1E000000	/* test condition */
#define	I		0x00002000	/* selects type of second ALU operand */
#define	ASI		0x00001FE0	/* alternate address space indicator */
#define	SHCNT		0x0000001F	/* shift count */

/* instruction fields: opcode, register, constant, and miscellaneous shift counts */

#define	DISP30_SHIFT_CT	 2		/* 30-bit sign-extended word displacement */
#define	DISP22_SHIFT_CT	 2		/* 22-bit sign-extended word displacement */
#define	IMM22_SHIFT_CT	10		/* 22-bit immediate value */

#define	OP_SHIFT_CT	30		/* general opcode */
#define	A_SHIFT_CT	29		/* annul bit */
#define	RD_SHIFT_CT	25		/* destination register */
#define	COND_SHIFT_CT	25		/* test condition */
#define	OP2_SHIFT_CT	22		/* op2 opcode */
#define	OP3_SHIFT_CT	19		/* op3 opcode */
#define	RS1_SHIFT_CT	14		/* source register 1 */
#define	I_SHIFT_CT	13		/* I field */
#define	OPFC_SHIFT_CT	 5		/* coprocessor opcode */
#define	ASI_SHIFT_CT	 5		/* alternate address space indicator */

/* instruction fields: things that don't fit under other categories */

#define	DISP22_SIGN_EXTEND	0xFFC00000	/* 22-bit sign-extended word displacement */
#define	SIMM13_SIGN_EXTEND	0xFFFFE000	/* 13-bit sign-extended immediate value */

/* instruction fields: opcode equates */

#define	OP_3		0xC0000000	/* general opcode: load/store */
#define	OP_2		0x80000000	/* general opcode: arithmetic */
#define	OP_1		0x40000000	/* general opcode: Call */
#define	OP_0		0x00000000	/* general opcode: Bicc, FBfcc, CBccc, SETHI */
#define	OP2_7		0x01C00000	/* op2 opcode: CBccc */
#define	OP2_6		0x01800000	/* op2 opcode: FBfcc */
#define	OP2_5		0x01400000	/* op2 opcode: (n/i) */
#define	OP2_4		0x01000000	/* op2 opcode: SETHI */
#define	OP2_3		0x00C00000	/* op2 opcode: (n/i) */
#define	OP2_2		0x00800000	/* op2 opcode: Bicc */
#define	OP2_1		0x00400000	/* op2 opcode: (n/i) */
#define	OP2_0		0x00000000	/* op2 opcode: UNIMPlemented */
#define	OP3_3F		0x01F80000	/* op3 opcode */
#define	OP3_3E		0x01F00000	/* op3 opcode */
#define	OP3_3D		0x01E80000	/* op3 opcode */
#define	OP3_3C		0x01E00000	/* op3 opcode */
#define	OP3_3B		0x01D80000	/* op3 opcode */
#define	OP3_3A		0x01D00000	/* op3 opcode */
#define	OP3_39		0x01C80000	/* op3 opcode */
#define	OP3_38		0x01C00000	/* op3 opcode */
#define	OP3_37		0x01B80000	/* op3 opcode */
#define	OP3_36		0x01B00000	/* op3 opcode */
#define	OP3_35		0x01A80000	/* op3 opcode */
#define	OP3_34		0x01A00000	/* op3 opcode */
#define	OP3_33		0x01980000	/* op3 opcode */
#define	OP3_32		0x01900000	/* op3 opcode */
#define	OP3_31		0x01880000	/* op3 opcode */
#define	OP3_30		0x01800000	/* op3 opcode */
#define	OP3_2F		0x01780000	/* op3 opcode */
#define	OP3_2E		0x01700000	/* op3 opcode */
#define	OP3_2D		0x01680000	/* op3 opcode */
#define	OP3_2C		0x01600000	/* op3 opcode */
#define	OP3_2B		0x01580000	/* op3 opcode */
#define	OP3_2A		0x01500000	/* op3 opcode */
#define	OP3_29		0x01480000	/* op3 opcode */
#define	OP3_28		0x01400000	/* op3 opcode */
#define	OP3_27		0x01380000	/* op3 opcode */
#define	OP3_26		0x01300000	/* op3 opcode */
#define	OP3_25		0x01280000	/* op3 opcode */
#define	OP3_24		0x01200000	/* op3 opcode */
#define	OP3_23		0x01180000	/* op3 opcode */
#define	OP3_22		0x01100000	/* op3 opcode */
#define	OP3_21		0x01080000	/* op3 opcode */
#define	OP3_20		0x01000000	/* op3 opcode */
#define	OP3_1F		0x00F80000	/* op3 opcode */
#define	OP3_1E		0x00F00000	/* op3 opcode */
#define	OP3_1D		0x00E80000	/* op3 opcode */
#define	OP3_1C		0x00E00000	/* op3 opcode */
#define	OP3_1B		0x00D80000	/* op3 opcode */
#define	OP3_1A		0x00D00000	/* op3 opcode */
#define	OP3_19		0x00C80000	/* op3 opcode */
#define	OP3_18		0x00C00000	/* op3 opcode */
#define	OP3_17		0x00B80000	/* op3 opcode */
#define	OP3_16		0x00B00000	/* op3 opcode */
#define	OP3_15		0x00A80000	/* op3 opcode */
#define	OP3_14		0x00A00000	/* op3 opcode */
#define	OP3_13		0x00980000	/* op3 opcode */
#define	OP3_12		0x00900000	/* op3 opcode */
#define	OP3_11		0x00880000	/* op3 opcode */
#define	OP3_10		0x00800000	/* op3 opcode */
#define	OP3_0F		0x00780000	/* op3 opcode */
#define	OP3_0E		0x00700000	/* op3 opcode */
#define	OP3_0D		0x00680000	/* op3 opcode */
#define	OP3_0C		0x00600000	/* op3 opcode */
#define	OP3_0B		0x00580000	/* op3 opcode */
#define	OP3_0A		0x00500000	/* op3 opcode */
#define	OP3_09		0x00480000	/* op3 opcode */
#define	OP3_08		0x00400000	/* op3 opcode */
#define	OP3_07		0x00380000	/* op3 opcode */
#define	OP3_06		0x00300000	/* op3 opcode */
#define	OP3_05		0x00280000	/* op3 opcode */
#define	OP3_04		0x00200000	/* op3 opcode */
#define	OP3_03		0x00180000	/* op3 opcode */
#define	OP3_02		0x00100000	/* op3 opcode */
#define	OP3_01		0x00080000	/* op3 opcode */
#define	OP3_00		0x00000000	/* op3 opcode */
#define	OPFC_1FF	0x00003FE0	/* floating point or coprocessor opcode */
#define	OPFC_1FE	0x00003FC0	/* floating point or coprocessor opcode */
#define	OPFC_1FD	0x00003FA0	/* floating point or coprocessor opcode */
#define	OPFC_1FC	0x00003F80	/* floating point or coprocessor opcode */
#define	OPFC_1FB	0x00003F60	/* floating point or coprocessor opcode */
#define	OPFC_1FA	0x00003F40	/* floating point or coprocessor opcode */
#define	OPFC_1F9	0x00003F20	/* floating point or coprocessor opcode */
#define	OPFC_1F8	0x00003F00	/* floating point or coprocessor opcode */
#define	OPFC_1F7	0x00003EE0	/* floating point or coprocessor opcode */
#define	OPFC_1F6	0x00003EC0	/* floating point or coprocessor opcode */
#define	OPFC_1F5	0x00003EA0	/* floating point or coprocessor opcode */
#define	OPFC_1F4	0x00003E80	/* floating point or coprocessor opcode */
#define	OPFC_1F3	0x00003E60	/* floating point or coprocessor opcode */
#define	OPFC_1F2	0x00003E40	/* floating point or coprocessor opcode */
#define	OPFC_1F1	0x00003E20	/* floating point or coprocessor opcode */
#define	OPFC_1F0	0x00003E00	/* floating point or coprocessor opcode */
#define	OPFC_1EF	0x00003DE0	/* floating point or coprocessor opcode */
#define	OPFC_1EE	0x00003DC0	/* floating point or coprocessor opcode */
#define	OPFC_1ED	0x00003DA0	/* floating point or coprocessor opcode */
#define	OPFC_1EC	0x00003D80	/* floating point or coprocessor opcode */
#define	OPFC_1EB	0x00003D60	/* floating point or coprocessor opcode */
#define	OPFC_1EA	0x00003D40	/* floating point or coprocessor opcode */
#define	OPFC_1E9	0x00003D20	/* floating point or coprocessor opcode */
#define	OPFC_1E8	0x00003D00	/* floating point or coprocessor opcode */
#define	OPFC_1E7	0x00003CE0	/* floating point or coprocessor opcode */
#define	OPFC_1E6	0x00003CC0	/* floating point or coprocessor opcode */
#define	OPFC_1E5	0x00003CA0	/* floating point or coprocessor opcode */
#define	OPFC_1E4	0x00003C80	/* floating point or coprocessor opcode */
#define	OPFC_1E3	0x00003C60	/* floating point or coprocessor opcode */
#define	OPFC_1E2	0x00003C40	/* floating point or coprocessor opcode */
#define	OPFC_1E1	0x00003C20	/* floating point or coprocessor opcode */
#define	OPFC_1E0	0x00003C00	/* floating point or coprocessor opcode */
#define	OPFC_1DF	0x00003BE0	/* floating point or coprocessor opcode */
#define	OPFC_1DE	0x00003BC0	/* floating point or coprocessor opcode */
#define	OPFC_1DD	0x00003BA0	/* floating point or coprocessor opcode */
#define	OPFC_1DC	0x00003B80	/* floating point or coprocessor opcode */
#define	OPFC_1DB	0x00003B60	/* floating point or coprocessor opcode */
#define	OPFC_1DA	0x00003B40	/* floating point or coprocessor opcode */
#define	OPFC_1D9	0x00003B20	/* floating point or coprocessor opcode */
#define	OPFC_1D8	0x00003B00	/* floating point or coprocessor opcode */
#define	OPFC_1D7	0x00003AE0	/* floating point or coprocessor opcode */
#define	OPFC_1D6	0x00003AC0	/* floating point or coprocessor opcode */
#define	OPFC_1D5	0x00003AA0	/* floating point or coprocessor opcode */
#define	OPFC_1D4	0x00003A80	/* floating point or coprocessor opcode */
#define	OPFC_1D3	0x00003A60	/* floating point or coprocessor opcode */
#define	OPFC_1D2	0x00003A40	/* floating point or coprocessor opcode */
#define	OPFC_1D1	0x00003A20	/* floating point or coprocessor opcode */
#define	OPFC_1D0	0x00003A00	/* floating point or coprocessor opcode */
#define	OPFC_1CF	0x000039E0	/* floating point or coprocessor opcode */
#define	OPFC_1CE	0x000039C0	/* floating point or coprocessor opcode */
#define	OPFC_1CD	0x000039A0	/* floating point or coprocessor opcode */
#define	OPFC_1CC	0x00003980	/* floating point or coprocessor opcode */
#define	OPFC_1CB	0x00003960	/* floating point or coprocessor opcode */
#define	OPFC_1CA	0x00003940	/* floating point or coprocessor opcode */
#define	OPFC_1C9	0x00003920	/* floating point or coprocessor opcode */
#define	OPFC_1C8	0x00003900	/* floating point or coprocessor opcode */
#define	OPFC_1C7	0x000038E0	/* floating point or coprocessor opcode */
#define	OPFC_1C6	0x000038C0	/* floating point or coprocessor opcode */
#define	OPFC_1C5	0x000038A0	/* floating point or coprocessor opcode */
#define	OPFC_1C4	0x00003880	/* floating point or coprocessor opcode */
#define	OPFC_1C3	0x00003860	/* floating point or coprocessor opcode */
#define	OPFC_1C2	0x00003840	/* floating point or coprocessor opcode */
#define	OPFC_1C1	0x00003820	/* floating point or coprocessor opcode */
#define	OPFC_1C0	0x00003800	/* floating point or coprocessor opcode */
#define	OPFC_1BF	0x000037E0	/* floating point or coprocessor opcode */
#define	OPFC_1BE	0x000037C0	/* floating point or coprocessor opcode */
#define	OPFC_1BD	0x000037A0	/* floating point or coprocessor opcode */
#define	OPFC_1BC	0x00003780	/* floating point or coprocessor opcode */
#define	OPFC_1BB	0x00003760	/* floating point or coprocessor opcode */
#define	OPFC_1BA	0x00003740	/* floating point or coprocessor opcode */
#define	OPFC_1B9	0x00003720	/* floating point or coprocessor opcode */
#define	OPFC_1B8	0x00003700	/* floating point or coprocessor opcode */
#define	OPFC_1B7	0x000036E0	/* floating point or coprocessor opcode */
#define	OPFC_1B6	0x000036C0	/* floating point or coprocessor opcode */
#define	OPFC_1B5	0x000036A0	/* floating point or coprocessor opcode */
#define	OPFC_1B4	0x00003680	/* floating point or coprocessor opcode */
#define	OPFC_1B3	0x00003660	/* floating point or coprocessor opcode */
#define	OPFC_1B2	0x00003640	/* floating point or coprocessor opcode */
#define	OPFC_1B1	0x00003620	/* floating point or coprocessor opcode */
#define	OPFC_1B0	0x00003600	/* floating point or coprocessor opcode */
#define	OPFC_1AF	0x000035E0	/* floating point or coprocessor opcode */
#define	OPFC_1AE	0x000035C0	/* floating point or coprocessor opcode */
#define	OPFC_1AD	0x000035A0	/* floating point or coprocessor opcode */
#define	OPFC_1AC	0x00003580	/* floating point or coprocessor opcode */
#define	OPFC_1AB	0x00003560	/* floating point or coprocessor opcode */
#define	OPFC_1AA	0x00003540	/* floating point or coprocessor opcode */
#define	OPFC_1A9	0x00003520	/* floating point or coprocessor opcode */
#define	OPFC_1A8	0x00003500	/* floating point or coprocessor opcode */
#define	OPFC_1A7	0x000034E0	/* floating point or coprocessor opcode */
#define	OPFC_1A6	0x000034C0	/* floating point or coprocessor opcode */
#define	OPFC_1A5	0x000034A0	/* floating point or coprocessor opcode */
#define	OPFC_1A4	0x00003480	/* floating point or coprocessor opcode */
#define	OPFC_1A3	0x00003460	/* floating point or coprocessor opcode */
#define	OPFC_1A2	0x00003440	/* floating point or coprocessor opcode */
#define	OPFC_1A1	0x00003420	/* floating point or coprocessor opcode */
#define	OPFC_1A0	0x00003400	/* floating point or coprocessor opcode */
#define	OPFC_19F	0x000033E0	/* floating point or coprocessor opcode */
#define	OPFC_19E	0x000033C0	/* floating point or coprocessor opcode */
#define	OPFC_19D	0x000033A0	/* floating point or coprocessor opcode */
#define	OPFC_19C	0x00003380	/* floating point or coprocessor opcode */
#define	OPFC_19B	0x00003360	/* floating point or coprocessor opcode */
#define	OPFC_19A	0x00003340	/* floating point or coprocessor opcode */
#define	OPFC_199	0x00003320	/* floating point or coprocessor opcode */
#define	OPFC_198	0x00003300	/* floating point or coprocessor opcode */
#define	OPFC_197	0x000032E0	/* floating point or coprocessor opcode */
#define	OPFC_196	0x000032C0	/* floating point or coprocessor opcode */
#define	OPFC_195	0x000032A0	/* floating point or coprocessor opcode */
#define	OPFC_194	0x00003280	/* floating point or coprocessor opcode */
#define	OPFC_193	0x00003260	/* floating point or coprocessor opcode */
#define	OPFC_192	0x00003240	/* floating point or coprocessor opcode */
#define	OPFC_191	0x00003220	/* floating point or coprocessor opcode */
#define	OPFC_190	0x00003200	/* floating point or coprocessor opcode */
#define	OPFC_18F	0x000031E0	/* floating point or coprocessor opcode */
#define	OPFC_18E	0x000031C0	/* floating point or coprocessor opcode */
#define	OPFC_18D	0x000031A0	/* floating point or coprocessor opcode */
#define	OPFC_18C	0x00003180	/* floating point or coprocessor opcode */
#define	OPFC_18B	0x00003160	/* floating point or coprocessor opcode */
#define	OPFC_18A	0x00003140	/* floating point or coprocessor opcode */
#define	OPFC_189	0x00003120	/* floating point or coprocessor opcode */
#define	OPFC_188	0x00003100	/* floating point or coprocessor opcode */
#define	OPFC_187	0x000030E0	/* floating point or coprocessor opcode */
#define	OPFC_186	0x000030C0	/* floating point or coprocessor opcode */
#define	OPFC_185	0x000030A0	/* floating point or coprocessor opcode */
#define	OPFC_184	0x00003080	/* floating point or coprocessor opcode */
#define	OPFC_183	0x00003060	/* floating point or coprocessor opcode */
#define	OPFC_182	0x00003040	/* floating point or coprocessor opcode */
#define	OPFC_181	0x00003020	/* floating point or coprocessor opcode */
#define	OPFC_180	0x00003000	/* floating point or coprocessor opcode */
#define	OPFC_17F	0x00002FE0	/* floating point or coprocessor opcode */
#define	OPFC_17E	0x00002FC0	/* floating point or coprocessor opcode */
#define	OPFC_17D	0x00002FA0	/* floating point or coprocessor opcode */
#define	OPFC_17C	0x00002F80	/* floating point or coprocessor opcode */
#define	OPFC_17B	0x00002F60	/* floating point or coprocessor opcode */
#define	OPFC_17A	0x00002F40	/* floating point or coprocessor opcode */
#define	OPFC_179	0x00002F20	/* floating point or coprocessor opcode */
#define	OPFC_178	0x00002F00	/* floating point or coprocessor opcode */
#define	OPFC_177	0x00002EE0	/* floating point or coprocessor opcode */
#define	OPFC_176	0x00002EC0	/* floating point or coprocessor opcode */
#define	OPFC_175	0x00002EA0	/* floating point or coprocessor opcode */
#define	OPFC_174	0x00002E80	/* floating point or coprocessor opcode */
#define	OPFC_173	0x00002E60	/* floating point or coprocessor opcode */
#define	OPFC_172	0x00002E40	/* floating point or coprocessor opcode */
#define	OPFC_171	0x00002E20	/* floating point or coprocessor opcode */
#define	OPFC_170	0x00002E00	/* floating point or coprocessor opcode */
#define	OPFC_16F	0x00002DE0	/* floating point or coprocessor opcode */
#define	OPFC_16E	0x00002DC0	/* floating point or coprocessor opcode */
#define	OPFC_16D	0x00002DA0	/* floating point or coprocessor opcode */
#define	OPFC_16C	0x00002D80	/* floating point or coprocessor opcode */
#define	OPFC_16B	0x00002D60	/* floating point or coprocessor opcode */
#define	OPFC_16A	0x00002D40	/* floating point or coprocessor opcode */
#define	OPFC_169	0x00002D20	/* floating point or coprocessor opcode */
#define	OPFC_168	0x00002D00	/* floating point or coprocessor opcode */
#define	OPFC_167	0x00002CE0	/* floating point or coprocessor opcode */
#define	OPFC_166	0x00002CC0	/* floating point or coprocessor opcode */
#define	OPFC_165	0x00002CA0	/* floating point or coprocessor opcode */
#define	OPFC_164	0x00002C80	/* floating point or coprocessor opcode */
#define	OPFC_163	0x00002C60	/* floating point or coprocessor opcode */
#define	OPFC_162	0x00002C40	/* floating point or coprocessor opcode */
#define	OPFC_161	0x00002C20	/* floating point or coprocessor opcode */
#define	OPFC_160	0x00002C00	/* floating point or coprocessor opcode */
#define	OPFC_15F	0x00002BE0	/* floating point or coprocessor opcode */
#define	OPFC_15E	0x00002BC0	/* floating point or coprocessor opcode */
#define	OPFC_15D	0x00002BA0	/* floating point or coprocessor opcode */
#define	OPFC_15C	0x00002B80	/* floating point or coprocessor opcode */
#define	OPFC_15B	0x00002B60	/* floating point or coprocessor opcode */
#define	OPFC_15A	0x00002B40	/* floating point or coprocessor opcode */
#define	OPFC_159	0x00002B20	/* floating point or coprocessor opcode */
#define	OPFC_158	0x00002B00	/* floating point or coprocessor opcode */
#define	OPFC_157	0x00002AE0	/* floating point or coprocessor opcode */
#define	OPFC_156	0x00002AC0	/* floating point or coprocessor opcode */
#define	OPFC_155	0x00002AA0	/* floating point or coprocessor opcode */
#define	OPFC_154	0x00002A80	/* floating point or coprocessor opcode */
#define	OPFC_153	0x00002A60	/* floating point or coprocessor opcode */
#define	OPFC_152	0x00002A40	/* floating point or coprocessor opcode */
#define	OPFC_151	0x00002A20	/* floating point or coprocessor opcode */
#define	OPFC_150	0x00002A00	/* floating point or coprocessor opcode */
#define	OPFC_14F	0x000029E0	/* floating point or coprocessor opcode */
#define	OPFC_14E	0x000029C0	/* floating point or coprocessor opcode */
#define	OPFC_14D	0x000029A0	/* floating point or coprocessor opcode */
#define	OPFC_14C	0x00002980	/* floating point or coprocessor opcode */
#define	OPFC_14B	0x00002960	/* floating point or coprocessor opcode */
#define	OPFC_14A	0x00002940	/* floating point or coprocessor opcode */
#define	OPFC_149	0x00002920	/* floating point or coprocessor opcode */
#define	OPFC_148	0x00002900	/* floating point or coprocessor opcode */
#define	OPFC_147	0x000028E0	/* floating point or coprocessor opcode */
#define	OPFC_146	0x000028C0	/* floating point or coprocessor opcode */
#define	OPFC_145	0x000028A0	/* floating point or coprocessor opcode */
#define	OPFC_144	0x00002880	/* floating point or coprocessor opcode */
#define	OPFC_143	0x00002860	/* floating point or coprocessor opcode */
#define	OPFC_142	0x00002840	/* floating point or coprocessor opcode */
#define	OPFC_141	0x00002820	/* floating point or coprocessor opcode */
#define	OPFC_140	0x00002800	/* floating point or coprocessor opcode */
#define	OPFC_13F	0x000027E0	/* floating point or coprocessor opcode */
#define	OPFC_13E	0x000027C0	/* floating point or coprocessor opcode */
#define	OPFC_13D	0x000027A0	/* floating point or coprocessor opcode */
#define	OPFC_13C	0x00002780	/* floating point or coprocessor opcode */
#define	OPFC_13B	0x00002760	/* floating point or coprocessor opcode */
#define	OPFC_13A	0x00002740	/* floating point or coprocessor opcode */
#define	OPFC_139	0x00002720	/* floating point or coprocessor opcode */
#define	OPFC_138	0x00002700	/* floating point or coprocessor opcode */
#define	OPFC_137	0x000026E0	/* floating point or coprocessor opcode */
#define	OPFC_136	0x000026C0	/* floating point or coprocessor opcode */
#define	OPFC_135	0x000026A0	/* floating point or coprocessor opcode */
#define	OPFC_134	0x00002680	/* floating point or coprocessor opcode */
#define	OPFC_133	0x00002660	/* floating point or coprocessor opcode */
#define	OPFC_132	0x00002640	/* floating point or coprocessor opcode */
#define	OPFC_131	0x00002620	/* floating point or coprocessor opcode */
#define	OPFC_130	0x00002600	/* floating point or coprocessor opcode */
#define	OPFC_12F	0x000025E0	/* floating point or coprocessor opcode */
#define	OPFC_12E	0x000025C0	/* floating point or coprocessor opcode */
#define	OPFC_12D	0x000025A0	/* floating point or coprocessor opcode */
#define	OPFC_12C	0x00002580	/* floating point or coprocessor opcode */
#define	OPFC_12B	0x00002560	/* floating point or coprocessor opcode */
#define	OPFC_12A	0x00002540	/* floating point or coprocessor opcode */
#define	OPFC_129	0x00002520	/* floating point or coprocessor opcode */
#define	OPFC_128	0x00002500	/* floating point or coprocessor opcode */
#define	OPFC_127	0x000024E0	/* floating point or coprocessor opcode */
#define	OPFC_126	0x000024C0	/* floating point or coprocessor opcode */
#define	OPFC_125	0x000024A0	/* floating point or coprocessor opcode */
#define	OPFC_124	0x00002480	/* floating point or coprocessor opcode */
#define	OPFC_123	0x00002460	/* floating point or coprocessor opcode */
#define	OPFC_122	0x00002440	/* floating point or coprocessor opcode */
#define	OPFC_121	0x00002420	/* floating point or coprocessor opcode */
#define	OPFC_120	0x00002400	/* floating point or coprocessor opcode */
#define	OPFC_11F	0x000023E0	/* floating point or coprocessor opcode */
#define	OPFC_11E	0x000023C0	/* floating point or coprocessor opcode */
#define	OPFC_11D	0x000023A0	/* floating point or coprocessor opcode */
#define	OPFC_11C	0x00002380	/* floating point or coprocessor opcode */
#define	OPFC_11B	0x00002360	/* floating point or coprocessor opcode */
#define	OPFC_11A	0x00002340	/* floating point or coprocessor opcode */
#define	OPFC_119	0x00002320	/* floating point or coprocessor opcode */
#define	OPFC_118	0x00002300	/* floating point or coprocessor opcode */
#define	OPFC_117	0x000022E0	/* floating point or coprocessor opcode */
#define	OPFC_116	0x000022C0	/* floating point or coprocessor opcode */
#define	OPFC_115	0x000022A0	/* floating point or coprocessor opcode */
#define	OPFC_114	0x00002280	/* floating point or coprocessor opcode */
#define	OPFC_113	0x00002260	/* floating point or coprocessor opcode */
#define	OPFC_112	0x00002240	/* floating point or coprocessor opcode */
#define	OPFC_111	0x00002220	/* floating point or coprocessor opcode */
#define	OPFC_110	0x00002200	/* floating point or coprocessor opcode */
#define	OPFC_10F	0x000021E0	/* floating point or coprocessor opcode */
#define	OPFC_10E	0x000021C0	/* floating point or coprocessor opcode */
#define	OPFC_10D	0x000021A0	/* floating point or coprocessor opcode */
#define	OPFC_10C	0x00002180	/* floating point or coprocessor opcode */
#define	OPFC_10B	0x00002160	/* floating point or coprocessor opcode */
#define	OPFC_10A	0x00002140	/* floating point or coprocessor opcode */
#define	OPFC_109	0x00002120	/* floating point or coprocessor opcode */
#define	OPFC_108	0x00002100	/* floating point or coprocessor opcode */
#define	OPFC_107	0x000020E0	/* floating point or coprocessor opcode */
#define	OPFC_106	0x000020C0	/* floating point or coprocessor opcode */
#define	OPFC_105	0x000020A0	/* floating point or coprocessor opcode */
#define	OPFC_104	0x00002080	/* floating point or coprocessor opcode */
#define	OPFC_103	0x00002060	/* floating point or coprocessor opcode */
#define	OPFC_102	0x00002040	/* floating point or coprocessor opcode */
#define	OPFC_101	0x00002020	/* floating point or coprocessor opcode */
#define	OPFC_100	0x00002000	/* floating point or coprocessor opcode */
#define	OPFC_0FF	0x00001FE0	/* floating point or coprocessor opcode */
#define	OPFC_0FE	0x00001FC0	/* floating point or coprocessor opcode */
#define	OPFC_0FD	0x00001FA0	/* floating point or coprocessor opcode */
#define	OPFC_0FC	0x00001F80	/* floating point or coprocessor opcode */
#define	OPFC_0FB	0x00001F60	/* floating point or coprocessor opcode */
#define	OPFC_0FA	0x00001F40	/* floating point or coprocessor opcode */
#define	OPFC_0F9	0x00001F20	/* floating point or coprocessor opcode */
#define	OPFC_0F8	0x00001F00	/* floating point or coprocessor opcode */
#define	OPFC_0F7	0x00001EE0	/* floating point or coprocessor opcode */
#define	OPFC_0F6	0x00001EC0	/* floating point or coprocessor opcode */
#define	OPFC_0F5	0x00001EA0	/* floating point or coprocessor opcode */
#define	OPFC_0F4	0x00001E80	/* floating point or coprocessor opcode */
#define	OPFC_0F3	0x00001E60	/* floating point or coprocessor opcode */
#define	OPFC_0F2	0x00001E40	/* floating point or coprocessor opcode */
#define	OPFC_0F1	0x00001E20	/* floating point or coprocessor opcode */
#define	OPFC_0F0	0x00001E00	/* floating point or coprocessor opcode */
#define	OPFC_0EF	0x00001DE0	/* floating point or coprocessor opcode */
#define	OPFC_0EE	0x00001DC0	/* floating point or coprocessor opcode */
#define	OPFC_0ED	0x00001DA0	/* floating point or coprocessor opcode */
#define	OPFC_0EC	0x00001D80	/* floating point or coprocessor opcode */
#define	OPFC_0EB	0x00001D60	/* floating point or coprocessor opcode */
#define	OPFC_0EA	0x00001D40	/* floating point or coprocessor opcode */
#define	OPFC_0E9	0x00001D20	/* floating point or coprocessor opcode */
#define	OPFC_0E8	0x00001D00	/* floating point or coprocessor opcode */
#define	OPFC_0E7	0x00001CE0	/* floating point or coprocessor opcode */
#define	OPFC_0E6	0x00001CC0	/* floating point or coprocessor opcode */
#define	OPFC_0E5	0x00001CA0	/* floating point or coprocessor opcode */
#define	OPFC_0E4	0x00001C80	/* floating point or coprocessor opcode */
#define	OPFC_0E3	0x00001C60	/* floating point or coprocessor opcode */
#define	OPFC_0E2	0x00001C40	/* floating point or coprocessor opcode */
#define	OPFC_0E1	0x00001C20	/* floating point or coprocessor opcode */
#define	OPFC_0E0	0x00001C00	/* floating point or coprocessor opcode */
#define	OPFC_0DF	0x00001BE0	/* floating point or coprocessor opcode */
#define	OPFC_0DE	0x00001BC0	/* floating point or coprocessor opcode */
#define	OPFC_0DD	0x00001BA0	/* floating point or coprocessor opcode */
#define	OPFC_0DC	0x00001B80	/* floating point or coprocessor opcode */
#define	OPFC_0DB	0x00001B60	/* floating point or coprocessor opcode */
#define	OPFC_0DA	0x00001B40	/* floating point or coprocessor opcode */
#define	OPFC_0D9	0x00001B20	/* floating point or coprocessor opcode */
#define	OPFC_0D8	0x00001B00	/* floating point or coprocessor opcode */
#define	OPFC_0D7	0x00001AE0	/* floating point or coprocessor opcode */
#define	OPFC_0D6	0x00001AC0	/* floating point or coprocessor opcode */
#define	OPFC_0D5	0x00001AA0	/* floating point or coprocessor opcode */
#define	OPFC_0D4	0x00001A80	/* floating point or coprocessor opcode */
#define	OPFC_0D3	0x00001A60	/* floating point or coprocessor opcode */
#define	OPFC_0D2	0x00001A40	/* floating point or coprocessor opcode */
#define	OPFC_0D1	0x00001A20	/* floating point or coprocessor opcode */
#define	OPFC_0D0	0x00001A00	/* floating point or coprocessor opcode */
#define	OPFC_0CF	0x000019E0	/* floating point or coprocessor opcode */
#define	OPFC_0CE	0x000019C0	/* floating point or coprocessor opcode */
#define	OPFC_0CD	0x000019A0	/* floating point or coprocessor opcode */
#define	OPFC_0CC	0x00001980	/* floating point or coprocessor opcode */
#define	OPFC_0CB	0x00001960	/* floating point or coprocessor opcode */
#define	OPFC_0CA	0x00001940	/* floating point or coprocessor opcode */
#define	OPFC_0C9	0x00001920	/* floating point or coprocessor opcode */
#define	OPFC_0C8	0x00001900	/* floating point or coprocessor opcode */
#define	OPFC_0C7	0x000018E0	/* floating point or coprocessor opcode */
#define	OPFC_0C6	0x000018C0	/* floating point or coprocessor opcode */
#define	OPFC_0C5	0x000018A0	/* floating point or coprocessor opcode */
#define	OPFC_0C4	0x00001880	/* floating point or coprocessor opcode */
#define	OPFC_0C3	0x00001860	/* floating point or coprocessor opcode */
#define	OPFC_0C2	0x00001840	/* floating point or coprocessor opcode */
#define	OPFC_0C1	0x00001820	/* floating point or coprocessor opcode */
#define	OPFC_0C0	0x00001800	/* floating point or coprocessor opcode */
#define	OPFC_0BF	0x000017E0	/* floating point or coprocessor opcode */
#define	OPFC_0BE	0x000017C0	/* floating point or coprocessor opcode */
#define	OPFC_0BD	0x000017A0	/* floating point or coprocessor opcode */
#define	OPFC_0BC	0x00001780	/* floating point or coprocessor opcode */
#define	OPFC_0BB	0x00001760	/* floating point or coprocessor opcode */
#define	OPFC_0BA	0x00001740	/* floating point or coprocessor opcode */
#define	OPFC_0B9	0x00001720	/* floating point or coprocessor opcode */
#define	OPFC_0B8	0x00001700	/* floating point or coprocessor opcode */
#define	OPFC_0B7	0x000016E0	/* floating point or coprocessor opcode */
#define	OPFC_0B6	0x000016C0	/* floating point or coprocessor opcode */
#define	OPFC_0B5	0x000016A0	/* floating point or coprocessor opcode */
#define	OPFC_0B4	0x00001680	/* floating point or coprocessor opcode */
#define	OPFC_0B3	0x00001660	/* floating point or coprocessor opcode */
#define	OPFC_0B2	0x00001640	/* floating point or coprocessor opcode */
#define	OPFC_0B1	0x00001620	/* floating point or coprocessor opcode */
#define	OPFC_0B0	0x00001600	/* floating point or coprocessor opcode */
#define	OPFC_0AF	0x000015E0	/* floating point or coprocessor opcode */
#define	OPFC_0AE	0x000015C0	/* floating point or coprocessor opcode */
#define	OPFC_0AD	0x000015A0	/* floating point or coprocessor opcode */
#define	OPFC_0AC	0x00001580	/* floating point or coprocessor opcode */
#define	OPFC_0AB	0x00001560	/* floating point or coprocessor opcode */
#define	OPFC_0AA	0x00001540	/* floating point or coprocessor opcode */
#define	OPFC_0A9	0x00001520	/* floating point or coprocessor opcode */
#define	OPFC_0A8	0x00001500	/* floating point or coprocessor opcode */
#define	OPFC_0A7	0x000014E0	/* floating point or coprocessor opcode */
#define	OPFC_0A6	0x000014C0	/* floating point or coprocessor opcode */
#define	OPFC_0A5	0x000014A0	/* floating point or coprocessor opcode */
#define	OPFC_0A4	0x00001480	/* floating point or coprocessor opcode */
#define	OPFC_0A3	0x00001460	/* floating point or coprocessor opcode */
#define	OPFC_0A2	0x00001440	/* floating point or coprocessor opcode */
#define	OPFC_0A1	0x00001420	/* floating point or coprocessor opcode */
#define	OPFC_0A0	0x00001400	/* floating point or coprocessor opcode */
#define	OPFC_09F	0x000013E0	/* floating point or coprocessor opcode */
#define	OPFC_09E	0x000013C0	/* floating point or coprocessor opcode */
#define	OPFC_09D	0x000013A0	/* floating point or coprocessor opcode */
#define	OPFC_09C	0x00001380	/* floating point or coprocessor opcode */
#define	OPFC_09B	0x00001360	/* floating point or coprocessor opcode */
#define	OPFC_09A	0x00001340	/* floating point or coprocessor opcode */
#define	OPFC_099	0x00001320	/* floating point or coprocessor opcode */
#define	OPFC_098	0x00001300	/* floating point or coprocessor opcode */
#define	OPFC_097	0x000012E0	/* floating point or coprocessor opcode */
#define	OPFC_096	0x000012C0	/* floating point or coprocessor opcode */
#define	OPFC_095	0x000012A0	/* floating point or coprocessor opcode */
#define	OPFC_094	0x00001280	/* floating point or coprocessor opcode */
#define	OPFC_093	0x00001260	/* floating point or coprocessor opcode */
#define	OPFC_092	0x00001240	/* floating point or coprocessor opcode */
#define	OPFC_091	0x00001220	/* floating point or coprocessor opcode */
#define	OPFC_090	0x00001200	/* floating point or coprocessor opcode */
#define	OPFC_08F	0x000011E0	/* floating point or coprocessor opcode */
#define	OPFC_08E	0x000011C0	/* floating point or coprocessor opcode */
#define	OPFC_08D	0x000011A0	/* floating point or coprocessor opcode */
#define	OPFC_08C	0x00001180	/* floating point or coprocessor opcode */
#define	OPFC_08B	0x00001160	/* floating point or coprocessor opcode */
#define	OPFC_08A	0x00001140	/* floating point or coprocessor opcode */
#define	OPFC_089	0x00001120	/* floating point or coprocessor opcode */
#define	OPFC_088	0x00001100	/* floating point or coprocessor opcode */
#define	OPFC_087	0x000010E0	/* floating point or coprocessor opcode */
#define	OPFC_086	0x000010C0	/* floating point or coprocessor opcode */
#define	OPFC_085	0x000010A0	/* floating point or coprocessor opcode */
#define	OPFC_084	0x00001080	/* floating point or coprocessor opcode */
#define	OPFC_083	0x00001060	/* floating point or coprocessor opcode */
#define	OPFC_082	0x00001040	/* floating point or coprocessor opcode */
#define	OPFC_081	0x00001020	/* floating point or coprocessor opcode */
#define	OPFC_080	0x00001000	/* floating point or coprocessor opcode */
#define	OPFC_07F	0x00000FE0	/* floating point or coprocessor opcode */
#define	OPFC_07E	0x00000FC0	/* floating point or coprocessor opcode */
#define	OPFC_07D	0x00000FA0	/* floating point or coprocessor opcode */
#define	OPFC_07C	0x00000F80	/* floating point or coprocessor opcode */
#define	OPFC_07B	0x00000F60	/* floating point or coprocessor opcode */
#define	OPFC_07A	0x00000F40	/* floating point or coprocessor opcode */
#define	OPFC_079	0x00000F20	/* floating point or coprocessor opcode */
#define	OPFC_078	0x00000F00	/* floating point or coprocessor opcode */
#define	OPFC_077	0x00000EE0	/* floating point or coprocessor opcode */
#define	OPFC_076	0x00000EC0	/* floating point or coprocessor opcode */
#define	OPFC_075	0x00000EA0	/* floating point or coprocessor opcode */
#define	OPFC_074	0x00000E80	/* floating point or coprocessor opcode */
#define	OPFC_073	0x00000E60	/* floating point or coprocessor opcode */
#define	OPFC_072	0x00000E40	/* floating point or coprocessor opcode */
#define	OPFC_071	0x00000E20	/* floating point or coprocessor opcode */
#define	OPFC_070	0x00000E00	/* floating point or coprocessor opcode */
#define	OPFC_06F	0x00000DE0	/* floating point or coprocessor opcode */
#define	OPFC_06E	0x00000DC0	/* floating point or coprocessor opcode */
#define	OPFC_06D	0x00000DA0	/* floating point or coprocessor opcode */
#define	OPFC_06C	0x00000D80	/* floating point or coprocessor opcode */
#define	OPFC_06B	0x00000D60	/* floating point or coprocessor opcode */
#define	OPFC_06A	0x00000D40	/* floating point or coprocessor opcode */
#define	OPFC_069	0x00000D20	/* floating point or coprocessor opcode */
#define	OPFC_068	0x00000D00	/* floating point or coprocessor opcode */
#define	OPFC_067	0x00000CE0	/* floating point or coprocessor opcode */
#define	OPFC_066	0x00000CC0	/* floating point or coprocessor opcode */
#define	OPFC_065	0x00000CA0	/* floating point or coprocessor opcode */
#define	OPFC_064	0x00000C80	/* floating point or coprocessor opcode */
#define	OPFC_063	0x00000C60	/* floating point or coprocessor opcode */
#define	OPFC_062	0x00000C40	/* floating point or coprocessor opcode */
#define	OPFC_061	0x00000C20	/* floating point or coprocessor opcode */
#define	OPFC_060	0x00000C00	/* floating point or coprocessor opcode */
#define	OPFC_05F	0x00000BE0	/* floating point or coprocessor opcode */
#define	OPFC_05E	0x00000BC0	/* floating point or coprocessor opcode */
#define	OPFC_05D	0x00000BA0	/* floating point or coprocessor opcode */
#define	OPFC_05C	0x00000B80	/* floating point or coprocessor opcode */
#define	OPFC_05B	0x00000B60	/* floating point or coprocessor opcode */
#define	OPFC_05A	0x00000B40	/* floating point or coprocessor opcode */
#define	OPFC_059	0x00000B20	/* floating point or coprocessor opcode */
#define	OPFC_058	0x00000B00	/* floating point or coprocessor opcode */
#define	OPFC_057	0x00000AE0	/* floating point or coprocessor opcode */
#define	OPFC_056	0x00000AC0	/* floating point or coprocessor opcode */
#define	OPFC_055	0x00000AA0	/* floating point or coprocessor opcode */
#define	OPFC_054	0x00000A80	/* floating point or coprocessor opcode */
#define	OPFC_053	0x00000A60	/* floating point or coprocessor opcode */
#define	OPFC_052	0x00000A40	/* floating point or coprocessor opcode */
#define	OPFC_051	0x00000A20	/* floating point or coprocessor opcode */
#define	OPFC_050	0x00000A00	/* floating point or coprocessor opcode */
#define	OPFC_04F	0x000009E0	/* floating point or coprocessor opcode */
#define	OPFC_04E	0x000009C0	/* floating point or coprocessor opcode */
#define	OPFC_04D	0x000009A0	/* floating point or coprocessor opcode */
#define	OPFC_04C	0x00000980	/* floating point or coprocessor opcode */
#define	OPFC_04B	0x00000960	/* floating point or coprocessor opcode */
#define	OPFC_04A	0x00000940	/* floating point or coprocessor opcode */
#define	OPFC_049	0x00000920	/* floating point or coprocessor opcode */
#define	OPFC_048	0x00000900	/* floating point or coprocessor opcode */
#define	OPFC_047	0x000008E0	/* floating point or coprocessor opcode */
#define	OPFC_046	0x000008C0	/* floating point or coprocessor opcode */
#define	OPFC_045	0x000008A0	/* floating point or coprocessor opcode */
#define	OPFC_044	0x00000880	/* floating point or coprocessor opcode */
#define	OPFC_043	0x00000860	/* floating point or coprocessor opcode */
#define	OPFC_042	0x00000840	/* floating point or coprocessor opcode */
#define	OPFC_041	0x00000820	/* floating point or coprocessor opcode */
#define	OPFC_040	0x00000800	/* floating point or coprocessor opcode */
#define	OPFC_03F	0x000007E0	/* floating point or coprocessor opcode */
#define	OPFC_03E	0x000007C0	/* floating point or coprocessor opcode */
#define	OPFC_03D	0x000007A0	/* floating point or coprocessor opcode */
#define	OPFC_03C	0x00000780	/* floating point or coprocessor opcode */
#define	OPFC_03B	0x00000760	/* floating point or coprocessor opcode */
#define	OPFC_03A	0x00000740	/* floating point or coprocessor opcode */
#define	OPFC_039	0x00000720	/* floating point or coprocessor opcode */
#define	OPFC_038	0x00000700	/* floating point or coprocessor opcode */
#define	OPFC_037	0x000006E0	/* floating point or coprocessor opcode */
#define	OPFC_036	0x000006C0	/* floating point or coprocessor opcode */
#define	OPFC_035	0x000006A0	/* floating point or coprocessor opcode */
#define	OPFC_034	0x00000680	/* floating point or coprocessor opcode */
#define	OPFC_033	0x00000660	/* floating point or coprocessor opcode */
#define	OPFC_032	0x00000640	/* floating point or coprocessor opcode */
#define	OPFC_031	0x00000620	/* floating point or coprocessor opcode */
#define	OPFC_030	0x00000600	/* floating point or coprocessor opcode */
#define	OPFC_02F	0x000005E0	/* floating point or coprocessor opcode */
#define	OPFC_02E	0x000005C0	/* floating point or coprocessor opcode */
#define	OPFC_02D	0x000005A0	/* floating point or coprocessor opcode */
#define	OPFC_02C	0x00000580	/* floating point or coprocessor opcode */
#define	OPFC_02B	0x00000560	/* floating point or coprocessor opcode */
#define	OPFC_02A	0x00000540	/* floating point or coprocessor opcode */
#define	OPFC_029	0x00000520	/* floating point or coprocessor opcode */
#define	OPFC_028	0x00000500	/* floating point or coprocessor opcode */
#define	OPFC_027	0x000004E0	/* floating point or coprocessor opcode */
#define	OPFC_026	0x000004C0	/* floating point or coprocessor opcode */
#define	OPFC_025	0x000004A0	/* floating point or coprocessor opcode */
#define	OPFC_024	0x00000480	/* floating point or coprocessor opcode */
#define	OPFC_023	0x00000460	/* floating point or coprocessor opcode */
#define	OPFC_022	0x00000440	/* floating point or coprocessor opcode */
#define	OPFC_021	0x00000420	/* floating point or coprocessor opcode */
#define	OPFC_020	0x00000400	/* floating point or coprocessor opcode */
#define	OPFC_01F	0x000003E0	/* floating point or coprocessor opcode */
#define	OPFC_01E	0x000003C0	/* floating point or coprocessor opcode */
#define	OPFC_01D	0x000003A0	/* floating point or coprocessor opcode */
#define	OPFC_01C	0x00000380	/* floating point or coprocessor opcode */
#define	OPFC_01B	0x00000360	/* floating point or coprocessor opcode */
#define	OPFC_01A	0x00000340	/* floating point or coprocessor opcode */
#define	OPFC_019	0x00000320	/* floating point or coprocessor opcode */
#define	OPFC_018	0x00000300	/* floating point or coprocessor opcode */
#define	OPFC_017	0x000002E0	/* floating point or coprocessor opcode */
#define	OPFC_016	0x000002C0	/* floating point or coprocessor opcode */
#define	OPFC_015	0x000002A0	/* floating point or coprocessor opcode */
#define	OPFC_014	0x00000280	/* floating point or coprocessor opcode */
#define	OPFC_013	0x00000260	/* floating point or coprocessor opcode */
#define	OPFC_012	0x00000240	/* floating point or coprocessor opcode */
#define	OPFC_011	0x00000220	/* floating point or coprocessor opcode */
#define	OPFC_010	0x00000200	/* floating point or coprocessor opcode */
#define	OPFC_00F	0x000001E0	/* floating point or coprocessor opcode */
#define	OPFC_00E	0x000001C0	/* floating point or coprocessor opcode */
#define	OPFC_00D	0x000001A0	/* floating point or coprocessor opcode */
#define	OPFC_00C	0x00000180	/* floating point or coprocessor opcode */
#define	OPFC_00B	0x00000160	/* floating point or coprocessor opcode */
#define	OPFC_00A	0x00000140	/* floating point or coprocessor opcode */
#define	OPFC_009	0x00000120	/* floating point or coprocessor opcode */
#define	OPFC_008	0x00000100	/* floating point or coprocessor opcode */
#define	OPFC_007	0x000000E0	/* floating point or coprocessor opcode */
#define	OPFC_006	0x000000C0	/* floating point or coprocessor opcode */
#define	OPFC_005	0x000000A0	/* floating point or coprocessor opcode */
#define	OPFC_004	0x00000080	/* floating point or coprocessor opcode */
#define	OPFC_003	0x00000060	/* floating point or coprocessor opcode */
#define	OPFC_002	0x00000040	/* floating point or coprocessor opcode */
#define	OPFC_001	0x00000020	/* floating point or coprocessor opcode */
#define	OPFC_000	0x00000000	/* floating point or coprocessor opcode */

/* instruction fields: integer unit register equates */

#define	RD_i7		0x3E000000	/* destination register: %i7 */
#define	RD_i6		0x3C000000	/* destination register: %i6 */
#define	RD_i5		0x3A000000	/* destination register: %i5 */
#define	RD_i4		0x38000000	/* destination register: %i4 */
#define	RD_i3		0x36000000	/* destination register: %i3 */
#define	RD_i2		0x34000000	/* destination register: %i2 */
#define	RD_i1		0x32000000	/* destination register: %i1 */
#define	RD_i0		0x30000000	/* destination register: %i0 */
#define	RD_l7		0x2E000000	/* destination register: %l7 */
#define	RD_l6		0x2C000000	/* destination register: %l6 */
#define	RD_l5		0x2A000000	/* destination register: %l5 */
#define	RD_l4		0x28000000	/* destination register: %l4 */
#define	RD_l3		0x26000000	/* destination register: %l3 */
#define	RD_l2		0x24000000	/* destination register: %l2 */
#define	RD_l1		0x22000000	/* destination register: %l1 */
#define	RD_l0		0x20000000	/* destination register: %l0 */
#define	RD_o7		0x1E000000	/* destination register: %o7 */
#define	RD_o6		0x1C000000	/* destination register: %o6 */
#define	RD_o5		0x1A000000	/* destination register: %o5 */
#define	RD_o4		0x18000000	/* destination register: %o4 */
#define	RD_o3		0x16000000	/* destination register: %o3 */
#define	RD_o2		0x14000000	/* destination register: %o2 */
#define	RD_o1		0x12000000	/* destination register: %o1 */
#define	RD_o0		0x10000000	/* destination register: %o0 */
#define	RD_g7		0x0E000000	/* destination register: %g7 */
#define	RD_g6		0x0C000000	/* destination register: %g6 */
#define	RD_g5		0x0A000000	/* destination register: %g5 */
#define	RD_g4		0x08000000	/* destination register: %g4 */
#define	RD_g3		0x06000000	/* destination register: %g3 */
#define	RD_g2		0x04000000	/* destination register: %g2 */
#define	RD_g1		0x02000000	/* destination register: %g1 */
#define	RD_g0		0x00000000	/* destination register: %g0 */
#define	RS1_i7		0x0007C000	/* source register 1: %i7 */
#define	RS1_i6		0x00078000	/* source register 1: %i6 */
#define	RS1_i5		0x00074000	/* source register 1: %i5 */
#define	RS1_i4		0x00070000	/* source register 1: %i4 */
#define	RS1_i3		0x0006C000	/* source register 1: %i3 */
#define	RS1_i2		0x00068000	/* source register 1: %i2 */
#define	RS1_i1		0x00064000	/* source register 1: %i1 */
#define	RS1_i0		0x00060000	/* source register 1: %i0 */
#define	RS1_l7		0x0005C000	/* source register 1: %l7 */
#define	RS1_l6		0x00058000	/* source register 1: %l6 */
#define	RS1_l5		0x00054000	/* source register 1: %l5 */
#define	RS1_l4		0x00050000	/* source register 1: %l4 */
#define	RS1_l3		0x0004C000	/* source register 1: %l3 */
#define	RS1_l2		0x00048000	/* source register 1: %l2 */
#define	RS1_l1		0x00044000	/* source register 1: %l1 */
#define	RS1_l0		0x00040000	/* source register 1: %l0 */
#define	RS1_o7		0x0003C000	/* source register 1: %o7 */
#define	RS1_o6		0x00038000	/* source register 1: %o6 */
#define	RS1_o5		0x00034000	/* source register 1: %o5 */
#define	RS1_o4		0x00030000	/* source register 1: %o4 */
#define	RS1_o3		0x0002C000	/* source register 1: %o3 */
#define	RS1_o2		0x00028000	/* source register 1: %o2 */
#define	RS1_o1		0x00024000	/* source register 1: %o1 */
#define	RS1_o0		0x00020000	/* source register 1: %o0 */
#define	RS1_g7		0x0001C000	/* source register 1: %g7 */
#define	RS1_g6		0x00018000	/* source register 1: %g6 */
#define	RS1_g5		0x00014000	/* source register 1: %g5 */
#define	RS1_g4		0x00010000	/* source register 1: %g4 */
#define	RS1_g3		0x0000C000	/* source register 1: %g3 */
#define	RS1_g2		0x00008000	/* source register 1: %g2 */
#define	RS1_g1		0x00004000	/* source register 1: %g1 */
#define	RS1_g0		0x00000000	/* source register 1: %g0 */
#define	RS2_i7		0x0000001F	/* source register 2: %i7 */
#define	RS2_i6		0x0000001E	/* source register 2: %i6 */
#define	RS2_i5		0x0000001D	/* source register 2: %i5 */
#define	RS2_i4		0x0000001C	/* source register 2: %i4 */
#define	RS2_i3		0x0000001B	/* source register 2: %i3 */
#define	RS2_i2		0x0000001A	/* source register 2: %i2 */
#define	RS2_i1		0x00000019	/* source register 2: %i1 */
#define	RS2_i0		0x00000018	/* source register 2: %i0 */
#define	RS2_l7		0x00000017	/* source register 2: %l7 */
#define	RS2_l6		0x00000016	/* source register 2: %l6 */
#define	RS2_l5		0x00000015	/* source register 2: %l5 */
#define	RS2_l4		0x00000014	/* source register 2: %l4 */
#define	RS2_l3		0x00000013	/* source register 2: %l3 */
#define	RS2_l2		0x00000012	/* source register 2: %l2 */
#define	RS2_l1		0x00000011	/* source register 2: %l1 */
#define	RS2_l0		0x00000010	/* source register 2: %l0 */
#define	RS2_o7		0x0000000F	/* source register 2: %o7 */
#define	RS2_o6		0x0000000E	/* source register 2: %o6 */
#define	RS2_o5		0x0000000D	/* source register 2: %o5 */
#define	RS2_o4		0x0000000C	/* source register 2: %o4 */
#define	RS2_o3		0x0000000B	/* source register 2: %o3 */
#define	RS2_o2		0x0000000A	/* source register 2: %o2 */
#define	RS2_o1		0x00000009	/* source register 2: %o1 */
#define	RS2_o0		0x00000008	/* source register 2: %o0 */
#define	RS2_g7		0x00000007	/* source register 2: %g7 */
#define	RS2_g6		0x00000006	/* source register 2: %g6 */
#define	RS2_g5		0x00000005	/* source register 2: %g5 */
#define	RS2_g4		0x00000004	/* source register 2: %g4 */
#define	RS2_g3		0x00000003	/* source register 2: %g3 */
#define	RS2_g2		0x00000002	/* source register 2: %g2 */
#define	RS2_g1		0x00000001	/* source register 2: %g1 */
#define	RS2_g0		0x00000000	/* source register 2: %g0 */

/* instruction fields: floating point unit register equates */

#define	RD_f31		0x3E000000	/* destination register: %f31 */
#define	RD_f30		0x3C000000	/* destination register: %f30 */
#define	RD_f29		0x3A000000	/* destination register: %f29 */
#define	RD_f28		0x38000000	/* destination register: %f28 */
#define	RD_f27		0x36000000	/* destination register: %f27 */
#define	RD_f26		0x34000000	/* destination register: %f26 */
#define	RD_f25		0x32000000	/* destination register: %f25 */
#define	RD_f24		0x30000000	/* destination register: %f24 */
#define	RD_f23		0x2E000000	/* destination register: %f23 */
#define	RD_f22		0x2C000000	/* destination register: %f22 */
#define	RD_f21		0x2A000000	/* destination register: %f21 */
#define	RD_f20		0x28000000	/* destination register: %f20 */
#define	RD_f19		0x26000000	/* destination register: %f19 */
#define	RD_f18		0x24000000	/* destination register: %f18 */
#define	RD_f17		0x22000000	/* destination register: %f17 */
#define	RD_f16		0x20000000	/* destination register: %f16 */
#define	RD_f15		0x1E000000	/* destination register: %f15 */
#define	RD_f14		0x1C000000	/* destination register: %f14 */
#define	RD_f13		0x1A000000	/* destination register: %f13 */
#define	RD_f12		0x18000000	/* destination register: %f12 */
#define	RD_f11		0x16000000	/* destination register: %f11 */
#define	RD_f10		0x14000000	/* destination register: %f10 */
#define	RD_f09		0x12000000	/* destination register: %f09 */
#define	RD_f08		0x10000000	/* destination register: %f08 */
#define	RD_f07		0x0E000000	/* destination register: %f07 */
#define	RD_f06		0x0C000000	/* destination register: %f06 */
#define	RD_f05		0x0A000000	/* destination register: %f05 */
#define	RD_f04		0x08000000	/* destination register: %f04 */
#define	RD_f03		0x06000000	/* destination register: %f03 */
#define	RD_f02		0x04000000	/* destination register: %f02 */
#define	RD_f01		0x02000000	/* destination register: %f01 */
#define	RD_f00		0x00000000	/* destination register: %f00 */
#define	RS1_f31		0x0007C000	/* source register 1: %f31 */
#define	RS1_f30		0x00078000	/* source register 1: %f30 */
#define	RS1_f29		0x00074000	/* source register 1: %f29 */
#define	RS1_f28		0x00060000	/* source register 1: %f28 */
#define	RS1_f27		0x0006C000	/* source register 1: %f27 */
#define	RS1_f26		0x00068000	/* source register 1: %f26 */
#define	RS1_f25		0x00064000	/* source register 1: %f25 */
#define	RS1_f24		0x00060000	/* source register 1: %f24 */
#define	RS1_f23		0x0005C000	/* source register 1: %f23 */
#define	RS1_f22		0x00058000	/* source register 1: %f22 */
#define	RS1_f21		0x00054000	/* source register 1: %f21 */
#define	RS1_f20		0x00050000	/* source register 1: %f20 */
#define	RS1_f19		0x0004C000	/* source register 1: %f19 */
#define	RS1_f18		0x00048000	/* source register 1: %f18 */
#define	RS1_f17		0x00044000	/* source register 1: %f17 */
#define	RS1_f16		0x00040000	/* source register 1: %f16 */
#define	RS1_f15		0x0003C000	/* source register 1: %f15 */
#define	RS1_f14		0x00038000	/* source register 1: %f14 */
#define	RS1_f13		0x00034000	/* source register 1: %f13 */
#define	RS1_f12		0x00030000	/* source register 1: %f12 */
#define	RS1_f11		0x0002C000	/* source register 1: %f11 */
#define	RS1_f10		0x00028000	/* source register 1: %f10 */
#define	RS1_f09		0x00024000	/* source register 1: %f09 */
#define	RS1_f08		0x00020000	/* source register 1: %f08 */
#define	RS1_f07		0x0001C000	/* source register 1: %f07 */
#define	RS1_f06		0x00018000	/* source register 1: %f06 */
#define	RS1_f05		0x00014000	/* source register 1: %f05 */
#define	RS1_f04		0x00010000	/* source register 1: %f04 */
#define	RS1_f03		0x0000C000	/* source register 1: %f03 */
#define	RS1_f02		0x00008000	/* source register 1: %f02 */
#define	RS1_f01		0x00004000	/* source register 1: %f01 */
#define	RS1_f00		0x00000000	/* source register 1: %f00 */
#define	RS2_f31		0x0000001F	/* source register 2: %f31 */
#define	RS2_f30		0x0000001E	/* source register 2: %f30 */
#define	RS2_f29		0x0000001D	/* source register 2: %f29 */
#define	RS2_f28		0x0000001C	/* source register 2: %f28 */
#define	RS2_f27		0x0000001B	/* source register 2: %f27 */
#define	RS2_f26		0x0000001A	/* source register 2: %f26 */
#define	RS2_f25		0x00000019	/* source register 2: %f25 */
#define	RS2_f24		0x00000018	/* source register 2: %f24 */
#define	RS2_f23		0x00000017	/* source register 2: %f23 */
#define	RS2_f22		0x00000016	/* source register 2: %f22 */
#define	RS2_f21		0x00000015	/* source register 2: %f21 */
#define	RS2_f20		0x00000014	/* source register 2: %f20 */
#define	RS2_f19		0x00000013	/* source register 2: %f19 */
#define	RS2_f18		0x00000012	/* source register 2: %f18 */
#define	RS2_f17		0x00000011	/* source register 2: %f17 */
#define	RS2_f16		0x00000010	/* source register 2: %f16 */
#define	RS2_f15		0x0000000F	/* source register 2: %f15 */
#define	RS2_f14		0x0000000E	/* source register 2: %f14 */
#define	RS2_f13		0x0000000D	/* source register 2: %f13 */
#define	RS2_f12		0x0000000C	/* source register 2: %f12 */
#define	RS2_f11		0x0000000B	/* source register 2: %f11 */
#define	RS2_f10		0x0000000A	/* source register 2: %f10 */
#define	RS2_f09		0x00000009	/* source register 2: %f09 */
#define	RS2_f08		0x00000008	/* source register 2: %f08 */
#define	RS2_f07		0x00000007	/* source register 2: %f07 */
#define	RS2_f06		0x00000006	/* source register 2: %f06 */
#define	RS2_f05		0x00000005	/* source register 2: %f05 */
#define	RS2_f04		0x00000004	/* source register 2: %f04 */
#define	RS2_f03		0x00000003	/* source register 2: %f03 */
#define	RS2_f02		0x00000002	/* source register 2: %f02 */
#define	RS2_f01		0x00000001	/* source register 2: %f01 */
#define	RS2_f00		0x00000000	/* source register 2: %f00 */

/* instruction fields: miscellaneous equates */

#define	A_1		0x20000000	/* annul bit on */
#define	A_0		0x00000000	/* annul bit off */
#define	COND_F		0x1E000000	/* test condition F */
#define	COND_E		0x1C000000	/* test condition E */
#define	COND_D		0x1A000000	/* test condition D */
#define	COND_C		0x18000000	/* test condition C */
#define	COND_B		0x16000000	/* test condition B */
#define	COND_A		0x14000000	/* test condition A */
#define	COND_9		0x12000000	/* test condition 9 */
#define	COND_8		0x10000000	/* test condition 8 */
#define	COND_7		0x0E000000	/* test condition 7 */
#define	COND_6		0x0C000000	/* test condition 6 */
#define	COND_5		0x0A000000	/* test condition 5 */
#define	COND_4		0x08000000	/* test condition 4 */
#define	COND_3		0x06000000	/* test condition 3 */
#define	COND_2		0x04000000	/* test condition 2 */
#define	COND_1		0x02000000	/* test condition 1 */
#define	COND_0		0x00000000	/* test condition 0 */
#define	I_1		0x00002000	/* second ALU operand is simm13 */
#define	I_0		0x00000000	/* second ALU operand is rs2 and possibly ASI */
#define	ASI_SUPER_D	0x00000160	/* alternate address space: supervisor data */
#define	ASI_USER_D	0x00000140	/* alternate address space: user data */
#define	ASI_SUPER_I	0x00000120	/* alternate address space: supervisor instruction */
#define	ASI_USER_I	0x00000100	/* alternate address space: user instruction */


/* instruction types */

/*	Listed in order of first occurrence in dsmLib_sparc.c. */
/*	Note the reversal of where you would think formats 1 & 2 should be. */

/* instruction types: instruction format 2 (OP_0: Bicc, FBfcc, CBccc, SETHI) */

#define	itUnimp			0x00		/* unimplemented instruction */
#define	itBranch		0x01		/* branch instructions */
#define	itNop			0x02		/* no operation */
#define	itSethi			0x03		/* SETHI instruction */

/* instruction types: instruction format 1 (OP_1: Call)  */

#define	itCall			0x10		/* CALL disp30 */

/* instruction types: instruction format 3 (OP_2: arithmetic, etc.) */

#define	itArith			0x20		/* arithmetic ops: general case */
#define	itArithRs1SpoofMov	0x21		/* arithmetic ops: special rs1 values */
#define	itArithOrSet		0x22		/* arithmetic ops: special 'or' case */
#define	itArithRs1Spoof		0x23		/* arithmetic ops: special rs1 values */
#define	itArithRs1Rs2Spoof	0x24		/* arithmetic ops: special rs1 and rs2 values */
#define	itArithRs2Spoof		0x25		/* arithmetic ops: special rs2 values */
#define	itArithRdSpoof		0x26		/* arithmetic ops: special rd values */
#define	itArithRs1RdSpoof	0x27		/* arithmetic ops: special rs1 and rd values */
#define	itArithRs2RdSpoof	0x28		/* arithmetic ops: special rs2 and rd values */
#define	itArithRdSpoofCmp	0x29		/* arithmetic ops: special rd values */
#define	itShift			0x30		/* general case shift ops */
#define	itRdY			0x40		/* read from %y */
#define	itRdPsr			0x41		/* read from %psr */
#define	itRdWim			0x42		/* read from %wim */
#define	itRdTbr			0x43		/* read from %tbr */
#define	itWrY			0x44		/* write to %y */
#define	itWrPsr			0x45		/* write to %psr */
#define	itWrWim			0x46		/* write to %wim */
#define	itWrTbr			0x47		/* write to %tbr */
#define	itRdAsr			0x48		/* RDASR */
#define	itWrAsr			0x49		/* WRASR */
#define	itFloat2op		0x50		/* floating point, 2-operand ops */
#define	itFloat3op		0x51		/* floating point, 3-operand ops */
#define	itFloatCompare		0x52		/* floating point, compare ops */
#define	itCpop1			0x60		/* coprocessor operations, class 1 */
#define	itCpop2			0x61		/* coprocessor operations, class 2 */
#define	itRet			0x80		/* return (simple case) */
#define	itJmp			0x81		/* jump */
#define	itJmpl			0x82		/* jump and link */
#define	itRett			0x83		/* return from trap */
#define	itTrap			0x90		/* trap instructions */
#define	itFlush			0xA0		/* flush cache */
#define	itSaveTrivial		0xA1		/* save window */
#define	itSave			0xA2		/* save window */
#define	itRestoreTrivial	0xA3		/* restore window */
#define	itRestore		0xA4		/* restore window */

/* instruction types: instruction format 3 (OP_3: load/store) */

#define	itLd			0xB0		/* general load */
#define	itSt			0xB1		/* general store */
#define	itClr			0xB2		/* store from %g0 */
#define	itSwap			0xB3		/* general swap */
#define	itLdAsi			0xB4		/* load from alternate address space */
#define	itStAsi			0xB5		/* store to alternate address space */
#define	itClrAsi		0xB6		/* store %g0 to alternate address space */
#define	itSwapAsi		0xB7		/* swap with alternate address space */
#define	itLdFreg		0xC0		/* load from floating point register */
#define	itLdFsr			0xC1		/* load from floating point status register */
#define	itStFreg		0xC2		/* store to floating point register */
#define	itStFsr			0xC3		/* store to floating point status register */
#define	itStFq			0xC4		/* store to floating point queue */
#define	itLdCreg		0xC5		/* load from coprocessor register */
#define	itLdCsr			0xC6		/* load from coprocessor status register */
#define	itStCreg		0xC7		/* store to coprocessor register */
#define	itStCsr			0xC8		/* store to coprocessor status register */
#define	itStCq			0xC9		/* store to coprocessor queue */

/* the following structure is not size compatible with the
	one for the Motorola 68k family due to changed word size. */

typedef struct
    {
    char *name;
    unsigned long op;
    unsigned long mask;
    int type;
    } INST;

/* function declarations */

#if defined(__STDC__) || defined(__cplusplus)

IMPORT    int          dsmData (INSTR *binInst, unsigned int address);
IMPORT    int          dsmInst (FAST INSTR *binInst, unsigned int address,
                                VOIDFUNCPTR prtAddress);
IMPORT    int          dsmNbytes (FAST INSTR *binInst);

#else

IMPORT    int          dsmData ();
IMPORT    int          dsmInst ();
IMPORT    int          dsmNbytes ();

#endif  /* __STDC__ */

#ifdef __cplusplus
}
#endif

#endif /* __INCdsmSimsparcLibh */
