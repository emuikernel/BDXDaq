/* harrierSmcShow.c - Harrier System Memory Controller Show Routines. */

/* Copyright 1984-2001 Wind River Systems, Inc. */
/* Copyright 1999-2001 Motorola, Inc., All Rights Reserved */

/*
modification history
--------------------
01b,17oct00,krp  display of SMC SDRAM addressing & control settings
01a,31aug00,dmw  Written (from verison 01b of mcpn765/hawkSmcShow.c).
*/

/*
DESCRIPTION
This file contains the Harrier System Memory Controller (SMC) Show utility
routines. To use these routines, define INCLUDE_SHOW_ROUTINES in config.h.
*/

#include "vxWorks.h"
#include "config.h"
#include "prpmc800.h"
#include "stdio.h"
#include "harrierSmc.h"


/*******************************************************************************
*
* smcPrintGeneralCtrl - print the smc sdram General Control settings.
*
* This routine prints the harrier sdram memory General Control values
*
* RETURNS: N/A
*/

void smcPrintGeneralCtrl
    (
    HARRIER_SMC *pSmcReg	/* harrier register image storage */
    )
    {
    UINT32 sdramGenCtrl;

    sdramGenCtrl = pSmcReg->sdramGeneralControl;
    printf ("\n SDRAM General Control info...\n");

    printf ("  Multiply Refresh Rate = %2u \n",
               (sdramGenCtrl & HARRIER_SDGC_MXRR));

    if (sdramGenCtrl & HARRIER_SDGC_DREF)
        printf ("  Disable Refresh is Set \n");
    else
        printf ("  Disable Refresh is Cleared \n");

    if (sdramGenCtrl & HARRIER_SDGC_DERC)
        printf ("  Disable Error Correction is Set \n");
    else
        printf ("  Disable Error Correction is Cleared \n");

    if (sdramGenCtrl & HARRIER_SDGC_RWCB)
        printf ("  Read/Write Checkbits is Set \n");
    else
        printf ("  Read/Write Checkbits is Cleared \n");

    if (sdramGenCtrl & HARRIER_SDGC_ENRV)
        printf ("  Enable Reset Vector is Set \n");
    else
        printf ("  Enable Reset Vector is Cleared \n");

    if (sdramGenCtrl & HARRIER_SDGC_SWVT)
        printf ("  Swap Vector Table is Set \n");
    else
        printf ("  Swap Vector Table is Cleared \n");
    }


/*******************************************************************************
*
* smcPrintTiming - read and print the smc sdram timing information.
*
* This routine prints the harrier sdram memory controller timing values in bus
* clocks.
*
* RETURNS: N/A
*/

void smcPrintTiming
    (
    HARRIER_SMC *pSmcReg	/* harrier register image storage */
    )
    {
    UINT32 sdramCtrl;
    UINT32 tempVal;

    sdramCtrl = pSmcReg->sdramTimingControl;

    printf ("\n SDRAM Timing info...\n");
    printf ("  Memory clock frequency = %u\n", MEMORY_BUS_SPEED);

    /* the CL3 bit selects between 3 clocks (set) or 2 clocks (cleared). */

    printf ("  CAS Latency = %2u clocks.\n",
            ((sdramCtrl >> HARRIER_SDTC_CL3_SHIFT) & HARRIER_SDTC_CL3_MASK));

    /* the TRC timing parmater */

    tempVal = (sdramCtrl >> HARRIER_SDTC_TRC_SHIFT) & HARRIER_SDTC_TRC_MASK;
    if (tempVal < 4)
       tempVal += 8; 
    else    
        {
        switch (tempVal)
            {
            case 4:
            case 5:
                tempVal = 0xff;          /* reserved value */
                break;
            default: 
                break;
            }
        }
    printf ("  tRC  = %2u clocks.\n", tempVal);

    /* the TRAS timing parmater */

    tempVal = (sdramCtrl >> HARRIER_SDTC_TRAS_SHIFT) & 
                            HARRIER_SDTC_TRAS_MASK;
    tempVal += 4;
    printf ("  tRAS  = %2u clocks.\n", tempVal);

    /* the tDP bit selects between 2 clocks (set) or 1 clock (cleared). */

    tempVal = (sdramCtrl >> HARRIER_SDTC_TDP_SHIFT) &
                            HARRIER_SDTC_TDP_SHIFT_MASK;
    tempVal = (tempVal == 0 ? 1 : 2);
    printf ("  tDP  = %2u clocks.\n", tempVal);

    /* the tRP bit selects between 3 clocks (set) or 2 clocks (cleared). */

    tempVal = (sdramCtrl >> HARRIER_SDTC_TRP_SHIFT) &
                            HARRIER_SDTC_TRP_SHIFT_MASK;
    tempVal = (tempVal == 0 ? 2 : 3);
    printf ("  tRP = %2u clocks.\n", tempVal);

    /* the tRCD bit selects between 3 clocks (set) or 2 clocks (cleared). */

    tempVal = (sdramCtrl >> HARRIER_SDTC_TRCD_SHIFT) & 
                            HARRIER_SDTC_TRCD_SHIFT_MASK;
    tempVal = (tempVal == 0 ? 2 : 3);
    printf ("  tRCD = %2u clocks.\n", tempVal);

    }


