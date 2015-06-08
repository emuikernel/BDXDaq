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
#     This is the main makefile for building tests, this inturn invokes       *
#     the makefiles in the sub directories                                       *
#                                                                             *
# DEPENDENCIES:	                                                              *
#       Makefiles in the sub directories nbench,memtest,regparse and main.    *
#                                                                             *
#*****************************************************************************/
# 
#modification history
#--------------------
#41,25aug2004,btp,instead of doing archive , now we are  creating .o file
#		 inorder to statically link the testtool 
#41,25aug2004,btp,made the variable PMC280_DUAL_CPU as export for reflecting
#		 this value in Sub-Make files.
#
###############################################################################
#CPU             = PPC604
#TOOL            = gnu

#TGT_DIR=$(WIND_BASE)/target

#include $(TGT_DIR)/h/make/defs.bsp
#include $(TGT_DIR)/h/make/make.$(CPU)$(TOOL)
#include $(TGT_DIR)/h/make/defs.$(WIND_HOST_TYPE)

LD=ldppc

TEST_DIR= nbench memtest  lmbench main regparse misc
OBJS=testtool.o

## Only redefine make definitions below this point, or your definitions will
## be overwritten by the makefile stubs above.
$(OBJS): Makefile $(foreach dir,$(TEST_DIR),$(dir)/*.c) \
          $(foreach dir,$(TEST_DIR),$(dir)/*.h)$(foreach dir,$(TEST_DIR),$(dir)/Makefile)

exe: $(TEST_DIR)
	$(LD) -r -o $(OBJS) $(foreach dir,$(TEST_DIR), $(dir)/$(dir).o) 

$(TEST_DIR):: 
	 $(MAKE) -C $@  CPU=$(CPU) TOOL=$(TOOL)  OUT=$@.o "ADDED_CFLAGS=$(ADDED_CFLAGS)"

clean :
	$(RM) -f *.o
	$(RM) -f $(foreach dir,$(TEST_DIR),$(dir)/*.o)
