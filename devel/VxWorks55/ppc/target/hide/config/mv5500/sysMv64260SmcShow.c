/* sysMv64260SmcShow.c - Show routines for the MV64260's SMC and SPD */

/* Copyright 1999 Wind River Systems, Inc. */
/* Copyright 1998-2002 Motorola, Inc., All Rights Reserved */

/*
modification history
--------------------
01b,20nov02,yyz Added debugging abilities if SPD is invalid.
01a,01oct02,scb Copied from hxeb100 base (ver 01a).
*/

/*
DESCRIPTION
This file contains routines to display the contents of the MV64260's 
System Memory Controller registers, as well as the SPD contents for all
memory devices onboard. 
*/

/* includes */

#include "vxWorks.h"
#include "config.h"
#include "stdio.h"
#include "sysLib.h"
#include "sysMotVpd.h"
#include "sdramSpd.h"
#include "mv64260.h"
#include "mv64260Smc.h"

/* defines */

/* typedefs */

/* globals */

/* locals */

/* forward declarations */

LOCAL STATUS sysMv64260GetSpdData (UINT32 spdAddr, UINT32 offset, 
				   UINT32 dataSize, UCHAR * spdData);
STATUS sysMv64260SmcShow (void);
STATUS sysMv64260SpdShow (void);

/* external references */

IMPORT STATUS sysMotI2cRead (UINT32 addr, UINT32 offset, 
			     UINT32 addrBytes, UCHAR *spdData, 
			     UINT32 numBytes);

/******************************************************************************
*
* sysMv64260GetSpdData - read and validate the spd information
*
* This function reads the contents of the caller specified serial presence
* detect EEPROM and validates the checksum.
*
* RETURNS: OK if the SPD contents are valid, ERROR if not.
*/

LOCAL STATUS sysMv64260GetSpdData
    (
    UINT32 spdAddr,	/* SROM address for current bank */
    UINT32 offset,	/* first byte of SROM to read */
    UINT32 dataSz,	/* number of SROM bytes to read */
    UCHAR *spdData	/* address of caller's SPD buffer */
    )
    {
    UCHAR checksum = 0;	/* running checksum */
    int index;		/* index into SPD data buffer */

    if ( sysMotI2cRead (spdAddr, offset, SPD_BYTE_ADDR_SZ,
			spdData, dataSz) == OK) 
        {
        for (index = 0; index < SPD_CHECKSUM_INDEX; index++)
            checksum += spdData[index];
        checksum %= 256; 
        if (checksum != spdData[SPD_CHECKSUM_INDEX])
           {
           printf("Checksum for SPD device %d is incorrect!\n\r",
			(spdAddr - SPD_EEPROM_ADDR_0)/2);
           printf("Calculated checksum: %X\n\r", checksum);
           printf("Actual checksum: %X\n\r", spdData[SPD_CHECKSUM_INDEX]);
           }
        return (OK);
        }
    return (ERROR);
    }

/******************************************************************************
*
* sysMv64260SmcShow - Display the SMC registers.
*
* This function reads & displays the SMC registers of the MV64260.
*
* RETURNS: OK 
*/

