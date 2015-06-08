/******************************************************************************
*              (c), Copyright, MOTOROLA  INC - ECC                  *
*                                                                             *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MOTOROLA , INC.        *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT *
* OF MOTOROLA  OR ANY THIRD PARTY. MOTOROLA  RESERVES THE RIGHT AT ITS SOLE DISCRETION*
* TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MOTOROLA .       *
* THIS CODE IS PROVIDED "AS IS". MOTOROLA  MAKES NO WARRANTIES, EXPRESS,*
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.  *
*******************************************************************************

*******************************************************************************
* gtPci.c - gtPci functions 
*
* DESCRIPTION:
*       None.
*
* DEPENDENCIES:
*       None.
*
******************************************************************************/

/* includes */
#include "gtPci.h"
#include "gtMemory.h"
#include "gtCore.h"
#include <string.h>




/*******************************************************************************
* frcPci0WriteConfigReg - Write to a PCI configuration register.
*
* DESCRIPTION:
*       The GT holds two registers to support configuration accesses as defined 
*       in the PCI spec rev 2.2: Configuration Address and Configuration Data 
*       registers. The mechanism for accessing configuration space is to write a
*       value into the Configuration Address register that specifies the PCI bus
*       number (this function use the value of 0 by default for this parameter),
*       Device number on the bus, Function number within the device (will be 
*       combined with the register offset) and Configuration register offset 
*       within the device/function being accessed. A subsequent write to the PCI 
*       Configuration Data register causes the GT to translate that 
*       Configuration Address value to the requested cycle on the PCI bus (in 
*       this case - write) or internal configuration space. This function writes
*       to an agent’s configuration register at any of the 8 possible function 
*       in its Configuration Space Header.
*
*      EXAMPLE:
*       The value 0x004 is combined from the function number (bits[11:8]) and 
*       the register offset (bits[7:0]) in the Configuration Space Header. In 
*       this case, the fuction number is 0 and the register offset is 0x04. 
*        
*       ...
*       frcPci0WriteConfigReg(0x004,6,PCI_MASTER_ENABLE); 
*       ...
*       
*       The configuration address register (0xCF8) fields are:                     
*
*             31 30    24 23  16 15  11 10     8 7      2  0     <=bit Number
*       |congif|Reserved|  Bus |Device|Function|Register|00|
*       |Enable|        |Number|Number| Number | Number |  |    <=field Name
*
*
* INPUT:
*       regOffset - The register offset PCI configuration Space Header combined 
*                   with the function number as shown in the example above.
*       pciDevNum - The agent’s device number.
*       data      - data to be written.            
*
* OUTPUT:
*       PCI write configuration cycle.
*
* RETURN:
*       None.
*
*******************************************************************************/
void _frcPci0WriteConfigReg(unsigned int bus, unsigned int dev,
                          unsigned int func, unsigned int offset,
                          unsigned int data)
{                                                      
    volatile unsigned int addr;

    offset &= 0xFC;
    addr = (bus << 16)|(dev << 11)|(func << 8)|(offset) | PCI_CONFIG_ENABLE;

    GT_REG_WRITE(PCI_0_CONFIG_ADDR, addr);
    GT_REG_WRITE(PCI_0_CONFIG_DATA_VIRTUAL_REG, data);
}

void frcPci0WriteConfigReg(unsigned int regOffset,unsigned int pciDevNum,
                          unsigned int data)
{                                                      
    volatile unsigned int DataForRegCf8;
    unsigned int functionNum;
    unsigned int busNum = 0;

    if(pciDevNum > 32) /* illegal device Number */
        return;
    if(pciDevNum == PCI_SELF) /* configure our configuration space. */
    {
        pciDevNum = ((GTREGREAD(PCI_0_P2P_CONFIG)) >> 24) & 0x1f;
        busNum = (GTREGREAD(PCI_0_P2P_CONFIG)) & 0xff0000;
    }
   	functionNum = regOffset & 0x00000700;
    if((functionNum >> 8) > 0x7) /* illegal function Number */
        return;
    pciDevNum = pciDevNum << 11;
    regOffset = regOffset & 0xfc;
    DataForRegCf8 = busNum|pciDevNum|functionNum |regOffset | PCI_CONFIG_ENABLE;
    GT_REG_WRITE(PCI_0_CONFIG_ADDR,DataForRegCf8);
    GT_REG_WRITE(PCI_0_CONFIG_DATA_VIRTUAL_REG,data);
}

/*******************************************************************************
* frcPci0ReadConfigReg - Read from a PCI configuration register.
*
* DESCRIPTION:
*       The GT holds two registers to support configuration accesses as defined 
*       in the PCI spec rev 2.2: Configuration Address and Configuration Data 
*       registers. The mechanism for accessing configuration space is to write a
*       value into the Configuration Address register that specifies the PCI bus
*       number (this function use the value of 0 by default for this parameter),
*       Device number on the bus, Function number within the device (will be 
*       combined with the register offset) and Configuration register offset 
*       within the device/function being accessed. A subsequent read to the PCI 
*       Configuration Data register causes the GT to translate that 
*       Configuration Address value to the requested cycle on the PCI bus (in 
*       this case - read) or internal configuration space. This function reads
*       from an agent’s configuration register at any of the 8 possible function 
*       in its Configuration Space Header.
*
*      EXAMPLE:
*       The value 0x004 is combined from the function number (bits[11:8]) and 
*       the register offset (bits[7:0]) in the Configuration Space Header. In 
*       this case, the fuction number is 0 and the register offset is 0x04. 
*        
*       ...
*       data = frcPci0ReadConfigReg(0x004,6); 
*       ...
*       
*       The configuration address register (0xCF8) fields are:                     
*
*             31 30    24 23  16 15  11 10     8 7      2  0     <=bit Number
*       |congif|Reserved|  Bus |Device|Function|Register|00|
*       |Enable|        |Number|Number| Number | Number |  |    <=field Name
*
*
* INPUT:
*       regOffset - The register offset PCI configuration Space Header combined 
*                   with the function number as shown in the example above.
*       pciDevNum - The agent’s device number.
*
* OUTPUT:
*       PCI write configuration cycle.
*
* RETURN:
*       32 bit read data from the agent's configuration register. if the 
*       data == 0xffffffff check the master abort bit in the cause register to 
*       make sure the data is valid.
*
*******************************************************************************/
unsigned int _frcPci0ReadConfigReg (unsigned int bus, unsigned int dev, 
                                  unsigned int func, unsigned int offset)
{
    unsigned int data, temp;

    offset &= 0xFC;
    data = (bus << 16)|(dev << 11)|(func << 8)|(offset)|PCI_CONFIG_ENABLE;
    GT_REG_WRITE(PCI_0_CONFIG_ADDR, data);
    GT_REG_READ(PCI_0_CONFIG_ADDR, &temp);
    if (data != temp)
        return 0xFFFFFFFF;

    GT_REG_READ(PCI_0_CONFIG_DATA_VIRTUAL_REG, &data);
    return data;
}

unsigned int frcPci0ReadConfigReg (unsigned int regOffset,unsigned int pciDevNum)
{
    volatile unsigned int DataForRegCf8;
    unsigned int data;
    unsigned int functionNum;
    unsigned int busNum = 0;
   	
    if(pciDevNum > 32) /* illegal device Number */
        return 0xffffffff;
    if(pciDevNum == PCI_SELF) /* configure our configuration space. */
    {
        pciDevNum = ((GTREGREAD(PCI_0_P2P_CONFIG)) >> 24) & 0x1f;
        busNum = (GTREGREAD(PCI_0_P2P_CONFIG)) & 0xff0000;
    }
    functionNum = regOffset & 0x00000700;
    if((functionNum >> 8) > 0x7) /* illegal function Number */
        return 0xffffffff;
    pciDevNum = pciDevNum << 11;
    regOffset = regOffset & 0xfc;
    DataForRegCf8 = (regOffset | pciDevNum | functionNum | busNum) |           \
                                                              PCI_CONFIG_ENABLE;
    GT_REG_WRITE(PCI_0_CONFIG_ADDR,DataForRegCf8);
    GT_REG_READ(PCI_0_CONFIG_ADDR, &data);
    if (data != DataForRegCf8)
        return 0xffffffff;
    GT_REG_READ(PCI_0_CONFIG_DATA_VIRTUAL_REG, &data);
    return data;
}

/*******************************************************************************
* gtPci0OverBridgeWriteConfigReg - Write to a PCI configuration register of an 
*                                  agent beyond PCI 0 bus.
*
* DESCRIPTION:
*   The GT incorporates bridges to communicate between different PCI buses. 
*   This function writes to an agent’s configuration register, on a PCI bus 
*   other than PCI 0. If ‘busNum’ paramter is equal to the same bus of the
*   master who initiated the over bridge write configuration transaction (in
*   this case, bus 0), the master will response with master abort. The over
*   bridge configuration write transaction will skip (and remain a type 1 
*   transaction ) over buses till the desired bus is reached. The method 
*   for writing to a configuration register of an agent over a PCI bus other
*   than PCI 0 is similar to the method described in 'frcPci0WriteConfigReg' 
*   function with the only difference that the bus number parameter is 
*   added. For more details on P2P transactions please refer to the PCI spec
*   rev2.2. 
*       
*
* INPUT:
*   regOffset - The register offset PCI configuration Space Header combined 
*               with the function number as shown in the example above.
*   pciDevNum - The agent’s device number.
*   busNum    - The agent's bus number to write its configuration register. 
*   data      - Data to be written.            
*
* OUTPUT:
*       None.
*
* RETURN:
*       None.
*
*******************************************************************************/
void gtPci0OverBridgeWriteConfigReg(unsigned int regOffset,
                                    unsigned int pciDevNum, unsigned int busNum,
                                    unsigned int data)
{
    unsigned int   functionNum;

    functionNum =  regOffset & 0x00000700;
    if((functionNum >> 8) > 0x7) /* illegal function Number */
        return;
    pciDevNum = ( pciDevNum << 11 ) & 0x0000f800;
    regOffset &= 0xff;
    busNum = ( busNum << 16 ) & 0x00ff0000;
    GT_REG_WRITE(PCI_0_CONFIG_ADDR,regOffset | pciDevNum | functionNum | busNum
                                               | PCI_CONFIG_ENABLE | PCI_TYPE1);                            
    /* The PCI is working in LE Mode So it swaps the Data. */
    GT_REG_WRITE(PCI_0_CONFIG_DATA_VIRTUAL_REG,GT_WORD_SWAP(data));
}

/*******************************************************************************
* gtPci0OverBridgeReadConfigReg - Read from a PCI configuration register of an 
*                                 agent beyond PCI 0 bus.
*
* DESCRIPTION:
*   The GT incorporates bridges to communicate between different PCI buses. 
*   This function reads from an agent’s configuration register, on a PCI bus 
*   other than PCI 0. If ‘busNum’ paramter is equal to the same bus of the
*   master who initiated the over bridge read configuration transaction (in
*   this case, bus 0), the master will response with master abort. The over
*   bridge configuration read transaction will skip (and remain a type 1 
*   transaction ) over buses till the desired bus is reached. The method 
*   for reading from a configuration register of an agent over PCI bus other
*   than PCI 0 is similar to the method described in 'frcPci0ReadConfigReg' 
*   function with the only difference that the bus number parameter is 
*   added. For more details on P2P transactions please refer to the PCI spec
*   rev2.2. 
*
* INPUT:
*   regOffset - The register offset PCI configuration Space Header combined 
*               with the function number as shown in the example above.
*   pciDevNum - The agent’s device number.
*   busNum    - The agent's bus number to read its configuration register. 
*
* OUTPUT:
*       None.
*
* RETURN:
*       32 bit read data from the agent's configuration register. if the 
*       data == 0xffffffff check the master abort bit in the cause register to 
*       make sure the data is valid.
*
*******************************************************************************/
unsigned int gtPci0OverBridgeReadConfigReg(unsigned int regOffset,
                                           unsigned int pciDevNum,
                                           unsigned int busNum)
{
    unsigned int data;
    unsigned int functionNum;      
   	
    functionNum =  regOffset & 0x00000700;
    if((functionNum >> 8) > 0x7) /* illegal function Number */
        return 0xffffffff;
    pciDevNum = ( pciDevNum << 11 ) & 0x0000f800;
    regOffset = regOffset & 0xff;
    busNum = ( busNum << 16 ) & 0x00ff0000;
    GT_REG_WRITE(PCI_0_CONFIG_ADDR,regOffset | pciDevNum | functionNum | busNum
                                               | PCI_CONFIG_ENABLE | PCI_TYPE1);
    GT_REG_READ(PCI_0_CONFIG_DATA_VIRTUAL_REG, &data);
    /* The PCI is working in LE Mode So it swaps the Data. */
    return GT_WORD_SWAP(data);
}

/*******************************************************************************
* gtPci0MapIOspace - Set new base address and size for PCI 0 I/O space .
*
* DESCRIPTION:
*       This function sets new base address and size for the PCI 0 I/O memory 
*       space. The PCI master will initiates an I/O transaction over the PCI bus
*       if there is a hit on this memory space for an address comming from the 
*       CPU (see CPU address decoding section for more details).
*
* INPUT:
*       pci0IoBase - Set new base address for PCI 0 I/O memory space.
*       pci0IoSize - Set new size for PCI 0 I/O memory space.
*
* OUTPUT:
*       None.
*
* RETURN:
*       None.
*
*******************************************************************************/
void gtPci0MapIOspace(unsigned int pci0IoBase,unsigned int pci0IoSize)
{
    unsigned int currentLow;

    pci0IoBase >>= 16;
    if(pci0IoSize > 0)
    {
        pci0IoSize  = (pci0IoSize - 1) >> 16; 
        GT_REG_WRITE(PCI_0_IO_SIZE,pci0IoSize);
        GT_REG_READ(PCI_0_IO_BASE_ADDR,&currentLow);
        pci0IoBase = (pci0IoBase & 0xfffff) | (currentLow & 0xfff00000);
        GT_REG_WRITE(PCI_0_IO_BASE_ADDR,pci0IoBase);
        gtMemoryEnableWindow(MEM_PCI_0_IO);
    }
    else
    {
        gtMemoryDisableWindow(MEM_PCI_0_IO);
    }
}

