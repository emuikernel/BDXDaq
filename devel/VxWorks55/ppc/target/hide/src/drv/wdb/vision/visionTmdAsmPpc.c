/* visionTmdAsmPpc.c - Wind River Vision Transparent Mode Device Driver */

/* Copyright 1988-2002 Wind River Systems Inc. */

/*
modification history
--------------------
01j,13jul02,tcr  Added support for the 440.
01i,30jan02,g_h  Correct non-portably written assembly that cause
		 problem with GNU
01h,28jan02,g_h  Correct syntax error in tmdPPC604SignalTxReady()
01g,21dec01,g_h  Rename to visionTmdAsmPpc.c
01f,30nov01,g_h  Cleaning for T2.2
01e,07feb01,g_h  renaming module name and cleaning
01d,11may01,g_h  Add support for PPC40x
01c,26apr01,g_h  Add support for MPC755
01b,07feb01,g_h  renaming module name and cleaning
01a,07may97,est  Adapted from memdrv.c
*/

/*
DESCRIPTION
This driver .....

*/

#include "vxWorks.h"
#include "drv/wdb/vision/visionTmdDrv.h"

/* externals */

IMPORT unsigned long txDesc; /* global copies for rx/tx */
IMPORT unsigned long rxDesc; /* emulator descriptors    */

#if ((CPU == PPC603) || (CPU == PPC860))
/***************************************************************************
*
* tmdPPCSignalDescReady - inform emulator the location of Rx/Tx descriptors
*
* This routine inform emulator the location of Rx/Tx descriptors.
*
* RETURNS: N/A
*/

void tmdPPCSignalDescReady
    (
    void
    )
    {
    _WRS_ASM("      lis   3,txDesc@h         ");
    _WRS_ASM("      ori   3,3,txDesc@l       ");
    _WRS_ASM("      lwz   4,0(3)             ");

    _WRS_ASM("      lis   3,rxDesc@h         ");
    _WRS_ASM("      ori   3,3,rxDesc@l       ");
    _WRS_ASM("      lwz   5,0(3)             ");

    _WRS_ASM("      lis   6,txBdmBgnd@h      ");
    _WRS_ASM("      ori   6,6,txBdmBgnd@l    ");

    _WRS_ASM("      lis   3,0x5C5C5C5C@h     ");
    _WRS_ASM("      ori   3,3,0x5C5C5C5C@l   ");
    _WRS_ASM("      .long 0x03FFFFF1         ");
    _WRS_ASM("      xor   3,3,3              ");
    }
#endif /* ((CPU == PPC603) || (CPU == PPC860)) */

#if (CPU == PPC604)
/***************************************************************************
*
* tmdPPC604SignalDescReady - inform emulator the loaction of Rx/Tx 
*                            descriptors
*
* This routine inform emulator the location of Rx/Tx descriptors
*
* RETURNS: N/A
*/

void tmdPPC604SignalDescReady
    (
    void
    )
    {
    _WRS_ASM("      lis   3,txDesc@h         ");
    _WRS_ASM("      ori   3,3,txDesc@l       ");
    _WRS_ASM("      lwz   4,0(3)             ");

    _WRS_ASM("      lis   3,rxDesc@h         ");
    _WRS_ASM("      ori   3,3,rxDesc@l       ");
    _WRS_ASM("      lwz   5,0(3)             ");

    _WRS_ASM("      lis   6,txBdmBgnd@h      ");
    _WRS_ASM("      ori   6,6,txBdmBgnd@l    ");

    _WRS_ASM("	    mfspr 7,1010             ");
    _WRS_ASM("      mfspr 3,1010             ");
    _WRS_ASM("      mfmsr 8                  ");
    _WRS_ASM("	    andi. 8,8,0x0040         ");
    _WRS_ASM("	    cmpwi 8,0                ");
    _WRS_ASM("      beq   setVectorLow       ");
    _WRS_ASM("      lis   3,0xFFF0           ");
    _WRS_ASM("	    ori   3,3,0x0702         ");
    _WRS_ASM("	    b     next               ");
    _WRS_ASM("setVectorLow:                  ");
    _WRS_ASM("      lis   3,0x0000           ");
    _WRS_ASM("      ori   3,3,0x0702         ");
    _WRS_ASM("next:                          ");
    _WRS_ASM("      mtspr 1010,3             ");

    _WRS_ASM("      lis   3,0x5C5C5C5C@h     ");
    _WRS_ASM("      ori   3,3,0x5C5C5C5C@l   ");
    _WRS_ASM("     .long 0x03FFFFF1          ");
    _WRS_ASM("      xor   3,3,3              ");

    _WRS_ASM("	    mtspr 1010,7             ");
    }
#endif /* (CPU == 604) */

