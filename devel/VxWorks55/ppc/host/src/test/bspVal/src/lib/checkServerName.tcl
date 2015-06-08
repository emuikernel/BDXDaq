# checkServerName.tcl - Tcl script, test validity of target server name
#
# Copyright 1995 Wind River Systems, Inc.
#
# modification history
# --------------------
# 01b,31jul97,db  doc tweaks for man page generation.
# 01a,25mar97,sbs taken from wtxtest test routines.
#
# DESCRIPTION
# test validity of target server name 
# for example tgtsvrName could be vxsim0 or vxsim0@aven
#	<vxsim0> simulator name
#	<aven>   host station name
#*/

###############################################################################
#
# checkServerName - test validity of target server
#
# SYNOPSIS:
#   checkServerName tgtsvrName
#
# PARAMETERS:
#   tgtsvrName: name of target server to tested
#
# RETURNS: 
# 1 if any error
#
# ERRORS:   
# error statement received in getting wtxInfoQ list.
# "There is an ambiguity in your target server name"
#  
 
proc checkServerName {tgtsvrName} {
    set list {}

    # get target server list
    if {[catch "wtxInfoQ .*" servList]} {
	puts stdout "Error: $servList"
	exit 3
    }

    # search if there is same name on different hosts 
    foreach i $servList {
       set tmp [string match *$tgtsvrName* [lindex $i 0] ]
       if {[string match *$tgtsvrName* [lindex $i 0] ]} {
		set list [linsert $list 0 [lindex $i 0]]
		}
       }
    set sameName [llength $list]

    # Test if Target Server is known and different than Wind Registry
    if {$list == "" || [string match wtxregd* $list]} {
	exit 1
	}

    # Test if there is an ambiguity in Target Server name
    if {$sameName > 1} {
	puts stdout "ERROR : There is an ambiguity in your target server name"
	puts stdout $list
	exit 2
	}

    set tgtsvrName $list

    # check if target server name is valid (with or without host name)
    foreach server $servList {
	set svName [lindex $server 0]
	if { [string first @ $tgtsvrName] == -1 } {
	     set pos [string first @ $svName] 
	     set shortSvName [string range $svName 0 [expr $pos-1]]

	     set err [string compare $tgtsvrName $shortSvName] 

	   } else {

	     set err [string compare $tgtsvrName $svName] 
	   }

	if {$err == 0 } {
	      break
	   }
    }
}
