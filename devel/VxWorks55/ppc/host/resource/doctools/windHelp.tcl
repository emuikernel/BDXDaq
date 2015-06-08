# windHelp.tcl - Tornado Help
#
# Copyright 1998 Wind River Systems, Inc.
#
# modification history
# --------------------
# 01c,01oct98,jmp  renamed netNavigator in htmlBrowser to avoid conflict with
#                  pre Tornado 2.0 netNavigator setting in .wind/launch.tcl.
# 01b,02sep98,fle  changed call to windHelpFileNameFixup in call to
#                  fileNameFixup, and source fileLib.tcl
# 01a,24aug98,jmp  written.
#

#
# DESCRIPTION
#
# This library contains the entry point for windHelpLib.
#
# INCLUDES
#   fileLib.tcl
#   windHelpLib.tcl

# INTERNAL
#
# PROCEDURES:
#
# windHelp		- windHelpLib command line parser

################################################################################
#
# windHelp - windHelpLib command line parser
#
# SYNOPSIS:
#   wtxtcl .../windHelp.tcl [-h] [-keyWord <keyWord>] [-helpDir <helpDir>]
#				[-browser <browser>] <routineOrLibrary>
#
# ARGUMENTS:
#   -h			: print windHelp help
#   -keyWord <keyWord>	: keyWord to differentiate host and target (for example)
#   -helpDir <helpDir>	: help directory (default to $(WIND_BASE)/docs)
#			  or documentation web site entry point.
#   -browser <browser>
#   <routineOrLibrary>	: tool, routine or library name to display
#
# EXAMPLES:
#    wtxtcl .../windHelpLib.tcl i
#				-> display WindSh i() html help.
#
#    wtxtcl .../windHelpLib.tcl -keyWord target i
#				-> display target function i() html help.
#
# RETURNS: N/A
#
# ERRORS: N/A
#
# NOMANUAL
#

proc windHelp {{argv ""} {argc 0}} {

    global docDir
    global htmlBrowser

    # empty args command line

    if { $argc == 0 } {
	return
    }

    set argList $argv
    set argCount [llength $argList]
    set descName ""
    set useKeyWord ""

    # parsing command

    for {set ix 0} {$ix < $argCount} {incr ix} {

        switch -- [lindex $argList $ix] {

            "-h" {			;# command line help message
	    	puts stdout "
usage: wtxtcl \$(WIND_BASE)/host/resource/doctools/windHelpLib.tcl
		\[-h\] \[-keyWord <keyWord>\] \[-helpDir <helpDir>\]
		\[-browser <browser>\] <routineOrLibrary>

Options available are:
-h			: print windHelp help
-keyWord <keyWord>	: keyWord to differentiate host and target (for example)
-helpDir <helpDir>	: help directory (default to \$(WIND_BASE)/docs)
			  or documentation web site entry point.
-browser <browser>
<routineOrLibrary>	: tool, routine or library name to display\n"
            }

	    "-keyWord" {
		incr ix
	    	set useKeyWord [lindex $argList $ix]
	    }

	    "-helpDir" {
	    	incr ix
		set docDir [fileNameFixup [lindex $argList $ix]]

		# re-initialize database with new docDir
		windHelpInit $docDir
	    }

	    "-browser" {
	    	incr ix
		set htmlBrowser [lindex $argList $ix]
	    }

            default {			;# gets files names
		set descName [lindex $argList $ix]
            }
        }
    }

    if {$descName == ""} {
    	return
    }

    set helpLink [windHelpLinkGet -name $descName $useKeyWord]
    if {$helpLink != ""} {
    	windHelpDisplay $helpLink
    }

}

#
# main - the windHelp main entry
#

# source windHelp library
source [wtxPath host resource doctools]windHelpLib.tcl

# parse commande line

windHelp $argv $argc
