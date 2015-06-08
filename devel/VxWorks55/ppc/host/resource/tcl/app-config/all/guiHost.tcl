# guiHost.tcl - tcl default host commands bindings
#
# host.tcl used to contain this code, but it was broken into two files.
# One file that contains the code that does not require tk, and the other
# that does require tk.   
#
# The stuff that runs on MainWin cannot run any tk code.
#
# Copyright 1994-1998 Wind River Systems, Inc.
#
# modification history
# --------------------
# 01c,25mar02,jab  updated tornadoVersion() copyright notice for 2002
# 01b,27feb01,hk   update tornadoVersion{} copyright notice to cover 2001.
# 01a,24mar99,jmp  updated Copyright in tornadoVersion{} (SPR# 25648).
#

##############################################################################
#
# topLevelErrorHandler - WTX top level error handler
#
# This catches Tcl errors that have returned to the top evaluation level.
# A dialog box is posted offering the user the choice of inspecting the 
# error.
#

proc topLevelErrorHandler {err} {
    global errorInfo

    # store the errorInfo in a local variable to avoid having
    # its value overwritten by any possible new error raised
    # during the following question posting

    set errorDescription $errorInfo

    set rslt [noticePost question \
		  "A Tcl evaluation resulted in an error:\n$err" \
		  "Details..." "Continue"]

    if {$rslt} {
	textWindowSet "Tcl Error" $errorDescription
	textWindowPost "Tcl Error"
    }
}

textWindowCreate "Tcl Error" -size 350 300
 
##############################################################################
#
# tornadoVersion - Display a window with Tornado version information.
#
 
proc tornadoVersion {} {
 
    set text "Tornado Development Environment\n \
    \nCopyright 1995-2002 Wind River Systems, Inc.\n \
    \nTornado version [wtxTsVersionGet]"
 
    noticePost info $text
}

