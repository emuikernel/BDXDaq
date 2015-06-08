/******************************************************************************
*
* Sergey June 2009: trying to combile TEST and Dave's new PCI-TI GEN 
*
*******************************************************************************/
#ifndef __PCI_ROL__
#define __PCI_ROL__

#include <stdio.h>
#include <pthread.h>
#include "wdc_defs.h"
#include "wdc_lib.h"
#include "utils.h"
#include "status_strings.h"
#include "tirpci_lib.h"

static int PCI_handlers,PCIflag;
static int PCI_isAsync;
static unsigned int *PCIPollAddr = NULL;
static unsigned int PCIPollMask;
static unsigned int PCIPollValue;
static unsigned long PCI_prescale = 1;
static unsigned long PCI_count = 0;


TIR_STRUCT  ti_pci;
WDC_DEVICE_HANDLE hDev = NULL;
DWORD dwStatus;
pthread_mutex_t tir_mutex = PTHREAD_MUTEX_INITIALIZER;


/*----------------------------------------------------------------------------*/
static DWORD
pcitirInit( )
{
  WD_PCI_SLOT slot;

  /* Initialize the TIRPCI library */
  dwStatus = TIRPCI_LibInit();
  if (WD_STATUS_SUCCESS != dwStatus)
  {
    printf("pcitirInit: Failed to initialize the TIRPCI library: %s",
	     TIRPCI_GetLastErr());
    return dwStatus;
  }

  /* Find and open a TIRPCI device (by default ID) */
  if (!TIRPCI_DeviceFind(TIRPCI_DEFAULT_VENDOR_ID, TIRPCI_DEFAULT_DEVICE_ID, &slot))
  {
    return dwStatus;
  }

  hDev = TIRPCI_DeviceOpen(&slot);
  if(hDev == NULL)
  {
    printf("pcitirInit: Failed to create Device Handle: %s",
	   TIRPCI_GetLastErr());
    return dwStatus;
  }

  /* Reset TIR */
  TIRPCI_Reset(hDev);

  return(WD_STATUS_SUCCESS);
}


static void 
pcitenable(int code, int val)
{
  PCIflag = 1;

#ifndef POLLING_MODE
  printf("PCITI: interrupt mode\n");
#ifdef TRIG_SUPV
  printf("PCITI: trigger supervisor mode\n");
  TIRPCI_Enable(hDev,6);
#else
  printf("PCITI: standalone mode\n");
  TIRPCI_Enable(hDev,7);
#endif
#else
  printf("PCITI: polling mode\n");
#ifdef TRIG_SUPV
  printf("PCITI: trigger supervisor mode\n");
  TIRPCI_Enable(hDev,2);
#else
  printf("PCITI: standalone mode\n");
  TIRPCI_Enable(hDev,3);
#endif
#endif
}

static void 
pcitdisable(int code, int val)
{
  PCIflag = 0;

  TIRPCI_Disable(hDev);
  dwStatus = TIRPCI_IntDisable(hDev);
}

static unsigned long 
pcittype(int code)
{
  unsigned int trigData, ttype=1;
  
  trigData = TIRPCI_Read_tdr(hDev);

#ifdef TRIG_SUPV  
  /*Sergey: use 4 bits only for event type !!!
  ttype = (trigData&0x3f);*/
  ttype = (trigData&0xf);

  syncFlag = (trigData&0x80)>>7;
  lateFail = (trigData&0x40)>>6;
#else
  ttype = trigData&0x7;
#endif
  /*
  printf("pcittype: trigData=0x%08x (type=0x%x syncFlag=0x%x lateFail=0x%x)\n",
    trigData,ttype,syncFlag,lateFail);
  */
  return(ttype);
}

static int 
pcittest(int code)
{
  unsigned int tt=0;
  unsigned int tData = 0xffffffff;
  tData = TIRPCI_Read_csr(hDev);
  /*
printf("pcittest: tData=0x%08x (bit 0x8000 must be set)\n",tData);
sleep(1);
  */
  if(tData == 0xffffffff)
  {
    tt=0;
  }
  else if((tData&TIRPCI_CSR_TRIG_LATCHED))
  {
    tt=1;
	/*printf("pcittest: tData=0x%08x (bit 0x8000 must be set)\n",tData);*/
  }
  else
  {
    tt=0;
  }
  return(tt);


  /*was in TEST
  PCI_count++;
  if(PCIflag && ((PCI_count%PCI_prescale) == 0))
    return(1);
  else
    return(0);
  */
}




/**************************/
/*interrupts-related stuff*/

static void 
pcitriglink(int code, VOIDFUNCPTR isr)
{

  dwStatus = TIRPCI_IntEnable(hDev, isr);

}

static void 
pcitack(int code, int val)
{
  TIRPCI_Ack(hDev);
}

void PCI_int_handler(WDC_DEVICE_HANDLE hTIR, TIRPCI_INT_RESULT *pIntPesult)
{
  theIntHandler(PCI_handlers);                   /* Call our handler */
}

/**************************/
/**************************/




/* define CODA readout list specific routines/definitions */


#define PCI_TEST  pcittest

#define PCI_INIT { PCI_handlers=0; PCI_isAsync = 0;PCIflag = 0;}

#define PCI_ASYNC(code)  {PCI_handlers=1; PCI_isAsync = 1; pcitriglink(code,PCI_int_handler);}

#define PCI_SYNC(code)   {PCI_handlers=1; PCI_isAsync = 0;}

#define PCI_SETA(code) PCIflag = code;

#define PCI_SETS(code) PCIflag = code;

#define PCI_ENA(code,val) pcitenable(code,val);

#define PCI_DIS(code,val) pcitdisable(code,val);

#define PCI_CLRS(code) PCIflag = 0;

#define PCI_TTYPE pcittype

#define PCI_START(val)	 {;}

#define PCI_STOP(val)	 {pcitdisable(val);}

#define PCI_ENCODE(code) (code)




#define PCI_ACK(code,val)   pcitack(code,val);

#endif

