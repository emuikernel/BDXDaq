/* mpc107Msg.c - MPC107 Message Unit  support */

/* Copyright 1996-2000 Wind River Systems, Inc. */
#include "copyright_wrs.h"

/*
modification history
--------------------
01a,11sep00,rcs  fix include paths
01a,26jun00,bri written
*/

/*
DESCRIPTION
This module contains routines for the Message Unit interface of MPC107 .

The Message Unit (MU) of MPC107  facilitates communications between
the host processor and peripheral processors in a multiprocessor system .

The Message Unit consists of generic messages registers, doorbell registers
and an an I2O-compliant interface.


.SH INITIALIZATION
If the processor associated with this MPC107 is to be configured as the
host processor ,the Message Unit Interface is initialized by calling
the routine mpc107MsgMessageHostInit().

If the processor associated with this MPC107 is  to be configured as
as a peripheral processor the Message Unit Interface is initialized
by calling the routine mpc107MsgMessagePeripheralInit() .
The user has to specify to which interrupt of the processor these
interrupts are connected.
*/

/* includes */

#include "vxWorks.h"
/* #include "config.h" */
#include "sysLib.h"
#include "stdlib.h"
#include "errno.h"
#include "errnoLib.h"
#include "intLib.h"
#include "memLib.h"
#include "drv/multi/mpc107.h"
#include "drv/multi/mpc107Msg.h"
#include "drv/intrCtl/mpc107Epic.h"

/* globals */

UINT32 pciBaseAddr = 0;  /* PCI base address for oubound interface */

/* static file scope locals */

BOOL	intHandlerInbound = FALSE;  /* Flag for Inbound Interrupt Handler  */
BOOL	intHandlerOutbound = FALSE; /* Flag for Outbound Interrupt Handler */

/* forward Declarations */

void		mpc107MessageOutInt (void);
void		mpc107MessageInInt (void);
LOCAL void 	mpc107MsgWrite (UINT32 pciBar, UINT32 messageRegNum,
                            UINT32 message);
LOCAL ULONG 	mpc107MsgRead (UINT32 pciBar, UINT32 messageRegNum );
LOCAL UINT32 	mpc107MsgRegMod (UINT32 pciBar, UINT32 ioControlFlag,
                                UINT32 address, UINT32 wdata1, UINT32 wdata2);

/***************************************************************************
*
* mpc107MsgMessageHostInit -  initializaion  of Message Unit (Host)
*
* This routine initializes the Message Unit for multiprocessor communications
* using  Inbound/Outbound Message Registers and Inbound/Outbound Doorbell
* Registers for the host interface
*
* This routine should be called once during hardware initialization .
*
* This routine should  called before using the Message Unit for Multiprocessor
* communications using  Inbound/Outbound Message Registers  and
* Inbound/Outbound  Doorbell Registers  for the host interface
*
* RETURNS: N/A
*/

void mpc107MsgMessageHostInit(void)
    {

    if (!intHandlerInbound) /* If Interrupt handler is not hooked */
        {

        intConnect ((VOIDFUNCPTR*)MPC107_EPIC_MU_INT_VECT,
                                   mpc107MessageInInt, 0);

        /*
         * Setting Flag indicating that Inbound
         * Interrupt handler is hooked
         */

        intHandlerInbound = TRUE;

        /*
         * Enable the Interrupts in the Inbound Message
         * Interrupt Mask Register
         */

        mpc107MsgRegMod (0x0, MPC107_MSG_READ_OR_WRITE, MPC107_MSG_I2O_IMIMR,
                         !(MPC107_MSG_I2O_IM0IM | MPC107_MSG_I2O_IM1IM |
                           MPC107_MSG_I2O_IDIM | MPC107_MSG_I2O_MCIM), 0);
        }

    }


/***************************************************************************
*
* mpc107MsgMessagePeripheralInit - initialization of Message Unit (Peripheral)
*
* This routine initializes the Message Unit for multiprocessor communications
* using Inbound/Outbound Message Registers and Inbound/Outbound Doorbell
* Registers  with MPC107 as a peripheral processor
*
* This routine should  called before using the Message Unit for Multiprocessor
* communications using Inbound/Outbound Message Registers  and
* Inbound/Outbound  Doorbell Registers as a peripheral interface
*
* RETURNS: N/A
*/

void mpc107MsgMessagePeripheralInit (void )
    {

    if (!intHandlerOutbound)
        {

        /*
         * The PCI base Address  should be obtained
         * after the PCI auto Config  and assigned to
         * the variable <pciBaseAddr>
         */

        pciBaseAddr = MPC107_PCSRBAR_VAL;

        /*
         * The ISR handlers should be hooked here if
         * it is not already hooked
         */

        intConnect ((VOIDFUNCPTR*)MPC107_EPIC_MU_INT_VECT,
                               mpc107MessageOutInt, 0);

        intHandlerOutbound = TRUE;

        /*
         * Enable the Interrupts in the Outbound Message
         * Interrupt Mask Register
         */

        mpc107MsgRegMod (pciBaseAddr, MPC107_MSG_READ_OR_WRITE,
                         MPC107_MSG_I2O_PCI_OMIMR, !(MPC107_MSG_I2O_OM0IM |
                         MPC107_MSG_I2O_OM1IM | MPC107_MSG_I2O_ODIM ), 0);
        }

    }

