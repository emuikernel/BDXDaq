/*----------------------------------------------------------------------------*
 *  Copyright (c) 2009,2010   Southeastern Universities Research Association, *
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
 * SVN: $Rev: 393 $
 *
 *----------------------------------------------------------------------------*/

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdarg.h>
#include <sys/mman.h>
#include "jvme.h"
#include "jlabgef.h"
#include "gef/gefcmn_vme_tempe.h"
#include "gef/gefcmn_osa.h"
#include "gef/gefcmn_vme_framework.h"

#define A32_MAX_WINDOW_SIZE   0xf000000

/* Global pointers to the Userspace windows, available to other
   libraries (define as extern) */
void *a16_window=NULL;
void *a24_window=NULL;
void *a32_window=NULL;
void *a32blt_window=NULL;
void *a32slave_window=NULL;

/* VME address handles for GEF library */
static GEF_VME_MASTER_HDL a16_hdl, a24_hdl, a32_hdl, a32blt_hdl;
static GEF_VME_SLAVE_HDL a32slave_hdl;
static GEF_MAP_HDL a16map_hdl, a24map_hdl, a32map_hdl, a32bltmap_hdl, a32slavemap_hdl;

unsigned int a32_window_width    = 0x00010000;
unsigned int a32blt_window_width = 0x0a000000;
unsigned int a24_window_width    = 0x01000000;
unsigned int a16_window_width    = 0x00010000;

/* Some additional global pointers */
struct _GEF_VME_MAP_MASTER *mapHdl;

extern unsigned int vmeQuietFlag;

pthread_mutex_t tsi_mutex = PTHREAD_MUTEX_INITIALIZER;

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

GEF_VME_ADDR addr_A32slave =
  {
    .upper = 0x00000000,
    .lower = 0x08000000,
    .addr_space = GEF_VME_ADDR_SPACE_A32,
    .addr_mode = GEF_VME_ADDR_MODE_USER | GEF_VME_ADDR_MODE_DATA,
    .transfer_mode = GEF_VME_TRANSFER_MODE_2eSST,
    .transfer_max_dwidth = GEF_VME_TRANSFER_MAX_DWIDTH_32,
    .vme_2esst_rate = GEF_VME_2ESST_RATE_320,
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

/******************************************************************************
 *
 * jlabgefOpenA16
 * jlabgefOpenA24
 * jlabgefOpenA32
 * jlabgefOpenA32Blt  - Open the specified VME addressing window
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
jlabgefOpenA16(void *a16p) 
{

  GEF_STATUS status;

  LOCK_TSI;

  status = gefVmeCreateMasterWindow(vmeHdl, &addr_A16, 
				    a16_window_width, &a16_hdl);
  if (status != GEF_STATUS_SUCCESS) {
    printf("ERROR: gefVmeCreateMasterWindow A16 failed: code 0x%08x\n",
	   status);
    UNLOCK_TSI;
    return -1;
  } 

  /* Now map the VME A16 Window into a local address */
  status = gefVmeMapMasterWindow(a16_hdl, 0, a16_window_width, &a16map_hdl, a16p);
  if(status != GEF_STATUS_SUCCESS) {
    printf("\ngefVmeMapMasterWindow (A16) failed: code 0x%08x\n",status);
    gefVmeReleaseMasterWindow(a16_hdl);
    a16p = NULL;
    UNLOCK_TSI;
    return -1;
  }

  UNLOCK_TSI;

  return 0;
}

GEF_STATUS 
jlabgefOpenA24(void *a24p) 
{

  GEF_STATUS status;

  LOCK_TSI;

  status = gefVmeCreateMasterWindow(vmeHdl, &addr_A24, a24_window_width, &a24_hdl);
  if (status != GEF_STATUS_SUCCESS) {
    printf("ERROR: gefVmeCreateMasterWindow A24 failed: code 0x%08x\n",status);
    UNLOCK_TSI;
    return -1;
  } 

  /* Now map the VME A24 Window into a local address */
  status = gefVmeMapMasterWindow(a24_hdl, 0, a24_window_width, &a24map_hdl, a24p);
  if(status != GEF_STATUS_SUCCESS) {
    printf("\ngefVmeMapMasterWindow (A24) failed: code 0x%08x\n",status);
    gefVmeReleaseMasterWindow(a24_hdl);
    a24p = NULL;
    UNLOCK_TSI;
    return -1;
  }

  UNLOCK_TSI;

  return 0;
}

GEF_STATUS 
jlabgefOpenA32(unsigned int base, unsigned int size, void *a32p) 
{

  GEF_STATUS status;

  if((size == 0)||(size>A32_MAX_WINDOW_SIZE)) {
    printf("ERROR: jlabgefOpenA32 : Invalid Window size specified = 0x%x\n",size);
    return -1;
  }

  addr_A32.lower = base;

  LOCK_TSI;

  status = gefVmeCreateMasterWindow(vmeHdl, &addr_A32, size, &a32_hdl);
  if (status != GEF_STATUS_SUCCESS) {
    printf("ERROR: gefVmeCreateMasterWindow A32 failed: code 0x%08x\n",status);
    UNLOCK_TSI;
    return -1;
  } 

  /* Now map the VME A32 Window into a local address */
  status = gefVmeMapMasterWindow(a32_hdl, 0, size, &a32map_hdl, a32p);
  if(status != GEF_STATUS_SUCCESS) {
    printf("\ngefVmeMapMasterWindow (A32) failed: code 0x%08x\n",status);
    gefVmeReleaseMasterWindow(a32_hdl);
    a32p = NULL;
    UNLOCK_TSI;
    return -1;
  }

  UNLOCK_TSI;

  return 0;
}

