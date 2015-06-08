# wtxEventListGetTest.tcl - Tcl script, test WTX_EVENT_LIST_GET
#
# Copyright 1998 Wind River Systems, Inc.
#
#
# modification history
# --------------------
# 01b,05mar02,fle  made it exec on its own executable name
# 01a,28jan98,p_b  written
#

#
# DESCRIPTION
#
# WTX_EVENT_GET  - WTX get an event list
#


#*****************************************************************************
#
# wtxEventListGetTest1 -
#
# Verify if wtxEventListGet get an event list that contains EVENTTEST01,02 and 03
#

proc wtxEventListGetTest1 {name Msg} {

    global env
    global EvMsg
    global errEvMsg

    puts stdout "$Msg" nonewline

    set err 0
    set nameFile [tclPathGet]genListEvent.tcl

    # verify if genListEvent.tcl exists in right directory
    set nameTmpFile [initEventsFile $nameFile "wtxEventListGet1" $name]

    # run an other wtxtcl, which run genListEvent script
    # the genListEvent script attach to target server and then detach
    
    exec [wtxtclPathGet] $nameTmpFile ;# 2> /dev/null 

    # Get event list
    set eventInfo [wtxEventListGet]

    set result [string match *EVENTTEST01* $eventInfo]
    if { $result == 0 } {
      set err 1
     }

    set result [string match *EVENTTEST02* $eventInfo]
    if { $result == 0 } {
      set err 1
     }

    set result [string match *EVENTTEST03* $eventInfo]
    if { $result == 0 } {
      set err 1
     }

   # clean up
   file delete $nameTmpFile
   
   if { $err != 0 } { 
	    testFail $errEvMsg(8)
	    return
        }

    wtxTestPass
}

#*****************************************************************************
#
# wtxEventListGetTest -
#
# Initialization of variables.
# Initialization of variables that contain the description of the tests.
# Call the test scripts.
#

proc wtxEventListGetTest {tgtsvrName timeout invalidAddr} {
    
    global vEvMsg

    if { [llength $tgtsvrName] == 0 } {
        puts stdout "usage : wtxEventListGetTest targetServerName\
				timeout invalidAddr"
        return
    }

    # variables initializations
    set serviceType 2

    # connection to the target server
    wtxToolAttachCall $tgtsvrName $serviceType $timeout
    # enable events
    catch {wtxRegisterForEvent .*} st

    wtxErrorHandler [lindex [wtxHandle] 0] errorHandler

    # call the test procedures
    wtxEventListGetTest1 $tgtsvrName $vEvMsg(10)

    # deconnection
    wtxToolDetachCall "wtxEventListGetTest"
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
