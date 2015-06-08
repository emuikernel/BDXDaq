/*----------------------------------------------------------------------------*
 *  Copyright (c) 2009        Southeastern Universities Research Association, *
 *                            Thomas Jefferson National Accelerator Facility  *
 *                                                                            *
 *    This software was developed under a United States Government license    *
 *    described in the NOTICE file included as part of this distribution.     *
 *                                                                            *
 *    Author:  Bryan Moffit                                                   *
 *             moffit@jlab.org                   Jefferson Lab, MS-12B3       *
 *             Phone: (757) 269-5660             12000 Jefferson Ave.         *
 *             Fax:   (757) 269-5800             Newport News, VA 23606       *
 *                                                                            *
 *----------------------------------------------------------------------------*
 *
 * Description:
 *     JLab extra routines to compliment the GEFanuc API
 *
 *----------------------------------------------------------------------------*/

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdarg.h>
#include <sys/mman.h>
#include <pthread.h>
#include "jlabgef.h"
#include "gef/gefcmn_vme_tempe.h"
#include "gef/gefcmn_osa.h"
#include "gef/gefcmn_vme_framework.h"
#include "tsi148.h"

#define A32_MAX_WINDOW_SIZE   0xf000000

/* Some internal function prototypes (shouldn't be used by the USER) */
GEF_STATUS tempeWriteRegister(GEF_UINT32 offset, GEF_UINT32 buffer);
GEF_STATUS map_tsi();
GEF_STATUS unmap_tsi();

/* Global pointers to the Userspace windows, available to other
   libraries (define as extern) */
void *a16_window=NULL;
void *a24_window=NULL;
void *a32_window=NULL;
void *a32blt_window=NULL;

/* VME address handles for GEF library */
static GEF_VME_MASTER_HDL a16_hdl, a24_hdl, a32_hdl, a32blt_hdl;
static GEF_MAP_HDL a16map_hdl, a24map_hdl, a32map_hdl, a32bltmap_hdl;

unsigned int a32_window_width    = 0x00010000;
unsigned int a32blt_window_width = 0x0a000000;
unsigned int a24_window_width    = 0x01000000;
unsigned int a16_window_width    = 0x00010000;

/* Some DMA Global Variables */
GEF_VME_DMA_HDL dmaLL_hdl;  /* GEF HDL for DMA Linked List buffer */
#define TSI148_DMA_MAX_LL 21
tsi148DmaDescriptor_t *dmaDescList;
GEF_UINT32 dmaList_dsat;
GEF_UINT32 dmaList_dctl;

/* Some additional global pointers */
struct _GEF_VME_MAP_MASTER *mapHdl;
tsi148_t *pTempe;

/* tempe chip mutex */
pthread_mutex_t tsi_mutex = PTHREAD_MUTEX_INITIALIZER;
#define LOCK_TEMPE {				\
    if(pthread_mutex_lock(&tsi_mutex)<0)	\
      perror("pthread_mutex_lock");		\
  }
#define UNLOCK_TEMPE {				\
    if(pthread_mutex_unlock(&tsi_mutex)<0)	\
      perror("pthread_mutex_lock");		\
  }

/* Some default settings */
GEF_VME_ADDR addr_A16 =
  {
    .upper = 0x00000000,
    .lower = 0x00000000,
    .addr_space = GEF_VME_ADDR_SPACE_A16,
    .addr_mode = GEF_VME_ADDR_MODE_USER | GEF_VME_ADDR_MODE_DATA,
    .transfer_mode = GEF_VME_TRANSFER_MODE_MBLT,
    .transfer_max_dwidth = GEF_VME_TRANSFER_MAX_DWIDTH_32,
    .vme_2esst_rate = GEF_VME_2ESST_RATE_160,
    .broadcast_id = 0,
    .flags = 0
    /*     .flags = GEF_VME_WND_EXCLUSIVE */
  };

GEF_VME_ADDR addr_A24 =
  {
    .upper = 0x00000000,
    .lower = 0x00000000,
    .addr_space = GEF_VME_ADDR_SPACE_A24,
    .addr_mode = GEF_VME_ADDR_MODE_USER | GEF_VME_ADDR_MODE_DATA,
    .transfer_mode = GEF_VME_TRANSFER_MODE_MBLT,
    .transfer_max_dwidth = GEF_VME_TRANSFER_MAX_DWIDTH_32,
    .vme_2esst_rate = GEF_VME_2ESST_RATE_160,
    .broadcast_id = 0,
    .flags = 0
    /*     .flags = GEF_VME_WND_EXCLUSIVE */
  };

GEF_VME_ADDR addr_A32 =
  {
    .upper = 0x00000000,
    .lower = 0x08000000,
    .addr_space = GEF_VME_ADDR_SPACE_A32,
    .addr_mode = GEF_VME_ADDR_MODE_USER | GEF_VME_ADDR_MODE_DATA,
    .transfer_mode = GEF_VME_TRANSFER_MODE_SCT,
    .transfer_max_dwidth = GEF_VME_TRANSFER_MAX_DWIDTH_32,
    .vme_2esst_rate = GEF_VME_2ESST_RATE_160,
    .broadcast_id = 0,
    .flags = 0
    /*     .flags = GEF_VME_WND_EXCLUSIVE */
  };

GEF_VME_ADDR addr_A32blt =
  {
    .upper = 0x00000000,
    .lower = 0x08000000,
    .addr_space = GEF_VME_ADDR_SPACE_A32,
    .addr_mode = GEF_VME_ADDR_MODE_USER | GEF_VME_ADDR_MODE_DATA,
    .transfer_mode = GEF_VME_TRANSFER_MODE_MBLT,
    .transfer_max_dwidth = GEF_VME_TRANSFER_MAX_DWIDTH_32,
    .vme_2esst_rate = GEF_VME_2ESST_RATE_160,
    .broadcast_id = 0,
    .flags = 0
    /*     .flags = GEF_VME_WND_EXCLUSIVE */
  };

/* didOpen=0(1) when the default windows have not (have been) opened */
int didOpen=0;

/* Prototypes of routines not included in the gefaunc headers */
GEF_STATUS GEF_STD_CALL 
gefVmeReadReg (GEF_VME_BUS_HDL  bus_hdl,
               GEF_UINT32       offset,
               GEF_VME_DWIDTH   width,
               void            *buffer);

GEF_STATUS GEF_STD_CALL 
gefVmeWriteReg (GEF_VME_BUS_HDL  bus_hdl,
		GEF_UINT32       offset,
		GEF_VME_DWIDTH   width,
		void            *buffer);

STATUS taskDelay(int ticks) {
  return usleep(ticks);  
}

