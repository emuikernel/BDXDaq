#
# makefile for laser/sc/s directory
#
SHELL = /bin/sh

CC  = cc
CFLAGS = -xO5 -KPIC -mt -xarch=v8plusa -DSunOS -I. -I$(CODA)/common/include
LD = $(CC) -xO5

LIB = -L$(CODA)/SunOS_sun4u/lib -lmysql4.1.20 -lsocket -lnsl -lresolv

.c.o:
	$(CC) -c $(CFLAGS) $<

EXE = TOF_laser TOF_laser_on TOF_laser_off TOF_laser_status
OBJ = TOF_laser_library.o bits.o init_checks.o \
	system_setup.o system_check.o system_shutdown.o


all: $(EXE)

TOF_laser: TOF_laser.o $(OBJ)
	$(CC) -o TOF_laser $< $(OBJ) $(LIB)

TOF_laser_on: TOF_laser_on.o TOF_laser_library.o
	$(CC) -o TOF_laser_on TOF_laser_library.o $< $(LIB)

TOF_laser_off: TOF_laser_off.o TOF_laser_library.o
	$(CC) -o TOF_laser_off TOF_laser_library.o $< $(LIB)

TOF_laser_status: TOF_laser_status.o bits.o
	$(CC) -o TOF_laser_status TOF_laser_library.o bits.o $< $(LIB)

clean:
	rm -f *.o

distclean:
	rm -f *.o $(EXE)

install: exports
exports:
	rm -f     $(CLON_BIN)/TOF_laser
	rm -f     $(CLON_BIN)/TOF_laser_on
	rm -f     $(CLON_BIN)/TOF_laser_off
	rm -f     $(CLON_BIN)/TOF_laser_status
	rm -f     $(CLON_PARMS)/TOF_config/configuration.txt
	cp $(EXE)            $(CLON_BIN)
	cp configuration.txt $(CLON_PARMS)/TOF_config
	-chmod -f ug+rw  $(CLON_BIN)/TOF_*
	-chmod -f a+x    $(CLON_BIN)/TOF_*
	-chmod -f ug+rw  $(CLON_PARMS)/TOF_config/configuration.txt

