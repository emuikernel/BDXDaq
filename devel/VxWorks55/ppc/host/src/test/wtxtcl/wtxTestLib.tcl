# wtxTestLib.tcl  - Tcl script with common routines
#
# Copyright 1995-1996 Wind River Systems, Inc.
#
#
# modification history
# --------------------
# 02h,18jan99,p_b  rename wtxCommonProc.tcl in wtxTestLib.tcl
# 02g,05may98,p_b  WTX 2.0 tests
# 02f,28mar98,p_b  added tclPathGet & initEventsFile, rename wtxTestMsge
#		   in wtxTestMsg, added mainCommonProc
# 02f,12mar98,jmb  HPSIM merge of Mark Mason's patch from 13nov96:  add
#                  use of targetTool.
# 02e,11mar97,jmp  modified catch format when a filename is given in
#                  parameters to avoid failures on windows side (SPR #7868).
# 02d,06jan97,elp  make sure event queue is empty before a funcCall.
# 02c,18nov96,jmp  modified wtxErrorMatch to match either upper or lower case
#		   letters.
# 02b,25jun96,jmp  removed wtxHostInit since SPR# 4941 is fixed.
# 02a,19nov95,p_m  changed gnucoff to gnu for 960.
# 01z,20oct95,jmp  fixed a bug in invalidAddrGet.
# 01y,23aug95,jmp  fixed a syntax error in funcCall (2 return call).
#		   fixed a bug in duplicated $arg1 in testTaskCreate.
#		   removed useless malloc for wtxContextCreate,
#		   removed useless return value of readContextCreate.
#		   added architechture x86-win32 for NT to set O_CREATE.
#		   added indices "t" and "mb" in cleanUp procedure.
#		   changed objGetPath implementation and name to objPathGet.
#		   changed openVio to vioOpen, closeVio to vioClose,
#		   emptyVio to vioEmpty, getFormatTest to loaderFormatGet.
#		   added a wtxTimeout for the tests in wtxToolAttachCall.
#		   changed tests control method: added errorHandler,
#		   warningHandler and testFail.
#		   added invalidAddrGet
# 01x,27jul95,pad  Changed O_CREAT value for parisc-hpux9 from 0x400 to
#                  0x100 (SPR #4548).
# 01w,25jul95,p_m  added MC68LC040, AM29030 and AM29200 CPUs in
#		   objGetPath (#SPR 4531).
# 01v,27jun95,f_v  catched few WTX calls
#	      jco  added wtxHostInit	
# 01u,23jun95,c_s  added wtxTestRun procedure
# 01t,16jun95,p_m  added wtxRegisterForEvent ".*" in funcCall.
#		   XXX This fix should be fully validated.
# 01s,14jun95,f_v  changed wtxToolAttach name parameter
#		   added 386 and 486 CPU number
# 01r,30may95,f_v  changed openVio, updated wtxContextCreate call
#		   added a generic cleanUp procedure
# 01q,22may95,jcf  name revision.
# 01p,21may95,p_m  removed detection related stuff
# 01o,16may95,f_v  removed tcl extended functions
# 01n,15may95,c_s  replaced use of lassign with bindNamesToList.
# 01m,12may95,c_s  upgraded for new wtxtcl version.
# 01l,03may95,f_v  changed objGetPath proc,added getFormatTest procedure
# 01k,28apr95,c_s  defined wtxErrorMatch.  Moved posix-style error codes
#                  and command options here.
# 01j,27apr95,f_v  replace funcCall1,funcCall2,funcCall3 by a generic
#		   funcCall procedure
# 01i,19apr95,f_v  change wtxTestPass and wtxTestFail
#	 	   remove uplevel in testTaskCreate thanks to new
#		   wtxTestPass, wtxTestFail structure
#		   added testEventPointAdd procedure
# 01h,13apr95,f_v  added openVio,emptyVio,readContextCreate procedures
#		   fix a bug in testTaskCreate
# 01g,12apr95,f_v  added format parameter in objGetPath
#		   change arg to args and place it in last position in
#		   testTaskCreate - args is a key word
# 01f,11apr95,f_v  added uplevel in testTaskCreate for messages
# 01f,10apr95,c_s  added testTaskCreate,testSymFind,testTaskCleanup
# 01f,09mar95,f_v  changed SIMSPARCSUNOS code
# 01e,20feb95,kpn  added funcCall1{}, funcCall2{} and funcCall3{}.
# 01d,20feb95,f_v  added parameter to objGetPath 
#		   cahnge CPU_FAMILY to cpuId to avoid confusion
# 01c,26Jan95,kpn  added wtxToolDetachCall{}
#		   added the wtxError.tcl, wtxTestMsg.tcl and 
#		   wtxTestErr.tcl loading
#                  documentation.
# 01b,22Jan95,kpn  added objGetPath{} and wtxTestLib{}
#	      f_v  replaced return command by error and removed wtxTestFail{}
#		   call from wtxToolAttachCall{}.
# 01a,04Jan95,kpn  written.
#

