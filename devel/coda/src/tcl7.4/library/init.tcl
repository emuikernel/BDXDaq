# init.tcl --
#
# Default system startup file for Tcl-based applications.  Defines
# "unknown" procedure and auto-load facilities.
#
# @(#) init.tcl 1.37 95/03/29 10:26:32
#
# Copyright (c) 1991-1993 The Regents of the University of California.
# Copyright (c) 1994 Sun Microsystems, Inc.
#
# See the file "license.terms" for information on usage and redistribution
# of this file, and for a DISCLAIMER OF ALL WARRANTIES.
#
# ========================================================================
# >>>>>>>>>>>>>>>> INCLUDES MODIFICATIONS FOR [incr Tcl] <<<<<<<<<<<<<<<<<
#
#  AUTHOR:  Michael J. McLennan       Phone: (610)712-2842
#           AT&T Bell Laboratories   E-mail: michael.mclennan@att.com
#     RCS:  $Id: init.tcl,v 1.1.1.1 1996/08/21 19:30:31 heyes Exp $
# ========================================================================
#             Copyright (c) 1993-1995  AT&T Bell Laboratories
# ------------------------------------------------------------------------
# See the file "license.terms" for information on usage and redistribution
# of this file, and for a DISCLAIMER OF ALL WARRANTIES.
#

set auto_path [info library]
set errorCode ""
set errorInfo ""

set unknown_handler_order {}

# ------------------------------------------------------------------------
#  USAGE: unknown ?command arg arg...?
#
#  Invoked automatically whenever an unknown command is encountered.
#  Works through a list of "unknown handlers" that have been registered
#  to deal with unknown commands.  Extensions can integrate their own
#  handlers into the "unknown" facility via "unknown_handle".
#
#  If a handler exists that recognizes the command, then it will
#  take care of the command action and return a valid result or a
#  Tcl error.  Otherwise, it should return "-code continue" (=2)
#  and responsibility for the command is passed to the next handler.
# ------------------------------------------------------------------------
proc unknown {args} {
    global unknown_handler_order unknown_handlers errorInfo errorCode

    foreach handler $unknown_handler_order {
        set status [catch {uplevel $unknown_handlers($handler) $args} result]

        if {$status == 1} {
            #
            # Strip the last five lines off the error stack (they're
            # from the "uplevel" command).
            #
            set new [split $errorInfo \n]
            set new [join [lrange $new 0 [expr [llength $new] - 6]] \n]
            return -code $status -errorcode $errorCode \
                -errorinfo $new $result

        } elseif {$status != 4} {
            return -code $status $result
        }
    }

    set name [lindex $args 0]
    return -code error "tcl7.4(init.tcl): invalid command name \"$name\""
}

# ------------------------------------------------------------------------
#  USAGE: unknown_handler name command ?arg arg...?
#
#  Registers another handler to be used in conjunction with the
#  unknown command.  Each time an unknown command is encountered, the
#  list of handlers is consulted to determine how to handle the command.
#  If a handler recognizes the command, then it should load, define,
#  or otherwise handle the command and return.  Otherwise, it should
#  return "-code continue", which tells the "unknown" facility to
#  continue with another handler.
#
#  Handlers registered first are consulted last.  This way, extensions
#  that are loaded later have higher priority for handling special cases.
#
#  Usually "command" is the name of a handling procedure.  But extra
#  arguments can be specified when the handler is registered, and
#  these will be kept as arguments when the handler is invoked.  The
#  actual unknown command and its arguments are appended after these.
# ------------------------------------------------------------------------
proc unknown_handler {name args} {
    global unknown_handler_order unknown_handlers

    set unknown_handlers($name) $args
    set unknown_handler_order [linsert $unknown_handler_order 0 $name]
}

# tcl_unknown:
# Invoked when a Tcl command is invoked that doesn't exist in the
# interpreter:
#
#	1. See if the autoload facility can locate the command in a
#	   Tcl script file.  If so, load it and execute it.
#	2. If the command was invoked interactively at top-level:
#	    (a) see if the command exists as an executable UNIX program.
#		If so, "exec" the command.
#	    (b) see if the command requests csh-like history substitution
#		in one of the common forms !!, !<number>, or ^old^new.  If
#		so, emulate csh's history substitution.
#	    (c) see if the command is a unique abbreviation for another
#		command.  If so, invoke the command.

