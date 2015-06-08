/* mv64360MpscSerial.c - BSP serial device initialization */

/* Copyright 1984-1997,1999 Wind River Systems, Inc. */
/* Copyright 1999-2003 Motorola, Inc. All Rights Reserved */

#include "copyright_wrs.h"

/*
modification history
--------------------
01a,29Jul03,simon  ported from motoload.
*/

/* 
DESCRIPTION
This file contains serial driver over MPSC serial channel in debug mode.

DESCRIPTION
This is the driver for the mv64360 MPSC DUART work in debug mode . 
This device includes two MPSC for two universal 
asynchronous receiver/transmitters and can use clock from two baud rate 
generator. 

A mv64360Serial_CHAN structure is used to describe the serial channel. 
This data structure is defined in mv64360MpscSerial.h.

Only asynchronous serial operation is supported by this driver.The default 
serial settings are 8 data bits, 1 stop bit, no parity, 9600baud, and software 
flow control.  

USAGE
The BSP's sysHwInit() routine typically calls sysSerialHwInit(),which creates 
the mv64360Serial_CHAN structure and initializes all the values in the 
structure (except the SIO_DRV_FUNCS) before calling mv64360SerialInit().
The BSP's sysHwInit2() routine typically calls sysSerialHwInit2(), which
connects the chips interrupts via intConnect().

This driver handles setting of hardware options such as parity(odd, even) and
number of data bits(5, 6, 7, 8). Hardware flow control is provided with the
handshakes RTS/CTS. 

INCLUDE FILES: mv64360UartDrv.h

*/

/* includes */
#include "vxWorks.h"
#include "config.h"
#include "errno.h"
#include "mv64360UartDrv.h"
#include "mv64360.h"

/* defines */
extern UINT32 cpuBusSpeed;

/* locals   */

LOCAL int mv64360UartDrvIoctl(mv64360Serial_CHAN *,int ,int );	/* driver/device I/O control */
LOCAL int mv64360UartDrvRxDPoll(SIO_CHAN *,char *);	/* driver/device RxD poll */
LOCAL void mv64360UartDrvSetBaud(mv64360Serial_CHAN *,int );	/* driver/device baud rate set */
LOCAL void mv64360UartDrvSetMode(mv64360Serial_CHAN *, int );	/* driver/device operating mode set */
LOCAL int mv64360UartDrvTxDPoll(SIO_CHAN *,char);	/* driver/device TxD poll */
LOCAL int mv64360UartDrvTxDStartup(mv64360Serial_CHAN *);/* driver/device TxD startup */
LOCAL int mv64360UartDrvCallbackInstall(SIO_CHAN *, int, STATUS (*)(),void *);/* install ISR callbacks to get/put chars. */

/* local driver function table */

LOCAL SIO_DRV_FUNCS mv64360SerialDrvFuncs =
    {
    (int (*)())			mv64360UartDrvIoctl,
    (int (*)())			mv64360UartDrvTxDStartup,
    (int (*)())                 mv64360UartDrvCallbackInstall,
    (int (*)())			mv64360UartDrvRxDPoll,
    (int (*)(SIO_CHAN *,char))	mv64360UartDrvTxDPoll
    };

/*******************************************************************************
* mv64360SerialInit - Initialize the MPSC as debug serial port.
*
* This routine initializes some SIO_CHAN function pointers and then resets
* the chip in a quiescent state.  Before this routine is called, the BSP
* must already have initialized all the device addresses, etc. in the
* mv64360Serial_CHAN structure.
*
* INPUT:
*       mv64360Serial_CHAN     *pChan      MV serial channel struct.
*
* RETURNS: N/A
*
*******************************************************************************/