/*******************************************************************************
* gtPci0MapMemory0space - Set new base address and size for PCI 0 memory0 space.
*
* DESCRIPTION:
*       This function sets new base address and size for the PCI 0 memory0 
*       space. The PCI master will initiates a memory transaction over the PCI 
*       bus if there is a hit on this memory space for an address comming from
*       the CPU (see CPU address decoding section for more details).
*
* INPUT:
*       pci0Mem0Base - Set new base address for PCI 0 memory0 space.
*       pci0Mem0Size - Set new size for PCI 0 memory0 space.
*
* OUTPUT:
*       None.
*
* RETURN:
*       None.
*
*******************************************************************************/
void gtPci0MapMemory0space(unsigned int pci0Mem0Base, unsigned int pci0Mem0Size)
{
    unsigned int currentLow;

    pci0Mem0Base >>= 16;
    if(pci0Mem0Size > 0)
    {
        pci0Mem0Size  = (pci0Mem0Size - 1) >> 16;   
        GT_REG_WRITE(PCI_0_MEMORY0_SIZE,pci0Mem0Size );
        GT_REG_READ(PCI_0_MEMORY0_BASE_ADDR,&currentLow);
        pci0Mem0Base = (pci0Mem0Base & 0xfffff) | (currentLow & 0xfff00000);
        GT_REG_WRITE(PCI_0_MEMORY0_BASE_ADDR,pci0Mem0Base);
        gtMemoryEnableWindow(MEM_PCI_0_MEM0);
    }
    else
    {
        gtMemoryDisableWindow(MEM_PCI_0_MEM0);
    }
}

/*******************************************************************************
* gtPci0MapMemory1space - Set new base address and size for PCI 0 memory1 space.
*
* DESCRIPTION:
*       This function sets new base address and size for the PCI 0 memory1 
*       space. The PCI master will initiates a memory transaction over the PCI 
*       bus if there is a hit on this memory space for an address comming from
*       the CPU (see CPU address decoding section for more details).
*
* INPUT:
*       pci0Mem1Base - Set new base address for PCI 0 memory1 space.
*       pci0Mem1Size - Set new size for PCI 0 memory1 space.
*
* OUTPUT:
*       None.
*
* RETURN:
*       None.
*
*******************************************************************************/
void gtPci0MapMemory1space(unsigned int pci0Mem1Base, unsigned int pci0Mem1Size)
{
    unsigned int currentLow;

    pci0Mem1Base >>= 16;
    if(pci0Mem1Size > 0)
    {
        pci0Mem1Size  = (pci0Mem1Size - 1) >> 16;   
        GT_REG_READ(PCI_0_MEMORY1_BASE_ADDR,&currentLow);
        pci0Mem1Base = (pci0Mem1Base & 0xfffff) | (currentLow & 0xfff00000);
        GT_REG_WRITE(PCI_0_MEMORY1_SIZE,pci0Mem1Size);
        GT_REG_WRITE(PCI_0_MEMORY1_BASE_ADDR,pci0Mem1Base);
        gtMemoryEnableWindow(MEM_PCI_0_MEM1);
    }
    else
    {
        gtMemoryDisableWindow(MEM_PCI_0_MEM1);
    }
}

/*******************************************************************************
* gtPci0MapMemory2space - Set new base address and size for PCI 0 memory2 space.
*
* DESCRIPTION:
*       This function sets new base address and size for the PCI 0 memory2 
*       space. The PCI master will initiates a memory transaction over the PCI 
*       bus if there is a hit on this memory space for an address comming from
*       the CPU (see CPU address decoding section for more details).
*
* INPUT:
*       pci0Mem2Base - Set new base address for PCI 0 memory2 space.
*       pci0Mem2Size - Set new size for PCI 0 memory2 space.
*
* OUTPUT:
*       None.
*
* RETURN:
*       None.
*
*******************************************************************************/
void gtPci0MapMemory2space(unsigned int pci0Mem2Base, unsigned int pci0Mem2Size)
{
    unsigned int currentLow;

    pci0Mem2Base >>= 16;
    if(pci0Mem2Size > 0)
    {
        pci0Mem2Size  = (pci0Mem2Size - 1)>> 16;   
        GT_REG_READ(PCI_0_MEMORY2_BASE_ADDR,&currentLow);
        pci0Mem2Base = (pci0Mem2Base & 0xfffff) | (currentLow & 0xfff00000);
        GT_REG_WRITE(PCI_0_MEMORY2_SIZE,pci0Mem2Size);
        GT_REG_WRITE(PCI_0_MEMORY2_BASE_ADDR,pci0Mem2Base);
gtMemorySetPciRemapValue(PCI_0_MEM2,0,0);

        gtMemoryEnableWindow(MEM_PCI_0_MEM2);
    }
    else
    {
        gtMemoryDisableWindow(MEM_PCI_0_MEM2);
    }
}

/*******************************************************************************
* gtPci0MapMemory3space - Set new base address and size for PCI 0 memory3 space.
*
* DESCRIPTION:
*       This function sets new base address and size for the PCI 0 memory3 
*       space. The PCI master will initiates a memory transaction over the PCI 
*       bus if there is a hit on this memory space for an address comming from
*       the CPU (see CPU address decoding section for more details).
*
* INPUT:
*       pci0Mem3Base - Set new base address for PCI 0 memory3 space.
*       pci0Mem3Size - Set new size for PCI 0 memory3 space.
*
* OUTPUT:
*       None.
*
* RETURN:
*       None.
*
*******************************************************************************/
void gtPci0MapMemory3space(unsigned int pci0Mem3Base, unsigned int pci0Mem3Size)
{
    unsigned int currentLow;

    pci0Mem3Base >>= 16;
    if(pci0Mem3Size > 0)
    {
        pci0Mem3Size  = (pci0Mem3Size - 1) >> 16;   
        GT_REG_READ(PCI_0_MEMORY3_BASE_ADDR,&currentLow);
        pci0Mem3Base = (pci0Mem3Base & 0xfffff) | (currentLow & 0xfff00000);
        GT_REG_WRITE(PCI_0_MEMORY3_SIZE,pci0Mem3Size);
        GT_REG_WRITE(PCI_0_MEMORY3_BASE_ADDR,pci0Mem3Base);
        gtMemoryEnableWindow(MEM_PCI_0_MEM3);
    }
    else
    {
        gtMemoryDisableWindow(MEM_PCI_0_MEM3);
    }
}

/*******************************************************************************
* gtPci0GetIOspaceBase - Return PCI 0 I/O memory space base address.
*
* DESCRIPTION:
*       This function returns the base address of the PCI 0 I/O memory space.
*
* INPUT:
*       None.
*
* OUTPUT:
*       None.
*
* RETURN:
*       32 bit base address of the PCI 0 I/O memory space.
*
*******************************************************************************/
unsigned int gtPci0GetIOspaceBase(void)
{
    unsigned int base;

    GT_REG_READ(PCI_0_IO_BASE_ADDR,&base);
    return (base << 16);
}

/*******************************************************************************
* gtPci0GetIOspaceSize - Return PCI 0 I/O memory space size.
*
* DESCRIPTION:
*       This function returns the size of the PCI 0 I/O memory space.
*
* INPUT:
*       None.
*
* OUTPUT:
*       None.
*
* RETURN:
*       32 bit size of the PCI 0 I/O memory space.
*
*******************************************************************************/
unsigned int gtPci0GetIOspaceSize(void)
{
    unsigned int size;

    if(gtMemoryGetMemWindowStatus(MEM_PCI_0_IO) != 0)
        return 0;
    GT_REG_READ(PCI_0_IO_SIZE,&size);
    size = ((size << 16) | 0xffff) + 1; 
    return size;
}

/*******************************************************************************
* gtPci0GetMemory0Base - Return PCI 0 memory0 space base address.
*
* DESCRIPTION:
*       This function returns the base address of the PCI 0 memory0 space.
*
* INPUT:
*       None.
*
* OUTPUT:
*       None.
*
* RETURN:
*       32 bit base address of the PCI 0 memory0 space.
*
*******************************************************************************/
unsigned int gtPci0GetMemory0Base(void)
{
    unsigned int base;

    GT_REG_READ(PCI_0_MEMORY0_BASE_ADDR,&base);
    return (base << 16);
}

/*******************************************************************************
* gtPci0GetMemory0Size - Return PCI 0 memory0 space size.
*
* DESCRIPTION:
*       This function returns the size of the PCI 0 memory0 space.
*
* INPUT:
*       None.
*
* OUTPUT:
*       None.
*
* RETURN:
*       32 bit size of the PCI 0 memory0 space.
*
*******************************************************************************/
unsigned int gtPci0GetMemory0Size(void)
{
    unsigned int size;

    if(gtMemoryGetMemWindowStatus(MEM_PCI_0_MEM0) != 0)
        return 0;
    GT_REG_READ(PCI_0_MEMORY0_SIZE,&size);
    size = ((size << 16) | 0xffff) + 1; 
    return size;
}

/*******************************************************************************
* gtPci0GetMemory1Base - Return PCI 0 memory1 space base address.
*
* DESCRIPTION:
*       This function returns the base address of the PCI 0 memory1 space.
*
* INPUT:
*       None.
*
* OUTPUT:
*       None.
*
* RETURN:
*       32 bit base address of the PCI 0 memory1 space.
*
*******************************************************************************/
unsigned int gtPci0GetMemory1Base(void)
{
    unsigned int base;

    GT_REG_READ(PCI_0_MEMORY1_BASE_ADDR,&base);
    return (base << 16);
}

/*******************************************************************************
* gtPci0GetMemory1Size - Return PCI 0 memory1 space size.
*
* DESCRIPTION:
*       This function returns the size of the PCI 0 memory1 space.
*
* INPUT:
*       None.
*
* OUTPUT:
*       None.
*
* RETURN:
*       32 bit size of the PCI 0 memory1 space.
*
*******************************************************************************/
unsigned int gtPci0GetMemory1Size(void)
{
    unsigned int size;

    if(gtMemoryGetMemWindowStatus(MEM_PCI_0_MEM1) != 0)
        return 0;
    GT_REG_READ(PCI_0_MEMORY1_SIZE,&size);
    size = ((size << 16) | 0xffff) + 1; 
    return size;
}

/*******************************************************************************
* gtPci0GetMemory2Base - Return PCI 0 memory2 space base address.
*
* DESCRIPTION:
*       This function returns the base address of the PCI 0 memory2 space.
*
* INPUT:
*       None.
*
* OUTPUT:
*       None.
*
* RETURN:
*       32 bit base address of the PCI 0 memory2 space.
*
*******************************************************************************/
unsigned int gtPci0GetMemory2Base(void)
{
    unsigned int base;

    GT_REG_READ(PCI_0_MEMORY2_BASE_ADDR,&base);
    return (base << 16);
}

/*******************************************************************************
* gtPci0GetMemory2Size - Return PCI 0 memory2 space size.
*
* DESCRIPTION:
*       This function returns the size of the PCI 0 memory2 space.
*
* INPUT:
*       None.
*
* OUTPUT:
*       None.
*
* RETURN:
*       32 bit size of the PCI 0 memory2 space.
*
*******************************************************************************/
unsigned int gtPci0GetMemory2Size(void)
{
    unsigned int size;

    if(gtMemoryGetMemWindowStatus(MEM_PCI_0_MEM2) != 0)
        return 0;
    GT_REG_READ(PCI_0_MEMORY2_SIZE,&size);
    size = ((size << 16) | 0xffff) + 1; 
    return size;
}

/*******************************************************************************
* gtPci0GetMemory3Base - Return PCI 0 memory3 space base address.
*
* DESCRIPTION:
*       This function returns the base address of the PCI 0 memory3 space.
*
* INPUT:
*       None.
*
* OUTPUT:
*       None.
*
* RETURN:
*       32 bit base address of the PCI 0 memory3 space.
*
*******************************************************************************/
unsigned int gtPci0GetMemory3Base(void)
{
    unsigned int base;

    GT_REG_READ(PCI_0_MEMORY3_BASE_ADDR,&base);
    return (base << 16);
}

/*******************************************************************************
* gtPci0GetMemory3Size - Return PCI 0 memory3 space size.
*
* DESCRIPTION:
*       This function returns the size of the PCI 0 memory3 space.
*
* INPUT:
*       None.
*
* OUTPUT:
*       None.
*
* RETURN:
*       32 bit size of the PCI 0 memory3 space.
*
*******************************************************************************/
unsigned int gtPci0GetMemory3Size(void)
{
    unsigned int size;

    if(gtMemoryGetMemWindowStatus(MEM_PCI_0_MEM3) != 0)
        return 0;
    GT_REG_READ(PCI_0_MEMORY3_SIZE,&size);
    size = ((size << 16) | 0xffff) + 1; 
    return size;
}

