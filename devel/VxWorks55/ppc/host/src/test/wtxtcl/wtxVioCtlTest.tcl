# wtxVioCtlTest.tcl - Tcl script, test WTX_VIO_CTL
#
# Copyright 1995 Wind River Systems, Inc.
#
#
# modification history
# --------------------
# 01p,18jan99,p_b  rename wtxCommonProc.tcl in wtxtestlib.tcl
# 01o,28jan98,p_b  Adapted for WTX 2.0
# 01n,11jul96,jmp  replaced host specific flags by WTX_OPEN_FLAG flags.
# 01m,20oct95,jmp  added wtxVioCtlTest3.
#		   added invalidAddr argument to wtxVioCtlTest.
# 01l,09aug95,jmp  changed call of readContextCreate
#		   added timeout to wtxToolAttachCall in wtxVioCtlTest.
#                  changed openVio to vioOpen, emptyVio to vioEmpty.
#                  changed tests control method, now uses wtxErrorHandler.
# 01k,23jun95,f_v  added wtxTestRun call
# 01j,07jun95,f_v  updated Vio use
# 01i,22may95,jcf  name revision.
# 01h,27apr95,c_s  now uses wtxErrorMatch.
# 01g,27apr95,f_v  replace call to funcCall1 by funcCall
# 01f,13apr95,f_v  moved openVio,emptyVio,readContextCreate
#		   to wtxtestlib.tcl
# 01e,07apr95,f_v  change few error messages
# 01d,30mar95,f_v  replace S_wdb_xx by agentErr
# 01c,28mar95,f_v  added wtxRegisterForEvent
# 01d,07mar95,f_v  changed error messages
# 01c,20feb95,kpn  changed wtxMemZero -> wtxMemSet
#                  replaced wtxFuncCall() and wtxEventPoll by funcCall() or
#                  funcCall3().
#		   added tgtSvrErr and vxWorksErr, added readContextCreate{}
# 01b,13feb95,kpn  added wtxVioCtlTest2{}
# 01a,30jan95,kpn  written.
#
#

#
# DESCRIPTION
#
# WTX_VIO_CTL - perform control on virtual I/O
#
# TESTS :
#
# (1) wtxVioCtlTest1 - wtxVioCtl with valid parameters.
#
# (2) wtxVioCtlTest2 - verify that wtxVioCtl returns the error code
#	WTX_ERR_SVR_INVALID_FILE_DESCRIPTOR when it's called 
#	with an invalid <filename> argument.
#	


#*************************************************************************
#
# wtxVioCtlTest1 - wtxVioCtl with valid parameters.
#
# This test checks that wtxVioCtl service redirects a virtual I/O channel 
# to a file previously opened with wtxOpen service for reading and with
# no virtual I/O redirection. The test checks that the redirection is performed
# into the right channel and the file has been passed on entirely.
#

