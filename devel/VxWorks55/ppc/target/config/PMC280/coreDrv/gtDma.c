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
* gtDma.c - DMA functions and definitions
*
* DESCRIPTION:
*     This file gives the user a complete interface to the powerful DMA engines,
*     including functions for controling the priority mechanism.
*     To fully understand the capabilities of the DMA engines please spare some
*     time to go trough the spec.
*
* DEPENDENCIES:
*       None.
*
******************************************************************************/

/* includes */
#include "gtDma.h"

#ifdef PMC280_DUAL_CPU
/*
 * VIRTUAL_TO_PHY routine has been redefined here. This is because this
 * module seems to be the *only* consumer of this routine. Take a look
 * at sysLib.c for the initialisation of frcVirtToPhyCPU1memBase.
 */
#undef VIRTUAL_TO_PHY
#define VIRTUAL_TO_PHY(address) (frcVirtToPhyCPU1memBase + address)
extern unsigned int frcVirtToPhyCPU1memBase;
#endif /* PMC280_DUAL_CPU */

/*******************************************************************************
* gtDmaCommand - Writes a command to a DMA engine.
*
* DESCRIPTION:
*       This function writes a command to a specific DMA engine.
*       The command defines the mode in which the engine will work in.
*       There are several commands and thay are all defind in gtDma.h.
*       It is possible to combine several commands using the or ( | ) operation.
*     NOTE:
*       This function will not enable the DMA transfer unless the
*       CHANNEL_ENABLE command is combined within the command parameter.
*
* INPUT:
*       engine  - One of the possible engines as defind in gtDma.h
*       command - The command to be written to the given engine number . 
*                 All possible Values are defined in gtDma.h and can be for 
*                 example (for more details please refer to the GT data sheet at
*                 the IDMA section): 
*
*                 - DMA_HOLD_SOURCE_ADDR.
*                 - DMA_HOLD_DEST_ADDR.
*                 - DMA_DTL_8BYTES
*                 - DMA_DTL_16BYTES.
*                   etc.
* OUTPUT:
*       None.
*
* RETURN:
*       true on success, false on erroneous parameter..
*
*******************************************************************************/
bool gtDmaCommand(DMA_ENGINE engine,unsigned int command)
{
    if(IS_INVALID_ENGINE(engine)) 
        return false;
	GT_REG_WRITE(DMA_CHANNEL0_CONTROL + engine*4,command);
    return true;
}

/*******************************************************************************
* gtDmaTransfer - Transfer data from sourceAddr to destAddr on one of the 4
*                 DMA channels.
* DESCRIPTION:
*      NOTES:
*            1) When using the chain mode feature,the records must be 16
*               Bytes aligned. If the records reside on the system local 
*               memory,the function will take care of that for you but you need
*               to allocate one more record for that , meaning: if you are 
*               having 3 records ,declare 4 (see the example bellow) and start 
*               using the second one.If the records reside over PCI0/1 , it is 
*               the user responsibility to make sure thay are a 16 Bytes 
*               aligned.
*            2) If using the override feature ( source or destination 
*               address over PCI ) windows 1 and 2 must be allocated for 
*               PCI0 and PCI1 respectively before using this function.
*               Window 3 must be ignored !!!!
*            3) DMA_DTL_8BYTES was intentionally defined as BIT1 from backwords
*               compatibility reasons although its defined differently in the 
*               datasheet.if using different DTL on source and destination ,use
*               the DMA_DTL_8BYTES ( if needed) as is , the function will take
*               care of the rest for you. 
* INPUT:
*       engine     - Select one of the four available DMA engines.
*       sourceAddr - The source address on which the transfer will begin.
*       destAddr   - The destination address on which the transfer will move the
*                    data.
*       numOfBytes - The total number of bytes to transfer.
*       command    - The command selects different operation mode of the DMA
*                    engine such as different DTL , source/destination address
*                    override and so ( for more details please refer to the GT's
*                    datasheet ).The command can be combined with the | operator.
*       *pNextRecordPointer - If we are using chain mode DMA transfer,
*                            then this pointer should point to the next record,
*                            otherwise it should be NULL.
*    
* OUTPUT:
*       DMA transfer.
*
* RETURN:
*       DMA_NO_SUCH_CHANNEL if channel does not exist,
*       DMA_CHANNEL_BUSY    if channel is active.
*       DMA_OK              if the transfer ended successfully.
*
* EXAMPLE:
*       Performing a chain mode DMA transfer(Copy a 1/4 mega of data using 
*       chain mode DMA) with different DTLs for source and destination:
*
*       DMA_RECORD gtDmaRecordArray[4];
*        gtDmaRecordArray[1].ByteCnt    = _64KB;
*        gtDmaRecordArray[1].DestAdd    = destAddress   + _64KB;
*        gtDmaRecordArray[1].SrcAdd     = sourceAddress + _64KB;
*        gtDmaRecordArray[1].NextRecPtr = &gtDmaRecordArray[2];
*        gtDmaRecordArray[2].ByteCnt    = _64KB;
*        gtDmaRecordArray[2].DestAdd    = destAddress   + 2*_64KB;
*        gtDmaRecordArray[2].SrcAdd     = sourceAddress + 2*_64KB;
*        gtDmaRecordArray[2].NextRecPtr = &gtDmaRecordArray[3];
*        gtDmaRecordArray[3].ByteCnt    = _64KB;
*        gtDmaRecordArray[3].DestAdd    = destAddress   + 3*_64KB;
*        gtDmaRecordArray[3].SrcAdd     = sourceAddress + 3*_64KB;
*        gtDmaRecordArray[3].NextRecPtr = NULL;
*
*      gtDmaTransfer(DMA_ENG_0,sourceAddress,destAddress,_64KB,
*               DMA_BLOCK_TRANSFER_MODE | DMA_DTL_8BYTES | DMA_DEST_DTL_16BYTES,
*               &gtDmaRecordArray[1]);
*
*******************************************************************************/
/*
 * Added parameters "tgtIf" and "srcIf" . This is necessary for using 
 * SRAM as the source/destination for DMA.
 */
