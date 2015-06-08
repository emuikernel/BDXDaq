# Utils.tcl - handy, global utilities used by the config tool
#
# modification history
# --------------------
# 02f,18jun02,rbl  adding printStack function
# 02e,28may02,rbl  randomize file name in tempFileGet to support parallel
#                  builds
# 02d,22may02,cjs  Move filenameQuoteIfNecessary() to Utils.tcl
# 02c,22may02,cjs  Use Wind::tmpDirGet() in tempFileGet()
# 02b,15may02,cjs  Fix spr 73941: problems running in restricted mode on
#                  Windows XP by making tempFileGet() smarter
# 02a,14may02,cjs  Fix spr 75899: allow user to type in a read-only dependency
#                  path; defer terrible directory search dialog
# 01z,02apr02,rbl  add more characters to the list of unacceptable file name
#                  characters. Also add new textControlTextSetKeepingSelection
#                  function to set text in an edit control while retaining
#                  the cursor position or selection
# 01y,11dec01,rbl  fix buildRuleSplit to cope with macros in target or
#                  dependency names
# 01x,13nov01,rbl  merge project file/dir manipulation routines from T3
# 01w,24oct01,cjs  Add extra param, support for files, to
#                  removeDuplicatesFromList; merge T3's filePathEatDots()
# 01v,23oct01,cjs  Adding tempFileGet(), a routine to get a reusable temporary
#                  file
# 01u,17sep01,rbl  merge from T3: (1) add outOfDate functionality, needed for
#                  auto-generation of tcInfo_<tool_family> files
#                  (2) add removeTrailingSlash function, needed by prjRelPath
#                  (3) fix regsubPath so that it does case insensitive
#                  match on Windows, and copes with back slashes in
#                  match pattern (4) add removeDuplicatesFromList function
# 01t,19mar99,rbl  adding a barePathSubstitute function that does a "shallow" 
#                  substitution on paths, and regsubPath which does a case insensitive 
#                  regsub for windows and a case sensitive regsub for UNIX.
# 01s,11mar99,cjs  Add support for build rules with *either* blank rules or
#                  dependencies in buildRuleSplit/Join()
# 01r,05mar99,ren  changing unix GetWtxLoadOptions to use gui
# 01q,04mar99,ren  Changed behavior of GetWtxLoadOptions on windows
# 01p,03mar99,ren  Adding rename dialog.
# 01o,02mar99,ren  Adding file tails proc
# 01n,01mar99,ren  Exposing download options to GUI
# 01m,24feb99,ren    Added GetWtxObjModuleLoadOptions
# 01l,18feb99,ren    Added EscapeRegexpCharacters
# 01k,15jan99,cjs    Add routine for validating files (no bad chars)
# 01j,06nov98,cjs	 Protect filePathFix, filePathUnFix from bad argument 
# 01i,29oct98,cjs	 Add an optional sequence of prjRelPath/prjFullPath calls
#                    to filePathFix() 
# 01h,22oct98,cjs	 added filePathUnFix(); removed DOSify(); moved mTimeGet()
#                    here from file.psheet.tcl
# 01g,15sep98,cjs	 added preferredPageSort() to sort tab sheets against
#                    a desired order
# 01g,02aug98,cjs	 added buildRuleSplit/Join() 
# 01f,14aug98,cjs	 added pathTruncate() 
# 01e,23jun98,cjs	 changed  nameExtract, uniqueNameCreate to work with
#                    more args
# 01d,01may98,cjs	 change filePathFix() to unix-ify files; add DOSify() 
# 01c,16apr89,cjs	 remove wtxPath() implementation (wtxtcl.dll
#				     is now loaded).
# 01b,30mar98,cjs    documentation update.
# 01a,27mar98,cjs    written.
#
# DESCRIPTION
# This module contains handy utilites for parsing lists,
# identifying flags in arg lists, implementing inheritance
# between namespaces, and so forth.
# It also implements the namespace Object, which is the root
# of all the config tool friendly objects.
#
#############################################################

# Needed to use utils in the Wind package, like tmpDirGet()
package require Wind


# Check to see if file name contains invalid chars
proc ::badFileCharsGet {} {
	return "\[|*<>;\${}\() \r\n\t]|\\\[|\\\]" 
}

#########################################################################
#
# ::regsubPath - does a case insensitive regsub for windows and a case sensitive regsub for UNIX.
#
# For windows, we use -nocase AND lower the case of the match expression.
# We also replace all /'s in the match expression with [\\/] 
# (match forward or backslash)
#
# SYNOPSIS
#   ::regsubPath   args
#
# PARAMETERS
#   args: same arguments as for the tcl regsub function.
#
# RETURNS: output of regsub function.
#
# ERRORS: N / A
#
proc ::regsubPath { args } {
	# first, lower the case of the match expression
	# so find the match expression, skipping past options
	set i 0
	set isArgOption 1
	while { $i < [llength $args] && $isArgOption} {
		set strArg [lindex $args $i]
		if { $strArg == "--" } {
			incr i
			set isArgOption 0
		} elseif { [string match "-*" $strArg] } {
			incr i
		} else {
			set isArgOption 0
		}
	}
	# now lower the case of the match expression and match
        # backslashes as well as forward slashes

	global tcl_platform
	if { $tcl_platform(platform) == "windows" } {
                set loweredMatchPattern [string tolower [lindex $args $i]]
                regsub -all {/} $loweredMatchPattern {[\\\/]} loweredMatchPattern
		set args [lreplace $args $i $i $loweredMatchPattern]
		return [uplevel regsub -nocase $args]
	} else {
		return [uplevel regsub $args]
	}
}

