#
# bootpd   - BOOTP server daemon
#
# Wind River Modification History
# -------------------------------
# 01c,04dec97,spm  added settings to build for Solaris with native compiler
# 01b,16oct92,kdl  made Sun configuration the default.
# 01a,06jun92,elh  original CMU version.


# $Header: Makefile,v 2.1 93/06/07 15:18:41 wise active $
#

# The next few lines may be uncommented and changed to alter the default
# filenames bootpd uses for its configuration and dump files.
#CONFFILE=-DCONFIG_FILE=\"/usr/etc/bootptab\"
#DUMPFILE=-DDUMP_FILE=\"/usr/etc/bootpd.dump\"
#FILESPECS=${CONFFILE} ${DUMPFILE}

# SunOS 4.0 is supported by default with -DSUNOS40 in BOOTPDOPT below.
# Solaris builds require a native Solaris C compiler. In addition, Solaris
# users will need to use the alternate BOOTPDOPT setting (which replaces 
# -DSUNOS40 with -DSOLARIS) as well as the LDFLAGS setting below.
# Remove the -DVEND_CMU if you don't wish to support the "CMU vendor format"
# in addition to the RFC1048 format.

BOOTPDOPT=-DSUNOS40 -DSYSLOG -DDEBUG -DVEND_CMU -DPRIVATE=static ${FILESPECS}
#BOOTPDOPT=-DSOLARIS -DSYSLOG -DDEBUG -DVEND_CMU -DPRIVATE=static -misalign ${FILESPECS}

INSTALL=/usr/bin/install
DESTDIR=
ETCDIR=/etc
OBJS=bootpd.o readfile.o hash.o
CFLAGS=${BOOTPDOPT}

# Use the LDFLAGS setting for Solaris builds
# LDFLAGS=-lsocket -lnsl

all: bootpd

bootpd:	${OBJS}
	sh newvers.sh Version.c
	${CC} ${CFLAGS} -o bootpd version.c ${OBJS} ${LDFLAGS}

bootpd.o: bootpd.c bootpd.h bootp.h
readfile.o: readfile.c bootpd.h bootp.h
hash.o: hash.c hash.h

system: install

install: all
	${INSTALL} -c -s bootpd ${DESTDIR}${ETCDIR}/bootpd

clean:
	@rm -f core bootpd *.BAK *.CKP *~ .emacs* *.o version.c
