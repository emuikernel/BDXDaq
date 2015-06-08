# wtxEventAddTest.tcl - Tcl script, test WTX_EVENT_ADD
#
# Copyright 1995 Wind River Systems, Inc.
#
#
# modification history
# --------------------
# 01o,05mar02,fle  made it exec on its own executable name
# 01n,28jan98,p_b  added tclPathGet & initEventsFile 
#                  replace "exec cmd" with TCL commands 
# 01m,20oct95,jmp  added invalidAddr argument to wtxEventAddTest.
# 01l,22aug95,jmp  added timeout to wtxToolAttachCall in wtxEventAddTest.
#                  changed tests control method, now uses wtxErrorHandler.
# 01k,09jul95,jcf  resolved permission problem.
# 01j,26jun95,f_v  added test in front of rm -f
# 01i,23jun95,f_v  added wtxTestRun call
# 01h,09jun95,f_v  added rm -f before cp
# 01g,22may95,jcf  name revision.
# 01f,17may95,f_v  runs genEvent in the foreground but redirect stderr
# 01e,16may95,f_v  runs genEvent in the background
# 01d,15may95,c_s  now runs genEvent in the foreground, so that it will be
#                    synchronous with the test.
# 01c,28mar95,f_v  added wtxRegisterForEvent, added clean-up section
# 01b,14mar95,f_v  changed file path. 
# 01a,16feb95,f_v  written. 
#
#
#
# DESCRIPTION
#
# WTX_EVENT_ADD  -  WTX add event
#
#    
#    This routine adds an event to the Target Server tools  event
#    list.   Only  the tools that have registered for the type of
#    event added will be able to get the events.
#
#    WARNING : This test must use two wtxtcl tools for certain part.
#


#*****************************************************************************
#
# wtxEventAddTest1 -
#
# Verify if wtxEventAdd add a event
#

proc wtxEventAddTest1 {name Msg} {

    global env
    global EvMsg
    global errEvMsg

    puts stdout "$Msg" nonewline
    set nameFile [tclPathGet]genEvent2.tcl
    set err 0

    # Check if  $nameFile exists in right directory and preparation
    set nameTmpFile [initEventsFile $nameFile "wtxEventAddTest1" $name]

    # run an other wtxtcl, which run genEvent2 script
    # the genEvent2 script attach to target server and then detach 

    exec [wtxtclPathGet] $nameTmpFile  ;# 2> /dev/null 

    # loop until EVENTTEST arrives
    set eventInfo [wtxEventGet]
    while { [string compare [lindex $eventInfo 0] EVENTTEST]} {
           set eventInfo [wtxEventGet]
           if { $eventInfo == "" }  {
                  set err 1
                  break
           }
        }
 
    # clean up
    if {[file exists $nameTmpFile]} {
	file delete $nameTmpFile 
    }

    if { $err != 0 } { 
	 testFail $errEvMsg(4)
	 return
     }

    wtxTestPass
}

#*****************************************************************************
#
# wtxEventAddTest2 - Verify if wtxEventAdd add a event of 256 characters
#

proc wtxEventAddTest2 {name Msg} {

    global env
    global EvMsg
    global errEvMsg

    puts stdout "$Msg" nonewline
    set nameFile [tclPathGet]genEvent3.tcl
    set err 0

    # Check if $nameFile exists in right directory
    set nameTmpFile [initEventsFile $nameFile "wtxEventAddTest2" $name]

    # Run an other wtxtcl, which run genEvent3 script
    # the genEvent3 script attach to target server and then detach 

    exec [wtxtclPathGet] $nameTmpFile ;# 2> /dev/null

    # Creation of event name (256 characters the same as genEvent3.tcl)
    set event ""
    for { set i 0 } { $i < 255 } { incr i } {
    lappend event x 
     }
    set event [join $event {}]

    # loop until EVENTTEST arrives
    set eventInfo [wtxEventGet]
        while { [string compare [lindex $eventInfo 0] $event]} {
           set eventInfo [wtxEventGet]
           if { $eventInfo == "" }  {
                  set err 1
                  break
              }
           }

    # clean up
    if {[file exists $nameTmpFile]} {
	file delete $nameTmpFile
    }

    if { $err != 0 } { 
         testFail $errEvMsg(5)
         return
     }

    wtxTestPass
}


#*****************************************************************************
#
# wtxEventAddTest -
#
# Initialization of variables.
# Initialization of variables that contain the description of the tests.
# Call the test scripts.
#

proc wtxEventAddTest {tgtsvrName timeout invalidAddr} {

    global vEvMsg

    if { [llength $tgtsvrName] == 0 } {
        puts stdout "usage : wtxEventAddTest targetServerName\
				timeout invalidAddr"
        return
    }

    # variables initializations
    set NBTESTS 2
    set serviceType 2

    # connection to the target server
    wtxToolAttachCall $tgtsvrName $serviceType $timeout
    # Enable all events
    catch {wtxRegisterForEvent .*} st
    wtxErrorHandler [lindex [wtxHandle] 0] errorHandler

    # call the test procedures
    wtxEventAddTest1 $tgtsvrName $vEvMsg(5)
    wtxEventAddTest2 $tgtsvrName $vEvMsg(6)

    # deconnection
    wtxToolDetachCall "wtxEventAddTest"
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
