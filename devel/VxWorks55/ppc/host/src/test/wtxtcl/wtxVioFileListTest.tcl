# wtxVioFileListTest.tcl - Tcl script, test WTX_VIO_FILE_LIST
#
# Copyright 1995 Wind River Systems, Inc.
#
#
# modification history
# --------------------
# 01k,28jan98,p_b  Adapted for WTX 2.0
# 01j,25jun96,jmp  replaced host specific flags by WTX_OPEN_FLAG flags.
# 01i,20oct95,jmp  added invalidAddr argument to wtxVioFileListTest.
# 01h,21aug95,jmp  fixed bugs in call of vioClose without arguments
#		   added timeout to wtxToolAttachCall in wtxVioFileListTest.
#                  changed openVio to vioOpen, closeVio to vioClose.
#                  changed tests control method, now uses wtxErrorHandler.
# 01g,26jun95,f_v  added ENOENT test in wtxVioFileListTest3
# 01f,23jun95,f_v  added wtxTestRun call
# 01e,23jun95,f_v  updated wtxVioFileListTest3
# 01d,16jun95,f_v  updated vio uses,added wtxRegisterForEvent
# 01c,22may95,jcf  name revision.
# 01b,27apr95,c_s  now uses wtxErrorMatch.
# 01a,07apr95,f_v  change errVioMsg(3) by errMemMsg.
# 01a,22mar95,f_v  written.
#
#

#
# DESCRIPTION
#
# WTX_VIO_FILE_LIST - list the files managed by the target server
#
# This call returns the following information about all Virtual I/O associated
# files:
#
# TESTS :
#
# (1) wtxVioFileListTest1 - check if wtxVioFileList returns valid parameters
#     			    with no redirection then redirection. use file
# (2) wtxVioFileListTest2 - check if wtxVioFileList returns valid parameters.
#			    with no redirection. use pipe
# (3) wtxVioFileListTest3 - check if wtxVioFileList returns valid parameters.
#			    with redirection. use new file
#
#	

#*************************************************************************
#
# wtxVioFileListTest1 - wtxVioFileList check returned parameters 
#
# This test checks if wtxVioFileList returns right parameters
# 
# ex : 
#
# {/dev/vio/console 10 0 0 0 2 1 6276}
#        |           | | | | | |  |
#        |           | | | | | |  +---- process id (VIO console only)
#        |           | | | | | +------- status (OPEN_FILE (1) / CLOSED_FILE (0))
#        |           | | | | +--------- open flags (O_RDONLY/O_WRONLY etc...)
#        |           | | | +----------- file type (IO_FD (0) for file or socket
#        |           | | |              PIPE_FD(1) for pipe)
#        |           | | +------------- file pointer (optional)
#        |           | +--------------- VIO channel (-1 mean no redirection)
#        |           +----------------- file descriptor
#        +----------------------------- filename
#
# In this test wtxOpen is called with these parameters :
# <filename> : the name of an existing file here - wtxVioCtlTestFile -
# <flags>    : WTX_O_RDONLY (open for reading only)
# <mode >    : 0
# <channel>  : -1 (no  virtual  I/O  redirection)
#

