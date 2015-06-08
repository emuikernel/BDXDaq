/* v1495.c - CAEN v1495 logic unit library



    --------------------------------------------------------------------------

                   --- CAEN SpA - Front End Electronics  ---

    --------------------------------------------------------------------------

    Name        :   V1495Upgrade.c

    Project     :   V1495Upgrade

    Description :   V1495 Upgrade Software


	  This program writes the configuration file (Altera RBF Format) into the
	  flash memory of the Module V1495. This allows to upgrade the firmware
	  for either the VME_INT and the USER fpga from the VME.
	  The program is based on the CAEN Bridge (V1718 or V2718).
	  The software can be compiled for other VME CPUs, changing the functions
      in the custom VME functions (VME_Init(),VME_Write(), VME_Read()).
      Comment away CAENVMElib.h include in this case.

    Date        :   March 2006
    Release     :   1.0
    Author      :   C.Tintori

    --------------------------------------------------------------------------


    --------------------------------------------------------------------------
*/

/*

  Revision History:
  1.0    CT     First release
  1.1    LC     USER Flash page map changed. Only STD update allowed.

Adjustment for Motorola VME controllers: Sergey Boyarinov April 23 2007

*/

#ifdef VXWORKS


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#include <vxWorks.h>

#include "v1495.h"

/*sergey*/
#define EIEIO    __asm__ volatile ("eieio")
#define SYNC     __asm__ volatile ("sync")

/* Parameters for the access to the Flash Memory */
#define VME_FIRST_PAGE_STD    768    /* first page of the image STD */
#define VME_FIRST_PAGE_BCK    1408   /* first page of the image BCK */
#define USR_FIRST_PAGE_STD    48     /* first page of the image STD */
#define USR_FIRST_PAGE_BCK    1048   /* first page of the image BCK */
#define PAGE_SIZE       264 /* Number of bytes per page in the target flash */

/* flash memory Opcodes */
#define MAIN_MEM_PAGE_READ          0x00D2
#define MAIN_MEM_PAGE_PROG_TH_BUF1  0x0082


/*Andrea*/
static unsigned short vmeRead16(volatile unsigned short *addr);
static unsigned int vmeRead32(volatile unsigned int *addr);
static void vmeWrite16(volatile unsigned short *addr, unsigned short val);
static void vmeWrite32(volatile unsigned int *addr, unsigned int val);


/****************************************************************************
 write_flash_page
    flag=0 for USER flash (default)
        =1 for VME flash
****************************************************************************/
int
write_flash_page1(unsigned int addr, unsigned char *page, int pagenum, int flag)
{
  volatile V1495 *v1495 = (V1495 *) addr;
  int i, flash_addr, data;
  unsigned char addr0, addr1, addr2;
  int res = 0;
  unsigned short *Sel_Flash; /* VME Address of the FLASH SELECTION REGISTER */
  unsigned short *RW_Flash;  /* VME Address of the FLASH Read/Write REGISTER */

  if(flag==1)
  {
    Sel_Flash = (short *)&(v1495->selflashVME);
    RW_Flash = (short *)&(v1495->flashVME);
  }
  else
  {
    Sel_Flash = (short *)&(v1495->selflashUSER);
    RW_Flash = (short *)&(v1495->flashUSER);
  }

  EIEIO;
  SYNC;
  flash_addr = pagenum << 9;
  addr0 = (unsigned char)flash_addr;
  addr1 = (unsigned char)(flash_addr>>8);
  addr2 = (unsigned char)(flash_addr>>16);

  EIEIO;
  SYNC;
  /* enable flash (NCS = 0) */
  data = 0;
  *Sel_Flash = data;

  EIEIO;
  SYNC;
  /* write opcode */
  data = MAIN_MEM_PAGE_PROG_TH_BUF1;
  *RW_Flash = data;

  EIEIO;
  SYNC;
  /* write address */
  data = addr2;
  *RW_Flash = data;
  data = addr1;
  *RW_Flash = data;
  data = addr0;
  *RW_Flash = data;

  EIEIO;
  SYNC;
  /* write flash page */
  for(i=0; i<PAGE_SIZE; i++)
  {
    data = page[i];
    *RW_Flash = data;
  }

  EIEIO;
  SYNC;
  /* wait 20ms (max time required by the flash to complete the writing) */
  taskDelay(1/*10*/); /* 1 tick = 10ms */

  EIEIO;
  SYNC;
  /* disable flash (NCS = 1) */
  data = 1;
  *Sel_Flash = data;


  EIEIO;
  SYNC;
  /* wait 20ms (max time required by the flash to complete the writing) */
  taskDelay(2/*20*/);
  EIEIO;
  SYNC;

  return(res);
}

/****************************************************************************
 read_flash_page
****************************************************************************/
int
read_flash_page1(unsigned int addr, unsigned char *page, int pagenum, int flag)
{
  volatile V1495 *v1495 = (V1495 *) addr;
  int i, flash_addr, data;
  /*volatile*/ unsigned short data16;
  unsigned char addr0,addr1,addr2;
  int res = 0;
  unsigned short *Sel_Flash; /* VME Address of the FLASH SELECTION REGISTER */
  unsigned short *RW_Flash;  /* VME Address of the FLASH Read/Write REGISTER */

  if(flag==1)
  {
    Sel_Flash = (short *)&(v1495->selflashVME);
    RW_Flash = (short *)&(v1495->flashVME);
  }
  else
  {
    Sel_Flash = (short *)&(v1495->selflashUSER);
    RW_Flash = (short *)&(v1495->flashUSER);
  }

  EIEIO;
  SYNC;
  flash_addr = pagenum << 9;
  addr0 = (unsigned char)flash_addr;
  addr1 = (unsigned char)(flash_addr>>8);
  addr2 = (unsigned char)(flash_addr>>16);

  EIEIO;
  SYNC;
  /* enable flash (NCS = 0) */
  data = 0;
  *Sel_Flash = data;


  EIEIO;
  SYNC;
  /* write opcode */
  data = MAIN_MEM_PAGE_READ;
  *RW_Flash = data;



  EIEIO;
  SYNC;
  /* write address */
  data = addr2;
  *RW_Flash = data;
  data = addr1;
  *RW_Flash = data;
  data = addr0;
  *RW_Flash = data;


  EIEIO;
  SYNC;
  /* additional don't care bytes */
  data = 0;
  for(i=0; i<4; i++)
  {
    *RW_Flash = data;
  }





  EIEIO;
  SYNC;
  /* read flash page */
  for(i=0; i<PAGE_SIZE; i++)
  {
    data16 = *RW_Flash;
    page[i] = (unsigned char)data16;
  }
  EIEIO;
  SYNC;

  /* disable flash (NCS = 1) */
  data = 1;
  *Sel_Flash = data;
  EIEIO;
  SYNC;

  return(res);
}

/*

  example:

ld < /usr/local/clas/devel/coda/src/rol/VXWORKS_ppc/obj/v1495.o

cd "/usr/local/clas/devel/coda/src/rol/code.s"

v1495firmware(0xfa510000,"v1495USER1.0.rbf",0,0)
v1495firmware(0xfa510000,"v1495vme04.rbf",0,1)
v1495firmware(0xfa510000,"HallBTrigger.rbf",0,0)

v1495firmware(0xf0410000,"v1495vme04.rbf",0,1)
v1495firmware(0xf0410000,"HallBTrigger_V2_1.rbf",0,0)

v1495firmware(0xfa510000,"HallBFlexTrigger.rbf",0,0) - clastrig2
v1495firmware(0xf0410000,"HallBFlexTrigger.rbf",0,0) - clastrig2

v1495firmware(0xfa510000,"HallBFlexTrigger_V3_2.rbf",0,0) - croctest10
v1495firmware(0xf0410000,"HallBFlexTrigger_V3_2.rbf",0,0) - clastrig2



_PHOTON triggers

v1495firmware(0xfa510000,"HallBFlexTrigger_March2010_Rev4_2.rbf",0,0) - croctest10
v1495firmware(0xf0410000,"HallBFlexTrigger_March2010_Rev4_2.rbf",0,0) - clastrig2



_NOV2010 triggers

$IOC/clastrig2/boot_clastrig2
$CODA/VXWORKS_ppc/bootscripts/boot_clastrig2

v1495firmware(0xf0410000,"HallBFlexTrigger_Nov2010_Rev5_0.rbf",0,0) - clastrig2

v1495firmware(0x90520000,"TOFPanelOR_V1_0.rbf",0,0) - sc2
v1495firmware(0x90530000,"TOFPanelOR_V1_0.rbf",0,0) - sc2
v1495firmware(0xf0410000,"HallBFlexTrigger.rbf",0,0) - clastrig2


PCAL trigger
ld < /usr/local/clas/devel/coda/src/rol/VXWORKS_ppc/obj/v1495.o
cd "/usr/local/clas/devel/coda/src/rol/code.s"
v1495firmware(0xf0410000,"PCALTrigger.rbf",0,0) - mv5500



****************************************
DVCS trigger (old VME firmware !!!!!)

cd "/usr/local/clas/devel/coda/src/rol/code.s"

v1495firmware(0xfa510000,"v1495vme03.rbf",0,1)
v1495firmware(0xfa520000,"v1495vme03.rbf",0,1)
v1495firmware(0xfa530000,"v1495vme03.rbf",0,1)
v1495firmware(0xfa540000,"v1495vme03.rbf",0,1)
v1495firmware(0xfa550000,"v1495vme03.rbf",0,1)





cd "/usr/local/clas/devel/coda/src/rol/code.s/ICTriggerLatestFirmware"

v1495firmware(0xfa510000,"DVCSMTrigger_V1_3.rbf",0,0)
v1495firmware(0xfa510000,"DVCSMTrigger_V1_4.rbf",0,0)

v1495firmware(0xfa520000,"DVCSQTrigger_Q1.rbf",0,0)
v1495firmware(0xfa530000,"DVCSQTrigger_Q2.rbf",0,0)
v1495firmware(0xfa540000,"DVCSQTrigger_Q3.rbf",0,0)
v1495firmware(0xfa550000,"DVCSQTrigger_Q4.rbf",0,0)

v1495firmware(0xfa510000,"v1495usr.rbf",0,0)



tests

v1495test1(0xfa510000)
v1495test1(0xfa520000)
v1495test1(0xfa530000)
v1495test1(0xfa540000)
v1495test1(0xfa550000)

v1495test2(0xfa520000)
v1495test2(0xfa530000)
v1495test2(0xfa540000)
v1495test2(0xfa550000)




*/


int
v1495test1(unsigned int address)
{
  volatile V1495 *v1495 = (V1495 *) address;
  unsigned short *data16 = (unsigned short *)&(v1495->control);

  printf("Control      [0x%08x] = 0x%04x\n",&(v1495->control),v1495->control);
  printf("firmwareRev  [0x%08x] = 0x%04x\n",&(v1495->firmwareRev),v1495->firmwareRev);
  printf("selflashVME  [0x%08x] = 0x%04x\n",&(v1495->selflashVME),v1495->selflashVME);
  printf("flashVME     [0x%08x] = 0x%04x\n",&(v1495->flashVME),v1495->flashVME);
  printf("selflashUSER [0x%08x] = 0x%04x\n",&(v1495->selflashUSER),v1495->selflashUSER);
  printf("flashUSER    [0x%08x] = 0x%04x\n",&(v1495->flashUSER),v1495->flashUSER);
  printf("configROM    [0x%08x] = 0x%04x\n",&(v1495->configROM[0]),v1495->configROM[0]);

  return(0);
}

v1495test2(unsigned int address)
{
  volatile V1495 *v1495 = (V1495 *) address;
  unsigned short *ptr_reset  = (unsigned short *) (address+6);
  unsigned short *ptr_enable = (unsigned short *) (address+6);
  unsigned short *ptr_buffer = (unsigned short *) (address+0x4000);
  int i;

  printf("Writing 0 to 0x%08x\n",ptr_enable);
  *ptr_enable = 0;
  taskDelay(100);
  for(i=211; i>=0; i--) printf("[%3d] address 0x%08x -> data 0x%08x\n",i,&ptr_buffer[i],ptr_buffer[i]);
  taskDelay(100);
  printf("Writing 1 to 0x%08x\n",ptr_enable);
  *ptr_enable = 1;
}

v1495test3(unsigned int address)
{
  volatile V1495 *v1495 = (V1495 *) address;
  unsigned short *ptr_reset  = (unsigned short *) (address+0x10);
  int i;

  while(1)
  {  
    *ptr_reset = 0;
    taskDelay(10);
    *ptr_reset = 1;
    taskDelay(10);
  }
}


/*****************************************************************************
   MAIN

     baseaddr: full board address (for example 0x80510000)
     filename: RBF file name
     page: =0 for standard, =1 for backup
     user_vme: Firmware to update selector = 0 => USER, 1 => VME

*****************************************************************************/
int
v1495firmware(unsigned int baseaddr, char *filename, int page, int user_vme)
{
  unsigned short *reload = (unsigned short *) (baseaddr+0x8016);
  int finish,i;
  int bp, bcnt, pa;
  char c;
  unsigned char pdw[PAGE_SIZE], pdr[PAGE_SIZE];
  unsigned long vboard_base_address;
  FILE *cf;

  /*page = 0;     ONLY STD !!!!!!!!!!!!! */
  /*user_vme = 0; ONLY USER !!!!!!!!!!!! */

  printf("\n");
  printf("********************************************************\n");
  printf("* CAEN SpA - Front-End Division                        *\n");
  printf("* ---------------------------------------------------- *\n");
  printf("* Firmware Upgrade of the V1495                        *\n");
  printf("* Version 1.1 (27/07/06)                               *\n");
  printf("*   Sergey Boyarinov: CLAS version 23-Apr-2007         *\n");
  printf("********************************************************\n\n");

  /* open the configuration file */
  cf = fopen(filename,"rb");
  if(cf==NULL)
  {
    printf("\n\nCan't open v1495 firmware file >%s< - exit\n",filename);
    exit(0);
  }

  if(user_vme == 0) /* FPGA "User" */
  {
    if(page == 0)
    {
      pa = USR_FIRST_PAGE_STD;
    }
    else if(page == 1)
    {
      printf("Backup image not supported for USER FPGA\n");
      exit(0);
	}
    else
    {
      printf("Bad Image.\n");
	  exit(0);
    }

    printf("Updating firmware of the FPGA USER with the file %s\n",filename);
  }
  else if(user_vme == 1) /* FPGA "VME_Interface" */
  {
    if(page == 0)
    {
      printf("Writing STD page of the VME FPGA\n");
      pa = VME_FIRST_PAGE_STD;
	}
    else if(page == 1)
    {
      printf("Writing BCK page of the VME FPGA\n");
      pa = VME_FIRST_PAGE_BCK;
	}
    else
    {
      printf("Bad Image.\n");
      exit(0);
	}

    printf("Updating firmware of the FPGA VME with the file %s\n", filename);
  }
  else
  {
    printf("Bad FPGA Target.\n");
	exit(0);
  }






  bcnt = 0; /* byte counter */
  bp = 0;   /* byte pointer in the page */
  finish = 0;

  /* while loop */
  while(!finish)
  {
    c = (unsigned char) fgetc(cf); /* read one byte from file */

    /* mirror byte (lsb becomes msb) */
    pdw[bp] = 0;
    for(i=0; i<8; i++)
    {
      if(c & (1<<i))
	  {
        pdw[bp] = pdw[bp] | (0x80>>i);
	  }
	}

    bp++;
    bcnt++;
    if(feof(cf))
    {
      printf("End of file: bp=%d bcnt=%d\n",bp,bcnt);
      finish = 1;
    }

    /* write and verify a page */
    if((bp == PAGE_SIZE) || finish)
    {
      write_flash_page1(baseaddr, pdw, pa, user_vme);
      read_flash_page1(baseaddr, pdr, pa, user_vme);
      for(i=0; i<PAGE_SIZE; i++)
      {
        if(pdr[i] != pdw[i])
        {
          printf("[%3d] written 0x%02x, read back 0x%02x",i,pdw[i],pdr[i]);
          printf(" -> Flash writing failure (byte %d of page %d)!\n",i,pa);
          printf("\nFirmware not loaded !\n");
          exit(0);
        }
	  }
      bp=0;
      pa++;
    }
  } /* end of while loop */

  fclose(cf);
  printf("\nFirmware loaded successfully. Written %d bytes\n", bcnt);

  /* reload new firmware for USER only */
  if(user_vme == 0)
  {
    printf("Activating updated version of the User FPGA, should be running now\n");
    *reload = 1;
  }
  else
  {
    printf("Write 1 at address 0x8016 to reload updated version of the User FPGA\n");
  }

  exit(0);
}








/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/
/* v1495 level1 scalers library */
/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/

/* NOV_2010 version has different set of scalers again */
#undef NOV_2010

/* PHOTON_RUN version has different set of scalers, and they are 32-bit */
#define PHOTON_RUN


#include <semLib.h>

#include "V1495VMERemote.h"


#ifdef PHOTON_RUN

#ifdef NOV_2010
#include "HallBTriggerBoardRegs2010.h"
#else
#include "HallBTriggerBoardRegsPhoton.h"
#endif

#else

#include "HallBTriggerBoardRegs.h"

#endif



#ifdef NOV_2010
#define MAXSCALERS 19
#else
#define MAXSCALERS 8 /* 8 in dvcstrig, 1 in clastrig2 */
#endif


/* holds time interval in seconds between scalers reads */
static int v1495ScalersReadInterval = 0;

static SEM_ID v1495scalers_lock;

static unsigned int  v1495scalersnum = 0;
static unsigned int  v1495scalersbegin[MAXSCALERS]; /*VME address of the first scaler (by switches on board) */
static unsigned int  v1495scalersend[MAXSCALERS];   /*last scaler, the same address space */
static unsigned int  v1495scalerslen[MAXSCALERS];   /*scalers space (the number of bytes) */
static unsigned int  v1495scalersaddr[MAXSCALERS];  /*actual VME address (after sysBusToLocalAdrs()) */
static unsigned int  v1495scalersenable[MAXSCALERS];/*actual VME address (after sysBusToLocalAdrs()) */
  static unsigned int  v1495scalersdisable[MAXSCALERS];/*actual VME address (after sysBusToLocalAdrs()) */
static unsigned int *v1495scalers[MAXSCALERS];      /*scalers memory space address*/

/* parameter 'time' in seconds */
int
v1495SetScalersReadInterval(int time)
{
  if(time < 0)        v1495ScalersReadInterval = 0; /* 0 means no reads */
  else if(time > 100) v1495ScalersReadInterval = 100;
  else                v1495ScalersReadInterval = time;

  logMsg("v1495SetScalersReadInterval: v1495ScalersReadInterval set to %d\n",
      v1495ScalersReadInterval,2,3,4,5,6);

  return(v1495ScalersReadInterval);
}

int
v1495GetScalersReadInterval()
{
  return(v1495ScalersReadInterval);
}

/* function for forcing synch. event every 10 seconds; use command
      taskSpawn "FORCE_SYNC",119,0,6000,ts2syncTask
   in the boot script */
void
v1495ReadTask()
{
  extern unsigned long sysClkRateGet();

  while(1)
  {
    if(v1495ScalersReadInterval==0) /* if interval==0, wait 1 sec and check again */
    {
      taskDelay(sysClkRateGet());
    }
    else
    {
      taskDelay(sysClkRateGet()*v1495ScalersReadInterval);
      v1495ScalersRead();
	  /*printf("v1495ReadTask: reading scalers ...\n");*/
    }
  }

  v1495ScalersReadoutStop();

  return;
}


/* following function called from boot script for every board */
/* addr - first scaler address in 'switch' address space (0x08524000 for example)
   len - the number of 32-bit scalers (the number of words)
   enable - address of the enable/disable register
*/

int
v1495ScalersReadoutAdd(unsigned int addr, unsigned int len, unsigned int enable)
{
  if(v1495scalersnum >= MAXSCALERS)
  {
    printf("v1495ScalersReadoutAdd ERROR: reach max number of scalers (%d)\n",
      v1495scalersnum);
    return(-1);
  }

  if(sysBusToLocalAdrs(0x39,addr&0xFFFFFF,&v1495scalersaddr[v1495scalersnum]) < 0)
  {
    v1495scalersaddr[v1495scalersnum] = 0;
    printf("v1495ScalersReadoutAdd ERROR: cannot set base address\n");
    return(-2);
  }

  if(enable > 0)
  {
    if(sysBusToLocalAdrs(0x39,enable&0xFFFFFF,&v1495scalersenable[v1495scalersnum]) < 0)
    {
      printf("v1495ScalersReadoutAdd ERROR: cannot set enable address\n");
      return(-3);
    }
  }
  else
  {
    v1495scalersenable[v1495scalersnum] = 0;
  }

  v1495scalerslen[v1495scalersnum] = len;
  v1495scalersbegin[v1495scalersnum] = addr;
  v1495scalersend[v1495scalersnum] = addr+(len<<2)-4;
  printf("v1495ScalersReadoutAdd: Scalers Addresses: 0x%08x:0x%08x->0x%08x, len=%d words, enable=0x%08x\n",
      v1495scalersbegin[v1495scalersnum],v1495scalersend[v1495scalersnum],
      v1495scalersaddr[v1495scalersnum],v1495scalerslen[v1495scalersnum],
      v1495scalersenable[v1495scalersnum]);

  if( (v1495scalers[v1495scalersnum] = malloc(len<<2)) <= 0)
  {
    printf("ERROR v1495ScalersReadoutAdd: cannot allocate memory\n");
    return(-4);
  }
  printf("v1495ScalersReadoutAdd: Allocated 0x%08x bytes at address 0x%08x\n",
		 len<<2,v1495scalers[v1495scalersnum]);

  v1495scalersnum ++;

  return(0);
}


/* following functions inserts enable/disable call into the scaler group starting
    at 'addr', must be called after corresponding v1495ScalersReadoutAdd()
    has been called for that 'addr' !!! */
/* addr - first scaler address in 'switch' address space (0x08524000 for example)
   enable - address of the enable register

ld < /usr/local/clas/devel/coda/src/rol/VXWORKS_ppc/obj/v1495.o
*/

int
v1495ScalersEnableAdd(unsigned int addr, unsigned int enable)
{
  int found, k;
  unsigned int address;

  if(sysBusToLocalAdrs(0x39,addr&0xFFFFFF,&address) < 0)
  {
    printf("v1495ScalersEnableAdd ERROR: cannot set base address\n");
    return(-2);
  }

  found = 0;
  printf("v1495ScalersEnableAdd: looking for 0x%08x among %d scalers\n",
    address,v1495scalersnum);
  for(k=0; k<v1495scalersnum; k++)
  {
    if(v1495scalersaddr[k] == address)
    {
      printf("v1495ScalersEnableAdd: found [%d] 0x%08x\n",k,v1495scalersaddr[k]);
      found = 1;
      if(sysBusToLocalAdrs(0x39,enable&0xFFFFFF,&v1495scalersenable[k]) < 0)
      {
        printf("v1495ScalersEnableAdd ERROR: cannot set enable address\n");
        return(-3);
      }
      break;
    }
  }

  if(found==0)
  {
    printf("v1495ScalersEnableAdd ERROR: cannot found specified address\n");
    return(-4);
  }

  return(0);
}

int
v1495ScalersDisableAdd(unsigned int addr, unsigned int disable)
{
  int found, k;
  unsigned int address;

  if(sysBusToLocalAdrs(0x39,addr&0xFFFFFF,&address) < 0)
  {
    printf("v1495ScalersEnableAdd ERROR: cannot set base address\n");
    return(-2);
  }

  found = 0;
  for(k=0; k<v1495scalersnum; k++)
  {
    if(v1495scalersaddr[k] == address)
    {
      found = 1;
      if(sysBusToLocalAdrs(0x39,disable&0xFFFFFF,&v1495scalersdisable[k]) < 0)
      {
        printf("v1495ScalersDisableAdd ERROR: cannot set disable address\n");
        return(-3);
      }
      break;
    }
  }

  if(found==0)
  {
    printf("v1495ScalersDisableAdd ERROR: cannot found specified address\n");
    return(-4);
  }

  return(0);
}




/* following function called from boot script for every board */

int
v1495ScalersReadoutStart()
{
  v1495scalers_lock = semBCreate(SEM_Q_FIFO, SEM_FULL);
  if(v1495scalers_lock == NULL)
  {
    printf("v1495ScalersReadoutStart ERROR: could not allocate a semaphore\n");
    return(-1);
  }

  if(taskSpawn("v1495READ", 250, 0, 100000,(FUNCPTR)v1495ReadTask,
	   0,0,0,0,0,0,0,0,0,0) == ERROR)
  {
    printf("ERROR v1495ScalersReadoutStart: cannot start thread\n");
    perror("taskSpawn"); 
    return(-2);
  }
  else
  {
    printf("v1495ScalersReadoutStart: 'v1495READ' task started\n");
  }

  return(0);
}

int
v1495ScalersReadoutStop()
{
  /* TODO: END TASK HERE !!!!!!!!!!!!!!!!!!!!!!!! */

  free(v1495scalers);

  semGive(v1495scalers_lock);
  semDelete(v1495scalers_lock);

  return(0);
}


int
v1495ScalersRead()
{
  volatile unsigned int *ptr32;
  volatile unsigned short *ptr16;
  unsigned int *mem32;
  unsigned short *mem16;
  int i, k, nscalers;

  semTake(v1495scalers_lock, WAIT_FOREVER);
  /*printf("v1495scalersnum=%d\n",v1495scalersnum);*/

  /* disable scalers */
  for(k=0; k<v1495scalersnum; k++)
  {
    /* disable scalers (it will stop them) */
    if(v1495scalersenable[k] > 0)
    {
#ifdef PHOTON_RUN
      ptr32 = (volatile unsigned int *)(v1495scalersenable[k]);
      *ptr32 = 0x0; /*disable scalers*/
#else
      ptr16 = (volatile unsigned short *)(v1495scalersenable[k]);
      *ptr16 = 0x0; /*disable scalers*/
#endif
      /*printf("disable: write 0 to 0x%08x\n",ptr16);*/
      EIEIO;
      SYNC;
    }
  }

  /* read snapshot of scalers */
  for(k=0; k<v1495scalersnum; k++)
  {
#ifdef PHOTON_RUN
    ptr32 = (volatile unsigned int *)(v1495scalersaddr[k]);
    mem32 = (unsigned int *)(v1495scalers[k]);
#else
    ptr16 = (volatile unsigned short *)(v1495scalersaddr[k]);
    mem16 = (unsigned short *)(v1495scalers[k]);
#endif
    EIEIO;
    SYNC;
    /*printf("v1495ScalersRead: ptr16=0x%08x, mem16=0x%08x, len=%d 32-bit words\n",
      ptr16,mem16,v1495scalerslen[k]);*/


    /* NEED REVERSE READOUT HERE FOR A24/D16 !!!!!!!!!! */
    for(i=0; i<v1495scalerslen[k]; i++)
    {
#ifdef PHOTON_RUN
      mem32[0] = ptr32[0];
#else
      mem16[1] = ptr16[1];
      mem16[0] = ptr16[0];
#endif
      EIEIO;
      SYNC;
#ifdef PHOTON_RUN
      mem32++;
      ptr32++;
#else
      mem16+=2;
      ptr16+=2;
#endif
    }

    EIEIO;
    SYNC;
  }

  /* enable scalers (it will clear and start them) */
  for(k=0; k<v1495scalersnum; k++)
  {
    if(v1495scalersenable[k] > 0)
    {
#ifdef PHOTON_RUN
      ptr32 = (volatile unsigned int *)(v1495scalersenable[k]);
      *ptr32 = 0x1; /*enable scalers*/
#else
      ptr16 = (volatile unsigned short *)(v1495scalersenable[k]);
      /*printf("enable: write 1 to 0x%08x\n",ptr16);*/
      *ptr16 = 0x1; /*enable scalers*/
#endif
      EIEIO;
      SYNC;
	}
  }

  semGive(v1495scalers_lock);

  return(0);
}

int
v1495ScalersReadFromMemory()
{
  semTake(v1495scalers_lock, WAIT_FOREVER);
  semGive(v1495scalers_lock);

  return(0);
}

unsigned int
v1495GetScalerMemoryAddress(unsigned int addr)
{
  int i;
  unsigned int tmp;

  for(i=0; i<v1495scalersnum; i++)
  {
    if( (addr >= v1495scalersbegin[i])  && (addr <= v1495scalersend[i]) )
	{
      tmp = addr - v1495scalersbegin[i] + ((unsigned int)v1495scalers[i]);
      /*printf("tmp=0x%08x (0x%08x 0x%08x 0x%08x)\n",
        tmp,addr,v1495scalersbegin[i],((unsigned int)v1495scalers[i]));*/
      return(tmp);
	}
  }

  printf("v1495GetScalerMemoryAddress: cannot get address for 0x%08x\n",addr);

  return(-1);
}