DMA_STATUS gtDmaTransfer (DMA_ENGINE engine, unsigned int sourceAddr,
                          unsigned int destAddr, unsigned int numOfBytes,
                          unsigned int command, char srcIf, char tgtIf, 
                          DMA_RECORD *pNextRecordPointer)
{
    unsigned int tempData, checkBits, alignmentOffset = 0;
    unsigned int descripOwner = NO_BIT;
    DMA_RECORD   *pNext = pNextRecordPointer;
    unsigned int engOffReg = 0; /* Engine Offset Register */

    if(IS_INVALID_ENGINE(engine))
        return DMA_NO_SUCH_CHANNEL;
    if(numOfBytes > 0xffff)
    {
        command = command | DMA_16M_DESCRIPTOR_MODE;
        descripOwner = BIT31;
    }
    if(numOfBytes > (_16M - 1)) 
        return DMA_GENERAL_ERROR;       
    if(gtDmaIsChannelActive(engine)) 
        return DMA_CHANNEL_BUSY; 
    /* check if destination DTL is different */
    if((command & (BIT0 | BIT1 | BIT2)) != 0x0)
    {
        if(((command & BIT1) != 0) && ((command & (BIT0 | BIT2)) == 0x0))
            command = command & ~BIT1;   /* real definition of DMA_DTL_8BYTES */
    }
    else
    {   /* setting the destination DTL to be equal to the source DTL */
        command = command | ((command >> 6) & 0x7);
    }
    if(pNext != NULL) /* case of chain Mode */
    {
        alignmentOffset = ((unsigned int)pNext % 16);
    }
    engOffReg = engine * 4;
    checkBits = command & (DMA_REC_ADDR_IN_PCI0 | DMA_REC_ADDR_IN_PCI1);
    if(checkBits == 0)
    {
        while(pNext != NULL)
        {
            GT_WRITE_WORD(VIRTUAL_TO_PHY((unsigned int)pNext - alignmentOffset),
                          GT_WORD_SWAP(pNext->ByteCnt | descripOwner));
            GT_WRITE_WORD(((unsigned int)pNext - alignmentOffset),
                          GT_WORD_SWAP(pNext->ByteCnt | descripOwner));
            
            tempData = (unsigned int)pNext->SrcAdd;
            GT_WRITE_WORD(VIRTUAL_TO_PHY((unsigned int)pNext +4-alignmentOffset), GT_WORD_SWAP(VIRTUAL_TO_PHY(tempData)));
            GT_WRITE_WORD(((unsigned int)pNext + 4 - alignmentOffset), GT_WORD_SWAP(VIRTUAL_TO_PHY(tempData)));
            
            tempData = (unsigned int)pNext->DestAdd;
            GT_WRITE_WORD(VIRTUAL_TO_PHY((unsigned int)pNext +8-alignmentOffset), GT_WORD_SWAP(VIRTUAL_TO_PHY(tempData)));
            GT_WRITE_WORD(((unsigned int)pNext + 8 - alignmentOffset), GT_WORD_SWAP(VIRTUAL_TO_PHY(tempData)));
            tempData = (unsigned int)pNext->NextRecPtr;
            if(tempData == (unsigned int)NULL)
            {
                GT_WRITE_WORD(VIRTUAL_TO_PHY((unsigned int)pNext + 12 -
                                             alignmentOffset),NULL);
                GT_WRITE_WORD(((unsigned int)pNext + 12 - alignmentOffset),NULL);
            }
            else
            {
                GT_WRITE_WORD(VIRTUAL_TO_PHY((unsigned int)pNext + 12 - alignmentOffset), GT_WORD_SWAP(VIRTUAL_TO_PHY(tempData-alignmentOffset)));
                GT_WRITE_WORD(((unsigned int)pNext + 12 - alignmentOffset), GT_WORD_SWAP(VIRTUAL_TO_PHY(tempData-alignmentOffset)));
            }
            pNext = (DMA_RECORD *)tempData;
            if(pNext == pNextRecordPointer) 
                pNext = NULL;
        }
    }
    GT_REG_WRITE(DMA_CHANNEL0_BYTE_COUNT + engOffReg, numOfBytes);

    tempData = sourceAddr;

    if(srcIf == DMA_SRC_PCI)
    {
        GT_REG_WRITE(DMA_CHANNEL0_SOURCE_ADDR + engOffReg, tempData);
    }
    else if(srcIf == DMA_SRC_SRAM)
    {
        GT_REG_WRITE(DMA_CHANNEL0_SOURCE_ADDR + engOffReg, tempData);
    }
    else
    {
        GT_REG_WRITE(DMA_CHANNEL0_SOURCE_ADDR + engOffReg,
                     VIRTUAL_TO_PHY(tempData));
    }

    tempData = destAddr;

    if(tgtIf == DMA_TGT_PCI)
    {
        GT_REG_WRITE(DMA_CHANNEL0_DESTINATION_ADDR + engOffReg, tempData);
    }
    else if(tgtIf == DMA_TGT_SRAM)
    {
        GT_REG_WRITE(DMA_CHANNEL0_DESTINATION_ADDR + engOffReg, tempData);
    }
    else
    {
	    GT_REG_WRITE(DMA_CHANNEL0_DESTINATION_ADDR + engOffReg,
                     VIRTUAL_TO_PHY(tempData));
    }

    if(pNextRecordPointer != NULL)
    {
        tempData = (unsigned int)pNextRecordPointer - alignmentOffset;
        if(command & (DMA_REC_ADDR_IN_PCI0 | DMA_REC_ADDR_IN_PCI1))
        {
            GT_REG_WRITE(DMA_CHANNEL0_NEXT_DESCRIPTOR_POINTER + engOffReg,
                         tempData);
        }
        else
        {
            GT_REG_WRITE(DMA_CHANNEL0_NEXT_DESCRIPTOR_POINTER + engOffReg,
                         VIRTUAL_TO_PHY(tempData));
        }
        command = command | DMA_CHANNEL_ENABLE;
    }
    else
    {
        command = command | DMA_CHANNEL_ENABLE | DMA_NON_CHAIN_MODE;
    }
    /* Activate DMA engine By writting to gtDmaControlRegister */
    GT_REG_WRITE(DMA_CHANNEL0_CONTROL + engOffReg, command);
    return DMA_OK;
}


