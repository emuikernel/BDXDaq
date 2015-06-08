# widl.tcl - IDL-compiler-driver and proxy/stub generator
#
# Copyright (c) 2001 Wind River Systems, Inc.
#
# modification history
# --------------------
# 01a,23oct01,nel  created
#
# SYNOPSIS:
# .tS
# wtxtcl $WIND_BASE/host/x86-win32/bin/widl.tcl <options> <inputFile>
# .tE
#
# <inputFile> is the name of the IDL source file, or a type-library.
# <options> can be any combination of:-
#     -I <include-dir> : use extra include directory
#     -noh : Do not generate header files.
#     -h <headerDir> : Generate header files in this directory
#     -nops : Do not generate proxy/stub code.
#     -ps <psDir> : Generate proxy/stub code in this directory
#     -noi : Do not generate _i.c files.
#     -dep : Generate GNU style dependencies.
#
# Writes the proxy/stub code, post-processes the MIDL-output header
# file to remove all includes, and to substitute a single include
# of 'dcomLib.h' instead.
#
##########################################################################

set widlOptions [lrange $argv 0 [expr [llength $argv]]]

set widlExec $env(WIND_BASE)/host/$env(WIND_HOST_TYPE)/bin/widl 

set widlCmd "exec $widlExec $widlOptions"

set errCode [catch {eval $widlCmd} widlOutput]
puts $widlOutput
exit $errCode