unsigned int
v1495GetScalerFromMemory(unsigned int addr)
{
  int i;
  unsigned int value, tmp;

  /*printf("v1495GetScalerFromMemory: read addr is 0x%08x\n",addr);*/
  semTake(v1495scalers_lock, WAIT_FOREVER);

  for(i=0; i<v1495scalersnum; i++)
  {
    if( (addr >= v1495scalersbegin[i])  && (addr <= v1495scalersend[i]) )
	{
      tmp = addr - v1495scalersbegin[i] + ((unsigned int)v1495scalers[i]);
      /*printf("tmp=0x%08x (0x%08x 0x%08x 0x%08x)\n",
        tmp,addr,v1495scalersbegin[i],((unsigned int)v1495scalers[i]));*/
      value = *((unsigned int *)tmp);
      /*printf("value=%d\n",value);*/
      semGive(v1495scalers_lock);
      return(value);
	}
  }

  printf("v1495GetScalerFromMemory: cannot get value from 0x%08x\n",addr);
  semGive(v1495scalers_lock);

  return(-1);
}





#ifdef PHOTON_RUN


#ifdef NOV_2010


#define STRLEN 1024


/**************************************************/
/* download/upload functions for the lookup table */

/* for example:

ld < /usr/local/clas/devel/coda/src/rol/VXWORKS_ppc/obj/v1495.o
v1495LUTDownload(0x08510000,"/usr/local/clas/parms/trigger/config/tpenew/tables/trig1lut_0.lut",1,0)
v1495LUTDownload(0x08510000,"/usr/local/clas/parms/trigger/config/tpenew/tables/trig1lut_1.lut",1,1)
v1495LUTDownload(0x08510000,"/usr/local/clas/parms/trigger/config/tpenew/tables/trig1lut_2.lut",1,2)
v1495LUTDownload(0x08510000,"/usr/local/clas/parms/trigger/config/tpenew/tables/trig1lut_3.lut",1,3)
v1495LUTDownload(0x08510000,"/usr/local/clas/parms/trigger/config/tpenew/tables/trig1lut_4.lut",1,4)
v1495LUTDownload(0x08510000,"/usr/local/clas/parms/trigger/config/tpenew/tables/trig1lut_5.lut",1,5)
v1495LUTDownload(0x08510000,"/usr/local/clas/parms/trigger/config/tpenew/tables/trig1lut_6.lut",1,6)
v1495LUTDownload(0x08510000,"/usr/local/clas/parms/trigger/config/tpenew/tables/trig1lut_7.lut",1,7)
v1495LUTDownload(0x08510000,"/usr/local/clas/parms/trigger/config/tpenew/tables/trig1lut_8.lut",1,8)
v1495LUTDownload(0x08510000,"/usr/local/clas/parms/trigger/config/tpenew/tables/trig1lut_9.lut",1,9)
v1495LUTDownload(0x08510000,"/usr/local/clas/parms/trigger/config/tpenew/tables/trig1lut_10.lut",1,10)
v1495LUTDownload(0x08510000,"/usr/local/clas/parms/trigger/config/tpenew/tables/trig1lut_11.lut",1,11)
v1495LUTDownload(0x08510000,"/usr/local/clas/parms/trigger/config/tpenew/tables/trig1lut_12.lut",1,12)

*/

#define NLUT   4096 /* the number of loookup table addresses */
static unsigned int v1495lut[NLUT];
static unsigned int lut[NLUT];

/* reads config file and updates hardware lookup tables */
int
v1495LUTDownload(unsigned int addr, char *fname, int trigbit, int flag)
{
  FILE *fd;
  char *ch, str[STRLEN], keyword[STRLEN];
  int i, nregs, nline;
  unsigned int value;
  volatile unsigned int *ptr32;
  unsigned int *mem32;
  unsigned int v1495base;


  if(trigbit<1 || trigbit>13)
  {
    printf("v1495LUTDownload: ERROR: trigbit out of range - exit\n");
    return(-1);
  }

  if(sysBusToLocalAdrs(0x39,addr&0xFFFFFF,&v1495base) < 0)
  {
    v1495base = 0;
    printf("v1495LUTDownload ERROR: undefined base address\n");
    return(-3);
  }

  if(v1495base == 0)
  {
    printf("v1495LUTDownload ERROR: undefined base address\n");
    return(-4);
  }

  if( (fd = fopen(fname,"r")) == NULL)
  {
    printf("v1495LUTDownload: ERROR: Can't open config file >%s<\n",fname);
    return(-5);
  }

  /*v1495LUTDownload("/home/clasrun/g12/lut/tables/trig_ecp_STOF2.lut",1,1)*/


  nline = 0;
  for(i=0; i<NLUT; i++) v1495lut[i] = lut[i] = 0;
  while((ch = fgets(str, STRLEN, fd)) != NULL)
  {
	/*printf(">%s< %d\n",str,ch);*/
    if( ch[0] == '#' || ch[0] == ' ' || ch[0] == '\t' || ch[0] == '\n' || ch[0] == '\r' )
    {
      ;
    }
    else
    {
      sscanf(str,"%x %s",&value,keyword);
      /*printf("value = %d key >%s<\n",value,keyword);*/
      if(nline < NLUT)
	  {
        v1495lut[nline++] = value;
	  }
      else
	  {
        printf("v1495LUTDownload: File too long (nline=%d) - interrupt reading\n",nline);
        return(-6);
	  }
	}
  }

  fclose(fd);

  /*for(i=0; i<31; i++) printf("[%2d] 0x%04x\n",i,v1495lut[i]);*/

  /*
  semTake(v1495scalers_lock, WAIT_FOREVER);
  */

  /* download */
  if(trigbit==13)
    ptr32 = (volatile unsigned int *)(v1495base + TS_CFG_LUT_L2);
  else
    ptr32 = (volatile unsigned int *)(v1495base + TS_CFG_LUT_1 + (((trigbit-1)&0xF)<<8) + (flag*0x4));

  printf("v1495LUTDownload: downloading LUT to address 0x%08x\n",ptr32);


  if(flag==0) /*actual lookup table*/
  {
    *ptr32 = 0x80000000; /* reset address pointer */
    EIEIO;
    SYNC;
    for(i=0; i<nline; i++)
    {
      *ptr32 = v1495lut[i];
      EIEIO;
      SYNC;
    }

    /*upload and compare*/
    *ptr32 = 0x80000000; /* reset address pointer */
    EIEIO;
    SYNC;
    for(i=0; i<nline; i++)
    {
      lut[i] = *ptr32;
      EIEIO;
      SYNC;
    }
  }
  else /* 'one word' lookup table */
  {
    EIEIO;
    SYNC;
    value = 0;
    for(i=0; i<nline; i++)
    {
      value |= (v1495lut[i]<<i);
    }
    *ptr32 = value;
    EIEIO;
    SYNC;

    /*upload and compare*/
    value = *ptr32;
    EIEIO;
    SYNC;
    for(i=0; i<nline; i++)
    {
      lut[i] = (value>>i)&0x1;
      EIEIO;
      SYNC;
    }
  }

  /*
  semGive(v1495scalers_lock);
  */

  printf("/nv1495LUTDownload: reading back and comparing\n");
  for(i=0; i<nline; i++)
  {
    if(v1495lut[i] != lut[i])
	{
      printf("[%4d] v1495LUTDownload ERROR at address 0x%04x: load 0x%04x, read back 0x%04x\n",
        i,i,v1495lut[i],lut[i]);
      return(-7);
	}
    else
	{
      /*printf("[%4d] v1495LUTDownload: address 0x%04x passed: 0x%04x = 0x%04x\n",i,i,v1495lut[i],lut[i])*/;
	}
  }

  return(0);
}


#define LOADLUTS(lutfilename_macro, par1_macro) { \
    char fname[256]; \
    strcpy(fname,lutfilename_macro); \
    strcat(fname,"_0.lut"); \
    ret=v1495LUTDownload(addr,fname,par1_macro,0); \
    strcpy(fname,lutfilename_macro); \
    strcat(fname,"_1.lut"); \
    ret=v1495LUTDownload(addr,fname,par1_macro,1); \
    strcpy(fname,lutfilename_macro); \
    strcat(fname,"_2.lut"); \
    ret=v1495LUTDownload(addr,fname,par1_macro,2); \
    strcpy(fname,lutfilename_macro); \
    strcat(fname,"_3.lut"); \
    ret=v1495LUTDownload(addr,fname,par1_macro,3); \
    strcpy(fname,lutfilename_macro); \
    strcat(fname,"_4.lut"); \
    ret=v1495LUTDownload(addr,fname,par1_macro,4); \
    strcpy(fname,lutfilename_macro); \
    strcat(fname,"_5.lut"); \
    ret=v1495LUTDownload(addr,fname,par1_macro,5); \
    strcpy(fname,lutfilename_macro); \
    strcat(fname,"_6.lut"); \
    ret=v1495LUTDownload(addr,fname,par1_macro,6); \
    strcpy(fname,lutfilename_macro); \
    strcat(fname,"_7.lut"); \
    ret=v1495LUTDownload(addr,fname,par1_macro,7); \
    strcpy(fname,lutfilename_macro); \
    strcat(fname,"_8.lut"); \
    ret=v1495LUTDownload(addr,fname,par1_macro,8); \
    strcpy(fname,lutfilename_macro); \
    strcat(fname,"_9.lut"); \
    ret=v1495LUTDownload(addr,fname,par1_macro,9); \
    strcpy(fname,lutfilename_macro); \
    strcat(fname,"_10.lut"); \
    ret=v1495LUTDownload(addr,fname,par1_macro,10); \
    strcpy(fname,lutfilename_macro); \
    strcat(fname,"_11.lut"); \
    ret=v1495LUTDownload(addr,fname,par1_macro,11); \
    strcpy(fname,lutfilename_macro); \
    strcat(fname,"_12.lut"); \
    ret=v1495LUTDownload(addr,fname,par1_macro,12); \
  }

#define NREGS  (0x3200/4)
#define NREGS_ACTUAL 109
static regs_actual[NREGS_ACTUAL] = {

  0x1000/4,
  0x1004/4,
  0x1008/4,
  0x1100/4,
  0x1104/4,
  0x1108/4,
  0x1200/4,
  0x1204/4,
  0x1208/4,
  0x1300/4,
  0x1304/4,
  0x1308/4,
  0x1400/4,
  0x1404/4,
  0x1408/4,
  0x1500/4,
  0x1504/4,
  0x1508/4,

  0x1020/4,
  0x1120/4,
  0x1220/4,
  0x1320/4,
  0x1420/4,
  0x1520/4,

  0x1028/4,
  0x1128/4,
  0x1228/4,
  0x1328/4,
  0x1428/4,
  0x1528/4,

  0x1024/4,
  0x1124/4,
  0x1224/4,
  0x1324/4,
  0x1424/4,
  0x1524/4,

  0x102C/4,
  0x112C/4,
  0x122C/4,
  0x132C/4,
  0x142C/4,
  0x152C/4,

  0x101C/4,
  0x111C/4,
  0x121C/4,
  0x131C/4,
  0x141C/4,
  0x151C/4,

  0x100C/4,
  0x1010/4,
  0x1014/4,
  0x1018/4,
  0x110C/4,
  0x1110/4,
  0x1114/4,
  0x1118/4,
  0x120C/4,
  0x1210/4,
  0x1214/4,
  0x1218/4,
  0x130C/4,
  0x1310/4,
  0x1314/4,
  0x1318/4,
  0x140C/4,
  0x1410/4,
  0x1414/4,
  0x1418/4,
  0x150C/4,
  0x1510/4,
  0x1514/4,
  0x1518/4,

  0x2010/4,
  0x2110/4,
  0x2210/4,
  0x2310/4,
  0x2410/4,
  0x2510/4,
  0x2610/4,
  0x2710/4,
  0x2810/4,
  0x2910/4,
  0x2A10/4,
  0x2B10/4,

  0x1078/4,
  0x1178/4,
  0x1278/4,
  0x1378/4,
  0x1478/4,
  0x1578/4,

  0x2000/4,
  0x2100/4,
  0x2200/4,
  0x2300/4,
  0x2400/4,
  0x2500/4,
  0x2600/4,
  0x2700/4,
  0x2800/4,
  0x2900/4,
  0x2A00/4,
  0x2B00/4,

  0x3010/4,
  0x3020/4,
  0x3024/4,
  0x302C/4,
  0x3104/4,
  0x3108/4,
  0x310C/4
};

/*
ld < /usr/local/clas/devel/coda/src/rol/VXWORKS_ppc/obj/v1495.o
v1495ConfigDownload(0x08510000,"/usr/local/clas/parms/trigger/config/tpenew/tables/test.dat")
*/

static unsigned int v1495regs[NREGS];
static unsigned int regs[NREGS];

/* reads config file and updates hardware registers */
int
v1495ConfigDownload(unsigned int addr, char *fname)
{
  FILE *fd;
  char *ch, str[STRLEN], keyword[STRLEN], lutfilename[STRLEN];
  int i, nregs, ret;
  unsigned int value;
  volatile unsigned int *ptr32;
  unsigned int *mem32;
  unsigned int v1495base;

  if(sysBusToLocalAdrs(0x39,addr&0xFFFFFF,&v1495base) < 0)
  {
    v1495base = 0;
    printf("v1495ConfigDownload ERROR: undefined base address\n");
    return(-1);
  }

  if(v1495base == 0)
  {
    printf("v1495ConfigDownload ERROR: undefined base address\n");
    return(-1);
  }

  if( (fd = fopen(fname,"r")) == NULL)
  {
    printf("v1495ConfigDownload: Can't open config file >%s<\n",fname);
    return(-2);
  }

  for(i=0; i<NREGS; i++) v1495regs[i] = regs[i] = 0;
  ret = 0;
  while((ch = fgets(str, STRLEN, fd)) != NULL)
  {
	/*printf(">%s< %d\n",str,ch);*/
    if( ch[0] == '#' || ch[0] == ' ' || ch[0] == '\t' || ch[0] == '\n' || ch[0] == '\r' )
    {
      ;
    }
    else
    {
      sscanf(str,"%s %s", keyword, lutfilename);
      value = atoi(lutfilename);
      printf("key >%s< lutfilename >%s< value = %d\n",keyword,lutfilename,value);

      /*sscanf(str,"%s %d", keyword, &value);*/
      /*printf("key >%s< value = %d\n",keyword,value);*/

      if(     !strcmp(keyword,"TS_MORA_DELAY")) v1495regs[0x3020/4] = value & 0x1F;
      else if(!strcmp(keyword,"TS_MORB_DELAY")) v1495regs[0x3024/4] = value & 0x1F;

	  else if(!strcmp(keyword,"TS_TOFP1_1_DELAY")) v1495regs[0x1000/4] = value & 0x1F;
	  else if(!strcmp(keyword,"TS_TOFP2_1_DELAY")) v1495regs[0x1004/4] = value & 0x1F;
	  else if(!strcmp(keyword,"TS_TOFP3_1_DELAY")) v1495regs[0x1008/4] = value & 0x1F;
	  else if(!strcmp(keyword,"TS_TOFP1_2_DELAY")) v1495regs[0x1100/4] = value & 0x1F;
	  else if(!strcmp(keyword,"TS_TOFP2_2_DELAY")) v1495regs[0x1104/4] = value & 0x1F;
	  else if(!strcmp(keyword,"TS_TOFP3_2_DELAY")) v1495regs[0x1108/4] = value & 0x1F;
	  else if(!strcmp(keyword,"TS_TOFP1_3_DELAY")) v1495regs[0x1200/4] = value & 0x1F;
	  else if(!strcmp(keyword,"TS_TOFP2_3_DELAY")) v1495regs[0x1204/4] = value & 0x1F;
	  else if(!strcmp(keyword,"TS_TOFP3_3_DELAY")) v1495regs[0x1208/4] = value & 0x1F;
	  else if(!strcmp(keyword,"TS_TOFP1_4_DELAY")) v1495regs[0x1300/4] = value & 0x1F;
	  else if(!strcmp(keyword,"TS_TOFP2_4_DELAY")) v1495regs[0x1304/4] = value & 0x1F;
	  else if(!strcmp(keyword,"TS_TOFP3_4_DELAY")) v1495regs[0x1308/4] = value & 0x1F;
	  else if(!strcmp(keyword,"TS_TOFP1_5_DELAY")) v1495regs[0x1400/4] = value & 0x1F;
	  else if(!strcmp(keyword,"TS_TOFP2_5_DELAY")) v1495regs[0x1404/4] = value & 0x1F;
	  else if(!strcmp(keyword,"TS_TOFP3_5_DELAY")) v1495regs[0x1408/4] = value & 0x1F;
	  else if(!strcmp(keyword,"TS_TOFP1_6_DELAY")) v1495regs[0x1500/4] = value & 0x1F;
	  else if(!strcmp(keyword,"TS_TOFP2_6_DELAY")) v1495regs[0x1504/4] = value & 0x1F;
	  else if(!strcmp(keyword,"TS_TOFP3_6_DELAY")) v1495regs[0x1508/4] = value & 0x1F;

      else if(!strcmp(keyword,"TS_ECin1P_DELAY")) v1495regs[0x1020/4] = value & 0x1F;
      else if(!strcmp(keyword,"TS_ECin2P_DELAY")) v1495regs[0x1120/4] = value & 0x1F;
      else if(!strcmp(keyword,"TS_ECin3P_DELAY")) v1495regs[0x1220/4] = value & 0x1F;
      else if(!strcmp(keyword,"TS_ECin4P_DELAY")) v1495regs[0x1320/4] = value & 0x1F;
      else if(!strcmp(keyword,"TS_ECin5P_DELAY")) v1495regs[0x1420/4] = value & 0x1F;
      else if(!strcmp(keyword,"TS_ECin6P_DELAY")) v1495regs[0x1520/4] = value & 0x1F;

      else if(!strcmp(keyword,"TS_ECin1E_DELAY")) v1495regs[0x1028/4] = value & 0x1F;
      else if(!strcmp(keyword,"TS_ECin2E_DELAY")) v1495regs[0x1128/4] = value & 0x1F;
      else if(!strcmp(keyword,"TS_ECin3E_DELAY")) v1495regs[0x1228/4] = value & 0x1F;
      else if(!strcmp(keyword,"TS_ECin4E_DELAY")) v1495regs[0x1328/4] = value & 0x1F;
      else if(!strcmp(keyword,"TS_ECin5E_DELAY")) v1495regs[0x1428/4] = value & 0x1F;
      else if(!strcmp(keyword,"TS_ECin6E_DELAY")) v1495regs[0x1528/4] = value & 0x1F;

      else if(!strcmp(keyword,"TS_ECtot1P_DELAY")) v1495regs[0x1024/4] = value & 0x1F;
      else if(!strcmp(keyword,"TS_ECtot2P_DELAY")) v1495regs[0x1124/4] = value & 0x1F;
      else if(!strcmp(keyword,"TS_ECtot3P_DELAY")) v1495regs[0x1224/4] = value & 0x1F;
      else if(!strcmp(keyword,"TS_ECtot4P_DELAY")) v1495regs[0x1324/4] = value & 0x1F;
      else if(!strcmp(keyword,"TS_ECtot5P_DELAY")) v1495regs[0x1424/4] = value & 0x1F;
      else if(!strcmp(keyword,"TS_ECtot6P_DELAY")) v1495regs[0x1524/4] = value & 0x1F;

      else if(!strcmp(keyword,"TS_ECtot1E_DELAY")) v1495regs[0x102C/4] = value & 0x1F;
      else if(!strcmp(keyword,"TS_ECtot2E_DELAY")) v1495regs[0x112C/4] = value & 0x1F;
      else if(!strcmp(keyword,"TS_ECtot3E_DELAY")) v1495regs[0x122C/4] = value & 0x1F;
      else if(!strcmp(keyword,"TS_ECtot4E_DELAY")) v1495regs[0x132C/4] = value & 0x1F;
      else if(!strcmp(keyword,"TS_ECtot5E_DELAY")) v1495regs[0x142C/4] = value & 0x1F;
      else if(!strcmp(keyword,"TS_ECtot6E_DELAY")) v1495regs[0x152C/4] = value & 0x1F;

      else if(!strcmp(keyword,"TS_CC1_DELAY")) v1495regs[0x101C/4] = value & 0x1F;
      else if(!strcmp(keyword,"TS_CC2_DELAY")) v1495regs[0x111C/4] = value & 0x1F;
      else if(!strcmp(keyword,"TS_CC3_DELAY")) v1495regs[0x121C/4] = value & 0x1F;
      else if(!strcmp(keyword,"TS_CC4_DELAY")) v1495regs[0x131C/4] = value & 0x1F;
      else if(!strcmp(keyword,"TS_CC5_DELAY")) v1495regs[0x141C/4] = value & 0x1F;
      else if(!strcmp(keyword,"TS_CC6_DELAY")) v1495regs[0x151C/4] = value & 0x1F;

	  else if(!strcmp(keyword,"TS_ST1_DELAY"))  v1495regs[0x100C/4] = value & 0x1F;
	  else if(!strcmp(keyword,"TS_ST2_DELAY"))  v1495regs[0x1010/4] = value & 0x1F;
	  else if(!strcmp(keyword,"TS_ST3_DELAY"))  v1495regs[0x1014/4] = value & 0x1F;
	  else if(!strcmp(keyword,"TS_ST4_DELAY"))  v1495regs[0x1018/4] = value & 0x1F;
	  else if(!strcmp(keyword,"TS_ST5_DELAY"))  v1495regs[0x110C/4] = value & 0x1F;
	  else if(!strcmp(keyword,"TS_ST6_DELAY"))  v1495regs[0x1110/4] = value & 0x1F;
	  else if(!strcmp(keyword,"TS_ST7_DELAY"))  v1495regs[0x1114/4] = value & 0x1F;
	  else if(!strcmp(keyword,"TS_ST8_DELAY"))  v1495regs[0x1118/4] = value & 0x1F;
	  else if(!strcmp(keyword,"TS_ST9_DELAY"))  v1495regs[0x120C/4] = value & 0x1F;
	  else if(!strcmp(keyword,"TS_ST10_DELAY")) v1495regs[0x1210/4] = value & 0x1F;
	  else if(!strcmp(keyword,"TS_ST11_DELAY")) v1495regs[0x1214/4] = value & 0x1F;
	  else if(!strcmp(keyword,"TS_ST12_DELAY")) v1495regs[0x1218/4] = value & 0x1F;
	  else if(!strcmp(keyword,"TS_ST13_DELAY")) v1495regs[0x130C/4] = value & 0x1F;
	  else if(!strcmp(keyword,"TS_ST14_DELAY")) v1495regs[0x1310/4] = value & 0x1F;
	  else if(!strcmp(keyword,"TS_ST15_DELAY")) v1495regs[0x1314/4] = value & 0x1F;
	  else if(!strcmp(keyword,"TS_ST16_DELAY")) v1495regs[0x1318/4] = value & 0x1F;
	  else if(!strcmp(keyword,"TS_ST17_DELAY")) v1495regs[0x140C/4] = value & 0x1F;
	  else if(!strcmp(keyword,"TS_ST18_DELAY")) v1495regs[0x1410/4] = value & 0x1F;
	  else if(!strcmp(keyword,"TS_ST19_DELAY")) v1495regs[0x1414/4] = value & 0x1F;
	  else if(!strcmp(keyword,"TS_ST20_DELAY")) v1495regs[0x1418/4] = value & 0x1F;
	  else if(!strcmp(keyword,"TS_ST21_DELAY")) v1495regs[0x150C/4] = value & 0x1F;
	  else if(!strcmp(keyword,"TS_ST22_DELAY")) v1495regs[0x1510/4] = value & 0x1F;
	  else if(!strcmp(keyword,"TS_ST23_DELAY")) v1495regs[0x1514/4] = value & 0x1F;
	  else if(!strcmp(keyword,"TS_ST24_DELAY")) v1495regs[0x1518/4] = value & 0x1F;

	  else if(!strcmp(keyword,"TS_PRESCALE_TRIG1"))   v1495regs[0x2010/4] = value & 0x3FF;
	  else if(!strcmp(keyword,"TS_PRESCALE_TRIG2"))   v1495regs[0x2110/4] = value & 0x3FF;
	  else if(!strcmp(keyword,"TS_PRESCALE_TRIG3"))   v1495regs[0x2210/4] = value & 0x3FF;
	  else if(!strcmp(keyword,"TS_PRESCALE_TRIG4"))   v1495regs[0x2310/4] = value & 0x3FF;
	  else if(!strcmp(keyword,"TS_PRESCALE_TRIG5"))   v1495regs[0x2410/4] = value & 0x3FF;
	  else if(!strcmp(keyword,"TS_PRESCALE_TRIG6"))   v1495regs[0x2510/4] = value & 0x3FF;
	  else if(!strcmp(keyword,"TS_PRESCALE_TRIG7"))   v1495regs[0x2610/4] = value & 0x3FF;
	  else if(!strcmp(keyword,"TS_PRESCALE_TRIG8"))   v1495regs[0x2710/4] = value & 0x3FF;
	  else if(!strcmp(keyword,"TS_PRESCALE_TRIG9"))   v1495regs[0x2810/4] = value & 0x3FF;
	  else if(!strcmp(keyword,"TS_PRESCALE_TRIG10"))  v1495regs[0x2910/4] = value & 0x3FF;
	  else if(!strcmp(keyword,"TS_PRESCALE_TRIG11"))  v1495regs[0x2A10/4] = value & 0x3FF;
	  else if(!strcmp(keyword,"TS_PRESCALE_TRIG12"))  v1495regs[0x2B10/4] = value & 0x3FF;

	  else if(!strcmp(keyword,"TS_SECTOR1_CFG_ECPCC"))  v1495regs[0x1078/4] |= (value & 0x3)<<16;
	  else if(!strcmp(keyword,"TS_SECTOR1_CFG_ECECC"))  v1495regs[0x1078/4] |= (value & 0x3)<<12;
	  else if(!strcmp(keyword,"TS_SECTOR1_CFG_STOFP1")) v1495regs[0x1078/4] |= (value & 0x3)<<8;
	  else if(!strcmp(keyword,"TS_SECTOR1_CFG_STOFP2")) v1495regs[0x1078/4] |= (value & 0x3)<<4;
	  else if(!strcmp(keyword,"TS_SECTOR1_CFG_STOFP3")) v1495regs[0x1078/4] |= (value & 0x3);

	  else if(!strcmp(keyword,"TS_SECTOR2_CFG_ECPCC"))  v1495regs[0x1178/4] |= (value & 0x3)<<16;
	  else if(!strcmp(keyword,"TS_SECTOR2_CFG_ECECC"))  v1495regs[0x1178/4] |= (value & 0x3)<<12;
	  else if(!strcmp(keyword,"TS_SECTOR2_CFG_STOFP1")) v1495regs[0x1178/4] |= (value & 0x3)<<8;
	  else if(!strcmp(keyword,"TS_SECTOR2_CFG_STOFP2")) v1495regs[0x1178/4] |= (value & 0x3)<<4;
	  else if(!strcmp(keyword,"TS_SECTOR2_CFG_STOFP3")) v1495regs[0x1178/4] |= (value & 0x3);

	  else if(!strcmp(keyword,"TS_SECTOR3_CFG_ECPCC"))  v1495regs[0x1278/4] |= (value & 0x3)<<16;
	  else if(!strcmp(keyword,"TS_SECTOR3_CFG_ECECC"))  v1495regs[0x1278/4] |= (value & 0x3)<<12;
	  else if(!strcmp(keyword,"TS_SECTOR3_CFG_STOFP1")) v1495regs[0x1278/4] |= (value & 0x3)<<8;
	  else if(!strcmp(keyword,"TS_SECTOR3_CFG_STOFP2")) v1495regs[0x1278/4] |= (value & 0x3)<<4;
	  else if(!strcmp(keyword,"TS_SECTOR3_CFG_STOFP3")) v1495regs[0x1278/4] |= (value & 0x3);

	  else if(!strcmp(keyword,"TS_SECTOR4_CFG_ECPCC"))  v1495regs[0x1378/4] |= (value & 0x3)<<16;
	  else if(!strcmp(keyword,"TS_SECTOR4_CFG_ECECC"))  v1495regs[0x1378/4] |= (value & 0x3)<<12;
	  else if(!strcmp(keyword,"TS_SECTOR4_CFG_STOFP1")) v1495regs[0x1378/4] |= (value & 0x3)<<8;
	  else if(!strcmp(keyword,"TS_SECTOR4_CFG_STOFP2")) v1495regs[0x1378/4] |= (value & 0x3)<<4;
	  else if(!strcmp(keyword,"TS_SECTOR4_CFG_STOFP3")) v1495regs[0x1378/4] |= (value & 0x3);

	  else if(!strcmp(keyword,"TS_SECTOR5_CFG_ECPCC"))  v1495regs[0x1478/4] |= (value & 0x3)<<16;
	  else if(!strcmp(keyword,"TS_SECTOR5_CFG_ECECC"))  v1495regs[0x1478/4] |= (value & 0x3)<<12;
	  else if(!strcmp(keyword,"TS_SECTOR5_CFG_STOFP1")) v1495regs[0x1478/4] |= (value & 0x3)<<8;
	  else if(!strcmp(keyword,"TS_SECTOR5_CFG_STOFP2")) v1495regs[0x1478/4] |= (value & 0x3)<<4;
	  else if(!strcmp(keyword,"TS_SECTOR5_CFG_STOFP3")) v1495regs[0x1478/4] |= (value & 0x3);

	  else if(!strcmp(keyword,"TS_SECTOR6_CFG_ECPCC"))  v1495regs[0x1578/4] |= (value & 0x3)<<16;
	  else if(!strcmp(keyword,"TS_SECTOR6_CFG_ECECC"))  v1495regs[0x1578/4] |= (value & 0x3)<<12;
	  else if(!strcmp(keyword,"TS_SECTOR6_CFG_STOFP1")) v1495regs[0x1578/4] |= (value & 0x3)<<8;
	  else if(!strcmp(keyword,"TS_SECTOR6_CFG_STOFP2")) v1495regs[0x1578/4] |= (value & 0x3)<<4;
	  else if(!strcmp(keyword,"TS_SECTOR6_CFG_STOFP3")) v1495regs[0x1578/4] |= (value & 0x3);

	  else if(!strcmp(keyword,"TS_TRIG1_CFG_MORAB"))  v1495regs[0x2000/4] |= (value & 0x3)<<4;
	  else if(!strcmp(keyword,"TS_TRIG1_CFG_ST"))     v1495regs[0x2000/4] |= (value & 0x3);
	  else if(!strcmp(keyword,"TS_TRIG2_CFG_MORAB"))  v1495regs[0x2100/4] |= (value & 0x3)<<4;
	  else if(!strcmp(keyword,"TS_TRIG2_CFG_ST"))     v1495regs[0x2100/4] |= (value & 0x3);
	  else if(!strcmp(keyword,"TS_TRIG3_CFG_MORAB"))  v1495regs[0x2200/4] |= (value & 0x3)<<4;
	  else if(!strcmp(keyword,"TS_TRIG3_CFG_ST"))     v1495regs[0x2200/4] |= (value & 0x3);
	  else if(!strcmp(keyword,"TS_TRIG4_CFG_MORAB"))  v1495regs[0x2300/4] |= (value & 0x3)<<4;
	  else if(!strcmp(keyword,"TS_TRIG4_CFG_ST"))     v1495regs[0x2300/4] |= (value & 0x3);
	  else if(!strcmp(keyword,"TS_TRIG5_CFG_MORAB"))  v1495regs[0x2400/4] |= (value & 0x3)<<4;
	  else if(!strcmp(keyword,"TS_TRIG5_CFG_ST"))     v1495regs[0x2400/4] |= (value & 0x3);
	  else if(!strcmp(keyword,"TS_TRIG6_CFG_MORAB"))  v1495regs[0x2500/4] |= (value & 0x3)<<4;
	  else if(!strcmp(keyword,"TS_TRIG6_CFG_ST"))     v1495regs[0x2500/4] |= (value & 0x3);
	  else if(!strcmp(keyword,"TS_TRIG7_CFG_MORAB"))  v1495regs[0x2600/4] |= (value & 0x3)<<4;
	  else if(!strcmp(keyword,"TS_TRIG7_CFG_ST"))     v1495regs[0x2600/4] |= (value & 0x3);
	  else if(!strcmp(keyword,"TS_TRIG8_CFG_MORAB"))  v1495regs[0x2700/4] |= (value & 0x3)<<4;
	  else if(!strcmp(keyword,"TS_TRIG8_CFG_ST"))     v1495regs[0x2700/4] |= (value & 0x3);
	  else if(!strcmp(keyword,"TS_TRIG9_CFG_MORAB"))  v1495regs[0x2800/4] |= (value & 0x3)<<4;
	  else if(!strcmp(keyword,"TS_TRIG9_CFG_ST"))     v1495regs[0x2800/4] |= (value & 0x3);
	  else if(!strcmp(keyword,"TS_TRIG10_CFG_MORAB")) v1495regs[0x2900/4] |= (value & 0x3)<<4;
	  else if(!strcmp(keyword,"TS_TRIG10_CFG_ST"))    v1495regs[0x2900/4] |= (value & 0x3);
	  else if(!strcmp(keyword,"TS_TRIG11_CFG_MORAB")) v1495regs[0x2A00/4] |= (value & 0x3)<<4;
	  else if(!strcmp(keyword,"TS_TRIG11_CFG_ST"))    v1495regs[0x2A00/4] |= (value & 0x3);
	  else if(!strcmp(keyword,"TS_TRIG12_CFG_MORAB")) v1495regs[0x2B00/4] |= (value & 0x3)<<4;
	  else if(!strcmp(keyword,"TS_TRIG12_CFG_ST"))    v1495regs[0x2B00/4] |= (value & 0x3);

      /*TS_STMULT_THRESHOLD*/
	  else if(!strcmp(keyword,"TS_ST_MULT_MAX")) v1495regs[0x3010/4] |= (value & 0x1F)<<8;
	  else if(!strcmp(keyword,"TS_ST_MULT_MIN")) v1495regs[0x3010/4] |= value & 0x1F;

	  else if(!strcmp(keyword,"TS_L2_SECLOGIC"))     v1495regs[0x3104/4] = value; /*32bit value*/
	  else if(!strcmp(keyword,"TS_L2_OUTPUTDELAY"))  v1495regs[0x3108/4] = value & 0x3FF;
	  else if(!strcmp(keyword,"TS_L2_OUTPUTWIDTH"))  v1495regs[0x310C/4] = value & 0xFF;

	  else if(!strcmp(keyword,"TS_EVENT_CFG_LATCH_WIDTH"))  v1495regs[0x302C/4] = (value & 0xFF)<<8;
	  else if(!strcmp(keyword,"TS_EVENT_CFG_LATCH_DELAY"))  v1495regs[0x302C/4] = (value & 0xFF)<<16;

      else if(!strcmp(keyword,"TS_TRIG1_LUTFILE"))  LOADLUTS(lutfilename,1)
      else if(!strcmp(keyword,"TS_TRIG2_LUTFILE"))  LOADLUTS(lutfilename,2)
      else if(!strcmp(keyword,"TS_TRIG3_LUTFILE"))  LOADLUTS(lutfilename,3)
      else if(!strcmp(keyword,"TS_TRIG4_LUTFILE"))  LOADLUTS(lutfilename,4)
      else if(!strcmp(keyword,"TS_TRIG5_LUTFILE"))  LOADLUTS(lutfilename,5)
      else if(!strcmp(keyword,"TS_TRIG6_LUTFILE"))  LOADLUTS(lutfilename,6)
      else if(!strcmp(keyword,"TS_TRIG7_LUTFILE"))  LOADLUTS(lutfilename,7)
      else if(!strcmp(keyword,"TS_TRIG8_LUTFILE"))  LOADLUTS(lutfilename,8)
      else if(!strcmp(keyword,"TS_TRIG9_LUTFILE"))  LOADLUTS(lutfilename,9)
	  else if(!strcmp(keyword,"TS_TRIG10_LUTFILE")) LOADLUTS(lutfilename,10)
      else if(!strcmp(keyword,"TS_TRIG11_LUTFILE")) LOADLUTS(lutfilename,11)
      else if(!strcmp(keyword,"TS_TRIG12_LUTFILE")) LOADLUTS(lutfilename,12)

      else if(!strcmp(keyword,"TS_CFG_LUT_L2FILE")) ret=v1495LUTDownload(addr,lutfilename,13,0);

      else
	  {
        printf("v1495ConfigDownload ERROR: unknown keyword >%s< (%d 0x%02x)\n",keyword,ch[0],ch[0]);
        return(-3);
	  }

      if(ret!=0) return(-4);
	}
  }

  fclose(fd);


  /* ST MAX MULTIPLICITY cannot be more then 24 */
  if( ((v1495regs[0x3010/4]>>8) & 0x1F) > 24) {regs[0x3010/4] &= 0x1F; v1495regs[0x3010/4] |= 24<<8;}


  /*for(i=0; i<31; i++) printf("[%2d] 0x%04x\n",i,v1495regs[i]);*/

  semTake(v1495scalers_lock, WAIT_FOREVER);


  /************************************************/
  /* IMPORTANT: NEED REVERSE I/O HERE FOR PPC !!! */

  /* download */
  nregs = NREGS_ACTUAL;
  ptr32 = (volatile unsigned int *)v1495base;
  mem32 = (unsigned int *) &v1495regs[0];
  for(i=(nregs-1); i>=0; i--)
  {
    ptr32[regs_actual[i]] = mem32[regs_actual[i]];
    EIEIO;
    SYNC;
	/*
printf("v1495ConfigDownload: [%3d] write to address 0x%04x: %d %d\n",
i,(regs_actual[i]*4),mem32[regs_actual[i]],ptr32[regs_actual[i]]);
	*/
  }

  /*upload and compare*/
  ptr32 = (volatile unsigned int *)v1495base;
  mem32 = (unsigned int *) &regs[0];
  for(i=(nregs-1); i>=0; i--)
  {
    EIEIO;
    SYNC;
    mem32[regs_actual[i]] = ptr32[regs_actual[i]];
  }

  semGive(v1495scalers_lock);

  /*printf("nregs = %d\n",nregs);*/
  for(i=0; i<nregs; i++)
  {
    if(v1495regs[regs_actual[i]] != regs[regs_actual[i]])
	{
      printf("v1495ConfigDownload ERROR: [0x%04x] load %d, read back %d\n",
        (regs_actual[i]*4),v1495regs[regs_actual[i]],regs[regs_actual[i]]);
      /*return(-4);*/
	}
    else
	{
      printf("v1495ConfigDownload PASSED: [0x%04x] load %d, read back %d\n",
        (regs_actual[i]*4),v1495regs[regs_actual[i]],regs[regs_actual[i]]);
	}
  }

  return(0);
}

