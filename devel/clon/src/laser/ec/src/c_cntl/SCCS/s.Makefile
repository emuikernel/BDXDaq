h43134
s 00002/00002/00388
d D 1.13 04/08/11 17:43:10 sergpozd 14 13
c *** empty log message ***
e
s 00265/00300/00125
d D 1.12 04/08/11 17:06:24 sergpozd 13 12
c *** empty log message ***
e
s 00006/00002/00419
d D 1.11 03/04/25 10:14:14 giovanet 12 11
c changed the send_command make part it now works
c 
e
s 00001/00001/00420
d D 1.10 03/04/25 09:42:15 giovanet 11 10
c errors in the laser_motor_test fixed
c 
e
s 00005/00000/00416
d D 1.9 03/04/25 09:39:18 giovanet 10 9
c included a laser_motor_test compile and link piece
c 
e
s 00042/00042/00374
d D 1.8 01/03/10 15:12:33 giovanet 9 8
c remove the -lC from all links (42 occurences)
c 
e
s 00001/00001/00415
d D 1.7 01/03/10 15:09:28 giovanet 8 7
c remove -lC from lpoweroff
e
s 00001/00001/00415
d D 1.6 01/03/10 15:03:17 giovanet 7 6
c problem with -lC try removing it
c 
c 
e
s 00003/00000/00413
d D 1.5 00/10/30 11:57:30 wolin 6 5
c Fixed distclean
e
s 00020/00010/00393
d D 1.4 00/10/26 16:30:20 wolin 5 4
c Major mods for new compiler, SS53, etc.
e
s 00004/00000/00399
d D 1.3 00/10/05 15:51:01 wolin 4 3
c Protections
e
s 00001/00000/00398
d D 1.2 00/09/21 14:25:38 wolin 3 1
c Added exports
e
s 00000/00000/00000
d R 1.2 00/09/21 14:22:45 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 laser/ec/src/c_cntl/Makefile
e
s 00398/00000/00000
d D 1.1 00/09/21 14:22:44 wolin 1 0
c date and time created 00/09/21 14:22:44 by wolin
e
u
U
f e 0
t
T
I 1
###
# as you comple modules:
D 13
#  executable that test and do basic support should have
#   compile link sequence
#   should be listed on the support line so that they will be compiled in one step for all support prog
#   should be listed in the update part if theya re for general use  

E 13
I 13
#   executable that test and do basic support should have compile
#   link sequence should be listed on the support line so that
#   they will be compiled in one step for all support prog should
#   be listed in the update part if theya re for general use  
#
E 13
# -V  is verbose
D 13


E 13
I 13
#
#
E 13
# ipcbank2dd: ipcbank2dd.o
#	rtlink -cxx $(DEBUG) -o $@ $@.o  $(CLON_WSOURCE)/run_log/s/dd_fpack.o
#	-L$(CLON_LIB) -lipc -lutil -L$(CMLOG)/lib/solaris -lcmlog 
#	-L$(CODA)/SunOS/lib -ldd -ltcl -lmsql  -lposix4
D 13

# update will copy the necsary files to the general executable area of clasrun
#        because i will be doing both TOF and EC i will include speciffically each module that
#        icopy and need to rename so that there is no conflict
#     eg lpoweroff  becomes EC_laser_off  ... or TOF_laser_off

E 13
I 13
#
# update will copy the necsary files to the general executable area
#   of clasrun because I will be doing both TOF and EC I will include
#   speciffically each module that I copy and need to rename so that
#   there is no conflict eg:
#      lpoweroff  becomes EC_laser_off
#                  ... or TOF_laser_off
#
#
E 13
# Makefile for /~clasrun/jmu/c_cntl
#  
# Kevin Giovanetti [giovankl@jmu.edu]
#
# I should improve to use many of the default make rules
#
# All usable routines will be kept in the library area and put into liblaser.a
D 13
#  each module will need to be entered in to the ROUTINES list and a seperate target
#  created for use see parse.  All include files will be kept in the include area
#  just add new header files to the INCLUDE list (no commas).
E 13
I 13
#  each module will need to be entered in to the ROUTINES list and a seperate
#  target created for use see parse.  All include files will be kept in the
#  include area just add new header files to the INCLUDE list (no commas).
E 13
#  
#  use  gmake
#        gmake -v     = verbose
#
D 13
# CCRT in this compiler is for use with the smart sockets stuff seems to be necessary for DP_cmd calls
E 13
I 13
# CCRT in this compiler is for use with the smart sockets stuff seems
#  to be necessary for DP_cmd calls
E 13

CCRT =  rtlink -g

D 13
#command to build a library
E 13
I 13
# command to build a library
E 13
AR = ar -r 

D 13
#compiler used to build the routines
E 13
I 13
# compiler used to build the routines
E 13
CC =  cc 

D 13
#define the local areas for libraries and include files.
E 13
I 13
# define the local areas for libraries and include files.
E 13
LINC  = ./include/
LLIB  = ./lib/
LLIB1 = ./lib

D 13
#create pointers to libraries and include files for the compile and link steps
# I had trouble with the libraries and had to reorder them to link. If there are unresolved
#references consider appending  a library at the end. You can repeat libraries.
# Ichanges ss_send, unix_master, ss_recieive  other have sames libs and INCS but old order.
# note -L  indicates the directory followed by -lxxx where libxxx.a is the library and
#the dir is the preceding -L
E 13
I 13
# Create pointers to libraries and include files for the compile
# and link steps, I had trouble with the libraries and had to reorder
# them to link. If there are unresolved references consider appending
# a library at the end. You can repeat libraries.
# I changes ss_send, unix_master, ss_recieive  other have sames
# libs and INCS but old order.
# Note -L  indicates the directory followed by -lxxx where libxxx.a
# is the library and the dir is the preceding -L
E 13

D 13
# jan 200 remove 2 refs to -ldd
E 13
I 13
INC_ECLASER  =	-I./include
LINK_ECLASER =	-L$(LLIB1) -llaser
E 13

D 13
INC_ECLASER   = -I./include
LINK_ECLASER  = -L$(LLIB1) -llaser
INC_CLAS    = -I$(CLON_INC)
LINK_CLAS   =  -L$(CLON_LIB) -lipc -lutil -L$(CMLOG)/lib/solaris -lcmlog -L$(CODA)/SunOS/lib  -ltcl -lmsql  -lposix4
INC_CODA    = -I$(CODA)/common/include
LINK_CODA  = -L$(CODA)/SunOS/lib -lmsql  -ldpS -L/usr/local/coda/2.0b1/SunOS/lib -lca 
E 13
I 13
INC_CLAS     =	-I$(CLON_INC)
LINK_CLAS    =	-L$(CLON_LIB) -lipc -lutil
#LINK_CLAS    =	-L$(CLON_LIB) -lipc -lutil -L$(CMLOG)/lib/solaris -lcmlog \
#		-L$(CODA)/SunOS/lib  -ltcl -lmsql  -lposix4
E 13

D 13
BIN_DIR    = $(CLON_BIN)
PARMS_DIR  = $(CLON_PARMS)/EC_config
E 13
I 13
INC_CODA     =	-I$(CODA)/common/include
LINK_CODA    =	-L$(CODA)/SunOS/lib -lmsql  -ldpS -L/usr/local/coda/2.0b1/SunOS/lib -lca
E 13

D 13
#lists    
#  add name of all local header files  and put includes as a dependency where needed.   
#  add the names of all routine to the list and build a target with command list for construction
#  include the library list to those routines that need it but do nat add any addtional libraries it
#    is used as a target.
E 13
I 13
#lists
#  add name of all local header files  and put includes as a dependency
#  where needed, add the names of all routine to the list and build a
#  target with command list for construction include the library list
#  to those routines that need it but do nat add any addtional libraries
#  it is used as a target.
E 13

