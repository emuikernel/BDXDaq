/******************************************************************************
*              (c), Copyright, MOTOROLA  INC - ECC                 *
*                                                                             *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MOTOROLA , INC.        *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT *
* OF MOTOROLA  OR ANY THIRD PARTY. MOTOROLA  RESERVES THE RIGHT AT ITS SOLE DISCRETION*
* TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MOTOROLA .       *
* THIS CODE IS PROVIDED "AS IS". MOTOROLA  MAKES NO WARRANTIES, EXPRESS,*
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.  *
*******************************************************************************

*******************************************************************************
* gtMemory.c - Memory mappings and remapping functions 
*
* DESCRIPTION:
*       None.
*
* DEPENDENCIES:
*       None.
*
******************************************************************************/

/* includes */
#include "gtMemory.h"

#ifdef PMC280_DEBUG_UART_VX
#include "dbgprints.h"
#endif /* PMC280_DEBUG_UART_VX */

#ifdef PMC280_DUAL_CPU
#include "gtSmp.h"
#endif /* PMC280_DUAL_CPU */

/*******************************************************************************
* gtMemoryGetBankBaseAddress - Returns the base address of a memory bank.
*
* DESCRIPTION:
*       This function returns the base address of one of the SDRAM’s memory 
*       banks. There are 4 memory banks and each one represents one DIMM side. 
*
* INPUT:
*       bank - Selects one of the four banks as defined in gtMemory.h.
*
* OUTPUT:
*       None.
*
* RETURN:
*       32 bit Memory bank base address.
*
*******************************************************************************/
unsigned int gtMemoryGetBankBaseAddress(MEMORY_BANK bank)
{
    unsigned int base;
    unsigned int regOffset;

    switch (bank) 
    {
    case BANK0:
        regOffset = CS_0_BASE_ADDR;
        break;
    case BANK1:
        regOffset = CS_1_BASE_ADDR;
        break;
    case BANK2:
        regOffset = CS_2_BASE_ADDR;
        break;
    case BANK3:
        regOffset = CS_3_BASE_ADDR;
        break;
    default:
        regOffset = CS_0_BASE_ADDR;    /* default value */
        break;
    }
    GT_REG_READ(regOffset,&base); 
    base = base << 16;
    return base;
}

/*******************************************************************************
* gtMemoryGetDeviceBaseAddress - Returns the base address of a device.
*
* DESCRIPTION:
*       This function returns the base address of a device on the system. There
*       are 5 possible devices (0 - 4 and one boot device) as defined in 
*       gtMemory.h. Each of the device parameters is maped to one of the CS 
*       (Devices chip selects) base address register.       
*
* INPUT:
*       device - Selects one of the five devices as defined in gtMemory.h.
*
* OUTPUT:
*       None.
*
* RETURN:
*       32 bit Device base address.
*
*******************************************************************************/
unsigned int gtMemoryGetDeviceBaseAddress(GT_DEVICE device)
{
    unsigned int regBase;
    unsigned int regOffset;

    switch (device) 
    {
    case DEVICE0:
        regOffset = DEV_CS0_BASE_ADDR;
        break;
    case DEVICE1:
        regOffset = DEV_CS1_BASE_ADDR;
        break;
    case DEVICE2:
        regOffset = DEV_CS2_BASE_ADDR;
        break;
    case DEVICE3:
        regOffset = DEV_CS3_BASE_ADDR;
        break;
    case BOOT_DEVICE:
        regOffset = BOOTCS_BASE_ADDR;
        break;
    default:
        regOffset = DEV_CS0_BASE_ADDR; /* default value */
        break;
    }
    GT_REG_READ(regOffset, &regBase);
    regBase = regBase << 16;
    return regBase;
}

/*******************************************************************************
* gtMemoryGetBankSize - Returns the size of a memory bank.
*
* DESCRIPTION:
*       This function returns the size of memory bank as described in 
*       'gtMemoryGetBankBaseAddress' function.
*
* INPUT:
*       bank - Selects one of the four banks as defined in gtMemory.h.
*
* OUTPUT:
*       None.
*
* RETURN:
*       32 bit size memory bank size or 0 for a closed or non populated bank.
*
*******************************************************************************/
unsigned int gtMemoryGetBankSize(MEMORY_BANK bank)
{
    unsigned int sizeReg , size;
    MEMORY_WINDOW window;

    switch (bank) 
    {
    case BANK0:
        sizeReg = CS_0_SIZE;
        window = MEM_CS_0;
        break;
    case BANK1:
        sizeReg = CS_1_SIZE;
        window = MEM_CS_1;
        break;
    case BANK2:
        sizeReg = CS_2_SIZE;
        window = MEM_CS_2;
        break;
    case BANK3:
        sizeReg = CS_3_SIZE;
        window = MEM_CS_3;
        break;
    default:
        sizeReg = CS_0_SIZE;  /* default value */
        window = MEM_CS_0;
        break;
    }
    /* If the window is closed, a size of 0 is returned */
    if(gtMemoryGetMemWindowStatus(window) != MEM_WINDOW_ENABLED)
        return 0;
    GT_REG_READ(sizeReg,&size); 
    size = ((size << 16) | 0xffff) + 1;    
    return size;
}

/*******************************************************************************
* gtMemoryGetDeviceSize - Returns the size of a device memory space.
*
* DESCRIPTION:
*       This function returns the memory space size of a given device.
*
* INPUT:
*       device - Selects one of the five devices as defined in gtMemory.h.
*
* OUTPUT:
*       None.
*
* RETURN:
*       32 bit size of a device memory space.
*
*******************************************************************************/
unsigned int gtMemoryGetDeviceSize(GT_DEVICE device)
{
    unsigned int sizeReg , size;
    MEMORY_WINDOW window;

    switch (device) 
    {
        case DEVICE0:
            sizeReg = DEV_CS0_SIZE;
            window = MEM_DEVCS_0;
            break;
        case DEVICE1:
            sizeReg = DEV_CS1_SIZE;
            window = MEM_DEVCS_1;
            break;
        case DEVICE2:
            sizeReg = DEV_CS2_SIZE;
            window = MEM_DEVCS_2;
            break;
        case DEVICE3:
            sizeReg = DEV_CS3_SIZE;
            window = MEM_DEVCS_3;
            break;
        case BOOT_DEVICE:
            sizeReg = BOOTCS_SIZE;
            window = MEM_BOOT_CS;
            break;
        default:
            sizeReg = DEV_CS0_SIZE;  /* default value */
            window = MEM_DEVCS_0;
            break;
    }
    /* If the window is closed, a size of 0 is returned */
    if(gtMemoryGetMemWindowStatus(window) != MEM_WINDOW_ENABLED)
        return 0;
    GT_REG_READ(sizeReg,&size);
    size = ((size << 16) | 0xffff) + 1;    
    return size;
}