#
# DESCRIPTION
#
# To run test scripts independently
# ---------------------------------
# We can run the test scripts independently, without the WTX Test Tool. In this
# purpose this file (wtxTestLib.tcl) has been created. 
#
# (1) This file contains the procedures that are common to every test scripts :
#
# - wtxTestFail{} and wtxTestPass{} : prints the description of the testcase
#                                     units that have failed or passed,and 
#                                     display the number of failures or
#                                     success.
#
# - wtxToolAttachCall{} : calls the wtxToolAttach routine, this procedure can be
#                         called by each WTX service test to attach tool to a
#                         target server.
#
# - objPathGet{} : provides the path of object files.
#
# - wtxTestLib{} : gives the ability to bring shell functionnalities. This
#                     procedure must be called before running the test script.
#
# - funcCall{}  : call a function on the target and wait for event until to 
#		   get the right event. 
#
#
# (2) And loads necessary files :
#
# - shelcore.tcl     : allows to add some shell possibilities,
#
# - wtxError.tcl     : contains the WTX and VxWorks error codes,
#		       allows to verify the result of wtx services call,	
#
# - wtxTestErr.tcl : contains the testcase units errors
#		       allows to display the reason of a test failure,
#
# - wtxTestMsg.tcl  : contains the description of the testcase units.
#		       allows to inform the user of the wtx test tool which
#		       kind of test has been performed and which test has 
#		       failed or succeeded.
#
#
#
# To have no redundancy 
# ---------------------
# This file (wtxTestLib.tcl) is the mean to have no redundancy in the test
# scripts too. For example each test needs to be attached as tool to the target
# server (thru a call to wtxToolAttach routine). Furthermore they need to verify
# if the call to this routine went off smoothly. So the same code is repeated
# for each test...
# It is then easier, faster and safer to use the procedure wtxToolAttachCall to
# perform the actions above.
#
#
# USAGE : > wtxtcl
#         % source ~/wpwr/host/src/test/wtxtcl/wtxTestLib.tcl
#         % wtxTestLib <targetServerName> <timeout>
#
# Note that if wtxtcl has been run in the wtxTestTool.tcl directory, you may
# use the source command as following : source wtxTestLib.tcl
#
#

#*************************************************************************
#
# errorHandler - Error handling procedure 
#
# Print the error message provided by the exception and call wtxTestFail.
# 
# REMARKS : 
#
#	- This routine must be used only in the tests with valid parameters.
#	- When the errorHandler procedure is installed, it's useless to
#	catch the called function, because the handler does it.
#	- When an exception occured, the interrupt function return an 
#	empty string ("").
#
#  - Example of call when the errorHandler is used :
#
#	set resMemAlloc [wtxMemAlloc $validSize]
#	if {$resMemAlloc == ""} {
#		return
#		}
#
#	- If an error occured while executing "wtxMemAlloc $validSize"
#	the printed result will be something like :
#
#	Allocate memory on target with valid parameter		FAIL
#	WTX Error 0x10046 (TGTMEM_NOT_ENOUGH_MEMORY)
#	While executing: wtxMemAlloc 100000000
#

proc errorHandler {handle cmd err tag} {
	puts stdout "FAIL\n$err\nWhile executing: $cmd\n"
	wtxTestFail
}


#*************************************************************************
#
# warningHandler - Warning handling procedure 
#
# Print the warning message provided by the exception.
# This routine is used by clean up procedures.
#
# This is an example of warningHandler result
#
#	Unload an object module
#	WARNING : WTX Error 0x10030 (LOADER_OBJ_MODULE_NOT_FOUND)
#	While executing: wtxObjModuleUnload 0x79018             PASS
#
#
proc warningHandler {handle cmd err tag} {
	puts stdout "\nWARNING : $err\nWhile executing: $cmd\t\t" nonewline
}


#*************************************************************************
#
# testFail - Error printing procedure
#
# Print the message of errors not treated by errorHandler and call wtxTestFail
#

proc testFail {Msg} {

	puts stdout "FAIL\n$Msg"
	wtxTestFail
}
	
#***************************************************************************
#
# wtxTestFail - Increment fail test
#
# Increment the number of failures.
#

proc wtxTestFail {} {

    global countingFile
    puts $countingFile "0 1"
}


#***************************************************************************
#
# wtxTestPass - Print "PASS" after the test message and increment pass test
#
# Print "PASS" after the testcase message and increment
# the number of success.
#

proc wtxTestPass {} {

    global countingFile
    puts stdout "PASS"
    puts $countingFile "1 0"
}

#*****************************************************************************
#
# wtxTestRun - run a test
#
# This procedure takes two arguments: a test to run, and a parameter to run
# it with. If the test returns an error, we print errorInfo, which will 
# contain the Tcl error stack.  If the test does not return an error, 
# we assume that the test itself called wtxTestPass or wtxTestFail.
#

