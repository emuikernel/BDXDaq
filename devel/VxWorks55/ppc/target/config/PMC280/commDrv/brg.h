/******************************************************************************
*              (c), Copyright, FORCE COMPUTERS INDIA Limited                  *
*                                                                             *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF FORCE COMPUTERS, INC.        *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT *
* OF FORCE OR ANY THIRD PARTY. FORCE RESERVES THE RIGHT AT ITS SOLE DISCRETION*
* TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO FORCE COMPUTERS.       *
* THIS CODE IS PROVIDED "AS IS". FORCE COMPUTERS MAKES NO WARRANTIES, EXPRESS,*
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.  *
*******************************************************************************


*******************************************************************************
* brg.h - Header File for : BRG engine.
*
* DESCRIPTION:
*		None.
*
* DEPENDENCIES:
*		None.
*
******************************************************************************/

#ifndef __INCbrgh
#define __INCbrgh

/* includes */
#include "gtCore.h"
#include "brg.h"


#ifdef __cplusplus
extern "C" {
#endif

#if ((CPU_FAMILY==I960) && (defined __GNUC__))
#pragma align 1                 /* tell gcc960 not to optimize alignments */
#endif	/* CPU_FAMILY==I960 */


/* defines  */

/* BRG Configuration Register (BCR) */
#define BCR_CDV_MASK                0x0000FFFF
#define BCR_EN_ENABLE_BRG           0x00010000
#define BCR_RST_RESET_BRG           0x00020000
#define BRG_CLK_SRC_BCLKIN          0x00000000
#define BRG_CLK_SRC_SCLK0           0x00080000
#define BRG_CLK_SRC_TSCLK0          0x000c0000
#define BRG_CLK_SRC_SCLK1           0x00180000
#define BRG_CLK_SRC_TSCLK1          0x001c0000
#define BRG_CLK_SRC_SYSCLK          0x00200000

#define BRG_MAX_CDV					0x0000FFFF

/* typedefs */
typedef enum _brgNum
{
	BRG_0,
	BRG_1
}BRG_NUM;

typedef enum _brgClkSrc
{
	BCLKIN,
	SCLK0 ,
	TSCLK0,
	SCLK1 ,
	TSCLK1,
	SYSCLK,

}BRG_CLK_SRC;

typedef struct
{
    BRG_NUM             brgNum;         /* Number of BRG. See BRG_NUM  		 */
    BRG_CLK_SRC         brgSrcClk;  	/* BRG clock source. See BRG_CLK_SRC */
    unsigned int        brgSrcClkRate;	/* Clock source rate.				 */
    unsigned int        brgConfigReg;  	/* BRG configuration register value	 */
}BRG_ENGINE;


/* brg.h API list */
void brgInit  (BRG_ENGINE *pBrg);
void brgStart (BRG_ENGINE *pBrg);
void brgSetCdv(BRG_ENGINE *pBrg, unsigned short brgCdv);

#ifdef __cplusplus
}
#endif

#endif /* __INCbrgh */

