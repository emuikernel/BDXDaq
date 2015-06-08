/* lnIsa_eeprom.c - AMD Am79C961 PCnet-ISA+ Ethernet device's EEPROM 
   programming and Plug n' Play utilities, The EEPROM is a 93C56 device */

/*
modification history
--------------------
01a,20Oct96,joes first implementation
*/

/*
 * Note: Reading or Writing the EEPROM halts operation of the ethernet device.
 * 	 DO NOT USE THESE FUNCTIONS IF LOGGED IN OVER THE NETWORK.
 *	Call lnIsa_restart(pDrvCtrl) to restart the device after updating
 *	the EEPROM.
 */ 

#include "vxWorks.h"
#include "config.h"
#include "sysLib.h"
#include "stdlib.h"
#include "taskLib.h"
#include "logLib.h"
#include "intLib.h"
#include "netLib.h"
#include "stdio.h"
#include "arpa/inet.h" /* for inet_addr */

IMPORT UINT16 sysInWord (int address);
IMPORT void sysOutWord (int address, UINT16 data);
IMPORT UINT8 lnIsa_eedata [];

/* Function Prototypes */
STATUS 	lnIsa_eeprog(DRV_CTRL *pDrvCtrl, UINT16 hiword, UINT16 midword, 
#ifdef IP_ADDR_STORED_IN_EEPROM
		     UINT16 loword, char *ipStr);
#else
		     UINT16 loword);
#endif
STATUS 	lnIsa_eewrite(DRV_CTRL *pDrvCtrl, UINT8 *data, int nwords);
void 	lnIsa_eeread(DRV_CTRL *pDrvCtrl, int nwords);
STATUS 	lnIsa_eewrite16(DRV_CTRL *pDrvCtrl, UINT8 wordAddr, 
				UINT16 data16);
UINT16 	lnIsa_eeread16(DRV_CTRL *pDrvCtrl, UINT8 wordAddr);
LOCAL void 	lnIsa_eewen(DRV_CTRL *pDrvCtrl);
LOCAL void 	lnIsa_eewds(DRV_CTRL *pDrvCtrl);
void 		lnIsa_eeeraseall(DRV_CTRL *pDrvCtrl);
LOCAL void 	lnIsa_eewriteBit(DRV_CTRL *pDrvCtrl, int bit);
LOCAL int 	lnIsa_eereadBit(DRV_CTRL *pDrvCtrl);
LOCAL void 	lnIsa_eetimeout(DRV_CTRL *pDrvCtrl);

#define WR_EE_REG(x) lnIdpWrite(pDrvCtrl,ISACSR3,x) /* ISACSR3 is the EE_REG */
#define RD_EE_REG    lnIdpRead(pDrvCtrl, ISACSR3)
#define EE_TIMEOUT   lnIsa_eetimeout(pDrvCtrl)

/*
 * lnIsa_eeprog - program the AMD961's ethernet address into the 
 * 	          EEPROM along with all the required plug and play 
 *		  information required to initialise the device.
 */
STATUS lnIsa_eeprog(DRV_CTRL *pDrvCtrl, UINT16 hiword, UINT16 midword, 
#ifdef IP_ADDR_STORED_IN_EEPROM
		    UINT16 loword, char *ipStr)
#else
		    UINT16 loword)
#endif
{
  STATUS stat;
  UINT16 *etherData = (UINT16 *)&lnIsa_eedata[0];
  UINT32 ipAddr;
#ifndef IP_ADDR_STORED_IN_EEPROM
  char *ipStr = NULL;
#endif
  
  if(((hiword & ENET_MASK_HI) != ENET_VALUE_HI) ||
     ((midword & ENET_MASK_MID) != ENET_VALUE_MID) ||
     ((loword & ENET_MASK_LOW) != ENET_VALUE_LOW))
  {
    printf("Invalid Ethernet Address\n");
    return ERROR;
  }

  if(ipStr == NULL)
    ipAddr = 0xffffffff; /* set it to an error value */
  else
    ipAddr = inet_addr(ipStr);

  etherData[0] = hiword;
  etherData[1] = midword;
  etherData[2] = loword;
  etherData[EE_WORD_LEN_MIN] = (UINT16)(ipAddr & 0xffff);
  etherData[EE_WORD_LEN_MIN+1] = (UINT16)(ipAddr >> 16);


  stat = lnIsa_eewrite(pDrvCtrl, lnIsa_eedata, EE_WORD_LEN_MIN+2);
  if(stat == OK)
    printf("Reboot pack for update to take effect\n");
  else
    printf("Ethernet EEPROM update failed\n");

  return stat;
}

