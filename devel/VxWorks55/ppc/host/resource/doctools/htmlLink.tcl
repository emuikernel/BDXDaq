# htmlLink.tcl - install cross references in HTML documents
#
# Copyright 1998 Wind River Systems, Inc.
#
# modification history
# --------------------
# 01o,11feb02,wsl  add *.lib and *.rtn files to LIB and RTN files
# 01n,07feb02,wsl  change extensions of nav files to .htm (instead of .html)
# 01m,29feb00,dgp  change ERROR reporting to Error to conform to build scripts
# 01l,11mar99,wsl  revised comment header
# 01k,04mar99,fle  made htmlLink able to ignore sections with no chapter names
# 01j,26feb99,fle  documented and enhanced -skip option
# 01i,30nov98,p_m  Updated documentation
# 01h,10nov98,fle  changed the titles form the index files
# 01g,03nov98,fle  removed PARAMETERS formatting
# 01f,16oct98,fle  updated since subEntriesFileCreate <rule> parameter
#                  + removed all references to WIND_BASE
# 01e,06oct98,dbt  doc: fixed documentation.
# 01d,30sep98,fle  adapted to new category field
# 01c,09sep98,fle  added the -skip option
# 01b,18aug98,fle  made it use the IMPORT files
# 01a,29jul98,fle  written
#

#
# DESCRIPTION
# This tool uses the information in the linkage files RTN and LIB to create
# HTML links from references in documents to the appropriate reference
# entries.  It can also concatenate the various RTN and LIB files under each
# directory into RTN.SUB and LIB.SUB files recursively.
#
# This command is used to create all the cross reference links
# to HTML reference manuals previously generated with `refgen'.
# It should only
# be used after all the HTML documentation files have been
# built for a given project.
#
# SYNOPSIS
#   htmlLink [-h] [-R] [-skip <dirName>] [<directory>]
#
# OPTIONS
# .iP -h 12 3
# Display a brief message describing the usage of this tool.
# .iP -R 
# Make links in all subdirectories (recursion).
# .iP "-skip <dirName>" 
# Do not process directory <dirName> when -R flag is in effect.
# Each directory to be skipped must be listed separately, as:
# .CS
#   htmlLink -R -skip notes -skip lost+found -skip private
# .CE
# .iP <directory>
# The directory to be processed.  If the -R flag is used, all its
# subdirectories will also be processed.  If no directory is specified, the
# current working directory is processed.
#
# RETURNS: N/A
#
# ERRORS: N/A
#
# EXAMPLE
# 
# Building cross-references for a BSP help files.
#
# .CS
# Go to the BSP directory and build the HTML documentation:
#
#   % cd $WIND_BASE/target/config/myBsp
#   % make man
#
# Generate cross-references in the BSP help files:
#
#   % cd $WIND_BASE/docs/vxworks/bsp/myBsp
#   % htmlLink .
# .CE
#
# SEE ALSO
#   `refgen', `windHelpLib', `htmlBook'
#
# NOROUTINES
#

################################################################################
#
# docFileFromEntryFileUpdate - update the local doc files from the entry files
#
# This routine updates all the doc files from <docFileList> according to 
# <entryFileList>.
#
# SYNOPSIS
#   docFileFromEntryFileUpdate <docFileList> <entryFileList>
#
# PARAMETERS
#   docFileList : list of doc files to be updated
#   entryFileList : list of entry files to update doc files with
#
# RETURNS: a list containing OK or ERROR as a first element, and error
#          description if operation failed in remaining elements
#
# ERRORS:
#   File manipulation error
#
# EXAMPLE
# .CS
#   docFileFromEntryFileUpdate {./tgtmem.html ./tgtmgt.html} {LIB RTN}
# .CE
#