/*******************************************************************************
* gtMemoryGetDeviceWidth - Returns the width of a given device.
*
* DESCRIPTION:
*       The GT's device interface supports up to 32 Bit wide devices. A device 
*       can have a  1, 2, 4 or 8 Bytes data width. This function returns the 
*       width of a device as defined by the user or the operating system.
*
* INPUT:
*       device - Selects one of the five devices as defined in gtMemory.h.
*
* OUTPUT:
*       None.
*
* RETURN:
*       Device width in Bytes (1,2,4 or 8), 0 if error had occurred.
*
*******************************************************************************/
unsigned int gtMemoryGetDeviceWidth(GT_DEVICE device)
{
    unsigned int width;
    unsigned int regValue;

    GT_REG_READ(DEVICE_BANK0_PARAMETERS + device*4,&regValue); 
    width =  (regValue & (BIT20 | BIT21)) >> 20;
    return  (BIT0 << width);
}

/*******************************************************************************
* gtMemoryMapBank0 - Sets new bases and size for bank0 memory space.
*
* DESCRIPTION:
*       The CPU interface address decoding map consists of 21 address windows 
*       for the different devices (SCS[3:0] ,PCI_1 Mem 0/1/2/3 ...). Each window
*       can have a minimum of 1Mbytes of address space, and up to 4Gbyte space.
*       Each address window is defined by two registers - Low and High. The CPU 
*       address is compared with the values in the various CPU Low and High 
*       decode registers of each window until a match is found and the address 
*       is than targeted to that window.This function sets new base and size for 
*       bank0 (SCS0). It is the programmer`s responsibility to make sure that 
*       there are no conflicts with other memory spaces. When two memory spaces 
*       overlap, the GT’s behavior is not defined .If a bank needs to be closed,
*       set the ’bank0Size’ parameter size to 0x0, it gives the low decode 
*       address register max value (0xffff) and to the high decode address 
*       register min value (0x0).
*
* INPUT:
*       bank0Base - new base address for SDRAM bank 0.
*       bank0Size - new size for SDRAM bank 0.
*
* OUTPUT:
*       None.
*
* RETURN:
*       None.
*
*******************************************************************************/
void gtMemoryMapBank0(unsigned int bank0Base,unsigned int bank0Size) 
{
    unsigned int    newBase,newSize;      

    newBase = bank0Base >> 16;
    if(bank0Size > 0x0)
    {
        newSize = (bank0Size - 1) >> 16;
        /* writing the new values */
        GT_REG_WRITE(CS_0_BASE_ADDR,newBase);
        GT_REG_WRITE(CS_0_SIZE,newSize);
        gtMemoryEnableWindow(MEM_CS_0);
    }
    else
    {
        gtMemoryDisableWindow(MEM_CS_0);
        /* Closing the window from the PCI side also */
        GT_SET_REG_BITS(PCI_0_BASE_ADDR_REG_ENABLE, MEM_CS_0);
        GT_SET_REG_BITS(PCI_1_BASE_ADDR_REG_ENABLE, MEM_CS_0);
    }
}

/*******************************************************************************
* gtMemoryMapBank1 - Sets new bases and size for bank1 memory space.
*
* DESCRIPTION:
*       The CPU interface address decoding map consists of 21 address windows 
*       for the different devices (SCS[3:0] ,PCI_1 Mem 0/1/2/3 ...). Each window
*       can have a minimum of 1Mbytes of address space, and up to 4Gbyte space.
*       Each address window is defined by two registers - Low and High. The CPU 
*       address is compared with the values in the various CPU Low and High 
*       decode registers of each window until a match is found and the address 
*       is than targeted to that window.This function sets new base and size for 
*       bank1 (SCS1). It is the programmer`s responsibility to make sure that 
*       there are no conflicts with other memory spaces. When two memory spaces 
*       overlap, the GT’s behavior is not defined .If a bank needs to be closed,
*       set the ’bank1Size’ parameter size to 0x0, it gives the low decode 
*       address register max value (0xffff) and to the high decode address 
*       register min value (0x0).
*
* INPUT:
*       bank1Base - new base address for SDRAM bank 1.
*       bank1Size - new size for SDRAM bank 1.
*
* OUTPUT:
*       None.
*
* RETURN:
*       None.
*
*******************************************************************************/
void gtMemoryMapBank1(unsigned int bank1Base,unsigned int bank1Size) 
{
    unsigned int    newBase,newSize;      

    newBase = bank1Base >> 16;
    if(bank1Size > 0x0)
    {
        newSize = (bank1Size - 1) >> 16;
        /* writing the new values */
        GT_REG_WRITE(CS_1_BASE_ADDR,newBase);
        GT_REG_WRITE(CS_1_SIZE,newSize);
        gtMemoryEnableWindow(MEM_CS_1);
    }
    else
    {
        gtMemoryDisableWindow(MEM_CS_1);
        /* Closing the window from the PCI side also */
        GT_SET_REG_BITS(PCI_0_BASE_ADDR_REG_ENABLE, MEM_CS_1);
        GT_SET_REG_BITS(PCI_1_BASE_ADDR_REG_ENABLE, MEM_CS_1);
    }
}