proc wtxVioCtlTest1 {Msg} {


 # GLOBAL VARIABLES

    # array that contains the testcase error codes
    global errVioMsg

    # array that contains the environment variables
    global env
    # control operation for wtxVioCtl
    global VIO_REDIRECT

    puts stdout "$Msg" nonewline
 

 # LOCAL VARIABLES

    # channel id for redirection
    set chanId  -1 
    set channel 0
    # mode value for wtxOpen
    set mode 0
    # max size for buffer that receives read bytes on a virtual I/O
    set readBytesMaxNum 512
    # maximum number of bytes that can be read in one time on a virtual I/O
    set maxBytesRead 512


 # The content of the wtxVioCtlTestFile file will be send to the virtual I/O.
 # When this is will be done, it will has to verify that the read command
 # on the target has received the right datas from the virtual I/O.
 # So it's the reason why we read the wtxVioCtlTestFile file and put its content
 # in a buffer : the result of the read command will be able to compare
 # to the buffer content.

    # check the WIND_BASE environment variables exists
    set findWindBaseVar [info exists env(WIND_BASE)]
    # WIND_BASE environment variable doesn't exist : display a message
    if {$findWindBaseVar == 0} {
        testFail $errVioMsg(1)
        return
    }
    # get the value of the WIND_BASE environment variable
    set windBasePath [set env(WIND_BASE)]

    # path of the file that will be opened
    if { $env(WIND_HOST_TYPE) == "x86-win32" } {
     set vioCtlTestFile "$windBasePath\\host\\resource\\test\\wtxVioCtlTestFile"
    } else {
     set vioCtlTestFile "$windBasePath/host/resource/test/wtxVioCtlTestFile"
    }

    # verify the file exists
    if {[file exists $vioCtlTestFile] == 0} {
        testFail "cannot find file: $vioCtlTestFile"
        return
    }

    # open the file "wtxVioCtlTestFile" for reading, return a file identifier
    set fileIdent [open $vioCtlTestFile r]

    # read and return all of the bytes remaining in the previously opened file
    set bytesRead [read $fileIdent]

    # create a memory block, filled with the content of the previously
    # opened file
    set bytesReadBlock [memBlockCreate -string $bytesRead]

    # get the length of read bytes block
    set bytesReadBlockLength [expr [string length $bytesRead] + 1]

    # get the value of this block to can compare it  with data read from
    # virtual I/O
    set blockReadValue [memBlockGet $bytesReadBlock]

    # free all memBlocks
    cleanUp mb $bytesReadBlock


 # OPEN VIO

    set resVioOpen [vioOpen]
    if {[lindex $resVioOpen 0] == 0} {
        puts stdout $errVioMsg(20)
        cleanUp f $fileIdent
        return
    }

    # get the file descriptor of the file associated with VIO channel,
    # necessary to can close the file at the end of the test
    set fileDescriptor [lindex $resVioOpen 0]
    set channelNb [lindex $resVioOpen 1]


 # EMPTY THE VIO

    set resVioEmpty [vioEmpty $fileDescriptor $readBytesMaxNum $maxBytesRead]
    if {[lindex $resVioEmpty 0] == 0} {
        puts stdout $errVioMsg(19)
        cleanUp f $fileIdent fc $fileDescriptor
        return
    }

    set roomDataRead [lindex $resVioEmpty 0]


 # READ DATA FROM VIO

    set targetSystemAssignedId [readContextCreate $fileDescriptor $roomDataRead]
    if {$targetSystemAssignedId == 0} {
        puts stdout $errVioMsg(24)
        cleanUp f $fileIdent fc $fileDescriptor
        return
    }


 # SEND data to VIO

    # channel parameter is set to -1 so no virtual I/O redirection are performed
    # wtxOpen opens the file $vioCtlTestFile for reading
    # wtxOpen return a descriptor for that file

    set fileDesc [wtxOpen -channel $chanId $vioCtlTestFile WTX_O_RDONLY $mode]
    if {$fileDesc == ""} {
	cleanUp c $targetSystemAssignedId f $fileIdent ms $roomDataRead\
		fc $fileDescriptor
        return
    }

    # wtxVioCtl : redirects the virtual I/O channel to the file previously 
    # opened with wtxOpen
    set resVioCtl [wtxVioCtl $channelNb $VIO_REDIRECT $fileDesc]
    if {$resVioCtl == ""} {
	cleanUp c $targetSystemAssignedId f $fileIdent ms $roomDataRead\
		vs $fileDesc fc $fileDescriptor
	return
    }

    after 1000


 # READ the data received from the VIO

    # read the bytes written at the address $roomDataRead on the target
    set blockRead [wtxMemRead $roomDataRead $bytesReadBlockLength]
    if {$blockRead == ""} {
	cleanUp f $fileIdent ms $roomDataRead vs $fileDesc fc $fileDescriptor
        return
    }

    # get the contents of this block
    set blockReadContent [memBlockGet $blockRead]


 # verify the validity of the data received from VIO

    # compare the content of the block read from the virtual I/O with
    # the initial bytes contained in the file "wtxvioCtlTestFile"

    if {$blockReadValue != $blockReadContent} {
        testFail $errVioMsg(7)
        cleanUp f $fileIdent ms $roomDataRead fc $fileDescriptor
        return
    }


 # CLOSE

    set resCloseVio [wtxClose $fileDesc]
    if {$resCloseVio == ""} {
        testFail $errVioMsg(16)
	cleanUp f $fileIdent ms $roomDataRead fc $fileDescriptor
        return
    }

    # clean up

    cleanUp f $fileIdent ms $roomDataRead fc $fileDescriptor

    wtxTestPass
}

