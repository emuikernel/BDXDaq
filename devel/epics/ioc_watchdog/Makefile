#***************************************************************************
#
# Purpose: makefile for the hv_display utility
#
#***************************************************************************

## name of the operating system
OS := $(shell uname)

## Directory structure
#HDIR = /home/fast/fast_daq/my_bsd_clientserver_closesocket
#HDIR = /afs/cern.ch/user/v/vsytnik/delete_processes_by_name
#/home/fast/fast_daq/delete_processes_by_name

#BDIR = $(HOME)/bin
BDIR = .
SDIR = ./src
ODIR = ./obj
IDIR = ./include
LDIR = ./lib

#***************************************************************************
## names for commands
CC         = g++
F77        = g77

#***************************************************************************
## options for compiling

# Uncomment the next line when debugging
DEBUG      = -Wall -g

# Uncomment the next line when NOT debugging
#DEBUG      = -Wall -O

CCOPT      = $(DEBUG) -Wall -Df2cFortran
F77OPT     = $(DEBUG) -Wall -ff90 -fno-second-underscore

#***************************************************************************
## includes and linking X libraries and forms libs

#CXXFLAGS      = -O -Wall -fno-rtti -fno-exceptions -fPIC -I$(ROOTSYS)/include


#***************************************************************************
## other includes and libs

#ifeq ($(OS),Linux)
             # True for CERN version of RH 5.1 and RH 5.2
#  LINUX_VERSION := $(shell uname -r)
#  ifeq ($(LINUX_VERSION),2.0.36)
   # LIB    = /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.90.29/libf2c.a -lm -lfl
#  else
  #  LIB    = -lg2c -lnsl -lm -lfl
#  endif
#endif

#***************************************************************************
## get object file names from source file names

#***************************************************************************

all : $(BDIR)/ioc_cron_exec $(BDIR)/ioc_cron_sem_kill
#$(BDIR)/emudcs_watch_dog $(BDIR)/dcscontrol_watch_dog

$(BDIR)/ioc_cron_exec: $(SDIR)/ioc_cron_exec.cxx  Makefile 
	g++ $(CCOPT) $(CXXFLAGS) -o $(BDIR)/ioc_cron_exec\
        $(SDIR)/ioc_cron_exec.cxx -lpthread; \

$(BDIR)/ioc_cron_sem_kill: $(SDIR)/ioc_cron_sem_kill.cxx  Makefile 
	g++ $(CCOPT) $(CXXFLAGS) -o $(BDIR)/ioc_cron_sem_kill\
        $(SDIR)/ioc_cron_sem_kill.cxx -lpthread; \

# cp emudcs_watchdog_crontab_wrap $(HOME)/bin/.; \
#        cp delete_crontab $(HOME)/bin/.
	
#$(BDIR)/emudcs_watch_dog: $(SDIR)/emudcs_watch_dog.cxx  Makefile 
#	g++ $(CCOPT) $(CXXFLAGS) -o $(BDIR)/emudcs_watch_dog \
#        $(SDIR)/emudcs_watch_dog.cxx; chmod -s $(BUILD_HOME)/emu/emuDCS/e2p/test/linux/x86_slc4/simple_config_plus_dcs_new.exe; chmod -s $(HOME)/data_liu_OSU_driver/read_single; chmod -s $(BDIR)/emudcs_watch_dog; #cp emudcs_exe_wrap $(HOME)/bin/.;cp emudcs_exe_wrap_minus $(HOME)/bin/.


#$(BDIR)/dcscontrol_watch_dog: $(SDIR)/dcscontrol_watch_dog.cxx  Makefile 
#	g++ $(CCOPT) $(CXXFLAGS) -o $(BDIR)/dcscontrol_watch_dog \
#        $(SDIR)/dcscontrol_watch_dog.cxx;# chmod -s $(XDAQ_ROOT)/daq/xdaq/bin/linux/x86/xdaq.sh; chmod -s $(BDIR)/dcscontrol_watch_dog
clean :
	rm -f $(ODIR)//*;rm -f $(BDIR)/ioc_cron_exec; rm *~; rm */*~
        
install:
	cp $(BDIR)/ioc_cron_exec $(CLAS)/epics/bin/$(EPICS_HOST_ARCH)/ 
	cp $(BDIR)/*_cron $(CLAS)/epics/bin/$(EPICS_HOST_ARCH)/