/*******************************************************************************
* gtPci0ScanSelfBars - Perform a self scan of the base address and size of all 
*                      BARS (I/O, MEMORY0, MEMORY1 etc...).
*
* DESCRIPTION:
*       The GT holds several BARs in different functions (See PCI spec rev. 
*       2.1/2.2 for more details about the functions) that define the base 
*       address for several devices (such as SDRAM , devices ...). This function
*       performs a self scan of these BARs (and size registers which reside in 
*       the GT internal registers memory space) and fill the delivered structure
*       'pci0SelfBars' (defined in gtPci.h) with the read data.
*
* INPUT:
*       pci0SelfBars - Pointer to a structure 'PCI_SELF_BARS' (defined in 
*                      gtPci.h). All of its fields cover all the available 
*                      BARs of a PCI agent as defined in PCI spec rev2.1/2.2.
*
* OUTPUT:
*       Structure 'pci0SelfBars' filled with all the relevant data about the 
*       BARs and their size.
*
* RETURN:
*       None.
*
*******************************************************************************/
void gtPci0ScanSelfBars(PCI_SELF_BARS *pci0SelfBars)
{
    unsigned int size;

    /* SCS 0 */
    pci0SelfBars->SCS0Base = 
             frcPci0ReadConfigReg(PCI_SCS_0_BASE_ADDR_LOW,PCI_SELF) & 0xfffff000;
    pci0SelfBars->SCS0BaseHigh = 
             frcPci0ReadConfigReg(PCI_SCS_0_BASE_ADDR_HIGH,PCI_SELF);
    size = GTREGREAD(PCI_0_CS_0_BANK_SIZE);
    if(size)
        pci0SelfBars->SCS0Size = size + 0x1000;
    else
        pci0SelfBars->SCS0Size = 0;
    /* SCS 1 */
    pci0SelfBars->SCS1Base = 
             frcPci0ReadConfigReg(PCI_SCS_1_BASE_ADDR_LOW,PCI_SELF) & 0xfffff000;
    pci0SelfBars->SCS1BaseHigh = 
             frcPci0ReadConfigReg(PCI_SCS_1_BASE_ADDR_HIGH,PCI_SELF);
    size = GTREGREAD(PCI_0_CS_1_BANK_SIZE);
    if(size)
        pci0SelfBars->SCS1Size = size + 0x1000;
    else
        pci0SelfBars->SCS1Size = 0;
    /* SCS 2 */
    pci0SelfBars->SCS2Base = 
             frcPci0ReadConfigReg(PCI_SCS_2_BASE_ADDR_LOW,PCI_SELF) & 0xfffff000;
    pci0SelfBars->SCS2BaseHigh = 
             frcPci0ReadConfigReg(PCI_SCS_2_BASE_ADDR_HIGH,PCI_SELF);
    size = GTREGREAD(PCI_0_CS_2_BANK_SIZE);
    if(size)
        pci0SelfBars->SCS2Size = size + 0x1000;
    else
        pci0SelfBars->SCS2Size = 0;
    /* SCS 3 */
    pci0SelfBars->SCS3Base = 
             frcPci0ReadConfigReg(PCI_SCS_3_BASE_ADDR_LOW,PCI_SELF) & 0xfffff000;
    pci0SelfBars->SCS3BaseHigh = 
             frcPci0ReadConfigReg(PCI_SCS_3_BASE_ADDR_HIGH,PCI_SELF);
    size = GTREGREAD(PCI_0_CS_3_BANK_SIZE);
    if(size)
        pci0SelfBars->SCS3Size = size + 0x1000;
    else
        pci0SelfBars->SCS3Size = 0;
    /* CS 0 */
    pci0SelfBars->CS0Base = 
           frcPci0ReadConfigReg(PCI_DEVCS_0_BASE_ADDR_LOW,PCI_SELF) & 0xfffff000;
    pci0SelfBars->CS0BaseHigh = 
           frcPci0ReadConfigReg(PCI_DEVCS_0_BASE_ADDR_HIGH,PCI_SELF);
    size = GTREGREAD(PCI_0_DEVCS_0_BANK_SIZE);
    if(size)
        pci0SelfBars->CS0Size = size + 0x1000;
    else
        pci0SelfBars->CS0Size = 0;
    /* CS 1 */
    pci0SelfBars->CS1Base = 
           frcPci0ReadConfigReg(PCI_DEVCS_1_BASE_ADDR_LOW,PCI_SELF) & 0xfffff000;
    pci0SelfBars->CS1BaseHigh = 
           frcPci0ReadConfigReg(PCI_DEVCS_1_BASE_ADDR_HIGH,PCI_SELF);
    size = GTREGREAD(PCI_0_DEVCS_1_BANK_SIZE);
    if(size)
        pci0SelfBars->CS1Size = size + 0x1000;
    else
        pci0SelfBars->CS1Size = 0;
    /* CS 2 */
    pci0SelfBars->CS2Base = 
           frcPci0ReadConfigReg(PCI_DEVCS_2_BASE_ADDR_LOW,PCI_SELF) & 0xfffff000;
    pci0SelfBars->CS2BaseHigh = 
           frcPci0ReadConfigReg(PCI_DEVCS_2_BASE_ADDR_HIGH,PCI_SELF);
    size = GTREGREAD(PCI_0_DEVCS_2_BANK_SIZE);
    if(size)
        pci0SelfBars->CS2Size = size + 0x1000;
    else
        pci0SelfBars->CS2Size = 0;
    /* CS 3 */
    pci0SelfBars->CS3Base = 
           frcPci0ReadConfigReg(PCI_DEVCS_3_BASE_ADDR_LOW,PCI_SELF) & 0xfffff000;
    pci0SelfBars->CS3BaseHigh = 
           frcPci0ReadConfigReg(PCI_DEVCS_3_BASE_ADDR_HIGH,PCI_SELF);
    size = GTREGREAD(PCI_0_DEVCS_3_BANK_SIZE);
    if(size)
        pci0SelfBars->CS3Size = size + 0x1000;
    else
        pci0SelfBars->CS3Size = 0;
    /* CS BOOT  */
    pci0SelfBars->CSBootBase = 
           frcPci0ReadConfigReg(PCI_BOOT_CS_BASE_ADDR_LOW,PCI_SELF) & 0xfffff000;
    pci0SelfBars->CSBootBaseHigh = 
           frcPci0ReadConfigReg(PCI_BOOT_CS_BASE_ADDR_HIGH,PCI_SELF);
    size = GTREGREAD(PCI_0_DEVCS_BOOT_BANK_SIZE);
    if(size)
        pci0SelfBars->CSBootSize = size + 0x1000;
    else
        pci0SelfBars->CSBootSize = 0;
    /* MEMORY MAPPED INTERNAL BAR */
    pci0SelfBars->internalMemBase = frcPci0ReadConfigReg(
               PCI_INTERNAL_REG_MEM_MAPPED_BASE_ADDR_LOW,PCI_SELF) & 0xfffff000;
    pci0SelfBars->internalMemBaseHigh = frcPci0ReadConfigReg(
               PCI_INTERNAL_REG_MEM_MAPPED_BASE_ADDR_HIGH,PCI_SELF);
    /* I/O MAPPED INTERNAL BAR */
    pci0SelfBars->internalIOBase = frcPci0ReadConfigReg(
                  PCI_INTERNAL_REGS_I_O_MAPPED_BASE_ADDR,PCI_SELF) & 0xfffff000;
    /* P2P MEM0  */
    pci0SelfBars->P2PMem0Base = frcPci0ReadConfigReg(
                              PCI_P2P_MEM0_BASE_ADDR_LOW,PCI_SELF) & 0xfffff000;
    pci0SelfBars->P2PMem0BaseHigh = frcPci0ReadConfigReg(
                              PCI_P2P_MEM0_BASE_ADDR_HIGH,PCI_SELF);
    size = GTREGREAD(PCI_0_P2P_MEM0_BAR_SIZE);
    if(size)
        pci0SelfBars->P2PMem0Size = size + 0x1000;
    else
        pci0SelfBars->P2PMem0Size = 0;
    /* P2P MEM1  */
    pci0SelfBars->P2PMem1Base = 
          frcPci0ReadConfigReg(PCI_P2P_MEM1_BASE_ADDR_LOW,PCI_SELF) & 0xfffff000;
    pci0SelfBars->P2PMem1BasEHigh = 
          frcPci0ReadConfigReg(PCI_P2P_MEM1_BASE_ADDR_HIGH,PCI_SELF);
    size = GTREGREAD(PCI_0_P2P_MEM1_BAR_SIZE);
    if(size)
        pci0SelfBars->P2PMem1Size = size + 0x1000;
    else
        pci0SelfBars->P2PMem1Size = 0;
    /* P2P IO  */
    pci0SelfBars->P2PIOBase = 
               frcPci0ReadConfigReg(PCI_P2P_I_O_BASE_ADDR,PCI_SELF) & 0xfffff000;
    size = GTREGREAD(PCI_0_P2P_I_O_BAR_SIZE);
    if(size)
        pci0SelfBars->P2PIOSize = size + 0x1000;
    else
        pci0SelfBars->P2PIOSize = 0;
#ifdef INCLUDE_CPU_MAPPING 
    /* CPU BASE  */
    pci0SelfBars->CPUBase = 
               frcPci0ReadConfigReg(PCI_CPU_BASE_ADDR_LOW,PCI_SELF) & 0xfffff000;
    pci0SelfBars->CPUBaseHigh = 
               frcPci0ReadConfigReg(PCI_CPU_BASE_ADDR_HIGH,PCI_SELF);
    size = GTREGREAD(PCI_0_CPU_BAR_SIZE);
    if(size)
        pci0SelfBars->CPUSize = size + 0x1000;
    else
        pci0SelfBars->CPUSize = 0;
#endif /* INCLUDE_CPU_MAPPING */
}

/*******************************************************************************
* gtPci0MapInternalRegSpace - Maps the internal registers memory space base 
*                             address (32 bit).
*
* DESCRIPTION:
*       This function sets a new base address to the slave’s internal registers 
*       memory space (the default value is 0x14000000) of the GT.
*
* INPUT:
*       pci0InternalBase - The base address for the internal registers 
*                            memory space.
*
* OUTPUT:
*       None.
*
* RETURN:
*       None.
*
*******************************************************************************/
void gtPci0MapInternalRegSpace(unsigned int pci0InternalBase)
{
  	pci0InternalBase &= 0xffff0000;
    pci0InternalBase |= (frcPci0ReadConfigReg(
              PCI_INTERNAL_REG_MEM_MAPPED_BASE_ADDR_LOW,PCI_SELF) & 0x0000000f);
    frcPci0WriteConfigReg(PCI_INTERNAL_REG_MEM_MAPPED_BASE_ADDR_LOW,PCI_SELF,
                       pci0InternalBase);
    /* Clear the base address high value */
    frcPci0WriteConfigReg(PCI_INTERNAL_REG_MEM_MAPPED_BASE_ADDR_HIGH,PCI_SELF,0);
    /* Enable window */
    GT_RESET_REG_BITS(PCI_0_BASE_ADDR_REG_ENABLE,BIT9);
}

/*******************************************************************************
* gtPci0MapInternalRegSpaceDAC - Maps the internal registers memory space base 
*                                address (64 bit).
*
* DESCRIPTION:
*       This function sets a new base address to the slave’s internal registers 
*       memory space (the default value is 0x14000000) of the GT.
*
* INPUT:
*       pci0InternalBaseLow  - The lower part of the base address for the 
*                              internal registers memory space.
*       pci0InternalBaseHigh - The higher part of the base address for the 
*                              internal registers memory space.
*
* OUTPUT:
*       None.
*
* RETURN:
*       None.
*
*******************************************************************************/
void gtPci0MapInternalRegSpaceDAC(unsigned int pci0InternalBaseLow,
                                  unsigned int pci0InternalBaseHigh)
{
  	pci0InternalBaseLow &= 0xffff0000;
    pci0InternalBaseLow |= (frcPci0ReadConfigReg(
              PCI_INTERNAL_REG_MEM_MAPPED_BASE_ADDR_LOW,PCI_SELF) & 0x0000000f);
    frcPci0WriteConfigReg(PCI_INTERNAL_REG_MEM_MAPPED_BASE_ADDR_LOW,PCI_SELF,
                       pci0InternalBaseLow);
    frcPci0WriteConfigReg(PCI_INTERNAL_REG_MEM_MAPPED_BASE_ADDR_HIGH,PCI_SELF,
                       pci0InternalBaseHigh);
    /* Enable window */
    GT_RESET_REG_BITS(PCI_0_BASE_ADDR_REG_ENABLE,BIT9);
}

/*******************************************************************************
* gtPci0MapInternalRegIOSpace - Maps the internal registers I/O space base 
*                               address (32 bit).
*
* DESCRIPTION:
*       This function sets a new base address to the slave’s internal registers 
*       I/O space (the default value is 0x14000000) of the GT.
*
* INPUT:
*       pci0InternalBase - The base address for the internal registers 
*                          I/O space.
*
* OUTPUT:
*       None.
*
* RETURN:
*       None.
*
*******************************************************************************/
void gtPci0MapInternalRegIOSpace(unsigned int pci0InternalBase)
{
  	pci0InternalBase &= 0xffff0000;
    pci0InternalBase |= (frcPci0ReadConfigReg(
                 PCI_INTERNAL_REGS_I_O_MAPPED_BASE_ADDR,PCI_SELF) & 0x00000001);
    frcPci0WriteConfigReg(PCI_INTERNAL_REGS_I_O_MAPPED_BASE_ADDR,PCI_SELF,
                         pci0InternalBase);
    /* Enable window */
    GT_RESET_REG_BITS(PCI_0_BASE_ADDR_REG_ENABLE,BIT10);
}

