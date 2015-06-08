#
# Copyright (c) 1988 The Regents of the University of California.
# All rights reserved.
#
# Redistribution and use in source and binary forms are permitted
# provided that the above copyright notice and this paragraph are
# duplicated in all such forms and that any documentation,
# advertising materials, and other materials related to such
# distribution and use acknowledge that the software was developed
# by the University of California, Berkeley.  The name of the
# University may not be used to endorse or promote products derived
# from this software without specific prior written permission.
# THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
# IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
# WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
#
#	@(#)Makefile	5.6 (Berkeley) 9/28/88
#
# make file for intermachine communications package
#
# Files are:
#	/etc/remote		remote host description file
#	/etc/phones		phone number file, owned by ${OWNER} and
#				  mode 6??
#	${ADM}/aculog		ACU accounting file, owned by ${OWNER} and
#				  mode 6?? {if ACULOG defined}
# Presently supports:
#	BIZCOMP
#	DEC DF02-AC, DF03-AC
#	DEC DN-11/Able Quadracall
#	HAYES and Hayes emulators
#	USR COURIER (2400 baud)
#	VENTEL 212+
#	VADIC 831 RS232 adaptor
#	VADIC 3451
#	Hayes Smartmodem 2400
#	Telebit Trailblazer
# (drivers are located in libacu.a)
#
# Configuration defines:
#	DF02, DF03, DN11	ACU's supported
#	  BIZ1031, BIZ1022, VENTEL, V831, V3451, HAYES, COURIER
#	  SM2400, TELEBIT
#	ACULOG			turn on tip logging of ACU use
#	PRISTINE		no phone #'s put in ACU log file
#	CONNECT			worthless command
#	DEFBR			default baud rate to make connection at
#	DEFFS			default frame size for FTP buffering of
#				writes on local side
#	BUFSIZ			buffer sizing from stdio, must be fed
#				explicitly to remcap.c if not 1024
#	STREAMS			system has streams tty interface (SunOS 4.0)
ADM=	usr/adm
OWNER=	uucp
GROUP=	daemon
CONFIG=	-DV3451 -DHAYES -DTELEBIT
CFLAGS=	-O -DDEFBR=1200 -DDEFFS=BUFSIZ -DPRISTINE
LDFLAGS=
OBJS=	acu.o cmds.o cmdtab.o cu.o hunt.o \
	log.o partab.o remote.o tip.o tipout.o value.o vars.o \
	login.o slip.o page.o strerror.o
SPECIAL=acutab.c remcap.c uucplock.c
DRIVERS=libacu/*.c
SOURCES=acu.c acutab.c cmds.c cmdtab.c cu.c hunt.c \
	log.c partab.c remote.c tip.c tipout.c value.c vars.c \
	login.c slip.c page.c strerror.c \
	${SPECIAL} ${DRIVERS}
CXREF=	/usr/ucb/ctags -xw
CC= gcc -g
MAKE= make

# Explicitly define compiliation rule since SunOS 4.X's make doesn't like gcc.
.c.o:
	rm -f $@; $(CC) $(CFLAGS) -c $*.c

tip:	${OBJS} acutab.o remcap.o uucplock.o libacu/libacu.a
	${CC} -o tip ${LDFLAGS} ${OBJS} acutab.o remcap.o uucplock.o \
		libacu/libacu.a

${OBJS}: tip.h

remcap.o: remcap.c
	${CC} -c -DBUFSIZ=1024 ${CFLAGS} remcap.c

# acutab is configuration dependent, and so depends on the makefile
acutab.o: Makefile
acutab.o: acutab.c
	${CC} -c ${CFLAGS} ${CONFIG} acutab.c

# remote.o depends on the makefile because of DEFBR and DEFFS
remote.o: Makefile

# log.o depends on the makefile because of ACULOG
log.o:	Makefile

libacu/libacu.a: ${DRIVERS}
	cd libacu; ${MAKE} ${MFLAGS}

install: tip
# Can't do this until we fix some security problems
#	install -m 4711 -o ${OWNER} -g ${GROUP} tip ${DESTDIR}/usr/bin/tip
	install -m 755 tip ${DESTDIR}/usr/bin/tip
	rm -f ${DESTDIR}/usr/bin/cu
	ln ${DESTDIR}/usr/bin/tip ${DESTDIR}/usr/bin/cu
#	cp /dev/null ${DESTDIR}/${ADM}/aculog
#	chown ${OWNER} ${DESTDIR}/${ADM}/aculog
#	chmod 600 ${DESTDIR}/${ADM}/aculog
#	@echo "create /etc/remote and /etc/phones"

install-conf:
	install -g slip -m 440 login.script.unix ${DESTDIR}/etc
	install -g slip -m 440 login.script.netblazer ${DESTDIR}/etc

tags:	${SOURCES} tip.h
	ctags -w ${SOURCES} tip.h

clean:
	cd libacu; ${MAKE} ${MFLAGS} clean
	rm -f tip cu *.o

print:
	@pr README TODO makefile
	@size *.o | pr -h SIZES
	@${CXREF} tip.h ${SOURCES} | pr -h XREF
	@ls -l | pr 
	@pr tip.h ${SOURCES}

lint:
	-lint -hbacvx ${CFLAGS} ${SOURCES}
