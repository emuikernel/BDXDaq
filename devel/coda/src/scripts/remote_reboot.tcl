#!/bin/sh
#\
exec /usr/bin/expect -- "$0" ${1+"$@"}
#

set roc $argv

set roc_name "dc1 dc2 dc3 dc4 dc5 dc6 dc7 dc8 dc9 dc10 dc11 \
	lac1 cc1 cc2 ec1 ec2 sc1 tage3 \
	scaler1 scaler2 scaler3 scaler4 \
	pretrig1 pretrig2 camac1 camac2 sc-laser1 \
	dc12 dccntrl ioctorus1 iocptarg clasgas \
	classc1 classc2 classc3 classc4 classc5 classc6 classc7 classc8 classc12 \
	primexts2 primexroc4 primexroc5 primexroc6 primexioc1 primexioc2 \
	croctest4"

set reset_line "reset2 reset2 reset5 reset5 reset4 reset4 reset4 reset4 reset5 reset5 reset4 \
	reset3 reset1 reset1 reset1 reset1 reset1 reset5 \
	reset1 reset1 reset1 reset1 \
	reset1 reset1 reset1 reset3 reset2 \
	reset5 reset5 reset2 reset5 reset6 \
	reset2 reset5 reset2 reset1 reset1 reset2 reset1 reset1 reset2 \
	reset4 reset4 reset4 reset4 reset4 reset4 \
	test-reset"

set target_name "DC1 DC2 DC3 DC4 DC5 DC6 DC7 DC8 DC9 DC10 DC11 \
	LACS CCTAG1 CC2 EC1 EC2 TOF1 PRETRG1 \
	SCALE1 SCALE2 SCALE3 SCALE4 \
	PRETRIG1 PRETRIG2 CAMAC1 CAMAC2 LASER1 \
	POLAR DCHV TORUS PTARG CLASGAS \
	CLASSC1 SC2 SC3 SC4 CLASSC5 CLASSC6 CLASSC7 SC8 SC12 \
	PXTS2 PXROC2 PXROC5 PXROC6 PXIOC1 PXIOC2 \
	URVME1"

set i [lsearch -exact $roc_name $roc]
set rline [lindex $reset_line $i]
set hop [lindex $target_name $i]

###spawn /usr/local/system/tsconnect $rline
spawn tsconnect $rline
expect "help."
send "!$hop:" 
expect "$hop>"
send RES
expect RES
send "\r\r"
send "~x"

exit


#	bonuspc1 bonuspc2 gluex_ts2 \
#	reset2 reset2 reset7 \
#	URVME2 TRIGMON GLUEX1 \

#	croctest4 croctest5"
#	test-reset test-reset"
#	URVME1 CTRIG2"