/*******************************************************************************
* gtMemoryMapBank2 - Sets new bases and size for bank2 memory space.
*
* DESCRIPTION:
*       The CPU interface address decoding map consists of 21 address windows 
*       for the different devices (SCS[3:0] ,PCI_1 Mem 0/1/2/3 ...). Each window
*       can have a minimum of 1Mbytes of address space, and up to 4Gbyte space.
*       Each address window is defined by two registers - Low and High. The CPU 
*       address is compared with the values in the various CPU Low and High 
*       decode registers of each window until a match is found and the address 
*       is than targeted to that window.This function sets new base and size for 
*       bank2 (SCS2). It is the programmer`s responsibility to make sure that 
*       there are no conflicts with other memory spaces. When two memory spaces 
*       overlap, the GT’s behavior is not defined .If a bank needs to be closed,
*       set the ’bank2Size’ parameter size to 0x0, it gives the low decode 
*       address register max value (0xffff) and to the high decode address 
*       register min value (0x0).
*
* INPUT:
*       bank2Base - new base address for SDRAM bank 1.
*       bank2Size - new size for SDRAM bank 1.
*
* OUTPUT:
*       None.
*
* RETURN:
*       None.
*
*******************************************************************************/
void gtMemoryMapBank2(unsigned int bank2Base,unsigned int bank2Size) 
{
    unsigned int    newBase,newSize;  

    newBase = bank2Base >> 16;
    if(bank2Size > 0x0)
    {
        newSize = (bank2Size - 1) >> 16;
        /* writing the new values */
        GT_REG_WRITE(CS_2_BASE_ADDR,newBase);
        GT_REG_WRITE(CS_2_SIZE,newSize);
        gtMemoryEnableWindow(MEM_CS_2);
    }
    else
    {
        gtMemoryDisableWindow(MEM_CS_2);
        /* Closing the window from the PCI side also */
        GT_SET_REG_BITS(PCI_0_BASE_ADDR_REG_ENABLE, MEM_CS_2);
        GT_SET_REG_BITS(PCI_1_BASE_ADDR_REG_ENABLE, MEM_CS_2);
    }
}

/*******************************************************************************
* gtMemoryMapBank3 - Sets new bases and size for bank3 memory space.
*
* DESCRIPTION:
*       The CPU interface address decoding map consists of 21 address windows 
*       for the different devices (SCS[3:0] ,PCI_1 Mem 0/1/2/3 ...). Each window
*       can have a minimum of 1Mbytes of address space, and up to 4Gbyte space.
*       Each address window is defined by two registers - Low and High. The CPU 
*       address is compared with the values in the various CPU Low and High 
*       decode registers of each window until a match is found and the address 
*       is than targeted to that window.This function sets new base and size for 
*       bank3 memory space (SCS3). It is the programmer`s responsibility to make
*       sure that there are no conflicts with other memory spaces. When two 
*       memory spaces overlap, the GT’s behavior is not defined .If a bank needs
*       to be closed, set the ’bank3Size’ parameter size to 0x0, it gives the 
*       low decode address register max value (0xffff) and to the high decode 
*       address register min value (0x0).
*
* INPUT:
*       bank3Base - new base address for SDRAM bank 1.
*       bank3Size - new size for SDRAM bank 1.
*
* OUTPUT:
*       None.
*
* RETURN:
*       None.
*
*******************************************************************************/
void gtMemoryMapBank3(unsigned int bank3Base,unsigned int bank3Size) 
{
    unsigned int    newBase,newSize;

    newBase = bank3Base >> 16;
    if(bank3Size > 0x0)
    {
        newSize = (bank3Size - 1) >> 16;
        /* writing the new values */
        GT_REG_WRITE(CS_3_BASE_ADDR,newBase);
        GT_REG_WRITE(CS_3_SIZE,newSize);
        gtMemoryEnableWindow(MEM_CS_3);
    }
    else
    {
        gtMemoryDisableWindow(MEM_CS_3);
        /* Closing the window from the PCI side also */
        GT_SET_REG_BITS(PCI_0_BASE_ADDR_REG_ENABLE, MEM_CS_3);
        GT_SET_REG_BITS(PCI_1_BASE_ADDR_REG_ENABLE, MEM_CS_3);
    }
}

/*******************************************************************************
* gtMemoryMapDevice0Space - Sets new bases and size for device 0 memory space.
*
* DESCRIPTION:
*       The CPU interface address decoding map consists of 21 address windows 
*       for the different devices (SCS[3:0] ,PCI_1 Mem 0/1/2/3 ...). Each window
*       can have a minimum of 1Mbytes of address space, and up to 4Gbyte space.
*       Each address window is defined by two registers - Low and High. The CPU 
*       address is compared with the values in the various CPU Low and High 
*       decode registers of each window until a match is found and the address 
*       is than targeted to that window.This function sets new base and size for 
*       device 0 memory space(CS0). It is the programmer`s responsibility to 
*       make sure that there are no conflicts with other memory spaces. When two
*       memory spaces overlap, the GT’s behavior is not defined .If the memory 
*       space needs to be closed, set the 'device0Size' parameter size to 0x0, 
*       it gives the low decode address register max value (0xffff) and to the 
*       high decode address register min value (0x0).
*
* INPUT:
*       device0Base - new base address for device 0 memory space.
*       device0Size - new size for device 0 memory space.
*
* OUTPUT:
*       None.
*
* RETURN:
*       None.
*
*******************************************************************************/
void gtMemoryMapDevice0Space(unsigned int device0Base,unsigned int device0Size)
{
    unsigned int    newBase,newSize;  

    newBase = device0Base >> 16;
    if(device0Size > 0x0)
    {
        newSize = (device0Size - 1) >> 16;
        /* writing the new values */
        GT_REG_WRITE(DEV_CS0_BASE_ADDR,newBase);
        GT_REG_WRITE(DEV_CS0_SIZE,newSize);
        gtMemoryEnableWindow(MEM_DEVCS_0);
    }
    else
    {
        gtMemoryDisableWindow(MEM_DEVCS_0);
        /* Closing the window from the PCI side also */
        GT_SET_REG_BITS(PCI_0_BASE_ADDR_REG_ENABLE, MEM_DEVCS_0);
        GT_SET_REG_BITS(PCI_1_BASE_ADDR_REG_ENABLE, MEM_DEVCS_0);
    }
}

