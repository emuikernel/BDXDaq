# hostCacheTest.tcl - Tcl script, test if Obj Module is cached on host
#
# Copyright 1997 Wind River Systems, Inc.
#
#
# modification history
# --------------------
# 01c,27may98,p_b  WTX 2.0 new changes
# 01b,20apr98,p_b   use wtxLogging 
# 01a,17jul97,f_l   written.
#
#
# DESCRIPTION : An object module loaded in target memory is also cached in
# host memory. By default every WTX read/write operations on object module
# segments are tranmitted to the target agent through WDB_MEM_READ/WRITE
# requests,except for a read operation on the text segment.
# Indeed every read operations on the text segment are processed
# on the host cache.
# For the three segments of an object module in target
# memory (i.e text, data and bss segments), this routine carries out read and
# write operations by means of the wtxMemRead and wtxMemWrite commands.
# It verifies if the object module in properly cached on the host by checking
# the WDB_MEM_READ/WRITE requests on the backend log file.
#


################################################################################
#
# logFileMark - put a "test label" at the end of the backend log file
#
# DESCRIPTION : This routine marks the beginning of each test on the 
#		backend log file. Thus, every WDB request following
#		this label can easily be identified and associated
#		with the current test (see logFileRequestSeek)


proc logFileMark {testLabel} {

    global fileName ;#

    set fileId [open $fileName a+]
    puts $fileId ""
    puts $fileId ""
    puts $fileId $testLabel
    puts $fileId ""
    close  $fileId
}


################################################################################
#
# logFileRequestSeek - tests the last WDB request on the backend log file
#
# DESCRIPTION :
# This routine scans the 500 last bytes of the backend log file.
# Once the current "test label" is detected, it looks for the following
# WDB request .There are three possibilities :
# - There is no following WDB request
# - The following WDB request is a WDB_MEM_READ request
# - he following WDB request is a WDB_MEM_WRITE request
# 
# PARAMETERS : testLabel
#
# RETURNS : reqStatus -  status of the WDB request detection,that is :
# -FATAL - no label found
# -NOREQUEST - no new request after the label
# -READ - WDB_MEM_READ request detected
# -WRITE - WDB_MEM_WRITE request detected
#
# and if WDB_MEM_READ or WDB_MEM_WRITE request are detected :
#
# -address - address for the WDB read/write access
# -numBytes - number of bytes read/written
#
#

proc logFileRequestSeek {testLabel} {
	
    global fileName	 ; # Backend Log file name

    set fileId [open $fileName r]   ; # set the seek offset 
    seek $fileId -500 end           ; # 500 bytes before end of file

    # verify the "test label" on log file

    while {[eof $fileId] != 1} {
        gets $fileId  curtline
		
	# the current "test label" is detected
	if {[lindex $curtline 0] == $testLabel} {
	    break
		
	}
    }

    # no label found (This case probably never happens.)

    if {[eof $fileId] == 1} {
        puts stdout "I didn't catch test label !!"
        puts stdout "fatal error"
        set reqStatus "FATAL"
        close $fileId
        return $reqStatus
    }

    # extract the new WDB request (if any) from the log file

    while {[eof  $fileId] != 1} {
        gets $fileId  curtline
		
	# WDB_MEM_READ request detected

	if {[lindex $curtline 1] == "WDB_MEM_READ"} {

	    # extract the WDB_MEM_READ request main arguments 
 	    # reqStatus="READ", address and numBytes
	     
	    set reqStatus "READ"
	    gets $fileId curtline  ;           # Skip one line
	    gets $fileId curtline
	    set address [lindex $curtline 1]
 	    gets $fileId curtline
 	    set numBytes [lindex $curtline 1]
	    close  $fileId
    	    return "$reqStatus $address $numBytes"
 
		
	# WDB_MEM_WRITE request detected

	} elseif {[lindex $curtline 1] == "WDB_MEM_WRITE"} { 
            
	    # extract the WDB_MEM_WRITE request main arguments
	    # reqStatus="WRITE", address and numBytes
     
	    set reqStatus "WRITE" 
	    gets $fileId curtline  ;           # Skip one line
	    gets $fileId curtline
	    set numBytes [lindex $curtline 1]
	    gets $fileId curtline
	    set address  [lindex $curtline 1]
	    close  $fileId
    	    return "$reqStatus $address $numBytes"

	}
    }
    
    # no new WDB request on the log file

    set reqStatus "NOREQUEST"
    close  $fileId 
    return $reqStatus       
}



