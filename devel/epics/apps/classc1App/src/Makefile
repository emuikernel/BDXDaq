TOP=../..

include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE

#=============================
# Build the IOC support library

##LIBRARY_IOC += classc1Support
##LIBRARY_IOC_vxWorks += classc1Support

# xxxRecord.h will be created from xxxRecord.dbd
##DBDINC += xxxRecord
# Install devXxxSoft.dbd into <top>/dbd
##DBD += xxxSupport.dbd
##DBD += classc1Support.dbd
##classc1Support_DBD += harp_genericSupport.dbd
##classc1Support_DBD += scalerSupport.dbd
##classc1Support_DBD += scaler_dSupport.dbd
##classc1Support_DBD += scaler_eSupport.dbd
##classc1Support_DBD += asymSupport.dbd

# Compile and add the code to the support library
##classc1Support_SRCS += xxxRecord.c
##classc1Support_SRCS += devXxxSoft.c

# Link locally-provided code into the support library,
# rather than directly into the IOC application.
##classc1Support_SRCS += dbSubExample.c
##classc1Support_SRCS += classc1Hello.c
##classc1Support_SRCS += initTrace.c

##classc1Support_LIBS += $(EPICS_BASE_IOC_LIBS)

#=============================
# Build the IOC application

##PROD_IOC = classc1
PROD_IOC_vxWorks = classc1
# classc1.dbd will be created and installed
DBD += classc1.dbd

# classc1.dbd will be made up from these files:
##classc1_DBD += base.dbd
##classc1_DBD += xxxSupport.dbd
##classc1_DBD += dbSubExample.dbd
##classc1_DBD += classc1Hello.dbd
##classc1_DBD += initTrace.dbd
##classc1_DBD += classc1Support.dbd
classc1_DBD += harp_genericSupport.dbd
classc1_DBD += scalerSupport.dbd
classc1_DBD += scaler_dSupport.dbd
classc1_DBD += scaler_eSupport.dbd
classc1_DBD += asymSupport.dbd
classc1_DBD += devIocStats.dbd
# classc1_registerRecordDeviceDriver.cpp derives from classc1.dbd
##classc1_SRCS += classc1_registerRecordDeviceDriver.cpp

# Build the main IOC entry point on workstation OSs.
classc1_SRCS_DEFAULT += classc1Main.cpp
classc1_SRCS_vxWorks += -nil-
classc1_SRCS_vxWorks += classc1_registerRecordDeviceDriver.cpp

# Add support from base/src/vxWorks if needed
classc1_OBJS_vxWorks += $(EPICS_BASE_BIN)/vxComLibrary

# Link in the code from the support library
##classc1_LIBS += classc1Support

classc1_LIBS += mca
classc1_LIBS += calc
classc1_LIBS += busy
classc1_LIBS += sscan
classc1_LIBS += autosave
classc1_LIBS += stream streamSynApps
classc1_LIBS += Aerotech
classc1_LIBS += Attocube
classc1_LIBS += oms
classc1_LIBS += Mclennan
classc1_LIBS += Micos
classc1_LIBS += MicroMo
classc1_LIBS += PI
classc1_LIBS += Acs
classc1_LIBS += Ims
classc1_LIBS += Newport
classc1_LIBS += softMotor
classc1_LIBS += motorSimSupport
classc1_LIBS += SmartMotor
classc1_LIBS += KohzuMotor
classc1_LIBS += motor
classc1_LIBS += vme
classc1_LIBS += mcaSIS
classc1_LIBS += mcaCanberra
classc1_LIBS += std
classc1_LIBS += asyn
classc1_LIBS += devIocStats

classc1_LIBS += seqDev seq pv 

##classc1_LIBS += classc1AllSupport
classc1_LIBS += harp_genericSupport
classc1_LIBS += scalerSupport
classc1_LIBS += scaler_dSupport
classc1_LIBS += scaler_eSupport
classc1_LIBS += asymSupport
##classc1_LIBS += devIocStats

# NOTE: To build SNL programs, SNCSEQ must be defined
# in the <top>/configure/RELEASE file

ifneq ($(SNCSEQ),)
    # Build sncExample into classc1AllSupport
##    sncExample_SNCFLAGS += +r
##    classc1_DBD += sncExample.dbd
##    classc1Support_SRCS += sncExample.stt
##    classc1Support_LIBS += seq pv
##    classc1_LIBS += seq pv
##    classc1_SRCS += harp_scan_generic.st
##    classc1_SRCS += reset_motor.st

##    classc1_SRCS += scaler_restart.st
##    classc1_SRCS += scaler_warning.st

##    classc1_SRCS += scaler_d_restart.st

##    classc1_SRCS += scaler_e_restart.st

##    classc1_SRCS += asym.st

    classc1Support_LIBS += seq pv

    # Build sncProgram as a standalone program
##    PROD_HOST += sncProgram
##    sncProgram_SNCFLAGS += +m
##    sncProgram_SRCS += sncProgram.st
##    sncProgram_LIBS += seq pv
##    sncProgram_LIBS += $(EPICS_BASE_HOST_LIBS)
endif

# Finally link to the EPICS Base libraries
classc1_LIBS += $(EPICS_BASE_IOC_LIBS)

#=============================

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

