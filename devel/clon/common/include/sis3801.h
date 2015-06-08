/***************************************************************************
                          sis3801.h   Scaler 3801 GmbH
                             -------------------
    begin                : Mon Sep 9 09:15:00 UTC 2002
    email                : tanest@jlab.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/ 

struct fifo_scaler{
  volatile unsigned long csr;
  volatile unsigned long irq;
  volatile unsigned long clear;
  volatile unsigned long next;
  volatile unsigned long enable;
  volatile unsigned long disable;
  volatile unsigned long reset;
  volatile unsigned long test;
  volatile unsigned long fifo[64];
  volatile unsigned long scaler;
  volatile unsigned long rclear[32];
};

#define ENABLE_EXT_NEXT   0x00010000
#define DISABLE_EXT_NEXT  0x01000000
#define ENABLE_EXT_CLEAR  0x00020000
#define DISABLE_EXT_CLEAR 0x02000000
#define ENABLE_EXT_DIS    0x00040000
#define DISABLE_EXT_DIS   0x04000000

#define ENABLE_MODE0      0x00000000
#define ENABLE_MODE1      0x00000004
#define ENABLE_MODE2      0x00000008
#define ENABLE_MODE3      0x0000000c
#define DISABLE_MODES     0x00000c00

#define ENABLE_TEST       0x00000020
#define DISABLE_TEST      0x00002000

#define ENABLE_25MHZ      0x00000010
#define DISABLE_25MHZ     0x00001000

#define ENABLE_IRQ        0x00f00000
#define ENABLE_IRQ0       0x00100000
#define ENABLE_IRQ1       0x00200000
#define ENABLE_IRQ2       0x00400000
#define ENABLE_IRQ3       0x00800000

#define DISABLE_IRQ       0xf0000000
#define DISABLE_IRQ0      0x10000000
#define DISABLE_IRQ1      0x20000000
#define DISABLE_IRQ2      0x40000000
#define DISABLE_IRQ3      0x80000000

/* interupt vector */
#define SCAL_VME_INT_LEVEL   5
#define SCAL_INT_VEC         0xda
/* masks for status register */

#define FIFO_FULL         0x00001000
#define FIFO_ALMOST_FULL  0x00000800
#define FIFO_HALF_FULL    0x00000400
#define FIFO_ALMOST_EMPTY 0x00000200
#define FIFO_EMPTY        0x00000100