proc wtxTestRun {testProc args} {

    global errorInfo

    if {[catch {eval [concat $testProc $args]} result]} {
	puts stdout [lindex $args [expr [llength $args]-1]]
	testFail "error in $testProc: \n$errorInfo
    }
    return $result
}

#*****************************************************************************
#
# wtxToolAttachCall - Call wtxToolAttach routine
#
# This procedure is called by the  WTX service tests to attach themselves to
# target server.
#

proc wtxToolAttachCall {tgtsvrName serviceType timeout} {

    set serviceType wtxtest-$serviceType

    # connection to the target server
    catch "wtxToolAttach $tgtsvrName $serviceType" resToolAttach

    # if timeout is equal to 0 tests use default value.
    if {$timeout != 0} {
	wtxTimeout $timeout
	}
    if {$resToolAttach == "must detach exitsing connection before\
        making another"} {
        puts stdout "\n$msge\n" 
        error "\nERROR : You must DETACH existing CONNECTION before\
	making this test\n"
    }

    if {$resToolAttach == "unable to find that target server"} {
        puts stdout "\n$msge\n"
        error "\nERROR : The TARGET NAME you've given is UNKNOWN\
	: $tgtsvrName\n"
    }

}

#*****************************************************************************
#
# wtxToolDetachCall - Call wtxToolDetach routine 
#
# This procedure is called by the WTX service tests to detach themselves from
# the target server.
#

proc wtxToolDetachCall {msge} {

    catch "wtxToolDetach" errmsg

    if {$errmsg != ""} {
        puts stdout "$msge FAIL"
        puts stdout "\nERROR : Can't detach tool\n"
    }
}



#*******************************************************************************
#
# tclPathGet - Provides TCL files directory
#

proc tclPathGet {} {
 global env

 if { $env(WIND_HOST_TYPE) == "x86-win32" } {

  set ret $env(WIND_BASE)\\host\\src\\test\\wtxtcl\\

 } else {

  set ret $env(WIND_BASE)/host/src/test/wtxtcl/

 }

 return $ret

}


#*******************************************************************************
#
# objPathGet - This procedure provide the path of object file
# 
# if you give a flag this procedure return formatType number instead of path
# 
# you can find these values in ~/wpwr/host/include/cputypes.h

proc objPathGet {{name test} {flag 0}} {
	upvar #0 vxCpuFamily fam
        global env
	global cpuType
	global cpuFamily

        if [catch {set nbCPU [lindex [lindex [wtxTsInfoGet] 2 ] 0]}] {
         exit 0
        }

	set cpuId $cpuType($nbCPU)

        set format [targetTool]
        if {$cpuFamily($nbCPU) == "am29k"} {
                set formatNb 2
        } else {
                set formatNb 0
        }

       if { $env(WIND_HOST_TYPE) == "x86-win32" } {

       set objpath $env(WIND_BASE)\\target\\lib\\obj[set cpuId][set format]$name\\

       } else {

       set objpath $env(WIND_BASE)/target/lib/obj[set cpuId][set format]$name/

       }

       case $flag in {
          0 { return $objpath }
	  1 { return $formatNb }
	  2 { set name $fam($nbCPU)
	      return "$nbCPU $name"
	      }
	  default { return $objpath }
       }
}


#*******************************************************************************
#
# invalidAddrGet - This procedure provide an invalid address for the tests
# 
# This routine calls vxMemProbe with MEM_TOP as parameters to see if it is
# readable and writable, if it is then the routine adds 0x1000000 as increment
# to MEM_TOP until it is not readable and writable.
#
# RETURNS : An invalid address.
#
# This routine can be called when the error handler is used or not, so it's
# very important to test the returned values in both cases.
#

proc invalidAddrGet {} {
    
    if [catch {set tsMemInfo [lindex [wtxTsInfoGet] 6]}] {
     exit 0
    }
    set memBase	[lindex $tsMemInfo 0]
    set memSize [lindex $tsMemInfo 1]

    set memTop [format 0x%x [expr $memBase + $memSize]]

    set testAddr $memTop

    # Test if an exception occurred (case with no error handling procedure)
    if {[catch "wtxMemAlloc 4" addr]} {
	return ""
	}

    # Test if errorHandler has caught an exception
    # (case with use of an error handling procedure)
    if {$addr == ""} {
	return ""
	}


    set resMemProbeRead [funcCall vxMemProbe $testAddr 0 4 $addr]
    set resMemProbeWrite [funcCall vxMemProbe $testAddr 1 4 $addr]

    while {$resMemProbeRead == 0 || $resMemProbeWrite == 0} {
	set testAddr [expr $testAddr + 0x1000000]
	set testAddr [format 0x%x $testAddr]

	set resMemProbeRead  [funcCall vxMemProbe $testAddr 0 4 $addr]
	set resMemProbeWrite [funcCall vxMemProbe $testAddr 1 4 $addr]
	}

    cleanUp ms $addr
    
    return $testAddr
}


#******************************************************************************
#
# wtxTestLib - This script allows to add some shell possibilities
#
#