void mv64360UartDrvInit(mv64360Serial_CHAN *pChan)
{	
	UINT32 temp1;

	if (1 < pChan->MPSCNum) return;
	if (1 < pChan->BRGNum) return;

	/* initialize the driver function pointers in the SIO_CHAN's */

	pChan->pDrvFuncs    = &mv64360SerialDrvFuncs;

	/* set the non BSP-specific constants */

	/* pChan->channelMode  = SIO_MODE_POLL; */    /* undefined */
	pChan->channelMode  = 0;

	/*
	 * The following programming should be performed on MPSC Mask
	 *
	 * Bit 0 = 0 disable Mpsc0Rx Interrupt.
	 *     1 = 0  disable Mpsc0RxErr interrupt.
	 *     2 = 0  reserved.
	 *     3 = 0  disable Mpsc0TxErr interrupt.
	 *     4 = 0  disable Mpsc0RLSC interrupt.
	 *     5 = 0  disable Mpsc0RHNT interrupt.
	 *     6 = 0  disable Mpsc0RFSC interrupt.
	 *     7 = 0  disable Mpsc0RCSC interrupt.
	 *     8 = 0  disable Mpsc0ROVR interrupt.
	 *     9 = 0  disable Mpsc0RCDL interrupt.
	 *     10 = 0  disable Mpsc0RCKG interrupt.
	 *     11 = 0  disable MPsc0BPER interrupt.
	 *     12 = 0  disable Mpsc0TEIDL interrupt.
	 *     13 = 0  disable Mpsc0TUDR interrupt.
	 *     14 = 0  disable Mpsc0TCTSL interrupt.
	 *     15 = 0  disable Mpsc0TCKG interrupt.
	 *     31:16 = 0  reserved.
	 */
	MV64360_WRITE32_PUSH (MV64360_REG_BASE,  \
							(CUI_MPSC_MASK0 + 0x8 * pChan->MPSCNum),0);

	/*
	 * The following programming should be performed on MPSC Routing (MRR)
	 *
	 * Bit 2:0 = 0  MPSC0 Routing Connected.
	 *     5:3 = 7  Reserved.
	 *     8:6 = 0  MPSC1 Routing Connected.
	 *     31:9 = 0  Reserved.
	 */
	temp1 = MV64360_READ32 (MV64360_REG_BASE, CUI_MPSC_ROUTG);
	temp1 &= ~(0x07 << (6 * pChan->MPSCNum));
	MV64360_WRITE32_PUSH (MV64360_REG_BASE,  CUI_MPSC_ROUTG, temp1);
   
	/*
	 * The following programming should be performed on MPSC Routing (MRR)
	 *
	 * Bit 2:0 = 0  MPSC0 Routing Connected.
	 *     5:3 = 7  Reserved.
	 *     8:6 = 0  MPSC1 Routing Connected.
	 *     31:9 = 0  Reserved.
	 */
	temp1 = MV64360_READ32 (MV64360_REG_BASE, CUI_RX_CLK_ROUTG);
	temp1 &= ~(0x07 << (8 * pChan->MPSCNum));
	MV64360_WRITE32_PUSH (MV64360_REG_BASE,  CUI_RX_CLK_ROUTG, temp1);

	/*
	 * The following programming should be performed on MPSC Routing (MRR)
	 *
	 * Bit 2:0 = 0  MPSC0 Routing Connected.
	 *     5:3 = 7  Reserved.
	 *     8:6 = 0  MPSC1 Routing Connected.
	 *     31:9 = 0  Reserved.
	 */
	temp1 = MV64360_READ32 (MV64360_REG_BASE, CUI_TX_CLK_ROUTG);
	temp1 &= ~(0x07 << (8 * pChan->MPSCNum));
	MV64360_WRITE32_PUSH (MV64360_REG_BASE,  CUI_TX_CLK_ROUTG, temp1);

	/*
	* set the initial baud rate
	*/
	mv64360UartDrvSetBaud(pChan, pChan->baudRate);

	/*
	* set the initial operating mode, interrupt mode
	*/
	mv64360UartDrvSetMode(pChan, -1);
	/*
	* clear receive buffer and pending interrupts
	*/
	MV64360_WRITE32_PUSH(MV64360_REG_BASE, \
						(CUI_MPSC_CAUSE0 + 0x8 * pChan->MPSCNum),  0);
	
	return ;
}

/*
 * mv64360UartDrvInt - driver/device interrupt service routine
 * description:
 *	This function's purpose is to perform the necessary actions
 *	when the device interrupt is asserted (i.e., interrupt service
 *	routine).
 * call:
 *	argument #1 = pointer to serial driver table entry
 * return:
 *	none
 */
 