/********************************/
/* Sergey: lets make it simpler */
/********************************/

DMA_STATUS
gtDmaStart(unsigned int chan,
           unsigned int sourceAddr,
           unsigned int destAddr,
           unsigned int numOfBytes,
           unsigned int command)
{
  unsigned int tempData;
  unsigned int engOffReg = 0; /* Channel Offset Register */

  /*if(IS_INVALID_ENGINE(engine)) return(DMA_NO_SUCH_CHANNEL);*/

  /*if(numOfBytes > 0xffff)*/ command = command | DMA_16M_DESCRIPTOR_MODE;

  /*if(numOfBytes > (_16M - 1)) return(DMA_GENERAL_ERROR);*/

  if(gtDmaIsChannelActive((DMA_ENGINE)chan)) return(DMA_CHANNEL_BUSY);

  /* Sergey: set transfer mode */
  command = command | DMA_BLOCK_TRANSFER_MODE | DMA_DTL_32BYTES;

  /* setting the destination DTL to be equal to the source DTL */
  command = command | ((command >> 6) & 0x7);

  engOffReg = chan * 4;
  GT_REG_WRITE(DMA_CHANNEL0_BYTE_COUNT + engOffReg, numOfBytes);

  /* */
  tempData = sourceAddr;
  GT_REG_WRITE(DMA_CHANNEL0_SOURCE_ADDR + engOffReg, tempData);

  /* */
  tempData = destAddr;
  GT_REG_WRITE(DMA_CHANNEL0_DESTINATION_ADDR + engOffReg, tempData);

  /* set 'enable' and 'non-chain' bits */
  command = command | DMA_CHANNEL_ENABLE | DMA_NON_CHAIN_MODE;

  /* Activate DMA engine By writting to gtDmaControlRegister */
  GT_REG_WRITE(DMA_CHANNEL0_CONTROL + engOffReg, command);

  return(DMA_OK);
}





