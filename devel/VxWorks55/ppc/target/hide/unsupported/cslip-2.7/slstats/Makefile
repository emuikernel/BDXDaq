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

PROG=	slstats
# Can't use because inet_ntoa() chews it on the sparc
#CC= gcc -g -fpcc-struct-return
INCL= 
OPT= -g
CFLAGS=	${OPT} ${INCL}
LIBC=	/lib/libc.a
SRCS=	${PROG}.c
OBJS=
LIBS= -lkvm

all: ${PROG}

${PROG}: ${PROG}.c ${LIBC}
	${CC} -o $@ ${CFLAGS} $@.c ${LIBS}

clean:
	rm -f ${PROG} ${OBJS} core

cleandir: clean
	rm -f tags .depend

depend: ${SRCS}
	mkdep -p ${CFLAGS} ${SRCS}

install: ${PROG}
	install -g kmem -m 2755 ${PROG} ${DESTDIR}/usr/etc

install-man:
	install -c -m 444 ${PROG}.8 ${DESTDIR}/usr/man/man8

lint: ${SRCS}
	lint ${SRCS}

tags: ${SRCS}
	ctags -wt ${SRCS}