GEF_STATUS 
jlabgefOpenA32Blt(unsigned int base, unsigned int size, void *a32p) 
{

  GEF_STATUS status;

  if((size == 0)||(size>A32_MAX_WINDOW_SIZE)) {
    printf("ERROR: jlabgefOpenA32 : Invalid Window size specified = 0x%x\n",size);
    return -1;
  }

  addr_A32blt.lower = base;

  LOCK_TSI;

  status = gefVmeCreateMasterWindow(vmeHdl, &addr_A32blt, size, &a32blt_hdl);
  if (status != GEF_STATUS_SUCCESS) {
    printf("ERROR: gefVmeCreateMasterWindow A32-BLT failed: code 0x%08x\n",status);
    UNLOCK_TSI;
    return -1;
  } 

  /* Now map the VME A32 Window into a local address */
  status = gefVmeMapMasterWindow(a32blt_hdl, 0, size, &a32bltmap_hdl, a32p);
  if(status != GEF_STATUS_SUCCESS) {
    printf("\ngefVmeMapMasterWindow (A32-BLT) failed: code 0x%08x\n",status);
    gefVmeReleaseMasterWindow(a32blt_hdl);
    a32p = NULL;
    UNLOCK_TSI;
    return -1;
  }

  UNLOCK_TSI;

  return 0;
}

GEF_STATUS
jlabgefOpenSlaveA32(unsigned int base, unsigned int size)
{
  GEF_STATUS status;
  UINT32 itat;

  addr_A32slave.lower = base;

  LOCK_TSI;

  status = gefVmeCreateSlaveWindow(vmeHdl, &addr_A32slave, size, &a32slave_hdl);
  if (status != GEF_STATUS_SUCCESS) {
    printf("ERROR: gefVmeCreateSlaveWindow A32-Slave failed: code 0x%08x\n",status);
    UNLOCK_TSI;
    return -1;
  } 

  /* Now map the VME A32 Slave Window into a local address */
  status = gefVmeMapSlaveWindow(a32slave_hdl, 0, size, &a32slavemap_hdl, 
				(GEF_MAP_PTR*)&a32slave_window);
  if(status != GEF_STATUS_SUCCESS) {
    printf("\ngefVmeMapSlaveWindow (A32-Slave) failed: code 0x%08x\n",status);
    gefVmeRemoveSlaveWindow(a32slave_hdl);
    a32slave_window = NULL;
    UNLOCK_TSI;
    return -1;
  }

  /* Set bits in ITAT0, to turn all all types of block transfers.
     Assume that window 0 was the one setup, for the moment */

  itat = jlabgefReadRegister(TEMPE_ITAT(0));
  if(itat & TSI148_LCSR_ITAT_EN)
    {
      itat |= (TSI148_LCSR_ITAT_2eSSTB | TSI148_LCSR_ITAT_2eSST | TSI148_LCSR_ITAT_2eVME | 
	       TSI148_LCSR_ITAT_MBLT | TSI148_LCSR_ITAT_BLT);
      itat |= (TSI148_LCSR_ITAT_SUPR | TSI148_LCSR_ITAT_NPRIV |
	       TSI148_LCSR_ITAT_PGM | TSI148_LCSR_ITAT_DATA);
      pTempe->lcsr.inboundTranslation[0].itat = LSWAP(itat);
    }
  else
    {
      printf("%s: ERROR: Inbound window attributes not modified.  Unexpected window number.\n",
	     __FUNCTION__);
    }

  UNLOCK_TSI;


}

/******************************************************************************
 *
 * jlabgefCloseA16
 * jlabgefCloseA24
 * jlabgefCloseA32
 * jlabgefCloseA32Blt  - Close the specified VME addressing window
 *
 *  These routines will close the vme address window that was
 *  opened with their jlabgefOpen counterpart.
 *
 *  RETURNS: GEF_STATUS_SUCCESS if successful,
 *           otherwise, error status from GEF_STATUS_ENUM
 *
 */

GEF_STATUS 
jlabgefCloseA16()
{

  GEF_STATUS status;

  LOCK_TSI;

  /* Unmap A16 address space */
  status = gefVmeUnmapMasterWindow(a16map_hdl);
  if(status != GEF_STATUS_SUCCESS)
    {
      printf("\ngefVmeUnmapMasterWindow (A16) failed: code 0x%08x\n",
	     status);
      UNLOCK_TSI;
      return -1;
    }


  /* Release the A16 Master window */
  status = gefVmeReleaseMasterWindow(a16_hdl);
  if (status != GEF_STATUS_SUCCESS)
    {
      printf("ERROR: gefVmeReleaseMasterWindow (A16) failed: code 0x%08x\n",
	     status);
      UNLOCK_TSI;
      return -1;
    }
        
  a16_window=NULL;

  UNLOCK_TSI;

  return 0;
}

