/* sysMv64360SmcShow.c - Show routines for the MV64360's SMC and SPD */

/* Copyright 1999 Wind River Systems, Inc. */
/* Copyright 1998-2003 Motorola, Inc., All Rights Reserved */

/*
modification history
--------------------
01f,24jan03,cak  Added debugging abilities for invalid SPD.
01e,11sep02,cak  Modified during SMC debug. 
01d,29aug02,cak  Fixed some #define's that were changed.
01c,10jul02,cak  Register mnemonic name changes.
01b,29may02,cak  Modified I2C routine calls.
01a,06may02,cak  Created.
*/


/*
DESCRIPTION
This file contains routines to display the contents of the MV64360's 
System Memory Controller registers, as well as the SPD contents for all
memory devices onboard. 
*/

/* includes */

#include "vxWorks.h"
#include "config.h"
#include "sysMotVpd.h"
#include "sdramSpd.h"
#include "mv64360.h"
#include "mv64360Smc.h"

/* defines */

/* typedefs */

/* globals */

/* locals */

/* forward declarations */

LOCAL STATUS sysMv64360GetSpdData (UINT32 spdAddr, UINT32 offset, 
				   UINT32 dataSize, UCHAR * spdData);
STATUS sysMv64360SmcShow (void);
STATUS sysMv64360SpdShow (void);

/* external references */

IMPORT STATUS sysMotI2cRead (UINT32 spdAddr, UINT32 offset, 
			     UINT32 devAddrBytes, UCHAR *spdData, 
			     UINT32 numBytes);

/******************************************************************************
*
* sysMv64360GetSpdData - read and validate the spd information.
*
* This function reads the contents of the caller specified serial presence
* detect EEPROM and validates the checksum.
*
* RETURNS: TRUE if able to read the SPD, ERROR if not able to read the SPD.
*/

LOCAL STATUS sysMv64360GetSpdData
    (
    UINT32 spdAddr,      /* SROM address for current bank */
    UINT32 offset,       /* first byte of SROM to read */
    UINT32 dataSize,     /* number of SROM bytes to read */
    UCHAR *spdData       /* address of caller's SPD buffer */
    )
    {
    register UCHAR checksum = 0;    /* running checksum */
    register int   index;           /* index into SPD data buffer */

    if ( sysMotI2cRead (spdAddr, offset, SPD_BYTE_ADDR_SZ,
			spdData, dataSize) == OK) 
        {
        for (index = 0; index < SPD_CHECKSUM_INDEX; index++)
            checksum += spdData[index];
        checksum %= 256; 
        if (checksum != spdData[SPD_CHECKSUM_INDEX])
            {
            printf("Checksum for %X is incorrect!\n\r", spdAddr);
            printf("Calculated checksum: %X\n\r", checksum);
            printf("Actual checksum: %X\n\r", spdData[SPD_CHECKSUM_INDEX]);
            }
        return (OK);
        }
    return (ERROR);
    }

/******************************************************************************
*
* sysMv64360SmcShow - Display the SMC registers.
*
* This function reads & displays the SMC registers of the MV64360.
*
*/