D 13
INCLUDES = $(LINC)laser.h $(LINC)bit_defs.h  $(LINC)parse.h  $(LINC)ssinclude.h 
ROUTINES = $(LLIB)parse.o $(LLIB)motor_idle.o $(LLIB)bit_TST.o $(LLIB)bit_SET.o $(LLIB)limit.o $(LLIB)limit_TST.o $(LLIB)bit_split.o $(LLIB)motor_pos.o $(LLIB)motor_INIT.o $(LLIB)bit_SET32.o $(LLIB)ss_init.o  $(LLIB)ss_prog.o $(LLIB)boswrite.o $(LLIB)ss_status.o $(LLIB)add_status.o 
LIBRARIES = $(LLIB)liblaser.a
E 13
I 13
INCLUDES      =	$(LINC)laser.h $(LINC)bit_defs.h  $(LINC)parse.h  $(LINC)ssinclude.h
E 13

D 13
EXECUTABLES = unix_master
E 13
I 13
ROUTINES      =	$(LLIB)parse.o     $(LLIB)motor_idle.o $(LLIB)bit_TST.o \
		$(LLIB)bit_SET.o   $(LLIB)limit.o      $(LLIB)limit_TST.o \
		$(LLIB)bit_split.o $(LLIB)motor_pos.o  $(LLIB)motor_INIT.o \
		$(LLIB)bit_SET32.o $(LLIB)ss_init.o    $(LLIB)ss_prog.o \
		$(LLIB)boswrite.o  $(LLIB)ss_status.o  $(LLIB)add_status.o
E 13

D 13
SUPPORT = lpoweroff lpoweron status laser_disable laser_enable ss_receive trigger_status camac3_status get_config 
E 13
I 13
LIBRARIES     =	$(LLIB)liblaser.a
E 13

I 13
EXECUTABLES   =	unix_master
E 13

D 13
all:  unix_master $(SUPPORT)
I 3
exports: update
E 3
export: update
install: update 
E 13
I 13
SUPPORT       =	laser_enable  laser_disable  lpoweron  lpoweroff \
		get_config    ss_receive     status    trigger_status
E 13

D 13
 
# Jpulser_test  set_bit_11 
# 
E 13

D 13
parsetmp: $(LLIB)parsetmp.o
E 13
I 13
all:  unix_master $(SUPPORT)
E 13

D 13
unix_master:unix_master.c $(LIBRARIES) $(INCLUDES)
D 5
	$(CCRT) unix_master.c  $(INC_CLAS)  $(INC_CODA) $(INC_ECLASER) $(LINK_ECLASER) $(LINK_CODA) $(LINK_CLAS) -lC  -o unix_master
E 5
I 5
	cc -c unix_master.c $(INC_CLAS)  $(INC_CODA) $(INC_ECLASER)  -I$(RTHOME)/include
D 7
	rtlink -cxx unix_master.o  $(LINK_ECLASER) $(LINK_CODA) $(LINK_CLAS) -lsocket -lnsl -lC -o unix_master
E 7
I 7
	rtlink -cxx unix_master.o  $(LINK_ECLASER) $(LINK_CODA) $(LINK_CLAS) -lsocket -lnsl  -o unix_master
E 13
E 7
E 5

D 13
test2_EC:test2_EC.c $(LIBRARIES) $(INCLUDES)
D 9
	$(CCRT) test2_EC.c  $(INC_CLAS)  $(INC_CODA) $(INC_ECLASER) $(LINK_ECLASER) $(LINK_CODA) $(LINK_CLAS) -lC  -o test2_EC
E 9
I 9
	$(CCRT) test2_EC.c  $(INC_CLAS)  $(INC_CODA) $(INC_ECLASER) $(LINK_ECLASER) $(LINK_CODA) $(LINK_CLAS)    -o test2_EC
E 13
I 13
unix_master: unix_master.c $(LIBRARIES) $(INCLUDES)
	cc -c unix_master.c $(INC_CLAS) $(INC_CODA) \
	$(INC_ECLASER) -I$(RTHOME)/include
	rtlink -cxx unix_master.o  $(LINK_ECLASER) $(LINK_CODA) \
	$(LINK_CLAS) -lsocket -lnsl  -o unix_master
E 13
E 9

D 13
master2:master2.c $(LIBRARIES) $(INCLUDES)
D 9
	$(CCRT) master2.c  $(INC_CLAS)  $(INC_CODA) $(INC_ECLASER) $(LINK_ECLASER) $(LINK_CODA) $(LINK_CLAS) -lC  -o master2
E 9
I 9
	$(CCRT) master2.c  $(INC_CLAS)  $(INC_CODA) $(INC_ECLASER) $(LINK_ECLASER) $(LINK_CODA) $(LINK_CLAS)    -o master2
E 13
I 13
laser_enable: laser_enable.c $(LIBRARIES) $(INCLUDES)
	cc -c laser_enable.c $(INC_CLAS) $(INC_CODA) $(INC_ECLASER)
	rtlink -cxx laser_enable.o $(LINK_CLAS) $(LINK_CODA) \
	$(LINK_ECLASER)  -o laser_enable
E 13
E 9

D 12
send_command: send_command.c
D 9
	$(CCRT) send_command.c $(INC_CLAS) $(LINK_CLAS) $(INC_CODA) $(LINK_CODA) $(INC_ECLASER) $(LINK_ECLASER) -lC  -o send_command
E 9
I 9
	$(CCRT) send_command.c $(INC_CLAS) $(LINK_CLAS) $(INC_CODA) $(LINK_CODA) $(INC_ECLASER) $(LINK_ECLASER)    -o send_command
E 12
I 12
D 13
### send_command: send_command.c
###	cc -c  send_command.c $(INC_CLAS) $(LINK_CLAS) $(INC_CODA) $(LINK_CODA) $(INC_ECLASER) $(LINK_ECLASER)    -o send_command
E 12
E 9

I 12

send_command:send_command.c $(LIBRARIES) $(INCLUDES)
	cc -c send_command.c $(INC_CLAS)  $(INC_CODA) $(INC_ECLASER)  -I$(RTHOME)/include
	rtlink -cxx send_command.o  $(LINK_ECLASER) $(LINK_CODA) $(LINK_CLAS) -lsocket -lnsl  -o send_command
E 12
sendcom: sendcom.c $(LIBRARIES) $(INCLUDES)
D 9
	$(CCRT) sendcom.c $(INC_CLAS) $(LINK_CLAS) $(INC_CODA) $(LINK_CODA) $(INC_ECLASER) $(LINK_ECLASER) -lC  -o sendcom
E 9
I 9
	$(CCRT) sendcom.c $(INC_CLAS) $(LINK_CLAS) $(INC_CODA) $(LINK_CODA) $(INC_ECLASER) $(LINK_ECLASER)    -o sendcom
E 9

status: status.c $(LIBRARIES) $(INCLUDES)
	@echo "updating status"
D 5
	$(CCRT) status.c $(INC_CLAS) $(LINK_CLAS) $(INC_CODA) $(LINK_CODA) $(INC_ECLASER) $(LINK_ECLASER) -lC  -o status
E 5
I 5
	cc -c status.c $(INC_CLAS) $(LINK_CLAS) $(INC_CODA) $(INC_ECLASER) 
D 9
	rtlink -cxx status.o $(LINK_CODA) $(LINK_ECLASER) -lC  -o status
E 9
I 9
	rtlink -cxx status.o $(LINK_CODA) $(LINK_ECLASER)    -o status
E 9
E 5

E 13
laser_disable: laser_disable.c $(LIBRARIES) $(INCLUDES)
D 13
	@echo "updating laser_disable"
D 5
	$(CCRT) laser_disable.c $(INC_CLAS) $(LINK_CLAS) $(INC_CODA) $(LINK_CODA) $(INC_ECLASER) $(LINK_ECLASER) -lC  -o laser_disable
E 5
I 5
	cc -c laser_disable.c $(INC_CLAS) $(INC_CODA) $(INC_ECLASER) 
D 9
	rtlink -cxx laser_disable.o $(LINK_CLAS) $(LINK_CODA) $(LINK_ECLASER) -lC  -o laser_disable
E 9
I 9
	rtlink -cxx laser_disable.o $(LINK_CLAS) $(LINK_CODA) $(LINK_ECLASER)    -o laser_disable
E 13
I 13
	cc -c laser_disable.c $(INC_CLAS) $(INC_CODA) $(INC_ECLASER)
	rtlink -cxx laser_disable.o $(LINK_CLAS) $(LINK_CODA) \
	$(LINK_ECLASER)  -o laser_disable
