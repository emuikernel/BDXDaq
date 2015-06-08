#!/bin/sh
#\
exec /usr/bin/expect -- "$0" ${1+"$@"}
#
set roc $argv
set roc [string tolower $roc]
if { $roc == "sc-laser1" } { set ROC LASER1
} else { set ROC [string toupper $roc] }
puts "roc=$roc"
puts "ROC=$ROC\n"

set env(TCL_LIBRARY)  "/usr/local/coda/2.2.1r2/common/lib/tcl7.4"
set clonlog           "$env(CLON_LOG)/rocs"
set f_out_real_name   "$clonlog/boot_$roc.log"
set f_bootenv         "$env(CLON_VXWORKS)/vxbootenv"
set tsconnect         "/usr/local/system/tsconnect"

set timeout 10
set control_X \x18


#*** Default ROC boot parameters ***
set n_parm 14
set parm(1)  "dc"                     ;# boot device
set parm(2)  "0"                      ;# processor number
set parm(3)  "clon10-daq1"            ;# host name
set parm(4)  "\$VXKERN_???"           ;# file name
set parm(5)  "???.??.??.??:fffffc00"  ;# inet on ethernet (e)
set parm(6)  ""                       ;# inet on backplane (b)
set parm(7)  "129.57.68.21"           ;# host inet (h)
set parm(8)  ""                       ;# gateway inet (g)
set parm(9)  "clasboot"               ;# user (u)
set parm(10) ""                       ;# ftp password (pw) (blank = use rsh)
set parm(11) "0x20"                   ;# flags (f)
set parm(12) "???"                    ;# target name (tn)
set parm(13) "\$VXBOOT_???"           ;# startup script (s)
set parm(14) ""                       ;# other (o)


#*** All possible CLAS ROC's names ***
set roc_name "dc1 dc2 dc3 dc4 dc5 dc6 dc7 dc8 dc9 dc10 dc11 \
	lac1 tage tage3 cc1 cc2 ec1 ec2 sc1 \
	scaler1 scaler2 scaler3 scaler4 polar \
	clastrig2 dccntrl camac1 camac2 camac3 \
	sc-laser1 ioctorus1 clasgas \
	classc1 classc2 classc3 classc4 classc5 classc8 classc12 \
	pretrig1 pretrig2 croctest1 croctest2 croctest4 \
	primexts primexroc1"

set reset_line "reset2 reset2 reset5 reset5 reset4 reset4 reset4 reset4 reset5 reset5 reset4 \
	reset3 reset5 reset5 reset1 reset1 reset1 reset1 reset1 \
	reset1 reset1 reset1 reset1 reset2 \
	reset1 reset2 reset1 reset3 reset1 \
	reset2 reset2 reset6 \
	reset2 reset5 reset2 reset1 reset1 reset1 reset2 \
	reset1 reset1 test-reset test-reset test-reset \
	reset5 reset5"

set target_name "DC1 DC2 DC3 DC4 DC5 DC6 DC7 DC8 DC9 DC10 DC11 \
	LACS CLASTAG TRIGMON CCTAG1 CC2 EC1 EC2 TOF1 \
	SCALE1 SCALE2 SCALE3 SCALE4 POLAR \
	CTRIG2 DCHV CAMAC1 CAMAC2 CAMAC3 \
	LASER1 TORUS CLASGAS \
	CLASSC1 SC2 SC3 SC4 CLASSC5 SC8 SC12 \
	PRETRIG1 PRETRIG2 PRETRG1 URVME2 URVME1 \
	PRIMEXTS PRIMEXROC1"


#*** Open output file for any useful and error messages ***
set    f_out [open "$f_out_real_name" a]
spawn  chmod 664 $f_out_real_name
spawn  date
expect "\n"
puts  $f_out "\n=============================================================="
puts  $f_out "\nROC = \"$roc\",  TIME = $expect_out(buffer)"
close $f_out


#*** Determine ROC reset-line & TAG-name ***
set i     [lsearch -exact $roc_name $roc]
if { $i == -1 } {
    set    f_out [open "$f_out_real_name" a]
    puts  $f_out " roc_name error: Unknown ROC = \"$roc\" \n"
    close $f_out
    exit
}
set rline [lindex $reset_line  $i]
set hop   [lindex $target_name $i]
puts "$i: roc=$roc, reset_line=$rline, reset_name=$hop\n"