unknown_handler "tcl" tcl_unknown

proc tcl_unknown args {
    global auto_noexec auto_noload env unknown_pending tcl_interactive
    global errorCode errorInfo

    # Save the values of errorCode and errorInfo variables, since they
    # may get modified if caught errors occur below.  The variables will
    # be restored just before re-executing the missing command.

    set savedErrorCode $errorCode
    set savedErrorInfo $errorInfo
    set name [lindex $args 0]
    if ![info exists auto_noload] {
	#
	# Make sure we're not trying to load the same proc twice.
	#
	if [info exists unknown_pending($name)] {
	    unset unknown_pending($name)
	    if {[array size unknown_pending] == 0} {
		unset unknown_pending
	    }
	    return -code error "self-referential recursion in \"unknown\" for command \"$name\"";
	}
	set unknown_pending($name) pending;
	set ret [catch {auto_load $name} msg]
	unset unknown_pending($name);
	if {$ret != 0} {
	    return -code $ret "error while autoloading \"$name\": $msg"
	}
	if ![array size unknown_pending] {
	    unset unknown_pending
	}
	if $msg {
	    set errorCode $savedErrorCode
	    set errorInfo $savedErrorInfo
	    set code [catch {uplevel $args} msg]
	    if {$code ==  1} {
		#
		# Strip the last five lines off the error stack (they're
		# from the "uplevel" command).
		#

		set new [split $errorInfo \n]
		set new [join [lrange $new 0 [expr [llength $new] - 6]] \n]
		return -code error -errorcode $errorCode \
			-errorinfo $new $msg
	    } else {
		return -code $code $msg
	    }
	}
    }
    if {([info level] == 1) && ([info script] == "") \
	    && [info exists tcl_interactive] && $tcl_interactive} {
	if ![info exists auto_noexec] {
	    if [auto_execok $name] {
		set errorCode $savedErrorCode
		set errorInfo $savedErrorInfo
		return [uplevel exec >&@stdout <@stdin $args]
	    }
	}
	set errorCode $savedErrorCode
	set errorInfo $savedErrorInfo
	if {$name == "!!"} {
	    return [uplevel {history redo}]
	}
	if [regexp {^!(.+)$} $name dummy event] {
	    return [uplevel [list history redo $event]]
	}
	if [regexp {^\^([^^]*)\^([^^]*)\^?$} $name dummy old new] {
	    return [uplevel [list history substitute $old $new]]
	}
	set cmds [info commands $name*]
	if {[llength $cmds] == 1} {
	    return [uplevel [lreplace $args 0 0 $cmds]]
	}
	if {[llength $cmds] != 0} {
	    if {$name == ""} {
		return -code error "empty command name \"\""
	    } else {
		return -code error \
			"ambiguous command name \"$name\": [lsort $cmds]"
	    }
	}
    }
    return -code continue
}

# auto_load:
# Checks a collection of library directories to see if a procedure
# is defined in one of them.  If so, it sources the appropriate
# library file to create the procedure.  Returns 1 if it successfully
# loaded the procedure, 0 otherwise.

