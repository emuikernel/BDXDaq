/******************************************************************************
*              (c), Copyright, FORCE COMPUTERS INDIA Limited                  *
*                                                                             *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF FORCE COMPUTERS, INC.        *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT *
* OF FORCE OR ANY THIRD PARTY. FORCE RESERVES THE RIGHT AT ITS SOLE DISCRETION*
* TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO FORCE COMPUTERS.       *
* THIS CODE IS PROVIDED "AS IS". FORCE COMPUTERS MAKES NO WARRANTIES, EXPRESS,*
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.  *
*******************************************************************************

*******************************************************************************
* gtInitLib.c - GT Initialization library file
*
* DESCRIPTION:
*       None.
*
* DEPENDENCIES:
*       None.
*
******************************************************************************/

/* includes */
#include "gtSysGal.h"
#include "gtMemory.h"
#include "gtSram.h"
#include "gtPci.h"
#include "gtInitLib.h"
#include "frcBoardId.h"
#include "config.h"

#ifdef PMC280_DUAL_CPU
#include "gtSmp.h"
#endif /* PMC280_DUAL_CPU */

#ifdef PMC280_DEBUG_UART_VX
#include "dbgprints.h"
#endif /* PMC280_DEBUG_UART_VX */

/* defines */


/* typedefs */


/* locals   */

#ifdef PMC280_DUAL_CPU
#ifdef PMC280_PCI_BOOT
extern void frcInitPciBootSyncLocation();
extern int frcSetPciBootSyncLocation();
extern int frcReadPciBootSyncLocation();
#endif /* PMC280_PCI_BOOT */

#ifdef PMC280_BOOTROM_BOOT
extern void frcInitBootSyncLocation();
extern int frcSetBootSyncLocation();
extern int frcReadBootSyncLocation();
#endif /* PMC280_BOOTROM_BOOT */

/*sergey*/
#ifdef PMC280_USERFLASH_BOOT
extern void frcInitUserBootSyncLocation();
extern int frcSetUserBootSyncLocation();
extern int frcReadUserBootSyncLocation();
#endif

IMPORT void frcPermitCPU1ToBoot(void);
#endif /* PMC280_DUAL_CPU */

IMPORT void flashAutoDetect (void);
IMPORT UINT8 frcGetExtSramSize(void);
IMPORT UINT8 frcGetBank0MemSize(void);
IMPORT UINT8 frcGetBank1MemSize(void);

