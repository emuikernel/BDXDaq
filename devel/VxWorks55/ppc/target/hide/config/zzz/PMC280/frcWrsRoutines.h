/******************************************************************************
*              (c), Copyright, MOTOROLA  INC - ECC                 *
*                                                                             *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MOTOROLA , INC.        *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT *
* OF MOTOROLA   OR ANY THIRD PARTY. MOTOROLA   RESERVES THE RIGHT AT ITS SOLE DISCRETION*
* TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MOTOROLA .       *
* THIS CODE IS PROVIDED "AS IS". MOTOROLA  MAKES NO WARRANTIES, EXPRESS,*
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.  *
******************************************************************************/

#ifndef _FRC_WRS_ROUTINES_H_
#define _FRC_WRS_ROUTINES_H_

/* Forward declarations */
STATUS frcmmuPpcPteGet( MMU_TRANS_TBL *, void *, PTE **);
void frcmmuPpcPteUpdate(PTE *, PTE *);

/* External functions */
void frcmmuPpcTlbie(void *);
void mmuPpcSdr1Set(unsigned int);
void frcEnableMMU();
void frcEnableMMUAndResetStackPointer();

#endif /* _FRC_WRS_ROUTINES_H_ */
