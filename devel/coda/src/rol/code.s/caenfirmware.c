
/************************************************************************

    CAEN firmware upgrade

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


/* flash memory accesses adresses & opcodes */

#define MAIN_MEM_PAGE_READ         0x00D2
#define MAIN_MEM_PAGE_PROG_TH_BUF1 0x0082
/*#define PAGE_ERASE                 0x0081*/


/* modules config info */

typedef struct caenfirmware_config
{
  char *MODEL;
  int  SEL_FLASH;
  int  RW_FLASH;
  int  REG_SIZE;
  int  PAGE_SIZE;
  int  FIRST_PAGE_STD;
  int  FIRST_PAGE_BCK;
  int  FLASH_ENABLE;
} CAENFIRMWARE;

CAENFIRMWARE config[] = {"V1190",
                          0x1032,
						  0x1034,
						  2,
						  264,
						  768,
						  1408,
						  0,

						  "V1495USR",
						  0x8012,
						  0x8014,
						  2,
						  264,
						  48,
						  1048,
						  0,

						  "V1495VME",
						  0x800E,
						  0x8010,
						  2,
						  264,
						  768,
						  1408,
						  0,

						  "DIGITIZERS",
						  0xEF2C,
						  0xEF30,
						  4,
						  264,
						  48,
						  2072,
						  0
};

CAENFIRMWARE *conf;


/********************/
/* write flash page */

