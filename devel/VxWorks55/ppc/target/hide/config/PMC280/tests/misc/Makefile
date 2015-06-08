#******************************************************************************
#              (c), Copyright, FORCE COMPUTERS INDIA Limited                  *
#                                                                             *
# THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF FORCE COMPUTERS, INC.        *
# NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT *
# OF FORCE OR ANY THIRD PARTY. FORCE RESERVES THE RIGHT AT ITS SOLE DISCRETION*
# TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO FORCE COMPUTERS.       *
# THIS CODE IS PROVIDED "AS IS". FORCE COMPUTERS MAKES NO WARRANTIES, EXPRESS,*
# IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.  *
#******************************************************************************
#                                                                             *
# Makefile                                                                    *
#                                                                             *
# DESCRIPTION:	                                                              *
#     This is the makefile contains rules for building pciperf tool           *
#                                                                             *
# DEPENDENCIES:	                                                              *
#     None                                                                    *
#                                                                             *
#*****************************************************************************/

CPU             = PPC604
TOOL            = gnu

TGT_DIR=$(WIND_BASE)/target

OBJECTS		= memmap.o frcPciCopyTest.o i2cTest.o intSwitchTest.o mem_perf.o vxDbTest.o vxDmaTest_dualCpu.o vxDmaTest_singleCpu.o\
				 watchDogTest.o flashtest.o
				

ADDED_CFLAGS= -I../ -I../../ -I../memtest -I../../coreDrv

include $(TGT_DIR)/h/make/defs.bsp
include $(TGT_DIR)/h/make/make.$(CPU)$(TOOL)
include $(TGT_DIR)/h/make/defs.$(WIND_HOST_TYPE)

#include $(WIND_BASE)/target/h/make/defs.bsp
#include $(WIND_BASE)/target/h/make/make.$(CPU)$(TOOL)
#include $(WIND_BASE)/target/h/make/defs.$(WIND_HOST_TYPE)


#CC_OPTIM_TARGET		+= -g

#AR	= dar
#AS	= das
#CC	= dcc
#LD	= dld

#DTARGET=PPC750

#CC_ARCH_SPEC = -XPPC750 -Xcomdat=0 -D__EABI__ -D__BIG_ENDIAN__ -Wa,-Xobject-format=elf
#CC_OPTIM_TARGET = -XO 
#CC_OPTIM_NORMAL = -O
#CC_OPTIM_DRIVER =
#CC_WARNINGS_ALL =

$(OBJECTS):	Makefile 

misc.o:   $(OBJECTS)
	    $(LD) -r $(OBJECTS) -o ./misc.o 


clean:
	rm -f *.o

exe: misc.o