void mv64360UartDrvInt(mv64360Serial_CHAN *pChan)
{
	UINT32 errorMask = 0;
	UINT32 errorCode = 0;
	UINT32 registerData = 0;
	UINT8 characterData = 0;
    
	errorMask = MV64360_READ32( MV64360_REG_BASE, \
							(CUI_MPSC_MASK0 + 0x8 * pChan->MPSCNum));
	MV64360_WRITE32_PUSH(MV64360_REG_BASE, \
							(CUI_MPSC_MASK0 + 0x8 * pChan->MPSCNum),  \
							0);
	errorCode = MV64360_READ32( MV64360_REG_BASE, \
							(CUI_MPSC_CAUSE0 + 0x8 * pChan->MPSCNum));
	MV64360_WRITE32_PUSH(MV64360_REG_BASE, \
							(CUI_MPSC_CAUSE0 + 0x8 * pChan->MPSCNum),  \
							0);
	
	if (errorCode & MV64360_UART_INTERRUPT_CAUSE_TEIDL)
      	{
		if ((*pChan->getTxChar) (pChan->getTxArg, &characterData) == OK)	
			{
				if (pChan->channelMode == SIO_MODE_POLL)
				    return;

				/* is the transmitter ready to accept a character? */
				
				registerData = MV64360_READ32( MV64360_REG_BASE, \
										(CHANNEL0_REGISTER2 + 0x1000 * pChan->MPSCNum));
				while  (0 != (registerData & MV64360_UART_CHR2_COMMAND_TCS)) 
					registerData = MV64360_READ32( MV64360_REG_BASE, \
											(CHANNEL0_REGISTER2 + 0x1000 * pChan->MPSCNum));
											
				/* set byte to transmit */
				registerData = MV64360_READ32( MV64360_REG_BASE, \
										(CHANNEL0_REGISTER1 + 0x1000 * pChan->MPSCNum));
				registerData = (registerData & (~(0xff))) | characterData;
				MV64360_WRITE32_PUSH (MV64360_REG_BASE,  \
										(CHANNEL0_REGISTER1 + 0x1000 * pChan->MPSCNum), \
										registerData);
				
				registerData = MV64360_READ32( MV64360_REG_BASE, \
										(CHANNEL0_REGISTER2 + 0x1000 * pChan->MPSCNum));
				/* MPSC as UART transmit command */
				registerData |=  MV64360_UART_CHR2_COMMAND_TCS; 
				/* start MPSC as UART transmit */
				MV64360_WRITE32_PUSH (MV64360_REG_BASE,  \
										(CHANNEL0_REGISTER2 + 0x1000 * pChan->MPSCNum), \
										registerData);
			}
	 }      	

	if (errorCode & MV64360_UART_INTERRUPT_CAUSE_RCC)
	{
		/*   Rx Buffer Return */
		registerData = MV64360_READ32( MV64360_REG_BASE, \
						(MPSC0_CHANNEL_REG10 + 0x1000 * pChan->MPSCNum));
		characterData = (registerData >> 16) & 0xff;
		MV64360_WRITE32_PUSH(MV64360_REG_BASE, \
						(MPSC0_CHANNEL_REG10 + 0x1000 * pChan->MPSCNum),  \
						registerData);
         
		(*pChan->putRcvChar) (pChan->putRcvArg, characterData);
         }
	
	MV64360_WRITE32_PUSH(MV64360_REG_BASE, \
							(CUI_MPSC_MASK0 + 0x8 * pChan->MPSCNum),  \
							errorMask);
   } 

/*******************************************************************************
* mv64360SerialIoctl - special device control

*
* DESCRIPTION:
*		This routine controls the device basic functionality like baud rate
*		and mode (polling or interrupt).
*
* INPUT:
*       *pChan  - MV serial channel struct, device to control.
*       request - request code      
*       arg     - some argument     
*
* OUTPUT:
*		See description.
*
* RETURN:
* 		OK on success, EIO on device error, ENOSYS on unsupported request.
*
*******************************************************************************/

