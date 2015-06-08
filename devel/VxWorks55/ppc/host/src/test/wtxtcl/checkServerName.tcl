# checkServerName.tcl - Tcl script, test validity of target server name
#
# Copyright 1995 Wind River Systems, Inc.
#
#
# modification history
# --------------------
# 01i,28jan98,p_b  Adapted for WTX 2.0
# 01h,15may96,jmp  checked that tgtsvrName regular expression is different
#		   wtxregd (Wind Registry).
# 01g,12feb96,jmp  now checkServerName returns 1 if Target Server is unknown,
#		   2 if there is an ambiguity in target server name.
# 		   catched wtxInfoQ.
# 01f,31oct95,jmp  fixed a minor bug in regular expression.
# 01e,29aug95,jmp  managed regular expression for tgtsvrName
# 01d,27jun95,f_v  fixed a message
# 01c,16may95,f_v  removed extended tcl functions
# 01b,11may95,f_v  managed same target server name on different hosts.
# 01a,17mar95,f_v  written.
#
#
#
# DESCRIPTION
#
# test validity of target server name 
# for example tgtsvrName could be vxsim0 or vxsim0@aven
#	<vxsim0> simulator name
#	<aven>   host station name
#
# if there is an error this script returns 1
#
#

proc checkServerName { tgtsvrName } {
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