################################################################################
#
# hostCacheTest1 - wtxMemRead test on object module text segment
# 
# REMARKS : see inner coments for details 
#


proc hostCacheTest1 {Msg} {

    global errHostCache
    global USE_TSFORLOAD

    puts stdout "$Msg" nonewline
    
    # verify if hostCache.o exists in right directory
     
    set name [objPathGet test]hostCache.o
    if { [file exists $name] == 0 } {
          testFail "File $name is missing for hostCacheTest-test 1"
          return
       }
    
    # load hostCache.o file
    
    if {[catch {wtxObjModuleLoad $USE_TSFORLOAD\
		[objPathGet test]hostCache.o} tModuleId]} {
        testFail "$tModuleId  in hostCacheTest-test 1"
        return
        }
    
    # set object module text segment base address

    set textSegAdd [lindex $tModuleId 1]

    # put the startCacheTest1 "test label" at the end of the backend log file  
    
    logFileMark "startCacheTest1"
    
    wtxMemRead $textSegAdd 8	      ;# read 8 bytes 

    # tests the last WDB request on the backend log file
   
    set detection [logFileRequestSeek "startCacheTest1"]
    scan $detection {%s %s %s} reqStatus address numBytes

    # check the status of the WDB request detection 
    
    switch -exact $reqStatus {
        "FATAL" { 

	    # FATAL - no label found 
	    #(This case probably never happens.)
            
	    testFail "Backend log file error for hostCacheTest-test 1"
	}
        "NOREQUEST" {
			
	    # NOREQUEST - no new request after the label
	    # clean up

            cleanUp o [lindex $tModuleId 0]
	    wtxTestPass
	    return
	}
	"READ" {

	    # READ - WDB_MEM_READ request detected

	    testFail $errHostCache(1)
	}
	default {
	
	    # (This case probably never happens.)

	    testFail $errHostCache(2)
 	}
    }
    
    # clean up

    cleanUp o [lindex $tModuleId 0]
    return

} 

	
################################################################################
#
# hostCacheTest2 - wtxMemWrite test on object module text segment
#
# REMARKS : see inner comments for details
#
#

proc hostCacheTest2 {Msg} {

    global errHostCache
    global fileName
    global USE_TSFORLOAD
    global myBlock      ;# host memory block to write to target memory

    puts stdout "$Msg" nonewline

    # verify if hostCache.o exist in right directory

    set name [objPathGet test]hostCache.o
    if { [file exists $name] == 0 } {
          testFail "File $name is missing for hostCacheTest-test 2"
          return
       }

    # load hostCache.o file

    if {[catch {wtxObjModuleLoad $USE_TSFORLOAD\
	 [objPathGet test]hostCache.o} tModuleId]} {
        testFail "$tModuleId  in hostCacheTest-test 2"
        return
        }

    # set object module text segment base address

    set textSegAdd [lindex $tModuleId 1]

    # put the startCacheTest2 "test label" at the end of the backend log file

    wtxLogging wdb off

    logFileMark "startCacheTest2"

    if [catch {wtxLogging wdb on $fileName}] {
     testFail "Can't run wtxLogging with wdb,on,$fileName parameters"
     return 0
    }

    wtxMemWrite $myBlock $textSegAdd         ;# Write block on target 

    set detection [logFileRequestSeek "startCacheTest2"]
    scan $detection {%s %s %s} reqStatus address numBytes

    # check the status of the WDB request detection

    switch -exact $reqStatus {
        "FATAL" {
            # FATAL - no label found
            #(This case probably never happens.)

            testFail "Backend log file error for hostCacheTest-test 2"
	}
	"NOREQUEST" {
 
            # NOREQUEST - no new request after the label
           
            testFail $errHostCache(3)
        }
	"WRITE" {
			
	    # verify Write access validity (8 bytes written)

	    if {($address == $textSegAdd) && ($numBytes == 8)} {
               
	        # clean up
                
                cleanUp o [lindex $tModuleId 0]
                wtxTestPass
                return
            } else {
		testFail $errHostCache(4)
	    }
	}
	default {
		
	    # The WDB request expected was WDB_MEM_WRITE but
	    # we got WDB_MEM_READ here !
	    # (This case probably never happens.)
			
	    testFail $errHostCache(5)
        }
    }

    # clean up

    cleanUp o [lindex $tModuleId 0]
    return

}


################################################################################
#
# hostCacheTest3 - wtxMemRead test on object module data segment
#
# REMARKS : see routine testCache1 for details
#
#


