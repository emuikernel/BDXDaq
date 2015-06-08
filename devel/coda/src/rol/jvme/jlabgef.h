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
 * SVN: $Rev: 393 $
 *
 *----------------------------------------------------------------------------*/

#ifndef __JLABGEF__
#define __JLABGEF__

#include <pthread.h>
#include "jvme.h"
#include "tsi148.h"
#include "gef/gefcmn_vme.h"


/* tempe chip mutex */
#define LOCK_TSI {				\
    if(pthread_mutex_lock(&tsi_mutex)<0)	\
      perror("pthread_mutex_lock");		\
  }
#define UNLOCK_TSI {				\
    if(pthread_mutex_unlock(&tsi_mutex)<0)	\
      perror("pthread_mutex_unlock");		\
  }

volatile tsi148_t *pTempe;
GEF_VME_BUS_HDL vmeHdl;

GEF_STATUS jlabgefOpenA16(void *a16p);
GEF_STATUS jlabgefOpenA24(void *a24p);
GEF_STATUS jlabgefOpenA32(unsigned int base, unsigned int size, void *a32p);
GEF_STATUS jlabgefOpenA32Blt(unsigned int base, unsigned int size, void *a32p);
GEF_STATUS jlabgefOpenSlaveA32(unsigned int base, unsigned int size);
GEF_STATUS jlabgefCloseA16();
GEF_STATUS jlabgefCloseA24();
GEF_STATUS jlabgefCloseA32();
GEF_STATUS jlabgefCloseA32Blt();
GEF_STATUS jlabgefCloseA32Slave();
GEF_STATUS jlabgefOpenDefaultWindows();
GEF_STATUS jlabgefCloseDefaultWindows();
GEF_UINT32 jlabgefReadRegister(GEF_UINT32 offset);
GEF_STATUS jlabgefWriteRegister(GEF_UINT32 offset, GEF_UINT32 buffer);
GEF_STATUS jlabgefSysReset();
int        jlabgefBERRIrqStatus();
GEF_STATUS jlabgefDisableBERRIrq(int pflag);
GEF_STATUS jlabgefEnableBERRIrq(int pflag);
GEF_STATUS jlabgefMapTsi();
GEF_STATUS jlabgefUnmapTsi();

void jlabgefClearException(int pflag);
void jlabgefClearBERR();
int jlabgefMemProbe(char* addr, int size, char *rval);
int jlabgefCheckAddress(char *addr);
int jlabgefIntConnect(unsigned int vector, unsigned int level, VOIDFUNCPTR routine, unsigned int arg);
int jlabgefIntDisconnect(unsigned int level);
int jlabgefVmeBusToLocalAdrs(int vmeAdrsSpace, char *vmeBusAdrs, char **pPciAdrs);
int jlabgefSetDebugFlags(int flags);


#endif /* __JLABGEF__ */