#########################################################################
#
# ::printStack - prints the current call stack
#
# Borrowed from Ousterhout "Tcl and the Tk Toolkit", pg 131
#
# PARAMETERS: N / A
#
# RETURNS: N / A
#
# ERRORS: N / A
#
proc ::printStack {} {
    set level [info level]
    for {set i [expr $level - 1]} {$i >= 1} {incr i -1} {
        puts "Level $i: [info level $i]"
    }
}

############################################################################
#
# barePathSubstitute does a "shallow" substitution of ${strNewPath} for 
# ${strOldPath} in ${strOldString}. 
#
# Only paths that go no deeper than ${strOldPath} (i.e. they end in 
# whitespace, a new line or a continuation character) are substituted.
# Trailing forward slashes are taken into account.
#
# ARGUMENTS:
#
# strOldPath: the old path, e.g. c:/temp
# strNewPath: the new path, e.g. c:/newtemp
# strOldString: a string containing one or more directories, with forward
#            slash separators, and possibly <backslash> <newline> between
#            them: e.g. 
#                       c:/temp/fred \
#                       c:/temp/\
#                       e:/temp/bill
#
#            strOldString also contains the output, e.g. 
#                       c:/temp/fred \
#                       c:/newtemp/\
#                       e:/temp/bill
#
#            notice that c:/temp\ was substituted but c:/temp/fred \ was not. 
#            
# RETURNS: none
#
proc ::barePathSubstitute {strOldString strOldPath strNewPath} {
	upvar $strOldString string

	set strOldPath [EscapeRegexpCharacters $strOldPath]

	# space termination
	::regsubPath -all -- "${strOldPath} " $string "${strNewPath} " string
	::regsubPath -all -- "${strOldPath}\/ " $string "${strNewPath}/ " string

	# tab termination
	::regsubPath -all -- "${strOldPath}\t" $string "${strNewPath}\t" string
	::regsubPath -all -- "${strOldPath}\/\t" $string "${strNewPath}/\t" string

	# end of string termination
	::regsubPath -all -- "${strOldPath}$" $string "${strNewPath}" string
	::regsubPath -all -- "${strOldPath}\/$" $string "${strNewPath}/" string

	# continuation character (backslash) continuation
	::regsubPath -all -- "${strOldPath}\\\\" $string "${strNewPath}\\" string
	::regsubPath -all -- "${strOldPath}\/\\\\" $string "${strNewPath}/\\" string

}

proc ::isFileNameOk {file} {
	return [expr ![regexp [::badFileCharsGet] $file tmp]]
}

# Substitue the system path delimiter for "/" in the path 
proc ::filePathFix {pathName {hProj ""}} {
	upvar $pathName path
	if {$path != ""} {
		regsub -all {\\} $path {/} path
		if {$hProj != ""} {
			set path [::prjRelPath $hProj $path]
			set path [::prjFullPath $hProj $path]
		}
	}
}

proc ::filePathUnFix {pathName} {
	upvar $pathName path
	if {$path != ""} {
		if {[string match [wtxHostType] "x86-win32"]} {
			set sysSlash "\\"
			regsub -all "/" $path $sysSlash path
		}
	}
}

# Truncate the beginning of a file path and replace missing
# portion w/ ".../" so that it fits in 'len' chars
proc ::pathTruncate {pathName len} {
	set bTruncated 0
	set retval $pathName
	while {[string length $retval] > [expr $len - 4]} {
		set split [file split $retval]
		set split [lrange $split 1 end]
		set retval [eval file join $split]
		set bTruncated 1
	}

	if {$bTruncated} {
		set retval [format ".../%s" $retval]
	}
	return $retval
}

proc mTimeGet {fileName} {
	if {[file exists $fileName]} {
		set retval [clock format [file mtime $fileName] \
			-format "Last modified: %I:%M:%S %p %m/%d/%y"]
	} else {
		set retval "(file not found)"
	}
}

proc ::car {lst} {
	if {[llength $lst] > 0} {
		return [lindex $lst 0]
	} else {
		return ""
	}
}

proc ::cdr {lst} {
	if {[llength $lst] > 0} {
		return [lrange $lst 1 end]
	} else {
		return ""
	}
}

proc ::flagFind {argz flag} {
	set idx [lsearch $argz $flag]
	if {$idx == -1} {
		return 0
	} else {
		return 1
	}
}

proc ::flagValueGet {argz flag} {
	set retval ""
	set idx [lsearch $argz $flag]
	if {$idx != -1} {
		if {[llength $argz] > $idx} {
			set retval [lindex $argz [expr $idx + 1]]
		}
	}
	return $retval
}

