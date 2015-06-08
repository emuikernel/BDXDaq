#ifndef PCIPERF_H__
#define PCIPERF_H__

#define VxWORKS

#include<stdio.h>
#ifndef VxWORKS
#define OK 1
#define ERROR 2
#define _PCID_H_
#endif

#ifndef VxWORKS
typedef unsigned long long BITS64;
typedef unsigned int BITS32;
typedef unsigned short BITS16;
typedef unsigned char BITS8;
typedef  BITS32 STATUS;
#else
typedef unsigned long long BITS64;
typedef UINT32 BITS32;
typedef UINT16 BITS16;
typedef unsigned char BITS8;
#endif
#ifdef VxWORKS
typedef UINT32 SIZE_TYPE;
typedef unsigned long long ADDR_TYPE;
#else
typedef BITS32 SIZE_TYPE;	
typedef BITS32 ADDR_TYPE;
#endif
#define NOTHING 0



#define MAX_PCI_DEVICE 10	/*Maximum number of PCI devices */
#define MAX_BUS 10

#define ADDR_FORMAT "%X"
#define BUS_DEV_FUN "%02x:%02x.%02x"
#define PRINT_HR_FORMAT(x)	if (!(x)) return;\
		if ((x) < 1024) printf("%d", (int) (x));\
      	else if ((x) < (1024*1024) ) printf("%dK", (int)((x)/ 1024));\
	else if ((x) < (1024*1024*1024)) printf("%dM", (int)((x) / 1048576));\
      		else printf(ADDR_FORMAT,(x)); 

static   void  print_base ( ADDR_TYPE addr)
{
printf("[Base:%llx",addr);
}	
static  void  print_size ( SIZE_TYPE x)
{
printf("[Size:");
if (!x)
	return;
if ((x) < 1024) 
	printf("%d", (int) x);
else if ((x) < (1024*1024) ) 
	printf("%dK", (int)(x/ 1024));
else if ((x) < (1024*1024*1024)) 
	printf("%dM", (int)(x / 1048576));
else 
	printf(ADDR_FORMAT,x); 
printf("]");	
}

struct BaseAddress
{
  ADDR_TYPE start;
  SIZE_TYPE size;
  struct BaseAddress *next;
};
struct MemorySpace
{
  int bus;
  int dev;
  int fun;
  struct BaseAddress *base;
};
struct MemorySpace memory[MAX_PCI_DEVICE];
/*
SIZE_TYPE pci_size (SIZE_TYPE,SIZE_TYPE);
STATUS pci_read (int bus, int dev, int fun, int off, BITS32 * data);
STATUS pci_write (int bus, int dev, int fun, int off, BITS32 data);
STATUS get_pci_device_BAR (int bus, int dev, int fun,  struct BaseAddress **start);
int scan_all_pci_device (void);
*/


#define  PCI_BASE_ADDRESS_SPACE_MEMORY 0x00UL
#define  PCI_BASE_ADDRESS_SPACE	0x01UL	/* 0 = memory, 1 = I/O */
#define  PCI_BASE_ADDRESS_MEM_MASK	(~0x0fUL)
#define PCI_BASE_ADDRESS_0	0x10	/* 32 bits */
#define  PCI_BASE_ADDRESS_MEM_TYPE_64	0x04UL	/* 64 bit address */
#define  PCI_BASE_ADDRESS_MEM_TYPE_MASK 0x06UL

#ifdef _PCID_H_
#define GET_DURATION 901
#define SET_TYPE 902
#define SET_FROM_ADDR 903
#define SET_TO_ADDR 904
#define SET_FROM_SIZE 905
#define SET_TO_SIZE 906
#define DO_VIR2PHY 907

#define FOR_NONE  1001 /*From Memory to Memory do  operations.*/
#define FOR_DST  1002 /*From Memory to some PCI device do  operations.*/
#define FOR_SRC  1003 /*From PCI device to Memory do  operations.*/
#define FOR_BOTH  1004 /*From one PCI device to same or other PCI device  operations.*/

#define SIZE1 801
#define SIZE2 802
#define SIZE4 803
#define SIZE8 804

struct  Base 
{
SIZE_TYPE size;
ADDR_TYPE  start;
};
#endif

/*Global variables*/
extern char pcilist[MAX_PCI_DEVICE][9];
extern unsigned int min_size;

/*Function definitions*/
int pciperfAll (unsigned long input);
STATUS parsePciString (const char *, struct MemorySpace *);
SIZE_TYPE pci_size (SIZE_TYPE, SIZE_TYPE);
STATUS pciperf (char *, char *);
int scanPci (int);
STATUS isPciDevicePresent (int, int, int);
STATUS pci_read (int , int , int , int , BITS32 * );
STATUS pci_write (int , int , int , int , BITS32  );
STATUS get_pci_device_BAR (int , int , int , struct BaseAddress ** );

int do_pciperf (ADDR_TYPE, ADDR_TYPE, SIZE_TYPE, SIZE_TYPE);
void print_rate (SIZE_TYPE, BITS32);

#endif /*#ifndef PCIPERF_H__*/