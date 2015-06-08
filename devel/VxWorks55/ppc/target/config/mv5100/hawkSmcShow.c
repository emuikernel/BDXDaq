/* hawkSmcShow.c - Hawk System Memory Controller Show Routines. */

/* Copyright 1984-1999 Wind River Systems, Inc. */
/* Copyright 1999-2000 Motorola, Inc., All Rights Reserved */

/*
modification history
--------------------
01a,13mar00,dmw     Written (from verison 01a of mcpn765/smcShow.c).
*/

/*
DESCRIPTION
This file contains the Hawk System Memory Controller (SMC) Show utility
routines. To use these routines, define INCLUDE_SHOW_ROUTINES in config.h.
*/

#include "vxWorks.h"
#include "config.h"
#include "stdio.h"
#include "hawkSmc.h"


/*******************************************************************************
*
* smcPrintTiming - read and print the smc sdram timing information.
*
* This routine prints the hawk sdram memory controller timing values in bus
* clocks.
*
* RETURNS: N/A
*/

void smcPrintTiming
    (
    HAWK_SMC *pSmcReg	/* hawk register image storage */
    )
    {
    UINT32 sdramCtrl;
    UINT32 temp;

    sdramCtrl = pSmcReg->sdramCtrl;

    printf ("\nSMC SDRAM Timing info...\n");
    printf ("  Memory clock frequency = %u\n", MEMORY_BUS_SPEED);

    /* the CL3 bit selects between 3 clocks (set) or 2 clocks (cleared). */

    printf ("  CAS Latency = %2u clocks.\n",
            ((sdramCtrl >> SDRAM_CL3_SHIFT) & SDRAM_CL3_MASK) ? 3 : 2);

    /*
     * extract the tRC parameter and convert it to bus clocks.
     * (codes 0-3 = 8-11 clocks respectively, while 6 = 6 clocks and
     * 7 = 7 clocks. codes 4 & 5 are reserved).
     */

    temp = (sdramCtrl >> SDRAM_TRC_SHIFT) & SDRAM_TRC_MASK;
    if (temp < 4)
        temp += 8;
    printf ("  tRC  = %2u clocks.\n", temp);

    /*
     * extract the tRAS parameter and convert it to bus clocks. (0 = 4 clocks,
     * 1 = 5 clocks, etc).
     */ 

    temp = ((sdramCtrl >> SDRAM_TRAS_SHIFT) & SDRAM_TRAS_MASK) + 4;
    printf ("  tRAS = %2u clocks.\n", temp);

    /* the tDP bit selects between 2 clocks (set) or 1 clock (cleared). */

    printf ("  tDP  = %2u clocks.\n",
            ((sdramCtrl >> SDRAM_TDP_SHIFT) & SDRAM_TDP_MASK) ? 2 : 1);

    /* the tRP bit selects between 3 clocks (set) or 2 clocks (cleared). */

    printf ("  tRP  = %2u clocks.\n",
            ((sdramCtrl >> SDRAM_TRP_SHIFT) & SDRAM_TRP_MASK) ? 3 : 2);

    /* the tRCD bit selects between 3 clocks (set) or 2 clocks (cleared). */

    printf ("  tRCD = %2u clocks.\n",
            ((sdramCtrl >> SDRAM_TRCD_SHIFT) & SDRAM_TRCD_MASK) ? 3 : 2);
    }


/*******************************************************************************
*
* smcPrintBankInfo - read and print the smc sdram bank information.
*
* This routine prints the size and starting address of each enabled hawk sdram
* bank.
*
* RETURNS: N/A
*/