/*
v1495ConfigUpload(0x08510000,"/home/boiarino/zzz.dat")
*/

/* reads hardware setting and write them to the file */
int
v1495ConfigUpload(unsigned int addr, char *fname)
{
  FILE *fd;
  int i, nregs;
  volatile unsigned int *ptr32;
  unsigned int *mem32;
  unsigned int v1495base;

  if(sysBusToLocalAdrs(0x39,addr&0xFFFFFF,&v1495base) < 0)
  {
    v1495base = 0;
    printf("v1495ConfigUpload ERROR: undefined base address\n");
    return(-1);
  }

  if(v1495base == 0)
  {
    printf("v1495ConfigUpload ERROR: undefined base address\n");
    return(-1);
  }

  if( (fd = fopen(fname,"w")) == NULL)
  {
    printf("v1495ConfigUpload: Can't create output file >%s<\n",fname);
    return(-2);
  }

  nregs =  NREGS_ACTUAL;
  printf("nregs=%d\n",nregs);
  for(i=0; i<NREGS; i++) regs[i] = 0;

  semTake(v1495scalers_lock, WAIT_FOREVER);

  ptr32 = (volatile unsigned int *)v1495base;
  mem32 = (unsigned int *) &regs[0];

  /* NEED REVERSE READOUT HERE */
  for(i=(nregs-1); i>=0; i--)
  {
    mem32[regs_actual[i]] = ptr32[regs_actual[i]];
  }

  semGive(v1495scalers_lock);

  /* ST MAX MULTIPLICITY cannot be more then 24 */
  if( ((regs[0x3010/4]>>8) & 0x1F) > 24) {regs[0x3010/4] &= 0x1F; regs[0x3010/4] |= 24<<8;}


  fprintf(fd,"# Hall B Trigger Module Configuration Register Settings\n");
  /*fprintf(fd,"# Uploaded from hardware: Wednesday, March 26, 2008 at 11:13:55AM\n\n");*/
  fprintf(fd,"# version Nov 2010, Uploaded from hardware: %s\n",ctime(NULL));


  fprintf(fd,"TS_MORA_DELAY             %6d\n", regs[0x3020/4] & 0x1F);
  fprintf(fd,"TS_MORB_DELAY             %6d\n", regs[0x3024/4] & 0x1F);

  fprintf(fd,"TS_TOFP1_1_DELAY          %6d\n", regs[0x1000/4] & 0x1F);
  fprintf(fd,"TS_TOFP2_1_DELAY          %6d\n", regs[0x1004/4] & 0x1F);
  fprintf(fd,"TS_TOFP3_1_DELAY          %6d\n", regs[0x1008/4] & 0x1F);
  fprintf(fd,"TS_TOFP1_2_DELAY          %6d\n", regs[0x1100/4] & 0x1F);
  fprintf(fd,"TS_TOFP2_2_DELAY          %6d\n", regs[0x1104/4] & 0x1F);
  fprintf(fd,"TS_TOFP3_2_DELAY          %6d\n", regs[0x1108/4] & 0x1F);
  fprintf(fd,"TS_TOFP1_3_DELAY          %6d\n", regs[0x1200/4] & 0x1F);
  fprintf(fd,"TS_TOFP2_3_DELAY          %6d\n", regs[0x1204/4] & 0x1F);
  fprintf(fd,"TS_TOFP3_3_DELAY          %6d\n", regs[0x1208/4] & 0x1F);
  fprintf(fd,"TS_TOFP1_4_DELAY          %6d\n", regs[0x1300/4] & 0x1F);
  fprintf(fd,"TS_TOFP2_4_DELAY          %6d\n", regs[0x1304/4] & 0x1F);
  fprintf(fd,"TS_TOFP3_4_DELAY          %6d\n", regs[0x1308/4] & 0x1F);
  fprintf(fd,"TS_TOFP1_5_DELAY          %6d\n", regs[0x1400/4] & 0x1F);
  fprintf(fd,"TS_TOFP2_5_DELAY          %6d\n", regs[0x1404/4] & 0x1F);
  fprintf(fd,"TS_TOFP3_5_DELAY          %6d\n", regs[0x1408/4] & 0x1F);
  fprintf(fd,"TS_TOFP1_6_DELAY          %6d\n", regs[0x1500/4] & 0x1F);
  fprintf(fd,"TS_TOFP2_6_DELAY          %6d\n", regs[0x1504/4] & 0x1F);
  fprintf(fd,"TS_TOFP3_6_DELAY          %6d\n", regs[0x1508/4] & 0x1F);

  fprintf(fd,"TS_ECin1P_DELAY           %6d\n", regs[0x1020/4] & 0x1F);
  fprintf(fd,"TS_ECin2P_DELAY           %6d\n", regs[0x1120/4] & 0x1F);
  fprintf(fd,"TS_ECin3P_DELAY           %6d\n", regs[0x1220/4] & 0x1F);
  fprintf(fd,"TS_ECin4P_DELAY           %6d\n", regs[0x1320/4] & 0x1F);
  fprintf(fd,"TS_ECin5P_DELAY           %6d\n", regs[0x1420/4] & 0x1F);
  fprintf(fd,"TS_ECin6P_DELAY           %6d\n", regs[0x1520/4] & 0x1F);

  fprintf(fd,"TS_ECin1E_DELAY           %6d\n", regs[0x1028/4] & 0x1F);
  fprintf(fd,"TS_ECin2E_DELAY           %6d\n", regs[0x1128/4] & 0x1F);
  fprintf(fd,"TS_ECin3E_DELAY           %6d\n", regs[0x1228/4] & 0x1F);
  fprintf(fd,"TS_ECin4E_DELAY           %6d\n", regs[0x1328/4] & 0x1F);
  fprintf(fd,"TS_ECin5E_DELAY           %6d\n", regs[0x1428/4] & 0x1F);
  fprintf(fd,"TS_ECin6E_DELAY           %6d\n", regs[0x1528/4] & 0x1F);

  fprintf(fd,"TS_ECtot1P_DELAY          %6d\n", regs[0x1024/4] & 0x1F);
  fprintf(fd,"TS_ECtot2P_DELAY          %6d\n", regs[0x1124/4] & 0x1F);
  fprintf(fd,"TS_ECtot3P_DELAY          %6d\n", regs[0x1224/4] & 0x1F);
  fprintf(fd,"TS_ECtot4P_DELAY          %6d\n", regs[0x1324/4] & 0x1F);
  fprintf(fd,"TS_ECtot5P_DELAY          %6d\n", regs[0x1424/4] & 0x1F);
  fprintf(fd,"TS_ECtot6P_DELAY          %6d\n", regs[0x1524/4] & 0x1F);

  fprintf(fd,"TS_ECtot1E_DELAY          %6d\n", regs[0x102C/4] & 0x1F);
  fprintf(fd,"TS_ECtot2E_DELAY          %6d\n", regs[0x112C/4] & 0x1F);
  fprintf(fd,"TS_ECtot3E_DELAY          %6d\n", regs[0x122C/4] & 0x1F);
  fprintf(fd,"TS_ECtot4E_DELAY          %6d\n", regs[0x132C/4] & 0x1F);
  fprintf(fd,"TS_ECtot5E_DELAY          %6d\n", regs[0x142C/4] & 0x1F);
  fprintf(fd,"TS_ECtot6E_DELAY          %6d\n", regs[0x152C/4] & 0x1F);

  fprintf(fd,"TS_CC1_DELAY              %6d\n", regs[0x101C/4] & 0x1F);
  fprintf(fd,"TS_CC2_DELAY              %6d\n", regs[0x111C/4] & 0x1F);
  fprintf(fd,"TS_CC3_DELAY              %6d\n", regs[0x121C/4] & 0x1F);
  fprintf(fd,"TS_CC4_DELAY              %6d\n", regs[0x131C/4] & 0x1F);
  fprintf(fd,"TS_CC5_DELAY              %6d\n", regs[0x141C/4] & 0x1F);
  fprintf(fd,"TS_CC6_DELAY              %6d\n", regs[0x151C/4] & 0x1F);

  fprintf(fd,"TS_ST1_DELAY              %6d\n", regs[0x100C/4] & 0x1F);
  fprintf(fd,"TS_ST2_DELAY              %6d\n", regs[0x1010/4] & 0x1F);
  fprintf(fd,"TS_ST3_DELAY              %6d\n", regs[0x1014/4] & 0x1F);
  fprintf(fd,"TS_ST4_DELAY              %6d\n", regs[0x1018/4] & 0x1F);
  fprintf(fd,"TS_ST5_DELAY              %6d\n", regs[0x110C/4] & 0x1F);
  fprintf(fd,"TS_ST6_DELAY              %6d\n", regs[0x1110/4] & 0x1F);
  fprintf(fd,"TS_ST7_DELAY              %6d\n", regs[0x1114/4] & 0x1F);
  fprintf(fd,"TS_ST8_DELAY              %6d\n", regs[0x1118/4] & 0x1F);
  fprintf(fd,"TS_ST9_DELAY              %6d\n", regs[0x120C/4] & 0x1F);
  fprintf(fd,"TS_ST10_DELAY             %6d\n", regs[0x1210/4] & 0x1F);
  fprintf(fd,"TS_ST11_DELAY             %6d\n", regs[0x1214/4] & 0x1F);
  fprintf(fd,"TS_ST12_DELAY             %6d\n", regs[0x1218/4] & 0x1F);
  fprintf(fd,"TS_ST13_DELAY             %6d\n", regs[0x130C/4] & 0x1F);
  fprintf(fd,"TS_ST14_DELAY             %6d\n", regs[0x1310/4] & 0x1F);
  fprintf(fd,"TS_ST15_DELAY             %6d\n", regs[0x1314/4] & 0x1F);
  fprintf(fd,"TS_ST16_DELAY             %6d\n", regs[0x1318/4] & 0x1F);
  fprintf(fd,"TS_ST17_DELAY             %6d\n", regs[0x140C/4] & 0x1F);
  fprintf(fd,"TS_ST18_DELAY             %6d\n", regs[0x1410/4] & 0x1F);
  fprintf(fd,"TS_ST19_DELAY             %6d\n", regs[0x1414/4] & 0x1F);
  fprintf(fd,"TS_ST20_DELAY             %6d\n", regs[0x1418/4] & 0x1F);
  fprintf(fd,"TS_ST21_DELAY             %6d\n", regs[0x150C/4] & 0x1F);
  fprintf(fd,"TS_ST22_DELAY             %6d\n", regs[0x1510/4] & 0x1F);
  fprintf(fd,"TS_ST23_DELAY             %6d\n", regs[0x1514/4] & 0x1F);
  fprintf(fd,"TS_ST24_DELAY             %6d\n", regs[0x1518/4] & 0x1F);

  fprintf(fd,"TS_PRESCALE_TRIG1         %6d\n", regs[0x2010/4] & 0x3FF);
  fprintf(fd,"TS_PRESCALE_TRIG2         %6d\n", regs[0x2110/4] & 0x3FF);
  fprintf(fd,"TS_PRESCALE_TRIG3         %6d\n", regs[0x2210/4] & 0x3FF);
  fprintf(fd,"TS_PRESCALE_TRIG4         %6d\n", regs[0x2310/4] & 0x3FF);
  fprintf(fd,"TS_PRESCALE_TRIG5         %6d\n", regs[0x2410/4] & 0x3FF);
  fprintf(fd,"TS_PRESCALE_TRIG6         %6d\n", regs[0x2510/4] & 0x3FF);
  fprintf(fd,"TS_PRESCALE_TRIG7         %6d\n", regs[0x2610/4] & 0x3FF);
  fprintf(fd,"TS_PRESCALE_TRIG8         %6d\n", regs[0x2710/4] & 0x3FF);
  fprintf(fd,"TS_PRESCALE_TRIG9         %6d\n", regs[0x2810/4] & 0x3FF);
  fprintf(fd,"TS_PRESCALE_TRIG10        %6d\n", regs[0x2910/4] & 0x3FF);
  fprintf(fd,"TS_PRESCALE_TRIG11        %6d\n", regs[0x2A10/4] & 0x3FF);
  fprintf(fd,"TS_PRESCALE_TRIG12        %6d\n", regs[0x2B10/4] & 0x3FF);

  fprintf(fd,"TS_SECTOR1_CFG_ECPCC      %6d\n", (regs[0x1078/4]>>16) & 0x3);
  fprintf(fd,"TS_SECTOR1_CFG_ECECC      %6d\n", (regs[0x1078/4]>>12) & 0x3);
  fprintf(fd,"TS_SECTOR1_CFG_STOFP1     %6d\n", (regs[0x1078/4]>>8) & 0x3);
  fprintf(fd,"TS_SECTOR1_CFG_STOFP2     %6d\n", (regs[0x1078/4]>>4) & 0x3);
  fprintf(fd,"TS_SECTOR1_CFG_STOFP3     %6d\n", regs[0x1078/4] & 0x3);

  fprintf(fd,"TS_SECTOR2_CFG_ECPCC      %6d\n", (regs[0x1178/4]>>16) & 0x3);
  fprintf(fd,"TS_SECTOR2_CFG_ECECC      %6d\n", (regs[0x1178/4]>>12) & 0x3);
  fprintf(fd,"TS_SECTOR2_CFG_STOFP1     %6d\n", (regs[0x1178/4]>>8) & 0x3);
  fprintf(fd,"TS_SECTOR2_CFG_STOFP2     %6d\n", (regs[0x1178/4]>>4) & 0x3);
  fprintf(fd,"TS_SECTOR2_CFG_STOFP3     %6d\n", regs[0x1178/4] & 0x3);

  fprintf(fd,"TS_SECTOR3_CFG_ECPCC      %6d\n", (regs[0x1278/4]>>16) & 0x3);
  fprintf(fd,"TS_SECTOR3_CFG_ECECC      %6d\n", (regs[0x1278/4]>>12) & 0x3);
  fprintf(fd,"TS_SECTOR3_CFG_STOFP1     %6d\n", (regs[0x1278/4]>>8) & 0x3);
  fprintf(fd,"TS_SECTOR3_CFG_STOFP2     %6d\n", (regs[0x1278/4]>>4) & 0x3);
  fprintf(fd,"TS_SECTOR3_CFG_STOFP3     %6d\n", regs[0x1278/4] & 0x3);

  fprintf(fd,"TS_SECTOR4_CFG_ECPCC      %6d\n", (regs[0x1378/4]>>16) & 0x3);
  fprintf(fd,"TS_SECTOR4_CFG_ECECC      %6d\n", (regs[0x1378/4]>>12) & 0x3);
  fprintf(fd,"TS_SECTOR4_CFG_STOFP1     %6d\n", (regs[0x1378/4]>>8) & 0x3);
  fprintf(fd,"TS_SECTOR4_CFG_STOFP2     %6d\n", (regs[0x1378/4]>>4) & 0x3);
  fprintf(fd,"TS_SECTOR4_CFG_STOFP3     %6d\n", regs[0x1378/4] & 0x3);

  fprintf(fd,"TS_SECTOR5_CFG_ECPCC      %6d\n", (regs[0x1478/4]>>16) & 0x3);
  fprintf(fd,"TS_SECTOR5_CFG_ECECC      %6d\n", (regs[0x1478/4]>>12) & 0x3);
  fprintf(fd,"TS_SECTOR5_CFG_STOFP1     %6d\n", (regs[0x1478/4]>>8) & 0x3);
  fprintf(fd,"TS_SECTOR5_CFG_STOFP2     %6d\n", (regs[0x1478/4]>>4) & 0x3);
  fprintf(fd,"TS_SECTOR5_CFG_STOFP3     %6d\n", regs[0x1478/4] & 0x3);

  fprintf(fd,"TS_SECTOR6_CFG_ECPCC      %6d\n", (regs[0x1578/4]>>16) & 0x3);
  fprintf(fd,"TS_SECTOR6_CFG_ECECC      %6d\n", (regs[0x1578/4]>>12) & 0x3);
  fprintf(fd,"TS_SECTOR6_CFG_STOFP1     %6d\n", (regs[0x1578/4]>>8) & 0x3);
  fprintf(fd,"TS_SECTOR6_CFG_STOFP2     %6d\n", (regs[0x1578/4]>>4) & 0x3);
  fprintf(fd,"TS_SECTOR6_CFG_STOFP3     %6d\n", regs[0x1578/4] & 0x3);

  fprintf(fd,"TS_TRIG1_CFG_MORAB        %6d\n", (regs[0x2000/4]>>4) & 0x3);
  fprintf(fd,"TS_TRIG1_CFG_ST           %6d\n", regs[0x2000/4] & 0x3);
  fprintf(fd,"TS_TRIG2_CFG_MORAB        %6d\n", (regs[0x2100/4]>>4) & 0x3);
  fprintf(fd,"TS_TRIG2_CFG_ST           %6d\n", regs[0x2100/4] & 0x3);
  fprintf(fd,"TS_TRIG3_CFG_MORAB        %6d\n", (regs[0x2200/4]>>4) & 0x3);
  fprintf(fd,"TS_TRIG3_CFG_ST           %6d\n", regs[0x2200/4] & 0x3);
  fprintf(fd,"TS_TRIG4_CFG_MORAB        %6d\n", (regs[0x2300/4]>>4) & 0x3);
  fprintf(fd,"TS_TRIG4_CFG_ST           %6d\n", regs[0x2300/4] & 0x3);
  fprintf(fd,"TS_TRIG5_CFG_MORAB        %6d\n", (regs[0x2400/4]>>4) & 0x3);
  fprintf(fd,"TS_TRIG5_CFG_ST           %6d\n", regs[0x2400/4] & 0x3);
  fprintf(fd,"TS_TRIG6_CFG_MORAB        %6d\n", (regs[0x2500/4]>>4) & 0x3);
  fprintf(fd,"TS_TRIG6_CFG_ST           %6d\n", regs[0x2500/4] & 0x3);
  fprintf(fd,"TS_TRIG7_CFG_MORAB        %6d\n", (regs[0x2600/4]>>4) & 0x3);
  fprintf(fd,"TS_TRIG7_CFG_ST           %6d\n", regs[0x2600/4] & 0x3);
  fprintf(fd,"TS_TRIG8_CFG_MORAB        %6d\n", (regs[0x2700/4]>>4) & 0x3);
  fprintf(fd,"TS_TRIG8_CFG_ST           %6d\n", regs[0x2700/4] & 0x3);
  fprintf(fd,"TS_TRIG9_CFG_MORAB        %6d\n", (regs[0x2800/4]>>4) & 0x3);
  fprintf(fd,"TS_TRIG9_CFG_ST           %6d\n", regs[0x2800/4] & 0x3);
  fprintf(fd,"TS_TRIG10_CFG_MORAB       %6d\n", (regs[0x2900/4]>>4) & 0x3);
  fprintf(fd,"TS_TRIG10_CFG_ST          %6d\n", regs[0x2900/4] & 0x3);
  fprintf(fd,"TS_TRIG11_CFG_MORAB       %6d\n", (regs[0x2A00/4]>>4) & 0x3);
  fprintf(fd,"TS_TRIG11_CFG_ST          %6d\n", regs[0x2A00/4] & 0x3);
  fprintf(fd,"TS_TRIG12_CFG_MORAB       %6d\n", (regs[0x2B00/4]>>4) & 0x3);
  fprintf(fd,"TS_TRIG12_CFG_ST          %6d\n", regs[0x2B00/4] & 0x3);

  /*TS_STMULT_THRESHOLD*/
  fprintf(fd,"TS_ST_MULT_MAX            %6d\n", (regs[0x3010/4]>>8) & 0x1F);
  fprintf(fd,"TS_ST_MULT_MIN            %6d\n", regs[0x3010/4] & 0x1F);

  fprintf(fd,"TS_L2_SECLOGIC            %6d\n", regs[0x3104/4]); /*32bit value*/
  fprintf(fd,"TS_L2_OUTPUTDELAY         %6d\n", regs[0x3108/4] & 0x3FF);
  fprintf(fd,"TS_L2_OUTPUTWIDTH         %6d\n", regs[0x310C/4] & 0xFF);

  fprintf(fd,"TS_EVENT_CFG_LATCH_WIDTH  %6d\n", (regs[0x302C/4]>>8) & 0xFF);
  fprintf(fd,"TS_EVENT_CFG_LATCH_DELAY  %6d\n", (regs[0x302C/4]>>16) & 0xFF);

  fclose(fd);

  return(0);
}


