//------------------------------------------------------------------------------  
//title: VMEBus Library for Universe Driver
//version: Linux 1.1
//date: February 1998
//designer: Michael Wyrick                                                      
//programmer: Michael Wyrick                                                    
//company: Umbra System Inc.
//project: VMELinux Project in association with Chesapeake Research
//platform: Linux 2.2.x, 2.4.x
//language: GCC 2.95, GCC 3.0
//module: 
//------------------------------------------------------------------------------  
//  Purpose:                                                                    
//  Docs:                                                                       
//------------------------------------------------------------------------------  
#ifndef VMELIB_H
#define VMELIB_H

#define VME_CR_CSR	0x00
#define VME_A16		  0x01
#define VME_A24		  0x02
#define VME_A32		  0x03

#define VME_PRG_DATA	0x04			    
#define VME_SUP_USR	  0x08

#define VME_CYCLE	    0x10

#define VME_SIZE      0x60 
#define VME_SIZE_8	  0x00
#define VME_SIZE_16	  0x20
#define VME_SIZE_32	  0x40
#define VME_SIZE_64	  0x60

#define MODE_PROGRAMMED 0x01
#define MODE_DMA        0x02

//----------------------------------------------------------------------------
// Prototypes
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
  VMEBus();
  VMEBus(int image, unsigned int addr, int count, char space);
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
};

#endif // VMELIB_H