/***************************************************************************
*
* mpc107MsgExtDoorbellInit -  initialization of Extended Doorbell Registers
*
* This routine initializes the Message Unit so that Extended Doorbell
* Registers can be used for communication in a  multiprocessor configuration
*
* This routine should be called before using the Extended Doorbell Registers
* for multiprocessor communication
*
* If the External Doorbell registers are  used for multiprocessor
* communication, then the rest of the Message Unit should not be used
* (or enabled).
*
* RETURNS: OK, or ERROR if the specified interrupt type <cpuIntA> is
* not correct.
*/

STATUS mpc107MsgExtDoorbellInit
    (
    UINT32	cpuIntA /* Whether message is for generating INTA (zero) */
                        /* or CPU INT (one)*/

    )
    {

    if (cpuIntA == MPC107_MSG_INTAGEN)
        {

        /*
         * Disable the CPU0 Interrupt  and Enable the INTA Interrupt
         * in the Extended Door Bell Mask Register
         */

        mpc107MsgRegMod (pciBaseAddr, MPC107_MSG_READ_ANDOR_WRITE,
                         MPC107_MSG_EDBSR, ~(MPC107_MSG_EDBMR_INTAIM_BIT),
                         MPC107_MSG_EDBMR_C0IM_BIT);

        }
    else if (cpuIntA == MPC107_MSG_CP0GEN)
        {

        /*
         * Enable the CPU0 Interrupt  and Disable the INTA Interrupt
         * in the Extended Door Bell Mask Register
         */

        mpc107MsgRegMod (0x0, MPC107_MSG_READ_ANDOR_WRITE,
                         MPC107_MSG_EDBSR,~(MPC107_MSG_EDBMR_C0IM_BIT),
                         MPC107_MSG_EDBMR_INTAIM_BIT);

        }

    else
        {

        errnoSet (EINVAL);
        return (ERROR);

        }

    /*
     * Disable the Interrupts in the
     * Inbound Message Interrupt Mask Register
     */

    mpc107MsgRegMod (pciBaseAddr, MPC107_MSG_READ_OR_WRITE,
                     MPC107_MSG_I2O_IMIMR, MPC107_MSG_I2O_IM0IM |
                     MPC107_MSG_I2O_IM1IM | MPC107_MSG_I2O_IDIM |
                     MPC107_MSG_I2O_MCIM, 0);

    return (OK);

    }


/***************************************************************************
*
* mpc107MsgMessageWrite  -  write to Message Registers
*
* This routine is used for writing data to the Inbound /Out Bound
* Message Registers.
*
* A remote processor can write a 32 bit message to the Inbound Message Register
* which inturn generates an interrupt (INT to assert) to the local processor
*
* The local processor can write a 32 bit message to the Outbound Message
* Register which in turn  causes the outbound interrupt signal INTA to be
* asserted.
*
* RETURNS: OK, or ERROR if  <inOutBound> is  neither Inbound nor Outbound,
* and  <messageRegNum> is not a valid register.
*/

STATUS mpc107MsgMessageWrite
    (
    UINT32 	inOutBound,    /* Inbound or Outbound Message Register */
    UINT32 	messageRegNum, /* register Number */
    UINT32 	message        /* message to be sent */
    )
    {

    ULONG regNum;  /* register number */

    if (inOutBound == MPC107_MSG_INBOUND) /* Inbound Registers */
        {
        if (messageRegNum == MPC107_MSG_REGISTER0)
            {

            regNum = MPC107_MSG_IMR0; /* Inbound Message Register 0 */

            }

        else if (messageRegNum == MPC107_MSG_REGISTER1)
            {

            regNum = MPC107_MSG_IMR1;  /* InBound Message Register 1 */

            }
        else
            {

            errnoSet (EINVAL);
            return (ERROR);            /* Invalid register */

            }

        mpc107MsgRegMod (0x0, MPC107_MSG_WRITE, regNum, message, 0);

        }
    else if (inOutBound == MPC107_MSG_OUTBOUND) /* Outbound Registers */
        {

        if (messageRegNum == MPC107_MSG_REGISTER0)
            {

            regNum = MPC107_MSG_OMR0; /* Outbound Message Register 0 */

            }

        else if (messageRegNum == MPC107_MSG_REGISTER1)
            {

            regNum = MPC107_MSG_OMR1; /* Outbound Message Register 1 */

            }
        else
            {

            errnoSet (EINVAL);
            return (ERROR);            /* Invalid register */

            }

        /* Write the message to the specified register */

        mpc107MsgRegMod (pciBaseAddr, MPC107_MSG_WRITE, regNum, message, 0);

        }

    else
        {

        errnoSet (EINVAL);
        return (ERROR);            /* Invalid register */

        }

        return (OK);

    }


/***************************************************************************
*
* mpc107MsgMessageRead - read from Message Registers
*
* This routine is used for reading Inbound/Outbound Message registers .
*
* The Inbound Message register is read  by the local processor .
* The Outbound Message register is read by the remote processor
*
* RETURNS: OK, or ERROR if  <inOutBound> is  neither Inbound nor Outbound,
* and  <messageRegNum> is not a valid register.
*/

