//------------------------------------------------------------------------------  
//title: VME Debugger for XVME-655 
//version: Linux 1.1
//date: February 1998
//designer: Michael Wyrick                                                      
//programmer: Michael Wyrick                                                    
//company: Umbra System Inc.
//project: VMELinux Project in association with Chesapeake Research
//platform: Linux 2.2.x, 2.4.x                                         
//language: GCC 2.95, 3.0
//module: 
//------------------------------------------------------------------------------  
//  Purpose:                                                                    
//  Docs:                                                                       
//------------------------------------------------------------------------------  

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
#define MODE_DMA	      0x02

extern int vme_handle;
extern int pci_size;
extern unsigned int pci_base_addr;
extern unsigned int pci_bound_addr;
extern unsigned int vme_base_addr;

//----------------------------------------------------------------------------
// Prototypes
//----------------------------------------------------------------------------
int init_vmelib(void);
void shutdown_vmelib(void);
void vmemap(unsigned int addr,int count,char space);
void vmesize(int size);

unsigned int ReadUniReg(int reg);
void WriteUniReg(int reg, unsigned int v);

unsigned char  rb(int addr, int *error);
unsigned short rw(int addr, int *error);
unsigned int   rl(int addr, int *error);

void wb(int addr,unsigned char v, int *error);
void ww(int addr,unsigned short v, int *error);
void wl(int addr,unsigned int v, int *error);