proc hostCacheTest3 {Msg} {

    global errHostCache
    global USE_TSFORLOAD

    puts stdout "$Msg" nonewline
   
    # verify if hostCache.o exists in right directory

    set name [objPathGet test]hostCache.o
    if { [file exists $name] == 0 } {
          testFail "File $name is missing for hostCacheTest-test 3"
          return
       }

    # load hostCache.o file

    if {[catch {wtxObjModuleLoad $USE_TSFORLOAD\
		[objPathGet test]hostCache.o} tModuleId]} {
        testFail "$tModuleId  in hostCacheTest-test 3"
        return
        }

    # set object module data segment base address

    set dataSegAdd [lindex $tModuleId 2]

    # put the startCacheTest3 "test label" at the end of the backend log file

    logFileMark "startCacheTest3"
    wtxMemRead $dataSegAdd 8        ;# read 8 bytes

    # tests the last WDB request on the backend log file

    set detection [logFileRequestSeek "startCacheTest3"]
    scan $detection {%s %s %s} reqStatus address numBytes

    # check the status of the WDB request detection

    switch -exact $reqStatus {
	"FATAL" {

            # FATAL - no label found
            #(This case probably never happens.)

            testFail "Backend log file error for hostCacheTest-test 3"
        }
        "NOREQUEST" {
 
            # NOREQUEST - no new request after the label

            testFail $errHostCache(6)
        }
        "READ" {
	    
            # verify Read access validity (8 bytes read)

            if {($address == $dataSegAdd) && ($numBytes == 8)} {
 
                # clean up

                cleanUp o [lindex $tModuleId 0]
                wtxTestPass
                return
            } else {
                testFail $errHostCache(7)
            }
        }
        default {

            # The WDB request expected was WDB_MEM_READ but
            # we got WDB_MEM_WRITE here !
            # (This case probably never happens.)

            testFail $errHostCache(2)
        }
    }

    # clean up

    cleanUp o [lindex $tModuleId 0]
    return

}


################################################################################
#
# hostCacheTest4 - wtxMemWrite test on object module data segment
#
# REMARKS : see inner comments for details
#
#

proc hostCacheTest4 {Msg} {

    global errHostCache
    global USE_TSFORLOAD
    global myBlock 	;# host memory block to write to target memory

    puts stdout "$Msg" nonewline

    # verify if hostCache.o exist in right directory

    set name [objPathGet test]hostCache.o
    if { [file exists $name] == 0 } {
          testFail "File $name is missing for hostCacheTest-test 4"
          return
       }

    # load hostCache.o file

    if {[catch {wtxObjModuleLoad $USE_TSFORLOAD\
	 [objPathGet test]hostCache.o} tModuleId]} {
        testFail "$tModuleId  in hostCacheTest-test 4"
        return
        }

    # set object module data segment base address

    set dataSegAdd [lindex $tModuleId 2]

    # put the startCacheTest4 "test label" at the end of the backend log file

    logFileMark "startCacheTest4"
    wtxMemWrite $myBlock $dataSegAdd         ;# Write block on target
    set detection [logFileRequestSeek "startCacheTest4"]
    scan $detection {%s %s %s} reqStatus address numBytes

    # check the status of the WDB request detection

    switch -exact $reqStatus {
        "FATAL" {
            # FATAL - no label found
            #(This case probably never happens.)

            testFail "Backend log file error for hostCacheTest-test 4"
        }
        "NOREQUEST" {

            # NOREQUEST - no new request after the label

            testFail $errHostCache(9)
        }
        "WRITE" {

            # verify Write access validity (8 bytes written)

            if {($address == $dataSegAdd) && ($numBytes == 8)} {

                # clean up

                cleanUp o [lindex $tModuleId 0]
                wtxTestPass
                return
            } else {
                testFail $errHostCache(4)
            }
        }
        default {

            # The WDB request expected was WDB_MEM_WRITE but
            # we got WDB_MEM_READ here !
            # (This case probably never happens.)
               
            testFail $errHostCache(5)
        }
    }

    # clean up

    cleanUp o [lindex $tModuleId 0]
    return

}



################################################################################
#
# hostCacheTest5 - wtxMemRead test on object module bss segment
#
# REMARKS : see routine testCache3 for details
#
#


