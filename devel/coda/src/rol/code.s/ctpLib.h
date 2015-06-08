/*----------------------------------------------------------------------------*
 *  Copyright (c) 2010        Southeastern Universities Research Association, *
 *                            Thomas Jefferson National Accelerator Facility  *
 *                                                                            *
 *    This software was developed under a United States Government license    *
 *    described in the NOTICE file included as part of this distribution.     *
 *                                                                            *
 *    Authors: Bryan Moffit                                                   *
 *             moffit@jlab.org                   Jefferson Lab, MS-12B3       *
 *             Phone: (757) 269-5660             12000 Jefferson Ave.         *
 *             Fax:   (757) 269-5800             Newport News, VA 23606       *
 *                                                                            *
 *----------------------------------------------------------------------------*
 *
 * Description:
 *     Status and Control library for the JLAB Crate Trigger Processor
 *     (CTP) module using an i2c interface from the JLAB Trigger
 *     Interface/Distribution (TID) module.
 *
 * SVN: $Rev: 445 $
 *
 *----------------------------------------------------------------------------*/
#ifndef CTPLIB_H
#define CTPLIB_H

/* Structure to handle the similarities between the FPGA registers 
   The VLX110 has extra registers and will be handled separately */
struct CTP_FPGAStruct
{
  /* 0xn00 */ volatile unsigned int status0;   /* 0 */
  /* 0xn04 */ volatile unsigned int status1;   /* 1 */
  /* 0xn08 */ volatile unsigned int config0;   /* 2 */
  /* 0xn0C */ volatile unsigned int config1;   /* 3 */
  /* 0xn10 */ volatile unsigned int temp;      /* 4 */
  /* 0xn14 */ volatile unsigned int vint;      /* 5 */
};

struct TID_CTPStruct
{
  /* 0x0000 */          unsigned int blankCTP0[(0x3C00-0x0000)/4];
  /* 0x3C00 */ struct   CTP_FPGAStruct fpga1;
  /* 0x3C18 */          unsigned int blankCTP1[(0x5C00-0x3C18)/4];
  /* 0x5C00 */ struct   CTP_FPGAStruct fpga2;
  /* 0x5C18 */          unsigned int blankCTP2[(0x7C00-0x5C18)/4];
  /* 0x7C00 */ struct   CTP_FPGAStruct fpga3;
  /* 0x7C18 */ volatile unsigned int sum_threshold_lsb;  /* 6 */
  /* 0x7C1C */ volatile unsigned int sum_threshold_msb;  /* 7 */
  /* 0x7C20 */ volatile unsigned int history_buffer_lsb; /* 8 */
  /* 0x7C24 */ volatile unsigned int history_buffer_msb; /* 9 */
  /* 0x7C28 */          unsigned int blankCTP3[(0x10000-0x7C28)/4];
};

/* CTP Register bits and masks */
/* Lane_up mask shifts by 2 bits for each channel (two lanes/channel) */
#define CTP_FPGA_STATUS0_LANE_UP_MASK            0x3

#define CTP_FPGA_STATUS1_FIRMWARE_VERSION_MASK   0xFF00
/* History buffer only for VLX110 */
#define CTP_FPGA_STATUS1_HISTORY_BUFFER_READY    (1<<2)

int  ctpInit();
int  ctpStatus();
int  ctpSetFinalSumThreshold(unsigned int threshold, int arm);
int  ctpGetFinalSumThreshold();
int  ctpSetPayloadEnableMask(int enableMask);
int  ctpSetVmeSlotEnableMask(unsigned int vmemask);
int  ctpEnableSlotMask(unsigned int inMask);
int  ctpResetGTP();
int  ctpGetAllChanUp();

#endif /* CTPLIB_H */