GEF_STATUS 
jlabgefCloseA24()
{

  GEF_STATUS status;

  LOCK_TSI;

  /* Unmap A24 address space */
  status = gefVmeUnmapMasterWindow(a24map_hdl);
  if(status != GEF_STATUS_SUCCESS)
    {
      printf("\ngefVmeUnmapMasterWindow (A24) failed: code 0x%08x\n",
	     status);
      UNLOCK_TSI;
      return -1;
    }


  /* Release the A24 Master window */
  status = gefVmeReleaseMasterWindow(a24_hdl);
  if (status != GEF_STATUS_SUCCESS)
    {
      printf("ERROR: gefVmeReleaseMasterWindow (A24) failed: code 0x%08x\n",
	     status);
      UNLOCK_TSI;
      return -1;
    }
        

  a24_window=NULL;

  UNLOCK_TSI;

  return 0;
}

GEF_STATUS 
jlabgefCloseA32()
{

  GEF_STATUS status;

  LOCK_TSI;

  /* Unmap A32 address space */
  status = gefVmeUnmapMasterWindow(a32map_hdl);
  if(status != GEF_STATUS_SUCCESS)
    {
      printf("\ngefVmeUnmapMasterWindow (A32) failed: code 0x%08x\n",status);
      UNLOCK_TSI;
      return -1;
    }


  /* Release the A32 Master window */
  status = gefVmeReleaseMasterWindow(a32_hdl);
  if (status != GEF_STATUS_SUCCESS)
    {
      printf("ERROR: gefVmeReleaseMasterWindow (A32) failed: code 0x%08x\n",
	     status);
      UNLOCK_TSI;
      return -1;
    }
        

  a32_window=NULL;

  UNLOCK_TSI;

  return 0;
}

GEF_STATUS 
jlabgefCloseA32Blt()
{

  GEF_STATUS status;

  LOCK_TSI;

  /* Unmap A32 address space */
  status = gefVmeUnmapMasterWindow(a32bltmap_hdl);
  if(status != GEF_STATUS_SUCCESS)
    {
      printf("\ngefVmeUnmapMasterWindow (A32-BLT) failed: code 0x%08x\n",
	     status);
      UNLOCK_TSI;
      return -1;
    }


  /* Release the A32 Master window */
  status = gefVmeReleaseMasterWindow(a32blt_hdl);
  if (status != GEF_STATUS_SUCCESS)
    {
      printf("ERROR: gefVmeReleaseMasterWindow (A32-BLT) failed: code 0x%08x\n",
	     status);
      UNLOCK_TSI;
      return -1;
    }
        

  a32blt_window=NULL;

  UNLOCK_TSI;

  return 0;
}

GEF_STATUS 
jlabgefCloseA32Slave()
{

  GEF_STATUS status;

  LOCK_TSI;

  /* Unmap A32 address space */
  status = gefVmeUnmapSlaveWindow(a32slavemap_hdl);
  if(status != GEF_STATUS_SUCCESS)
    {
      printf("\ngefVmeUnmapMasterWindow (A32-Slave) failed: code 0x%08x\n",
	     status);
      UNLOCK_TSI;
      return -1;
    }


  /* Release the A32 Master window */
  status = gefVmeRemoveSlaveWindow(a32slave_hdl);
  if (status != GEF_STATUS_SUCCESS)
    {
      printf("ERROR: gefVmeRemoveSlaveWindow (A32-Slave) failed: code 0x%08x\n",
	     status);
      UNLOCK_TSI;
      return -1;
    }
        

  a32slave_window=NULL;

  UNLOCK_TSI;

  return 0;
}