/*******************************************************************************
* gtPci0MapMemoryBank0 - Maps PCI0 memory bank0 for the slave (32 bit base 
*                        address).
*
* DESCRIPTION:
*       This function sets a new base address (32 bit address) and size for the
*       PCI0 memory bank0. To close this memory space, set the parameter 
*       ‘pci0Dram0Size’ to 0, in addition of writing the value of 0 to the size
*       register of memory bank0, this function will also disable memory bank0 
*       by setting the corresponding bit in register 0xc3c (see the PCI section
*       in the GT's datasheet for more details).
*
* INPUT:
*       pci0Dram0Base - Set new base address for memory bank0.
*       pci0Dram0Size - Set new size for memory bank0.
*
* OUTPUT:
*       None.
*
* RETURN:
*       None.
*
*******************************************************************************/
void gtPci0MapMemoryBank0(unsigned int pci0Dram0Base,unsigned int pci0Dram0Size)
{
    if(pci0Dram0Size > 0)
    {
        pci0Dram0Size = (pci0Dram0Size - 1) & 0xfffff000;
        pci0Dram0Base &= 0xfffff000;
        pci0Dram0Base |= (frcPci0ReadConfigReg(PCI_SCS_0_BASE_ADDR_LOW,PCI_SELF)
                                                                  & 0x0000000f);
        frcPci0WriteConfigReg(PCI_SCS_0_BASE_ADDR_LOW,PCI_SELF,pci0Dram0Base);

        GT_REG_WRITE(PCI_0_CS_0_BANK_SIZE,pci0Dram0Size);

        /* Setting the high base address to 0 */
        frcPci0WriteConfigReg(PCI_SCS_0_BASE_ADDR_HIGH,PCI_SELF,0);
        /* Enabling window */
        GT_RESET_REG_BITS(PCI_0_BASE_ADDR_REG_ENABLE,BIT0);
    }
    else
    {
        /* Size <= 0  ---->  Closing the window */
        GT_SET_REG_BITS(PCI_0_BASE_ADDR_REG_ENABLE,BIT0);
    }
}

/*******************************************************************************
* gtPci0MapMemoryBank0DAC - Maps PCI0 memory bank0 for the slave (64 bit base 
*                          address).
*
* DESCRIPTION:
*       This function sets a new base address (64 bit base address) and size for
*       the PCI0 memory bank0. To close this memory space, set the parameter 
*       'pci0Dram0Size' to 0, in addition of writing the value of 0 to the size
*       register of memory bank0, this function will also disable memory bank0 
*       by setting the corresponding bit in register 0xc3c (see the PCI section 
*       in the GT's datasheet for more details).
*
* INPUT:
*       pci0Dram0BaseLow  - Set base address (lower part) for memory bank0.
*       pci0Dram0BaseHigh - Set base address (higher part) for memory bank0.
*       pci0Dram0Size     - Set new size for memory bank0.
*
* OUTPUT:
*       None.
*
* RETURN:
*       None.
*
*******************************************************************************/
void gtPci0MapMemoryBank0DAC(unsigned int pci0Dram0BaseLow,
                             unsigned int pci0Dram0BaseHigh,
                             unsigned int pci0Dram0Size)
{
    if(pci0Dram0Size > 0)
    {
        pci0Dram0Size = (pci0Dram0Size - 1) & 0xfffff000;
        pci0Dram0BaseLow &= 0xfffff000;
        pci0Dram0BaseLow |= (frcPci0ReadConfigReg(PCI_SCS_0_BASE_ADDR_LOW,
                                                        PCI_SELF) & 0x0000000f);
        frcPci0WriteConfigReg(PCI_SCS_0_BASE_ADDR_LOW,PCI_SELF,pci0Dram0BaseLow);
        GT_REG_WRITE(PCI_0_CS_0_BANK_SIZE,pci0Dram0Size);
        frcPci0WriteConfigReg(PCI_SCS_0_BASE_ADDR_HIGH,PCI_SELF,pci0Dram0BaseHigh);
        /* Enabling window */
        GT_RESET_REG_BITS(PCI_0_BASE_ADDR_REG_ENABLE,BIT0);
    }
    else
    {
        /* Size <= 0  ---->  Closing the window */
        GT_SET_REG_BITS(PCI_0_BASE_ADDR_REG_ENABLE,BIT0);
    }
}

/*******************************************************************************
* gtPci0MapMemoryBank1 - Maps PCI0 memory bank1 for the slave (32 bit base 
*                        address).
*
* DESCRIPTION:
*       This function sets a new base address (32 bit address) and size for the 
*       PCI0 memory bank1. To close this memory space, set the parameter 
*       ‘pci0Dram1Size’ to 0, in addition of writing the value of 0 to the size
*       register of memory bank1, this function will also disable memory bank1 
*       by setting the corresponding bit in register 0xc3c (see the PCI section 
*       in the GT's datasheet for more details).
*
* INPUT:
*       pci0Dram1Base - Set new base address for memory bank1.
*       pci0Dram1Size - Set new size for memory bank1.
*
* OUTPUT:
*       None.
*
* RETURN:
*       None.
*
*******************************************************************************/
void gtPci0MapMemoryBank1(unsigned int pci0Dram1Base,unsigned int pci0Dram1Size)
{
    if(pci0Dram1Size > 0)
    {
        pci0Dram1Size = (pci0Dram1Size - 1) & 0xfffff000;
        pci0Dram1Base &= 0xfffff000;
        pci0Dram1Base |= (frcPci0ReadConfigReg(PCI_SCS_1_BASE_ADDR_LOW,PCI_SELF)
                                                                  & 0x0000000f);
        frcPci0WriteConfigReg(PCI_SCS_1_BASE_ADDR_LOW,PCI_SELF,pci0Dram1Base);

        GT_REG_WRITE(PCI_0_CS_1_BANK_SIZE,pci0Dram1Size);

        /* Setting the high base address to 0 */
        frcPci0WriteConfigReg(PCI_SCS_1_BASE_ADDR_HIGH,PCI_SELF,0);
        /* Enabling window */
        GT_RESET_REG_BITS(PCI_0_BASE_ADDR_REG_ENABLE, BIT1);
    }
    else
    {
        /* Size <= 0  ---->  Closing the window */
        GT_SET_REG_BITS(PCI_0_BASE_ADDR_REG_ENABLE,BIT1);
    }
}

/*******************************************************************************
* gtPci0MapMemoryBank1DAC - Maps PCI0 memory bank1 for the slave (64 bit base 
*                           address).
*
* DESCRIPTION:
*       This function sets a new base address (64 bit address) and size for the 
*       PCI0 memory bank1. To close this memory space, set the parameter 
*       ‘pci0Dram1Size’ to 0, in addition of writing the value of 0 to the size
*       register of memory bank1, this function will also disable memory bank1 
*       by setting the corresponding bit in register 0xc3c (see the PCI section 
*       in the GT's datasheet for more details).
*
* INPUT:
*       pci0Dram1BaseLow  - Set base address (lower part) for memory bank1.
*       pci0Dram1BaseHigh - Set base address (higher part) for memory bank1.
*       pci0Dram1Size     - Set new size for memory bank1.
*
* OUTPUT:
*       None.
*
* RETURN:
*       None.
*
*******************************************************************************/
void gtPci0MapMemoryBank1DAC(unsigned int pci0Dram1BaseLow,
                             unsigned int pci0Dram1BaseHigh,
                             unsigned int pci0Dram1Size)
{
    if(pci0Dram1Size > 0)
    {
        pci0Dram1Size = (pci0Dram1Size - 1) & 0xfffff000;
        pci0Dram1BaseLow &= 0xfffff000;
        pci0Dram1BaseLow |= (frcPci0ReadConfigReg(PCI_SCS_1_BASE_ADDR_LOW,
                                                        PCI_SELF) & 0x0000000f);
        frcPci0WriteConfigReg(PCI_SCS_1_BASE_ADDR_LOW,PCI_SELF,pci0Dram1BaseLow);
        GT_REG_WRITE(PCI_0_CS_1_BANK_SIZE,pci0Dram1Size);
        frcPci0WriteConfigReg(PCI_SCS_1_BASE_ADDR_HIGH,PCI_SELF,pci0Dram1BaseHigh);
        /* Enabling window */
        GT_RESET_REG_BITS(PCI_0_BASE_ADDR_REG_ENABLE,BIT1);
    }
    else
    {
        /* Size <= 0  ---->  Closing the window */
        GT_SET_REG_BITS(PCI_0_BASE_ADDR_REG_ENABLE,BIT1);
    }
}
/*******************************************************************************
* gtPci0MapMemoryBank2 - Maps PCI0 memory bank2 for the slave (32 bit address).
*
* DESCRIPTION:
*       This function sets a new base address (32 bit address) and size for the 
*       PCI0 memory bank2. To close this memory space, set the parameter 
*       ‘pci0Dram2Size’ to 0, in addition of writing the value of 0 to the size
*       register of memory bank2, this function will also disable memory bank2 
*       by setting the corresponding bit in register 0xc3c (see the PCI section 
*       in the GT's datasheet for more details).
*
* INPUT:
*       pci0Dram2Base - Set new base address for memory bank2.
*       pci0Dram2Size - Set new size for memory bank2.
*
* OUTPUT:
*       None.
*
* RETURN:
*       None.
*
*******************************************************************************/
void gtPci0MapMemoryBank2(unsigned int pci0Dram2Base,unsigned int pci0Dram2Size)
{
    if(pci0Dram2Size > 0)
    {
        pci0Dram2Size = (pci0Dram2Size - 1) & 0xfffff000;
        pci0Dram2Base &= 0xfffff000;
        pci0Dram2Base |= (frcPci0ReadConfigReg(PCI_SCS_2_BASE_ADDR_LOW,PCI_SELF)
                                                                  & 0x0000000f);
        frcPci0WriteConfigReg(PCI_SCS_2_BASE_ADDR_LOW,PCI_SELF,pci0Dram2Base);

        GT_REG_WRITE(PCI_0_CS_2_BANK_SIZE,pci0Dram2Size);

        /* Setting the high base address to 0 */
        frcPci0WriteConfigReg(PCI_SCS_2_BASE_ADDR_HIGH,PCI_SELF,0);
        /* Enabling window */
        GT_RESET_REG_BITS(PCI_0_BASE_ADDR_REG_ENABLE,BIT2);
    }
    else
    {
        /* Size <= 0  ---->  Closing the window */
        GT_SET_REG_BITS(PCI_0_BASE_ADDR_REG_ENABLE,BIT2);
    }
}

/*******************************************************************************
* gtPci0MapMemoryBank2DAC - Maps PCI0 memory bank2 for the slave (64 bit 
*                           address).
*
* DESCRIPTION:
*       This function sets a new base address (64 bit address) and size for the 
*       PCI0 memory bank2. To close this memory space, set the parameter 
*       ‘pci0Dram2Size’ to 0, in addition of writing the value of 0 to the size
*       register of memory bank2, this function will also disable memory bank2 
*       by setting the corresponding bit in register 0xc3c (see the PCI section 
*       in the GT's datasheet for more details).
*
* INPUT:
*       pci0Dram2BaseLow  - Set base address (lower part) for memory bank2.
*       pci0Dram2BaseHigh - Set base address (higher part) for memory bank2.
*       pci0Dram2Size     - Set new size for memory bank2.
*
* OUTPUT:
*       None.
*
* RETURN:
*       None.
*
*******************************************************************************/
void gtPci0MapMemoryBank2DAC(unsigned int pci0Dram2BaseLow,
                             unsigned int pci0Dram2BaseHigh,
                             unsigned int pci0Dram2Size)
{
    if(pci0Dram2Size > 0)
    {
        pci0Dram2Size = (pci0Dram2Size - 1) & 0xfffff000;
        pci0Dram2BaseLow &= 0xfffff000;
        pci0Dram2BaseLow |= (frcPci0ReadConfigReg(PCI_SCS_2_BASE_ADDR_LOW,
                                                        PCI_SELF) & 0x0000000f);
        frcPci0WriteConfigReg(PCI_SCS_2_BASE_ADDR_LOW,PCI_SELF,pci0Dram2BaseLow);
        GT_REG_WRITE(PCI_0_CS_2_BANK_SIZE,pci0Dram2Size);
        frcPci0WriteConfigReg(PCI_SCS_2_BASE_ADDR_HIGH,PCI_SELF,pci0Dram2BaseHigh);
        /* Enabling window */
        GT_RESET_REG_BITS(PCI_0_BASE_ADDR_REG_ENABLE,BIT2);
    }
    else
    {
        /* Size <= 0  ---->  Closing the window */
        GT_SET_REG_BITS(PCI_0_BASE_ADDR_REG_ENABLE,BIT2);
    }
}

/*******************************************************************************
* gtPci0MapMemoryBank3 - Maps PCI0 memory bank3 for the slave (32 bit address).
*
* DESCRIPTION:
*       This function sets a new base address (32 bit address) and size for the
*       PCI0 memory bank3. To close this memory space, set the parameter 
*       ‘pci0Dram3Size’ to 0, in addition of writing the value of 0 to the size
*       register of memory bank3, this function will also disable memory bank3 
*       by setting the corresponding bit in register 0xc3c (see the PCI section 
*       in the GT's datasheet for more details).
*
* INPUT:
*       pci0Dram3Base - Set new base address for memory bank3.
*       pci0Dram3Size - Set new size for memory bank3.
*
* OUTPUT:
*       None.
*
* RETURN:
*       None.
*
*******************************************************************************/
void gtPci0MapMemoryBank3(unsigned int pci0Dram3Base,unsigned int pci0Dram3Size)
{
    if(pci0Dram3Size > 0)
    {
        pci0Dram3Size = (pci0Dram3Size - 1) & 0xfffff000;
        pci0Dram3Base &= 0xfffff000;
        pci0Dram3Base |= (frcPci0ReadConfigReg(PCI_SCS_3_BASE_ADDR_LOW,
                                                        PCI_SELF) & 0x0000000f);
        frcPci0WriteConfigReg(PCI_SCS_3_BASE_ADDR_LOW,PCI_SELF,pci0Dram3Base);

        GT_REG_WRITE(PCI_0_CS_3_BANK_SIZE ,pci0Dram3Size);

        /* Setting the high base address to 0 */
        frcPci0WriteConfigReg(PCI_SCS_3_BASE_ADDR_HIGH,PCI_SELF,0);
        /* Enabling window */
        GT_RESET_REG_BITS(PCI_0_BASE_ADDR_REG_ENABLE,BIT3);
    }
    else
    {
        /* Size <= 0  ---->  Closing the window */
        GT_SET_REG_BITS(PCI_0_BASE_ADDR_REG_ENABLE,BIT3);
    }
}

