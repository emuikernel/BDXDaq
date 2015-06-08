# fileLib.tcl - files manipulation library
#
# Copyright 1998 Wind River Systems, Inc.
#
# modification history
# --------------------
# 01c,08nov99,fle  fixed a bug in recurseFilesFind procedure
# 01b,08feb99,fle  fixed a bug in treeGet when following links
# 01a,24aug98,fle  written, based on htmlLink.tcl 01a
#

#
# DESCRIPTION
# This library provides Tcl with several files manipulations procedures like
# getting a whole tree.
#

#
# INTERNAL
#
# This Tcl source file contains the following procedures :
#
# fileNameFixup		- gets the name of a file without `/' repetitions
# recurseFilesFind	- recursively get all the specified file names
# treeGet		- gets the directories tree from current directory

################################################################################
#
# fileNameFixup - gets the name of a file without `/' repetitions
#
# This procedure returns the name of a file getting rid of all the `/'
# repetitions patterns.
#
# SYNOPSIS
#   fileNameFixup fileName
#
# PARAMETERS
#   fileName : the file name to be corrected
#
# EXAMPLE
#
# .CS
#   fileNameFixup c:/docs//htlm/help.html
#   c:/docs/htlm/help.html
# .CE
#
# RETURNS: The corrected file name
#
# ERRORS: N/A
#

proc fileNameFixup { fileName } {

    set splittedFileName [file split $fileName]

    if { [string first "/" [lindex $splittedFileName 0]] != -1 } {

	set firstElt [join "[lindex $splittedFileName 0] \
			   [lindex $splittedFileName 1]" {}]

	set splittedFileName [lrange $splittedFileName 2 end]
	set splittedFileName [linsert $splittedFileName 0 $firstElt]
    }

    set correctFileName [join $splittedFileName /]
    return $correctFileName
}

################################################################################
#
# recurseFilesFind - recursively get all the specified file names
#
# This routines retrieves recursively all the <fileName> occurence in all
# sub directories.
#
# SYNOPSIS
#   recurseFilesFind fileNameToFind
#
# PARAMETERS
#   fileNameToFind : the file name to retrieve recursively
#
# RETURNS: The list of found files or {} if no files are found
#
# ERRORS: N/A
#

proc recurseFilesFind { fileNameToFind } {

    set foundFiles	{}			;# list of found files
    set subDirList	{}			;# sub directories list
    set fileNames	[glob -nocomplain *]	;# current directory file names
    set prevPath	""			;# previous working dir

    # first look for local occurences of <fileNameToFind>

    set localFiles [glob -nocomplain $fileNameToFind]
    foreach localFile $localFiles {
	lappend foundFiles "[pwd]/$localFile"
    }

    # then look for all sub directories for recursive search

    foreach fileName $fileNames {

	# update the list of directories

	if { [file isdirectory $fileName] } {
	    lappend subDirList $fileName
	}
    }

    # now look for file name in all sub directories

    foreach dirName $subDirList {

	set prevPath [pwd]
	cd $dirName
	set subDirFoundFiles [recurseFilesFind $fileNameToFind]

	foreach foundFile $subDirFoundFiles {
	    lappend foundFiles $foundFile
	}

	cd $prevPath
    }

    # now return the list

    set foundFiles [lsort $foundFiles]
    return $foundFiles
}

################################################################################
#
# treeGet - gets the directories tree from current directory
#
# This procedure gets all the directories and su directories from current
# directory
#
# SYNOPSIS
#   treeGet
#
# PARAMETERS: N/A
#
# RETURNS: A list containing all the sub directories or {} if no sub-dirs
#

proc treeGet {} {

    set dirList {}
    set fileNames [glob -nocomplain *]

    # get all the directoris from current location

    foreach fileName $fileNames {
	if { [file isdirectory $fileName] } {
	    lappend dirList $fileName
	}
    }

    # get all the sub directories from the directories list

    foreach dirName $dirList {

	set oldPwd [pwd]
	cd $dirName
	set subDirList [treeGet]

	foreach subDir $subDirList {
	    lappend dirList "$dirName/$subDir"
	}

	cd "$oldPwd"
    }

    # return the list of directories and sub directories

    set dirList [lsort $dirList]
    return $dirList
}