/******************************************************************************
 *
 * jlabgefOpenDefaultWindows  - Open all default VME addressing windows
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
jlabgefOpenDefaultWindows() 
{
  GEF_STATUS status;

  if(didOpen) 
    return 1;

  LOCK_TSI;

  status = gefVmeOpen(&vmeHdl);
  if (status != GEF_STATUS_SUCCESS) 
    {
      printf("gefVmeOpen failed: %x\n",status);
      UNLOCK_TSI;
      return -1;
    }

  /* Turn off the Debug Flags, by default */
  gefVmeSetDebugFlags(vmeHdl,0x0);

  UNLOCK_TSI;

  /* Open A32 windows */
  if(a32_window==NULL) 
    {
      if(!vmeQuietFlag)
	printf("Opening A32 Window ");
      status = jlabgefOpenA32(0xfb000000,a32_window_width,&a32_window);
    }
  if(status==GEF_STATUS_SUCCESS) 
    {
      if(!vmeQuietFlag)
	printf("(opened at [0x%x,0x%x])\n",
	       (unsigned int)a32_window,(unsigned int)(a32_window+a32_window_width));
    } 
  else 
    {
      if(!vmeQuietFlag)
	printf("... Open Failed!\n");
    }

  if(a32blt_window==NULL) 
    {
      if(!vmeQuietFlag)
	printf("Opening A32Blt Window ");
      status = jlabgefOpenA32Blt(0x08000000,a32blt_window_width,&a32blt_window);
    }
  if(status==GEF_STATUS_SUCCESS) 
    {
      if(!vmeQuietFlag)
	printf("(opened at [0x%x,0x%x])\n",
	       (unsigned int)a32blt_window,
	       (unsigned int)(a32blt_window+a32blt_window_width));
    } 
  else 
    {
      if(!vmeQuietFlag)
	printf("... Open Failed!\n");
    }

  /* Open A24 window */
  if(a24_window==NULL) 
    {
      if(!vmeQuietFlag)
	printf("Opening A24 Window ");
      status = jlabgefOpenA24(&a24_window);
    }
  if(status==GEF_STATUS_SUCCESS) 
    {
      if(!vmeQuietFlag)
	printf("(opened at [0x%x,0x%x])\n",
	       (unsigned int)a24_window,
	       (unsigned int)(a24_window+a24_window_width));
    } 
  else 
    {
      if(!vmeQuietFlag)
	printf("... Open Failed!\n");
    }

  /* Open A16 window */
  if(a16_window==NULL) 
    {
      if(!vmeQuietFlag)
	printf("Opening A16 Window ");
      status = jlabgefOpenA16(&a16_window);
    }
  if(status==GEF_STATUS_SUCCESS) 
    {
      if(!vmeQuietFlag)
	printf("(opened at [0x%x,0x%x])\n",
	       (unsigned int)a16_window,
	       (unsigned int)(a16_window+a16_window_width));
    } 
  else 
    {
      if(!vmeQuietFlag)
	printf("... Open Failed!\n");
    }

  jlabgefMapTsi();

  /* By JLAB default... disable IRQ on BERR */
  status = jlabgefDisableBERRIrq(1);
  if(status != GEF_STATUS_SUCCESS)
    {
      printf("%s: ERROR Disabling IRQ on BERR",__FUNCTION__);
    }

  /* Create Shared Mutex */
  vmeBusCreateLockShm();

  didOpen=1;

  return status;
}

/******************************************************************************
 *
 * jlabgefCloseDefaultWindows  - Close all default VME addressing windows
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
jlabgefCloseDefaultWindows() 
{
  GEF_STATUS status;
  if(didOpen==0) return 1;

  /* Kill Shared Mutex - Here we just unmap it (don't remove the shm file) */
  vmeBusKillLockShm(0);


  jlabgefUnmapTsi();
  
  /* Close A16 window */
  if(!vmeQuietFlag)
    printf("Closing A16 Window\n");
  status = jlabgefCloseA16();
  if (status != GEF_SUCCESS)
    {
      if(!vmeQuietFlag)
	printf("failed: code 0x%08x\n",status);
      return -1;
    }

  /* Close A24 window */
  if(!vmeQuietFlag)
    printf("Closing A24 Window\n");
  status = jlabgefCloseA24();
  if (status != GEF_SUCCESS)
    {
      if(!vmeQuietFlag)
	printf("failed: code 0x%08x\n",status);
      return -1;
    }
  
  /* Close A32 windows */
  if(!vmeQuietFlag)
    printf("Closing A32Blt Window\n");
  status = jlabgefCloseA32Blt();
  if (status != GEF_SUCCESS)
    {
      if(!vmeQuietFlag)
	printf("failed: code 0x%08x\n",status);
      return -1;
    }
  if(!vmeQuietFlag)
    printf("Closing A32 Window\n");
  status = jlabgefCloseA32();
  if (status != GEF_SUCCESS)
    {
      if(!vmeQuietFlag)
	printf("failed: code 0x%08x\n",status);
      return -1;
    }

  /* Close the VME Device file */
  if(!vmeQuietFlag)
    printf("Calling gefVmeClose\n");
  LOCK_TSI;
  status = gefVmeClose(vmeHdl);
  if (status != GEF_SUCCESS)
    {
      if(!vmeQuietFlag)
	printf("gefVmeClose failed: code 0x%08x\n",status);
      UNLOCK_TSI;
      return -1;
    }
  UNLOCK_TSI;

  didOpen=0;

  return status;
}

/******************************************************************************
 *
 * jlabgefReadRegister - Read a 32bit register from the tempe memory map.
 *                     "offset" is the offset from the tempe base address.
 *                     A list of macros defining offsets from register names 
 *                     is found in the header file: gefvme_vme_tempe.h
 *
 * RETURNS: if successful, returns 32bit value at the requested register
 *          otherwise, -1.
 *
 */

GEF_UINT32
jlabgefReadRegister(GEF_UINT32 offset)
{
  GEF_STATUS status;
  GEF_UINT32 temp;

  status = gefVmeReadReg(vmeHdl,offset,GEF_VME_DWIDTH_D32,&temp);
  if (status != GEF_STATUS_SUCCESS) {
    printf("jlabgefReadRegister failed with error status = %x\n",status);
    return -1;
  } else {
    return temp;
  }
  
}


/******************************************************************************
 *
 * jlabgefWriteRegister - Write a 32bit word ("buffer") to a 32bit register in the
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
jlabgefWriteRegister(GEF_UINT32 offset, GEF_UINT32 buffer)
{
  GEF_STATUS status;

  status = gefVmeWriteReg(vmeHdl,offset,GEF_VME_DWIDTH_D32,&buffer);
  if (status != GEF_STATUS_SUCCESS) 
    {
      printf("jlabgefWriteRegister failed with error status = %x\n",status);
    }

  return status;
  
}

/******************************************************************************
 *
 * jlabgefSysReset - Assert SYSRESET on the VMEbus.
 *
 * RETURNS: if successful, returns GEF_STATUS_SUCCESS
 *          otherwise, error from GEF_STATUS_ENUM.
 *
 */