source $env(WIND_BASE)/host/resource/tcl/shelcore.tcl

proc wtxTestLib { tgtsvrName timeout } {

    if [catch {wtxToolAttachCall $tgtsvrName 5 $timeout}] {
     puts stdout "Can not attach target server $tgtsvrName"
     exit 0
    }
    if [catch {shellCoreInit}] {
     puts stdout "shellCoreInit failed."
     puts stdout ""
     exit 0
    } 
    wtxToolDetach
}

#******************************************************************************
#
# funcCall - call a function on the target and wait for event until 
#            the right event is gotten.
#
# This procedure calls the function <func> passed as argument with 
# some parameters <args> (thru the wtxFuncCall() routine) and waits for event
# until it gets the right one (thru the wtxEventPoll() routine).
#
# If wtxFuncCall{} fails, this routine raises an error exception.
#
# RETURNS: the value returned by the called function.
#

proc funcCall {func args} {

    # register for (all) events before making a call.

    # Test if an exception occurred (case with no error handling procedure)
    if {[catch {wtxRegisterForEvent ".*"} resRegist]} {
	error "\nERROR : Can't register for event for $func\n"
	}
    # Test if errorHandler has caught an exception
    # (case with use of an error handling procedure)
    if {$resRegist == ""} {
	error "\nERROR : Can't register for event for $func\n"
	}


    set funcAddr [lindex [wtxSymFind -name $func] 1]

    # Make sure event queue is empty to be sure we will receive the event
    # generated by this function call and not a previous one.

    while {[wtxEventGet] != ""} {}

    # Test if an exception occurred (case with no error handling procedure)
    if {[catch "wtxFuncCall $funcAddr $args" funcId]} {
        error "\nERROR : Can't call $func\n"
        }
    # Test if errorHandler has caught an exception
    # (case with use of an error handling procedure)
    if {$funcId == ""} {
	error "\nERROR : Can't call $func\n"
	}

    # wait for event
    # Test if an exception occurred (case with no error handling procedure)
    if {[catch "wtxEventPoll 100 50" resEventPoll]} {
        error "\nERROR : Can't get event for $func\n"
        }
    # Test if errorHandler has caught an exception
    # (case with use of an error handling procedure)
    if {$resEventPoll == ""} {
	error "\nERROR : Can't get event for $func\n"
	}

    # wait for event until we get the right event
    while {([string compare [lindex $resEventPoll 0] CALL_RETURN] && \
            [string compare [lindex $resEventPoll 1]  $funcId])} {
	# Test if an exception occurred
	# (case with no error handling procedure)
        if {[catch "wtxEventPoll 100 50" resEventPoll]} {
            error "\nERROR : Can't get event for $func\n"
            }
	# Test if errorHandler has caught an exception
	# (case with use of an error handling procedure)
	if {$resEventPoll == ""} {
	    error "\nERROR : Can't get event for $func\n"
	    }
    }

    return [lindex $resEventPoll 2]
}


#******************************************************************************
#
# waitEvent - Loop until event get
#
# WARNING : this procedure waits until the right event arrives.
#
#

proc waitEvent {eventName Id} {

    catch "wtxEventPoll 100" eventInfo
    while { [string compare [lindex $eventInfo 0] $eventName] && \
            [string compare [lindex $eventInfo 1] $Id]} {
        catch "wtxEventPoll 100" eventInfo
      }
}

#******************************************************************************
#
# testTaskCreate - create a new task and download object module file
#
# 

proc testTaskCreate {Msg objfile entry \
	{priority 100} {options 0} {args 0} } {

    global USE_TSFORLOAD

    # defaults arguments for wtxContextCreate
    set stackAddr 0
    set stackSize 0

    # get args and assign to 0 if {}
    if {$args == {}} {
    	set args {0 0 0 0 0 0 0 0 0 0}
     }
    set pos [lsearch $args {}]
    if { $pos != -1 } {
	set args [lreplace $args $pos end 0 0 0 0 0 0 0 0 0 0]
    }

    bindNamesToList {arg0 arg1 arg2 arg3 arg4 arg5 arg6 arg7 arg8 arg9} $args

    # download module

    # verify if module exists in right directory
    set name [objPathGet test]$objfile

    if { [file exists $name] == 0 } {
          testFail "File $name is missing."
          return 1 
    }

    # load file
    if {[catch {wtxObjModuleLoad $USE_TSFORLOAD\
		[objPathGet test]$objfile} tModuleId]} {
        testFail "$tModuleId - Could not load $name"
	return 1 
    }

    if {$tModuleId == ""} {
	return 1
	}
    
    set tModuleId [lindex $tModuleId 0]

    if {![set symAddr [testSymFind $entry]]} {
	return 1
	}
    
    # create user task - taskInit
    if {[catch "wtxContextCreate CONTEXT_TASK Test[pid] $priority $options\
	     $stackAddr $stackSize $symAddr 0 0 $arg0 $arg1 \
	     $arg2 $arg3 $arg4 $arg5 $arg6 $arg7 $arg8 $arg9" contextId]} {
	testFail "$contextId - Could not create context for $entry"
	return 1
	}

    if {$contextId == ""} {
	return 1
	}

     return "$contextId $tModuleId"
}