int
logMsg(const char *format, ...)
{
  va_list args;
  int retval;

  va_start(args,format);
  retval = vprintf(format, args);
  va_end(args);

  return retval;
}

unsigned long long int rdtsc(void)
{
  /*    unsigned long long int x; */
  unsigned a, d;
   
  __asm__ volatile("rdtsc" : "=a" (a), "=d" (d));

  return ((unsigned long long)a) | (((unsigned long long)d) << 32);;
}

/******************************************************************************
 *
 * vmeOpenA16
 * vmeOpenA24
 * vmeOpenA32
 * vmeOpenA32Blt  - Open the specified VME addressing window
 *
 *  These routines create a VME master window to the specified
 *  address space.  This space is then mapped to the pointer
 *  provide as an argument.
 *
 *  For the A32/A32Blt, the VME Base and Size must also be provided.
 *
 *  RETURNS: N/A
 *
 */

GEF_STATUS 
vmeOpenA16(void *a16p) 
{

  GEF_STATUS status;

  LOCK_TEMPE;

  status = gefVmeCreateMasterWindow(vmeHdl, &addr_A16, 
				    a16_window_width, &a16_hdl);
  if (status != GEF_STATUS_SUCCESS) {
    printf("ERROR: gefVmeCreateMasterWindow A16 failed: code 0x%08x\n",
	   status);
    UNLOCK_TEMPE;
    return -1;
  } 

  /* Now map the VME A16 Window into a local address */
  status = gefVmeMapMasterWindow(a16_hdl, 0, 0x10000, &a16map_hdl, a16p);
  if(status != GEF_STATUS_SUCCESS) {
    printf("\ngefVmeMapMasterWindow (A16) failed: code 0x%08x\n",status);
    gefVmeReleaseMasterWindow(a16_hdl);
    a16p = NULL;
    UNLOCK_TEMPE;
    return -1;
  }

  UNLOCK_TEMPE;

  return 0;
}

GEF_STATUS 
vmeOpenA24(void *a24p) 
{

  GEF_STATUS status;

  LOCK_TEMPE;

  status = gefVmeCreateMasterWindow(vmeHdl, &addr_A24, a24_window_width, &a24_hdl);
  if (status != GEF_STATUS_SUCCESS) {
    printf("ERROR: gefVmeCreateMasterWindow A24 failed: code 0x%08x\n",status);
    UNLOCK_TEMPE;
    return -1;
  } 

  /* Now map the VME A24 Window into a local address */
  status = gefVmeMapMasterWindow(a24_hdl, 0, a24_window_width, &a24map_hdl, a24p);
  if(status != GEF_STATUS_SUCCESS) {
    printf("\ngefVmeMapMasterWindow (A24) failed: code 0x%08x\n",status);
    gefVmeReleaseMasterWindow(a16_hdl);
    a24p = NULL;
    UNLOCK_TEMPE;
    return -1;
  }

  UNLOCK_TEMPE;

  return 0;
}

GEF_STATUS 
vmeOpenA32(unsigned int base, unsigned int size, void *a32p) 
{

  GEF_STATUS status;

  if((size == 0)||(size>A32_MAX_WINDOW_SIZE)) {
    printf("ERROR: vmeOpenA32 : Invalid Window size specified = 0x%x\n",size);
    return -1;
  }

  addr_A32.lower = base;

  LOCK_TEMPE;

  status = gefVmeCreateMasterWindow(vmeHdl, &addr_A32, size, &a32_hdl);
  if (status != GEF_STATUS_SUCCESS) {
    printf("ERROR: gefVmeCreateMasterWindow A32 failed: code 0x%08x\n",status);
    UNLOCK_TEMPE;
    return -1;
  } 

  /* Now map the VME A32 Window into a local address */
  status = gefVmeMapMasterWindow(a32_hdl, 0, size, &a32map_hdl, a32p);
  if(status != GEF_STATUS_SUCCESS) {
    printf("\ngefVmeMapMasterWindow (A32) failed: code 0x%08x\n",status);
    gefVmeReleaseMasterWindow(a32_hdl);
    a32p = NULL;
    UNLOCK_TEMPE;
    return -1;
  }

  UNLOCK_TEMPE;

  return 0;
}

GEF_STATUS 
vmeOpenA32Blt(unsigned int base, unsigned int size, void *a32p) 
{

  GEF_STATUS status;

  if((size == 0)||(size>A32_MAX_WINDOW_SIZE)) {
    printf("ERROR: vmeOpenA32 : Invalid Window size specified = 0x%x\n",size);
    return -1;
  }

  addr_A32.lower = base;

  LOCK_TEMPE;

  status = gefVmeCreateMasterWindow(vmeHdl, &addr_A32blt, size, &a32blt_hdl);
  if (status != GEF_STATUS_SUCCESS) {
    printf("ERROR: gefVmeCreateMasterWindow A32-BLT failed: code 0x%08x\n",status);
    UNLOCK_TEMPE;
    return -1;
  } 

  /* Now map the VME A32 Window into a local address */
  status = gefVmeMapMasterWindow(a32blt_hdl, 0, size, &a32bltmap_hdl, a32p);
  if(status != GEF_STATUS_SUCCESS) {
    printf("\ngefVmeMapMasterWindow (A32-BLT) failed: code 0x%08x\n",status);
    gefVmeReleaseMasterWindow(a32_hdl);
    a32p = NULL;
    UNLOCK_TEMPE;
    return -1;
  }

  UNLOCK_TEMPE;

  return 0;
}

/******************************************************************************
 *
 * vmeCloseA16
 * vmeCloseA24
 * vmeCloseA32
 * vmeCloseA32Blt  - Close the specified VME addressing window
 *
 *  These routines will close the vme address window that was
 *  opened with their vmeOpen counterpart.
 *
 *  RETURNS: GEF_STATUS_SUCCESS if successful,
 *           otherwise, error status from GEF_STATUS_ENUM
 *
 */

GEF_STATUS 
vmeCloseA16()
{

  GEF_STATUS status;

  LOCK_TEMPE;

  /* Unmap A16 address space */
  status = gefVmeUnmapMasterWindow(a16map_hdl);
  if(status != GEF_STATUS_SUCCESS)
    {
      printf("\ngefVmeUnmapMasterWindow (A16) failed: code 0x%08x\n",
	     status);
    }


  /* Release the A16 Master window */
  status = gefVmeReleaseMasterWindow(a16_hdl);
  if (status != GEF_STATUS_SUCCESS)
    {
      printf("ERROR: gefVmeReleaseMasterWindow (A16) failed: code 0x%08x\n",
	     status);
    }
        
  a16_window=NULL;

  UNLOCK_TEMPE;

  return 0;
}