STATUS sysMv64360SmcShow ()
    {
    MV64360_SMC * pSmcReg;	     /* pointer to SMC register storage area */
    MV64360_SMC smcReg;		     /* SMC register storage area */
    UINT32 base = MV64360_REG_BASE;  /* base of MV64360 register set */

    pSmcReg = &smcReg;

    /* Read SMC registers into SMC register storage area */

    pSmcReg->csxBaseAddr[0] = MV64360_READ32(base, CPUIF_CS0_BASE_ADDR);
    pSmcReg->csxSz[0] = MV64360_READ32(base, CPUIF_CS0_SIZE);
    pSmcReg->csxBaseAddr[1] = MV64360_READ32(base, CPUIF_CS1_BASE_ADDR);
    pSmcReg->csxSz[1] = MV64360_READ32(base, CPUIF_CS1_SIZE);
    pSmcReg->csxBaseAddr[2] = MV64360_READ32(base, CPUIF_CS2_BASE_ADDR);
    pSmcReg->csxSz[2] = MV64360_READ32(base, CPUIF_CS2_SIZE);
    pSmcReg->csxBaseAddr[3] = MV64360_READ32(base, CPUIF_CS3_BASE_ADDR);
    pSmcReg->csxSz[3] = MV64360_READ32(base, CPUIF_CS3_SIZE);
    pSmcReg->baseAddrEnb = MV64360_READ32(base, CPUIF_BASE_ADDR_ENABLE);
    pSmcReg->sdramCfg = MV64360_READ32(base, DDR_SDRAM_CFG);
    pSmcReg->sdramTmngLo = MV64360_READ32(base, DDR_SDRAM_TMNG_LO);
    pSmcReg->sdramTmngHi = MV64360_READ32(base, DDR_SDRAM_TMNG_HI);
    pSmcReg->sdramAddrCtrl = MV64360_READ32(base, DDR_SDRAM_ADDR_CTRL);
    pSmcReg->sdramOpenPagesCtrl = MV64360_READ32(base, 
	DDR_SDRAM_OPEN_PAGES_CTRL);
    pSmcReg->sdramMode = MV64360_READ32(base, DDR_SDRAM_MODE);
    pSmcReg->sdramIfXbarCtrlLo = MV64360_READ32(base, 
	DDR_SDRAM_IF_XBAR_CTRL_LO);
    pSmcReg->sdramIfXbarCtrlHi = MV64360_READ32(base,
	DDR_SDRAM_IF_XBAR_CTRL_HI);
    pSmcReg->sdramIfXbarTmout = MV64360_READ32(base, DDR_SDRAM_IF_XBAR_TMOUT);
    pSmcReg->sdramErrDataLo = MV64360_READ32(base, DDR_SDRAM_ERR_DATA_LO);
    pSmcReg->sdramErrDataHi = MV64360_READ32(base, DDR_SDRAM_ERR_DATA_HI);
    pSmcReg->sdramErrAddr = MV64360_READ32(base, DDR_SDRAM_ERR_ADDR);
    pSmcReg->sdramReceivedEcc = MV64360_READ32(base, DDR_SDRAM_RCVD_ECC);
    pSmcReg->sdramCalcEcc = MV64360_READ32(base, DDR_SDRAM_CALC_ECC);
    pSmcReg->sdramEccCtrl = MV64360_READ32(base, DDR_SDRAM_ECC_CTRL);
    pSmcReg->sdramEccCntr = MV64360_READ32(base, DDR_SDRAM_ECC_CNTR);

    /* Display SMC register values */

    printf("MV64360 DDR SDRAM Controller Registers:\r\n");
    printf("---------------------------------------\r\n");
    printf("Chip Select 0 Base Addr:  0x%08X\r\n", pSmcReg->csxBaseAddr[0]);
    printf("Chip Select 0 Size:       0x%08X\r\n", pSmcReg->csxSz[0]);
    printf("Chip Select 1 Base Addr:  0x%08X\r\n", pSmcReg->csxBaseAddr[1]);
    printf("Chip Select 1 Size:       0x%08X\r\n", pSmcReg->csxSz[1]);
    printf("Chip Select 2 Base Addr:  0x%08X\r\n", pSmcReg->csxBaseAddr[2]);
    printf("Chip Select 2 Size:       0x%08X\r\n", pSmcReg->csxSz[2]);
    printf("Chip Select 3 Base Addr:  0x%08X\r\n", pSmcReg->csxBaseAddr[3]);
    printf("Chip Select 3 Size:       0x%08X\r\n", pSmcReg->csxSz[3]);
    printf("Base Address Enable:      0x%08X\r\n", pSmcReg->baseAddrEnb);
    printf("SDRAM Configuration:      0x%08X\r\n", pSmcReg->sdramCfg);
    printf("SDRAM Timing (Low):       0x%08X\r\n", pSmcReg->sdramTmngLo);
    printf("SDRAM Timing (High):      0x%08X\r\n", pSmcReg->sdramTmngHi);
    printf("SDRAM Address Control:    0x%08X\r\n", pSmcReg->sdramAddrCtrl);
    printf("SDRAM Open Pages Control: 0x%08X\r\n", pSmcReg->sdramOpenPagesCtrl);
    printf("SDRAM Mode:               0x%08X\r\n", pSmcReg->sdramMode);
    printf("SDRAM IF Xbar Ctrl(Low):  0x%08X\r\n", pSmcReg->sdramIfXbarCtrlLo);
    printf("SDRAM IF Xbar Ctrl(High): 0x%08X\r\n", pSmcReg->sdramIfXbarCtrlHi);
    printf("SDRAM IF Xbar Timeout:    0x%08X\r\n", pSmcReg->sdramIfXbarTmout);
    printf("SDRAM Error Data (Low):   0x%08X\r\n", pSmcReg->sdramErrDataLo);
    printf("SDRAM Error Data (High):  0x%08X\r\n", pSmcReg->sdramErrDataHi);
    printf("SDRAM Error Address:      0x%08X\r\n", pSmcReg->sdramErrAddr);
    printf("SDRAM Received ECC:       0x%08X\r\n", pSmcReg->sdramReceivedEcc);
    printf("SDRAM Calculated ECC:     0x%08X\r\n", pSmcReg->sdramCalcEcc);
    printf("SDRAM ECC Control:        0x%08X\r\n", pSmcReg->sdramEccCtrl);
    printf("SDRAM ECC Counter:        0x%08X\r\n", pSmcReg->sdramEccCntr);
    return (OK);
    }

/******************************************************************************
*
* sysMv64360SpdShow - Display the SPD values.
*
* This function reads & displays the SPD values for the memory devices on
* the board.
*
*/

STATUS sysMv64360SpdShow ()
    {
    UCHAR *  spdPtrs[MV64360_SDRAM_BANKS];    		/* spd buffer ptrs */
    int spd;                         		  	/* Spd index ctr */
    int spdByte;					/* SPD byte index ctr */
    UCHAR   spdData[(MV64360_SDRAM_BANKS / 2) * SPD_SIZE];  /* spd data */
    UCHAR * pBfr = &spdData[0];                         /* temp buffer ptr */
    UCHAR * pData;

    /* loop through each spd device */

    for (spd = 0; spd < MV64360_SDRAM_BANKS; spd +=2)
        {
        spdPtrs[spd] = NULL;

        /* read the spd data into the current buffer and validate */

        if (sysMv64360GetSpdData (SPD_EEPROM_ADDR_0 + spd, 0, SPD_SIZE, pBfr) 
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

    for (spd = 0; spd < MV64360_SDRAM_BANKS; spd += 2)
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
