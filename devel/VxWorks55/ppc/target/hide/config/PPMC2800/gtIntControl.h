/******************************************************************************
*              (c), Copyright, MOTOROLA  INC - ECC                 *
*                                                                             *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MOTOROLA , INC.        *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT *
* OF MOTOROLA   OR ANY THIRD PARTY. MOTOROLA   RESERVES THE RIGHT AT ITS SOLE DISCRETION*
* TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MOTOROLA .       *
* THIS CODE IS PROVIDED "AS IS". MOTOROLA  MAKES NO WARRANTIES, EXPRESS,*
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.  *
*******************************************************************************


*******************************************************************************
* gtIntControl.h - Header File for : GT Interrupt controller driver 
*
* DESCRIPTION:
*       None.
*
* DEPENDENCIES:
*       None.
*
******************************************************************************/

#ifndef __INCgtIntControlh
#define __INCgtIntControlh

/* includes */
#include "intLib.h"
#include "logLib.h"
#include "./coreDrv/gtCore.h"


/* defines  */
#define SWAP_CAUSE_BIT(cause)   (GT_WORD_SWAP(1 << ((cause) % 32)))


/* typedefs */

/* GT_INT_CAUSE enumerator combines the two cause registers into one.  */
/* GT_INT_CAUSE is a cause bit that moves between 0 and 63             */
typedef enum _gtIntCause {
    MAIN_CAUSE_START = -1, /* Cause Start boundry */
    /* Main Interrupt Cause Low register (offset 0x004) */
    RSRVD_L00   ,    
    DEV_ERR     , /* Device bus error (parity, Ready# timer, burst violation)*/
    DMA_ERR     , /* DMA error (address decoding and protection)             */
    CPU_ERR     , /* CPU error (parity, address decoding and protection)     */
    IDMA0       , /* IDMA Channels 0 completion */
    IDMA1       , /* IDMA Channels 1 completion */
    IDMA2       , /* IDMA Channels 2 completion */
    IDMA3       , /* IDMA Channels 3 completion */
    TIMER0      , /* Timers 0 Interrupt         */
    TIMER1      , /* Timers 1 Interrupt         */
    TIMER2      , /* Timers 2 Interrupt         */
    TIMER3      , /* Timers 3 Interrupt         */
    PCI0        , /* PCI0 (summary of PCI0 Cause register) */
    SRAM_ERR    , /* SRAM parity error                     */
    GE_ERR      , /* Gb Ethernet error  (address decoding and protection) */
    SERIAL_ERR  , /* Serial ports error (address decoding and protection) */
    PCI1        , /* PCI1 (summary of PCI1 Cause register)                */
    DRAM_ERR    , /* DRAM ECC error                                       */
    WD_NMI      , /* WD reached its NMI threshold                         */
    WDE         , /* WD reached terminal count                            */
    PCI0_IN     , /* PCI_0 Inbound (summary of Inbound Cause register).   */
    PCI0_OUT    , /* PCI_0 Outbound (summary of Outbound Cause register)  */
    PCI1_IN     , /* PCI_1 Inbound (summary of Inbound Cause register).   */
    PCI1_OUT    , /* PCI_1 Outbound (summary of Outbound Cause register). */
    P1_GPP_7_0  , /* CPU1 GPP[ 7: 0] Interrupt */
    P1_GPP_15_8 , /* CPU1 GPP[15: 8] Interrupt */
    P1_GPP_23_16, /* CPU1 GPP[23:16] Interrupt */
    P1_GPP_31_24, /* CPU1 GPP[31:24] Interrupt */
    P1_CPU_DB   , /* Summary of CPU1 Doorbell Cause register */
    RSRVD_L29   ,
    RSRVD_L30   ,
    RSRVD_L31   ,
    
    /* Main Interrupt Cause High register (offset 0x00c) */
    GE0         , /* Gb Ethernet0 summary */
    GE1         , /* Gb Ethernet1 summary */
    GE2         , /* Gb Ethernet2 summary */
    RSRVD_H03   , 
    SDMA0       , /* SDMA0 (summary of MPSC0 SDMA Cause register) */
    I2C         , /* I2C Interrupt                                */ 
    SDMA1       , /* SDMA1 (summary of MPSC1 SDMA Cause register) */
    BRG         , /* BRG                                          */
    MPSC0       , /* MPSC0 (summary of MPSC0 Cause register) */
    MPSC1       , /* MPSC1 (summary of MPSC1 cause register) */
    G0_RX       , /* Gb Ethernet0 Rx   summary */
    G0_TX       , /* Gb Ethernet0 Tx   summary */
    G0_MISC     , /* Gb Ethernet0 misc summary */
    G1_RX       , /* Gb Ethernet1 Rx   summary */
    G1_TX       , /* Gb Ethernet1 Tx   summary */
    G1_MISC     , /* Gb Ethernet1 misc summary */
    G2_RX       , /* Gb Ethernet2 Rx   summary */
    G2_TX       , /* Gb Ethernet2 Tx   summary */
    G2_MISC     , /* Gb Ethernet2 misc summary */
    RSRVD_H19   , 
    RSRVD_H20   , 
    RSRVD_H21   , 
    RSRVD_H22   , 
    RSRVD_H23   , 
    P0_GPP_7_0  , /* CPU0 GPP[ 7: 0] Interrupt */
    P0_GPP_15_8 , /* CPU0 GPP[15: 8] Interrupt */
    P0_GPP_23_16, /* CPU0 GPP[23:16] Interrupt */
    P0_GPP_31_24, /* CPU0 GPP[31:24] Interrupt */
    P0_CPU_DB   , /* Summary of CPU0 Doorbell Cause register */
    RSRVD_H29   ,
    RSRVD_H30   ,
    RSRVD_H31   ,
              
    /* End of enumerator */
    MAIN_CAUSE_END   /* Cause End boundry */
} GT_INT_CAUSE;


typedef struct _gtIsrEntry {
    UINT        causeBit;  /* A specific cause bit in the cause register.*/
    VOIDFUNCPTR userISR;   /* A user ISR pointer.*/
    int         arg;       /* An argument to interrupt ISR. */
    int         prio;      /* An interrupt priority. */
} GT_ISR_ENTRY;

/* gtIntControl.h API list */
void         gtIntCtrlInit  (void); 
STATUS       gtIntConnect   (GT_INT_CAUSE cause, VOIDFUNCPTR routine,
                             int parameter, int prio);
STATUS       gtIntDisonnect (GT_INT_CAUSE cause);
unsigned int gtIntEnable    (GT_INT_CAUSE cause);
unsigned int gtIntDisable   (GT_INT_CAUSE cause);

#endif /* __INCgtIntControlh */