/*
 * lnIsa_eewrite - write nwords of data to the EEPROM.
 * Each 16 bit word is written in bit-reversed order, i.e., bit 15 first 
 * followed by bit 14 and so on down to bit 0.
 * An 8 bit checksum is generated for the complete data block (52 bytes)
 * and written to the device.
 * The Ethernet device requires that the first 0x1A words contains a valid 
 * checksum.
 * The layout of this area of the EEPROM is as follows:
 *
 *   Word No.	-------------------------------------
 *	0	| Enet Address 1  | Enet Address 0  |
 *		-------------------------------------
 *	1	| Enet Address 3  | Enet Address 2  |
 *		-------------------------------------
 *	2	| Enet Address 5  | Enet Address 4  | 
 *		-------------------------------------
 *	3	|    Reserved     |    Reserved     |
 *		-------------------------------------
 *	4	|   HWID (01H)    |    Reserved     |
 *		-------------------------------------
 *	5	|           User Space 1            |
 *		-------------------------------------
 *	6	|        16 Bit Checksum 1          |
 *		-------------------------------------
 *	7	| ASCII W (0x57)  | ASCII W (0x57)  |
 *		-------------------------------------
 *	8	|   EISA Byte 1   |   EISA Byte 0   |
 *		-------------------------------------
 *	9	|   EISA Byte 3   |   EISA Byte 2   |
 *		-------------------------------------
 *	A	|          MSRDA, ISACSR0           |
 *		------------------------------------- 
 *	B	|          MSRDA, ISACSR1           |
 *		------------------------------------- 
 *	C	|       MISC Config, ISACSR2        |
 *		------------------------------------- 
 *	D	|       LED1 Config, ISACSR5        |
 *		------------------------------------- 
 *	E	|       LED2 Config, ISACSR6        |
 *		------------------------------------- 
 *	F	|       LED3 Config, ISACSR7        |
 *		------------------------------------- 
 *	10	|     PnP 0x61    |     PnP 0x60    |
 *		-------------------------------------
 *	11	|     PnP 0x71    |     PnP 0x70    |
 *		-------------------------------------
 *	12	|      Unused     |     PnP 0x74    |
 *		-------------------------------------
 *	13	|     PnP 0x41    |     PnP 0x40    |
 *		-------------------------------------
 *	14	|     PnP 0x43    |     PnP 0x42    |
 *		-------------------------------------
 *	15	|      Unused     |     PnP 0x44    |
 *		-------------------------------------
 *	16	|     PnP 0x49    |     PnP 0x48    |
 *		-------------------------------------
 *	17	|     PnP 0x4b    |     PnP 0x4A    |
 *		-------------------------------------
 *	18	|      Unused     |     PnP 0x4c    |
 *		-------------------------------------
 *	19	| 8bit Checksum 2 |     PnP 0xF0    |
 *		-------------------------------------
 *	1A	|       External Shift Chain        |	<-- last word in 
 *		-------------------------------------       Checksum 2 area.
 *	1B	|  (Optional) IP Address Low Word   |
 *		-------------------------------------
 *	1C	|  (Optional) IP Address High Word  |
 *		-------------------------------------
 *	1B	|                                   |
 *	 :	|                                   |
 *	1F	|        Unused Locations           |
 *		-------------------------------------
 *	20      |  Plug and Play Starting Location  |
 *		-------------------------------------
 *
 *  Checksum 1 is calculated on Words 0 through 5 plus word 7. 
 *  Checksum 2 is calculated on the first 54 bytes (words 0 through 0x1A). 
 *  Both checksums are the one's complement of the checksummed data , i.e.,
 *  summing words 0 through 7 with a valid checksum yields 0xFFFF and summing
 *  the first 54 bytes with a valid checksum yields 0xFF.
 *
 *  The data area after Word 0x1A is used for Plug and Play Information. It
 *  is not required by the AMD961 and is not supported by this driver. 
 *
 * lnIsa_eewrite automatically calculates and inserts checksums 1 and 2. It can
 * also write more than the first 54 bytes. However any further checksum fields
 * must be calculated and inserted into the data block before calling 
 * lnIsa_eewrite. The updated EEPROM will not be recognised until after the 
 * board is reset.
 */

