#
# export scripts
#

all:
	@echo "Nothing to make in scripts dir"


install: exports
exports:
	-chmod -f gu+rwx  $(CLON)/common/scripts/*
	-chmod -f a+x     $(CLON)/common/scripts/*
	-chmod -f gu+rwx  $(CLON)/common/perl/*
	-chmod -f a+x     $(CLON)/common/perl/*
#####################
#####################
#
	cp OPLOG.pm 	        	$(CLON)/common/perl
	cp CaldbUtil.pm 	        $(CLON)/common/perl

	cp checklist_run_print 	    $(CLON)/common/scripts
##	cp monb 	    			$(CLON)/common/scripts
	cp online_buttons 	    	$(CLON)/common/scripts
	cp pedman_action			$(CLON)/common/scripts
	cp pedman_read_all			$(CLON)/common/scripts
	cp pedman_dump_all			$(CLON)/common/scripts
	cp pedman_links				$(CLON)/common/scripts
	cp rlComment				$(CLON)/common/scripts
	cp photon_prestart_2313		$(CLON)/common/scripts
	cp photon_prestart_noMOR_40	$(CLON)/common/scripts
	cp photon_prestart_noMOR_20	$(CLON)/common/scripts
	cp photon_prestart_g12		$(CLON)/common/scripts
	cp photon_prestart_g9frost	$(CLON)/common/scripts
	cp db2clonmap.pl			$(CLON)/common/scripts
	cp addfunction.pl			$(CLON)/common/scripts
	cp makelut				$(CLON)/common/scripts
	cp ic_clusters.pl			$(CLON)/common/scripts
	cp s_tof_all_angles			$(CLON)/common/scripts
	cp s_tof_large_angle			$(CLON)/common/scripts
	cp s_cc_all_on				$(CLON)/common/scripts
	cp s_cc_all_off				$(CLON)/common/scripts
	cp s_tpe_or				$(CLON)/common/scripts
	cp setIC_inner_ring.pl			$(CLON)/common/scripts
	cp create_pretrig3conf.pl		$(CLON)/common/scripts
#
###	cp clasrun.cshrc 	    /home/clasrun/.cshrc
###	cp clasrun.login 	    /home/clasrun/.login
	cp clasrun.alias 	    $(CLON)/common/scripts
	cp complete.tcsh 	    $(CLON)/common/scripts
	-chmod -f gu+rwx  $(CLON)/common/scripts/*
	-chmod -f a+x     $(CLON)/common/scripts/*
	-chmod -f gu+rwx  $(CLON)/common/perl/*
	-chmod -f a+x     $(CLON)/common/perl/*

clean: distclean
distclean:
	@echo "Nothing to clean in scripts area"





