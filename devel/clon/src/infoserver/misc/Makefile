#
# Makefile for scaler info-server
#
INC = -I../includes

CFLAGS = $(INC)

all: killer sender

killer: killer.c ../includes/servmsg.h ../includes/argproc.h
	rtlink $(CFLAGS) -o killer killer.c

sender: evtsender.c ../includes/servmsg.h ../includes/argproc.h
	rtlink $(CFLAGS) -o sender evtsender.c 


distclean:
	rm -f *.o killer sender

clean:
	rm -f *.o

install: exports
exports:
	echo "Nothing to do for install in misc"