STATUS mpc107MsgMessageRead
    (
    UINT32 	inOutBound,    /* Inbound or Outbound Message Register */
    UINT32 	messageRegNum, /* register Number */
    UINT32 *	pMessageRead   /* pointer to the buffer to read message */
    )
    {

    ULONG 	regNum; /* register */

    if (inOutBound == MPC107_MSG_INBOUND) /* Inbound registers */
        {
        if (messageRegNum == MPC107_MSG_REGISTER0)
            {

            regNum = MPC107_MSG_IMR0; /* Inbound Message Register 0 */

            }

        else if (messageRegNum == MPC107_MSG_REGISTER1)
            {

            regNum = MPC107_MSG_IMR1;  /* InBound Message Register 1 */

            }
        else
            {

            errnoSet (EINVAL);
            return (ERROR);            /* Invalid register */

            }

        * (UINT32 *)pMessageRead  = mpc107MsgRegMod (0x0, MPC107_MSG_READ,
                                                     regNum, 0, 0);

        }
    else if (inOutBound == MPC107_MSG_OUTBOUND) /* Outbound registers */
        {
        if (messageRegNum == MPC107_MSG_REGISTER0)
            {

            regNum = MPC107_MSG_OMR0; /* Outbound Message Register 0 */

            }

        else if (messageRegNum == MPC107_MSG_REGISTER1)
            {

            regNum = MPC107_MSG_OMR1;  /* Outbound Message Register 1 */

            }
        else
            {

            errnoSet (EINVAL);
            return (ERROR);            /* Invalid register */

            }
        * (UINT32 *)pMessageRead = mpc107MsgRegMod (pciBaseAddr,MPC107_MSG_READ,
                                                    regNum, 0, 0);
        }

    else
        {

        errnoSet (EINVAL);
        return (ERROR);            /* Invalid register */

        }

    /* Read the message from the specified register */

    * (UINT32 *)pMessageRead  = mpc107MsgRegMod (pciBaseAddr, MPC107_MSG_READ,
                                                 regNum, 0, 0);

    return (OK);

    }


/***************************************************************************
*
* mpc107MsgDoorbellWrite -  write to  Doorbell Registers
*
* This routine is used for writing data to the Inbound /Outbound
* Doorbell Registers
*
* A remote processor can set a bit in the Inbound Doorbell Register from
* the PCI bus which inturn generates an interrupt (INT to assert) to the
* local processor. If the Machine Check bit is set then a machine
* check condition is conveyed to the local processor .
*
* The local processor can write to the Outbound Doorbell Register , which
* causes the outbound interrupt signal INTA to be asserted, thus interrupting
* the remote processor.
*
* RETURNS: OK, or ERROR if  <inOutBound> is  neither Inbound nor Outbound .
*/

STATUS mpc107MsgDoorbellWrite
    (
    UINT32 	inOutBound,   /* whether Inbound or Outbound Message Register */
    UINT32 	message,      /* message to be sent */
    UINT32 	machineCheck  /* machine Check Bit (zero or one ) This is */
                              /* valid only for Inbound Door Bell */
                              /* Register */
    )
    {

    ULONG regNum;

    if (inOutBound == MPC107_MSG_INBOUND) /* Inbound Doorbell Register */
        {

        regNum = MPC107_MSG_IDBR;

        message |= (machineCheck << MPC107_MSG_IDBR_MC_SHIFT);

        /* Write the message to the specified register */

        mpc107MsgRegMod (0x0, MPC107_MSG_WRITE, regNum, message, 0);
        }

    else if (inOutBound == MPC107_MSG_OUTBOUND) /* Outbound Doorbell Register */
        {

        regNum = MPC107_MSG_ODBR;

        message |= MPC107_MSG_ODBR_DATA_MASK;

        /* Write the message to the specified register */

        mpc107MsgRegMod (pciBaseAddr, MPC107_MSG_WRITE, regNum, message, 0);
        }

    else
        {

        errnoSet (EINVAL);
        return (ERROR);

        }

        return (OK);

    }


/***************************************************************************
*
* mpc107MsgDoorbellRead -  read  from  Doorbell Registers
*
* This routine is used for reading data from the Inbound / Outbound
* Doorbell registers
*
* The Inbound Doorbell register is read  by the local proceesor  and the
* bits set in the register are cleared by writing a 1 those bits .
*
* The Outbound Doorbell Register is read by the remote processor  and  the
* bits set in the register are cleared by writing a 1 those bits .
*
* RETURNS: OK, or ERROR if  <inOutBound> is  neither Inbound nor Outbound.
*/