#******************************************************************************
#
# testSymFind - find a symbol by name
#
# Return symbol address or 0 if symbol isn't found
#
#
proc testSymFind {name} {

    # test if symbol isn't found
    if {[catch "wtxSymFind -name $name" sym]} {
	testFail $sym
	return 0
    }

    # test if errorHandler has caught the error
    if {$sym == ""} {
	return 0
    }

    return [lindex $sym 1]
}

#******************************************************************************
#
# initEventsFile - used with tests that use events 
#

proc initEventsFile {nameFile testName name} {

    global env

    if {[file exists $nameFile] == 0 } {
       testFail "File $nameFile is missing for $testName"
       return 0
    }

    if { $env(WIND_HOST_TYPE) == "x86-win32" } {

     set nameTmpFile $env(TMP)\\[file tail $nameFile]
     file copy -force $nameFile $nameTmpFile
     file attributes $nameTmpFile -readonly 0
    } else {
 
     set nameTmpFile /tmp/[file tail $nameFile]
     file copy -force $nameFile $nameTmpFile
     file attributes $nameTmpFile -permissions 00666
 
    }

    if {[file exists $nameTmpFile] == 0 } {
       testFail "File $nameTmpFile wasn't created for $testName"
       return 0
    }

    if [catch {set Id [open $nameTmpFile a]}] {
       testFail "open $nameTmpFile Fails."
       return 0
    }

    # Add target server name at the end of file
    puts $Id $name

    # flush and close the Events file
    flush $Id
    close $Id

    return $nameTmpFile

}

#******************************************************************************
#
# testTaskCleanup - clean up routine
#
# This routine cleans task previously created by testTaskCreate,
# it uses a warning handler that prints a warning message provided
# by the occured exeption.
#

proc testTaskCleanup {taskDesc {context CONTEXT_TASK}} {
   
   # save active handler to restore it at the end of the routine
   set oldHandler [wtxErrorHandler [lindex [wtxHandle] 0]]

   # install warningHandler
   wtxErrorHandler [lindex [wtxHandle] 0] warningHandler

   wtxContextKill $context [lindex $taskDesc 0]

   wtxObjModuleUnload [lindex $taskDesc 1]

   # restore old handler
   wtxErrorHandler [lindex [wtxHandle] 0] $oldHandler 
}


#*************************************************************************
#
# testEventpointAdd - generic procedure to add breakpoint
#

proc testEventpointAdd {Msg eventArg contextId \
			{eventType TEXT_ACCESS}\
			{textType CONTEXT_TASK}\
			{actionType ACTION_NOTIFY|STOP}\
			{actionArg 0}\
			{callRtn 0}\
			{callArg 0}} {

    # set a breakpoint on user task ( at the beginning of the task )
    if {[catch "wtxEventpointAdd $eventType $eventArg\
			    $textType $contextId\
			    $actionType $actionArg\
			    $callRtn $callArg" bkpNb]} {
		testFail "Could not add breakpoint"
		return ""
    }

    # return breakpoint number
    return $bkpNb
}

#******************************************************************************
#
# Load wtxTestMsg.tcl that contains the description of the testcase units.
# Load wtxTestErr.tcl that contains the testcase units errors.
#

#*************************************************************************
#
# vioOpen - open a virtual I/O file associated with a virtual I/O
#           channel 
#
# RETURN : the open routine result (i.e the file descriptor of the
#          virtual I/O file associated with a virtual I/O channel)
#	   and the channel number	
#
proc vioOpen {}  {

 # GLOBAL VARIABLES

    global O_RDWR

    # get a Vio channel number
    set vioNb [wtxVioChanGet]

    # name of the device
    set vioDev "/vio/$vioNb"

    # operations to give the device name to the open function
    set vioDevBlk [memBlockCreate -string $vioDev]

    set allocSize [expr [string length $vioDev] + 1]

    if {[catch "wtxMemAlloc $allocSize" memBuf]} {
	testFail $memBuf
        cleanUp mb $vioDevBlk
        return 0
        }

    if {$memBuf == ""} {
	cleanUp mb $vioDevBlk
	return 0
	}

    # clear the allocated block of memory
    if {[catch "wtxMemSet $memBuf $allocSize 0" bufMemSet]} {
	testFail $bufMemSet
        cleanUp mb $vioDevBlk ms $memBuf
        return 0
	}

    if {$bufMemSet == ""} {
        cleanUp mb $vioDevBlk ms $memBuf
        return 0
	}

    if {[catch "wtxMemWrite $vioDevBlk $memBuf" resMemWrite]} {
	testFail $resMemWrite
        cleanUp mb $vioDevBlk ms $memBuf
        return 0
	}

    if {$resMemWrite == ""} {
	cleanUp mb $vioDevBlk ms $memBuf
        return 0
	}

    # open a virtual I/O file associated with virtual I/O channel 0 
    set fileDescriptor [funcCall open $memBuf $O_RDWR 0]

    # clean up
    cleanUp mb $vioDevBlk ms $memBuf

    return "$fileDescriptor $vioNb"
}



