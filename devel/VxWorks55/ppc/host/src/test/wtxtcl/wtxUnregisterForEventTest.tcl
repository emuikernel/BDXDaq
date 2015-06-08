# wtxUnregisterForEventTest.tcl - Tcl script, test WTX_UNREGISTER_FOR_EVENT
#
# Copyright 1995 Wind River Systems, Inc.
#
#
# modification history
# --------------------
# 01p,05mar02,fle  made it exec on its own executable name
# 01o,28jan98,p_b  written
#
# DESCRIPTION
#
# WTX_UNREGISTER_FOR_EVENT  -  WTX unregister for events
#
#    WARNING : This test must use two wtxtcl tools for certain part.
#


#*****************************************************************************
#
# wtxUnregisterForEventTest1 -
#
# Verify if wtxUnregisterForEvent disable no events 
#

proc wtxUnregisterForEventTest1 {name Msg} {

    global env
    global EvMsg
    global errEvMsg

    puts stdout "$Msg" nonewline
     
    set err 0
    set nameFile [tclPathGet]genListEvent.tcl

    # verify if genEvent exists in right directory
    set nameTmpFile [initEventsFile $nameFile "wtxUnregisterForEventTest1" $name]

    # disable all UNKNOWN events
    set status [wtxRegisterForEvent .*]
    if {$status == ""} {
	return
    }
    set status [wtxUnregisterForEvent ^UNKNOWN.*]
    if {$status == ""} {
    	return
    }

    # run an other wtxtcl, which run genListEvent script
    # the genListEvent script attach to target server and then detach 
    
    exec [wtxtclPathGet] $nameTmpFile ;# 2> /dev/null 

    set listEvents [wtxEventListGet]

    set result [string match *TOOL_ATTACH* $listEvents]
    if { $result == 0 } {
      set err 1
    }

    set result [string match *EVENTTEST01* $listEvents]
    if { $result == 0 } {
          set err 1
    }

    set result [string match *TOOL_DETACH* $listEvents]
    if { $result == 0 } {
	  set err 1
    }

    if { $err != 0 || $status != 0 } { 
	 testFail $errEvMsg(1)
	 return
    }

    # clean up
    if {[file exists $nameTmpFile]} {
        file delete $nameTmpFile
    }


    wtxTestPass
}


#*****************************************************************************
#
# wtxUnregisterForEventTest2 -
#
# Verify if wtxUnregisterForEvent disable EVENTTEST?? events 
#

proc wtxUnregisterForEventTest2 {name Msg} {

    global EvMsg
    global errEvMsg

    puts stdout "$Msg" nonewline

    set err 0
    set nameFile [tclPathGet]genListEvent.tcl

    # verify if genEvent exists in right directory
    set nameTmpFile [initEventsFile $nameFile "wtxUngsterForEventTest2" $name]

    # disable *EVENTTEST* event
    set status [wtxRegisterForEvent .*]
    if {$status == ""} {
        return
    }

    set status [wtxUnregisterForEvent ^EVENTTEST.*]
    if {$status == ""} {
        return
    }

    # run an other wtxtcl, which run genListEvent script
    # the genListEvent script attach to target server and then detach 
    
    exec [wtxtclPathGet] $nameTmpFile ;# 2> /dev/null 

    # Get the event list
    set listEvents [wtxEventListGet]

    # Check if TOOL_ATTACH event is in the list
    set result [string match *TOOL_ATTACH* $listEvents]
    if { $result == 0 } {
      set err 1
    }

    # Check if TOOL_DETACH event is in the list
    set result [string match *TOOL_DETACH* $listEvents]
    if { $result == 0 } {
          set err 1
    }
 
    # Check if *EVENTTEST* event is in the list, if yes then ERROR
    set result [string match *EVENTTEST* $listEvents]
    if { $result == 1 } {
          set err 1
    }

    if { $err != 0 || $status != 0 } { 
	 testFail $errEvMsg(2)
	 return
     }

    # clean up
    if {[file exists $nameTmpFile]} {
        file delete $nameTmpFile
    }

    wtxTestPass
}

#*****************************************************************************
#
# wtxUnregisterForEventTest -
#
# Initialization of variables.
# Initialization of variables that contain the description of the tests.
# Call the test scripts.
#

proc wtxUnregisterForEventTest {tgtsvrName timeout invalidAddr} {

    global UnevMsg

    if { [llength $tgtsvrName] == 0 } {
        puts stdout "usage : wtxUnregisterForEventTest targetServerName\
				timeout invalidAddr"
        return
    }

    # variables initializations
    set NBTESTS 2
    set serviceType 2

    # connection to the target server
    wtxToolAttachCall $tgtsvrName $serviceType $timeout

    wtxErrorHandler [lindex [wtxHandle] 0] errorHandler
    
    # call the test procedures
    wtxUnregisterForEventTest1 $tgtsvrName $UnevMsg(1)
    wtxUnregisterForEventTest2 $tgtsvrName $UnevMsg(2)

    # deconnection
    wtxToolDetachCall "wtxUnregisterForEventTest"
}

################################################################################
#
# wtxtclPathGet - get path of wtxtcl executable currently used
#
# RETURNS: The path to this wtxtcl executable, without the extension
#

proc wtxtclPathGet {} {

    return [file rootname [info nameofexecutable]]
}