proc ::ifaceValidate {namespace procList} {
	set status 1
	foreach proc $procList { 
		set cmd "info proc $proc"
		if {[namespace eval $namespace $cmd] == ""} {
			set status 0
			break
		}
	}
	return $status
}

proc ::inherit {from} {
	if {[catch {namespace parent $from}]} {
		error "attempt to inherit from non-existent namespace '$from'"
	} else {
		set to [uplevel {namespace current}]
    	catch {namespace eval $to [set [format "%s::public" $from]]}
    	catch {namespace eval $to [set [format "%s::protected" $from]]}
	}
}

proc ::preferredPageSort {preferredList realList} {
    set panes [lsort -increasing $realList]
    foreach preferred $preferredList {
        set idx [lsearch $panes $preferred]
        if {$idx != -1} {
            lappend orderedPanes $preferred
            set panes [lreplace $panes $idx $idx]
        }
    }
    if {[info exists orderedPanes]} {
        set orderedPanes [concat $orderedPanes $panes]
        set panes $orderedPanes
    }
    return $panes
}


##########################################################################
#
# buildRuleSplit - given a build rule:
#	{rule : deps} {rule1} {rule2} ...
# return {rule deps {rule1 rule2 ...}}
#
proc buildRuleSplit {ruleCommands} {
	set retval ""
	if {$ruleCommands != ""} {
		set targetLine [lindex $ruleCommands 0]
		set idx [lsearch $targetLine \:]
		if {$idx != -1} {
			set targetName [join [lrange $targetLine 0 [expr $idx - 1]]]
			set dependencies [join [lrange $targetLine [expr $idx + 1] end]]
			regsub "^ *" $dependencies "" dependencies

			set ruleCommands [lrange $ruleCommands 1 end]
			set data ""
			foreach line $ruleCommands {
				if {$data != ""} {
					set data [format "%s\r\n%s" $data $line]
				} else {
					set data $line
				}
			}
			set retval [list $targetName $dependencies $data]
		}
	}
	return $retval
}

##########################################################################
#
# buildRuleJoin - join a list consisting of {ruleName deps rule1 rule2 ...}
# 	into a rule of the form {rule : deps} {rule1} {rule2} ...
#
proc buildRuleJoin {ruleName deps commands} {
	set ruleToAdd ""
	if {$ruleName != ""} {
		regsub -all "\r\n" $commands "\n" commands
		foreach line [split $commands \n] {
   			lappend ruleToAdd $line
		}

		# Get rule name
		scan $ruleName "%s" ruleName
		set dependencies [join $deps]
		set targetLine "$ruleName : $dependencies"
		set ruleToAdd [linsert $ruleToAdd 0 $targetLine]
	}
	return $ruleToAdd
}

namespace eval ::Utils {
	variable _uniqueIndex 0
}

#############################################################
#
# The ::Object namespace implements the following routines
# for creating a low overhead "object" whose instance data is
# defined by a list, and whose class methods and class data
# are defined by a namespace.
#
# ::Object::objectCreate <namespace> <objectDataList>
# ::Object::objEval <object> <proc> [args]
#
# All objects (and therefore, the underlying namespace) are
# expected to implement the following functions:
# 	objName {objData}
#	objType {objData} 
#
# Other methods implemented by the namespace can expect that
# the first argument is always the instance data, and any
# additional arguments are specific to the method.
#
# The following are all equally valid signatures for methods:
#	proc objType {data} {...}
#	proc test1Arg {data arg1} {...}
#	proc testnArgs {args} {...}
#
#############################################################
namespace eval ::Object {

	#
	# evaluate <object> <proc> [args]
	#
	proc evaluate {handle method {argz ""}} {
		set namespace [lindex $handle 0]
		set objData [lindex $handle 1]

		if {$argz != ""} {
			return [eval ${namespace}::$method [list $objData] $argz]
		} else {
			return [eval ${namespace}::$method [list $objData]]
		}
	}

	proc create {namespace objData} {
		namespace eval $namespace {
			if {([info procs Name] == "") || \
				([info procs Type] == "")} {
				set error "Error: namespace '[namespace current]' must"
				set error "$error support Name() and Type()"
				error $error
			}
		}
		return [list $namespace $objData]
	}

	proc nameExtract {id} {
		if {[regexp {^(.+)@@(.+)@@(.+)$} $id dummy prefix prefix2 suffix]} {
			return $suffix
		} elseif {[regexp {^(.+)@@(.+)$} $id dummy prefix suffix]} {
			return $suffix
		} else {
			error "nameExtract '$id' failed"
		}
	}

	proc uniqueNameCreate {args} {
		if {[llength $args] == 3} {
			set prefix [lindex $args 0]
			set prefix2 [lindex $args 1]
			set suffix [lindex $args 2]
			return [format "%s@@%s@@%s" $prefix $prefix2 $suffix]
		} elseif {[llength $args] == 2} {
			set prefix [lindex $args 0]
			set suffix [lindex $args 1]
			return [format "%s@@%s" $prefix $suffix]
		} else {
			error "uniqueNameCreate: bad args '$args'"
		}
	}