/*******************************************************************************
* gtMemoryMapDevice1Space - Sets new bases and size for device 1 memory space.
*
* DESCRIPTION:
*       The CPU interface address decoding map consists of 21 address windows 
*       for the different devices (SCS[3:0] ,PCI_1 Mem 0/1/2/3 ...). Each window
*       can have a minimum of 1Mbytes of address space, and up to 4Gbyte space
*       Each address window is defined by two registers - Low and High. The CPU 
*       address is compared with the values in the various CPU Low and High 
*       decode registers of each window until a match is found and the address 
*       is than targeted to that window.This function sets new base and size for 
*       device 1 memory space(CS1). It is the programmer`s responsibility to 
*       make sure that there are no conflicts with other memory spaces. When two
*       memory spaces overlap, the GT’s behavior is not defined .If the memory 
*       space needs to be closed, set the 'device1Size' parameter size to 0x0, 
*       it gives the low decode address register max value (0xffff) and to the 
*       high decode address register min value (0x0).
*
* INPUT:
*       device1Base - new base address for device 1 memory space.
*       device1Size - new size for device 1 memory space.
*
* OUTPUT:
*       None.
*
* RETURN:
*       None.
*
*******************************************************************************/
void gtMemoryMapDevice1Space(unsigned int device1Base,unsigned int device1Size) 
{
    unsigned int    newBase,newSize;  
    
    newBase = device1Base >> 16;
    if(device1Size > 0x0)
    {
        newSize = (device1Size - 1) >> 16;
        /* writing the new values */
        GT_REG_WRITE(DEV_CS1_BASE_ADDR,newBase);
        GT_REG_WRITE(DEV_CS1_SIZE,newSize);
        gtMemoryEnableWindow(MEM_DEVCS_1);
    }
    else
    {
        gtMemoryDisableWindow(MEM_DEVCS_1);
        /* Closing the window from the PCI side also */
        GT_SET_REG_BITS(PCI_0_BASE_ADDR_REG_ENABLE, MEM_DEVCS_1);
        GT_SET_REG_BITS(PCI_1_BASE_ADDR_REG_ENABLE, MEM_DEVCS_1);
    }
}

/*******************************************************************************
* gtMemoryMapDevice2Space - Sets new bases and size for device 2 memory space.
*
* DESCRIPTION:
*       The CPU interface address decoding map consists of 21 address windows 
*       for the different devices (SCS[3:0] ,PCI_1 Mem 0/1/2/3 ...). Each window
*       can have a minimum of 1Mbytes of address space, and up to 4Gbyte space
*       Each address window is defined by two registers - Low and High. The CPU 
*       address is compared with the values in the various CPU Low and High 
*       decode registers of each window until a match is found and the address 
*       is than targeted to that window.This function sets new base and size for 
*       device 2 memory space(CS2). It is the programmer`s responsibility to 
*       make sure that there are no conflicts with other memory spaces. When two
*       memory spaces overlap, the GT’s behavior is not defined .If the memory 
*       space needs to be closed, set the 'device2Size' parameter size to 0x0, 
*       it gives the low decode address register max value (0xffff) and to the 
*       high decode address register min value (0x0).
*
* INPUT:
*       device2Base - new base address for device 2 memory space.
*       device2Size - new size for device 2 memory space.
*
* OUTPUT:
*       None.
*
* RETURN:
*       None.
*
*******************************************************************************/
void gtMemoryMapDevice2Space(unsigned int device2Base,unsigned int device2Size)
{
    unsigned int    newBase,newSize;  

    newBase = device2Base >> 16;
    if(device2Size > 0x0)
    {
        newSize = (device2Size - 1) >> 16;
        /* writing the new values */
        GT_REG_WRITE(DEV_CS2_BASE_ADDR,newBase);
        GT_REG_WRITE(DEV_CS2_SIZE,newSize);
        gtMemoryEnableWindow(MEM_DEVCS_2);
    }
    else
    {
        gtMemoryDisableWindow(MEM_DEVCS_2);
        /* Closing the window from the PCI side also */
        GT_SET_REG_BITS(PCI_0_BASE_ADDR_REG_ENABLE, MEM_DEVCS_2);
        GT_SET_REG_BITS(PCI_1_BASE_ADDR_REG_ENABLE, MEM_DEVCS_2);
    }
}

/*******************************************************************************
* gtMemoryMapDevice3Space - Sets new bases and size for device 3 memory space.
*
* DESCRIPTION:
*       The CPU interface address decoding map consists of 21 address windows 
*       for the different devices (SCS[3:0] ,PCI_1 Mem 0/1/2/3 ...). Each window
*       can have a minimum of 1Mbytes of address space, and up to 4Gbyte space.  
*       Each address window is defined by two registers - Low and High. The CPU 
*       address is compared with the values in the various CPU Low and High 
*       decode registers of each window until a match is found and the address 
*       is than targeted to that window.This function sets new base and size for 
*       device 3 memory space(CS3). It is the programmer`s responsibility to 
*       make sure that there are no conflicts with other memory spaces. When two
*       memory spaces overlap, the GT’s behavior is not defined .If the memory 
*       space needs to be closed, set the 'device3Size' parameter size to 0x0, 
*       it gives the low decode address register max value (0xffff) and to the 
*       high decode address register min value (0x0).
*
* INPUT:
*       device3Base - new base address for device 3 memory space.
*       device3Size - new size for device 3 memory space.
*
* OUTPUT:
*       None.
*
* RETURN:
*       None.
*
*******************************************************************************/
void gtMemoryMapDevice3Space(unsigned int device3Base,unsigned int device3Size)
{
    unsigned int newBase,newSize;

    newBase = device3Base >> 16;
    if(device3Size > 0x0)
    {
        newSize = (device3Size - 1) >> 16;
        /* writing the new values */
        GT_REG_WRITE(DEV_CS3_BASE_ADDR,newBase);
        GT_REG_WRITE(DEV_CS3_SIZE,newSize);
        gtMemoryEnableWindow(MEM_DEVCS_3);
    }
    else
    {
        gtMemoryDisableWindow(MEM_DEVCS_0);
        /* Closing the window from the PCI side also */
        GT_SET_REG_BITS(PCI_0_BASE_ADDR_REG_ENABLE, MEM_DEVCS_3);
        GT_SET_REG_BITS(PCI_1_BASE_ADDR_REG_ENABLE, MEM_DEVCS_3);
    }
}

