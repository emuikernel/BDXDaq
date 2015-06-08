
/************************************************************************

  CCONF: CAEN Load FPGA Configuration File

  This program writes the configuration file (Altera RBF Format) into one
  of the two pages (STD and BCK) of the flash memory in the CAEN's modules.
  The flash memory is written via VME.

*************************************************************************/

#ifdef VXWORKS

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <vxWorks.h>

/*sergey*/
#define EIEIO    __asm__ volatile ("eieio")
#define SYNC     __asm__ volatile ("sync")

#include "tdc890.h"


/* flash memory accesses adresses & opcodes */

#define MAIN_MEM_PAGE_READ         0x00D2
#define MAIN_MEM_PAGE_PROG_TH_BUF1 0x0082
/*#define PAGE_ERASE                 0x0081*/



/* write flash page */

void
write_flash_page(unsigned int addr, unsigned char *page, unsigned int pagenum)
{
  volatile TDC890 *tdc890 = (TDC890 *) addr;
  unsigned int flash_addr, data;
  unsigned char addr0, addr1, addr2;
  unsigned short i;

  EIEIO;
  SYNC;
  flash_addr = pagenum << 9;
  addr0 = (unsigned char) flash_addr;
  addr1 = (unsigned char) (flash_addr>>8);
  addr2 = (unsigned char) (flash_addr>>16);

  EIEIO;
  SYNC;
  /* enable flash (NCS = 0) */
  data = 0;
  tdc890->selflash = data;

  EIEIO;
  SYNC;
  /* write opcode */
  data = MAIN_MEM_PAGE_PROG_TH_BUF1;
  tdc890->flash = data;

  EIEIO;
  SYNC;
  /* write address */
  data = addr2;
  tdc890->flash = data;
  data = addr1;
  tdc890->flash = data;
  data = addr0;
  tdc890->flash = data;

  EIEIO;
  SYNC;
  /* write flash page (264 bytes for page, 256 used) */
  for(i=0; i<264; i++)
  {
    data = page[i];
	/*if(i<5) printf("write: data[%3d]=0x%08x\n",i,data);*/
    tdc890->flash = data;
  }

  EIEIO;
  SYNC;
  /* wait 20ms (max time required by the flash to complete the writing) */
  taskDelay(10); /* 1 tick = 10ms */

  EIEIO;
  SYNC;
  /* disable flash (NCS = 1) */
  data = 1;
  tdc890->selflash = data;

  EIEIO;
  SYNC;
  /* wait 20ms (max time required by the flash to complete the writing) */
  taskDelay(10);
  EIEIO;
  SYNC;

  return;
}


/* read flash page */

void
read_flash_page(unsigned int addr, unsigned char *page, unsigned int pagenum)
{
  volatile TDC890 *tdc890 = (TDC890 *) addr;
  unsigned int flash_addr, data;
  unsigned char addr0, addr1, addr2;
  unsigned short i;
  volatile short *flashptr;

  EIEIO;
  SYNC;
  flash_addr = pagenum << 9;
  addr0 = (unsigned char) flash_addr;
  addr1 = (unsigned char) (flash_addr>>8);
  addr2 = (unsigned char) (flash_addr>>16);

  EIEIO;
  SYNC;
  /* enable flash (NCS = 0) */
  data = 0;
  tdc890->selflash = data;

  EIEIO;
  SYNC;
  /* write opcode */
  data = MAIN_MEM_PAGE_READ;
  tdc890->flash = data;

  EIEIO;
  SYNC;
  /* write address */
  data = addr2;
  tdc890->flash = data;
  data = addr1;
  tdc890->flash = data;
  data = addr0;
  tdc890->flash = data;

  EIEIO;
  SYNC;
  /* additional don't care bytes */
  data = 0;
  for(i=0; i<4; i++)
  {
    tdc890->flash = data;
  }

  /* read flash page (264 bytes for page, 256 used) */
  /* D32 readout ! - wrong !!!!???????? */
  EIEIO;
  SYNC;

  flashptr = (short *)&(tdc890->flash);
  printf("->flashptr=0x%08x\n",flashptr); /* does not read correctly without print ??!! */
  EIEIO;
  SYNC;

  for(i=0; i<264; i++)
  {

  EIEIO;
  SYNC;
    data = *flashptr/*tdc890->flash*/;
  EIEIO;
  SYNC;

	/*if(i<5) printf("read: data[%3d]=0x%08x\n",i,data);*/
    page[i] = data;
  }
  EIEIO;
  SYNC;

  /* disable flash (NCS = 1) */
  data = 1;
  tdc890->selflash = data;
  EIEIO;
  SYNC;

  return;
}