#*************************************************************************
#
# vioClose - close a virtual I/O file associated with the virtual I/O
#            channel chanId and release this channel 
#
# This routine uses a warning handler that prints a warning message
# provided by the occured exeption.
#
# RETURN : nothing 
#

proc vioClose {fd chanId} {

    #save active handler to restore it at the end of the routine
    set oldHandler [wtxErrorHandler [lindex [wtxHandle] 0]]

    # install warningHandler
    wtxErrorHandler [lindex [wtxHandle] 0] warningHandler

    # close the virtual I/O file associated with a virtual I/O channel 
    if {[funcCall close $fd]} {
	puts stdout "\nWARNING : Can't close virtual I/O file\t\t\t\t" nonewline
	}


    wtxVioChanRelease $chanId

    # restore old handler
    wtxErrorHandler [lindex [wtxHandle] 0] $oldHandler
}

#*************************************************************************
#
# vioEmpty - empty a virtual I/O file associated with the virtual
#               I/O channel 0
#
# Get the bytes number contained in the virtual I/O with the ioctl function
# If the number of bytes is not zero then the virtual I/O is read until
# it's empty.
# This procedure is used by the testcase units before sending data on the
# virtual I/O to check that the virtual I/O doesn't already contain some data.
# It's necessary for the robustness of the tests.
#
# RETURN : Room address for data that will be read from VIO channel 0.
#          Address of the read() function.
#

proc vioEmpty {fileDescriptor readBytesMaxNum maxBytesRead} {


 # GLOBAL VARIABLES

    # array that contains the testcase error codes
    global errVioMsg

    # flag value for ioctl function
    global FIONREAD

    # allocate room for the ioctl function result
    if {[catch "wtxMemAlloc 4" nBytes]} {
	testFail $nBytes
        return 0
        }
    if {$nBytes == ""} {
	return 0
	}

    # clear the allocated block
    if {[catch "wtxMemSet $nBytes 4 0" blockClear]} {
	testFail $blockClear
	cleanUp ms $nBytes
	return 0
	}

    if {$blockClear == ""} {
	cleanUp ms $nBytes
        return 0
        }

    # execute the ioctl function
    catch "funcCall ioctl $fileDescriptor $FIONREAD $nBytes" ioctlRet

    # verify that ioctl has succeeded
    if {$ioctlRet != 0} {
	testFail $errVioMsg(15) 
	cleanUp ms $nBytes
        return 0
	}

    # get the bytes number that the virtual I/O contains
    if {[catch "wtxMemRead $nBytes 4" vioNumBytes]} {
	testFail $vioNumBytes
	cleanUp ms $nBytes
	return 0
	}
    if {$vioNumBytes == ""} {
	cleanUp ms $nBytes
        return 0
        }

    set getVioNumBytes [memBlockGet $vioNumBytes]

    # clear a memory block of 4 bytes to compare it with the result of the
    # ioctl function, i.e the bytes number contained in the VIO
    set clearBlock [memBlockCreate -B 4 0]
    set blockGet [memBlockGet $clearBlock]

    # free $clearBlock block 
    cleanUp mb $clearBlock

    # allocate room for data that will be read from virtual I/O channel 0
    if {[catch "wtxMemAlloc $readBytesMaxNum" roomDataRead]} {
	testFail $roomDataRead
	cleanUp ms $nBytes
	return 0
	}
    if {$roomDataRead == ""} {
	cleanUp ms $nBytes
        return 0
        }

 # verify the VIO is EMPTY

    while {$getVioNumBytes != $blockGet} {

        # clear the allocated block of memory that will contain data read
        # from virtual I/O
        if {[catch "wtxMemSet $roomDataRead $readBytesMaxNum 0" \
	    roomDataMemSet]} {
		testFail $roomDataMemSet
		cleanUp ms $nBytes $roomDataRead
		return 0
		}
	if {$roomDataMemSet == ""} {
		cleanUp ms $nBytes $roomDataRead
                return 0
                }

        # read VIO to empty it
        catch "funcCall read $fileDescriptor $roomDataRead $maxBytesRead"\
        resFuncCallRead

        if {[catch "wtxMemSet $nBytes 4 0" blockClear]} {
		testFail $blockClear
		cleanUp ms $nBytes $roomDataRead
		return 0
		}
	if {$blockClear == ""} {
		cleanUp ms $nBytes $roomDataRead
                return 0
                }

        # execute ioctl function to obtain the bytes number contained in
        # the virtual I/O
        catch "funcCall ioctl  $fileDescriptor $FIONREAD $nBytes" ioctlRet

        # verify that ioctl has succeeded
        if {$ioctlRet != 0} {
		testFail $errVioMsg(15)
		cleanUp ms $nBytes $roomDataRead
		return 0
        }

        if {[catch "wtxMemRead $nBytes 4" vioNumBytes]} {
		testFail $vioNumBytes
		cleanUp ms $nBytes $roomDataRead
		return 0
		}
	if {$vioNumBytes == ""} {
		cleanUp ms $nBytes $roomDataRead
                return 0
                }

	set getVioNumBytes [memBlockGet $vioNumBytes]
    }

    # free memory block
    cleanUp ms $nBytes

    return "$roomDataRead"
}


