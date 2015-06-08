/* hawkSmc.c - Support for the Hawk's System Memory Controller */

/* Copyright 1998,1999 Motorola, Inc., All Rights Reserved */

/*
modification history
--------------------
01l,22nov99,srr  Add fix for 256 MB DRAMs needing faster refresh.
01k,17jun99,rhv  Incorporating WRS code review changes.
01j,03may99,rhv  Correcting the omission of 16MX8 SDRAM configuration.
01i,27jan99,rhv  Fixing Bootrom Error code symbol names.
01h,27jan99,rhv  Changing name of ROM startup routines to be consistent with
                 WRS naming (Bootrom instead of RomStart).
01f,26jan99,rhv  Fixing a bug in sysHawkSdramBaseInit.
01e,26jan99,rhv  Adding provisions for forcing the use of default SMC
                 configuration values.
01d,22jan99,rhv  Adding ROM startup error reporting and correcting a file
                 history typo.
01c,13jan99,rhv  Re-ordering file history.
01b,12jan99,rhv  Added SMC routines based on BUG code.
01a,16dec98,rhv  Written (based on sysSpeed.c version 01d.)
*/


/*
DESCRIPTION
This file contains the routines used to calculate the proper configuration
values for the Hawk's System Memory Controller (SMC) using information contained
in the Serial Presence Detect (SPD) EEPROMs. The SPD information is used to
determine memory timing, bank sizes and starting addresses.

CAVEATS:
This code executes very early in the startup sequence (called from romInit.s),
before the image has been copied to RAM (assuming a non-ROM image). As such,
this file must be added to the BOOT_EXTRA list in the Makefile to prevent it
from being compressed during keernel generation. Additionally, extreme caution
must be exercised when modifying these routines to prevent the use of absolute
memory addresses which reference locations in the RAM-relocated image. These
locations are invalid and references to them will cause unpredictable behavior.
Absolute memory addresses are generated by the compiler when referencing tables,
static data structures and global variables. All of these must be avoided. In
some places in the code, nested if-else constructs are used to avoid the jump
table created when a simple switch construct is used. The jump table address was
loaded using an absolute address and the code broke because the execution image
had not been copied to the RAM address produced by the compiler.
*/

/* includes */

#include "vxWorks.h"
#include "config.h"
#include "sysMotVpd.h"
#include "sdramSpd.h"
#include "hawkSmc.h"

/* defines */

#define MHZ	    1000000
#define NANO2SEC    1000000000  /* number of nanoseconds per second */

/* typedefs */

/* globals */

/* locals */

/* forward declarations */

void sysSetRomStartError (UINT32, UINT32);

/* external references */

IMPORT STATUS sysVpdPktInit ( UCHAR, UCHAR, VPD *, VPD_PACKET **, UINT32);
IMPORT STATUS sysVpdPktGet ( UCHAR, UINT32, VPD_PACKET **, VPD_PACKET **);
IMPORT STATUS sysHawkI2cRangeRead (UCHAR, UCHAR, UINT16, UCHAR *);

/******************************************************************************
*
* sysRomGetBusSpd - rom-phase routine to get the speed of the 60x processor bus
*
* This routine reads the VPD and returns the speed (in MHz) of the 60x system
* bus. If the VPD is invalid, a default value is returned. This routine is
* executed before the Hawk System Memory Controller is initialized.
*
* RETURNS: The bus speed (in Hz).
*/

LOCAL UINT sysRomGetBusSpd (void)
    {
    VPD vpd;                                     /* temp vpd storage area */
    VPD_PACKET * vpdPkts[VPD_PKT_LIMIT] = { 0 }; /* temp vpd storage area */
    VPD_PACKET * pVpdPkt;			 /* pointer to bus speed pkt */

    /* read the vpd and find the external clock frequency packet. */

    if ( sysVpdPktInit (VPD_BRD_EEPROM_ADRS, VPD_BRD_OFFSET, &vpd,
                        &vpdPkts[0], VPD_PKT_LIMIT) == OK)
        if ( sysVpdPktGet (VPD_PID_ECS, 0, &vpdPkts[0], &pVpdPkt) == OK )
            if ( pVpdPkt->size == sizeof (UINT32) )
                return (*(UINT32 *)&pVpdPkt->data[0]);

    /* set an error bit so error can be reported once debug port is up */

    sysSetRomStartError(HAWK_MPC_GPREG0_U, BOOTROM_NO_VPD_BUS_SPEED);

    return (DEFAULT_BUS_CLOCK);

    }