/*******************************************************************************
* gtPci0MapMemoryBank3DAC - Maps PCI0 memory bank3 for the slave (64 bit address).
*
* DESCRIPTION:
*       This function sets a new base address (64 bit address) and size for the
*       PCI0 memory bank3. To close this memory space, set the parameter 
*       ‘pci0Dram3Size’ to 0, in addition of writing the value of 0 to the size
*       register of memory bank3, this function will also disable memory bank3 
*       by setting the corresponding bit in register 0xc3c (see the PCI section 
*       in the GT's datasheet for more details).
*
* INPUT:
*       pci0Dram3BaseLow  - Set base address (lower part) for memory bank3.
*       pci0Dram3BaseHigh - Set base address (higher part) for memory bank3.
*       pci0Dram3Size     - Set new size for memory bank3.
*
* OUTPUT:
*       None.
*
* RETURN:
*       None.
*
*******************************************************************************/
void gtPci0MapMemoryBank3DAC(unsigned int pci0Dram3BaseLow,
                             unsigned int pci0Dram3BaseHigh,
                             unsigned int pci0Dram3Size)
{
    if(pci0Dram3Size > 0)
    {
        pci0Dram3Size = (pci0Dram3Size - 1) & 0xfffff000;
        pci0Dram3BaseLow &= 0xfffff000;
        pci0Dram3BaseLow |= (frcPci0ReadConfigReg(PCI_SCS_3_BASE_ADDR_LOW,
                                                        PCI_SELF) & 0x0000000f);
        frcPci0WriteConfigReg(PCI_SCS_3_BASE_ADDR_LOW,PCI_SELF,pci0Dram3BaseLow);
        GT_REG_WRITE(PCI_0_CS_3_BANK_SIZE ,pci0Dram3Size);
        frcPci0WriteConfigReg(PCI_SCS_3_BASE_ADDR_HIGH,PCI_SELF,
                                                             pci0Dram3BaseHigh);
        /* Enabling window */
        GT_RESET_REG_BITS(PCI_0_BASE_ADDR_REG_ENABLE,BIT3);
    }
    else
    {
        /* Size <= 0  ---->  Closing the window */
        GT_SET_REG_BITS(PCI_0_BASE_ADDR_REG_ENABLE,BIT3);
    }
}

/*******************************************************************************
* gtPci0MapDevice0MemorySpace - Maps PCI0 device0 memory space for the slave (32
*                               bit address).
*
* DESCRIPTION:
*       This function sets a new base address (32 bit address) and size for the
*       PCI0 device0 memory space. To close this memory space, set the parameter 
*       'pci0Dev0Size' to 0, in addition of writing the value of 0 to the size
*       register of device0 memory space, this function will also disable the 
*       device0 memory space by setting the corresponding bit in register 0xc3c 
*       (see the PCI section in the GT's datasheet for more details).
*
* INPUT:
*       pci0Dev0Base - Set new base address for device0 memory space.
*       pci0Dev0Size - Set new size for device0 memory space.
*
* OUTPUT:
*       None.
*
* RETURN:
*       None.
*
*******************************************************************************/
void gtPci0MapDevice0MemorySpace(unsigned int pci0Dev0Base,
                                 unsigned int pci0Dev0Size)
{
    if(pci0Dev0Size > 0)
    {
        pci0Dev0Size = (pci0Dev0Size - 1) & 0xfffff000;
        pci0Dev0Base &= 0xfffff000;
        pci0Dev0Base |= (frcPci0ReadConfigReg(PCI_DEVCS_0_BASE_ADDR_LOW,
                                                        PCI_SELF) & 0x0000000f);
        frcPci0WriteConfigReg(PCI_DEVCS_0_BASE_ADDR_LOW,PCI_SELF,pci0Dev0Base);

        GT_REG_WRITE(PCI_0_DEVCS_0_BANK_SIZE,pci0Dev0Size);

        /* Setting the high base address to 0 */
        frcPci0WriteConfigReg(PCI_DEVCS_0_BASE_ADDR_HIGH,PCI_SELF,0);
        /* Enabling window */
        GT_RESET_REG_BITS(PCI_0_BASE_ADDR_REG_ENABLE,BIT4);
    }
    else
    {
        /* Size <= 0  ---->  Closing the window */
        GT_SET_REG_BITS(PCI_0_BASE_ADDR_REG_ENABLE,BIT4);
    }
}

/*******************************************************************************
* gtPci0MapDevice0MemorySpaceDAC - Maps PCI0 device0 memory space for the slave 
*                                  (64 bit address).
*
* DESCRIPTION:
*       This function sets a new base address (64 bit address) and size for the 
*       PCI0 device0 memory space. To close this memory space, set the parameter 
*       'pci0Dev0Size' to 0, in addition of writing the value of 0 to the size
*       register of device0 memory space, this function will also disable the 
*       device0 memory space by setting the corresponding bit in register 0xc3c 
*       (see the PCI section in the GT's datasheet for more details).
*
* INPUT:
*       pci0Dev0BaseLow  - Set base address (lower part) for device0 memory 
*                           space.
*       pci0Dev0BaseHigh - Set base address (higher part) for device0 memory 
*                            space.
*       pci0Dev0Size     - Set new size for device0 memory space.
*
* OUTPUT:
*       None.
*
* RETURN:
*       None.
*
*******************************************************************************/
void gtPci0MapDevice0MemorySpaceDAC(unsigned int pci0Dev0BaseLow,
                                    unsigned int pci0Dev0BaseHigh,
                                    unsigned int pci0Dev0Size)
{
    if(pci0Dev0Size > 0)
    {
        pci0Dev0Size = (pci0Dev0Size - 1) & 0xfffff000;
        pci0Dev0BaseLow &= 0xfffff000;
        pci0Dev0BaseLow |= (frcPci0ReadConfigReg(
                              PCI_DEVCS_0_BASE_ADDR_LOW,PCI_SELF) & 0x0000000f);
        frcPci0WriteConfigReg(PCI_DEVCS_0_BASE_ADDR_LOW,PCI_SELF,
                                                               pci0Dev0BaseLow);
        GT_REG_WRITE(PCI_0_DEVCS_0_BANK_SIZE,pci0Dev0Size);
        frcPci0WriteConfigReg(PCI_DEVCS_0_BASE_ADDR_HIGH,PCI_SELF,
                                                              pci0Dev0BaseHigh);
        /* Enabling window */
        GT_RESET_REG_BITS(PCI_0_BASE_ADDR_REG_ENABLE,BIT4);
    }
    else
    {
        /* Size <= 0  ---->  Closing the window */
        GT_SET_REG_BITS(PCI_0_BASE_ADDR_REG_ENABLE,BIT4);
    }
}

/*******************************************************************************
* gtPci0MapDevice1MemorySpace - Maps PCI0 device1 memory space for the slave (32
*                               bit address).
*
* DESCRIPTION:
*       This function sets a new base address (32 bit address) and size for the
*       PCI0 device1 memory space. To close this memory space, set the parameter 
*       'pci0Dev1Size' to 0, in addition of writing the value of 0 to the size
*       register of device1 memory space, this function will also disable the 
*       device1 memory space by setting the corresponding bit in register 0xc3c 
*       (see the PCI section in the GT's datasheet for more details).
*
* INPUT:
*       pci0Dev1Base - Set new base address for device1 memory space.
*       pci0Dev1Size - Set new size for device1 memory space.
*
* OUTPUT:
*       None.
*
* RETURN:
*       None.
*
*******************************************************************************/
void gtPci0MapDevice1MemorySpace(unsigned int pci0Dev1Base,
                                 unsigned int pci0Dev1Size)
{
    if(pci0Dev1Size > 0)
    {
        pci0Dev1Size = (pci0Dev1Size - 1) & 0xfffff000;
        pci0Dev1Base &= 0xfffff000;
        pci0Dev1Base |= (frcPci0ReadConfigReg(
                              PCI_DEVCS_1_BASE_ADDR_LOW,PCI_SELF) & 0x0000000f);
        frcPci0WriteConfigReg(PCI_DEVCS_1_BASE_ADDR_LOW,PCI_SELF,pci0Dev1Base);

        GT_REG_WRITE(PCI_0_DEVCS_1_BANK_SIZE,pci0Dev1Size); 

        /* Setting the high base address to 0 */
        frcPci0WriteConfigReg(PCI_DEVCS_1_BASE_ADDR_HIGH,PCI_SELF,0);
        /* Enabling window */
        GT_RESET_REG_BITS(PCI_0_BASE_ADDR_REG_ENABLE,BIT5);
    }
    else
    {
        /* Size <= 0  ---->  Closing the window */
        GT_SET_REG_BITS(PCI_0_BASE_ADDR_REG_ENABLE,BIT5);
    }
}

/*******************************************************************************
* gtPci0MapDevice1MemorySpaceDAC - Maps PCI0 device1 memory space for the slave 
*                                  (64 bit address).
*
* DESCRIPTION:
*       This function sets a new base address (64 bit address) and size for the 
*       PCI0 device1 memory space. To close this memory space, set the parameter 
*       'pci0Dev1Size' to 0, in addition of writing the value of 0 to the size
*       register of device1 memory space, this function will also disable the 
*       device1 memory space by setting the corresponding bit in register 0xc3c 
*       (see the PCI section in the GT's datasheet for more details).
*
* INPUT:
*       pci0Dev1BaseLow  - Set base address (lower part) for device1 memory 
*                           space.
*       pci0Dev1BaseHigh - Set base address (higher part) for device1 memory 
*                            space.
*       pci0Dev1Size     - Set new size for device1 memory space.
*
* OUTPUT:
*       None.
*
* RETURN:
*       None.
*
*******************************************************************************/
void gtPci0MapDevice1MemorySpaceDAC(unsigned int pci0Dev1BaseLow,
                                    unsigned int pci0Dev1BaseHigh,
                                    unsigned int pci0Dev1Size)
{
    if(pci0Dev1Size > 0)
    {
        pci0Dev1Size = (pci0Dev1Size - 1) & 0xfffff000;
        pci0Dev1BaseLow &= 0xfffff000;
        pci0Dev1BaseLow |= (frcPci0ReadConfigReg(PCI_DEVCS_1_BASE_ADDR_LOW,
                                                        PCI_SELF) & 0x0000000f);
        frcPci0WriteConfigReg(PCI_DEVCS_1_BASE_ADDR_LOW,PCI_SELF,
                                                               pci0Dev1BaseLow);
        GT_REG_WRITE(PCI_0_DEVCS_1_BANK_SIZE,pci0Dev1Size);
        frcPci0WriteConfigReg(PCI_DEVCS_1_BASE_ADDR_HIGH,PCI_SELF,
                                                              pci0Dev1BaseHigh);
        /* Enabling window */
        GT_RESET_REG_BITS(PCI_0_BASE_ADDR_REG_ENABLE,BIT5);
    }
    else
    {
        /* Size <= 0  ---->  Closing the window */
        GT_SET_REG_BITS(PCI_0_BASE_ADDR_REG_ENABLE,BIT5);
    }
}

/*******************************************************************************
* gtPci0MapDevice2MemorySpace - Maps PCI0 device2 memory space for the slave (32
*                               bit address).
*
* DESCRIPTION:
*       This function sets a new base address (32 bit address) and size for the
*       PCI0 device2 memory space. To close this memory space, set the parameter 
*       'pci0Dev2Size' to 0, in addition of writing the value of 0 to the size
*       register of device2 memory space, this function will also disable the 
*       device2 memory space by setting the corresponding bit in register 0xc3c 
*       (see the PCI section in the GT's datasheet for more details).
*
* INPUT:
*       pci0Dev2Base - Set new base address for device2 memory space.
*       pci0Dev2Size - Set new size for device2 memory space.
*
* OUTPUT:
*       None.
*
* RETURN:
*       None.
*
*******************************************************************************/
void gtPci0MapDevice2MemorySpace(unsigned int pci0Dev2Base,
                                 unsigned int pci0Dev2Size)
{
    if(pci0Dev2Size > 0)
    {
        pci0Dev2Size = (pci0Dev2Size - 1) & 0xfffff000;
        pci0Dev2Base &= 0xfffff000;
        pci0Dev2Base |= (frcPci0ReadConfigReg(
                              PCI_DEVCS_2_BASE_ADDR_LOW,PCI_SELF) & 0x0000000f);
        frcPci0WriteConfigReg(PCI_DEVCS_2_BASE_ADDR_LOW,PCI_SELF,pci0Dev2Base);

        GT_REG_WRITE(PCI_0_DEVCS_2_BANK_SIZE,pci0Dev2Size);

        /* Setting the high base address to 0 */
        frcPci0WriteConfigReg(PCI_DEVCS_2_BASE_ADDR_HIGH,PCI_SELF,0);
        /* Enabling window */
        GT_RESET_REG_BITS(PCI_0_BASE_ADDR_REG_ENABLE,BIT6);
    }
    else
    {
        /* Size <= 0  ---->  Closing the window */
        GT_SET_REG_BITS(PCI_0_BASE_ADDR_REG_ENABLE,BIT6);
    }
}