GEF_STATUS
jlabgefSysReset()
{
  GEF_STATUS status;
  UINT32 read;

  read = jlabgefReadRegister(TEMPE_VCTRL);
  printf("VCTRL = 0x%08x SRESET = 0x%x\n",
	 read,
	 read & TEMPE_VCTRL_SRESET);

  status = gefVmeSysReset(vmeHdl);
  if (status != GEF_STATUS_SUCCESS) 
    {
      printf("%s: ERROR: Failed with error status = %x\n",
	     __FUNCTION__,status);
    }

  read = jlabgefReadRegister(TEMPE_VCTRL);

  /* Wait 200us... SYSRESET should clear by then */
  usleep(200);
  read = jlabgefReadRegister(TEMPE_VCTRL);

  if(read & TEMPE_VCTRL_SRESET)
    {
      /* Not cleared... clear it */
      jlabgefWriteRegister(TEMPE_VCTRL, read & ~TEMPE_VCTRL_SRESET);
    }

  return status;
}

/******************************************************************************
 *
 * jlabgefBERRIrqStatus() - Get the status of interrupts from the tempe chip upon
 *                         a VME Bus Error.  
 *
 *  RETURNS: 1 - IRQ is enabled on BERR
 *           0 - IRQ is disabled
 *          -1 - Error in reading from the INTEN/INTEO register
 *
 */

int
jlabgefBERRIrqStatus()
{
  GEF_UINT32 tmpCtl;
  unsigned int inten_enabled=0, inteo_enabled=0;

  LOCK_TSI;
  tmpCtl = jlabgefReadRegister(TEMPE_INTEN);
  UNLOCK_TSI;
  if(tmpCtl == -1) 
    {
      printf("%s: ERROR TEMPE_INTEN read failed.", __FUNCTION__);
      return ERROR;
    }
  
  /* Check if BERR IRQ is enabled in INTEN */
  if(tmpCtl&BIT_INT_VME_ERR)
    inten_enabled=1;
  else
    inten_enabled=0;

  LOCK_TSI;
  tmpCtl = jlabgefReadRegister(TEMPE_INTEO);
  UNLOCK_TSI;
  if(tmpCtl == -1) 
    {
      printf("%s: ERROR: TEMPE_INTEO read failed.", __FUNCTION__);
      return ERROR;
    }
  
  /* Check if BERR IRQ is enabled in INTEO */
  if(tmpCtl&BIT_INT_VME_ERR)
    inteo_enabled=1;
  else
    inteo_enabled=0;

  if(inten_enabled==inteo_enabled)
    {
      return inten_enabled;
    }
  else
    {
      printf("%s: ERROR: TEMPE_INTEN != TEMPE_INTEO (%d != %d)",
	     __FUNCTION__, inten_enabled, inteo_enabled);
      return ERROR;
    }

}

/******************************************************************************
 *
 * jlabgefDisableBERRIrq() - Disable CPU interrupts from the tempe chip upon
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
jlabgefDisableBERRIrq(int pflag)
{
  GEF_UINT32 tmpCtl;
  GEF_STATUS status;

  LOCK_TSI;
  tmpCtl = jlabgefReadRegister(TEMPE_INTEN);
  UNLOCK_TSI;
  if(tmpCtl == -1) 
    {
      printf("jlabgefDisableBERRIrq (TEMPE_INTEN) read failed.");
    }
  
  tmpCtl &= ~BIT_INT_VME_ERR;

  LOCK_TSI;
  status = jlabgefWriteRegister(TEMPE_INTEN,tmpCtl);
  UNLOCK_TSI;
  if(status != GEF_STATUS_SUCCESS) 
    {
      printf("jlabgefDisableBERRIrq (TEMPE_INTEN) failed with error status 0x%x\n",
	     status);
      return status;
    }

  LOCK_TSI;
  status = jlabgefWriteRegister(TEMPE_INTEO,tmpCtl);
  UNLOCK_TSI;
  if(status != GEF_STATUS_SUCCESS) 
    {
      printf("jlabgefDisableBERRIrq (TEMPE_INTEO) failed with error status 0x%x\n",
	     status);
      return status;
    }

  if(!vmeQuietFlag)
    if(pflag==1)
      printf("VME Bus Error IRQ Disabled\n");

  return GEF_STATUS_SUCCESS;
}

/******************************************************************************
 *
 * jlabgefEnableBERRIrq() - Enable CPU interrupts from the tempe chip upon
 *                         a VME Bus Error.  
 *
 *  RETURNS: GEF_STATUS_SUCCESS if successful,
 *           otherwise, error status from GEF_STATUS_ENUM
 *
 */

