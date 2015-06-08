
#ifdef VXWORKS

/*
 * laser.c - written by Kyle Burchesky, 6/26/98
 *
 * Laser control system for VxWorks.
 *
 * These routines are intended to compliment a GUI written in
 * TCL/TK via the DP_ask command.  These routines are accessed
 * by typing the name of the function (gas, pwr, or ena) followed
 * by two integers...  the first is between 1 and 4 and specifies
 * the laser system to talk to.  The second is either 0 or 1.
 * A "1" turns the device on, and a "0" turns it off.
 *
 *    Questions email to burchesk@jlab.org, or kyle@osfn.org
 *
 *
 *    first modified by:   Kyungmo Kim, KNU, SP, July 1999
 *      - hardware setting of LCS are changed, I/O module, cabling, etc.
 *      - address and "ena", "input", "output",etc routines are changed.
 *
 *    Questions email to kmkim@jlab.org, or kmkim@nuclear.kyungpook.ac.kr
 *
 *
 *    modified by:   SP (sergpozd@jlab.org), June 2003
 *      - hexbin(), save result of modification in static area;
 *      - output() and input(), changed manner of data read;
 *      - new: gas_ON(), gas_OFF(), pwr_ON(), pwr_OFF();
 *      - new: TOF_laser_enable(), TOF_laser_disable().
 *
 */

/* ######## Mask Conversion Table ####### */
/* Channel  Binary Address  Hex Address   */
/*    0       0000 0001         0x01      */
/*    1       0000 0010         0x02      */
/*    2       0000 0100         0x04      */
/*    3       0000 1000         0x08      */
/*    4       0001 0000         0x10      */
/*    5       0010 0000         0x20      */
/*    6       0100 0000         0x40      */
/*    7       1000 0000         0x80      */


/* ######## OUTPUT PORT REGISTER [addr = 2] on camac1  ###########*/

/* Gas1 0x01  */
/* Pwr1 0x02  */                       /* VMEbus Base Address 0ec */
/* Ena1 0x04  */
/* Dis1 0x08  */


/* ######## INPUT PORT REGISTER [addr = 6] on camac1  ############*/

/* pwr1 on  0000 0000 0001 0000                                   */
/* ready1   0000 0000 0010 0000                                   */
/* enable1  0000 0000 0100 0000                                   */


/* ######## OUTPUT PORT REGISTER [addr = 6] on sc-laser1 #########*/ 
 
/* Gas2 0x01  */   /* Gas3 0x10  */
/* Pwr2 0x02  */   /* Pwr3 0x20  */    /* VMEbus Base Address 0ed */
/* Ena2 0x04  */   /* Ena3 0x40  */
/* Dis2 0x08  */   /* Dis3 0x80  */

/* Gas4 0x01  */
/* Pwr4 0x02  */                       /* VMEbus Base Address 0ee */
/* Ena4 0x04  */
/* Dis4 0x08  */


/* ######## INPUT PORT REGISTER [addr = 8] on sc-laser1 ##########*/

/* pwr2 on  0000 0000 0001 0000     pwr3 on  0000 0010 0000 0000  */
/* ready2   0000 0000 0100 0000     ready3   0000 0100 0000 0000  */
/* enable2  0000 0000 1000 0000     enable3  0000 1000 0000 0000  */

/* pwr4 on  0000 0000 0001 0000                                   */
/* ready4   0000 0000 0010 0000                                   */
/* enable4  0000 0000 0100 0000                                   */



#include <stdio.h>
#include <taskLib.h>
#include <sysLib.h>
#include "laservx.h"

static char result[50];

static unsigned short *out_addr1 = NULL;
static unsigned short *out_addr2 = NULL;
static unsigned short *out_addr3 = NULL;
static unsigned short *out_addr4 = NULL;
static unsigned short  *in_addr1 = NULL;
static unsigned short  *in_addr2 = NULL;
static unsigned short  *in_addr3 = NULL;
static unsigned short  *in_addr4 = NULL;

void
set_address()
{
  unsigned long offset16;
  
  sysBusToLocalAdrs(0x29, NULL, (char **)&offset16);
  
  /* declare output address */
  out_addr1 = (unsigned short *) (offset16 + 0x0ec2); /* FLEXIO */
  out_addr2 = (unsigned short *) (offset16 + 0x0ed6); /* TI */
  out_addr3 = (unsigned short *) (offset16 + 0x0ed6); /* TI */
  out_addr4 = (unsigned short *) (offset16 + 0x0ee6); /* TI */
  
  /* declare input address */
  in_addr1 = (unsigned short *) (offset16 + 0x0ec6); /* FLEXIO */
  in_addr2 = (unsigned short *) (offset16 + 0x0ed8); /* TI */
  in_addr3 = (unsigned short *) (offset16 + 0x0ed8); /* TI */
  in_addr4 = (unsigned short *) (offset16 + 0x0ee8); /* TI */
}

void
delay(int seconds)
{
  taskDelay(sysClkRateGet()*seconds);
}

