/* VM-USB EASY VME library
 * 
 * Copyright (C) WIENER, Plein & Baus / JTEC 2005
 * version 1.1, 12/05/05
 *
 *     This program is free software; you can redistribute it and/or
 *     modify it under the terms of the GNU General Public License as
 *     published by the Free Software Foundation, version 2.
 *
 * VM-USB EASY VME calls are direct calls through USB  
 * 
*/
//****************   Pre definition of functions   ************************

#ifndef VM_USB_EASYVME_H
#define VM_USB_EASYVME_H

#include "xxusbdll.h"
#include <windows.h>

short vme_read_16(usb_dev_handle *hdev,short Adress_Modifier, long VME_Address, long *Data);
short vme_read_32(usb_dev_handle *hdev, short Adress_Modifier, long VME_Address, long *Data);
short vme_BLT_read_32(usb_dev_handle *hdev, short Adress_Modifier, int count, long VME_Address, long Data[]);
short vme_write_16(usb_dev_handle *hdev, short Adress_Modifier, long VME_Address, long Data);
short vme_write_32(usb_dev_handle *hdev, short Adress_Modifier, long VME_Address, long Data);


short vme_write_32(usb_dev_handle *hdev, short Adress_Modifier, long VME_Address, long Data)
{
    long intbuf[1000];
    short ret;
    intbuf[0]=7;
    intbuf[1]=0;
    intbuf[2]=Adress_Modifier;
    intbuf[3]=0;
    intbuf[4]=(VME_Address & 0xffff);
    intbuf[5]=((VME_Address >>16) & 0xffff);
    intbuf[6]=(Data & 0xffff);
    intbuf[7]=((Data >> 16) & 0xffff);
    ret = xxusb_stack_execute(hdev, intbuf);
    return ret;
}

short vme_read_32(usb_dev_handle *hdev, short Adress_Modifier, long VME_Address, long *Data)
{
    long intbuf[1000];
    short ret;
    intbuf[0]=5;
    intbuf[1]=0;
    intbuf[2]=Adress_Modifier +0x100;
    intbuf[3]=0;
    intbuf[4]=(VME_Address & 0xffff);
    intbuf[5]=((VME_Address >>16) & 0xffff);
    ret = xxusb_stack_execute(hdev, intbuf);
    *Data=intbuf[0] + (intbuf[1] * 0x10000);
    return ret;
}


short vme_write_16(usb_dev_handle *hdev, short Adress_Modifier, long VME_Address, long Data)
{
    long intbuf[1000];
    short ret;
    intbuf[0]=7;
    intbuf[1]=0;
    intbuf[2]=Adress_Modifier;
    intbuf[3]=0;
    intbuf[4]=(VME_Address & 0xffff)+ 0x01;
    intbuf[5]=((VME_Address >>16) & 0xffff);
    intbuf[6]=(Data & 0xffff);
    intbuf[7]=0;
    ret = xxusb_stack_execute(hdev, intbuf);
    return ret;
}

short vme_read_16(usb_dev_handle *hdev,short Adress_Modifier, long VME_Address, long *Data)
{
    long intbuf[1000];
    short ret;
    intbuf[0]=5;
    intbuf[1]=0;
    intbuf[2]=Adress_Modifier +0x100;
    intbuf[3]=0;
    intbuf[4]=(VME_Address & 0xffff)+ 0x01;
    intbuf[5]=((VME_Address >>16) & 0xffff);
    ret = xxusb_stack_execute(hdev, intbuf);
    *Data=intbuf[0];
    return ret;
}

short vme_BLT_read_32(usb_dev_handle *hdev, short Adress_Modifier, int count, long VME_Address, long Data[])
{
    long intbuf[1000];
    short ret;
    int i=0;
    if (count > 255) return -1;
    intbuf[0]=5;
    intbuf[1]=0;
    intbuf[2]=Adress_Modifier +0x100;
    intbuf[3]=(count << 8);
    intbuf[4]=(VME_Address & 0xffff);
    intbuf[5]=((VME_Address >>16) & 0xffff);
    ret = xxusb_stack_execute(hdev, intbuf);
    int j=0;
    for (i=0;i<(2*count);i=i+2)
    {
		Data[j]=intbuf[i] + (intbuf[i+1] * 0x10000);
		j++;
    }
    return ret;
}

#endif