GEF_STATUS
jlabgefEnableBERRIrq(int pflag)
{
  GEF_UINT32 tmpCtl;
  GEF_STATUS status;

  LOCK_TSI;
  tmpCtl = jlabgefReadRegister(TEMPE_INTEN);
  UNLOCK_TSI;
  if(tmpCtl == -1) 
    {
      printf("jlabgefEnableBERRIrq (TEMPE_INTEN) read failed.");
      return -1;
    }

  tmpCtl  |= BIT_INT_VME_ERR;

  LOCK_TSI;
  status = jlabgefWriteRegister(TEMPE_INTEN,tmpCtl);
  UNLOCK_TSI;
  if(status != GEF_STATUS_SUCCESS) 
    {
      printf("jlabgefEnableBERRIrq (TEMPE_INTEN) failed with error status 0x%x\n",
	     status);
      return status;
    }

  LOCK_TSI;
  status = jlabgefWriteRegister(TEMPE_INTEO,tmpCtl);
  UNLOCK_TSI;
  if(status != GEF_STATUS_SUCCESS) 
    {
      printf("jlabgefEnableBERRIrq (TEMPE_INTEO) failed with error status 0x%x\n",
	     status);
      return status;
    }
  
  if(!vmeQuietFlag)
    if(pflag==1)
      printf("VME Bus Error IRQ Enabled\n");

  return GEF_STATUS_SUCCESS;
}

/******************************************************************************
 * 
 * jlabgefMapTsi - map the tempe register space to user space.
 *          
 * This map is made available through the pTempe pointer.
 *
 *  RETURNS: GEF_STATUS_SUCCESS if successful,
 *           otherwise, error status from GEF_STATUS_ENUM
 *
 */

GEF_STATUS
jlabgefMapTsi()
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
  pci_base = jlabgefReadRegister(TEMPE_PCI_MBARL) & 0xfffff000;
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
      printf("jlabgefMapTsi: ERROR: gefOsaMap returned 0x%x\n",status);
      pTempe = NULL;
      return status;
    }

  pTempe = (tsi148_t *)virtual_addr;

  /* Quick check of the map, compared to a read through the kernel driver */
  LOCK_TSI;
  driver_deviveni = jlabgefReadRegister(TEMPE_DEVI_VENI);
  map_deviveni = (SSWAP(pTempe->gcsr.veni)) + ((SSWAP(pTempe->gcsr.devi))<<16);
  UNLOCK_TSI;
  
  if(driver_deviveni != map_deviveni) 
    {
      printf("jlabgef: ERROR: jlabgefMapTsi: kernel map inconsistent with userspace map\n");
      printf("\tdriver_deviveni\t= 0x%x\n\tmap_deviveni\t= 0x%x\n",
	     driver_deviveni,map_deviveni);
    }
  else
    {
      if(!vmeQuietFlag)
	printf("jlabgefMapTsi: Tempe Chip Userspace map successful\n");
    }

  return status;
}

/******************************************************************************
 * 
 * jlabgefUnmapTsi - unmap the tempe register space from user space.
 *          
 *  RETURNS: GEF_STATUS_SUCCESS if successful,
 *           otherwise, error status from GEF_STATUS_ENUM
 *
 */

GEF_STATUS
jlabgefUnmapTsi()
{
  GEF_STATUS status;
  status = gefOsaUnMap(mapHdl->osa_map_hdl);
  if(status != GEF_STATUS_SUCCESS) 
    {
      printf("jlabgef: jlabgefUnmapTsi ERROR: gefOsaUnMap returned 0x%x\n",status);
    } 
  
  free(mapHdl);
  return status;
}

/******************************************************************************
 * 
 * jlabgefClearException - Clear any VME Exceptions if they exist
 *
 * INPUT: pflag = 0 for printf output turned off
 *                1 for printf output turned on
 * RETURNS: N/A
 *
 */

void
jlabgefClearException(int pflag)
{
  /* check the VME Exception Attribute... clear it, and put out a warning */
  volatile unsigned int vmeExceptionAttribute=0;
  LOCK_TSI;
  vmeExceptionAttribute = jlabgefReadRegister(TEMPE_VEAT);
  /*   vmeExceptionAttribute = LSWAP(pTempe->lcsr.veat); */
  if(vmeExceptionAttribute & TEMPE_VEAT_VES) 
    {
      if(pflag==1)
	{
	  printf("jlabgefClearException: Clearing VME Exception (0x%x) at VME address 0x%x\n",
		 vmeExceptionAttribute,LSWAP(pTempe->lcsr.veal));
	}
      pTempe->lcsr.veat = LSWAP(TEMPE_VEAT_VESCL);
    }      
  UNLOCK_TSI;

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
  volatile unsigned int vmeExceptionAttribute=0;

  /* check the VME Exception Attribute... clear it, and put out a warning */
  LOCK_TSI;
  vmeExceptionAttribute = jlabgefReadRegister(TEMPE_VEAT);
/*   vmeExceptionAttribute = LSWAP(pTempe->lcsr.veat); */
  if( (vmeExceptionAttribute & TEMPE_VEAT_VES) && 
      ((vmeExceptionAttribute & TEMPE_VEAT_BERR) ||
       (vmeExceptionAttribute & TEMPE_VEAT_2eST)) )
    {
      printf("jlabgefClearBERR: Clearing VME BERR/2eST (0x%x) at VME address 0x%x\n",
	     vmeExceptionAttribute,
	     jlabgefReadRegister(TEMPE_VEAL));
      pTempe->lcsr.veat = LSWAP(TEMPE_VEAT_VESCL);
    }      
  UNLOCK_TSI;

}