#if (CPU == PPC405)
/***************************************************************************
* 
* tmdPPC405SignalDescReady - inform emulator the location of Rx/Tx 
*                            Descriptors
*
* This routine inform emulator the location of Rx/Tx Descriptors.
*
* RETURNS: N/A
*/

void tmdPPC405SignalDescReady
    (
    void
    )
    {
    _WRS_ASM("      lis   3,txDesc@h         ");
    _WRS_ASM("      ori   3,3,txDesc@l       ");
    _WRS_ASM("      lwz   4,0(3)             ");

    _WRS_ASM("      lis   3,rxDesc@h         ");
    _WRS_ASM("      ori   3,3,rxDesc@l       ");
    _WRS_ASM("      lwz   5,0(3)             ");

    _WRS_ASM("      lis   6,txBdmBgnd@h      ");
    _WRS_ASM("      ori   6,6,txBdmBgnd@l    ");
                     
    _WRS_ASM("      lis   3,0x5C5C5C5C@h     ");
    _WRS_ASM("      ori   3,3,0x5C5C5C5C@l   ");

    _WRS_ASM("      mfspr 7, 0x3F2           "); /* save DBCR */
    _WRS_ASM("      mfspr 8, 0x3F2           ");
    _WRS_ASM("      oris  8,8, 0x8100        "); /* set Ext Debug and Trap Exception bits */
    _WRS_ASM("      mtspr 0x3F2, 8           "); /* update DBCR	*/
    _WRS_ASM("      .long 0x7FE00008         "); /* Force a trap exception to occur */    
    _WRS_ASM("      mtspr 0x3f2, 7           "); /* restore original DBCR */		
    _WRS_ASM("      xor   3,3,3              ");

    _WRS_ASM("endDescReady:                  ");
    }
#endif /* (CPU == PPC405)  */

#if (CPU == PPC440)
/***************************************************************************
* 
* tmdPPC440SignalDescReady - inform emulator the location of Rx/Tx 
*                            Descriptors
*
* This routine inform emulator the location of Rx/Tx Descriptors.
*
* RETURNS: N/A
*/

void tmdPPC440SignalDescReady
    (
    void
    )
    {
    _WRS_ASM("      lis   3,txDesc@h         ");
    _WRS_ASM("      ori   3,3,txDesc@l       ");
    _WRS_ASM("      lwz   4,0(3)             ");

    _WRS_ASM("      lis   3,rxDesc@h         ");
    _WRS_ASM("      ori   3,3,rxDesc@l       ");
    _WRS_ASM("      lwz   5,0(3)             ");

    _WRS_ASM("      lis   6,txBdmBgnd@h      ");
    _WRS_ASM("      ori   6,6,txBdmBgnd@l    ");
                     
    _WRS_ASM("      lis   3,0x5C5C5C5C@h     ");
    _WRS_ASM("      ori   3,3,0x5C5C5C5C@l   ");

    _WRS_ASM("      mfspr 7, 0x134           "); /* save DBCR */
    _WRS_ASM("      mfspr 8, 0x134           ");
    _WRS_ASM("      oris  8,8, 0x8100        "); /* set Ext Debug and Trap Exception bits */
    _WRS_ASM("      mtspr 0x134, 8           "); /* update DBCR	*/
    _WRS_ASM("	    isync		     "); /* ISYNC				*/
    _WRS_ASM("      .long 0x7FE00008         "); /* Force a trap exception to occur */    
    _WRS_ASM("      mtspr 0x134, 7           "); /* restore original DBCR */		
    _WRS_ASM("      xor   3,3,3              ");

    _WRS_ASM("endDescReady:                  ");
    }
#endif /* (CPU == PPC440) */

#if ((CPU == PPC603) || (CPU == PPC860))
/***************************************************************************
*
* tmdPPCSignalTxReady - inform emulator that the transmit descriptor is 
*                       ready
*
* This routine inform emulator that the transmit descriptor is ready.           
*
* RETURNS: N/A
*/                                                                              

void tmdPPCSignalTxReady
    (
    void
    )
    {
    _WRS_ASM("txBdmBgnd:                     ");
    _WRS_ASM("      .long 0x03FFFFF1         ");
    }
#endif /* ((CPU == PPC603) || (CPU == PPC860)) */

#if (CPU == PPC604)
/***************************************************************************
*
* tmdPPC604SignalTxReady - inform emulator that the transmit descriptor is 
*                          ready.
*
* This routine inform emulator that the transmit descriptor is ready.           
*
* RETURNS: N/A
*/                                                                              