GEF_STATUS 
vmeCloseA24()
{

  GEF_STATUS status;

  LOCK_TEMPE;

  /* Unmap A24 address space */
  status = gefVmeUnmapMasterWindow(a24map_hdl);
  if(status != GEF_STATUS_SUCCESS)
    {
      printf("\ngefVmeUnmapMasterWindow (A24) failed: code 0x%08x\n",
	     status);
    }


  /* Release the A24 Master window */
  status = gefVmeReleaseMasterWindow(a24_hdl);
  if (status != GEF_STATUS_SUCCESS)
    {
      printf("ERROR: gefVmeReleaseMasterWindow (A24) failed: code 0x%08x\n",
	     status);
    }
        

  a24_window=NULL;

  UNLOCK_TEMPE;

  return 0;
}

GEF_STATUS 
vmeCloseA32()
{

  GEF_STATUS status;

  LOCK_TEMPE;

  /* Unmap A32 address space */
  status = gefVmeUnmapMasterWindow(a32map_hdl);
  if(status != GEF_STATUS_SUCCESS)
    {
      printf("\ngefVmeUnmapMasterWindow (A32) failed: code 0x%08x\n",status);
    }


  /* Release the A32 Master window */
  status = gefVmeReleaseMasterWindow(a32_hdl);
  if (status != GEF_STATUS_SUCCESS)
    {
      printf("ERROR: gefVmeReleaseMasterWindow (A32) failed: code 0x%08x\n",
	     status);
    }
        

  a32_window=NULL;

  UNLOCK_TEMPE;

  return 0;
}

GEF_STATUS 
vmeCloseA32Blt()
{

  GEF_STATUS status;

  LOCK_TEMPE;

  /* Unmap A32 address space */
  status = gefVmeUnmapMasterWindow(a32bltmap_hdl);
  if(status != GEF_STATUS_SUCCESS)
    {
      printf("\ngefVmeUnmapMasterWindow (A32-BLT) failed: code 0x%08x\n",
	     status);
    }


  /* Release the A32 Master window */
  status = gefVmeReleaseMasterWindow(a32blt_hdl);
  if (status != GEF_STATUS_SUCCESS)
    {
      printf("ERROR: gefVmeReleaseMasterWindow (A32-BLT) failed: code 0x%08x\n",
	     status);
    }
        

  a32blt_window=NULL;

  UNLOCK_TEMPE;

  return 0;
}

/******************************************************************************
 *
 * vmeOpenDefaultWindows  - Open all default VME addressing windows
 *
 *  This routines will open all VME addressing windows that are standard
 *  to JLab used VME controllers.  
 *
 *  A memory map of the TSI148 (VME bridge chip) is also made available.
 *
 *  RETURNS: GEF_STATUS_SUCCESS if successful,
 *           otherwise, error status from GEF_STATUS_ENUM
 *
 */

GEF_STATUS
vmeOpenDefaultWindows() 
{
  GEF_STATUS status;

  if(didOpen) 
    return 1;

  LOCK_TEMPE;

  status = gefVmeOpen(&vmeHdl);
  if (status != GEF_STATUS_SUCCESS) 
    {
      printf("gefVmeOpen failed: %x\n",status);
      UNLOCK_TEMPE;
      return -1;
    }

  /* Turn off the Debug Flags, by default */
  gefVmeSetDebugFlags(vmeHdl,0x0);

  UNLOCK_TEMPE;

  /* Open A32 windows */
  if(a32_window==NULL) 
    {
      printf("Opening A32 Window ");
      status = vmeOpenA32(0xfb000000,a32_window_width,&a32_window);
    }
  if(status==GEF_STATUS_SUCCESS) 
    {
      printf("(opened at [0x%x,0x%x])\n",
	     (unsigned int)a32_window,(unsigned int)(a32_window+a32_window_width));
    } 
  else 
    {
      printf("... Open Failed!\n");
    }

  if(a32blt_window==NULL) 
    {
      printf("Opening A32Blt Window ");
      status = vmeOpenA32Blt(0x08000000,a32blt_window_width,&a32blt_window);
    }
  if(status==GEF_STATUS_SUCCESS) 
    {
      printf("(opened at [0x%x,0x%x])\n",
	     (unsigned int)a32blt_window,
	     (unsigned int)(a32blt_window+a32blt_window_width));
    } 
  else 
    {
      printf("... Open Failed!\n");
    }

  /* Open A24 window */
  if(a24_window==NULL) 
    {
      printf("Opening A24 Window ");
      status = vmeOpenA24(&a24_window);
    }
  if(status==GEF_STATUS_SUCCESS) 
    {
      printf("(opened at [0x%x,0x%x])\n",
	     (unsigned int)a24_window,
	     (unsigned int)(a24_window+a24_window_width));
    } 
  else 
    {
      printf("... Open Failed!\n");
    }

  /* Open A16 window */
  if(a16_window==NULL) 
    {
      printf("Opening A16 Window ");
      status = vmeOpenA16(&a16_window);
    }
  if(status==GEF_STATUS_SUCCESS) 
    {
      printf("(opened at [0x%x,0x%x])\n",
	     (unsigned int)a16_window,
	     (unsigned int)(a16_window+a16_window_width));
    } 
  else 
    {
      printf("... Open Failed!\n");
    }

  map_tsi();
  didOpen=1;

  return status;
}

/******************************************************************************
 *
 * vmeCloseDefaultWindows  - Close all default VME addressing windows
 *
 *  This routines will close all VME addressing windows that are standard
 *  to JLab used VME controllers.
 *
 *  A memory map of the TSI148 (VME bridge chip) is un-mapped here.
 *
 *  RETURNS: GEF_STATUS_SUCCESS if successful,
 *           otherwise, error status from GEF_STATUS_ENUM
 *
 */

GEF_STATUS
vmeCloseDefaultWindows() 
{
  GEF_STATUS status;
  if(didOpen==0) return 1;

  unmap_tsi();
  
  /* Close A16 window */
  printf("Closing A16 Window\n");
  status = vmeCloseA16();
  if (status != GEF_SUCCESS)
    {
      printf("failed: code 0x%08x\n",status);
      return -1;
    }

  /* Close A24 window */
  printf("Closing A24 Window\n");
  status = vmeCloseA24();
  if (status != GEF_SUCCESS)
    {
      printf("failed: code 0x%08x\n",status);
      return -1;
    }
  
  /* Close A32 windows */
  printf("Closing A32Blt Window\n");
  status = vmeCloseA32Blt();
  if (status != GEF_SUCCESS)
    {
      printf("failed: code 0x%08x\n",status);
      return -1;
    }
  printf("Closing A32 Window\n");
  status = vmeCloseA32();
  if (status != GEF_SUCCESS)
    {
      printf("failed: code 0x%08x\n",status);
      return -1;
    }

  /* Close the VME Device file */
  printf("Calling gefVmeClose\n");
  LOCK_TEMPE;
  status = gefVmeClose(vmeHdl);
  if (status != GEF_SUCCESS)
    {
      printf("gefVmeClose failed: code 0x%08x\n",status);
      UNLOCK_TEMPE;
      return -1;
    }
  UNLOCK_TEMPE;

  didOpen=0;

  return status;
}