/* EEPROM data filled in with config.h defined paramters, and placeholders for
   the dynamic fields, like IEEE address */
UINT8 lnIsa_eedata [] = { EE_PAD, 	EE_PAD, /* IEEE Address Place holder */
			  EE_PAD, 	EE_PAD, /* IEEE Address Place holder */
			  EE_PAD, 	EE_PAD, /* IEEE Address Place holder */
			  EE_PAD, 	EE_PAD,	    /* Pad Bytes */
			  EE_PAD, 	0x01,	    /* HW ID */
			  EE_PAD, 	EE_PAD,
			  EE_PAD, 	EE_PAD,
			  0x57, 	0x57,	    /* ASCII W */
			  EISA0,	EISA1,	    /* EISA Config Registers */
			  EISA2,	EISA3,
			  ISACSR0_LO,	ISACSR0_HI, /* Internal Registers */
			  ISACSR1_LO,	ISACSR1_HI,
			  ISACSR2_LO,	ISACSR2_HI,
			  ISACSR5_LO,	ISACSR5_HI,
			  ISACSR6_LO,	ISACSR6_HI,
			  ISACSR7_LO,	ISACSR7_HI,
			  PNP_60,	PNP_61,
			  PNP_70,	PNP_71,
			  PNP_74,	EE_PAD,
			  PNP_40,	PNP_41,
			  PNP_42,	PNP_43,
			  EE_PAD,	PNP_44,
			  PNP_48,	PNP_49,
			  PNP_4A,	PNP_4B,
			  EE_PAD,	PNP_4C,
			  EE_CHKSUM,	PNP_F0,
			  EXT_SHIFT0,	EXT_SHIFT1,
			  EE_PAD,	EE_PAD,
			  EE_PAD,	EE_PAD
}; /* end of lnIsa_eedata */


STATUS lnIsa_eewrite(DRV_CTRL *pDrvCtrl, UINT8 *data, int nwords)
{
  int i;
  UINT8 sum8;
  UINT16 sum16;
  UINT16 *data16 = (UINT16 *)data;
  STATUS stat = OK;


  /* check we have enough data */
  if(nwords < EE_WORD_LEN_MIN)
  {
#ifdef LNISA_DEBUG
    printf("eewrite: Not Enough Data, Min = 0x1B words\n");
#endif
    return ERROR;
  }

  /* check for too much data */
  if(nwords > EE_WORD_LEN_MAX)
  {
#ifdef LNISA_DEBUG
    printf("eewrite: Too Much data, Max = %d words\n",EE_WORD_LEN_MAX);
#endif
    return ERROR;
  }

  lnCsrWrite(pDrvCtrl,0,4); /* place the device in the STOP state */

  /* generate checksum 1:  Words 0 through 7 */
  data16[EE_CHKSUM1_OFFSET] = 0; /* clear checksum area */
  sum16 = 0;
  for(i = 0; i < 8; i++)
    sum16 += data16[i];
  data16[EE_CHKSUM1_OFFSET] = ~sum16;

#ifdef LNISA_DEBUG
  printf("checksum 1 = %04x\n",data16[EE_CHKSUM1_OFFSET]);
#endif
  
  /* 
   * generate checksum 2: Bytes 0 through 53.
   */
  sum8 = 0;
  data[EE_CHKSUM2_OFFSET] = 0; 	/* clear chksum byte down */
  for(i = 0; i < 54; i++)
    sum8 += data[i];
  sum8 = (~sum8); 		/* ones's complement */
  data[EE_CHKSUM2_OFFSET] = sum8;

#ifdef LNISA_DEBUG
  printf("Checksum 2 = %02x\n",sum8);
#endif
  
  WR_EE_REG(EE_EN);		/* enable the EEPROM interface */
  EE_TIMEOUT;
  lnIsa_eewen(pDrvCtrl);	/* Enable EEPROM writes */
  
  for(i = 0; i < nwords; i++)
  {
    stat = lnIsa_eewrite16(pDrvCtrl, (UINT8)i,*(UINT16 *)(&data[i*2]));
    if(stat == ERROR)
    {
      printf("Error writing %04x to location %d\n",*(UINT16 *)(&data[i*2]),i);
      break;
    }
  }

  lnIsa_eewds(pDrvCtrl); /* disable writes to EEPROM */

  WR_EE_REG(0); /* clear down the EE config register */

#ifdef LNISA_DEBUG
  if(stat != ERROR)
    printf("Wrote 0x%x words to EEPROM OK\n",nwords);
#endif

  return stat;

}

