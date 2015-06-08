//---------------------------------------------------------------------------  
//title: VMEBus Library for Universe Driver
//version: Linux 1.1
//date: April 1999
//designer: Michael Wyrick                                                     
//programmer: Michael Wyrick                                                   
//company: Umbra System Inc.
//platform: Linux 2.2.x, 2.4.x                                    
//language: GCC 2.95, 3.0
//module: 
//---------------------------------------------------------------------------  
//  Purpose:                                                                   
//  Docs:                                                                      
//---------------------------------------------------------------------------  
#include <stdio.h>
#include <ctype.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

#include "universe.h"
#include "libvme.h"

// Internal constants only to libvme
#define VME_CYCLE	      0x10
#define VME_SIZE 	      0x60
#define MODE_PROGRAMMED 0x01
#define MODE_DMA        0x02
#define VME_SUP_USR	    0x08
#define VME_PRG_DATA    0x04
#define MODE_PROGRAMMED 0x01
#define MODE_DMA	      0x02

#define DEBUG
#undef DEBUG
//-----------------------------------------------------------------------------
// Function   : SwapEndian
// Inputs     : any MultiByte type
// Outputs    : a MultiByte type with the Bytes swapped
// Description: Flip the Endianess of any Multibyte Type
// Remarks    :
// History    :
//-----------------------------------------------------------------------------
template<class T>T SwapEndian(T in)
{
  T out;
  int s = sizeof(T);

  char *pin  = reinterpret_cast<char *>(&in);
  char *pout = reinterpret_cast<char *>(&out);
  
  for (int x=0;x<s;x++)
    pout[x] = pin[(s-1)-x];
  return out;
}