/*******************************************************************************
* gtPci0MapDevice2MemorySpaceDAC - Maps PCI0 device2 memory space for the slave 
*                                  (64 bit address).
*
* DESCRIPTION:
*       This function sets a new base address (64 bit address) and size for the 
*       PCI0 device2 memory space. To close this memory space, set the parameter 
*       'pci0Dev2Size' to 0, in addition of writing the value of 0 to the size
*       register of device2 memory space, this function will also disable the 
*       device2 memory space by setting the corresponding bit in register 0xc3c 
*       (see the PCI section in the GT's datasheet for more details).
*
* INPUT:
*       pci0Dev2BaseLow  - Set base address (lower part) for device2 memory 
*                           space.
*       pci0Dev2BaseHigh - Set base address (higher part) for device2 memory 
*                            space.
*       pci0Dev2Size     - Set new size for device2 memory space.
*
* OUTPUT:
*       None.
*
* RETURN:
*       None.
*
*******************************************************************************/
void gtPci0MapDevice2MemorySpaceDAC(unsigned int pci0Dev2BaseLow,
                                    unsigned int pci0Dev2BaseHigh,
                                    unsigned int pci0Dev2Size)
{
    if(pci0Dev2Size > 0)
    {
        pci0Dev2Size = (pci0Dev2Size - 1) & 0xfffff000;
        pci0Dev2BaseLow &= 0xfffff000;
        pci0Dev2BaseLow |= (frcPci0ReadConfigReg(PCI_DEVCS_2_BASE_ADDR_LOW,
                                                        PCI_SELF) & 0x0000000f);
        frcPci0WriteConfigReg(PCI_DEVCS_2_BASE_ADDR_LOW,PCI_SELF,
                                                               pci0Dev2BaseLow);
        GT_REG_WRITE(PCI_0_DEVCS_2_BANK_SIZE,pci0Dev2Size);
        frcPci0WriteConfigReg(PCI_DEVCS_2_BASE_ADDR_HIGH,PCI_SELF,
                                                              pci0Dev2BaseHigh);
        /* Enabling window */
        GT_RESET_REG_BITS(PCI_0_BASE_ADDR_REG_ENABLE,BIT6);
    }
    else
    {
        /* Size <= 0  ---->  Closing the window */
        GT_SET_REG_BITS(PCI_0_BASE_ADDR_REG_ENABLE,BIT6);
    }
}
/*******************************************************************************
* gtPci0MapDevice3MemorySpace - Maps PCI0 device3 memory space for the slave (32
*                               bit address).
*
* DESCRIPTION:
*       This function sets a new base address (32 bit address) and size for the
*       PCI0 device3 memory space. To close this memory space, set the parameter 
*       'pci0Dev3Size' to 0, in addition of writing the value of 0 to the size
*       register of device3 memory space, this function will also disable the 
*       device3 memory space by setting the corresponding bit in register 0xc3c 
*       (see the PCI section in the GT's datasheet for more details).
*
* INPUT:
*       pci0Dev3Base - Set new base address for device3 memory space.
*       pci0Dev3Size - Set new size for device3 memory space.
*
* OUTPUT:
*       None.
*
* RETURN:
*       None.
*
*******************************************************************************/
void gtPci0MapDevice3MemorySpace(unsigned int pci0Dev3Base,
                                 unsigned int pci0Dev3Size)
{
    if(pci0Dev3Size > 0)
    {
        pci0Dev3Size = (pci0Dev3Size - 1) & 0xfffff000;
        pci0Dev3Base &= 0xfffff000;
        pci0Dev3Base |= (frcPci0ReadConfigReg(PCI_DEVCS_3_BASE_ADDR_LOW,
                                                        PCI_SELF) & 0x0000000f);
        frcPci0WriteConfigReg(PCI_DEVCS_3_BASE_ADDR_LOW,PCI_SELF,pci0Dev3Base);

        GT_REG_WRITE(PCI_0_DEVCS_3_BANK_SIZE,pci0Dev3Size);

        /* Setting the high base address to 0 */
        frcPci0WriteConfigReg(PCI_DEVCS_3_BASE_ADDR_HIGH,PCI_SELF,0);
        /* Enabling window */
        GT_RESET_REG_BITS(PCI_0_BASE_ADDR_REG_ENABLE,BIT7);
    }
    else
    {
        /* Size <= 0  ---->  Closing the window */
        GT_SET_REG_BITS(PCI_0_BASE_ADDR_REG_ENABLE,BIT7);
    }
}

/*******************************************************************************
* gtPci0MapDevice3MemorySpaceDAC - Maps PCI0 device3 memory space for the slave 
*                                  (64 bit address).
*
* DESCRIPTION:
*       This function sets a new base address (64 bit address) and size for the 
*       PCI0 device3 memory space. To close this memory space, set the parameter 
*       'pci0Dev3Size' to 0, in addition of writing the value of 0 to the size
*       register of device3 memory space, this function will also disable the 
*       device3 memory space by setting the corresponding bit in register 0xc3c 
*       (see the PCI section in the GT's datasheet for more details).
*
* INPUT:
*       pci0Dev3BaseLow  - Set base address (lower part) for device3 memory 
*                           space.
*       pci0Dev3BaseHigh - Set base address (higher part) for device3 memory 
*                           space.
*       pci0Dev3Size     - Set new size for device3 memory space.
*
* OUTPUT:
*       None.
*
* RETURN:
*       None.
*
*******************************************************************************/
void gtPci0MapDevice3MemorySpaceDAC(unsigned int pci0Dev3BaseLow,
                                    unsigned int pci0Dev3BaseHigh,
                                    unsigned int pci0Dev3Size)
{
    if(pci0Dev3Size > 0)
    {
        pci0Dev3Size = (pci0Dev3Size - 1) & 0xfffff000;
        pci0Dev3BaseLow &= 0xfffff000;
        pci0Dev3BaseLow |= (frcPci0ReadConfigReg(PCI_DEVCS_3_BASE_ADDR_LOW,
                                                        PCI_SELF) & 0x0000000f);
        frcPci0WriteConfigReg(PCI_DEVCS_3_BASE_ADDR_LOW,PCI_SELF,pci0Dev3BaseLow);
        GT_REG_WRITE(PCI_0_DEVCS_3_BANK_SIZE,pci0Dev3Size);
        frcPci0WriteConfigReg(PCI_DEVCS_3_BASE_ADDR_HIGH,PCI_SELF,pci0Dev3BaseHigh);
        /* Enabling window */
        GT_RESET_REG_BITS(PCI_0_BASE_ADDR_REG_ENABLE,BIT7);
    }
    else
    {
        /* Size <= 0  ---->  Closing the window */
        GT_SET_REG_BITS(PCI_0_BASE_ADDR_REG_ENABLE,BIT7);
    }
}

/*******************************************************************************
* gtPci0MapBootDeviceMemorySpace - Maps PCI0 boot device memory space for the 
*                                  slave.
*
* DESCRIPTION:
*       This function sets a new base address (32 bit address) and size for the
*       PCI0 boot device memory space. To close this memory space, set the 
*       parameter 'pci0DevBootSize' to 0, in addition of writing the value of
*       0 to the size register of boot device memory space, this function will 
*       also disable the boot device memory space by setting the corresponding 
*       bit in register 0xc3c (see the PCI section in the GT's datasheet for 
*       more details).
*
* INPUT:
*       pci0DevBootBase - Set base address (32 bit address) for boot device 
*                           memory space.
*       pci0DevBootSize - Set new size for boot device memory space.
*
* OUTPUT:
*       None.
*
* RETURN:
*       None.
*
*******************************************************************************/
void gtPci0MapBootDeviceMemorySpace(unsigned int pci0DevBootBase,
                                    unsigned int pci0DevBootSize)
{
    if(pci0DevBootSize > 0)
    {
        pci0DevBootSize = (pci0DevBootSize - 1) & 0xfffff000;
        pci0DevBootBase &= 0xfffff000;
        pci0DevBootBase |= (frcPci0ReadConfigReg(PCI_BOOT_CS_BASE_ADDR_LOW,
                                                        PCI_SELF) & 0x0000000f);
        frcPci0WriteConfigReg(PCI_BOOT_CS_BASE_ADDR_LOW,PCI_SELF,pci0DevBootBase);

        GT_REG_WRITE(PCI_0_DEVCS_BOOT_BANK_SIZE,pci0DevBootSize);

        /* Setting the high base address to 0 */
        frcPci0WriteConfigReg(PCI_BOOT_CS_BASE_ADDR_HIGH,PCI_SELF,0);
        /* Enabling window */
        GT_RESET_REG_BITS(PCI_0_BASE_ADDR_REG_ENABLE,BIT8);
    }
    else
    {
        /* Size <= 0  ---->  Closing the window */
        GT_SET_REG_BITS(PCI_0_BASE_ADDR_REG_ENABLE,BIT8);
    }
}

/*******************************************************************************
* gtPci0MapBootDeviceMemorySpaceDAC - Maps PCI0 boot device memory space for the 
*                                     slave.
*
* DESCRIPTION:
*       This function sets a new base address (64 bit address) and size for the
*       PCI0 boot device memory space. To close this memory space, set the 
*       parameter 'pci0DevBootSize' to 0, in addition of writing the value of
*       0 to the size register of boot device memory space, this function will 
*       also disable the boot device memory space by setting the corresponding 
*       bit in register 0xc3c (see the PCI section in the GT's datasheet for 
*       more details).
*
* INPUT:
*       pci0DevBootBaseLow  - Set base address (lower part of the base address)
*                              for boot device memory space.
*       pci0DevBootBaseHigh - Set base address (higher part of the base 
*                               address) for boot device memory space.
*       pci0DevBootSize     - Set new size for boot device memory space.
*
* OUTPUT:
*       None.
*
* RETURN:
*       None.
*
*******************************************************************************/
void gtPci0MapBootDeviceMemorySpaceDAC(unsigned int pci0DevBootBaseLow,
                                       unsigned int pci0DevBootBaseHigh,
                                       unsigned int pci0DevBootSize)
{
    if(pci0DevBootSize > 0)
    {
        pci0DevBootSize = (pci0DevBootSize - 1) & 0xfffff000;
        pci0DevBootBaseLow &= 0xfffff000;
        pci0DevBootBaseLow |= (frcPci0ReadConfigReg(PCI_BOOT_CS_BASE_ADDR_LOW,
                                                        PCI_SELF) & 0x0000000f);
        frcPci0WriteConfigReg(PCI_BOOT_CS_BASE_ADDR_LOW,PCI_SELF,
                                                            pci0DevBootBaseLow);
        GT_REG_WRITE(PCI_0_DEVCS_BOOT_BANK_SIZE,pci0DevBootSize);
        frcPci0WriteConfigReg(PCI_BOOT_CS_BASE_ADDR_HIGH,PCI_SELF,
                                                           pci0DevBootBaseHigh);
        /* Enabling window */
        GT_RESET_REG_BITS(PCI_0_BASE_ADDR_REG_ENABLE,BIT8);
    }
    else
    {
        /* Size <= 0  ---->  Closing the window */
        GT_SET_REG_BITS(PCI_0_BASE_ADDR_REG_ENABLE,BIT8);
    }
}

/*******************************************************************************
* gtPci0MapP2pMem0Space - Maps PCI0 P2P memory space0 for the slave (32 bit).
*
* DESCRIPTION:
*       This function sets a new base address and size for the pci P2P memory 
*       space0. To close this memory space, set the parameter 
*       'pci0P2pMem0Size' to 0, in addition of writing the value of 0 to the 
*       size register of the P2P memory space0, this function will also disable 
*       the the P2P memory space0 by setting the corresponding bit in register 
*       0xc3c (see the PCI section in the GT's datasheet for more details).
*
* INPUT:
*       pci0P2pMem0Base - Set new base address (32 bit) for the P2P memory 
*                           space0.
*       pci0P2pMem0Size - Set new size for the P2P memory space0.
*
* OUTPUT:
*       None.
*
* RETURN:
*       None.
*
*******************************************************************************/
void gtPci0MapP2pMem0Space(unsigned int pci0P2pMem0Base,
                           unsigned int pci0P2pMem0Size)
{
    if(pci0P2pMem0Size > 0)
    {
        pci0P2pMem0Size = (pci0P2pMem0Size - 1) & 0xfffff000;
        pci0P2pMem0Base &= 0xfffff000;
        pci0P2pMem0Base |= (frcPci0ReadConfigReg(
                             PCI_P2P_MEM0_BASE_ADDR_LOW,PCI_SELF) & 0x0000000f);
        frcPci0WriteConfigReg(PCI_P2P_MEM0_BASE_ADDR_LOW,PCI_SELF,
                                                               pci0P2pMem0Base);
        GT_REG_WRITE(PCI_0_P2P_MEM0_BAR_SIZE ,pci0P2pMem0Size);
        /* Setting the high base address to 0 */
        frcPci0WriteConfigReg(PCI_P2P_MEM0_BASE_ADDR_HIGH,PCI_SELF,0);
        /* Enabling window */
        GT_RESET_REG_BITS(PCI_0_BASE_ADDR_REG_ENABLE, BIT11);
    }
    else
    {
        /* Size <= 0  ---->  Closing the window */
        GT_SET_REG_BITS(PCI_0_BASE_ADDR_REG_ENABLE, BIT11);
    }
}
/*******************************************************************************
* gtPci0MapP2pMem0SpaceDAC - Maps PCI0 P2P memory space0 for the slave (64 bit).
*
* DESCRIPTION:
*       This function sets a new base address and size for the PCI0 P2P memory 
*       space0. To close this memory space, set the parameter 
*       'pci0P2pMem0Size' to 0, in addition of writing the value of 0 to the 
*       size register of the P2P memory space0, this function will also disable 
*       the the P2P memory space0 by setting the corresponding bit in register 
*       0xc3c (see the PCI section in the GT's datasheet for more details).
*
* INPUT:
*       pci0P2pMem0BaseLow - Set new base address (low 64 bit) for the P2P 
*                              memory space0.
*       pci0P2pMem0BaseHigh - Set new base address (high 64 bit) for the P2P 
*                               memory space0.
*       pci0P2pMem0Size - Set new size for the P2P memory space0.
*
* OUTPUT:
*       None.
*
* RETURN:
*       None.
*
*******************************************************************************/
void gtPci0MapP2pMem0SpaceDAC(unsigned int pci0P2pMem0BaseLow, 
                              unsigned int pci0P2pMem0BaseHigh, 
                              unsigned int pci0P2pMem0Size)
{
    if(pci0P2pMem0Size > 0)
    {
        pci0P2pMem0Size = (pci0P2pMem0Size - 1) & 0xfffff000;
        pci0P2pMem0BaseLow &= 0xfffff000;
        pci0P2pMem0BaseLow |= (frcPci0ReadConfigReg(PCI_P2P_MEM0_BASE_ADDR_LOW,
                                                        PCI_SELF) & 0x0000000f);
        frcPci0WriteConfigReg(PCI_P2P_MEM0_BASE_ADDR_LOW,PCI_SELF,
                                                            pci0P2pMem0BaseLow);
        GT_REG_WRITE(PCI_0_P2P_MEM0_BAR_SIZE ,pci0P2pMem0Size);
        frcPci0WriteConfigReg(PCI_P2P_MEM0_BASE_ADDR_HIGH,PCI_SELF,
                                                           pci0P2pMem0BaseHigh);
        /* Enabling window */
        GT_RESET_REG_BITS(PCI_0_BASE_ADDR_REG_ENABLE, BIT11);
    }
    else
    {
        /* Size <= 0  ---->  Closing the window */
        GT_SET_REG_BITS(PCI_0_BASE_ADDR_REG_ENABLE, BIT11);
    }
}

