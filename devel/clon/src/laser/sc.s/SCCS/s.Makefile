h64840
s 00003/00003/00050
d D 1.3 04/08/12 12:17:38 sergpozd 4 3
c *** empty log message ***
e
s 00027/00017/00026
d D 1.2 03/07/21 14:11:20 sergpozd 3 1
c Completely new revision
e
s 00000/00000/00000
d R 1.2 03/03/03 14:04:15 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 laser/sc/s/Makefile
e
s 00043/00000/00000
d D 1.1 03/03/03 14:04:14 boiarino 1 0
c date and time created 03/03/03 14:04:14 by boiarino
e
u
U
f e 0
t
T
I 1
#
# makefile for laser/sc/s directory
#
SHELL = /bin/sh

INC = .
CC  = cc
D 3
CFLAGS = -fast -KPIC -mt -DSunOS -I$(INC)
LD = $(CC) -fast 
	
E 3
I 3
CFLAGS = -fast -KPIC -mt -xarch=v8plusa -DSunOS -I$(INC)
LD = $(CC) -fast

E 3
LIB = -L$(CODA)/SunOS/lib -ldpS -lsocket -lnsl -lresolv
D 3
	
E 3
I 3

E 3
.c.o:
	$(CC) -c $(CFLAGS) $<

D 3
EXE = TOF_laser_on TOF_laser_off TOF_laser TOF_laser_status
OBJ = ss_prog.o enable_laser.o disable_laser.o evaluate_ready.o boswrite.c \
		add_status.o init_checks.o ss_status.c system_setup.o system_check.o \
		get_motor_pos.o bits.o
E 3
I 3
EXE = TOF_laser TOF_laser_on TOF_laser_off TOF_laser_status
OBJ = TOF_laser_library.o bits.o init_checks.o \
	system_setup.o system_check.o system_shutdown.o
E 3

I 3

E 3
all: $(EXE)

D 3
TOF_laser_status: TOF_laser_status.o bits.o
	$(CC) -o TOF_laser_status bits.o $< $(LIB)
E 3
I 3
TOF_laser: TOF_laser.o $(OBJ)
	$(CC) -o TOF_laser $< $(OBJ) $(LIB)
E 3

D 3
TOF_laser_on: TOF_laser_on.o
	$(CC) -o TOF_laser_on $< $(LIB)
E 3
I 3
TOF_laser_on: TOF_laser_on.o TOF_laser_library.o
	$(CC) -o TOF_laser_on TOF_laser_library.o $< $(LIB)
E 3

D 3
TOF_laser_off: TOF_laser_off.o
	$(CC) -o TOF_laser_off $< $(LIB)
E 3
I 3
TOF_laser_off: TOF_laser_off.o TOF_laser_library.o
	$(CC) -o TOF_laser_off TOF_laser_library.o $< $(LIB)
E 3

D 3
TOF_laser: TOF_laser.o $(OBJ)
	$(CC) -o TOF_laser $< $(OBJ) $(LIB)
E 3
I 3
TOF_laser_status: TOF_laser_status.o bits.o
	$(CC) -o TOF_laser_status TOF_laser_library.o bits.o $< $(LIB)
E 3

clean:
	rm -f *.o

distclean:
	rm -f *.o $(EXE)

install: exports
exports:
D 3
	cp $(EXE) $(CLON_BIN)
E 3
I 3
	rm -f     $(CLON_BIN)/TOF_laser
	rm -f     $(CLON_BIN)/TOF_laser_on
	rm -f     $(CLON_BIN)/TOF_laser_off
	rm -f     $(CLON_BIN)/TOF_laser_status
	rm -f     $(CLON_PARMS)/TOF_config/configuration.txt
	cp $(EXE)            $(CLON_BIN)
	cp configuration.txt $(CLON_PARMS)/TOF_config
D 4
	-chmod ug+rw  $(CLON_BIN)/TOF_*
	-chmod a+x    $(CLON_BIN)/TOF_*
	-chmod ug+rw  $(CLON_PARMS)/TOF_config/configuration.txt
E 4
I 4
	-chmod -f ug+rw  $(CLON_BIN)/TOF_*
	-chmod -f a+x    $(CLON_BIN)/TOF_*
	-chmod -f ug+rw  $(CLON_PARMS)/TOF_config/configuration.txt
E 4

E 3
E 1
