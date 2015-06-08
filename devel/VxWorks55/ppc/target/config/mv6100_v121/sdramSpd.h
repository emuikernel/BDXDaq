/* sdramSpd.h - SDRAM Serial Presence Detect definitions file */

/* Copyright 1998-2000 Wind River Systems, Inc. */
/* Copyright 1998-2002 Motorola, Inc. */

/*
modification history
--------------------
01a,03may02,cak  Ported from ver 01b, mcp820/sdramSpd.h.
*/

/*
This file contains the field definitions for the SDRAM Serial Presence Detect
(SPD) EEPROMs.
*/

#ifndef	INCsdramSpdh
#define	INCsdramSpdh

#ifdef __cplusplus
    extern "C" {
#endif /* __cplusplus */
 
#define SPD_SIZE	       256	/* Number of Bytes in the SPD */
#define SPD_NUM_BYTES_INDEX	 0	/* Number of Bytes Utilized index */
#define SPD_DEVICE_SIZE_INDEX	 1	/* Number of Device Bytes index */
#define SPD_MEMORY_TYPE_INDEX	 2	/* Memory Type index */
#define SPD_ROW_ADDR_INDEX	 3	/* Number of Row Addresses index */
#define SPD_COL_ADDR_INDEX	 4	/* Number of Column Addresses index */
#define SPD_NUM_DIMMBANKS_INDEX  5      /* Number of DIMM Banks index */
#define SPD_DATA_WIDTH_INDEX 	 6	/* Data Width of Assembly */
#define SPD_TCYC_INDEX		 9	/* SDRAM Cycle Time index */
#define SPD_DIMM_TYPE_INDEX     11      /* DIMM Configuration Type index */
#define SPD_REFRESH_RATE_INDEX	12	/* Refresh Rate/Type */
#define SPD_DEV_WIDTH_INDEX	13      /* Primary SDRAM Device Width index */ 
#define SPD_ECC_WIDTH_INDEX	14	/* Error Checking Device Width index */
#define SPD_DEV_BANKS_INDEX	17      /* Number of Device Banks index */
#define SPD_CL_INDEX		18      /* CAS latency byte index */
#define SPD_CS_LATENCY_INDEX	19	/* CS Latency index */
#define SPD_WE_LATENCY_INDEX	20	/* WE Latency index */
#define SPD_ATTRIBUTES_INDEX	21	/* SDRAM attributes index */
#define SPD_TCYC_RCL_INDEX	23	/* CAS latency @ X-1 index */
#define SPD_TCYC_RCL2_INDEX	25	/* CAS Latency @ x-2 index */
#define SPD_TRP_INDEX		27      /* Row Precharge Time index */
#define SPD_TRRD_INDEX		28	/* Min Row active to Row active */
#define SPD_TRCD_INDEX 		29      /* RAS to CAS delay index */
#define SPD_TRAS_INDEX		30      /* RAS pulse width index */
#define SPD_TRC_INDEX		41	/* tRC */
#define SPD_TRFC_INDEX		42	/* tRFC */
#define SPD_CHECKSUM_INDEX	63	/* Checksum for bytes 0-62 index */

/* SPD Refresh Rate Definitions */

#define SPD_REF_SELF_REFRESH	0x80	/* Self Refresh Flag */

#define SPD_REF_NORMAL		0x00	/* Normal 15.625 us */
#define SPD_REF_DIV4		0x01	/* Reduced 3.9 us */
#define SPD_REF_DIV2		0x02	/* Reduced 7.8 us */
#define SPD_REF_2X		0x03	/* 2X 31.3 us */
#define SPD_REF_4X		0x04	/* 4X 62.5 us */
#define SPD_REF_8X		0x05	/* 8X 125 us */

#define SPD_REF_NORMAL_MS	15625	/* Normal in milliseconds */
#define SPD_REF_DIV4_MS		3900	/* Reduced 3.9 us in milliseconds */
#define SPD_REF_DIV2_MS		7800	/* Reduced 7.8 us in milliseconds */
#define SPD_REF_2X_MS		31300	/* 2X in milliseconds */
#define SPD_REF_4X_MS		62500	/* 4X in milliseconds */
#define SPD_REF_8X_MS		125000	/* 8X in milliseconds */

/* SPD Module Attributes Definition */

#define SPD_MOD_ATTR_REG	0x02	/* Registered bit */

#ifdef __cplusplus
    }
#endif /* __cplusplus */

#endif	/* INCsdramSpdh */
