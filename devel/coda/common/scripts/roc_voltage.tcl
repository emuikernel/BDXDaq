#!/bin/sh
#\
exec /usr/bin/expect -- "$0" ${1+"$@"}
#
#   SP, 26-Jun-2002
#   SP, 06-May-2009  revised

set timeout    10
set clonparms  "$env(CLON_PARMS)/rocs"
set ch(3)      "      "
set ch(4)      "     "
set ch(5)      "    "
set ch(6)      "   "
set ch(7)      "  "
set ch(8)      " "
set ch(9)      ""

set roc_name   "cc1 ec1 ec2 sc1 \
		lac1 \
		dc1 dc2 \
		dc5 dc6 dc7 dc8 dc11 \
		dc3 dc4 dc9 dc10 tage \
		croctest1"

set reset_line "reset1 reset1 reset1 reset1 \
		reset3 \
		reset2 reset2 \
		reset4 reset4 reset4 reset4 reset4 \
		reset5 reset5 reset5 reset5 reset5 \
		test-reset"

set target_name "CCTAG1 EC1 EC2 TOF1 \
		 LACS \
		 DC1 DC2 \
		 DC5 DC6 DC7 DC8 DC11 \
		 DC3 DC4 DC9 DC10 CLASTAG \
		 PRETRG1"

set nn    [llength $argv]
if { $nn < 1 } { puts " No input arguments"; exit }

set roc   [lindex $argv 0]
set ii    [lsearch -exact $roc_name $roc]
set rline [lindex $reset_line $ii]
foreach roc $argv {
  set ii  [lsearch -exact $roc_name $roc]
  set tmp [lindex $reset_line $ii]
  if { $tmp != $rline } { puts " Mixed reset lines"; exit }
}

spawn tsconnect $rline
expect {
    "already in use." {
	for {set i 0} { $i<$nn } { incr i } {
	    set roc [lindex $argv $i]
	    set ll  [llength [split $roc ""]]
	    set    f_out  [open "$clonparms/vstat_$roc.log" w]
	    puts  $f_out " $roc$ch($ll) volt. status:  reset line is busy, call later"
	    close $f_out
	}
    }
    "help." {
	for {set i 0} { $i<$nn } { incr i } {
	    set roc [lindex $argv $i]
	    set ll  [llength [split $roc ""]]
	    set ii  [lsearch -exact $roc_name $roc]
	    set hop [lindex $target_name $ii]
	    send "!$hop:"
	    expect {
		timeout {
		    set f_out [open "$clonparms/vstat_$roc.log" w]
puts $f_out " $roc$ch($ll) volt. status:  timeout error, tag \"$hop\" does not answer"
		    close $f_out
		}
		"$hop>" {
		    send "STA"
		    expect "S:"
		    expect "\n"
		    set stat [lindex  [split $expect_out(buffer)] 0]
		    set ll   [llength [split $roc  ""]  ]
		    set vv   [lindex  [split $stat ""] 1]
		    set aa   [lindex  [split $stat ""] 3]
		    set bb   [lindex  [split $stat ""] 5]
		    set vstat "bad, "
		    set astat "bad, "
		    set bstat "bad"
		    if { $vv == 1 } { set vstat "good," }
		    if { $aa == 0 } { set astat "good," }
		    if { $bb == 0 } { set bstat "good " }
		    set f_out [open "$clonparms/vstat_$roc.log" w]
puts $f_out " $roc$ch($ll) volt. status:  +/-15 & +5 = $vstat   -2 = $astat   -5.2 = $bstat"
		    close $f_out
		}
	    }
	}
	send "\r\r"
	send "~x"
    }
}

exit