STATUS mpc107MsgDoorbellRead
    (
    UINT32	inOutBound,       /* whether Inbound or Outbound Message Reg */
    UINT32	*pMsgDoorbellRead /* data read */
    )
    {

    ULONG       tempReadData; /* data read from the register */

    if (inOutBound == MPC107_MSG_INBOUND) /* Inbound Doorbell Register */
        {

        /* read the inbound doorbell register */

        tempReadData = mpc107MsgRegMod (0x0, MPC107_MSG_READ,
                                             MPC107_MSG_IDBR, 0, 0);

        *(UINT32 *)pMsgDoorbellRead = tempReadData;


        /* clear the bits set in the door bell registers */

        mpc107MsgRegMod (0x0, MPC107_MSG_WRITE, MPC107_MSG_IDBR,
                                                tempReadData, 0);
        }

    else if (inOutBound == MPC107_MSG_OUTBOUND) /* Outbound Doorbell Register */
        {

        /* read the Outbound doorbell register */

        tempReadData = mpc107MsgRegMod (pciBaseAddr, MPC107_MSG_READ,
                                                     MPC107_MSG_ODBR, 0, 0);

        *(UINT32 *)pMsgDoorbellRead = tempReadData;

        /* clear the bits set in the door bell registers */

        mpc107MsgRegMod (pciBaseAddr, MPC107_MSG_WRITE, MPC107_MSG_ODBR,
                         tempReadData, 0);

        }

    else
        {

        errnoSet (EINVAL);
        return (ERROR);

        }


        return (OK);

    }


/***************************************************************************
*
* mpc107MsgExtDoorbellSetWrite - write to Extended Doorbell Write1 Set Register
*
* This routine is used for writing data to Extended Doorbell Write 1
* Set register  causing an interrupt to be generated .
*
* RETURNS: OK, or ERROR if the specified interrupt type <cpuIntA> is
* not correct.
*/

STATUS mpc107MsgExtDoorbellSetWrite
    (
    UINT32	message, /* message to be sent */
    UINT32	cpuIntA  /* whether message is for generating INTA (zero) */
                         /* or CPU Interrupt (One) */
    )
    {


    if (cpuIntA == MPC107_MSG_INTAGEN)
        {
        message = ((message  & MPC107_MSG_GENERAL_LSB_MSK )
                            << MPC107_MSG_EDBW1S_INTA_SHIFT)
                             & MPC107_MSG_EDBW1S_INTA_MSK ;

        }

    else if (cpuIntA == MPC107_MSG_CP0GEN)
        {

        message = ((message  & MPC107_MSG_GENERAL_LSB_MSK )
                            << MPC107_MSG_EDBW1S_CP0_SHIFT)
                             & MPC107_MSG_EDBW1S_CP0_MSK ;

        }

    else
        {

        errnoSet (EINVAL);
        return (ERROR);

        }

    /* Write  the  data to  Extended Doorbell Write 1 Register  */

    mpc107MsgRegMod (pciBaseAddr, MPC107_MSG_WRITE, MPC107_MSG_EDBW1S, 0, 0);

    return (OK);

    }


/***************************************************************************
*
* mpc107MsgExtDoorbellClrRead - read Extended Doorbell Write1 Clear Register
*
* This routine is used for reading data from the  Extended Doorbell Write 1
* Clear Register.This register is cleared after reading the data
*
* RETURNS: The data read is returned
*/

ULONG mpc107MsgExtDoorbellClrRead (void)
    {

    ULONG tempData ;

    /* Read the data  from Extended Door Bell Write1 Clear Register */

    tempData = mpc107MsgRegMod (pciBaseAddr, MPC107_MSG_WRITE,
                                             MPC107_MSG_EDBW1C, 0, 0);

    /* Clear all the bits in the Extended Doorbell Write1 Clear Register */

    mpc107MsgRegMod (pciBaseAddr, MPC107_MSG_WRITE,
                                  MPC107_MSG_EDBW1C, tempData, 0);

    return (tempData);

    }



/***************************************************************************
*
* mpc107MsgI2oInit -  initialization  of I20 Interface of Message Unit
*
* This routine sets up the size of the inbound and outbound FIFOs .
* Memory for messages is allocated and the inbound Free Fifo/Outbound Free FIFO
* are initialized with the Message Frame Addresses .
*
* RETURNS: OK, or ERROR if memory allocation has failed.
*/

