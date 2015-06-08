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
