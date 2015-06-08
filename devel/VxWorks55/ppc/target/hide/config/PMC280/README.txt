/************************************************************************
*                Force Computers India Design Center                    *
*                      Bangalore, INDIA                                 *
*	Copyright (c) 2002 by FORCE COMPUTERS                           *
*                                                                       *
* All rights reserved. Copying, compilation, modification, distribution *
* or any other use whatsoever of this material is strictly prohibited   *
* without written permission of FORCE COMPUTERS India Design Center     *
*************************************************************************
*									
* Project Name : PPMC-280.		
*									
* Created : 31/OCT/2005 
* Author  : Saravanan Gopu
* Revision: T2.2/3.5
* State   : Final.
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
* Non Monarch Mode : Device No has been assigned to 31 for Scan support.
	
Limitations and Caveats:
------------------------
* Simultaneous access of I2C devices( EEPROM, RTC) from both CPUs is not 
  supported. Application must provide a mechanism to synchronize such accesses. 

Known issues/bugs:
------------------
* The VPD read and write from external host to PMC280 will not work. 

-----------------------------------------------------------------------