STATUS mpc107MsgI2oInit (void)
    {

    UINT32 *	pI2oQbaseAddr;        /* pointer to Circular queue */
    UINT32 	tempI2oData;          /* temporary data */
    UINT32 	i2oQueueSize;         /* queue size */
    UINT32 *	pInboundFreeListBase; /* pointer to Inbound Free List FIFO */
    UINT32 *	pOutboundFreeListBase;/* pointer to Outbound Free List FIFO */
    UINT32 	tempIndex;            /* index */
    UINT32 	tempMfaAddress;       /* temporary MFA address */

    /* Initiailze the Message Unit I20 Control Register */

    mpc107MsgRegMod (0x0, MPC107_MSG_WRITE, MPC107_MSG_I2O_MUCR,
                          MPC107_MSG_I2O_MUCR_DEFAULT, 0);


    tempI2oData = mpc107MsgRegMod (0x0, MPC107_MSG_READ,
                                   MPC107_MSG_I2O_MUCR , 0, 0)
                                   & MPC107_MSG_I2O_CQS;

    switch (tempI2oData & MPC107_MSG_I2O_CQS)
        {

        case MPC107_MSG_I2O_CQS_16K:

            /* FIFO size is 4 K  entries (16 K Bytes) */

            i2oQueueSize = MPC107_MSG_I2O_CQS_16K_SIZE;

            break;

        case MPC107_MSG_I2O_CQS_32K:

             /* FIFO size is 8 K  entries (32 K Bytes) */

            i2oQueueSize = MPC107_MSG_I2O_CQS_32K_SIZE;

            break;

        case MPC107_MSG_I2O_CQS_64K:

            /* FIFO size is 16 K  entries (64 K Bytes) */

            i2oQueueSize = MPC107_MSG_I2O_CQS_64K_SIZE;

            break;

        case MPC107_MSG_I2O_CQS_128K:

             /* FIFO size is 32 K  entries (128 K Bytes) */

             i2oQueueSize = MPC107_MSG_I2O_CQS_128K_SIZE;

            break;

        case MPC107_MSG_I2O_CQS_256K:

            /* FIFO size is 64 K  entries (256 K Bytes) */

            i2oQueueSize = MPC107_MSG_I2O_CQS_256K_SIZE;

            break;

        default:

            /* FIFO size is 4 K  entries (16 K Bytes) */

            i2oQueueSize = MPC107_MSG_I2O_CQS_16K_SIZE;

            break;
        }

    /* Allocate memory for the circular queues */

    pI2oQbaseAddr = (UINT32 *)memalign (MPC107_MSG_I2O_MUCR_QBAR_ALIGN,
                             sizeof(UINT32) * i2oQueueSize * 4 );

    if ((pI2oQbaseAddr) != 0 )  /* If memory allocated */
        {

        /* Initialize the Queue Base Address Register */

        mpc107MsgRegMod (0x0, MPC107_MSG_WRITE, MPC107_MSG_I2O_QBAR,
                        (UINT32)pI2oQbaseAddr, 0);

        /* Pointer to Inbound Free List FIFO base */

        (UINT32 *)pInboundFreeListBase  = (UINT32 *) (pI2oQbaseAddr);

        /* Pointer to Outbound Free List FIFO base */

        (UINT32 *)pOutboundFreeListBase  =(UINT32 *) (pI2oQbaseAddr +
                                                    ((i2oQueueSize) *3));

        /* Initialize the In Bound Free FIFO list with the MFAs  */

        for (tempIndex = 0; tempIndex <i2oQueueSize; tempIndex++)
            {

            tempMfaAddress = ((UINT32)malloc(sizeof(UINT32)) *
                                    MPC107_MSG_I2O_MFRAME_SIZE);

            if ((tempMfaAddress) != 0) /* If memory is allocated */
                {

                *((UINT32 *) (pInboundFreeListBase + tempIndex)) =
                                                     tempMfaAddress;

                }
            else
                return (ERROR);

            }

        /* Initialize the Out Bound Free FIFO list with the MFAs  */

        for (tempIndex = 0; tempIndex <i2oQueueSize; tempIndex++)
            {

            tempMfaAddress = ((UINT32)malloc(sizeof(UINT32)) *
                                           MPC107_MSG_I2O_MFRAME_SIZE);

            if ((tempMfaAddress) != 0) /* If memory is allocated */
                {

                *((UINT32 *) (pOutboundFreeListBase + tempIndex)) =
                                                       tempMfaAddress;
                }
            else
                return (ERROR);

            }

        }

        else
            return (ERROR);

    return (OK);

    }


/***************************************************************************
*
* mpc107MsgI2oInboundRead -  read from I2O Inbound Interface
*
* This routine is used by the host processor to read messages posted to
* the I2O Outbound Interface by the external PCI master
*
* RETURNS: N/A
*/

void mpc107MsgI2oInboundRead (void)
    {

    UINT32 **	ppTempMfa;     /* pointer to Message Frame pointer */
    UINT32 *	pTempReadMfa; /* pointer to Message Frame */
    ULONG 	tempIndex;     /* index */
    UINT32 	pBuf[MPC107_MSG_I2O_MFRAME_SIZE]; /* buffer for read data */

    /* Read Inbound Post Fifo Tail Pointer Register (IPTPR) to get MFA */

    ppTempMfa = (UINT32 **)mpc107MsgRegMod (0, MPC107_MSG_READ,
                                               MPC107_MSG_I2O_IPTPR, 0, 0);

    pTempReadMfa = *ppTempMfa;

    /* Read the messages */

    for (tempIndex = 0; tempIndex < MPC107_MSG_I2O_MFRAME_SIZE; tempIndex++)
        {

        *((UINT32 *)(pBuf + tempIndex)) =
                            *((UINT32 *)(pTempReadMfa + tempIndex));

        }

    /* Increment the value in Inbound Post FIFO Tail Pointer Reg (IPTPR) */

    mpc107MsgRegMod (0, MPC107_MSG_WRITE, MPC107_MSG_I2O_IPTPR,
                                         (UINT32)(ppTempMfa + 1) , 0);

    /*
     * Return the message to the Inbound Free List Fifo  by
     * writing the MFAs to Inbound Free Fifo Head Pointer Reg
     */

    mpc107MsgRegMod (0, MPC107_MSG_WRITE, MPC107_MSG_I2O_IFHPR,
                                         (UINT32)ppTempMfa, 0);

    }

