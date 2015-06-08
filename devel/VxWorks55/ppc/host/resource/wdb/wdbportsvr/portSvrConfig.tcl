# portSvrConfig.tcl - Iniatilize a portServer for a wdb communication
#
# ************************************************************************
#         Copyright 1996-1998 by Wind River Systems, Inc.
#
#                        All Rights Reserved
#
# Permission to use, copy, modify, and distribute this software and its
# documentation for any purpose and without fee is hereby granted, provided
# that the above copyright notice appears in all copies and that both that
# copyright notice and this permission notice appear in supporting
# documentation, and that the name of WRS not be used in advertising or
# publicity pertaining to distribution of the software without specific,
# written prior permission.
#
# WRS DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING ALL
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL WRS BE
# LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
# WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
# ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR
# IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
#
# Wind River Systems, Inc. 1010 Atlantic Avenue, Alameda, California 94501
# ************************************************************************/
#
# Modification History
# --------------------
# 01b,05may98,rlp  ported on windows and renamed source to expect_src.
# 01a,29jan98,rlp  written
#
 
#default parameters

set windBase		$env(WIND_BASE)
set windHostType	$env(WIND_HOST_TYPE)
set home		$env(HOME)

set workDir	host/resource/wdb/wdbportsvr
set prog	$windBase/$workDir/portSvrConfig.exp

if {$windHostType != "x86-win32"} {
    set expect_src  $windBase/host/$windHostType/bin/expect
} else {
    if {[file exists $home\\.wind\\wdbportsvr.tcl]} {
        source $home\\.wind\\wdbportsvr.tcl
    }
}

#portServer parameters
if {$windHostType == "x86-win32"} {
	set configFile "$windBase\\host\\resource\\wdb\\wdbportsvr\\portSvrConfig"
} else {
	set configFile "$windBase/host/resource/wdb/wdbportsvr/portSvrConfig"
}

set fd [open $configFile r]

set usage \
  "usage: wtxtcl portSvrConfig.tcl <ctrl> <baudRate> <port> <ip_address>"

#check cmd line argument
if {$argc != 4 } {
        puts "$usage"
        exit -1
        }

#get portServer login
set portSvrLogin [gets $fd]
if { $portSvrLogin == ""} {
        puts "\nError: portServer login missing in your portSvrConfig file\n"
        exit -1
}
 
#get portServer password
set portSvrPasswd [gets $fd]
if { $portSvrPasswd == ""} {
        puts "\nError: portServer passwd missing in your portSvrConfig file\n"
        exit -1
}
 
#get ctrl from arg list
set ctrl [lindex $argv 0]
 
#get baudRate from arg list
set baudRate [lindex $argv 1]

#test baudRate
if {$baudRate !=9600 && $baudRate != 19200 &&	\
    $baudRate != 38400 && $baudRate !=57600} {
	puts " BAUDRATE = {9600, 19200, 38400, 57600} $baudRate isn't allowed"
	exit -1
	}

#get port from arg list
set port [lindex $argv 2]

#get portSvr  from arg list
set portSvr [lindex $argv 3]

exec $expect_src $prog $ctrl $baudRate $port $portSvr $portSvrLogin $portSvrPasswd

