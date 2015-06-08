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

******************************************************************************
* File : vpd.c
*
* Description: This file contains all the routines that are related to VPD 
*               (Vital Product Data).
*
* Oct 2nd 2002
* Now swaping VPD data returned on VPD reads/writes. using frcPciConfigRead & 
* frcPciConfigWrite for VPD data only
******************************************************************************/
/* Includes */
/*Modified Date :July23rd '02 1)Added VPD tag delete feature
                  2)Also accepting "space" char. in the input string    
*/
#include "vxWorks.h"
#include "stdio.h"

#include "gtCore.h"
#include "gt64360r.h"
#include "pciScan.h"
#include "flashDrv.h"

/* Declarations */
STATUS frcVPDInit(void);
void frcVPDWrite(UINT32,UINT32,UINT32,UINT16,UINT32);
void frcVPDRead(UINT32,UINT32,UINT32,UINT16,UINT32 *);

/******************************************************************************
*
* Function: frcVPDInit.
*
* Description:  This functions initializes the VPD functionality of the loacl board and
*               hence had to called by the local CPU in pciScan().
* Input : None.
*
* Output: None
*
******************************************************************************/
STATUS frcVPDInit() {    
    UINT32 temp, base;


    GT_REG_READ( BASE_ADDR_ENABLE, &temp );
    printf("Read from address %x is %x\n",BASE_ADDR_ENABLE,temp);

    if ( !(temp & 0x100) )
    {
        /* boot CS enabled */
        base = BOOTCS_BASE_ADRS + _1M - 0x8000;  /* last 32K */
        base = base >> 15;
        printf("base = %x\n",base);
    }
    else
        return(ERROR);

    GT_REG_READ(PCI_0_ADDR_DECODE_CONTROL, &temp);

    printf("Read from address %x is %x\n",PCI_0_ADDR_DECODE_CONTROL,temp);

    temp = temp & 0xff; 
    temp = temp | ( base << 8 ); /*Set VPD loction to last 32 K block address */

    GT_REG_WRITE(PCI_0_ADDR_DECODE_CONTROL, temp);
    printf("Written to address %x is %x\n",PCI_0_ADDR_DECODE_CONTROL,temp);
    return(OK);
}

/******************************************************************************
*
* Function: frcVPDWrite.
*
* Description: This function is to write a 32 bit data into the VPD area of a 
*              PCI device.
*
* Inputs    : bus - PCI  device bus number.
*             dev - PCI device number.
*             fun - PCI device function number.
*             vpAddr - The VPV address where the VPD has to be written.This is
*                      only 15 bits wide.
*             vpdData - The VPD to be written.
*
* Outputs : None.
*
******************************************************************************/

void frcVPDWrite(UINT32 bus, UINT32 dev, UINT32 fun,UINT16 vpdAddr,UINT32 vpdData) {
    UINT32 temp,temp1;
    UINT32 vpdConfigAddr0;
    UINT32 vpdConfigAddr1;
    bool flag=true;

    frcPciSetActive(0);

    vpdAddr = vpdAddr & 0x7fff; /* Maake sure its a 15 bit address */

    /* Get the pointer to the Power management capability */
    frcPciConfigRead(bus,dev,fun,0x34,&temp);
    temp = temp & 0x000000ff;
    printf("Value of temp is %x\n",temp);

    /* Get the pointer to the VPD */
    frcPciConfigRead(bus,dev,fun,temp,&vpdConfigAddr0);
    vpdConfigAddr0 = vpdConfigAddr0 >> 8;
    vpdConfigAddr0 = vpdConfigAddr0 & 0x000000ff; 
    vpdConfigAddr1 = vpdConfigAddr0 + 4;
    printf("vpdConfigAddr0 = %x\nvpdConfigAddr1 = %x\n",vpdConfigAddr0,vpdConfigAddr1);

    /*Write the VPD data to the VPD data register in the configuration space */
    frcPciConfigWrite(bus,dev,fun,vpdConfigAddr1,vpdData);
    printf("Written to address %x is %x\n",vpdConfigAddr1,vpdData);
    /* Write the address and the flag to the vpd address register */
    temp1 = (UINT32)vpdAddr;
    temp1 = temp1 << 16;
    frcPciConfigRead(bus,dev,fun,vpdConfigAddr0,&temp);
    printf("Read from %x is %x\n",vpdConfigAddr0,temp);
    temp = temp & 0xffff;
    temp = temp | temp1;
    temp = temp | 0x80000000; /* Write flag */
    frcPciConfigWrite(bus,dev,fun,vpdConfigAddr0,temp);
    printf("Written to address %x is %x\n",vpdConfigAddr0,temp);

    /* Wait infinitely until the data is written */
    while(flag) {
    frcPciConfigRead(bus,dev,fun,vpdConfigAddr0,&temp);
    if((temp & 0x80000000) == 0)
        flag=false;
    }
}

/******************************************************************************
*
* Function: frcVPDRead.
*
* Description: This function is to read a 32 bit VPD from the VPD area of a 
*              PCI device.
*
* Inputs    : bus - PCI  device bus number.
*             dev - PCI device number.
*             fun - PCI device function number.
*             vpAddr - The VPD address where the VPD has to be read.This is
*                      only 15 bits wide.
*             vpdData - The returned VPD.
*
* Outputs : None.
*
******************************************************************************/

void frcVPDRead(UINT32 bus, UINT32 dev, UINT32 fun,UINT16 vpdAddr,UINT32 *vpdData) {
    UINT32 temp,temp1;
    UINT32 vpdConfigAddr0;
    UINT32 vpdConfigAddr1;
    bool flag=true;

    vpdAddr = vpdAddr & 0x7fff; /* Maake sure its a 15 bit address */

    /* Get the pointer to the Power management capability */
    frcPciConfigRead(bus,dev,fun,0x34,&temp);
    temp = temp & 0x000000ff;

    /* Get the pointer to the VPD */
    frcPciConfigRead(bus,dev,fun,temp,&vpdConfigAddr0);
    vpdConfigAddr0 = vpdConfigAddr0 >> 8;
    vpdConfigAddr0 = vpdConfigAddr0 & 0x000000ff;
    vpdConfigAddr1 = vpdConfigAddr0 + 4;
    printf("vpdConfigAddr0 = %x\nvpdConfigAddr1 = %x\n",vpdConfigAddr0,vpdConfigAddr1);

    /* Write the address and the flag to the vpd address register */
    temp1 = (UINT32)vpdAddr;
    temp1 = temp1 << 16;
    frcPciConfigRead(bus,dev,fun,vpdConfigAddr0,&temp);
    printf("Read from %x is %x\n",vpdConfigAddr0,temp);
    temp = temp & 0xffff;
    temp = temp | temp1;
    temp = temp & 0x7fffffff; /* Write flag */
    frcPciConfigWrite(bus,dev,fun,vpdConfigAddr0,temp);
    printf("Written to address %x is %x\n",vpdConfigAddr0,temp);

    /* Wait infinitely until the data is read */
    while(flag) {
    frcPciConfigRead(bus,dev,fun,vpdConfigAddr0,&temp);
    if((temp & 0x80000000) != 0)
        flag=false;
    }

    /*Read the VPD data from the VPD data register in the configuration space*/
    frcPciConfigRead(bus,dev,fun,vpdConfigAddr1,vpdData);
    printf("Read from address %x is %x\n",vpdConfigAddr1,*vpdData);

}
