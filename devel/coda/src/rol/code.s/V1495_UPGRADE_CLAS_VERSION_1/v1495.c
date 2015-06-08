/*
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
  taskDelay(10); /* 1 tick = 10ms */

  EIEIO;
  SYNC;
  /* disable flash (NCS = 1) */
  data = 1;
  *Sel_Flash = data;


  EIEIO;
  SYNC;
  /* wait 20ms (max time required by the flash to complete the writing) */
  taskDelay(20);
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

v1495firmware(0x80510000,"V1495USER1.0.rbf",0,0)

v1495firmware(0x80510000,"V1495vme01.rbf",0,1)

*/


int
v1495test1()
{
  volatile V1495 *v1495 = (V1495 *) 0x80510000;
  unsigned short *data16 = &(v1495->control);

  printf("Control      [0x%08x] = 0x%04x\n",&(v1495->control),v1495->control);
  printf("firmwareRev  [0x%08x] = 0x%04x\n",&(v1495->firmwareRev),v1495->firmwareRev);
  printf("selflashVME  [0x%08x] = 0x%04x\n",&(v1495->selflashVME),v1495->selflashVME);
  printf("flashVME     [0x%08x] = 0x%04x\n",&(v1495->flashVME),v1495->flashVME);
  printf("selflashUSER [0x%08x] = 0x%04x\n",&(v1495->selflashUSER),v1495->selflashUSER);
  printf("flashUSER    [0x%08x] = 0x%04x\n",&(v1495->flashUSER),v1495->flashUSER);
  printf("configROM    [0x%08x] = 0x%04x\n",&(v1495->configROM[0]),v1495->configROM[0]);

  return(0);
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
  int finish,i;
  int bp, bcnt, pa;
  char c;
  unsigned char pdw[PAGE_SIZE], pdr[PAGE_SIZE];
  unsigned long vboard_base_address;
  FILE *cf;

page = 0;     /* ONLY STD !!!!!!!!!!!!! */
user_vme = 0; /* ONLY USER !!!!!!!!!!!! */

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
  printf("Write 1 at address 0x8016 to reload updated version of the User FPGA\n");

  exit(0);
}


#else /* no UNIX version */

void
v1495_dummy()
{
  return;
}

#endif
