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
 *     Header for JLab extra routines to compliment the GEFanuc API
 *
 *----------------------------------------------------------------------------*/

#ifndef __JLABGEF__
#define __JLABGEF__

#include "gef/gefcmn_vme.h"

#define UINT32 GEF_UINT32
#define STATUS int
#define TRUE  1
#define FALSE 0
#define OK    0
#define ERROR -1
/* #define BOOL unsigned int */
#define LOCAL 
/* #define VOIDFUNCPTR void * */
#ifndef _ROLDEFINED
typedef void            (*VOIDFUNCPTR) ();
#endif
typedef char            BOOL;

#define LSWAP(x)        ((((x) & 0x000000ff) << 24) | \
                         (((x) & 0x0000ff00) <<  8) | \
                         (((x) & 0x00ff0000) >>  8) | \
                         (((x) & 0xff000000) >> 24))

#define SSWAP(x)        ((((x) & 0x00ff) << 8) | \
                         (((x) & 0xff00) >> 8))

GEF_VME_BUS_HDL vmeHdl;

/* Routine prototypes */
STATUS taskDelay(int ticks);
int logMsg(const char *format, ...);
unsigned long long int rdtsc(void);
GEF_STATUS vmeOpenA16(void *a16p);
GEF_STATUS vmeOpenA24(void *a24p);
GEF_STATUS vmeOpenA32(unsigned int base, unsigned int size, void *a32p);
GEF_STATUS vmeOpenA32Blt(unsigned int base, unsigned int size, void *a32p);
GEF_STATUS vmeCloseA16();
GEF_STATUS vmeCloseA24();
GEF_STATUS vmeCloseA32();
GEF_STATUS vmeCloseA32Blt();
GEF_STATUS vmeOpenDefaultWindows();
GEF_STATUS vmeCloseDefaultWindows();
GEF_UINT32 tempeReadRegister(GEF_UINT32 offset);
GEF_STATUS tempeDisableBERRIrq();
GEF_STATUS tempeEnableBERRIrq();
void jlabgefReadDMARegs();
void jlabgefStartDMA(GEF_VME_DMA_HDL inpDmaHdl,GEF_VME_ADDR gefVmeAddr,
		     GEF_UINT32 offset,GEF_UINT32 dma_count);
GEF_STATUS jlabgefDMADone();
void jlabgefClearException();
void jlabgefClearBERR();
int jlabgefCheckAddress(char *addr);
GEF_STATUS jlabgefSetupDMALLBuf();
GEF_STATUS jlabgefFreeDMALLBuf();
void jlabgefDMAListSetVmeAttr (GEF_VME_ADDR vmeAddr);
void jlabgefDMAListSet(GEF_UINT32 *vmeAddr, GEF_UINT32 *localAddr, GEF_UINT32 *dmaSize, GEF_UINT32 numt);
void jlabgefStartDMALL();
GEF_UINT32 dmaHdl_to_PhysAddr(GEF_VME_DMA_HDL inpDmaHdl);




#endif
