/* sdramSpd.h - SDRAM Serial Presence Detect definitions file */

/* Copyright 1998-2000 Motorola, Inc. */

/*
modification history
--------------------
01b,16aug00,dmw  Wind River coding standards review.
01a,13mar00,dmw  Written (from version 01a of mcpn765/sdramSpd.h).
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
#define SPD_TCYC_INDEX		 9	/* SDRAM Cycle Time index */
#define SPD_DIMM_TYPE_INDEX     11      /* DIMM Configuration Type index */
#define SPD_REFRESH_RATE_INDEX	12	/* Refresh Rate/Type */
#define SPD_DEV_WIDTH_INDEX	13      /* Primary SDRAM Device Width index */ 
#define SPD_ECC_WIDTH_INDEX	14	/* Error Checking Device Width index */
#define SPD_DEV_BANKS_INDEX	17      /* Number of Device Banks index */
#define SPD_CL_INDEX		18      /* CAS latency byte index */
#define SPD_CS_LATENCY_INDEX	19	/* CS Latency index */
#define SPD_WE_LATENCY_INDEX	20	/* WE Latency index */
#define SPD_TCYC_RCL_INDEX	23	/* CAS latency @ X-1 index */
#define SPD_TRP_INDEX		27      /* Row Precharge Time index */
#define SPD_TRCD_INDEX 		29      /* RAS to CAS delay index */
#define SPD_TRAS_INDEX		30      /* RAS pulse width index */
#define SPD_CHECKSUM_INDEX	63	/* Checksum for bytes 0-62 index */

#ifdef __cplusplus
    }
#endif /* __cplusplus */

#endif	/* INCsdramSpdh */
