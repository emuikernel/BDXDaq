#
# Copyright (c) 1988 Regents of the University of California.
# All rights reserved.
#
# Redistribution and use in source and binary forms are permitted
# provided that the above copyright notice and this paragraph are
# duplicated in all such forms and that any documentation, advertising
# materials, and other materials related to such redistribution and
# use acknowledge that the software was developed by the University
# of California, Berkeley.  The name of the University may not be
# used to endorse or promote products derived from this software
# without specific prior written permission.  THIS SOFTWARE IS PROVIDED
# ``AS IS'' AND WITHOUT ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING,
# WITHOUT LIMITATION, THE IMPLIED WARRANTIES OF MERCHANTIBILITY AND
# FITNESS FOR A PARTICULAR PURPOSE.
#
# %W% (Berkeley) %G%
#

CC= cc
INCL= 
OPT= -g
CFLAGS=	${OPT} ${INCL} -DDEBUG
LIBC=	/lib/libc.a
SRCS=	sliplogin.c strerror.c
OBJS=	sliplogin.o strerror.o
LIBS=
MAN=	sliplogin.0

all: sliplogin

sliplogin: ${OBJS} ${LIBC}
	${CC} -o $@ ${CFLAGS} ${OBJS} ${LIBS}

clean:
	rm -f ${OBJS} core sliplogin

cleandir: clean
	rm -f ${MAN} tags .depend

depend: ${SRCS}
	mkdep -p ${CFLAGS} ${SRCS}

install: sliplogin
	install -o root -g slip -m 4750 sliplogin ${DESTDIR}/etc

install-conf:
	install -g slip -m 550 slip.login ${DESTDIR}/etc
	install -g slip -m 550 slip.logout ${DESTDIR}/etc
	install -g slip -m 440 slip.hosts ${DESTDIR}/etc

install-man:
	install -c -m 444 sliplogin.8 ${DESTDIR}/usr/man/man8

lint: ${SRCS}
	lint ${SRCS}

tags: ${SRCS}
	ctags ${SRCS}