	# Object creation test program; invoke as ::Object::test
	proc test {} {
		namespace eval ::Object::test {
			proc objName {data} { return [lindex $data 0] }
			proc objType {data} { return [lindex $data 1] }
			proc test1Arg {data arg1} {
				puts "arg1 is $arg1"
			}
			proc testnArg {args} {
				puts "data is [lindex $args 0]; args is [lrange $args 1 end]"
			}
		}
		set myObj [::Object::objectCreate ::Object::test \
			{INCLUDE_WDB componentObject {foo bar} grub slickdit}]
		puts [::Object::objEval $myObj objName]
		puts [::Object::objEval $myObj objType]
		puts [::Object::objEval $myObj test1Arg arg1]
		puts [::Object::objEval $myObj testnArg]
		puts [::Object::objEval $myObj testnArg arg1]
		puts [::Object::objEval $myObj testnArg arg1 arg2]
	}
}

namespace eval ::Null {
	proc Type {args} {
		return nullObject
	}

	proc Name {args} {
		return None
	}

	proc DisplayName {args} {
		return None
	}
}

proc NativeFileNames { listFileNames } {
	mapcar [list file nativename] $listFileNames
}

# the old lisp method, apply the proc to every element in the list
# return a list of the results
proc mapcar { proc list } {
	set listResult [list ]
	foreach element $list {
		lappend listResult [eval $proc [list $element]]
	}
	return $listResult
}

proc FileTails { listFileNames } {
	mapcar [list file tail] $listFileNames
}

#########################################################################
#
# ::maxModTime - maximum modification time 
# 
# Finds the maximum modification time of a set of files, using the 
# TCL function file mtime
#
# SYNOPSIS 
#   ::maxModTime  files 
#
# PARAMETERS 
#   files:  list of files. Wildcards can be used, e.g. "hello.c *.cpp"
#
# RETURNS: the maximum modification time of files, using the 
# TCL function file mtime
#
# ERRORS: files not found
#
proc ::maxModTime {files} {
    set files [eval glob $files]
    set nFiles [llength $files]
    set modTimes [lsort -decreasing -integer [mapcar [list file mtime] $files]]
    return [lindex $modTimes 0]
}

#########################################################################
#
# ::minModTime - minimum modification time 
# 
# Finds the minimum modification time of a set of files, using the 
# TCL function file mtime
#
# SYNOPSIS 
#   ::minModTime  files 
#
# PARAMETERS 
#   files:  list of files. Wildcards can be used, e.g. "hello.c *.cpp"
#
# RETURNS: the minimum modification time of files, using the 
# TCL function file mtime
#
# ERRORS: files not found
#
proc ::minModTime {files} {
    set files [eval glob $files]
    set nFiles [llength $files]
    set modTimes [lsort -increasing -integer [mapcar [list file mtime] $files]]
    return [lindex $modTimes 0]
}


#########################################################################
#
# ::outOfDate - tests if target files are out of date with respect to dependency files
#
# SYNOPSIS 
#   ::outOfDate  targets dependencies 
#
# PARAMETERS 
#   targets:    set of target files. Wildcards can be used, 
#               e.g. "hello.o *.out"   
#   dependencies:   set of dependency files. Wildcards can be used, 
#               e.g. "hello.c *.cpp"   
#
# RETURNS: 1 if any target files are out of date with respect to 
# any dependency files, or if any of the target files do not exist;
# 0 otherwise. 
#
# ERRORS: N / A
#
proc ::outOfDate {targets dependencies} {
    set targets [eval glob -nocomplain $targets]
    set targetsAllExist 1

    if {[llength $targets] == 0} {
        set targetsAllExist 0
    }

    foreach target $targets {
        if {![file exists $target]} {
            set targetsAllExist 0
        }
    }

    if {$targetsAllExist == 0} {
        return 1
    }

    if {[maxModTime $dependencies] > [minModTime $targets]} {
        return 1
    }

    return 0

}

#########################################################################
#
# removeTrailingSlash - remove a trailing slash from a directory
#
# PARAMETERS 
#   dirName: name of the variable holding the directory
#
# RETURNS: N / A
#
# ERRORS: N / A
#
proc removeTrailingSlash {dirName} {
    upvar $dirName dir
    set lastCharIndex [expr [string length $dir] - 1]
    set lastChar [string index $dir $lastCharIndex]
    if {$lastChar == "/" || $lastChar == "\\"} {
        set dir [string range $dir 0 [expr $lastCharIndex - 1]]
    }
}