proc auto_load cmd {
    global auto_index auto_oldpath auto_path env errorInfo errorCode

    if {[info exists auto_index($cmd)]} {
        set loadcmd $auto_index($cmd)
    } else {
        foreach nsref [uplevel import all] {
            set ns [lindex $nsref 0]
			if {$ns == "::"} {
            	set fullname "::$cmd"
			} else {
            	set fullname "$ns::$cmd"
			}
            if {[info exists auto_index($fullname)]} {
                set loadcmd $auto_index($fullname)
                break
            }
        }
    }
    if {[info exists loadcmd]} {
	uplevel #0 $loadcmd
	return [expr {[info which $cmd] != ""}]
    }

    if [catch {set path $auto_path}] {
	if [catch {set path $env(TCLLIBPATH)}] {
	    if [catch {set path [info library]}] {
		return 0
	    }
	}
    }
    if [info exists auto_oldpath] {
	if {$auto_oldpath == $path} {
	    return 0
	}
    }
    set auto_oldpath $path
    catch {unset auto_index}
    for {set i [expr [llength $path] - 1]} {$i >= 0} {incr i -1} {
	set dir [lindex $path $i]
	set f ""
	if [catch {set f [open $dir/tclIndex]}] {
	    continue
	}
	set error [catch {
	    set id [gets $f]
	    if {$id == "# Tcl autoload index file, version 2.0" ||
		$id == "# Tcl autoload index file, version 2.0 for \[incr Tcl\]"} {
		eval [read $f]
	    } elseif {$id == "# Tcl autoload index file: each line identifies a Tcl"} {
		while {[gets $f line] >= 0} {
		    if {([string index $line 0] == "#")
			    || ([llength $line] != 2)} {
			continue
		    }
		    set name [lindex $line 0]
		    set auto_index($name) "source $dir/[lindex $line 1]"
		}
	    } else {
		error "$dir/tclIndex isn't a proper Tcl index file"
	    }
	} msg]
	if {$f != ""} {
	    close $f
	}
	if $error {
	    error $msg $errorInfo $errorCode
	}
    }

    if {[info exists auto_index($cmd)]} {
        set loadcmd $auto_index($cmd)
    } else {
        foreach nsref [uplevel import all] {
            set ns [lindex $nsref 0]
			if {$ns == "::"} {
            	set fullname "::$cmd"
			} else {
            	set fullname "$ns::$cmd"
			}
            if {[info exists auto_index($fullname)]} {
                set loadcmd $auto_index($fullname)
                break
            }
        }
    }
    if {[info exists loadcmd]} {
	uplevel #0 $loadcmd
	return [expr {[info which $cmd] != ""}]
    }
    return 0
}

# auto_execok:
# Returns 1 if there's an executable in the current path for the
# given name, 0 otherwise.  Builds an associative array auto_execs
# that caches information about previous checks, for speed.

proc auto_execok name {
    global auto_execs env

    if [info exists auto_execs($name)] {
	return $auto_execs($name)
    }
    set auto_execs($name) 0
    if {[string first / $name] >= 0} {
	if {[file executable $name] && ![file isdirectory $name]} {
	    set auto_execs($name) 1
	}
	return $auto_execs($name)
    }
    foreach dir [split $env(PATH) :] {
	if {$dir == ""} {
	    set dir .
	}
	if {[file executable $dir/$name] && ![file isdirectory $dir/$name]} {
	    set auto_execs($name) 1
	    return 1
	}
    }
    return 0
}

# auto_reset:
# Destroy all cached information for auto-loading and auto-execution,
# so that the information gets recomputed the next time it's needed.
# Also delete any procedures that are listed in the auto-load index
# except those related to auto-loading.

proc auto_reset {} {
    global auto_execs auto_index auto_oldpath
    foreach p [info procs] {
	if {[info exists auto_index($p)] && ($p != "unknown")
		&& ![string match auto_* $p]} {
	    rename $p {}
	}
    }
    catch {unset auto_execs}
    catch {unset auto_index}
    catch {unset auto_oldpath}
}

# auto_mkindex:
# Regenerate a tclIndex file from Tcl source files.  Takes as argument
# the name of the directory in which the tclIndex file is to be placed,
# floowed by any number of glob patterns to use in that directory to
# locate all of the relevant files.

proc auto_mkindex {dir args} {
    global errorCode errorInfo
    set oldDir [pwd]
    cd $dir
    set dir [pwd]

    global ::tcl::mkindex-parser::index
    set index ""

    append index "# Tcl autoload index file, version 2.0 for \[incr Tcl\]\n"
    append index "# This file is generated by the \"auto_mkindex\" command\n"
    append index "# and sourced to set up indexing information for one or\n"
    append index "# more commands.  Typically each line is a command that\n"
    append index "# sets an element in the auto_index array, where the\n"
    append index "# element name is the name of a command and the value is\n"
    append index "# a script that loads the command.\n\n"

    foreach file [eval glob $args] {
        if {[catch {tcl::mkindex-parser::mkindex $file} msg] != 0} {
            set code $errorCode
            set info $errorInfo
            cd $oldDir
            error $msg $info $code
        }
    }

    set fid [open tclIndex w]
    puts $fid $index nonewline
    close $fid
    cd $oldDir
}

