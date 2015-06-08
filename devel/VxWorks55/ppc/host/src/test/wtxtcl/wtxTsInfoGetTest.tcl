# wtxTsInfoGetTest.tcl - Tcl script, test WTX_TS_INFO_GET 
#
# Copyright 1995 Wind River Systems, Inc.
#
#
# modification history
# --------------------
# 01h,28jan98,p_b  Adapted for WTX 2.0
# 01g,17sep96,jmp  canceled 01e & 01f modifications, back to version 01d.
# 01f,16sep96,wmd  typo in previous fix caused fixed to fail, so spr #7131
#                  was filed against it, condition test should be !=.
# 01e,28jun96.jmp  modified wtxTsInfoGetTest1 that may fail on x86-win32
#		   arch if using a core file located on a Unix host.
# 01d,20oct95,jmp  added invalidAddr argument to wtxTsInfoGetTest.
# 01c,21aug95,jmp  removed unreliables tests (user & memory size).
#		   added timeout to wtxToolAttachCall in wtxTsInfoGetTest.
#                  changed tests control method, now uses wtxErrorHandler.
# 01b,23jun95,f_v  added wtxTestRun call
# 01a,15may95,f_v  written.
#


#
# DESCRIPTION
#
# WTX_TS_INFO_GET - Inquire target and target server Information
#
# WARNING : This test check only if wtxTsInfoGet returns the
# 	fourteen informations parameters, and if the operating
#	system filename given by wtxTsInfoGet exists.
#


#****************************************************************************
#
# wtxTsInfoGetTest1 - Check the parameters returned by wtxTsInfoGet
#
# This test check if wtxTsInfoGet returns fourteen parameters,
# and if the operating system filename given by wtxTsInfoGet exists.
#

proc wtxTsInfoGetTest1 {Msg} {

    global errSessionMsg

    puts stdout "$Msg" nonewline

    # get general information
    set geneInfo [wtxTsInfoGet]

    # get length of information
    set infoLenght [llength $geneInfo]

    # check number of return argument
    if { $infoLenght == 14} { 
         testFail $errSessionMsg(4)
         return
     }

    # check if the operating system filename given by wtxTsInfoGet exists

    # get core file name.
    set fileInfo [lindex [wtxTsInfoGet] 5]
    set index [expr [string first : $fileInfo] + 1]
    set fileName [string range $fileInfo $index end]

    if {[file exist $fileName] != 1} {
	testFail $errSessionMsg(4)
	return
     }

    wtxTestPass
}


#****************************************************************************
#
# wtxTsInfoGetTest -
#
# Initialization of variables.
# Initialization of variables that contain the description
# of the tests.
# Call the test scripts.
#

proc wtxTsInfoGetTest {tgtsvrName timeout invalidAddr} {

    global tsInfoGetMsg

    if { [llength $tgtsvrName] == 0 } {
        puts stdout "usage : wtxTsInfoGetTest targetServerName\
				timeout invalidAddr"
        return
    }

    # variables initializations
    set serviceType 2

    # connection to the target server
    wtxToolAttachCall $tgtsvrName $serviceType $timeout
    wtxErrorHandler [lindex [wtxHandle] 0] errorHandler

    wtxTsInfoGetTest1 $tsInfoGetMsg(1)

    # deconnection
    wtxToolDetachCall "wtxTsInfoGetTest"
}