proc wtxVioFileListTest1 {Msg} {


 # GLOBAL VARIABLES

    # array that contains the testcase error codes
    global errVioMsg
    # array that contains the environment variables
    global env
    # control operation for wtxVioCtl
    global VIO_REDIRECT


    puts stdout "$Msg" nonewline
 

 # LOCAL VARIABLES

    # channel id for no redirection
    set chanId  -1 

 # OPEN VIO
    set resVioOpen [vioOpen]
    if {[lindex $resVioOpen 0] == 0} {
        testFail $errVioMsg(20)
        return
        }

    set fileDescriptor [lindex $resVioOpen 0]
    set channel [lindex $resVioOpen 1]
    
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
	vioClose $fileDescriptor $channel
        testFail $errVioMsg(1)
        return
    }

    # path of the file that will be opened
    if { $env(WIND_HOST_TYPE) == "x86-win32" } {
     set vioCtlTestFile\
	 "$env(WIND_BASE)\\host\\resource\\test\\wtxVioCtlTestFile"
    } else {
     set vioCtlTestFile "$env(WIND_BASE)/host/resource/test/wtxVioCtlTestFile"
    }

 # OPEN file for VIO

    # channel parameter is set to -1 so no virtual I/O redirection are performed
    # wtxOpen opens the file $vioCtlTestFile for reading
    # wtxOpen return a descriptor for that file

    set fileDesc [wtxOpen -channel $chanId $vioCtlTestFile WTX_O_RDONLY $mode]
    if {$fileDesc == ""} {
	vioClose $fileDescriptor $channel
        testFail $errVioMsg(2)
        return
    }

    set infoGlob [wtxVioFileList]

    foreach a $infoGlob {
    set c [lsearch $a $vioCtlTestFile]
    if { $c != -1 } {
	set info  $a
       }
    }

    set fileName [lindex $info 0]
    set fd [lindex $info 1]

    # like there isn't any redirection this value shoudbe -1
    set rtChannel [lindex $info 2]

    # should be 0 - file
    set type [lindex $info 4]

    # like the file is open this value should be 1
    set rtStatus [lindex $info 6]


    if { ($vioCtlTestFile != $fileName) || ($fd != $fileDesc) ||
        ($rtChannel != -1) || ($type != 0) || ($rtStatus != 1) } {
	set resCloseVio [wtxClose $fileDesc]
	vioClose $fileDescriptor $channel
	testFail $errVioMsg(25)
	return
        }


    # wtxVioCtl : redirects the virtual I/O channel to the file previously 
    # opened with wtxOpen
    set resVioCtl [wtxVioCtl $channel $VIO_REDIRECT $fileDesc]

    if {$resVioCtl == ""} {
	set resCloseVio [wtxClose $fileDesc]
	vioClose $fileDescriptor $channel
	return
    }

    after 1000

    set infoGlob [wtxVioFileList]

    foreach a $infoGlob {
    set c [lsearch $a $vioCtlTestFile]
    if { $c != -1 } {
	set info  $a
       }
    }

    set fileName [lindex $info 0]
    set fd [lindex $info 1]

    # like there is a redirection this value should be channel number
    set rtChannel [lindex $info 2]

    # should be 0 - file
    set type [lindex $info 4]

    # like the file is close this value should be 0
    set rtStatus [lindex $info 6]


    if { ($vioCtlTestFile != $fileName) || ($fd != $fileDesc) ||\
        ($rtChannel != $channel) || ($type != 0) || ($rtStatus != 0) } {
        set resCloseVio [wtxClose $fileDesc]
	vioClose $fileDescriptor $channel
        testFail $errVioMsg(25) 
        return
        }


 # CLOSE

    vioClose $fileDescriptor $channel

    set resCloseVio [wtxClose $fileDesc]
    if {$resCloseVio == ""} {
        return
    }

    wtxTestPass
}

#*************************************************************************
#
# wtxVioFileListTest2 - wtxVioFileList check returned parameters
#
# This test checks if wtxVioFileList returns right parameters
#
# 
# In this test wtxOpen is called with these parameters :
# <filename> : result of the `date` unix command
# <flags>    : WTX_O_RDONLY (open for reading only)
# <mode >    : 0
# <channel>  : -1 (no  virtual  I/O  redirection)
#

proc wtxVioFileListTest2 {Msg} {


 # GLOBAL VARIABLES

    # array that contains the testcase error codes
    global errVioMsg
    # array that contains the environment variables
    global env

    puts stdout "$Msg" nonewline

 # LOCAL VARIABLES

    # channel id for no redirection
    set chanId  -1
    set channel 0
    # mode value for wtxOpen
    set mode 0
    # max size for buffer that receives read bytes on a virtual I/O
    set readBytesMaxNum 512
    # maximum number of bytes that can be read in one time on a virtual I/O
    set maxBytesRead 512

 # OPEN with NO VIO redirection

    # channel parameter is set to -1 so no virtual I/O redirection are performed
    # wtxOpen opens the file $vioCtlTestFile for reading
    # wtxOpen return a descriptor for that file

    set fileDesc [wtxOpen -channel $chanId |date WTX_O_RDONLY $mode]
    if {$fileDesc == ""} {
        return
    }

    set infoGlob [wtxVioFileList]

    foreach a $infoGlob {
    set c [lsearch $a |date]
    if { $c != -1 } {
        set info  $a
       }
    }

    set fileName [lindex $info 0]
    set fd [lindex $info 1]

    # like there isn't any redirection this value shoudbe -1
    set rtChannel [lindex $info 2]

    # shuld be 1 - pipe
    set type [lindex $info 4]

    set flags [lindex $info 5]

    # like the file is open this value should be 1
    set rtStatus [lindex $info 6]


    if { ("|date" != $fileName) || ($fd != $fileDesc) ||\
        ($rtChannel != -1) || ($type != 1) || ($rtStatus != 1) } {
        set resCloseVio [wtxClose $fileDesc]
        testFail $errVioMsg(25)
        return
        }

 # CLOSE

    set resCloseVio [wtxClose $fileDesc]
    if {$resCloseVio == ""} {
        return
    }

    wtxTestPass
}