E 13
E 9
E 5

D 13
laser_enable: laser_enable.c $(LIBRARIES) $(INCLUDES)
	@echo "updating laser_enable"
D 5
	$(CCRT) laser_enable.c $(INC_CLAS) $(LINK_CLAS) $(INC_CODA) $(LINK_CODA) $(INC_ECLASER) $(LINK_ECLASER) -lC  -o laser_enable
E 5
I 5
	cc -c laser_enable.c $(INC_CLAS) $(INC_CODA) $(INC_ECLASER) 
D 9
	rtlink -cxx laser_enable.o $(LINK_CLAS) $(LINK_CODA) $(LINK_ECLASER) -lC  -o laser_enable
E 9
I 9
	rtlink -cxx laser_enable.o $(LINK_CLAS) $(LINK_CODA) $(LINK_ECLASER)    -o laser_enable
E 9
E 5

lpoweroff: lpoweroff.c $(LIBRARIES) $(INCLUDES)
	@echo "updating lpoweroff"
D 5
	$(CCRT) lpoweroff.c $(INC_CLAS) $(LINK_CLAS) $(INC_CODA) $(LINK_CODA) $(INC_ECLASER) $(LINK_ECLASER) -lC  -o lpoweroff
E 5
I 5
	cc -c lpoweroff.c $(INC_CLAS) $(LINK_CLAS) $(INC_CODA) 
D 8
	rtlink -cxx lpoweroff.o $(LINK_CODA) $(INC_ECLASER) $(LINK_ECLASER) -lC  -o lpoweroff
E 8
I 8
	rtlink -cxx lpoweroff.o $(LINK_CODA) $(INC_ECLASER) $(LINK_ECLASER)   -o lpoweroff
E 8
E 5

EC_laser_check:EC_laser_check.csh
	chmod +x EC_laser_check.csh

gason: gason.c $(LIBRARIES) $(INCLUDES)
	@echo "updating gason"
D 9
	$(CCRT) gason.c $(INC_CLAS) $(LINK_CLAS) $(INC_CODA) $(LINK_CODA) $(INC_ECLASER) $(LINK_ECLASER) -lC  -o gason
E 9
I 9
	$(CCRT) gason.c $(INC_CLAS) $(LINK_CLAS) $(INC_CODA) $(LINK_CODA) $(INC_ECLASER) $(LINK_ECLASER)    -o gason
E 9

gasoff: gasoff.c $(LIBRARIES) $(INCLUDES)
	@echo "updating gasoff"
D 9
	$(CCRT) gasoff.c $(INC_CLAS) $(LINK_CLAS) $(INC_CODA) $(LINK_CODA) $(INC_ECLASER) $(LINK_ECLASER) -lC  -o gasoff
E 9
I 9
	$(CCRT) gasoff.c $(INC_CLAS) $(LINK_CLAS) $(INC_CODA) $(LINK_CODA) $(INC_ECLASER) $(LINK_ECLASER)    -o gasoff
E 9

E 13
lpoweron: lpoweron.c $(LIBRARIES) $(INCLUDES)
D 13
	@echo "updating lpoweron"
E 13
D 5
	$(CCRT) lpoweron.c $(INC_CLAS) $(LINK_CLAS) $(INC_CODA) $(LINK_CODA) $(INC_ECLASER) $(LINK_ECLASER) -lC  -o lpoweron
E 5
I 5
D 14
	cc -c lpoweron.c $(INC_CLAS) $(LINK_CLAS) $(INC_CODA)
E 14
I 14
	cc -c lpoweron.c $(INC_CLAS) $(INC_CODA) $(INC_ECLASER)
E 14
D 9
	rtlink -cxx lpoweron.o $(LINK_CODA) $(INC_ECLASER) $(LINK_ECLASER) -lC  -o lpoweron
E 9
I 9
D 13
	rtlink -cxx lpoweron.o $(LINK_CODA) $(INC_ECLASER) $(LINK_ECLASER)    -o lpoweron
E 13
I 13
	rtlink -cxx lpoweron.o $(LINK_CODA) $(INC_ECLASER) \
	$(LINK_ECLASER)  -o lpoweron
E 13
E 9
E 5

D 13
stepperon: stepperon.c $(LIBRARIES) $(INCLUDES)
	@echo "updating stepperon"
D 9
	$(CCRT) stepperon.c $(INC_CLAS) $(LINK_CLAS) $(INC_CODA) $(LINK_CODA) $(INC_ECLASER) $(LINK_ECLASER) -lC  -o stepperon
E 9
I 9
	$(CCRT) stepperon.c $(INC_CLAS) $(LINK_CLAS) $(INC_CODA) $(LINK_CODA) $(INC_ECLASER) $(LINK_ECLASER)    -o stepperon
E 13
I 13
lpoweroff: lpoweroff.c $(LIBRARIES) $(INCLUDES)
D 14
	cc -c lpoweroff.c $(INC_CLAS) $(LINK_CLAS) $(INC_CODA)
E 14
I 14
	cc -c lpoweroff.c $(INC_CLAS) $(INC_CODA) $(INC_ECLASER)
E 14
	rtlink -cxx lpoweroff.o $(LINK_CODA) $(INC_ECLASER) \
	$(LINK_ECLASER)  -o lpoweroff
E 13
E 9

D 13
stepperoff: stepperoff.c $(LIBRARIES) $(INCLUDES)
	@echo "updating stepperoff"
D 9
	$(CCRT) stepperoff.c $(INC_CLAS) $(LINK_CLAS) $(INC_CODA) $(LINK_CODA) $(INC_ECLASER) $(LINK_ECLASER) -lC  -o stepperoff
E 9
I 9
	$(CCRT) stepperoff.c $(INC_CLAS) $(LINK_CLAS) $(INC_CODA) $(LINK_CODA) $(INC_ECLASER) $(LINK_ECLASER)    -o stepperoff
E 9
#11111
E 13
get_config: get_config.c $(LIBRARIES) $(INCLUDES)
D 13
	@echo "updating get_config"
D 5
	$(CCRT) get_config.c $(INC_CLAS) $(LINK_CLAS) $(INC_CODA) $(LINK_CODA) $(INC_ECLASER) $(LINK_ECLASER) -lC  -o get_config
E 5
I 5
	cc -c get_config.c $(INC_CLAS) $(LINK_CLAS) $(INC_CODA) $(LINK_CODA) $(INC_ECLASER) $(LINK_ECLASER)
D 9
	rtlink -cxx get_config.o $(INC_CLAS) $(LINK_CLAS) $(INC_CODA) $(LINK_CODA) $(INC_ECLASER) $(LINK_ECLASER) -lC  -o get_config
E 9
I 9
	rtlink -cxx get_config.o $(INC_CLAS) $(LINK_CLAS) $(INC_CODA) $(LINK_CODA) $(INC_ECLASER) $(LINK_ECLASER)    -o get_config
E 13
I 13
	cc -c get_config.c $(INC_CLAS) $(LINK_CLAS) $(INC_CODA) \
	$(LINK_CODA) $(INC_ECLASER) $(LINK_ECLASER)
	rtlink -cxx get_config.o $(INC_CLAS) $(LINK_CLAS) $(INC_CODA) \
	$(LINK_CODA) $(INC_ECLASER) $(LINK_ECLASER)  -o get_config
E 13
E 9
E 5

D 13
getstatus: getstatus.c $(LIBRARIES) $(INCLUDES)
	@echo "updating getstatus"
D 9
	$(CCRT) getstatus.c $(INC_CLAS) $(LINK_CLAS) $(INC_CODA) $(LINK_CODA) $(INC_ECLASER) $(LINK_ECLASER) -lC  -o getstatus
E 9
I 9
	$(CCRT) getstatus.c $(INC_CLAS) $(LINK_CLAS) $(INC_CODA) $(LINK_CODA) $(INC_ECLASER) $(LINK_ECLASER)    -o getstatus