/******************************************************************************
*
* sysHawkSdramSpeedInit - routine to initialize the hawk's sdram control reg.
*
* This function's purpose is to initialize the SDRAM Control Register.
* The register value is placed in a structure pointer and returned to
* the calling program.
*
* RETURNS: N/A
*/

LOCAL void sysHawkSdramSpeedInit
    (
    hawkSmc *pSmcReg,	/* points to caller's SMC register storage area */
    UCHAR * spdArray[]	/* array of pointers to SPD buffers */
			/* (odd entries not used ) */
    )
    {
    UCHAR * pData;		/* address of SPD buffer */
    register int bank;		/* Bank index counter */
    int validSpd;		/* SPD Validity flag */
    UINT spdValue;		/* Temp SPD Data Value */
    UINT busSpeed;		/* Bus Speed for the board */
    UINT clockPeriod;		/* NanoSeconds per clock cycle */
    UINT casLatency;		/* Cas Latency of the SDRAM */
    UINT tCycReducedCl;		/* Cycle Time @ reduced CAS Latency */
    UINT sdramTrc;		/* SDRAM Control Reg tRC field */ 
    UINT sdramTras;		/* SDRAM Control Reg tRAS field */
    UINT sdramTrp;		/* SDRAM Control Reg tRP field */
    UINT sdramTrcd;		/* SDRAM Control Reg tRCD field */
    UINT sdramCl3;		/* SDRAM Control Reg cl3 field */
    UINT sdramTdp;		/* SDRAM Control Reg tDP field */
    UINT sdramSwrDpl;		/* SDRAM Control Reg swr_dpl field */
     
    /*
     * Get the BusSpeed in Hertz from the VPD SROM.  First convert the   
     * BusSpeed into nanoseconds, then convert bus_speed to Mhz to
     * store in the CLK Frequency register.
     */

    busSpeed = sysRomGetBusSpd ();

    /* 
     * Calculate the clock period as equal to the number of nano seconds in 
     * one second over the Bus speed (In Hertz) 
     *    ClockPeriod = 1/10(-9) / BusSpeed
     */

    clockPeriod = (NANO2SEC / busSpeed);
    pSmcReg->clkFrequency = (busSpeed / MHZ);

    validSpd = FALSE;
    sdramTras = 0;
    sdramTrp = 0;
    sdramTrcd = 0;
    sdramTdp = 0;
    sdramTrc = 0;
    sdramSwrDpl = 0;
    tCycReducedCl = 0; 
    casLatency = 0xFF;
 
    /*
     * Start with the first Bank and check all Banks for memory.
     */
    for (bank = 0; bank < HAWK_SDRAM_BANKS; bank += 2)
        {
   
        /*
         * Get valid SPD data from the current SPD Address.
         */
        pData = spdArray[bank];
        if (pData != NULL)
            { 
            validSpd = TRUE;

            /*
             * Get the SDRAM Device Attributes CAS latency. The CL
             * parameter must be the greater of all SDRAM CAS latencies.
             */
            spdValue = pData[SPD_CL_INDEX];
            casLatency &= spdValue;
   
            /*
             * Get the SDRAM Minimum Clock Cycle Time at Reduced CAS
             * latency, X-1.  Ignore fractional precision.  Use upper
             * nibble only.
             */

            spdValue = (pData[SPD_TCYC_RCL_INDEX] >> 4);
            if (spdValue > tCycReducedCl)
                tCycReducedCl = spdValue;
            
   
            /*
             * Get the SDRAM Minimum RAS Pulse Width (tRAS).  Ignore
             * fractional precision. Use upper nibble only.
             */
            spdValue = pData[SPD_TRAS_INDEX];
            if (spdValue > sdramTras)
                sdramTras = spdValue;
   
            /*
             * Get the SDRAM Minimum Row Precharge Time (tRP).  Ignore
             * fractional precision. Use upper nibble only.
             */
            spdValue = pData[SPD_TRP_INDEX];
            if (spdValue > sdramTrp)
                sdramTrp = spdValue;
   
            /*
             * Get the SDRAM Minimum RAS to CAS Delay.  Ignore fractional 
             * precision. Use upper nibble only.
             */
            spdValue =pData[SPD_TRCD_INDEX];
            if (spdValue > sdramTrcd) 
                sdramTrcd = spdValue;

            /*
             * Get the SDRAM Refresh Rate.  (Fix for 256 MB DRAMs)
             * If the value is 2, then cut the hawk refresh rate in half.
             */
            if (pData[SPD_REFRESH_RATE_INDEX] == 2)
		pSmcReg->clkFrequency = (busSpeed / MHZ) >> 1;
            }  
        }
   
    if (validSpd)
        {

 
        /* 
         * Determine the CAS latency for cl3, This will be determined by the
         * SDRAM Minimum Cycle Time at reduced CAS Latency, X-1 (Byte 23) and
         * the SDRAM Device Attributes CAS latency (Byte 17).  Initialize the
         * cl3 parameter to 1 for a CAS latency of 3.  If the reduced CAS
         * Latency, X-1, is set in byte 17 then check if the minimum cyle time
         * at CAS latency X-1 is less than or equal to the ClockPeriod.  If it
         * is, set the cl3 parameter to 0 for the reduced CAS latency X-1.
         */
        sdramCl3 = 0x01;
        if (casLatency != 0xFF)
            if (casLatency & 0x02)
                if (tCycReducedCl <= clockPeriod)
                    sdramCl3 = 0x00; 

        /*
         * Calculate the timing for the SDRAM Timing Parameter tRC. Add the
         * tRAS and tRP timing to get tRC.
         */
        if (sdramTras && sdramTrp)
            {
            sdramTrc = (((sdramTras + sdramTrp) +
                          (clockPeriod - 1)) / clockPeriod);
            if (sdramTrc < 6 || sdramTrc > 11)
                sdramTrc = (sdramTrc < 6 ? 6 : 11);
            }
        else
            sdramTrc = SDRAM_TRC_DEFAULT;
   
        /*  
         * tRAS equals the Minimum RAS Pulse Width divided by the ClockPeriod.
         * The Hawk spec requirement for tRAS is 4 to 7 clocks, adjust any value
         * outside of this requirement to the closest spec value.
         */
   
        if (sdramTras)
            {
            sdramTras = ((sdramTras + (clockPeriod - 1)) / clockPeriod);
            if (sdramTras < 4 || sdramTras > 7)
                sdramTras = (sdramTras < 4 ? 4 : 7);
            }
        else
            sdramTras = SDRAM_TRAS_DEFAULT;
   
        /*  
         * tRP equals the Minimum Row Precharge Time divided by the ClockPeriod.
         * The HAWK spec requirement for tRP is 2 or 3 clocks, adjust any 
         * value outside of this requirement to the closest spec value.
         */
        if (sdramTrp)
            {
            sdramTrp = ((sdramTrp + (clockPeriod - 1)) / clockPeriod);
            if (sdramTrp < 2 || sdramTrp > 3)
                sdramTrp = (sdramTrp < 2 ? 2 : 3);
            }
        else
            sdramTrp = SDRAM_TRP_DEFAULT;
   
        /*
         * tRCD equals the Minimum RAS to CAS delay divided by the ClockPeriod.
         * The HAWK spec requirement for tRCD is 2 or 3 clocks, adjust any 
         * value outside of this requirement to the closest spec value
         */
     
        if (sdramTrcd)
            {
            sdramTrcd = ((sdramTrcd + (clockPeriod - 1)) / clockPeriod);
            if (sdramTrcd < 2 || sdramTrcd > 3)
                sdramTrcd = (sdramTrcd < 2 ? 2 : 3);
            }
        else
            sdramTrcd = SDRAM_TRCD_DEFAULT;
   
        /*
         * The tDP cannot be determined by the SDRAM SPD definitions.  It should
         * be set to the default.
         */
        sdramTdp = SDRAM_TDP_DEFAULT;
   
        /*
         * Initialize the SDRAM Timing Parameter swr_dpl. This register setting
         * causes the HAWK SMC to always wait until four clocks after the write
         * command portion of a single write before allowing a precharge to
         * occur. This can be cleared by software if unecessary.
         */
        sdramSwrDpl = SDRAM_SWR_DPL_DEFAULT; 
     
        /*
         * Load the SDRAM Control Register.  The Hawk Spec does not define the
         * use of all timing parameter bits.  Mask off those not used and shift
         * the bits to the correct location in the register.  The tDP parameter
         * uses the upper bit and requires a right shift prior to storing in the
         * register.
         */
  
        pSmcReg->sdramCtrl = 
            (((sdramCl3 & SDRAM_CL3_MASK) << SDRAM_CL3_SHIFT) |
             ((sdramTrc & SDRAM_TRC_MASK) << SDRAM_TRC_SHIFT) |
             ((sdramTras & SDRAM_TRAS_MASK) << SDRAM_TRAS_SHIFT) |
             ((sdramSwrDpl & SDRAM_SWR_DPL_MASK) << SDRAM_SWR_DPL_SHIFT) |
             (((sdramTdp & SDRAM_TDP_MASK) >> 1) << SDRAM_TDP_SHIFT) |
             ((sdramTrp & SDRAM_TRP_MASK) << SDRAM_TRP_SHIFT) |
             ((sdramTrcd & SDRAM_TRCD_MASK) << SDRAM_TRCD_SHIFT));

        }
    else
       {
       /* If the board does not have any valid SPD data for the SDRAM 
        * then we should default to the basic configuration that got
        * us to this point in the Firmware.
        */

       pSmcReg->sdramCtrl = *((UINT32 *)HAWK_SMC_SDRAM_CNTRL);

       sysSetRomStartError (HAWK_MPC_GPREG0_U, BOOTROM_DEFAULT_SMC_TIMING);
       }

    return;
    }