#########################################################################
#
# removeDuplicatesFromList - remove duplicate items from a list
#
# PARAMETERS 
#   nameOfList:   name of list to remove item from
#   bElementsAreFiles: 1, if elements should be treated as file paths
#
# RETURNS: 1 if items were removed, 0 if no duplicates found
#
# ERRORS: N / A
proc removeDuplicatesFromList {nameOfList {bElementsAreFiles 0}} {
    upvar $nameOfList pathList

    # Make Tcl hash list to avoid duplicates
    # Along the way,
    # a) flatten all relative paths
    # b) make sure hashing is case insensitive for Windows
    foreach path $pathList {
        if {$bElementsAreFiles == 1} {
            set path [filePathEatDots $path]
        }

        if {$bElementsAreFiles == 1} {
            if {[wtxHostType] == "x86-win32"} {
                set paths([string tolower $path]) $path
            } else {
                set paths($path) $path
            }
        } else {
            set paths($path) $path
        }
    }

    if {$bElementsAreFiles == 1} {
        # If Windows, recover originals from hashed list
        if {[wtxHostType] == "x86-win32"} {
            foreach name [array names paths] {
                lappend returnList $paths($name)
            }
            set pathList $returnList
        } else {
            set pathList [array names paths]
        }
    } else {
        set pathList [array names paths]
    }
}

#########################################################################
#
# ::filePathToAbsolute - convert a relative path to an absolute path
#
# Converts a relative path to an absolute path, 
# prepending the current working directory and unix-ifying the path if necessary.
# Any ../../'s are eaten up, the path is unixified (all slashes converted
# to forward slashes), and variables (e.g. $(WIND_BASE)) are substituted for
# using prjFullPath.
#
# SYNOPSIS
#   filePathToAbsolute  file
#
# PARAMETERS
#   file:   relative file path.
#   currentDirectory:  (optional) directory which is prepended to any relative
#                      path. If not specified, [pwd] is used.
# RETURNS: N / A
#
# ERRORS: N / A
#
proc ::filePathToAbsolute {file {currentDirectory ""}} {
    # get pwd if necessary
    if {$currentDirectory == ""} {
        set currentDirectory [pwd]
    } else {
        set currentDirectory [toPrjDirAbsolute $currentDirectory]
    }

    # trim whitespace from path
    set file [string trim $file]

    # unix-ify path
    ::filePathFix file

    # do substitutions, e.g. $(WIND_BASE), $(PRJ_DIR)...
    set file [prjFullPathRelativeTo "" $file $currentDirectory]

    # prepend pwd to a relative file path name (any path not beginning with "/" or "<drive letter>:"
    if {![regexp {^/} $file] && ![regexp {^[a-zA-Z]:/} $file]} {
	set file $currentDirectory/$file
    }

    # tdw: not sure why we did this. We need to substitute $(WIND_BASE)
    # before prepending the working directory.
    # do substitutions, e.g. $(WIND_BASE), $(PRJ_DIR)...
    #set file [prjFullPathRelativeTo "" $file $currentDirectory]

    return [filePathEatDots $file]
}

#########################################################################
#
# filePathEatDots - removes ..'s and .'s from file path
#
# SYNOPSIS 
#   filePathEatDots  file 
#
# PARAMETERS 
#   file: file path, e.g. ./joe/myDir/../fred
#
# RETURNS: path with dots eaten, e.g. joe/fred
#
# ERRORS: N / A
#
proc filePathEatDots {file} {
    # split path into elements (stuff between slashes) and eat up ..'s and .'s
    set pathElements [split $file /]
    set i 1
    while {$i < [llength $pathElements]} {
        # if we find a "..", eat it and the element to the left of it
        if {[lindex $pathElements $i] == ".."} {
            set pathElements [lreplace $pathElements [expr $i - 1] $i]
            incr i -1
        # if we find a ".", eat it
        } elseif {[lindex $pathElements $i] == "."} {
            set pathElements [lreplace $pathElements $i $i]
        } else {
            incr i
        }
    }
    return [join $pathElements /]
}

###############################################################################
#
# ::toPrjDirAbsolute - converts a project name, project file name or project directory to a project directory
#
# Takes as input:
# \ml
# \m -
# a project name
# \m -
# a project file name
# \m -
# a relative or absolute path to a directory
# \me
# and outputs an absolute path to a directory. If a name is specified, the
# directory is under [prjDirDefault].
#
# SYNOPSIS
#   toPrjDirAbsolute prjDir
#
# PARAMETERS
#   prjDir: a project name, project file name or project directory
#
# RETURNS: absolute path to the directory corresponding to prjDir
#
# ERRORS: N / A
#

proc ::toPrjDirAbsolute {prjDir} {

    # Don't do this to workspace files, which can live in arbitrary
    # directories
    if {[string match [file extension $prjDir] ".wsp"]} {
	return $prjDir
    }

    # substitute [pwd] for "."
    if {$prjDir == "."} {
	return [pwd]
    }

    # if we have a name without a path, prepend [prjDirDefault] to the name
    if {[file tail $prjDir] == $prjDir} {

        # strip off any extension
        #set prjDir [file rootname $prjDir]
        return [prjDirDefault]/$prjDir
    }

    # if we have a file with .wpj extension, return the directory
    if {[string match [file extension $prjDir] ".wpj"]} {
	set prjDir [file rootname $prjDir]
        set prjDir [file dirname $prjDir]
        return [filePathToAbsolute $prjDir]
    }

    # default: convert relative paths to absolute paths
    return [filePathToAbsolute $prjDir]
}