/******************************************************************************
 *
 * tempeReadRegister - Read a 32bit register from the tempe memory map.
 *                     "offset" is the offset from the tempe base address.
 *                     A list of macros defining offsets from register names 
 *                     is found in the header file: gefvme_vme_tempe.h
 *
 * RETURNS: if successful, returns 32bit value at the requested register
 *          otherwise, -1.
 *
 */

GEF_UINT32
tempeReadRegister(GEF_UINT32 offset)
{
  GEF_STATUS status;
  GEF_UINT32 temp;

  status = gefVmeReadReg(vmeHdl,offset,GEF_VME_DWIDTH_D32,&temp);
  if (status != GEF_STATUS_SUCCESS) {
    printf("tempeReadRegister failed with error status = %x\n",status);
    return -1;
  } else {
    return temp;
  }
  
}

/******************************************************************************
 *
 * tempeReadRegister - Write a 32bit word ("buffer") to a 32bit register in the
 *                     tempe memory map.  "offset" is the offset from
 *                     the tempe base address.  A list of macros
 *                     defining offsets from register names is found
 *                     in the header file: * gefvme_vme_tempe.h
 *
 * RETURNS: if successful, returns 32bit value at the requested register
 *          otherwise, -1.
 *
 */
GEF_STATUS
tempeWriteRegister(GEF_UINT32 offset, GEF_UINT32 buffer)
{
  GEF_STATUS status;

  LOCK_TEMPE;
  status = gefVmeWriteReg(vmeHdl,offset,GEF_VME_DWIDTH_D32,&buffer);
  if (status != GEF_STATUS_SUCCESS) 
    {
      printf("tempeWriteRegister failed with error status = %x\n",status);
    }

  UNLOCK_TEMPE;
  return status;
  
}

/******************************************************************************
 *
 * tempeDisableBERRIrq() - Disable CPU interrupts from the tempe chip upon
 *                         a VME Bus Error.  
 *
 * You'd typically want to disable VME Bus Error interrupts if you were
 * expecting them... and it's not a real error.
 *
 *  RETURNS: GEF_STATUS_SUCCESS if successful,
 *           otherwise, error status from GEF_STATUS_ENUM
 *
 */

GEF_STATUS
tempeDisableBERRIrq()
{
  GEF_UINT32 tmpCtl;
  GEF_STATUS status;

  tmpCtl = tempeReadRegister(TEMPE_INTEN);
  if(tmpCtl == -1) 
    {
      printf("tempeDisableBERRIrq (TEMPE_INTEN) read failed.");
    }
  
  tmpCtl &= ~BIT_INT_VME_ERR;

  status = tempeWriteRegister(TEMPE_INTEN,tmpCtl);
  if(status != GEF_STATUS_SUCCESS) 
    {
      printf("tempeDisableBERRIrq (TEMPE_INTEN) failed with error status 0x%x\n",
	     status);
      return status;
    }

  status = tempeWriteRegister(TEMPE_INTEO,tmpCtl);
  if(status != GEF_STATUS_SUCCESS) 
    {
      printf("tempeDisableBERRIrq (TEMPE_INTEO) failed with error status 0x%x\n",
	     status);
      return status;
    }

  printf("VME Bus Error IRQ Disabled\n");

  return GEF_STATUS_SUCCESS;
}

/******************************************************************************
 *
 * tempeEnableBERRIrq() - Enable CPU interrupts from the tempe chip upon
 *                         a VME Bus Error.  
 *
 *  RETURNS: GEF_STATUS_SUCCESS if successful,
 *           otherwise, error status from GEF_STATUS_ENUM
 *
 */

GEF_STATUS
tempeEnableBERRIrq()
{
  GEF_UINT32 tmpCtl;
  GEF_STATUS status;

  tmpCtl = tempeReadRegister(TEMPE_INTEN);
  if(tmpCtl == -1) 
    {
      printf("tempeDisableBERRIrq (TEMPE_INTEN) read failed.");
      return -1;
    }

  tmpCtl  |= BIT_INT_VME_ERR;

  status = tempeWriteRegister(TEMPE_INTEN,tmpCtl);
  if(status != GEF_STATUS_SUCCESS) 
    {
      printf("tempeDisableBERRIrq (TEMPE_INTEN) failed with error status 0x%x\n",
	     status);
      return status;
    }

  status = tempeWriteRegister(TEMPE_INTEO,tmpCtl);
  if(status != GEF_STATUS_SUCCESS) 
    {
      printf("tempeDisableBERRIrq (TEMPE_INTEO) failed with error status 0x%x\n",
	     status);
      return status;
    }
  
  printf("VME Bus Error IRQ Enabled\n");

  return GEF_STATUS_SUCCESS;
}

/******************************************************************************
 * 
 * map_tsi - map the tempe register space to user space.
 *          
 * This map is made available through the pTempe pointer.
 *
 *  RETURNS: GEF_STATUS_SUCCESS if successful,
 *           otherwise, error status from GEF_STATUS_ENUM
 *
 */