#else /*NOV_2010*/ /* following part is for PHOTON_RUN version */



#define STRLEN 1024


/**************************************************/
/* download/upload functions for the lookup table */

/* for example:

ld < /usr/local/clas/devel/coda/src/rol/VXWORKS_ppc/obj/v1495.o

v1495LUTDownload(0x08510000,"/usr/local/clas/parms/trigger/config/tpenew/tables/bit1_old.lut",1,0)
v1495LUTDownload(0x08510000,"/usr/local/clas/parms/trigger/config/tpenew/tables/bit2_old.lut",2,0)
v1495LUTDownload(0x08510000,"/usr/local/clas/parms/trigger/config/tpenew/tables/bit3_old.lut",3,0)
v1495LUTDownload(0x08510000,"/usr/local/clas/parms/trigger/config/tpenew/tables/bit4_old.lut",4,0)
v1495LUTDownload(0x08510000,"/usr/local/clas/parms/trigger/config/tpenew/tables/bit5_old.lut",5,0)
v1495LUTDownload(0x08510000,"/usr/local/clas/parms/trigger/config/tpenew/tables/bit6_old.lut",6,0)

v1495LUTDownload(0x08510000,"/usr/local/clas/parms/trigger/config/g9frost/tables/trig_ecc_STOF1_bit_1of6.lut",1,0)
v1495LUTDownload(0x08510000,"/usr/local/clas/parms/trigger/config/g9frost/tables/trig_ecc_STOF1_bit_2of6.lut",2,0)

v1495LUTDownload(0x08510000,"/usr/local/clas/parms/trigger/config/cosmic/tables/12_zero.lut",1,0)
v1495LUTDownload(0x08510000,"/usr/local/clas/parms/trigger/config/cosmic/tables/ecp_tof180.lut",1,1)
v1495LUTDownload(0x08510000,"/usr/local/clas/parms/trigger/config/cosmic/tables/L2_test.lut",13,1)

v1495LUTDownload(0x08510000,"/usr/local/clas/parms/trigger/config/g14b/tables/l2lut_pass12.lut",13,1)
v1495LUTDownload(0x08510000,"/usr/local/clas/parms/trigger/config/g14b/tables/l2lut_test.lut",13,1)

v1495LUTDownload(0x08510000,"/usr/local/clas/parms/trigger/config/g14b/tables/12_one.lut",12,0)
*/

#define NLUT   4096 /* the number of loookup table addresses */

static unsigned int v1495lut[NLUT];
static unsigned int lut[NLUT];

/* reads config file and updates hardware lookup tables */
int
v1495LUTDownload(unsigned int addr, char *fname, int trigbit, int ecc_ecp)
{
  FILE *fd;
  char *ch, str[STRLEN], keyword[STRLEN];
  int i, nregs, nline;
  unsigned int value;
  volatile unsigned int *ptr32;
  unsigned int *mem32;
  unsigned int v1495base;

  if(trigbit<1 || trigbit>13)
  {
    printf("v1495LUTDownload: ERROR: trigbit out of range - exit\n");
    return(-1);
  }

  if(ecc_ecp<0||ecc_ecp>1)
  {
    printf("v1495LUTDownload: ERROR: wrong ecc_ecp=%d\n",ecc_ecp);
    return(-2);
  }

  if(sysBusToLocalAdrs(0x39,addr&0xFFFFFF,&v1495base) < 0)
  {
    v1495base = 0;
    printf("v1495ConfigDownload ERROR: undefined base address\n");
    return(-3);
  }
  else
  {
    printf("v1495ConfigDownload: v1495base = 0x%08x\n",v1495base);
  }

  if(v1495base == 0)
  {
    printf("v1495LUTDownload ERROR: undefined base address\n");
    return(-4);
  }

  if( (fd = fopen(fname,"r")) == NULL)
  {
    printf("v1495LUTDownload: ERROR: Can't open config file >%s<\n",fname);
    return(-5);
  }

  /*v1495LUTDownload("/home/clasrun/g12/lut/tables/trig_ecp_STOF2.lut",1,1)*/


  nline = 0;
  for(i=0; i<NLUT; i++) v1495lut[i] = lut[i] = 0;
  while((ch = fgets(str, STRLEN, fd)) != NULL)
  {
	/*printf(">%s< %d\n",str,ch);*/
    if( ch[0] == '#' || ch[0] == ' ' || ch[0] == '\t' || ch[0] == '\n' || ch[0] == '\r' )
    {
      ;
    }
    else
    {
      sscanf(str,"%x %s",&value,keyword);
      /*printf("value = %d key >%s<\n",value,keyword);*/
      if(nline < NLUT)
	  {
        v1495lut[nline++] = value;
	  }
      else
	  {
        printf("v1495LUTDownload: File too long (nline=%d) - interrupt reading\n",nline);
        return(-6);
	  }
	}
  }

  fclose(fd);

  /*for(i=0; i<31; i++) printf("[%2d] 0x%04x\n",i,v1495lut[i]);*/

  /*
  semTake(v1495scalers_lock, WAIT_FOREVER);
  */

  /* download */
  ptr32 = (volatile unsigned int *)(v1495base + TS_CFG_LUT_BASE +
             (((trigbit-1)&0xF)<<9)+(ecc_ecp*0x1800));

  printf("v1495LUTDownload: downloading LUT to address 0x%08x\n",ptr32);

  *ptr32 = 0x80000000; /* reset address pointer */
  EIEIO;
  SYNC;
  for(i=0; i<NLUT; i++)
  {
    *ptr32 = v1495lut[i];
    EIEIO;
    SYNC;
  }

  /*upload and compare*/
  *ptr32 = 0x80000000; /* reset address pointer */
  EIEIO;
  SYNC;
  for(i=0; i<NLUT; i++)
  {
    lut[i] = *ptr32;
    EIEIO;
    SYNC;
  }

  /*
  semGive(v1495scalers_lock);
  */

  printf("/nv1495LUTDownload: reading back and comparing\n");
  for(i=0; i<NLUT; i++)
  {
    if(v1495lut[i] != lut[i])
	{
      printf("v1495LUTDownload ERROR: [%4d][0x%02x] load 0x%04x, read back 0x%04x\n",
        i,i,v1495lut[i],lut[i]);
      return(-7);
	}
    else
	{
      /*printf("v1495LUTDownload: [%4d][0x%02x] passed: 0x%04x = 0x%04x\n",i,i,v1495lut[i],lut[i])*/;
	}
  }

  return(0);
}


/* reads LUT from hardware and write it to the file */
/* NOT IMPLEMENTED YET !!!!!!!!!!!!!!!!*/
int
v1495LUTUpload(unsigned int addr, char *fname, int trigbit, int ecc_ecp)
{
  FILE *fd;
  unsigned short lut[NLUT];
  int i, j, nline;
  volatile unsigned short *ptr32;
  unsigned int v1495base;

  if(sysBusToLocalAdrs(0x39,addr&0xFFFFFF,&v1495base) < 0)
  {
    v1495base = 0;
    printf("v1495ConfigUpload ERROR: undefined base address\n");
    return(-1);
  }

  if(v1495base == 0)
  {
    printf("v1495LUTUpload ERROR: undefined base address\n");
    return(-1);
  }

  if( (fd = fopen(fname,"w")) == NULL)
  {
    printf("v1495LUTUpload: Can't create output file >%s<\n",fname);
    return(-2);
  }

  for(i=0; i<NLUT; i++) lut[i] = 0;

  semTake(v1495scalers_lock, WAIT_FOREVER);

  ptr32 = (volatile unsigned int *)(v1495base + TS_CFG_LUT_BASE +
             (((trigbit-1)&0xF)<<9)+(ecc_ecp*0x1800));
  *ptr32 = 0x80000000; /* reset address pointer */
  EIEIO;
  SYNC;
  for(i=0; i<NLUT; i++)
  {
    lut[i] = *ptr32;
    EIEIO;
    SYNC;
  }

  semGive(v1495scalers_lock);

  fprintf(fd,"# G12 Hall B Trigger Module Lookup Table Settings\n");
  /*fprintf(fd,"# Uploaded from hardware: Wednesday, March 26, 2008 at 11:13:55AM\n\n");*/
  fprintf(fd,"# Uploaded from hardware: %s\n",ctime(NULL));

  nline = 0;
  for(i=0; i<NLUT; i++)
  {
fprintf(fd,"%2x #0x%04x\n",lut[i],nline);
    nline ++;
  }

  fclose(fd);

  return(0);
}



#define NREGS  0x1000

static unsigned int v1495regs[NREGS];
static unsigned int regs[NREGS];

/*
v1495ConfigDownload(0x08510000,"/usr/local/clas/parms/trigger/config/tpenew/tables/tpe_old.dat")

v1495ConfigDownload(0x08510000,"/usr/local/clas/parms/trigger/config/cosmic/tables/tof60degrees_photon.dat")

v1495ConfigDownload(0x08510000,"/usr/local/clas/parms/trigger/config/g9frost/tables/g9frost_test.dat")

v1495ConfigDownload(0x08510000,"/usr/local/clas/parms/trigger/config/g14b/tables/g14b_mor.dat")
*/

/* reads config file and updates hardware registers */
int
v1495ConfigDownload(unsigned int addr, char *fname)
{
  FILE *fd;
  char *ch, str[STRLEN], keyword[STRLEN], lutfilename[STRLEN];
  int i, nregs, ret;
  unsigned int value;
  volatile unsigned int *ptr32;
  unsigned int *mem32;
  unsigned int v1495base;

  if(sysBusToLocalAdrs(0x39,addr&0xFFFFFF,&v1495base) < 0)
  {
    v1495base = 0;
    printf("v1495ConfigUpload ERROR: undefined base address\n");
    return(-1);
  }

  if(v1495base == 0)
  {
    printf("v1495ConfigDownload ERROR: undefined base address\n");
    return(-1);
  }

  if( (fd = fopen(fname,"r")) == NULL)
  {
    printf("v1495ConfigDownload: Can't open config file >%s<\n",fname);
    return(-2);
  }

  for(i=0; i<NREGS; i++) v1495regs[i] = regs[i] = 0;
  ret = 0;
  while((ch = fgets(str, STRLEN, fd)) != NULL)
  {
	/*printf(">%s< %d\n",str,ch);*/
    if( ch[0] == '#' || ch[0] == ' ' || ch[0] == '\t' || ch[0] == '\n' || ch[0] == '\r' )
    {
      ;
    }
    else
    {
      sscanf(str,"%s %s", keyword, lutfilename);
      value = atoi(lutfilename);
      printf("key >%s< lutfilename >%s< value = %d\n",keyword,lutfilename,value);

      /*sscanf(str,"%s %d", keyword, &value);*/
      /*printf("key >%s< value = %d\n",keyword,value);*/

      if(     !strcmp(keyword,"TS_MORA_DELAY")) v1495regs[0x00/4] = value & 0x1F;
      else if(!strcmp(keyword,"TS_MORB_DELAY")) v1495regs[0x04/4] = value & 0x1F;
	  else if(!strcmp(keyword,"TS_TOF1_DELAY")) v1495regs[0x08/4] = value & 0x1F;
	  else if(!strcmp(keyword,"TS_TOF2_DELAY")) v1495regs[0x0C/4] = value & 0x1F;
	  else if(!strcmp(keyword,"TS_TOF3_DELAY")) v1495regs[0x10/4] = value & 0x1F;
	  else if(!strcmp(keyword,"TS_TOF4_DELAY")) v1495regs[0x14/4] = value & 0x1F;
	  else if(!strcmp(keyword,"TS_TOF5_DELAY")) v1495regs[0x18/4] = value & 0x1F;
	  else if(!strcmp(keyword,"TS_TOF6_DELAY")) v1495regs[0x1C/4] = value & 0x1F;

      else if(!strcmp(keyword,"TS_ECin1P_DELAY")) v1495regs[0x20/4] = value & 0x1F;
      else if(!strcmp(keyword,"TS_ECin2P_DELAY")) v1495regs[0x24/4] = value & 0x1F;
      else if(!strcmp(keyword,"TS_ECin3P_DELAY")) v1495regs[0x28/4] = value & 0x1F;
      else if(!strcmp(keyword,"TS_ECin4P_DELAY")) v1495regs[0x2C/4] = value & 0x1F;
      else if(!strcmp(keyword,"TS_ECin5P_DELAY")) v1495regs[0x30/4] = value & 0x1F;
      else if(!strcmp(keyword,"TS_ECin6P_DELAY")) v1495regs[0x34/4] = value & 0x1F;

      else if(!strcmp(keyword,"TS_ECin1E_DELAY")) v1495regs[0x38/4] = value & 0x1F;
      else if(!strcmp(keyword,"TS_ECin2E_DELAY")) v1495regs[0x3C/4] = value & 0x1F;
      else if(!strcmp(keyword,"TS_ECin3E_DELAY")) v1495regs[0x40/4] = value & 0x1F;
      else if(!strcmp(keyword,"TS_ECin4E_DELAY")) v1495regs[0x44/4] = value & 0x1F;
      else if(!strcmp(keyword,"TS_ECin5E_DELAY")) v1495regs[0x48/4] = value & 0x1F;
      else if(!strcmp(keyword,"TS_ECin6E_DELAY")) v1495regs[0x4C/4] = value & 0x1F;
      else if(!strcmp(keyword,"TS_ECtot1P_DELAY")) v1495regs[0x50/4] = value & 0x1F;
      else if(!strcmp(keyword,"TS_ECtot2P_DELAY")) v1495regs[0x54/4] = value & 0x1F;
      else if(!strcmp(keyword,"TS_ECtot3P_DELAY")) v1495regs[0x58/4] = value & 0x1F;
      else if(!strcmp(keyword,"TS_ECtot4P_DELAY")) v1495regs[0x5C/4] = value & 0x1F;
      else if(!strcmp(keyword,"TS_ECtot5P_DELAY")) v1495regs[0x60/4] = value & 0x1F;
      else if(!strcmp(keyword,"TS_ECtot6P_DELAY")) v1495regs[0x64/4] = value & 0x1F;
      else if(!strcmp(keyword,"TS_ECtot1E_DELAY")) v1495regs[0x68/4] = value & 0x1F;
      else if(!strcmp(keyword,"TS_ECtot2E_DELAY")) v1495regs[0x6C/4] = value & 0x1F;
      else if(!strcmp(keyword,"TS_ECtot3E_DELAY")) v1495regs[0x70/4] = value & 0x1F;
      else if(!strcmp(keyword,"TS_ECtot4E_DELAY")) v1495regs[0x74/4] = value & 0x1F;
      else if(!strcmp(keyword,"TS_ECtot5E_DELAY")) v1495regs[0x78/4] = value & 0x1F;
      else if(!strcmp(keyword,"TS_ECtot6E_DELAY")) v1495regs[0x7C/4] = value & 0x1F;

      else if(!strcmp(keyword,"TS_CC1_DELAY")) v1495regs[0x80/4] = value & 0x1F;
      else if(!strcmp(keyword,"TS_CC2_DELAY")) v1495regs[0x84/4] = value & 0x1F;
      else if(!strcmp(keyword,"TS_CC3_DELAY")) v1495regs[0x88/4] = value & 0x1F;
      else if(!strcmp(keyword,"TS_CC4_DELAY")) v1495regs[0x8C/4] = value & 0x1F;
      else if(!strcmp(keyword,"TS_CC5_DELAY")) v1495regs[0x90/4] = value & 0x1F;
      else if(!strcmp(keyword,"TS_CC6_DELAY")) v1495regs[0x94/4] = value & 0x1F;

	  else if(!strcmp(keyword,"TS_ST1_DELAY")) v1495regs[0x98/4] = value & 0x1F;
	  else if(!strcmp(keyword,"TS_ST2_DELAY")) v1495regs[0x9C/4] = value & 0x1F;
	  else if(!strcmp(keyword,"TS_ST3_DELAY")) v1495regs[0xA0/4] = value & 0x1F;
	  else if(!strcmp(keyword,"TS_ST4_DELAY")) v1495regs[0xA4/4] = value & 0x1F;
	  else if(!strcmp(keyword,"TS_ST5_DELAY")) v1495regs[0xA8/4] = value & 0x1F;
	  else if(!strcmp(keyword,"TS_ST6_DELAY")) v1495regs[0xAC/4] = value & 0x1F;
	  else if(!strcmp(keyword,"TS_ST7_DELAY")) v1495regs[0xB0/4] = value & 0x1F;
	  else if(!strcmp(keyword,"TS_ST8_DELAY")) v1495regs[0xB4/4] = value & 0x1F;
	  else if(!strcmp(keyword,"TS_ST9_DELAY")) v1495regs[0xB8/4] = value & 0x1F;
	  else if(!strcmp(keyword,"TS_ST10_DELAY")) v1495regs[0xBC/4] = value & 0x1F;
	  else if(!strcmp(keyword,"TS_ST11_DELAY")) v1495regs[0xC0/4] = value & 0x1F;
	  else if(!strcmp(keyword,"TS_ST12_DELAY")) v1495regs[0xC4/4] = value & 0x1F;
	  else if(!strcmp(keyword,"TS_ST13_DELAY")) v1495regs[0xC8/4] = value & 0x1F;
	  else if(!strcmp(keyword,"TS_ST14_DELAY")) v1495regs[0xCC/4] = value & 0x1F;
	  else if(!strcmp(keyword,"TS_ST15_DELAY")) v1495regs[0xD0/4] = value & 0x1F;
	  else if(!strcmp(keyword,"TS_ST16_DELAY")) v1495regs[0xD4/4] = value & 0x1F;
	  else if(!strcmp(keyword,"TS_ST17_DELAY")) v1495regs[0xD8/4] = value & 0x1F;
	  else if(!strcmp(keyword,"TS_ST18_DELAY")) v1495regs[0xDC/4] = value & 0x1F;
	  else if(!strcmp(keyword,"TS_ST19_DELAY")) v1495regs[0xE0/4] = value & 0x1F;
	  else if(!strcmp(keyword,"TS_ST20_DELAY")) v1495regs[0xE4/4] = value & 0x1F;
	  else if(!strcmp(keyword,"TS_ST21_DELAY")) v1495regs[0xE8/4] = value & 0x1F;
	  else if(!strcmp(keyword,"TS_ST22_DELAY")) v1495regs[0xEC/4] = value & 0x1F;
	  else if(!strcmp(keyword,"TS_ST23_DELAY")) v1495regs[0xF0/4] = value & 0x1F;
	  else if(!strcmp(keyword,"TS_ST24_DELAY")) v1495regs[0xF4/4] = value & 0x1F;

	  else if(!strcmp(keyword,"TS_PERSIST_TRIG1")) v1495regs[0x100/4] = value & 0x7;
	  else if(!strcmp(keyword,"TS_PERSIST_TRIG2")) v1495regs[0x104/4] = value & 0x7;
	  else if(!strcmp(keyword,"TS_PERSIST_TRIG3")) v1495regs[0x108/4] = value & 0x7;
	  else if(!strcmp(keyword,"TS_PERSIST_TRIG4")) v1495regs[0x10C/4] = value & 0x7;
	  else if(!strcmp(keyword,"TS_PERSIST_TRIG5")) v1495regs[0x110/4] = value & 0x7;
	  else if(!strcmp(keyword,"TS_PERSIST_TRIG6")) v1495regs[0x114/4] = value & 0x7;
	  else if(!strcmp(keyword,"TS_PERSIST_TRIG7")) v1495regs[0x118/4] = value & 0x7;
	  else if(!strcmp(keyword,"TS_PERSIST_TRIG8")) v1495regs[0x11C/4] = value & 0x7;
	  else if(!strcmp(keyword,"TS_PERSIST_TRIG9")) v1495regs[0x120/4] = value & 0x7;
	  else if(!strcmp(keyword,"TS_PERSIST_TRIG10")) v1495regs[0x124/4] = value & 0x7;
	  else if(!strcmp(keyword,"TS_PERSIST_TRIG11")) v1495regs[0x128/4] = value & 0x7;
	  else if(!strcmp(keyword,"TS_PERSIST_TRIG12")) v1495regs[0x12C/4] = value & 0x7;

	  else if(!strcmp(keyword,"TS_PRESCALE_TRIG1"))  v1495regs[0x200/4] = value & 0x3FF;
	  else if(!strcmp(keyword,"TS_PRESCALE_TRIG2"))  v1495regs[0x204/4] = value & 0x3FF;
	  else if(!strcmp(keyword,"TS_PRESCALE_TRIG3"))  v1495regs[0x208/4] = value & 0x3FF;
	  else if(!strcmp(keyword,"TS_PRESCALE_TRIG4"))  v1495regs[0x20C/4] = value & 0x3FF;
	  else if(!strcmp(keyword,"TS_PRESCALE_TRIG5"))  v1495regs[0x210/4] = value & 0x3FF;
	  else if(!strcmp(keyword,"TS_PRESCALE_TRIG6"))  v1495regs[0x214/4] = value & 0x3FF;
	  else if(!strcmp(keyword,"TS_PRESCALE_TRIG7"))  v1495regs[0x218/4] = value & 0x3FF;
	  else if(!strcmp(keyword,"TS_PRESCALE_TRIG8"))  v1495regs[0x21C/4] = value & 0x3FF;
	  else if(!strcmp(keyword,"TS_PRESCALE_TRIG9"))  v1495regs[0x220/4] = value & 0x3FF;
	  else if(!strcmp(keyword,"TS_PRESCALE_TRIG10"))  v1495regs[0x224/4] = value & 0x3FF;
	  else if(!strcmp(keyword,"TS_PRESCALE_TRIG11"))  v1495regs[0x228/4] = value & 0x3FF;
	  else if(!strcmp(keyword,"TS_PRESCALE_TRIG12"))  v1495regs[0x22C/4] = value & 0x3FF;

	  else if(!strcmp(keyword,"TS_STOF_EN_SEC1")) v1495regs[0x300/4] |= (value & 0x1);
	  else if(!strcmp(keyword,"TS_STOF_EN_SEC2")) v1495regs[0x300/4] |= (value & 0x1)<<1;
	  else if(!strcmp(keyword,"TS_STOF_EN_SEC3")) v1495regs[0x300/4] |= (value & 0x1)<<2;
	  else if(!strcmp(keyword,"TS_STOF_EN_SEC4")) v1495regs[0x300/4] |= (value & 0x1)<<3;
	  else if(!strcmp(keyword,"TS_STOF_EN_SEC5")) v1495regs[0x300/4] |= (value & 0x1)<<4;
	  else if(!strcmp(keyword,"TS_STOF_EN_SEC6")) v1495regs[0x300/4] |= (value & 0x1)<<5;

	  else if(!strcmp(keyword,"TS_TOF_DIS_SEC1")) v1495regs[0x304/4] |= (value & 0x1);
	  else if(!strcmp(keyword,"TS_TOF_DIS_SEC2")) v1495regs[0x304/4] |= (value & 0x1)<<1;
	  else if(!strcmp(keyword,"TS_TOF_DIS_SEC3")) v1495regs[0x304/4] |= (value & 0x1)<<2;
	  else if(!strcmp(keyword,"TS_TOF_DIS_SEC4")) v1495regs[0x304/4] |= (value & 0x1)<<3;
	  else if(!strcmp(keyword,"TS_TOF_DIS_SEC5")) v1495regs[0x304/4] |= (value & 0x1)<<4;
	  else if(!strcmp(keyword,"TS_TOF_DIS_SEC6")) v1495regs[0x304/4] |= (value & 0x1)<<5;

	  else if(!strcmp(keyword,"TS_STS_DIS_SEC1")) v1495regs[0x308/4] |= (value & 0x1);
	  else if(!strcmp(keyword,"TS_STS_DIS_SEC2")) v1495regs[0x308/4] |= (value & 0x1)<<1;
	  else if(!strcmp(keyword,"TS_STS_DIS_SEC3")) v1495regs[0x308/4] |= (value & 0x1)<<2;
	  else if(!strcmp(keyword,"TS_STS_DIS_SEC4")) v1495regs[0x308/4] |= (value & 0x1)<<3;
	  else if(!strcmp(keyword,"TS_STS_DIS_SEC5")) v1495regs[0x308/4] |= (value & 0x1)<<4;
	  else if(!strcmp(keyword,"TS_STS_DIS_SEC6")) v1495regs[0x308/4] |= (value & 0x1)<<5;

	  else if(!strcmp(keyword,"TS_ECE_DIS_SEC1")) v1495regs[0x30C/4] |= (value & 0x1);
	  else if(!strcmp(keyword,"TS_ECE_DIS_SEC2")) v1495regs[0x30C/4] |= (value & 0x1)<<1;
	  else if(!strcmp(keyword,"TS_ECE_DIS_SEC3")) v1495regs[0x30C/4] |= (value & 0x1)<<2;
	  else if(!strcmp(keyword,"TS_ECE_DIS_SEC4")) v1495regs[0x30C/4] |= (value & 0x1)<<3;
	  else if(!strcmp(keyword,"TS_ECE_DIS_SEC5")) v1495regs[0x30C/4] |= (value & 0x1)<<4;
	  else if(!strcmp(keyword,"TS_ECE_DIS_SEC6")) v1495regs[0x30C/4] |= (value & 0x1)<<5;

	  else if(!strcmp(keyword,"TS_CC_DIS_SEC1")) v1495regs[0x310/4] |= (value & 0x1);
	  else if(!strcmp(keyword,"TS_CC_DIS_SEC2")) v1495regs[0x310/4] |= (value & 0x1)<<1;
	  else if(!strcmp(keyword,"TS_CC_DIS_SEC3")) v1495regs[0x310/4] |= (value & 0x1)<<2;
	  else if(!strcmp(keyword,"TS_CC_DIS_SEC4")) v1495regs[0x310/4] |= (value & 0x1)<<3;
	  else if(!strcmp(keyword,"TS_CC_DIS_SEC5")) v1495regs[0x310/4] |= (value & 0x1)<<4;
	  else if(!strcmp(keyword,"TS_CC_DIS_SEC6")) v1495regs[0x310/4] |= (value & 0x1)<<5;

	  else if(!strcmp(keyword,"TS_ECC_EN_SEC1")) v1495regs[0x314/4] |= (value & 0x1);
	  else if(!strcmp(keyword,"TS_ECC_EN_SEC2")) v1495regs[0x314/4] |= (value & 0x1)<<1;
	  else if(!strcmp(keyword,"TS_ECC_EN_SEC3")) v1495regs[0x314/4] |= (value & 0x1)<<2;
	  else if(!strcmp(keyword,"TS_ECC_EN_SEC4")) v1495regs[0x314/4] |= (value & 0x1)<<3;
	  else if(!strcmp(keyword,"TS_ECC_EN_SEC5")) v1495regs[0x314/4] |= (value & 0x1)<<4;
	  else if(!strcmp(keyword,"TS_ECC_EN_SEC6")) v1495regs[0x314/4] |= (value & 0x1)<<5;

	  else if(!strcmp(keyword,"TS_MORA_EN_TRIG1")) v1495regs[0x408/4] |= (value & 0x1);
	  else if(!strcmp(keyword,"TS_MORA_EN_TRIG2")) v1495regs[0x408/4] |= (value & 0x1)<<1;
	  else if(!strcmp(keyword,"TS_MORA_EN_TRIG3")) v1495regs[0x408/4] |= (value & 0x1)<<2;
	  else if(!strcmp(keyword,"TS_MORA_EN_TRIG4")) v1495regs[0x408/4] |= (value & 0x1)<<3;
	  else if(!strcmp(keyword,"TS_MORA_EN_TRIG5")) v1495regs[0x408/4] |= (value & 0x1)<<4;
	  else if(!strcmp(keyword,"TS_MORA_EN_TRIG6")) v1495regs[0x408/4] |= (value & 0x1)<<5;
	  else if(!strcmp(keyword,"TS_MORA_EN_TRIG7")) v1495regs[0x408/4] |= (value & 0x1)<<6;
	  else if(!strcmp(keyword,"TS_MORA_EN_TRIG8")) v1495regs[0x408/4] |= (value & 0x1)<<7;
	  else if(!strcmp(keyword,"TS_MORA_EN_TRIG9")) v1495regs[0x408/4] |= (value & 0x1)<<8;
	  else if(!strcmp(keyword,"TS_MORA_EN_TRIG10")) v1495regs[0x408/4] |= (value & 0x1)<<9;
	  else if(!strcmp(keyword,"TS_MORA_EN_TRIG11")) v1495regs[0x408/4] |= (value & 0x1)<<10;
	  else if(!strcmp(keyword,"TS_MORA_EN_TRIG12")) v1495regs[0x408/4] |= (value & 0x1)<<11;

	  else if(!strcmp(keyword,"TS_MORB_EN_TRIG1")) v1495regs[0x40C/4] |= (value & 0x1);
	  else if(!strcmp(keyword,"TS_MORB_EN_TRIG2")) v1495regs[0x40C/4] |= (value & 0x1)<<1;
	  else if(!strcmp(keyword,"TS_MORB_EN_TRIG3")) v1495regs[0x40C/4] |= (value & 0x1)<<2;
	  else if(!strcmp(keyword,"TS_MORB_EN_TRIG4")) v1495regs[0x40C/4] |= (value & 0x1)<<3;
	  else if(!strcmp(keyword,"TS_MORB_EN_TRIG5")) v1495regs[0x40C/4] |= (value & 0x1)<<4;
	  else if(!strcmp(keyword,"TS_MORB_EN_TRIG6")) v1495regs[0x40C/4] |= (value & 0x1)<<5;
	  else if(!strcmp(keyword,"TS_MORB_EN_TRIG7")) v1495regs[0x40C/4] |= (value & 0x1)<<6;
	  else if(!strcmp(keyword,"TS_MORB_EN_TRIG8")) v1495regs[0x40C/4] |= (value & 0x1)<<7;
	  else if(!strcmp(keyword,"TS_MORB_EN_TRIG9")) v1495regs[0x40C/4] |= (value & 0x1)<<8;
	  else if(!strcmp(keyword,"TS_MORB_EN_TRIG10")) v1495regs[0x40C/4] |= (value & 0x1)<<9;
	  else if(!strcmp(keyword,"TS_MORB_EN_TRIG11")) v1495regs[0x40C/4] |= (value & 0x1)<<10;
	  else if(!strcmp(keyword,"TS_MORB_EN_TRIG12")) v1495regs[0x40C/4] |= (value & 0x1)<<11;

	  else if(!strcmp(keyword,"TS_MOR_DIS_TRIG1")) v1495regs[0x410/4] |= (value & 0x1);
	  else if(!strcmp(keyword,"TS_MOR_DIS_TRIG2")) v1495regs[0x410/4] |= (value & 0x1)<<1;
	  else if(!strcmp(keyword,"TS_MOR_DIS_TRIG3")) v1495regs[0x410/4] |= (value & 0x1)<<2;
	  else if(!strcmp(keyword,"TS_MOR_DIS_TRIG4")) v1495regs[0x410/4] |= (value & 0x1)<<3;
	  else if(!strcmp(keyword,"TS_MOR_DIS_TRIG5")) v1495regs[0x410/4] |= (value & 0x1)<<4;
	  else if(!strcmp(keyword,"TS_MOR_DIS_TRIG6")) v1495regs[0x410/4] |= (value & 0x1)<<5;
	  else if(!strcmp(keyword,"TS_MOR_DIS_TRIG7")) v1495regs[0x410/4] |= (value & 0x1)<<6;
	  else if(!strcmp(keyword,"TS_MOR_DIS_TRIG8")) v1495regs[0x410/4] |= (value & 0x1)<<7;
	  else if(!strcmp(keyword,"TS_MOR_DIS_TRIG9")) v1495regs[0x410/4] |= (value & 0x1)<<8;
	  else if(!strcmp(keyword,"TS_MOR_DIS_TRIG10")) v1495regs[0x410/4] |= (value & 0x1)<<9;
	  else if(!strcmp(keyword,"TS_MOR_DIS_TRIG11")) v1495regs[0x410/4] |= (value & 0x1)<<10;
	  else if(!strcmp(keyword,"TS_MOR_DIS_TRIG12")) v1495regs[0x410/4] |= (value & 0x1)<<11;

      /*TS_STMULT_THRESHOLD*/
	  else if(!strcmp(keyword,"TS_ST_MULT_MAX")) v1495regs[0x414/4] |= (value & 0x1F)<<5;
	  else if(!strcmp(keyword,"TS_ST_MULT_MIN")) v1495regs[0x414/4] |= value & 0x1F;

	  else if(!strcmp(keyword,"TS_STMULT_DIS_TRIG1")) v1495regs[0x418/4] |= (value & 0x1);
	  else if(!strcmp(keyword,"TS_STMULT_DIS_TRIG2")) v1495regs[0x418/4] |= (value & 0x1)<<1;
	  else if(!strcmp(keyword,"TS_STMULT_DIS_TRIG3")) v1495regs[0x418/4] |= (value & 0x1)<<2;
	  else if(!strcmp(keyword,"TS_STMULT_DIS_TRIG4")) v1495regs[0x418/4] |= (value & 0x1)<<3;
	  else if(!strcmp(keyword,"TS_STMULT_DIS_TRIG5")) v1495regs[0x418/4] |= (value & 0x1)<<4;
	  else if(!strcmp(keyword,"TS_STMULT_DIS_TRIG6")) v1495regs[0x418/4] |= (value & 0x1)<<5;
	  else if(!strcmp(keyword,"TS_STMULT_DIS_TRIG7")) v1495regs[0x418/4] |= (value & 0x1)<<6;
	  else if(!strcmp(keyword,"TS_STMULT_DIS_TRIG8")) v1495regs[0x418/4] |= (value & 0x1)<<7;
	  else if(!strcmp(keyword,"TS_STMULT_DIS_TRIG9")) v1495regs[0x418/4] |= (value & 0x1)<<8;
	  else if(!strcmp(keyword,"TS_STMULT_DIS_TRIG10")) v1495regs[0x418/4] |= (value & 0x1)<<9;
	  else if(!strcmp(keyword,"TS_STMULT_DIS_TRIG11")) v1495regs[0x418/4] |= (value & 0x1)<<10;
	  else if(!strcmp(keyword,"TS_STMULT_DIS_TRIG12")) v1495regs[0x418/4] |= (value & 0x1)<<11;

	  else if(!strcmp(keyword,"TS_L2_SECLOGIC"))     v1495regs[0x500/4] = value & 0xFF;
	  else if(!strcmp(keyword,"TS_L2_OUTPUTDELAY"))  v1495regs[0x504/4] = value & 0x3FF;
	  else if(!strcmp(keyword,"TS_L2_OUTPUTWIDTH"))  v1495regs[0x508/4] = value & 0xFF;

      else if(!strcmp(keyword,"TS_TRIG1_ECCLUTFILE"))  ret=v1495LUTDownload(addr,lutfilename,1,0);
      else if(!strcmp(keyword,"TS_TRIG1_ECPLUTFILE"))  ret=v1495LUTDownload(addr,lutfilename,1,1);

      else if(!strcmp(keyword,"TS_TRIG2_ECCLUTFILE"))  ret=v1495LUTDownload(addr,lutfilename,2,0);
      else if(!strcmp(keyword,"TS_TRIG2_ECPLUTFILE"))  ret=v1495LUTDownload(addr,lutfilename,2,1);

      else if(!strcmp(keyword,"TS_TRIG3_ECCLUTFILE"))  ret=v1495LUTDownload(addr,lutfilename,3,0);
      else if(!strcmp(keyword,"TS_TRIG3_ECPLUTFILE"))  ret=v1495LUTDownload(addr,lutfilename,3,1);

      else if(!strcmp(keyword,"TS_TRIG4_ECCLUTFILE"))  ret=v1495LUTDownload(addr,lutfilename,4,0);
      else if(!strcmp(keyword,"TS_TRIG4_ECPLUTFILE"))  ret=v1495LUTDownload(addr,lutfilename,4,1);

      else if(!strcmp(keyword,"TS_TRIG5_ECCLUTFILE"))  ret=v1495LUTDownload(addr,lutfilename,5,0);
      else if(!strcmp(keyword,"TS_TRIG5_ECPLUTFILE"))  ret=v1495LUTDownload(addr,lutfilename,5,1);

      else if(!strcmp(keyword,"TS_TRIG6_ECCLUTFILE"))  ret=v1495LUTDownload(addr,lutfilename,6,0);
      else if(!strcmp(keyword,"TS_TRIG6_ECPLUTFILE"))  ret=v1495LUTDownload(addr,lutfilename,6,1);

      else if(!strcmp(keyword,"TS_TRIG7_ECCLUTFILE"))  ret=v1495LUTDownload(addr,lutfilename,7,0);
      else if(!strcmp(keyword,"TS_TRIG7_ECPLUTFILE"))  ret=v1495LUTDownload(addr,lutfilename,7,1);

      else if(!strcmp(keyword,"TS_TRIG8_ECCLUTFILE"))  ret=v1495LUTDownload(addr,lutfilename,8,0);
      else if(!strcmp(keyword,"TS_TRIG8_ECPLUTFILE"))  ret=v1495LUTDownload(addr,lutfilename,8,1);

      else if(!strcmp(keyword,"TS_TRIG9_ECCLUTFILE"))  ret=v1495LUTDownload(addr,lutfilename,9,0);
      else if(!strcmp(keyword,"TS_TRIG9_ECPLUTFILE"))  ret=v1495LUTDownload(addr,lutfilename,9,1);

      else if(!strcmp(keyword,"TS_TRIG10_ECCLUTFILE"))  ret=v1495LUTDownload(addr,lutfilename,10,0);
      else if(!strcmp(keyword,"TS_TRIG10_ECPLUTFILE"))  ret=v1495LUTDownload(addr,lutfilename,10,1);

      else if(!strcmp(keyword,"TS_TRIG11_ECCLUTFILE"))  ret=v1495LUTDownload(addr,lutfilename,11,0);
      else if(!strcmp(keyword,"TS_TRIG11_ECPLUTFILE"))  ret=v1495LUTDownload(addr,lutfilename,11,1);

      else if(!strcmp(keyword,"TS_TRIG12_ECCLUTFILE"))  ret=v1495LUTDownload(addr,lutfilename,12,0);
      else if(!strcmp(keyword,"TS_TRIG12_ECPLUTFILE"))  ret=v1495LUTDownload(addr,lutfilename,12,1);


      else if(!strcmp(keyword,"TS_CFG_LUT_L2FILE"))     ret=v1495LUTDownload(addr,lutfilename,13,1);

      else
	  {
        printf("v1495ConfigDownload ERROR: unknown keyword >%s< (%d 0x%02x)\n",keyword,ch[0],ch[0]);
        return(-3);
	  }

      if(ret!=0) return(-4);
	}
  }

  fclose(fd);


  /* ST MAX MULTIPLICITY cannot be more then 24 */
  if( ((v1495regs[0x414/4]>>5) & 0x1F) > 24) {regs[0x414/4] &= 0x1F; v1495regs[0x414/4] |= 24<<5;}


  /* some bits are inverted, so '0' in file must be written to the memory as '1'; taking care of that 
  v1495regs[0x3A/4] = v1495regs[0x3A/4] ^ 0x3FF;
  v1495regs[0x3C/4] = v1495regs[0x3C/4] ^ 0x3FF;
  */


  /*for(i=0; i<31; i++) printf("[%2d] 0x%04x\n",i,v1495regs[i]);*/

  semTake(v1495scalers_lock, WAIT_FOREVER);

  /* download */
  nregs = (TS_L2_OUTPUTWIDTH - TS_MORA_DELAY + 4) / 4;
  ptr32 = (volatile unsigned int *)(v1495base+TS_MORA_DELAY);
  mem32 = (unsigned int *) &v1495regs[0];

  /* NEED REVERSE READOUT HERE */
  /*for(i=0; i<nregs; i++)*/
  for(i=(nregs-1); i>=0; i--)
  {
    ptr32[i] = mem32[i];
  }

  /*upload and compare*/
  ptr32 = (volatile unsigned int *)(v1495base+TS_MORA_DELAY);
  mem32 = (unsigned int *) &regs[0];

  /* NEED REVERSE READOUT HERE */
  /*for(i=0; i<nregs; i++)*/
  for(i=(nregs-1); i>=0; i--)
  {
    mem32[i] = ptr32[i];
  }

  semGive(v1495scalers_lock);

  /*printf("nregs = %d\n",nregs);*/
  for(i=0; i<nregs; i++)
  {
    if(v1495regs[i] != regs[i])
	{
      printf("v1495ConfigDownload ERROR: [%2d][0x%02x] load 0x%08x, read back 0x%08x\n",
        i,i,v1495regs[i],regs[i]);
      return(-4);
	}
    else
	{
      /*printf("v1495ConfigDownload: [%2d][0x%02x] passed: 0x%08x = 0x%08x\n",i,i,v1495regs[i],regs[i])*/;
	}
  }

  return(0);
}

