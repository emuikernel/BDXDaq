TOP=../..
include $(TOP)/configure/CONFIG

#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE
#=============================


# sergey: should be by default ???
USR_INCLUDES += -I$(EPICS_BASE)/include
USR_INCLUDES += -I$(EPICS_BASE)/include/os/Linux

# ???
USR_LDFLAGS_DEFAULT += -L$(EPICS_BASE)/lib/linux-x86
LIB_LIBS += $(EPICS_BASE)/lib/linux-x86/librecIoc.a


ca_DIR           = $(EPICS_BASE_LIB)  
Com_DIR          = $(EPICS_BASE_LIB)


#=============================


# sergey: should be by default ???
USR_DBDFLAGS += -I$(EPICS_BASE)/dbd


# <name>.dbd will be created from <name>Include.dbd
DBD += ioccaen.dbd

#=============================

STATIC_BUILD=NO

#PROD_IOC = ioccaen
PROD += ioccaen

# <name>_registerRecordDeviceDriver.cpp will be created from <name>.dbd
ioccaen_SRCS += ioccaen_registerRecordDeviceDriver.cpp
ioccaen_SRCS += bigsub.c
ioccaen_SRCS += sub.c
ioccaen_SRCS += devCAEN.c
ioccaen_SRCS_DEFAULT += ioccaenMain.cpp
ioccaen_SRCS_vxWorks += -nil-

# my: +++++++++++
ioccaen_LIBS += $(EPICS_BASE_IOC_LIBS)

# Use win32 object libs for registered support
ioccaen_LIBS_win32 += recIocObj
ioccaen_LIBS_win32 += softDevIocObj
ioccaen_LIBS_win32 += testDevIocObj
ioccaen_LIBS_DEFAULT += recIoc
ioccaen_LIBS_DEFAULT += softDevIoc
ioccaen_LIBS_DEFAULT += testDevIoc

#my: comments ++++++++++=
#ioccaen_LIBS += iocsh
#ioccaen_LIBS += miscIoc
#ioccaen_LIBS += rsrvIoc
#ioccaen_LIBS += dbtoolsIoc
#ioccaen_LIBS += asIoc
#ioccaen_LIBS += dbIoc
#ioccaen_LIBS += registryIoc
#ioccaen_LIBS += dbStaticIoc
#ioccaen_LIBS += ca
#ioccaen_LIBS += Com

####ioccaen_LIBS += bigsubRecord

ioccaen_LIBS += sy1527epics1
ioccaen_LIBS += caenhvwrapper 
ioccaen_LIBS += hscaenet 

sy1527epics1_DIR += $(EPICSB_DRIVERS)/lib/$(T_A)

caenhvwrapper_DIR += $(CLAS)/clon/src/caen/CAENHVControlSoftware-1.1.2/lib
hscaenet_DIR += $(CLAS)/clon/src/caen/CAENHVControlSoftware-1.1.2/lib

#===========================

ioccaen_LIBS += smi
ioccaen_LIBS += smiui
ioccaen_LIBS += dim

smi_DIR += $(SMIDIR)/$(ODIR)
smiui_DIR += $(SMIDIR)/$(ODIR)
dim_DIR += $(DIMDIR)/$(ODIR)

USR_INCLUDES += -I $(SMIDIR)/smixx
USR_INCLUDES += -I $(DIMDIR)/dim

#===========================
INSTALL_BIN = $(EPICSB_DRIVERS)/bin/$(T_A)
include $(TOP)/configure/RULES

#----------------------------------------
#  ADD RULES AFTER THIS LINE
USR_INCLUDES += -I../../Driver/
#USR_INCLUDES += -I $(SMIDIR)/smixx
