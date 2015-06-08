###
# as you comple modules:
#   executable that test and do basic support should have compile
#   link sequence should be listed on the support line so that
#   they will be compiled in one step for all support prog should
#   be listed in the update part if theya re for general use  
#
# -V  is verbose
#
#
# ipcbank2dd: ipcbank2dd.o
#	rtlink -cxx $(DEBUG) -o $@ $@.o  $(CLON_WSOURCE)/run_log/s/dd_fpack.o
#	-L$(CLON_LIB) -lipc -lutil -L$(CMLOG)/lib/solaris -lcmlog 
#	-L$(CODA)/SunOS/lib -ldd -ltcl -lmysql4.1.20  -lposix4
#
# update will copy the necsary files to the general executable area
#   of clasrun because I will be doing both TOF and EC I will include
#   speciffically each module that I copy and need to rename so that
#   there is no conflict eg:
#      lpoweroff  becomes EC_laser_off
#                  ... or TOF_laser_off
#
#
# Makefile for /~clasrun/jmu/c_cntl
#  
# Kevin Giovanetti [giovankl@jmu.edu]
#
# I should improve to use many of the default make rules
#
# All usable routines will be kept in the library area and put into liblaser.a
#  each module will need to be entered in to the ROUTINES list and a seperate
#  target created for use see parse.  All include files will be kept in the
#  include area just add new header files to the INCLUDE list (no commas).
#  
#  use  gmake
#        gmake -v     = verbose
#
# CCRT in this compiler is for use with the smart sockets stuff seems
#  to be necessary for DP_cmd calls

CCRT =  rtlink -g

# command to build a library
AR = ar -r 

# compiler used to build the routines
CC =  cc 

# define the local areas for libraries and include files.
LINC  = ./include/
LLIB  = ./lib/
LLIB1 = ./lib

# Create pointers to libraries and include files for the compile
# and link steps, I had trouble with the libraries and had to reorder
# them to link. If there are unresolved references consider appending
# a library at the end. You can repeat libraries.
# I changes ss_send, unix_master, ss_recieive  other have sames
# libs and INCS but old order.
# Note -L  indicates the directory followed by -lxxx where libxxx.a
# is the library and the dir is the preceding -L

INC_ECLASER  =	-I./include
LINK_ECLASER =	-L$(LLIB1) -llaser

INC_CLAS     =	-I$(CLON_INC)
LINK_CLAS    =	-L$(CLON_LIB) -lipc -lutil
#LINK_CLAS    =	-L$(CLON_LIB) -lipc -lutil -L$(CMLOG)/lib/solaris -lcmlog \
#		-L$(CODA)/SunOS/lib  -ltcl -lmysql4.1.20  -lposix4

INC_CODA     =	-I$(CODA)/common/include
LINK_CODA    =	-L$(CODA)/SunOS/lib -lmysql4.1.20  -ldpS -L/usr/local/coda/2.0b1/SunOS/lib -lca

#lists
#  add name of all local header files  and put includes as a dependency
#  where needed, add the names of all routine to the list and build a
#  target with command list for construction include the library list
#  to those routines that need it but do nat add any addtional libraries
#  it is used as a target.

INCLUDES      =	$(LINC)laser.h $(LINC)bit_defs.h  $(LINC)parse.h  $(LINC)ssinclude.h

ROUTINES      =	$(LLIB)parse.o     $(LLIB)motor_idle.o $(LLIB)bit_TST.o \
		$(LLIB)bit_SET.o   $(LLIB)limit.o      $(LLIB)limit_TST.o \
		$(LLIB)bit_split.o $(LLIB)motor_pos.o  $(LLIB)motor_INIT.o \
		$(LLIB)bit_SET32.o $(LLIB)ss_init.o    $(LLIB)ss_prog.o \
		$(LLIB)boswrite.o  $(LLIB)ss_status.o  $(LLIB)add_status.o

LIBRARIES     =	$(LLIB)liblaser.a

EXECUTABLES   =	unix_master

SUPPORT       =	laser_enable  laser_disable  lpoweron  lpoweroff \
		get_config    ss_receive     status    trigger_status


all:  unix_master $(SUPPORT)


unix_master: unix_master.c $(LIBRARIES) $(INCLUDES)
	cc -c unix_master.c $(INC_CLAS) $(INC_CODA) \
	$(INC_ECLASER) -I$(RTHOME)/include
	rtlink -cxx unix_master.o  $(LINK_ECLASER) $(LINK_CODA) \
	$(LINK_CLAS) -lsocket -lnsl  -o unix_master