/*
 * lnIsa_eeread - read and display nwords of the lnIsa's EEPROM device
 */
void lnIsa_eeread(DRV_CTRL *pDrvCtrl, int nwords)
{
  int i;

  /* check for too much data */
  if(nwords > EE_WORD_LEN_MAX)
  {
#ifdef LNISA_DEBUG
    printf("eeread: Max No. of words = %d\n",EE_WORD_LEN_MAX);
#endif
    return;
  }

  lnCsrWrite(pDrvCtrl,0,4); /* place the device in the STOP state */
  WR_EE_REG(EE_EN);	/* enable the EEPROM interface */
  EE_TIMEOUT;

  printf("AMD961 EEPROM Contents:\n");
  for(i = 0; i < nwords; i+=2)
  {
    printf("\tWord %02x : %04x",i,lnIsa_eeread16(pDrvCtrl,(UINT8)i));
    printf("\tWord %02x : %04x\n",i+1,lnIsa_eeread16(pDrvCtrl,(UINT8)i+1));
  }

  WR_EE_REG(0); /* clear down the EE config register */
}


/* 
 * lnIsa_eewrite16 - write 16 data bits to the word at wordAddr in the device.
 *		This function assume that the WEN has already been sent to
 *		the device.
 * wordAddr is in the range of 0 - 128 for a 93C56 device.
 *
 * NOTES:
 * The AMD961 expects the data to be written in bit reversal format. Hence 
 * Bit 0 of each word is transmitted first as opposed to Bit 15 as shown in
 * the NS93C56 data book.
 */
STATUS lnIsa_eewrite16(DRV_CTRL *pDrvCtrl, UINT8 wordAddr, UINT16 data16)
{
  int i;
  UINT8 addrMask = 0x80;
  UINT16 dataMask = 0x0001; /* send bit 0 first */
  STATUS stat = OK;

  WR_EE_REG(RD_EE_REG | EE_CS);   /* assert Chip Select */
  EE_TIMEOUT;

  lnIsa_eewriteBit(pDrvCtrl,1); /* send start bit */
  lnIsa_eewriteBit(pDrvCtrl,0); /* send first command bit */
  lnIsa_eewriteBit(pDrvCtrl,1); /* send second command bit */

  /* Send the Address */
  for(i = 0; i < 8; i++)
  {
    lnIsa_eewriteBit(pDrvCtrl, (int)(addrMask & wordAddr));
    addrMask = addrMask >> 1; 
  }
  
  /* send the data */
  for(i = 0; i < 16; i++)
  {
    lnIsa_eewriteBit(pDrvCtrl,(int)(dataMask & data16));
    dataMask = dataMask << 1; /* Bit reversal format */
  }

  /* clear CS to initiate write sequence */
  WR_EE_REG(RD_EE_REG & (~EE_CS));   /* clear Chip Select */
  EE_TIMEOUT;
  EE_TIMEOUT;
  WR_EE_REG(RD_EE_REG | EE_CS);   /* select Chip Select to determine write OK*/

  taskDelay(EE_TWP);
  if(RD_EE_REG & EE_DATA)
    stat = OK;
  else
  {
#ifdef LNISA_DEBUG
    printf("EEPROM Write Failed\n");
#endif
    stat = ERROR;
  }

  WR_EE_REG(RD_EE_REG & (~EE_CS));   /* clear Chip Select */
  return stat;
}

/* 
 * lnIsa_eeread16 - read 16 data bits from the word at wordAddr in the device.
 *
 * wordAddr is in the range of 0 - 128 for a 93C56 device.
 * 
 * NOTES:
 *  Since the data was written in bit reversal format, it has to be read 
 *  back in the same fashion. So Bit 0 is read first instead of Bit 15.
 */
