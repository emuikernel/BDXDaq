//------------------------------------------------------------------------------  
//title: VMEBus Library for Universe Driver
//version: Linux 1.1
//date: April 1999
//designer: Michael Wyrick                                                      
//programmer: Michael Wyrick                                                    
//company: Umbra System Inc.
//platform: Linux 2.2.x, 2.4.x                                    
//language: GCC 2.95, 3.0
//module: 
//------------------------------------------------------------------------------  
//  Purpose:                                                                    
//  Docs:                                                                       
//------------------------------------------------------------------------------  
#ifndef VMELIB_H
#define VMELIB_H

// Default Mapping - Address Space : VME_CR_CSR 
//                   Access Type   : Program
//                   User Type     : User 
//                   Data Size     : DO8
//                   Block Mode    : Non-Block Mode

#define VME_CR_CSR	0x00
#define VME_A16		0x01   
#define VME_A24		0x02
#define VME_A32		0x03

#define VME_DATA	0x04     
#define VME_SUPERVISOR  0x08     

#define VME_SIZE_DO8	0x00
#define VME_SIZE_D16	0x20
#define VME_SIZE_D32	0x40
#define VME_SIZE_D64	0x60

#define VME_BLOCK       0x10
#define VME_DMA_MODE    0x80

//----------------------------------------------------------------------------
// VMEExecption Class
//----------------------------------------------------------------------------
const unsigned char VME_TYPE_READ  = 1; 
const unsigned char VME_TYPE_WRITE = 2;

class VMEExecption {
};

class VME_BusError : public VMEExecption {
public:
  int address;
  int count;
  int access_type;

  VME_BusError(int a, int c, int at) {
    address     = a;
    count       = c;
    access_type = at;
  }
};

class VME_DriverBusy : public VMEExecption {
};

class VME_InvalidMapping : public VMEExecption {
};

//----------------------------------------------------------------------------
// VMEBus Class
//----------------------------------------------------------------------------
class VMEBus {
private:
  int iSwapEndian;    
  int vme_handle;
  int uni_handle;
  int vme_space;
  int ctl;            
  unsigned int pci_base_addr;
  unsigned int pci_bound_addr;
  unsigned int vme_base_addr;

  int lasterror;

public:
  VMEBus(unsigned int addr, int count, char space,int image = -1);
  ~VMEBus();

  unsigned int ReadUniReg(int reg);
  void WriteUniReg(int reg, unsigned int v);

  int VMEerror();

  unsigned char  ReadByte(int);
  unsigned short ReadWord(int);
  unsigned int   ReadLong(int);

  void WriteByte(int, unsigned char);
  void WriteWord(int, unsigned short v);
  void WriteLong(int, unsigned int v);

  unsigned short ReadWord_wa(int);  
  void WriteWord_wa(int, unsigned short v);

  unsigned long ReadLong_la(int);  
  void WriteLong_la(int, unsigned long v);

  int ReadBlock(int, int, unsigned char *);
  int WriteBlock(int, int, unsigned char *);

  int SwapEndianOn(void);
  int SwapEndianOff(void);

  void Mode_DMA(void);
  void Mode_Programmed(void);
};

#endif // VMELIB_H