proc docFileFromEntryFileUpdate { docFileList entryFileList } {

    set entryList {}
    set status { {OK} }

    # check for empty doc file list

    if { $docFileList == {} } {
	return $status
    }

    # get all entries from all the entry files

    set entryList [entryListInFileGet $entryFileList]

    # create lists of patterns and links

    set patternList {}
    set linkList {}
    foreach entry $entryList {
	lappend patternList [lindex $entry 2]
	lappend linkList [lindex $entry 3]
    }

    # update links in doc files

    foreach docFile $docFileList {

	# get file content

	if { [catch "open $docFile r" fileId] } {

	    # set error message

	    set status [lreplace $status 0 0 ERROR]
	    lappend status $fileId
	    continue
	}

	set fileContent [read $fileId]
	close $fileId

	# replace all patterns with links

	set patternNum 0
	foreach pattern $patternList {

	    regsub -all $pattern $fileContent [lindex $linkList $patternNum]\
		   fileContent

	    incr patternNum
	}

	# save updated content into file

	if { [catch "open $docFile w" fileId] } {

	    # set error message

	    set status [lreplace $status 0 0 ERROR]
	    lappend status $fileId
	    continue
	}

	puts $fileId $fileContent
	close $fileId
    }

    return $status
}

################################################################################
#
# linkToFileTarget - updates a link toward a target file
#
# This routine retrieves the link as it had to be seen by the target file
# <toFile> knowing <link>
#
# SYNOPSIS
#   linkToFileTarget <toFile> <link>
#
# PARAMETERS
#   toFile : tha target file
#   link : the link description
#
# RETURNS: The updated link
#
# ERRORS: N/A
#
# EXAMPLE
# .CS
#   linkToFileTarget ./windsh/windsh.html#windsh ./wtxregd/wtxregd.html
#   ../windsh/windsh.html#windsh
# .CE
#

proc linkToFileTarget { toFile link } {

    # split the file name to get its depth

    set splittedFile [file split $toFile]
    if { [lindex $splittedFile 0] == "." } {
	set splittedFile [lrange $splittedFile 1 end]
    }

    # split the link to get its depth

    set splittedLink [file split $link]
    if { [lindex $splittedLink 0] == "." } {
	set splittedLink [lrange $splittedLink 1 end]
    }

    # get depth of file name and link

    set fileDepth [llength $splittedFile]
    set linkDepth [llength $splittedLink]

    set updatedLink ""
    set depth 0

    # keep common elements

    while { [lindex $splittedFile $depth] == [lindex $splittedLink $depth] } {
	incr depth
    }

    # remove common elements from link

    set splittedLink [lrange $splittedLink $depth end]

    # add the depths difference path

    set depthDiff [expr $fileDepth - $depth - 1]
    for { set ix 0 } { $ix < $depthDiff } { incr ix } {
	set updatedLink "../$updatedLink"
    }

    # always begin with ./ if link is nul

    if { $updatedLink == "" } {
	set updatedLink "."
    }

    # go down the link path

    foreach elt $splittedLink {
	set updatedLink "$updatedLink/$elt"
    }

    # return the correct updated link

    return [fileNameFixup $updatedLink]
}

################################################################################
#
# docLinksUpdate - update links in doc files
#
# This routine updates the doc files links from current directory, recursively
# or not.
#
# If <recurseMode> is set to 1, then all the sub-folders doc files links will
# be updated too.
#
# This routine first creates the SUB files (help entries from one-level
# sub-folders). If in recurse mode, then it does it for all the sub-folders.
#
# Updates all the doc files from the RTN and LIB files, doing it first locally.
#
# It then generates the IMPORT files, assuming that import files are made of
# all the `bolded' elements of a doc file. Those IMPORT files are then used to
# generate cross links between files if they are not in the same directory.
#
# SYNOPSIS
#   docLinksUpdate [<inDir>] [<recurseMode>]
#
# PARAMETERS
#   inDir : directory in which to update doc files links
#   recurseMode : set to one if links update has to be recursive (default 1)
#   skipDirList : list of directories to be skipped
#
# RETURNS: a list whisch first element is error status ( OK or ERROR ), and 
#          remaining part of list are error messages caught (if error status is
#          (ERROR)
#
# ERRORS: N/A
#