/*
v1495ConfigUpload("/home/boiarino/zzz.dat")
*/

/* reads hardware setting and write them to the file */
int
v1495ConfigUpload(unsigned int addr, char *fname)
{
  FILE *fd;
  int i, nregs;
  volatile unsigned int *ptr32;
  unsigned int *mem32;
  unsigned int v1495base;

  if(sysBusToLocalAdrs(0x39,addr&0xFFFFFF,&v1495base) < 0)
  {
    v1495base = 0;
    printf("v1495ConfigUpload ERROR: undefined base address\n");
    return(-1);
  }

  if(v1495base == 0)
  {
    printf("v1495ConfigUpload ERROR: undefined base address\n");
    return(-1);
  }

  if( (fd = fopen(fname,"w")) == NULL)
  {
    printf("v1495ConfigUpload: Can't create output file >%s<\n",fname);
    return(-2);
  }

  nregs = (TS_L2_OUTPUTWIDTH - TS_MORA_DELAY + 4) / 4;
  printf("nregs=%d\n",nregs);
  for(i=0; i<nregs; i++) regs[i] = 0;

  semTake(v1495scalers_lock, WAIT_FOREVER);

  ptr32 = (volatile unsigned int *)(v1495base + TS_MORA_DELAY);
  mem32 = (unsigned int *) &regs[0];

  /* NEED REVERSE READOUT HERE */
  /*for(i=0; i<nregs; i++)*/
  for(i=(nregs-1); i>=0; i--)
  {
    mem32[i] = ptr32[i];
  }

  semGive(v1495scalers_lock);



  /* some bits are inverted, so '0' in file must be written to the memory as '1'; taking care of that
  regs[0x3A/4] = regs[0x3A/4] ^ 0x3FF;
  regs[0x3C/4] = regs[0x3C/4] ^ 0x3FF;
  */

  /* ST MAX MULTIPLICITY cannot be more then 24 */
  if( ((regs[0x414/4]>>5) & 0x1F) > 24) {regs[0x414/4] &= 0x1F; regs[0x414/4] |= 24<<5;}


  fprintf(fd,"# Hall B Trigger Module Configuration Register Settings\n");
  /*fprintf(fd,"# Uploaded from hardware: Wednesday, March 26, 2008 at 11:13:55AM\n\n");*/
  fprintf(fd,"# Uploaded from hardware: %s\n",ctime(NULL));


  fprintf(fd,"TS_MORA_DELAY             %6d\n", regs[0x00/4] & 0x1F);
  fprintf(fd,"TS_MORB_DELAY             %6d\n", regs[0x04/4] & 0x1F);
  fprintf(fd,"TS_TOF1_DELAY             %6d\n", regs[0x08/4] & 0x1F);
  fprintf(fd,"TS_TOF2_DELAY             %6d\n", regs[0x0C/4] & 0x1F);
  fprintf(fd,"TS_TOF3_DELAY             %6d\n", regs[0x10/4] & 0x1F);
  fprintf(fd,"TS_TOF4_DELAY             %6d\n", regs[0x14/4] & 0x1F);
  fprintf(fd,"TS_TOF5_DELAY             %6d\n", regs[0x18/4] & 0x1F);
  fprintf(fd,"TS_TOF6_DELAY             %6d\n", regs[0x1C/4] & 0x1F);

  fprintf(fd,"TS_ECin1P_DELAY           %6d\n", regs[0x20/4] & 0x1F);
  fprintf(fd,"TS_ECin2P_DELAY           %6d\n", regs[0x24/4] & 0x1F);
  fprintf(fd,"TS_ECin3P_DELAY           %6d\n", regs[0x28/4] & 0x1F);
  fprintf(fd,"TS_ECin4P_DELAY           %6d\n", regs[0x2C/4] & 0x1F);
  fprintf(fd,"TS_ECin5P_DELAY           %6d\n", regs[0x30/4] & 0x1F);
  fprintf(fd,"TS_ECin6P_DELAY           %6d\n", regs[0x34/4] & 0x1F);

  fprintf(fd,"TS_ECin1E_DELAY           %6d\n", regs[0x38/4] & 0x1F);
  fprintf(fd,"TS_ECin2E_DELAY           %6d\n", regs[0x3C/4] & 0x1F);
  fprintf(fd,"TS_ECin3E_DELAY           %6d\n", regs[0x40/4] & 0x1F);
  fprintf(fd,"TS_ECin4E_DELAY           %6d\n", regs[0x44/4] & 0x1F);
  fprintf(fd,"TS_ECin5E_DELAY           %6d\n", regs[0x48/4] & 0x1F);
  fprintf(fd,"TS_ECin6E_DELAY           %6d\n", regs[0x4C/4] & 0x1F);
  fprintf(fd,"TS_ECtot1P_DELAY          %6d\n", regs[0x50/4] & 0x1F);
  fprintf(fd,"TS_ECtot2P_DELAY          %6d\n", regs[0x54/4] & 0x1F);
  fprintf(fd,"TS_ECtot3P_DELAY          %6d\n", regs[0x58/4] & 0x1F);
  fprintf(fd,"TS_ECtot4P_DELAY          %6d\n", regs[0x5C/4] & 0x1F);
  fprintf(fd,"TS_ECtot5P_DELAY          %6d\n", regs[0x60/4] & 0x1F);
  fprintf(fd,"TS_ECtot6P_DELAY          %6d\n", regs[0x64/4] & 0x1F);
  fprintf(fd,"TS_ECtot1E_DELAY          %6d\n", regs[0x68/4] & 0x1F);
  fprintf(fd,"TS_ECtot2E_DELAY          %6d\n", regs[0x6C/4] & 0x1F);
  fprintf(fd,"TS_ECtot3E_DELAY          %6d\n", regs[0x70/4] & 0x1F);
  fprintf(fd,"TS_ECtot4E_DELAY          %6d\n", regs[0x74/4] & 0x1F);
  fprintf(fd,"TS_ECtot5E_DELAY          %6d\n", regs[0x78/4] & 0x1F);
  fprintf(fd,"TS_ECtot6E_DELAY          %6d\n", regs[0x7C/4] & 0x1F);

  fprintf(fd,"TS_CC1_DELAY              %6d\n", regs[0x80/4] & 0x1F);
  fprintf(fd,"TS_CC2_DELAY              %6d\n", regs[0x84/4] & 0x1F);
  fprintf(fd,"TS_CC3_DELAY              %6d\n", regs[0x88/4] & 0x1F);
  fprintf(fd,"TS_CC4_DELAY              %6d\n", regs[0x8C/4] & 0x1F);
  fprintf(fd,"TS_CC5_DELAY              %6d\n", regs[0x90/4] & 0x1F);
  fprintf(fd,"TS_CC6_DELAY              %6d\n", regs[0x94/4] & 0x1F);

  fprintf(fd,"TS_ST1_DELAY              %6d\n", regs[0x98/4] & 0x1F);
  fprintf(fd,"TS_ST2_DELAY              %6d\n", regs[0x9C/4] & 0x1F);
  fprintf(fd,"TS_ST3_DELAY              %6d\n", regs[0xA0/4] & 0x1F);
  fprintf(fd,"TS_ST4_DELAY              %6d\n", regs[0xA4/4] & 0x1F);
  fprintf(fd,"TS_ST5_DELAY              %6d\n", regs[0xA8/4] & 0x1F);
  fprintf(fd,"TS_ST6_DELAY              %6d\n", regs[0xAC/4] & 0x1F);
  fprintf(fd,"TS_ST7_DELAY              %6d\n", regs[0xB0/4] & 0x1F);
  fprintf(fd,"TS_ST8_DELAY              %6d\n", regs[0xB4/4] & 0x1F);
  fprintf(fd,"TS_ST9_DELAY              %6d\n", regs[0xB8/4] & 0x1F);
  fprintf(fd,"TS_ST10_DELAY             %6d\n", regs[0xBC/4] & 0x1F);
  fprintf(fd,"TS_ST11_DELAY             %6d\n", regs[0xC0/4] & 0x1F);
  fprintf(fd,"TS_ST12_DELAY             %6d\n", regs[0xC4/4] & 0x1F);
  fprintf(fd,"TS_ST13_DELAY             %6d\n", regs[0xC8/4] & 0x1F);
  fprintf(fd,"TS_ST14_DELAY             %6d\n", regs[0xCC/4] & 0x1F);
  fprintf(fd,"TS_ST15_DELAY             %6d\n", regs[0xD0/4] & 0x1F);
  fprintf(fd,"TS_ST16_DELAY             %6d\n", regs[0xD4/4] & 0x1F);
  fprintf(fd,"TS_ST17_DELAY             %6d\n", regs[0xD8/4] & 0x1F);
  fprintf(fd,"TS_ST18_DELAY             %6d\n", regs[0xDC/4] & 0x1F);
  fprintf(fd,"TS_ST19_DELAY             %6d\n", regs[0xE0/4] & 0x1F);
  fprintf(fd,"TS_ST20_DELAY             %6d\n", regs[0xE4/4] & 0x1F);
  fprintf(fd,"TS_ST21_DELAY             %6d\n", regs[0xE8/4] & 0x1F);
  fprintf(fd,"TS_ST22_DELAY             %6d\n", regs[0xEC/4] & 0x1F);
  fprintf(fd,"TS_ST23_DELAY             %6d\n", regs[0xF0/4] & 0x1F);
  fprintf(fd,"TS_ST24_DELAY             %6d\n", regs[0xF4/4] & 0x1F);

  fprintf(fd,"TS_PERSIST_TRIG1          %6d\n", regs[0x100/4] & 0x7);
  fprintf(fd,"TS_PERSIST_TRIG2          %6d\n", regs[0x104/4] & 0x7);
  fprintf(fd,"TS_PERSIST_TRIG3          %6d\n", regs[0x108/4] & 0x7);
  fprintf(fd,"TS_PERSIST_TRIG4          %6d\n", regs[0x10C/4] & 0x7);
  fprintf(fd,"TS_PERSIST_TRIG5          %6d\n", regs[0x110/4] & 0x7);
  fprintf(fd,"TS_PERSIST_TRIG6          %6d\n", regs[0x114/4] & 0x7);
  fprintf(fd,"TS_PERSIST_TRIG7          %6d\n", regs[0x118/4] & 0x7);
  fprintf(fd,"TS_PERSIST_TRIG8          %6d\n", regs[0x11C/4] & 0x7);
  fprintf(fd,"TS_PERSIST_TRIG9          %6d\n", regs[0x120/4] & 0x7);
  fprintf(fd,"TS_PERSIST_TRIG10         %6d\n", regs[0x124/4] & 0x7);
  fprintf(fd,"TS_PERSIST_TRIG11         %6d\n", regs[0x128/4] & 0x7);
  fprintf(fd,"TS_PERSIST_TRIG12         %6d\n", regs[0x12C/4] & 0x7);

  fprintf(fd,"TS_PRESCALE_TRIG1         %6d\n", regs[0x200/4] & 0x3FF);
  fprintf(fd,"TS_PRESCALE_TRIG2         %6d\n", regs[0x204/4] & 0x3FF);
  fprintf(fd,"TS_PRESCALE_TRIG3         %6d\n", regs[0x208/4] & 0x3FF);
  fprintf(fd,"TS_PRESCALE_TRIG4         %6d\n", regs[0x20C/4] & 0x3FF);
  fprintf(fd,"TS_PRESCALE_TRIG5         %6d\n", regs[0x210/4] & 0x3FF);
  fprintf(fd,"TS_PRESCALE_TRIG6         %6d\n", regs[0x214/4] & 0x3FF);
  fprintf(fd,"TS_PRESCALE_TRIG7         %6d\n", regs[0x218/4] & 0x3FF);
  fprintf(fd,"TS_PRESCALE_TRIG8         %6d\n", regs[0x21C/4] & 0x3FF);
  fprintf(fd,"TS_PRESCALE_TRIG9         %6d\n", regs[0x220/4] & 0x3FF);
  fprintf(fd,"TS_PRESCALE_TRIG10        %6d\n", regs[0x224/4] & 0x3FF);
  fprintf(fd,"TS_PRESCALE_TRIG11        %6d\n", regs[0x228/4] & 0x3FF);
  fprintf(fd,"TS_PRESCALE_TRIG12        %6d\n", regs[0x22C/4] & 0x3FF);

  fprintf(fd,"TS_STOF_EN_SEC1           %6d\n", regs[0x300/4] & 0x1);
  fprintf(fd,"TS_STOF_EN_SEC2           %6d\n", (regs[0x300/4]>>1) & 0x1);
  fprintf(fd,"TS_STOF_EN_SEC3           %6d\n", (regs[0x300/4]>>2) & 0x1);
  fprintf(fd,"TS_STOF_EN_SEC4           %6d\n", (regs[0x300/4]>>3) & 0x1);
  fprintf(fd,"TS_STOF_EN_SEC5           %6d\n", (regs[0x300/4]>>4) & 0x1);
  fprintf(fd,"TS_STOF_EN_SEC6           %6d\n", (regs[0x300/4]>>5) & 0x1);

  fprintf(fd,"TS_TOF_DIS_SEC1           %6d\n", regs[0x304/4] & 0x1);
  fprintf(fd,"TS_TOF_DIS_SEC2           %6d\n", (regs[0x304/4]>>1) & 0x1);
  fprintf(fd,"TS_TOF_DIS_SEC3           %6d\n", (regs[0x304/4]>>2) & 0x1);
  fprintf(fd,"TS_TOF_DIS_SEC4           %6d\n", (regs[0x304/4]>>3) & 0x1);
  fprintf(fd,"TS_TOF_DIS_SEC5           %6d\n", (regs[0x304/4]>>4) & 0x1);
  fprintf(fd,"TS_TOF_DIS_SEC6           %6d\n", (regs[0x304/4]>>5) & 0x1);

  fprintf(fd,"TS_STS_DIS_SEC1           %6d\n", regs[0x308/4] & 0x1);
  fprintf(fd,"TS_STS_DIS_SEC2           %6d\n", (regs[0x308/4]>>1) & 0x1);
  fprintf(fd,"TS_STS_DIS_SEC3           %6d\n", (regs[0x308/4]>>2) & 0x1);
  fprintf(fd,"TS_STS_DIS_SEC4           %6d\n", (regs[0x308/4]>>3) & 0x1);
  fprintf(fd,"TS_STS_DIS_SEC5           %6d\n", (regs[0x308/4]>>4) & 0x1);
  fprintf(fd,"TS_STS_DIS_SEC6           %6d\n", (regs[0x308/4]>>5) & 0x1);

  fprintf(fd,"TS_ECE_DIS_SEC1           %6d\n", regs[0x30C/4] & 0x1);
  fprintf(fd,"TS_ECE_DIS_SEC2           %6d\n", (regs[0x30C/4]>>1) & 0x1);
  fprintf(fd,"TS_ECE_DIS_SEC3           %6d\n", (regs[0x30C/4]>>2) & 0x1);
  fprintf(fd,"TS_ECE_DIS_SEC4           %6d\n", (regs[0x30C/4]>>3) & 0x1);
  fprintf(fd,"TS_ECE_DIS_SEC5           %6d\n", (regs[0x30C/4]>>4) & 0x1);
  fprintf(fd,"TS_ECE_DIS_SEC6           %6d\n", (regs[0x30C/4]>>5) & 0x1);

  fprintf(fd,"TS_CC_DIS_SEC1            %6d\n", regs[0x310/4] & 0x1);
  fprintf(fd,"TS_CC_DIS_SEC2            %6d\n", (regs[0x310/4]>>1) & 0x1);
  fprintf(fd,"TS_CC_DIS_SEC3            %6d\n", (regs[0x310/4]>>2) & 0x1);
  fprintf(fd,"TS_CC_DIS_SEC4            %6d\n", (regs[0x310/4]>>3) & 0x1);
  fprintf(fd,"TS_CC_DIS_SEC5            %6d\n", (regs[0x310/4]>>4) & 0x1);
  fprintf(fd,"TS_CC_DIS_SEC6            %6d\n", (regs[0x310/4]>>5) & 0x1);

  fprintf(fd,"TS_ECC_EN_SEC1            %6d\n", regs[0x314/4] & 0x1);
  fprintf(fd,"TS_ECC_EN_SEC2            %6d\n", (regs[0x314/4]>>1) & 0x1);
  fprintf(fd,"TS_ECC_EN_SEC3            %6d\n", (regs[0x314/4]>>2) & 0x1);
  fprintf(fd,"TS_ECC_EN_SEC4            %6d\n", (regs[0x314/4]>>3) & 0x1);
  fprintf(fd,"TS_ECC_EN_SEC5            %6d\n", (regs[0x314/4]>>4) & 0x1);
  fprintf(fd,"TS_ECC_EN_SEC6            %6d\n", (regs[0x314/4]>>5) & 0x1);

  fprintf(fd,"TS_MORA_EN_TRIG1          %6d\n", regs[0x408/4] & 0x1);
  fprintf(fd,"TS_MORA_EN_TRIG2          %6d\n", (regs[0x408/4]>>1) & 0x1);
  fprintf(fd,"TS_MORA_EN_TRIG3          %6d\n", (regs[0x408/4]>>2) & 0x1);
  fprintf(fd,"TS_MORA_EN_TRIG4          %6d\n", (regs[0x408/4]>>3) & 0x1);
  fprintf(fd,"TS_MORA_EN_TRIG5          %6d\n", (regs[0x408/4]>>4) & 0x1);
  fprintf(fd,"TS_MORA_EN_TRIG6          %6d\n", (regs[0x408/4]>>5) & 0x1);
  fprintf(fd,"TS_MORA_EN_TRIG7          %6d\n", (regs[0x408/4]>>6) & 0x1);
  fprintf(fd,"TS_MORA_EN_TRIG8          %6d\n", (regs[0x408/4]>>7) & 0x1);
  fprintf(fd,"TS_MORA_EN_TRIG9          %6d\n", (regs[0x408/4]>>8) & 0x1);
  fprintf(fd,"TS_MORA_EN_TRIG10         %6d\n", (regs[0x408/4]>>9) & 0x1);
  fprintf(fd,"TS_MORA_EN_TRIG11         %6d\n", (regs[0x408/4]>>10) & 0x1);
  fprintf(fd,"TS_MORA_EN_TRIG12         %6d\n", (regs[0x408/4]>>11) & 0x1);

  fprintf(fd,"TS_MORB_EN_TRIG1          %6d\n", regs[0x40C/4] & 0x1);
  fprintf(fd,"TS_MORB_EN_TRIG2          %6d\n", (regs[0x40C/4]>>1) & 0x1);
  fprintf(fd,"TS_MORB_EN_TRIG3          %6d\n", (regs[0x40C/4]>>2) & 0x1);
  fprintf(fd,"TS_MORB_EN_TRIG4          %6d\n", (regs[0x40C/4]>>3) & 0x1);
  fprintf(fd,"TS_MORB_EN_TRIG5          %6d\n", (regs[0x40C/4]>>4) & 0x1);
  fprintf(fd,"TS_MORB_EN_TRIG6          %6d\n", (regs[0x40C/4]>>5) & 0x1);
  fprintf(fd,"TS_MORB_EN_TRIG7          %6d\n", (regs[0x40C/4]>>6) & 0x1);
  fprintf(fd,"TS_MORB_EN_TRIG8          %6d\n", (regs[0x40C/4]>>7) & 0x1);
  fprintf(fd,"TS_MORB_EN_TRIG9          %6d\n", (regs[0x40C/4]>>8) & 0x1);
  fprintf(fd,"TS_MORB_EN_TRIG10         %6d\n", (regs[0x40C/4]>>9) & 0x1);
  fprintf(fd,"TS_MORB_EN_TRIG11         %6d\n", (regs[0x40C/4]>>10) & 0x1);
  fprintf(fd,"TS_MORB_EN_TRIG12         %6d\n", (regs[0x40C/4]>>11) & 0x1);

  fprintf(fd,"TS_MOR_DIS_TRIG1          %6d\n", regs[0x410/4] & 0x1);
  fprintf(fd,"TS_MOR_DIS_TRIG2          %6d\n", (regs[0x410/4]>>1) & 0x1);
  fprintf(fd,"TS_MOR_DIS_TRIG3          %6d\n", (regs[0x410/4]>>2) & 0x1);
  fprintf(fd,"TS_MOR_DIS_TRIG4          %6d\n", (regs[0x410/4]>>3) & 0x1);
  fprintf(fd,"TS_MOR_DIS_TRIG5          %6d\n", (regs[0x410/4]>>4) & 0x1);
  fprintf(fd,"TS_MOR_DIS_TRIG6          %6d\n", (regs[0x410/4]>>5) & 0x1);
  fprintf(fd,"TS_MOR_DIS_TRIG7          %6d\n", (regs[0x410/4]>>6) & 0x1);
  fprintf(fd,"TS_MOR_DIS_TRIG8          %6d\n", (regs[0x410/4]>>7) & 0x1);
  fprintf(fd,"TS_MOR_DIS_TRIG9          %6d\n", (regs[0x410/4]>>8) & 0x1);
  fprintf(fd,"TS_MOR_DIS_TRIG10         %6d\n", (regs[0x410/4]>>9) & 0x1);
  fprintf(fd,"TS_MOR_DIS_TRIG11         %6d\n", (regs[0x410/4]>>10) & 0x1);
  fprintf(fd,"TS_MOR_DIS_TRIG12         %6d\n", (regs[0x410/4]>>11) & 0x1);

  fprintf(fd,"TS_ST_MULT_MAX            %6d\n", (regs[0x414/4]>>5) & 0x1F);
  fprintf(fd,"TS_ST_MULT_MIN            %6d\n", regs[0x414/4] & 0x1F);

  fprintf(fd,"TS_STMULT_DIS_TRIG1       %6d\n", regs[0x418/4] & 0x1);
  fprintf(fd,"TS_STMULT_DIS_TRIG2       %6d\n", (regs[0x418/4]>>1) & 0x1);
  fprintf(fd,"TS_STMULT_DIS_TRIG3       %6d\n", (regs[0x418/4]>>2) & 0x1);
  fprintf(fd,"TS_STMULT_DIS_TRIG4       %6d\n", (regs[0x418/4]>>3) & 0x1);
  fprintf(fd,"TS_STMULT_DIS_TRIG5       %6d\n", (regs[0x418/4]>>4) & 0x1);
  fprintf(fd,"TS_STMULT_DIS_TRIG6       %6d\n", (regs[0x418/4]>>5) & 0x1);
  fprintf(fd,"TS_STMULT_DIS_TRIG7       %6d\n", (regs[0x418/4]>>6) & 0x1);
  fprintf(fd,"TS_STMULT_DIS_TRIG8       %6d\n", (regs[0x418/4]>>7) & 0x1);
  fprintf(fd,"TS_STMULT_DIS_TRIG9       %6d\n", (regs[0x418/4]>>8) & 0x1);
  fprintf(fd,"TS_STMULT_DIS_TRIG10      %6d\n", (regs[0x418/4]>>9) & 0x1);
  fprintf(fd,"TS_STMULT_DIS_TRIG11      %6d\n", (regs[0x418/4]>>10) & 0x1);
  fprintf(fd,"TS_STMULT_DIS_TRIG12      %6d\n", (regs[0x418/4]>>11) & 0x1);

  fprintf(fd,"TS_ST_MULT_MIN            %6d\n", regs[0x500/4] & 0xFF);

  fprintf(fd,"TS_L2_SECLOGIC            %6d\n", regs[0x500/4] & 0xFF);
  fprintf(fd,"TS_L2_OUTPUTDELAY         %6d\n", regs[0x504/4] & 0x3FF);
  fprintf(fd,"TS_L2_OUTPUTWIDTH         %6d\n", regs[0x508/4] & 0xFF);

  fclose(fd);

  return(0);
}