STATUS sysMv64260SmcShow ()
    {
    MV64260_SMC * pSmcReg;	     /* pointer to SMC register storage area */
    MV64260_SMC smcReg;		     /* SMC register storage area */
    UINT32 base = MV64260_REG_BASE;  /* base of MV64260 register set */

    pSmcReg = &smcReg;

    /* Read SMC registers into SMC register storage area */

    pSmcReg->scsxLoDecodeAddr[0] = MV64260_READ32(base, 
	CPUIF_SCS0_LO_DECODE_ADDR);
    pSmcReg->scsxLoDecodeAddr[1] = MV64260_READ32(base, 
	CPUIF_SCS1_LO_DECODE_ADDR);
    pSmcReg->scsxLoDecodeAddr[2] = MV64260_READ32(base, 
	CPUIF_SCS2_LO_DECODE_ADDR);
    pSmcReg->scsxLoDecodeAddr[3] = MV64260_READ32(base, 
	CPUIF_SCS3_LO_DECODE_ADDR);
    pSmcReg->scsxHiDecodeAddr[0] = MV64260_READ32(base, 
	CPUIF_SCS0_HI_DECODE_ADDR);
    pSmcReg->scsxHiDecodeAddr[1] = MV64260_READ32(base, 
	CPUIF_SCS1_HI_DECODE_ADDR);
    pSmcReg->scsxHiDecodeAddr[2] = MV64260_READ32(base, 
	CPUIF_SCS2_HI_DECODE_ADDR);
    pSmcReg->scsxHiDecodeAddr[3] = MV64260_READ32(base, 
	CPUIF_SCS3_HI_DECODE_ADDR);
    pSmcReg->sdramBankParams[0] = MV64260_READ32(base, SDRAM_BANK0_PARAMS);
    pSmcReg->sdramBankParams[1] = MV64260_READ32(base, SDRAM_BANK1_PARAMS);
    pSmcReg->sdramBankParams[2] = MV64260_READ32(base, SDRAM_BANK2_PARAMS);
    pSmcReg->sdramBankParams[3] = MV64260_READ32(base, SDRAM_BANK3_PARAMS);
    pSmcReg->sdramCfg = MV64260_READ32(base, SDRAM_CFG);
    pSmcReg->sdramAddrCtrl = MV64260_READ32(base, SDRAM_ADDR_CTRL);
    pSmcReg->sdramTmngParams = MV64260_READ32(base, SDRAM_TMNG_PARAMS);
    pSmcReg->sdramUmaCtrl = MV64260_READ32(base, SDRAM_UMA_CTRL);
    pSmcReg->sdramIfXbarCtrlLo = MV64260_READ32(base, SDRAM_IF_XBAR_CTRL_LO);
    pSmcReg->sdramIfXbarCtrlHi = MV64260_READ32(base, SDRAM_IF_XBAR_CTRL_HI);
    pSmcReg->sdramIfXbarTmout = MV64260_READ32(base, SDRAM_IF_XBAR_TMOUT);
    pSmcReg->sdramErrDataLo = MV64260_READ32(base, SDRAM_ERR_DATA_LO);
    pSmcReg->sdramErrDataHi = MV64260_READ32(base, SDRAM_ERR_DATA_HI);
    pSmcReg->sdramErrAddr = MV64260_READ32(base, SDRAM_ERR_ADDR);
    pSmcReg->sdramRcvEcc = MV64260_READ32(base, SDRAM_RCV_ECC);
    pSmcReg->sdramCalcEcc = MV64260_READ32(base, SDRAM_CALC_ECC);
    pSmcReg->sdramEccCtrl = MV64260_READ32(base, SDRAM_ECC_CTRL);
    pSmcReg->sdramEccErrCntr = MV64260_READ32(base, SDRAM_ECC_ERR_CNTR);

    /* Display SMC register values */

    printf("MV64260 SDRAM Controller Registers:\r\n");
    printf("-----------------------------------\r\n");
    printf("SCS0 Low Decode Address: 0x%08X\r\n", pSmcReg->scsxLoDecodeAddr[0]);
    printf("SCS0 High Decode Address:0x%08X\r\n", pSmcReg->scsxHiDecodeAddr[0]);
    printf("Bank 0 Parameters:       0x%08X\r\n", pSmcReg->sdramBankParams[0]);
    printf("SCS1 Low Decode Address: 0x%08X\r\n", pSmcReg->scsxLoDecodeAddr[1]);
    printf("SCS1 High Decode Address:0x%08X\r\n", pSmcReg->scsxHiDecodeAddr[1]);
    printf("Bank 1 Parameters:       0x%08X\r\n", pSmcReg->sdramBankParams[1]);
    printf("SCS2 Low Decode Address: 0x%08X\r\n", pSmcReg->scsxLoDecodeAddr[2]);
    printf("SCS2 High Decode Address:0x%08X\r\n", pSmcReg->scsxHiDecodeAddr[2]);
    printf("Bank 2 Parameters:       0x%08X\r\n", pSmcReg->sdramBankParams[2]);
    printf("SCS3 Low Decode Address: 0x%08X\r\n", pSmcReg->scsxLoDecodeAddr[3]);
    printf("SCS3 High Decode Address:0x%08X\r\n", pSmcReg->scsxHiDecodeAddr[3]);
    printf("Bank 3 Parameters:       0x%08X\r\n", pSmcReg->sdramBankParams[3]);
    printf("SDRAM Configuration:     0x%08X\r\n", pSmcReg->sdramCfg);
    printf("SDRAM Address Control:   0x%08X\r\n", pSmcReg->sdramAddrCtrl);
    printf("SDRAM Timing Parameters: 0x%08X\r\n", pSmcReg->sdramTmngParams);
    printf("SDRAM IF Xbar Ctrl(Low): 0x%08X\r\n", pSmcReg->sdramIfXbarCtrlLo);
    printf("SDRAM IF Xbar Ctrl(High):0x%08X\r\n", pSmcReg->sdramIfXbarCtrlHi);
    printf("SDRAM IF Xbar Timeout:   0x%08X\r\n", pSmcReg->sdramIfXbarTmout);
    printf("SDRAM Error Data (Low):  0x%08X\r\n", pSmcReg->sdramErrDataLo);
    printf("SDRAM Error Data (High): 0x%08X\r\n", pSmcReg->sdramErrDataHi);
    printf("SDRAM Error Address:     0x%08X\r\n", pSmcReg->sdramErrAddr);
    printf("SDRAM Received ECC:      0x%08X\r\n", pSmcReg->sdramRcvEcc);
    printf("SDRAM Calculated ECC:    0x%08X\r\n", pSmcReg->sdramCalcEcc);
    printf("SDRAM ECC Control:       0x%08X\r\n", pSmcReg->sdramEccCtrl);
    printf("SDRAM ECC Error Counter: 0x%08X\r\n", pSmcReg->sdramEccErrCntr);

    return (OK);
    }