/******************************************************************************
 * 
 * jlabgefMemProbe - Probe the specified USERSPACE address (addr) and check 
 *                   for a bus error.  Value read from that address is stored
 *                   in rval.  size is in bytes.
 *
 * RETURNS: 0 if successful,
 *         -1 otherwise
 *
 */

int
jlabgefMemProbe(char* addr, int size, char *retVal)
{
  volatile unsigned int vmeExceptionAttribute=0;
  unsigned int lval; unsigned short sval; char cval;
  int irqOnBerr=0;

  /* If IRQ on BERR is enabled, disable it... enable it again at the end */
  irqOnBerr = jlabgefBERRIrqStatus();
  if(irqOnBerr==1)
    {
      jlabgefDisableBERRIrq(0);
    }

  /* Clear the Exception register, before trying to read */
  jlabgefClearException(0);

  /* Perform a test read */
  switch(size)
    {
    case 4:
      memcpy(&lval,(void *)addr,sizeof(unsigned int));
      lval = LSWAP((unsigned int)lval);
      memcpy(retVal,&lval,sizeof(unsigned int));
      break;
    case 2:
      memcpy(&sval,(void *)addr,sizeof(unsigned short));
      sval = SSWAP((unsigned short)sval);
      memcpy(retVal,&sval,sizeof(unsigned short));
      break;
    case 1:
      memcpy(&cval,(void *)addr,sizeof(char));
      memcpy(retVal,&cval,sizeof(char));
      break;
    default:
      printf("%s: ERROR: Invalid size %d",__FUNCTION__,size);
      return ERROR;
    }
  
  /* Check the Exception register for a VME Bus Error */
  LOCK_TSI;
  vmeExceptionAttribute = jlabgefReadRegister(TEMPE_VEAT);
  if( (vmeExceptionAttribute & TEMPE_VEAT_VES) && 
      (vmeExceptionAttribute & TEMPE_VEAT_BERR) )
    {
      /* Clear the exception register */
      printf("%s: Clearing VME BERR/2eST (0x%x) at VME address 0x%x\n",
	     __FUNCTION__,
	     vmeExceptionAttribute,
	     jlabgefReadRegister(TEMPE_VEAL));
      pTempe->lcsr.veat = LSWAP(TEMPE_VEAT_VESCL);
      UNLOCK_TSI;
      return ERROR;
    }      
  UNLOCK_TSI;

  if(irqOnBerr==1)
    {
      jlabgefEnableBERRIrq(0);
    }

  return OK;

}


/******************************************************************************
 *
 *  THIS ROUTINE IS DEPRECATED.  USE jlabgefMemProbe.
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
  volatile unsigned int testread;
  volatile unsigned int vmeExceptionAttribute=0;
  int irqOnBerr=0;


  /* If IRQ on BERR is enabled, disable it... enable it again at the end */
  irqOnBerr = jlabgefBERRIrqStatus();
  if(irqOnBerr==1)
    {
      jlabgefDisableBERRIrq(0);
    }

  /* Clear the Exception register, before trying to read */
  jlabgefClearException(0);

  /* Perform a test read */
  testread = *addr;

  /* Check the Exception register for a VME Bus Error */
  LOCK_TSI;
  vmeExceptionAttribute = jlabgefReadRegister(TEMPE_VEAT);
/*   vmeExceptionAttribute = LSWAP(pTempe->lcsr.veat); */
  if( (vmeExceptionAttribute & TEMPE_VEAT_VES) && 
      (vmeExceptionAttribute & TEMPE_VEAT_BERR) )
    {
      /* Clear the exception register */
      printf("%s: Clearing VME BERR/2eST (0x%x) at VME address 0x%x\n",
	     __FUNCTION__,
	     vmeExceptionAttribute,
	     jlabgefReadRegister(TEMPE_VEAL));
      pTempe->lcsr.veat = LSWAP(TEMPE_VEAT_VESCL);
      UNLOCK_TSI;
      return ERROR;
    }      
  UNLOCK_TSI;

  if(irqOnBerr==1)
    {
      jlabgefEnableBERRIrq(0);
    }

  return OK;

}

/* Stored array of GEF callback handles. Array size is the number of available int levels */
#define MAX_CB_HDL 25
GEF_CALLBACK_HDL gefCB_hdl[MAX_CB_HDL] = {
  0,0,0,0,0,
  0,0,0,0,0,
  0,0,0,0,0,
  0,0,0,0,0,
  0,0,0,0,0
};

int
jlabgefIntConnect(unsigned int vector,
		  unsigned int level,
		  VOIDFUNCPTR routine,
		  unsigned int arg)
{
  GEF_STATUS status;

  if(level>=MAX_CB_HDL)
    {
      printf("jlabgefIntConnect: ERROR: Interrupt level %d not supported\n",level);
      return ERROR;
    }

  if(gefCB_hdl[level]!=0)
    {
      printf("jlabgefIntConnect: ERROR: Interrupt already connected for level %d\n",level);
      return ERROR;
    }

  status = gefVmeAttachCallback (vmeHdl, level, vector, 
				 routine,(void *)&arg, &gefCB_hdl[level]);

  if (status != GEF_SUCCESS) 
    {
      printf("gefVmeAttachCallback failed: code 0x%08x\n",status);
      return ERROR;
    }
    
  return OK;
}