GEF_STATUS
map_tsi()
{
  GEF_STATUS status;
  GEF_UINT32 offset=0x0;
  unsigned int pci_base = 0x0;
  unsigned int driver_deviveni = 0x0;
  unsigned int map_deviveni = 0x0;

  char *virtual_addr;
  mapHdl = (struct _GEF_VME_MAP_MASTER *) malloc (sizeof (struct _GEF_VME_MAP_MASTER));
  
  if (NULL == mapHdl)
    {
      return(GEF_VME_SET_ERROR(GEF_STATUS_NO_MEM));
    }
  
  memset(mapHdl, 0, sizeof(struct _GEF_VME_MAP_MASTER));

  /* Obtain the base from the PCI-VME Bridge itself */
  pci_base = tempeReadRegister(TEMPE_PCI_MBARL) & 0xfffff000;
  mapHdl->phys_addr = pci_base;

  mapHdl->virt_addr = 0;
  GEF_UINT32 size = 0x7ffff;

  status = gefOsaMap (vmeHdl->osa_intf_hdl,
		      &(mapHdl->osa_map_hdl),
		      &virtual_addr,
		      ((GEF_UINT32)(mapHdl->phys_addr)+offset),
		      size);
  if(status != GEF_STATUS_SUCCESS) 
    {
      printf("map_tsi: ERROR: gefOsaMap returned 0x%x\n",status);
      pTempe = NULL;
      return status;
    }

  pTempe = (tsi148_t *)virtual_addr;

  /* Quick check of the map, compared to a read through the kernel driver */
  driver_deviveni = tempeReadRegister(TEMPE_DEVI_VENI);
  map_deviveni = (SSWAP(pTempe->gcsr.veni)) + ((SSWAP(pTempe->gcsr.devi))<<16);
  
  if(driver_deviveni != map_deviveni) 
    {
      printf("jlabgef: ERROR: map_tsi: kernel map inconsistent with userspace map\n");
      printf("\tdriver_deviveni\t= 0x%x\n\tmap_deviveni\t= 0x%x\n",
	     driver_deviveni,map_deviveni);
    }
  else
    {
      printf("map_tsi: Tempe Chip Userspace map successful\n");
    }

  return status;
}

/******************************************************************************
 * 
 * unmap_tsi - unmap the tempe register space from user space.
 *          
 *  RETURNS: GEF_STATUS_SUCCESS if successful,
 *           otherwise, error status from GEF_STATUS_ENUM
 *
 */

GEF_STATUS
unmap_tsi()
{
  GEF_STATUS status;
  status = gefOsaUnMap(mapHdl->osa_map_hdl);
  if(status != GEF_STATUS_SUCCESS) 
    {
      printf("jlabgef: unmap_tsi ERROR: gefOsaUnMap returned 0x%x\n",status);
    } 
  
  free(mapHdl);
  return status;
}

/******************************************************************************
 * 
 * dmaHdl_to_PhysAddr - Grab the physical address from the GEF DMA handle
 *
 * RETURNS: the 32bit physical address
 *
 */

GEF_UINT32
dmaHdl_to_PhysAddr(GEF_VME_DMA_HDL inpDmaHdl)
{
  /* This returns an UINT32 because we're not using 64bit addresses.. yet */
  return (GEF_UINT32)inpDmaHdl->phys_addr;
}

/******************************************************************************
 * 
 * jlabgefReadDMARegs - Print out a bunch of dma registers from the tempe chip
 *
 * RETURNS: N/A
 *
 */

void
jlabgefReadDMARegs()
{
  int channel=0; 

  printf("\n");

  printf(" dsal = 0x%08x\n",LSWAP(pTempe->lcsr.dma[channel].dsal));
  printf(" dsau = 0x%08x\n",LSWAP(pTempe->lcsr.dma[channel].dsau));
  printf(" ddal = 0x%08x\n",LSWAP(pTempe->lcsr.dma[channel].ddal));
  printf(" ddau = 0x%08x\n",LSWAP(pTempe->lcsr.dma[channel].ddau));
  printf(" dsat = 0x%08x\n",LSWAP(pTempe->lcsr.dma[channel].dsat));
  printf(" ddat = 0x%08x\n",LSWAP(pTempe->lcsr.dma[channel].ddat));
  printf(" dcnt = 0x%08x\n",LSWAP(pTempe->lcsr.dma[channel].dcnt));
  printf(" ddbs = 0x%08x\n",LSWAP(pTempe->lcsr.dma[channel].ddbs));

  printf("dnlal = 0x%08x\n",LSWAP(pTempe->lcsr.dma[channel].dnlal));
  printf("dnlau = 0x%08x\n",LSWAP(pTempe->lcsr.dma[channel].dnlau));


  printf(" dctl = 0x%08x\n",LSWAP(pTempe->lcsr.dma[channel].dctl));

  printf("\n");
}

/******************************************************************************
 * 
 * jlabgefSetupDMALLBuf - Set up the system buffer (mapped to userspace)
 *                        necesary for DMA Linked-List Mode.
 *                        DMA registers for each DMA transaction will
 *                        be stored in this buffer.
 *
 * RETURNS: Status from gefVmeAllocDmaBuf
 *
 */

GEF_STATUS
jlabgefSetupDMALLBuf
(
 )
{
  GEF_STATUS status;
  void *mapPtr;
  int listsize = TSI148_DMA_MAX_LL*sizeof(tsi148DmaDescriptor_t);
  
  /* Allocate some system memory - mapped to userspace*/
  status = gefVmeAllocDmaBuf (vmeHdl, listsize, &dmaLL_hdl, (GEF_MAP_PTR *) &mapPtr);
  if (status != GEF_SUCCESS)
    {
      printf("jlabgefSetupDMALLBuf: ERROR: gefVmeAllocDmaBuf returned %x\n",status);
    }
  
  /* Impose the DmaDescriptor structure on it, TSI148_DMA_MAX_LL times */
  /* I'm still not sure this will work */
  dmaDescList = (tsi148DmaDescriptor_t *)mapPtr;

  return status;
}

/******************************************************************************
 * 
 * jlabgefFreeDMALLBuf - Free up the system buffer allocated by 
 *                       jlabgefSetupDMALLBuf
 *
 * RETURNS: Status from gefVmeFreeDmaBuf
 *
 */

GEF_STATUS
jlabgefFreeDMALLBuf
(
 )
{
  GEF_STATUS status;

  status = gefVmeFreeDmaBuf(dmaLL_hdl);
  if (status != GEF_SUCCESS)
    {
      printf("jlabgefFreeDMALLBuf: ERROR: gefVmeFreeDmaBuf returned %x\n",status);
    }
  
  return status;
}


/******************************************************************************
 * 
 * jlabgefSetupDMA - Set up the tempe chip registers for a DMA transfer
 *
 * RETURNS: 32bit word containing the bits set in the dctl register.
 *
 */