/******************************************************************************
*
* sysMv64260SpdShow - Display the SPD values
*
* This function reads & displays the SPD values for the memory devices on
* the board.
*
* RETURNS: OK 
*/

STATUS sysMv64260SpdShow ()
    {
    UCHAR *  spdPtrs[MV64260_SDRAM_BANKS];    	/* spd buffer ptrs */
    int spd;                         	  	/* Spd index ctr */
    int spdByte;				/* SPD byte index ctr */
    UCHAR   spdData[(MV64260_SDRAM_BANKS / 2) * SPD_SIZE];  /* spd data */
    UCHAR * pBfr = &spdData[0];			/* temp buffer ptr */
    UCHAR * pData;

    /* loop through each spd device */

    for (spd = 0; spd < MV64260_SDRAM_BANKS; spd +=2)
        {
        spdPtrs[spd] = NULL;

        /* read the spd data into the current buffer and validate */

        if (sysMv64260GetSpdData (SPD_EEPROM_ADDR_0 + spd, 0, SPD_SIZE, pBfr) 
            == OK)
            {

            /* save current buffer address and advance to the next buffer */

            spdPtrs[spd] = pBfr;
            pBfr += SPD_SIZE;
            }
    	else
            {

            /* 
             * There is not a valid SPD at this address so just advance to 
             * the next buffer and leave the spdPtrs[] pointing at NULL.
             */
 
             pBfr += SPD_SIZE;
             printf("No SPD device detected at %X\n\r", 
                    SPD_EEPROM_ADDR_0 + spd);
             }
        }

    /* Display SPD Contents */

    for (spd = 0; spd < MV64260_SDRAM_BANKS; spd += 2)
        {
        if ((pData = spdPtrs[spd]) != NULL)
            {
	    printf("I2C Address: %X\n\r", SPD_EEPROM_ADDR_0 + spd);
    	    printf("---------------\r\n");
	    for (spdByte = 0; spdByte < SPD_SIZE; spdByte++)
		{
		printf("Byte %d = 0x%02X\n\r", spdByte, pData[spdByte]);
		} 
	    }
	}
    return (OK);
    }