#*************************************************************************
#
# wtxErrorMatch - test whether a WTX error message matches an expected 
#                 error string
# 
# "expected" will be of the form AGENT_UNSUPPORTED_REQUEST, while "received" 
# will typically be of the form "WTX Error 0x100c9 (AGENT_UNSUPPORTED_REQUEST).
# This function returns true if the expected string matches the text in 
# parentheses of the received string.
#

proc wtxErrorMatch {received expected} {
    global posixErr

    if {[regexp "WTX Error (\[x0-9a-fA-F\]+) \\((.*)\\)" \
	    $received all num msg] == 0} {
	return 0
    }

    # Test if the received string match the expected string
    # (match eiter upper or lower case letters.
    if {[regexp -nocase $msg $expected]} {return 1}

    # perhaps it's a POSIX style error code.  See if it's in the table 
    # and the hex numbers match.

    if [catch {set posNum $posixErr($expected)}] {
	# error! we lose.
	return 0
    } {
	return [expr $posNum == $num]
    }
}

#*************************************************************************
#
# readContextCreate - Create a context for read function
# 
# This routine return the value returned by wtxContextCreate
# or 0 if unsucessfull.
#
#

proc readContextCreate {fileDescriptor roomDataRead} {

 # GLOBAL VARIABLES

    # array that contains the testcase error codes
    global errCtxMsg
    # LOCAL VARIABLES

    # max size for buffer that receives read bytes on a virtual I/O
    set readBytesMaxNum 512
    # maximum number of bytes that can be read in one time on a virtual I/O
    set maxBytesRead 512
    # parameters to create a context
    set isTask 1
    set name Test[pid]
    set priority 100
    set options 0
    set stack 0
    set stackSize 0
    set redirIn 0
    set redirOut 0


    # clear the allocated block of memory that will contain data read
    # from virtual I/O
    
    if {[catch "wtxMemSet $roomDataRead $maxBytesRead 0" roomDataMemSet]} {
	testFail $roomDataMemSet
	return 0
	}
    # if error is caught by errorHandler
    if {$roomDataMemSet == ""} {
	return 0
	}

    set readAddr [lindex [wtxSymFind -name "read"] 1]
    
    if {[catch "wtxContextCreate CONTEXT_TASK $name $priority $options \
	$stack $stackSize $readAddr $redirIn $redirOut $fileDescriptor \
	$roomDataRead $maxBytesRead" targetSystemAssignedId]} {
		testFail $targetSystemAssignedId
        	return 0
    }

    # if error is caught by errorHandler
    if {$targetSystemAssignedId == ""} {
	return 0
	}

    set taskInfoList [activeTaskNameMap]

    # get the position of the new task in the list
    set IdPosition [lsearch $taskInfoList "Test[pid]"]
    if { $IdPosition < 0 } {
	testFail $errCtxMsg(13)
        return 0
    }

    # resume the task
    wtxContextResume CONTEXT_TASK $targetSystemAssignedId

    return "$targetSystemAssignedId"
}

#*************************************************************************
#
# loaderFormatGet - Return format used by loader
#
#