laser_enable: laser_enable.c $(LIBRARIES) $(INCLUDES)
	cc -c laser_enable.c $(INC_CLAS) $(INC_CODA) $(INC_ECLASER)
	rtlink -cxx laser_enable.o $(LINK_CLAS) $(LINK_CODA) \
	$(LINK_ECLASER)  -o laser_enable

laser_disable: laser_disable.c $(LIBRARIES) $(INCLUDES)
	cc -c laser_disable.c $(INC_CLAS) $(INC_CODA) $(INC_ECLASER)
	rtlink -cxx laser_disable.o $(LINK_CLAS) $(LINK_CODA) \
	$(LINK_ECLASER)  -o laser_disable

lpoweron: lpoweron.c $(LIBRARIES) $(INCLUDES)
	cc -c lpoweron.c $(INC_CLAS) $(INC_CODA) $(INC_ECLASER)
	rtlink -cxx lpoweron.o $(LINK_CODA) $(INC_ECLASER) \
	$(LINK_ECLASER)  -o lpoweron

lpoweroff: lpoweroff.c $(LIBRARIES) $(INCLUDES)
	cc -c lpoweroff.c $(INC_CLAS) $(INC_CODA) $(INC_ECLASER)
	rtlink -cxx lpoweroff.o $(LINK_CODA) $(INC_ECLASER) \
	$(LINK_ECLASER)  -o lpoweroff

get_config: get_config.c $(LIBRARIES) $(INCLUDES)
	cc -c get_config.c $(INC_CLAS) $(LINK_CLAS) $(INC_CODA) \
	$(LINK_CODA) $(INC_ECLASER) $(LINK_ECLASER)
	rtlink -cxx get_config.o $(INC_CLAS) $(LINK_CLAS) $(INC_CODA) \
	$(LINK_CODA) $(INC_ECLASER) $(LINK_ECLASER)  -o get_config

ss_receive: ss_receive.c $(LIBRARIES) $(INCLUDES)
	cc -c ss_receive.c $(INC_CLAS) $(INC_CODA) $(INC_ECLASER) \
	$(LINK_ECLASER) $(LINK_CODA) $(LINK_CLAS) -I$(RTHOME)/include
	rtlink -cxx ss_receive.o $(INC_CLAS) $(INC_CODA) $(INC_ECLASER) \
	$(LINK_ECLASER) $(LINK_CODA) $(LINK_CLAS)  -o ss_receive

status: status.c $(LIBRARIES) $(INCLUDES)
	cc -c status.c $(INC_CLAS) $(LINK_CLAS) $(INC_CODA) $(INC_ECLASER)
	rtlink -cxx status.o $(LINK_CODA) $(LINK_ECLASER)  -o status

trigger_status: trigger_status.c $(LIBRARIES) $(INCLUDES)
	cc -c trigger_status.c $(INC_CLAS) $(LINK_CLAS) $(INC_CODA) \
	$(LINK_CODA) $(INC_ECLASER) $(LINK_ECLASER)
	rtlink -cxx trigger_status.o $(INC_CLAS) $(LINK_CLAS) $(INC_CODA) \
	$(LINK_CODA) $(INC_ECLASER) $(LINK_ECLASER)  -o trigger_status


#---------------------------------------------------------------------
#---------------------------------------------------------------------


$(LIBRARIES): $(ROUTINES) $(INCLUDES)
	@echo "=====   updating librariries   ====="

$(LLIB)bit_split.o: $(LLIB)bit_split.c $(INCLUDES)
	@echo "=====   updating bit_split   ====="
	$(CC) -c  $(LLIB)bit_split.c $(INC_CLAS) \
	$(INC_CODA) $(INC_ECLASER)    -o$(LLIB)bit_split.o
	$(AR) $(LLIB)liblaser.a         $(LLIB)bit_split.o

$(LLIB)parse.o: $(LLIB)parse.c $(INCLUDES)
	@echo "=====   updating parse   ====="
	$(CC) -c  $(LLIB)parse.c $(INC_CLAS) \
	$(INC_CODA) $(INC_ECLASER)    -o$(LLIB)parse.o
	$(AR) $(LLIB)liblaser.a         $(LLIB)parse.o

