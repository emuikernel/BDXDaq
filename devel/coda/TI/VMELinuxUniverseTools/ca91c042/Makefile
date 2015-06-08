all: ca91c042.o
install: all devices

INCLUDE = /usr/src/linuxcic/include/

CFLAGS = -O2 -Wall

ca91c042.o: ca91c042.c ca91c042.h Makefile
	gcc -I${INCLUDE} ${CFLAGS} -D__KERNEL__ -c ca91c042.c

clean:
	rm ca91c042.o

devices:
	mknod /dev/vme_m0  c 221 0
	mknod /dev/vme_m1  c 221 1
	mknod /dev/vme_m2  c 221 2
	mknod /dev/vme_m3  c 221 3
	mknod /dev/vme_m4  c 221 4
	mknod /dev/vme_m5  c 221 5
	mknod /dev/vme_m6  c 221 6
	mknod /dev/vme_m7  c 221 7
	mknod /dev/vme_ctl c 221 8
	chown root /dev/vme*
	chgrp users /dev/vme*
	chmod 666 /dev/vme*
	chmod 600 /dev/vme_m3
	chmod 600 /dev/vme_m7