//-----------------------------------------------------------------------------
// Function   : VMEBus construstor
// Inputs     :
//   addr      = VME Base Address to map
//   count     = Size of mapping window
//   space     = which VME address space the memory is in (i.e. user/data etc.)
//   which_img = which of the four universe images to use
// Outputs    :
// Description:
//   Add a space description here
//   if which_img is -1 then scan for and pick the next free image,
//   only image 3 can be used for SUPERVISOR mappings
// Remarks    :
// History    :
//-----------------------------------------------------------------------------
VMEBus::VMEBus(unsigned int addr, int count, char space, int which_img)
{
  int to   = 0;
  int am   = 0;
  int size = 0;

  iSwapEndian = 1;
  ctl = 0x00800000;

  if (space & VME_SUPERVISOR)     // Only image can use supervisor space
    which_img = 3;
 
  if (which_img == -1) {  // Auto Image
    if ((vme_handle = open("//dev//vme_m0",O_RDWR,0)) > 0) {
      pci_base_addr  = 0xC0000000;
    } else {
      if ((vme_handle = open("//dev//vme_m1",O_RDWR,0)) > 0) {
        pci_base_addr  = 0xC4000000;
      } else {
        if ((vme_handle = open("//dev//vme_m2",O_RDWR,0)) > 0) {
          pci_base_addr  = 0xC8000000;
        } else {
          if ((vme_handle = open("//dev//vme_m3",O_RDWR,0)) > 0) {
            pci_base_addr  = 0xCD000000;
          }
        }
      }
    }
  } else { 
    switch (which_img) {  // Force Selected image
      case 0 :
        vme_handle = open("//dev//vme_m0",O_RDWR,0);
        pci_base_addr  = 0xC0000000;
        break;
      case 1 :
        vme_handle = open("//dev//vme_m1",O_RDWR,0);
        pci_base_addr  = 0xC4000000;
        break;
      case 2 :
        vme_handle = open("//dev//vme_m2",O_RDWR,0);
        pci_base_addr  = 0xC8000000;
        break;
      case 3 :
        vme_handle = open("//dev//vme_m3",O_RDWR,0);
        pci_base_addr  = 0xCC000000;
        break;
    }
  }
  uni_handle = open("//dev//vme_ctl",O_RDWR,0);
  
  if (vme_handle < 0) {
    throw VME_DriverBusy();
  }
  
  if (uni_handle < 0) {
    throw VME_DriverBusy();
  }

  size = space & VME_SIZE;
  
  switch (size) {
    case VME_SIZE_DO8:
      ctl &= ~CTL_VDW;
      ctl |= CTL_VDW_8;
      break;  
    case VME_SIZE_D16:
      ctl &= ~CTL_VDW;
      ctl |= CTL_VDW_16;
      break;  
    case VME_SIZE_D32:
      ctl &= ~CTL_VDW;
      ctl |= CTL_VDW_32;
      break;  
    case VME_SIZE_D64:
      ctl &= ~CTL_VDW;
      ctl |= CTL_VDW_64;
      break;  
  }
  
  am = space & 0x03;
	     
  if (space & VME_PRG_DATA) {
    ctl |= CTL_PGM;
  } else {
    ctl &= ~CTL_PGM;
  }		  
  
  if (space & VME_SUP_USR) {
    ctl |= CTL_SUPER;
  } else {
    ctl &= ~CTL_SUPER;
  }

  if (space & VME_CYCLE) {
    ctl |= CTL_VCT;
  } else {
    ctl &= ~CTL_VCT;
  }

  if (am == VME_A16) {
    vme_space = am;
    ctl &= ~CTL_VAS;
    ctl |= CTL_VAS_A16;
  } else if (am == VME_A24) {
    vme_space = am;
    ctl &= ~CTL_VAS;
    ctl |= CTL_VAS_A24;
  } else if (am == VME_A32) {
    vme_space = am;
    ctl &= ~CTL_VAS;
    ctl |= CTL_VAS_A32;
  } else if (am == VME_CR_CSR) {
    vme_space = am;
    ctl &= ~CTL_VAS;
    ctl |= CTL_VAS_CR_CSR;
  } else {
    throw VME_InvalidMapping();
  }

  pci_bound_addr = pci_base_addr + (((count-1) / 0x10000)+1)*0x10000;
  vme_base_addr  = addr;
  to = -pci_base_addr + vme_base_addr;

  ctl &= ~CTL_EN;    // Disable Slave
  ioctl(vme_handle,IOCTL_SET_CTL,ctl);  
  ioctl(vme_handle,IOCTL_SET_TO,to);
  ioctl(vme_handle,IOCTL_SET_BD,pci_bound_addr);
  ioctl(vme_handle,IOCTL_SET_BS,pci_base_addr);  // Must set this after BD
  ctl |= CTL_EN;     // Enable Slave
  ioctl(vme_handle,IOCTL_SET_CTL,ctl);

  if (space & VME_DMA_MODE)
    ioctl(vme_handle,IOCTL_SET_MODE,MODE_DMA);  
  else
    ioctl(vme_handle,IOCTL_SET_MODE,MODE_PROGRAMMED);
}

//-----------------------------------------------------------------------------
// Function   : ~VMEBus
// Inputs     : 
// Outputs    :
// Description: Close handles to Universe Driver
// Remarks    :
// History    :
//-----------------------------------------------------------------------------
VMEBus::~VMEBus()
{
  close(vme_handle);
  close(uni_handle);
}

//-----------------------------------------------------------------------------
// Function   : ReadUniReg
// Inputs     : 
//   int reg - Universe Register address to read
// Outputs    : value of universe register
// Description:
// Remarks    : See Universe ca91c042 documentation for registers
// History    :
//-----------------------------------------------------------------------------
unsigned int VMEBus::ReadUniReg(int reg)
{
  unsigned int v;
  
  lseek(uni_handle,reg,SEEK_SET);
  read(uni_handle,&v,4);
  return v;
}

//-----------------------------------------------------------------------------
// Function   : WriteUniReg
// Inputs     : 
//   int reg - Universe Register address to read
//   uint v  - value to write to the register
// Outputs    :
// Description: Write a value to the universe register
// Remarks    : See Universe ca91c042 documentation for registers
// History    :
//-----------------------------------------------------------------------------
void VMEBus::WriteUniReg(int reg, unsigned int v)
{
  lseek(uni_handle,reg,SEEK_SET);
  write(uni_handle,&v,4);
}