#########################################################################
#
# EscapeRegexpCharaters - make regular expression chars safe from immediate
# evalation
# args: 
#   str: any string
#  returns:
#   takes the input string and escapes any characters with
#   special meaning in regular expressions
proc EscapeRegexpCharacters { str } {
	regsub -all {[\?|^$.*+()]|\[|\]} \
		$str {\\\0} str
	set str
}

#########################################################################
#
# tempFileGet - Get a name for a temp file
# 
# To get the filename we use the TCL rand() function, which returns 
# a random number from a seed based on the internal
# clock of the machine at the time rand() is first called.
# The seed can be reset by calling srand(). 
# 
# For the directory we use [Wind::tmpDirGet]. 
# We check that the file does not currently exist. 
#
# The client is responsible for creating the file and cleaning it up
# when done with it. 
#
# RETURNS: name of a temp file
# e.g. on Windows this might be:
#
# C:/Documents and Settings/kiwi/Local Settings/Application Data/Wind River 
# Systems/Tornado/2.2/435269992536Tclprocess_output.tmp
#
# NB: this filename may (on Windows probably will) 
# contain spaces.  If you intend to use this with
# an {eval exec}, be sure to wrap the result in
# filenameQuoteIfNecessary().
#
# ERRORS: Could not find an unused temp file name after 100 tries
#
proc tempFileGet {} {
    set maxcount 100
    set count 0
    set file ""
    while {$file == "" && $count < $maxcount} {
        incr count
        set file [expr rand()]
        # remove decimal place
        regsub \\. $file {} file
        append file Tclprocess_output.tmp
        set file [file join [Wind::tmpDirGet] $file]
        if {[file exists $file]} {
            set file ""
        }
    }
    if {$count == [expr $maxcount]} {
        error "Could not find a free temp file in [Wind::tmpDirGet]"
    }
    return $file
}

#########################################################################
#
# bspsEnumerate - get the list of BSPs found in <wind_base>/target/config 
# BSPs are directories found in this path containing the file config.h
#
# SYNOPSIS
#     bspsEnumerate
#
# RETURNS
#    list of fully qualified BSP directory paths
#
# ERRORS N/A
#
proc bspsEnumerate {} {
	::beginWaitCursor
	::prjStatusLog "Reading directory.  Please wait..." 0 20 

	set dirs [glob -nocomplain [wtxPath target config]\*]
	::prjStatusLog "Looking for BSPs.  Please wait..." 20 20

	set count [llength $dirs]
	set idx 0
	foreach dir $dirs {
		set name [file tail $dir]
		if {[file isdirectory $dir]} {
			if {[file exists [file join $dir config.h]]} {
				::prjStatusLog "Found '$name'..." $idx $count
				lappend bspList $dir
			} else {
				::prjStatusLog "Checking '$name'.  Please wait..." $idx $count
			}
		} else {
			::prjStatusLog "Checking '$name'.  Please wait..." $idx $count
		}
		incr idx
	}
	if {![info exists bspList]} {
		set bspList ""
	}

	::prjStatusLog ""
	::endWaitCursor

	return $bspList
}