/*******************************************************************************
* gtMemoryMapDeviceBootSpace - Sets new bases and size for boot device memory 
*                              space.
*
* DESCRIPTION:
*       The CPU interface address decoding map consists of 21 address windows 
*       for the different devices (SCS[3:0] ,PCI_1 Mem 0/1/2/3 ...). Each window
*       can have a minimum of 1Mbytes of address space, and up to 4Gbyte space.  
*       Each address window is defined by two registers - Low and High. The CPU 
*       address is compared with the values in the various CPU Low and High 
*       decode registers of each window until a match is found and the address 
*       is than targeted to that window.This function sets new base and size for 
*       boot device memory space(BOOTCS). It is the programmer`s responsibility
*       to make sure that there are no conflicts with other memory spaces. When 
*       two memory spaces overlap, the GT’s behavior is not defined .If the 
*       memory space needs to be closed, set the 'bootDeviceSize' parameter size
*       to 0x0, it gives the low decode address register max value (0xffff) and 
*       to the high decode address register min value (0x0).
*
* INPUT:
*       bootDeviceBase - new base address for boot device memory space.
*       bootDeviceSize - new size for boot device memory space.
*
* OUTPUT:
*       None.
*
* RETURN:
*       None.
*
*******************************************************************************/
void gtMemoryMapDeviceBootSpace(unsigned int bootDeviceBase,
                                unsigned int bootDeviceSize)
{
    unsigned int    newBase,newSize;

    newBase = bootDeviceBase >> 16;
    if(bootDeviceSize > 0x0)
    {
        newSize = (bootDeviceSize - 1) >> 16;
        /* writing the new values */
        GT_REG_WRITE(BOOTCS_BASE_ADDR,newBase);
        GT_REG_WRITE(BOOTCS_SIZE,newSize);
        gtMemoryEnableWindow(MEM_BOOT_CS);
    }
    else
    {
        gtMemoryDisableWindow(MEM_BOOT_CS);
        /* Closing the window from the PCI side also */
        GT_SET_REG_BITS(PCI_0_BASE_ADDR_REG_ENABLE, MEM_BOOT_CS);
        GT_SET_REG_BITS(PCI_1_BASE_ADDR_REG_ENABLE, MEM_BOOT_CS);
    }
}

/*******************************************************************************
* gtMemoryMapInternalRegistersSpace - Sets new base address for the internal 
*                                     registers memory space.
*
* DESCRIPTION:
*       This function set new base address for the internal register’s memory
*       space (the size is fixed and cannot be modified). The function does not 
*       handle overlapping with other memory spaces, it is the programer's 
*       responsibility to ensure that overlapping does not occur. 
*       When two memory spaces overlap, the GT’s behavior is not defined.
*
* INPUT:
*       internalRegBase - new base address for the internal register’s memory 
*                         space.
*
* OUTPUT:
*       None.
*
* RETURN:
*       None.
*
*******************************************************************************/
void gtMemoryMapInternalRegistersSpace(unsigned int internalRegBase)
{
    unsigned int currentValue;
    unsigned int internalValue = internalRegBase;

    internalRegBase = (internalRegBase >> 16);
    GT_REG_READ(INTERNAL_SPACE_BASE_ADDR,&currentValue);
    internalRegBase = (currentValue & 0xff000000) | internalRegBase;
    GT_REG_WRITE(INTERNAL_SPACE_BASE_ADDR,internalRegBase);
    /* initializing also the global variable 'internalRegBaseAddr' */
    gtInternalRegBaseAddr = internalValue;
}

/*******************************************************************************
* gtMemoryGetInternalRegistersSpace - Returns the internal registers Base 
*                                     address.
*
* DESCRIPTION:
*       This function returns the base address of  the internal register’s 
*       memory space .
*
* INPUT:
*       None.
*
* OUTPUT:
*       None.
*
* RETURN:
*       32 bit base address of the internal register’s memory space.
*
*******************************************************************************/
unsigned int gtMemoryGetInternalRegistersSpace(void)
{
    unsigned int currentValue = 0;

    GT_REG_READ(INTERNAL_SPACE_BASE_ADDR,&currentValue);
    return ((currentValue & 0x000fffff) << 16);
}

/*******************************************************************************
* gtMemoryGetInternalSramBaseAddr - Returns the integrated SRAM base address.
*
* DESCRIPTION:
*       The Atlantis incorporate integrated 2MB SRAM for general use. This 
*       funcnion return the SRAM's base address.
*
* INPUT:
*       None.
*
* OUTPUT:
*       None.
*
* RETURN:
*       32 bit SRAM's base address.
*
*******************************************************************************/
unsigned int gtMemoryGetInternalSramBaseAddr(void)
{
    return ((GTREGREAD(INTEGRATED_SRAM_BASE_ADDR) & 0xfffff) << 16);
}

/*******************************************************************************
* gtMemorySetInternalSramBaseAddr - Set the integrated SRAM base address.
*
* DESCRIPTION:
*       The Atlantis incorporate integrated 2MB SRAM for general use. This 
*       function sets a new base address to the SRAM .
*
* INPUT:
*       sramBaseAddress - The SRAM's base address.
*
* OUTPUT:
*       None.
*
* RETURN:
*       None.
*
*******************************************************************************/
void gtMemorySetInternalSramBaseAddr(unsigned int sramBaseAddress)
{
#ifdef PMC280_DEBUG_UART_VX
#ifdef PMC280_DUAL_CPU
    if(frcWhoAmI())
    {
        dbg_printf1("CPU1: gtMemorySetInternalSramBaseAddr = 0x%08x\n",
                    sramBaseAddress);
    }
    else
    {
        dbg_printf0("CPU0: gtMemorySetInternalSramBaseAddr = 0x%08x\n",
                    sramBaseAddress);
    }
#else
    dbg_printf0("gtMemorySetInternalSramBaseAddr = 0x%08x\n",
               sramBaseAddress);
#endif /* PMC280_DUAL_CPU */
#endif /* PMC280_DEBUG_UART_VX */
    GT_REG_WRITE(INTEGRATED_SRAM_BASE_ADDR,sramBaseAddress >> 16); 
}

