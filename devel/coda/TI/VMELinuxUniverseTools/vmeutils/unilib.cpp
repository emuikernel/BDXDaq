//------------------------------------------------------------------------------  
//title: VME Debugger for XVME-655 
//version: Linux 1.1
//date: February 1998
//designer: Michael Wyrick                                                      
//programmer: Michael Wyrick & John Huggins
//project: VMELinux Project in association with Chesapeake Research
//copyright 1998 VMELinux Project and Michael Wyrick
//platform: Linux 2.2.x, 2.4.x
//language: GCC 2.95, GCC 3.0
//module: 
//------------------------------------------------------------------------------  
//  Purpose:                                                                    
//  Docs:                                                                       
//------------------------------------------------------------------------------  
#include <stdio.h>
#include <ctype.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include "universe.h"
#include "unilib.h"

// Set to one if 16 bit and/or 32 bit values should be byte swapped
// when going to or from the VMEbus.
int SwapEndian = 0;   

int vme_handle;
int uni_handle;
int vme_space;
int ctl = 0x00800000;
unsigned int pci_base_addr;
unsigned int pci_bound_addr;
unsigned int vme_base_addr;

extern int  which_img;

//----------------------------------------------------------------------------
// 
//----------------------------------------------------------------------------
unsigned int ReadUniReg(int reg)
{
  unsigned int v;

  lseek(uni_handle,reg,SEEK_SET);
  read(uni_handle,&v,4);
  return v;
}

//----------------------------------------------------------------------------
//  Setup the Universe
//----------------------------------------------------------------------------
void WriteUniReg(int reg, unsigned int v)
{
  lseek(uni_handle,reg,SEEK_SET);
  write(uni_handle,&v,4);
}

//----------------------------------------------------------------------------
// Read one or more bytes, one byte at a time.
//----------------------------------------------------------------------------
unsigned char rb(int addr,int *error)
{  
  int c;
  unsigned char v[4];

  lseek(vme_handle,addr,SEEK_SET);
  c = read(vme_handle,v,1);
  if (c != 1)
    *error = -1;
  else
    *error = 0;
  return v[0];
}

//----------------------------------------------------------------------------
// Read one or more words, one word at a time.
//----------------------------------------------------------------------------
unsigned short rw(int addr,int *error)
{      
  int c;
  unsigned short v[2];   
  unsigned short result;

  lseek(vme_handle,addr,SEEK_SET);
  c = read(vme_handle,&v,2);
  if (c != 2)
    *error = -1;
  else
    *error = 0;

  if (SwapEndian)
    result = ((v[0] & 0xFF00) >> 8) | ((v[0] & 0x00FF) << 8);
  else
    result = v[0]; 

  return result;
}
unsigned short result;

//----------------------------------------------------------------------------
// Read one or more longwords(32bit), one at a time.
//----------------------------------------------------------------------------
unsigned int rl(int addr,int *error)
{    
  int c;
  unsigned int v[4];
  unsigned int result;

  lseek(vme_handle,addr,SEEK_SET);
  c = read(vme_handle,&v,4);
  if (c != 4)
    *error = -1;
  else
    *error = 0;

  if (SwapEndian) // Added by JSH
    result = ((v[0] & 0xFF000000) >> 24) | ((v[0] & 0x00FF0000) >> 8) | ((v[0] & 0x0000FF00) << 8) | ((v[0] & 0x000000FF) << 24);
  else
    result = v[0]; 

  return result;
}

//----------------------------------------------------------------------------
//  Write one or more bytes, one byte at a time.
//----------------------------------------------------------------------------
void wb(int addr,unsigned char v,int *error)
{     
  int c;

  lseek(vme_handle,addr,SEEK_SET);
  c = write(vme_handle,&v,1);
  if (c != 1)
    *error = -1;
  else
    *error = 0;
}

//----------------------------------------------------------------------------
// Write one or more words, one word at a time.
//----------------------------------------------------------------------------
void ww(int addr,unsigned short v,int *error)
{     
  int c;
  unsigned short result; 

  if (SwapEndian)
    result = ((v & 0x00FF00) >> 8) | ((v & 0x00FF) << 8);
  else
    result = v; 

  lseek(vme_handle,addr,SEEK_SET);
  c = write(vme_handle,&result,2);
  if (c != 2)
    *error = -1;
  else
    *error = 0;
}

//----------------------------------------------------------------------------
// Write one or more longwords(32bit), one at a time.
//----------------------------------------------------------------------------
void wl(int addr,unsigned int v,int *error)
{        
  int c;
  unsigned int result;

  if (SwapEndian) // Added by JSH
    result = ((v & 0xFF000000) >> 24) | ((v & 0x00FF0000) >> 8) | ((v & 0x0000FF00) << 8) | ((v & 0x000000FF) << 24) ;
  else
    result = v; 

  lseek(vme_handle,addr,SEEK_SET);
//  c = write(vme_handle,&v,4);
  c = write(vme_handle,&result,4);
  if (c != 4)
    *error = -1;
  else
    *error = 0;
}

