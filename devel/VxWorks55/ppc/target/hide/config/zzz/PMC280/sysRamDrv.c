/******************************************************************************
*              (c), Copyright, MOTOROLA  INC  - ECC                  *
*                                                                             *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MOTOROLA , INC.        *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT *
* OF MOTOROLA   OR ANY THIRD PARTY. MOTOROLA   RESERVES THE RIGHT AT ITS SOLE DISCRETION*
* TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MOTOROLA .       *
* THIS CODE IS PROVIDED "AS IS". MOTOROLA  MAKES NO WARRANTIES, EXPRESS,*
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.  *
*******************************************************************************

*******************************************************************************
                        MOTOROLA   India Bangalore
Author : Champak Nath

Description:
This file was added to the tree to create a RAM disk on PMC280.
The RAM disk could be used to measure the through put of the ethernets.

Created: March 13 2002
******************************************************************************/

#include "ramDrv.h"
#include "dosFsLib.h"
#include "stdio.h"

IMPORT BLK_DEV * ramDevCreate (char *,int,int,int,int);

/****************************************************************************  
    Function : sysRamDrvConfig 
    Description: A 30MB of RAM disk is created with 4 tracks,
                 15360 blocks per track and 512 bytes per block 
    IN : --
    OUT: OK or ERROR
*****************************************************************************/
STATUS sysRamDrvConfig (void)
    {
	BLK_DEV *pRbd0; /* Pointer to RAM disk block device */

	pRbd0 = ramDevCreate ((char *)0,512,15360,61440,0);
	if ( pRbd0 == NULL)
	  {
	  	printf("Could not create RAM disk\n");
	  	return (ERROR);
	  }

	if ((dosFsMkfs  ("/rd0/", pRbd0) == NULL) )
            {
            printf("Could not create dosFs file system on RAM disk\n");
            return (ERROR);
            }
            else
            printf("dosFs file system created on RAM disk and available at /rd0/.\n");
	return(OK);
    }