/*******************************************************************************
* gtMemorySetProtectRegion - Set protection mode for one of the 8 regions.
*
* DESCRIPTION:
*       The CPU interface supports configurable access protection. This includes
*       up to eight address ranges defined to a different protection type : 
*       whether the address range is cacheable or not, whether it is writable or
*       not , and whether it is accessible or not. A Low and High registers 
*       define each window while the minimum address range of each window is 
*       1Mbyte. An address driven by the CPU, in addition to the address 
*       decoding and remapping process, is compared against the eight Access 
*       Protection Low/High registers , if an address matches one of the windows
*       , the GT device checks the transaction type against the protection bits 
*       defined in CPU Access Protection register, to determine if the access is
*       allowed. This function set a protection mode to one of the 8 possible 
*       regions.
*      NOTE:
*       The CPU address windows are restricted to a size of  2 power n and the 
*       start address must be aligned to the window size. For example, if using
*       a 16 MB window, the start address bits [23:0] must be 0.The GT's 
*       internal registers space is not protected, even if the access protection
*       windows contain this space.
*
* INPUT:
*       region - selects which region to be configured. The values defined in 
*                gtMemory.h:
*
*                 - MEM_REGION0
*                 - MEM_REGION1
*                 - etc.
*
*       memAccess - Allows or forbids access (read or write ) to the region. The
*                   values defined in gtMemory.h:
*
*                    - MEM_ACCESS_ALLOWED 
*                    - MEM_ACCESS_FORBIDEN
*
*       memWrite - CPU write protection to the region. The values defined in 
*                  gtMemory.h: 
*
*                   - MEM_WRITE_ALLOWED
*                   - MEM_WRITE_FORBIDEN
*
*       cacheProtection - Defines whether caching the region is allowed or not. 
*                         The values defined in gtMemory.h:
*
*                          - MEM_CACHE_ALLOWED
*                          - MEM_CACHE_FORBIDEN
*
*       baseAddress - Determines the region's base Address.
*       regionSize - Determines the region's size.
*
* OUTPUT:
*       None.
*
* RETURN:
*       None.
*
*******************************************************************************/
void gtMemorySetProtectRegion(MEMORY_PROTECT_WINDOW window,
                              MEMORY_ACCESS memAccess,
                              MEMORY_ACCESS_WRITE memWrite,
                              MEMORY_CACHE_PROTECT cacheProtection,
                              unsigned int baseAddress, 
                              unsigned int size)
{
    unsigned int   dataForReg ;

    baseAddress =  ((baseAddress >> 16) & 0xfffff);
    dataForReg = baseAddress | ((memAccess << 20) & BIT20) | 
         ((memWrite << 21) & BIT21) | ((cacheProtection << 22) & BIT22) | BIT31;
    GT_REG_WRITE(CPU_PROTECT_WINDOW_0_BASE_ADDR + 0x10*window,dataForReg);
    size = ((size - 1) >> 16) & 0xffff;
    GT_REG_WRITE((CPU_PROTECT_WINDOW_0_SIZE + 0x10*window),size);
}

/*******************************************************************************
* gtMemoryDisableProtectRegion - Disable a protected window.
*
* DESCRIPTION:
*       This function disable a protected window set by 
*       'gtMemorySetProtectRegion' function.
*
* INPUT:
*       window - one of the 4 windows ( defined in gtMemory.h ).
*
* OUTPUT:
*       None.
*
* RETURN:
*       None.
*
*******************************************************************************/
void gtMemoryDisableProtectRegion(MEMORY_PROTECT_WINDOW window)
{
    GT_RESET_REG_BITS(CPU_PROTECT_WINDOW_0_BASE_ADDR + 0x10*window,BIT31);
}

/*******************************************************************************
* gtMemoryDisableWindow - Disable a memory space by the disable bit.
*
* DESCRIPTION:
*       This function disables one of the 21 availiable windows dedicated for 
*       the CPU decoding mechanism. Its possible to combine several windows with
*       the OR command.
*
* INPUT:
*       window - One or more of the memory windows (defined in gtMemory.h).
*
* OUTPUT:
*       None.
*
* RETURN:
*       None.
*
*******************************************************************************/
void gtMemoryDisableWindow(MEMORY_WINDOW window)
{
    GT_SET_REG_BITS(BASE_ADDR_ENABLE,window);
}
/*******************************************************************************
* gtMemoryEnableWindow - Enable a memory space that was disabled by 
*                       'gtMemoryDisableWindow'.
*
* DESCRIPTION:
*       This function enables one of the 21 availiable windows dedicated for the
*       CPU decoding mechanism. Its possible to combine several windows with the 
*       OR command.                                      
*
* INPUT:
*       window - One or more of the memory windows (defined in gtMemory.h).
*
* OUTPUT:
*       None.
*
* RETURN:
*       None.
*
*******************************************************************************/
void gtMemoryEnableWindow(MEMORY_WINDOW window)
{
    GT_RESET_REG_BITS(BASE_ADDR_ENABLE,window);
}

