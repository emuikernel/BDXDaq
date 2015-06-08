# wtxCloseTest.tcl - Tcl script, test WTX_CLOSE
#
# Copyright 1995 Wind River Systems, Inc.
#
#
# modification history
# --------------------
# 01j,28jan98,p_b  Adapted for WTX 2.0
# 01i,11jul96,jmp  replaced host specific flags by WTX_OPEN_FLAG flags.
# 01h,20oct95,jmp  added invalidAddr argument to wtxCloseTest.
# 01g,09aug95,jmp  added timeout to wtxToolAttachCall in wtxCloseTest.
#                  changed tests control method.
# 01f,23jun95,f_v  added wtxTestRun call
# 01e,21jun95,f_v  clean up main procedure
# 01d,22may95,jcf  name revision.
# 01c,28apr95,c_s  now uses wtxErrorMatch.
# 01b,07apr95,f_v  changed few error messages
# 01b,22feb95,kpn  changed S_wtx_INVALID_FD -> 
#		   WTX_ERR_TGTSVR_INVALID_FILE_DESCRIPTOR 
#		   added  tgtSvrErr and vxWorksErr
# 01a,31jan95,kpn  written.
#
#

#
# DESCRIPTION
#
# WTX_CLOSE - close a file or pipe
#
# TESTS :
#
# (1) wtxCloseTest1 - Verify wtxClose with valid file descriptor.
#
# (2) wtxCloseTest2 - Verify wtxClose with valid pipe descriptor.
#
# (3) wtxCloseTest3 - Verify wtxClose with invalid argument
#	The third test verifies the result of wtxClose when it's called
#	with an invalid file descriptor, the result must correspond to the 
#	WTX_ERR_TGTSVR_INVALID_FILE_DESCRIPTOR error code.
#


#*************************************************************************
#
# wtxCloseTest1 - verify wtxClose with valid file descriptor.
#
# This testcase opens a file for virtual I/O, for reading and with no
# virtual  I/O  redirection. The goal of this test is to verify that 
# wtxClose service allows to close the opened file and so to make 
# unavailable the file descriptor obtained at the time of the file opening.
#

proc wtxCloseTest1 {Msg} {


 # GLOBAL VARIABLES

    # array that contains the testcase error codes
    global errVioMsg
    # array that contains the environment variables
    global env

    puts stdout "$Msg" nonewline

 # LOCAL VARIABLES

    # channel id for redirection
    set chanId -1
    # mode value for wtxOpen
    set mode 0

    # check the WIND_BASE environment variables exists
    catch "info exists env(WIND_BASE)" findWindBaseVar

    # WIND_BASE environment variable doesn't exist : display a message
    if {$findWindBaseVar == 0} {
        testFail $errVioMsg(1)
        return
    }
    # get the value of the WIND_BASE environment variable
    catch {set env(WIND_BASE)} windBasePath

    # path of the file that will be opened
    set OpenTestFile "$windBasePath/host/resource/test/vioTestFile"
    # verify the file exists
    if {[file exists $OpenTestFile] == 0} {
        testFail "cannot find file: $OpenTestFile"
        return
    }

    # channel is set to -1 so no  virtual  I/O  redirection are performed,
    # wtxOpen only opens the file $OpenTestFile for reading
    # wtxOpen return a descriptor for that file
    if {[catch "wtxOpen -channel $chanId $OpenTestFile WTX_O_RDONLY $mode"\
    fileDescriptor]} {
	testFail $fileDescriptor
        return
    }

    # verify wtxClose with valid file descriptor

    # close the file specified by the file descriptor returned by wtxOpen
    if {[catch "wtxClose $fileDescriptor" resClose]} {
	testFail $resClose
        return
    }

    # wtxClose result must be equal to zero
    if {$resClose != 0} {
        testFail $errVioMsg(9)
	return
    }

    # verify wtxClose has closed the file and so that the file descriptor
    # doesn't exist any more

    # if no exception occurred
    if {![catch "wtxClose $fileDescriptor" resClose2]} {
	testFail $errVioMsg(10)
	return
	}

    # if the caught exception is different than the expected error code
    if {![wtxErrorMatch $resClose2 SVR_INVALID_FILE_DESCRIPTOR]} {
        testFail $resClose2
        return
    }
 
    # if the caught exception is the expected error code
    wtxTestPass
}

