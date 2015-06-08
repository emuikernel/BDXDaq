# Log.tcl - Wind River Tcl Log facilities

# Copyright 2001 Wind River Systems, Inc.

#
# modification history
# --------------------
# 01a,21feb01,fle  written
#

#
# DESCRIPTION
#
# This namespace defines several Log facilities. They allow to have a better
# trace for all procedures calling for Log procedures.
#

namespace eval Log {

    variable mFileId	"stderr"		;# file id to write log to
    variable mFilePath	""			;# log file path
    variable mIndent	0			;# current indent level
    variable mVerbose	0			;# verbose mode
    variable mIndentStr	"  "			;# indent str pattern
    variable mVerbose	"off"			;# log verbose mode

}

################################################################################
#
# Log::close - closes the log file if needed
#
# SYNOPSIS
# \cs
#   Log::close
# \ce
#
# PARAMETERS: N/A
#
# This procedure closes the log file if needed. IT is not needed to close the
# log file if it is "stdin", "stdout" or "stderr". To know what is the log file,
# call for `Log::file' procedure.
#
# In general, it is necessary to call this procedure only if the log file was
# changed using `Log::file' <fileName>.
#
# RETURNS: N/A
#
# SEE ALSO: Log::file
#

proc Log::close {} {

    variable mFileId			;# log file Id

    if {($mFileId != "stdin") && ($mFileId != "stdout") &&
	($mFileId != "stderr") } {

	::close $mFileId
    }
}


############################################################################
#
# Log::error - log an error message
#
# SYNOPSIS
# \cs
#   Log::error <str> [<indentIncr>]
# \ce
#
# PARAMETERS
#   str : the string message to log after the error message
#   indentIncr : indent incrementation to apply to current indent level
#
# This procedure logs an error message on the standard output, whatever the
# verbose mode for this Log namespace is
#
# It applies the error string type, the error message is prepended with
# a line made or *** ERROR ***
#
# RETURNS: N/A
#
# SEE ALSO: Log::msg
#

proc Log::error {str {indentIncr 0}} {

    variable mFileId			;# file id to write log to
    variable mIndent			;# current indent level
    variable mVerbose			;# verbose mode

    # decrease indent before printing if it is negative

    if {$indentIncr < 0} {
	incr mIndent $indentIncr
    }

    puts $mFileId [format "%s*** ERROR ***\n%s%s" [indentStrGet] \
			  [indentStrGet] $str]

    # increase indent level after printing if it is positive

    if {$indentIncr > 0} {
	incr mIndent $indentIncr
    }
}

################################################################################
#
# Log::file - get / set the log file
#
# SYNOPSIS
# \cs
#   Log::file [<filePath>]
# \ce
#
# PARAMETERS
#   filePath : the file to use to log messages or errors in
#   mode : set to "+" to append log to already existing <filePath>
#
# This procedure returns the file used to log messages or errors.
#
# By default, the log file is `stderr', but on can modify this by specifying a
# <filePath> when calling `Log::file'. The file is then opened in write mode
# unless <mode> is set to "+".
#
# If the <filePath> open operation fails (file can't be written or any other
# error), the default `stderr' is restored.
#
# RETURNS: the file Path to write log to
#

proc Log::file {{filePath ""} {mode ""}} {

    variable mFileId			;# file id to log in
    variable mFilePath			;# file path to log in

    if {$filePath != ""} {

	if {($filePath != "stderr") && ($filePath != "stdout") && 
	    ($filePath != "stdin")} {

	    if {$mode == "+"} {

		# open the new file in append mode

		if {! [catch {open $filePath "a"} fileId]} {

		    # now close previous file Id if it was not any of the std
		    # file streams

		    if {($mFileId != "stderr") &&
			($mFileId != "stdout") && ($mFileId != "stdin")} {

			close $mFileId
		    }

		    set mFileId $fileId
		    set mFilePath $filePath
		}
	    } else {

		# open the new file in write mode

		if {! [catch {open $filePath "w"} fileId]} {

		    # now close previous file Id if it was not any of the std
		    # file streams

		    if {($mFileId != "stderr") &&
			($mFileId != "stdout") && ($mFileId != "stdin")} {

			close $mFileId
		    }

		    set mFileId $fileId
		    set mFilePath $filePath
		}
	    }
	} else {

	    # it is one of the standard streams

	    set mFilePath $filePath
	    set mFileId $filePath
	}
    }

    return $mFilePath
}