#*************************************************************************
#
# wtxVioCtlTest2 - verify that wtxVioCtl returns the error code
#		   WTX_ERR_SVR_INVALID_FILE_DESCRIPTOR when 
#		   it's called with an invalid <file descriptor> argument.
#
#

proc wtxVioCtlTest2 {Msg} {

 # GLOBAL VARIABLES

    # array that contains the testcase error codes
    global errVioMsg

    # wtx and VxWorks error codes
    # array that contains the environment variables
    global env
    # control operation for wtxVioCtl
    global VIO_REDIRECT

 # LOCAL VARIABLES

    # channel id for redirection
    set chanId  -1
    set channel 0
    # mode value for wtxOpen
    set mode 0
    # fd value for wtxVioCtl
    set invalidFd 500
    # max size for buffer that receives read bytes on a virtual I/O
    set readBytesMaxNum 512
    # maximum number of bytes that can be read in one time on a virtual I/O
    set maxBytesRead 512

    puts stdout "$Msg" nonewline


 # wtxOpen <filename> argument

    # check the WIND_BASE environment variables exists
    catch "info exists env(WIND_BASE)" findWindBaseVar
    # WIND_BASE environment variable doesn't exist : display a message
    if {$findWindBaseVar == 0} {
        testFail $errVioMsg(1)
        return
    }

    # get the value of the WIND_BASE environment variable
    catch {set env(WIND_BASE)} windBasePath

    # path of the file used as argument for wtxopen  
    if { $env(WIND_HOST_TYPE) == "x86-win32" } {
     set vioCtlTestFile "$windBasePath\\host\\resource\\test\\wtxVioCtlTestFile"
    } else {
     set vioCtlTestFile "$windBasePath/host/resource/test/wtxVioCtlTestFile"
    }

    # verify the file exists
    if {[file exists $vioCtlTestFile] == 0} {
        testFail "cannot find file: $vioCtlTestFile"
        return
    }


 # OPEN VIO

    set resVioOpen [vioOpen]

    if {[lindex $resVioOpen 0] == 0} {
        puts stdout $errVioMsg(20)
        return
    }

    # get the file descriptor of the file associated with VIO channel,
    # necessary to can close the file at the end of the test
    set fileDescriptor  [lindex $resVioOpen 0]
    set chanNb  [lindex $resVioOpen 1]


 # SEND data to VIO

    # channel parameter is set to -1 so no virtual I/O redirection are performed
    # wtxOpen opens the file $vioCtlTestFile for reading
    # wtxOpen return a descriptor for that file

    if {[catch "wtxOpen -channel $chanId $vioCtlTestFile WTX_O_RDONLY $mode"\
	fileDesc]} {
        testFail $errVioMsg(2)
	cleanUp fc $fileDescriptor
        return
    }

    # call wtxVioCtl with invalid <fileDescriptor>

    # if no exception occurred
    if {![catch "wtxVioCtl $chanNb $VIO_REDIRECT $invalidFd " resVioCtl]} {
	testFail $errVioMsg(26)
	cleanUp fc $fileDescriptor vs $fileDesc
	return
	}

    # if the caught exception is different than the expected error code
    if {![wtxErrorMatch $resVioCtl SVR_INVALID_FILE_DESCRIPTOR]} {
        testFail $resVioCtl
	cleanUp fc $fileDescriptor vs $fileDesc
        return
    }


 # CLOSE

    if {[catch "wtxClose $fileDesc" resCloseVio]} {
        testFail $errVioMsg(16)
	cleanUp fc $fileDescriptor
        return
	}

    # close the virtual I/O file associated with virtual I/O channel 0
    cleanUp fc $fileDescriptor

    wtxTestPass
}


#*************************************************************************
#
# wtxVioCtlTest3 - verify that wtxVioCtl returns the error code EINVAL
#	when it's called with an invalid <VIO_CTL_REQUEST> argument.
#
#

