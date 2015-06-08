/******************************************************************************
*              (c), Copyright, MOTOROLA  INC - ECC                  *
*                                                                             *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MOTOROLA , INC.        *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT *
* OF MOTOROLA  OR ANY THIRD PARTY. MOTOROLA  RESERVES THE RIGHT AT ITS SOLE DISCRETION*
* TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MOTOROLA .       *
* THIS CODE IS PROVIDED "AS IS". MOTOROLA  MAKES NO WARRANTIES, EXPRESS,*
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.  *
*******************************************************************************

*******************************************************************************
* vxWatchDog.c - MV Watchdog Timer Driver
*
* DESCRIPTION:
*       The MV internal watchdog timer is a 32-bit count down counter that can 
*       be used to generate a non-maskable interrupt or reset the system in the
*       event of unpredictable software behavior.
*       After the watchdog is enabled, it is a free running counter that needs
*       to be serviced periodically in order to prevent its expiration.
*       This driver provide API for full domination over the WD.
*
*   The API includes:
*       frcWatchdogLoad()    - Load WD counter with a new value
*       frcWatchdogService()	- WD service
*       frcWatchdogNMILoad()	- Load WD value register with NMI_VAL
*       frcWatchdogEnable()	- Enable WD operation
*       frcWatchdogDisable()	- Disable WD operation
*
*   Note: In order to use the NMI correctly, make sure to service the WD (using 
*         the frcWatchdogService routine) in the NMI connected to this driver. 
*         Avoiding this service will hang the system.
*   Note: In order to have the watchDog facility function correctly make sure 
*         the value of the watchdog timer is greater than the preset value 
*         which is used for the invokation of the NMI. Avoiding this will cause 
*         the watchdog to assert WDE interrupt (reset the system) prior to the 
*         NMI event.
*   Note: In order to be able to receive NMI the user must make HW changes to 
*         the development board. The watchdog facility generates the NMI in GPP
*         pin 10 (output). This pin is short to GPP pin 24 (input) using RNC8.
*         Only after making this change the user will be able to see the NMI on
*         GPP pin 24.
*
* DEPENDENCIES:
*       None.
*
******************************************************************************/

/* includes */
#include "vxGppIntCtrl.h"   
#include "vxWatchDog.h"   
#include "stdio.h"

/* locals   */

/* globals */
UINT32 preset_val = 0;
/******************************************************************************
* frcWatchdogInit()	- initialize WD counter 
* This routine initiates the MPP and GPP facilities to have NMI interrupt on 
* GPP pin 24. This routine also connects the user ISR for the NMI using the
* GPP driver routine frcGppIntConnect();. 
*
* Input:
*       VOIDFUNCPTR nmiIsr  - user defined NMI service routine.
*       int nmiIsrArg       - Argument to the above routine.
*                                              
* Output:
*       The MPP + GPP facilities are now ready for WD interrupt reception
*       and user defined NMI service routine is connected to the GPP interrupt
*       controller.
*
* RETURNS: N/A
******************************************************************************/

void frcWatchdogInit() {
    UINT32 temp;
     
    /*Make sure MPP[19] is set to WDE*/
    GT_REG_READ(MPP_CONTROL2,&temp);
    temp = temp & 0xFFFF0fff;
    temp = temp | 0x00004000;
    GT_REG_WRITE(MPP_CONTROL2,temp); 

}

/******************************************************************************
* frcWatchdogLoad()	- Load WD counter with a new value
* This function loads a value into the 24 most significant bits of the Watchdog 
* Configuration Register, each time it is enabled or serviced.
* 
* Inputs: value - 24 bit wide number 
*
* Output: The value is loaded into the 24 lsb of Watchdog Configuration Reg.
*                                              
* RETURNS: true if output succeeded              
*          false if argument is invalid.
******************************************************************************/

bool frcWatchdogLoad(unsigned int value)   {

    if (value > PRESET_VALUE_FIELD) {
	printf("frcWatchdogLoad: Value is more than the maximum allowed\n");
	return false;
    }
    preset_val=value;

    GT_REG_WRITE(WATCHDOG_CONFIG_REG, preset_val);

    return true;
}