#endif /*NOV_2010*/


#else /*PHOTON_RUN*/



#define NREGS1     18 /* configs and prescales */
#define NREGS2     54 /* delays */

#define STRLEN 1024


static unsigned short v1495regs1[NREGS1];
static unsigned short regs1[NREGS1];
static unsigned short v1495regs2[NREGS2];
static unsigned short regs2[NREGS2];
/*
v1495ConfigDownload(0x08510000,"/usr/local/clas/devel/coda/src/rol/test/TestTriggerConfig.dat")
*/

/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/
/* reads config file and updates hardware registers */
/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/
int
v1495ConfigDownload(unsigned int addr, char *fname)
{
  FILE *fd;
  char *ch, str[STRLEN], keyword[STRLEN], lutfilename[STRLEN];
  int i, nregs;
  unsigned int v1495base, value;
  volatile unsigned short *ptr16;
  unsigned short *mem16;

  if(sysBusToLocalAdrs(0x39,addr&0xFFFFFF,&v1495base) < 0)
  {
    v1495base = 0;
    printf("v1495ConfigDownload ERROR: undefined base address\n");
    return(-1);
  }

  if( (fd = fopen(fname,"r")) == NULL)
  {
    printf("v1495ConfigDownload: Can't open config file >%s<\n",fname);
    return(-2);
  }

  for(i=0; i<NREGS1; i++) v1495regs1[i] = regs1[i] = 0;
  for(i=0; i<NREGS2; i++) v1495regs2[i] = regs2[i] = 0;
  while((ch = fgets(str, STRLEN, fd)) != NULL)
  {
	/*printf(">%s< %d\n",str,ch);*/
    if( ch[0] == '#' || ch[0] == ' ' || ch[0] == '\t' || ch[0] == '\n' || ch[0] == '\r' )
    {
      ;
    }
    else
    {
      sscanf(str,"%s %s", keyword, lutfilename);
      value = atoi(lutfilename);
      printf("key >%s< lutfilename >%s< value = %d\n",keyword,lutfilename,value);



      /*sscanf(str,"%s %d", keyword, &value);*/
      /*printf("key >%s< value = %d\n",keyword,value);*/

      if(     !strcmp(keyword,"TS_SECTOR1_CFG_ECE"))    v1495regs1[0x00/2] |= (value & 0x1);
      else if(!strcmp(keyword,"TS_SECTOR1_CFG_CC"))     v1495regs1[0x00/2] |= (value & 0x1)<<1;
      else if(!strcmp(keyword,"TS_SECTOR1_CFG_FORCE1")) v1495regs1[0x00/2] |= (value & 0x1)<<2;
      else if(!strcmp(keyword,"TS_SECTOR1_CFG_FORCE0")) v1495regs1[0x00/2] |= (value & 0x1)<<3;

      else if(!strcmp(keyword,"TS_SECTOR2_CFG_ECE"))    v1495regs1[0x02/2] |= (value & 0x1);
      else if(!strcmp(keyword,"TS_SECTOR2_CFG_CC"))     v1495regs1[0x02/2] |= (value & 0x1)<<1;
      else if(!strcmp(keyword,"TS_SECTOR2_CFG_FORCE1")) v1495regs1[0x02/2] |= (value & 0x1)<<2;
      else if(!strcmp(keyword,"TS_SECTOR2_CFG_FORCE0")) v1495regs1[0x02/2] |= (value & 0x1)<<3;

      else if(!strcmp(keyword,"TS_SECTOR3_CFG_ECE"))    v1495regs1[0x04/2] |= (value & 0x1);
      else if(!strcmp(keyword,"TS_SECTOR3_CFG_CC"))     v1495regs1[0x04/2] |= (value & 0x1)<<1;
      else if(!strcmp(keyword,"TS_SECTOR3_CFG_FORCE1")) v1495regs1[0x04/2] |= (value & 0x1)<<2;
      else if(!strcmp(keyword,"TS_SECTOR3_CFG_FORCE0")) v1495regs1[0x04/2] |= (value & 0x1)<<3;

      else if(!strcmp(keyword,"TS_SECTOR4_CFG_ECE"))    v1495regs1[0x06/2] |= (value & 0x1);
      else if(!strcmp(keyword,"TS_SECTOR4_CFG_CC"))     v1495regs1[0x06/2] |= (value & 0x1)<<1;
      else if(!strcmp(keyword,"TS_SECTOR4_CFG_FORCE1")) v1495regs1[0x06/2] |= (value & 0x1)<<2;
      else if(!strcmp(keyword,"TS_SECTOR4_CFG_FORCE0")) v1495regs1[0x06/2] |= (value & 0x1)<<3;

      else if(!strcmp(keyword,"TS_SECTOR5_CFG_ECE"))    v1495regs1[0x08/2] |= (value & 0x1);
      else if(!strcmp(keyword,"TS_SECTOR5_CFG_CC"))     v1495regs1[0x08/2] |= (value & 0x1)<<1;
      else if(!strcmp(keyword,"TS_SECTOR5_CFG_FORCE1")) v1495regs1[0x08/2] |= (value & 0x1)<<2;
      else if(!strcmp(keyword,"TS_SECTOR5_CFG_FORCE0")) v1495regs1[0x08/2] |= (value & 0x1)<<3;

      else if(!strcmp(keyword,"TS_SECTOR6_CFG_ECE"))    v1495regs1[0x0A/2] |= (value & 0x1);
      else if(!strcmp(keyword,"TS_SECTOR6_CFG_CC"))     v1495regs1[0x0A/2] |= (value & 0x1)<<1;
      else if(!strcmp(keyword,"TS_SECTOR6_CFG_FORCE1")) v1495regs1[0x0A/2] |= (value & 0x1)<<2;
      else if(!strcmp(keyword,"TS_SECTOR6_CFG_FORCE0")) v1495regs1[0x0A/2] |= (value & 0x1)<<3;



	  else if(!strcmp(keyword,"TS_PRESCALE_TRIG1"))  v1495regs1[0x0C/2] = value & 0x3FF;
	  else if(!strcmp(keyword,"TS_PRESCALE_TRIG2"))  v1495regs1[0x0E/2] = value & 0x3FF;
	  else if(!strcmp(keyword,"TS_PRESCALE_TRIG3"))  v1495regs1[0x10/2] = value & 0x3FF;
	  else if(!strcmp(keyword,"TS_PRESCALE_TRIG4"))  v1495regs1[0x12/2] = value & 0x3FF;
	  else if(!strcmp(keyword,"TS_PRESCALE_TRIG5"))  v1495regs1[0x14/2] = value & 0x3FF;
	  else if(!strcmp(keyword,"TS_PRESCALE_TRIG6"))  v1495regs1[0x16/2] = value & 0x3FF;
	  else if(!strcmp(keyword,"TS_PRESCALE_TRIG7"))  v1495regs1[0x18/2] = value & 0x3FF;
	  else if(!strcmp(keyword,"TS_PRESCALE_TRIG8"))  v1495regs1[0x1A/2] = value & 0x3FF;
	  else if(!strcmp(keyword,"TS_PRESCALE_TRIG9"))  v1495regs1[0x1C/2] = value & 0x3FF;
	  else if(!strcmp(keyword,"TS_PRESCALE_TRIG10")) v1495regs1[0x1E/2] = value & 0x3FF;
	  else if(!strcmp(keyword,"TS_PRESCALE_TRIG11")) v1495regs1[0x20/2] = value & 0x3FF;
	  else if(!strcmp(keyword,"TS_PRESCALE_TRIG12")) v1495regs1[0x22/2] = value & 0x3FF;



	  else if(!strcmp(keyword,"TS_TOF1_DELAY")) v1495regs2[0x00/2] = value & 0x1F;
	  else if(!strcmp(keyword,"TS_TOF2_DELAY")) v1495regs2[0x02/2] = value & 0x1F;
	  else if(!strcmp(keyword,"TS_TOF3_DELAY")) v1495regs2[0x04/2] = value & 0x1F;
	  else if(!strcmp(keyword,"TS_TOF4_DELAY")) v1495regs2[0x06/2] = value & 0x1F;
	  else if(!strcmp(keyword,"TS_TOF5_DELAY")) v1495regs2[0x08/2] = value & 0x1F;
	  else if(!strcmp(keyword,"TS_TOF6_DELAY")) v1495regs2[0x0A/2] = value & 0x1F;

      else if(!strcmp(keyword,"TS_ECin1P_DELAY")) v1495regs2[0x0C/2] = value & 0x1F;
      else if(!strcmp(keyword,"TS_ECin2P_DELAY")) v1495regs2[0x0E/2] = value & 0x1F;
      else if(!strcmp(keyword,"TS_ECin3P_DELAY")) v1495regs2[0x10/2] = value & 0x1F;
      else if(!strcmp(keyword,"TS_ECin4P_DELAY")) v1495regs2[0x12/2] = value & 0x1F;
      else if(!strcmp(keyword,"TS_ECin5P_DELAY")) v1495regs2[0x14/2] = value & 0x1F;
      else if(!strcmp(keyword,"TS_ECin6P_DELAY")) v1495regs2[0x16/2] = value & 0x1F;

      else if(!strcmp(keyword,"TS_ECin1E_DELAY")) v1495regs2[0x18/2] = value & 0x1F;
      else if(!strcmp(keyword,"TS_ECin2E_DELAY")) v1495regs2[0x1A/2] = value & 0x1F;
      else if(!strcmp(keyword,"TS_ECin3E_DELAY")) v1495regs2[0x1C/2] = value & 0x1F;
      else if(!strcmp(keyword,"TS_ECin4E_DELAY")) v1495regs2[0x1E/2] = value & 0x1F;
      else if(!strcmp(keyword,"TS_ECin5E_DELAY")) v1495regs2[0x20/2] = value & 0x1F;
      else if(!strcmp(keyword,"TS_ECin6E_DELAY")) v1495regs2[0x22/2] = value & 0x1F;

      else if(!strcmp(keyword,"TS_ECtot1P_DELAY")) v1495regs2[0x24/2] = value & 0x1F;
      else if(!strcmp(keyword,"TS_ECtot2P_DELAY")) v1495regs2[0x26/2] = value & 0x1F;
      else if(!strcmp(keyword,"TS_ECtot3P_DELAY")) v1495regs2[0x28/2] = value & 0x1F;
      else if(!strcmp(keyword,"TS_ECtot4P_DELAY")) v1495regs2[0x2A/2] = value & 0x1F;
      else if(!strcmp(keyword,"TS_ECtot5P_DELAY")) v1495regs2[0x2C/2] = value & 0x1F;
      else if(!strcmp(keyword,"TS_ECtot6P_DELAY")) v1495regs2[0x2E/2] = value & 0x1F;

      else if(!strcmp(keyword,"TS_ECtot1E_DELAY")) v1495regs2[0x30/2] = value & 0x1F;
      else if(!strcmp(keyword,"TS_ECtot2E_DELAY")) v1495regs2[0x32/2] = value & 0x1F;
      else if(!strcmp(keyword,"TS_ECtot3E_DELAY")) v1495regs2[0x34/2] = value & 0x1F;
      else if(!strcmp(keyword,"TS_ECtot4E_DELAY")) v1495regs2[0x36/2] = value & 0x1F;
      else if(!strcmp(keyword,"TS_ECtot5E_DELAY")) v1495regs2[0x38/2] = value & 0x1F;
      else if(!strcmp(keyword,"TS_ECtot6E_DELAY")) v1495regs2[0x3A/2] = value & 0x1F;

      else if(!strcmp(keyword,"TS_CC1_DELAY")) v1495regs2[0x3C/2] = value & 0x1F;
      else if(!strcmp(keyword,"TS_CC2_DELAY")) v1495regs2[0x3E/2] = value & 0x1F;
      else if(!strcmp(keyword,"TS_CC3_DELAY")) v1495regs2[0x40/2] = value & 0x1F;
      else if(!strcmp(keyword,"TS_CC4_DELAY")) v1495regs2[0x42/2] = value & 0x1F;
      else if(!strcmp(keyword,"TS_CC5_DELAY")) v1495regs2[0x44/2] = value & 0x1F;
      else if(!strcmp(keyword,"TS_CC6_DELAY")) v1495regs2[0x46/2] = value & 0x1F;

      else if(!strcmp(keyword,"TS_E_ECin1E_DELAY")) v1495regs2[0x48/2] = value & 0x1F;
      else if(!strcmp(keyword,"TS_E_ECin2E_DELAY")) v1495regs2[0x4A/2] = value & 0x1F;
      else if(!strcmp(keyword,"TS_E_ECin3E_DELAY")) v1495regs2[0x4C/2] = value & 0x1F;
      else if(!strcmp(keyword,"TS_E_ECin4E_DELAY")) v1495regs2[0x4E/2] = value & 0x1F;
      else if(!strcmp(keyword,"TS_E_ECin5E_DELAY")) v1495regs2[0x50/2] = value & 0x1F;
      else if(!strcmp(keyword,"TS_E_ECin6E_DELAY")) v1495regs2[0x52/2] = value & 0x1F;

      else if(!strcmp(keyword,"TS_E_ECtot1E_DELAY")) v1495regs2[0x54/2] = value & 0x1F;
      else if(!strcmp(keyword,"TS_E_ECtot2E_DELAY")) v1495regs2[0x56/2] = value & 0x1F;
      else if(!strcmp(keyword,"TS_E_ECtot3E_DELAY")) v1495regs2[0x58/2] = value & 0x1F;
      else if(!strcmp(keyword,"TS_E_ECtot4E_DELAY")) v1495regs2[0x5A/2] = value & 0x1F;
      else if(!strcmp(keyword,"TS_E_ECtot5E_DELAY")) v1495regs2[0x5C/2] = value & 0x1F;
      else if(!strcmp(keyword,"TS_E_ECtot6E_DELAY")) v1495regs2[0x5E/2] = value & 0x1F;

      else if(!strcmp(keyword,"TS_E_CC1_DELAY")) v1495regs2[0x60/2] = value & 0x1F;
      else if(!strcmp(keyword,"TS_E_CC2_DELAY")) v1495regs2[0x62/2] = value & 0x1F;
      else if(!strcmp(keyword,"TS_E_CC3_DELAY")) v1495regs2[0x64/2] = value & 0x1F;
      else if(!strcmp(keyword,"TS_E_CC4_DELAY")) v1495regs2[0x66/2] = value & 0x1F;
      else if(!strcmp(keyword,"TS_E_CC5_DELAY")) v1495regs2[0x68/2] = value & 0x1F;
      else if(!strcmp(keyword,"TS_E_CC6_DELAY")) v1495regs2[0x6A/2] = value & 0x1F;

      else if(!strcmp(keyword,"TS_TRIG1_TRIGLUTFILE")) v1495LUTDownload(addr,lutfilename,1,0);
      else if(!strcmp(keyword,"TS_TRIG1_ECPLUTFILE"))  v1495LUTDownload(addr,lutfilename,1,1);
      else if(!strcmp(keyword,"TS_TRIG1_ECELUTFILE"))  v1495LUTDownload(addr,lutfilename,1,2);
      else if(!strcmp(keyword,"TS_TRIG1_ECCLUTFILE"))  v1495LUTDownload(addr,lutfilename,1,3);

      else if(!strcmp(keyword,"TS_TRIG2_TRIGLUTFILE")) v1495LUTDownload(addr,lutfilename,2,0);
      else if(!strcmp(keyword,"TS_TRIG2_ECPLUTFILE"))  v1495LUTDownload(addr,lutfilename,2,1);
      else if(!strcmp(keyword,"TS_TRIG2_ECELUTFILE"))  v1495LUTDownload(addr,lutfilename,2,2);
      else if(!strcmp(keyword,"TS_TRIG2_ECCLUTFILE"))  v1495LUTDownload(addr,lutfilename,2,3);

      else if(!strcmp(keyword,"TS_TRIG3_TRIGLUTFILE")) v1495LUTDownload(addr,lutfilename,3,0);
      else if(!strcmp(keyword,"TS_TRIG3_ECPLUTFILE"))  v1495LUTDownload(addr,lutfilename,3,1);
      else if(!strcmp(keyword,"TS_TRIG3_ECELUTFILE"))  v1495LUTDownload(addr,lutfilename,3,2);
      else if(!strcmp(keyword,"TS_TRIG3_ECCLUTFILE"))  v1495LUTDownload(addr,lutfilename,3,3);

      else if(!strcmp(keyword,"TS_TRIG4_TRIGLUTFILE")) v1495LUTDownload(addr,lutfilename,4,0);
      else if(!strcmp(keyword,"TS_TRIG4_ECPLUTFILE"))  v1495LUTDownload(addr,lutfilename,4,1);
      else if(!strcmp(keyword,"TS_TRIG4_ECELUTFILE"))  v1495LUTDownload(addr,lutfilename,4,2);
      else if(!strcmp(keyword,"TS_TRIG4_ECCLUTFILE"))  v1495LUTDownload(addr,lutfilename,4,3);

      else if(!strcmp(keyword,"TS_TRIG5_TRIGLUTFILE")) v1495LUTDownload(addr,lutfilename,5,0);
      else if(!strcmp(keyword,"TS_TRIG5_ECPLUTFILE"))  v1495LUTDownload(addr,lutfilename,5,1);
      else if(!strcmp(keyword,"TS_TRIG5_ECELUTFILE"))  v1495LUTDownload(addr,lutfilename,5,2);
      else if(!strcmp(keyword,"TS_TRIG5_ECCLUTFILE"))  v1495LUTDownload(addr,lutfilename,5,3);

      else if(!strcmp(keyword,"TS_TRIG6_TRIGLUTFILE")) v1495LUTDownload(addr,lutfilename,6,0);
      else if(!strcmp(keyword,"TS_TRIG6_ECPLUTFILE"))  v1495LUTDownload(addr,lutfilename,6,1);
      else if(!strcmp(keyword,"TS_TRIG6_ECELUTFILE"))  v1495LUTDownload(addr,lutfilename,6,2);
      else if(!strcmp(keyword,"TS_TRIG6_ECCLUTFILE"))  v1495LUTDownload(addr,lutfilename,6,3);

      else if(!strcmp(keyword,"TS_TRIG7_TRIGLUTFILE")) v1495LUTDownload(addr,lutfilename,7,0);
      else if(!strcmp(keyword,"TS_TRIG7_ECPLUTFILE"))  v1495LUTDownload(addr,lutfilename,7,1);
      else if(!strcmp(keyword,"TS_TRIG7_ECELUTFILE"))  v1495LUTDownload(addr,lutfilename,7,2);
      else if(!strcmp(keyword,"TS_TRIG7_ECCLUTFILE"))  v1495LUTDownload(addr,lutfilename,7,3);

      else if(!strcmp(keyword,"TS_TRIG8_TRIGLUTFILE")) v1495LUTDownload(addr,lutfilename,8,0);
      else if(!strcmp(keyword,"TS_TRIG8_ECPLUTFILE"))  v1495LUTDownload(addr,lutfilename,8,1);
      else if(!strcmp(keyword,"TS_TRIG8_ECELUTFILE"))  v1495LUTDownload(addr,lutfilename,8,2);
      else if(!strcmp(keyword,"TS_TRIG8_ECCLUTFILE"))  v1495LUTDownload(addr,lutfilename,8,3);

      else if(!strcmp(keyword,"TS_TRIG9_TRIGLUTFILE")) v1495LUTDownload(addr,lutfilename,9,0);
      else if(!strcmp(keyword,"TS_TRIG9_ECPLUTFILE"))  v1495LUTDownload(addr,lutfilename,9,1);
      else if(!strcmp(keyword,"TS_TRIG9_ECELUTFILE"))  v1495LUTDownload(addr,lutfilename,9,2);
      else if(!strcmp(keyword,"TS_TRIG9_ECCLUTFILE"))  v1495LUTDownload(addr,lutfilename,9,3);

      else if(!strcmp(keyword,"TS_TRIG10_TRIGLUTFILE")) v1495LUTDownload(addr,lutfilename,10,0);
      else if(!strcmp(keyword,"TS_TRIG10_ECPLUTFILE"))  v1495LUTDownload(addr,lutfilename,10,1);
      else if(!strcmp(keyword,"TS_TRIG10_ECELUTFILE"))  v1495LUTDownload(addr,lutfilename,10,2);
      else if(!strcmp(keyword,"TS_TRIG10_ECCLUTFILE"))  v1495LUTDownload(addr,lutfilename,10,3);

      else if(!strcmp(keyword,"TS_TRIG11_TRIGLUTFILE")) v1495LUTDownload(addr,lutfilename,11,0);
      else if(!strcmp(keyword,"TS_TRIG11_ECPLUTFILE"))  v1495LUTDownload(addr,lutfilename,11,1);
      else if(!strcmp(keyword,"TS_TRIG11_ECELUTFILE"))  v1495LUTDownload(addr,lutfilename,11,2);
      else if(!strcmp(keyword,"TS_TRIG11_ECCLUTFILE"))  v1495LUTDownload(addr,lutfilename,11,3);

      else
	  {
        printf("v1495ConfigDownload ERROR: unknown keyword >%s< (%d 0x%02x)\n",keyword,ch[0],ch[0]);
        return(-3);
	  }
	}
  }

  fclose(fd);


  semTake(v1495scalers_lock, WAIT_FOREVER);

  /*************************/
  /* download: 0x100-0x122 */
  nregs = NREGS1;
  ptr16 = (volatile unsigned short *)(v1495base+0x100);
  mem16 = (unsigned short *) &v1495regs1[0];
  EIEIO;
  SYNC;
  for(i=(nregs-1); i>=0; i--) /* NEED REVERSE READOUT HERE */
  {
    ptr16[i] = mem16[i];
    /*printf("[%d] white %d to 0x%08x\n",i,mem16[i],&ptr16[i]);*/
  }
  EIEIO;
  SYNC;

  /***************************/
  /* download: 0x1400-0x146A */
  nregs = NREGS2;
  ptr16 = (volatile unsigned short *)(v1495base+0x1400);
  mem16 = (unsigned short *) &v1495regs2[0];
  EIEIO;
  SYNC;
  for(i=(nregs-1); i>=0; i--) /* NEED REVERSE READOUT HERE */
  {
    ptr16[i] = mem16[i];
  }
  EIEIO;
  SYNC;

  /********************/
  /*upload and compare*/
  nregs = NREGS1;
  ptr16 = (volatile unsigned short *)(v1495base+0x100);
  mem16 = (unsigned short *) &regs1[0];
  for(i=(nregs-1); i>=0; i--) /* NEED REVERSE READOUT HERE */
  {
    mem16[i] = ptr16[i];
  }
  EIEIO;
  SYNC;

  nregs = NREGS2;
  ptr16 = (volatile unsigned short *)(v1495base+0x1400);
  mem16 = (unsigned short *) &regs2[0];
  for(i=(nregs-1); i>=0; i--) /* NEED REVERSE READOUT HERE */
  {
    mem16[i] = ptr16[i];
  }
  EIEIO;
  SYNC;

  semGive(v1495scalers_lock);

  nregs = NREGS1;
  for(i=0; i<nregs; i++)
  {
    if(v1495regs1[i] != regs1[i])
	{
      printf("v1495ConfigDownload ERROR: [0x%03x/2] load 0x%04x, read back 0x%04x\n",
        i*2,v1495regs1[i],regs1[i]);
      return(-4);
	}
    else
	{
      printf("v1495ConfigDownload: [0x%03x/2] passed: 0x%04x = 0x%04x\n",i*2,v1495regs1[i],regs1[i]);
	}
  }

  nregs = NREGS2;
  for(i=0; i<nregs; i++)
  {
    if(v1495regs2[i] != regs2[i])
	{
      printf("v1495ConfigDownload ERROR: [0x%03x/2] load 0x%04x, read back 0x%04x\n",
        i*2,v1495regs2[i],regs2[i]);
      return(-5);
	}
    else
	{
      printf("v1495ConfigDownload: [0x%03x/2] passed: 0x%04x = 0x%04x\n",i*2,v1495regs2[i],regs2[i]);
	}
  }

  return(0);
}