$(LLIB)parsetmp.o: $(LLIB)parsetmp.c $(INCLUDES)
	@echo "=====   updating parsetmp   ====="
	$(CC) -c  $(LLIB)parsetmp.c $(INC_CLAS) \
	$(INC_CODA) $(INC_ECLASER)    -o$(LLIB)parsetmp.o
	$(AR) $(LLIB)liblaser.a         $(LLIB)parsetmp.o

$(LLIB)motor_idle.o: $(LLIB)motor_idle.c $(INCLUDES)
	@echo "=====   updating motor_idle   ====="
	$(CC) -c  $(LLIB)motor_idle.c $(INC_CLAS) \
	$(INC_CODA) $(INC_ECLASER)    -o$(LLIB)motor_idle.o
	$(AR) $(LLIB)liblaser.a         $(LLIB)motor_idle.o

$(LLIB)motor_pos.o: $(LLIB)motor_pos.c $(INCLUDES)
	@echo "=====   updating motor_pos   ====="
	$(CC) -c  $(LLIB)motor_pos.c $(INC_CLAS) \
	$(INC_CODA) $(INC_ECLASER)    -o$(LLIB)motor_pos.o
	$(AR) $(LLIB)liblaser.a         $(LLIB)motor_pos.o

$(LLIB)motor_INIT.o: $(LLIB)motor_INIT.c $(INCLUDES)
	@echo "=====   updating motor_INIT   ====="
	$(CC) -c  $(LLIB)motor_INIT.c $(INC_CLAS) \
	$(INC_CODA) $(INC_ECLASER)    -o$(LLIB)motor_INIT.o
	$(AR) $(LLIB)liblaser.a         $(LLIB)motor_INIT.o

$(LLIB)bit_TST.o: $(LLIB)bit_TST.c $(INCLUDES)
	@echo "=====   updating bit_TST   ====="
	$(CC) -c  $(LLIB)bit_TST.c $(INC_CLAS) \
	$(INC_CODA) $(INC_ECLASER)    -o$(LLIB)bit_TST.o
	$(AR) $(LLIB)liblaser.a         $(LLIB)bit_TST.o

$(LLIB)bit_SET.o: $(LLIB)bit_SET.c $(INCLUDES)
	@echo "=====   updating bit_SET   ====="
	$(CC) -c  $(LLIB)bit_SET.c $(INC_CLAS) \
	$(INC_CODA) $(INC_ECLASER)    -o$(LLIB)bit_SET.o
	$(AR) $(LLIB)liblaser.a         $(LLIB)bit_SET.o

$(LLIB)limit.o: $(LLIB)limit.c $(INCLUDES)
	@echo "=====   updating limit   ====="
	$(CC) -c  $(LLIB)limit.c $(INC_CLAS) \
	$(INC_CODA) $(INC_ECLASER)    -o$(LLIB)limit.o
	$(AR) $(LLIB)liblaser.a         $(LLIB)limit.o

$(LLIB)limit_TST.o: $(LLIB)limit_TST.c $(INCLUDES)
	@echo "=====   updating limit_TST   ====="
	$(CC) -c  $(LLIB)limit_TST.c $(INC_CLAS) \
	$(INC_CODA) $(INC_ECLASER)    -o$(LLIB)limit_TST.o
	$(AR) $(LLIB)liblaser.a         $(LLIB)limit_TST.o

$(LLIB)bit_SET32.o: $(LLIB)bit_SET32.c $(INCLUDES)
	@echo "=====   updating bit_SET32   ====="
	$(CC) -c  $(LLIB)bit_SET32.c $(INC_CLAS) \
	$(INC_CODA) $(INC_ECLASER)    -o$(LLIB)bit_SET32.o
	$(AR) $(LLIB)liblaser.a         $(LLIB)bit_SET32.o

### following use CCRT and get the RT includes

$(LLIB)boswrite.o: $(LLIB)boswrite.c $(INCLUDES)
	@echo "=====   updating boswrite   ====="
	$(CCRT) -c  $(LLIB)boswrite.c $(INC_CLAS) \
	$(INC_CODA) $(INC_ECLASER)    -o$(LLIB)boswrite.o
	$(AR) $(LLIB)liblaser.a         $(LLIB)boswrite.o