void smcPrintBankInfo
    (
    HAWK_SMC *pSmcReg	/* hawk register image storage */
    )
    {
    UINT32 *pSdramAttr;     /* Pointer to the SDRAM attribute */
    UINT32 *pSdramBase;     /* Pointer to the SDRAM base */
    UINT32 attribute;       /* Attribute of a single bank */
    UCHAR  bankChar;        /* bank letter code (A through H) */
    UINT32 size;            /* Current size */
    UINT32 base;            /* Current base */
    UINT32 bank;            /* Bank Index counter */
    UINT32 shift;           /* Bank index into the register */

    printf ("\nSMC bank info...\n"); 

    for (bank = 0; bank < HAWK_SDRAM_BANKS; bank++)
        {
        bankChar = (UCHAR)((UINT32)'A' + bank);

        pSdramAttr = (bank < 4 ? &pSmcReg->sdramAttrA :
                                 &pSmcReg->sdramAttrE);

        pSdramBase = (bank < 4 ? &pSmcReg->sdramBaseA :
                                 &pSmcReg->sdramBaseE);

        /*
         * Compute the shift value for the byte attribute of the
         * appropriate bank in the SDRAM registers
         */

        shift = ((3 - (bank % 4)) * 8);
        attribute = ((*pSdramAttr >> shift) & SDRAM_ATTR_MASK);

        if (attribute & SDRAM_EN)
            {
            switch (attribute & SDRAM_SIZE_MASK)
                {
                case SDRAM_SIZE_32_4MX16:     /* DRAM Bank size 32MB */
                    size = 32;
                    break;
                case SDRAM_SIZE_64_8MX8:      /* DRAM Bank size 64MB */
                case SDRAM_SIZE_64_8MX16:     /* DRAM Bank size 64MB */
                    size = 64;
                    break;
                case SDRAM_SIZE_128_16MX4:    /* DRAM Bank size 128MB */
                case SDRAM_SIZE_128_16MX8:    /* DRAM Bank size 128MB */
                case SDRAM_SIZE_128_16MX16:   /* DRAM Bank size 128MB */
                    size = 128;
                    break;
                case SDRAM_SIZE_256_32MX4:    /* DRAM Bank size 256MB */
                case SDRAM_SIZE_256_32MX8:    /* DRAM Bank size 256MB */
                    size = 256;
                    break;
                case SDRAM_SIZE_512_64MX4:    /* DRAM Bank size 512MB */
                    size = 512;
                    break;
                default:
                    size = 0;
                    break;
                }
            base = (((*pSdramBase >> shift) & SDRAM_BASE_MASK)
                   << SDRAM_BASE_ASHIFT);

            printf (
                   "  Bank %c size = %3u MB (0x%08x), base address = 0x%08x.\n",
                    bankChar, size, (size * 1024 * 1024), base); 
            }
        else
            printf ("  Bank %c disabled.\n", bankChar);
        }
    }


/*******************************************************************************
*
* smcReadConfig - read the contents of the hawk smc sdram controller registers.
*
* This routine reads the contents of the smc sdram controller registers and
* stores the values into a caller provided storage area.
*
* RETURNS: N/A
*
*/

void smcReadConfig
    (
    HAWK_SMC *pSmcReg	/* hawk register image storage */
    )
    {
    pSmcReg->sdramAttrA = *(UINT32 *)HAWK_SMC_SDRAM_ATTR_AD;
    pSmcReg->sdramBaseA = *(UINT32 *)HAWK_SMC_SDRAM_BASE_AD;
    pSmcReg->sdramAttrE = *(UINT32 *)HAWK_SMC_SDRAM_ATTR_EH;
    pSmcReg->sdramBaseE = *(UINT32 *)HAWK_SMC_SDRAM_BASE_EH;
    pSmcReg->sdramCtrl  = *(UINT32 *)HAWK_SMC_SDRAM_CNTRL;
    }


/*******************************************************************************
*
* smcShow - display the SMC configuration data
*
* This routine reads and displays the Hawk SMC SDRAM timing and bank
* configuration information.
*
* RETURNS: OK, if successful or ERROR if unsuccessful.
*
* SEE ALSO: N/A
*/

void smcShow (void)
    {
    HAWK_SMC smcRegImage;

    /* read the hawk's sdram control registers into a local storage area. */

    smcReadConfig (&smcRegImage);

    /* display the sdram timing and bank size info. */

    smcPrintTiming (&smcRegImage);
    smcPrintBankInfo (&smcRegImage);

    }
