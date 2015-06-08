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


# Makefile for PMC280
#
# DESCRIPTION:
#       This file contains rules for building VxWorks on the specified board
#       using the Tornado 2.2 environment. (refer to BSP manual.)
#

CPU             = PPC604
TOOL            = gnu

TGT_DIR = $(WIND_BASE)/target
include $(TGT_DIR)/h/make/defs.bsp

## Only redefine make definitions below this point, or your definitions will
## be overwritten by the makefile stubs above.

TARGET_DIR = PMC280
VENDOR     = Force-Computers
BOARD      = PMC280

BOOTCONFIG = bootConfig.c

BOOTINIT   = bootInit.c

USRCONFIG  = usrConfig.c

CORE_DRV_DIR    = $(TGT_DIR)/config/$(TARGET_DIR)/coreDrv

COMM_DRV_DIR    = $(TGT_DIR)/config/$(TARGET_DIR)/commDrv

TESTS_DRV_DIR = $(TGT_DIR)/config/$(TARGET_DIR)/tests

SERIAL_DRV    = brg.o cunit.o vxCunitIntCtrl.o mpsc.o vxMpscIntCtrl.o \
		sdma.o vxSdmaIntCtrl.o serialChan.o vxMpscUart.o

#
# The constants ROM_TEXT_ADRS, ROM_SIZE, RAM_LOW_ADRS and
# RAM_HIGH_ADRS are defined in config.h and MakeSkel.
# All definitions for these constants must be identical.
#

##ROM_TEXT_ADRS = fff00100 # ROM cold entry address
##ROM_WARM_ADRS = fff00108 # ROM warm entry address
ROM_TEXT_ADRS = a0000100 # ROM cold entry address
ROM_WARM_ADRS = a0000108 # ROM warm entry address

ROM_SIZE      = 00100000 # number of bytes of ROM space
RAM_LOW_ADRS  = 00010000 # RAM text/data address
RAM_HIGH_ADRS = 00400000 # RAM text/data address for bootrom 

#
# User flags to be added when compiling/linking.
#

# Used when generating bootrom.hex
#HEX_FLAGS       = -a 0
USR_ENTRY       = usrInit 

LDFLAGS = -EB -X -N -Map $@.map

# 
# *******************************************************
# ALL DEFINES COMMON TO SINGLE AS WELL AS DUAL CPU BUILDS
# *******************************************************
#
# ******************
# MANDATORY DEFINES
# ******************
#
#
# ***********************
# OPTIONAL/DEBUG DEFINES
# ***********************
#
# PMC280_DEBUG_UART 
#    This will enable initialisation of MPSC[0] for debug prints
#    as well as print debug info (only bootrom prints). Use this to
#    find out all modules being initialised when debugging. These are
#    debug prints from romInit.s and bootInit.c
#
# PMC280_DEBUG_UART_VX 
#    This will print messages during the initialisation in VxWorks.
#    These are debug prints from sysALib.s, sysLib.c, usrConfig.c, etc. 
#    (Note that these are prints from VxWorks image and *not* boot ROM)
#
# PMC280_PCI_BOOT
#    This is necessary if the vxWorks image built has to boot-up across
#    the PCI. Note that when PMC280_PCI_BOOT is enabled
#
# **************************************
# ALL DEFINES SPECIFIC TO DUAL CPU BUILD
# **************************************
#
# ******************
# MANDATORY DEFINES
# ******************
#
# PMC280_DUAL_CPU
#    Enable support for dual CPUs (MPC7447) in this BSP. Do *not* turn this
#    off unless you know what you are doing.
#
# PMC280_DUAL_CPU_MMU_SETUP
#    Compiles in the support for MMU outside of the VxWorks library. Used
#    in usrConfig.c as well as src/arch/ppc/mmuPpcLib.c. Do *not* turn this
#    off unless you know what you are doing.
# 
# PMC280_DUAL_CPU_PTE_SETUP
#   Compile in support for Page Table entries into Dual CPU BSP. Do *not*
#   turn this off unless you know what you are doing. Also this has to be
#   used only when PMC280_DUAL_CPU is enabled.
#
# ***********************
# OPTIONAL/DEBUG DEFINES
# ***********************

# Used when compiling .c or .s files.
# Sergey: change 3 _DUAL_ flags to switch between single and dual versions

ADDED_CFLAGS    = $(NIC_CFLAGS) $(GALNET_CFLAGS) \
                  -DDPMC280_DEBUG_UART \
                  -DDPMC280_DEBUG_UART_VX \
		  -DDPMC280_BOOTROM_BOOT \
		  -DDPMC280_PCI_BOOT \
		  -DPMC280_USERFLASH_BOOT \
                  -DDPMC280_DUAL_CPU \
                  -DDPMC280_DUAL_CPU_MMU_SETUP \
                  -DDPMC280_DUAL_CPU_PTE_SETUP

ifneq (,$(findstring -DPMC280_DUAL_CPU, $(ADDED_CFLAGS)))
 
nonfrcLibs = -larch -los