void tmdPPC604SignalTxReady
    (
    void
    )
    {
    _WRS_ASM("      mfspr 4,1010             ");
    _WRS_ASM("      mfspr 3,1010             ");
    _WRS_ASM("      mfmsr 5                  ");
    _WRS_ASM("      andi. 5,5,0x0040         ");
    _WRS_ASM("      cmpwi 5,0                ");
    _WRS_ASM("      beq   setTxVectorLow     ");

    _WRS_ASM("      lis   3,0xFFF0           ");
    _WRS_ASM("      ori   3,3,0x0702         ");
    _WRS_ASM("      b     txNext             ");

    _WRS_ASM("setTxVectorLow:                ");
    _WRS_ASM("      lis   3,0x0              ");
    _WRS_ASM("      ori   3,3,0x0702         ");

    _WRS_ASM("txNext:                        ");
    _WRS_ASM("      mtspr 1010,3             ");

    _WRS_ASM("txBdmBgnd:                     ");
    _WRS_ASM("      .long 0x03FFFFF1         ");

    _WRS_ASM("      mtspr 1010,4             ");
    }
#endif /* (CPU == PPC604) */

#if (CPU == PPC405)
/***************************************************************************
*
* tmdPPC405SignalTxReady - inform emulator that the transmit descriptor is 
*                          ready
*
* This routine inform emulator that the transmit descriptor is ready.           
*
* RETURNS: N/A
*/                                                                              

void tmdPPC405SignalTxReady
    (
    void
    )
    {
    _WRS_ASM(" 	    mfspr 7, 0x3F2           "); /* save DBCR */
    _WRS_ASM(" 	    mfspr 8, 0x3F2           ");
    _WRS_ASM(" 	    oris  8,8, 0x8100        "); /* set Ext Debug and Trap Exception bits */
    _WRS_ASM(" 	    mtspr 0x3F2, 8           "); /* update DBCR */

    _WRS_ASM("txBdmBgnd:                     ");
    _WRS_ASM("      .long 0x7FE00008         ");
    _WRS_ASM("      mtspr 0x3F2, 7           "); /* restore original DBCR */
 
    _WRS_ASM("endTxReady:                    ");
    }
#endif /* (CPU == PPC405) */
#if (CPU == PPC440)
/***************************************************************************
*
* tmdPPC440SignalTxReady - inform emulator that the transmit descriptor is 
*                          ready
*
* This routine inform emulator that the transmit descriptor is ready.           
*
* RETURNS: N/A
*/                                                                              

void tmdPPC440SignalTxReady
    (
    void
    )
    {
    _WRS_ASM(" 	    mfspr 7, 0x134           "); /* save DBCR */
    _WRS_ASM(" 	    mfspr 8, 0x134           ");
    _WRS_ASM(" 	    oris  8,8, 0x8100        "); /* set Ext Debug and Trap Exception bits */
    _WRS_ASM(" 	    mtspr 0x134, 8           "); /* update DBCR */
    _WRS_ASM("	    isync		     "); /* ISYNC				*/

    _WRS_ASM("txBdmBgnd:                     ");
    _WRS_ASM("      .long 0x7FE00008         ");
    _WRS_ASM("      mtspr 0x134, 7           "); /* restore original DBCR */
 
    _WRS_ASM("endTxReady:                    ");
    }
#endif /* (CPU == PPC440) */

#if ((CPU == PPC603) || (CPU == PPC860))
/***************************************************************************
*
* tmdPPCSignalRxReady - inform emulator thay the receive descriptor is ready
*
* This routine inform emulator that the receive descriptor is ready.           
*
* RETURNS: N/A
*/                                                                              

void tmdPPCSignalRxReady
    (
    void
    )                                                
    {
    return;
    }
#endif /* ((CPU == PPC603) || (CPU == PPC860)) */

#if (CPU == PPC604)
/***************************************************************************
*
* tmdPPC604SignalRxReady - inform emulator thay the receive descriptor is 
*                          ready
*
* This routine inform emulator thay the receive descriptor is ready
*
* RETURNS: N/A
*/                                                                              

void tmdPPC604SignalRxReady
    (
    void
    )                                                
    {
    return;
    }
#endif /* (CPU == PPC604) */

#if (CPU == PPC405) 
/***************************************************************************
*
* tmdPPC405SignalRxReady - inform emulator thay the receive descriptor is 
*                          ready
*
* This routine inform emulator thay the receive descriptor is ready
*
* RETURNS: N/A
*/                                                                              

void tmdPPC405SignalRxReady
    (
    void
    )                                                
    {
    return;
    }
#endif /* (CPU == PPC405)  */
#if (CPU == PPC440)
/***************************************************************************
*
* tmdPPC440SignalRxReady - inform emulator thay the receive descriptor is 
*                          ready
*
* This routine inform emulator thay the receive descriptor is ready
*
* RETURNS: N/A
*/                                                                              

void tmdPPC440SignalRxReady
    (
    void
    )                                                
    {
    return;
    }
#endif /* (CPU == PPC440) */