E 13
I 13
ss_receive: ss_receive.c $(LIBRARIES) $(INCLUDES)
	cc -c ss_receive.c $(INC_CLAS) $(INC_CODA) $(INC_ECLASER) \
	$(LINK_ECLASER) $(LINK_CODA) $(LINK_CLAS) -I$(RTHOME)/include
	rtlink -cxx ss_receive.o $(INC_CLAS) $(INC_CODA) $(INC_ECLASER) \
	$(LINK_ECLASER) $(LINK_CODA) $(LINK_CLAS)  -o ss_receive
E 13
E 9

I 13
status: status.c $(LIBRARIES) $(INCLUDES)
	cc -c status.c $(INC_CLAS) $(LINK_CLAS) $(INC_CODA) $(INC_ECLASER)
	rtlink -cxx status.o $(LINK_CODA) $(LINK_ECLASER)  -o status
E 13

I 13
trigger_status: trigger_status.c $(LIBRARIES) $(INCLUDES)
	cc -c trigger_status.c $(INC_CLAS) $(LINK_CLAS) $(INC_CODA) \
	$(LINK_CODA) $(INC_ECLASER) $(LINK_ECLASER)
	rtlink -cxx trigger_status.o $(INC_CLAS) $(LINK_CLAS) $(INC_CODA) \
	$(LINK_CODA) $(INC_ECLASER) $(LINK_ECLASER)  -o trigger_status
E 13


D 13
ss_send:ss_send.c $(LIBRARIES) $(INCLUDES)
D 9
	$(CCRT) ss_send.c $(INC_CLAS)  $(INC_CODA) $(INC_ECLASER) $(LINK_ECLASER) $(LINK_CODA) $(LINK_CLAS) -lC  -o ss_send
E 9
I 9
	$(CCRT) ss_send.c $(INC_CLAS)  $(INC_CODA) $(INC_ECLASER) $(LINK_ECLASER) $(LINK_CODA) $(LINK_CLAS)    -o ss_send
E 13
I 13
#---------------------------------------------------------------------
#---------------------------------------------------------------------
E 13
E 9

D 13
ss_receive:ss_receive.c $(LIBRARIES) $(INCLUDES)
D 5
	$(CCRT) ss_receive.c  $(INC_CLAS)  $(INC_CODA) $(INC_ECLASER) $(LINK_ECLASER) $(LINK_CODA) $(LINK_CLAS) -lC  -o ss_receive
E 5
I 5
	cc -c ss_receive.c  $(INC_CLAS)  $(INC_CODA) $(INC_ECLASER) $(LINK_ECLASER) $(LINK_CODA) $(LINK_CLAS) -I$(RTHOME)/include
D 9
	rtlink -cxx ss_receive.o  $(INC_CLAS)  $(INC_CODA) $(INC_ECLASER) $(LINK_ECLASER) $(LINK_CODA) $(LINK_CLAS) -lC  -o ss_receive
E 9
I 9
	rtlink -cxx ss_receive.o  $(INC_CLAS)  $(INC_CODA) $(INC_ECLASER) $(LINK_ECLASER) $(LINK_CODA) $(LINK_CLAS)    -o ss_receive
E 13
E 9
E 5

D 13
laser_motor_com:laser_motor_com.c $(LIBRARIES) $(INCLUDES)
D 9
	$(CCRT) laser_motor_com.c $(INC_CLAS) $(LINK_CLAS) $(INC_CODA) $(LINK_CODA) $(INC_ECLASER) $(LINK_ECLASER) -lC  -o laser_motor_com
E 9
I 9
	$(CCRT) laser_motor_com.c $(INC_CLAS) $(LINK_CLAS) $(INC_CODA) $(LINK_CODA) $(INC_ECLASER) $(LINK_ECLASER)    -o laser_motor_com
E 9

E 13
$(LIBRARIES): $(ROUTINES) $(INCLUDES)
D 13
	@echo "updating librariries"
E 13
I 13
	@echo "=====   updating librariries   ====="
E 13

$(LLIB)bit_split.o: $(LLIB)bit_split.c $(INCLUDES)
D 13
	@echo "updating bit_split"
	$(CC) -c  $(LLIB)bit_split.c $(INC_CLAS) $(INC_CODA) $(INC_ECLASER)  -o$(LLIB)bit_split.o
	ls $(LLIB)bit_split.o  
	$(AR) $(LLIB)liblaser.a   $(LLIB)bit_split.o
E 13
I 13
	@echo "=====   updating bit_split   ====="
	$(CC) -c  $(LLIB)bit_split.c $(INC_CLAS) \
	$(INC_CODA) $(INC_ECLASER)    -o$(LLIB)bit_split.o
	$(AR) $(LLIB)liblaser.a         $(LLIB)bit_split.o
E 13

$(LLIB)parse.o: $(LLIB)parse.c $(INCLUDES)
D 13
	@echo "updating parse"
	$(CC) -c  $(LLIB)parse.c $(INC_CLAS) $(INC_CODA) $(INC_ECLASER)  -o$(LLIB)parse.o
	ls $(LLIB)parse.o  
	$(AR) $(LLIB)liblaser.a   $(LLIB)parse.o
E 13
I 13
	@echo "=====   updating parse   ====="
	$(CC) -c  $(LLIB)parse.c $(INC_CLAS) \
	$(INC_CODA) $(INC_ECLASER)    -o$(LLIB)parse.o
	$(AR) $(LLIB)liblaser.a         $(LLIB)parse.o
E 13

$(LLIB)parsetmp.o: $(LLIB)parsetmp.c $(INCLUDES)
D 13
	@echo "updating parsetmp"
	$(CC) -c  $(LLIB)parsetmp.c $(INC_CLAS) $(INC_CODA) $(INC_ECLASER)  -o$(LLIB)parsetmp.o
	ls $(LLIB)parsetmp.o  
	$(AR) $(LLIB)liblaser.a   $(LLIB)parsetmp.o
E 13
I 13
	@echo "=====   updating parsetmp   ====="
	$(CC) -c  $(LLIB)parsetmp.c $(INC_CLAS) \
	$(INC_CODA) $(INC_ECLASER)    -o$(LLIB)parsetmp.o
	$(AR) $(LLIB)liblaser.a         $(LLIB)parsetmp.o
E 13

$(LLIB)motor_idle.o: $(LLIB)motor_idle.c $(INCLUDES)
D 13
	@echo "updating motor_idle"
	$(CC) -c  $(LLIB)motor_idle.c $(INC_CLAS) $(INC_CODA) $(INC_ECLASER)  -o$(LLIB)motor_idle.o
	ls $(LLIB)motor_idle.o  
	$(AR) $(LLIB)liblaser.a   $(LLIB)motor_idle.o 
E 13
I 13
	@echo "=====   updating motor_idle   ====="
	$(CC) -c  $(LLIB)motor_idle.c $(INC_CLAS) \
	$(INC_CODA) $(INC_ECLASER)    -o$(LLIB)motor_idle.o
	$(AR) $(LLIB)liblaser.a         $(LLIB)motor_idle.o
E 13

D 13

E 13
$(LLIB)motor_pos.o: $(LLIB)motor_pos.c $(INCLUDES)
D 13
	@echo "updating motor_pos"
	$(CC) -c  $(LLIB)motor_pos.c $(INC_CLAS) $(INC_CODA) $(INC_ECLASER)  -o$(LLIB)motor_pos.o
	ls $(LLIB)motor_pos.o  
	$(AR) $(LLIB)liblaser.a   $(LLIB)motor_pos.o 
E 13
I 13
	@echo "=====   updating motor_pos   ====="
	$(CC) -c  $(LLIB)motor_pos.c $(INC_CLAS) \
	$(INC_CODA) $(INC_ECLASER)    -o$(LLIB)motor_pos.o
	$(AR) $(LLIB)liblaser.a         $(LLIB)motor_pos.o
E 13

$(LLIB)motor_INIT.o: $(LLIB)motor_INIT.c $(INCLUDES)
D 13
	@echo "updating motor_INIT"
	$(CC) -c  $(LLIB)motor_INIT.c $(INC_CLAS) $(INC_CODA) $(INC_ECLASER)  -o$(LLIB)motor_INIT.o
	ls $(LLIB)motor_INIT.o  
	$(AR) $(LLIB)liblaser.a   $(LLIB)motor_INIT.o 
