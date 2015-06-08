TOP=../..

include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE

#=============================
# Build the IOC support library

##LIBRARY_IOC += classc4Support

# xxxRecord.h will be created from xxxRecord.dbd
##DBDINC += xxxRecord
# Install devXxxSoft.dbd into <top>/dbd
##DBD += xxxSupport.dbd

# Compile and add the code to the support library
##classc4Support_SRCS += xxxRecord.c
##classc4Support_SRCS += devXxxSoft.c

# Link locally-provided code into the support library,
# rather than directly into the IOC application.
##classc4Support_SRCS += dbSubExample.c
##classc4Support_SRCS += classc4Hello.c
##classc4Support_SRCS += initTrace.c

##classc4Support_LIBS += $(EPICS_BASE_IOC_LIBS)

#=============================
# Build the IOC application

##PROD_IOC = classc4
PROD_IOC_vxWorks = classc4
# classc4.dbd will be created and installed
DBD += classc4.dbd

# classc4.dbd will be made up from these files:
##classc4_DBD += base.dbd
##classc4_DBD += xxxSupport.dbd
##classc4_DBD += dbSubExample.dbd
##classc4_DBD += classc4Hello.dbd
##classc4_DBD += initTrace.dbd
classc4_DBD += sixty_hzSupport.dbd
classc4_DBD += scaler_cSupport.dbd
classc4_DBD += frwd_scalerSupport.dbd
classc4_DBD += devIocStats.dbd
# classc4_registerRecordDeviceDriver.cpp derives from classc4.dbd
##classc4_SRCS += classc4_registerRecordDeviceDriver.cpp

# Build the main IOC entry point on workstation OSs.
classc4_SRCS_DEFAULT += classc4Main.cpp
##classc4_SRCS_vxWorks += -nil-
classc4_SRCS_vxWorks += classc4_registerRecordDeviceDriver.cpp

# Add support from base/src/vxWorks if needed
classc4_OBJS_vxWorks += $(EPICS_BASE_BIN)/vxComLibrary

# Link in the code from the support library
##classc4_LIBS += classc4Support

classc4_LIBS += mca
classc4_LIBS += calc
classc4_LIBS += busy
classc4_LIBS += sscan
classc4_LIBS += autosave
classc4_LIBS += std
classc4_LIBS += stream streamSynApps
classc4_LIBS += asyn
classc4_LIBS += mcaSIS
classc4_LIBS += mcaCanberra
classc4_LIBS += vme
classc4_LIBS += devIocStats

classc4_LIBS += seqDev seq pv 

classc4_LIBS += sixty_hzSupport
classc4_LIBS += scaler_cSupport
classc4_LIBS += frwd_scalerSupport

# NOTE: To build SNL programs, SNCSEQ must be defined
# in the <top>/configure/RELEASE file

ifneq ($(SNCSEQ),)
    # Build sncExample into classc4Support
##    sncExample_SNCFLAGS += +r
##    classc4_DBD += sncExample.dbd
##    classc4Support_SRCS += sncExample.stt
    classc4Support_LIBS += seq pv
##    classc4_LIBS += seq pv

    # Build sncProgram as a standalone program
##    PROD_HOST += sncProgram
##    sncProgram_SNCFLAGS += +m
##    sncProgram_SRCS += sncProgram.st
##    sncProgram_LIBS += seq pv
##    sncProgram_LIBS += $(EPICS_BASE_HOST_LIBS)
endif

# Finally link to the EPICS Base libraries
classc4_LIBS += $(EPICS_BASE_IOC_LIBS)

#=============================

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

