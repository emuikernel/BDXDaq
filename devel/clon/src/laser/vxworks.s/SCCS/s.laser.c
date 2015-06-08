h16264
s 00002/00002/00326
d D 1.6 04/08/10 17:07:34 sergpozd 7 6
c camac3 --> camac1
e
s 00207/00174/00121
d D 1.5 03/07/21 16:45:08 sergpozd 6 5
c Completely new revision
e
s 00078/00043/00217
d D 1.4 03/03/03 14:01:40 boiarino 5 4
c first version for 2306 CPU
e
s 00001/00000/00259
d D 1.3 03/02/14 13:02:23 boiarino 4 3
c #include "laser.h"
e
s 00150/00127/00109
d D 1.2 03/02/14 12:37:40 boiarino 3 1
c some cleanup
e
s 00000/00000/00000
d R 1.2 70/01/01 00:00:02 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 vxworks/laser/laser.c
e
s 00236/00000/00000
d D 1.1 00/08/03 10:53:14 gurjyan 1 0
c date and time created 00/08/03 10:53:14 by gurjyan
e
u
U
f e 0
t
T
I 3
D 6
/*                                                                 */
/* laser.c                                                         */
/*                                                                 */
E 3
I 1
/* Laser control system for VxWorks. By Kyle Burchesky 6/26/98     */
/* These routines are intended to compliment a GUI written in      */
/* TCL/TK via the DP_ask command.  These routines are accessed     */
/* by typing the name of the function (gas, pwr, or ena) followed  */
/* by two integers...  the first is between 1 and 4 and specifies  */
/* the laser system to talk to.  The second is either 0 or 1.      */
/* A "1" turns the device on, and a "0" turns it off.              */
/* Questions email burchesk@jlab.org, or kyle@osfn.org             */
E 6
I 6
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
E 6

D 6
/* First modification at July 1999.  By Kyungmo Kim, KNU           */
/* Hardware setting of LCS are changed, I/O module, cabling, etc.  */
/* Address and "ena", "input", "output",etc routines are changed.  */ 
/* Questions email. kmkim@jlab.org, kmkim@nuclear.kyungpook.ac.kr  */


E 6
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


D 7
/* ######## OUTPUT PORT REGISTER [addr = 2] on camac3  ###########*/
E 7
I 7
/* ######## OUTPUT PORT REGISTER [addr = 2] on camac1  ###########*/
E 7

/* Gas1 0x01  */
/* Pwr1 0x02  */                       /* VMEbus Base Address 0ec */
/* Ena1 0x04  */
/* Dis1 0x08  */


D 7
/* ######## INPUT PORT REGISTER [addr = 6] on camac3  ############*/
E 7
I 7
/* ######## INPUT PORT REGISTER [addr = 6] on camac1  ############*/
E 7

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
I 4
#include "laser.h"
E 4

D 5
short *out_addr1 = (short *) 0xffff0ec2;  /*declare output address */
short *out_addr2 = (short *) 0xffff0ed6;
short *out_addr3 = (short *) 0xffff0ed6;
short *out_addr4 = (short *) 0xffff0ee6;
short  *in_addr1 = (short *) 0xffff0ec6;   /*declare input address */
short  *in_addr2 = (short *) 0xffff0ed8;
short  *in_addr3 = (short *) 0xffff0ed8;
short  *in_addr4 = (short *) 0xffff0ee8;
E 5
I 5
D 6
static short *out_addr1 = NULL;
static short *out_addr2 = NULL;
static short *out_addr3 = NULL;
static short *out_addr4 = NULL;
static short  *in_addr1 = NULL;
static short  *in_addr2 = NULL;
static short  *in_addr3 = NULL;
static short  *in_addr4 = NULL;
E 6
I 6
static char result[50];
E 6
E 5

I 6
static unsigned short *out_addr1 = NULL;
static unsigned short *out_addr2 = NULL;
static unsigned short *out_addr3 = NULL;
static unsigned short *out_addr4 = NULL;
static unsigned short  *in_addr1 = NULL;
static unsigned short  *in_addr2 = NULL;
static unsigned short  *in_addr3 = NULL;
static unsigned short  *in_addr4 = NULL;