E 13
I 13
	@echo "=====   updating motor_INIT   ====="
	$(CC) -c  $(LLIB)motor_INIT.c $(INC_CLAS) \
	$(INC_CODA) $(INC_ECLASER)    -o$(LLIB)motor_INIT.o
	$(AR) $(LLIB)liblaser.a         $(LLIB)motor_INIT.o

E 13
$(LLIB)bit_TST.o: $(LLIB)bit_TST.c $(INCLUDES)
D 13
	@echo "updating bit_TST "
	$(CC) -c  $(LLIB)bit_TST.c $(INC_CLAS) $(INC_CODA) $(INC_ECLASER)  -o$(LLIB)bit_TST.o
	ls $(LLIB)bit_TST.o  
	$(AR) $(LLIB)liblaser.a   $(LLIB)bit_TST.o 
E 13
I 13
	@echo "=====   updating bit_TST   ====="
	$(CC) -c  $(LLIB)bit_TST.c $(INC_CLAS) \
	$(INC_CODA) $(INC_ECLASER)    -o$(LLIB)bit_TST.o
	$(AR) $(LLIB)liblaser.a         $(LLIB)bit_TST.o
E 13

$(LLIB)bit_SET.o: $(LLIB)bit_SET.c $(INCLUDES)
D 13
	@echo "updating bit_SET "
	$(CC) -c  $(LLIB)bit_SET.c $(INC_CLAS) $(INC_CODA) $(INC_ECLASER)  -o$(LLIB)bit_SET.o
	ls $(LLIB)bit_SET.o  
	$(AR) $(LLIB)liblaser.a   $(LLIB)bit_SET.o 
E 13
I 13
	@echo "=====   updating bit_SET   ====="
	$(CC) -c  $(LLIB)bit_SET.c $(INC_CLAS) \
	$(INC_CODA) $(INC_ECLASER)    -o$(LLIB)bit_SET.o
	$(AR) $(LLIB)liblaser.a         $(LLIB)bit_SET.o
E 13

$(LLIB)limit.o: $(LLIB)limit.c $(INCLUDES)
D 13
	@echo "updating limit "
	$(CC) -c  $(LLIB)limit.c $(INC_CLAS) $(INC_CODA) $(INC_ECLASER)  -o$(LLIB)limit.o
	ls $(LLIB)limit.o  
	$(AR) $(LLIB)liblaser.a   $(LLIB)limit.o
E 13
I 13
	@echo "=====   updating limit   ====="
	$(CC) -c  $(LLIB)limit.c $(INC_CLAS) \
	$(INC_CODA) $(INC_ECLASER)    -o$(LLIB)limit.o
	$(AR) $(LLIB)liblaser.a         $(LLIB)limit.o
E 13

$(LLIB)limit_TST.o: $(LLIB)limit_TST.c $(INCLUDES)
D 13
	@echo "updating limit_TST "
	$(CC) -c  $(LLIB)limit_TST.c $(INC_CLAS) $(INC_CODA) $(INC_ECLASER)  -o$(LLIB)limit_TST.o
	ls $(LLIB)limit_TST.o  
	$(AR) $(LLIB)liblaser.a   $(LLIB)limit_TST.o 
E 13
I 13
	@echo "=====   updating limit_TST   ====="
	$(CC) -c  $(LLIB)limit_TST.c $(INC_CLAS) \
	$(INC_CODA) $(INC_ECLASER)    -o$(LLIB)limit_TST.o
	$(AR) $(LLIB)liblaser.a         $(LLIB)limit_TST.o
E 13

$(LLIB)bit_SET32.o: $(LLIB)bit_SET32.c $(INCLUDES)
D 13
	@echo "updating bit_SET32 "
	$(CC) -c  $(LLIB)bit_SET32.c $(INC_CLAS) $(INC_CODA) $(INC_ECLASER)  -o$(LLIB)bit_SET32.o
	ls $(LLIB)bit_SET32.o  
	$(AR) $(LLIB)liblaser.a   $(LLIB)bit_SET32.o 
E 13
I 13
	@echo "=====   updating bit_SET32   ====="
	$(CC) -c  $(LLIB)bit_SET32.c $(INC_CLAS) \
	$(INC_CODA) $(INC_ECLASER)    -o$(LLIB)bit_SET32.o
	$(AR) $(LLIB)liblaser.a         $(LLIB)bit_SET32.o
E 13

### following use CCRT and get the RT includes

$(LLIB)boswrite.o: $(LLIB)boswrite.c $(INCLUDES)
D 13
	@echo "updating boswrite"
	$(CCRT) -c  $(LLIB)boswrite.c $(INC_CLAS) $(INC_CODA) $(INC_ECLASER)  -o$(LLIB)boswrite.o
	ls $(LLIB)boswrite.o  
	$(AR) $(LLIB)liblaser.a   $(LLIB)boswrite.o
E 13
I 13
	@echo "=====   updating boswrite   ====="
	$(CCRT) -c  $(LLIB)boswrite.c $(INC_CLAS) \
	$(INC_CODA) $(INC_ECLASER)    -o$(LLIB)boswrite.o
	$(AR) $(LLIB)liblaser.a         $(LLIB)boswrite.o
E 13

$(LLIB)ss_status.o: $(LLIB)ss_status.c $(INCLUDES)
D 13
	@echo "updating ss_status"
	$(CCRT) -c  $(LLIB)ss_status.c $(INC_CLAS) $(INC_CODA) $(INC_ECLASER)  -o$(LLIB)ss_status.o
	ls $(LLIB)ss_status.o  
	$(AR) $(LLIB)liblaser.a   $(LLIB)ss_status.o
E 13
I 13
	@echo "=====   updating ss_status   ====="
	$(CCRT) -c  $(LLIB)ss_status.c $(INC_CLAS) \
	$(INC_CODA) $(INC_ECLASER)    -o$(LLIB)ss_status.o
	$(AR) $(LLIB)liblaser.a         $(LLIB)ss_status.o
E 13
  
$(LLIB)ss_init.o: $(LLIB)ss_init.c $(INCLUDES)
D 13
	@echo "updating ss_init"
	$(CCRT) -c  $(LLIB)ss_init.c $(INC_CLAS) $(INC_CODA) $(INC_ECLASER)  -o$(LLIB)ss_init.o
	ls $(LLIB)ss_init.o  
	$(AR) $(LLIB)liblaser.a   $(LLIB)ss_init.o 
E 13
I 13
	@echo "=====   updating ss_init   ====="
	$(CCRT) -c  $(LLIB)ss_init.c $(INC_CLAS) \
	$(INC_CODA) $(INC_ECLASER)    -o$(LLIB)ss_init.o
	$(AR) $(LLIB)liblaser.a         $(LLIB)ss_init.o
E 13

$(LLIB)ss_prog.o: $(LLIB)ss_prog.c $(INCLUDES)
D 13
	@echo "updating ss_prog"
	$(CCRT) -c  $(LLIB)ss_prog.c $(INC_CLAS) $(INC_CODA) $(INC_ECLASER)  -o$(LLIB)ss_prog.o
	ls $(LLIB)ss_prog.o  
	$(AR) $(LLIB)liblaser.a   $(LLIB)ss_prog.o 
E 13
I 13
	@echo "=====   updating ss_prog   ====="
	$(CCRT) -c  $(LLIB)ss_prog.c $(INC_CLAS) \
	$(INC_CODA) $(INC_ECLASER)    -o$(LLIB)ss_prog.o
	$(AR) $(LLIB)liblaser.a         $(LLIB)ss_prog.o
E 13

$(LLIB)add_status.o: $(LLIB)add_status.c $(INCLUDES)
D 13
	@echo "updating add_status"
	$(CCRT) -c  $(LLIB)add_status.c $(INC_CLAS) $(INC_CODA) $(INC_ECLASER)  -o$(LLIB)add_status.o
	ls $(LLIB)add_status.o  
	$(AR) $(LLIB)liblaser.a   $(LLIB)add_status.o 
E 13
I 13
	@echo "=====   updating add_status   ====="
	$(CCRT) -c  $(LLIB)add_status.c $(INC_CLAS) \
	$(INC_CODA) $(INC_ECLASER)    -o$(LLIB)add_status.o
	$(AR) $(LLIB)liblaser.a         $(LLIB)add_status.o