/******************************************************************************
*
* sysHawkSdramBaseInit - initialize the base address of the hawk's sdram banks.
*
* This function's purpose is to initialize the SDRAM base address registers.
*
* RETURNS: The total size of all banks.
*
*/

LOCAL UINT sysHawkSdramBaseInit
    (
    hawkSmc *pSmcReg	/* points to caller's SMC register storage area */
    )
    {
    register UINT sizeCode;	/* Current size code */
    register UINT bank;		/* Bank Index counter */
    register UINT shift;	/* Bank index into the register */
    register UINT baseAddr;	/* Base Address variable */
    UINT *pSdramAttr;		/* Pointer to the SDRAM attribute */
    UINT *pSdramBase;		/* Pointer to the SDRAM base */
    UINT attribute;		/* Attribute of a single bank */
     
    baseAddr = 0x00000000;
    pSmcReg->sdramBaseA = 0;
    pSmcReg->sdramBaseE = 0;

    /*
     * Starting with the greatest SDRAM block size down to the smallest
     * SDRAM block size, load the attributes for each bank and compare.
     * If the attribute of the bank is equal to the block size set the
     * base address for that bank equal to the next possible base address.
     */
  
    for (sizeCode = SDRAM_SIZE_512_64MX4; sizeCode != SDRAM_SIZE_0; sizeCode--)
        {
        for (bank = 0; bank < HAWK_SDRAM_BANKS; bank++)
            {
        
            /*
             * The Hawk SMC has 2 seperate registers for both the attribute and
             * the base address.  The HAWK SDRAM attributes registers are not
             * contiguous.  Banks A,B,C,D are at FEF80010 and Banks E,F,G,H are
             * at FEF800C0.
             */
            pSdramAttr = (bank < 4 ? &pSmcReg->sdramAttrA :
                                     &pSmcReg->sdramAttrE);

            pSdramBase = (bank < 4 ? &pSmcReg->sdramBaseA :
                                     &pSmcReg->sdramBaseE);

            /*
             * Compute the shift value for the byte attribute of the
             * appropriate bank in the SDRAM registers 
             */
            shift = ((3 - (bank % 4)) * 8);

            /*
             * if the attribute matches the block size then set the base
             * address equal to the 8 most significant bits of the base
             * address.  Increment the current base address.
             */
            attribute = ((*pSdramAttr & (SDRAM_SIZE_MASK << shift)) >> shift);
            if (attribute == sizeCode)
                {
                *pSdramBase |= (((baseAddr >> SDRAM_BASE_ASHIFT) << shift) &
                              (SDRAM_BASE_MASK << shift));

                if (sizeCode == SDRAM_SIZE_32_4MX16)
                    baseAddr += 0x02000000;	/* 32MB */

                else if ((sizeCode == SDRAM_SIZE_64_8MX16) ||
                        (sizeCode == SDRAM_SIZE_64_8MX8))
                    baseAddr += 0x04000000;	/* 64MB */

                else if ((sizeCode == SDRAM_SIZE_128_16MX16) ||
		        (sizeCode == SDRAM_SIZE_128_16MX8)   ||
                        (sizeCode == SDRAM_SIZE_128_16MX4))
		    baseAddr += 0x08000000;	/* 128MB */

                else if ((sizeCode == SDRAM_SIZE_256_32MX8) ||
                        (sizeCode == SDRAM_SIZE_256_32MX4))
                    baseAddr += 0x10000000;	/* 256MB */

                else if (sizeCode == SDRAM_SIZE_512_64MX4)
		    baseAddr += 0x20000000;	/* 512MB */
                }
            }
        }
    return (baseAddr);
    }  