LOCAL STATUS mv64360UartDrvIoctl(mv64360Serial_CHAN *pChan, int request, int arg)
{
    STATUS 	status = OK;
    
   /*  perform the specified function  */
   switch (request) {
      case SIO_MODE_SET:
		if (!( arg == SIO_MODE_POLL ||  arg == SIO_MODE_INT))
		{
			status = EIO;
			break;
		}
		pChan->channelMode = (int)arg;
		mv64360UartDrvSetMode(pChan, -1);
		break;
      case SIO_BAUD_SET:
		pChan->baudRate = arg;
		mv64360UartDrvSetBaud(pChan, arg);
		break;
	case SIO_BAUD_GET:
		* (int *) arg = pChan->baudRate;
		break;
	case SIO_MODE_GET:
		* (int *) arg = pChan->channelMode;
		break;

	case SIO_AVAIL_MODES_GET:
	    *(int *)arg = SIO_MODE_INT | SIO_MODE_POLL;
		break;

	default:
		status = ENOSYS;
   }
   
   return (status);
}

/*******************************************************************************
* mv64360SerialBaudSet - set serial port baudrate.
*
* DESCRIPTION:
*		This routine is called to set serial port baudrate.
*
* INPUT:
*       *pChan  - MV serial channel struct.
*       arg - Required baud rate
*
* OUTPUT:
*       Appalling MPSC and BRG API to set the CDV parameter to produce the
*       necessary baud rate.
*
* RETURN:
*       false if the baud rate is zero or the baud rate can not be created 
*           using the given clock rate input.
*       true otherwise.
*
*******************************************************************************/

LOCAL void
mv64360UartDrvSetBaud(mv64360Serial_CHAN *pChan, int baudRate)
{
	unsigned int registerDivisor;
	int interruptKey;

	/*
	* calculate baud rate, TCDV = RCDV = 16. 
	* how to process when TCDV is different from RCDV?
	*/
	registerDivisor = pChan->sysCLK;
	registerDivisor /= (baudRate * 16 * 2);
	registerDivisor -= 1;

	/*
	* mask interrupts
	*/
	interruptKey = intLock();

	/*
	* write the calculated divisor to the device's baud rate registers
	*/
	MV64360_WRITE32_PUSH (MV64360_REG_BASE,  \
							(BRG_0_CFG_REG + 0x8 * pChan->BRGNum), \
							registerDivisor | MV64360_UART_BRG0_EN  \
							| MV64360_UART_BRG0_CLKS_SYSCLK);

	/*
	* restore interrupt mask
	*/
	intUnlock(interruptKey);
}

/*******************************************************************************
* mv64360UartDrvSetMode - set serial port baudrate.
*
* DESCRIPTION:
*		This routine is called to set serial port baudrate.
*
* INPUT:
*       *pChan  - MV serial channel struct.
*       arg - Required baud rate
*
* OUTPUT:
*       Appalling MPSC and BRG API to set the CDV parameter to produce the
*       necessary baud rate.
*
* RETURN:
*       false if the baud rate is zero or the baud rate can not be created 
*           using the given clock rate input.
*       true otherwise.
*
*******************************************************************************/