#*************************************************************************
#
# wtxVioFileListTest3 - wtxVioFileList check returned parameters
#
# This test checks if wtxVioFileList returns right parameters
#
# In this test wtxOpen is called with these parameters :
# <filename> : the name new file is - Test[pid] -
# <flags>    : WTX_O_RDONLY (open for reading only)
# <mode >    : 0
# <channel>  : 0 (virtual  I/O  redirection)
#

proc wtxVioFileListTest3 {Msg} {


 # GLOBAL VARIABLES

    # array that contains the testcase error codes
    global errVioMsg
    # array that contains the environment variables
    global env

    puts stdout "$Msg" nonewline



 # LOCAL VARIABLES

    # channel id for redirection
    #set chanId  0
    set channel 0
    # mode value for wtxOpen
    set mode 0
    # max size for buffer that receives read bytes on a virtual I/O
    set readBytesMaxNum 512
    # maximum number of bytes that can be read in one time on a virtual I/O
    set maxBytesRead 512


    # check the WIND_BASE environment variables exists
    set findWindBaseVar [info exists env(WIND_BASE)]

    # WIND_BASE environment variable doesn't exist : display a message
    if {$findWindBaseVar == 0} {
        testFail $errVioMsg(1)
        return
    }

    # path of the file that will be opened
    if { $env(WIND_HOST_TYPE) == "x86-win32" } {
    	set tmpFile "$env(TMP)\\Test[pid]"
    } else {
        set tmpFile "/tmp/Test[pid]"
    }

 # OPEN VIO
    set resVioOpen [vioOpen]
    if {[lindex $resVioOpen 0] == 0} {
        puts stdout $errVioMsg(20)
        return
        }

    set fileDescriptor [lindex $resVioOpen 0]
    set chanId [lindex $resVioOpen 1]


 # OPEN with VIO redirection

    # channel parameter is set to 0 so virtual I/O redirection are performed
    # wtxOpen create the file tmpFile 
    # wtxOpen return a descriptor for that file

    set fileDesc [wtxOpen -channel $chanId $tmpFile WTX_O_CREAT $mode]

    if {$fileDesc == ""} {
	vioClose $fileDescriptor $chanId
        return
    }

    after 2000
    set infoGlob [wtxVioFileList]
    if {$infoGlob == {}} {
	vioClose $fileDescriptor $chanId
        testFail "ERROR : No open Vio"
        return
    }


    foreach a $infoGlob {
    set c [lsearch $a $tmpFile]
    if { $c != -1 } {
        set info  $a
       }
    }

    set fileName [lindex $info 0]
    set fd [lindex $info 1]

    set rtChannel [lindex $info 2]

    # should be 0 - file
    set type [lindex $info 4]

    set flags [lindex $info 5]

    # like the file is close this value should be 0
    set rtStatus [lindex $info 6]


    if { ($tmpFile != $fileName) || ($fd != $fileDesc) ||\
        ($rtChannel != $chanId) || ($type != 0) || ($rtStatus != 0) } {
        set resCloseVio [wtxClose $fileDesc]
	vioClose $fileDescriptor $chanId
        testFail $errVioMsg(25)
        return
        }

 # CLOSE

    vioClose $fileDescriptor $chanId
    set resCloseVio [wtxClose $fileDesc]
    if {$resCloseVio == ""} {
        return
    }

    wtxTestPass
}


#*************************************************************************
#
# wtxVioFileListTest - main procedure
#
# The main procedure initializes variables that will be used by the test's
# testcase units, it makes accessible the array with the description of the
# testcase units and calls each testcase procedure contained in the test script.
# Furthermore it attaches itself to the target server (thru a wtxToolAttachCall
# command) and, when testcase units have been run, detaches itself from the
# target server.
#

proc wtxVioFileListTest {tgtsvrName timeout invalidAddr} {

    if { [llength $tgtsvrName] == 0 } {
        puts stdout "usage : wtxVioFileListTest targetServerName\
				timeout invalidAddr"
        return
    }

    global vioFileList

    # connection to the target server
    wtxToolAttachCall $tgtsvrName 2 $timeout
    # enabled events
    catch {wtxRegisterForEvent .*} st

    wtxErrorHandler [lindex [wtxHandle] 0] errorHandler

    # call the testcase units 
    wtxVioFileListTest1 $vioFileList(1)
    wtxVioFileListTest2 $vioFileList(2)
    wtxVioFileListTest3 $vioFileList(3)

    # detach from the target server
    wtxToolDetachCall "\nwtxVioFileListTest : "
}