/*******************************************************************************
* gtDmaIsChannelActive - Checks if a engine is busy.
*
* DESCRIPTION:
*       This function checks whether a given DMA engine ('engine' parameter) 
*       is active or not .Usefull for polling on a DMA engine to check if its 
*       still working.
*
* INPUT:
*       engine     - One of the possible engines as defind in gtDma.h
*
* OUTPUT:
*       None.
*
* RETURN:
*       DMA_CHANNEL_IDLE    - If the engine is idle.
*       DMA_CHANNEL_BUSY    - If the engine is busy.
*       DMA_NO_SUCH_CHANNEL - If there is no such engine. 
*
*******************************************************************************/
DMA_STATUS gtDmaIsChannelActive(DMA_ENGINE engine)
{
    unsigned int    data;

    if(IS_INVALID_ENGINE(engine)) 
        return DMA_NO_SUCH_CHANNEL;

    GT_REG_READ(DMA_CHANNEL0_CONTROL + 4*engine,&data);
    if(data & DMA_ACTIVITY_STATUS)
    {
        return DMA_CHANNEL_BUSY;
    }
    return DMA_CHANNEL_IDLE;
} 

/*******************************************************************************
* gtDmaEngineDisable - Halts the dma engine activity.
*
* DESCRIPTION:
*       This function halts a DMA engine number delivered by 'engine' parameter.
*       The engine will abort even if not all the transfer is completed.
*
* INPUT:
*       engine     - One of the possible engines as defind in gtDma.h
*
* OUTPUT:
*       DMA engine aborted.
*
* RETURN:
*       True on success , false on erroneous parameter.
*
*******************************************************************************/
bool gtDmaEngineDisable(DMA_ENGINE engine)
{
    if(IS_INVALID_ENGINE(engine)) 
        return false;
    GT_SET_REG_BITS(DMA_CHANNEL0_CONTROL + 4*engine,BIT20);
    return true;
}

/*******************************************************************************
* gtDmaUpdateArbiter - Update the arbiter`s priority for all 4 engines.  
* 
* INPUT:
*       pPriorityStruct - A priority Structure with 16 fields, each field 
*                         (slice) can be assigned to one of the DMA engines.
*
* OUTPUT:
*       None.
*
* RETURN:
*       false if one of the parameters is erroneous, true otherwise
*******************************************************************************/
bool gtDmaUpdateArbiter(DMA_PIZZA *pPriorityStruct) 
{
    unsigned int i;
    unsigned int arbiterPrio = 0;
    unsigned int baseAddress=(unsigned int)(pPriorityStruct);
    unsigned int temp;

    for(i=0; i<16; i++)
    {
        temp = *(unsigned char *)(baseAddress + i);
        if(temp > 3) 
        {
            return false;
        }
        arbiterPrio = arbiterPrio << 2;
        arbiterPrio = arbiterPrio + temp;
    }
    GT_REG_WRITE(DMA_ARBITER_CONTROL,arbiterPrio);
    return true;
}

