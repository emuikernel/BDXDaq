h28975
s 00001/00000/00060
d D 1.19 10/03/24 15:56:23 boiarino 19 18
c *** empty log message ***
e
s 00001/00000/00059
d D 1.18 08/11/26 12:38:37 puneetk 18 17
c added create_pretrig3conf.pl to copy list
c 
e
s 00005/00004/00054
d D 1.17 08/11/19 13:10:40 puneetk 17 16
c added setIC_inner_ring.pl to be copied to common area
c 
e
s 00002/00002/00056
d D 1.16 08/10/24 10:04:40 boiarino 16 15
c *** empty log message ***
e
s 00006/00004/00052
d D 1.15 08/10/24 10:03:52 boiarino 15 14
c *** empty log message ***
e
s 00002/00000/00054
d D 1.14 08/10/22 10:52:24 boiarino 14 13
c *** empty log message ***
e
s 00002/00000/00052
d D 1.13 08/10/22 10:51:16 puneetk 13 12
c *** empty log message ***
e
s 00003/00000/00049
d D 1.12 08/10/20 15:57:24 boiarino 12 11
c *** empty log message ***
e
s 00001/00000/00048
d D 1.11 07/10/26 22:34:24 boiarino 11 10
c *** empty log message ***
e
s 00001/00001/00047
d D 1.10 07/04/03 09:45:51 boiarino 10 9
c comment out 'monb' - it is in evmon/dab
c 
e
s 00001/00000/00047
d D 1.9 07/03/16 15:14:58 boiarino 9 8
c add photon_prestart_noMOR_20
c 
e
s 00001/00000/00046
d D 1.8 07/03/14 09:24:35 boiarino 8 7
c add monb
c 
e
s 00002/00000/00044
d D 1.7 07/03/12 23:13:44 boiarino 7 6
c add checklist_run_print
c 
e
s 00004/00004/00040
d D 1.6 06/12/05 10:37:06 boiarino 6 5
c *** empty log message ***
e
s 00001/00001/00043
d D 1.5 06/12/05 10:35:53 boiarino 5 4
c install OPLOG.pm to $CLON/common/perl instead of $CLON_LIB
c 
e
s 00001/00000/00043
d D 1.4 06/11/29 14:36:07 boiarino 4 3
c add pedman_links
c 
e
s 00002/00004/00041
d D 1.3 06/11/06 15:28:42 sergpozd 3 2
c *** empty log message ***
e
s 00010/00002/00035
d D 1.2 06/11/06 15:27:42 boiarino 2 1
c *** empty log message ***
e
s 00037/00000/00000
d D 1.1 06/07/01 09:29:04 boiarino 1 0
c date and time created 06/07/01 09:29:04 by boiarino
e
u
U
f e 0
t
T
I 1
#
# export scripts
#

all:
	@echo "Nothing to make in scripts dir"


install: exports
exports:
	-chmod -f gu+rwx  $(CLON)/common/scripts/*
	-chmod -f a+x     $(CLON)/common/scripts/*
D 6
	-chmod -f gu+rwx  $(CLON_LIB)/OPLOG.pm
	-chmod -f a+x     $(CLON_LIB)/OPLOG.pm
E 6
I 6
	-chmod -f gu+rwx  $(CLON)/common/perl/*
	-chmod -f a+x     $(CLON)/common/perl/*
E 6
#####################
#####################
#
D 2
	cp OPLOG.pm 	        $(CLON_LIB)
	cp online_buttons 	    $(CLON)/common/scripts
E 2
I 2
D 5
	cp OPLOG.pm 	        	$(CLON_LIB)
E 5
I 5
	cp OPLOG.pm 	        	$(CLON)/common/perl
I 12
	cp CaldbUtil.pm 	        $(CLON)/common/perl
E 12
I 7

	cp checklist_run_print 	    $(CLON)/common/scripts
I 8
D 10
	cp monb 	    			$(CLON)/common/scripts
E 10
I 10
##	cp monb 	    			$(CLON)/common/scripts
E 10
E 8
E 7
E 5
	cp online_buttons 	    	$(CLON)/common/scripts
	cp pedman_action			$(CLON)/common/scripts
	cp pedman_read_all			$(CLON)/common/scripts
	cp pedman_dump_all			$(CLON)/common/scripts
I 4
	cp pedman_links				$(CLON)/common/scripts
E 4
	cp rlComment				$(CLON)/common/scripts
D 3
	cp photon_prestart_g11		$(CLON)/common/scripts
	cp photon_prestart_noMOR	$(CLON)/common/scripts
	cp photon_prestart_norm		$(CLON)/common/scripts
	cp photon_prestart_STxMOR	$(CLON)/common/scripts
E 3
I 3
	cp photon_prestart_2313		$(CLON)/common/scripts
	cp photon_prestart_noMOR_40	$(CLON)/common/scripts
I 9
	cp photon_prestart_noMOR_20	$(CLON)/common/scripts
I 12
	cp photon_prestart_g12		$(CLON)/common/scripts
I 19
	cp photon_prestart_g9frost	$(CLON)/common/scripts
E 19
E 12
I 11
	cp db2clonmap.pl			$(CLON)/common/scripts
I 12
	cp addfunction.pl			$(CLON)/common/scripts
I 13
D 15
	cp makelut			$(CLON)/common/scripts
	cp ic_clusters.pl		$(CLON)/common/scripts
I 14
	cp s_tof_all_angles		$(CLON)/common/scripts
	cp s_tof_large_angle	$(CLON)/common/scripts
E 15
I 15
D 17
	cp makelut					$(CLON)/common/scripts
E 17
I 17
	cp makelut				$(CLON)/common/scripts
E 17
	cp ic_clusters.pl			$(CLON)/common/scripts
	cp s_tof_all_angles			$(CLON)/common/scripts
D 17
	cp s_tof_large_angle		$(CLON)/common/scripts
D 16
	cp s_cc_on					$(CLON)/common/scripts
	cp s_cc_off					$(CLON)/common/scripts
E 16
I 16
	cp s_cc_all_on					$(CLON)/common/scripts
	cp s_cc_all_off					$(CLON)/common/scripts
E 17
I 17
	cp s_tof_large_angle			$(CLON)/common/scripts
	cp s_cc_all_on				$(CLON)/common/scripts
	cp s_cc_all_off				$(CLON)/common/scripts
	cp setIC_inner_ring.pl			$(CLON)/common/scripts
I 18
	cp create_pretrig3conf.pl		$(CLON)/common/scripts
E 18
E 17
E 16
E 15
E 14
E 13
E 12
E 11
E 9
E 3
E 2
#
###	cp clasrun.cshrc 	    /home/clasrun/.cshrc
###	cp clasrun.login 	    /home/clasrun/.login
	cp clasrun.alias 	    $(CLON)/common/scripts
	cp complete.tcsh 	    $(CLON)/common/scripts
	-chmod -f gu+rwx  $(CLON)/common/scripts/*
	-chmod -f a+x     $(CLON)/common/scripts/*
D 6
	-chmod -f gu+rwx  $(CLON_LIB)/OPLOG.pm
	-chmod -f a+x     $(CLON_LIB)/OPLOG.pm
E 6
I 6
	-chmod -f gu+rwx  $(CLON)/common/perl/*
	-chmod -f a+x     $(CLON)/common/perl/*
E 6

clean: distclean
distclean:
	@echo "Nothing to clean in scripts area"





E 1