############################################################################
#
# Log::indent - get / set the Log indent level
#
# SYNOPSIS
# \cs
# \ce
#
# PARAMETERS
#   indentLevel : the indent level to set
#
# This procedure returns the current indentation level for that Log
# namespace. If <indentLevel> is provided and is a valid integer, then this
# Log namespace indent level is previously set to <indentLevel>.
#
# RETURNS: current indent level for that Log namespace
#

proc Log::indent {{indentLevel ""}} {

    variable mIndent			;# current indent level

    if {$indentLevel != ""} {

	if {[scan $indentLevel "%d" newIndentLevel] == 0} {

	    set mIndent $newIndentLevel
	}
    }

    return $mIndent
}

############################################################################
#
# Log::indentStr - set / get the indent string for that Log namespace
#
# SYNOPSIS
# \cs
#   Log::indentStr <str>
# \ce
#
# PARAMETERS
#   str : the string to set indentation string to
#
# This procedure returns this Log namespace indentation string.
#
# If the <str> parameter is provided (and is different from ""), this Log
# namespace intentation string is set to <str>.
#
# RETURNS
#

proc Log::indentStr {{str ""}} {

    variable mIndentStr			;# indentation str

    if {$str != ""} {

	set mIndentStr $str
    }

    return $mIndentStr
}

############################################################################
#
# Log::msg - log a message if verbose mode is on
#
# SYNOPSIS
# \cs
#   Log::msg <str> [<indentIncr>]
# \ce
#
# PARAMETERS:
#   str : the message to log
#   indentIncr : the indentation increment for that message
#
# This procedure prints the given <str> on the standard output if the
# verbose mode is on.
#
# Log indentation level is incremented of <indentIncr> after the message has
# been printed.
#
# RETURNS: N/A
#

proc Log::msg {str {indentIncr 0}} {

    variable mFileId			;# file id to write log to
    variable mIndent			;# current indent level
    variable mVerbose			;# verbose mode

    if {$mVerbose == "on"} {

	if {$indentIncr < 0} {
	    incr mIndent $indentIncr
	    puts $mFileId [format "%s%s" [indentStrGet] $str]
	} else {
	    puts $mFileId [format "%s%s" [indentStrGet] $str]
	    incr mIndent $indentIncr
	}
    }
}

############################################################################
#
# Log::verbose - get / set the verbose mode for that Log namespace
#
# SYNOPSIS
# \cs
#   Log::verbose [<verboseMode>]
# \ce
#
# PARAMETERS:
#   verboseMode : can be set to on or off
#
# This procedure turn on or off the verbose mode for that Log namespace.
#
# If <verboseMode> is not provided, then no action is done on the verbose
# mode of this Log.
#
# RETURNS: The verbose mode for this Log namespace
#

proc Log::verbose {{verboseMode ""}} {

    variable mVerbose		;# verbose mode

    if {$verboseMode != ""} {

	if {($verboseMode == "on") || ($verboseMode == "off")} {
	    set mVerbose $verboseMode
	}
    }

    return $mVerbose
}

# --------------------------- local procedures --------------------------- #

############################################################################
#
# Log::indentStrGet - gets current indentation string
#
# SYNOPSIS
# \cs
#   Log::indentStrGet
# \ce
#
# PARAMETERS: N/A
#
# This procedure returns a string made of <mIndent>*<mIndentStr>, where
# <mIndent> can be set / get using `Log::indent', and represent the current
# indentation depth. <mIndentStr> is the indentation string. It is possible
# to get / set it through a call to `Log::indentStr'.
#
# RETURNS: a string made of <mIndent>*<mIndentStr>
#

proc Log::indentStrGet {} {

    variable mIndent			;# current indent level
    variable mIndentStr			;# current indent str pattern

    set str ""

    for {set ix 0} {$ix < $mIndent} {incr ix} {

	set str [format "%s%s" $str $mIndentStr]
    }

    return $str
}