E 6
I 5
void
set_address()
{
  unsigned long offset16;
E 5
D 3
void delay(int seconds)
E 3
I 3
D 6

E 6
I 6
  
E 6
I 5
  sysBusToLocalAdrs(0x29, NULL, (char **)&offset16);
D 6
  printf("offset16=0x%08x\n",(int)offset16); /* 0xffff0000 for 162 */

E 6
I 6
  
E 6
  /* declare output address */
D 6
  out_addr1 = (short *) (offset16 + 0x0ec2); /* FLEXIO */
  out_addr2 = (short *) (offset16 + 0x0ed6); /* TI */
  out_addr3 = (short *) (offset16 + 0x0ed6); /* TI */
  out_addr4 = (short *) (offset16 + 0x0ee6); /* TI */

E 6
I 6
  out_addr1 = (unsigned short *) (offset16 + 0x0ec2); /* FLEXIO */
  out_addr2 = (unsigned short *) (offset16 + 0x0ed6); /* TI */
  out_addr3 = (unsigned short *) (offset16 + 0x0ed6); /* TI */
  out_addr4 = (unsigned short *) (offset16 + 0x0ee6); /* TI */
  
E 6
  /* declare input address */
D 6
  in_addr1 = (short *) (offset16 + 0x0ec6); /* FLEXIO */
  in_addr2 = (short *) (offset16 + 0x0ed8); /* TI */
  in_addr3 = (short *) (offset16 + 0x0ed8); /* TI */
  in_addr4 = (short *) (offset16 + 0x0ee8); /* TI */
E 6
I 6
  in_addr1 = (unsigned short *) (offset16 + 0x0ec6); /* FLEXIO */
  in_addr2 = (unsigned short *) (offset16 + 0x0ed8); /* TI */
  in_addr3 = (unsigned short *) (offset16 + 0x0ed8); /* TI */
  in_addr4 = (unsigned short *) (offset16 + 0x0ee8); /* TI */
E 6
}

E 5
void
delay(int seconds)
E 3
{
D 3
  taskDelay( sysClkRateGet() * seconds );
E 3
I 3
D 5
  taskDelay(sysClkRateGet() * seconds);
E 5
I 5
  taskDelay(sysClkRateGet()*seconds);
E 5
E 3
}

D 3
void gas(int id,int x)
E 3
I 3
void
gas(int id, int x)
E 3
{
I 5
  if(out_addr1 == NULL) set_address();
I 6
  
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
E 6

E 5
D 3
  switch (id) {
  case 1: {
    if (x == 1) *out_addr1=*out_addr1|0x01;
    if (x == 0) *out_addr1=*out_addr1^(*out_addr1&0x01);
    break;
E 3
I 3
D 6
  switch(id)
  {
E 6
I 6
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
E 6
    case 1:
D 5
      if(x == 1) *out_addr1=*out_addr1|0x01;
      if(x == 0) *out_addr1=*out_addr1^(*out_addr1&0x01);
E 5
I 5
D 6
      if(x == 1) *out_addr1 = *out_addr1 | 0x01;
      if(x == 0) *out_addr1 = *out_addr1 ^ (*out_addr1&0x01);
E 6
I 6
      *out_addr1 = *out_addr1 | 0x04;
      delay(1);
      *out_addr1 = *out_addr1 ^ (*out_addr1&0x04);
E 6
E 5
      break;
    case 2:
D 5
      if(x == 1) *out_addr2=*out_addr2|0x01;
      if(x == 0) *out_addr2=*out_addr2^(*out_addr2&0x01);
E 5
I 5
D 6
      if(x == 1) *out_addr2 = *out_addr2 | 0x01;
      if(x == 0) *out_addr2 = *out_addr2 ^ (*out_addr2&0x01);
E 6
I 6
      *out_addr2 = *out_addr2 | 0x04;
      delay(1);
      *out_addr2 = *out_addr2 ^ (*out_addr2&0x04);
E 6
E 5
      break;
    case 3:
D 5
      if(x == 1) *out_addr3=*out_addr3|0x10;
      if(x == 0) *out_addr3=*out_addr3^(*out_addr3&0x10);
E 5
I 5
D 6
      if(x == 1) *out_addr3 = *out_addr3 | 0x10;
      if(x == 0) *out_addr3 = *out_addr3 ^ (*out_addr3&0x10);
E 6
I 6
      *out_addr3 = *out_addr3 | 0x40;
      delay(1);
      *out_addr3 = *out_addr3 ^ (*out_addr3&0x40);
E 6
E 5
      break;
    case 4:
D 5
      if(x == 1) *out_addr4=*out_addr4|0x01;
      if(x == 0) *out_addr4=*out_addr4^(*out_addr4&0x01);
E 5
I 5
D 6
      if(x == 1) *out_addr4 = *out_addr4 | 0x01;
      if(x == 0) *out_addr4 = *out_addr4 ^ (*out_addr4&0x01);
E 6
I 6
      *out_addr4 = *out_addr4 | 0x04;
      delay(1);
      *out_addr4 = *out_addr4 ^ (*out_addr4&0x04);
E 6
E 5
      break;
D 6
    default:
      ;
E 6
I 6
    }
E 6
E 3
  }
D 3
  case 2: {
    if (x == 1) *out_addr2=*out_addr2|0x01;
    if (x == 0) *out_addr2=*out_addr2^(*out_addr2&0x01);
    break;
  }
  case 3: { 
    if (x == 1) *out_addr3=*out_addr3|0x10;
    if (x == 0) *out_addr3=*out_addr3^(*out_addr3&0x10);
    break;
  }
  case 4: {
    if (x == 1) *out_addr4=*out_addr4|0x01;
    if (x == 0) *out_addr4=*out_addr4^(*out_addr4&0x01);
    break;
  }
  }
E 3
D 6
}