/******************************************************************************
*
* calcBankSize - calculate the size of a sdram bank
*
* This function calculates the Hawk bank size code for a sdram bank using the
* spd info.
*
* RETURNS: Hawk SMC bank size code or 0 if invalid bank size.
*
*/
LOCAL UINT32 calcBankSize
    (
    UCHAR *spdData	/* pointer to SPD buffer for current bank */
    )
    {
    UINT32 bankSize;	/* calculated size of current bank in bytes */
    UINT32 sdramSize;	/* SMC size code for current bank */
    UINT32 devWidth;	/* width of the devices in the current bank */

    /*
     * Calculate the SDRAM Bank Size using the formula:
     * BankSize = (Total Row Addresses * Total Column Addresses *
     *              Number Device Banks * Data Width in Bytes);
     */

    bankSize = ((1 << spdData[SPD_ROW_ADDR_INDEX]) *
                 (1 << spdData[SPD_COL_ADDR_INDEX]) *
                  spdData[SPD_DEV_BANKS_INDEX] * 8);

    /*
     * Get the Primary Device Width from the SDRAM SPD (Byte 13).
     * Use it to determine the device width attributes required by
     * the Hawk specification.
     */

    devWidth = spdData[SPD_DEV_WIDTH_INDEX];

    /*
     * use the bank size and device width to select the proper smc bank size
     * code. NOTE: A switch statement cannot be used because the absolute
     * addresses entered into the resulting jump table resolve to RAM addresses
     * and this code is executed before the image has been moved to RAM. the
     * nested if-else structure produces PC-relative branches which will always
     * work.
     */

    if (bankSize == 0x02000000) /* 32 Meg */
        if (devWidth == 16)
            sdramSize = SDRAM_SIZE_32_4MX16;
        else
            sdramSize = SDRAM_SIZE_0;	/* invalid */

    else if (bankSize == 0x4000000) /* 64 Meg */
        if (devWidth == 8)
            sdramSize = SDRAM_SIZE_64_8MX8;
        else if (devWidth == 16)
            sdramSize = SDRAM_SIZE_64_8MX16;
        else
            sdramSize = SDRAM_SIZE_0;	/* invalid */

    else if (bankSize == 0x8000000) /* 128 Meg */
        if (devWidth == 4)
            sdramSize = SDRAM_SIZE_128_16MX4;
        else if (devWidth == 8)
            sdramSize = SDRAM_SIZE_128_16MX8;
        else if (devWidth == 16)
            sdramSize = SDRAM_SIZE_128_16MX16;
        else
            sdramSize = SDRAM_SIZE_0;	/* invalid */

    else if (bankSize == 0x10000000) /* 256 Meg */
        if (devWidth == 4)
            sdramSize = SDRAM_SIZE_256_32MX4;
        else if (devWidth == 8)
            sdramSize = SDRAM_SIZE_256_32MX8;
        else
            sdramSize = SDRAM_SIZE_0;	/* invalid */

    else if (bankSize == 0x20000000) /* 512 Meg */
        if (devWidth == 4)
            sdramSize = SDRAM_SIZE_512_64MX4;
        else
            sdramSize = SDRAM_SIZE_0;	/* invalid */
    else
        sdramSize = SDRAM_SIZE_0;	/* invalid */

    return (sdramSize);
    }

