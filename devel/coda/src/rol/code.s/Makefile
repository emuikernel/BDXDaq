#
# Description:
#   	Makefile for all ROC code (MV and PPC)...EJW, 27-may-97
#


all: ppc


mv:
	make -f Makefile.mv

ppc:
	make -f Makefile.ppc


clean:
	make -f Makefile.ppc clean
#	make -f Makefile.mv clean

distclean:
	make -f Makefile.ppc distclean
#	make -f Makefile.mv distclean

headers:
	rm -rf tmp
	mkdir tmp
	cp *.h tmp
	chmod 664 tmp/*
	cp tmp/* $(CLON_INC)
	rm -rf tmp

install: exports	
exports: headers
	make -f Makefile.ppc exports
#	make -f Makefile.mv exports