/***************************************************************************
*
* mpc107MsgI2oOutboundWrite -  write to  I20 Outbound interface
*
* This routine is used by the host processor  to write messages to
* the I2O Outbound Interface .
*
* The  host processor  obtains a free MFA  and  posts  the messages.
*
* RETURNS: N/A
*/

void mpc107MsgI2oOutboundWrite
    (
    UINT32 *	pBuf  /* pointer to message  to be written */
                      /* to Outbound I2O interface */
    )
    {

    UINT32 **	ppTempMfa;    /* pointer to Message Frame pointer */
    UINT32 **	ppTempOfhpr;  /* pointer to Message Frame pointer */
    UINT32 *	pTempWriteMfa;/* pointer to Message Frame */
    ULONG 	tempIndex;    /* index */

    /* Read Outbound Free Fifo Tail Pointer Reg (OFTPR)  to Get MFA */

    ppTempMfa = (UINT32 **) mpc107MsgRegMod (0, MPC107_MSG_READ,
                                                MPC107_MSG_I2O_OFTPR, 0, 0);

    /* Read Outbound Free Fifo Head Pointer Register (OFHPR)  to Get MFA */

    ppTempOfhpr = (UINT32 **) mpc107MsgRegMod (0, MPC107_MSG_READ,
                                                  MPC107_MSG_I2O_OFHPR, 0, 0);

    if (ppTempMfa != ppTempOfhpr) /* If the FIFO is not empty */
        {

        pTempWriteMfa = *ppTempMfa;

        /* Write the message in the Message Frame */

        for (tempIndex = 0; tempIndex < MPC107_MSG_I2O_MFRAME_SIZE; tempIndex++)
            {

            *((UINT32 *)(pTempWriteMfa + tempIndex)) =
                                     *((UINT32 *)(pBuf+ tempIndex));

            }

        /* Write the MFA to Outbound Post Fifo Head Pointer Reg OPHPR */

        mpc107MsgRegMod (0, MPC107_MSG_WRITE,
                            MPC107_MSG_I2O_OPHPR,(UINT32) ppTempMfa, 0);

        }
    }

/***************************************************************************
*
* mpc107MsgI2oOutboundRead -  read  from  I20 Outbound interface
*
* This routine is used by the external PCI master to read messages posted to
* the I2O Outbound Interface  by the host processor .
*
* RETURNS: N/A
*/

void mpc107MsgI2oOutboundRead (void)
    {

    UINT32 **ppTempMfa;    /* pointer to Message Frame pointer */
    UINT32 *pTempReadMfa;  /* pointer to Message Frame */
    ULONG tempIndex;       /* index  */
    UINT32 pBuf[MPC107_MSG_I2O_MFRAME_SIZE]; /* buffer to read data */

    /*
     * Read Outbound Fifo Queue Port Register (OFQPR)  to get the MFA
     * While FIFO is not empty
     */

     while((ppTempMfa = (UINT32 **)mpc107MsgRegMod(pciBaseAddr, MPC107_MSG_READ,
                                      MPC107_MSG_I2O_PCI_OFQPR, 0, 0)) !=
                                      (UINT32 **)MPC107_MSG_I2O_FIFO_EMPTY)
        {

        /* Read the messages  */

        pTempReadMfa  = *ppTempMfa;

        for (tempIndex = 0; tempIndex < MPC107_MSG_I2O_MFRAME_SIZE; tempIndex++)
            {

            *((UINT32 *)(pBuf+ tempIndex)) =
                      *((UINT32 *)(pTempReadMfa + tempIndex));


            /* Write the MFA to Outbound Fifo Queue Port Register (OFQPR) */

            mpc107MsgRegMod (pciBaseAddr, MPC107_MSG_WRITE,
                            MPC107_MSG_I2O_PCI_OFQPR,(UINT32) ppTempMfa, 0);
            }

        }

    }

/***************************************************************************
*
* mpc107MsgI2oInboundWrite -  write  to  I20 Inbound interface
*
* This routine is used by the external PCI master for writing messages to
* the I2O Inbound Interface .
*
* The external PCI master obtains a free MFA  and  posts  the messages.
*
* RETURNS: OK, or ERROR if free MFAs are not available.
*/

STATUS mpc107MsgI2oInboundWrite
    (
    UINT32 *	pBuf  /* pointer to message to be written to the Inbound */
                      /* I2O interface */
    )
    {
    UINT32 **	ppTempMfa;      /* pointer to Message Frame pointer */
    UINT32 *	pTempWriteMfa;  /* pointer to Message Frame  */
    ULONG 	tempIndex;      /* index */

    /* Read Inbound Fifo Queue Port Register  to Get MFA */

    ppTempMfa = (UINT32 **)mpc107MsgRegMod (pciBaseAddr, MPC107_MSG_READ,
                                            MPC107_MSG_I2O_PCI_IFQPR, 0, 0);

    /* Check to see if the Inbound interface has free MFAs  */

    if (ppTempMfa == (UINT32 **)MPC107_MSG_I2O_NO_FREE_MFA)
        {

        return(ERROR) ;  /* No free MFAs */

        }
    else  /* Free MFAs are available */
        {

        pTempWriteMfa =  *(ppTempMfa) ;

        /* Write the message in the Message Frame */

        for (tempIndex = 0; tempIndex < MPC107_MSG_I2O_MFRAME_SIZE;
                            tempIndex++)
            {

            *(UINT32 *)(pTempWriteMfa + tempIndex) =
                                      *(UINT32 *)(pBuf + tempIndex);
            }

        /* Write the MFA to the Inbound Fifo Queuue Port Register  */

        mpc107MsgRegMod (pciBaseAddr, MPC107_MSG_WRITE,
                         MPC107_MSG_I2O_PCI_IFQPR, (UINT32)ppTempMfa, 0);

        return (OK);
        }

   }



