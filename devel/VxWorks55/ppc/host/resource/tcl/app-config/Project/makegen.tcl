# makegen.tcl - generate project Makefile
# 
# modification history
# --------------------
# 01a,13nov01,rbl   written - based on prj.tcl from T3
#
# DESCRIPTION
# makegen is a command line utility to generate a project Makefile
# 
# SYNOPSIS 
# \ss
# 1. makegen [-help | ?] 
# 2. makegen
# 3. makegen <prjName>]
# \se
# 
# PARAMETERS
#   
# <prjDir>:      Project directory or name. 
#                <prjDir> can either be a name or a full path to a
#                project directory. 
#                If a name is used, the project is assumed to reside in 
#                a project directory of the same name
#                under $WIND_PROJ_BASE if this environment variable is
#                defined, or otherwise under $WIND_BASE/target/proj. 
#                If <prjDir> is not specified, the project directory
#                is assumed to be the current working directory. 
# -h[elp] \| ?:  Shows this help message. 
# 
# NOROUTINES
#

#
# Get access to the project scripting library
#
source [wtxPath host resource tcl app-config Project]prjLib.tcl

#
# Program namespace 
#
namespace eval makeGen {
}

###############################################################################
#
# makeGen::usagePrint - print out program usage
#
# print out program usage
#
# Note: This must be kept in sync with the comments above 
# (which are used in turn for the
# refgen manual pages). They were generated from the above using an editor macro.
# In MS Visual Studio the macro is:
# \cs
# Sub refgenToPuts()
# 'DESCRIPTION: converts refgen style comments to puts statements
# 	' replace \sh with sub-heading followed by \n
# 	ActiveDocument.Selection.ReplaceText "\\sh\:b*\(.*\)", "\\n\1\\n", dsMatchRegExp
# 	' escape double quote
# 	ActiveDocument.Selection.ReplaceText "\""", "\\\""", dsMatchRegExp
# 	' turn # into puts, doing a -nonewline for a list item marker (e.g. \m [1])
# 	ActiveDocument.Selection.ReplaceText "^#[^\\m]*\\m\:b+\([^\\m]*\)$", "    puts -nonewline ""\1 """, dsMatchRegExp
# 	ActiveDocument.Selection.ReplaceText "^#\(.*\)$", "    puts ""\1""", dsMatchRegExp
# 	' escape []
# 	ActiveDocument.Selection.ReplaceText "\[", "\\\[", dsMatchRegExp
# 	ActiveDocument.Selection.ReplaceText "\]", "\\\]", dsMatchRegExp
# 	' escape {}
# 	ActiveDocument.Selection.ReplaceText "{", "\\{", dsMatchRegExp
# 	ActiveDocument.Selection.ReplaceText "}", "\\}", dsMatchRegExp
# 	' escape $
# 	ActiveDocument.Selection.ReplaceText "\$", "\\\$", dsMatchRegExp
# 	' remove refgen tags
# 	ActiveDocument.Selection.ReplaceText "\\m[le]\:b*", "", dsMatchRegExp
# 	ActiveDocument.Selection.ReplaceText "\\[a-z]s\:b*", "", dsMatchRegExp
# 	ActiveDocument.Selection.ReplaceText "\\[a-z]e\:b*", "", dsMatchRegExp
# 	' remove blank lines
# 	'ActiveDocument.Selection.ReplaceText "^ *puts \"" *\""$", "", dsMatchRegExp
# 	' remove initial space
# 	ActiveDocument.Selection.ReplaceText "\(^ *puts \""\) ", "\1", dsMatchRegExp
# 
# End Sub
# \ce
#
# SYNOPSIS
#   makeGen::usagePrint
#
# PARAMETERS: N / A
#
# RETURNS: N / A
#
# ERRORS: N / A
#

proc makeGen::usagePrint {} {
    puts "DESCRIPTION"
    puts "makegen is a command line utility to generate a project Makefile"
    puts ""
    puts "SYNOPSIS "
    puts ""
    puts "1. makegen \[-help | ?\] "
    puts "2. makegen"
    puts "3. makegen <prjName>\]"
    puts ""
    puts ""
    puts "PARAMETERS"
    puts "  "
    puts "<prjDir>:      Project directory or name. "
    puts "               <prjDir> can either be a name or a full path to a"
    puts "               project directory. "
    puts "               If a name is used, the project is assumed to reside in "
    puts "               a project directory of the same name"
    puts "               under \$WIND_PROJ_BASE if this environment variable is"
    puts "               defined, or otherwise under \$WIND_BASE/target/proj. "
    puts "               If <prjDir> is not specified, the project directory"
    puts "               is assumed to be the current working directory. "
    puts "-h\[elp\] \| ?:  Shows this help message. "
}



#################################################################################
#
# makeGen::run - obtains and process all command line arguments
#
# This processes the command line arguments and either prints help or executes 
# the prjScriptLib command specified, then exits the wtxtcl shell. 
#
# If no command line arguments are 
# specified, we stay in the wtxtcl shell, (i.e. we do not exit).
#  
# Any -prjDir arguments are converted into 
# project handles by opening the corresponding project. After the prjScriptLib
# command is executed, all projects are closed.
#
# SYNOPSIS 
#   makeGen::run   
#
# PARAMETERS: N / A
#
# RETURNS: N / A
#
# ERRORS: not a valid prjScriptLib command. We print an error and exit in this
# case. 
#
proc makeGen::run {} {

    # make sure ::argc and ::argv are in sync
    set ::argc [llength $::argv]
    set prjDir ""
    set help ""

    ##################################################
    #
    # look for -help or ? and consume this option
    #

    set iHelpOption [lsearch -regexp $::argv {^(-h[e]*[l]*[p]*|[?].*)}]
    if {$iHelpOption != -1} {
        usagePrint
        exit
    }

    ##################################################
    #
    # only other option is prjDir
    #

    if {$::argc == 0} {
        set prjDir [pwd]
    } else {
        set prjDir [lindex $::argv 0]
        ::filePathFix prjDir
    }

    ##################################################
    #
    # open project, generate Makefile and exit
    #

    set prjDirExtension [file extension $prjDir]
    if {$prjDirExtension == ".wpj"} {
        set prjFile [filePathToAbsolute $prjDir]
    } else {

        set prjFile [glob -nocomplain [toPrjDirAbsolute $prjDir]/*.wpj]
        if {$prjFile == ""} {
            error "Open project failed: could not find a .wpj file in directory [toPrjDirAbsolute $prjDir]"
        }
    }
    set hProj [prjOpen $prjFile]
    set hiddenNamespace [::prjTypeGet $hProj]_hidden
    ${hiddenNamespace}::makeGen $hProj [file dir \
	    [prjInfoGet $hProj fileName]]/Makefile
    prjClose $hProj
}



#
# Start program
#

makeGen::run

