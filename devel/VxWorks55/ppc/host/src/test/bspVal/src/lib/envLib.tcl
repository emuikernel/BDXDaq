# envLib.tcl - read environment variables library
#
# Copyright 1997 Wind River Systems, Inc.
#
# modification history
# --------------------
# 01b,31jul97,db   doc tweaks for man page generation.
# 01a,17mar97,sbs  taken form lib routines of Bsp Validation Test and 
#                  modified as needed.
#
# DESCRIPTION:
# These routines provide the BSP tests with an interface to read environment
# variables.  readEnvOptional() returns a default value if the named
# environment variable is not set.  readEnvRequired() exits with an error
# message if the environment variable is not set.
#
#*/

################################################################################
#
# readEnvOptional - read an optional environment variable
#
# SYNOPSIS:
#   readEnvOptional readName defaultValue 
#
# PARAMETERS:
#   readName: name of the environment variable
#   defaultValue: default value to returned if the required variable is not set
#
# RETURNS:
# The value of <readName> if it is defined, else <defaultValue>.
#
# ERRORS: N/A
#

proc readEnvOptional {readName defaultValue} {
    global env

#   search for readName in the env array
    set searchId [array startsearch env]
    while {[array anymore env $searchId]} { 
	set nextElement [array nextelement env $searchId]
	if {[string match $readName $nextElement]} {
	    array donesearch env $searchId
	    return $env($nextElement)
	}
    }
    
#   handle the case where readName is not defined
    array donesearch env $searchId

    return $defaultValue
    }

################################################################################
#
# readEnvRequired - read a required environment variable
#
# SYNOPSIS:
#   readEnvRequired readName
#
# PARAMETERS:
#   readName: name of the environment variable
#
# RETURNS: 
# The value of <readName> if it is defined, else never returns.
#
# ERRORS:
# "<readName> not defined"
#

proc readEnvRequired {readName} {
    global env

#   search for readName in the env array
    set searchId [array startsearch env]
    while {[array anymore env $searchId]} {
	set nextElement [array nextelement env $searchId]
	if {[string match $readName $nextElement]} {
	    array donesearch env $searchId
	    return $env($nextElement)
	}
    }
    
#   handle the case where readName is not defined
    array donesearch env $searchId

    fatalError "$readName not defined"
    }

proc getEnvGlob {arg} {
    global env
    set result ""
    foreach i [array names env] {
	if [regexp $arg $i] {
	    append result $i=$env($i) "\n"
	}
    }
    return $result
}