void
gas(int id, int x)
{
  if(out_addr1 == NULL) set_address();
  
  switch(id) {
  case 1:
    if(x == 1) *out_addr1 = *out_addr1 | 0x01;
    if(x == 0) *out_addr1 = *out_addr1 ^ (*out_addr1&0x01);
    break;
  case 2:
    if(x == 1) *out_addr2 = *out_addr2 | 0x01;
    if(x == 0) *out_addr2 = *out_addr2 ^ (*out_addr2&0x01);
    break;
  case 3:
    if(x == 1) *out_addr3 = *out_addr3 | 0x10;
    if(x == 0) *out_addr3 = *out_addr3 ^ (*out_addr3&0x10);
    break;
  case 4:
    if(x == 1) *out_addr4 = *out_addr4 | 0x01;
    if(x == 0) *out_addr4 = *out_addr4 ^ (*out_addr4&0x01);
    break;
  }
}

int
gas_ON(int laser_number)
{
  gas(laser_number, 1);
  return(0);
}

int
gas_OFF(int laser_number)
{
  gas(laser_number, 0);
  return(0);
}

void
pwr(int id, int x)
{
  if(out_addr1 == NULL) set_address();
  
  switch(id) {
  case 1:
    if (x == 1) *out_addr1 = *out_addr1 | 0x02;
    if (x == 0) *out_addr1 = *out_addr1 ^ (*out_addr1&0x02);
    break;
  case 2:
    if (x == 1) *out_addr2 = *out_addr2 | 0x02;
    if (x == 0) *out_addr2 = *out_addr2 ^ (*out_addr2&0x02);
    break;
  case 3:
    if (x == 1) *out_addr3 = *out_addr3 | 0x20;
    if (x == 0) *out_addr3 = *out_addr3 ^ (*out_addr3&0x20);
    break;
  case 4:
    if (x == 1) *out_addr4 = *out_addr4 | 0x02;
    if (x == 0) *out_addr4 = *out_addr4 ^ (*out_addr4&0x02);
    break;
  }
}

int
pwr_ON(int laser_number)
{
  pwr(laser_number, 1);
  return(0);
}

int
pwr_OFF(int laser_number)
{
  pwr(laser_number, 0);
  return(0);
}

void
ena(int id, int x)
{
  if(out_addr1 == NULL) set_address();
  
  if(x == 1) {
    switch(id) {
    case 1:
      *out_addr1 = *out_addr1 | 0x04;
      delay(1);
      *out_addr1 = *out_addr1 ^ (*out_addr1&0x04);
      break;
    case 2:
      *out_addr2 = *out_addr2 | 0x04;
      delay(1);
      *out_addr2 = *out_addr2 ^ (*out_addr2&0x04);
      break;
    case 3:
      *out_addr3 = *out_addr3 | 0x40;
      delay(1);
      *out_addr3 = *out_addr3 ^ (*out_addr3&0x40);
      break;
    case 4:
      *out_addr4 = *out_addr4 | 0x04;
      delay(1);
      *out_addr4 = *out_addr4 ^ (*out_addr4&0x04);
      break;
    }
  }
  
  if(x == 0) {
    switch(id) {
    case 1:
      *out_addr1 = *out_addr1 | 0x08;
      delay(1);
      *out_addr1 = *out_addr1 ^ (*out_addr1&0x08);
      break;
    case 2:
      *out_addr2 = *out_addr2 | 0x08;
      delay(1);
      *out_addr2 = *out_addr2 ^ (*out_addr2&0x08);
      break;
    case 3:
      *out_addr3 = *out_addr3 | 0x80;
      delay(1);
      *out_addr3 = *out_addr3 ^ (*out_addr3&0x80);
      break;
    case 4:
      *out_addr4 = *out_addr4 | 0x08;
      delay(1);
      *out_addr4 = *out_addr4 ^ (*out_addr4&0x08);
      break;
    }
  }
}

int
TOF_laser_enable(int laser_number)
{
  ena(laser_number, 1);
  return(0);
}

int
TOF_laser_disable(int laser_number)
{
  ena(laser_number, 0);
  return(0);
}

/* ........................................................ */

void
hexbin(unsigned short h)  /* use 16 bits */
{
  int n;
  
  sprintf(result,"");
  for(n=1; n<=16; n++) {
    if(h & 0x8000) { printf("1"); strcat(result,"1"); }
    else           { printf("0"); strcat(result,"0"); }
    h = h << 1;
  }
  printf("\n");
}

char *
output(int laser_number)
{
  if(out_addr1 == NULL) set_address();
  
  switch(laser_number)
    {
    case 1:  hexbin(*out_addr1 & 0xffff);  break;
    case 2:  hexbin(*out_addr2 & 0x00ff);  break;
    case 3:  hexbin(*out_addr3 & 0x00ff);  break;
    case 4:  hexbin(*out_addr4 & 0x00ff);  break;
    }
  return(result);
}

char *
input(int laser_number)
{
  if(out_addr1 == NULL) set_address();
  
  switch(laser_number)
    {
    case 1:  hexbin(*in_addr1 & 0xffff);  break;
    case 2:  hexbin(*in_addr2 & 0x0fff);  break;
    case 3:  hexbin(*in_addr3 & 0x0fff);  break;
    case 4:  hexbin(*in_addr4 & 0x0fff);  break;
    }
  return(result);
}

/*----------------------------------------------------------------*/
/*----------------------------------------------------------------*/

void
in_out_print(int laser_number)
{
  printf("%s;; <-- output(%d) \n", output(laser_number), laser_number);
  printf("%s;; <-- input(%d)  \n", input(laser_number),  laser_number);
}

#else

void
laser_dummy()
{
  return;
}

#endif