proc docLinksUpdate { {inDir "."} {recurseMode 1} {skipDirList {}} } {

    global htmlRoutineIndex
    global htmlLibraryIndex

    cd $inDir				;# go in target dir

    set treeDepth {}			;# doc tree depth
    set depthMax 0			;# doc tree max depth
    set docTree [treeGet]		;# get the doc tree

    # get rid of the skipped directories

    set tmpDocTree $docTree
    set docTree {}

    foreach dirName $tmpDocTree {

	set skipIt 0

	foreach skipDir $skipDirList {
	    set splitSkipDir [file split $skipDir]
	    set splitDirName [lrange [file split $dirName] 0 \
				     [expr [llength $splitSkipDir] - 1]]

	    if { $splitDirName == $splitSkipDir } {
		set skipIt 1
	    }
	}

	if { (! $skipIt) && ($dirName != {}) } {
	    lappend docTree $dirName
	}
    }

    # update links in all html files from each directory

    if { $recurseMode } {
	set baseDir [pwd]
	foreach directory $docTree {
	    cd $directory
	    file delete "$htmlLibraryIndex"
	    file delete "$htmlRoutineIndex"
	    set docFileList [glob -nocomplain *.html]
	    docFileFromEntryFileUpdate $docFileList \
		    [glob -nocomplain RTN *.rtn LIB *.lib]
	    cd $baseDir
	}
    }

    file delete "$htmlLibraryIndex"
    file delete "$htmlRoutineIndex"
    set docFileList [glob -nocomplain *.html]
    docFileFromEntryFileUpdate $docFileList \
	    [glob -nocomplain RTN *.rtn LIB *.lib]

    # get the visible sub tree from current directory, and sort it by tree depth

    foreach subDir $docTree {

	set depth [llength [file split $subDir]]
	if { $depth > $depthMax } {
	    set depthMax $depth
	}

	lappend treeDepth "$subDir $depth"
    }

    # sort tree in term of depth

    set treeDepth [lsort -integer -decreasing -index 1 $treeDepth]

    # now update the entry files for all the directories that have a depth value
    # smaller than the max depth value (there is no need to update entry files
    # for a directory that doesnt have any sub folder)

    if { $recurseMode } {
	set baseDir [pwd]

	foreach dirDesc $treeDepth {
	    if { [lindex $dirDesc end] < $depthMax } {
		cd [lindex $dirDesc 0]
		subEntriesFileCreate -add "LIB" ;# includes *.lib
		subEntriesFileCreate -add "RTN" ;# includes *.rtn
		cd $baseDir
	    }
	}
    }

    # create all the sub directories ebtry files : take all entries from the
    # one level sub directories and save them in <fileName>.SUB

    subEntriesFileCreate -add "LIB" ;# includes *.lib
    subEntriesFileCreate -add "RTN" ;# includes *.rtn

    # get list of all available patterns in RTN and LIB files

    set entryList [entryListInFileGet \
	    [glob -nocomplain LIB *.lib LIB.SUB RTN *.rtn RTN.SUB]]

    # create a list of the patterns and associated links, and remove \ chars
    # from patterns

    set patternList {}
    set linkList {}

    set entryList [lsort -dictionary -index 0 $entryList]
    foreach entry $entryList {
	lappend patternList [stringReplace "\\" "" [lindex $entry 2]]
	lappend linkList [stringReplace "\\" "" [lindex $entry 3]]
    }

    # We now should have all the existing (from this folder) defined
    # routines / libraries patterns and links.
    # get the undefined patterns for all html files

    if { $recurseMode } {
	foreach directory $docTree {
	    entryImportFileCreate "*.html" $directory
	}
    }

    # now for local dir

    entryImportFileCreate "*.html" "."

    # get the list of IMPORT files in order to update html Files

    set importFileList [recurseFilesFind "IMPORT"]

    set importEntryList {}

    foreach importFile $importFileList {
	if { ! [catch "open $importFile r" fileId] } {
	    set importEntry [gets $fileId]
	    while { ! [eof $fileId] } {

		# create a list of help entries from import entries

		lappend importEntryList [list {} {} [lindex $importEntry 0] {} \
					      {} {} {} [lindex $importEntry 1]]
		set importEntry [gets $fileId]
	    }

	    close $fileId
	}
    }

    # sort import entries from their doc file name

    set entryImportByFileList [entryListSort -loc $importEntryList]

    # create a list of 
    #   - { { fileName1 {importName1 importName2 ...} }
    #       { fileName2 {importName1 importName2 ...} } 
    #       ... }

    set prevImportFileName ""
    set importNameList {}
    set entryByFileList {}

    foreach importEntry $entryImportByFileList {

	set importFileName [lindex $importEntry end]

	if { $importFileName != $prevImportFileName } {

	    if { $prevImportFileName != "" } {
		lappend entryByFileList [list $prevImportFileName \
					      $importNameList]
		set importNameList {}
	    }

	    set prevImportFileName $importFileName
	}

	lappend importNameList [lindex $importEntry 2]
    }

    # append the last list to the entryByFileList list

    if { $prevImportFileName != {} && $importNameList != {} } {
	lappend entryByFileList [list $prevImportFileName $importNameList]
    }

    # replace all the bolded patterns with their links in all html files that
    # need importations links

    foreach htmlFileToUpdate $entryByFileList {

	# get html file name

	set fileName ".[string range [lindex $htmlFileToUpdate 0] \
			       [string length [pwd]] end]"

	# get list of imported items

	set importNameList [lindex $htmlFileToUpdate 1]

	if { [file exists $fileName] && [file readable $fileName] && \
	     [file writable $fileName] } {

	    # get the doc file content

	    set fileId [open $fileName r]
	    set fileContent [read $fileId]
	    close $fileId

	    # look for all the importNames and if they can be solved

	    foreach importName $importNameList {

		set patternNum [lsearch $patternList $importName]

		if { $patternNum != -1 } {

		    # imported pattern has been found in the list of available
		    # links

		    set htmlLink [lindex $linkList $patternNum]
		    set linkTarget [htmlLinkExtract $htmlLink]

		    # update the html link toward the html target file

		    set updatedLink [linkToFileTarget $fileName $linkTarget]
		    set splittedLink [stringIsolate "\"" "\"" $htmlLink]
		    set updatedLink "[lindex $splittedLink 0]\"$updatedLink\"\
				     [lindex $splittedLink 2]"

		    set fileContent [stringReplace $importName $updatedLink \
						   $fileContent]
		}
	    }

	    # rewrite the updated doc file

	    set fileId [open $fileName w]
	    puts $fileId $fileContent
	    close $fileId
	}
    }

    # create a libIndex.htm and rtnIndex.htm file for each sub directory

    if { $recurseMode } {
	foreach subDir $treeDepth {

	    cd [lindex $subDir 0]			;# moving to sub dir

	    # gather informations for libraries index

	    set libEntryListIndex {}
	    set libEntryList [entryListInFileGet \
		    [glob -nocomplain "LIB.SUB" LIB *.lib]]

	    # for the libraries / tools indexes, put the name of the book then
	    # the name of the chapter as the file title

	    set oneEntry [lindex $libEntryList 0]
	    set bookName [lindex $oneEntry 4]
	    set chapterName [lindex $oneEntry 5]
	    set libIxTitle "$bookName : $chapterName"

	    if { $libEntryList != {} } {
		lappend libEntryListIndex [list $libEntryList $libIxTitle {} 1]
		htmlIndexCreate $libEntryListIndex  "$htmlLibraryIndex"
	    }

	    # gather information for routines index

	    set rtnEntryListIndex {}
	    set rtnEntryList [entryListInFileGet \
		    [glob -nocomplain "RTN.SUB" RTN *.rtn]]
	    if { $rtnEntryList != {} } {
		lappend rtnEntryListIndex [list $rtnEntryList \
						"$chapterName : Routines" {} 1]
		htmlIndexCreate $rtnEntryListIndex "$htmlRoutineIndex"
	    }

	    cd $baseDir					;# come back to base dir
	}
    }

    # now generate hml indexes file in current directory
    # gather informations for libraries index

    set libEntryListIndex {}
    set libEntryList [entryListInFileGet [glob -nocomplain "LIB.SUB" LIB *.lib]]

    if { $libEntryList != {} } {
	lappend libEntryListIndex [list $libEntryList "Libraries" {} 0]
	htmlIndexCreate $libEntryListIndex  "$htmlLibraryIndex"
    }

    # gather information for routines index

    set rtnEntryListIndex {}
    set rtnEntryList [entryListInFileGet [glob -nocomplain "RTN.SUB" RTN *.rtn]]
    if { $rtnEntryList != {} } {
	lappend rtnEntryListIndex [list $rtnEntryList "Routines" {} 1]
	htmlIndexCreate $rtnEntryListIndex "$htmlRoutineIndex"
    }
}

