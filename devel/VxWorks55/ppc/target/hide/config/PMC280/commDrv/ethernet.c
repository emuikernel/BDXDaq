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
* ethernet.c - Marvell's Gigabit Ethernet controller low level driver
*
* DESCRIPTION:
*       This file introduce low level API to Marvell's Gigabit Ethernet 
*		controller. This Gigabit Ethernet Controller driver API controls
*		1) Operations (i.e. port init, start, reset etc').
*		2) Data flow (i.e. port send, receive etc').
*		Each Gigabit Ethernet port is controlled via ETH_PORT_INFO struct.
*		This struct includes user configuration information as well as driver 
*		internal data needed for its operations.
*		                                                                       
*		Supported Features:													   
*		- This low level driver is OS independent. Allocating memory for the 
*		  descriptor rings and buffers are not within the scope of this driver.
*		- The user is free from Rx/Tx queue managing.
*		- This low level driver introduce functionality API that enable the 
*		  to operate Marvell's Gigabit Ethernet Controller in a convenient way.
*		- Simple Gigabit Ethernet port operation API.
*		- Simple Gigabit Ethernet port data flow API.
*		- Data flow and operation API support per queue functionality.
*		- Support cached descriptors for better performance.
*		- Enable access to all four DRAM banks and internal SRAM memory spaces.
*		- PHY access and control API.
*		- Port control register configuration API.
*		- Full control over Unicast and Multicast MAC configurations.
*																			   
*		Operation flow:
*
*		Initialization phase
*		This phase complete the initialization of the ETH_PORT_INFO struct. 
*		User information regarding port configuration has to be set prior to 
*		calling the port initialization routine. For example, the user has to 
*		assign the portPhyAddr field which is board depended parameter.
*		In this phase any port Tx/Rx activity is halted, MIB counters are 
*		cleared, PHY address is set according to user parameter and access to
*		DRAM and internal SRAM memory spaces.
*
*		Driver ring initialization
*		Allocating memory for the descriptor rings and buffers is not 
*		within the scope of this driver. Thus, the user is required to allocate 
*		memory for the descriptors ring and buffers. Those memory parameters 
*		are used by the Rx and Tx ring initialization routines in order to 
*		curve the descriptor linked list in a form of a ring. 
*		Note: Pay special attention to alignment issues when using cached 
*		descriptors/buffers. In this phase the driver store information in the 
*		ETH_PORT_INFO struct regarding each queue ring.
*
*		Driver start 
*		This phase prepares the Ethernet port for Rx and Tx activity. It uses 
*		the information stored in the ETH_PORT_INFO struct to initialize the 
*		various port registers.
*
*		Data flow:
*		All packet references to/from the driver are done using PKT_INFO struct.
*       This struct is a unified struct used with Rx and Tx operations. 
*       This way the user is not required to be familiar with neither Tx nor 
*       Rx descriptors structures.
*		The driver's descriptors rings are management by indexes. Those indexes
*		controls the ring resources and used to indicate a SW resource error:
*		'current' 
*			This index points to the current available resource for use. For 
*			example in Rx process this index will point to the descriptor  
*			that will be passed to the user upon calling the receive routine.
*			In Tx process, this index will point to the descriptor
*			that will be assigned with the user packet info and transmitted.
*		'used'    
*			This index points to the descriptor that need to restore its 
*			resources. For example in Rx process, using the Rx buffer return
*			API will attach the buffer returned in packet info to the descriptor 
*			pointed by 'used'. In Tx process, using the Tx descriptor return 
*			will merely return the user packet info with the command status of  
*			the transmitted buffer pointed by the 'used' index. Nevertheless, it 
*			is essential to use this routine to update the 'used' index.
*		'first'
*			This index supports Tx Scatter-Gather. It points to the first 
*			descriptor of a packet assembled of multiple buffers. For example 
*			when in middle of Such packet we have a Tx resource error the 
*			'curr' index get the value of 'first' to indicate that the ring 
*			returned to its state before trying to transmit this packet.
*																			   
*		Receive operation:
*		The ethPortReceive API set the packet information struct, passed by the 
*       caller, with received information from the 'current' SDMA descriptor. 
*		It is the user responsibility to return this resource back to the Rx 
*       descriptor ring to enable the reuse of this source. Return Rx resource 
*       is done using the ethRxReturnBuff API.
*		                                                                       
*		Transmit operation:
*		The ethPortSend API supports Scatter-Gather which enables to send a 
*		packet spanned over multiple buffers. This means that for each 
*		packet info structure given by the user and put into the Tx descriptors 
*		ring, will be transmitted only if the 'LAST' bit will be set in the 
*		packet info command status field. This API also consider restriction 
*       regarding buffer alignments and sizes.
*		The user must return a Tx resource after ensuring the buffer has been 
*		transmitted to enable the Tx ring indexes to update.
*		                                                                       
*		BOARD LAYOUT                                                           
*		This device is on-board.  No jumper diagram is necessary.              
*		                                                                       
*		EXTERNAL INTERFACE                                                     
*		                                                                       
*       Prior to calling the initialization routine ethPortInit() the user must
*       set the following fields under ETH_PORT_INFO struct:		                                                                       
*       portNum             User Ethernet port number.
*       portPhyAddr		    User PHY address of Ethernet port.
*       portMacAddr[6]	    User defined port MAC address.
*       portConfig          User port configuration value.
*       portConfigExtend    User port config extend value.
*       portSdmaConfig      User port SDMA config value.
*       portSerialControl   User port serial control value.
*       *portVirtToPhys ()  User function to cast virtual addr to CPU bus addr.
*       *portPrivate        User scratch pad for user specific data structures.
*		                                                                       
*       This driver introduce a set of default values:
*       PORT_CONFIG_VALUE           Default port configuration value
*       PORT_CONFIG_EXTEND_VALUE    Default port extend configuration value
*       PORT_SDMA_CONFIG_VALUE      Default sdma control value
*       PORT_SERIAL_CONTROL_VALUE   Default port serial control value
*
*		This driver data flow is done using the PKT_INFO struct which is a 
* 		unified struct for Rx and Tx operations:
*		byteCnt				Tx/Rx descriptor buffer byte count.
*		L4iChk				CPU provided TCP Checksum. For Tx operation only.
*		cmdSts				Tx/Rx descriptor command status.
*		bufPtr				Tx/Rx descriptor buffer pointer.
*		returnInfo			Tx/Rx user resource return information.
*		                                                                       
*
*		EXTERNAL SUPPORT REQUIREMENTS                                          
*		                                                                       
*		This driver requires the following external support:
*		                                                                       
*		D_CACHE_FLUSH_LINE (address, address offset)
*		                                                                       
*			This macro applies assembly code to flush and invalidate cache line.
*		address        - address base.                                 
*		address offset - address offset                   
*		                                                                       
*			                                                                   
*		CPU_PIPE_FLUSH
*		                                                                       
*			This macro applies assembly code to flush the CPU pipeline.
*		                                                                       
* Sept 28th 2002
* Commented out all the D-CACHE FLUSH and INVALIDATE operations for TX/RX descriptors,as they have been made non-cached.
* ethPortReceive routine no longer being used.
******************************************************************************/
/* includes */
#include "ethernet.h"   
#include "gtMemory.h"   
#include "gtSram.h"   

/* defines */
/* SDMA command macros */
#define ETH_ENABLE_TX_QUEUE(txQueue, ethPort)                     		\
    GT_REG_WRITE(ETH_TRANSMIT_QUEUE_COMMAND_REG(ethPort), (1 << txQueue)) 	
	
#define ETH_DISABLE_TX_QUEUE(txQueue, ethPort)                     		\
    GT_REG_WRITE(ETH_TRANSMIT_QUEUE_COMMAND_REG(ethPort), (1 << (8 + txQueue))) 

#define ETH_ENABLE_RX_QUEUE(rxQueue, ethPort)                     		\
    GT_REG_WRITE(ETH_RECEIVE_QUEUE_COMMAND_REG(ethPort), (1 << rxQueue)) 	

#define ETH_DISABLE_RX_QUEUE(rxQueue, ethPort)                     		\
    GT_REG_WRITE(ETH_RECEIVE_QUEUE_COMMAND_REG(ethPort), (1 << (8 + rxQueue))) 

#define CURR_RFD_GET(pCurrDesc, queue)									\
    ((pCurrDesc) = pEthPortCtrl->pRxCurrDescQ[queue])

#define CURR_RFD_SET(pCurrDesc, queue)									\
    (pEthPortCtrl->pRxCurrDescQ[queue] = (pCurrDesc))

#define USED_RFD_GET(pUsedDesc, queue)									\
    ((pUsedDesc) = pEthPortCtrl->pRxUsedDescQ[queue])

#define USED_RFD_SET(pUsedDesc, queue)									\
    (pEthPortCtrl->pRxUsedDescQ[queue] = (pUsedDesc))


#define CURR_TFD_GET(pCurrDesc, queue)									\
    ((pCurrDesc) = pEthPortCtrl->pTxCurrDescQ[queue])

#define CURR_TFD_SET(pCurrDesc, queue)									\
    (pEthPortCtrl->pTxCurrDescQ[queue] = (pCurrDesc))
	
#define USED_TFD_GET(pUsedDesc, queue)									\
    ((pUsedDesc) = pEthPortCtrl->pTxUsedDescQ[queue])
	
#define USED_TFD_SET(pUsedDesc, queue)									\
    (pEthPortCtrl->pTxUsedDescQ[queue] = (pUsedDesc))
	
#define FIRST_TFD_GET(pFirstDesc, queue)								\
    ((pFirstDesc) = pEthPortCtrl->pTxFirstDescQ[queue])
	
#define FIRST_TFD_SET(pFirstDesc, queue)								\
    (pEthPortCtrl->pTxFirstDescQ[queue] = (pFirstDesc))
	

/* Macros that save access to desc in order to find next desc pointer  */
#define RX_NEXT_DESC_PTR(pRxDesc, queue)									   \
	(ETH_RX_DESC*)(((((unsigned int)pRxDesc - 								   \
    (unsigned int)pEthPortCtrl->pRxDescAreaBase[queue]) + RX_DESC_ALIGNED_SIZE)\
    % pEthPortCtrl->rxDescAreaSize[queue]) + 								   \
    (unsigned int)pEthPortCtrl->pRxDescAreaBase[queue])

#define TX_NEXT_DESC_PTR(pTxDesc, queue)									   \
	(ETH_TX_DESC*)(((((unsigned int)pTxDesc - 								   \
    (unsigned int)pEthPortCtrl->pTxDescAreaBase[queue]) + TX_DESC_ALIGNED_SIZE)\
    % pEthPortCtrl->txDescAreaSize[queue]) + 								   \
    (unsigned int)pEthPortCtrl->pTxDescAreaBase[queue])

#define LINK_UP_TIMEOUT		100000
#define PHY_BUSY_TIMEOUT    10000000

/* locals */

/* PHY routines */
static void ethernetPhySet (ETH_PORT ethPortNum, int phyAddr);
static int  ethernetPhyGet (ETH_PORT ethPortNum);

/* Ethernet Port routines */
static void ethPortInitMacTables(ETH_PORT ethPortNum);
static void ethSetAccessControl(ETH_PORT ethPortNum, ETH_WIN_PARAM *param);
static bool ethPortUcAddr (ETH_PORT ethPortNum, 
                           unsigned char ucNibble, 
                           ETH_QUEUE queue, 
                           int option);
static bool ethPortSmcAddr(ETH_PORT ethPortNum, 
                           unsigned char mcByte, 
                           ETH_QUEUE queue, 
                           int option);
static bool ethPortOmcAddr(ETH_PORT ethPortNum, 
                           unsigned char crc8, 
                           ETH_QUEUE queue, 
                           int option);

void ethBCopy(unsigned int srcAddr, unsigned int dstAddr, int byteCount);

void ethDbg(ETH_PORT_INFO *pEthPortCtrl);

/*******************************************************************************
* ethPortInit - Initialize the Ethernet port driver
*
* DESCRIPTION:
*       This function prepares the ethernet port to start its activity:
*       1) Completes the ethernet port driver struct initialization toward port
*           start routine.
*       2) Resets the device to a quiescent state in case of warm reboot.
*       3) Enable SDMA access to all four DRAM banks as well as internal SRAM.
*       4) Clean MAC tables. The reset status of those tables is unknown.
*       5) Set PHY address. 
*       Note: Call this routine prior to ethPortStart routine and after setting
*       user values in the user fields of Ethernet port control struct (i.e.
*       portPhyAddr).
*
* INPUT:
*       ETH_PORT_INFO 	*pEthPortCtrl       Ethernet port control struct
*
* OUTPUT:
*       See description.
*
* RETURN:
*       None.
*
*******************************************************************************/
void ethPortInit(ETH_PORT_INFO *pEthPortCtrl)
{
    int queue;
    ETH_WIN_PARAM winParam;
	
    pEthPortCtrl->portConfig 	    =	PORT_CONFIG_VALUE;
    pEthPortCtrl->portConfigExtend  = 	PORT_CONFIG_EXTEND_VALUE;
    pEthPortCtrl->portSdmaConfig    =	PORT_SDMA_CONFIG_VALUE;
    pEthPortCtrl->portSerialControl =	PORT_SERIAL_CONTROL_VALUE;

    pEthPortCtrl->portRxQueueCommand = 0;
    pEthPortCtrl->portTxQueueCommand = 0;

    /* Zero out SW structs */
	for(queue = 0; queue < MAX_RX_QUEUE_NUM; queue++)
	{
        CURR_RFD_SET ((ETH_RX_DESC*)0x00000000, queue);
        USED_RFD_SET ((ETH_RX_DESC*)0x00000000, queue);
		pEthPortCtrl->rxResourceErr[queue] = false;
	}
	
	for(queue = 0; queue < MAX_TX_QUEUE_NUM; queue++)
	{
        CURR_TFD_SET ((ETH_TX_DESC*)0x00000000, queue);
        USED_TFD_SET ((ETH_TX_DESC*)0x00000000, queue);
		FIRST_TFD_SET((ETH_TX_DESC*)0x00000000, queue);
		pEthPortCtrl->txResourceErr[queue] = false;
	}

	ethPortReset(pEthPortCtrl->portNum);     

	/* Set access parameters for DRAM bank 0 */
	winParam.win 		= ETH_WIN0;				/* Use Ethernet window 0	*/
	winParam.target 	= ETH_TARGET_DRAM;		/* Window target - DDR		*/
	winParam.attributes = EBAR_ATTR_DRAM_CS0;  	/* Enable DRAM bank  		*/
	winParam.highAddr 	= 0;
	winParam.baseAddr 	= gtMemoryGetBankBaseAddress(BANK0);/* Get bank base*/
	winParam.size 		= gtMemoryGetBankSize(BANK0);  		/* Get bank size*/
	winParam.enable 	= 1;				   	/* Enable the access		*/
	winParam.accessCtrl = EWIN_ACCESS_FULL;		/* Enable full access		*/

	/* Set the access control for address window (EPAPR) READ & WRITE */ 
	ethSetAccessControl(pEthPortCtrl->portNum, &winParam);

	/* Set access parameters for DRAM bank 1 */
	winParam.win 		= ETH_WIN1;				/* Use Ethernet window 1	*/
	winParam.target 	= ETH_TARGET_DRAM;		/* Window target - DDR		*/
	winParam.attributes = EBAR_ATTR_DRAM_CS1;  	/* Enable DRAM bank  		*/
	winParam.highAddr 	= 0;
	winParam.baseAddr 	= gtMemoryGetBankBaseAddress(BANK1);/* Get bank base*/
	winParam.size 		= gtMemoryGetBankSize(BANK1);  		/* Get bank size*/
	winParam.enable 	= 1;				   	/* Enable the access		*/
	winParam.accessCtrl = EWIN_ACCESS_FULL;		/* Enable full access		*/

	/* Set the access control for address window (EPAPR) READ & WRITE */ 
	ethSetAccessControl(pEthPortCtrl->portNum, &winParam);

	/* Set access parameters for DRAM bank 2 */
	winParam.win 		= ETH_WIN2;				/* Use Ethernet window 2	*/
	winParam.target 	= ETH_TARGET_DRAM;		/* Window target - DDR		*/
	winParam.attributes = EBAR_ATTR_DRAM_CS2;  	/* Enable DRAM bank  		*/
	winParam.highAddr 	= 0;
	winParam.baseAddr 	= gtMemoryGetBankBaseAddress(BANK2);/* Get bank base*/
	winParam.size 		= gtMemoryGetBankSize(BANK2);  		/* Get bank size*/
	winParam.enable 	= 1;				   	/* Enable the access		*/
	winParam.accessCtrl = EWIN_ACCESS_FULL;		/* Enable full access		*/

	/* Set the access control for address window (EPAPR) READ & WRITE */ 
	ethSetAccessControl(pEthPortCtrl->portNum, &winParam);

	/* Set access parameters for DRAM bank 3 */
	winParam.win 		= ETH_WIN3;				/* Use Ethernet window 3	*/
	winParam.target 	= ETH_TARGET_DRAM;		/* Window target - DDR		*/
	winParam.attributes = EBAR_ATTR_DRAM_CS3;  	/* Enable DRAM bank  		*/
	winParam.highAddr 	= 0;
	winParam.baseAddr 	= gtMemoryGetBankBaseAddress(BANK3);/* Get bank base*/
	winParam.size 		= gtMemoryGetBankSize(BANK3);  		/* Get bank size*/
	winParam.enable 	= 1;				   	/* Enable the access		*/
	winParam.accessCtrl = EWIN_ACCESS_FULL;		/* Enable full access		*/

	/* Set the access control for address window (EPAPR) READ & WRITE */ 
	ethSetAccessControl(pEthPortCtrl->portNum, &winParam);

	/* Set access parameters for Internal SRAM */
	winParam.win 		= ETH_WIN4;				/* Use Ethernet window 0	*/
	winParam.target 	= EBAR_TARGET_CBS;		/* Target - Internal SRAM	*/
	winParam.attributes = EBAR_ATTR_CBS_SRAM | EBAR_ATTR_CBS_SRAM_BLOCK0;
	winParam.highAddr 	= 0;
	winParam.baseAddr 	= gtMemoryGetInternalSramBaseAddr();/* Get base addr*/
	winParam.size 		= INTERNAL_SRAM_SIZE;  		/* Get bank size*/
	winParam.enable 	= 1;				   	/* Enable the access		*/
	winParam.accessCtrl = EWIN_ACCESS_FULL;		/* Enable full access		*/
	
	/* Set the access control for address window (EPAPR) READ & WRITE */ 
	ethSetAccessControl(pEthPortCtrl->portNum, &winParam);
    
	ethPortInitMacTables(pEthPortCtrl->portNum);

	ethernetPhySet(pEthPortCtrl->portNum, pEthPortCtrl->portPhyAddr);
	
	return;
    
} 

/*******************************************************************************
* ethPortStart - Start the Ethernet port activity.
*
* DESCRIPTION:
*       This routine prepares the Ethernet port for Rx and Tx activity:
*       1. Initialize Tx and Rx Current Descriptor Pointer for each queue that
*           has been initialized a descriptor's ring (using etherInitTxDescRing 
*           for Tx and etherInitRxDescRing for Rx)
*       2. Initialize and enable the Ethernet configuration port by writing to 
*           the port's configuration and command registers.
*       3. Initialize and enable the SDMA by writing to the SDMA's 
*    configuration and command registers.
*       After completing these steps, the ethernet port SDMA can starts to 
*       perform Rx and Tx activities.
*
*       Note: Each Rx and Tx queue descriptor's list must be initialized prior  
*       to calling this function (use etherInitTxDescRing for Tx queues and 
*       etherInitRxDescRing for Rx queues).
*
* INPUT:
*       ETH_PORT_INFO 	*pEthPortCtrl       Ethernet port control struct
*
* OUTPUT:
*       Ethernet port is ready to receive and transmit.
*
* RETURN:
*       false if the port PHY is not up.
*       true otherwise.
*
*******************************************************************************/
bool ethPortStart(ETH_PORT_INFO *pEthPortCtrl)
{
    int queue;
    volatile ETH_TX_DESC *pTxCurrDesc;
    volatile ETH_RX_DESC *pRxCurrDesc;
	unsigned int 	phyRegData;
	ETH_PORT ethPortNum = pEthPortCtrl->portNum;
	
	
	/* Assignment of Tx CTRP of given queue */
	for(queue = 0; queue < MAX_TX_QUEUE_NUM; queue++)
	{
        	CURR_TFD_GET (pTxCurrDesc, queue);
		GT_REG_WRITE((ETH_TX_CURRENT_QUEUE_DESC_PTR_0(ethPortNum)+ (4* queue)), 
					 pEthPortCtrl->portVirtToPhys((unsigned int)pTxCurrDesc));
									
	}
	
    /* Assignment of Rx CRDP of given queue */
	for(queue = 0; queue < MAX_RX_QUEUE_NUM; queue++)
	{
        CURR_RFD_GET (pRxCurrDesc, queue);
		GT_REG_WRITE((ETH_RX_CURRENT_QUEUE_DESC_PTR_0(ethPortNum)+ (4* queue)), 
					 pEthPortCtrl->portVirtToPhys((unsigned int)pRxCurrDesc));
		
		if(pRxCurrDesc != NULL)
			/* Add the assigned Ethernet address to the port's address table */
			ethPortUcAddrSet(pEthPortCtrl->portNum, pEthPortCtrl->portMacAddr, queue);
	}

    /* Assign port configuration and command. */
    GT_REG_WRITE(ETH_PORT_CONFIG_REG(ethPortNum), pEthPortCtrl->portConfig);
    
	GT_REG_WRITE(ETH_PORT_CONFIG_EXTEND_REG(ethPortNum),  
											pEthPortCtrl->portConfigExtend);
    
	GT_REG_WRITE(ETH_PORT_SERIAL_CONTROL_REG(ethPortNum), 
											pEthPortCtrl->portSerialControl);

    GT_SET_REG_BITS(ETH_PORT_SERIAL_CONTROL_REG(ethPortNum), 
													ETH_SERIAL_PORT_ENABLE);

	/* Assign port SDMA configuration */
    GT_REG_WRITE(ETH_SDMA_CONFIG_REG(ethPortNum), pEthPortCtrl->portSdmaConfig);
	
    GT_REG_WRITE(ETH_TX_QUEUE_0_TOKEN_BUCKET_COUNT(ethPortNum),  0x3fffffff);
    GT_REG_WRITE(ETH_TX_QUEUE_0_TOKEN_BUCKET_CONFIG(ethPortNum), 0x03fffcff);
	/* Turn off the port/queue bandwidth limitation */
    GT_REG_WRITE(ETH_MAXIMUM_TRANSMIT_UNIT(ethPortNum), 0x0);
    
    /* Enable port Rx. */
    GT_REG_WRITE(ETH_RECEIVE_QUEUE_COMMAND_REG(ethPortNum), 
											pEthPortCtrl->portRxQueueCommand);
	
	/* Check if link is up */
    ethPortReadSmiReg(ethPortNum, 1, &phyRegData);

    if(!(phyRegData & 0x20))
		return false;
	
	return true;
}

/*******************************************************************************
* ethPortUcAddrSet - This function Set the port Unicast address.
*
* DESCRIPTION:
*		This function Set the port Ethernet MAC address.
*
* INPUT:
*		ETH_PORT ethPortNum     Port number.
*		char *        pAddr		Address to be set 
*		ETH_QUEUE 	  queue		Rx queue number for this MAC address.
*
* OUTPUT:
*		Set MAC address low and high registers. also calls ethPortUcAddr() 
*       To set the unicast table with the proper information.
*
* RETURN:
*		N/A.
*
*******************************************************************************/
void ethPortUcAddrSet(ETH_PORT ethPortNum, unsigned char *pAddr,ETH_QUEUE queue)
{
    unsigned int    macH;
    unsigned int 	macL;
				
    macL =  (pAddr[4] << 8) | (pAddr[5]);
    macH =  (pAddr[0] << 24)| (pAddr[1] << 16) |
            (pAddr[2] << 8) | (pAddr[3] << 0);
			
    GT_REG_WRITE(ETH_MAC_ADDR_LOW(ethPortNum),  macL);
    GT_REG_WRITE(ETH_MAC_ADDR_HIGH(ethPortNum), macH);

    /* Accept frames of this address */
    ethPortUcAddr(ethPortNum, pAddr[5], queue, ACCEPT_MAC_ADDR);

	return;
}

/*******************************************************************************
* ethPortUcAddr - This function Set the port unicast address table
*
* DESCRIPTION:
*		This function locates the proper entry in the Unicast table for the 
*		specified MAC nibble and sets its properties according to function 
*		parameters.
*
* INPUT:
*		ETH_PORT 	ethPortNum      Port number.
*		unsigned char ucNibble		Unicast MAC Address last nibble. 
*		ETH_QUEUE 		 queue		Rx queue number for this MAC address.
*		int 			option      0 = Add, 1 = remove address.
*
* OUTPUT:
*		This function add/removes MAC addresses from the port unicast address
*		table. 
*
* RETURN:
*		true is output succeeded.
*		false if option parameter is invalid.
*
*******************************************************************************/
static bool ethPortUcAddr(ETH_PORT 		ethPortNum, 
						  unsigned char ucNibble, 
						  ETH_QUEUE 	queue, 
						  int 			option)
{
    unsigned int unicastReg;
    unsigned int tblOffset;
    unsigned int regOffset;
	
    /* Locate the Unicast table entry */
	ucNibble  = (0xf & ucNibble);	
	tblOffset = (ucNibble / 4) * 4; /* Register offset from unicast table base*/
	regOffset = ucNibble % 4;		/* Entry offset within the above register */
	
	switch(option)
	{
		case REJECT_MAC_ADDR:
            /* Clear accepts frame bit at specified unicast DA table entry */
            GT_REG_READ(
				(ETH_DA_FILTER_UNICAST_TABLE_BASE(ethPortNum) + tblOffset), 
				&unicastReg);
            
			unicastReg &= (0x0E << (8*regOffset));
            
			GT_REG_WRITE(
				(ETH_DA_FILTER_UNICAST_TABLE_BASE(ethPortNum) + tblOffset),
				unicastReg);
            break;
        
		case ACCEPT_MAC_ADDR:
            /* Set accepts frame bit at unicast DA filter table entry */
            GT_REG_READ(
				(ETH_DA_FILTER_UNICAST_TABLE_BASE(ethPortNum) + tblOffset), 
				&unicastReg);
            
			unicastReg |= ((0x01 | queue) << (8*regOffset));
            
			GT_REG_WRITE(
				(ETH_DA_FILTER_UNICAST_TABLE_BASE(ethPortNum) + tblOffset), 
				unicastReg);
            
			break;
        
		default:
            return false;
	}
	return true;
}
/*******************************************************************************
* ethPortMcAddr - Multicast address settings.
*
* DESCRIPTION:
*		This API controls the MV device MAC multicast support. 
*		The MV device supports multicast using two tables:
*		1) Special Multicast Table for MAC addresses of the form 
*		   0x01-00-5E-00-00-XX (where XX is between 0x00 and 0xFF). 
*		   The MAC DA[7:0] bits are used as a pointer to the Special Multicast 
*		   Table entries in the DA-Filter table.
*		   In this case, the function calls ethPortSmcAddr() routine to set the
*		   Special Multicast Table. 
*		2) Other Multicast Table for multicast of another type. A CRC-8bit  
*		   is used as an index to the Other Multicast Table entries in the 
*		   DA-Filter table.
*		   In this case, the function calculates the CRC-8bit value and calls 
*		   ethPortOmcAddr() routine to set the Other Multicast Table. 
*
* INPUT:
*		ETH_PORT 	ethPortNum      Port number.
*		unsigned char 	*pAddr		Unicast MAC Address. 
*		ETH_QUEUE 		 queue		Rx queue number for this MAC address.
*		int 			option      0 = Add, 1 = remove address.
*
* OUTPUT:
*		See description.
*
* RETURN:
*		true is output succeeded.
*		false if addAddressTableEntry( ) failed.
*
*******************************************************************************/
void ethPortMcAddr(ETH_PORT 	 ethPortNum, 
				   unsigned char *pAddr, 
				   ETH_QUEUE 	 queue, 
				   int 			 option)
{
    unsigned int    macH;
    unsigned int 	macL;
	unsigned char	crcResult = 0;
	int macArray[48];
	int crc[8];
	int i;
	

	if((pAddr[0] == 0x01) && 
	   (pAddr[1] == 0x00) && 
	   (pAddr[2] == 0x5E) && 
	   (pAddr[3] == 0x00) && 
	   (pAddr[4] == 0x00))

		ethPortSmcAddr(ethPortNum, pAddr[5], queue, option);
	else
	{
		/* Calculate CRC-8 out of the given address */
		macH =  (pAddr[0] << 8) | (pAddr[1]);
		macL =  (pAddr[2] << 24)| (pAddr[3] << 16) |
				(pAddr[4] << 8) | (pAddr[5] << 0);
	
		for(i = 0 ; i < 32 ; i++)
			macArray[i] = (macL >> i) & 0x1;
		for(i = 32 ; i < 48 ; i++)
			macArray[i] = (macH >> (i - 32)) & 0x1;

	
		crc[0] = macArray[45] ^ macArray[43] ^ macArray[40] ^ macArray[39] ^
				 macArray[35] ^ macArray[34] ^ macArray[31] ^ macArray[30] ^
				 macArray[28] ^ macArray[23] ^ macArray[21] ^ macArray[19] ^ 
				 macArray[18] ^ macArray[16] ^ macArray[14] ^ macArray[12] ^ 
				 macArray[8]  ^ macArray[7]  ^ macArray[6]  ^ macArray[0];
	
		crc[1] = macArray[46] ^ macArray[45] ^ macArray[44] ^ macArray[43] ^ 
				 macArray[41] ^ macArray[39] ^ macArray[36] ^ macArray[34] ^
				 macArray[32] ^ macArray[30] ^ macArray[29] ^ macArray[28] ^ 
				 macArray[24] ^ macArray[23] ^ macArray[22] ^ macArray[21] ^ 
				 macArray[20] ^ macArray[18] ^ macArray[17] ^ macArray[16] ^
				 macArray[15] ^ macArray[14] ^ macArray[13] ^ macArray[12] ^ 
				 macArray[9]  ^ macArray[6]  ^ macArray[1]  ^ macArray[0];
	
		crc[2] = macArray[47] ^ macArray[46] ^ macArray[44] ^ macArray[43] ^ 
				 macArray[42] ^ macArray[39] ^ macArray[37] ^ macArray[34] ^ 
				 macArray[33] ^ macArray[29] ^ macArray[28] ^ macArray[25] ^
				 macArray[24] ^ macArray[22] ^ macArray[17] ^ macArray[15] ^
				 macArray[13] ^ macArray[12] ^ macArray[10] ^ macArray[8]  ^
				 macArray[6]  ^ macArray[2]  ^ macArray[1]  ^ macArray[0];
	
		crc[3] = macArray[47] ^ macArray[45] ^ macArray[44] ^ macArray[43] ^ 
				 macArray[40] ^ macArray[38] ^ macArray[35] ^ macArray[34] ^ 
				 macArray[30] ^ macArray[29] ^ macArray[26] ^ macArray[25] ^ 
				 macArray[23] ^ macArray[18] ^ macArray[16] ^ macArray[14] ^ 
				 macArray[13] ^ macArray[11] ^ macArray[9]  ^ macArray[7]  ^ 
				 macArray[3]  ^ macArray[2]  ^ macArray[1];
	
		crc[4] = macArray[46] ^ macArray[45] ^ macArray[44] ^ macArray[41] ^
				 macArray[39] ^ macArray[36] ^ macArray[35] ^ macArray[31] ^
				 macArray[30] ^ macArray[27] ^ macArray[26] ^ macArray[24] ^
				 macArray[19] ^ macArray[17] ^ macArray[15] ^ macArray[14] ^
				 macArray[12] ^ macArray[10] ^ macArray[8]  ^ macArray[4]  ^
				 macArray[3]  ^ macArray[2];
	
		crc[5] = macArray[47] ^ macArray[46] ^ macArray[45] ^ macArray[42] ^
				 macArray[40] ^ macArray[37] ^ macArray[36] ^ macArray[32] ^
				 macArray[31] ^ macArray[28] ^ macArray[27] ^ macArray[25] ^
				 macArray[20] ^ macArray[18] ^ macArray[16] ^ macArray[15] ^
				 macArray[13] ^ macArray[11] ^ macArray[9]  ^ macArray[5]  ^
				 macArray[4]  ^ macArray[3];
	
		crc[6] = macArray[47] ^ macArray[46] ^ macArray[43] ^ macArray[41] ^
				 macArray[38] ^ macArray[37] ^ macArray[33] ^ macArray[32] ^
				 macArray[29] ^ macArray[28] ^ macArray[26] ^ macArray[21] ^
				 macArray[19] ^ macArray[17] ^ macArray[16] ^ macArray[14] ^
				 macArray[12] ^ macArray[10] ^ macArray[6]  ^ macArray[5]  ^
				 macArray[4];
	
		crc[7] = macArray[47] ^ macArray[44] ^ macArray[42] ^ macArray[39] ^ 
				 macArray[38] ^ macArray[34] ^ macArray[33] ^ macArray[30] ^ 
				 macArray[29] ^ macArray[27] ^ macArray[22] ^ macArray[20] ^ 
				 macArray[18] ^ macArray[17] ^ macArray[15] ^ macArray[13] ^ 
				 macArray[11] ^ macArray[7]  ^ macArray[6]  ^ macArray[5];
	
		for(i = 0 ; i < 8 ; i++)
			crcResult = crcResult | (crc[i] << i);
		
		ethPortOmcAddr(ethPortNum, crcResult, queue, option);
	}
	return;
}

/*******************************************************************************
* ethPortSmcAddr - Special Multicast address settings.
*
* DESCRIPTION:
*		This routine controls the MV device special MAC multicast support. 
*		The Special Multicast Table for MAC addresses supports MAC of the form 
*		0x01-00-5E-00-00-XX (where XX is between 0x00 and 0xFF). 
*		The MAC DA[7:0] bits are used as a pointer to the Special Multicast 
*		Table entries in the DA-Filter table.
*		This function set the Special Multicast Table appropriate entry 
*		according to the argument given.
*
* INPUT:
*		ETH_PORT 	ethPortNum      Port number.
*		unsigned char 	mcByte		Multicast addr last byte (MAC DA[7:0] bits).
*		ETH_QUEUE 		 queue		Rx queue number for this MAC address.
*		int 			option      0 = Add, 1 = remove address.
*
* OUTPUT:
*		See description.
*
* RETURN:
*		true is output succeeded.
*		false if option parameter is invalid.
*
*******************************************************************************/
static bool ethPortSmcAddr(ETH_PORT 	 ethPortNum, 
						   unsigned char mcByte, 
						   ETH_QUEUE 	 queue, 
						   int 			 option)
{    
    unsigned int smcTableReg;
    unsigned int tblOffset;
    unsigned int regOffset;

    /* Locate the SMC table entry */
	tblOffset = (mcByte / 4) * 4;	/* Register offset from SMC table base 	  */
	regOffset = mcByte % 4;			/* Entry offset within the above register */
    queue &= 0x7;
	
	switch(option)
	{
		case REJECT_MAC_ADDR:
            /* Clear accepts frame bit at specified Special DA table entry */
            GT_REG_READ((ETH_DA_FILTER_SPECIAL_MULTICAST_TABLE_BASE(ethPortNum) 
													+ tblOffset), &smcTableReg);
            smcTableReg &= (0x0E << (8 * regOffset));
            
			GT_REG_WRITE((ETH_DA_FILTER_SPECIAL_MULTICAST_TABLE_BASE(ethPortNum)
													 + tblOffset), smcTableReg);
            break;
        
		case ACCEPT_MAC_ADDR:
            /* Set accepts frame bit at specified Special DA table entry */
            GT_REG_READ((ETH_DA_FILTER_SPECIAL_MULTICAST_TABLE_BASE(ethPortNum) 
													+ tblOffset), &smcTableReg);
            smcTableReg |= ((0x01 | queue) << (8 * regOffset));
            
			GT_REG_WRITE((ETH_DA_FILTER_SPECIAL_MULTICAST_TABLE_BASE(ethPortNum) 
													 + tblOffset), smcTableReg);
            break;
        
		default:
            return false;
	}
	return true;
}

/*******************************************************************************
* ethPortOmcAddr - Multicast address settings.
*
* DESCRIPTION:
*		This routine controls the MV device Other MAC multicast support. 
*		The Other Multicast Table is used for multicast of another type. 
*		A CRC-8bit is used as an index to the Other Multicast Table entries 
*		in the DA-Filter table.
*		The function gets the CRC-8bit value from the calling routine and 
*       set the Other Multicast Table appropriate entry according to the 
*		CRC-8 argument given.
*
* INPUT:
*		ETH_PORT 	ethPortNum      Port number.
*		unsigned char 	  crc8		A CRC-8bit (Polynomial: x^8+x^2+x^1+1). 
*		ETH_QUEUE 		 queue		Rx queue number for this MAC address.
*		int 			option      0 = Add, 1 = remove address.
*
* OUTPUT:
*		See description.
*
* RETURN:
*		true is output succeeded.
*		false if option parameter is invalid.
*
*******************************************************************************/
static bool ethPortOmcAddr(ETH_PORT 	 ethPortNum, 
						   unsigned char crc8, 
						   ETH_QUEUE 	 queue, 
						   int 			 option)
{
    unsigned int omcTableReg;
    unsigned int tblOffset;
    unsigned int regOffset;

    /* Locate the OMC table entry */
    tblOffset = (crc8 / 4) * 4;		/* Register offset from OMC table base    */
	regOffset = crc8 % 4;       	/* Entry offset within the above register */
	queue &= 0x7;
    
	switch(option)
	{
		case REJECT_MAC_ADDR:
            /* Clear accepts frame bit at specified Other DA table entry */
            GT_REG_READ((ETH_DA_FILTER_OTHER_MULTICAST_TABLE_BASE(ethPortNum) 
													+ tblOffset), &omcTableReg);
            omcTableReg &= (0x0E << (8 * regOffset));
            
			GT_REG_WRITE((ETH_DA_FILTER_OTHER_MULTICAST_TABLE_BASE(ethPortNum) 
													 + tblOffset), omcTableReg);
            break;
        
		case ACCEPT_MAC_ADDR:
            /* Set accepts frame bit at specified Other DA table entry */
            GT_REG_READ((ETH_DA_FILTER_OTHER_MULTICAST_TABLE_BASE(ethPortNum) 
													+ tblOffset), &omcTableReg);
            omcTableReg |= ((0x01 | queue) << (8 * regOffset));
            
			GT_REG_WRITE((ETH_DA_FILTER_OTHER_MULTICAST_TABLE_BASE(ethPortNum)
													 + tblOffset), omcTableReg);
            break;
        
		default:
            return false;
	}
	return true;
}

/*******************************************************************************
* ethPortInitMacTables - Clear all entrance in the UC, SMC and OMC tables
*
* DESCRIPTION:
*       Go through all the DA filter tables (Unicast, Special Multicast & Other 
*       Multicast) and set each entry to 0.
*
* INPUT:
*		ETH_PORT    ethPortNum   Ethernet Port number. See ETH_PORT enum. 
*
* OUTPUT:
*       Multicast and Unicast packets are rejected.
*
* RETURN:
*       None.
*
*******************************************************************************/
void ethPortInitMacTables(ETH_PORT ethPortNum)
{
    int tableIndex;

    /* Clear DA filter unicast table (ExDFUT)*/
    for (tableIndex = 0; tableIndex <= 0xC; tableIndex += 4) 
        GT_REG_WRITE((ETH_DA_FILTER_UNICAST_TABLE_BASE(ethPortNum) + 
                                                               tableIndex), 0);
    
    for (tableIndex = 0; tableIndex <= 0xFC; tableIndex += 4) 
    {
        /* Clear DA filter special multicast table (ExDFSMT)*/
        GT_REG_WRITE((ETH_DA_FILTER_SPECIAL_MULTICAST_TABLE_BASE(ethPortNum) + 
                                                                tableIndex),0);
        /* Clear DA filter other multicast table (ExDFOMT)*/
        GT_REG_WRITE((ETH_DA_FILTER_OTHER_MULTICAST_TABLE_BASE(ethPortNum) + 
                                                                tableIndex),0);
    }
}

/*******************************************************************************
* ethClearMibCounters - Clear all MIB counters
*
* DESCRIPTION:
*       This function clears all MIB counters of a specific ethernet port.
*       A read from the MIB counter will reset the counter.
*
* INPUT:
*		ETH_PORT    ethPortNum   Ethernet Port number. See ETH_PORT enum. 
*
* OUTPUT:
*       After reading all MIB counters, the counters resets.
*
* RETURN:
*       MIB counter value.
*
*******************************************************************************/
void ethClearMibCounters (ETH_PORT ethPortNum)
{
	int 			i;
	unsigned int 	dummy;

    /* Perform dummy reads from MIB counters */
    for(i = ETH_MIB_GOOD_OCTETS_RECEIVED_LOW; i < ETH_MIB_LATE_COLLISION; i+= 4)
        GT_REG_READ((ETH_MIB_COUNTERS_BASE(ethPortNum) + i), &dummy);
                                                   
	return;
}

/*******************************************************************************
* ethReadMibCounter - Read a MIB counter
*
* DESCRIPTION:
*       This function reads a MIB counter of a specific ethernet port.
*       NOTE - If read from ETH_MIB_GOOD_OCTETS_RECEIVED_LOW, then the 
*           following read must be from ETH_MIB_GOOD_OCTETS_RECEIVED_HIGH 
*           register. The same applies for ETH_MIB_GOOD_OCTETS_SENT_LOW and
*           ETH_MIB_GOOD_OCTETS_SENT_HIGH
*
* INPUT:
*		ETH_PORT    ethPortNum   Ethernet Port number. See ETH_PORT enum. 
*       unsigned int mibOffset   MIB counter offset (use ETH_MIB_... macros).
*
* OUTPUT:
*       After reading the MIB counter, the counter resets.
*
* RETURN:
*       MIB counter value.
*
*******************************************************************************/
unsigned int ethReadMibCounter (ETH_PORT ethPortNum , unsigned int mibOffset)
{
    return (ETH_MIB_COUNTERS_BASE(ethPortNum) + mibOffset);
}

/*******************************************************************************
* ethernetPhySet - Set the ethernet port PHY address.
*
* DESCRIPTION:
*       This routine set the ethernet port PHY address according to given 
*       parameter.
*
* INPUT:
*		ETH_PORT   ethPortNum   Ethernet Port number. See ETH_PORT enum. 
*
* OUTPUT:
*       Set PHY Address Register with given PHY address parameter.
*
* RETURN:
*       None.
*
*******************************************************************************/
static void ethernetPhySet(ETH_PORT ethPortNum, int phyAddr)
{
    unsigned int	regData;

    GT_REG_READ(ETH_PHY_ADDR_REG, &regData);
    
	regData &= ~(0x1F << (5 * ethPortNum));
	regData |= 	(phyAddr << (5 * ethPortNum));
	
	GT_REG_WRITE(ETH_PHY_ADDR_REG, regData);

    return;
}

/*******************************************************************************
* ethernetPhyGet - Get the ethernet port PHY address.
*
* DESCRIPTION:
*       This routine returns the given ethernet port PHY address.
*
* INPUT:
*		ETH_PORT   ethPortNum   Ethernet Port number. See ETH_PORT enum. 
*
* OUTPUT:
*       None.
*
* RETURN:
*       PHY address.
*
*******************************************************************************/
static int ethernetPhyGet(ETH_PORT ethPortNum)
{
    unsigned int	regData;

    GT_REG_READ(ETH_PHY_ADDR_REG, &regData);
	
	return ((regData >> (5 * ethPortNum)) & 0x1f);
}

/*******************************************************************************
* ethernetPhyReset - Reset Ethernet port PHY.
*
* DESCRIPTION:
*       This routine utilize the SMI interface to reset the ethernet port PHY.
*       The routine waits until the link is up again or link up is timeout.
*
* INPUT:
*		ETH_PORT   ethPortNum   Ethernet Port number. See ETH_PORT enum. 
*
* OUTPUT:
*       The ethernet port PHY renew its link.
*
* RETURN:
*       None.
*
*******************************************************************************/
bool ethernetPhyReset(ETH_PORT ethPortNum)
{
	unsigned int	timeOut = 50;
    unsigned int	phyRegData;
	
    /* Reset the PHY */
    ethPortReadSmiReg (ethPortNum, 0, &phyRegData);
	phyRegData |= 0x8000; /* Set bit 15 to reset the PHY */
	ethPortWriteSmiReg (ethPortNum, 0, phyRegData);
	
	/* Poll on the PHY LINK */
	do
	{
		ethPortReadSmiReg(ethPortNum, 1, &phyRegData);
		
		if(timeOut-- == 0)
			return false;
	}
	while(!(phyRegData & 0x20));

	return true;
    }

/*******************************************************************************
* ethPortReset - Reset Ethernet port
*
* DESCRIPTION:
* 		This routine resets the chip by aborting any SDMA engine activity and
*       clearing the MIB counters. The Receiver and the Transmit unit are in 
*       idle state after this command is performed and the port is disabled.
*
* INPUT:
*		ETH_PORT   ethPortNum   Ethernet Port number. See ETH_PORT enum. 
*
* OUTPUT:
*       Channel activity is halted.
*
* RETURN:
*       None.
*
*******************************************************************************/
void ethPortReset(ETH_PORT 	ethPortNum)
{
    unsigned int  	regData;
	

	/* Stop Tx port activity. Check port Tx activity. */
	GT_REG_READ(ETH_TRANSMIT_QUEUE_COMMAND_REG(ethPortNum), &regData);

    if(regData & 0xFF)
	{
		/* Issue stop command for active channels only */
		GT_REG_WRITE(ETH_TRANSMIT_QUEUE_COMMAND_REG(ethPortNum),(regData << 8));              

		/* Wait for all Tx activity to terminate. */
		do
		{
			/* Check port cause register that all Tx queues are stopped */
			GT_REG_READ(ETH_TRANSMIT_QUEUE_COMMAND_REG(ethPortNum), &regData);
		}
		while(regData & 0xFF);
	}
	
	/* Stop Rx port activity. Check port Rx activity. */
	GT_REG_READ(ETH_RECEIVE_QUEUE_COMMAND_REG(ethPortNum), &regData);

    if(regData & 0xFF)
	{
		/* Issue stop command for active channels only */
		GT_REG_WRITE(ETH_RECEIVE_QUEUE_COMMAND_REG(ethPortNum), (regData << 8));              

		/* Wait for all Rx activity to terminate. */
		do
		{
			/* Check port cause register that all Rx queues are stopped */
			GT_REG_READ(ETH_RECEIVE_QUEUE_COMMAND_REG(ethPortNum), &regData);
		}
		while(regData & 0xFF);
	}
	

	/* Clear all MIB counters */
    ethClearMibCounters(ethPortNum);

	/* Reset the Enable bit in the Configuration Register */
    GT_REG_READ(ETH_PORT_SERIAL_CONTROL_REG(ethPortNum), &regData);
	regData &= ~ETH_SERIAL_PORT_ENABLE;
    GT_REG_WRITE(ETH_PORT_SERIAL_CONTROL_REG(ethPortNum), regData);
	
	return;
    }

/*******************************************************************************
* ethernetSetConfigReg - Set specified bits in configuration register.
*
* DESCRIPTION:
*       This function sets specified bits in the given ethernet 
*       configuration register. 
*
* INPUT:
*		ETH_PORT   ethPortNum   Ethernet Port number. See ETH_PORT enum. 
*       unsigned int    value   32 bit value.
*
* OUTPUT:
*       The set bits in the value parameter are set in the configuration 
*       register.
*
* RETURN:
*       None.
*
*******************************************************************************/
void ethernetSetConfigReg(ETH_PORT ethPortNum, unsigned int value)
{
    unsigned int ethConfigReg;

	GT_REG_READ(ETH_PORT_CONFIG_REG(ethPortNum), &ethConfigReg);
	ethConfigReg |= value;
	GT_REG_WRITE(ETH_PORT_CONFIG_REG(ethPortNum), ethConfigReg);
	
	return;
}

/*******************************************************************************
* ethernetResetConfigReg - Reset specified bits in configuration register.
*
* DESCRIPTION:
*       This function resets specified bits in the given Ethernet 
*       configuration register. 
*
* INPUT:
*		ETH_PORT   ethPortNum   Ethernet Port number. See ETH_PORT enum. 
*       unsigned int    value   32 bit value.
*
* OUTPUT:
*       The set bits in the value parameter are reset in the configuration 
*       register.
*
* RETURN:
*       None.
*
*******************************************************************************/
void ethernetResetConfigReg(ETH_PORT ethPortNum, unsigned int value)
{
    unsigned int ethConfigReg;

	GT_REG_READ(ETH_PORT_CONFIG_EXTEND_REG(ethPortNum), &ethConfigReg);
	ethConfigReg &= ~value;
	GT_REG_WRITE(ETH_PORT_CONFIG_EXTEND_REG(ethPortNum), ethConfigReg);
	
	return;
}

/*******************************************************************************
* ethernetGetConfigReg - Get the port configuration register
*
* DESCRIPTION:
*       This function returns the configuration register value of the given 
*       ethernet port.
*
* INPUT:
*		ETH_PORT   ethPortNum   Ethernet Port number. See ETH_PORT enum. 
*
* OUTPUT:
*       None.
*
* RETURN:
*       Port configuration register value.
*
*******************************************************************************/
unsigned int ethernetGetConfigReg(ETH_PORT ethPortNum)
{
    unsigned int ethConfigReg;

	GT_REG_READ(ETH_PORT_CONFIG_EXTEND_REG(ethPortNum), &ethConfigReg);
	return ethConfigReg;
}


/*******************************************************************************
* ethPortReadSmiReg - Read PHY registers
*
* DESCRIPTION:
*       This routine utilize the SMI interface to interact with the PHY in 
*       order to perform PHY register read.
*
* INPUT:
*		ETH_PORT   ethPortNum   Ethernet Port number. See ETH_PORT enum. 
*       unsigned int   phyReg   PHY register address offset.
*       unsigned int   *value   Register value buffer.
*
* OUTPUT:
*       Write the value of a specified PHY register into given buffer.
*
* RETURN:
*       false if the PHY is busy or read data is not in valid state.
*       true otherwise.
*
*******************************************************************************/
bool ethPortReadSmiReg(ETH_PORT ethPortNum, unsigned int phyReg,
                   unsigned int* value)
{
    unsigned int regValue;
    unsigned int timeOut = PHY_BUSY_TIMEOUT;
    int phyAddr;
    
    phyAddr = ethernetPhyGet(ethPortNum);

	/* first check that it is not busy */
    do
	{
		GT_REG_READ(ETH_SMI_REG, &regValue);
		if(timeOut-- == 0)
		{
			return false;
		}
	}
    while(regValue & ETH_SMI_BUSY);
	
/* not busy */

    GT_REG_WRITE (ETH_SMI_REG, 
				  (phyAddr << 16) | (phyReg << 21) | ETH_SMI_OPCODE_READ);

    timeOut = PHY_BUSY_TIMEOUT; /* initialize the time out var again */
	
    do
	{
		GT_REG_READ(ETH_SMI_REG, &regValue);
		if(timeOut-- == 0)
		{
			return false;
		}
	}
    while(regValue & ETH_SMI_READ_VALID);
	
	/* Wait for the data to update in the SMI register */
    for(timeOut = 0 ; timeOut < PHY_BUSY_TIMEOUT ; timeOut++);

	GT_REG_READ(ETH_SMI_REG, &regValue);
	
	*value = regValue & 0xffff;
    
	return true;
}

/*******************************************************************************
* ethPortWriteSmiReg - Write to PHY registers
*
* DESCRIPTION:
*       This routine utilize the SMI interface to interact with the PHY in 
*       order to perform writes to PHY registers.
*
* INPUT:
*		ETH_PORT   ethPortNum   Ethernet Port number. See ETH_PORT enum. 
*       unsigned int   phyReg   PHY register address offset.
*       unsigned int    value   Register value.
*
* OUTPUT:
*       Write the given value to the specified PHY register.
*
* RETURN:
*       false if the PHY is busy.
*       true otherwise.
*
*******************************************************************************/
bool ethPortWriteSmiReg(ETH_PORT ethPortNum, unsigned int phyReg,
						unsigned int value)
{
    unsigned int regValue; 
    unsigned int timeOut = PHY_BUSY_TIMEOUT;
    int phyAddr;

    phyAddr = ethernetPhyGet(ethPortNum);

	/* first check that it is not busy */
    do
	{
		GT_REG_READ(ETH_SMI_REG, &regValue);
		if(timeOut-- == 0)
		{
			return false;
		}
	}
    while(regValue & ETH_SMI_BUSY);

	/* not busy */
    GT_REG_WRITE(ETH_SMI_REG, (phyAddr << 16) | (phyReg << 21) |
                 ETH_SMI_OPCODE_WRITE | (value & 0xffff));
    return true;
}

/*******************************************************************************
* ethSetAccessControl - Config address decode parameters for Ethernet unit
*                                  
* DESCRIPTION:                     
*       This function configures the address decode parameters for the Gigabit 
*       Ethernet Controller according the given parameters struct. 
*                                  
* INPUT:                           
*		ETH_PORT   ethPortNum   Ethernet Port number. See ETH_PORT enum. 
*       ETH_WIN_PARAM  *param   Address decode parameter struct.
*                                               
* OUTPUT:                                       
*       An access window is opened using the given access parameters.
*                                               
* RETURN:                                                                 
*       None.                          
*                                      
*******************************************************************************/
void ethSetAccessControl(ETH_PORT ethPortNum, ETH_WIN_PARAM *param)
{
	unsigned int accessProtReg;
	    
	if(param->size == 0)
	{
		return;
	}
	
	/* Set access control register */
	GT_REG_READ(ETH_ACCESS_PROTECTION_REG(ethPortNum), &accessProtReg);
	accessProtReg &= (~(3 << (param->win * 2))); /* clear window permission */
    	accessProtReg |= (param->accessCtrl << (param->win * 2));
	GT_REG_WRITE(ETH_ACCESS_PROTECTION_REG(ethPortNum), accessProtReg);

	
	/* Set window Size reg (SR) */ 
    GT_REG_WRITE((ETH_SIZE_REG_0 + (ETH_SIZE_REG_GAP * param->win)),
                  (((param->size / 0x10000) -1) << 16));
    
    /* Set window Base address reg (BA) */
    GT_REG_WRITE((ETH_BAR_0 + (ETH_BAR_GAP * param->win)),
                 (param->target	| param->attributes	| param->baseAddr));
    
	/* High address remap reg (HARR) */
    GT_REG_WRITE((ETH_HIGH_ADDR_REMAP_REG_0 +
                  (ETH_HIGH_ADDR_REMAP_REG_GAP * param->win)), param->highAddr);
    
    /* Base address enable reg (BARER) */
    if (param->enable == 1)
        GT_RESET_REG_BITS(ETH_BASE_ADDR_ENABLE_REG, (1 << param->win));
    else
        GT_SET_REG_BITS(ETH_BASE_ADDR_ENABLE_REG, (1 << param->win));
}

/*******************************************************************************
* etherInitRxDescRing - Curve a Rx chain desc list and buffer in memory.
*
* DESCRIPTION:
*       This function prepares a Rx chained list of descriptors and packet 
*       buffers in a form of a ring. The routine must be called after port 
*       initialization routine and before port start routine. 
*       The Ethernet SDMA engine uses CPU bus addresses to access the various 
*       devices in the system (i.e. DRAM). This function uses the ethernet 
*       struct 'virtual to physical' routine (set by the user) to set the ring 
*       with physical addresses.
*
* INPUT:
*		ETH_PORT_INFO   *pEthPortCtrl   Ethernet Port Control srtuct. 
*	    ETH_QUEUE   	rxQueue         Number of Rx queue.
*       int 			rxDescNum       Number of Rx descriptors
*       int 			rxBuffSize      Size of Rx buffer
*       unsigned int    rxDescBaseAddr  Rx descriptors memory area base addr.
*       unsigned int    rxBuffBaseAddr  Rx buffer memory area base addr.
*
* OUTPUT:
*       The routine updates the Ethernet port control struct with information 
*       regarding the Rx descriptors and buffers.
*
* RETURN:
*       false if the given descriptors memory area is not aligned according to
*       Ethernet SDMA specifications.
*       true otherwise.
*
*******************************************************************************/
bool etherInitRxDescRing(ETH_PORT_INFO 	*pEthPortCtrl, 
                         ETH_QUEUE 		rxQueue,
                         int 			rxDescNum,
						 int 			rxBuffSize,
						 unsigned int 	rxDescBaseAddr,
						 unsigned int 	rxBuffBaseAddr)
{
	ETH_RX_DESC	 *pRxDesc;
	ETH_RX_DESC  *pRxPrevDesc; /* pointer to link with the last descriptor */
	unsigned int bufferAddr; 
    int			 ix;				/* a counter */


    pRxDesc		 = (ETH_RX_DESC*)rxDescBaseAddr;
	pRxPrevDesc	 = pRxDesc;
	bufferAddr   = rxBuffBaseAddr;

    /* Rx desc Must be 4LW aligned (i.e. Descriptor_Address[3:0]=0000). */
    if (rxBuffBaseAddr & 0xF) 
		return false; 
	
    /* Rx buffers are limited to 64K bytes and Minimum size is 8 bytes  */
    if ((rxBuffSize < 8) || (rxBuffSize > RX_BUFFER_MAX_SIZE)) 
		return false; 
	
    /* Rx buffers must be 64-bit aligned.       */
    if ((rxBuffBaseAddr + rxBuffSize) & 0x7)
		return false; 

	/* initialize the Rx descriptors ring */
    for (ix = 0; ix < rxDescNum; ix++)
	{        
		pRxDesc->bufSize     = rxBuffSize;
        pRxDesc->byteCnt     = 0x0000;
        pRxDesc->cmdSts      = ETH_BUFFER_OWNED_BY_DMA |ETH_RX_ENABLE_INTERRUPT;
        pRxDesc->nextDescPtr = 
						  pEthPortCtrl->portVirtToPhys((unsigned int)pRxDesc) + 
						  RX_DESC_ALIGNED_SIZE;
        pRxDesc->bufPtr      = bufferAddr;
        pRxDesc->returnInfo  = 0x00000000;
		/*D_CACHE_FLUSH_LINE (pRxDesc, 0);*/
		bufferAddr += rxBuffSize;
        pRxPrevDesc = pRxDesc;
		pRxDesc = (ETH_RX_DESC*)((unsigned int)pRxDesc + RX_DESC_ALIGNED_SIZE);
    }
     
	/* Closing Tx descriptors ring */
    pRxPrevDesc->nextDescPtr = pEthPortCtrl->portVirtToPhys(rxDescBaseAddr);
	/*D_CACHE_FLUSH_LINE (pRxPrevDesc, 0);*/

	/* Save Rx desc pointer to driver struct. */  
	CURR_RFD_SET ((ETH_RX_DESC*)rxDescBaseAddr, rxQueue);
	USED_RFD_SET ((ETH_RX_DESC*)rxDescBaseAddr, rxQueue);
	
	pEthPortCtrl->pRxDescAreaBase[rxQueue] = (ETH_RX_DESC*)rxDescBaseAddr;
	pEthPortCtrl->rxDescAreaSize[rxQueue]  = rxDescNum * RX_DESC_ALIGNED_SIZE;

    pEthPortCtrl->portRxQueueCommand |= (1 << rxQueue);
    
	return true;
}

/*******************************************************************************
* etherInitTxDescRing - Curve a Tx chain desc list and buffer in memory.
*
* DESCRIPTION:
*       This function prepares a Tx chained list of descriptors and packet 
*       buffers in a form of a ring. The routine must be called after port 
*       initialization routine and before port start routine. 
*       The Ethernet SDMA engine uses CPU bus addresses to access the various 
*       devices in the system (i.e. DRAM). This function uses the ethernet 
*       struct 'virtual to physical' routine (set by the user) to set the ring 
*       with physical addresses.
*
* INPUT:
*		ETH_PORT_INFO   *pEthPortCtrl   Ethernet Port Control srtuct. 
*	    ETH_QUEUE   	txQueue         Number of Tx queue.
*       int 			txDescNum       Number of Tx descriptors
*       int 			txBuffSize      Size of Tx buffer
*       unsigned int    txDescBaseAddr  Tx descriptors memory area base addr.
*       unsigned int    txBuffBaseAddr  Tx buffer memory area base addr.
*
* OUTPUT:
*       The routine updates the Ethernet port control struct with information 
*       regarding the Tx descriptors and buffers.
*
* RETURN:
*       false if the given descriptors memory area is not aligned according to
*       Ethernet SDMA specifications.
*       true otherwise.
*
*******************************************************************************/
bool etherInitTxDescRing(ETH_PORT_INFO 	*pEthPortCtrl,
                         ETH_QUEUE 		txQueue,
                         int 			txDescNum,
						 int 			txBuffSize,
						 unsigned int 	txDescBaseAddr,
						 unsigned int 	txBuffBaseAddr)
{

	ETH_TX_DESC	 *pTxDesc;
	ETH_TX_DESC  *pTxPrevDesc;
	unsigned int bufferAddr; 
    int			 ix;				/* a counter */


    /* save the first desc pointer to link with the last descriptor */
    	pTxDesc		 = (ETH_TX_DESC*)txDescBaseAddr;
	pTxPrevDesc	 = pTxDesc;
	bufferAddr   = txBuffBaseAddr;

    /* Tx desc Must be 4LW aligned (i.e. Descriptor_Address[3:0]=0000). */
    if (txBuffBaseAddr & 0xF) 
        return false;    
    
    /* Tx buffers are limited to 64K bytes and Minimum size is 8 bytes  */
    if (txBuffSize > TX_BUFFER_MAX_SIZE)
        return false;    

	/* Initialize the Tx descriptors ring */
    for (ix = 0; ix < txDescNum; ix++)
	{
		pTxDesc->byteCnt     = 0x0000;
	        pTxDesc->L4iChk      = 0x0000;
 	       pTxDesc->cmdSts      = 0x00000000;
  	      pTxDesc->nextDescPtr = 
              pEthPortCtrl->portVirtToPhys((unsigned int)pTxDesc) + 
                          TX_DESC_ALIGNED_SIZE;  

        	pTxDesc->bufPtr      = bufferAddr;
        	pTxDesc->returnInfo  = 0x00000000;
		/*D_CACHE_FLUSH_LINE (pTxDesc, 0);*/
		bufferAddr += txBuffSize;
		pTxPrevDesc = pTxDesc;
		pTxDesc = (ETH_TX_DESC*)((unsigned int)pTxDesc + TX_DESC_ALIGNED_SIZE);

    }    
	/* Closing Tx descriptors ring */
    pTxPrevDesc->nextDescPtr = pEthPortCtrl->portVirtToPhys(txDescBaseAddr);
	/*D_CACHE_FLUSH_LINE (pTxPrevDesc, 0);*/
	/* Set Tx desc pointer in driver struct. */
	CURR_TFD_SET ((ETH_TX_DESC*)txDescBaseAddr, txQueue);
	USED_TFD_SET ((ETH_TX_DESC*)txDescBaseAddr, txQueue);
    
    /* Init Tx ring base and size parameters */
    pEthPortCtrl->pTxDescAreaBase[txQueue] = (ETH_TX_DESC*)txDescBaseAddr;
	pEthPortCtrl->txDescAreaSize[txQueue]  = (txDescNum * TX_DESC_ALIGNED_SIZE);
	  
    /* Add the queue to the list of Tx queues of this port */
    pEthPortCtrl->portTxQueueCommand |= (1 << txQueue);

    return true;
    }

/*******************************************************************************
* ethPortSend - Send an Ethernet packet
*
* DESCRIPTION:
*		This routine send a given packet described by pPktinfo parameter. It 
*       supports transmitting of a packet spaned over multiple buffers. The 
*       routine updates 'curr' and 'first' indexes according to the packet 
*       segment passed to the routine. In case the packet segment is first, 
*       the 'first' index is update. In any case, the 'curr' index is updated. 
*       If the routine get into Tx resource error it assigns 'curr' index as 
*       'first'. This way the function can abort Tx process of multiple 
*       descriptors per packet.
*
* INPUT:
*		ETH_PORT_INFO   *pEthPortCtrl   Ethernet Port Control srtuct. 
*	    ETH_QUEUE   	txQueue         Number of Tx queue.
*	    PKT_INFO        *pPktInfo       User packet buffer.
*
* OUTPUT:
*		Tx ring 'curr' and 'first' indexes are updated. 
*
* RETURN:
*       ETH_QUEUE_FULL in case of Tx resource error.
*		ETH_ERROR in case the routine can not access Tx desc ring.
*		ETH_QUEUE_LAST_RESOURCE if the routine uses the last Tx resource.
*       ETH_OK otherwise.
*
*******************************************************************************/
ETH_FUNC_RET_STATUS ethPortSend(ETH_PORT_INFO 	*pEthPortCtrl, 
                                ETH_QUEUE 		txQueue, 
                                PKT_INFO 		*pPktInfo)
{                                                                                       
    volatile ETH_TX_DESC *pTxDescFirst;
    volatile ETH_TX_DESC *pTxDescCurr;
    volatile ETH_TX_DESC *pTxNextDescCurr;
    volatile ETH_TX_DESC *pTxDescUsed;
	unsigned int commandStatus;


    /* Do not process Tx ring in case of Tx ring resource error */
	if(pEthPortCtrl->txResourceErr[txQueue] == true)
		return ETH_QUEUE_FULL;

	/* Get the Tx Desc ring indexes */
    CURR_TFD_GET (pTxDescCurr, txQueue);
    USED_TFD_GET (pTxDescUsed, txQueue);

	if(pTxDescCurr == NULL)
        return ETH_ERROR;

	/* The following parameters are used to save readings from memory */
	pTxNextDescCurr = TX_NEXT_DESC_PTR(pTxDescCurr, txQueue);
    commandStatus   = pPktInfo->cmdSts | ETH_ZERO_PADDING | ETH_GEN_CRC;

    if (commandStatus & (ETH_TX_FIRST_DESC))
	{
		/* Update first desc */
		FIRST_TFD_SET(pTxDescCurr, txQueue);
        pTxDescFirst = pTxDescCurr;
	}
    else
	{
        FIRST_TFD_GET(pTxDescFirst, txQueue);
        commandStatus |= ETH_BUFFER_OWNED_BY_DMA;
	}

    /* Buffers with a payload smaller than 8 bytes must be aligned to 64-bit */
    /* boundary. We use the memory allocated for Tx descriptor. This memory  */
    /* located in TX_BUF_OFFSET_IN_DESC offset within the Tx descriptor.	 */
	if(pPktInfo->byteCnt <= 8)
	{
		pTxDescCurr->bufPtr = (unsigned int)pTxDescCurr + TX_BUF_OFFSET_IN_DESC;
		ethBCopy(pPktInfo->bufPtr, pTxDescCurr->bufPtr, pPktInfo->byteCnt);
	}
	else
		pTxDescCurr->bufPtr = pPktInfo->bufPtr;
	
	pTxDescCurr->byteCnt    = pPktInfo->byteCnt;
    pTxDescCurr->returnInfo = pPktInfo->returnInfo;

    if (pPktInfo->cmdSts & (ETH_TX_LAST_DESC))
    {        
        /* Set last desc with DMA ownership and interrupt enable. */
        pTxDescCurr->cmdSts = commandStatus            | 
                              ETH_BUFFER_OWNED_BY_DMA  | 
                              ETH_TX_ENABLE_INTERRUPT;

		if(pTxDescCurr != pTxDescFirst)
			pTxDescFirst->cmdSts |= ETH_BUFFER_OWNED_BY_DMA;

		/* Flush CPU pipe */

		/*D_CACHE_FLUSH_LINE ((unsigned int)pTxDescCurr,  0);
		D_CACHE_FLUSH_LINE ((unsigned int)pTxDescFirst, 0);*/
		CPU_PIPE_FLUSH;

	    /* Apply send command */
	    ETH_ENABLE_TX_QUEUE(txQueue, pEthPortCtrl->portNum);
		
		/* Finish Tx packet. Update first desc in case of Tx resource error */
		pTxDescFirst = pTxNextDescCurr;
		FIRST_TFD_SET(pTxDescFirst, txQueue);
		
    }
	else
    {
		pTxDescCurr->cmdSts	= commandStatus;
		/*D_CACHE_FLUSH_LINE ((unsigned int)pTxDescCurr,  0);*/
	}
		
	/* Check for ring index overlap in the Tx desc ring */
    if(pTxNextDescCurr == pTxDescUsed)
	{
		/* Update the current descriptor */
		CURR_TFD_SET(pTxDescFirst, txQueue);
		
		pEthPortCtrl->txResourceErr[txQueue] = true;
		return ETH_QUEUE_LAST_RESOURCE; 
	}
	else
	{
		/* Update the current descriptor */
		CURR_TFD_SET(pTxNextDescCurr, txQueue);
		
		return ETH_OK;
	}
}

/*******************************************************************************
* ethTxReturnDesc - Free all used Tx descriptors and mBlks.
*
* DESCRIPTION:
*		This routine returns the transmitted packet information to the caller.
*       It uses the 'first' index to support Tx desc return in case a transmit 
*       of a packet spanned over multiple buffer still in process.
*       In case the Tx queue was in "resource error" condition, where there are 
*       no available Tx resources, the function resets the resource error flag.
*
* INPUT:
*		ETH_PORT_INFO   *pEthPortCtrl   Ethernet Port Control srtuct. 
*	    ETH_QUEUE   	txQueue         Number of Tx queue.
*	    PKT_INFO        *pPktInfo       User packet buffer.
*
* OUTPUT:
*		Tx ring 'first' and 'used' indexes are updated. 
*
* RETURN:
*		ETH_ERROR in case the routine can not access Tx desc ring.
*       ETH_RETRY in case there is transmission in process.
*		ETH_END_OF_JOB if the routine has nothing to release.
*       ETH_OK otherwise.
*
*******************************************************************************/
ETH_FUNC_RET_STATUS ethTxReturnDesc(ETH_PORT_INFO   *pEthPortCtrl, 
                                    ETH_QUEUE   	txQueue, 
                                    PKT_INFO        *pPktInfo)
{
    volatile ETH_TX_DESC*	pTxDescUsed = NULL;
    volatile ETH_TX_DESC*   pTxDescFirst = NULL;
	unsigned int commandStatus;

        	
	/* Get the Tx Desc ring indexes */
	USED_TFD_GET (pTxDescUsed, txQueue);
	FIRST_TFD_GET(pTxDescFirst, txQueue);


    /* Sanity check */
    if(pTxDescUsed == NULL)
		return ETH_ERROR;
	
	commandStatus = pTxDescUsed->cmdSts;

	/* Still transmitting... */
	if (commandStatus & (ETH_BUFFER_OWNED_BY_DMA)) 
	{
		/*D_CACHE_FLUSH_LINE ((unsigned int)pTxDescUsed, 0);*/
		return ETH_RETRY;
	}                            

    /* Stop release. About to overlap the current available Tx descriptor */		
    if((pTxDescUsed == pTxDescFirst) && 
								(pEthPortCtrl->txResourceErr[txQueue] == false))
	{
		/*D_CACHE_FLUSH_LINE ((unsigned int)pTxDescUsed, 0);*/
		return ETH_END_OF_JOB;
	}
		
	/* Pass the packet information to the caller */
	pPktInfo->cmdSts      = commandStatus;
	pPktInfo->returnInfo  = pTxDescUsed->returnInfo;
	
	/* Update the next descriptor to release. */
	USED_TFD_SET(TX_NEXT_DESC_PTR(pTxDescUsed, txQueue), txQueue);
    
	/* Any Tx return cancels the Tx resource error status */
	if(pEthPortCtrl->txResourceErr[txQueue] == true)
		pEthPortCtrl->txResourceErr[txQueue] = false;

	/*D_CACHE_FLUSH_LINE((unsigned int)pTxDescUsed, 0);*/
	
	return ETH_OK;

}

/*******************************************************************************
* ethPortReceive - Get received information from Rx ring.
*
* DESCRIPTION:
* 		This routine returns the received data to the caller. There is no 
*		data copying during routine operation. All information is returned 
*		using pointer to packet information struct passed from the caller. 
*       If the routine exhausts	Rx ring resources then the resource error flag 
*       is set.  
*
* INPUT:
*		ETH_PORT_INFO   *pEthPortCtrl   Ethernet Port Control srtuct. 
*	    ETH_QUEUE   	rxQueue         Number of Rx queue.
*	    PKT_INFO        *pPktInfo       User packet buffer.
*
* OUTPUT:
*		Rx ring current and used indexes are updated. 
*
* RETURN:
*		ETH_ERROR in case the routine can not access Rx desc ring.
*		ETH_QUEUE_FULL if Rx ring resources are exhausted.
*		ETH_END_OF_JOB if there is no received data.
*       ETH_OK otherwise.
*
*******************************************************************************/
ETH_FUNC_RET_STATUS ethPortReceive(ETH_PORT_INFO    *pEthPortCtrl, 
                                   ETH_QUEUE		rxQueue, 
                                   PKT_INFO         *pPktInfo)
{
    volatile ETH_RX_DESC *pRxCurrDesc;
    volatile ETH_RX_DESC *pRxNextCurrDesc;
    volatile ETH_RX_DESC *pRxUsedDesc;
	unsigned int commandStatus;

    /* Do not process Rx ring in case of Rx ring resource error */
	if(pEthPortCtrl->rxResourceErr[rxQueue] == true)
		return ETH_QUEUE_FULL;

	/* Get the Rx Desc ring 'curr and 'used' indexes */
	CURR_RFD_GET (pRxCurrDesc, rxQueue);	
	USED_RFD_GET (pRxUsedDesc, rxQueue);
	
    /* Sanity check */
	if (pRxCurrDesc == NULL)
		return ETH_ERROR;

	/* The following parameters are used to save readings from memory */

	pRxNextCurrDesc = RX_NEXT_DESC_PTR(pRxCurrDesc, rxQueue);
	commandStatus	= pRxCurrDesc->cmdSts;

	/* Nothing to receive... */
	if (commandStatus & (ETH_BUFFER_OWNED_BY_DMA))
	{
		/*D_CACHE_FLUSH_LINE ((unsigned int)pRxCurrDesc, 0);*/
		return ETH_END_OF_JOB;
	}

	pPktInfo->byteCnt    = (pRxCurrDesc->byteCnt) - RX_BUF_OFFSET;
	pPktInfo->cmdSts     = commandStatus;
	pPktInfo->bufPtr 	 = (pRxCurrDesc->bufPtr) + RX_BUF_OFFSET;
	pPktInfo->returnInfo  = pRxCurrDesc->returnInfo;
	pPktInfo->L4iChk 	 = pRxCurrDesc->bufSize; /* IP fragment indicator */

	/* Clean the return info field to indicate that the packet has been */
	/* moved to the upper layers 										*/
	pRxCurrDesc->returnInfo = 0; 


	/* Update 'curr' in data structure */
	CURR_RFD_SET(pRxNextCurrDesc, rxQueue);


    /* Rx cluster resource exhausted. Set the Rx ring resource error flag */
	if (pRxNextCurrDesc == pRxUsedDesc)
		pEthPortCtrl->rxResourceErr[rxQueue] = true;
	  
	/*D_CACHE_FLUSH_LINE ((unsigned int)pRxCurrDesc, 0);*/
	
	return ETH_OK;
}

/*******************************************************************************
* ethRxReturnBuff - Returns a Rx buffer back to the Rx ring.
*
* DESCRIPTION:
*		This routine returns a Rx buffer back to the Rx ring. It retrieves the 
*       next 'used' descriptor and attached the returned buffer to it.
*       In case the Rx ring was in "resource error" condition, where there are 
*       no available Rx resources, the function resets the resource error flag.
*
* INPUT:
*		ETH_PORT_INFO   *pEthPortCtrl   Ethernet Port Control srtuct. 
*	    ETH_QUEUE   	rxQueue         Number of Rx queue.
*       PKT_INFO        *pPktInfo       Information on the returned buffer.
*
* OUTPUT:
*		New available Rx resource in Rx descriptor ring.
*
* RETURN:
*		ETH_ERROR in case the routine can not access Rx desc ring.
*       ETH_OK otherwise.
*
*******************************************************************************/
ETH_FUNC_RET_STATUS ethRxReturnBuff(ETH_PORT_INFO   *pEthPortCtrl, 
                                    ETH_QUEUE   	rxQueue,
                                    PKT_INFO        *pPktInfo)
{
	volatile ETH_RX_DESC	*pUsedRxDesc;	/* Where to return Rx resource */
            
	/* Get 'used' Rx descriptor */
	USED_RFD_GET(pUsedRxDesc, rxQueue);
	
    /* Sanity check */
	if (pUsedRxDesc == NULL)
		return ETH_ERROR;
	
	pUsedRxDesc->bufPtr 	= pPktInfo->bufPtr - RX_BUF_OFFSET;
	pUsedRxDesc->returnInfo = pPktInfo->returnInfo;
	pUsedRxDesc->bufSize    = pPktInfo->byteCnt; 
	
    /* Flush the write pipe */
	CPU_PIPE_FLUSH;
	
	/* Return the descriptor to DMA ownership */
	pUsedRxDesc->cmdSts = ETH_BUFFER_OWNED_BY_DMA | ETH_RX_ENABLE_INTERRUPT;
	
	/* Flush descriptor and CPU pipe */
	/*D_CACHE_FLUSH_LINE ((unsigned int)pUsedRxDesc, 0);*/
	CPU_PIPE_FLUSH;
	
	/* Move the used descriptor pointer to the next descriptor */
	USED_RFD_SET(RX_NEXT_DESC_PTR(pUsedRxDesc, rxQueue), rxQueue);
    
	/* Any Rx return cancels the Rx resource error status */
	if(pEthPortCtrl->rxResourceErr[rxQueue] == true)
		pEthPortCtrl->rxResourceErr[rxQueue] = false;

    return ETH_OK;
}
/*******************************************************************************
* ethPortSetRxCoal - Sets coalescing interrupt mechanism on RX path
*
* DESCRIPTION:
*		This routine sets the RX coalescing interrupt mechanism parameter.
*		This parameter is a timeout counter, that counts in 64 tClk
*		chunks ; that when timeout event occurs a maskable interrupt
*		occurs.
*		The parameter is calculated using the tCLK frequency of the 
*		MV-643xx chip, the interface speed ( 10/100/1000 MBps )
*		and the required number of 64 bytes packets the RX SDMA has
*		received.
*
* INPUT:
*		ETH_PORT ethPortNum      Ethernet port number
*		unsigned int tClk        tClk of the MV-643xx chip in HZ units
*		unsigned int linkSpeed   Can get values of 10/100/1000 which is
*                                        the link speed in MBps units.
*		unsigned int numPackets  Number of packets required to be seen
*		                         on RX queue upon receiving the coalescing
*		                         interrupt.
*
* OUTPUT:
*		Interrupt coalescing mechanism value is set in MV-643xx chip.
*
* RETURN:
*		None.
*
*******************************************************************************/
void ethPortSetRxCoal (ETH_PORT ethPortNum, unsigned int tClk, 
		       unsigned int linkSpeed, unsigned int numPackets) {
	/* ~14*1000 Packets / sec on 10 MBps */
	/* ~140K packets / sec on 100 MBps */
	/* ~1400K packets / sec on 1000 MBps */
	/* 1/tClk is the time for a single clock */
	/* 64/tClk is the time for a interrupt coalescing single count down */
	/* To receive a single packet on 10MBps - 
	 * tClk / (14*1000) = (tClk*14*1000) seconds.
	 * for example, in 100MHz - the result is 7142 clks / packet
	 */

	/* Calculate number of clks to receive single packet in 10MBps link */
	unsigned int coal = tClk/(14*1000);
	/* Now calculate on our given link speed */
	coal = (coal * 10)/linkSpeed;
	/* And on how actual number of packets we want to see on the queue */
	coal = numPackets * coal;
	/* Then convert to interrupt coalescing mechanism single count down beat */
	coal = coal / 64;
	/* Set RX Coalescing mechanism */
	GT_REG_WRITE (ETH_SDMA_CONFIG_REG(ethPortNum), (coal << 8) |
		      (GTREGREAD (ETH_SDMA_CONFIG_REG(ethPortNum)) & 0xffc000ff));
}

/*******************************************************************************
* ethPortSetTxCoal - Sets coalescing interrupt mechanism on TX path
*
* DESCRIPTION:
*		This routine sets the TX coalescing interrupt mechanism parameter.
*		This parameter is a timeout counter, that counts in 64 tClk
*		chunks ; that when timeout event occurs a maskable interrupt
*		occurs.
*		The parameter is calculated using the tCLK frequency of the 
*		MV-643xx chip, the interface speed (10/100/1000 MBps)
*		and the required number of 64 bytes packets the TX SDMA has
*		transmitted.
*
* INPUT:
*		ETH_PORT ethPortNum      Ethernet port number
*		unsigned int tClk        tClk of the MV-643xx chip in HZ units
*		unsigned int linkSpeed   Can get values of 10/100/1000 which is
*                                        the link speed in MBps units.
*		unsigned int numPackets  Number of packets required to be seen
*		                         on TX queue upon receiving the coalescing
*		                         interrupt.
*
* OUTPUT:
*		Interrupt coalescing mechanism value is set in MV-643xx chip.
*
* RETURN:
*		None.
*
*******************************************************************************/
void ethPortSetTxCoal (ETH_PORT ethPortNum, unsigned int tClk, 
		       unsigned int linkSpeed, unsigned int numPackets) {
	/* Calculate number of clks to receive single packet in 10MBps link */
	unsigned int coal = tClk/(14*1000);
	/* Now calculate on our given link speed */
	coal = (coal * 10)/linkSpeed;
	/* And on how actual number of packets we want to see on the queue */
	coal = numPackets * coal;
	/* Then convert to interrupt coalescing mechanism single count down beat */
	coal = coal / 64;
	/* Set TX Coalescing mechanism */
	GT_REG_WRITE (ETH_TX_FIFO_URGENT_THRESHOLD_REG(ethPortNum), (coal << 4) |
		      ((ETH_TX_FIFO_URGENT_THRESHOLD_REG(ethPortNum)) & 0xfffe000f));
}

/*******************************************************************************
* ethBCopy - Copy bytes from source to destination
*
* DESCRIPTION:
*       This function supports the eight bytes limitation on Tx buffer size. 
*       The routine will zero eight bytes starting from the destination address
*       followed by copying bytes from the source address to the destination.
*
* INPUT:
*       unsigned int srcAddr    32 bit source address.
*       unsigned int dstAddr    32 bit destination address.
*       int        byteCount    Number of bytes to copy.
*
* OUTPUT:
*       See description.
*
* RETURN:
*       None.
*
*******************************************************************************/
void ethBCopy(unsigned int srcAddr, unsigned int dstAddr, int byteCount)
{
	/* Zero the dstAddr area */
	*(unsigned int*)dstAddr = 0x0;

	while(byteCount != 0)
	{
		*(char*)dstAddr = *(char*)srcAddr;
		dstAddr++;
		srcAddr++;
		byteCount--;
	}
}