UINT16 lnIsa_eeread16(DRV_CTRL *pDrvCtrl,UINT8 wordAddr)
{
  int i;
  UINT8 addrMask = 0x80;
  UINT16 dataMask = 0x0001; /* was 0x8000 */
  UINT16 dataRead = 0;

  WR_EE_REG(RD_EE_REG | EE_CS);   /* assert Chip Select */
  EE_TIMEOUT;

  lnIsa_eewriteBit(pDrvCtrl,1); /* send start bit */
  lnIsa_eewriteBit(pDrvCtrl,1); /* send first command bit */
  lnIsa_eewriteBit(pDrvCtrl,0); /* send second command bit */

  /* Send the Address */
  for(i = 0; i < 8; i++)
  {
    lnIsa_eewriteBit(pDrvCtrl, (int)(addrMask & wordAddr));
    addrMask = addrMask >> 1; 
  }
  
  /* read the data */
  for(i = 0; i < 16; i++)
  {
    if(lnIsa_eereadBit(pDrvCtrl))
      dataRead |= dataMask;
    dataMask = dataMask << 1; /* was >> */
  }

  /* clear CS to finish read sequence */
  WR_EE_REG(RD_EE_REG & (~EE_CS));   /* clear Chip Select */
  EE_TIMEOUT;
  EE_TIMEOUT;

  return dataRead;

}


/*
 * lnIsa_eewen - send Write Enable Command to EEPROM
 */
void lnIsa_eewen(DRV_CTRL *pDrvCtrl)
{
  int wen[] = { 1,0,0,1,1,0,0,0,0,0,0 };
  int i;

  WR_EE_REG(RD_EE_REG | EE_CS);   /* assert Chip Select */
  EE_TIMEOUT;
  for(i = 0; i < 11; i++)
    lnIsa_eewriteBit(pDrvCtrl,wen[i]);

  WR_EE_REG(RD_EE_REG & (~EE_CS));   /* clear Chip Select */
  EE_TIMEOUT;

}

/*
 * lnIsa_eewds - send Write Disable Command to EEPROM
 */
void lnIsa_eewds(DRV_CTRL *pDrvCtrl)
{
  int wds[] = { 1,0,0,0,0,0,0,0,0,0,0 };
  int i;

  WR_EE_REG(RD_EE_REG | EE_CS);   /* assert Chip Select */
  EE_TIMEOUT;
  for(i = 0; i < 11; i++)
    lnIsa_eewriteBit(pDrvCtrl,wds[i]);

  WR_EE_REG(RD_EE_REG & (~EE_CS));   /* clear Chip Select */
  EE_TIMEOUT;

}

/*
 * lnIsa_eeeraseall - erase the entire device
 */
void lnIsa_eeeraseall(DRV_CTRL *pDrvCtrl)
{
  int eral[] = { 1,0,0,1,0,0,0,0,0,0,0 };
  int i;

  lnCsrWrite(pDrvCtrl,0,4); /* place the device in the STOP state */

  WR_EE_REG(EE_EN);	/* enable the EEPROM interface */
  EE_TIMEOUT;

  lnIsa_eewen(pDrvCtrl); /* enable write access to EEPROM */

  WR_EE_REG(RD_EE_REG | EE_CS);   /* assert Chip Select */
  EE_TIMEOUT;

  for(i = 0; i < 11; i++)
    lnIsa_eewriteBit(pDrvCtrl,eral[i]);

  WR_EE_REG(RD_EE_REG & (~EE_CS));   /* clear Chip Select */
  EE_TIMEOUT;
  EE_TIMEOUT;
  WR_EE_REG(RD_EE_REG | EE_CS);   /* select Chip Select to determine erase OK*/

  taskDelay(EE_TWP);
#ifdef LNISA_DEBUG
  if(!(RD_EE_REG & EE_DATA))
    printf("EEPROM Erase Failed\n");
#endif

  WR_EE_REG(RD_EE_REG & (~EE_CS));   /* clear Chip Select */
  lnIsa_eewds(pDrvCtrl);	/* disable writes to EEPROM */
  WR_EE_REG(0); /* Disable EEPROM access */
}

/*
 * lnIsa_eewriteBit - write a single bit to the EEPROM. 
 * 
 * PARAMS:
 * 	bit - if zero write a zero, if non-zero write a one.
 */	
