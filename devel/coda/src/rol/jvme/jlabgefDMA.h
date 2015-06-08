/*----------------------------------------------------------------------------*
 *  Copyright (c) 2009        Southeastern Universities Research Association, *
 *                            Thomas Jefferson National Accelerator Facility  *
 *                                                                            *
 *    This software was developed under a United States Government license    *
 *    described in the NOTICE file included as part of this distribution.     *
 *                                                                            *
 *    Author:  Bryan Moffit                                                   *
 *             moffit@jlab.org                   Jefferson Lab, MS-12B3       *
 *             Phone: (757) 269-5660             12000 Jefferson Ave.         *
 *             Fax:   (757) 269-5800             Newport News, VA 23606       *
 *                                                                            *
 *----------------------------------------------------------------------------*
 *
 * Description:
 *     Header for JLab DMA routines to compliment the GEFanuc API
 *
 *----------------------------------------------------------------------------*/

#ifndef __JLABGEFDMA__
#define __JLABGEFDMA__

int  jlabgefDmaConfig(unsigned int addrType, unsigned int dataType, unsigned int sstMode);
int  jlabgefDmaSend(unsigned int locAdrs, unsigned int vmeAdrs, int size);
int  jlabgefDmaDone();
int  jlabgefDmaAllocLLBuffer();
int  jlabgefDmaFreeLLBuffer();
void jlabgefDmaSetupLL(unsigned int locAddrBase,unsigned int *vmeAddr,
		       unsigned int *dmaSize,unsigned int numt);
void jlabgefDmaSendLL();
void jlabgefReadDMARegs();

#endif /* __JLABGEFDMA__ */