/*******************************************************************************
* gtPci0MapP2pMem1Space - Maps PCI0 P2P memory space1 for the slave (32 bit).
*
* DESCRIPTION:
*       This function sets a new base address and size for the pci P2P memory 
*       space1. To close this memory space, set the parameter 
*       'pci0P2pMem1Size' to 0, in addition of writing the value of 0 to the 
*       size register of the P2P memory space1, this function will also disable 
*       the the P2P memory space1 by setting the corresponding bit in register 
*       0xc3c (see the PCI section in the GT's datasheet for more details).
*
* INPUT:
*       pci0P2pMem1Base - Set new base address (32 bit) for the P2P memory 
*                           space1.
*       pci0P2pMem1Size - Set new size for the P2P memory space1.
*
* OUTPUT:
*       None.
*
* RETURN:
*       None.
*
*******************************************************************************/
void gtPci0MapP2pMem1Space(unsigned int pci0P2pMem1Base, 
                           unsigned int pci0P2pMem1Size)
{
    if(pci0P2pMem1Size > 0)
    {
        pci0P2pMem1Size = (pci0P2pMem1Size - 1) & 0xfffff000;
        pci0P2pMem1Base &= 0xfffff000;
        pci0P2pMem1Base |= (frcPci0ReadConfigReg(PCI_P2P_MEM1_BASE_ADDR_LOW,
                                                        PCI_SELF) & 0x0000000f);
        frcPci0WriteConfigReg(PCI_P2P_MEM1_BASE_ADDR_LOW,PCI_SELF,
                                                               pci0P2pMem1Base);
        GT_REG_WRITE(PCI_0_P2P_MEM1_BAR_SIZE ,pci0P2pMem1Size);
        /* Setting the high base address to 0 */
        frcPci0WriteConfigReg(PCI_P2P_MEM1_BASE_ADDR_HIGH,PCI_SELF,0);
        /* Enabling window */
        GT_RESET_REG_BITS(PCI_0_BASE_ADDR_REG_ENABLE, BIT12);
    }
    else
    {
        /* Size <= 0  ---->  Closing the window */
        GT_SET_REG_BITS(PCI_0_BASE_ADDR_REG_ENABLE, BIT12);
    }
}

/*******************************************************************************
* pci0MapP2pMem1SpaceDAC - Maps PCI0 P2P memory space1 for the slave (64 bit).
*
* DESCRIPTION:
*       This function sets a new base address and size for the PCI0 P2P memory 
*       space1. To close this memory space, set the parameter 
*       'pci0P2pMem1Size' to 0, in addition of writing the value of 0 to the 
*       size register of the P2P memory space0, this function will also disable 
*       the the P2P memory space1 by setting the corresponding bit in register 
*       0xc3c (see the PCI section in the GT's datasheet for more details).
*
* INPUT:
*       pci0P2pMem1BaseLow - Set new base address (low 64 bit) for the P2P 
*                              memory space1.
*       pci0P2pMem1BaseHigh - Set new base address (high 64 bit) for the P2P 
*                               memory space1.
*       pci0P2pMem1Size - Set new size for the P2P memory space1.
*
* OUTPUT:
*       None.
*
* RETURN:
*       None.
*
*******************************************************************************/
void gtPci0MapP2pMem1SpaceDAC(unsigned int pci0P2pMem1BaseLow, 
                              unsigned int pci0P2pMem1BaseHigh, 
                              unsigned int pci0P2pMem1Size)
{
    if(pci0P2pMem1Size > 0)
    {
        pci0P2pMem1Size = (pci0P2pMem1Size - 1) & 0xfffff000;
        pci0P2pMem1BaseLow &= 0xfffff000;
        pci0P2pMem1BaseLow |= (frcPci0ReadConfigReg(PCI_P2P_MEM1_BASE_ADDR_LOW,
                                                        PCI_SELF) & 0x0000000f);
        frcPci0WriteConfigReg(PCI_P2P_MEM1_BASE_ADDR_LOW,PCI_SELF,
                                                            pci0P2pMem1BaseLow);
        GT_REG_WRITE(PCI_0_P2P_MEM1_BAR_SIZE ,pci0P2pMem1Size);
        frcPci0WriteConfigReg(PCI_P2P_MEM1_BASE_ADDR_HIGH,PCI_SELF,
                                                           pci0P2pMem1BaseHigh);
        /* Enabling window */
        GT_RESET_REG_BITS(PCI_0_BASE_ADDR_REG_ENABLE, BIT12);
    }
    else
    {
        /* Size <= 0  ---->  Closing the window */
        GT_SET_REG_BITS(PCI_0_BASE_ADDR_REG_ENABLE, BIT12);
    }
}

/*******************************************************************************
* gtPci0MapP2pIoSpace - Maps PCI0 I/O memory space for the slave.
*
* DESCRIPTION:
*       This function sets a new base address and size for the PCI0 I/O memory 
*       space. To close this memory space, set the parameter 'pci0P2pIoSize' 
*       to 0, in addition of writing the value of 0 to the size register of the
*       P2P I/O memory space, this function will also disable the the P2P I/O 
*       memory space by setting the corresponding bit in register 0xc3c (see the
*       PCI section in the GT's datasheet for more details).
*
* INPUT:
*       pci0P2pIoBase - Set new base address for the P2P I/O memory space.
*       pci0P2pIoSize - Set new size for the P2P I/O memory space.
*
* OUTPUT:
*       None.
*
* RETURN:
*       None.
*
*******************************************************************************/
void gtPci0MapP2pIoSpace(unsigned int pci0P2pIoBase,unsigned int pci0P2pIoSize)
{
    if(pci0P2pIoSize > 0)
    {
        pci0P2pIoSize = (pci0P2pIoSize - 1) & 0xfffff000;
        pci0P2pIoBase &= 0xfffff000;
        pci0P2pIoBase |= (frcPci0ReadConfigReg(PCI_P2P_MEM1_BASE_ADDR_LOW,
                                                        PCI_SELF) & 0x0000000f);
        frcPci0WriteConfigReg(PCI_P2P_I_O_BASE_ADDR,PCI_SELF,pci0P2pIoBase);
        GT_REG_WRITE(PCI_0_P2P_MEM1_BAR_SIZE ,pci0P2pIoSize);
        /* Enabling window */
        GT_RESET_REG_BITS(PCI_0_BASE_ADDR_REG_ENABLE, BIT13);
    }
    else
    {
        /* Size <= 0  ---->  Closing the window */
        GT_SET_REG_BITS(PCI_0_BASE_ADDR_REG_ENABLE, BIT13);
    }
}

#ifdef INCLUDE_CPU_MAPPING
/*******************************************************************************
* gtPci0MapCPUspace - Maps PCI0 CPU memory space for the slave.
*
* DESCRIPTION:
*       This function sets a new base address and size for the PCI0 CPU memory 
*       space. To close this memory space, set the parameter 'pci0CpuSize' to
*       0, in addition of writing the value of 0 to the size register of the P2P
*       CPU memory space, this function will also disable the the P2P CPU memory
*       space by setting the corresponding bit in register 0xc3c (see the PCI 
*       section in the GT's datasheet for more details).
*
* INPUT:
*       pci0CpuBase - Set new base address for the P2P CPU memory space.
*       pci0CpuSize - Set new size for the P2P CPU memory space.
*
* OUTPUT:
*       None.
*
* RETURN:
*       None.
*
*******************************************************************************/
void gtPci0MapCPUspace(unsigned int pci0CpuBase,unsigned int pci0CpuSize)
{
    if(pci0CpuSize > 0)
    {
        pci0CpuSize = (pci0CpuSize - 1) & 0xfffff000;
        pci0CpuBase &= 0xfffff000;
        pci0CpuBase |= (frcPci0ReadConfigReg(PCI_CPU_BASE_ADDR_LOW,PCI_SELF)
                                                                  & 0x0000000f);
        frcPci0WriteConfigReg(PCI_CPU_BASE_ADDR_LOW,PCI_SELF,pci0CpuBase);
        GT_REG_WRITE(PCI_0_CPU_BAR_SIZE ,pci0CpuSize);
        /* Setting the high base address to 0 */
        frcPci0WriteConfigReg(PCI_CPU_BASE_ADDR_LOW,PCI_SELF,0);
        /* Enabling window */
        GT_RESET_REG_BITS(PCI_0_BASE_ADDR_REG_ENABLE, BIT14);
    }
    else
    {
        /* Size <= 0  ---->  Closing the window */
        GT_SET_REG_BITS(PCI_0_BASE_ADDR_REG_ENABLE, BIT14);
    }
}

/*******************************************************************************
* gtPci0MapCPUspaceDAC - Maps PCI0 CPU memory space for the slave (64 bit base 
*                        address).
*
* DESCRIPTION:
*       This function sets a new base address (64 bit address)and size for the 
*       PCI0 CPU memory space. To close this memory space, set the parameter 
*       'pci0CpuSize' to 0, in addition of writing the value of 0 to the size 
*       register of the P2P CPU memory space, this function will also disable 
*       the the P2P CPU memory space by setting the corresponding bit in 
*       register 0xc3c (see the PCI section in the GT's datasheet for more 
*       details).
*
* INPUT:
*       pci0CpuBaseLow - Set new base address (lower part) for the P2P CPU 
*                          memory space.
*       pci0CpuBaseHigh - Set new base address (higher part) for the P2P CPU 
*                           memory space.
*       pci0CpuSize - Set new size for the P2P CPU memory space.
*
* OUTPUT:
*       None.
*
* RETURN:
*       None.
*
*******************************************************************************/
void gtPci0MapCPUspaceDAC(unsigned int pci0CpuBaseLow,
                          unsigned int pci0CpuBaseHigh,
                          unsigned int pci0CpuSize)
{
    if(pci0CpuSize > 0)
    {
        pci0CpuSize = (pci0CpuSize - 1) & 0xfffff000;
        pci0CpuBaseLow &= 0xfffff000;
        pci0CpuBaseLow |= (frcPci0ReadConfigReg(PCI_CPU_BASE_ADDR_LOW,PCI_SELF) 
                                                                  & 0x0000000f);
        frcPci0WriteConfigReg(PCI_CPU_BASE_ADDR_LOW,PCI_SELF,pci0CpuBaseLow);
        GT_REG_WRITE(PCI_0_CPU_BAR_SIZE ,pci0CpuSize);
        frcPci0WriteConfigReg(PCI_CPU_BASE_ADDR_LOW,PCI_SELF,pci0CpuBaseHigh);
        /* Enabling window */
        GT_RESET_REG_BITS(PCI_0_BASE_ADDR_REG_ENABLE, BIT14);
    }
    else
    {
        /* Size <= 0  ---->  Closing the window */
        GT_SET_REG_BITS(PCI_0_BASE_ADDR_REG_ENABLE, BIT14);
    }
}

