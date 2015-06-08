##TOP=../../../../..
##TOP = $(EPICS_BASE)
#---TOP =  /usr/local/clas/clas12/R3.14.12.3/base-3.14.12.3
##TOP = ../../../../../base
#---EPICS_BASE_HOST_BIN = $(EPICS_BASE)/bin/linux-x86/

#---include $(TOP)/configure/CONFIG

# my: +++++++++++++
TOP=../..
include $(TOP)/configure/CONFIG


#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE
#=============================

#USR_CFLAGS += 

ca_DIR           = $(EPICS_BASE_LIB)  
Com_DIR          = $(EPICS_BASE_LIB)


#=============================

# <name>.dbd will be created from <name>Include.dbd
DBD += ioccaenold.dbd

#=============================

STATIC_BUILD=NO

#PROD_IOC = ioccaen
PROD += ioccaenold

# <name>_registerRecordDeviceDriver.cpp will be created from <name>.dbd
ioccaenold_SRCS += ioccaenold_registerRecordDeviceDriver.cpp
ioccaenold_SRCS += bigsub.c
ioccaenold_SRCS += sub.c
ioccaenold_SRCS += devCAEN.c
ioccaenold_SRCS_DEFAULT += ioccaenMain.cpp
ioccaenold_SRCS_vxWorks += -nil-

# my: +++++++++++
ioccaenold_LIBS += $(EPICS_BASE_IOC_LIBS)

# Use win32 object libs for registered support
ioccaenold_LIBS_win32 += recIocObj
ioccaenold_LIBS_win32 += softDevIocObj
ioccaenold_LIBS_win32 += testDevIocObj
ioccaenold_LIBS_DEFAULT += recIoc
ioccaenold_LIBS_DEFAULT += softDevIoc
ioccaenold_LIBS_DEFAULT += testDevIoc

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

####ioccaenold_LIBS += bigsubRecord

ioccaenold_LIBS += sy527epics1
#ioccaenold_LIBS += caenhvwrapper 
#ioccaenold_LIBS += hscaenet 

#sy527epics1_DIR += $(EPICSB)/baseB/lib/$(T_A)
sy527epics1_DIR += $(EPICSB_DRIVERS)/lib/$(T_A)

#caenhvwrapper_DIR += $(EPICSB)/baseB/lib/$(T_A)
#hscaenet_DIR += $(EPICSB)/baseB/lib/$(T_A) 
####bigsubRecord_DIR += $(EPICSB)/baseB/lib/$(T_A) 

#===========================

####ioccaenold_OBJS += /usr/clas/clas12/coda/src/rol/Linux_i686/obj/all_rocs.o
ioccaenold_OBJS += $(CLAS)/coda/src/rol/Linux_i686_vme/obj/all_rocs.o
ioccaenold_LIBS += jvme
####jvme_DIR += /usr/clas/clas12/coda/src/rol/jvme 
jvme_DIR += $(CLAS)/coda/src/rol/jvme
#===========================

#ioccaenold_LIBS += smi
#ioccaenold_LIBS += smiui
#ioccaenold_LIBS += dim

#smi_DIR += $(SMIDIR)/$(ODIR)
#smiui_DIR += $(SMIDIR)/$(ODIR)
#dim_DIR += $(DIMDIR)/$(ODIR)

#USR_INCLUDES += -I $(SMIDIR)/smixx
#USR_INCLUDES += -I $(DIMDIR)/dim

#===========================
INSTALL_BIN = $(EPICSB_DRIVERS)/bin/$(T_A)
include $(TOP)/configure/RULES

#----------------------------------------
#  ADD RULES AFTER THIS LINE
USR_INCLUDES += -I../../driverOld
#USR_INCLUDES += -I $(SMIDIR)/smixx