GEF_UINT32
jlabgefSetupDMA
(
 GEF_VME_DMA_HDL inpDmaHdl, /* GEF DMA Handle */
 GEF_VME_ADDR gefVmeAddr,   /* GEF VME Address Structure */
 GEF_UINT32 offset,         /* Offset from PCI base address specified in inpDmaHdl */
 GEF_UINT32 dma_count       /* Maximum number of words in the DMA */
 )
{
  int channel=0; 
  GEF_UINT32 vme_addr=0, pci_addr=0, dsat=0, dcnt=0, dctl=0;
  

  /* Setup the VME address */
  vme_addr = LSWAP(gefVmeAddr.lower);

  /* Setup the PCI address */
  pci_addr = LSWAP(dmaHdl_to_PhysAddr(inpDmaHdl) + offset);

  /* Setup the Source (VME) Attributes */
  dsat |= 0x00000062; /* 32bit Bus Width, Super A32 Mode */
  dsat |= 0x10000000; /* VME Source */

  switch (gefVmeAddr.transfer_mode) /* Set Transfer Mode */
    {
    case GEF_VME_TRANSFER_MODE_BLT:
      dsat |= TSI148_LCSR_DSAT_TM_BLT;
      break;
    case GEF_VME_TRANSFER_MODE_MBLT:
      dsat |= TSI148_LCSR_DSAT_TM_MBLT;
      break;
    case GEF_VME_TRANSFER_MODE_2eVME:
      dsat |= TSI148_LCSR_DSAT_TM_2eVME;
      break;
    case GEF_VME_TRANSFER_MODE_2eSST:
      dsat |= TSI148_LCSR_DSAT_TM_2eSST;
      break;
    case GEF_VME_TRANSFER_MODE_2eSSTB:
      dsat |= TSI148_LCSR_DSAT_TM_2eSSTB;
      break;
    default: /* default is SCT */
      break;
    }

  if(gefVmeAddr.transfer_mode == GEF_VME_TRANSFER_MODE_2eSST) 
    {
      switch (gefVmeAddr.vme_2esst_rate) /* Set 2eSST Rate */
	{
	case GEF_VME_2ESST_RATE_267:
	  dsat |= TSI148_LCSR_DSAT_2eSSTM_267;
	  break;
	case GEF_VME_2ESST_RATE_320:
	  dsat |= TSI148_LCSR_DSAT_2eSSTM_320;
	  break;
	default: /* default is 160 MB/s */
	  break;
	}
    }

  dsat = LSWAP(dsat);
  
  /* Maximum number of DMA words */
  dcnt = LSWAP(dma_count);

  /* Setup the rest of the control bits */
  dctl |= 0x00830000; /* Set Direct mode, VFAR and PFAR by default */

  /* VON value - VME/PCI Block Size */
  switch (gefVmeAddr.flags & 0x000000f0)
    {
    case GEF_VME_DMA_VON_32:
      break;
    case GEF_VME_DMA_VON_64:
      dctl |= 0x1010;
      break;
    case GEF_VME_DMA_VON_128:
      dctl |= 0x2020;
      break;
    case GEF_VME_DMA_VON_256:
      dctl |= 0x3030;
      break;
    case GEF_VME_DMA_VON_512:
      dctl |= 0x4040;
      break;
    case GEF_VME_DMA_VON_1024:
      dctl |= 0x5050;
      break;
    case GEF_VME_DMA_VON_2048:
      dctl |= 0x6060;
      break;
    case GEF_VME_DMA_VON_4096:
      dctl |= 0x7070;
      break;
    default: /* default is 4096 */
      dctl |= 0x7070;
      break;
    }

  /* VOFF value - VMEbus Backoff Timer */
  switch (gefVmeAddr.flags & 0x00000f00)
    {
    case GEF_VME_DMA_VOFF_0us:
      break;
    case GEF_VME_DMA_VOFF_1us:
      dctl |= 0x101;
      break;
    case GEF_VME_DMA_VOFF_2us:
      dctl |= 0x202;
      break;
    case GEF_VME_DMA_VOFF_4us:
      dctl |= 0x303;
      break;
    case GEF_VME_DMA_VOFF_8us:
      dctl |= 0x404;
      break;
    case GEF_VME_DMA_VOFF_16us:
      dctl |= 0x505;
      break;
    case GEF_VME_DMA_VOFF_32us:
      dctl |= 0x606;
      break;
    case GEF_VME_DMA_VOFF_64us:
      dctl |= 0x707;
      break;
    default: /* default is 0us */
      break;
    }

  LOCK_TEMPE;
  pTempe->lcsr.dma[channel].dsal = vme_addr; 
  pTempe->lcsr.dma[channel].dsau = 0x0;
  pTempe->lcsr.dma[channel].ddal = pci_addr;
  pTempe->lcsr.dma[channel].ddau = 0x0;
  pTempe->lcsr.dma[channel].dsat = dsat;
  pTempe->lcsr.dma[channel].ddat = 0x0;
  pTempe->lcsr.dma[channel].dcnt = dcnt;
  pTempe->lcsr.dma[channel].ddbs = 0x0;

  pTempe->lcsr.dma[channel].dctl = LSWAP(dctl);
  UNLOCK_TEMPE;

  return dctl;
}

/******************************************************************************
 * 
 * jlabgefDMAListSetDSAT - Set up the source attributes for the linked
 * list DMA mode
 *
 * INPUT : GEF_UINT32 that has flags defined from GEF_VME_TRANSFER_MODE
 *         and GEF_VME_2ESST_RATE enums.
 *
 * RETURNS: N/A
 *
 */

void
jlabgefDMAListSetDSAT
(
 GEF_UINT32 transfer_mode,
 GEF_UINT32 vme_2esst_rate
)
{
  GEF_UINT32 dsat = 0x0;

  /* Setup the Source (VME) Attributes */
  dsat |= 0x00000062; /* 32bit Bus Width, Super A32 Mode */
  dsat |= 0x10000000; /* VME Source */

  switch (transfer_mode) /* Set Transfer Mode */
    {
    case GEF_VME_TRANSFER_MODE_BLT:
      dsat |= TSI148_LCSR_DSAT_TM_BLT;
      break;
    case GEF_VME_TRANSFER_MODE_MBLT:
      dsat |= TSI148_LCSR_DSAT_TM_MBLT;
      break;
    case GEF_VME_TRANSFER_MODE_2eVME:
      dsat |= TSI148_LCSR_DSAT_TM_2eVME;
      break;
    case GEF_VME_TRANSFER_MODE_2eSST:
      dsat |= TSI148_LCSR_DSAT_TM_2eSST;
      break;
    case GEF_VME_TRANSFER_MODE_2eSSTB:
      dsat |= TSI148_LCSR_DSAT_TM_2eSSTB;
      break;
    default: /* default is SCT */
      break;
    }

  if(transfer_mode == GEF_VME_TRANSFER_MODE_2eSST) 
    {
      switch (vme_2esst_rate) /* Set 2eSST Rate */
	{
	case GEF_VME_2ESST_RATE_267:
	  dsat |= TSI148_LCSR_DSAT_2eSSTM_267;
	  break;
	case GEF_VME_2ESST_RATE_320:
	  dsat |= TSI148_LCSR_DSAT_2eSSTM_320;
	  break;
	default: /* default is 160 MB/s */
	  break;
	}
    }

  dmaList_dsat = dsat;

}