$(LLIB)ss_status.o: $(LLIB)ss_status.c $(INCLUDES)
	@echo "=====   updating ss_status   ====="
	$(CCRT) -c  $(LLIB)ss_status.c $(INC_CLAS) \
	$(INC_CODA) $(INC_ECLASER)    -o$(LLIB)ss_status.o
	$(AR) $(LLIB)liblaser.a         $(LLIB)ss_status.o
  
$(LLIB)ss_init.o: $(LLIB)ss_init.c $(INCLUDES)
	@echo "=====   updating ss_init   ====="
	$(CCRT) -c  $(LLIB)ss_init.c $(INC_CLAS) \
	$(INC_CODA) $(INC_ECLASER)    -o$(LLIB)ss_init.o
	$(AR) $(LLIB)liblaser.a         $(LLIB)ss_init.o

$(LLIB)ss_prog.o: $(LLIB)ss_prog.c $(INCLUDES)
	@echo "=====   updating ss_prog   ====="
	$(CCRT) -c  $(LLIB)ss_prog.c $(INC_CLAS) \
	$(INC_CODA) $(INC_ECLASER)    -o$(LLIB)ss_prog.o
	$(AR) $(LLIB)liblaser.a         $(LLIB)ss_prog.o

$(LLIB)add_status.o: $(LLIB)add_status.c $(INCLUDES)
	@echo "=====   updating add_status   ====="
	$(CCRT) -c  $(LLIB)add_status.c $(INC_CLAS) \
	$(INC_CODA) $(INC_ECLASER)    -o$(LLIB)add_status.o
	$(AR) $(LLIB)liblaser.a         $(LLIB)add_status.o


#---------------------------------------------------------------------
#---------------------------------------------------------------------


Jpulser_test: Jpulser_test.c $(LIBRARIES) $(INCLUDES)
	$(CCRT) Jpulser_test.c $(INC_CLAS) $(LINK_CLAS) $(INC_CODA) \
	$(LINK_CODA) $(INC_ECLASER) $(LINK_ECLASER)  -o Jpulser_test

cal_motor_init: cal_motor_init.c $(LIBRARIES) $(INCLUDES)
	$(CCRT) cal_motor_init.c $(INC_CLAS) $(LINK_CLAS) $(INC_CODA) \
	$(LINK_CODA) $(INC_ECLASER) $(LINK_ECLASER)  -o cal_motor_init

gason: gason.c $(LIBRARIES) $(INCLUDES)
	$(CCRT) gason.c $(INC_CLAS) $(LINK_CLAS) $(INC_CODA) \
	$(LINK_CODA) $(INC_ECLASER) $(LINK_ECLASER)  -o gason

gasoff: gasoff.c $(LIBRARIES) $(INCLUDES)
	$(CCRT) gasoff.c $(INC_CLAS) $(LINK_CLAS) $(INC_CODA) \
	$(LINK_CODA) $(INC_ECLASER) $(LINK_ECLASER)  -o gasoff

getstatus: getstatus.c $(LIBRARIES) $(INCLUDES)
	$(CCRT) getstatus.c $(INC_CLAS) $(LINK_CLAS) $(INC_CODA) \
	$(LINK_CODA) $(INC_ECLASER) $(LINK_ECLASER)  -o getstatus

laser_motor: laser_motor.c $(LIBRARIES) $(INCLUDES)
	$(CCRT) laser_motor.c $(INC_CLAS) $(LINK_CLAS) $(INC_CODA) \
	$(LINK_CODA) $(INC_ECLASER) $(LINK_ECLASER)  -o laser_motor

laser_motor_com: laser_motor_com.c $(LIBRARIES) $(INCLUDES)
	$(CCRT) laser_motor_com.c $(INC_CLAS) $(LINK_CLAS) $(INC_CODA) \
	$(LINK_CODA) $(INC_ECLASER) $(LINK_ECLASER)  -o laser_motor_com

laser_motor_init: laser_motor_init.c $(LIBRARIES) $(INCLUDES)
	$(CCRT) laser_motor_init.c $(INC_CLAS) $(LINK_CLAS) $(INC_CODA) \
	$(LINK_CODA) $(INC_ECLASER) $(LINK_ECLASER)  -o laser_motor_init

laser_motor_status: laser_motor_status.c $(LIBRARIES) $(INCLUDES)
	$(CCRT) laser_motor_status.c $(INC_CLAS) $(LINK_CLAS) $(INC_CODA) \
	$(LINK_CODA) $(INC_ECLASER) $(LINK_ECLASER)  -o laser_motor_status