/*******************************************************************************
* gtMemoryGetMemWindowStatus - This function check whether the memory window is 
*                           closed or not.
*
* DESCRIPTION:
*       This function checks if the given memory window is closed .                                      
*
* INPUT:
*       window - One or more of the memory windows (defined in gtMemory.h).
*
* OUTPUT:
*       None.
*
* RETURN:
*       True for a closed window, false otherwise .
*
*******************************************************************************/
MEMORY_WINDOW_STATUS gtMemoryGetMemWindowStatus(MEMORY_WINDOW window)
{
    if(GTREGREAD(BASE_ADDR_ENABLE) & window)
        return  MEM_WINDOW_DISABLED;
    return MEM_WINDOW_ENABLED;
}
/*******************************************************************************
* gtMemorySetPciRemapValue - Set a remap value to a PCI memory space target.
*
* DESCRIPTION:
*       In addition to the address decoding mechanism, the CPU has an address 
*       remapping mechanism to be used by every PCI decoding window. Each PCI 
*       window can be remaped to a desired address target according to the remap
*       value within the remap register. The address remapping is useful when a 
*       CPU address range must be reallocated to a different location on the 
*       PCI bus. Also, it enables CPU access to a PCI agent located above the 
*       4Gbyte space. On system boot, each of the PCI memory spaces is maped to 
*       a defualt value (see CPU interface section in the GT spec for the 
*       default values). The remap mechanism does not always produce the deisred
*       address on the PCI bus because of the remap mechanism way of working 
*       (to fully understand why, please see the 'Address Remapping' section in 
*       the GT's spec). Therefor, this function sets a desired remap value to 
*       one of the PCI memory windows and return the effective address that 
*       should be used when exiting the PCI memory window. You should ALWAYS use
*       the returned value by this function when remapping a PCI window and 
*       exiting it. If for example the base address of PCI0 memory 0 is 
*       0x90000000, the size is 0x03ffffff and the remap value is 0x11000000, 
*       the function will return the value of 0x91000000 that MUST
*       be used to exit this memory window in order to achive the deisred 
*       remapping. Note that in some cases the fnuction will return the same 
*       value as the window base address.
*
* INPUT:
*       memoryWindow   - One of the PCI memory windows as defined in gtMemory.h
*       remapValueLow  - The low remap value.
*       remapValueHigh - The high remap value.
* OUTPUT:
*       None.
*
* RETURN:
*       The effective base address to exit the PCI, or 0xffffffff if one of the
*       parameters is erroneous, the effective base address is higher the top
*       decode value or the window is closed.
*
*******************************************************************************/
unsigned int gtMemorySetPciRemapValue(PCI_MEM_WINDOW memoryWindow,
                                      unsigned int remapValueHigh,
                                      unsigned int remapValueLow)
{
    unsigned int pciMemWindowBaseAddrReg = 0,baseAddrValue = 0;
    unsigned int pciMemWindowSizeReg = 0,windowSizeValue = 0;
    unsigned int effectiveBaseAddress,remapRegLow,remapRegHigh;

    /* Initializing the base and size variables of the PCI 
       memory windows */
    switch(memoryWindow)
    {
    case PCI_0_IO:
        pciMemWindowBaseAddrReg = PCI_0_IO_BASE_ADDR; 
        pciMemWindowSizeReg = PCI_0_IO_SIZE;
        remapRegLow = PCI_0_IO_ADDR_REMAP;
        remapRegHigh = PCI_0_IO_ADDR_REMAP;
        break;
    case PCI_0_MEM0:
        pciMemWindowBaseAddrReg = PCI_0_MEMORY0_BASE_ADDR; 
        pciMemWindowSizeReg = PCI_0_MEMORY0_SIZE;
        remapRegLow = PCI_0_MEMORY0_LOW_ADDR_REMAP;
        remapRegHigh = PCI_0_MEMORY0_HIGH_ADDR_REMAP;
        break;
    case PCI_0_MEM1:
        pciMemWindowBaseAddrReg = PCI_0_MEMORY1_BASE_ADDR; 
        pciMemWindowSizeReg = PCI_0_MEMORY1_SIZE;
        remapRegLow = PCI_0_MEMORY1_LOW_ADDR_REMAP;
        remapRegHigh = PCI_0_MEMORY1_HIGH_ADDR_REMAP;
        break;
    case PCI_0_MEM2:
        pciMemWindowBaseAddrReg = PCI_0_MEMORY2_BASE_ADDR; 
        pciMemWindowSizeReg = PCI_0_MEMORY2_SIZE;
        remapRegLow = PCI_0_MEMORY2_LOW_ADDR_REMAP;
        remapRegHigh = PCI_0_MEMORY2_HIGH_ADDR_REMAP;
        break;
    case PCI_0_MEM3:
        pciMemWindowBaseAddrReg = PCI_0_MEMORY3_BASE_ADDR; 
        pciMemWindowSizeReg = PCI_0_MEMORY3_SIZE;
        remapRegLow = PCI_0_MEMORY3_LOW_ADDR_REMAP;
        remapRegHigh = PCI_0_MEMORY3_HIGH_ADDR_REMAP;
        break;
    default:
        /* Retrun an invalid effective base address */
        return effectiveBaseAddress = 0xffffffff;
    }
    /* Writing the remap value to the remap regisers */
    GT_REG_WRITE(remapRegHigh,remapValueHigh);
    GT_REG_WRITE(remapRegLow,remapValueLow >> 16);
    /* Reading the values from the base address and size registers */
    baseAddrValue = GTREGREAD(pciMemWindowBaseAddrReg) & 0xfffff;
    windowSizeValue = GTREGREAD(pciMemWindowSizeReg) & 0xffff;
    effectiveBaseAddress = baseAddrValue << 16;
    /* The effective base address will be combined from the chopped (if any)
       remap value and the window's base address */
    effectiveBaseAddress |= (((windowSizeValue<<16) | 0xffff) & remapValueLow);
    /* If the effectiveBaseAddress exceed the window boundaries */
    if (effectiveBaseAddress > ((baseAddrValue<<16) + ((windowSizeValue<<16) | 
                                                                       0xffff)))
        return effectiveBaseAddress = 0xffffffff;
    return effectiveBaseAddress;
}