/******************************************************************************
 * 
 * jlabgefDMAListSetDCTL - Set up the DCTL for the linked list mode  DMA
 *
 * RETURNS: N/A
 *
 */

void
jlabgefDMAListSetDCTL
(
 GEF_UINT32 flags
 )
{
  GEF_UINT32 dctl=0x0;

  /* Setup the rest of the control bits */
  dctl |= 0x00030000; /* Set Linked List mode, VFAR and PFAR by default */

  /* VON value - VME/PCI Block Size */
  switch (flags & 0x000000f0)
    {
    case GEF_VME_DMA_VON_32:
      break;
    case GEF_VME_DMA_VON_64:
      dctl |= 0x1010;
      break;
    case GEF_VME_DMA_VON_128:
      dctl |= 0x2020;
      break;
    case GEF_VME_DMA_VON_256:
      dctl |= 0x3030;
      break;
    case GEF_VME_DMA_VON_512:
      dctl |= 0x4040;
      break;
    case GEF_VME_DMA_VON_1024:
      dctl |= 0x5050;
      break;
    case GEF_VME_DMA_VON_2048:
      dctl |= 0x6060;
      break;
    case GEF_VME_DMA_VON_4096:
      dctl |= 0x7070;
      break;
    default: /* default is 4096 */
      dctl |= 0x7070;
      break;
    }

  /* VOFF value - VMEbus Backoff Timer */
  switch (flags & 0x00000f00)
    {
    case GEF_VME_DMA_VOFF_0us:
      break;
    case GEF_VME_DMA_VOFF_1us:
      dctl |= 0x101;
      break;
    case GEF_VME_DMA_VOFF_2us:
      dctl |= 0x202;
      break;
    case GEF_VME_DMA_VOFF_4us:
      dctl |= 0x303;
      break;
    case GEF_VME_DMA_VOFF_8us:
      dctl |= 0x404;
      break;
    case GEF_VME_DMA_VOFF_16us:
      dctl |= 0x505;
      break;
    case GEF_VME_DMA_VOFF_32us:
      dctl |= 0x606;
      break;
    case GEF_VME_DMA_VOFF_64us:
      dctl |= 0x707;
      break;
    default: /* default is 0us */
      break;
    }

  dmaList_dctl = dctl;

}


/******************************************************************************
 * 
 * jlabgefDMAListSetVmeAttr - Set up the VME source attributes for the
 *                            linked list DMA mode
 *
 * INPUT : GEF_VME_ADDR vmeAddr
 *         This routine will only check out the transfer_mode, vme_2esst_rate
 *         and flags items in this structure.
 *
 * RETURNS: N/A
 *
 */

void
jlabgefDMAListSetVmeAttr
(
 GEF_VME_ADDR vmeAddr
)
{
  /* Just make calls to these local routines */
  jlabgefDMAListSetDSAT(vmeAddr.transfer_mode,vmeAddr.vme_2esst_rate);
  jlabgefDMAListSetDCTL(vmeAddr.flags);
}

/******************************************************************************
 * 
 * jlabgefDMAListSet - Set up the linked list for a DMA
 *
 * RETURNS: N/A
 *
 */

void
jlabgefDMAListSet
(
 GEF_UINT32 *vmeAddr,    /* Array of source (vme) addresses */
 GEF_UINT32 *localAddr,  /* Array of destination (system) addresses */
 GEF_UINT32 *dmaSize,    /* Array of DMA sizes (bytes) */
 GEF_UINT32 numt         /* Number of array entries */
 )
{
  unsigned int dmaDesc_phys_base; /* Base address of the list in system memory */
  int idma;
  GEF_UINT32 size_incr=0; /* "current" size of the entire DMA */

  /* check to see if the linked list structure array has been init'd */
  if (dmaDescList == NULL) 
    {
      printf("jlabgefDMAListSet: ERROR: dmaDescList not initialized\n");
      return;
    }

  /* Get the base address (system) of the DMA buffer from the GEF Hdl */
  dmaDesc_phys_base = dmaHdl_to_PhysAddr(dmaLL_hdl);
  

  for (idma=0; idma<numt; idma++) 
    {
      /* Set the vme (source) address */
      dmaDescList[idma].dsau = 0x0;
      dmaDescList[idma].dsal = LSWAP(vmeAddr[idma]);
      
      /* Set the local (destination) address */
      dmaDescList[idma].ddau = 0x0;
      dmaDescList[idma].ddal = LSWAP(localAddr[idma]);
      
      /* Set the byte count */
      dmaDescList[idma].dcnt = LSWAP(dmaSize[idma]);
      size_incr += dmaSize[idma];

      /* Set the address of the next descriptor */
      dmaDescList[idma].dnlau = 0x0;
      if( idma < (numt-1) ) 
	{
	  dmaDescList[idma].dnlal = 
	    LSWAP(dmaDesc_phys_base + (idma+1)*sizeof(tsi148DmaDescriptor_t));
	}
      else
	{
	  dmaDescList[idma].dnlal = LSWAP(TSI148_LCSR_DNLAL_LLA); /* Last Link Address (LLA) */
	}

      /* Still need to set the Source & Destination Attributes, 2eSST Broadcast select */
      /* These we'll assume to be 0x0 for now */
      dmaDescList[idma].ddat = 0x0;
      dmaDescList[idma].ddbs = 0x0;

      /* Get the DSAT from a global, for now */
      dmaDescList[idma].dsat = LSWAP(dmaList_dsat);

/* #define DEBUGLL */
#ifdef DEBUGLL
      printf("dmaDescList[%d]:\n",idma);
      printf("  DSAL = 0x%08x  DDAL = 0x%08x  DCNT = 0x%08x\n",
	     LSWAP(dmaDescList[idma].dsal),
	     LSWAP(dmaDescList[idma].ddal),
	     LSWAP(dmaDescList[idma].dcnt));
      printf(" DNLAL = 0x%08x  DSAT = 0x%08x\n",
	     LSWAP(dmaDescList[idma].dnlal),
	     LSWAP(dmaDescList[idma].dsat));
#endif      

    }

}


/******************************************************************************
 * 
 * jlabgefStartDMA - Set up and Start a DMA transfer.
 *
 * RETURNS: N/A
 *
 */