//-----------------------------------------------------------------------------
// Function   : ReadByte
// Inputs     : 
//   int addr - VME byte Address to Read the byte from
// Outputs    : the byte that was read
// Description: Read a byte from the VME Bus
// Remarks    : Will throw a BusError execption if there is an error
// History    :
//-----------------------------------------------------------------------------
unsigned char VMEBus::ReadByte(int addr)
{  
  int c;
  unsigned char v[4];
  
  lseek(vme_handle,addr,SEEK_SET);
  c = read(vme_handle,v,1);

#ifdef DEBUG
  printf("<libvme:ReadByte> addr=%08X v=%02X s=%01X\n", addr, *v, c);
#endif

  if (c != 1) {
    lasterror = -1;
    throw VME_BusError(addr + c, c, VME_TYPE_READ);
  } else
    lasterror = 0;
  return v[0];
}

//-----------------------------------------------------------------------------
// Function   : ReadWord
// Inputs     : 
//   int addr - VME byte Address to Read the word from
// Outputs    : the value of the word
// Description: Read a word from the VME Bus
// Remarks    : Will throw a BusError execption if there is an error
// History    :
//-----------------------------------------------------------------------------
unsigned short VMEBus::ReadWord(int addr)
{      
  int c;
  unsigned short v[2];	 

  lseek(vme_handle,addr,SEEK_SET);
  c = read(vme_handle,&v,2);

#ifdef DEBUG
  printf("<libvme:ReadWord> addr=%08X v=%02X s=%01X\n", addr, *v, c);
#endif

  if (c != 2) {
    lasterror = -1;
    throw VME_BusError(addr + c, c, VME_TYPE_READ);
  } else
    lasterror = 0;
  
  if (iSwapEndian)
    return SwapEndian(v[0]);
  else
    return v[0]; 
}

//-----------------------------------------------------------------------------
// Function   : ReadLong
// Inputs     : 
//   int addr - VME byte Address to Read the long word from
// Outputs    : the value of the long word
// Description: Read a long from the VME Bus
// Remarks    : Will throw a BusError execption if there is an error
// History    :
//-----------------------------------------------------------------------------
unsigned int VMEBus::ReadLong(int addr)
{    
  int c;
  unsigned int v;
  
  lseek(vme_handle, addr, SEEK_SET);
  c = read(vme_handle,&v,4);

#ifdef DEBUG
  printf("<libvme:Readlong> addr=%08X v=%02X s=%01X\n", addr, v, c);
#endif

  if (c != 4) {
    lasterror = -1;
    throw VME_BusError(addr + c, c, VME_TYPE_READ);
  } else
    lasterror = 0;

  if (iSwapEndian)
    return SwapEndian(v);
  else
    return v; 
}

//-----------------------------------------------------------------------------
// Function   : WriteByte
// Inputs     : 
//   int addr - VME byte Address to write the byte to
//   uchar v  - the byte to write
// Outputs    :
// Description: Write a Byte to the VME Bus
// Remarks    : Will throw a BusError execption if there is an error
// History    :
//-----------------------------------------------------------------------------
void VMEBus::WriteByte(int addr,unsigned char v)
{     
  int c;
  
  lseek(vme_handle, addr, SEEK_SET);
  c = write(vme_handle,&v,1);

#ifdef DEBUG
  printf("<libvme:WriteByte> addr=%08X v=%02X s=%01X\n", addr, v, c);
#endif

  if (c != 1) {
    lasterror = -1;
    throw VME_BusError(addr + c, c, VME_TYPE_WRITE);
  } else
    lasterror = 0;
}