/******************************************************************************
*
* sysHawkSdramSizeInit - initialize the SDRAM Size Attributes
*
* This function's purpose is to determine the correct size attributes
* for all banks and to set the ram enable for banks consisting of
* a valid memory configuration.
*
* RETURNS: N/A
*
*/
LOCAL void sysHawkSdramSizeInit
    (
    hawkSmc *pSmcReg,	/* points to caller's SMC register storage area */
    UCHAR * spdArray[]	/* array of pointers to SPD buffers */
                        /* (odd entries not used) */
    )
    {
    UCHAR * pData;		/* SPD pointer for current bank */
    register int bank;          /* Bank index counter */
    register int block;         /* Block index counter */
    register int shift;         /* Shift index to register */
    register int dimmBank;      /* DimmBank index counter */
    int validSpd;               /* SPD Validity flag */
    UINT numDimmBanks;          /* Number of DIMM Banks supported */
    UINT sdramSize;             /* SDRAM Size for the bank */
    UINT *pSdramAttr;		/* Pointer to the attributes register */

    validSpd = FALSE;
    pSmcReg->sdramAttrA = 0;
    pSmcReg->sdramAttrE = 0;
 
    /*
     * Fill the attributes registers with bank data from the SPD devices.
     */

    for (bank = 0; bank < HAWK_SDRAM_BANKS; bank += 2)
        {
        if ((pData = spdArray[bank]) != NULL)
            {
            validSpd = TRUE;

            /*
             * Get the number of DIMM banks supported by this SPD.  The
             * Hawk supports upto 2 DIMM Banks for each SPD.  Any number
             * other than 1 or 2 should be considered erroneous and reset
             * to 1 for this device.
             */

            numDimmBanks = pData[SPD_NUM_DIMMBANKS_INDEX];
            if (numDimmBanks < 1 || numDimmBanks > 2)
                numDimmBanks = 1;

            /*
             * Get the size attributes for the Bank.
             */
            sdramSize = calcBankSize(pData);

            /*
             * Load the SDRAM Attributes and set the bank enable for any block
             * with a size greater than Zero. The Hawk has two 32 bit registers
             * to store the 8 possible blocks of data.  Banks 0 to 3 go in the
             * first register and Banks 4 to 7 go in the second register.
             */
            pSdramAttr = (bank < 4) ? &pSmcReg->sdramAttrA :
                                      &pSmcReg->sdramAttrE;

            /*
             * Update the Hawk register for the number of DIMM Banks
             * supported by this size attribute information.
             */
            for (dimmBank = 0; dimmBank < numDimmBanks; dimmBank++)
                {
                block = ((bank % 4) + dimmBank);
                shift = ((3 - block) * 8);
                *pSdramAttr |= (((SDRAM_EN | sdramSize) &
                                  SDRAM_ATTR_MASK) << shift);
                }
            }
        }

    /*
     * Verify that we were able to read SDRAM size information from an
     * SPD device.  If unable then reset the registers to the pre-init
     * values and continue.
     */
    if (!validSpd)
        {
        pSmcReg->sdramAttrA = *((UINT32 *)HAWK_SMC_SDRAM_ATTR_AD);
        pSmcReg->sdramAttrE =  *((UINT32 *)HAWK_SMC_SDRAM_ATTR_EH);
        }
    }

