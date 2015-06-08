# clockLib.tcl - VxWorks clock test library
#
# Copyright 1997 Wind River Systems, Inc.
#
# modification history
# --------------------
# 01g,20apr98,p_b  Merge from ARM to Tornado 2_0_x
# 01f,17oct97,sbs  changed fatalErrors to warnings where necessary and 
#                  skips the test if clock rate setting fails.  
# 01e,04sep97,sbs  added more error checking. 
# 01d,28aug97,sbs  added looping for shParse in case of error.
# 01c,11aug97,db   added log messages.    
# 01b,31jul97,db   doc tweaks for man page generation.
# 01a,15mar97,sbs  written.
#
#
# DESCRIPTION
# This is a library of tcl routines for testing VxWorks clocks.
#
# clockRate() tests the clock rate.  It counts the clock interrupts in two
# intervals timed by the host, calculates the average interrupt rate,
# displays the percent error, and determines whether the rate is within
# ten percent of that expected.
#
# clockDisable() tests whether clock interrupts cease when the disable
# function is called.  clockParm() tests the parameter checking of the rate
# setting routine, and clockSetGet() tests the return value of the rate
# getting routine.
#

################################################################################
#
# clockRate - test the average clock interrupt rate
#
# SYNOPSIS:
#   clockRate clock rate name
#
# PARAMETERS:
#   clock: clock to be tested(eg: system Clock called as sysClk)
#   rate: rate at which it has to be tested
#   name: name of the clock for diagnostic messages
#
# RETURNS: N/A
#
# ERRORS: N/A
#
proc clockRate {clock rate name} {

    # set maximum trials to make in case of error

    set maxTries 5

    # create/initialize variable

    set cmd " clockTicks=0 "
    set ret [getWindShRet $cmd $maxTries]

    if {$ret == -1} {
        fatalError "initialization of clockTicks variable failed"
    }

    set cmd " sys${clock}RateSet($rate) "
    set ret [getWindShRet $cmd $maxTries]

    if {$ret == -1} {
       warning "sys${clock}RateSet failed to set rate at: $rate"
       testResult 2 "$name at $rate hertz"
       return
    }

    # count ticks in 10 second interval and 130 second interval

    msleep 1000

    set cmd " clockTicks "
    set ticks1 [getWindShRet $cmd $maxTries]

    send_user "   1 second  through $name test: ticks1=$ticks1" 0

    if {$ticks1 <= 0} {
        warning "$name failed to increment attached variable"
    }

    for {set i 0} {$i < 10} {incr i} { 
        msleep 1000
    }

    set cmd " clockTicks "
    set ticks2 [getWindShRet $cmd $maxTries]

    send_user "  11 seconds through $name test: ticks2=$ticks2" 0

    if {$ticks2 <= 0} {  
        warning "$name failed to increment attached variable" 
    }

    for {set i 0} {$i < 130} {incr i} {
        msleep 1000
    }

    set cmd " clockTicks "
    set ticks3 [getWindShRet $cmd $maxTries]

    send_user " 141 seconds through $name test: ticks3=$ticks3" 0
 
    if {$ticks3 <= 0} {
        warning "$name failed to increment attached variable" 
    }


    # calculate corrected ticks for 120 second interval

    set correct2 [expr ($ticks3-$ticks2)-($ticks2-$ticks1)]
    send_user "corrected value for 120 ticks is $correct2" 0

    # calculate ticks divided by rate -- should be close to 120

    set result1 [expr (((($correct2*10)/($rate))+5)/10)]
    send_user "ticks divided by rate = $result1" 0

    # calculate the measured rate and the percent error

    set mRate [expr (((($correct2*10)/(120))+5)/10)]
    set step1 [expr ((($mRate-$rate)*1000)/$rate)]
    if { $step1 < 0} { 
        set percentErr [expr ($step1-5)/10] 
    } else { 
        set percentErr [expr ($step1+5)/10]
    }

    # pass if within +/- 10% of target rate

    set result [format "%s at %s hertz (rate = %s  error = %d%%)" \
                        $name $rate $mRate $percentErr]
    testResult [expr {($result1 > 108) && ($result1 < 132)}] \
               $result
       
}

################################################################################
#
# clockDisable - test disabling of a clock
#
# Enable the clock before calling this routine.
#
# SYNOPSIS:
#   clockDisable clock name 
#
# PARAMETERS:
#   clock: clock to be tested
#   name: name of the clock
#
# RETURNS: N/A
#
# ERRORS: N/A
#

