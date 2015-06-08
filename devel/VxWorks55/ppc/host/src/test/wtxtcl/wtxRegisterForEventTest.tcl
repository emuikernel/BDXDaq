# wtxRegisterForEventTest.tcl - Tcl script, test WTX_REGISTER_FOR_EVENT
#
# Copyright 1995 Wind River Systems, Inc.
#
#
# modification history
# --------------------
# 01p,05mar02,fle  made it exec on its own executable name
# 01o,28jan98,p_b  rewritten
# 01n,20oct95,jmp  added invalidAddr argument to wtxRegisterForEventTest.
# 01m,21aug95,jmp  added timeout to wtxToolAttachCall in
#		   wtxRegisterForEventTest.
#                  changed tests control method, now uses wtxErrorHandler.
# 01l,09jul95,jcf  resolved permission problem.
# 01k,26jun95,f_v  added test in front of rm -f
# 01j,23jun95,f_v  added wtxTestRun call
# 01i,09jun95,f_v  added rm -f before cp
# 01h,22may95,jcf  name revision.
# 01g,17may95,f_v  runs genEvent in the foreground but redirect stderr
# 01f,16may95,f_v  runs genEvent in the background
# 01e,15may95,c_s  now runs genEvent in the foreground, so that it will be
#                    synchronous with the test.
# 01d,28mar95,f_v  added wtxRegisterForEvent
# 01c,14mar95,f_v  change file path
# 01b,15feb95,f_v  change test5 and test6 file in genEvent and genListEvent. 
# 01a,09feb95,f_v  written. 
#
#
#
# DESCRIPTION
#
# WTX_REGISTER_FOR_EVENT  -  WTX register for events
#
#    This routine allows a tool to register for a particular type
#    of  events  that  will  then  be obtained with wtxEventGet()
#    requests.  Only the events that a tool has  registered  for,
#    will be notified to the the tool.
#
#    WARNING : This test uses two wtxtcl tools for certain part.
#


#*****************************************************************************
#
# wtxRegisterForEventTest1 -
#
# Verify if wtxRegisterForEvent enable all events 
#

proc wtxRegisterForEventTest1 {name Msg} {

    global env
    global EvMsg
    global errEvMsg

    puts stdout "$Msg" nonewline
     
    set err 0
    set nameFile [tclPathGet]genListEvent.tcl

    # verify if genEvent exists in right directory
    set nameTmpFile [initEventsFile $nameFile "wtxRegisterForEventTest1" $name]

    # enable all events
    set status [wtxRegisterForEvent .*]
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
# wtxRegisterForEventTest2 -
#
# Verify if wtxRegisterForEvent disable all events except EVENTTEST?? events 
#

proc wtxRegisterForEventTest2 {name Msg} {

    global EvMsg
    global errEvMsg

    puts stdout "$Msg" nonewline

    set err 0
    set nameFile [tclPathGet]genListEvent.tcl

    # verify if genListEvent exists in right directory
    set nameTmpFile [initEventsFile $nameFile "wtxRegisterForEventTest2" $name]

    # enable EVENTTEST?? event
    set status [wtxRegisterForEvent ^EVENTTEST.*]

    # run an other wtxtcl, which run genListEvent script
    # the genListEvent script attach to target server and then detach 

    exec [wtxtclPathGet] $nameTmpFile ;# 2> /dev/null 

    # Get the event list
    set listEvents [wtxEventListGet]
 
    # Check if TOOL_ATTACH event is in the list, if yes then ERROR
    set result [string match *TOOL_ATTACH* $listEvents]
    if { $result == 1 } {
      set err 1
    }
 
    # Check if TOOL_DETACH event is in the list, if yes then ERROR
    set result [string match *TOOL_DETACH* $listEvents]
    if { $result == 1 } {
          set err 1
    }
 
    # Check if EVENTTEST?? events is in the list
    set result [string match *EVENTTEST* $listEvents]
    if { $result == 0 } {
          set err 1
    }

    # clean up
    if {[file exists $nameTmpFile]} {
        file delete $nameTmpFile 
    }

    if { $err != 0 || $status != 0 } { 
	 testFail $errEvMsg(2)
	 return
     }

    wtxTestPass
}

#*****************************************************************************
#
# wtxRegisterForEventTest -
#
# Initialization of variables.
# Initialization of variables that contain the description of the tests.
# Call the test scripts.
#

proc wtxRegisterForEventTest {tgtsvrName timeout invalidAddr} {

    global vEvMsg

    if { [llength $tgtsvrName] == 0 } {
        puts stdout "usage : wtxRegisterForEventTest targetServerName\
				timeout invalidAddr"
        return
    }

    # variables initializations
    set serviceType 2
    set NBTESTS 2

    # connection to the target server
    wtxToolAttachCall $tgtsvrName $serviceType $timeout
    # enable all events
    # catch {wtxRegisterForEvent .*} st

    wtxErrorHandler [lindex [wtxHandle] 0] errorHandler
    
    # call the test procedures
    wtxRegisterForEventTest1 $tgtsvrName $vEvMsg(1)
    wtxRegisterForEventTest2 $tgtsvrName $vEvMsg(2)

    # deconnection
    wtxToolDetachCall "wtxRegisterForEventTest"
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
