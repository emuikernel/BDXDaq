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
*									
* Project Name : PrPMC-2800.		
*									
* Created : 05/JULY/2006 
* Author  : BSP
* Revision: T2.2/5.0
* State   : Internal.
* ----------------------------------------------------------------------
*

Contents:
---------
* Tornado Host development environment
* Building VxWorks
* Changes in this Release
* Limitations and Caveats
* Known issues/bugs


Tornado Host development environment:
-------------------------------------
The sources are buildable on a Solaris machine which runs Tornado2.2

Building VxWorks:
-----------------
Refer to HOWTO.txt 

Changes in this Release:
------------------------
* Drivers for the User Flash, BootFlash have been updated for the new  ROHS Devices
*  I2c Driver have been updated for higher throughput .
* BIB driver is updated to version 2.1  	
Limitations and Caveats:
------------------------
* Simultaneous access of I2C devices( EEPROM, RTC) from both CPUs is not 
  supported. Application must provide a mechanism to synchronize such accesses. 

Known issues/bugs:
------------------
* The VPD read and write from external host to PMC280 will not work. 

-----------------------------------------------------------------------