//----------------------------------------------------------------------------
// 
//----------------------------------------------------------------------------
int init_vmelib(void)
{               
  switch (which_img) {
  case 0 :
    vme_handle = open("//dev//vme_m0",O_RDWR,0);
    break;
  case 1 :
    vme_handle = open("//dev//vme_m1",O_RDWR,0);
    break;
  case 2 :
    vme_handle = open("//dev//vme_m2",O_RDWR,0);
    break;
  case 3 :
    vme_handle = open("//dev//vme_m3",O_RDWR,0);
    break;
  }
  uni_handle = open("//dev//vme_ctl",O_RDWR,0);

  if (!vme_handle) {
    printf("Can't open VME device\n");
    return 1;
  }

  if (!uni_handle) {
    printf("Can't open Universe Control device\n");
    return 1;
  }

  return 0;
}

//----------------------------------------------------------------------------
// 
//----------------------------------------------------------------------------
void shutdown_vmelib(void)
{
  close(vme_handle);
  close(uni_handle);
}

//----------------------------------------------------------------------------
// 
//----------------------------------------------------------------------------
void vmespace(int i)
{ 
  int am;
  int size;

  size = i & VME_SIZE;

  switch (size) {
  case VME_SIZE_8:
    ctl &= ~CTL_VDW;
    ctl |= CTL_VDW_8;
    ioctl(vme_handle,IOCTL_SET_CTL,ctl);
    break;  
  case VME_SIZE_16:
    ctl &= ~CTL_VDW;
    ctl |= CTL_VDW_16;
    ioctl(vme_handle,IOCTL_SET_CTL,ctl);
    break;  
  case VME_SIZE_32:
    ctl &= ~CTL_VDW;
    ctl |= CTL_VDW_32;
    ioctl(vme_handle,IOCTL_SET_CTL,ctl);
    break;  
  case VME_SIZE_64:
    ctl &= ~CTL_VDW;
    ctl |= CTL_VDW_64;
    ioctl(vme_handle,IOCTL_SET_CTL,ctl);
    break;  
  }

  am = i & 0x03;

  if (i & VME_PRG_DATA) {
    ctl |= CTL_PGM;
    ioctl(vme_handle,IOCTL_SET_CTL,ctl);
  } else {
    ctl &= ~CTL_PGM;
    ioctl(vme_handle,IOCTL_SET_CTL,ctl);
  }     

  if (i & VME_SUP_USR) {
    ctl |= CTL_SUPER;
    ioctl(vme_handle,IOCTL_SET_CTL,ctl);
  } else {
    ctl &= ~CTL_SUPER;
    ioctl(vme_handle,IOCTL_SET_CTL,ctl);
  }

  if (i & VME_CYCLE) {
    ctl |= CTL_VCT;
    ioctl(vme_handle,IOCTL_SET_CTL,ctl);
  } else {
    ctl &= ~CTL_VCT;
    ioctl(vme_handle,IOCTL_SET_CTL,ctl);
  }

  if (am == VME_A16) {
    vme_space = am;
    ctl &= ~CTL_VAS;
    ctl |= CTL_VAS_A16;
    ioctl(vme_handle,IOCTL_SET_CTL,ctl);
  } else if (am == VME_A24) {
    vme_space = am;
    ctl &= ~CTL_VAS;
    ctl |= CTL_VAS_A24;
    ioctl(vme_handle,IOCTL_SET_CTL,ctl);
  } else if (am == VME_A32) {
    vme_space = am;
    ctl &= ~CTL_VAS;
    ctl |= CTL_VAS_A32;
    ioctl(vme_handle,IOCTL_SET_CTL,ctl);
  } else if (am == VME_CR_CSR) {
    vme_space = am;
    ctl &= ~CTL_VAS;
    ctl |= CTL_VAS_CR_CSR;
    ioctl(vme_handle,IOCTL_SET_CTL,ctl);
  } else {
    printf("Invaild VMESPACE\n");
  }
}

//----------------------------------------------------------------------------
//  Note: Images 1-3 have to be 64k aligned
//----------------------------------------------------------------------------
void vmemap(unsigned int addr,int count,char space)
{
  int to;

  vmespace(space);

  switch (which_img) {
  case 0: 
    pci_base_addr  = 0xC0000000;
    break;
  case 1:
    pci_base_addr  = 0xC1000000;
    break;
  case 2:
    pci_base_addr  = 0xC2000000;
    break;
  case 3:
    pci_base_addr  = 0xC3000000;
    break;
  }

  pci_bound_addr = pci_base_addr + (((count-1) / 0x10000)+1)*0x10000;
  vme_base_addr  = addr;
  to = -pci_base_addr + vme_base_addr;

  ctl &= ~CTL_EN; // Disable Slave
  ioctl(vme_handle,IOCTL_SET_CTL,ctl);  
  ioctl(vme_handle,IOCTL_SET_TO,to);
  ioctl(vme_handle,IOCTL_SET_BD,pci_bound_addr);
  ioctl(vme_handle,IOCTL_SET_BS,pci_base_addr);  // Must set this after BD
  ctl |= CTL_EN;     // Enable Slave
  ioctl(vme_handle,IOCTL_SET_CTL,ctl);


  // Lets do programed for now
  ioctl(vme_handle,IOCTL_SET_MODE,MODE_PROGRAMMED);
}
