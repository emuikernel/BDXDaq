# serLib.tcl - Tcl script with Common procedures for serial connection 
#              handling. 
#
# Copyright 1997 Wind River Systems, Inc.
#
# modification history
# --------------------
# 01c,11dec01,pch  Improve message for serial open failure
# 01b,31jul97,db   doc tweaks for man page generation.
# 01a,03jun97,sbs  written.
#
#
# DESCRIPTION:
# This module contains all the serial channel handling routines using the
# tcl serial library routines.
#*/

############################################################################
#
# serLibInit - initialise the serial library.
#
# This routine intializes the serial library to be used for all tcl serial
# channel interface. 
#
# SYNOPSIS:
#   serLibInit
#
# PARAMETERS: N/A
#
# RETURNS: N/A
#
# ERRORS: N/A
#
 
proc serLibInit {} {

    set ext [info sharedlibextension]
    set dllDir [wtxPath host [wtxHostType] bin]
    load [file join $dllDir pkgtclserial$ext]
}

############################################################################
#
# serChanOpen - opens a serial channel 
#
# This routine opens a serial channel with given parameters.
#
# SYNOPSIS:
#   serChanOpen device baudRate  
#
# PARAMETERS:
#   device: serial device to be opened.
#   baudRate: baud rate of the channel. 
#
# RETURNS:
# handle or file descriptor for the serial channel or never returns.
# 
# ERRORS:
# "Serial Channel already in use"
#

proc serChanOpen {device baudRate} {

    set fd [tclSerOpen $device $baudRate 8 1 none]
    if {$fd == -1} {
        fatalError "Serial Channel $device cannot be opened.  Already in use?"
    } else {
        return $fd
    }
}

#############################################################################
#
# serChanClose - close a serial channel.
#
# This routine closes a serial channel given the handle for it.
#
# SYNOPSIS:
#   serChanClose handle
#
# PARAMETERS
#   handle: serial channel handle
#
# RETURNS: N/A               
# 
# ERRORS: 
# "serial channel handle or file descriptor invalid"
#

proc serChanClose {handle} {
    set ret1 [tclSerClose $handle] 
    if {$ret1 == -1} {
        warning "serial channel handle or file descriptor invalid." 
    }
}

#############################################################################
#
# serChanFlush - clears serial channel buffer
#
# This routine is used to clear the serial channel buffer.
# 
# SYNOPSIS:
#   serChanFlush handle
#
# PARAMETERS:
#   handle: handle of the serial channel
#
# RETURNS: N/A
#
# ERRORS:
# "serial channel handle or file descriptor invalid"
#


proc serChanFlush {handle} {

    set ret1 [tclSerFlush $handle]
    if {$ret1 == -1} { 
        warning "serial channel handle or file descriptor invalid." 
    }
}

#############################################################################
#
# serChanSend - sends the given string over the serial channel.
#
# This routine sends the given string over the serial channel.
#
# SYNOPSIS:
#   serChanSend handle command
#
# PARAMETERS:
#   handle: serial channel handle
#   command: command string to be sent over the serial channel
#
# RETURNS: 
# 0 if success or never returns.
#
# ERRORS:
# "serial channel send failed"
# 

proc serChanSend {handle command} {

    set ret1 [tclSerSend $handle $command [string length $command]]
    if {$ret1 == -1} { 
        fatalError "serial channel send failed" 
    } else {
        return 0
    }   
}


#############################################################################
#
# serChanReceive - receives an incoming string from the serial channel. 
#
# This routine receives an incoming string from the serial channel until the
# given end pattern is received. It allows to specify the number of times 
# to try to receive the given end pattern. patrnFlag can be specified if 
# the total received pattern has to be returned for any further use. 
# 
# SYNOPSIS:
#   serChanRecv handle bufSize timeout patrnFlag [maxTries]
#
# PARAMETERS:
#   handle: serial channel handle
#   bufSize: maximum buffer size to be received
#   timeout: time out value for tclSerReceive
#   patrnFlag: 1 to return received pattern else 0.
#   maxTries: maximum times to try to receive the pattern
#
# RETURNS: 
# number of bytes received or the received buffer contents or status.
# 
# ERRORS: N/A
#   

proc serChanRecv {handle bufSize timeout endPattern patrnFlag {maxTries 0}} {

    if {$maxTries > 0} {
        set count 0
        set done 0
        while {$count < $maxTries && $done != 1} {
            set ret1 [tclSerReceive $handle $bufSize $timeout $endPattern]
            set res [lindex $ret1 0]
            if {$res > 0} {
                set done 1
            } else {
            incr count
            }
        }
    } else {
    set ret1 [tclSerReceive $handle $bufSize $timeout $endPattern]
    set res [lindex $ret1 0]
    }

    if {$res > 0} {
        if {$patrnFlag == 0} {
            return $res
        } else {
            return $ret1
        }
    } elseif {$res == 0} {
        append retVal "timeoutError "
        append retVal $ret1
        return $retVal 
    } else {
        return "Error"
    }
}

