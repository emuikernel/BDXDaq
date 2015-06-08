# @(#) $Header: Makefile,v 1.12 93/11/01 17:02:47 leres Exp $ (LBL)

ALL= myetheraddr slinfo sliplogin slstats tip
EVERYTHING= ${ALL} tools/ifconfig tools/slattach

all: ${ALL}

everything: ${EVERYTHING}

tar:
	@cwd=`pwd` ; dir=`basename $$cwd` ; name=`cat RELEASE | tr A-Z a-z` ; \
	    opt=cFFhf ; \
	    echo \
	    "rm -f ../$$name; ln -s $$dir ../$$name" ; \
	     rm -f ../$$name; ln -s $$dir ../$$name ; \
	    echo \
	    "(cd .. ; tar $$opt - $$name) | compress > /tmp/$$name.tar.Z" ; \
	     (cd .. ; tar $$opt - $$name) | compress > /tmp/$$name.tar.Z ; \
	    echo \
	    "rm -f ../$$name" ; \
	     rm -f ../$$name

tools/ifconfig:
	cd tools/ifconfig; ${MAKE}

tools/slattach:
	cd tools/slattach; ${MAKE}

slinfo:
	cd slinfo; ${MAKE}

sliplogin:
	cd sliplogin; ${MAKE}

slstats:
	cd slstats; ${MAKE}

tip:
	cd tip; ${MAKE}

clean:
	rm -f ${TARNAME}
	for i in ${EVERYTHING}; do \
	    (echo "[Cleaning $$i]"; cd $$i; f=""; \
	    if [ -f Makefile.oldmake ]; then f="-f Makefile.oldmake"; fi; \
	    ${MAKE} ${MFLAGS} $$f clean); done

install: ${ALL}
	for i in ${ALL}; do (cd $$i; ${MAKE} ${MFLAGS} install); done
