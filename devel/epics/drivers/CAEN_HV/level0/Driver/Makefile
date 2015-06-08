
# my: comment out +++++++++++ so far I do not undertand why it is needed for 
EPICS_BASE_HOST_BIN = $(EPICS_BASE)/bin/linux-x86/

TOP=../..
include $(TOP)/configure/CONFIG

# my: comment out +++++++++++ so far I do not undertand why it is needed for 
STATIC_BUILD=NO

LIB_SRCS += sy1527.c 
LIB_SRCS += sy1527epics1.c
PROD_SRC += sy1527_test.c

PROD_LIBS += sy1527epics1

PROD_LIBS += caenhvwrapper 
PROD_LIBS += hscaenet 
PROD_LIBS += pthread
hscaenet_DIR += $(CLAS)/clon/src/caen/CAENHVControlSoftware-1.1.2/lib
caenhvwrapper_DIR += $(CLAS)/clon/src/caen/CAENHVControlSoftware-1.1.2/lib
pthread_DIR += /usr/lib

LIBRARY += sy1527epics1
PROD += sy1527_test

INSTALL_LIB = $(EPICSB_DRIVERS)/lib/$(T_A)
INSTALL_LOCATION_LIB = $(EPICSB_DRIVERS)/lib

#my: changed INSTALL_PROD to INSTALL_BIN: threwise does not work ++++++++++++++++++
#INSTALL_PROD = $(EPICSB)/baseB/bin/$(T_A)
INSTALL_BIN = $(EPICSB_DRIVERS)/bin/$(T_A)

#===========================

PROD_LIBS += smi
PROD_LIBS += smiui
PROD_LIBS += dim

smi_DIR += $(SMIDIR)/$(ODIR)
smiui_DIR += $(SMIDIR)/$(ODIR)
dim_DIR += $(DIMDIR)/$(ODIR)

USR_INCLUDES += -I $(SMIDIR)/smixx
USR_INCLUDES += -I $(DIMDIR)/dim

#===========================
include $(TOP)/configure/RULES