void
writeFlashPage16(unsigned int addr, unsigned char *page, unsigned int pagenum)
{
  unsigned short *Sel_Flash = (unsigned short *)(addr+conf->SEL_FLASH);
  unsigned short *RW_Flash = (unsigned short *)(addr+conf->RW_FLASH);
  unsigned int flash_addr, data;
  unsigned char addr0, addr1, addr2;
  unsigned short i;

  EIEIO;
  SYNC;

  if(conf->PAGE_SIZE==264) flash_addr = pagenum << 9; /* 4 and 8 Mbit */
  else if(conf->PAGE_SIZE==528) flash_addr = pagenum << 10; /* 16 and 32 Mbit */
  else flash_addr = pagenum << 11; /* 64 Mbit */

  addr0 = (unsigned char) (flash_addr&0xFF);
  addr1 = (unsigned char) ((flash_addr>>8)&0xFF);
  addr2 = (unsigned char) ((flash_addr>>16)&0xFF);

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
  for(i=0; i<conf->PAGE_SIZE; i++)
  {
    data = page[i];
	/*if(i<5) printf("write: data[%3d]=0x%08x\n",i,data);*/
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
  taskDelay(10);
  EIEIO;
  SYNC;

  return;
}

void
writeFlashPage32(unsigned int addr, unsigned char *page, unsigned int pagenum)
{
  unsigned int *Sel_Flash = (unsigned int *)(addr+conf->SEL_FLASH);
  unsigned int *RW_Flash = (unsigned int *)(addr+conf->RW_FLASH);
  unsigned int flash_addr, data;
  unsigned char addr0, addr1, addr2;
  unsigned short i;

  EIEIO;
  SYNC;

  if(conf->PAGE_SIZE==264) flash_addr = pagenum << 9; /* 4 and 8 Mbit */
  else if(conf->PAGE_SIZE==528) flash_addr = pagenum << 10; /* 16 and 32 Mbit */
  else flash_addr = pagenum << 11; /* 64 Mbit */

  addr0 = (unsigned char) (flash_addr&0xFF);
  addr1 = (unsigned char) ((flash_addr>>8)&0xFF);
  addr2 = (unsigned char) ((flash_addr>>16)&0xFF);

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
  for(i=0; i<conf->PAGE_SIZE; i++)
  {
    data = page[i];
	/*if(i<5) printf("write: data[%3d]=0x%08x\n",i,data);*/
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
  taskDelay(10);
  EIEIO;
  SYNC;

  return;
}


/*******************/
/* read flash page */

void
readFlashPage16(unsigned int addr, unsigned char *page, unsigned int pagenum)
{
  unsigned short *Sel_Flash = (unsigned short *)(addr+conf->SEL_FLASH);
  unsigned short *RW_Flash = (unsigned short *)(addr+conf->RW_FLASH);
  unsigned int flash_addr, data;
  unsigned char addr0, addr1, addr2;
  unsigned short i, data16;
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
  for(i=0; i<conf->PAGE_SIZE; i++)
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

  return;
}

void
readFlashPage32(unsigned int addr, unsigned char *page, unsigned int pagenum)
{
  unsigned int *Sel_Flash = (unsigned int *)(addr+conf->SEL_FLASH);
  unsigned int *RW_Flash = (unsigned int *)(addr+conf->RW_FLASH);
  unsigned int flash_addr, data;
  unsigned char addr0, addr1, addr2;
  unsigned short i;
  unsigned int data32;
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
  for(i=0; i<conf->PAGE_SIZE; i++)
  {
    data32 = *RW_Flash;
    page[i] = (unsigned char)data32;
  }

  EIEIO;
  SYNC;

  /* disable flash (NCS = 1) */
  data = 1;
  *Sel_Flash = data;
  EIEIO;
  SYNC;

  return;
}



/*

  example:

ld < /usr/local/clas/devel/coda/src/rol/VXWORKS_ppc/obj/caenfirmware.o
cd "/usr/local/clas/devel/coda/src/rol/code.s"

5100:

caenfirmware("V1190", 0xfa210000,"v1190core0.6.rbf",0)

6100:

caenfirmware("V1190", 0x90210000,"v1190core0.6.rbf",0)
caenfirmware("DIGITIZERS", 0x90110000,"v1720_rev_3.0_0.9.rbf",0)

*/


/*****************************************************************************/
/* MAIN
/*****************************************************************************/
int
caenfirmware(char *model, unsigned int baseaddr, char *filename, int page)
{
  int finish, i;
  int pp, bp, bcnt;
  char c;
  unsigned char pdw[/*PAGE_SIZE*/1024], pdr[/*PAGE_SIZE*/1024];
  FILE *cf;

  /*board config information*/
  if(      !strcmp(model,"V1190") )      conf = &config[0];
  else if( !strcmp(model,"V1495USR") )   conf = &config[1];
  else if( !strcmp(model,"V1495VME") )   conf = &config[2];
  else if( !strcmp(model,"DIGITIZERS") ) conf = &config[3];
  else
  {
    printf("Unknown model >%s< - exit\n",model);
  }

  printf("\nUpdate firmware for model >%s< with following params:\n",model);
  printf("0x%04x 0x%04x %d %d %d %d %d\n",conf->SEL_FLASH,conf->RW_FLASH,
		 conf->REG_SIZE,conf->PAGE_SIZE,conf->FIRST_PAGE_STD,
         conf->FIRST_PAGE_BCK,conf->FLASH_ENABLE);
  printf("Base address 0x%08x, filename >%s<, page=%d\n\n",
         baseaddr, filename, page);

  /* open binary firmware file */
  cf = fopen(filename,"rb");
  if(cf == NULL)
  {
    printf("\n\nCan't open firmware file >%s< - exit\n",filename);
    exit(0);
  }

  printf("\nLoading binary firmware file >%s<\n",filename);
  if(page == 0)
  {
    printf("Writing copy STD of the firmware\n");
    pp = conf->FIRST_PAGE_STD; /* page pointer */
  }
  else if(page == 1)
  {
    printf("Writing copy BCK of the firmware\n");
    pp = conf->FIRST_PAGE_BCK; /* page pointer */
  }
  else
  {
    printf("Bad Image.\n");
    exit(0);
  }

  bcnt=0;            /* byte counter */
  bp = 0;            /* byte pointer in the page */
  finish = 0;        /* it goes high when eof is found */

  /* while loop */
  while(!finish)
  {
    c = (unsigned char) fgetc(cf);  /* read one byte from file */
    /* mirror byte (lsb becomes msb) */
    pdw[bp] = 0;
    for(i=0; i<8; i++)
    {
      if(c & (1<<i))
      {
        pdw[bp] = pdw[bp] | (0x80>>i);
      }
    }
	/*		
    printf("[%3d] from file 0x%02x, mirrored 0x%02x\n",bp,pdw[bp]);
	*/
    bp++;
    bcnt++;
    if(feof(cf))
    {
      printf("End of file: bp=%d bcnt=%d\n",bp,bcnt);
      finish = 1;
	}

    /* write and verify a page */
    if((bp==conf->PAGE_SIZE) || finish)
    {
      if(conf->REG_SIZE==2)
	  {
        writeFlashPage16(baseaddr, pdw, pp);
        readFlashPage16(baseaddr, pdr, pp);
	  }
	  else
	  {
        writeFlashPage32(baseaddr, pdw, pp);
        readFlashPage32(baseaddr, pdr, pp);
	  }

      for(i=0; i<conf->PAGE_SIZE; i++)       /* verify page */
      {
        if(pdr[i] != pdw[i])
        {
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
printf("page: bcnt=%d n",bcnt);
	  */
      bp = 0;
      pp ++;
    }


  }  /* end of while loop */


  fclose(cf);

  printf("\nFirmware loaded without errors. Written %d bytes\n",bcnt);

  exit(0);
}


#else /* no UNIX version */

void
caenfirmware_dummy()
{
  return;
}

#endif