/*******************************************************************************
*
* smcPrintBankInfo - read and print the smc sdram bank information.
*
* This routine prints the size and starting address of each enabled harrier
* sdram bank.
*
* RETURNS: N/A
*/

void smcPrintBankInfo
    (
    HARRIER_SMC *pSmcReg	/* harrier register image storage */
    )
    {
    UINT32 *pSdramBase;     /* Pointer to the SDRAM base */
    UINT32 attribute;       /* Attribute of a single bank */
    UCHAR  bankChar;        /* bank letter code (A through H) */
    UINT32 size;            /* Current size */
    UINT32 base;            /* Current base */
    UINT32 bank;            /* Bank Index counter */

    pSdramBase = &pSmcReg->sdramBlkAddrA;
    printf ("\n SDRAM Block Addressing info...\n");

    for (bank = 0; bank < HARRIER_SDRAM_BANKS; bank++, pSdramBase++)
        {
        bankChar = (UCHAR)((UINT32)'A' + bank);

        attribute = *pSdramBase;
        if (attribute & (UINT32)HARRIER_SDBA_ENB)
            {
            switch (attribute & (UINT32)HARRIER_SDBA_SIZE_MASK)   
                {
                case HARRIER_SDBA_SIZE_32_4MX16:    /* DRAM Bank size 32MB */
                    size = 32;
                    break;
                case HARRIER_SDBA_SIZE_64_8MX8:      /* DRAM Bank size 64MB */
                case HARRIER_SDBA_SIZE_64_8MX16:     /* DRAM Bank size 64MB */
                    size = 64;
                    break;
                case HARRIER_SDBA_SIZE_128_16MX4:    /* DRAM Bank size 128MB */
                case HARRIER_SDBA_SIZE_128_16MX8:    /* DRAM Bank size 128MB */
                case HARRIER_SDBA_SIZE_128_16MX16:   /* DRAM Bank size 128MB */
                    size = 128;
                    break;
                case HARRIER_SDBA_SIZE_256_32MX4:    /* DRAM Bank size 256MB */
                case HARRIER_SDBA_SIZE_256_32MX8:    /* DRAM Bank size 256MB */
                case HARRIER_SDBA_SIZE_256_32MX16:   /* DRAM Bank size 256MB */
                    size = 256;
                    break;
                case HARRIER_SDBA_SIZE_512_64MX4:    /* DRAM Bank size 512MB */
                case HARRIER_SDBA_SIZE_512_64MX8:    /* DRAM Bank size 512MB */
                case HARRIER_SDBA_SIZE_512_64MX16:   /* DRAM Bank size 512MB */
                    size = 512;
                    break;
                case HARRIER_SDBA_SIZE_1024_128MX4:  /* DRAM Bank size 1024MB */
                case HARRIER_SDBA_SIZE_1024_128MX8:  /* DRAM Bank size 1024MB */
                    size = 1024;
                    break;
                case HARRIER_SDBA_SIZE_2048_256MX4:  /* DRAM Bank size 2048MB */
                    size = 2048;
                    break;
                default:
                    size = 0;
                    break;
                }

            base = (*pSdramBase & HARRIER_SDBA_BASE_MASK) >>
                                  HARRIER_SDBA_BASE_SHIFT;

            printf("  Bank %c size = %3u MB (0x%08x), base address = 0x%08x.\n",
                      bankChar, size, (size * 1024 * 1024), base); 
            }
        else
            printf ("  Bank %c disabled.\n", bankChar);
        }
    }


/*******************************************************************************
*
* smcReadConfig - read the contents of the harrier smc sdram controller
*                 registers.
*
* This routine reads the contents of the smc sdram controller registers and
* stores the values into a caller provided storage area.
*
* RETURNS: N/A
*
*/

void smcReadConfig
    (
    HARRIER_SMC *pSmcReg        /* harrier register image storage */
    )
    {
    int bank;
    UINT32 *offset = (UINT32 *) HARRIER_REG_SDRAM_BLOCK_ADDRESSING_A;
    UINT32 *pSdramBlkAddr = &pSmcReg->sdramBlkAddrA;
 
    for (bank = 0; bank < HARRIER_SDRAM_BANKS; bank++)
        {
        *pSdramBlkAddr++ = *offset++;
        }

    /*  SDRAM General Control settings */

    *pSdramBlkAddr++ = * (UINT32 *) HARRIER_SDRAM_GENERAL_CONTROL_REG;  

    /*  SDRAM Timing Control settings */

    *pSdramBlkAddr = * (UINT32 *) HARRIER_SDRAM_TIMING_CONTROL_REG;
    }


/*******************************************************************************
*
* smcShow - display the SMC configuration data
*
* This routine reads and displays the Harrier SMC SDRAM timing and bank
* configuration information.
*
* RETURNS: OK, if successful or ERROR if unsuccessful.
*
* SEE ALSO: N/A
*/

void smcShow (void)
    {
    HARRIER_SMC smcRegImage;

    /* read the harrier's sdram control registers into a local storage area. */

    smcReadConfig (&smcRegImage);

    /* display the sdram timing and bank size info. */

    smcPrintGeneralCtrl(&smcRegImage);
    smcPrintTiming (&smcRegImage);
    smcPrintBankInfo (&smcRegImage);

    }
