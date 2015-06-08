# @(#) $Header: Makefile,v 1.4 91/03/31 15:35:21 leres Exp $ (LBL)

INCL= -I.
OPT= -g
CFLAGS=	${OPT} ${INCL}
SRCS= kvm.c
HDRS= kvm.h
OBJS= kvm.o
LIBS= 
MAN=	

libkvm.a: ${OBJS}
	rm -f libkvm.a
	ar cr libkvm.a ${OBJS}

kvm.c: kvm.h
	${CC} -c ${CFLAGS} kvm.c

clean:
	rm -f ${OBJS} libkvm.a core

depend: ${SRCS}
	mkdep -p ${CFLAGS} ${SRCS}

install: libkvm.a
	install libkvm.a ${DESTDIR}/usr/lib
	ranlib ${DESTDIR}/usr/lib/libkvm.a

install-man: kvm.3
	install kvm.3 ${DESTDIR}/usr/man/man3

install-include:
	install kvm.h ${DESTDIR}/usr/include

lint: ${SRCS}
	lint ${SRCS}

tags: ${SRCS}
	ctags -t ${SRCS} ${HDRS}
