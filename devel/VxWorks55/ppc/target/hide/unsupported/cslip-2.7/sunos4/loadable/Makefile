# @(#) $Header: Makefile,v 1.2 93/02/12 16:33:39 leres Exp $ (LBL)

COMDIR=	../../common/net
MACHDIR=../net

OBJS=	slcompress.o if_sl.o xxxinit.o
INCLS=	-I${MACHDIR} -I${COMDIR} -I/sys/net
VERS=	-DVERSION="\"LBL `cat ../../RELEASE`\""
CFLAGS=	-DKERNEL -DINET ${INCLS} ${VERS}

all: cslip.o

cslip.o : ${OBJS}
	ld -r ${OBJS} -o cslip.o

sl.h:
	echo "#define NSL 2" > sl.h

bpfilter.h:
	echo "#define NBPFILTER 16" > bpfilter.h

if_sl.c:
	ln -s ${MACHDIR}/$@

slcompress.c:
	ln -s ${COMDIR}/$@

if_sl.o: sl.h bpfilter.h

clean:
	rm -f cslip.o ${OBJS} sl.h bpfilter.h slcompress.c if_sl.c