//-----------------------------------------------------------------------------
// Function   : WriteWord
// Inputs     : 
//   int addr - VME byte Address to write the word to
//   ushort v - the word to write
// Outputs    :
// Description: Write a word to the VME Bus
// Remarks    : Will throw a BusError execption if there is an error
// History    :
//-----------------------------------------------------------------------------
void VMEBus::WriteWord(int addr, unsigned short v)
{     
  int c;
  unsigned short result; 

  if (iSwapEndian)
    result = SwapEndian(v);
  else
    result = v; 
  
  lseek(vme_handle,addr,SEEK_SET);
  c = write(vme_handle,&result,2);

#ifdef DEBUG
  printf("<libvme:WriteWord> addr=%08X v=%02X s=%01X\n", addr, result, c);
#endif

  if (c != 2) {
    lasterror = -1;
    throw VME_BusError(addr + c, c, VME_TYPE_WRITE);
  } else
    lasterror = 0;
}

//-----------------------------------------------------------------------------
// Function   : WriteLong
// Inputs     : 
//   int addr - VME byte Address to write the long word to
//   uint v   - the long word to write
// Outputs    :
// Description: Write a long word to the VME Bus
// Remarks    : Will throw a BusError execption if there is an error
// History    :
//  99/Apr/9 Fixed unsigned short result to unsigned long result
//           This would not write a long correctly before this fix.
//----------------------------------------------------------------------------
void VMEBus::WriteLong(int addr,unsigned int v)
{		     
  int c;
  unsigned long result; 

  if (iSwapEndian)
    result = SwapEndian(v);
  else
    result = v; 

  lseek(vme_handle,addr,SEEK_SET);
  c = write(vme_handle,&result,4);

#ifdef DEBUG
  printf("<libvme:WriteLong> addr=%08X v=%08lX s=%01X\n", addr, result, c);
#endif

  if (c != 4) {
    lasterror = -1;
    throw VME_BusError(addr + c, c, VME_TYPE_WRITE);
  } else
    lasterror = 0;
}

//-----------------------------------------------------------------------------
// Function   : VMEerror
// Inputs     :
// Outputs    : the value of the last error that happened
// Description: Return lasterror
// Remarks    :
//   This is not really used anymore, we now throw execptions, but it is left
//   in for older programs.  Please don't use it anymore
// History    :
//-----------------------------------------------------------------------------
int VMEBus::VMEerror()
{
  return lasterror;
}

//-----------------------------------------------------------------------------
// Function   : ReadWord_wa
// Inputs     :
//   int addr - the Word Address of the word to read
// Outputs    : the word that was read from the VME bus
// Description: This function uses word addressing insted of byte addressing
// Remarks    :
//   This function is used when the native register map for a board is in words
// History    :
//-----------------------------------------------------------------------------
unsigned short VMEBus::ReadWord_wa(int addr)
{      
  return ReadWord(addr * 2);
}

//-----------------------------------------------------------------------------
// Function   : WriteWord_wa
// Inputs     :
//   int addr - the Word Address of the word to write
//   ushort v - the word to write
// Outputs    : 
// Description: This function uses word addressing insted of byte addressing
// Remarks    :
//   This function is used when the native register map for a board is in words
// History    :
//-----------------------------------------------------------------------------
void VMEBus::WriteWord_wa(int addr,unsigned short v)
{      
  WriteWord(addr * 2,v);
}

//-----------------------------------------------------------------------------
// Function   : ReadLong_la
// Inputs     :
//   int addr - the long word  Address of the long word to read
// Outputs    : the long word that was read from the VME bus
// Description: This function uses long word addressing insted of byte 
//              addressing
// Remarks    :
//   This function is used when the native register map for a board is in 
//   long words
// History    :
//-----------------------------------------------------------------------------
unsigned long VMEBus::ReadLong_la(int addr)
{      
  return ReadLong(addr * 4);
}

//-----------------------------------------------------------------------------
// Function   : WriteLong_la
// Inputs     :
//   int addr - the long word Address of the long word to write to
//   uint v   - the long word to write
// Outputs    : 
// Description: This function uses long word addressing insted of byte 
//              addressing
// Remarks    :
//   This function is used when the native register map for a board is in 
//   long words
// History    :
//-----------------------------------------------------------------------------
void VMEBus::WriteLong_la(int addr,unsigned long v)
{      
  WriteLong(addr * 4,v);
}