proc wtxVioCtlTest3 {Msg} {

 # GLOBAL VARIABLES

    # array that contains the testcase error codes
    global errVioMsg

    # wtx and VxWorks error codes
    # array that contains the environment variables
    global env
    # control operation for wtxVioCtl
    global VIO_REDIRECT

 # LOCAL VARIABLES

    # channel id for redirection
    set chanId  -1
    set channel 0
    # mode value for wtxOpen
    set mode 0
    # fd value for wtxVioCtl
    set invalidFd 500
    # max size for buffer that receives read bytes on a virtual I/O
    set readBytesMaxNum 512
    # maximum number of bytes that can be read in one time on a virtual I/O
    set maxBytesRead 512

    puts stdout "$Msg" nonewline


    # check the WIND_BASE environment variables exists
    catch "info exists env(WIND_BASE)" findWindBaseVar
    # WIND_BASE environment variable doesn't exist : display a message
    if {$findWindBaseVar == 0} {
        testFail $errVioMsg(1)
        return
    }

    # get the value of the WIND_BASE environment variable
    catch {set env(WIND_BASE)} windBasePath

    # path of the file used as argument for wtxopen  
    if { $env(WIND_HOST_TYPE) == "x86-win32" } {
     set vioCtlTestFile "$windBasePath\\host\\resource\\test\\wtxVioCtlTestFile"
    } else {
     set vioCtlTestFile "$windBasePath/host/resource/test/wtxVioCtlTestFile"
    }

    # verify the file exists
    if {[file exists $vioCtlTestFile] == 0} {
        testFail "cannot find file: $vioCtlTestFile"
        return
    }


 # OPEN VIO

    set resVioOpen [vioOpen]

    if {[lindex $resVioOpen 0] == 0} {
        puts stdout $errVioMsg(20)
        return
    }

    # get the file descriptor of the file associated with VIO channel,
    # necessary to can close the file at the end of the test
    set fileDescriptor  [lindex $resVioOpen 0]
    set chanNb  [lindex $resVioOpen 1]


 # SEND data to VIO

    # channel parameter is set to -1 so no virtual I/O redirection are performed
    # wtxOpen opens the file $vioCtlTestFile for reading
    # wtxOpen return a descriptor for that file

    if {[catch "wtxOpen -channel $chanId $vioCtlTestFile WTX_O_RDONLY $mode"\
	fileDesc]} {
        testFail $errVioMsg(2)
	cleanUp fc $fileDescriptor
        return
    }

    # call wtxVioCtl with invalid <VIO_CTL_REQUEST> argument
    set invalidRequest 1000

    # if no exception occurred
    if {![catch "wtxVioCtl $chanNb $invalidRequest $fileDesc" resVioCtl]} {
	testFail $errVioMsg(27)
	cleanUp fc $fileDescriptor vs $fileDesc
	return
	}

    # if the caught exception is different than the expected error code
    if {![wtxErrorMatch $resVioCtl SVR_EINVAL]} {
        testFail $resVioCtl
	cleanUp fc $fileDescriptor vs $fileDesc
        return
    }


 # CLOSE

    if {[catch "wtxClose $fileDesc" resCloseVio]} {
        testFail $errVioMsg(16)
	cleanUp fc $fileDescriptor
        return
	}

    # close the virtual I/O file associated with virtual I/O channel 0
    cleanUp fc $fileDescriptor

    wtxTestPass
}


#*************************************************************************
#
# wtxVioCtlTest - main procedure
#
# The main procedure initializes variables that will be used by the test's
# testcase units, it makes accessible the array with the description of the
# testcase units and calls each testcase procedure contained in the test script.
# Furthermore it attaches itself to the target server (thru a wtxToolAttachCall
# command) and, when testcase units have been run, detaches itself from the
# target server.
#

proc wtxVioCtlTest {tgtsvrName timeout invalidAddr} {

    if { [llength $tgtsvrName] == 0 } {
        puts stdout "usage : wtxVioCtlTest targetServerName\
			timeout invalidAddr"
        return
    }

    # tests description
    global vioCtlMsg

    # call valid tests 
    wtxToolAttachCall $tgtsvrName 2 $timeout
    # enable all events
    catch {wtxRegisterForEvent .*} st

    wtxErrorHandler [lindex [wtxHandle] 0] errorHandler

    wtxVioCtlTest1 $vioCtlMsg(1)

    wtxErrorHandler [lindex [wtxHandle] 0] ""

    # call invalid tests
    wtxVioCtlTest2 $vioCtlMsg(2)
    wtxVioCtlTest3 $vioCtlMsg(3)

    # detach from the target server
    wtxToolDetachCall "\nwtxVioCtlTest : "
}