/***************************************************************************
*
* mpc107MessageInInt - ISR for interrupts from the Inbound Message Unit
*
* This routine services the Inbound Message,Inbound Doorbell and
* Inbound I2O interrupts .
*
* RETURNS: N/A
*/

void mpc107MessageInInt (void)
    {

     ULONG 	tempStatusData; /* interrupt status register contents */
     ULONG 	tempMaskData;   /* interrupt Mask register contents */
     UINT32 	pMessageRead;   /* data read */

     /* Read the Inbound Message Interrupt Status Register */

    tempStatusData = mpc107MsgRegMod (0x0,MPC107_MSG_READ,
                                          MPC107_MSG_I2O_IMISR, 0, 0);

    /* Read the Inbound Message Interrupt Mask Register */

    tempMaskData = mpc107MsgRegMod (0x0, MPC107_MSG_READ ,
                                         MPC107_MSG_I2O_IMIMR, 0, 0);

    /* Inbound Message 0 Interrupt */

    if ((tempStatusData & MPC107_MSG_I2O_IM0I & MPC107_MSG_I2O_IM0IM) != 0)
        {

        mpc107MsgMessageRead (MPC107_MSG_INBOUND ,
                              MPC107_MSG_REGISTER0 , &pMessageRead);
        }

    /* Inbound Message 1 Interrupt */

    if ((tempStatusData & MPC107_MSG_I2O_IM1I & MPC107_MSG_I2O_IM1IM) !=0 )
        {

         mpc107MsgMessageRead (MPC107_MSG_INBOUND ,
                               MPC107_MSG_REGISTER1 , &pMessageRead);

        }

    /* Inbound Doorbell  Interrupt */

    if ((tempStatusData & MPC107_MSG_I2O_IDI & MPC107_MSG_I2O_IDIM) !=0 )
        {

        mpc107MsgDoorbellRead (MPC107_MSG_INBOUND,&pMessageRead);

        }

    /* Inbound Doorbell  Register Machine Check */

    if ((tempStatusData & MPC107_MSG_I2O_MCI & MPC107_MSG_I2O_MCIM) != 0)
        {

        }

    /* Inbound Post Queue interrupt ,indicating  MFA in IFQPR */

    if ((tempStatusData & MPC107_MSG_I2O_IPQI & MPC107_MSG_I2O_IPQIM) !=0 )
        {

           mpc107MsgI2oInboundRead();

        }

    /* Inbound Post List Overflow  */

    if ((tempStatusData & MPC107_MSG_I2O_IPOI & MPC107_MSG_I2O_IPOIM) != 0)
        {


        }

   /* Outbound Post List Overflow  */

    if ((tempStatusData & MPC107_MSG_I2O_OFOI & MPC107_MSG_I2O_OFOIM) != 0)
        {

        }


    /* Clear the interrupts */

    mpc107MsgRegMod (0x0,MPC107_MSG_WRITE,
                         MPC107_MSG_I2O_IMISR, tempStatusData, 0);

    }


/***************************************************************************
*
* mpc107MessageOutInt - ISR for the interrupts  from Outbound Message Unit
*
* This routine services the Outbound Message,Outbound Doorbell and
* Outbound I2O interrupts .
*
* RETURNS: N/A
*/

void mpc107MessageOutInt (void)
    {

     ULONG 	tempStatusData; /* interrupt status register contents */
     ULONG 	tempMaskData;   /* interrupt Mask register contents */
     UINT32 	pMessageRead;

    /* Read the Outbound Message Interrupt Status Register */

    tempStatusData = mpc107MsgRegMod (pciBaseAddr, MPC107_MSG_READ,
                                      MPC107_MSG_I2O_PCI_OMISR, 0, 0);

    /* Read the Outbound Message Interrupt Mask Register */

    tempMaskData = mpc107MsgRegMod (pciBaseAddr, MPC107_MSG_READ ,
                                    MPC107_MSG_I2O_PCI_OMIMR, 0, 0);


    /* Outbound Message 0 Interrupt */

    if ((tempStatusData & MPC107_MSG_I2O_OM0I & MPC107_MSG_I2O_OM0IM) != 0)
        {

        mpc107MsgMessageRead (MPC107_MSG_OUTBOUND ,
                              MPC107_MSG_REGISTER0 , &pMessageRead);
        }

    /* Outbound Message 1 Interrupt */

    if ((tempStatusData & MPC107_MSG_I2O_OM1I & MPC107_MSG_I2O_OM1IM) != 0)
        {

         mpc107MsgMessageRead (MPC107_MSG_OUTBOUND ,
                               MPC107_MSG_REGISTER1 , &pMessageRead);
        }

    /* Outbound Doorbell  Interrupt */

    if ((tempStatusData & MPC107_MSG_I2O_ODI & MPC107_MSG_I2O_ODIM) != 0)
        {

        mpc107MsgDoorbellRead (MPC107_MSG_OUTBOUND, &pMessageRead);

        }

    /* Outbound Post Queue interrupt ,indicating  MFA in OFQPR */

    if ((tempStatusData & MPC107_MSG_I2O_OPQI & MPC107_MSG_I2O_OPQIM) != 0)
        {

           mpc107MsgI2oOutboundRead();

        }

    /* Clear the interrupts */

    mpc107MsgRegMod (pciBaseAddr,MPC107_MSG_WRITE,
                     MPC107_MSG_I2O_PCI_OMISR, tempStatusData, 0);

    }