LD_LINK_PATH_EXTRA = -L$(TGT_DIR)/config/$(TARGET_DIR)/force/lib/$(VX_CPU_FAMILY)/$(CPU)/$(TOOL_COMMON_DIR)  
LD_LINK_LIBS = $(filter-out $(nonfrcLibs) , $(subst .a,, $(subst lib, -l, \
                                 $(notdir $(foreach dir, \
                                            $(subst -L,, $(LD_LINK_PATH)), \
                                            $(wildcard $(dir)/lib*.a))))))
 
VX_OS_LIBS = $(LD_LINK_LIBS) $(TGT_DIR)/config/$(TARGET_DIR)/force/lib/lib$(CPU)$(TOOL)frc.a
 
endif

#Sergey
##FEI_FILES = sys557Lib.o fei82557End.obj
##GEI_FILES = sysGei82543End.o gei82543End.obj
FEI_FILES = 
GEI_FILES = 


BSP_MODS        = pciConfigLib.o pciDevConfig.o pciScan.o gtInitLib.o \
		  gtIntControl.o sysIntCtrl.o userRsrvMem.o $(FEI_FILES) $(GEI_FILES) \

ETHERNET_DRV    = ethernet.o vxEthernetIntCtrl.o mgiEnd.obj

CORE_DRV        = gtCore.o gtPci.o gtMemory.o gtDma.o gtSram.o \
		  vxGppIntCtrl.o gtSmp.o vxDbIntCtrl.o \
		  gtCntmr.o vxCntmrIntCtrl.o vxDmaIntCtrl.o vxWatchDog.o 
		  
COMM_DRV        = $(ETHERNET_DRV) $(SERIAL_DRV)

TEST_PATH	= tests/
TESTS_DRV       = testtool.o

MOD_PMC280      = dbgprints.o  sysRamDrv.o bFlashDrv.o flashDrv.o \
		  i2cDrv.o rtc.o frcBibShow.o frcBibDrv.o bibSupport.o \
		  pciBoot.o regFileBurn.o vpd.o \

# These are needed before the compressed image is uncompressed and run.
BOOT_EXTRA      = itab.o 

# When using NIC Over Ethernet or UART Over MPSC add $(COMM_DRV)
MACH_EXTRA      = $(BOOT_EXTRA) $(BSP_MODS) $(CORE_DRV) $(COMM_DRV) \
		  $(MOD_PMC280) ${TEST_PATH}$(TESTS_DRV)

# Sergey: my extra modules
MACH_EXTRA += usrDma.o sysAUtil.o

# for Tornado 2.1 project facility 
EXTRA_MODULES   = $(CORE_DRV) $(COMM_DRV) $(TESTS_DRV)

EXTRA_INCLUDE   =  -I$(CORE_DRV_DIR) -I$(COMM_DRV_DIR) -I$(TESTS_DRV_DIR)

VPATH           = $(CORE_DRV_DIR) $(COMM_DRV_DIR) $(TESTS_DRV_DIR)

# Cache library target 
mkPpcCacheLibs:
		- @ $(RM) cacheArchLib.obj
		- @ $(RM) cacheALib.obj
		- @ $(RM) sysL2BackCache.obj
		- @ $(RM) sysL3Cache.obj
		$(CC) -c $(CFLAGS) -o cacheArchLib.obj cacheArchLib.c
		$(CC) -c $(CFLAGS_AS) -o cacheALib.obj cacheALib.s
		$(CC) -c $(CFLAGS_AS) -o sysL2BackCache.obj sysL2BackCache.s
		$(CC) -c $(CFLAGS_AS) -o sysL3Cache.obj sysL3Cache.s


## Only redefine make definitions above this point, or the expansion of 
## makefile target dependencies may be incorrect.

include $(TGT_DIR)/h/make/rules.bsp

cleanall:	clean cleantest

cleantest:
	$(MAKE) -C tests clean
	$(RM) testtool.o


tests/testtool.o:
	$(MAKE) -C tests exe CPU=$(CPU) TOOL=$(TOOL) "ADDED_CFLAGS=-I$(TGT_DIR)/config/$(TARGET_DIR) -I$(TGT_DIR)/config/$(TARGET_DIR)/tests -I$(TGT_DIR)/config/$(TARGET_DIR)/tests/main -I$(TGT_DIR)/config/$(TARGET_DIR)/tests/memtest -I$(COMM_DRV_DIR) -I$(CORE_DRV_DIR) -I./ ${ADDED_CFLAGS} "
#tests/testtool.o:
#	$(MAKE) -C tests "ADDED_CFLAGS= -I./tests/main -I../../ -I../../coreDrv -I../../commDrv -I../ ${ADDED_CFLAGS} "

bootrom.bin:	bootrom
	elfToBin < bootrom > bootrom.bin

vxWorks.st_rom.bin:	vxWorks.st_rom
	elfToBin < vxWorks.st_rom > vxWorks.st_rom.bin

###
### user specific !!!
###
install_single:
	cp vxWorks.st $(CODA)/VXWORKS_ppc/bin/vx280
# sudo cp bootrom.bin /tftpboot/boot_ppmc280.bin

install_dual:
	cp vxWorks.st $(CODA)/VXWORKS_ppc/bin/vx280dual
# sudo cp bootrom.bin /tftpboot/boot_ppmc280dual.bin