/*******************************************************************************
* gtInit - GT Initialization routine.
*
* DESCRIPTION:
*       This routine aborts the GT magor facilities activities and masks all 
*       interrupts in order to ensure quiet state in early boot stages. 
*       The following facilities are stopped: 
*       watchdog, timer/counter, DMA engines 
*
*       This routine also makes the following initialization:
*       1) In case the GT internal PCI arbiter is included, enable it.
*       2) Establish the SRAM size on board and adjust the PCI BARs according
*          to it. Any other GT initialization which is called in the sysHwInit
*          phase can be located here.
*
* INPUT:
*       None.
*
* OUTPUT:
*       See description.
*
* RETURN:
*       None.
*
*******************************************************************************/
void gtInit(void)
{
    unsigned int temp,temp1;

#ifdef PMC280_DEBUG_UART_VX
#ifdef PMC280_DUAL_CPU
    if(frcWhoAmI())
    {
        dbg_puts1("CPU1: Entering gtInit\n");
    }
    else
    {
        dbg_puts0("CPU0: Entering gtInit\n");
    }
#else
    dbg_puts0("Entering gtInit\n");
#endif /* PMC280_DUAL_CPU */
#endif /* PMC280_DEBUG_UART_VX */

#ifdef PMC280_DUAL_CPU
    /* All initialisations specific to CPU1 will be done at _cpu_1 */
    if(frcWhoAmI())     goto _cpu_1;
#endif /* PMC280_DUAL_CPU */

    /* If system operates with internal bus arbiter                 */
    /* disassert AACK Delay bit [25] in CPU configuration register. */
    GT_REG_READ(CPU_MASTER_CONTROL, &temp);
    if(temp & BIT8)
    {
        GT_REG_READ(CPU_CONFIG, &temp);
        GT_REG_WRITE(CPU_CONFIG, (temp & ~BIT25)); /*  p400 bit25 */
    }


#ifdef PMC280_DEBUG_UART_VX
#ifdef PMC280_DUAL_CPU
    if(frcWhoAmI())
    {
        dbg_puts1("CPU1: Passing CPU config initialisation\n");
    }
    else
    {
        dbg_puts0("CPU0: Passing CPU config initialisation\n");
    }
#else
    dbg_puts0("Passing CPU config initialisation\n");
#endif /* PMC280_DUAL_CPU */
#endif /* PMC280_DEBUG_UART_VX */

    /* Set MPP / GPP control registers and Interrupt level/IO */
    GT_REG_WRITE(MPP_CONTROL0, MPP_CONTROL0_VALUE);
    GT_REG_WRITE(MPP_CONTROL1, MPP_CONTROL1_VALUE);
    GT_REG_WRITE(MPP_CONTROL2, MPP_CONTROL2_VALUE);
    GT_REG_WRITE(MPP_CONTROL3, MPP_CONTROL3_VALUE);

#ifdef PMC280_DEBUG_UART_VX
#ifdef PMC280_DUAL_CPU
    if(frcWhoAmI())
    {
        dbg_puts1("CPU1: Passing MPP initialisation\n");
    }
    else
    {
        dbg_puts0("CPU0: Passing MPP initialisation\n");
    }
#else
    dbg_puts0("Passing MPP initialisation\n");
#endif /* PMC280_DUAL_CPU */
#endif /* PMC280_DEBUG_UART_VX */

    GT_REG_WRITE(GPP_LEVEL_CONTROL, GPP_LEVEL_CONTROL_VALUE);
    GT_REG_WRITE(GPP_IO_CONTROL,    GPP_IO_CONTROL_VALUE);


  /*  ::  I2C Bug Fix :: 04 Jul 2005 -start */

/* Program Mpp[9] as Gpp[9] */
    GT_REG_READ( MPP_CONTROL1, &temp1 );
    temp1 &= 0xffffff0f;
    GT_REG_WRITE( MPP_CONTROL1, temp1 );


   /* Program Gpp[9] as Ouput */
    GT_REG_READ( GPP_IO_CONTROL, &temp1 );
    temp1 |= 0x00000200;
    GT_REG_WRITE(GPP_IO_CONTROL, temp1 );

    /* Program Gpp[9] as Active High */
    GT_REG_READ( GPP_LEVEL_CONTROL, &temp1 );
    temp1 &= 0xfffffdff;
    GT_REG_WRITE( GPP_LEVEL_CONTROL, temp1 );

    /* Write the Value Register */
    GT_REG_READ( GPP_VALUE, &temp1 );
    temp1 |= 0x00000200;			/* Set Gpp[9] = 1 */
    GT_REG_WRITE( GPP_VALUE, temp1 );

	
/* ::  I2C Bug Fix :: 04 Jul 2005 - End */



#ifdef PMC280_DEBUG_UART_VX
#ifdef PMC280_DUAL_CPU
    if(frcWhoAmI())
    {
        dbg_puts1("CPU1: Passing GPP initialisation\n");
    }
    else
    {
        dbg_puts0("CPU0: Passing GPP initialisation\n");
    }
#else
    dbg_puts0("Passing MPP initialisation\n");
#endif /* PMC280_DUAL_CPU */
#endif /* PMC280_DEBUG_UART_VX */

    
    /* Set All windows to default parameters acording to board .H file */
    gtInitWindows();
    
    /* If SRAM functions as boot device do not set the device parameters,    */
    /* This means that these paramemters has been initialaized by I2C EEPROM */
    /* device, Also Change Device 0 and Boot Device Base address and size to */
    /* the parameters in the I2C for reboot                                  */


    /* Disable any watchdog facility activity 
    GT_REG_READ(WATCHDOG_CONFIG_REG, &temp);
    if(temp & 0x80000000)  Is the WD enabled ? 
    {
        temp &= ~0x3000000;
        temp |=  0x1000000;
        GT_REG_WRITE(WATCHDOG_CONFIG_REG, temp);
        temp &= ~0x3000000;
        temp |=  0x2000000;
        GT_REG_WRITE(WATCHDOG_CONFIG_REG, temp);
    }*/

    /* Stop Timer/counters */
    REG_CONTENT(TIMER_COUNTER_0_3_CONTROL) = 0;
#ifdef INCLUDE_CNTMR_4_7
    REG_CONTENT(TIMER_COUNTER_4_7_CONTROL) = 0;
#endif

    /* Abort any DMA acivity. Write '1' to channel control register bit 20  */
    REG_CONTENT(DMA_CHANNEL0_CONTROL) = GT_WORD_SWAP(1<<20);
    REG_CONTENT(DMA_CHANNEL1_CONTROL) = GT_WORD_SWAP(1<<20);
    REG_CONTENT(DMA_CHANNEL2_CONTROL) = GT_WORD_SWAP(1<<20);
    REG_CONTENT(DMA_CHANNEL3_CONTROL) = GT_WORD_SWAP(1<<20);
#ifdef INCLUDE_DMA_4_7
    REG_CONTENT(DMA_CHANNEL4_CONTROL) = GT_WORD_SWAP(1<<20);
    REG_CONTENT(DMA_CHANNEL5_CONTROL) = GT_WORD_SWAP(1<<20);
    REG_CONTENT(DMA_CHANNEL6_CONTROL) = GT_WORD_SWAP(1<<20);
    REG_CONTENT(DMA_CHANNEL7_CONTROL) = GT_WORD_SWAP(1<<20);
#endif
    
#if 0 /* leave ETH unit for now */
    /* Abort Ethernet ports activity and clear interrupt mask registers*/
    REG_CONTENT(ETH_UNIT_INTERRUPT_MASK_REG) = 0;
    {
        int eth, ethEnd = 1;
#ifdef INCLUDE_ETH_PORT_2
        ethEnd = 2;
#endif
        for(eth = 0; eth <= ethEnd ; eth ++)
        {
            /* Stop any port activity */
            GT_REG_WRITE(ETH_SDMA_CONFIG_REG(eth), (1<<15));
            /* Wait for all activity to terminate */
            do
             {
                GT_REG_READ(ETH_SDMA_CONFIG_REG(eth), &temp);
             }
            while(temp  & (1<<15));

            /* Disable port. */
            GT_REG_WRITE(ETH_PORT_CONFIG_REG(eth), 0);
        }
        REG_CONTENT(ETH_INTERRUPT_MASK_REG(eth)       ) = 0;
        REG_CONTENT(ETH_INTERRUPT_EXTEND_MASK_REG(eth)) = 0;
    }
#endif

    /* Mask all interrups */
    REG_CONTENT(SMP_CPU0_DOORBELL_MASK) = 0;
    REG_CONTENT(SMP_CPU1_DOORBELL_MASK) = 0;
    REG_CONTENT(DEVICE_INTERRUPT_MASK) = 0;
    REG_CONTENT(CPU_ERROR_MASK) = 0;
    
    REG_CONTENT(PCI_0_SERR_MASK ) = 0;
    REG_CONTENT(PCI_0_ERROR_MASK) = 0;

    REG_CONTENT(CUNIT_INTERRUPT_MASK_REG) = 0;
    REG_CONTENT(MPSC_MASK_REG(0)        ) = 0;
    REG_CONTENT(MPSC_MASK_REG(1)        ) = 0;
    REG_CONTENT(SDMA_MASK_REG           ) = 0;
    REG_CONTENT(BRG_MASK_REG            ) = 0;
    REG_CONTENT(DMA_INTERRUPT_CAUSE_MASK) = 0;
    REG_CONTENT(TIMER_COUNTER_0_3_INTERRUPT_MASK) = 0;

    REG_CONTENT(GPP_INTERRUPT_MASK0) = 0;
    REG_CONTENT(GPP_INTERRUPT_MASK1) = 0;
    REG_CONTENT(CPU_INTERRUPT0_MASK_LOW ) = 0;
    REG_CONTENT(CPU_INTERRUPT0_MASK_HIGH) = 0;
    REG_CONTENT(CPU_INTERRUPT1_MASK_LOW ) = 0;
    REG_CONTENT(CPU_INTERRUPT1_MASK_HIGH) = 0;
    REG_CONTENT(INTERRUPT0_MASK_0_LOW ) = 0;
    REG_CONTENT(INTERRUPT0_MASK_0_HIGH) = 0;
    REG_CONTENT(INTERRUPT1_MASK_0_LOW ) = 0;
    REG_CONTENT(INTERRUPT1_MASK_0_HIGH) = 0;

    /* Init internal SRAM memory allocation driver */
    gtSramInit();

    /* 
     * After we have finished all the Memory and device initializations we 
     * disable retries 
     */
    GT_SET_REG_BITS(CPU_CONFIG, BIT17); 
#ifdef PMC280_DUAL_CPU 
    goto   _continue;

_cpu_1:
    /*
     * Put all board/system controller initialisation that will be
     * done by CPU1.
     */
     gtInitWindows();
     goto   _continue;
#endif /* PMC280_DUAL_CPU */

#ifdef PMC280_DUAL_CPU
_continue: 
#ifdef PMC280_DEBUG_UART_VX
        if(frcWhoAmI())
        {
            dbg_puts1("CPU1: Exiting gtInit\n");
        }
        else
        {
            dbg_puts0("CPU0: Exiting gtInit\n");
        }
#endif /* PMC280_DEBUG_UART_VX */
#else
#ifdef PMC280_DEBUG_UART_VX
    dbg_puts0("Exiting gtInit\n");
#endif /* PMC280_DEBUG_UART_VX */
#endif /* PMC280_DUAL_CPU */
}