/***************************************************************************
*
* mpc107MsgRegMod - i2o Registers In/OutLong and/or-ing wrapper.
*
* The purpose of this function is to perform and, or and
* and/or i2oPciIn/OutLong operations with syncronization.
*
* RETURNS: The data  for read operations.
*/

LOCAL UINT32 mpc107MsgRegMod
    (
    UINT32 	pciBar,        /* Zero for EUMBBAR or a base Address */
                               /* for PCI access */
    UINT32 	ioControlFlag, /* input/ouput control flag */
                               /* 0, write */
                               /* 1, read */
                               /* 2, read/modify/write (ORing) */
                               /* 3, read/modify/write (ANDing) */
                               /* 4, read/modify/write (AND/ORing) */
    UINT32 	address,       /* address of device register  */
    UINT32 	wdata1,        /* data item 1 for read/write operation */
    UINT32 	wdata2         /* data item 2 for read/write operation */
    )
    {
    UINT32 i2oTempData;

    if (ioControlFlag == MPC107_MSG_WRITE) /* Write */
        {

        /*
         * Data wdata1 is to be written in
         * the register specified by address
         */

        mpc107MsgWrite (pciBar, address, wdata1);

        }

    else if (ioControlFlag == MPC107_MSG_READ) /* Read */
        {

        /*
         * Data wdata1 is read from the register
         * specified by address
         */

        i2oTempData = mpc107MsgRead (pciBar, address);

        }

    else if (ioControlFlag == MPC107_MSG_READ_OR_WRITE) /* Read OR write */
        {

        /*
         * Data wdata1 is bitwise ORed with the data
         * read from the register specified by address
         * and the resultant  data is written to the
         * register
         */

        i2oTempData = mpc107MsgRead (pciBar, address);
        SYNC;
        i2oTempData |= wdata1;
        mpc107MsgWrite (pciBar, address, i2oTempData);

        }

    else if (ioControlFlag == MPC107_MSG_READ_AND_WRITE)  /* Read AND Write */
        {

        /*
         * Data wdata1 is bitwise ANDed with the data
         * read from the register specified by address
         * and the resultant  data is written to the
         * register
         */

        i2oTempData = mpc107MsgRead (pciBar, address);
        SYNC;
        i2oTempData &= wdata1;
        mpc107MsgWrite (pciBar, address, i2oTempData);
        }

     else if (ioControlFlag == MPC107_MSG_READ_ANDOR_WRITE)
        {

        /*
         * Data wdata1 is bitwise ANDed with the data
         * read from the register specified by address
         * and  data wdata2 is bitwise ORed  ,
         * and the resultant  data is written to the register
         */

        i2oTempData = mpc107MsgRead (pciBar, address);
        SYNC;
        i2oTempData &= wdata1;
        i2oTempData |= wdata2;
        mpc107MsgWrite (pciBar, address, i2oTempData);
        }
    SYNC;
    return (i2oTempData);

    }

/***************************************************************************
*
* mpc107MsgWrite - write data to  Message Unit Registers
*
* This routine writes the data in the specified Register
* added to the offset  specified by the base address register .
*
* RETURNS: N/A
*/

LOCAL void mpc107MsgWrite
    (
    UINT32 	pciBar,        /* Zero for EUMBBAR or a base Address */
                               /* for PCI access */
    UINT32 	messageRegNum, /* Message Register Number */
    UINT32 	message        /* Message */
    )
    {

    if ((pciBar) != 0)
        {

        *(UINT32 *)(pciBar + messageRegNum) = MPC107LONGSWAP(message);

        }
    else
        {

        MPC107EUMBBARWRITE (messageRegNum, message);

        }

    }

/***************************************************************************
*
* mpc107MsgRead - read data from  Message Unit Registers
*
* This routine  reads  the data in the specified Register  added to the offset
* specified by the base address register.
*
* RETURNS: The data read from the register is returned
*/

LOCAL ULONG mpc107MsgRead
    (
    UINT32 	pciBar,        /* zero for EUMBBAR or a base Address */
                               /* for PCI access */
    UINT32 	messageRegNum  /* message register number */
    )
    {

    if ((pciBar) != 0)
        {

        return (*(UINT32 *)(pciBar + messageRegNum));

        }
    else
        {

        return (MPC107EUMBBARREAD(messageRegNum));

        }
    }