#
# Create a namespace that can be used to parse Tcl source files to
# generate a tclIndex file for autoloading.  This namespace contains
# commands for things that need index entries.  Each time a command
# is executed, it writes an entry out to the index file.
#
# An isolated namespace is set up to filter incoming commands.
# Those that aren't registered in the "mkindex-parser" namespace
# are simply ignored.
#
namespace ::tcl::mkindex-parser {
    ::public variable index ""          ;# maintains index as it is built
    ::protected variable scriptFile ""  ;# name of file being processed
    ::protected variable scopeStack ""  ;# stack of namespace scopes

    # --------------------------------------------------------------------
    # USAGE:  mkindex <fileName>
    #
    # Scans Tcl code from the specified <fileName> and adds entries
    # into the global "index" variable for elements that are recognized
    # by the autoloader.
    # --------------------------------------------------------------------
    ::public ::proc mkindex {file} {
        global scriptFile
        set scriptFile $file

        set fid [open $file]
        set contents [read $fid]
        close $fid

        #
        # There is one problem with sourcing files into the isolated
        # namespace:  references like "$x" will fail since code is not
        # really being executed and variables do not really exist.
        # Be careful to escape all naked "$" before evaluating.
        #
        regsub -all {([^\$])\$([^\$])} $contents {\1\\$\2} contents
        ::namespace ::tcl::mkindex-parser::isolated $contents
    }

    #
    # Set up an isolated namespace to filter incoming commands.
    # Put the "enforce_cmd" proc in the parent namespace, so commands
    # within it work with some sanity.
    #
    ::proc enforce_cmd {name} {
        global commands
        if {[info exists commands($name)]} {
            return $commands($name)
        }
        return "mkindex_ignore"
    }
    ::namespace isolated -local -enforced yes

    #
    # Set up a command registry.  All commands that require an
    # tclIndex entry, and all possible namespace paths for these
    # commands, should be registered in this array.
    #
    # The "source" command should work as usual, so that files
    # can be read into the namespace.
    #
    protected variable commands

    set commands(source) ::source
    set commands(::source) ::source

    #
    # Most commands get mapped to "mkindex_ignore" and are ignored.
    #
    ::proc mkindex_ignore {args} {}

    #
    # HANDLE:  namespace name ... {commands...}
    #
    ::proc namespace {name args} {
        global scopeStack
        set scopeStack [linsert $scopeStack 0 $name]

        set cmds [lindex $args end]
        ::namespace isolated $cmds

        set scopeStack [lrange $scopeStack 1 end]
    }
    set commands(namespace) namespace
    set commands(::namespace) namespace

    #
    # USAGE:  mkindex_path <name>
    #
    # Returns the complete namespace path for the specified <name>.
    # If <name> starts with "::", then it is returned directly.
    # Otherwise, elements from the current namespace stack are added
    # until the complete path starts with "::", or until all elements
    # have been added.
    #
    ::protected ::proc mkindex_path {name} {
        global scopeStack
        if {[string match ::* $name]} {
            return $name
        }
        foreach ns $scopeStack {
            set name "$ns::$name"
            if {[string match ::* $name]} {
                return $name
            }
        }
        return "::$name"
    }

    #
    # HANDLE:  ensemble name body
    #
    ::proc ensemble {name body} {
        global index scriptFile
        set name [mkindex_path $name]
        append index "set [list auto_index($name)]"
        append index " \"source \$dir/$scriptFile\"\n"
    }
    set commands(ensemble) ensemble
    set commands(::ensemble) ensemble

    #
    # HANDLE:  public ...
    #          protected ...
    #          private ...
    #
    ::proc plevel {args} {
        if {[llength $args] == 1} {
            ::namespace isolated [lindex $args 0]
        } else {
            ::namespace isolated $args
        }
    }
    set commands(public) plevel
    set commands(::public) plevel
    set commands(protected) plevel
    set commands(::protected) plevel
    set commands(private) plevel
    set commands(::private) plevel

    #
    # HANDLE:  proc name arglist body
    #
    ::proc proc {name arglist body} {
        global index scriptFile
        set name [mkindex_path $name]
        append index "set [list auto_index($name)]"
        append index " \"source \$dir/$scriptFile\"\n"
    }
    set commands(proc) proc
    set commands(::proc) proc
}