/*******************************************************************************
* gtInitSdramSize - Initialize SDRAM size.
*
* DESCRIPTION:
*       The SDRAM auto configuration feature is consist of two parts:
*       1) SDRAM width detection and bank parameters setting (Assembly code)
*       2) SDRAM address decoding and PCI address decoding (C code)
*       This function establishs the size of the SDRAM according to the
*       information gathered on the assembly phase (sdramAutoConfig).
*       This function sets the Chip Select registers according to the SDRMA size
*       detected and changes the PCI bars accordingly.
*
* INPUT:
*       None.
*
* OUTPUT:
*       See description.None.
*
* RETURN:
*       The total size of the SDRAM.
*
*******************************************************************************/
unsigned int  gtInitSdramSize(void)
{
    UINT32 bank0Size, bank1Size, temp;

    switch(frcGetBank0MemSize())
    {
        case CS0_MEM_SIZE_128MB:  bank0Size = 128  * _1M; break;
        case CS0_MEM_SIZE_256MB:  bank0Size = 256  * _1M; break;
        case CS0_MEM_SIZE_512MB:  bank0Size = 512  * _1M; break;
        case CS0_MEM_SIZE_1GB:    bank0Size = 1024 * _1M; break;
        case CS0_MEM_SIZE_2GB:    temp = 2048; 
                                  bank0Size = temp * _1M; break;
        default:                  bank0Size = 128  * _1M; break;
    }

    switch(frcGetBank1MemSize())
    {
        case CS1_MEM_SIZE_0MB:    bank1Size = 0    * _1M; break;
        case CS1_MEM_SIZE_128MB:  bank1Size = 128  * _1M; break;
        case CS1_MEM_SIZE_256MB:  bank1Size = 256  * _1M; break;
        case CS1_MEM_SIZE_512MB:  bank1Size = 512  * _1M; break;
        case CS1_MEM_SIZE_1GB:    bank1Size = 1024 * _1M; break;
        case CS1_MEM_SIZE_2GB:    temp = 2048;
                                  bank1Size = temp * _1M; break;
        default:                  bank1Size = 0    * _1M; break;
    }

    /* Reconfig the system with the new banks size. */
    gtMemoryMapBank0(0,bank0Size);
    gtMemoryMapBank1(bank0Size, bank1Size);

/*Sergey: just debugging*/
*(unsigned int *)0x10000000 = bank0Size;
*(unsigned int *)0x10000004 = bank1Size;

    /* Fix PCI0 bars */
	gtPci0MapMemoryBank0(0,bank0Size);
    gtPci0MapMemoryBank1(bank0Size,bank1Size);

    /* Close Bank 2 according to the PCI spec */
    GT_REG_READ (PCI_0_BASE_ADDR_REG_ENABLE,&temp); 
    GT_REG_WRITE(PCI_0_BASE_ADDR_REG_ENABLE,temp | 0x4);

    /* Close Bank 3 according to the PCI spec */
    GT_REG_READ (PCI_0_BASE_ADDR_REG_ENABLE,&temp); 
    GT_REG_WRITE(PCI_0_BASE_ADDR_REG_ENABLE,temp | 0x8);

    return (bank0Size + bank1Size);
}