#*** HARD RESET for epics ROCs without corrections of boot parameters ***
#*** We can't check their boot parameters, because they are not unify ***
if { ($roc == "ioctorus1") || ($roc == "clasgas") || \
     ($roc == "classc1")   || ($roc == "classc2") || \
     ($roc == "classc3")   || ($roc == "classc4") || \
     ($roc == "classc5")   || ($roc == "classc8") || ($roc == "classc12") } {
  spawn $tsconnect $rline
  expect {
    "No such device" {
       set    f_out [open "$f_out_real_name" a]
       puts  $f_out " tsconnect error: \"$rline\" is unavailable.\n"
       close $f_out
       exit
    }
    "help." {
       send  "!$hop:" 
       expect "$hop>"
       send   RES
       expect RES
       send "\r\r"
       send "~x"
       set    f_out [open "$f_out_real_name" a]
       puts  $f_out "  ROC \"$roc\":   RESET  done.\n"
       close $f_out
       exit
    }
  }
}


#*** Finding ROC inet ***
spawn getipbyname $roc
expect "$roc"
set tmpstr $expect_out(buffer)
expect {
  "not found" {
    set    f_out [open "$f_out_real_name" a]
    puts  $f_out " getipbyname error: $tmpstr $expect_out(0,string) \n"
    close $f_out
    exit
  }
  "." {
    expect "\n"
    set roc_inet [split  $expect_out(buffer)]
    set roc_inet [lindex $roc_inet 1]
  }
}
###spawn nslookup $roc
###expect "$roc"
###expect {
###  "Address:  " {
###    expect "\n"
###    set roc_inet [split  $expect_out(buffer)]
###    set roc_inet [lindex $roc_inet 0]
###  }
###  "Non-existent host/domain" {
###    set    f_out [open "$f_out_real_name" a]
###    puts  $f_out " nslookup error: $expect_out(0,string) \"$roc\"\n"
###    close $f_out
###    exit
###  }
###  "Server failed" {
###    set    f_out [open "$f_out_real_name" a]
###    puts  $f_out " nslookup error: $expect_out(0,string) \"$roc\"\n"
###    close $f_out
###    exit
###  }
###}


#*** Corrections of ROC boot parameters ***
#*** ================================== ***

#***  . correction of boot device name: ei OR dc OR er ***
puts "default   parm(1)=$parm(1);"
set f_r  [open "$f_bootenv" r]
set bootdev ""
set f_end 0
while { ([gets $f_r line] != -1) && ($f_end == 0) } {
	if {[string match "*#*" $line]} {
	} elseif {[string match "*VXKERN_$ROC *" $line]} {
	    puts "  line=$line"
	    set CPU_full [lindex [split [lindex $line 2] "vx"] 2]
	    set CPU_n [join [lrange [split $CPU_full ""] 0 2] ""]
	    puts "  CPU_n=$CPU_n"
	    if       { ($CPU_n==162) || ($CPU_n==167) } { set bootdev "ei"
	    } elseif { ($CPU_n==230) || ($CPU_n==240) } { set bootdev "dc"
	    } elseif {  $CPU_n==510                   } { set bootdev "fei"
	    } elseif {  $CPU_n==550                   } { set bootdev "gei" }
	    puts "  bootdev=$bootdev"
	    set f_end 1
	}
}
close $f_r
if { $bootdev != "" } {
	set parm(1) $bootdev
} else {
	set    f_out [open "$f_out_real_name" a]
	puts  $f_out " vxbootenv error: Did not find env variable VXKERN_$ROC"
	puts  $f_out "                  in file $f_bootenv \n"
	close $f_out
	exit
}
puts "corrected parm(1)=$parm(1);\n"

#***  . correction of host name ***
puts "default   parm(3)=$parm(3);"
set subnet [lindex [split $roc_inet "."] 2]
if       { $subnet == 167 } { set parm(3) "clon10"
} elseif { $subnet ==  69 } { set parm(3) "clon10-69" }
puts "  roc_inet=$roc_inet"
puts "  subnet=$subnet"
puts "corrected parm(3)=$parm(3);\n"

#***  . correction of Kernel file name ***
puts "default   parm(4)=$parm(4);"
set p [split $parm(4) ""]
set p [lreplace $p 8 10 $ROC]
set parm(4) [join $p ""]
puts "corrected parm(4)=$parm(4);\n"

#***  . correction of ROC inet ***
puts "default   parm(5)=$parm(5);"
if { ($subnet == 167) || ($subnet == 68) || ($subnet == 69) } {
    set parm(5) "$roc_inet:ffffff00"
}
puts "corrected parm(5)=$parm(5);\n"