proc loaderFormatGet { } {

    global env

    set i 0
    set a(0) "" 

    set cpuFamilyName [lindex [objPathGet foo 2] 1] 

    set name $env(WIND_BASE)/host/resource/loader/omflist

    if { [file exists $name] == 0 } {
          puts stdout " File $name is missing."
          return 
    }

    # open file for reading
    set libFH [open $name r]

    # will stock line which match proc in an array when
    # scanfile will be launch
    while {[gets $libFH line] != -1 } {
	if { [regexp {^[^#]} $line] == 1} {
	  set a($i) $line
	  incr i
	}
    }

    # print procedure name and its arguments
    foreach i [array names a] {
       set info [lrange $a($i) 0 2]
       if {[lindex $info 0] == $cpuFamilyName } {
	    set format [lindex $info 1]
	    break
	    }
    }

    # clean up close context and file
    close $libFH

    return $format

}

#*************************************************************************
#
# cleanUp - clean up each sort of WTX test part
#
# This routine uses a warning handler that prints a warning
# message provided by the occured exeption.
#
# user must pass a list with indice then Id
#
# indice are :
#  
#  b    breakpoint     		- delele breakpoint	
#  c    context			- kill task context
#  f    file			- close open file
#  ml   all tcl memory blocks	- free tcl memory pool (no argument necessary)
#  mb   tcl memory block	- delete tcl memory block
#  ms   target server memory	- free target server memory pool
#  mt   target memory		- free target memory pool
#  o    object file		- unload object file
#  vs   target server vio	- close target server virtual I/O
#  vt   target vio		- close target virtual I/O
#  t    task			- call testTaskCleanup
#  
#

proc cleanUp {args} {
    set status 0

    # save active handler to restore it at the end of the routine
    set oldHandler [wtxErrorHandler [lindex [wtxHandle] 0]]

    # install warningHandler
    wtxErrorHandler [lindex [wtxHandle] 0] warningHandler


    foreach i $args {

	# if it's an indice without arguments
	if {[regexp {(^[a-z]+)} $i tmp]} {
	   switch $tmp {
	       ml 
		{foreach block [memBlockList] {memBlockDelete $block}}

	       default
	        { }
	   }


	# if it's an indice with one or severals arguments
	} else {

	   switch $tmp {
	       b {wtxEventpointDelete $i }

	       c {wtxContextKill $i }

	       f {close $i }

	       fc {
		if {[funcCall close $i]} {
			puts stdout "\nWARNING : Can't close virtual I/O\
			file\t\t\t\t" nonewline
			}
		}

	       ms {wtxMemFree $i }

	       mt {funcCall free $i }

	       mb {memBlockDelete $i } 

	       o { wtxObjModuleUnload $i }

	       vs {wtxClose $i }

	       vt {vioClose [lindex $i 0] [lindex $i 1] }

	       t {testTaskCleanup $i }

	       default {puts stdout "ERROR : indice $tmp doesn't exist" }
	   }
       }
    } 

    # restore old handler
    wtxErrorHandler [lindex [wtxHandle] 0] $oldHandler

 }

#*************************************************************************
#
# mainCommonProc - main Common Proc
#

proc mainCommonProc {testPathFile tgtSvrName timeout invalidAddr tsLoad} {

  global countingFile
  global env
  global O_RDWR
  global FIONREAD
  global VIO_REDIRECT
  global USE_TSFORLOAD

  set varRun  0
  set varPass 0
  set varFail 0
  
  set posixErr(ENOMEM)            0xc
  set posixErr(EACCES)            0xd
  set posixErr(EBADF)             0x9
  set posixErr(ENOENT)            0x2
 
  set O_RDWR                      2
  set FIONREAD                    1
  set VIO_REDIRECT                1
  if { $tsLoad == "1" } {
   set USE_TSFORLOAD               "-ts"
  } else {
   set USE_TSFORLOAD               ""
  }

  set countFile countFile[clock seconds]
 
  if { [file exists [tclPathGet]wtxTestMsg.tcl] == 0 } {
        puts stdout "\n$testErrMsgPath wtxTestMsg.tcl : FILE NOT FOUND\n"
        return
     } else {

     source [tclPathGet]wtxTestMsg.tcl
  }
 
  if { [file exists [tclPathGet]wtxTestErr.tcl] == 0 } {
        puts stdout "\n$testErrMsgPath wtxTestErr.tcl : FILE NOT FOUND\n"
        return
     } else {

     source [tclPathGet]wtxTestErr.tcl

  }

  if { $invalidAddr != 0 } {
 
    if { $invalidAddr == "SUPPRESS" } {
 
        puts stdout "Tests with invalid addresses will not run"
 
    } else {
 
        puts stdout "Tests will use $invalidAddr as invalid address"
 
    }
  }

  if { $env(WIND_HOST_TYPE) == "x86-win32" } {

   set countingFile [open $env(TMP)\\$countFile w+] 

   } else {

   set countingFile [open /tmp/$countFile w+]

  }
    
  puts stdout "" 
  wtxTestLib $tgtSvrName $timeout

  foreach testItem $testPathFile {

   puts stdout $testItem
   set testName [file root [file tail $testItem]]
   if {[string match *Test.tcl [tclPathGet]$testItem.tcl] == 1} {
       catch {eval "\$testName $tgtSvrName $timeout $invalidAddr"} status
       puts stdout ""
    }

  }

  flush $countingFile

  seek $countingFile 0

  gets $countingFile var
  while { ![eof $countingFile] } {
   
    incr varPass [lindex $var 0]
    incr varFail [lindex $var 1]
    gets $countingFile var

  }
  
  set varRun [expr $varPass + $varFail]
  if { $varRun != 0 } {
  	puts stdout "\t\tRUN  : $varRun"
  	puts stdout "\t\tPASS : $varPass"
  	puts stdout "\t\tFAIL : $varFail"
  } else {

	puts stdout "No test matches the string passed as argument," 
        puts stdout "or tests are not supported under this plateform."

  }
  puts stdout ""

  close $countingFile
  if { $env(WIND_HOST_TYPE) == "x86-win32" } {
 
   file delete $env(TMP)\\$countFile
 
   } else {
 
   file delete /tmp/$countFile
 
  }
 
 return 1

}