void
jlabgefStartDMA
(
 GEF_VME_DMA_HDL inpDmaHdl, /* GEF DMA Handle */
 GEF_VME_ADDR gefVmeAddr,   /* GEF VME Address Structure */
 GEF_UINT32 offset,         /* Offset from PCI base address specified in inpDmaHdl */
 GEF_UINT32 dma_count       /* Maximum number of words in the DMA */
)
{
  int channel=0; 
  GEF_UINT32 dctl;

  /* First perfom the setup */
  dctl = jlabgefSetupDMA(inpDmaHdl,gefVmeAddr,offset,dma_count);

  /* Set the "DGO" Bit */
  dctl |= 0x02000000;
  /*   jlabgefReadDMARegs(); */
  LOCK_TEMPE;
  pTempe->lcsr.dma[channel].dctl = LSWAP(dctl);
  UNLOCK_TEMPE;

}


/******************************************************************************
 * 
 * jlabgefStartDMALL - Set up and Start a DMA Linked List mode transfer
 *
 * RETURNS: N/A
 *
 */

void
jlabgefStartDMALL
()
{
  int channel=0; 
  GEF_UINT32 dctl = 0x0; 

  LOCK_TEMPE;
  /* Write the address of the first entry in the linked list to the TEMPE regs */
  pTempe->lcsr.dma[channel].dnlal = LSWAP(dmaHdl_to_PhysAddr(dmaLL_hdl));
  pTempe->lcsr.dma[channel].dnlau = 0x0;

  /* Set the CTL reg */
  pTempe->lcsr.dma[channel].dctl = LSWAP(dmaList_dctl);

  /* Set the "DGO" Bit */
  dctl = dmaList_dctl;
  dctl |= 0x02000000;

  /*   jlabgefReadDMARegs(); */
  pTempe->lcsr.dma[channel].dctl = LSWAP(dctl);
  UNLOCK_TEMPE;

}

/******************************************************************************
 * 
 * jlabgefDMADone - Wait until a DMA finishes or times-out
 *
 * RETURNS: if successful, GEF_STATUS_SUCCESS,
 *          otherwise GEF_STATUS_TIMED_OUT
 *
 */

GEF_STATUS
jlabgefDMADone()
{
  unsigned int val=0,ii=0;
  int channel=0; 
  unsigned int timeout=100;
  unsigned int vmeExceptionAttribute=0;
  GEF_STATUS status = GEF_STATUS_SUCCESS;

  val = LSWAP(pTempe->lcsr.dma[channel].dsta);
  while( ((val&0x1e000000)==0) && (ii<timeout) )
    {
      val = LSWAP(pTempe->lcsr.dma[channel].dsta);
      ii++;
      msync(&pTempe->lcsr.dma[channel].dsta,sizeof(unsigned int),MS_SYNC);
    }

  if(ii>=timeout) 
    {
      printf("jlabgefDMADone: timeout\n");
      status = GEF_STATUS_TIMED_OUT;
    }

  /* check the VME Exception Attribute... clear it if the DMA ended on BERR */
  vmeExceptionAttribute = LSWAP(pTempe->lcsr.veat);
  if(vmeExceptionAttribute & TEMPE_VEAT_BERR) 
    {
      LOCK_TEMPE;
      pTempe->lcsr.veat = LSWAP(TEMPE_VEAT_VESCL);
      UNLOCK_TEMPE;
    }

  return status;
}

/******************************************************************************
 * 
 * jlabgefClearException - Clear any VME Exceptions if they exist
 *
 * RETURNS: N/A
 *
 */

void
jlabgefClearException()
{
  /* check the VME Exception Attribute... clear it, and put out a warning */
  unsigned int vmeExceptionAttribute=0;
  msync(&pTempe->lcsr.veat,sizeof(unsigned int),MS_SYNC);
  vmeExceptionAttribute = LSWAP(pTempe->lcsr.veat);
  if(vmeExceptionAttribute & TEMPE_VEAT_VES) 
    {
      printf("jlabgefClearException: Clearing VME Exception (0x%x) at VME address 0x%x\n",
	     vmeExceptionAttribute,LSWAP(pTempe->lcsr.veal));
      LOCK_TEMPE;
      pTempe->lcsr.veat = LSWAP(TEMPE_VEAT_VESCL);
      msync(&pTempe->lcsr.veat,sizeof(unsigned int),MS_SYNC);
      UNLOCK_TEMPE;
    }      

}

/******************************************************************************
 * 
 * jlabgefClearBERR - Clear any VME Bus Errors, if they exist
 *
 * RETURNS: N/A
 *
 */

void
jlabgefClearBERR()
{
  unsigned int vmeExceptionAttribute=0;

  msync(&pTempe->lcsr.veat,sizeof(unsigned int),MS_SYNC);

  /* check the VME Exception Attribute... clear it, and put out a warning */
  vmeExceptionAttribute = LSWAP(pTempe->lcsr.veat);
  if( (vmeExceptionAttribute & TEMPE_VEAT_VES) && 
     (vmeExceptionAttribute & TEMPE_VEAT_BERR) ) 
    {
      printf("jlabgefClearBERR: Clearing VME BERR (0x%x) at VME address 0x%x\n",
	     vmeExceptionAttribute,LSWAP(pTempe->lcsr.veal));
      LOCK_TEMPE;
      pTempe->lcsr.veat = LSWAP(TEMPE_VEAT_VESCL);
      msync(&pTempe->lcsr.veat,sizeof(unsigned int),MS_SYNC);
      UNLOCK_TEMPE;
    }      

}

/******************************************************************************
 * 
 * jlabgefCheckAddress - Check an address to see if it corresponds to 
 *                       a physical module.
 *
 *  addr = VME address + address_offset (a16_window,a24_window,etc..)
 *
 * RETURNS: 0 if successful,
 *         -1 otherwise
 *
 */

int
jlabgefCheckAddress(char* addr)
{
  int testread;
  unsigned int vmeExceptionAttribute=0;

  /* Clear the Exception register, before trying to read */
  jlabgefClearBERR();

  /* Perform a test read */
  testread = (unsigned int)*addr;
  msync(&testread,sizeof(unsigned int),MS_SYNC);

  /* Check the Exception register for a VME Bus Error */
  msync(&pTempe->lcsr.veat,sizeof(unsigned int),MS_SYNC);
  vmeExceptionAttribute = LSWAP(pTempe->lcsr.veat);
  if( (vmeExceptionAttribute & TEMPE_VEAT_VES) && 
      (vmeExceptionAttribute & TEMPE_VEAT_BERR) ) 
    {
      /* Clear the exception register */
      LOCK_TEMPE;
      pTempe->lcsr.veat = LSWAP(TEMPE_VEAT_VESCL);
      msync(&pTempe->lcsr.veat,sizeof(unsigned int),MS_SYNC);
      UNLOCK_TEMPE;
      return -1;
    }      

  return 0;

}
