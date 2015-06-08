#
#  copies rcscripts to $(CODA)/common/rcscripts directories
#

SHELL = /bin/csh

all: 
	@echo Nothing to do for all in rcscripts

clean:
	@echo Nothing to clean in rcscripts

distclean:
	@echo Nothing to distclean in rcscripts

exports: install
install:
	cp run_dispatch       $(CODA)/common/rcscripts
	chmod ug+rwx         $(CODA)/common/rcscripts/run_dispatch
	chmod a+x            $(CODA)/common/rcscripts/run_dispatch
	/bin/csh -s < rcexport