LOCAL void
mv64360UartDrvSetMode(mv64360Serial_CHAN *pChan, int initializationFlag)
{
	unsigned int lineSettings;
	int interruptKey;
	unsigned int registerIER;
	unsigned int registerMPCR;
	unsigned int registerCR;   
	UINT32 temp1;

	/*
	* save device interrupt state
	*/
	registerIER = MV64360_READ32( MV64360_REG_BASE, \
							(CUI_MPSC_MASK0 + 0x8 * pChan->MPSCNum));

	/*
	* disable all device interrupts
	*/
	MV64360_WRITE32_PUSH(MV64360_REG_BASE, \
							(CUI_MPSC_MASK0 + 0x8 * pChan->MPSCNum),  0x00);

	/*
	* retrieve line settings
	*/
	lineSettings = pChan->channelMode;

	/*
	* update mpsc main configuration low register
	*/
	temp1 = MV64360_READ32 (MV64360_REG_BASE, \
				(CUI_MPSCX_MAIN_CFG0_LO + 0x1000 * pChan->MPSCNum));
	temp1 |= (MV64360_UART_MAIN_CONFIG_LOW_MODE_UART \
	                                      | MV64360_UART_MAIN_CONFIG_LOW_NLM \
	                                      | MV64360_UART_MAIN_CONFIG_LOW_ET \
	                                      | MV64360_UART_MAIN_CONFIG_LOW_ER);
	MV64360_WRITE32_PUSH (MV64360_REG_BASE,  \
				(CUI_MPSCX_MAIN_CFG0_LO + 0x1000 * pChan->MPSCNum), 
				temp1);

	/*
	* update mpsc main configuration high register, maybe, TCDV and RCDV shoud be 
	* set before baud rate set.
	*/
	temp1 = MV64360_READ32 (MV64360_REG_BASE, \
				(CUI_MPSCX_MAIN_CFG0_HI + 0x1000 * pChan->MPSCNum));
	temp1 |= (MV64360_UART_MAIN_CONFIG_HIGH_TCDV_16X \
	                                      | MV64360_UART_MAIN_CONFIG_HIGH_RDW \
	                                      | MV64360_UART_MAIN_CONFIG_HIGH_RCDV_16X);
	MV64360_WRITE32_PUSH (MV64360_REG_BASE,  \
				(CUI_MPSCX_MAIN_CFG0_HI + 0x1000 * pChan->MPSCNum), 
				temp1);

	/*
	* form protocol configuration register value from the line settings
	*/
	registerMPCR = 0x0000;
	registerMPCR |= MV64360_UART_PROTOCOL_CONTROL_WLS_8BITS;
	registerMPCR |= MV64360_UART_PROTOCOL_CONTROL_ISO_ASYN;

	/*
	* write the device's uart protocol configuration register
	*/
	MV64360_WRITE32_PUSH (MV64360_REG_BASE,  \
							(MPSC0_PROTOCOL_CONFIGURATION + 0x1000 * pChan->MPSCNum), \
							registerMPCR);
	    
	registerCR = 0x0000;   
	registerCR |= MV64360_UART_CHR2_COMMAND_TPM_OPS;
	registerCR |= MV64360_UART_CHR2_COMMAND_RPM_OPS;

	/*
	* update mpsc char4 register 
	*/
	temp1 = MV64360_READ32 (MV64360_REG_BASE, \
				(CHANNEL0_REGISTER4 + 0x1000 * pChan->MPSCNum));
	temp1 |= (MV64360_UART_CHR4_FILTER_CFR \
	                                      | MV64360_UART_CHR4_FILTER_Z);
	MV64360_WRITE32_PUSH (MV64360_REG_BASE,  \
							(CHANNEL0_REGISTER4 + 0x1000 * pChan->MPSCNum), \
							temp1);
	                                     
	/*
	* update mpsc char5 register 
	*/
	temp1 = MV64360_READ32 (MV64360_REG_BASE, \
				(CHANNEL0_REGISTER5 + 0x1000 * pChan->MPSCNum));
	temp1 |= (MV64360_UART_CHR5_CHAR_INT \
	                                      | MV64360_UART_CHR5_CHAR_VALID);          
	MV64360_WRITE32_PUSH (MV64360_REG_BASE,  \
							(CHANNEL0_REGISTER5 + 0x1000 * pChan->MPSCNum), \
							temp1);

	/*
	* form interrupt enable register value from the line settings
	*/
	if (initializationFlag) 
	{
		 registerIER |= (MV64360_UART_INTERRUPT_MASK_RCC);
	}

	/*
	* write the device's uart char2 command register
	*/
	MV64360_WRITE32_PUSH (MV64360_REG_BASE,  \
							(CHANNEL0_REGISTER2 + 0x1000 * pChan->MPSCNum), \
							(registerCR | MV64360_UART_CHR2_COMMAND_EH));   

	/*
	* mask interrupts
	*/
	interruptKey = intLock();

	/*
	* restore device's interrupt enable state (modified)
	*/
	if (lineSettings == SIO_MODE_INT) 
	{
		registerIER = (MV64360_UART_INTERRUPT_MASK_TEIDL \
			               | MV64360_UART_INTERRUPT_MASK_RCC);
		MV64360_WRITE32_PUSH (MV64360_REG_BASE,  \
								(CUI_MPSC_MASK0 + 0x8 * pChan->MPSCNum), \
								registerIER);
	}

	/*
	* restore interrupt mask
	*/
	intUnlock(interruptKey);
}