E 13


D 13
check:
	ar -t $(LLIB)liblaser.a	
	nm $(LLIB)liblaser.a
#	@echo "listing others"
#	ar -t $(CLON_LIB)/libipc.a
#	nm  $(CLON_LIB)/libipc.a
E 13
I 13
#---------------------------------------------------------------------
#---------------------------------------------------------------------
E 13

D 13
clean:
	rm -f *.o
	rm -f lib/*.o
E 13

I 13
Jpulser_test: Jpulser_test.c $(LIBRARIES) $(INCLUDES)
	$(CCRT) Jpulser_test.c $(INC_CLAS) $(LINK_CLAS) $(INC_CODA) \
	$(LINK_CODA) $(INC_ECLASER) $(LINK_ECLASER)  -o Jpulser_test
E 13

I 13
cal_motor_init: cal_motor_init.c $(LIBRARIES) $(INCLUDES)
	$(CCRT) cal_motor_init.c $(INC_CLAS) $(LINK_CLAS) $(INC_CODA) \
	$(LINK_CODA) $(INC_ECLASER) $(LINK_ECLASER)  -o cal_motor_init
E 13

D 13
update:
	cp $(EXECUTABLES) $(BIN_DIR)
	cp lpoweroff  $(BIN_DIR)/EC_laser_off
	cp lpoweron  $(BIN_DIR)/EC_laser_on
	cp status  $(BIN_DIR)/EC_laser_status
	cp laser_disable  $(BIN_DIR)/EC_laser_disable
	cp laser_enable  $(BIN_DIR)/EC_laser_enable
	cp ss_receive  $(BIN_DIR)/EC_laser_receive
	cp trigger_status  $(BIN_DIR)/EC_trigger_status
	cp camac3_status  $(BIN_DIR)/EC_laser_camac3_status
	cp get_config  $(BIN_DIR)/EC_laser_get_config
	cp EC_laser_check.csh  $(BIN_DIR)/
	cp laser_on.msg  $(BIN_DIR)/
I 4
	-chmod ug+rwx $(BIN_DIR)/EC_laser_check.csh
	-chmod a+x $(BIN_DIR)/EC_laser_check.csh
	-chmod ug+rwx $(BIN_DIR)/laser_on.msg
	-chmod a+x $(BIN_DIR)/laser_on.msg
E 13
I 13
gason: gason.c $(LIBRARIES) $(INCLUDES)
	$(CCRT) gason.c $(INC_CLAS) $(LINK_CLAS) $(INC_CODA) \
	$(LINK_CODA) $(INC_ECLASER) $(LINK_ECLASER)  -o gason
E 13
E 4

I 13
gasoff: gasoff.c $(LIBRARIES) $(INCLUDES)
	$(CCRT) gasoff.c $(INC_CLAS) $(LINK_CLAS) $(INC_CODA) \
	$(LINK_CODA) $(INC_ECLASER) $(LINK_ECLASER)  -o gasoff
E 13

D 13
loadConf:
	cp config/configuration.txt $(PARMS_DIR)
E 13
I 13
getstatus: getstatus.c $(LIBRARIES) $(INCLUDES)
	$(CCRT) getstatus.c $(INC_CLAS) $(LINK_CLAS) $(INC_CODA) \
	$(LINK_CODA) $(INC_ECLASER) $(LINK_ECLASER)  -o getstatus
E 13

D 13
distclean:
I 6
	rm -f *.o
	rm -f lib/*.o
E 6
	rm -f $(EXECUTABLES)  *.o core $(SUPPORT)
I 6
	rm -f lib/liblaser.a
E 13
I 13
laser_motor: laser_motor.c $(LIBRARIES) $(INCLUDES)
	$(CCRT) laser_motor.c $(INC_CLAS) $(LINK_CLAS) $(INC_CODA) \
	$(LINK_CODA) $(INC_ECLASER) $(LINK_ECLASER)  -o laser_motor
E 13
E 6

I 13
laser_motor_com: laser_motor_com.c $(LIBRARIES) $(INCLUDES)
	$(CCRT) laser_motor_com.c $(INC_CLAS) $(LINK_CLAS) $(INC_CODA) \
	$(LINK_CODA) $(INC_ECLASER) $(LINK_ECLASER)  -o laser_motor_com
E 13

D 13
#  sendcom laser_motor_com send_command cal_motor_init 
E 13
I 13
laser_motor_init: laser_motor_init.c $(LIBRARIES) $(INCLUDES)
	$(CCRT) laser_motor_init.c $(INC_CLAS) $(LINK_CLAS) $(INC_CODA) \
	$(LINK_CODA) $(INC_ECLASER) $(LINK_ECLASER)  -o laser_motor_init
E 13

I 13
laser_motor_status: laser_motor_status.c $(LIBRARIES) $(INCLUDES)
	$(CCRT) laser_motor_status.c $(INC_CLAS) $(LINK_CLAS) $(INC_CODA) \
	$(LINK_CODA) $(INC_ECLASER) $(LINK_ECLASER)  -o laser_motor_status
E 13

I 13
laser_motor_test: laser_motor_test.c $(LIBRARIES) $(INCLUDES)
	cc -c laser_motor_test.c $(INC_CLAS) $(INC_CODA) \
	$(INC_ECLASER) -I$(RTHOME)/include
	rtlink -cxx laser_motor_test.o $(LINK_ECLASER) $(LINK_CODA) \
	$(LINK_CLAS) -lsocket -lnsl  -o laser_motor_test
E 13

I 13
make_pulser_data: make_pulser_data.c $(LIBRARIES) $(INCLUDES)
	$(CCRT) make_pulser_data.c $(INC_CLAS) $(LINK_CLAS) $(INC_CODA) \
	$(LINK_CODA) $(INC_ECLASER) $(LINK_ECLASER)  -o make_pulser_data
E 13

I 13
send_command: send_command.c $(LIBRARIES) $(INCLUDES)
	cc -c send_command.c $(INC_CLAS) $(INC_CODA) \
	$(INC_ECLASER)  -I$(RTHOME)/include
	rtlink -cxx send_command.o  $(LINK_ECLASER) $(LINK_CODA) \
	$(LINK_CLAS) -lsocket -lnsl  -o send_command
E 13

I 13
set_filter: set_filter.c $(LIBRARIES) $(INCLUDES)
	$(CCRT) set_filter.c $(INC_CLAS) $(LINK_CLAS) $(INC_CODA) \
	$(LINK_CODA) $(INC_ECLASER) $(LINK_ECLASER)  -o set_filter
E 13

I 13
ss_send: ss_send.c $(LIBRARIES) $(INCLUDES)
	$(CCRT) ss_send.c $(INC_CLAS) $(INC_CODA) $(INC_ECLASER) \
	$(LINK_ECLASER) $(LINK_CODA) $(LINK_CLAS)  -o ss_send
E 13

D 13
cal_motor_init:cal_motor_init.c $(LIBRARIES) $(INCLUDES)
D 9
	$(CCRT) cal_motor_init.c $(INC_CLAS) $(LINK_CLAS) $(INC_CODA) $(LINK_CODA) $(INC_ECLASER) $(LINK_ECLASER) -lC  -o cal_motor_init
E 9
I 9
	$(CCRT) cal_motor_init.c $(INC_CLAS) $(LINK_CLAS) $(INC_CODA) $(LINK_CODA) $(INC_ECLASER) $(LINK_ECLASER)    -o cal_motor_init
E 13
I 13
stepperon: stepperon.c $(LIBRARIES) $(INCLUDES)
	$(CCRT) stepperon.c $(INC_CLAS) $(LINK_CLAS) $(INC_CODA) \
	$(LINK_CODA) $(INC_ECLASER) $(LINK_ECLASER)  -o stepperon
E 13
E 9

D 13
test_dp:test_dp.c $(LIBRARIES) $(INCLUDES)
D 9
	$(CCRT) test_dp.c $(INC_CLAS) $(LINK_CLAS) $(INC_CODA) $(LINK_CODA) $(INC_ECLASER) $(LINK_ECLASER) -lC  -o test_dp
E 9
I 9
	$(CCRT) test_dp.c $(INC_CLAS) $(LINK_CLAS) $(INC_CODA) $(LINK_CODA) $(INC_ECLASER) $(LINK_ECLASER)    -o test_dp
E 13
I 13
stepperoff: stepperoff.c $(LIBRARIES) $(INCLUDES)
	$(CCRT) stepperoff.c $(INC_CLAS) $(LINK_CLAS) $(INC_CODA) \
	$(LINK_CODA) $(INC_ECLASER) $(LINK_ECLASER)  -o stepperoff
E 13
E 9

D 13
child_spawn:child_spawn.c $(LIBRARIES) $(INCLUDES)
D 9
	$(CCRT) child_spawn.c $(INC_CLAS) $(LINK_CLAS) $(INC_CODA) $(LINK_CODA) $(INC_ECLASER) $(LINK_ECLASER) -lC  -o child_spawn
E 9
I 9
	$(CCRT) child_spawn.c $(INC_CLAS) $(LINK_CLAS) $(INC_CODA) $(LINK_CODA) $(INC_ECLASER) $(LINK_ECLASER)    -o child_spawn
E 13
E 9

D 13
child_exe:child_exe.c $(LIBRARIES) $(INCLUDES)
D 9
	$(CCRT) child_exe.c $(INC_CLAS) $(LINK_CLAS) $(INC_CODA) $(LINK_CODA) $(INC_ECLASER) $(LINK_ECLASER) -lC  -o child_exe
E 9
I 9
	$(CCRT) child_exe.c $(INC_CLAS) $(LINK_CLAS) $(INC_CODA) $(LINK_CODA) $(INC_ECLASER) $(LINK_ECLASER)    -o child_exe
E 13
I 13
#---------------------------------------------------------------------
#---------------------------------------------------------------------
#---------------------------------------------------------------------
E 13
E 9

D 13
child_exe2:child_exe2.c $(LIBRARIES) $(INCLUDES)
D 9
	$(CCRT) child_exe2.c $(INC_CLAS) $(LINK_CLAS) $(INC_CODA) $(LINK_CODA) $(INC_ECLASER) $(LINK_ECLASER) -lC  -o child_exe2
E 9
I 9
	$(CCRT) child_exe2.c $(INC_CLAS) $(LINK_CLAS) $(INC_CODA) $(LINK_CODA) $(INC_ECLASER) $(LINK_ECLASER)    -o child_exe2
E 13
E 9

D 13
child_exe3:child_exe3.c $(LIBRARIES) $(INCLUDES)
D 9
	$(CCRT) child_exe3.c $(INC_CLAS) $(LINK_CLAS) $(INC_CODA) $(LINK_CODA) $(INC_ECLASER) $(LINK_ECLASER) -lC  -o child_exe3
E 9
I 9
	$(CCRT) child_exe3.c $(INC_CLAS) $(LINK_CLAS) $(INC_CODA) $(LINK_CODA) $(INC_ECLASER) $(LINK_ECLASER)    -o child_exe3
E 13
I 13
check:
	ar -t	$(LLIB)liblaser.a	
	nm	$(LLIB)liblaser.a
#	@echo "listing others"
#	ar -t	$(CLON_LIB)/libipc.a
#	nm	$(CLON_LIB)/libipc.a
E 13
E 9

D 13
child_test:child_test.c $(LIBRARIES) $(INCLUDES)
D 9
	$(CCRT) child_test.c $(INC_CLAS) $(LINK_CLAS) $(INC_CODA) $(LINK_CODA) $(INC_ECLASER) $(LINK_ECLASER) -lC  -o child_test
E 9
I 9
	$(CCRT) child_test.c $(INC_CLAS) $(LINK_CLAS) $(INC_CODA) $(LINK_CODA) $(INC_ECLASER) $(LINK_ECLASER)    -o child_test
E 13
E 9

D 13
go_pulse:go_pulse.c $(LIBRARIES) $(INCLUDES)
D 9
	$(CCRT) go_pulse.c $(INC_CLAS) $(LINK_CLAS) $(INC_CODA) $(LINK_CODA) $(INC_ECLASER) $(LINK_ECLASER) -lC  -o go_pulse
E 9
I 9
	$(CCRT) go_pulse.c $(INC_CLAS) $(LINK_CLAS) $(INC_CODA) $(LINK_CODA) $(INC_ECLASER) $(LINK_ECLASER)    -o go_pulse
E 9
make_pulser_data:make_pulser_data.c $(LIBRARIES) $(INCLUDES)
D 9
	$(CCRT) make_pulser_data.c $(INC_CLAS) $(LINK_CLAS) $(INC_CODA) $(LINK_CODA) $(INC_ECLASER) $(LINK_ECLASER) -lC  -o make_pulser_data
E 9
I 9
	$(CCRT) make_pulser_data.c $(INC_CLAS) $(LINK_CLAS) $(INC_CODA) $(LINK_CODA) $(INC_ECLASER) $(LINK_ECLASER)    -o make_pulser_data
E 9
test_pulser_data:test_pulser_data.c $(LIBRARIES) $(INCLUDES)
D 9
	$(CCRT) test_pulser_data.c $(INC_CLAS) $(LINK_CLAS) $(INC_CODA) $(LINK_CODA) $(INC_ECLASER) $(LINK_ECLASER) -lC  -o test_pulser_data
E 9
I 9
	$(CCRT) test_pulser_data.c $(INC_CLAS) $(LINK_CLAS) $(INC_CODA) $(LINK_CODA) $(INC_ECLASER) $(LINK_ECLASER)    -o test_pulser_data
E 9
Jpulser_test:Jpulser_test.c $(LIBRARIES) $(INCLUDES)
D 9
	$(CCRT) Jpulser_test.c $(INC_CLAS) $(LINK_CLAS) $(INC_CODA) $(LINK_CODA) $(INC_ECLASER) $(LINK_ECLASER) -lC  -o Jpulser_test
E 9
I 9
	$(CCRT) Jpulser_test.c $(INC_CLAS) $(LINK_CLAS) $(INC_CODA) $(LINK_CODA) $(INC_ECLASER) $(LINK_ECLASER)    -o Jpulser_test
E 13
I 13
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
E 13
E 9


D 13
init_laser:init_laser.c $(LIBRARIES) $(INCLUDES)
D 9
	$(CCRT) init_laser.c $(INC_CLAS) $(LINK_CLAS) $(INC_CODA) $(LINK_CODA) $(INC_ECLASER) $(LINK_ECLASER) -lC  -o init_laser
E 9
I 9
	$(CCRT) init_laser.c $(INC_CLAS) $(LINK_CLAS) $(INC_CODA) $(LINK_CODA) $(INC_ECLASER) $(LINK_ECLASER)    -o init_laser
E 13
I 13
loadConf:
	cp config/configuration.txt $(CLON_PARMS)/EC_config
E 13
E 9

D 13
set_filter:set_filter.c $(LIBRARIES) $(INCLUDES)
D 9
	$(CCRT) set_filter.c $(INC_CLAS) $(LINK_CLAS) $(INC_CODA) $(LINK_CODA) $(INC_ECLASER) $(LINK_ECLASER) -lC  -o set_filter
E 9
I 9
	$(CCRT) set_filter.c $(INC_CLAS) $(LINK_CLAS) $(INC_CODA) $(LINK_CODA) $(INC_ECLASER) $(LINK_ECLASER)    -o set_filter
E 13
E 9

D 13
test_filter:test_filter.c $(LIBRARIES) $(INCLUDES)
D 9
	$(CCRT) test_filter.c $(INC_CLAS) $(LINK_CLAS) $(INC_CODA) $(LINK_CODA) $(INC_ECLASER) $(LINK_ECLASER) -lC  -o test_filter
E 9
I 9
	$(CCRT) test_filter.c $(INC_CLAS) $(LINK_CLAS) $(INC_CODA) $(LINK_CODA) $(INC_ECLASER) $(LINK_ECLASER)    -o test_filter
E 13
I 13
clean:
	rm -f *.o
	rm -f lib/*.o
E 13
E 9

D 13
laser_motor_status:laser_motor_status.c $(LIBRARIES) $(INCLUDES)
D 9
	$(CCRT) laser_motor_status.c $(INC_CLAS) $(LINK_CLAS) $(INC_CODA) $(LINK_CODA) $(INC_ECLASER) $(LINK_ECLASER) -lC  -o laser_motor_status
E 9
I 9
	$(CCRT) laser_motor_status.c $(INC_CLAS) $(LINK_CLAS) $(INC_CODA) $(LINK_CODA) $(INC_ECLASER) $(LINK_ECLASER)    -o laser_motor_status
E 13
E 9

I 10
D 13
laser_motor_test:laser_motor_test.c $(LIBRARIES) $(INCLUDES)
	cc -c laser_motor_test.c $(INC_CLAS)  $(INC_CODA) $(INC_ECLASER)  -I$(RTHOME)/include
D 11
	rtlink -cxx unix_master.o  $(LINK_ECLASER) $(LINK_CODA) $(LINK_CLAS) -lsocket -lnsl  -o unix_master
E 11
I 11
	rtlink -cxx laser_motor_test.o  $(LINK_ECLASER) $(LINK_CODA) $(LINK_CLAS) -lsocket -lnsl  -o laser_motor_test
E 13
I 13
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
E 13
E 11
E 10

I 10

D 13

E 10
unix_master_redux3:unix_master_redux3.c $(LIBRARIES) $(INCLUDES)
D 9
	$(CCRT) unix_master_redux3.c $(INC_CLAS) $(LINK_CLAS) $(INC_CODA) $(LINK_CODA) $(INC_ECLASER) $(LINK_ECLASER) -lC  -o unix_master_redux3
E 9
I 9
	$(CCRT) unix_master_redux3.c $(INC_CLAS) $(LINK_CLAS) $(INC_CODA) $(LINK_CODA) $(INC_ECLASER) $(LINK_ECLASER)    -o unix_master_redux3
E 9

TEST:TEST.c $(LIBRARIES) $(INCLUDES)
D 9
	$(CCRT) TEST.c $(INC_CLAS) $(LINK_CLAS) $(INC_CODA) $(LINK_CODA) $(INC_ECLASER) $(LINK_ECLASER) -lC  -o TEST
E 9
I 9
	$(CCRT) TEST.c $(INC_CLAS) $(LINK_CLAS) $(INC_CODA) $(LINK_CODA) $(INC_ECLASER) $(LINK_ECLASER)    -o TEST
E 9

set_bit_11:set_bit_11.c $(LIBRARIES) $(INCLUDES)
D 9
	$(CCRT) set_bit_11.c $(INC_CLAS) $(LINK_CLAS) $(INC_CODA) $(LINK_CODA) $(INC_ECLASER) $(LINK_ECLASER) -lC  -o set_bit_11
E 9
I 9
	$(CCRT) set_bit_11.c $(INC_CLAS) $(LINK_CLAS) $(INC_CODA) $(LINK_CODA) $(INC_ECLASER) $(LINK_ECLASER)    -o set_bit_11
E 9


trigger_status:trigger_status.c $(LIBRARIES) $(INCLUDES)
D 5
	$(CCRT) trigger_status.c $(INC_CLAS) $(LINK_CLAS) $(INC_CODA) $(LINK_CODA) $(INC_ECLASER) $(LINK_ECLASER) -lC  -o trigger_status
E 5
I 5
	cc -c trigger_status.c $(INC_CLAS) $(LINK_CLAS) $(INC_CODA) $(LINK_CODA) $(INC_ECLASER) $(LINK_ECLASER)
D 9
	rtlink -cxx trigger_status.o $(INC_CLAS) $(LINK_CLAS) $(INC_CODA) $(LINK_CODA) $(INC_ECLASER) $(LINK_ECLASER) -lC  -o trigger_status
E 9
I 9
	rtlink -cxx trigger_status.o $(INC_CLAS) $(LINK_CLAS) $(INC_CODA) $(LINK_CODA) $(INC_ECLASER) $(LINK_ECLASER)    -o trigger_status
E 9
E 5

#222222
#get_config:get_config.c $(LIBRARIES) $(INCLUDES)
D 9
#	$(CCRT) get_config.c $(INC_CLAS) $(LINK_CLAS) $(INC_CODA) $(LINK_CODA) $(INC_ECLASER) $(LINK_ECLASER) -lC  -o get_config
E 9
I 9
#	$(CCRT) get_config.c $(INC_CLAS) $(LINK_CLAS) $(INC_CODA) $(LINK_CODA) $(INC_ECLASER) $(LINK_ECLASER)    -o get_config
E 9

camac3_status:camac3_status.c $(LIBRARIES) $(INCLUDES)
D 5
	$(CCRT) camac3_status.c $(INC_CLAS) $(LINK_CLAS) $(INC_CODA) $(LINK_CODA) $(INC_ECLASER) $(LINK_ECLASER) -lC  -o camac3_status
E 5
I 5
	cc -c camac3_status.c $(INC_CLAS) $(LINK_CLAS) $(INC_CODA) $(LINK_CODA) $(INC_ECLASER) $(LINK_ECLASER)
D 9
	rtlink -cxx camac3_status.o $(INC_CLAS) $(LINK_CLAS) $(INC_CODA) $(LINK_CODA) $(INC_ECLASER) $(LINK_ECLASER) -lC  -o camac3_status
E 9
I 9
	rtlink -cxx camac3_status.o $(INC_CLAS) $(LINK_CLAS) $(INC_CODA) $(LINK_CODA) $(INC_ECLASER) $(LINK_ECLASER)    -o camac3_status
E 9
E 5

trigger:trigger.c $(LIBRARIES) $(INCLUDES)
D 9
	$(CCRT) trigger.c $(INC_CLAS) $(LINK_CLAS) $(INC_CODA) $(LINK_CODA) $(INC_ECLASER) $(LINK_ECLASER) -lC  -o trigger
E 9
I 9
	$(CCRT) trigger.c $(INC_CLAS) $(LINK_CLAS) $(INC_CODA) $(LINK_CODA) $(INC_ECLASER) $(LINK_ECLASER)    -o trigger
E 9

zzzzz:zzzzz.c $(LIBRARIES) $(INCLUDES)
D 9
	$(CCRT) zzzzz.c $(INC_CLAS) $(LINK_CLAS) $(INC_CODA) $(LINK_CODA) $(INC_ECLASER) $(LINK_ECLASER) -lC  -o zzzzz
E 9
I 9
	$(CCRT) zzzzz.c $(INC_CLAS) $(LINK_CLAS) $(INC_CODA) $(LINK_CODA) $(INC_ECLASER) $(LINK_ECLASER)    -o zzzzz
E 9

test_bit_shift:test_bit_shift.c $(LIBRARIES) $(INCLUDES)
D 9
	$(CCRT) test_bit_shift.c $(INC_CLAS) $(LINK_CLAS) $(INC_CODA) $(LINK_CODA) $(INC_ECLASER) $(LINK_ECLASER) -lC  -o test_bit_shift
E 9
I 9
	$(CCRT) test_bit_shift.c $(INC_CLAS) $(LINK_CLAS) $(INC_CODA) $(LINK_CODA) $(INC_ECLASER) $(LINK_ECLASER)    -o test_bit_shift
E 9

trig_bits:trig_bits.c $(LIBRARIES) $(INCLUDES)
D 9
	$(CCRT) trig_bits.c $(INC_CLAS) $(LINK_CLAS) $(INC_CODA) $(LINK_CODA) $(INC_ECLASER) $(LINK_ECLASER) -lC  -o trig_bits
E 9
I 9
	$(CCRT) trig_bits.c $(INC_CLAS) $(LINK_CLAS) $(INC_CODA) $(LINK_CODA) $(INC_ECLASER) $(LINK_ECLASER)    -o trig_bits
E 9

reboot:
	chmod +x remote_reset_camac3.tcl
	chmod +x camac3_reboot


testit:
	rtlink -g ss_send1.c -I/usr/local/clas/include -I./include -L/usr/loca/clas/lib -lipc -L./lib -llaser 



E 13
E 1
