/******************************************************************************
*              (c), Copyright, MOTOROLA  INC - ECC                 *
*                                                                             *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MOTOROLA , INC.        *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT *
* OF MOTOROLA   OR ANY THIRD PARTY. MOTOROLA   RESERVES THE RIGHT AT ITS SOLE DISCRETION*
* TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MOTOROLA .       *
* THIS CODE IS PROVIDED "AS IS". MOTOROLA  MAKES NO WARRANTIES, EXPRESS,*
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.  *
*******************************************************************************

*******************************************************************************
* configNet.h - Header File for : END Network configuration 
*
* DESCRIPTION:
*       This file defines the network devices that can be attached to the MUX.
*	This file main purpose to prepare endDevTbl [] table which describes
*	the ethernet devices that will be attached to the MUX.
*
* DEPENDENCIES:
*       WRS endLib library.
*
******************************************************************************/

#ifndef __INCconfigNeth
#define __INCconfigNeth

/* includes */
#include "vxWorks.h"
#include "end.h"
#include "config.h"


/* defines  */

/* Intel's NIC device support */
#ifdef INCLUDE_END_FEI
#define FEI_LOAD_FUNC	    frcFei82557EndLoad    
#define FEI_BUFF_LOAN       1   	/*we support buffer loan*/
#define  FEI_LOAD_STRING    "-1:0:0x20:0x64:0x00"
IMPORT END_OBJ* FEI_LOAD_FUNC(char*, void*);  
#endif


/* Ethernet Port 0,1,2 devices support.	                                    */
/* The mgi string defines the following:                                    */
/* "<portNum>:<portMacAddr>:<memBase>:<memSize>:<nCFDs>:<nRFDs>:<flags>"    */
#ifdef INCLUDE_END_MGI
    #define MGI_FUNC		mgiEndLoad
    #define MGI0_STRING	"0:00-23-45-67-89-ab:-1:0:64:c8:0"
    #define MGI1_STRING	"1:00-11-22-33-44-55:-1:0:64:c8:0"
    IMPORT END_OBJ* MGI_FUNC(char*, void*);
#endif


#ifdef INCLUDE_GEI_82543_END
#define GEI82543_LOAD_FUNC     gei82543EndLoad  /* driver external interface */
#define GEI82543_BUFF_LOAN     1                /* enable buffer loan */
/*#define GEI82543_LOAD_STRING_0 geiEndLoadStr[0] */
IMPORT END_OBJ* GEI82543_LOAD_FUNC (char*, void*);
#endif
	
/* max number of ipAttachments we can have */
#undef	IP_MAX_UNITS
#define IP_MAX_UNITS (NELEMENTS (endDevTbl) - 1)


END_TBL_ENTRY endDevTbl [] =
{

#ifdef INCLUDE_END_MGI
	{ 0, MGI_FUNC, MGI0_STRING, 1, NULL, FALSE},
	{ 1, MGI_FUNC, MGI1_STRING, 1, NULL, FALSE},
#endif /* INCLUDE_END_MGI */

#ifdef INCLUDE_END_FEI
/*    { 0, FEI_LOAD_FUNC, FEI_LOAD_STRING, FEI_BUFF_LOAN, NULL, FALSE},*/

{ 0, END_TBL_END, NULL, 0, NULL, FALSE},		/* End of END device table */
      { 0, END_TBL_END, NULL, 0, NULL, FALSE},		/* End of END device table */

#endif 
#ifdef INCLUDE_GEI_82543_END
    { 0, END_TBL_END, NULL, 0, NULL, FALSE},
#endif	
    
    { 0, END_TBL_END, NULL, 0, NULL, FALSE}	/* End of END device table */
};

#endif /* __INCconfigNeth */