proc clockDisable {clock name} {

    # set maximum trials to make in case of error

    set maxTries 5

    # create/initialize variable

    set cmd " clockTicks = 0 "
    set ret [getWindShRet $cmd $maxTries]

    if {$ret == -1} {
        warning "clockTicks variable initialization failed"
    }

    # disable the clock

    set cmd " sys${clock}Disable "
    set ret [getWindShRet $cmd $maxTries]

    if {$ret == -1} {
        fatalError "sys${clock}Disable failed"
    }

    # read the test counter

    set cmd " clockTicks "
    set retA [getWindShRet $cmd $maxTries]

    if {$retA == -1} {
        warning "clockTicks variable value get failed"
    }

    # wait 2 seconds

    msleep 2000 

    # read the test counter again

    set cmd " clockTicks "
    set retB [getWindShRet $cmd $maxTries]

    if {$retB == -1} {
        warning "clockTicks variable value get failed"
    }


    set result [format "sys%sDisable() disables %s" $clock $name] 
    testResult [expr {($retA == $retB) && ($retA != -1)}] \
               $result

}


###############################################################################
#
# clockParm - test parameter checking of sysClkRateSet()
#
# SYNOPSIS:
#   clockParm clock minRate maxRate
#
# PARAMETERS:
#   clock: clock to be tested
#   minRate: minimum rate parameter
#   maxRate: maximum rate parameter
#
# RETURNS: N/A
#
# ERRORS: N/A
#

proc clockParm {clock minRate maxRate} {

    # set maximum trials to make in case of error

    set maxTries 5

    # attempt to set several clock rates; check the return values

    set cmd " sys${clock}RateSet $minRate "
    set retA [getWindShRet $cmd $maxTries]

    if {$retA == -1} {
        warning "sys${clock}RateSet for $minRate failed"
    }

    set cmd " sys${clock}RateSet $maxRate "
    set retB [getWindShRet $cmd $maxTries]

    if {$retB == -1} {
        warning "sys${clock}RateSet for $maxRate failed"
    }

    set retC [shParse  sys${clock}RateSet(0)  ]
    set retD [shParse  sys${clock}RateSet(-1)  ]

    if {$retD != -1} {
        send_user "shParse for sys${clock}RateSet(-1) failed... retrying" 0
        set retD [shParse  sys${clock}RateSet(-1)  ]
    }

    set retE [shParse  sys${clock}RateSet([expr $maxRate+1])  ]

    if {$retE != -1} {
        send_user "shParse for sys${clock}RateSet([expr $maxRate+1]) \
                   failed... retrying" 0
        set retE [shParse  sys${clock}RateSet([expr $maxRate+1])  ]
    }

    set retF [shParse  sys${clock}RateSet([expr $minRate-1])  ]

    if {$retF != -1} {
        send_user "shParse for sys${clock}RateSet([expr $minRate-1]) \
                   failed... retrying" 0
        set retF [shParse  sys${clock}RateSet([expr $minRate-1])  ]
    }


    set result [format "sys%sRateSet() parameter checking" $clock]
    testResult [expr {($retA ==  0) && ($retB ==  0) && \
                      ($retC ==  -1) && ($retD == -1) && \
                      ($retE ==  -1) && ($retF == -1)}] \
               $result

}

################################################################################
#
# clockSetGet - test setting and getting of clock rate
#
# Enable the clock before calling this routine.
#
# SYNOPSIS:
#   clockSetGet clock validRate
#
# PARAMETERS:
#   clock: name of the clock
#   validRate: a valid rate to set the clock
#
# RETURNS: N/A
#
# ERRORS: N/A
#

proc clockSetGet {clock validRate} { 

    # set maximum trials to make in case of error

    set maxTries 5

    # set a valid clock rate, then read the clock rate

    set cmd " sys${clock}RateSet($validRate) "
    set retA [getWindShRet $cmd $maxTries]

    if {$retA == -1} {
        warning "sys${clock}RateSet for $validRate failed"
    }

    set cmd " sys${clock}RateGet "
    set retB [getWindShRet $cmd $maxTries]

    if {$retB == -1} {
        warning "sys${clock}RateGet failed"
    }

    # attempt to set an invalid rate, then read the clock rate

    set retC [shParse sys${clock}RateSet(-5)]

    if {$retC != -1} {
        send_user "shParse for sys${clock}RateSet(-5) failed... retrying" 0
        set retC [shParse sys${clock}RateSet(-5)]
    }


    msleep 500

    set cmd " sys${clock}RateGet "
    set retD [getWindShRet $cmd $maxTries]

    if {$retD == -1} {
        warning "sys${clock}RateGet failed"
    }


    set result [format "sys%sRateGet() return value" $clock]
    testResult [expr {($retB == $validRate) && ($retD == $validRate)}] \
               $result

}
