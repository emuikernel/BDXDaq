# wtxLoggingTest.tcl - Tcl script, test WTX_LOGGING
#
# Copyright 1998 Wind River Systems, Inc.
#
#
# modification history
# --------------------
# 01c,27may98,p_b  WTX 2.0 new changes
# 01b,20may98,p_b  change loop value in Test5
# 01a,28jan98,p_b  written
#
 
 
#
# DESCRIPTION
#
# WTX_LOGGING - Test program for wtxLogging. This test checks the behaviour
#               of WTX & WDB log files. Note that this service is just a API
#               but doesn't match to any WTX function.
#
 
#****************************************************************************
#
# wtxLoggingTest1 - Checks wtx Log file creation
#
#
 
proc wtxLoggingTest1 {fileSize Msg} {
    global errMiscMsg
 
    puts stdout "$Msg" nonewline

    set fileNameLogWTX [tclPathGet]wtxLog

    if { [file exists $fileNameLogWTX] == 1 } {
        file delete $fileNameLogWTX
    }

    # Tests if the file system is writable
    set fileTest [open $fileNameLogWTX w 0666]
    puts $fileTest "_"
    close $fileTest
  
    if { [file exists $fileNameLogWTX] == 1 } {
	file delete $fileNameLogWTX
    } else {
	testFail "File system isn't writable"
	return 
    }
 
    if [catch {wtxLogging wtx on $fileNameLogWTX $fileSize}] {
     testFail "\tCan't run wtxLogging with wtx, on, $fileSize, $fileNameLogWTX\
						   parameters"
     return
    }

    # The following two lines update the file system.
    # Necessary when two identical views are opened on different hosts, when
    # you use Rational Clearcase.

    cd [tclPathGet]
    file dirname .

    if { [file exists $fileNameLogWTX] == 0 } {
       testFail "\tFile $fileNameLogWTX is missing"
       return 0
    }
 
    wtxTestPass
}
 
#****************************************************************************
#
# wtxLoggingTest4 - Checks wtx log file size 
#                   Fills log file by using wtxMemInfoGet
#
 
proc wtxLoggingTest4 {fileSize Msg} {
    global errMiscMsg
 
    puts stdout "$Msg" nonewline
 
    set fileNameLogWTX [tclPathGet]wtxLog
 
    if { [file exists $fileNameLogWTX] == 0 } {
       testFail "\tFile $fileNameLogWTX is missing"
       return 0
    }
 
    # Send wtx commands
    for {set i 0} {$i < 100} {incr i 1} {
     set resMemInfoGet [wtxMemInfoGet]
    }

    # The following two lines update the file system.
    # Necessary when two identical views are opened on different hosts, when
    # you use Rational Clearcase.

    cd [tclPathGet]
    file dirname .

    # 5000 is a delta (necessary because WTX request are not truncated
    if { [file size $fileNameLogWTX] >= [expr $fileSize + 5096] } {
       testFail "File $fileNameLogWTX is greater\
	: [file size $fileNameLogWTX] than the expected limit, $fileSize"
       return 0
    }
 
    wtxTestPass
}

#****************************************************************************
#
# wtxLoggingTest2 - Checks wdb log file creation
#
#
 
proc wtxLoggingTest2 {fileSize Msg} {
    global errMiscMsg
 
    puts stdout "$Msg" nonewline
 
    set fileNameLogWDB [tclPathGet]wdbLog
 
    if { [file exists $fileNameLogWDB] == 1 } {
        file delete $fileNameLogWDB
    }
 
    # Tests if the file system is writable
    set fileTest [open $fileNameLogWDB w 0666]
    puts $fileTest "_"	
    close $fileTest
 
    if { [file exists $fileNameLogWDB] == 1 } {
        file delete $fileNameLogWDB
    } else {
        testFail "File system isn't writable"        
        return 
    }

    if [catch {wtxLogging wdb on $fileNameLogWDB $fileSize}] {
     testFail "\tCan't run wtxLogging with wdb,on,$fileSize,$fileNameLogWDB\
	parameters"
     return
    }

    # The following two lines update the file system.
    # Necessary when two identical views are opened on different hosts, when
    # you use Rational Clearcase.

    cd [tclPathGet]
    file dirname .
 
    if { [file exists $fileNameLogWDB] == 0 } {
        testFail "\tFile $fileNameLogWDB is missing"
        return 0 
    }
 
    wtxTestPass
}