################################################################################
#
# htmlLinkCommandeParse - parses the htmlLink utility commande line
#
# This is the commmande line parser for htmlLink utility.
#
# SYNOPSIS
#   htmlLinkCommandeParse <argv> <argc>
#
# PARAMETERS
#   argv : the arguments vector
#   argc : the arguments counter
#
# RETURNS: N/A
#
# ERRORS: N/A
#

proc htmlLinkCommandeParse { argv argc } {

    # global variables

    global htmlLinkRecurseMode		;# do we use recurse mode or not ?
    global htmlLinkOutDir		;# directory to update html file in
    global htmlSkipDirList		;# list of skipped directories

    # case of empty commande line

    set argList $argv
    set argCount [llength $argList]

    #parsing command

    for {set ix 0} {$ix < $argCount} {incr ix} {
	switch -- [lindex $argList $ix] {

	    "-h" {
		htmlLinkUsage ""
	    }

	    "-R" {
		set htmlLinkRecurseMode 1
	    }

	    "-skip" {
		incr ix
		lappend htmlSkipDirList [lindex $argv $ix]
	    }

	    default {
		set htmlLinkOutDir [lindex $argList $ix]
		if { ! [file isdirectory $htmlLinkOutDir] } {
		    set errMsg "*** htmlLink Error ***\n"
		    append errMsg "\nUnknown directory or option\
				   $htmlLinkOutDir\n"
		    htmlLinkUsage $errMsg
		}
	    }
	}
    }
}