void lnIsa_eewriteBit(DRV_CTRL *pDrvCtrl, int bit)
{
  UINT16 eeData;
  UINT16 eeReg;
  /* 
   * All bits are clocked into chip on a lo to hi transition of SK, so 
   * set it lo to start.
   */
  if(bit)
    eeData = EE_DATA;
  else
    eeData = 0;

  /* 
   * Read current contents of EE register.
   * Clear out the DATA bit, so a simple OR will suffice later when setting
   * up the bit to write. Set the clock low, as data is clocked in on the
   * rising edge of clock.
   */
  eeReg = RD_EE_REG;
  eeReg &= (~(EE_SK | EE_DATA)); 
  WR_EE_REG(eeReg); /* set clock low */
  EE_TIMEOUT;

  eeReg |= eeData;
  WR_EE_REG(eeReg);	/* set up the data */
  EE_TIMEOUT;
  eeReg |= EE_SK;
  WR_EE_REG(eeReg); /* clock the bit into the EEPROM */
  EE_TIMEOUT;

}
			   
/*
 * lnIsa_eereadBit - clock in a bit from the EEPROM
 */
int lnIsa_eereadBit(DRV_CTRL *pDrvCtrl)
{
  UINT16 retBit = 0;
  UINT16 eeReg;

  eeReg = RD_EE_REG;
  eeReg &= ~(EE_SK); 
  WR_EE_REG(eeReg); /* set clock low */
  EE_TIMEOUT;
  eeReg |= EE_SK;   
  WR_EE_REG(eeReg); /* set clock High */
  EE_TIMEOUT;
  retBit = RD_EE_REG & EE_DATA;
  EE_TIMEOUT;

  return (int)retBit;
}

/* 
 * lnIsa_eetimeout - Since a sysInWord takes at least 130 nSec then use 
 *		an lnIdpRead to generate the required timeout for the EEPROM 
 * 		Shift Clock. There are two reads per lnIdpRead so each == 260 
 *		nSecs. The 93C56 max clock rate is 1 Mhz. We should be well 
 *              within this using this method.
 */

void lnIsa_eetimeout(DRV_CTRL *pDrvCtrl)
{
  int i;
  for(i = 0; i < 10; i++)
    RD_EE_REG;
}

/*
 * pnpInitiation - write the Plug 'n Play initiation sequence to the PnP 
 * 		   address port. This prepares all PnP devices for the 
 * 		   isolation algorithm
 *
 * Note: The sequence must be written monolithically. Any other accesses to 
 * the PNP_ADDR_PORT must be disabled while the init sequence is being 
 * written. This may include disabling interrupts.
 *
 */
UINT8 pnpInitKey [] = { 0x6a, 0xb5, 0xda, 0xed, 0xf6, 0xfb, 0x7d, 0xbe,
			 0xdf, 0x6f, 0x37, 0x1b, 0x0d, 0x86, 0xc3, 0x61,
			 0xb0, 0x58, 0x2c, 0x16, 0x8b, 0x45, 0xa2, 0xd1,
			 0xe8, 0x74, 0x3a, 0x9d, 0xce, 0xe7, 0x73, 0x39 };


UINT8 pnpRelocateKey [] = { 0x6b, 0x35, 0x9a, 0xcd, 0xe6, 0xf3, 0x79, 0xbc,
			    0x5e, 0xaf, 0x57, 0x2b, 0x15, 0x8a, 0xc5, 0xe2,
			    0xf1, 0xf8, 0x7c, 0x3e, 0x9f, 0x4f, 0x27, 0x13,
			    0x09, 0x84, 0x42, 0xa1, 0xd0, 0x68, 0x34, 0x1a };

/*
 * lnIsaPnPInitiation - send the PnP software relocatable Key to the AMD961
 *			to take it out of "Wait for Key" state. These writes
 *			must occur sequencially, any extraneous writes to the 
 *			PNP_ADDR_PORT will cause the initiation to fail.
 */
void lnIsaPnPInitiation(void)
{
  int i;

  sysOutByte (PNP_ADDR_PORT,PNP_CONFIG_CTRL);
  sysOutByte (PNP_WR_PORT,2);	/* Set Wait for Key State */

  /* send Initiation Key */
  sysOutByte(PNP_ADDR_PORT, 0);	/* reset comparison logic */
  sysOutByte(PNP_ADDR_PORT, 0);
  for(i = 0; i < PNP_INIT_KEY_LEN; i++)
    sysOutByte(PNP_ADDR_PORT, pnpRelocateKey[i]); /* write initiation key */

}

