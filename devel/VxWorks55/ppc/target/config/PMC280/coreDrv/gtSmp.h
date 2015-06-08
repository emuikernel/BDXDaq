/******************************************************************************
*              (c), Copyright, FORCE COMPUTERS INDIA Limited                  *
*                                                                             *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF FORCE COMPUTERS, INC.        *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT *
* OF FORCE OR ANY THIRD PARTY. FORCE RESERVES THE RIGHT AT ITS SOLE DISCRETION*
* TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO FORCE COMPUTERS.       *
* THIS CODE IS PROVIDED "AS IS". FORCE COMPUTERS MAKES NO WARRANTIES, EXPRESS,*
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.  *
******************************************************************************/

#ifndef _GT_SMP_H_
#define _GT_SMP_H_

#define MV64360_SMP_NO_WAIT		0x0000
#define MV64360_SMP_WAIT_FOREVER        0x0001

#define MPSC_SEM			0x0
#define CNTMR_SEM			0x1
#define GPP_SEM				0x2
#define DMA_SEM				0x3

extern int frcMv64360semTake(unsigned int sem_reg_no, unsigned int timeout);
extern int frcMv64360semGive(unsigned int sem_reg_no);
extern int frcWhoAmI();
#endif /* _GT_SMP_H_ */