proc hostCacheTest5 {Msg} {

    global errHostCache
    global USE_TSFORLOAD

    puts stdout "$Msg" nonewline
  
    # verify if hostCache.o exists in right directory

    set name [objPathGet test]hostCache.o
    if { [file exists $name] == 0 } {
          testFail "File $name is missing for hostCacheTest-test 5"
          return
       }

    # load hostCache.o file

    if {[catch {wtxObjModuleLoad $USE_TSFORLOAD\
	[objPathGet test]hostCache.o} tModuleId]} {
        testFail "$tModuleId  in hostCacheTest-test 5"
        return
        }

    # set object module data segment base address

    set bssSegAdd [lindex $tModuleId 3]

    # put the startCacheTest5 "test label" at the end of the backend log file

    logFileMark "startCacheTest5"
    wtxMemRead $bssSegAdd 8              ;# read 8 bytes
    set detection [logFileRequestSeek "startCacheTest5"]
    scan $detection {%s %s %s} reqStatus address numBytes

    switch -exact $reqStatus {
        "FATAL" {
            testFail "Backend log file error for hostCacheTest-test 5"
        }
        "NOREQUEST" {
            testFail $errHostCache(8)
        }
        "READ" {
        
	    # verify Read access validity (8 bytes read)

            if {($address == $bssSegAdd) && ($numBytes == 8)} {

                # clean up

                cleanUp o [lindex $tModuleId 0]
                wtxTestPass
                return
            } else {
                testFail $errHostCache(7)
            }
        }
        default {
            testFail $errHostCache(2)
        }
    }

    # clean up

    cleanUp o [lindex $tModuleId 0]
    return

}


################################################################################
#
# hostCacheTest6 - wtxMemWrite test on object module bss segment
#
# REMARKS : see routine testCache2 for details
#
#


proc hostCacheTest6 {Msg} {

    global errHostCache
    global USE_TSFORLOAD
    global myBlock 

    puts stdout "$Msg" nonewline

    # verify if hostCache.o exist in right directory

    set name [objPathGet test]hostCache.o
    if { [file exists $name] == 0 } {
          testFail "File $name is missing for hostCacheTest-test 6"
          return
       }

    # load hostCache.o file

    if {[catch {wtxObjModuleLoad $USE_TSFORLOAD\
    		[objPathGet test]hostCache.o} tModuleId]} {
        testFail "$tModuleId  in hostCacheTest-test 6"
        return
        }

    # set object module text segment base address

    set bssSegAdd [lindex $tModuleId 3]

    # put the startCacheTest6 "test label" at the end of the backend log file

    logFileMark "startCacheTest6"

    wtxMemWrite $myBlock $bssSegAdd         ;# Write block on target
    set detection [logFileRequestSeek "startCacheTest6"]
    scan $detection {%s %s %s} reqStatus address numBytes

    switch -exact $reqStatus {
        "FATAL" {
            testFail "Backend log file error for hostCacheTest-test 6"
        }
        "NOREQUEST" {
            testFail $errHostCache(10)
        }
        "WRITE" {

            # verify Write access validity (8 bytes written)

            if {($address == $bssSegAdd) && ($numBytes == 8)} {

                # clean up

                cleanUp o [lindex $tModuleId 0]
                wtxTestPass
                return
            } else {
                testFail $errHostCache(4)
            }
        }
        default {
            testFail $errHostCache(5)
        }
    }

    # clean up

    cleanUp o [lindex $tModuleId 0]
    return

}

################################################################################
#
# hostCacheTest - main procedure
#
# PARAMETERS :
# 		- targetServerName 
# 	        - timeout

proc hostCacheTest { tgtsvrName timeout invalidAddr } {
	
    if { [llength $tgtsvrName] == 0 } {
        return
    }

    # variables initializations

    global myBlock
    global fileName
    global hostCacheMsg
    global errHostCache

    set fileName [tclPathGet]wdbLog

    # connection to the target server

    wtxToolAttachCall $tgtsvrName 0 $timeout

    wtxLogging all off

    if [catch {wtxLogging wdb on $fileName}] {
     testFail "Can't run wtxLogging with wdb, on, $fileName parameters"
     return
    }

    # create an 8 bytes memory block filled with 0x55
      
    set myBlock [memBlockCreate 8 0x55]

    hostCacheTest1 $hostCacheMsg(1)
    hostCacheTest2 $hostCacheMsg(2)
    hostCacheTest3 $hostCacheMsg(3)
    hostCacheTest4 $hostCacheMsg(4)
    hostCacheTest5 $hostCacheMsg(5)
    hostCacheTest6 $hostCacheMsg(6)

    wtxLogging wdb off
    file delete $fileName

    # detach from the target server
    wtxToolDetachCall "\nhostCacheTest : "

}

