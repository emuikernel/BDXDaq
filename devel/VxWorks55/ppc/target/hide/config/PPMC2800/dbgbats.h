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

#ifndef _DBGBATS_H_
#define _DBGBATS_H_

extern unsigned int read_ibat0u(void);
extern unsigned int read_ibat0l(void);
extern unsigned int read_ibat1u(void);
extern unsigned int read_ibat1l(void);
extern unsigned int read_ibat2u(void);
extern unsigned int read_ibat2l(void);
extern unsigned int read_ibat3u(void);
extern unsigned int read_ibat3l(void);
extern unsigned int read_ibat4u(void);
extern unsigned int read_ibat4l(void);
extern unsigned int read_ibat5u(void);
extern unsigned int read_ibat5l(void);
extern unsigned int read_ibat6u(void);
extern unsigned int read_ibat6l(void);
extern unsigned int read_ibat7u(void);
extern unsigned int read_ibat7l(void);

extern unsigned int read_dbat0u(void);
extern unsigned int read_dbat0l(void);
extern unsigned int read_dbat1u(void);
extern unsigned int read_dbat1l(void);
extern unsigned int read_dbat2u(void);
extern unsigned int read_dbat2l(void);
extern unsigned int read_dbat3u(void);
extern unsigned int read_dbat3l(void);
extern unsigned int read_dbat4u(void);
extern unsigned int read_dbat4l(void);
extern unsigned int read_dbat5u(void);
extern unsigned int read_dbat5l(void);
extern unsigned int read_dbat6u(void);
extern unsigned int read_dbat6l(void);
extern unsigned int read_dbat7u(void);
extern unsigned int read_dbat7l(void);

#endif /* _DBGBATS_H_ */
