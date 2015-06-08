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

#include <stdio.h>
#include <vxWorks.h>
#include "./../../coreDrv/gtCore.h"
#include "./../../coreDrv/gt64360r.h"
#include "./../../gtIntControl.h"
#include "./../../coreDrv/vxGppIntCtrl.h" 
  
#ifdef K2_DUAL_CPU
#include "gtSmp.h"
#endif /* K2_DUAL_CPU */

/******************************************************************************
* TestIntConnect: This routine connects the particular testisr to the input   *
* MPP pin which is wired to an output MPP pin which is asserted in software   *
* to cause an interrupt.                                                      *         
*                                                                             * 
* Input                                                                       *
*   input : The input MPP number to which an ISR is connected.                *       
*   Output: The output MPP number which is wired to the input that is used to *
*   testIsr :The test Interrupt service routine which services the interrupt  *
*   testIsrArg: The argument to the interrupt service routine                 *
*                                                                             *
* Output:                                                                     *
*     N/A                                                                     *
*            .                                                                *
* Returns:                                                                    *  
*    OK    if successful                                                      *
*    ERROR if unsuccessfull                                                   *
*******************************************************************************/ 
STATUS TestIntConnect(int input,int output,VOIDFUNCPTR testIsr,int testIsrArg)
{
   UINT32 temp;
   int output1,input1;
   STATUS status; 
    /* Make Output as GPP */
    if( 0<=output && output<= 7 ) 
    {  
       
       GT_REG_READ(MPP_CONTROL0,&temp);
       temp = temp & (~(0xf<<(output*4)));
       GT_REG_WRITE(MPP_CONTROL0,temp);
       
    }
    else if(8<=output && output<=15)
    { 
       output1=output-8;
       GT_REG_READ(MPP_CONTROL1,&temp);
       temp = temp & (~(0xf<<(output1*4)));
       GT_REG_WRITE(MPP_CONTROL1,temp);
           
    }
    else if(16<=output && output<=23)
    {  
       output1=output-16;  
       GT_REG_READ(MPP_CONTROL2,&temp);
       temp = temp & (~(0xf<<(output1*4)));
       GT_REG_WRITE(MPP_CONTROL2,temp); 
         
    }
    else if(24<=output && output<=31)
    {  
       output1=output-24;
       GT_REG_READ(MPP_CONTROL3,&temp);
       temp = temp & (~(0xf<<(output1*4)));
       GT_REG_WRITE(MPP_CONTROL3,temp);
       
    }
    
   
    /* Make output as output pin */
    GT_REG_READ(GPP_IO_CONTROL,&temp);
    temp = temp | (0x1<<output);
    GT_REG_WRITE(GPP_IO_CONTROL,temp);
    

    /* Make output as active low */
    GT_REG_READ(GPP_LEVEL_CONTROL, &temp);
    temp = temp | (0x1<<output);
    GT_REG_WRITE(GPP_LEVEL_CONTROL,temp);  
    
     /* Make Input MPP as GPP */
    if( 0<= input && input<=7) 
    {
        
        GT_REG_READ(MPP_CONTROL0,&temp);
        temp = temp & (~(0xf<<(input*4)));
        GT_REG_WRITE(MPP_CONTROL0,temp);
        
    }
    else if(8<=input && input<=15)
    {  
        input1=input-8;
        GT_REG_READ(MPP_CONTROL1,&temp);
        temp = temp & (~(0xf<<(input1*4)));
        GT_REG_WRITE(MPP_CONTROL1,temp); 
       
    }
    else if(16<=input&& input<=23)
    {  
        input1=input-16;
        GT_REG_READ(MPP_CONTROL2,&temp);
        temp = temp & (~(0xf<<(input1*4)));
        GT_REG_WRITE(MPP_CONTROL2,temp);
        
    }
    else if(24<=input && input<=31)
    {  
        input1=input-24;
        GT_REG_READ(MPP_CONTROL3,&temp);
        temp = temp & (~(0xf<<(input1*4)));
        GT_REG_WRITE(MPP_CONTROL3,temp);
        
    }

   
    /* Make input as input pin */
    GT_REG_READ(GPP_IO_CONTROL,&temp);
    temp = temp & (~(0x1<<input));
    GT_REG_WRITE(GPP_IO_CONTROL,temp);
    

    /* Make input as active low */
    GT_REG_READ(GPP_LEVEL_CONTROL, &temp);
    temp = temp | (0x1<<input);
    GT_REG_WRITE(GPP_LEVEL_CONTROL,temp);
    
    
    status=frcGppIntConnect(input, testIsr,testIsrArg,9);
    
    return(status);
    
}
/********************************************************************************
* testIsr :The test service routine that is invoked everytime that the interrupt*
* is asserted.This routine also services the interrupt by deasserting it.       *
*                                                                               *
* INPUT:                                                                        *
*    int output The output pin that must be deasserted in the ISR               *
*                                                                               * 
* OUTPUT:                                                                       * 
*    Services the interrupts                                                    *
*                                                                               *
* RETURNS:                                                                      *
*    N/A                                                                        *
*********************************************************************************/                                                                             
void testIsr(int output)
{
   UINT32 temp1;

   GT_REG_READ(GPP_VALUE,&temp1);
   temp1 = temp1 & (~(0x1<<output));
   GT_REG_WRITE(GPP_VALUE,temp1);
   
   logMsg("Entered TestIsr \n",1,2,3,4,5,6 );

  
}
/******************************************************************************
* frcTestConnect:This routine passes the control to the TestIntConnect which   *
* configures the input and output pins and also hooks the ISR to the input pin.*
*                                                                              *
*INPUT:                                                                        *
*   int input  input pin connected to the ISR                                  *                               *
*   int output output pin wired back to the input                              *                                 *
*                                                                              *
* OUTPUT:                                                                      *
*   N/A                                                                        *
*******************************************************************************/
void frcTestConnect(int input,int output)    
{    
   TestIntConnect(input,output,(VOIDFUNCPTR)testIsr, output);

}
/* This routine disables the Input pin */
void frcTestDisable(int input)
{
  frcGppIntDisable(input);
}
/*******************************************************************************
* frcTestSwitch :This routine generates the software interrupt and enables the *
* input pin
********************************************************************************/
void frcTestSwitch(int input,int output)
{
UINT32 temp,tocpu=0;
#ifdef K2_DUAL_CPU
    if(frcWhoAmI())
       tocpu=1;
    else
       tocpu=0;
#endif 
 
   if(tocpu)
   {
                 
        if(0<=input && input<= 7)
        {    
            /* Generate a software interrupt by setting output high */
            GT_REG_READ(GPP_VALUE,&temp);
            temp = temp | (0x1<<output);
            GT_REG_WRITE(GPP_VALUE,temp);                               
            gtIntEnable(P1_GPP_7_0);                                       
            frcGppIntEnable(input);
                           
        }
       else if(8<=input && input<=15)
        {                 
             /* Generate a software interrupt by setting output high */
            GT_REG_READ(GPP_VALUE,&temp);
            temp = temp | (0x1<<output);
            GT_REG_WRITE(GPP_VALUE,temp);         
            gtIntEnable(P1_GPP_15_8);                        
            frcGppIntEnable(input);                
        }
       else if(16<=input && input<=23)
        {  
             /* Generate a software interrupt by setting output high */
            GT_REG_READ(GPP_VALUE,&temp);
            temp = temp | (0x1<<output);
            GT_REG_WRITE(GPP_VALUE,temp);         
            gtIntEnable(P1_GPP_23_16);                        
            frcGppIntEnable(input);              
        }
       else if(24<=input && input<=31)
        {  
            /* Generate a software interrupt by setting output high */
            GT_REG_READ(GPP_VALUE,&temp);
            temp = temp | (0x1<<output);
            GT_REG_WRITE(GPP_VALUE,temp);         
            gtIntEnable(P1_GPP_31_24);	                
            frcGppIntEnable(input);
              
        }               
       
    }
   else
    {  
       /* Generate a software interrupt by setting output high */
       GT_REG_READ(GPP_VALUE,&temp);
       temp = temp | (0x1<<output);
       GT_REG_WRITE(GPP_VALUE,temp);                   
       frcGppIntEnable(input);                   
    }
    
} 
