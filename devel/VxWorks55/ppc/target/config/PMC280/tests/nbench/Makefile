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
#      This file contains rules for building the nbench Benchmark Suite for   *
#      the desired architecture                                               * 
#                                                                             *
# DEPENDENCIES:	                                                              *
#     None                                                                    *
#                                                                             *
#*****************************************************************************/
CPU             = PPC604
TOOL            = gnu

TGT_DIR=$(WIND_BASE)/target
include $(TGT_DIR)/h/make/defs.bsp
include $(TGT_DIR)/h/make/make.$(CPU)$(TOOL)
include $(TGT_DIR)/h/make/defs.$(WIND_HOST_TYPE)

OBJECTS		= emfloat.o pointer.o misc.o nbench0.o nbench1.o new.o hello.o sysspec.o
ADDED_CFLAGS= -I../main

$(OBJECTS):	Makefile nmglobal.h emfloat.h misc.h nbench0.h nbench1.h pointer.h sysspec.h

nbench.o:   $(OBJECTS)
	    $(LD) -r $(OBJECTS) -o ./nbench.o 

clean:
	rm -f *.o

exe: nbench.o
