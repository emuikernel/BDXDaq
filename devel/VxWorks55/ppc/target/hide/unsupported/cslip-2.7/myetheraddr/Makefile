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

#CC= gcc -g -fpcc-struct-return
CC= cc -g
INCL= 
#OPT= -O
CFLAGS=	${OPT} ${INCL}
SRCS=	myetheraddr.c
OBJS=
LIBS= -lkvm
MAN=	myetheraddr.0

all: myetheraddr

myetheraddr: myetheraddr.c
	${CC} -o $@ ${CFLAGS} $@.c ${LIBS}

clean:
	rm -f ${OBJS} core myetheraddr

cleandir: clean
	rm -f ${MAN} tags .depend

depend: ${SRCS}
	mkdep -p ${CFLAGS} ${SRCS}

install: myetheraddr
	install -g kmem -m 2755 myetheraddr ${DESTDIR}/usr/etc

install-man:
	install -c -m 444 myetheraddr.8 ${DESTDIR}/usr/man/man8

lint: ${SRCS}
	lint ${SRCS}

tags: ${SRCS}
	ctags ${SRCS}