D 3
void pwr(int id,int x)
E 3
I 3
void
pwr(int id, int x)
E 3
{
I 5
  if(out_addr1 == NULL) set_address();

E 5
D 3
  switch (id) {
  case 1: { 
    if (x == 1) *out_addr1=*out_addr1|0x02;
    if (x == 0) *out_addr1=*out_addr1^(*out_addr1&0x02);
    break;
E 3
I 3
  switch(id)
  {
E 6
I 6
  
  if(x == 0) {
    switch(id) {
E 6
    case 1:
D 5
      if (x == 1) *out_addr1=*out_addr1|0x02;
      if (x == 0) *out_addr1=*out_addr1^(*out_addr1&0x02);
E 5
I 5
D 6
      if (x == 1) *out_addr1 = *out_addr1 | 0x02;
      if (x == 0) *out_addr1 = *out_addr1 ^ (*out_addr1&0x02);
E 6
I 6
      *out_addr1 = *out_addr1 | 0x08;
      delay(1);
      *out_addr1 = *out_addr1 ^ (*out_addr1&0x08);
E 6
E 5
      break;
    case 2:
D 5
      if (x == 1) *out_addr2=*out_addr2|0x02;
      if (x == 0) *out_addr2=*out_addr2^(*out_addr2&0x02);
E 5
I 5
D 6
      if (x == 1) *out_addr2 = *out_addr2 | 0x02;
      if (x == 0) *out_addr2 = *out_addr2 ^ (*out_addr2&0x02);
E 6
I 6
      *out_addr2 = *out_addr2 | 0x08;
      delay(1);
      *out_addr2 = *out_addr2 ^ (*out_addr2&0x08);
E 6
E 5
      break;
    case 3:
D 5
      if (x == 1) *out_addr3=*out_addr3|0x20;
      if (x == 0) *out_addr3=*out_addr3^(*out_addr3&0x20);
E 5
I 5
D 6
      if (x == 1) *out_addr3 = *out_addr3 | 0x20;
      if (x == 0) *out_addr3 = *out_addr3 ^ (*out_addr3&0x20);
E 6
I 6
      *out_addr3 = *out_addr3 | 0x80;
      delay(1);
      *out_addr3 = *out_addr3 ^ (*out_addr3&0x80);
E 6
E 5
      break;
    case 4:
D 5
      if (x == 1) *out_addr4=*out_addr4|0x02;
      if (x == 0) *out_addr4=*out_addr4^(*out_addr4&0x02);
E 5
I 5
D 6
      if (x == 1) *out_addr4 = *out_addr4 | 0x02;
      if (x == 0) *out_addr4 = *out_addr4 ^ (*out_addr4&0x02);
E 6
I 6
      *out_addr4 = *out_addr4 | 0x08;
      delay(1);
      *out_addr4 = *out_addr4 ^ (*out_addr4&0x08);
E 6
E 5
      break;
D 6
    default:
      ;
E 6
I 6
    }
E 6
E 3
  }
D 3
  case 2: { 
    if (x == 1) *out_addr2=*out_addr2|0x02;
    if (x == 0) *out_addr2=*out_addr2^(*out_addr2&0x02);
    break;
  }
  case 3: {
    if (x == 1) *out_addr3=*out_addr3|0x20;
    if (x == 0) *out_addr3=*out_addr3^(*out_addr3&0x20);
    break;
  }
  case 4: {
    if (x == 1) *out_addr4=*out_addr4|0x02;
    if (x == 0) *out_addr4=*out_addr4^(*out_addr4&0x02);
    break;
  }
  }
E 3
}

D 3
void ena(int id,int x)
E 3
I 3
D 6
void
ena(int id, int x)
E 6
I 6
int
TOF_laser_enable(int laser_number)
E 6
E 3
{
I 5
D 6
  if(out_addr1 == NULL) set_address();
E 6
I 6
  ena(laser_number, 1);
  return(0);
}
E 6

E 5
D 3
  if (x == 1) {
    switch (id) {
    case 1: { 
      *out_addr1=*out_addr1|0x04;
      delay(1);
      *out_addr1=*out_addr1^(*out_addr1&0x04);
      break;} 
    case 2: { 
      *out_addr2=*out_addr2|0x04;
      delay(1);
      *out_addr2=*out_addr2^(*out_addr2&0x04);
      break;}
    case 3: {
      *out_addr3=*out_addr3|0x40;
      delay(1);
      *out_addr3=*out_addr3^(*out_addr3&0x40);
      break;}
    case 4: {
      *out_addr4=*out_addr4|0x04;
      delay(1);
      *out_addr4=*out_addr4^(*out_addr4&0x04);
      break;}
E 3
I 3
D 6
  if(x == 1)
  {
    switch(id)
    {
      case 1:
D 5
        *out_addr1=*out_addr1|0x04;
E 5
I 5
        *out_addr1 = *out_addr1 | 0x04;
E 5
        delay(1);
D 5
        *out_addr1=*out_addr1^(*out_addr1&0x04);
E 5
I 5
        *out_addr1 = *out_addr1 ^ (*out_addr1&0x04);
E 5
        break;
      case 2:
D 5
        *out_addr2=*out_addr2|0x04;
E 5
I 5
        *out_addr2 = *out_addr2 | 0x04;
E 5
        delay(1);
D 5
        *out_addr2=*out_addr2^(*out_addr2&0x04);
E 5
I 5
        *out_addr2 = *out_addr2 ^ (*out_addr2&0x04);
E 5
        break;
      case 3:
D 5
        *out_addr3=*out_addr3|0x40;
E 5
I 5
        *out_addr3 = *out_addr3 | 0x40;
E 5
        delay(1);
D 5
        *out_addr3=*out_addr3^(*out_addr3&0x40);
E 5
I 5
        *out_addr3 = *out_addr3 ^ (*out_addr3&0x40);
E 5
        break;
      case 4:
D 5
        *out_addr4=*out_addr4|0x04;
E 5
I 5
        *out_addr4 = *out_addr4 | 0x04;
E 5
        delay(1);
D 5
        *out_addr4=*out_addr4^(*out_addr4&0x04);
E 5
I 5
        *out_addr4 = *out_addr4 ^ (*out_addr4&0x04);
E 5
        break;
      default:
        ;
E 3
    }
D 3
  } 
  if (x == 0) {
    switch (id) {
    case 1: { 
      *out_addr1=*out_addr1|0x08;
      delay(1);
      *out_addr1=*out_addr1^(*out_addr1&0x08);
      break;}
    case 2: { 
      *out_addr2=*out_addr2|0x08;
      delay(1);
      *out_addr2=*out_addr2^(*out_addr2&0x08);
      break;}
    case 3: {
      *out_addr3=*out_addr3|0x80;
      delay(1);
      *out_addr3=*out_addr3^(*out_addr3&0x80);
      break;}
    case 4: {
      *out_addr4=*out_addr4|0x08;
      delay(1);
      *out_addr4=*out_addr4^(*out_addr4&0x08);
      break;}
E 3
I 3
  }
 
  if(x == 0)
  {
    switch(id)
    {
      case 1:
D 5
        *out_addr1=*out_addr1|0x08;
E 5
I 5
printf("out_addr1=0x%08x\n",out_addr1);
        *out_addr1 = *out_addr1 | 0x08;
E 5
        delay(1);
D 5
        *out_addr1=*out_addr1^(*out_addr1&0x08);
E 5
I 5
        *out_addr1 = *out_addr1 ^ (*out_addr1&0x08);
E 5
        break;
      case 2:
D 5
        *out_addr2=*out_addr2|0x08;
E 5
I 5
        *out_addr2 = *out_addr2 | 0x08;
E 5
        delay(1);
D 5
        *out_addr2=*out_addr2^(*out_addr2&0x08);
E 5
I 5
        *out_addr2 = *out_addr2 ^ (*out_addr2&0x08);
E 5
        break;
      case 3:
D 5
        *out_addr3=*out_addr3|0x80;
E 5
I 5
        *out_addr3 = *out_addr3 | 0x80;
E 5
        delay(1);
D 5
        *out_addr3=*out_addr3^(*out_addr3&0x80);
E 5
I 5
        *out_addr3 = *out_addr3 ^ (*out_addr3&0x80);
E 5
        break;
      case 4:
D 5
        *out_addr4=*out_addr4|0x08;
E 5
I 5
        *out_addr4 = *out_addr4 | 0x08;
E 5
        delay(1);
D 5
        *out_addr4=*out_addr4^(*out_addr4&0x08);
E 5
I 5
        *out_addr4 = *out_addr4 ^ (*out_addr4&0x08);
E 5
        break;
      default:
        ;
E 3
    }
  }
E 6
I 6
int
TOF_laser_disable(int laser_number)
{
  ena(laser_number, 0);
  return(0);
E 6
}

I 6
/* ........................................................ */
E 6
I 5

D 6
/* ... */

E 6
E 5
D 3
void hexbin(short h)                 /* use 16 bits */
E 3
I 3
void
D 6
hexbin(short h)  /* use 16 bits */
E 6
I 6
hexbin(unsigned short h)  /* use 16 bits */
E 6
E 3
{
  int n;
D 3
  for (n=1 ; n<=16 ; n++)  
    {
      if (h & 0x8000) printf("1");      
      else printf("0");
      h=h<<1;
    }
E 3
I 3
D 6

  for(n=1; n<=16; n++)  
  {
    if(h & 0x8000) printf("1");      
    else printf("0");
E 6
I 6
  
  sprintf(result,"");
  for(n=1; n<=16; n++) {
    if(h & 0x8000) { printf("1"); strcat(result,"1"); }
    else           { printf("0"); strcat(result,"0"); }
E 6
D 5
    h=h<<1;
E 5
I 5
    h = h << 1;
E 5
  }
E 3
  printf("\n");
}

D 3
void output(int id)
E 3
I 3
D 6
void
output(int id)
E 6
I 6
char *
output(int laser_number)
E 6
E 3
D 5
{  
E 5
I 5
{
  if(out_addr1 == NULL) set_address();
D 6

E 5
D 3
  switch (id) {
  case 1: { 
    hexbin(*out_addr1-0xffff0000); /* should be check!! */
    break;}
  case 2: { 
    hexbin(*out_addr2-0xffffff00);
    break;}
  case 3: {
    hexbin(*out_addr3-0xffffff00);
    break;}
  case 4: {
    hexbin(*out_addr4-0xffffff00);
      break;}
E 3
I 3
  switch(id)
  {
    case 1:
      hexbin(*out_addr1-0xffff0000); /* should be check!! */
      break;
    case 2:
      hexbin(*out_addr2-0xffffff00);
      break;
    case 3:
      hexbin(*out_addr3-0xffffff00);
      break;
    case 4:
      hexbin(*out_addr4-0xffffff00);
      break;
    default:
      ;
E 3
  }
E 6
I 6
  
  switch(laser_number)
    {
    case 1:  hexbin(*out_addr1 & 0xffff);  break;
    case 2:  hexbin(*out_addr2 & 0x00ff);  break;
    case 3:  hexbin(*out_addr3 & 0x00ff);  break;
    case 4:  hexbin(*out_addr4 & 0x00ff);  break;
    }
  return(result);
E 6
}

D 3
void input(int id)
E 3
I 3
D 6
void
input(int id)
E 6
I 6
char *
input(int laser_number)
E 6
E 3
{
I 5
  if(out_addr1 == NULL) set_address();
I 6
  
  switch(laser_number)
    {
    case 1:  hexbin(*in_addr1 & 0xffff);  break;
    case 2:  hexbin(*in_addr2 & 0x0fff);  break;
    case 3:  hexbin(*in_addr3 & 0x0fff);  break;
    case 4:  hexbin(*in_addr4 & 0x0fff);  break;
    }
  return(result);
}
E 6

E 5
D 3
  switch (id) {
  case 1: { 
    hexbin(*in_addr1-0xffff0000); /* should be check!! */
    break;}
  case 2: { 
    hexbin(*in_addr2-0xfffff000);
    break;}
  case 3: {
    hexbin(*in_addr3-0xfffff000);
    break;}
  case 4: {
    hexbin(*in_addr4-0xfffff000);
    break;}
E 3
I 3
D 6
  switch(id)
  {
    case 1:
      hexbin(*in_addr1-0xffff0000); /* should be check!! */
      break;
    case 2:
      hexbin(*in_addr2-0xfffff000);
      break;
    case 3:
      hexbin(*in_addr3-0xfffff000);
      break;
    case 4:
      hexbin(*in_addr4-0xfffff000);
      break;
E 3
  }
E 6
I 6
/*----------------------------------------------------------------*/
/*----------------------------------------------------------------*/

void
in_out_print(int laser_number)
{
  printf("%s;; <-- output(%d) \n", output(laser_number), laser_number);
  printf("%s;; <-- input(%d)  \n", input(laser_number),  laser_number);
E 6
}
I 5

E 5
E 1
