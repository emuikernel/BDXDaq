#
# Copyright (c) 1991 Regents of the University of California.
# All rights reserved.
#
# Redistribution and use in source and binary forms are permitted
# provided that the above copyright notice and this paragraph are
# duplicated in all such forms and that any documentation,
# advertising materials, and other materials related to such
# distribution and use acknowledge that the software was developed
# by the University of California, Lawrence Berkeley Laboratory,
# Berkeley, CA.  The name of the University may not be used to
# endorse or promote products derived from this software without
# specific prior written permission.
# THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
# IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
# WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
#
CCOPT = -O
CFLAGS = $(CCOPT) $(INCLUDE)
#LIBS= -lsyslog
#CC = gcc -g -fstrength-reduce -fcombine-regs -fforce-addr -fforce-mem
CC= cc 

slattach: slattach.c
	$(CC) $(CFLAGS) -o slattach slattach.c ${LIBS}

install: slattach
	install slattach /usr/etc

clean:
	rm -f *.o core slattach
