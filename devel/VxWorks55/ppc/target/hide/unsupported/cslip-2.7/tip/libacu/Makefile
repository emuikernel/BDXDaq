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
#	@(#)Makefile	5.4 (Berkeley) 9/2/88
#
# make file for tip device drivers
#
# Current drivers:
#	BIZCOMP
#	USR COURIER (2400 baud)
#	DEC DF02-AC, DF03-AC
#	DEC DN-11/Able Quadracall
#	HAYES and Hayes emulators
#	VENTEL 212+ (w/o echo)
#	VADIC 831 RS232 adaptor
#	VADIC 3451
#	Hayes Smartmodem 2400
#	Telebit Trailblazer

CFLAGS=	-O -I..
CC= gcc -g

# Explicitly define compiliation rule since SunOS 4.X's make doesn't like gcc.
.c.o:
	rm -f $@; $(CC) $(CFLAGS) -c $*.c

ALLOBJS= biz22.o biz31.o courier.o df.o dn11.o hayes.o ventel.o v831.o \
	v3451.o sm2400.o telebit.o
OBJS= telebit.o hayes.o v3451.o

# Explicitly define compiliation rule since SunOS 4.X's make doesn't like gcc.
.c.o:
	rm -f $@; $(CC) $(CFLAGS) -c $*.c

libacu.a: ${OBJS}
	ar cu libacu.a *.o
	ranlib libacu.a

${OBJS}: ../tip.h

clean:
	rm -f libacu.a *.o core errs a.out