laser_motor_test: laser_motor_test.c $(LIBRARIES) $(INCLUDES)
	cc -c laser_motor_test.c $(INC_CLAS) $(INC_CODA) \
	$(INC_ECLASER) -I$(RTHOME)/include
	rtlink -cxx laser_motor_test.o $(LINK_ECLASER) $(LINK_CODA) \
	$(LINK_CLAS) -lsocket -lnsl  -o laser_motor_test

make_pulser_data: make_pulser_data.c $(LIBRARIES) $(INCLUDES)
	$(CCRT) make_pulser_data.c $(INC_CLAS) $(LINK_CLAS) $(INC_CODA) \
	$(LINK_CODA) $(INC_ECLASER) $(LINK_ECLASER)  -o make_pulser_data

send_command: send_command.c $(LIBRARIES) $(INCLUDES)
	cc -c send_command.c $(INC_CLAS) $(INC_CODA) \
	$(INC_ECLASER)  -I$(RTHOME)/include
	rtlink -cxx send_command.o  $(LINK_ECLASER) $(LINK_CODA) \
	$(LINK_CLAS) -lsocket -lnsl  -o send_command

set_filter: set_filter.c $(LIBRARIES) $(INCLUDES)
	$(CCRT) set_filter.c $(INC_CLAS) $(LINK_CLAS) $(INC_CODA) \
	$(LINK_CODA) $(INC_ECLASER) $(LINK_ECLASER)  -o set_filter

ss_send: ss_send.c $(LIBRARIES) $(INCLUDES)
	$(CCRT) ss_send.c $(INC_CLAS) $(INC_CODA) $(INC_ECLASER) \
	$(LINK_ECLASER) $(LINK_CODA) $(LINK_CLAS)  -o ss_send

stepperon: stepperon.c $(LIBRARIES) $(INCLUDES)
	$(CCRT) stepperon.c $(INC_CLAS) $(LINK_CLAS) $(INC_CODA) \
	$(LINK_CODA) $(INC_ECLASER) $(LINK_ECLASER)  -o stepperon

stepperoff: stepperoff.c $(LIBRARIES) $(INCLUDES)
	$(CCRT) stepperoff.c $(INC_CLAS) $(LINK_CLAS) $(INC_CODA) \
	$(LINK_CODA) $(INC_ECLASER) $(LINK_ECLASER)  -o stepperoff


#---------------------------------------------------------------------
#---------------------------------------------------------------------
#---------------------------------------------------------------------


check:
	ar -t	$(LLIB)liblaser.a	
	nm	$(LLIB)liblaser.a
#	@echo "listing others"
#	ar -t	$(CLON_LIB)/libipc.a
#	nm	$(CLON_LIB)/libipc.a


install: update
exports: update
export:  update
update:
	cp $(EXECUTABLES)	$(CLON_BIN)
	cp laser_enable		$(CLON_BIN)/EC_laser_enable
	cp laser_disable	$(CLON_BIN)/EC_laser_disable
	cp lpoweron		$(CLON_BIN)/EC_laser_on
	cp lpoweroff		$(CLON_BIN)/EC_laser_off
	cp get_config		$(CLON_BIN)/EC_laser_get_config
	cp ss_receive		$(CLON_BIN)/EC_laser_receive
	cp status		$(CLON_BIN)/EC_laser_status
	cp trigger_status	$(CLON_BIN)/EC_trigger_status
	cp EC_laser_check.csh	$(CLON_BIN)
	-chmod -f ug+rw		$(CLON_BIN)/EC_*
	-chmod -f a+x		$(CLON_BIN)/EC_*


loadConf:
	cp config/configuration.txt $(CLON_PARMS)/EC_config


clean:
	rm -f *.o
	rm -f lib/*.o


distclean:
	rm -f *.o
	rm -f lib/*.o
	rm -f $(EXECUTABLES) $(SUPPORT) core
	rm -f lib/liblaser.a
	rm -f Jpulser_test
	rm -f cal_motor_init
	rm -f gason
	rm -f gasoff
	rm -f getstatus
	rm -f laser_motor
	rm -f laser_motor_com
	rm -f laser_motor_init
	rm -f laser_motor_status
	rm -f laser_motor_test
	rm -f make_pulser_data
	rm -f send_command
	rm -f set_filter
	rm -f ss_send
	rm -f stepperon
	rm -f stepperoff


