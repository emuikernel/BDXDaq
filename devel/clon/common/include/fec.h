/****************************************************************/
/*                                                              */
/*  file: rcu.h 	                                        */
/*                                                              */
/* This is the header file for the rcu library	 		*/
/*                                                              */
/* Created 		10 May 2008				*/
/* Last Modified 	21 May 2008				*/
/*								*/
/****************************************************************/


/* IS Instructions */
#define FEC_RD		0x0
#define FEC_WR		0x2
#define FEC_CMD		0x1
#define IS_CMD		0x3

/* IS Internal Codes */
#define LOOP		0x10000
#define WAIT		0x20000
#define ENDSEQ		0x80000
#define ENDMEM		0xF0000

/* IS Sequence Length */
#define FEC_RD_LEN	2
#define FEC_WR_LEN	3

/* ALTRO PARAMETERS */
#define ALTRO		0x0
#define BC		0x1
#define BCAST		0x1
#define NBCAST		0x0

/* ALTRO REGISTERS ADDRESSES */
/* Channel Individual Registers */
#define A_K1		0x00	// R/W
#define A_K2		0x01	// R/W
#define A_K3		0x02	// R/W
#define A_L1		0x03	// R/W
#define A_L2		0x04	// R/W
#define A_L3		0x05	// R/W
#define A_VFPED		0x06	// R+R/W
#define A_PMDTA		0x07	// R/W
#define A_ADEVL		0x11	// R
/* Global Registers */
#define A_ZSTHR		0x08	// R/W
#define A_BCTHR		0x09	// R/W
#define A_TRCFG		0x0A	// R/W
#define A_DPCFG		0x0B	// R/W
#define A_BFNPT		0x0C	// R/W
#define A_PMADD		0x0D	// R/W
#define A_ERSTR		0x10	// R
#define A_TRCNT		0x12	// R

/* ALTRO REGISTERS READ MASKS */
/* Channel Individual Registers */
#define M_K1		0x0ffff
#define M_K2		0x0ffff
#define M_K3		0x0ffff
#define M_L1		0x0ffff
#define M_L2		0x0ffff
#define M_L3		0x0ffff
#define M_VFPED		0xfffff
#define M_PMDTA		0x003ff
#define M_ADEVL		0x0ffff
/* Global Registers */
#define M_ZSTHR		0xfffff
#define M_BCTHR		0xfffff
#define M_TRCFG		0xfffff
#define M_DPCFG		0xfffff
#define M_BFNPT		0x0001f
#define M_PMADD		0xfffff
#define M_ERSTR		0x0ffff
#define M_TRCNT		0x003ff
/* ALTRO REGISTERS CHECK MASKS */
#define C_VFPED		0x003ff
/* for all other register the "check" 
masks are identical to the read masks  */

/* Arbitrary package identifer. Should be greater then 100 to avouid overlap with standard ATLAS identifiers */
#define P_ID_FEC 102

/* Declare the error codes: */
#define FEC_LIB_SUCCESS 	0
#define FEC_ALTRO_Read_FAIL 	1
#define RCU_LIB_FAIL 		6