/*
v1495ConfigUpload(0x08510000,"/home/boiarino/zzz.dat")
*/

/* reads hardware setting and write them to the file */
int
v1495ConfigUpload(unsigned int addr, char *fname)
{
  FILE *fd;
  unsigned int v1495base;
  int i, nregs;
  volatile unsigned short *ptr16;
  unsigned short *mem16;

  if(sysBusToLocalAdrs(0x39,addr&0xFFFFFF,&v1495base) < 0)
  {
    v1495base = 0;
    printf("v1495ConfigUpload ERROR: undefined base address\n");
    return(-1);
  }

  if( (fd = fopen(fname,"w")) == NULL)
  {
    printf("v1495ConfigUpload: Can't create output file >%s<\n",fname);
    return(-2);
  }

  for(i=0; i<NREGS1; i++) regs1[i] = 0;
  for(i=0; i<NREGS2; i++) regs2[i] = 0;

  semTake(v1495scalers_lock, WAIT_FOREVER);

  nregs = NREGS1;
  ptr16 = (volatile unsigned short *)(v1495base+0x100);
  mem16 = (unsigned short *) &regs1[0];
  for(i=(nregs-1); i>=0; i--) /* NEED REVERSE READOUT HERE */
  {
    mem16[i] = ptr16[i];
  }
  EIEIO;
  SYNC;

  nregs = NREGS2;
  ptr16 = (volatile unsigned short *)(v1495base+0x1400);
  mem16 = (unsigned short *) &regs2[0];
  for(i=(nregs-1); i>=0; i--) /* NEED REVERSE READOUT HERE */
  {
    mem16[i] = ptr16[i];
  }
  EIEIO;
  SYNC;

  semGive(v1495scalers_lock);


  fprintf(fd,"# Hall B Trigger Module Configuration Register Settings\n");
  /*fprintf(fd,"# Uploaded from hardware: Wednesday, March 26, 2008 at 11:13:55AM\n\n");*/
  fprintf(fd,"# Uploaded from hardware: %s\n",ctime(NULL));



  fprintf(fd,"TS_SECTOR1_CFG_ECE        %6d\n", regs1[0x00/2] & 0x1);
  fprintf(fd,"TS_SECTOR1_CFG_CC         %6d\n", (regs1[0x00/2]>>1) & 0x1);
  fprintf(fd,"TS_SECTOR1_CFG_FORCE1     %6d\n", (regs1[0x00/2]>>2) & 0x1);
  fprintf(fd,"TS_SECTOR1_CFG_FORCE0     %6d\n", (regs1[0x00/2]>>3) & 0x1);

  fprintf(fd,"TS_SECTOR2_CFG_ECE        %6d\n", regs1[0x02/2] & 0x1);
  fprintf(fd,"TS_SECTOR2_CFG_CC         %6d\n", (regs1[0x02/2]>>1) & 0x1);
  fprintf(fd,"TS_SECTOR2_CFG_FORCE1     %6d\n", (regs1[0x02/2]>>2) & 0x1);
  fprintf(fd,"TS_SECTOR2_CFG_FORCE0     %6d\n", (regs1[0x02/2]>>3) & 0x1);

  fprintf(fd,"TS_SECTOR3_CFG_ECE        %6d\n", regs1[0x04/2] & 0x1);
  fprintf(fd,"TS_SECTOR3_CFG_CC         %6d\n", (regs1[0x04/2]>>1) & 0x1);
  fprintf(fd,"TS_SECTOR3_CFG_FORCE1     %6d\n", (regs1[0x04/2]>>2) & 0x1);
  fprintf(fd,"TS_SECTOR3_CFG_FORCE0     %6d\n", (regs1[0x04/2]>>3) & 0x1);

  fprintf(fd,"TS_SECTOR4_CFG_ECE        %6d\n", regs1[0x06/2] & 0x1);
  fprintf(fd,"TS_SECTOR4_CFG_CC         %6d\n", (regs1[0x06/2]>>1) & 0x1);
  fprintf(fd,"TS_SECTOR4_CFG_FORCE1     %6d\n", (regs1[0x06/2]>>2) & 0x1);
  fprintf(fd,"TS_SECTOR4_CFG_FORCE0     %6d\n", (regs1[0x06/2]>>3) & 0x1);

  fprintf(fd,"TS_SECTOR5_CFG_ECE        %6d\n", regs1[0x08/2] & 0x1);
  fprintf(fd,"TS_SECTOR5_CFG_CC         %6d\n", (regs1[0x08/2]>>1) & 0x1);
  fprintf(fd,"TS_SECTOR5_CFG_FORCE1     %6d\n", (regs1[0x08/2]>>2) & 0x1);
  fprintf(fd,"TS_SECTOR5_CFG_FORCE0     %6d\n", (regs1[0x08/2]>>3) & 0x1);

  fprintf(fd,"TS_SECTOR6_CFG_ECE        %6d\n", regs1[0x0A/2] & 0x1);
  fprintf(fd,"TS_SECTOR6_CFG_CC         %6d\n", (regs1[0x0A/2]>>1) & 0x1);
  fprintf(fd,"TS_SECTOR6_CFG_FORCE1     %6d\n", (regs1[0x0A/2]>>2) & 0x1);
  fprintf(fd,"TS_SECTOR6_CFG_FORCE0     %6d\n", (regs1[0x0A/2]>>3) & 0x1);



  fprintf(fd,"TS_PRESCALE_TRIG1         %6d\n", regs1[0x0C/2] & 0x3FF);
  fprintf(fd,"TS_PRESCALE_TRIG2         %6d\n", regs1[0x0E/2] & 0x3FF);
  fprintf(fd,"TS_PRESCALE_TRIG3         %6d\n", regs1[0x10/2] & 0x3FF);
  fprintf(fd,"TS_PRESCALE_TRIG4         %6d\n", regs1[0x12/2] & 0x3FF);
  fprintf(fd,"TS_PRESCALE_TRIG5         %6d\n", regs1[0x14/2] & 0x3FF);
  fprintf(fd,"TS_PRESCALE_TRIG6         %6d\n", regs1[0x16/2] & 0x3FF);
  fprintf(fd,"TS_PRESCALE_TRIG7         %6d\n", regs1[0x18/2] & 0x3FF);
  fprintf(fd,"TS_PRESCALE_TRIG8         %6d\n", regs1[0x1A/2] & 0x3FF);
  fprintf(fd,"TS_PRESCALE_TRIG9         %6d\n", regs1[0x1C/2] & 0x3FF);
  fprintf(fd,"TS_PRESCALE_TRIG10        %6d\n", regs1[0x1E/2] & 0x3FF);
  fprintf(fd,"TS_PRESCALE_TRIG11        %6d\n", regs1[0x20/2] & 0x3FF);
  fprintf(fd,"TS_PRESCALE_TRIG12        %6d\n", regs1[0x22/2] & 0x3FF);



  fprintf(fd,"TS_TOF1_DELAY             %6d\n", regs2[0x00/2] & 0x1F);
  fprintf(fd,"TS_TOF2_DELAY             %6d\n", regs2[0x02/2] & 0x1F);
  fprintf(fd,"TS_TOF3_DELAY             %6d\n", regs2[0x04/2] & 0x1F);
  fprintf(fd,"TS_TOF4_DELAY             %6d\n", regs2[0x06/2] & 0x1F);
  fprintf(fd,"TS_TOF5_DELAY             %6d\n", regs2[0x08/2] & 0x1F);
  fprintf(fd,"TS_TOF6_DELAY             %6d\n", regs2[0x0A/2] & 0x1F);

  fprintf(fd,"TS_ECin1P_DELAY           %6d\n", regs2[0x0C/2] & 0x1F);
  fprintf(fd,"TS_ECin2P_DELAY           %6d\n", regs2[0x0E/2] & 0x1F);
  fprintf(fd,"TS_ECin3P_DELAY           %6d\n", regs2[0x10/2] & 0x1F);
  fprintf(fd,"TS_ECin4P_DELAY           %6d\n", regs2[0x12/2] & 0x1F);
  fprintf(fd,"TS_ECin5P_DELAY           %6d\n", regs2[0x14/2] & 0x1F);
  fprintf(fd,"TS_ECin6P_DELAY           %6d\n", regs2[0x16/2] & 0x1F);

  fprintf(fd,"TS_ECin1E_DELAY           %6d\n", regs2[0x18/2] & 0x1F);
  fprintf(fd,"TS_ECin2E_DELAY           %6d\n", regs2[0x1A/2] & 0x1F);
  fprintf(fd,"TS_ECin3E_DELAY           %6d\n", regs2[0x1C/2] & 0x1F);
  fprintf(fd,"TS_ECin4E_DELAY           %6d\n", regs2[0x1E/2] & 0x1F);
  fprintf(fd,"TS_ECin5E_DELAY           %6d\n", regs2[0x20/2] & 0x1F);
  fprintf(fd,"TS_ECin6E_DELAY           %6d\n", regs2[0x22/2] & 0x1F);

  fprintf(fd,"TS_ECtot1P_DELAY          %6d\n", regs2[0x24/2] & 0x1F);
  fprintf(fd,"TS_ECtot2P_DELAY          %6d\n", regs2[0x26/2] & 0x1F);
  fprintf(fd,"TS_ECtot3P_DELAY          %6d\n", regs2[0x28/2] & 0x1F);
  fprintf(fd,"TS_ECtot4P_DELAY          %6d\n", regs2[0x2A/2] & 0x1F);
  fprintf(fd,"TS_ECtot5P_DELAY          %6d\n", regs2[0x2C/2] & 0x1F);
  fprintf(fd,"TS_ECtot6P_DELAY          %6d\n", regs2[0x2E/2] & 0x1F);

  fprintf(fd,"TS_ECtot1E_DELAY          %6d\n", regs2[0x30/2] & 0x1F);
  fprintf(fd,"TS_ECtot2E_DELAY          %6d\n", regs2[0x32/2] & 0x1F);
  fprintf(fd,"TS_ECtot3E_DELAY          %6d\n", regs2[0x34/2] & 0x1F);
  fprintf(fd,"TS_ECtot4E_DELAY          %6d\n", regs2[0x36/2] & 0x1F);
  fprintf(fd,"TS_ECtot5E_DELAY          %6d\n", regs2[0x38/2] & 0x1F);
  fprintf(fd,"TS_ECtot6E_DELAY          %6d\n", regs2[0x3A/2] & 0x1F);

  fprintf(fd,"TS_CC1_DELAY              %6d\n", regs2[0x3C/2] & 0x1F);
  fprintf(fd,"TS_CC2_DELAY              %6d\n", regs2[0x3E/2] & 0x1F);
  fprintf(fd,"TS_CC3_DELAY              %6d\n", regs2[0x40/2] & 0x1F);
  fprintf(fd,"TS_CC4_DELAY              %6d\n", regs2[0x42/2] & 0x1F);
  fprintf(fd,"TS_CC5_DELAY              %6d\n", regs2[0x44/2] & 0x1F);
  fprintf(fd,"TS_CC6_DELAY              %6d\n", regs2[0x46/2] & 0x1F);

  fprintf(fd,"TS_E_ECin1E_DELAY         %6d\n", regs2[0x48/2] & 0x1F);
  fprintf(fd,"TS_E_ECin2E_DELAY         %6d\n", regs2[0x4A/2] & 0x1F);
  fprintf(fd,"TS_E_ECin3E_DELAY         %6d\n", regs2[0x4C/2] & 0x1F);
  fprintf(fd,"TS_E_ECin4E_DELAY         %6d\n", regs2[0x4E/2] & 0x1F);
  fprintf(fd,"TS_E_ECin5E_DELAY         %6d\n", regs2[0x50/2] & 0x1F);
  fprintf(fd,"TS_E_ECin6E_DELAY         %6d\n", regs2[0x52/2] & 0x1F);

  fprintf(fd,"TS_E_ECtot1E_DELAY        %6d\n", regs2[0x54/2] & 0x1F);
  fprintf(fd,"TS_E_ECtot2E_DELAY        %6d\n", regs2[0x56/2] & 0x1F);
  fprintf(fd,"TS_E_ECtot3E_DELAY        %6d\n", regs2[0x58/2] & 0x1F);
  fprintf(fd,"TS_E_ECtot4E_DELAY        %6d\n", regs2[0x5A/2] & 0x1F);
  fprintf(fd,"TS_E_ECtot5E_DELAY        %6d\n", regs2[0x5C/2] & 0x1F);
  fprintf(fd,"TS_E_ECtot6E_DELAY        %6d\n", regs2[0x5E/2] & 0x1F);

  fprintf(fd,"TS_E_CC1_DELAY            %6d\n", regs2[0x60/2] & 0x1F);
  fprintf(fd,"TS_E_CC2_DELAY            %6d\n", regs2[0x62/2] & 0x1F);
  fprintf(fd,"TS_E_CC3_DELAY            %6d\n", regs2[0x64/2] & 0x1F);
  fprintf(fd,"TS_E_CC4_DELAY            %6d\n", regs2[0x66/2] & 0x1F);
  fprintf(fd,"TS_E_CC5_DELAY            %6d\n", regs2[0x68/2] & 0x1F);
  fprintf(fd,"TS_E_CC6_DELAY            %6d\n", regs2[0x6A/2] & 0x1F);

  fclose(fd);

  return(0);
}







/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/
/* download/upload functions for the lookup table */
/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/

/*v1495LUTDownload(0x08510000,"/usr/local/clas/devel/coda/src/rol/test/tables/trig_ic2.lut",1,0)*/

#define NLUT    256
#define NLUT4   4

/* reads config file and updates hardware registers */
int
v1495LUTDownload(unsigned int addr, char *fname, int trigbit, int trig_ecp_ece_ecc)
{
  FILE *fd;
  char *ch, str[STRLEN], keyword[STRLEN];
  int i, nregs, nline, nbit, nlut;
  unsigned int value, v1495base;
  volatile unsigned short *ptr16;
  unsigned short *mem16;
  unsigned short v1495lut[NLUT];
  unsigned short lut[NLUT];

  printf("v1495LUTDownload: file >%s<, trigbit=%d, trig_ecp_ece_ecc=%d\n",
		 fname,trigbit,trig_ecp_ece_ecc);

  if(trigbit<1 || trigbit>12)
  {
    printf("v1495LUTDownload: error: trigbit out of range - exit\n");
    return(-1);
  }

  if(trig_ecp_ece_ecc<0 || trig_ecp_ece_ecc>3)
  {
    printf("v1495LUTDownload: error: trig_ecp_ece_ecc out of range - exit\n");
    return(-2);
  }

  if(sysBusToLocalAdrs(0x39,addr&0xFFFFFF,&v1495base) < 0)
  {
    v1495base = 0;
    printf("v1495LUTDownload ERROR: undefined base address\n");
    return(-3);
  }

  if( (fd = fopen(fname,"r")) == NULL)
  {
    printf("v1495LUTDownload: Can't open config file >%s<\n",fname);
    return(-4);
  }

  if(trig_ecp_ece_ecc==3) nlut = NLUT4;
  else                    nlut = NLUT;

  nline = nbit = 0;
  mem16 = (unsigned short *) &v1495lut[0];
  for(i=0; i<nlut; i++) v1495lut[i] = lut[i] = 0;
  while((ch = fgets(str, STRLEN, fd)) != NULL)
  {
	/*printf(">%s< %d\n",str,ch);*/
    if( ch[0] == '#' || ch[0] == ' ' || ch[0] == '\t' || ch[0] == '\n' || ch[0] == '\r' )
    {
      ;
    }
    else
    {
      nline ++;
      sscanf(str,"%d %s",&value,keyword);
      /*printf("value = %d key >%s<\n",value,keyword);*/

      if(nline <= (nlut*16))
	  {
        *mem16 |= (value&0x1) << nbit;
        nbit ++;
        if(nbit >= 16)
        {
          /*printf("===> nline=%d, value=0x%04x\n",nline,*mem16);*/
          nbit = 0;
          mem16 ++;
        }
	  }
      else
	  {
        printf("v1495LUTDownload: File too long (nline=%d) - interrupt reading\n",nline);
        fclose(fd);
        return(-3);
	  }
	}
  }

  fclose(fd);

  /*for(i=0; i<31; i++) printf("[%2d] 0x%04x\n",i,v1495lut[i]);*/

  semTake(v1495scalers_lock, WAIT_FOREVER);

  /* download */

  printf("v1495LUTDownload: loading LUT to trigger bit %d (counting from 1 to 12)\n",trigbit);
  ptr16 = (volatile unsigned short *)(v1495base + TS_TRIG_SEL);
  *ptr16 = (1<<(trigbit-1));
  EIEIO;
  SYNC;

  if(trig_ecp_ece_ecc==0)      ptr16 = (volatile unsigned short *)(v1495base + TS_TRIG_LUT_TRIG);
  else if(trig_ecp_ece_ecc==1) ptr16 = (volatile unsigned short *)(v1495base + TS_TRIG_LUT_ECP);
  else if(trig_ecp_ece_ecc==2) ptr16 = (volatile unsigned short *)(v1495base + TS_TRIG_LUT_ECE);
  else if(trig_ecp_ece_ecc==3) ptr16 = (volatile unsigned short *)(v1495base + TS_TRIG_LUT_ECC);

  mem16 = (unsigned short *) &v1495lut[0];

  for(i=(nlut-1); i>=0; i--) /* NEED REVERSE READOUT HERE */
  {
    EIEIO;
    SYNC;
    ptr16[i] = mem16[i];
    EIEIO;
    SYNC;
	/*printf("my: [%3d] addr=0x%08x(0x%08x) value=0x%04x\n",i,&ptr16[i],&mem16[i],mem16[i]);*/
  }



/* reading hardware back is not implemented yet !!! */
semGive(v1495scalers_lock);
goto skipcheck1;





  /*upload and compare*/
  if(trig_ecp_ece_ecc)
  {
    /* ECC */
    ptr16 = (volatile unsigned short *)(v1495base + (((trigbit-1))<<9) );
  }
  else
  {
    /* ECP */
    ptr16 = (volatile unsigned short *)(v1495base + (((trigbit-1)+12)<<9) );
  }
  mem16 = (unsigned short *) &lut[0];

  for(i=(nlut-1); i>=0; i--) /* NEED REVERSE READOUT HERE */
  {
    mem16[i] = ptr16[i];
  }

  semGive(v1495scalers_lock);




  printf("/nv1495LUTDownload: reading back and comparing\n");
  for(i=0; i<nlut; i++)
  {
    if(v1495lut[i] != lut[i])
	{
      printf("v1495LUTDownload ERROR: [0x%02x] load 0x%04x, read back 0x%04x\n",
        i,v1495lut[i],lut[i]);
      /*return(-4);*/
	}
    else
	{
      printf("v1495LUTDownload: [0x%02x] passed: 0x%04x = 0x%04x\n",i,v1495lut[i],lut[i]);
	}
  }

skipcheck1:

  return(0);
}







/* reads LUT from hardware and write it to the file */
/* NOT IMPLEMENTED YET !!!!!!!!!!!!!!!!*/
int
v1495LUTUpload(unsigned int addr, char *fname, int trigbit, int trig_ecp_ece_ecc)
{
  FILE *fd;
  unsigned short lut[NLUT];
  int i, j, nline;
  volatile unsigned short *ptr16;
  unsigned short *mem16;
  unsigned int v1495base;

  if(sysBusToLocalAdrs(0x39,addr&0xFFFFFF,&v1495base) < 0)
  {
    v1495base = 0;
    printf("v1495ConfigUpload ERROR: undefined base address\n");
    return(-1);
  }

  if(v1495base == 0)
  {
    printf("v1495LUTUpload ERROR: undefined base address\n");
    return(-1);
  }

  if( (fd = fopen(fname,"w")) == NULL)
  {
    printf("v1495LUTUpload: Can't create output file >%s<\n",fname);
    return(-2);
  }

  for(i=0; i<NLUT; i++) lut[i] = 0;

  semTake(v1495scalers_lock, WAIT_FOREVER);

  if(trig_ecp_ece_ecc)
  {
    /* ECC */
    printf("v1495LUTUpload: uploading ECC for trigger bit %d (counting from 1 to 12)\n",trigbit);
    ptr16 = (volatile unsigned short *)(v1495base + (((trigbit-1))<<9) );
  }
  else
  {
    /* ECP */
    printf("v1495LUTUpload: uploading ECP for trigger bit %d (counting from 1 to 12)\n",trigbit);
    ptr16 = (volatile unsigned short *)(v1495base + (((trigbit-1)+12)<<9) );
  }
  mem16 = (unsigned short *) &lut[0];

  /* NEED REVERSE READOUT HERE */
  /*for(i=0; i<NLUT; i++)*/
  for(i=(NLUT-1); i>=0; i--)
  {
    mem16[i] = ptr16[i];
  }

  semGive(v1495scalers_lock);

  fprintf(fd,"# G12 Hall B Trigger Module Lookup Table Settings\n");
  /*fprintf(fd,"# Uploaded from hardware: Wednesday, March 26, 2008 at 11:13:55AM\n\n");*/
  fprintf(fd,"# Uploaded from hardware: %s\n",ctime(NULL));

  nline = 0;
  for(i=0; i<NLUT; i++)
  {
    for(j=0; j<16; j++)
	{
      fprintf(fd,"%1d #0x%04x\n",(mem16[i]>>j)&0x1,nline);
      nline ++;
	}
  }

  fclose(fd);

  return(0);
}


#endif /*PHOTON_RUN*/




/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/
/* v1495 tcp servers */
/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/

#include <vxWorks.h>
#include <sockLib.h>
#include <inetLib.h>
#include <taskLib.h>
#include <stdioLib.h>
#include <strLib.h>
#include <ioLib.h>
#include <fioLib.h>

#include "libdb.h" 


#define DEBUG_REMOTE_CMD	0


#define SERVER_WORK_PRIORITY 250  /* priority of server's work task (<ROC !)*/ 
#define SERVER_STACK_SIZE 100000  /* stack size of server's work task */ 
#define SERVER_MAX_CONNECTIONS 4  /* max clients connected at a time */


/* User FPGA Addresses */
#define ADDR_SEL_FLASH_USER	0x8012
#define ADDR_RW_FLASH_USER	0x8014


/* VME FPGA Addresses */
#define ADDR_SEL_FLASH_VME	0x800E
#define ADDR_RW_FLASH_VME	0x8010

/* readiness flag */
static int request_in_progress;

/* function declarations */ 
/*char *targetName();*/ /* from roc_component.c */ 
VOID v1495tcpServerWorkTask1 (int gClientSocket, char * address, unsigned short port); 

extern STATUS execute();
int sysBusToLocalAdrs();


STATUS v1495tcpServer1(void) {v1495tcpServer(1);}

int
ProgramPage(Cmd_WritePage *pCmd_WritePage, unsigned int sel_addr, unsigned int rw_addr)
{
  volatile unsigned short *pAddr;
  int page = pCmd_WritePage->Page<<9;
  unsigned char adr0 = page;
  unsigned char adr1 = page>>8;
  unsigned char adr2 = page>>16;
  int i, verified = 1;

  sysBusToLocalAdrs(0x39, pCmd_WritePage->Address,(unsigned long *)&pAddr);

  pAddr[sel_addr>>1] = 0;			/* NCS = 0 */
  pAddr[rw_addr>>1] = 0x0082;		/* MAIN_MEM_PAGE_PROG_TH_BUF1 op code */
  pAddr[rw_addr>>1] = adr2;
  pAddr[rw_addr>>1] = adr1;
  pAddr[rw_addr>>1] = adr0;
  for(i = 0; i < PAGE_SIZE; i++)
  {
	pAddr[rw_addr>>1] = (unsigned short)pCmd_WritePage->Values[i];
  }
  pAddr[sel_addr>>1] = 1;			/* NCS = 1 */
  taskDelay(2);

  pAddr[sel_addr>>1] =  0;		/* NCS = 0 */
  pAddr[rw_addr>>1] = 0x00D2;		/* MAIN_MEM_PAGE_READ op code */
  pAddr[rw_addr>>1] = adr2;
  pAddr[rw_addr>>1] = adr1;
  pAddr[rw_addr>>1] = adr0;
  pAddr[rw_addr>>1] = 0;
  pAddr[rw_addr>>1] = 0;
  pAddr[rw_addr>>1] = 0;
  pAddr[rw_addr>>1] = 0;

  EIEIO;
  SYNC;
  for(i=0; i<PAGE_SIZE; i++)
  {
	unsigned short data = pAddr[rw_addr>>1];
	if((unsigned char)data != (unsigned char)pCmd_WritePage->Values[i])
	{
	  verified = 0;
	  break;
	}
  }
  pAddr[sel_addr>>1] = 1;			/* NCS = 1 */

  return(verified);
}