/*******************************************************************************
* gtDmaSetMemorySpace - Set Atlantis IDMA memory decoding map.
*
* DESCRIPTION:
*       the Atlantis IDMA has its own address decoding map that is de-coupled
*       from the CPU interface address decoding windows. The four DMA channels
*       share eight address windows. Each region can be individually configured
*       by this function by associating it to a target interface and setting 
*       base and size values.
*
* INPUT:
*       memSpace       - One of the possible memory spaces (defined in gtDma.h).
*       memSpaceTarget - The target interface to be associated with the 
*                        region (DRAM, PCI, devices...).
*       memSpaceAttr   - Memory space attributes.The memory space attributes 
*                        differ in each memory space target (please see the 
*                        IDMA section in the Atlantis spec for more details).
*                        values for each memory space are defined in gtDma.h.
*                        please note that you give the appropriate values to the
*                        corresponding interface . 
*       baseAddress    - Memory space's base address.
*       size           - Memory space's size. This function will decrement the 
*                        'size' parameter by one and then check if the size 
*                        is valid. A valid size must be programed from LSB to 
*                        MSB as sequence of ‘1’s followed by sequence of ‘0’s.
*                        To close a memory window simply set the size to 0. 
*
*      NOTE!!!
*       The size must be in 64Kbyte granularity.     
*       The base address must be aligned to the size.     
*
* OUTPUT:
*       None.
*
* RETURN:
*       True for success, false otherwise.
*
*******************************************************************************/
bool gtDmaSetMemorySpace(DMA_MEM_SPACE memSpace,
                         DMA_MEM_SPACE_TARGET memSpaceTarget,
                         unsigned int memSpaceAttr,
                         unsigned int baseAddress,
                         unsigned int size)
{
    unsigned int rShift = 0, temp;

    /* Closing the memory space. */
    if(size == 0)
    {
        GT_SET_REG_BITS(DMA_BASE_ADDR_ENABLE_REG, BIT0 << memSpace);
        return true;
    }
    /* The base address must be aligned to the size.  */
    if(baseAddress % size != 0)
    {
        return false ;
    }
    if(size >= MIN_DMA_WINDOW_SIZE)
    {
        size &= 0xffff0000;
        /* Checking that the size is a sequence of '1' followed by a 
           sequence of '0' starting from LSB to MSB. */
        temp = (size >> 16) - 1;
        for(rShift = 0; rShift < 16; rShift++)
        {
            temp = temp >> rShift;                
            if((temp & 0x1) == 0) /* Either we got to the last '1' */
            {                     /* or the size is not valid       */
                if(temp > 0x0)  
                    return false;
                else
                    break;
            }
        }
        baseAddress = (baseAddress & 0xffff0000);
        GT_REG_WRITE(DMA_BASE_ADDR_REG0 + memSpace*8,
                                 (baseAddress | memSpaceTarget | memSpaceAttr));
        GT_REG_WRITE((DMA_SIZE_REG0 + memSpace*8),(size - 1) & 0xffff0000);
        GT_RESET_REG_BITS(DMA_BASE_ADDR_ENABLE_REG,BIT0 << memSpace);
        return true;
    }
    return false;
}

/*******************************************************************************
* gtDmaSetMemorySpaceAttr - Set attributes for a DMA memory space.
*
* DESCRIPTION:
*
* INPUT: 
*        memSpace     - One of the possible memory spaces (defined in gtDma.h).
*        memSpaceAttr - gtMemory space attributes.The memory space attributes 
*                       differ in each memory space target (please see the 
*                       IDMA section in the Atlantis spec for more details).
*                       values for each memory space are defined in gtDma.h.
*                       please note that you give the appropriate values to the
*                       corresponding interface . 
*
* OUTPUT:
*       None.
*
* RETURN: 
*       None.
*******************************************************************************/
void gtDmaSetMemorySpaceAttr(DMA_MEM_SPACE memSpace,unsigned int memSpaceAttr)
{
    unsigned int memSpaceBit = 0x1;

    memSpaceBit = memSpaceBit << memSpace;
    /* closing the memory space temporarily */
    GT_SET_REG_BITS(DMA_BASE_ADDR_ENABLE_REG,memSpaceBit);
    /* Clear the last attributes (bits 8 - 15)*/
    GT_RESET_REG_BITS(DMA_BASE_ADDR_REG0 + memSpace*8,0x0007f100);
    /* Setting the attributes */
    GT_SET_REG_BITS(DMA_BASE_ADDR_REG0 + memSpace*8,memSpaceAttr);
    /* Enable back the window */
    GT_RESET_REG_BITS(DMA_BASE_ADDR_ENABLE_REG,memSpaceBit);
}

/*******************************************************************************
* gtDmaSetEngineAccessProtect - Configure access attributes bits for DMA engine.
*
* DESCRIPTION:
*       Each engine can be configured with access attributes for each of the
*       gtMemory spaces defined by 'gtDmaSetMemorySpace' function. This 
*       function sets access attributes to a given window for the given engine
*
* INPUTS: 
*       engine   - One of the 4 possible engines ( defined in gtDma.h ).
*       memSpace - One of the 8 possible gtMemory spaces (defined in gtDma.h).
*       access   - The access type for the region ( defined in gtDma.h )
*
* OUTPUT:
*       None.
*
* RETURN: 
*       True for success, false otherwise.
*
*******************************************************************************/
bool gtDmaSetEngineAccessProtect(DMA_ENGINE engine,DMA_MEM_SPACE memSpace,
                                 DMA_MEM_SPACE_ACCESS access)
{
    if(IS_INVALID_ENGINE(engine)) 
        return false;
    access = access << memSpace*2;
    GT_SET_REG_BITS(DMA_CHANNEL0_ACCESS_PROTECTION_REG + engine*4,access);
    return true;
}