/*******************************************************************************
* mv64360SerialTxStartup - transmitter startup routine
*
* DESCRIPTION:
*       This routine transmits characters taken from the VxWorks IO buffer by  
*       Tx callback function. 
*
* INPUT:
*       *pChan  - MV serial channel struct.
*
* OUTPUT:
*       Calling the channel transmit routine with the proper packet information.
*
* RETURN:
*       None.
*
*******************************************************************************/

LOCAL int mv64360UartDrvTxDStartup(mv64360Serial_CHAN *pChan)
{
	char	outChar;
	UINT32 temp1;

	if (pChan->channelMode == SIO_MODE_INT)
	{
		if ((*pChan->getTxChar) (pChan->getTxArg, &outChar) == OK)	
		{
		
			temp1 = MV64360_READ32( MV64360_REG_BASE, \
									(CHANNEL0_REGISTER2 + 0x1000 * pChan->MPSCNum));
			/* check the transmitter is free */
			while (temp1 &  MV64360_UART_CHR2_COMMAND_TCS)
			{ 
				temp1 = MV64360_READ32( MV64360_REG_BASE, \
										(CHANNEL0_REGISTER2 + 0x1000 * pChan->MPSCNum));
			}
			
			/* set byte to transmit */
			#if 0
			temp1 = MV64360_READ32( MV64360_REG_BASE, \
									(CHANNEL0_REGISTER1 + 0x1000 * pChan->MPSCNum));
			temp1 = (temp1 & (~(0xff))) | outChar;
			MV64360_WRITE32_PUSH (MV64360_REG_BASE,  \
									(CHANNEL0_REGISTER1 + 0x1000 * pChan->MPSCNum), \
									temp1);
			#endif
		       sysPciOutByte(MV64360_REG_BASE + (CHANNEL0_REGISTER1 + 0x1000 * pChan->MPSCNum), outChar);
			
			/* MPSC as UART transmit command */
			temp1 |=  MV64360_UART_CHR2_COMMAND_TCS; 
			/* start MPSC as UART transmit */
			MV64360_WRITE32_PUSH (MV64360_REG_BASE,  \
									(CHANNEL0_REGISTER2 + 0x1000 * pChan->MPSCNum), \
									temp1);
		}
		return (OK);
	}
	else
	{
		return (ENOSYS);
	}
}

/*******************************************************************************
* mv64360SerialPollOutput - output a character in polled mode.
*
* DESCRIPTION:
*       This function transmits a character using the given serial channel.
*       The routine fills the packet information struct with the proper 
*		information for the given character.
*
* INPUT:
*       *pSioChan - SIO channel struct.
*       outChar   - The transmitted character.
*
* OUTPUT:
*       Calling the channel transmit routine with the proper packet information.
*
* RETURN:
*       OK if a character arrived, ERROR on device error, EAGAIN
*          if the output buffer if full.

*
*******************************************************************************/

LOCAL int  mv64360UartDrvTxDPoll(SIO_CHAN *pSioChan, char outChar)
{
	UINT32 temp1;
	mv64360Serial_CHAN * pChan = (mv64360Serial_CHAN *) pSioChan;

	/* check if the Tx is free */
	temp1 = MV64360_READ32( MV64360_REG_BASE, \
							(CHANNEL0_REGISTER2 + 0x1000 * pChan->MPSCNum));
	while (0 != (temp1 & MV64360_UART_CHR2_COMMAND_TCS)) 
	{
		temp1 = MV64360_READ32( MV64360_REG_BASE, \
								(CHANNEL0_REGISTER2 + 0x1000 * pChan->MPSCNum));
	}
	
	/* set byte to transmit */
	temp1 = MV64360_READ32( MV64360_REG_BASE, \
							(CHANNEL0_REGISTER1 + 0x1000 * pChan->MPSCNum));
	temp1 = (temp1 & (~(0xff))) | outChar;
	MV64360_WRITE32_PUSH (MV64360_REG_BASE,  \
							(CHANNEL0_REGISTER1 + 0x1000 * pChan->MPSCNum), \
							temp1);
	
	temp1 = MV64360_READ32( MV64360_REG_BASE, \
							(CHANNEL0_REGISTER10 + 0x1000 * pChan->MPSCNum));
	/* MPSC as UART transmit command */
	temp1 |=  MV64360_UART_CHR2_COMMAND_TCS; 
	/* start MPSC as UART transmit */
	MV64360_WRITE32_PUSH (MV64360_REG_BASE,  \
							(CHANNEL0_REGISTER2 + 0x1000 * pChan->MPSCNum), \
							temp1);
	
	return (OK);
}