#***  . correction of host inet ***
puts "default   parm(7)=$parm(7);"
if       { $subnet == 167 } { set parm(7) "129.57.167.14"
} elseif { $subnet ==  69 } { set parm(7) "129.57.69.2" }
puts "corrected parm(7)=$parm(7);\n"

#***  . correction of gateway inet (special for TAGE & CROCTEST1) ***
puts "default   parm(8)=$parm(8);"
if { $subnet == 69 } { set parm(8) "129.57.69.1" }
puts "corrected parm(8)=$parm(8);\n"

#***  . correction of ROC target name ***
puts "default   parm(12)=$parm(12);"
set parm(12) $roc
puts "corrected parm(12)=$parm(12);\n"

#***  . correction of startup script name ***
puts "default   parm(13)=$parm(13);"
set p [split $parm(13) ""]
set p [lreplace $p 8 10 $ROC]
set parm(13) [join $p ""]
puts "corrected parm(13)=$parm(13);\n"
for {set i 1} {$i <= $n_parm} {incr i} { puts " $i:  $parm($i);" }


#*** Start conversation with ROC ***
spawn $tsconnect $rline
expect {
	"No such device" {
		set    f_out [open "$f_out_real_name" a]
		puts  $f_out " tsconnect error: \"$rline\" is unavailable.\n"
		close $f_out
		exit
	}
	"help." {
		send  "!$hop:" 
		expect "$hop>"
		send   RES
		expect RES
		send "\r\r"
		send "~x"
		sleep 3
	}
}

spawn $tsconnect $roc
expect "help."
send $control_X
expect {
	timeout {
	    set    f_out [open "$f_out_real_name" a]
	    puts  $f_out " tsconnect error: Time out. ROC \"$roc\" does not answer.\n"
	    close $f_out
	    exit
	}
	"Press any key to stop auto-boot" {
	    send "\r\r"
	}
	"VxWorks Boot" {
	    send "\r"
	}
}


#*** Check ROC boot parameters and loading good ones ***
expect {
	"VxWorks Boot" {
	    send "c\r"
	    expect "= quit"
	    expect "\n"
	    expect "\n"
	    for {set i 1} {$i <= $n_parm} {incr i} {
			expect ": "
			set str1 [split $expect_out(buffer)]
			if { [string length [lindex $str1 0]] < 1 } {
				set str1 [lreplace $str1 0 0 ""]
			}
			set str1 [join $str1]
			send "\r"
			expect "\n"
			set str2 [split $expect_out(buffer)]
			set str2 [lindex $str2 0]
			if { ($str2 != $parm($i)) && ($str2 != "$parm($i)0") } {
				set    f_out [open "$f_out_real_name" a]
				puts  $f_out " Wrong boot parameter, $str1$str2"
				close $f_out
				if {$i == $n_parm} {
					send "c\r"
					send "\r\r\r\r\r\r\r\r\r\r\r\r\r"
					send ".\r"
				} else {
					expect ": "
					send -- "-"
					send "\r"
					expect ": "
					if {$parm($i) == ""} {
						send ".\r"
						expect "."
						expect "\n"
					} else {
						send "$parm($i)\r"
						expect "$parm($i)"
						expect "\n"
					}
				}
			}
	    }
	}
}


#*** Start SOFT RESET ***
expect "VxWorks Boot"
send "@\r"
expect "Attaching network"
send "\r\r"
send "~x"


#*** Start HARD RESET through RESET line ***
spawn $tsconnect $rline
expect "help."
send  "!$hop:" 
expect "$hop>"
send   RES
expect RES
send "\r\r"
send "~x"


#*** Waiting for the end of ROC booting ***
spawn date
expect "\n"
for {set i 1} {$i<=8} {incr i} {
  sleep 30
  spawn date
  expect "\n"
  set tt $expect_out(buffer)
  spawn get_roc_transition $roc
  expect {
    -- "-1" {
      if { $i == 8 } {
        set    f_out [open "$f_out_real_name" a]
        puts  $f_out "  ROC \"$roc\" did not boot within 4 minutes.\n"
        close $f_out
      }
      puts "\n  from \"-1\"   : i=$i"
    }
    "booted" {
      set    f_out [open "$f_out_real_name" a]
      puts  $f_out "  ROC \"$roc\" is booted. $tt \n"
      close $f_out
      set i 9
      puts "\n  from booted : i=$i \n\n"
    }
  }
}


exit