int
jlabgefIntDisconnect(unsigned int level)
{
  GEF_STATUS status;

  if(level>=MAX_CB_HDL)
    {
      printf("jlabgefIntDisconnect: ERROR: Interrupt level %d not supported\n",level);
      return ERROR;
    }

  if(gefCB_hdl[level]==0)
    {
      printf("jlabgefIntDisconnect: WARN: Interrupt not connected for level %d\n",level);
      return OK;
    }

  status = gefVmeReleaseCallback(gefCB_hdl[level]);
  if (status != GEF_SUCCESS) 
    {
      printf("gefVmeReleaseCallback failed: code 0x%08x\n",status);
      return ERROR;
    } 

  gefCB_hdl[level] = 0;

  return OK;
}

int
jlabgefVmeBusToLocalAdrs(int vmeAdrsSpace,
			 char *vmeBusAdrs,
			 char **pPciAdrs)
{
  unsigned int vmeSpaceMask;
  unsigned int vmeAdrToConvert;
  unsigned int base;
  unsigned int limit;
  unsigned int trans;
  unsigned int busAdrs;
  int adrConverted=0;
  char *pciBusAdrs = 0;

  switch (vmeAdrsSpace)
    {
    case GEF_VME_ADDR_MOD_A32SP:
    case GEF_VME_ADDR_MOD_A32SD:
    case GEF_VME_ADDR_MOD_A32UP:
    case GEF_VME_ADDR_MOD_A32UD:

      /* See if the window is A32 enabled */

      if (a32_window!=NULL || a32blt_window!=NULL)
	{
	  vmeSpaceMask = 0xffffffff;
	  vmeAdrToConvert = (unsigned int)vmeBusAdrs;
	  base = addr_A32.lower;
	  limit = a32_window_width + addr_A32.lower;
	  trans = 0;

	  if ( ((base + trans) <= vmeAdrToConvert) &&
	       ((limit + trans) >= vmeAdrToConvert) ) 
	    {
	      busAdrs = vmeAdrToConvert - base;
	      pciBusAdrs = (char *)((unsigned int)busAdrs + (unsigned int)a32_window);
	      adrConverted = 1;
	      break;
	    }
	  else 
	    {
	      base = addr_A32blt.lower;
	      limit = a32blt_window_width + addr_A32blt.lower;
	      trans = 0;
	      if ( ((base + trans) <= vmeAdrToConvert) &&
		   ((limit + trans) >= vmeAdrToConvert) ) 
		{
		  busAdrs = vmeAdrToConvert - base;
		  pciBusAdrs = (char *)((unsigned int)busAdrs + (unsigned int)a32blt_window);
		  adrConverted = 1;
		  break;
		}
	      
	    }

	  break;
	}

    case GEF_VME_ADDR_MOD_A24SP:
    case GEF_VME_ADDR_MOD_A24SD:
    case GEF_VME_ADDR_MOD_A24UP:
    case GEF_VME_ADDR_MOD_A24UD:

      /* See if the window is A24 enabled */

      if (a24_window!=NULL)
	{
	  vmeSpaceMask = 0x00ffffff;
	  vmeAdrToConvert = (unsigned int)vmeBusAdrs & vmeSpaceMask;
	  base = addr_A24.lower;
	  limit = a24_window_width + addr_A24.lower;
	  trans = 0;
	  if ( ((base + trans) <= vmeAdrToConvert) &&
	       ((limit + trans) >= vmeAdrToConvert) ) 
	    {
	      busAdrs = vmeAdrToConvert - base;
	      pciBusAdrs = (char *)((unsigned int)busAdrs + (unsigned int)a24_window);
	      adrConverted = 1;
	      break;
	    }
	  
	  break;
	}

    case GEF_VME_ADDR_MOD_A16S:
    case GEF_VME_ADDR_MOD_A16U:

      /* See if the window is A24 enabled */

      if (a16_window!=NULL)
	{
	  vmeSpaceMask = 0x0000ffff;
	  vmeAdrToConvert = (unsigned int)vmeBusAdrs & vmeSpaceMask;
	  base = addr_A16.lower;
	  limit = a16_window_width + addr_A16.lower;
	  trans = 0;
	  if ( ((base + trans) <= vmeAdrToConvert) &&
	       ((limit + trans) >= vmeAdrToConvert) ) 
	    {
	      busAdrs = vmeAdrToConvert - base;
	      pciBusAdrs = (char *)((unsigned int)busAdrs + (unsigned int)a16_window);
	      adrConverted = 1;
	      break;
	    }

	  break;
	}

    default:
      return (ERROR);		/* invalid address space */
    }

  if (adrConverted == 1)
    {
      *pPciAdrs = pciBusAdrs;
      return (OK);
    }
    return (ERROR);
    
}

int
jlabgefSetDebugFlags(int flags)
{
  GEF_STATUS status;
  
  status = gefVmeSetDebugFlags(vmeHdl,flags);

  if(status != GEF_SUCCESS)
    {
      printf("gefVmeSetDebugFlags returned 0x%08x\n",status);
      return ERROR;
    }
  
  return OK;
}