#########################################################################
#
# projectsEnumerate - get the list of projects found in
#     <wind_base>/target/proj matching the type specified by prjNS
#
# SYNOPSIS
#    projectsEnumerate
#
# RETURNS: list of projects of type prjNS, as fully qualified .wpj file paths
#
# ERRORS: N/A
#
proc projectsEnumerate {prjNS} {
	::beginWaitCursor
	::prjStatusLog "Reading directory.  Please wait..." 0 20 
	set dirs [glob -nocomplain [::prjDirDefault]\/*]
	set count [llength $dirs]
	::prjStatusLog "Looking for projects.  Please wait..." 0 $count 

	set idx 0
	foreach dir $dirs {
		if {[file isdirectory $dir]} {
			set prjs [glob -nocomplain [file join $dir \*.wpj]]
			if {$prjs != ""} { 
				foreach prj $prjs {
					set name [file tail $prj]
					::prjStatusLog "Found '$name'..." $idx $count
					lappend prjList $prj
				}
			} else {
				::prjStatusLog "Checking '$dir'.  Please wait..." $idx $count
			}
		}
		incr idx
	}
	if {![info exists prjList]} {
		set prjList ""
	}

	::prjStatusLog ""
	::endWaitCursor

	return $prjList
}

#########################################################################
#
# prjDirDefault - default directory for creating new projects.
#
# This fetches the environment variable WIND_PROJ_BASE if defined, or returns
# $WIND_BASE/target/proj otherwise. It returns a UNIX-style path, i.e. with 
# forward slashes. 
#
# SYNOPSIS 
#   prjDirDefault   
#
# RETURNS: N / A
#
# ERRORS: N / A
#
set prjDirDefault_target_proj_path ""
proc prjDirDefault {} {
    global prjDirDefault_target_proj_path

    if {[info exist ::env(WIND_PROJ_BASE)]} {
        set prjDirDefault_target_proj_path $::env(WIND_PROJ_BASE)
    } elseif {$prjDirDefault_target_proj_path != ""} {
        return $prjDirDefault_target_proj_path
    } else {
        set cwd [pwd]
        cd [wtxPath]target/proj
        set prjDirDefault_target_proj_path [pwd]
        cd $cwd
    }
    ::filePathFix prjDirDefault_target_proj_path
    return $prjDirDefault_target_proj_path
}

#########################################################################
#
# filenameQuoteIfNecessary - quotes a filename if it has spaces
#
# TCL file commands (e.g. file, open) can handle filenames with spaces.
# But when executing a shell command using exec, such filenames need to
# be quoted. So a typical usage is
#
# eval exec $command 2> [filenameQuoteIfNecessary $errorFile]
#
# SYNOPSIS
#   filenameQuoteIfNecessary  filename
#
# PARAMETERS
#   filename: file path, with forward slashes, e.g. c:/temp, c:/te mp
#
# RETURNS: path quoted if necessary, e.g. c:/temp, "c:/te mp"
#
# ERRORS: N / A
#
# EXAMPLES
# eval exec $command 2> [filenameQuoteIfNecessary $errorFile]
# \ce

proc filenameQuoteIfNecessary {filename} {
    if {[string first " " $filename] != -1} {
        set filename \"$filename\"
    }
    return $filename
}

#########################################################################
#
# textControlTextSetKeepingSelection - sets text control, keeping selection if possible
#
# PARAMETERS 
#   textControl: text control name
#   text:   new text to set in control
#
# RETURNS: N / A
#
# ERRORS: N / A
#
proc textControlTextSetKeepingSelection {textControl text} {
    # need to use join because selection is returned in form "{9 9}"
    set selection [join [controlSelectionGet $textControl]]
    controlTextSet $textControl $text
    set length [string length $text]
    if {[lindex $selection 0] <= $length && \
        [lindex $selection 1] <= $length } {
        controlSelectionSet $textControl $selection
    }
}

proc GetWtxObjModuleLoadOptions { } {
	::DownloadOptionsPage::GetWtxObjModuleLoadOptions
}

namespace eval DialogRename {
	set _controlTextNewName textNewName
	set _controlButtonOk buttonOK
	set _controlButtonCancel buttonCancel
	set _strDialogName [namespace current]
	set _strNewName {}
	set _strOldName {}
}

# returns a the new name, or "" if cancel
proc DialogRename::DoModal { strWhat strOldProjectName } {
	# this code was originally written to rename projects, so that
	# explains the variable names
	set nTextHeight 14
	set nLabelHeight 10
	set nButtonHeight 14
	set nButtonWidth 50
	set nBorder 6
	set nInterspace 4
	set nTextWidth 100

	set nDialogWidth [expr { $nBorder * 2 + $nInterspace * 2 + $nTextWidth + $nButtonWidth}]
	set nDialogHeight [expr { $nBorder * 2 + $nInterspace + $nButtonHeight * 2 }]
	set nButtonX [expr { $nBorder + $nTextWidth + $nInterspace * 2}]

	variable _strNewName
	variable _strDialogName
	variable _controlTextNewName
	variable _controlButtonOk
	variable _controlButtonCancel
	variable _strOldName
	
	set _strNewName {}
	set _strOldName $strOldProjectName

	dialogCreate -name $_strDialogName -title "Rename $strWhat $strOldProjectName" \
		-width $nDialogWidth -height $nDialogHeight \
		-init [namespace current]::OnInit -controls \
		[list [list label -title "Enter a &new project name:" \
				   -name lbl1 -x $nBorder -y $nBorder -w $nTextWidth -h $nLabelHeight] \
			 [list text -name $_controlTextNewName -x $nBorder \
				  -y [expr { $nBorder + $nLabelHeight + $nInterspace }] -w $nTextWidth \
				  -h $nTextHeight] \
			 [list button -name $_controlButtonOk -title "&OK" -width $nButtonWidth \
				  -height $nButtonHeight -x $nButtonX -y $nBorder -default\
				  -callback [namespace current]::OnOk ]\
			 [list button -name $_controlButtonCancel -title "&Cancel" \
				  -width $nButtonWidth -height $nButtonHeight -x $nButtonX \
				  -y [expr {$nBorder + $nButtonHeight + $nInterspace}] \
				  -callback [namespace current]::OnCancel] \
			]
	return $_strNewName
}

proc DialogRename::OnInit { } {
	variable _strOldName
	variable _strDialogName
	variable _controlTextNewName
	controlValuesSet $_strDialogName.$_controlTextNewName $_strOldName
}

proc DialogRename::OnOk { } {
	variable _strNewName
	variable _strOldName
	variable _strDialogName
	variable _controlTextNewName

	set _strNewName [controlValuesGet $_strDialogName.$_controlTextNewName]
	# if they click OK, but they haven't changed the name, then it's
	# effectiviely a Cancel
	if { $_strNewName == $_strOldName } {
		set _strNewName {}
		windowClose $_strDialogName
	} elseif { ![isFileNameOk $_strNewName] } {	
		messageBox "The name you have entered contains illegal characters.  Please enter another name."
	} else {
		windowClose $_strDialogName
	}
}

proc DialogRename::OnCancel { } {
	variable _strNewName
	variable _strDialogName

	set _strNewName {}
	windowClose $_strDialogName
}

namespace eval UpdateDialog {
    variable _bRunningAlready 0

    variable _bUpdate 0
    variable _initialDir ""
    variable _callback ""

    ###################################################
    #
    # ::UpdateDialog::onUpdate - 
    # Dialog to show allow the user to edit, enter or browse
    # for a directory
    #
    # SYNOPSIS
    #     onUpdate parent path callback bUpdate
    #
    # PARAMETERS
    #     parent:   parent dialog
    #     path:     initial directory/entry
    #     callback: a proc called when the ok button is pressed;
    #               proc callback {yourCookie selection}
    #     bUpdate:  determines whether user is editing an existing path,
    #               or entering a new one
    # RETURNS: n/a
    #
    proc onUpdate {parent path callback bUpdate} {
        set ::UpdateDialog::_bUpdate $bUpdate
        set ::UpdateDialog::_initialDir $path
        set ::UpdateDialog::_callback $callback

        if {$::UpdateDialog::_bUpdate} {
            set title "Edit Include Path"
        } else {
            set title "Add Include Path"
        }
        dialogCreate -name ::UpdateDialog::IDD_UPDATE_PATH \
            -title $title -width 240 -height 50 \
            -parent $parent \
            -init ::UpdateDialog::updateInit
    }

    proc updateInit {} {

        set controls [list \
            [list text -name ID_TEXT -x 10 -y 10 \
             -width 140 -height 12 \
             -callback ::UpdateDialog::onEdit] \
            [list button -name ID_OK -title "OK" -x 180 -y 10 \
             -width 50 -height 14 -default \
             -callback ::UpdateDialog::onOk] \
            [list button -name ID_CANCEL -title "Cancel" -x 180 -y 30 \
             -width 50 -height 14 \
             -callback ::UpdateDialog::onCancel] \
            [list button -name ID_BROWSE -title "..." -x 155 -y 10 \
             -width 14 -height 14 \
             -callback ::UpdateDialog::onAdd] \
        ]
        foreach control $controls {
            controlCreate ::UpdateDialog::IDD_UPDATE_PATH $control
        }


        controlTextSet \
            ::UpdateDialog::IDD_UPDATE_PATH.ID_TEXT \
            [set ::UpdateDialog::_initialDir]
        controlSelectionSet \
            ::UpdateDialog::IDD_UPDATE_PATH.ID_TEXT 0 10000
        controlEnable \
            ::UpdateDialog::IDD_UPDATE_PATH.ID_OK 0
    }

    proc onOk {} {
        set selection [controlTextGet \
            ::UpdateDialog::IDD_UPDATE_PATH.ID_TEXT]
        ::filePathFix selection 
        set ::UpdateDialog::initialDir $selection
        eval [set ::UpdateDialog::_callback] $selection
        windowClose ::UpdateDialog::IDD_UPDATE_PATH
    }

    proc onCancel {} {
        windowClose ::UpdateDialog::IDD_UPDATE_PATH
    }

    proc onEdit {} {
        set selection [controlTextGet \
            ::UpdateDialog::IDD_UPDATE_PATH.ID_TEXT]
        if {$selection == ""} {
            controlEnable \
                ::UpdateDialog::IDD_UPDATE_PATH.ID_OK 0
        } else { 
            controlEnable \
                ::UpdateDialog::IDD_UPDATE_PATH.ID_OK 1
        }
    }

    proc onAdd {} {
        # Feeble attempt to prevent reentry, since dirBrowseDlg doesn't
        # have a parent
        if {$::UpdateDialog::_bRunningAlready} {
            messageBeep -exclamation
            return
        } else {
            set ::UpdateDialog::_bRunningAlready 1
        }

        set path [dirBrowseDialogCreate \
            -title "Select or enter directory" \
            -initialdir \
                [file nativename \
                [controlValuesGet ::UpdateDialog::IDD_UPDATE_PATH.ID_TEXT]] \
            -okbuttontitle "Add"]

        set ::UpdateDialog::_bRunningAlready 0

        if {$path != ""} {
            set path [file nativename $path] 
            controlTextSet \
                ::UpdateDialog::IDD_UPDATE_PATH.ID_TEXT $path
            controlSelectionSet \
                ::UpdateDialog::IDD_UPDATE_PATH.ID_TEXT 0 9999 
        }
    }

    ###################################################
    #
    # ::UpdateDialog::isUpdate - 
    # Determine whether this was a New/Add or an Edit/Update 
    #
    # SYNOPSIS
    #     isUpdate
    #
    # PARAMETERS n/a
    #
    # RETURNS:
    # 1 if this is an edit/update, 0 if new/add
    #
    proc isUpdate {} {
        return [set ::UpdateDialog::_bUpdate]
    }
}