/******************************************************************************
* frcWatchdogService() - WD service and NMI interrupt acknowledge.
*                       This function service the WD in order to avoid NMI or 
*                       reset (WDE#). Watchdog service is performed by 
*                       writing ‘01’ to CTL2, followed by writing 
*                       ‘10’ to CTL2. Upon watchdog service, the GT 
*                       clears the NMI bits (if set) and reloads the 
*                       Preset_VAL into the watchdog counter.
* Inputs: None
*
* Output: The Preset_VAL is reload.
*
* RETURNS: N/A
******************************************************************************/

void frcWatchdogService()   {
    WATCHDOG_CONFIG config;

    config = preset_val | 0x04000000; /* Set CTL2 field with 1nd service sequence */
    GT_REG_WRITE(WATCHDOG_CONFIG_REG, config);

    config = preset_val | 0x08000000; /* Set CTL2 field with 2nd service sequence */
    GT_REG_WRITE(WATCHDOG_CONFIG_REG, config); 


}

/******************************************************************************
* frcWatchdogNMILoad() - Load WD value register with a new value (NMI_VAL)
*                       This function loads a value into the Watchdog Value 
*                       Register. This value is the 24 least significant bits 
*                       of a 32 bit value. When the WD counter reaches a value
*                       euqal to NMI_VAL an NMI interrupt is generated. 
* 
* Inputs: value - 24 bit wide number 
*
* Output: The value is loaded into the 24 lsb of Watchdog Value Register.
*                                              
* RETURNS: true if output succeeded              
*          false if argument is invalid.
******************************************************************************/

bool frcWatchdogNMILoad(unsigned int value)
{
    if(value>PRESET_VALUE_FIELD)
        return false;
    GT_REG_WRITE(WATCHDOG_VALUE_REG, value);
    return true;
}

/******************************************************************************
* frcWatchdogEnable() - Enable WD operation
*                      This function enables the WD operation.A write sequence
*                      of ‘01’ followed by ‘10’ into CTL1 
*                      disables/enables the watchdog. The watchdog’s current
*                      status can be read in bit 31 of WDC. When disabled, the
*                      GT clears the NMI bits (if set) and reloads the 
*                      Preset_VAL into the watchdog counter.
*
* Inputs: None
*
* Output: The Disabled WD is now Enabled.
*
* RETURN: N/A
******************************************************************************/

void frcWatchdogEnable()
    {
    WATCHDOG_CONFIG config,prev;

	GT_REG_READ(WATCHDOG_CONFIG_REG, &prev);

	if (prev & 0x80000000){
		printf ("\nWatchDog already Enabled \n");
		return ;
	}else {
	
	config = preset_val | 0x01000000; /* Set CTL1 field with 1st service 
                                         sequence */ 
	GT_REG_WRITE(WATCHDOG_CONFIG_REG, config);
	config = preset_val | 0x02000000; /* Set CTL1 field with 1st service 
                                         sequence */
	GT_REG_WRITE(WATCHDOG_CONFIG_REG, config);
	}
}
    
/******************************************************************************
* frcWatchdogDisable() - Disable WD operation
*                       This function Disables the WD operation. A write 
*                       sequence of ‘01’ followed by ‘10’ into 
*                       CTL1 disables/enables the watchdog. The watchdog’s 
*                       current status can be read in bit 31 of WDC. When 
*                       disabled, the GT clears the NMI bits (if set) and 
*                       reloads the Preset_VAL into the watchdog counter.
*
* Inputs: None
*
* Output: The Enabled WD is now Disabled.
*
* RETURN: N/A
******************************************************************************/

void frcWatchdogDisable()
    {
    WATCHDOG_CONFIG config,prev;

	GT_REG_READ(WATCHDOG_CONFIG_REG, &prev);

	if (!(prev & 0x80000000)){
		printf ("\nWatchDog already Disabled \n");
		return ;
	}else {
		
	config = preset_val | 0x01000000; /* Set CTL1 field with 1st service 
                                         sequence */ 
	GT_REG_WRITE(WATCHDOG_CONFIG_REG, config);
	config = preset_val | 0x02000000; /* Set CTL1 field with 1st service 
                                         sequence */
	GT_REG_WRITE(WATCHDOG_CONFIG_REG, config);
	}

}