/*
 * lnIsaPnPSetup - set up the necessary Plug n' Play registers
 * 		   and activate the device on the ISA bus. This function
 *		   allows the AMD961 to be placed in a mode such that the
 * 		   the EEPROM may be programmed with the Plug n' Play 
 *		   information.
 */
void lnIsaPnPSetup (char * ioAdrs, UINT8 intLvl, UINT8 intType,
		    UINT8 dmaChan)
{
  sysOutByte (PNP_ADDR_PORT,PNP_WAKE);
  sysOutByte (PNP_WR_PORT,0);	/* write 0 to WAKE to enter Isolation */

  sysOutByte (PNP_ADDR_PORT,PNP_SET_RD_DATA);
  sysOutByte (PNP_WR_PORT,(PNP_RD_PORT >> 2)); /* Init READ-PORT */

  sysOutByte (PNP_ADDR_PORT,PNP_CSN);
  sysOutByte (PNP_WR_PORT,4);	/* Set up non-zero CSN */

  sysOutByte (PNP_ADDR_PORT,PNP_WAKE);
  sysOutByte (PNP_WR_PORT,4);	/* write CSN of 4 (arbitrary) to WAKE to 
				   enter Config Mode */

  sysOutByte (PNP_ADDR_PORT,PNP_DEV_NUM);

  /* set the IO address range */
  sysOutByte(PNP_ADDR_PORT,PNP_IO_BASE0_HIBYTE);
  sysOutByte(PNP_WR_PORT,(UINT8)((UINT32)(ioAdrs)>>8));

  sysOutByte(PNP_ADDR_PORT,PNP_IO_BASE0_LOBYTE);
  sysOutByte(PNP_WR_PORT,(UINT8)((UINT32)ioAdrs & 0xff));

  /* set up the Interrupt level and type */
  sysOutByte(PNP_ADDR_PORT,PNP_IRQ_LEVEL_SEL0);
  sysOutByte(PNP_WR_PORT,intLvl);

  sysOutByte(PNP_ADDR_PORT,PNP_IRQ_TYPE_SEL0);
  sysOutByte(PNP_WR_PORT,intType);

  /* set the DMA Channel */
  sysOutByte(PNP_ADDR_PORT,PNP_DMA_CHANNEL_SEL0);
  sysOutByte(PNP_WR_PORT,dmaChan);

  /* enable the device on the ISA bus */
  sysOutByte(PNP_ADDR_PORT,PNP_IO_RANGE);
  sysOutByte(PNP_WR_PORT,0x0); /* disable IO range check */
  sysOutByte(PNP_ADDR_PORT,PNP_ACTIVATE);
  sysOutByte(PNP_WR_PORT,0x01); /* Put us on the ISA bus */

  /* take device out of Config mode */
  sysOutByte(PNP_ADDR_PORT,PNP_CONFIG_CTRL);
  sysOutByte(PNP_WR_PORT,PNP_CONFIG_CTRL_WAITFORKEY); /*back to Wait for Key */
    
}

#ifdef LNISA_DEBUG
/* 
 * idpWrite - local version of lnIdpWrite
 */
void idpWrite
    (
    int 	reg,			/* register to select */
    UINT16 	value			/* value to write */
    )
    {
    int dv = (int)IO_ADRS_LNISA;

    /* select IDP */
    sysOutWord(dv+0x12,reg);;

    /* write val to IDP */
    sysOutWord(dv+0x16,value);;

    }

UINT16 idpRead
    (
    int		reg			/* register to select */
    )
    {
    int dv = (int)IO_ADRS_LNISA;

    /* select IDP register */
    sysOutWord(dv+0x12,reg);;

    /* get contents of IDP register */
    return (sysInWord(dv+0x16));

    }

void csrWrite
    (
    int 	reg,			/* register to select */
    USHORT 	value			/* value to write */
    )
    {
    int dv = (int)IO_ADRS_LNISA;

    /* select CSR */
    sysOutWord(dv+0x12,reg);;

    /* write val to CSR */
    sysOutWord(dv+0x10,value);;

    }

UINT16 csrRead
    (
    int		reg			/* register to select */
    )
    {
    int dv = (int)IO_ADRS_LNISA;

    /* select CSR */
    sysOutWord(dv+0x12,reg);;

    /* get contents of CSR */
    return (sysInWord(dv+0x10));

    }
#endif /* LNISA_DEBUG */