/*******************************************************************************
* mv64360SerialPollInput - input a character in polled mode.
*
* DESCRIPTION:
*       This function receives a character using the given serial channel.
*       The routine fills the given char buffer with the received char.
*
* INPUT:
*       *pSioChan - SIO channel struct.
*       *thisChar - Rx character buffer.
*
* OUTPUT:
*       Calling the channel receive routine with the proper packet information.
*
* RETURN:
*       OK if a character arrived, ERROR on device error, EAGAIN
*       if the output buffer if full.
*
*******************************************************************************/

LOCAL int mv64360UartDrvRxDPoll(SIO_CHAN *pSioChan, char *thisChar)
{
	UINT32 temp1, temp2;
	mv64360Serial_CHAN * pChan = (mv64360Serial_CHAN *) pSioChan;

    
	temp1 = MV64360_READ32( MV64360_REG_BASE, \
							(CUI_MPSC_CAUSE0 + 0x8 * pChan->MPSCNum));
	/* check if received a char */
	if (0 == (temp1 & MV64360_UART_INTERRUPT_CAUSE_RCC))
		return (EAGAIN);
	
	/* read recieved value */
	temp2 = MV64360_READ32( MV64360_REG_BASE, \
							(CHANNEL0_REGISTER10 + 0x1000 * pChan->MPSCNum));
	
	/* extract byte value */
	*thisChar = (temp2 >> 16) & 0xff;   
	/* clear RCRn field in the CHR10 register*/
	MV64360_WRITE32_PUSH (MV64360_REG_BASE,  \
							(CHANNEL0_REGISTER10 + 0x1000 * pChan->MPSCNum), \
							temp2);
	temp1 &= ~MV64360_UART_INTERRUPT_CAUSE_RCC;
	MV64360_WRITE32_PUSH (MV64360_REG_BASE,  \
							(CUI_MPSC_CAUSE0 + 0x8 * pChan->MPSCNum), \
							temp1);
	return (OK);

}

/*******************************************************************************
* mv64360UartDrvCallbackInstall - install ISR callbacks to get/put chars.
*
* DESCRIPTION:
*       This function installs ISR callbacks that transfers the data to and 
*       from the OS serial buffers.       
*
* INPUT:
*       *pSioChan      - SIO channel struct.
*       callbackType   - Rx or Tx type of callback routine.
*       (* callback)() - Callback routine.
*       *callbackArg   - Callback routine argument.
*
* OUTPUT:
*       Calling Rx callback will deliver the received data to the OS layer. 
*       Calling Tx callback will retrieve data from the OS layer.
*
* RETURN:
*       OK if callback installation succeeded, ENOSYS on wrong callbackType.
*
*******************************************************************************/

LOCAL int mv64360UartDrvCallbackInstall(SIO_CHAN *pSioChan, int	callbackType,
									STATUS (* callback)(), void *callbackArg)
{
    mv64360Serial_CHAN * pChan = (mv64360Serial_CHAN *) pSioChan;

    switch (callbackType)
    {
        case SIO_CALLBACK_GET_TX_CHAR:
            pChan->getTxChar    = callback;
            pChan->getTxArg     = callbackArg;
            return (OK);
	    break;

        case SIO_CALLBACK_PUT_RCV_CHAR:
            pChan->putRcvChar   = callback;
            pChan->putRcvArg    = callbackArg;
            return (OK);
	    break;

        default:
            return (ENOSYS);
    }
}