#*************************************************************************
#
# wtxCloseTest2 - verify wtxClose with valid pipe descriptor
#
# This testcase opens a pipe for virtual I/O, for reading and with no
# virtual  I/O  redirection. The goal of this test is to verify that
# wtxClose service allows to close the opened pipe and so to make
# unavailable the file descriptor obtained at the time of the pipe opening.
#

proc wtxCloseTest2 {Msg} {


 # GLOBAL VARIABLES

    # array that contains the testcase error codes
    global errVioMsg

    # array that contains the environment variables
    global env


    puts stdout "$Msg" nonewline

 # LOCAL VARIABLES

    # channel id for redirection
    set chanId -1
    # mode value for wtxOpen
    set mode 0


    # channel is set to -1 so no  virtual  I/O  redirection are performed,
    # wtxOpen only opens the pipe for reading
    # wtxOpen return a descriptor for that pipe 
    if {[catch "wtxOpen -channel $chanId |ls WTX_O_RDONLY $mode" fileDescriptor]} {
        testFail $fileDescriptor
        return
    }

    # verify wtxClose with valid file descriptor

    # close the pipe specified by the file descriptor returned by wtxOpen
    if {[catch "wtxClose $fileDescriptor" resClose]} {
        testFail $resClose
        return
    }

    # wtxClose result must be equal to zero
    if {$resClose != 0} {
        testFail $errVioMsg(9)
        return
    }

    # verify wtxClose has closed the pipe and so that the file descriptor
    # doesn't exist any more

    # if no exception occurred
    if {![catch "wtxClose $fileDescriptor" resClose2]} {
	testFail $errVioMsg(11)
	return
	}

    # if the caught exception is different than the expected error code
    if {![wtxErrorMatch $resClose2 SVR_INVALID_FILE_DESCRIPTOR]} {
        testFail $resClose2
        return
    }

    # if the caught exception is the expected error code
    wtxTestPass
}


#*************************************************************************
#
# wtxCloseTest3 - verify wtxClose with invalid argument
#
# This test verifies the result of wtxClose when it's called with an
# invalid file descriptor, the result must correspond to the 
# WTX_ERR_TGTSVR_INVALID_FILE_DESCRIPTOR error code.
#

proc wtxCloseTest3 {Msg} {

 # GLOBAL VARIABLES

    # array that contains the testcase error codes
    global errVioMsg

    puts stdout "$Msg" nonewline

 # LOCAL VARIABLES

    set invalidFd 555

    # if no exception occurred
    if {![catch "wtxClose $invalidFd" resClose]} {
	testFail $errVioMsg(26)
	return
	}

    # if the caught exception is different than the expected error code
    if {![wtxErrorMatch $resClose SVR_INVALID_FILE_DESCRIPTOR]} {
        testFail $resClose
        return
    }

    # if the caught exception is the expected error code
    wtxTestPass
}

#*************************************************************************
#
# wtxCloseTest -
#
# The main procedure initializes variables that will be used by the test's
# testcase units, it makes accessible the array with the description of the
# testcase units and calls each testcase procedure contained in the test script.
# Furthermore it attaches itself to the target server (thru a wtxToolAttachCall
# command) and, when testcase units have been run, detaches itself from the
# target server.
#

proc wtxCloseTest {tgtsvrName timeout invalidAddr} {

    if { [llength $tgtsvrName] == 0 } {
        puts stdout "usage : wtxCloseTest targetServerName\
				timeout invalidAddr"
        return
    }

    global closeMsg

    # connection to the target server
    wtxToolAttachCall $tgtsvrName 2 $timeout

    # call the testcase units
    wtxCloseTest1 $closeMsg(1)
    wtxCloseTest2 $closeMsg(2)
    wtxCloseTest3 $closeMsg(3)

    # detach from the target server
    wtxToolDetachCall "\nwtxCloseTest : "
}