/*******************************************************************************
* gtMemoryGetDeviceParam - Extract the device parameters from the device bank
*                          parameters register.
*
* DESCRIPTION:
*       To allow interfacing with very slow devices and fast synchronous SRAMs,
*       each device can be programed to different timing parameters. Each bank 
*       has its own parameters register. Bank width can be programmed to 8, 16,
*       or 32-bits. Bank timing parameters can be programmed to support 
*       different device types (e.g. Sync Burst SRAM, Flash , ROM, I/O 
*       Controllers). The GT allows you to set timing parameters and width for 
*       each device through parameters register .
*       This function extracts the parameters described from the Device Bank 
*       parameters register and fills the given 'deviceParam' (defined in 
*       gtMemory.h) structure with the read data.
*
* INPUT:
*       deviceParam -  pointer to a structure GT_DEVICE_PARAM (defined in 
*                      gtMemory.h).For details about each structure field please
*                      see the device timing parameter section in the GT 
*                      datasheet.
*       deviceNum  -   Select on of the five device banks (defined in 
*                      gtMemory.h) :
*         
*                       - DEVICE0
*                       - DEVICE1
*                       - DEVICE2
*                       - etc.
*
* OUTPUT:
*       None.
*
* RETURN:
*       false if one of the parameters is erroneous,true otherwise.
*
*******************************************************************************/
bool gtMemoryGetDeviceParam(GT_DEVICE_PARAM *deviceParam, GT_DEVICE deviceNum)
{
    unsigned int valueOfReg;
    unsigned int calcData;

    if(deviceNum > 4)
        return false;
    GT_REG_READ(DEVICE_BANK0_PARAMETERS + 4 * deviceNum, &valueOfReg);
    calcData = (0x7 & valueOfReg) + ((BIT22 & valueOfReg) >> 19);
    deviceParam -> turnOff = calcData;          /* Turn Off */

    calcData = ((0x78 & valueOfReg) >> 3) + ((BIT23 & valueOfReg) >> 19);
    deviceParam -> acc2First = calcData;        /* Access To First */

    calcData = ((0x780 & valueOfReg) >> 7) + ((BIT24 & valueOfReg) >> 20);
    deviceParam -> acc2Next = calcData;         /* Access To Next */
                                                
    calcData = ((0x3800 & valueOfReg) >> 11) + ((BIT25 & valueOfReg) >> 22);
    deviceParam -> ale2Wr = calcData;           /* Ale To Write */
                                                
    calcData = ((0x1c000 & valueOfReg) >> 14) + 
                                               ((BIT26 & valueOfReg) >> 23);
    deviceParam -> wrLow = calcData;            /* Write Active */

    calcData = ((0xe0000 & valueOfReg) >> 17) + 
                                               ((BIT27 & valueOfReg) >> 24);
    deviceParam -> wrHigh = calcData;           /* Write High */

    calcData = ((0x300000 & valueOfReg) >> 20);
    deviceParam->deviceWidth = (BIT0 << calcData); /* In bytes */
    calcData = ((0x30000000 & valueOfReg) >> 28);
    deviceParam -> badrSkew = calcData;        /* Cycles gap between BAdr 
                                                 toggle to read data sample.*/
    calcData = ((0x40000000 & valueOfReg) >> 30);
    deviceParam -> DPEn = calcData;           /*  Data Parity enable  */   
    return true;
}

/*******************************************************************************
* gtMemorySetDeviceParam - Set new parameters for a device.
*
*
* DESCRIPTION:
*       To allow interfacing with very slow devices and fast synchronous SRAMs,
*       each device can be programed to different timing parameters. Each bank 
*       has its own parameters register. Bank width can be programmed to 8, 16,
*       or 32-bits. Bank timing parameters can be programmed to support 
*       different device types (e.g. Sync Burst SRAM, Flash , ROM, I/O 
*       Controllers). The GT allows you to set timing parameters and width for 
*       each device through parameters register. This function set new 
*       parameters to a device Bank from the delivered structure 'deviceParam' 
*       (defined in gtMemory.h). The structure must be initialized with data 
*       prior to the use of these function.
*
* INPUT:
*       deviceParam -  pointer to a structure GT_DEVICE_PARAM (defined in 
*                      gtMemory.h).For details about each structure field please
*                      see the device timing parameter section in the GT 
*                      datasheet.
*       deviceNum  -   Select on of the five device banks (defined in 
*                      gtMemory.h) :
*         
*                       - DEVICE0
*                       - DEVICE1
*                       - DEVICE2
*                       - etc.
*
* OUTPUT:
*       None.
*
* RETURN:
*       false if one of the parameters is erroneous,true otherwise.
*
*******************************************************************************/
bool gtMemorySetDeviceParam(GT_DEVICE_PARAM *deviceParam, GT_DEVICE deviceNum)
{
    unsigned int valueForReg;

    if((deviceParam->turnOff > 0x7) || (deviceParam->acc2First > 0xf) || 
       (deviceParam->acc2Next > 0xf) || (deviceParam->ale2Wr > 0x7) ||
       (deviceParam->wrLow > 0x7) || (deviceParam->wrHigh > 0x7) || 
       (deviceParam->badrSkew > 0x2) || (deviceParam->DPEn > 0x1))
    {
        return false;
    }
    valueForReg = (((deviceParam -> turnOff) & 0x7) |
                   (((deviceParam -> turnOff) & 0x8) << 19) |
                   (((deviceParam -> acc2First) & 0xf) << 3) |
                   (((deviceParam -> acc2First) & 0x10) << 19) |
                   (((deviceParam -> acc2Next) & 0xf) << 7) |
                   (((deviceParam -> acc2Next) & 0x10) << 20) |
                   (((deviceParam -> ale2Wr) & 0x7) << 11) |
                   (((deviceParam -> ale2Wr) & 0xf) << 22) |
                   (((deviceParam -> wrLow) & 0x7) << 14) |
                   (((deviceParam -> wrLow) & 0xf) << 23) |
                   (((deviceParam -> wrHigh) & 0x7) << 17) |
                   (((deviceParam -> wrHigh) & 0xf) << 24) |
                   (((deviceParam -> badrSkew) & 0x3) << 28)|  
                   (((deviceParam -> DPEn) & 0x1) << 30));
    
    /* insert the device width: */
    switch(deviceParam->deviceWidth)
    {
    case 1:
        valueForReg = valueForReg | _8BIT;
        break;
    case 2:
        valueForReg = valueForReg | _16BIT;
        break;
    case 4:
        valueForReg = valueForReg | _32BIT;
        break;
    default:
        valueForReg = valueForReg | _8BIT;
        break;
    }
    GT_REG_WRITE(DEVICE_BANK0_PARAMETERS + 4 * deviceNum, valueForReg);
    return true;
}

