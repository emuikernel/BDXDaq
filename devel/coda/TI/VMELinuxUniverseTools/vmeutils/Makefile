target: vmeutils

INCLUDE = -I.
CC      = gcc
CFLAGS  = -O2 -Wall ${INCLUDE}
LIBS    = -lstdc++

.SUFFIXES:
.SUFFIXES:	.cpp .c .o .dep

.cpp.o:
	${CC} ${CFLAGS} -c -o ${<:.cpp=.o} $<

vmeutils: vmeutils.o commands.o unilib.o Makefile
	${CC} ${CFLAGS} -o vmeutils vmeutils.o commands.o unilib.o ${LIBS}

clean:
	rm -f *.o
	rm -f vmeutils