/*

  example:

ld < /usr/local/clas/devel/coda/src/rol/VXWORKS_ppc/obj/tdc1190firmware.o
cd "/usr/local/clas/devel/coda/src/rol/code.s"

5100:

tcd1190firmware(0xfa210000,"v1190core0.5.rbf",0)
tcd1190firmware(0xfa220000,"v1190core0.5.rbf",0)
tcd1190firmware(0xfa230000,"v1190core0.5.rbf",0)
tcd1190firmware(0xfa240000,"v1190core0.5.rbf",0)
tcd1190firmware(0xfa250000,"v1190core0.5.rbf",0)
tcd1190firmware(0xfa260000,"v1190core0.5.rbf",0)
tcd1190firmware(0xfa270000,"v1190core0.5.rbf",0)
tcd1190firmware(0xfa280000,"v1190core0.5.rbf",0)

tcd1190firmware(0xfa210000,"v1190core0.6.rbf",0)
tcd1190firmware(0xfa220000,"v1190core0.6.rbf",0)
tcd1190firmware(0xfa230000,"v1190core0.6.rbf",0)
tcd1190firmware(0xfa240000,"v1190core0.6.rbf",0)
tcd1190firmware(0xfa250000,"v1190core0.6.rbf",0)
tcd1190firmware(0xfa260000,"v1190core0.6.rbf",0)
tcd1190firmware(0xfa270000,"v1190core0.6.rbf",0)
tcd1190firmware(0xfa280000,"v1190core0.6.rbf",0)

6100:

tcd1190firmware(0x90210000,"v1190core0.5.rbf",0)
tcd1190firmware(0x90220000,"v1190core0.5.rbf",0)
tcd1190firmware(0x90230000,"v1190core0.5.rbf",0)
tcd1190firmware(0x90240000,"v1190core0.5.rbf",0)
tcd1190firmware(0x90250000,"v1190core0.5.rbf",0)
tcd1190firmware(0x90260000,"v1190core0.5.rbf",0)
tcd1190firmware(0x90270000,"v1190core0.5.rbf",0)
tcd1190firmware(0x90280000,"v1190core0.5.rbf",0)

tcd1190firmware(0x90210000,"v1190core0.6.rbf",0)
tcd1190firmware(0x90220000,"v1190core0.6.rbf",0)
tcd1190firmware(0x90230000,"v1190core0.6.rbf",0)
tcd1190firmware(0x90240000,"v1190core0.6.rbf",0)
tcd1190firmware(0x90250000,"v1190core0.6.rbf",0)
tcd1190firmware(0x90260000,"v1190core0.6.rbf",0)
tcd1190firmware(0x90270000,"v1190core0.6.rbf",0)
tcd1190firmware(0x90280000,"v1190core0.6.rbf",0)

*/


#define FIRST_PAGE_STD    768    /* first page of the copy STD */
#define FIRST_PAGE_BCK    1408   /* first page of the copy BCK */


/*****************************************************************************/
/* MAIN
/*****************************************************************************/
int
tcd1190firmware(unsigned int baseaddr, char *filename, int page)
{
  int finish, i;
  int pp, bp, bcnt, pcnt;
  char c;
  unsigned char pin[264], pdw[264], pdr[264];
  FILE *cf;

page = 0; /* ONLY STD !!!!!!!!!!!!! */

  printf("\n");
  printf("********************************************************\n");
  printf("* CAEN SpA - Front-End Division                        *\n");
  printf("* ---------------------------------------------------- *\n");
  printf("* ISP Write Flash for FPGA configuration               *\n");
  printf("* Version 1.0 (28/07/04)                               *\n");
  printf("*   Sergey Boyarinov: CLAS version 19-Oct-2004         *\n");
  printf("********************************************************\n");

  /* open the configuration file */
  cf = fopen(filename,"rb");
  if(cf == NULL)
  {
    printf("\n\nCan't open tdc1190 firmware file >%s< - exit\n",filename);
    exit(0);
  }

  printf("\nLoading file >%s<\n",filename);
  if(page == 0)
  {
    printf("Writing copy STD of the firmware\n");
  }
  else
  {
    printf("Writing copy BCK of the firmware\n");
  }

  pcnt = 0;          /* page counter */
  bcnt=0;            /* byte counter */
  if(page == 0)
  {
    pp = FIRST_PAGE_STD; /* page pointer */
  }
  else
  {
    pp = FIRST_PAGE_BCK; /* page pointer */
  }

  bp = 0;              /* byte pointer in the page */
  finish = 0;          /* it goes high when eof is found */

  /* while loop */
  while(!finish)
  {
    c = (unsigned char) fgetc(cf);  /* read one byte from file */
    /* mirror byte (lsb becomes msb) */
    pin[bp] = c;
    pdw[bp] = 0;
    for(i=0; i<8; i++)
    {
      if(c & (1<<i))
      {
        pdw[bp] = pdw[bp] | (0x80>>i);
      }
    }
	/*		
    printf("[%3d] from file 0x%02x, mirrored 0x%02x\n",bp,pin[bp],pdw[bp]);
	*/
    bp++;
    bcnt++;
    if(feof(cf))
    {
      printf("End of file: bp=%d bcnt=%d\n",bp,bcnt);
      finish = 1;
	}

    /* write and verify a page */
    if((bp==264) || finish)
    {
      write_flash_page(baseaddr, pdw, pp); /* write page */
      read_flash_page(baseaddr, pdr, pp);  /* read page */
      for(i=0; i<bp; i++)       /* verify page */
      {
        if(pdr[i] != pdw[i])
        {
          ;
		  
          printf("[%3d] written 0x%02x, read back 0x%02x",i,pdw[i],pdr[i]);
          printf(" -> Flash writing failure !!!\n");

		  printf("\nAbort firmware loading !\n");
          exit(0);

        }
        else
        {
          ;
		  /*
          printf("[%3d] written 0x%02x, read back 0x%02x",i,pdw[i],pdr[i]);
          printf("\n");
		  */
        }
	  }
	  /*
printf("page: bcnt=%d pcnt=%d\n",bcnt,pcnt);
	  */
      bp = 0;
      pp ++;
      pcnt ++;
    }


  }  /* end of while loop */


  fclose(cf);

  printf("\nFirmware loaded without errors. Written %d bytes\n",bcnt);

  exit(0);
}




tdc1190firmware_test()
{
  unsigned int baseaddr = 0xfa210000;
  unsigned char pdr[264];

  read_flash_page(baseaddr, pdr, FIRST_PAGE_STD);  /* read page */
}



#else /* no UNIX version */

void
tdc1190firmware_dummy()
{
  return;
}

#endif
