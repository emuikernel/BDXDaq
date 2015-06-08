/* m8260Mux.h - Motorola MPC8260 CPM Multiplexor header file */

/* Copyright 1984-1999 Wind River Systems, Inc. */

/*
modification history
--------------------
01a,12sep99,ms_  created from m8260Cpm.h, 01d.
*/

/*
 * This file contains constants for the Communications Processor Module's
 * Multiplexor (CMX) in the Motorola MPC8260 PowerQUICC II integrated 
 * Communications Processor
 */

#ifndef __INCm8260CpmMuxh
#define __INCm8260CpmMuxh

#ifdef __cplusplus
extern "C" {
#endif
    
#ifndef M8260ABBREVIATIONS
#define M8260ABBREVIATIONS

#ifdef  _ASMLANGUAGE
#define CAST(x)
#else /* _ASMLANGUAGE */
typedef volatile UCHAR VCHAR;   /* shorthand for volatile UCHAR */
typedef volatile INT32 VINT32; /* volatile unsigned word */
typedef volatile INT16 VINT16; /* volatile unsigned halfword */
typedef volatile INT8 VINT8;   /* volatile unsigned byte */
typedef volatile UINT32 VUINT32; /* volatile unsigned word */
typedef volatile UINT16 VUINT16; /* volatile unsigned halfword */
typedef volatile UINT8 VUINT8;   /* volatile unsigned byte */
#define CAST(x) (x)
#endif  /* _ASMLANGUAGE */

#endif /* M8260ABBREVIATIONS */

/*
 * MPC8260 internal register/memory map (section 17 of prelim. spec)
 * note that these are offsets from the value stored in the IMMR
 * register. Also note that in the MPC8260, the IMMR is not a special
 * purpose register, but it is memory mapped.
 */
 
/* CPM mux FCC clock route register */

#define M8260_CMXFCR(base)   (CAST(VUINT32 *)((base) + 0x11B04))

/* CPM mux SCC clock route register */

#define M8260_CMXSCR(base)   (CAST(VUINT32 *)((base) + 0x11B08))
 
/* CMX FCC Clock Route Register bit definition (CMXFCR - 0x11B04) */

#define M8260_CMXFCR_GR1	0x80000000	/* Grant Support of FCC1 */
#define M8260_CMXFCR_FC1_MUX	0x40000000	/* FCC1 Connection - mux SI */
#define M8260_CMXFCR_R1CS_MSK	0x38000000	/* FCC1 Receive Clock Source */
#define M8260_CMXFCR_R1CS_BRG5	0x00000000	/* BRG5 clock source */
#define M8260_CMXFCR_R1CS_BRG6	0x08000000	/* BRG6 clock source */
#define M8260_CMXFCR_R1CS_BRG7	0x10000000	/* BRG7 clock source */
#define M8260_CMXFCR_R1CS_BRG8	0x18000000	/* BRG8 clock source */
#define M8260_CMXFCR_R1CS_CLK9	0x20000000	/* CLK9 clock source */
#define M8260_CMXFCR_R1CS_CLK10	0x28000000	/* CLK10 clock source */
#define M8260_CMXFCR_R1CS_CLK11	0x30000000	/* CLK11 clock source */
#define M8260_CMXFCR_R1CS_CLK12	0x38000000	/* CLK12 clock source */
#define M8260_CMXFCR_T1CS_MSK	0x07000000	/* FCC1 Transmit Clock Source */
#define M8260_CMXFCR_T1CS_BRG5	0x00000000	/* BRG5 clock source */
#define M8260_CMXFCR_T1CS_BRG6	0x01000000	/* BRG6 clock source */
#define M8260_CMXFCR_T1CS_BRG7	0x02000000	/* BRG7 clock source */
#define M8260_CMXFCR_T1CS_BRG8	0x03000000	/* BRG8 clock source */
#define M8260_CMXFCR_T1CS_CLK9	0x04000000	/* CLK9 clock source */
#define M8260_CMXFCR_T1CS_CLK10	0x05000000	/* CLK10 clock source */
#define M8260_CMXFCR_T1CS_CLK11	0x06000000	/* CLK11 clock source */
#define M8260_CMXFCR_T1CS_CLK12	0x07000000	/* CLK12 clock source */

#define M8260_CMXFCR_GR2	0x00800000	/* Grant Support of FCC2 */
#define M8260_CMXFCR_FC2_MUX	0x00400000	/* FCC2 Connection - mux SI */
#define M8260_CMXFCR_R2CS_MSK	0x00380000	/* FCC2 Receive Clock Source */
#define M8260_CMXFCR_R2CS_BRG5	0x00000000	/* BRG5 clock source */
#define M8260_CMXFCR_R2CS_BRG6	0x00080000	/* BRG6 clock source */
#define M8260_CMXFCR_R2CS_BRG7	0x00100000	/* BRG7 clock source */
#define M8260_CMXFCR_R2CS_BRG8	0x00180000	/* BRG8 clock source */
#define M8260_CMXFCR_R2CS_CLK13	0x00200000	/* CLK13 clock source */
#define M8260_CMXFCR_R2CS_CLK14	0x00280000	/* CLK14 clock source */
#define M8260_CMXFCR_R2CS_CLK15	0x00300000	/* CLK15 clock source */
#define M8260_CMXFCR_R2CS_CLK16	0x00380000	/* CLK16 clock source */
#define M8260_CMXFCR_T2CS_MSK	0x00070000	/* FCC2 Transmit Clock Source */
#define M8260_CMXFCR_T2CS_BRG5	0x00000000	/* BRG5 clock source */
#define M8260_CMXFCR_T2CS_BRG6	0x00010000	/* BRG6 clock source */
#define M8260_CMXFCR_T2CS_BRG7	0x00020000	/* BRG7 clock source */
#define M8260_CMXFCR_T2CS_BRG8	0x00030000	/* BRG8 clock source */
#define M8260_CMXFCR_T2CS_CLK13	0x00040000	/* CLK13 clock source */
#define M8260_CMXFCR_T2CS_CLK14	0x00050000	/* CLK14 clock source */
#define M8260_CMXFCR_T2CS_CLK15	0x00060000	/* CLK15 clock source */
#define M8260_CMXFCR_T2CS_CLK16	0x00070000	/* CLK16 clock source */

#define M8260_CMXFCR_GR3	0x00008000	/* Grant Support of FCC3 */
#define M8260_CMXFCR_FC3_MUX	0x00004000	/* FCC3 Connection - mux SI */
#define M8260_CMXFCR_R3CS_MSK	0x00003800	/* FCC3 Receive Clock Source */
#define M8260_CMXFCR_R3CS_BRG5	0x00000000	/* BRG5 clock source */
#define M8260_CMXFCR_R3CS_BRG6	0x00000800	/* BRG6 clock source */
#define M8260_CMXFCR_R3CS_BRG7	0x00001000	/* BRG7 clock source */
#define M8260_CMXFCR_R3CS_BRG8	0x00001800	/* BRG8 clock source */
#define M8260_CMXFCR_R3CS_CLK13	0x00002000	/* CLK13 clock source */
#define M8260_CMXFCR_R3CS_CLK14	0x00002800	/* CLK14 clock source */
#define M8260_CMXFCR_R3CS_CLK15	0x00003000	/* CLK15 clock source */
#define M8260_CMXFCR_R3CS_CLK16	0x00003800	/* CLK16 clock source */
#define M8260_CMXFCR_T3CS_MSK	0x00000700	/* FCC3 Transmit Clock Source */
#define M8260_CMXFCR_T3CS_BRG5	0x00000000	/* BRG5 clock source */
#define M8260_CMXFCR_T3CS_BRG6	0x00000100	/* BRG6 clock source */
#define M8260_CMXFCR_T3CS_BRG7	0x00000200	/* BRG7 clock source */
#define M8260_CMXFCR_T3CS_BRG8	0x00000300	/* BRG8 clock source */
#define M8260_CMXFCR_T3CS_CLK13	0x00000400	/* CLK13 clock source */
#define M8260_CMXFCR_T3CS_CLK14	0x00000500	/* CLK14 clock source */
#define M8260_CMXFCR_T3CS_CLK15	0x00000600	/* CLK15 clock source */
#define M8260_CMXFCR_T3CS_CLK16	0x00000700	/* CLK16 clock source */

/* CMX SCC Clock Route Register bit definition (CMXSCR - 0x11B08) */

#define M8260_CMXSCR_GR1	0x80000000	/* Grant Support of SCC1 */
#define M8260_CMXSCR_SC1_MUX	0x40000000	/* SCC1 Connection - mux SI */
#define M8260_CMXSCR_R1CS_MSK	0x38000000	/* SCC1 Receive Clock Source */
#define M8260_CMXSCR_R1CS_BRG1	0x00000000	/* BRG1 clock source */
#define M8260_CMXSCR_R1CS_BRG2	0x08000000	/* BRG2 clock source */
#define M8260_CMXSCR_R1CS_BRG3	0x10000000	/* BRG3 clock source */
#define M8260_CMXSCR_R1CS_BRG4	0x18000000	/* BRG4 clock source */
#define M8260_CMXSCR_R1CS_CLK11	0x20000000	/* CLK11 clock source */
#define M8260_CMXSCR_R1CS_CLK12	0x28000000	/* CLK12 clock source */
#define M8260_CMXSCR_R1CS_CLK3	0x30000000	/* CLK3 clock source */
#define M8260_CMXSCR_R1CS_CLK4	0x38000000	/* CLK4 clock source */
#define M8260_CMXSCR_T1CS_MSK	0x07000000	/* SCC1 Transmit Clock Source */
#define M8260_CMXSCR_T1CS_BRG1	0x00000000	/* BRG1 clock source */
#define M8260_CMXSCR_T1CS_BRG2	0x01000000	/* BRG2 clock source */
#define M8260_CMXSCR_T1CS_BRG3	0x02000000	/* BRG3 clock source */
#define M8260_CMXSCR_T1CS_BRG4	0x03000000	/* BRG4 clock source */
#define M8260_CMXSCR_T1CS_CLK11	0x04000000	/* CLK11 clock source */
#define M8260_CMXSCR_T1CS_CLK12	0x05000000	/* CLK12 clock source */
#define M8260_CMXSCR_T1CS_CLK3	0x06000000	/* CLK3 clock source */
#define M8260_CMXSCR_T1CS_CLK4	0x07000000	/* CLK4 clock source */

#define M8260_CMXSCR_GR2	0x00800000	/* Grant Support of SCC2 */
#define M8260_CMXSCR_SC2_MUX	0x00400000	/* SCC2 Connection - mux SI */
#define M8260_CMXSCR_R2CS_MSK	0x00380000	/* SCC2 Receive Clock Source */
#define M8260_CMXSCR_R2CS_BRG1	0x00000000	/* BRG1 clock source */
#define M8260_CMXSCR_R2CS_BRG2	0x00080000	/* BRG2 clock source */
#define M8260_CMXSCR_R2CS_BRG3	0x00100000	/* BRG3 clock source */
#define M8260_CMXSCR_R2CS_BRG4	0x00180000	/* BRG4 clock source */
#define M8260_CMXSCR_R2CS_CLK11	0x00200000	/* CLK11 clock source */
#define M8260_CMXSCR_R2CS_CLK12	0x00280000	/* CLK12 clock source */
#define M8260_CMXSCR_R2CS_CLK3	0x00300000	/* CLK3 clock source */
#define M8260_CMXSCR_R2CS_CLK4	0x00380000	/* CLK4 clock source */
#define M8260_CMXSCR_T2CS_MSK	0x00070000	/* SCC2 Transmit Clock Source */
#define M8260_CMXSCR_T2CS_BRG1	0x00000000	/* BRG1 clock source */
#define M8260_CMXSCR_T2CS_BRG2	0x00010000	/* BRG2 clock source */
#define M8260_CMXSCR_T2CS_BRG3	0x00020000	/* BRG3 clock source */
#define M8260_CMXSCR_T2CS_BRG4	0x00030000	/* BRG4 clock source */
#define M8260_CMXSCR_T2CS_CLK11	0x00040000	/* CLK11 clock source */
#define M8260_CMXSCR_T2CS_CLK12	0x00050000	/* CLK12 clock source */
#define M8260_CMXSCR_T2CS_CLK3	0x00060000	/* CLK3 clock source */
#define M8260_CMXSCR_T2CS_CLK4	0x00070000	/* CLK4 clock source */

#define M8260_CMXSCR_GR3	0x00008000	/* Grant Support of SCC3 */
#define M8260_CMXSCR_SC3_MUX	0x00004000	/* SCC3 Connection - mux SI */
#define M8260_CMXSCR_R3CS_MSK	0x00003800	/* SCC3 Receive Clock Source */
#define M8260_CMXSCR_R3CS_BRG1	0x00000000	/* BRG1 clock source */
#define M8260_CMXSCR_R3CS_BRG2	0x00000800	/* BRG2 clock source */
#define M8260_CMXSCR_R3CS_BRG3	0x00001000	/* BRG3 clock source */
#define M8260_CMXSCR_R3CS_BRG4	0x00001800	/* BRG4 clock source */
#define M8260_CMXSCR_R3CS_CLK5	0x00002000	/* CLK5 clock source */
#define M8260_CMXSCR_R3CS_CLK6	0x00002800	/* CLK6 clock source */
#define M8260_CMXSCR_R3CS_CLK7	0x00003000	/* CLK7 clock source */
#define M8260_CMXSCR_R3CS_CLK8	0x00003800	/* CLK8 clock source */
#define M8260_CMXSCR_T3CS_MSK	0x00000700	/* SCC3 Transmit Clock Source */
#define M8260_CMXSCR_T3CS_BRG1	0x00000000	/* BRG1 clock source */
#define M8260_CMXSCR_T3CS_BRG2	0x00000100	/* BRG2 clock source */
#define M8260_CMXSCR_T3CS_BRG3	0x00000200	/* BRG3 clock source */
#define M8260_CMXSCR_T3CS_BRG4	0x00000300	/* BRG4 clock source */
#define M8260_CMXSCR_T3CS_CLK5	0x00000400	/* CLK5 clock source */
#define M8260_CMXSCR_T3CS_CLK6	0x00000500	/* CLK6 clock source */
#define M8260_CMXSCR_T3CS_CLK7	0x00000600	/* CLK7 clock source */
#define M8260_CMXSCR_T3CS_CLK8	0x00000700	/* CLK8 clock source */

#define M8260_CMXSCR_GR4	0x00000080	/* Grant Support of SCC4 */
#define M8260_CMXSCR_SC4_MUX	0x00000040	/* SCC4 Connection - mux SI */
#define M8260_CMXSCR_R4CS_MSK	0x00000038	/* SCC4 Receive Clock Source */
#define M8260_CMXSCR_R4CS_BRG1	0x00000000	/* BRG1 clock source */
#define M8260_CMXSCR_R4CS_BRG2	0x00000008	/* BRG2 clock source */
#define M8260_CMXSCR_R4CS_BRG3	0x00000010	/* BRG3 clock source */
#define M8260_CMXSCR_R4CS_BRG4	0x00000018	/* BRG4 clock source */
#define M8260_CMXSCR_R4CS_CLK1	0x00000020	/* CLK1 clock source */
#define M8260_CMXSCR_R4CS_CLK2	0x00000028	/* CLK2 clock source */
#define M8260_CMXSCR_R4CS_CLK3	0x00000030	/* CLK3 clock source */
#define M8260_CMXSCR_R4CS_CLK4	0x00000038	/* CLK4 clock source */
#define M8260_CMXSCR_T4CS_MSK	0x00000007	/* SCC4 Transmit Clock Source */
#define M8260_CMXSCR_T4CS_BRG1	0x00000000	/* BRG1 clock source */
#define M8260_CMXSCR_T4CS_BRG2	0x00000001	/* BRG2 clock source */
#define M8260_CMXSCR_T4CS_BRG3	0x00000002	/* BRG3 clock source */
#define M8260_CMXSCR_T4CS_BRG4	0x00000003	/* BRG4 clock source */
#define M8260_CMXSCR_T4CS_CLK1	0x00000004	/* CLK1 clock source */
#define M8260_CMXSCR_T4CS_CLK2	0x00000005	/* CLK2 clock source */
#define M8260_CMXSCR_T4CS_CLK3	0x00000006	/* CLK3 clock source */
#define M8260_CMXSCR_T4CS_CLK4	0x00000007	/* CLK4 clock source */

/* CMX SMC Clock Route Register bit definition (CMXSMR - 0x11B0C) */

#define M8260_CMXSMR_SMC1_MUX		0x80000000	/* SMC1 Connection mux*/
#define M8260_CMXSMR_SMC1CS_MSK		0x30000000	/* SMC1 Clock Source */
#define M8260_CMXSMR_SMC1CS_BRG1	0x00000000	/* BRG1 clock source */
#define M8260_CMXSMR_SMC1CS_BRG7	0x10000000	/* BRG7 clock source */
#define M8260_CMXSMR_SMC1CS_CLK7	0x20000000	/* CLK7 clock source */
#define M8260_CMXSMR_SMC1CS_CLK9	0x30000000	/* CLK9 clock source */

#define M8260_CMXSMR_SMC2_MUX		0x08000000	/* SMC2 Connection mux*/
#define M8260_CMXSMR_SMC2CS_MSK		0x03000000	/* SMC2 Clock Source */
#define M8260_CMXSMR_SMC2CS_BRG2	0x00000000	/* BRG2 clock source */
#define M8260_CMXSMR_SMC2CS_BRG8	0x01000000	/* BRG8 clock source */
#define M8260_CMXSMR_SMC2CS_CLK19	0x02000000	/* CLK19 clock source */
#define M8260_CMXSMR_SMC2CS_CLK20	0x03000000	/* CLK20 clock source */

#ifdef __cplusplus
}
#endif

#endif /* __INCm8260CpmMuxh */