/******************************************************************************
*
* getSpdData - read and validate the spd information.
*
* This function reads the contents of the caller specified serial presence
* detect EEPROM and validates the checksum.
*
* RETURNS: TRUE if the SPD contents are valid, FALSE if not.
*
*/
STATUS sysGetSpdData
    (
    UCHAR spdAddr,	/* SROM address for current bank */
    UCHAR offset,	/* first byte of SROM to read */
    UINT16 dataSize,	/* number of SROM bytes to read */
    UCHAR *spdData	/* address of caller's SPD buffer */
    )
    {
    register UCHAR checksum = 0;	/* running checksum */
    register int   index;		/* index into SPD data buffer */

    if ( sysHawkI2cRangeRead (spdAddr, offset, dataSize, spdData) == OK)
        {
        for (index = 0; index < SPD_CHECKSUM_INDEX; index++)
            checksum += spdData[index];

        checksum %= 256; 
        if (checksum == spdData[SPD_CHECKSUM_INDEX])
            return (OK);
        }
    return (ERROR);
    }

/******************************************************************************
*
* sysHawkParamConfig - calculate the proper hawk smc initialization values.
*
* This function reads the serial presence detect EEPROM(s) and calculates the
* proper values for configuring the hawk smc.
*
* RETURNS: Size of memory configured.
*
*/

