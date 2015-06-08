/*----------------------------------------------------------------------------*
 *  Copyright (c) 2010        Southeastern Universities Research Association, *
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
 *     A front for the stuff that actually does the work.
 *      APIs are switched from the Makefile
 *
 * SVN: $Rev: 393 $
 *
 *----------------------------------------------------------------------------*/

/* This is required for using mutex robust-ness */
#define _GNU_SOURCE

#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdarg.h>
#include <pthread.h>
#include <errno.h>
#include "jvme.h"

#ifdef GEFANUC
#include "jlabgef.h"
#include "jlabgefDMA.h"
#endif

/* global variables */
unsigned int vmeQuietFlag=0;

STATUS taskDelay(int ticks) {
  return usleep(16700*ticks);  
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

unsigned long long int 
rdtsc(void)
{
  /*    unsigned long long int x; */
  unsigned a, d;
   
  __asm__ volatile("rdtsc" : "=a" (a), "=d" (d));

  return ((unsigned long long)a) | (((unsigned long long)d) << 32);;
}

void
vmeSetQuietFlag(unsigned int pflag)
{
  if(pflag <= 1)
    vmeQuietFlag = pflag;
  else
    printf("%s: ERROR: invalid argument pflag=%d\n",
	   __FUNCTION__,pflag);
}

int
vmeOpenDefaultWindows()
{
  int status;

#ifdef GEFANUC
  status = (int)jlabgefOpenDefaultWindows();
#endif

  return status;
}

int
vmeCloseDefaultWindows()
{
  int status;

#ifdef GEFANUC
  status = (int)jlabgefCloseDefaultWindows();
#endif

  return status;
}

int
vmeOpenSlaveA32(unsigned int base, unsigned int size)
{
  unsigned int rval;

#ifdef GEFANUC
  rval = (unsigned int)jlabgefOpenSlaveA32(base, size);
#endif

  return rval;
}

int
vmeCloseA32Slave()
{
  unsigned int rval;

#ifdef GEFANUC
  rval = (unsigned int)jlabgefCloseA32Slave();
#endif

  return rval;
}

unsigned int
vmeReadRegister(unsigned int offset)
{
  unsigned int rval;

#ifdef GEFANUC
  rval = (unsigned int)jlabgefReadRegister(offset);
#endif

  return rval;
}

int
vmeWriteRegister(unsigned int offset, unsigned int buffer)
{
  int status;

#ifdef GEFANUC
  status = (int)jlabgefWriteRegister(offset,buffer);
#endif

  return status;
}

int
vmeSysReset()
{
  int status;

#ifdef GEFANUC
  status = (int)jlabgefSysReset();
#endif

  return status;
}

int
vmeBERRIrqStatus()
{
  int status;

#ifdef GEFANUC
  status = (int)jlabgefBERRIrqStatus();
#endif
  
  return status;
}

int
vmeDisableBERRIrq(int pflag)
{
  int status;

#ifdef GEFANUC
  status = (int)jlabgefDisableBERRIrq(pflag);
#endif
  
  return status;
}

int
vmeEnableBERRIrq(int pflag)
{
  int status;

#ifdef GEFANUC
  status = (int)jlabgefEnableBERRIrq(pflag);
#endif
  
  return status;
}

int
vmeMemProbe(char *addr, int size, char *rval)
{
  int status;

#ifdef GEFANUC
  status = jlabgefMemProbe(addr, size, rval);
#endif

  return status;
}

void
vmeClearException(int pflag)
{
#ifdef GEFANUC
  jlabgefClearException(pflag);
#endif
}


int
vmeIntConnect(unsigned int vector, unsigned int level, VOIDFUNCPTR routine, unsigned int arg)
{
  int status;

#ifdef GEFANUC
  status = (int)jlabgefIntConnect(vector, level, routine, arg);
#endif
  
  return status;

}

int
vmeIntDisconnect(unsigned int level)
{
  int status;

#ifdef GEFANUC
  status = (int)jlabgefIntDisconnect(level);
#endif
  
  return status;

}

int
vmeBusToLocalAdrs(int vmeAdrsSpace, char *vmeBusAdrs, char **pPciAdrs)
{
  int status;

#ifdef GEFANUC
  status = (int)jlabgefVmeBusToLocalAdrs(vmeAdrsSpace, vmeBusAdrs, pPciAdrs);
#endif
  
  return status;

}

int
vmeSetDebugFlags(int flags)
{
  int status;

#ifdef GEFANUC
  status = jlabgefSetDebugFlags(flags);
#endif

  return status;

}





/*!
  Routine to change the address modifier of the A24 Outbound VME Window.
  The A24 Window must be opened, prior to calling this routine.

  @param addr_mod Address modifier to be used.  If 0, the default (0x39) will be used.

  @return 0, if successful. -1, otherwise
*/

int
vmeSetA24AM(int addr_mod)
{
  int status;

#ifdef GEFANUC
  status = jlabgefSetA24AM(addr_mod);
#endif

  return status;
}





/* DMA SUBROUTINES */

int
vmeDmaConfig(unsigned int addrType, unsigned int dataType, unsigned int sstMode)
{
  int retVal;

#ifdef GEFANUC
  retVal = jlabgefDmaConfig(addrType,dataType,sstMode);
#endif

  return retVal;
}

int
vmeDmaSend(unsigned int locAdrs, unsigned int vmeAdrs, int size)
{
  int status;

#ifdef GEFANUC
  status = jlabgefDmaSend(locAdrs,vmeAdrs,size);
#endif

  return status;

}

int
vmeDmaDone()
{
  int retVal;

#ifdef GEFANUC
  retVal = jlabgefDmaDone();
#endif

  return retVal;
}

int  
vmeDmaAllocLLBuffer()
{
  int retVal;
  
#ifdef GEFANUC
  retVal = jlabgefDmaAllocLLBuffer();
#endif

  return retVal;
}

int
vmeDmaFreeLLBuffer()
{
  int retVal;

#ifdef GEFANUC
  retVal = jlabgefDmaFreeLLBuffer();
#endif

  return retVal;
}

void 
vmeDmaSetupLL(unsigned int locAddrBase,unsigned int *vmeAddr,
	      unsigned int *dmaSize,unsigned int numt)
{

#ifdef GEFANUC
  jlabgefDmaSetupLL(locAddrBase,vmeAddr,dmaSize,numt);
#endif

}

void
vmeDmaSendLL()
{

#ifdef GEFANUC
  jlabgefDmaSendLL();
#endif

}

void
vmeReadDMARegs()
{

#ifdef GEFANUC
  jlabgefReadDMARegs();
#endif

}

/* API independent code here */

/* Shared Robust Mutex for vme bus access */
char* shm_name_vmeBus = "/vmeBus";
/* Keep this as a structure, in case we want to add to it in the future */
struct shared_memory_mutex
{
    pthread_mutex_t mutex;
};
struct shared_memory_mutex *p_sync=NULL;
/* mmap'd address of shared memory mutex */
void *addr_shm = NULL;

int
vmeBusCreateLockShm()
{
  int fd_shm;
  pthread_mutexattr_t m_attr;
  int needMutexInit=0;

  /* First check to see if the file already exists */
  fd_shm = shm_open(shm_name_vmeBus, O_RDWR, S_IRUSR|S_IWUSR);
  if(fd_shm<0)
    {
      /* Bad file handler.. */
      if(errno == ENOENT)
	{
	  needMutexInit=1;
	}
      else
	{
	  perror("shm_open");
	  printf(" %s: ERROR: Unable to open shared memory\n",__FUNCTION__);
	  return ERROR;
	}
    }

  if(needMutexInit)
    {
      /* Create and map 'mutex' shared memory */
      fd_shm = shm_open(shm_name_vmeBus, O_CREAT|O_RDWR, S_IRUSR|S_IWUSR);
      if(fd_shm<0)
	{
	  perror("shm_open");
	  printf(" %s: ERROR: Unable to open shared memory\n",__FUNCTION__);
	  return ERROR;
	}
      ftruncate(fd_shm, sizeof(struct shared_memory_mutex));
    }

  addr_shm = mmap(0, sizeof(struct shared_memory_mutex), PROT_READ|PROT_WRITE, MAP_SHARED, fd_shm, 0);
  if(addr_shm<0) {
    perror("mmap");
    printf("%s: ERROR: Unable to mmap shared memory\n",__FUNCTION__);
    return ERROR;
  }
  p_sync = addr_shm;

  if(needMutexInit)
    {
      if(pthread_mutexattr_init(&m_attr)<0) 
	{
	  perror("pthread_mutexattr_init");
	  printf("%s: ERROR:  Unable to initialized mutex attribute\n",__FUNCTION__);
	  return ERROR;
	}
      if(pthread_mutexattr_setpshared(&m_attr, PTHREAD_PROCESS_SHARED)<0) 
	{
	  perror("pthread_mutexattr_setpshared");
	  printf("%s: ERROR:  Unable to set shared attribute\n",__FUNCTION__);
	  return ERROR;
	}
      if(pthread_mutexattr_setrobust_np(&m_attr, PTHREAD_MUTEX_ROBUST_NP)<0)
	{
	  perror("pthread_mutexattr_setrobust_np");
	  printf("%s: ERROR:  Unable to set shared attribute\n",__FUNCTION__);
	  return ERROR;
	}
      if(pthread_mutex_init(&(p_sync->mutex), &m_attr)<0)
	{
	  perror("pthread_mutex_init");
	  printf("%s: ERROR:  Unable to initialize shared mutex\n",__FUNCTION__);
	  return ERROR;
	}
      if(pthread_mutexattr_destroy(&m_attr)<0)
	{
	  perror("pthread_mutexattr_destroy");
	  printf("%s: ERROR:  Unable to destroy mutex attribute\n",__FUNCTION__);
	  return ERROR;
	}
    }

  if(!vmeQuietFlag)
    printf("%s: vmeBus shared memory mutex created\n",__FUNCTION__);
  return OK;
}

int
vmeBusKillLockShm(int kflag)
{
  if(munmap(addr_shm, sizeof(struct shared_memory_mutex))<0)
    perror("munmap");
  if(kflag==1)
    {
      if(shm_unlink(shm_name_vmeBus)<0) 
	perror("shm_unlink");
      printf("%s: vmeBus shared memory mutex destroyed\n",__FUNCTION__);
    }
  return OK;
}

int
vmeBusLock()
{
  int rval;

  if(p_sync!=NULL)
    {
      rval = pthread_mutex_lock(&(p_sync->mutex));
      if(rval<0) 
	{
	  perror("pthread_mutex_lock");
	  printf("%s: ERROR locking vmeBus\n",__FUNCTION__);
	  return ERROR;
	}
      if(rval==EOWNERDEAD)
	{
	  printf("%s: WARN: Previous owner of vmeBus (mutex) died unexpectedly\n",
		 __FUNCTION__);
	  printf("  Attempting to recover..\n");
	  if(pthread_mutex_consistent_np(&(p_sync->mutex))<0)
	    perror("pthread_mutex_consistent_np");
	}
      if(rval==ENOTRECOVERABLE)
	{
	  printf("%s: ERROR: vmeBus mutex in an unrecoverable state!\n",
		 __FUNCTION__);
	  return ERROR;
	}
    }
  else
    {
      printf("%s: ERROR: vmeBusLock not initialized.\n",__FUNCTION__);
      return ERROR;
    }
  return OK;
}

int
vmeBusUnlock()
{
  if(p_sync!=NULL)
    {
      if(pthread_mutex_unlock(&(p_sync->mutex))<0) 
	{
	  perror("pthread_mutex_unlock");
	  printf("%s: ERROR unlocking vmeBus\n",__FUNCTION__);
	  return ERROR;
	}
    }
  else
    {
      printf("%s: ERROR: vmeBusUnlock not initialized.\n",__FUNCTION__);
      return ERROR;
    }
  return OK;
}

