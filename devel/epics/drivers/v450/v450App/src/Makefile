TOP=../..

include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE

CROSS_COMPILER_TARGET_ARCHS=vxWorks-ppc604_long

#=============================
# Build the IOC support library

LIBRARY_IOC_vxWorks += v450Support

# xxxRecord.h will be created from xxxRecord.dbd
#DBDINC += xxxRecord
# Install devXxxSoft.dbd into <top>/dbd
DBD += devV450.dbd

# Compile and add the code to the support library
v450Support_SRCS += devAiV450.c
v450Support_SRCS += drvV450.c

# Link locally-provided code into the support library,
# rather than directly into the IOC application.

v450Support_LIBS += $(EPICS_BASE_IOC_LIBS)

#=============================
# Build the IOC application

PROD_IOC_vxWorks = v450
# v450.dbd will be created and installed
DBD += v450.dbd

# v450.dbd will be made up from these files:
v450_DBD += base.dbd
v450_DBD += devV450.dbd

# v450_registerRecordDeviceDriver.cpp derives from v450.dbd
v450_SRCS += v450_registerRecordDeviceDriver.cpp

# Build the main IOC entry point on workstation OSs.
v450_SRCS_DEFAULT += v450Main.cpp
v450_SRCS_vxWorks += -nil-

# Add support from base/src/vxWorks if needed
#v450_OBJS_vxWorks += $(EPICS_BASE_BIN)/vxComLibrary

# Link in the code from the support library
v450_LIBS += v450Support

# NOTE: To build SNL programs, SNCSEQ must be defined
# in the <top>/configure/RELEASE file



# Finally link to the EPICS Base libraries
v450_LIBS += $(EPICS_BASE_IOC_LIBS)

#=============================

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