UINT32 sysHawkParamConfig
    (
    hawkSmc *pSmcReg	/* points to caller's SMC register storage area */
    )
    {

    /*
     * note: the hawk banks are arranged in pairs with one spd device per
     * bank pair. therefore only the even numbered entries in the spdPtrs
     * array are used.
     */
    UCHAR * spdPtrs[HAWK_SDRAM_BANKS];	/* spd buffer ptrs */
    register int spd;			/* Spd index counter */

#ifndef BYPASS_SPD
    UCHAR   spdData[HAWK_SDRAM_BANKS / 2 * SPD_SIZE];	/* spd data */
    UCHAR * pBfr = &spdData[0];				/* temp buffer ptr */
#endif

    /* loop through each spd device */

    for (spd = 0; spd < HAWK_SDRAM_BANKS; spd +=2)
        {
        spdPtrs[spd] = NULL;

#ifndef BYPASS_SPD

        /* read the spd data into the current buffer and validate */

        if (sysGetSpdData (SPD_EEPROM_ADRS + spd, 0, SPD_SIZE, pBfr) == OK)
            {

            /* save current buffer address and advance to the next buffer */

            spdPtrs[spd] = pBfr;
            pBfr += SPD_SIZE;

            }
#endif
        }

    /* calculate the smc intialization parameters */
    sysHawkSdramSpeedInit (pSmcReg, &spdPtrs[0]);
    sysHawkSdramSizeInit (pSmcReg, &spdPtrs[0]);
    return (sysHawkSdramBaseInit (pSmcReg));

    }

/******************************************************************************
*
* sysSetRomStartError - Set a RomStartup error bit.
*
* This routine sets one or more error flags in the RomStartup error registers.
* The error registers are the 4 general purpose registers available in the
* Hawk's Pci-Host Bridge (PHB). This mechanism allows low-level code to save any
* errors detected before a debug console is available. Once the debug console
* has been configured, these error flags are logged to the debug console to aid
* in system debug.
*
* RETURNS: N/A
*
*/

void sysSetRomStartError
    (
    UINT32 offset,	/* offset of the selected Hawk register */
    UINT32 value	/* bit(s) to merge into register contents */
    )
    {
    UINT32 * adrs;

    adrs = (UINT32 *)(HAWK_PHB_BASE_ADRS + offset);

    *adrs |= value;

    }
