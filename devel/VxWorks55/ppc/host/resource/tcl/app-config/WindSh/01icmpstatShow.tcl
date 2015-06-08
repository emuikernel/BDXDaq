# 01icmpstatShow.tcl - Implementation of shell icmpstatShow commands in Tcl
#
# Copyright 1998 Wind River Systems, Inc.
#
# modification history
# --------------------
# 01a,15jun98,f_l  written.
#
# DESCRIPTION
# This file holds the Tcl implementation of the windsh icmpstatShow command
#
# RESOURCE FILES
# wpwr/host/resource/tcl/icmpstatShowCore.tcl
#


# source the data extraction and formatting routines

source [wtxPath host resource tcl]icmpstatShowCore.tcl

##############################################################################
#
# icmpstatShow - display statistics for ICMP
#
# This routine displays statistics for the ICMP
# (Internet Control Message Protocol) protocol.
#
# SYNOPSIS: N/A
#
# PARAMETERS: N/A
#
# RETURNS: N/A
#
# ERRORS: N/A
#

shellproc icmpstatShow {args} {

    set icmpstatInfo [icmpstatInfoGet]

    bindNamesToList {icps_error    \
		     icps_oldshort \
		     icps_oldicmp  \
		     icps_outhist  \
		     icps_badcode  \
		     icps_tooshort \
		     icps_checksum \
		     icps_badlen   \
		     icps_reflect  \
		     icps_inhist} $icmpstatInfo

    set icmpNames [list "echo reply" "#1" "#2" "destination unreachable"\
			"source quench" "routing redirect" "#6" "#7" "echo"\
			"#9" "#10" "time exceeded" "parameter problem"\
			"time stamp" "time stamp reply" "information request"\
			"information request reply" "address mask request"\
			 "address mask reply"]

    puts stdout [format "ICMP:\n\t%u call%s to icmp_error" $icps_error\
			[plural $icps_error]]
    puts stdout [format "\t%u error%s not generated because old\
			 message was icmp" $icps_oldicmp [plural $icps_oldicmp]]
    set first 1

    # contant ICMP_MAXTYPE = 18

    for { set ix 0} {$ix < 18} {incr ix} {

	set outhistElement [lindex $icps_outhist $ix]

	if {$outhistElement != 0} {
	    if {$first} {
		puts stdout "\tOutput histogram:"
	 	set first 0
	    }
	    puts stdout [format "\t\t%s: %u" [lindex $icmpNames $ix]\
					$outhistElement]
	}
    }

    puts stdout [format "\t%u message%s with bad code fields" \
    		$icps_badcode [plural $icps_badcode]]
    puts stdout [format "\t%u message%s < minimum length" \
    		$icps_tooshort [plural $icps_tooshort]]
    puts stdout [format "\t%u bad checksum%s" $icps_checksum\
    			[plural $icps_checksum]]
    puts stdout [format "\t%u message%s with bad length" $icps_badlen\
    			[plural $icps_badlen]]
    set first 1

    # contant ICMP_MAXTYPE = 18

    for { set ix 0} {$ix < 18} {incr ix} {

	set inhistElement [lindex $icps_inhist $ix]

	if {$inhistElement != 0} {
	    if {$first} {
		puts stdout "\tInput histogram:"
	 	set first 0
	    }
	    puts stdout [format "\t\t%s: %u" [lindex $icmpNames $ix]\
					$inhistElement]
	}
    }

    puts stdout [format "\t%u message response%s generated" $icps_reflect\
			[plural $icps_reflect]]
}