################################################################################
#
# htmlLinkUsage - diplays the htmlLink utility usage
#
# This procedure displays the htmlLink utility help message.
#
# SYNOPSIS
#   htmlLinkUsage <msg>
#
# PARAMETERS
#   msg : the message to display previously to the help message
#
# RETURNS: N/A
#
# ERRORS: N/A
#

proc htmlLinkUsage { msg } {
    puts stdout "$msg"
    puts stdout "htmlLink : updates links in an HTML file from entry files\n\n"
    puts stdout "usage : htmlLink \[-h\] \[-R\] \[<dirName>\]"
    puts stdout "\t\[-h\]\t\t\tdisplays this help message"
    puts stdout "\t\[-R\]\t\t\tTo update links recursively"
    puts stdout "\t\[-skip\] <dirList>\tTo skip the given directory"
    puts stdout "\t\[<dirName>\]\t\tdirectory path to update html files in"
    exit
}

# globale variables

global htmlLinkOutDir
global htmlLinkRecurseMode
global htmlSkipDirList
global htmlRoutineIndex
global htmlLibraryIndex

# globals initialization

set htmlLinkOutDir "."
set htmlLinkRecurseMode 0
set htmlSkipDirList {}
set htmlRoutineIndex "rtnIndex.htm"
set htmlLibraryIndex "libIndex.htm"

# set source directory

if { [catch "set htmlLinkSrcPath [wtxPath host resource]doctools"] } {
    set htmlLinkSrcPath "."
}

# source Tcl libraries

source "$htmlLinkSrcPath/fileLib.tcl"
source "$htmlLinkSrcPath/helpEntryLib.tcl"
source "$htmlLinkSrcPath/stringLib.tcl"
source "$htmlLinkSrcPath/htmlLib.tcl"

# parse the commande line

htmlLinkCommandeParse $argv $argc

docLinksUpdate $htmlLinkOutDir $htmlLinkRecurseMode $htmlSkipDirList
