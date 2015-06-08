#
# to compile vxWorks version, create 'O.vxWorks-ppc604_long' directory
# and type:
#
# gmake -C O.vxWorks-ppc604_long -f ../Makefile TOP=../../../.. T_A=vxWorks-ppc604_long clean
# gmake -C O.vxWorks-ppc604_long -f ../Makefile TOP=../../../.. T_A=vxWorks-ppc604_long
#

#TOP=../../..

#EPICS_BASE_HOST_BIN = $(EPICS_BASE)/bin/$(T_A)

#include $(TOP)/configure/CONFIG

#STATIC_BUILD=NO

#LIB_SRCS += sy527.c 
#LIB_SRCS += sy527epics1.c
#LIB_SRCS += all_rocs.c
#LIB_SRCS += jvme.c

    #PROD_SRC += all_rocs.c
     #PROD_SRC += jvme.c
#PROD_SRC += v288.c

    #PROD_SRC += sy527_test.c
#PROD_LIBS += sy527epics1

#LIBRARY += sy527epics1
     #PROD += sy527_test
#PROD += v288

#INSTALL_LIB = $(EPICSB)/baseB/lib/$(T_A)
      #INSTALL_PROD = $(EPICSB)/baseB/bin/$(T_A)

#include $(TOP)/configure/RULES

#======================================================

#TOP=../../../../..
#TOP = ../../../../../base
#TOP = /usr/local/clas/clas12/R3.14.8.2/base
#TOP = $(EPICS_BASE)
# my: comment out +++++++++++
#TOP =  /usr/local/clas/clas12/R3.14.12.3/base-3.14.12.3

# my: comment out +++++++++++ so far I do not undertand why it is needed for 
EPICS_BASE_HOST_BIN = $(EPICS_BASE)/bin/linux-x86/

#my: comment out +++++++++++++
#include $(TOP)/configure/CONFIG

# my: +++++++++++++++++++++
TOP=../..
include $(TOP)/configure/CONFIG

# my: comment out +++++++++++ so far I do not undertand why it is needed for 
#STATIC_BUILD=NO

LIB_SRCS += sy1527.c 
LIB_SRCS += sy1527epics1.c
LIB_SRCS += v288.o
#LIB_SRCS += test1.c

#LIB_SRCS += all_rocs.c
#LIB_SRCS += jvme.c
#PROD_SRC += v288.c
PROD_SRC += sy527_test.c

PROD_LIBS += sy527epics1

#PROD_LIBS += caenhvwrapper 
#PROD_LIBS += hscaenet 
#PROD_LIBS += pthread
#hscaenet_DIR += $(EPICSB)/baseB/lib/$(T_A)
#caenhvwrapper_DIR += $(EPICSB)/baseB/lib/$(T_A)
#pthread_DIR += /usr/lib

LIBRARY += sy527epics1
#PROD += v288
PROD += sy527test

INSTALL_LIB = $(EPICSB_DRIVERS)/lib/$(T_A)
#INSTALL_LIBS = $(EPICSB)/baseB/lib/$(T_A)
#INSTALL_LOCATION_LIB = $(EPICSB)/baseB/lib/$(T_A)
INSTALL_LOCATION_LIB = $(EPICSB_DRIVERS)/lib
#my: changed INSTALL_PROD to INSTALL_BIN: threwise does not work ++++++++++++++++++
#INSTALL_PROD = $(EPICSB)/baseB/bin/$(T_A)
INSTALL_BIN = $(EPICSB_DRIVERS)/bin/$(T_A)

#===========================

PROD_LIBS += jvme
####PROD_OBJS += /usr/clas/clas12/coda/src/rol/Linux_i686/obj/all_rocs.o
PROD_OBJS += $(CLAS)/coda/src/rol/Linux_i686_vme/obj/all_rocs.o
###PROD_OBJS += $(CLAS)/coda/Linux_i686/obj/all_rocs.o

PROD_OBJS += sy527_test.o
#PROD_OBJS += v288.o

#PROD_LIBS += smi
#PROD_LIBS += smiui
#PROD_LIBS += dim

#rol_DIR += /usr/clas/clas12/coda/src/rol/Linux_i686/lib
####jvme_DIR += /usr/clas/clas12/coda/src/rol/jvme
jvme_DIR += $(CLAS)/coda/src/rol/jvme
#smi_DIR += $(SMIDIR)/$(ODIR)
#smiui_DIR += $(SMIDIR)/$(ODIR)
#dim_DIR += $(DIMDIR)/$(ODIR)

#USR_INCLUDES += /misc/clas/clas12/coda/src/rol/Linux_i686/obj/all_rocs.o
USR_CPPFLAGS += -DLinux_vme
USR_CXXFLAGS += -DLinux_vme
USR_CFLAGS += -DLinux_vme

#USR_INCLUDES += -I $(SMIDIR)/smixx
#USR_INCLUDES += -I $(DIMDIR)/dim

#===========================
include $(TOP)/configure/RULES