//-----------------------------------------------------------------------------
// Function   : ReadBlock
// Inputs     :
//   int addr   - VME Byte address to read from
//   int count  - The number of BYTES to read
//   uchar*buff - Pointer to place data into (the user must provide enough room)
// Outputs    : The number of Byte read
// Description:
// Remarks    : Will throw a BusError execption if there is an error. This 
//   function can be used to do DMA transfers if DMA mode is enabled.
// History    :
//-----------------------------------------------------------------------------
int VMEBus::ReadBlock(int addr, int count, unsigned char *buff)
{  
  int c;

  lseek(vme_handle,addr,SEEK_SET);
  c = read(vme_handle,buff,count);
  if (c != count) {
    lasterror = -1;
    throw VME_BusError(addr + c, c, VME_TYPE_READ);
  } else
    lasterror = 0;
  return c;
}

//-----------------------------------------------------------------------------
// Function   : WriteBlock
// Inputs     :
//   int addr   - VME Byte address to read from
//   int count  - The number of BYTES to write
//   uchar*buff - Pointer to data 
// Outputs    : The number of Byte written
// Description:
// Remarks    : Will throw a BusError execption if there is an error. This 
//   function can be used to do DMA transfers if DMA mode is enabled.
// History    :
//-----------------------------------------------------------------------------
int VMEBus::WriteBlock(int addr, int count, unsigned char *buff)
{  
  int c;
  
  lseek(vme_handle,addr,SEEK_SET);
  c = write(vme_handle,buff,count);
  if (c != count) {
    lasterror = -1;
    throw VME_BusError(addr + c, c, VME_TYPE_WRITE);
  } else
    lasterror = 0;
  return c;
}

//-----------------------------------------------------------------------------
// Function   : SwapEndianOff
// Inputs     :
// Outputs    : returns the value of iSwapEndian
// Description: Use this function to disable the Endian Swapping that this
//   library will do on word and long word accesses
// Remarks    :
// History    :
//-----------------------------------------------------------------------------
int VMEBus::SwapEndianOff(void)
{
  iSwapEndian = 0;
  return (iSwapEndian);
}

//-----------------------------------------------------------------------------
// Function   : SwapEndianOn
// Inputs     :
// Outputs    : returns the value of iSwapEndian
// Description: Use this function to enable the Endian Swapping that this
//   library will do on word and long word accesses
// Remarks    :
// History    :
//-----------------------------------------------------------------------------
int VMEBus::SwapEndianOn(void)
{
  iSwapEndian = 1;
  return (iSwapEndian);
}

//-----------------------------------------------------------------------------
// Function   : Mode_DMA
// Inputs     :
// Outputs    :
// Description: Turn on DMA mode transfers
// Remarks    :
// History    :
//-----------------------------------------------------------------------------
void VMEBus::Mode_DMA(void)
{
  ioctl(vme_handle,IOCTL_SET_MODE,MODE_DMA);
}

//-----------------------------------------------------------------------------
// Function   : Mode_Programmed
// Inputs     :
// Outputs    :
// Description: Turn off DMA mode transfers
// Remarks    :
// History    :
//-----------------------------------------------------------------------------
void VMEBus::Mode_Programmed(void)
{
  ioctl(vme_handle,IOCTL_SET_MODE,MODE_PROGRAMMED);
}






/*
/////////////////////////////////////////////
// Sergey: C interface for the VMEBus class
/////////////////////////////////////////////


// the types of the class factories
typedef VMEBus* create();
typedef void destroy(VMEBus*);



class VMEBusC: public VMEBus {

  public:
  virtual int bla() const {;}

};


// the class factories

extern "C" VMEBus* create() {
  return new VMEBusC;
}

extern "C" void destroy(VMEBus* p) {
  delete p;
}
*/