/**************************************************************************** 
* * tcpServerWorkTask - process client requests 
* * This routine reads from the server's socket, and processes client 
* requests. If the client requests a reply message, this routine 
* will send a reply to the client. 
* * RETURNS: N/A. */ 

/*int gClientSocket;*/
RemoteMsgStruct OutgoingMsg;

/*int gListenSocket;*/
RemoteMsgStruct IncomingMsg;

int
ProcessRemoteMsg(RemoteMsgStruct *pRemoteMsgStruct)
{
  int readfrommemory;

#if DEBUG_REMOTE_CMD
  printf("Message: Type=0x%08X, Len=%u, Board=%u\n", pRemoteMsgStruct->MsgType, pRemoteMsgStruct->Length, pRemoteMsgStruct->BoardType);
#endif

  switch(pRemoteMsgStruct->MsgType)
  {
	case REMOTE_CMD_READ16:
	{
	  Cmd_Read16 *pCmd_Read16 = (Cmd_Read16 *)pRemoteMsgStruct->Msg;
	  Cmd_Read16_Rsp *pCmd_Read16_Rsp = (Cmd_Read16_Rsp *)OutgoingMsg.Msg;
	  unsigned short *pData;

	  sysBusToLocalAdrs(0x39, pCmd_Read16->Address,(unsigned long *)&pData);


	  OutgoingMsg.BoardType = pRemoteMsgStruct->BoardType;
	  OutgoingMsg.Length = REMOTE_MSG_SIZE(Cmd_Read16_Rsp)-4;
	  OutgoingMsg.MsgType = CMD_RSP(REMOTE_CMD_READ16);

	  pCmd_Read16_Rsp->Value = *pData;

#if DEBUG_REMOTE_CMD
	  printf("Cmd_Read16: Address = 0x%08x (0x%08x), Data = 0x%04x\n", pCmd_Read16->Address, pData, pCmd_Read16_Rsp->Value);
#endif
	  return(OutgoingMsg.Length + 4);

	  break;
	}

	case REMOTE_CMD_WRITE16:
	{
	  Cmd_Write16 *pCmd_Write16 = (Cmd_Write16 *)pRemoteMsgStruct->Msg;
	  unsigned short *pData;

	  sysBusToLocalAdrs(0x39, pCmd_Write16->Address,(unsigned long *)&pData);
	  *pData = pCmd_Write16->Value;
#if DEBUG_REMOTE_CMD
	  printf("Cmd_Write16: Address = 0x%08X (0x%08x), Data = 0x%04hX\n", pCmd_Write16->Address,pData, pCmd_Write16->Value);
#endif
	  break;
	}

	case REMOTE_CMD_BLKREAD16:
	{
	  Cmd_BlkRead16 *pCmd_BlkRead16 = (Cmd_BlkRead16 *)pRemoteMsgStruct->Msg;
	  Cmd_BlkRead16_Rsp *pCmd_BlkRead16_Rsp = (Cmd_BlkRead16_Rsp *)OutgoingMsg.Msg;
	  volatile unsigned short *pData;
      unsigned short *sData;
	  int i;

	  sysBusToLocalAdrs(0x39, pCmd_BlkRead16->Address,(unsigned long *)&pData);

#if DEBUG_REMOTE_CMD
	  printf("Cmd_BlkRead16: Address = 0x%08x (0x%08x), count=%d\n", pCmd_BlkRead16->Address, pData, pCmd_BlkRead16->Count);
#endif

	  OutgoingMsg.BoardType = pRemoteMsgStruct->BoardType;
	  OutgoingMsg.Length = REMOTE_MSG_SIZE(Cmd_BlkRead16_Rsp)-sizeof(Cmd_BlkRead16_Rsp)+pCmd_BlkRead16->Count*sizeof(unsigned short)-4;
	  OutgoingMsg.MsgType = CMD_RSP(REMOTE_CMD_BLKREAD16);

	  if(0/*pCmd_BlkRead16->NoAddressInc*/) /* fifo-style readout (scope feature) */
	  {
		for(i = 0; i < pCmd_BlkRead16->Count; i++)
		{
		  pCmd_BlkRead16_Rsp->Values[i] = pData[0];
#if DEBUG_REMOTE_CMD
		  printf("{0x%08X => 0x%08X}\n", pCmd_BlkRead16->Address + i*4, pCmd_BlkRead16_Rsp->Values[i]);
#endif
		}
	  }
	  else /* array readout */
	  {
        /* check if specified space is scaler's space; if so, read scaler's image
        from memory space instead - if interval is NOT zero !! */
        readfrommemory = 0;
        if(v1495ScalersReadInterval>0)
        {
          for(i=0; i<v1495scalersnum; i++)
          {
            if( (pCmd_BlkRead16->Address == v1495scalersbegin[i])  && ((pCmd_BlkRead16->Count/4) == v1495scalerslen[i]) )
            {
              sData = (unsigned int *)(v1495scalers[i]);
#if DEBUG_REMOTE_CMD
              printf("found 0x%08x %6d -> read from memory 0x%08x\n",v1495scalersbegin[i],v1495scalerslen[i],pData);
#endif
              for(i=0; i<pCmd_BlkRead16->Count; i++) pCmd_BlkRead16_Rsp->Values[i] = sData[i];
              readfrommemory = 1;
              break;
            }
          }
        }

        /* read from VME space */
        if(readfrommemory==0)
        {
#if DEBUG_REMOTE_CMD
          printf("normal blk16 from 0x%08x, len=%d\n",pData,pCmd_BlkRead16->Count);
#endif
          for(i = 0; i<pCmd_BlkRead16->Count; i++)
		  /*for(i = pCmd_BlkRead16->Count-1; i >= 0 ; i--) temporary for old DVCS GUI compartibility*/
		  {
		    pCmd_BlkRead16_Rsp->Values[i] = pData[i];
#if DEBUG_REMOTE_CMD
		    printf("{0x%08X => 0x%08X}\n", pCmd_BlkRead16->Address + i*4, pCmd_BlkRead16_Rsp->Values[i]);
#endif
		  }
	    }

	  }

	  return(OutgoingMsg.Length + 4); 

	  break;
	}

    case REMOTE_CMD_WRITEPAGE: /*for USER FPGA firmware update only*/
	{
	  Cmd_WritePage *pCmd_WritePage = (Cmd_WritePage *)pRemoteMsgStruct->Msg;
	  Cmd_WritePage_Rsp *pCmd_WritePage_Rsp = (Cmd_WritePage_Rsp *)OutgoingMsg.Msg;

	  pCmd_WritePage_Rsp->Verified = ProgramPage(pCmd_WritePage, ADDR_SEL_FLASH_USER, ADDR_RW_FLASH_USER);
#if DEBUG_REMOTE_CMD
	  printf("REMOTE_CMD_WRITEPAGE %u Verified %u\n", pCmd_WritePage->Address, pCmd_WritePage_Rsp->Verified);
#endif
	  OutgoingMsg.BoardType = pRemoteMsgStruct->BoardType;
	  OutgoingMsg.Length = REMOTE_MSG_SIZE(Cmd_WritePage_Rsp)-4;
	  OutgoingMsg.MsgType = CMD_RSP(REMOTE_CMD_WRITEPAGE);

	  return(OutgoingMsg.Length + 4);

	  break;
	}

	case REMOTE_CMD_WRITEPAGE_VME: /*for VME FPGA firmware update only*/
	{
	  Cmd_WritePage *pCmd_WritePage = (Cmd_WritePage *)pRemoteMsgStruct->Msg;
	  Cmd_WritePage_Rsp *pCmd_WritePage_Rsp = (Cmd_WritePage_Rsp *)OutgoingMsg.Msg;
#if DEBUG_REMOTE_CMD
	  printf("REMOTE_CMD_WRITEPAGE_VME %u Verified %u\n", pCmd_WritePage->Address, pCmd_WritePage_Rsp->Verified);
#endif
	  pCmd_WritePage_Rsp->Verified = ProgramPage(pCmd_WritePage, ADDR_SEL_FLASH_VME, ADDR_RW_FLASH_VME);
	  OutgoingMsg.BoardType = pRemoteMsgStruct->BoardType;
	  OutgoingMsg.Length = REMOTE_MSG_SIZE(Cmd_WritePage_Rsp)-4;
	  OutgoingMsg.MsgType = CMD_RSP(REMOTE_CMD_WRITEPAGE_VME);

	  return(OutgoingMsg.Length + 4); 

	  break;
	}

	case REMOTE_CMD_READ32:
	{
	  Cmd_Read32 *pCmd_Read32 = (Cmd_Read32 *)pRemoteMsgStruct->Msg;
	  Cmd_Read32_Rsp *pCmd_Read32_Rsp = (Cmd_Read32_Rsp *)OutgoingMsg.Msg;
	  unsigned int *pData;
	  unsigned int *sData;
      int i;

	  sysBusToLocalAdrs(0x39, pCmd_Read32->Address,(unsigned long *)&pData);
			
	  OutgoingMsg.BoardType = pRemoteMsgStruct->BoardType;
	  OutgoingMsg.Length = REMOTE_MSG_SIZE(Cmd_Read32_Rsp)-4;
	  OutgoingMsg.MsgType = CMD_RSP(REMOTE_CMD_READ32);

      readfrommemory = 0;
      /* check if specified space is scaler's space; if so, read scaler's image
        from memory space instead - if interval is NOT zero !! */
      if(v1495ScalersReadInterval>0)
      {
        for(i=0; i<v1495scalersnum; i++)
        {
          if( (pCmd_Read32->Address >= v1495scalersbegin[i]) && (pCmd_Read32->Address <= v1495scalersend[i]) )
          {
            sData = (unsigned int *)(v1495scalers[i]);
#if DEBUG_REMOTE_CMD
            printf("found 0x%08x %6d -> read from memory 0x%08x\n",v1495scalersbegin[i],v1495scalerslen[i],pData);
#endif
            pCmd_Read32_Rsp->Value = sData[(pCmd_Read32->Address-v1495scalersbegin[i])/4];
            readfrommemory = 1;
            break;
          }
        }
      }

      /* read from VME space */
      if(readfrommemory==0)
      {
#if DEBUG_REMOTE_CMD
        printf("normal read32 from 0x%08x\n",pData);
#endif
        pCmd_Read32_Rsp->Value = *pData;
      }

	  return(OutgoingMsg.Length + 4); 

#if DEBUG_REMOTE_CMD
	  printf("Cmd_Read32: Address = 0x%08X, Data = 0x%08X\n", pCmd_Read32->Address, pCmd_Read32_Rsp->Value);
#endif
	  break;
	}

	case REMOTE_CMD_WRITE32:
	{
	  Cmd_Write32 *pCmd_Write32 = (Cmd_Write32 *)pRemoteMsgStruct->Msg;
	  unsigned int *pData;
      int i, flag;

	  sysBusToLocalAdrs(0x39, pCmd_Write32->Address,(unsigned long *)&pData);

	  /*sergey: ignore scalers enable/disable*/
      flag = 0;
      if(v1495ScalersReadInterval>0)
      {
        for(i=0; i<v1495scalersnum; i++)
        {
          if(pData == v1495scalersenable[i])
          {
            flag = 1;
            break;
		  }
        }
      }
      if(flag) break;

	  *pData = pCmd_Write32->Value;

#if DEBUG_REMOTE_CMD
	  printf("Cmd_Write32: Address = 0x%08X, Data = 0x%08X\n", pCmd_Write32->Address, pCmd_Write32->Value);
#endif
	  break;
	}

	case REMOTE_CMD_BLKREAD32:
	{
	  Cmd_BlkRead32 *pCmd_BlkRead32 = (Cmd_BlkRead32 *)pRemoteMsgStruct->Msg;
	  Cmd_BlkRead32_Rsp *pCmd_BlkRead32_Rsp = (Cmd_BlkRead32_Rsp *)OutgoingMsg.Msg;
	  volatile unsigned int *pData;
      unsigned int *sData;
	  int i;

	  sysBusToLocalAdrs(0x39, pCmd_BlkRead32->Address,(unsigned long *)&pData);

	  OutgoingMsg.BoardType = pRemoteMsgStruct->BoardType;
	  OutgoingMsg.Length = REMOTE_MSG_SIZE(Cmd_BlkRead32_Rsp)-sizeof(Cmd_BlkRead32_Rsp)+pCmd_BlkRead32->Count*sizeof(unsigned int)-4;
	  OutgoingMsg.MsgType = CMD_RSP(REMOTE_CMD_BLKREAD32);
	  if(pCmd_BlkRead32->NoAddressInc) /* fifo-style readout (scope feature) */
	  {
		for(i = 0; i < pCmd_BlkRead32->Count; i++)
		{
		  pCmd_BlkRead32_Rsp->Values[i] = pData[0];
#if DEBUG_REMOTE_CMD
		  printf("{0x%08X => 0x%08X}\n", pCmd_BlkRead32->Address + i*4, pCmd_BlkRead32_Rsp->Values[i]);
#endif
		}
	  }
	  else /* array readout */
	  {
        /* check if specified space is scaler's space; if so, read scaler's image
        from memory space instead - if interval is NOT zero !! */
        readfrommemory = 0;
        if(v1495ScalersReadInterval>0)
        {
          for(i=0; i<v1495scalersnum; i++)
          {
            if( (pCmd_BlkRead32->Address == v1495scalersbegin[i])  && ((pCmd_BlkRead32->Count/4) == v1495scalerslen[i]) )
            {
              sData = (unsigned int *)(v1495scalers[i]);
#if DEBUG_REMOTE_CMD
              printf("found 0x%08x %6d -> read from memory 0x%08x\n",v1495scalersbegin[i],v1495scalerslen[i],pData);
#endif
              for(i=0; i<pCmd_BlkRead32->Count; i++) pCmd_BlkRead32_Rsp->Values[i] = sData[i];
              readfrommemory = 1;
              break;
            }
          }
        }

        /* read from VME space */
        if(readfrommemory==0)
        {
#if DEBUG_REMOTE_CMD
          printf("normal blk32 from 0x%08x, len=%d\n",pData,pCmd_BlkRead32->Count);
#endif
          for(i = pCmd_BlkRead32->Count-1; i >= 0 ; i--)
		  {
		    pCmd_BlkRead32_Rsp->Values[i] = pData[i];
#if DEBUG_REMOTE_CMD
		    printf("{0x%08X => 0x%08X}\n", pCmd_BlkRead32->Address + i*4, pCmd_BlkRead32_Rsp->Values[i]);
#endif
		  }
	    }

	  }


	  return(OutgoingMsg.Length + 4); 

	  break;
	}

	case REMOTE_CMD_BLKWRITE16:

	case REMOTE_CMD_BLKWRITE32:

	default:
	  printf("Unhandled Message Type = 0x%08X\n", pRemoteMsgStruct->MsgType);
	  break;
  }

  return(0);
}





VOID 
v1495tcpServerWorkTask1(int gClientSocket, char *address, unsigned short port) 
{
  STATUS ret;
  int len;  /* number of bytes read */ 
  int size;
  int oldStdErr, oldStdOut;


again:

  if( (len=recv(gClientSocket, (char *)&IncomingMsg, 4, NULL)) == 4)
  {

	/*	
	printf("IncomingMsg: %d(0x%08x)\n",IncomingMsg.Length,IncomingMsg.Length);
	*/ 

	if(IncomingMsg.Length <= sizeof(RemoteMsgStruct) - 4)
	{
	  if( (len=recv(gClientSocket, (char *)&IncomingMsg.BoardType, IncomingMsg.Length, NULL))
            == IncomingMsg.Length)
	  {
		/*
        printf ("FROM CLIENT (Internet Address %s, port %d, length %d): message 0x%02x 0x%02x ..\n", 
	      address, port, len, IncomingMsg.BoardType, IncomingMsg.MsgType);
		*/

        /************************/
        /* Executing the message */

        if(len)
	    {
	      size = ProcessRemoteMsg(&IncomingMsg);
          if(size > 0)
          {
	        /* redirect IO */
            fflush(stderr);
            fflush(stdout);
            oldStdErr = ioTaskStdGet(0, STD_ERR);
            oldStdOut = ioTaskStdGet(0, STD_OUT);
            ioTaskStdSet(0, STD_ERR, gClientSocket);
            ioTaskStdSet(0, STD_OUT, gClientSocket);

	        fwrite(&OutgoingMsg, 1, size, stdout);
	        /*if(send(gClientSocket, (const char *)&OutgoingMsg, size, 0) <= 0) goto sockst()*/

            /* restore IO settings */
            fflush(stderr);
            fflush(stdout);
            ioTaskStdSet(0, STD_ERR, oldStdErr);
            ioTaskStdSet(0, STD_OUT, oldStdOut);
		  }
		  /*
          else goto socket() again ??? 
		  */
          goto again;
	    }

        /************************/
        /************************/

	  }
      else
	  {
        printf("ERROR2: unexpected len=%d\n",len);
	  }
	}
    else
	{
      printf("message too long (%d) - exit\n",IncomingMsg.Length);
	}

    request_in_progress = 0;
  }
  else if(len == ERROR) /* error from recv() */ 
  {
    perror("ERROR (recv)"); 
  }
  else if(len == 0)
  {
    printf("connection closed, exit thread\n");
  }
  else
  {
    printf("ERROR1: unexpected len=%d\n",len);
  }

  ret = close(gClientSocket);    /* close server socket connection */ 
  if(ret==ERROR) printf("ERROR in close()\n");

  free(address); /* free malloc from inet_ntoa() */

  /*
  ret = shutdown(gClientSocket,2);
  if(ret==ERROR) printf("ERROR in shutdown()\n");
  */

  request_in_progress = 0;

  return;
}






STATUS 
v1495tcpServer(int option)
{ 
  struct sockaddr_in serverAddr; 
  struct sockaddr_in clientAddr; 
  int sockAddrSize;              /* size of socket address structure */ 
  int gListenSocket;             /* socket file descriptor */ 
  int gClientSocket;             /* socket descriptor from accept */ 
  int ix = 0;                    /* counter for work task names */ 
  int portnum;                   /* desired port number; can be changed if that number in use enc */
  char workName[16];             /* name of work task */ 
  int on = TRUE;  /* non-blocking */
  int off = FALSE; /* blocking */
  int status;
  MYSQL *dbsock = NULL;
  MYSQL_RES *result;
  int numRows;
  char tmp[1000], temp[100], *myname;
  int itmp;

  /* some cleanup */
  sockAddrSize = sizeof(struct sockaddr_in); 
  bzero((char *)&serverAddr, sockAddrSize); 
  bzero((char *)&clientAddr, sockAddrSize); 


  if(option==1) portnum = VMESERVER_PORT;
  else          portnum = VMESERVER_PORT;


  /* creates an endpoint for communication and returns a socket file descriptor */
  if((gListenSocket = socket(AF_INET, SOCK_STREAM, 0)) == ERROR)
  {
    perror("socket"); 
    return(ERROR); 
  } 

  /* set up the local address */ 
  serverAddr.sin_family = AF_INET; 
  serverAddr.sin_port = htons(portnum); 
  serverAddr.sin_addr.s_addr = htonl(INADDR_ANY); /* create a TCP-based socket */ 

  /* bind socket to local address */
  itmp = portnum;
  while(bind(gListenSocket, (struct sockaddr *)&serverAddr, sockAddrSize) == ERROR)
  {
    perror("bind");

    /* try another port (just increment on one) */
    portnum ++;
    if((portnum-itmp) > 10)
    {
      close(gListenSocket); 
      return(ERROR);
    }

    serverAddr.sin_port = htons(portnum);
  }
  printf("bind on port %d\n",portnum);

  /* create queue for client connection requests */ 
  if(listen (gListenSocket, SERVER_MAX_CONNECTIONS) == ERROR)
  {
    perror ("listen"); 
    close (gListenSocket); 
    return (ERROR);
  }





  /* update database with port number */
  myname = targetName();
  printf("myname >%s<\n",myname);
  /* update daq database 'Ports' table with port number and host name */
  dbsock = dbConnect(getenv("MYSQL_HOST"), "daq");
  /* trying to select our name from 'Ports' table */
  sprintf(tmp,"SELECT Name FROM Ports WHERE Name='%s'",myname);
  if(mysql_query(dbsock, tmp) != 0)
  {
	printf("mysql error (%s)\n",mysql_error(dbsock));
    return(ERROR);
  }
  /* gets results from previous query */
  /* we assume that numRows=0 if our Name does not exist,
     or numRows=1 if it does exist */
  if( !(result = mysql_store_result(dbsock)) )
  {
    printf("ERROR in mysql_store_result (%)\n",mysql_error(dbsock));
    return(ERROR);
  }
  else
  {
    numRows = mysql_num_rows(result);
    mysql_free_result(result);

    /* NOTE: for VXWORKS 'Host' the same as 'Name' */
    /*printf("nrow=%d\n",numRows);*/
    if(numRows == 0)
    {
      sprintf(tmp,"INSERT INTO Ports (Name,Host,Trigger_tcp) VALUES ('%s','%s',%d)",
        myname,myname,portnum);
    }
    else if(numRows == 1)
    {
      sprintf(tmp,"UPDATE Ports SET Host='%s',Trigger_tcp=%d WHERE Name='%s'",myname,portnum,myname);
    }
    else
    {
      printf("ERROR: unknown nrow=%d",numRows);
      return(ERROR);
    }

    if(mysql_query(dbsock, tmp) != 0)
    {
	  printf("ERROR\n");
      return(ERROR);
    }
    else
    {
      printf("Query >%s< succeeded\n",tmp);
    }
  }
  dbDisconnect(dbsock);







  request_in_progress = 0;
  /* accept new connect requests and spawn tasks to process them */ 
  while(1)
  {
    /* check for free net buffers */
    usrNetStackSysPoolStatus("tcpServer",0);
    usrNetStackDataPoolStatus("tcpServer",0);

    /* do not accept new request if current one is not finished yet; too
    many requests may create network buffer shortage */
    if(request_in_progress)
    {
      /*printf("wait: request in progress\n");*/
      taskDelay(100);
      continue;
    }

    if((gClientSocket = accept(gListenSocket, (struct sockaddr *) &clientAddr, &sockAddrSize)) == ERROR)
    {
      perror ("accept"); 
      close (gListenSocket); 
      return (ERROR); 
    }

    sprintf (workName, "v1495Work%d", ix++);
	
    request_in_progress = 1;

    if(option==1)
    {
      if(taskSpawn(workName, SERVER_WORK_PRIORITY, 0, SERVER_STACK_SIZE,
        (FUNCPTR) v1495tcpServerWorkTask1, 
	    gClientSocket, (int) inet_ntoa (clientAddr.sin_addr),
        ntohs (clientAddr.sin_port), 
	    0, 0, 0, 0, 0, 0, 0)
          == ERROR)
      {
	    perror("taskSpawn"); 
	    close(gClientSocket);
        request_in_progress = 0;
      }
	}
    else
	{
      printf("ERROR: unknown tcpServerWorkTask - exit\n");
      return;
	}
	
    /* sleep 1 sec before processing next request; we do not want to process
    too many requests per minute to avoid network buffers shortage */
    taskDelay(100);
  }

} 


v1495BDXTrigger(int ba,int pattern,int multiplicity){
  int offset24=0x0;
  int addr=0x0;
  int pattern_L,pattern_H;
  /* get global offset for A24 */
  sysBusToLocalAdrs(0x39,0,&offset24);
  addr=ba+offset24;

  pattern_L=(pattern&0xFFFF);
  pattern_H=((pattern<<16)&0xFFFF);

  vmeWrite16(addr|PATTERN_REG_1,pattern_L);
  vmeWrite16(addr|PATTERN_REG_2,pattern_H);
  vmeWrite16(addr|MULTIPLICITY_REG,multiplicity);
}

v1495BDXGateWidth(int ba,int width){
  int offset24=0x0;
  int addr=0x0;
   /* get global offset for A24 */
  sysBusToLocalAdrs(0x39,0,&offset24);
  addr=ba+offset24;
  width=width&0xFFFF; /*16 bits*/
  vmeWrite16(addr|DELAY_REG,width);
}

v1495BDXPrescale(int ba,int prescale){
  int offset24=0x0;
  int addr=0x0;
   /* get global offset for A24 */
  sysBusToLocalAdrs(0x39,0,&offset24);
  addr=ba+offset24;
  prescale=prescale&0xFFFF; /*16 bits*/
  vmeWrite16(addr|PRESCALE_REG,prescale);
}

int v1495BDXTriggerConfig(char *filename){
  FILE   *fd;
  char   fname[STRLEN] = { "" };  /* config file name */

  char *getenv();
  char *clonparms;

  int    ii, jj, ch;	
  int ba;
  int gate_width,coinc_level,prescale;
  int this_channel,this_logic;
  int mlogic[16]={0x0};
  int logic=0x0;
	
 
  char   str_tmp[STRLEN], str2[STRLEN], keyword[STRLEN],data[STRLEN],data1[STRLEN],data2[STRLEN];

  ba=0xa00000;
  gate_width=DEFAULT_GATE_WIDTH;
  coinc_level=DEFAULT_COINC_LEVEL;

  clonparms = getenv("CLON_PARMS");




  sprintf(fname, "%s/trigger/%s", clonparms,filename);
  if((fd=fopen(fname,"r")) == NULL)
    {
      printf("\nv1495BDXTriggerConfig: Can't open config file >%s<\n",fname);
      return(-1);
    }
  
  while ((ch = getc(fd)) != EOF){
    if ( ch == '#' || ch == ' ' || ch == '\t' ){
      while (getc(fd) != '\n') {}
    }
    else if( ch == '\n' ) {}
    else{
	 ungetc(ch,fd);
	 fgets(str_tmp, STRLEN, fd); /*Read a line*/
 	 sscanf (str_tmp, "%s %s", keyword, data);
 	/* printf("fgets returns %s so keyword=%s data=%s \n",str_tmp,keyword,data);*/
	if(strcmp(keyword,"GATE_WIDTH") == 0){
		gate_width=atoi(data);
		if (gate_width<0) gate_width=DEFAULT_GATE_WIDTH;
	}	
	else if (strcmp(keyword,"COINC_LEVEL") == 0){
		coinc_level=atoi(data);
		if ((coinc_level<1)||(coinc_level>16)) coinc_level=DEFAULT_COINC_LEVEL;
	}
	else if (strcmp(keyword,"PRESCALE") == 0){
		prescale=atoi(data);
		if ((prescale<0)||(prescale>100000)) prescale=DEFAULT_PRESCALE;
	}
	else if (strcmp(keyword,"CHANNEL_LOGIC") == 0){
 		sscanf (str_tmp,"%s %s %s",keyword,data1,data2);
	  
		this_channel=atoi(data1);
		this_logic=atoi(data2);
		if ((this_channel>=0)&&(this_channel<16)) mlogic[this_channel]=this_logic;
	}	
	}
  }

  fclose(fd);

  for (ii=0;ii<16;ii++){
	if (mlogic[ii]!=0) logic=logic|(1<<ii);
  }

  printf("v1495BDXTriggerConfig\n");
  printf("Gate width is %i\n",gate_width);
  printf("Coinc level is %i\n",coinc_level);
  printf("Prescale is %i\n",prescale);
  printf("Channels logic \n CHANNEL \t LOGIC \n");
  for (ii=0;ii<16;ii++){
	if (mlogic[ii]==0) printf(" %i \t \t AND \n",ii);
	else printf(" %i \t \t OR \n",ii);
  }
  printf("Channels logic: %x\n",logic);

 v1495BDXTrigger(ba,logic,coinc_level);
 v1495BDXGateWidth(ba,gate_width);
 v1495BDXPrescale(ba,prescale);



}


/*******************************************************************************
 *
 * vmeRead16         - Read a 16bit register
 * vmeWrite16        - Write to a 16bit register
 * vmeRead32         - Read a 32bit register
 * vmeWrite32        - Write to a 16bit register
 *
 */

static unsigned short
vmeRead16(volatile unsigned short *addr)
{
  unsigned short rval;
  rval = *addr;
#ifndef VXWORKS
  rval = SSWAP(rval);
#endif
  return rval;
}

static void
vmeWrite16(volatile unsigned short *addr, unsigned short val)
{
#ifndef VXWORKS
  val = SSWAP(val);
#endif
  *addr = val;
  return;
}

static unsigned int
vmeRead32(volatile unsigned int *addr)
{
  unsigned int rval;
  rval = *addr;
#ifndef VXWORKS
  rval = LSWAP(rval);
#endif
  return rval;
}

static void
vmeWrite32(volatile unsigned int *addr, unsigned int val)
{
#ifndef VXWORKS
  val = LSWAP(val);
#endif
  *addr = val;
  return;
}








#else /* no UNIX version */

void
v1495_dummy()
{
  return;
}

#endif