#****************************************************************************
#
# wtxLoggingTest5 - Check wdb log file size
#                   Fills log file by using wtxFuncCall. 
#
 
proc wtxLoggingTest5 {fileSize Msg} {
    global errMiscMsg
    global USE_TSFORLOAD
 
    puts stdout "$Msg" nonewline
 
    set fileNameLogWDB [tclPathGet]wdbLog
 
    if { [file exists $fileNameLogWDB] == 0 } {
       testFail "\tFile $fileNameLogWDB is missing"
       return 0
    }
 
    # Send wdb commands by calling "wtxFuncCall" one hundred times.
    set status [wtxObjModuleLoad $USE_TSFORLOAD LOAD_GLOBAL_SYMBOLS\
       [objPathGet test]funcCallTest.o]
    set resFind [wtxSymFind -name "funcCallIntTest"]
    if {$resFind == ""} {
        cleanUp o $resObjModuleLoad
        return
    }
    set taskAdd [lindex $resFind 1]

    for {set i 0} {$i < 50} {incr i 1} { 
     # call this routine on the target, execute it in the context of the WDB
     # server, returns the context id of the routine.
     set resFuncCall [wtxFuncCall $taskAdd 1 2 3 4 5 6 7 8 9 10]
    }

    # clean up
    wtxObjModuleUnload [objPathGet test]funcCallTest.o

    # The following two lines update the file system.
    # Necessary when two identical views are opened on different hosts, when
    # you use Rational Clearcase.

    cd [tclPathGet]
    file dirname . 

    if { [file size $fileNameLogWDB] >= [expr $fileSize + 5096] } { 
       testFail "File $fileNameLogWDB is greater :\
       	[file size $fileNameLogWDB]\
       		than the expected limit, $fileSize"
       return 0
    }
 
    wtxTestPass
}

#****************************************************************************
#
# wtxLoggingTest3 - Check wtxLogging with "all off" parameters
#
#
 
proc wtxLoggingTest3 {Msg} {
    global errMiscMsg
 
    set fileNameLogWDB [tclPathGet]wdbLog
    set fileNameLogWTX [tclPathGet]wtxLog

    puts stdout "$Msg" nonewline
 
    if [catch {wtxLogging all off}] {
     testFail "Error, can't execute wtxLogging with all and off"
     return
    }

    if { [file exists $fileNameLogWTX] == 1 } {
        file delete $fileNameLogWTX
    }

    if { [file exists $fileNameLogWDB] == 1 } {
        file delete $fileNameLogWDB
    }

    wtxTestPass
}

#****************************************************************************
#
# wtxLoggingTest - Main procedure
#
# Initialization of variables.
# Initialization of variables that contain the description
# of the tests.
# Call the test scripts.
#
 
proc wtxLoggingTest {tgtsvrName timeout invalidAddr} {
 
    global loggingMsg
 
    if { [llength $tgtsvrName] == 0 } {
        puts stdout "usage : wtxLoggingTest targetServerName\
                        timeout invalidAddr"
        return
    }
 
    # variables initializations
    set serviceType 2
 
    # connection to the target server
    wtxToolAttachCall $tgtsvrName $serviceType $timeout
    #wtxErrorHandler [lindex [wtxHandle] 0] errorHandler

    set size 12000
    wtxLoggingTest3 $loggingMsg(3)
    wtxLoggingTest1 $size $loggingMsg(1)
    wtxLoggingTest2 $size $loggingMsg(2)
    wtxLoggingTest4 $size $loggingMsg(4)
    wtxLoggingTest5 $size $loggingMsg(5)
    wtxLoggingTest3 $loggingMsg(3)

    # deconnection
    wtxToolDetachCall "wtxLoggingTest"
}
 