#endif /* INCLUDE_CPU_MAPPING */
/*******************************************************************************
* gtPci0SetRegionFeatures - Configure one of the 8 memory regions with the 
*                           features defined in gtPci.h.
*
* DESCRIPTION:
*       The GT implements a very sophisticated mechanism to control the memory 
*       accessed from the PCI via the slave. The mechanism consists of 8 memory 
*       regions in which the user can set various attributes to control the 
*       working mode with those regions such as read-only mode from the region, 
*       control the burst limit size for a single transaction between a PCI 
*       slave and the other interfaces, Data Swap Control, Access Protect to the
*       region etc.. All these attributes are defined in gtPci.h and can be 
*       combined using the | ( logial OR ) when using the following functions. 
*       The Possible attributes are (as defined in gtPci.h):
*
*        - DELAYED_READ_ENABLE  
*        - PREFETCH_ENABLE 
*        - PREFETCH_DISABLE 
*        - AGGRESIVE_PREFETCH
*        - etc.
*       
* INPUT:
*       region      - Select one of the 8 possible configurable regions.
*       features    - Features supported by this region (defined in gtPci.h).
*       baseAddress - The region's base address (32 bit address). 
*       regionSize  - The region's size.
*
* OUTPUT:
*       None.
*
* RETURN:
*       true on success, flase otherwise.
*
*******************************************************************************/
bool gtPci0SetRegionFeatures(PCI_ACCESS_REGIONS region,unsigned int features,
                             unsigned int baseAddress,unsigned int regionSize)
{
    unsigned int    dataForReg;

    if(region < REGION0 || REGION5 < region)
        return false;
    if(regionSize > 0) 
    {
        regionSize--;
        /* base Address bits are [31:12] */
        baseAddress &= 0xfffff000;
        dataForReg = baseAddress | (features & 0x00000fff);
        /* write to the Low Access Region register */
        GT_REG_WRITE((PCI_0_ACCESS_CONTROL_BASE_0_LOW +0x10*region),dataForReg);
        GT_REG_WRITE((PCI_0_ACCESS_CONTROL_BASE_0_HIGH+0x10*region),0);
        GT_REG_WRITE((PCI_0_ACCESS_CONTROL_SIZE_0     +0x10*region),regionSize);
        /* Enabling the region */
        GT_SET_REG_BITS((PCI_0_ACCESS_CONTROL_BASE_0_LOW + 0x10*region),
                                              PCI_ACCESS_CONTROL_WINDOW_ENABLE);
    }
    else  /* Size =< 0 */
    {
        gtPci0DisableAccessRegion(region);
    }
    return true;
}

/*******************************************************************************
* gtPci0SetRegionFeaturesDAC - Configure one of the 8 memory regions with the 
*                           features defined in gtPci.h.
*
* DESCRIPTION:
*       The GT implements a very sophisticated mechanism to control the memory 
*       accessed from the PCI via the slave. The mechanism consists of 8 memory 
*       regions in which the user can set various attributes to control the 
*       working mode with those regions such as read-only mode from the region, 
*       control the burst limit size for a single transaction between a PCI 
*       slave and the other interfaces, Data Swap Control, Access Protect to the
*       region etc.. All these attributes are defined in gtPci.h and can be 
*       combined using the | ( logial OR ) when using the following functions. 
*       The Possible attributes are (as defined in gtPci.h):
*
*        - DELAYED_READ_ENABLE  
*        - PREFETCH_ENABLE 
*        - PREFETCH_DISABLE 
*        - AGGRESIVE_PREFETCH
*        - etc.
*       
* INPUT:
*       region - Select one of the 8 possible configurable regions.
*       features - Features supported by this region (defined in gtPci.h).
*       baseAddressLow - The region's lower part of the base address 
*                        (bit[31::0]). 
*       baseAddressHigh - The region's higher part of the base address 
*                         (bit[63::32]).
*       regionSize - The region's size.
*
* OUTPUT:
*       None.
*
* RETURN:
*       true on success, flase otherwise.
*
*******************************************************************************/
bool gtPci0SetRegionFeaturesDAC(PCI_ACCESS_REGIONS region,unsigned int features, 
                                unsigned int baseAddressLow,
                                unsigned int baseAddressHigh,
                                unsigned int regionSize)
{
    unsigned int    dataForReg;

    if(region < REGION0 || REGION5 < region)
        return false;
    /* closing the region temporarly */
    gtPci0DisableAccessRegion(region);
    if(regionSize > 0) 
    {    
        regionSize--;
        /* base Address bits are [31:12] */
        baseAddressLow &= 0xfffff000;
        dataForReg = baseAddressLow | (features & 0x00000fff);
        /* write to the Low Access Region register */
        GT_REG_WRITE((PCI_0_ACCESS_CONTROL_BASE_0_LOW +0x10*region),dataForReg);
        GT_REG_WRITE((PCI_0_ACCESS_CONTROL_BASE_0_HIGH+0x10*region),
                                                               baseAddressHigh);
        GT_REG_WRITE((PCI_0_ACCESS_CONTROL_SIZE_0     +0x10*region),regionSize);
        /* Enabling the region */
        GT_SET_REG_BITS((PCI_0_ACCESS_CONTROL_BASE_0_LOW + 0x10*region),
                                              PCI_ACCESS_CONTROL_WINDOW_ENABLE);
    }
    else  /* Size =< 0 */
    {
        gtPci0DisableAccessRegion(region);
    }
    return true;
}

/*******************************************************************************
* gtPci0DisableAccessRegion - Close one of the 8 cunfigurable regions as 
*                             described in 'gtPci0SetRegionFeatures'.
*
* DESCRIPTION:
*       This function disables one of the 8 cunfigurable regions and sets its 
*       attributes to the resepective default values as defined in the GT data 
*       sheet.
*
* INPUT:
*       region - Select one of the 8 possible regions (defined in gtPci.h).
*
* OUTPUT:
*       None.
*
* RETURN:
*       None.
*
*******************************************************************************/
void gtPci0DisableAccessRegion(PCI_ACCESS_REGIONS region)
{
    /* Closing any access to this region */
    GT_RESET_REG_BITS(PCI_0_ACCESS_CONTROL_BASE_0_LOW + 0x10*region,
                                              PCI_ACCESS_CONTROL_WINDOW_ENABLE);
}

/*******************************************************************************
* gtPci0ArbiterEnable - Enable the PCI0 arbitration mechanism.
*
* DESCRIPTION:
*       The GT supports both external or internal arbiter configuration through
*       the PCI Arbiter Control register enable bit. The internal PCI arbiter 
*       can handle up to 6 external agents plus one internal agent. This 
*       function enables the internal arbiter by setting the enable bit to '1'.    
*      NOTE:
*       The internal arbiter pins are part of the GT Multi Purpose Pins (MPP). 
*       The user must configure these pins prior to enabling the arbiters. 
*
* INPUT:
*       None.
*
* OUTPUT:
*       None.
*
* RETURN:
*       None.
*
*******************************************************************************/
void gtPci0ArbiterEnable(void)
{
    unsigned int regData;

    GT_REG_READ(PCI_0_ARBITER_CONTROL,&regData);
    GT_REG_WRITE(PCI_0_ARBITER_CONTROL,regData | BIT31);
}

/*******************************************************************************
* gtPci0ArbiterDisable - Disable the PCI0 arbitration mechanism.
*
* DESCRIPTION:
*       This function disables the internal arbiter by setting the enable bit of
*       the PCI Arbiter Control registerto '0'.    
* 
* INPUT:
*       None.
*
* OUTPUT:
*       None.
*
* RETURN:
*       None.
*
*******************************************************************************/
void gtPci0ArbiterDisable(void)
{
    unsigned int regData;

    GT_REG_READ(PCI_0_ARBITER_CONTROL,&regData);
    GT_REG_WRITE(PCI_0_ARBITER_CONTROL,regData & 0x7fffffff);
}

/*******************************************************************************
* gtPci0ParkingDisable - Disable the parking on the last granted agent.
*
* DESCRIPTION:
*       The term “park” implies permission for the arbiter to assert GNT# 
*       (grant) to a selected agent when no agent is currently using or 
*       requesting the bus. A PCI arbiter must park on an agent to achieve 
*       proper PCI behavior. When an agent’s GNT# signal is asserted, it must 
*       drive the PCI bus. The GT’s arbiters let the user define the parking 
*       policy. To overcome problems that happen with some PCI devices that do 
*       not handle parking properly, the parking option can be disabled. This 
*       functions disables the parking on the last granted agent by setting the 
*       PCI Arbiter Control register's PD corresponding bits [21:14] to '1' . 
*      NOTE:
*       In addition to disabling parking to avoid issues with some problematic 
*       devices, it is required to disable parking on any unused request/grant 
*       pair. This is to avoid possible parking on non existent PCI masters.
*       For example, if only three external agents are connected to PCI0 
*       arbiter, then PD[6:4] must be set to 
*
* INPUT:
*       internalAgent  - Parking option for internal agent.
*       externalAgent0 - Parking option for external#0 agent.
*       externalAgent1 - Parking option for external#1 agent.
*       externalAgent2 - Parking option for external#2 agent.
*       externalAgent3 - Parking option for external#3 agent.
*       externalAgent4 - Parking option for external#4 agent.
*       externalAgent5 - Parking option for external#5 agent.
*
* OUTPUT:
*       None.
*
* RETURN:
*       None.
*
*******************************************************************************/
void gtPci0ParkingDisable(PCI_AGENT_PARK internalAgent,
                          PCI_AGENT_PARK externalAgent0,
                          PCI_AGENT_PARK externalAgent1,
                          PCI_AGENT_PARK externalAgent2,
                          PCI_AGENT_PARK externalAgent3,
                          PCI_AGENT_PARK externalAgent4,
                          PCI_AGENT_PARK externalAgent5)
{
    unsigned int writeData ,regData;

    GT_REG_READ(PCI_0_ARBITER_CONTROL,&regData);
    writeData = (internalAgent  << 14) | (externalAgent0 << 15) |     
                (externalAgent1 << 16) | (externalAgent2 << 17) |    
                (externalAgent3 << 18) | (externalAgent4 << 19) |    
                (externalAgent5 << 20);
    regData |= writeData;
    GT_REG_WRITE(PCI_0_ARBITER_CONTROL,writeData);
}

/*******************************************************************************
* gtPci0EnableBrokenAgentDetection - enables the broken agent detection 
*                                    mechanism.
*
* DESCRIPTION:
*       A master is said to be broken if it fails to respond to grant assertion
*       within a window specified in the input value: 'brokenValue'. This 
*       function enables the broken agent detection mechanism and set The 
*       maximum number of cycles that the arbiter waits for a PCI master to 
*       respond to its grant assertion.
*
* INPUT:
*       brokenValue - The maximum number of cycles that the arbiter waits for a 
*                     PCI master to respond to its grant assertion. If a PCI 
*                     master fails to assert FRAME* within this time, the PCI 
*                     arbiter aborts the transaction and performs a new 
*                     arbitration cycle and a maskable interrupt is generated.
*                     This value must be greater than 0 and lower than 16.
*
* OUTPUT:
*       None.
*
* RETURN:
*       True on success, false for an illegal 'brokenValue' value.
*
*******************************************************************************/
bool gtPci0EnableBrokenAgentDetection(unsigned char brokenValue)
{
    unsigned int data;
    unsigned int regData;

    /* brokenValue must be 4 bit , must be greater than 1 for conventional 
       PCI and greater than 5 for PCI-X.*/
    if (brokenValue <= 0x1 || 0xf < brokenValue || 
               (brokenValue <= 0x5 && (gtPci0GetBusMode() != CONVENTIONAL_PCI))) 
        return false; 
    data = brokenValue << 3;
    GT_REG_READ(PCI_0_ARBITER_CONTROL,&regData);
    regData = (regData & 0xffffff87) | data;
    GT_REG_WRITE(PCI_0_ARBITER_CONTROL,regData | BIT1);
    return true;
}

/*******************************************************************************
* gtPci0DisableBrokenAgentDetection - This function disable the Broken agent
*                                     detection mechanism.
*
* DESCRIPTION:
*       This function disable the broken agent detection mechanism described in 
*       'gtPci0EnableBrokenAgentDetection' function.
*      NOTE: This operation may cause a dead lock to the pci0 arbitration.
*
* INPUT:
*       None.
*
* OUTPUT:
*       None.
*
* RETURN:
*       None.
*
*******************************************************************************/
void gtPci0DisableBrokenAgentDetection(void)
{
    unsigned int regData;

    GT_REG_READ(PCI_0_ARBITER_CONTROL,&regData);
    regData &= 0xfffffffd;
    GT_REG_WRITE(PCI_0_ARBITER_CONTROL,regData);
}

/*******************************************************************************
* gtPci0P2PConfig - configurate the P2P bridge for PCI0
*
* DESCRIPTION:
*       This function configurate the P2P bridge for PCI0. It sets the 
*       boundaries of the buses connected directly or indirectly on the other 
*       side of the P2P bridge. 
*
* INPUT:
*       SecondBusLow - Secondary PCI Interface bus range lower boundary. 
*       SecondbusHigh - Secondary PCI Interface bus range upper boundary. 
*       busNum - The PCI bus number to which the PCI interface is connected.
*       devNum - The PCI interface’s device number.
*
* OUTPUT:
*       None.
*
* RETURN:
*       None.
*
*******************************************************************************/
void gtPci0P2PConfig(unsigned int SecondBusLow,unsigned int SecondBusHigh,
                     unsigned int busNum,unsigned int devNum)
{
    unsigned int regData;

    regData = (SecondBusLow & 0xff) | ((SecondBusHigh & 0xff) << 8) |
                              ((busNum & 0xff) << 16) | ((devNum & 0x1f) << 24);
    GT_REG_WRITE(PCI_0_P2P_CONFIG,regData);
}

/*******************************************************************************
* gtPci0GetBusMode - Returns the current bus operation mode (PCI/PCI-X).
*
* DESCRIPTION:
*       This function reads the PCI mode register and determine if the current 
*       bus operation mode is PCI-X or conventional PCI.
*
* INPUT:
*       None.
*
* OUTPUT:
*       None.
*
* RETURN:
*       CONVENTIONAL_PCI,PCI_X_66MHZ,PCI_X_100MHz or PCI_X_133MHz.
*
*******************************************************************************/
PCI_BUS_MODE gtPci0GetBusMode(void)
{
    unsigned int mode;

    mode = (GTREGREAD(PCI_0_MODE) & PCI_OPERATION_MODE);
    mode >>= 4;
    switch(mode)
    {
    case 0:
        return CONVENTIONAL_PCI;
        break;
    case 1:
        return PCI_X_66MHZ;
        break;
    case 2:
        return PCI_X_100MHZ;
        break;
    case 3:
        return PCI_X_133MHZ;
        break;
    }
    return CONVENTIONAL_PCI;
}