/*******************************************************************************
* gtInitWindows - short description
*
* DESCRIPTION:
*       None.
*
* INPUT:
*       None.
*
* OUTPUT:
*       Set Default parameters in System Controllers registers.
*
* RETURN:
*       None.
*
*******************************************************************************/
void gtInitWindows(void)
{
#ifdef PMC280_DUAL_CPU
    UINT32 temp;
#endif /* K2_DUAL_CPU */

#ifdef PMC280_DEBUG_UART_VX
#ifdef PMC280_DUAL_CPU
    if(frcWhoAmI())
    {
        dbg_puts1("CPU1: Entering gtInitWindows\n");
    }
    else
    {
        dbg_puts0("CPU0: Entering gtInitWindows\n");
    }
#else
    dbg_puts0("Entering gtInitWindows\n");
#endif /* PMC280_DUAL_CPU */
#endif /* PMC280_DEBUG_UART_VX */

#ifdef PMC280_DUAL_CPU
    if(frcWhoAmI())   goto _cpu_1;
#endif /* PMC280_DUAL_CPU */

#ifdef PMC280_DUAL_CPU
#ifdef PMC280_BOOTROM_BOOT

    frcInitBootSyncLocation();

    /*
     * Permit CPU1 to boot and wait for the signal that it is (CPU1) is
     * running from DRAM (so that we can go ahead and program the
     * PCI related registers without any problems.
     */
    frcPermitCPU1ToBoot();

    while(!frcReadBootSyncLocation());
#else

#ifdef PMC280_PCI_BOOT

    frcInitPciBootSyncLocation();
#endif /* PMC280_PCI_BOOT */

/*sergey*/
#ifdef PMC280_USERFLASH_BOOT
	frcInitUserBootSyncLocation();

	/*Sergey: we have CPU1 bootrom image in user_flash at offset 0x01f00000;
    we will disable boot_flash and map user_flash at address 0xfff00000, so
    CPU1 booting from address 0xfff00100 will actualy boot from user_flash
    offset 0x01f00100 */

    gtMemoryMapDeviceBootSpace(0,0); /*disable BootCS*/
    gtMemoryMapDevice1Space(0xfff00000,0x01000000); /*map user_flash2 at 0xfff00000*/
#endif

    /*
     * Permit CPU1 to boot and wait for the signal that it is (CPU1) is
     * running from DRAM (so that we can go ahead and program the
     * PCI related registers without any problems.
     */
    frcPermitCPU1ToBoot();

#ifdef PMC280_PCI_BOOT
    while(!frcReadPciBootSyncLocation()); 
#endif /* PMC280_PCI_BOOT */

/*sergey*/
#ifdef PMC280_USERFLASH_BOOT
    while(!frcReadUserBootSyncLocation()) {;}
#endif

#endif /* PMC280_BOOTROM_BOOT */
#endif /* PMC280_DUAL_CPU */

    /* Set CS[0,1] address space */
    flashAutoDetect();   

    gtMemorySetInternalSramBaseAddr(INTERNAL_SRAM_ADRS);
    gtMemoryMapDeviceBootSpace(BOOTCS_BASE_ADRS,BOOTCS_ADRS_SPACE_SIZE);

    /* Know wether Ext. SRAM is present and accordingly enable its window */
    
    switch(frcGetExtSramSize()) {
        case EXT_SRAM_1MB:
            gtMemoryMapDevice2Space(CS2_BASE_ADRS, _1M);
            break;
        case EXT_SRAM_2MB:
            gtMemoryMapDevice2Space(CS2_BASE_ADRS, _2M);
            break;
        default:
            /* close */
            gtMemoryMapDevice2Space(0,0);
    }




    /* Set PCI0 address space windows according to sizes in board .H file */

	/*sergey: is setting tables in sysLib.c and frcMmu.c enough, why we need this ??*/
    /* CPU1 maps hosts memory without following ..*/

    gtPci0MapIOspace     (PCI0_IO_BASE_ADRS  , PCI0_IO_SIZE);
    gtPci0MapMemory0space(PCI0_MEM0_BASE_ADRS, PCI0_MEM0_SIZE);
    gtPci0MapMemory1space(PCI0_MEM1_BASE_ADRS, PCI0_MEM1_SIZE);
    gtPci0MapMemory2space(PCI0_MEM2_BASE_ADRS, PCI0_MEM2_SIZE);
    gtPci0MapMemory3space(PCI0_MEM3_BASE_ADRS, PCI0_MEM3_SIZE);

#ifdef PMC280_DUAL_CPU
    goto   _continue;

_cpu_1:
	/* All configurations to be done by CPU1 */

#ifdef PMC280_BOOTROM_BOOT
        frcSetBootSyncLocation();
#else

#ifdef PMC280_PCI_BOOT
        frcSetPciBootSyncLocation();
#else

#ifdef PMC280_USERFLASH_BOOT

		/*sergey*/
        frcSetUserBootSyncLocation();

#else
#error "PMC280_BOOTROM_BOOT or PMC280_PCI_BOOT or PMC280_USERFLASH_BOOT have to be defined" 
#endif /* PMC280_USERFLASH_BOOT */
#endif /* PMC280_PCI_BOOT */

#endif /* PMC280_BOOTROM_BOOT */

        for(temp=0;temp<100000;temp++); /* Wait for CPU 0 to finish with flash */
        flashAutoDetect();  /* CPU 0 and 1 is take care inside */ 

        goto _continue;

_continue:
#endif /* PMC280_DUAL_CPU */

#ifdef PMC280_DEBUG_UART_VX
#ifdef PMC280_DUAL_CPU
    if(frcWhoAmI())
    {
        dbg_puts1("CPU1: Exiting gtInitWindows\n");
    }
    else
    {
        dbg_puts0("CPU0: Exiting gtInitWindows\n");
    }
#else
    dbg_puts0("Exiting gtInitWindows\n");
#endif /* PMC280_DUAL_CPU */
#endif /* PMC280_DEBUG_UART_VX */
}
