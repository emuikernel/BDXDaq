#
# Makefile for pedman/spardb/unix
#

CC = cc

INC   = -I$(CODA)/common/include 
LIBS  = -L$(CODA)/SunOS_sun4u/lib -lrpcsvc -lmysql4.1.20 -lpthread -lresolv \
				-lXpm -lXm -lXmu -lX11 -lXt -lX -lXext \
				-lsocket -lnsl -ldl -lm -lrt


all : pedman_init pedman_read pedman_write pedman_dump

pedman_init: pedman_init.c
	$(CC) pedman_init.c $(INC) $(LIBS) -o pedman_init

pedman_read: pedman_read.c
	$(CC) pedman_read.c $(INC) $(LIBS) -o pedman_read

pedman_write: pedman_write.c
	$(CC) pedman_write.c $(INC) $(LIBS) -o pedman_write

pedman_dump: pedman_dump.c
	$(CC) pedman_dump.c $(INC) $(LIBS) -o pedman_dump


clean :
	rm -f *.o pedman_init pedman_read pedman_write pedman_dump

distclean :
	rm -f *.o pedman_init pedman_read pedman_write pedman_dump

exports: install
install:
	cp pedman_init pedman_read pedman_write pedman_dump $(CLON_BIN)










