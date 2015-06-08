# wtxCpuInfoGetTest.tcl - tcl script to test wtxCpuInfoGet TCL API
#
# Copyright 1998 Wind River Systems, Inc.
#
# modification history
# --------------------
# 01b,10dec01,fle  updated list of available options for wtxCpuInfoGet
# 01a,29apr98,fle  written
#

#
# DESCRIPTION
#
# This is the wtxCpuInfoGet test suite for the wtxCpuIngoGet TCL API.
#

# globals

global availOptions			;# wtxCpuInfoGet available options
global cpuNum				;# connected CPU number

################################################################################
#
# wtxCpuInfoGetTest1 - test number of element returned by wtxCpuInfoGet
#
# SYNOPSIS
#   wtxCpuInfoGetTest1 Msg
#
# PARAMETERS
#   the message to display when entering this procedure
#
# RETURNS  N/A
#
# ERRORS  N/A
#

proc wtxCpuInfoGetTest1 {Msg} {

    global errMiscMsg			;# error messages
    global cpuNum			;# current CPU number
    global availOptions			;# wtxCpuInfoGet available options

    puts stdout "$Msg" nonewline
    set wtxCpuInfo [wtxCpuInfoGet $cpuNum]	;# get all info on current CPU

    if { $wtxCpuInfo == "" } {
	testFail $errMiscMsg(35)
	return
    }

    # test number of returned parameters

    if { [llength $wtxCpuInfo] != [llength $availOptions] } {
	testFail $errMiscMsg(36)
	return
    }

    wtxTestPass
}

################################################################################
#
# wtxCpuInfoGetTest2 - test the options separately
#
# SYNOPSIS
#   wtxCpuInfoGetTest2 Msg
#
# PARAMETERS
#   the message to display when entering this procedure
#
# RETURNS  N/A
#
# ERRORS  N/A
#

proc wtxCpuInfoGetTest2 {Msg} {

    global errMiscMsg			;# error messages
    global cpuNum			;# current CPU number
    global availOptions			;# wtxCpuInfoGet available options

    puts stdout "$Msg" nonewline
    set masterCpuInfo [wtxCpuInfoGet $cpuNum]

    foreach option $availOptions {
	set wtxCpuInfo [wtxCpuInfoGet $option $cpuNum]

	# test if returned result was in the master version

	if { ($wtxCpuInfo != "") && \
	     [lsearch $masterCpuInfo $wtxCpuInfo] == -1 } {
	    testFail $errMiscMsg(37)
	    return
	}
    }

    wtxTestPass
}

################################################################################
#
# wtxCpuInfoGetTest - calling procedure for wtxCpuInfoGetTest tests
#
# SYNOPSIS
#   wtxCpuInfoGetTest tgtsvrName timeout invalidAddr
#
# PARAMETERS
#   tgtsvrName : the name of the target server to connect to
#   timeout : the WTX connection timeout
#   invalidAddr : equal SUPPRESS if invalid addresses are not supported
#
# RETURNS  N/A
#
# ERRORS  N/A
#

proc wtxCpuInfoGetTest { tgtsvrName timeout invalidAddr } {

    global availOptions			;# wtxCpuInfoGet available options
    global cpuNum			;# connected CPU number
    global cpuInfoGetMsg		;# wtxCpuInfoGetTest messages

    if { [llength $tgtsvrName] == 0 } {
	puts stdout "usage : wtxCpuInfoGetTest targetServerName timeout \
		     invalidAddr"
	return
    }

    set serviceType 2

    # connection to the target server

    wtxToolAttachCall $tgtsvrName $serviceType $timeout
    wtxErrorHandler [lindex [wtxHandle] 0] errorHandler

    # globals initialization

    set availOptions {-a -d -l -n -o}
    set cpuNum [lindex [lindex [wtxTsInfoGet] 2] 0]	;# get current CPU num

    # call for the test procedures

    wtxCpuInfoGetTest1 $cpuInfoGetMsg(1)
    wtxCpuInfoGetTest2 $cpuInfoGetMsg(2)

    # deconnection

    wtxToolDetachCall "wtxCpuInfoGetTest"
}
