# procLib - extension of proc for declaring proc args with comments
#
# Copyright 1998 Wind River Systems, Inc.
#
# modification history
# --------------------
# 01a,15may98,rhp  Initial check-in.

# INTERNAL
# This Tcl fragment, when sourced, redefines `proc' to permit comments
# in the argument list (as required by the WRS coding conventions).
#
# NOROUTINES

if {[info commands __proc__] == ""} {
    rename proc __proc__
    __proc__ proc {name arglist body} {
	regsub -all "#\[^\n]*\n" $arglist "\n" arglist
	eval [list __proc__ $name $arglist $body]
    }
}
