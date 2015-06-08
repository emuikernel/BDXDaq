# windHelpLib.tcl - Tornado help Tcl library
#
# Copyright 1998 Wind River Systems, Inc.
#
# modification history
# --------------------
# 02c,18apr02,wsl  add handling of *.lib and *.rtn files
# 02b,30oct01,mht  fix SPR#29181: On line Help index shows duplicate entries
# 02a,19mar99,fle  doc : synopses put between .tS /.tE
# 01z,11mar99,fle  made the file exeistency check apply only on files (not on
#                  URLs) in windHelpDisplay
# 01y,10mar99,fle  rewrote windHelpTreeBuild. The tree was not completely tree
#                  control compliant
# 01x,04mar99,fle  translated all URL's backslashes to slash for browsers
#                    conveniance
#                  + made it able to deal with entries that have no chapter name
# 01w,19feb99,jmp  added startNetscapeFromUI testing on hpux to workaround
#                  (SPRs #24072, #24300).
# 01v,26jan99,fle  made TREE.WRS have \n to have less than 8000 characters
#                  line for clearcase conveniance
# 01u,15jan99,fle  fixed a typo
# 01t,11dec98,jmp  fixed a regular expression in windHelpLinkGet (SPR #23869).
# 01s,02dec98,fle  fixed a problem with windHelpLinkGet and associated
#                  keywords
# 01r,25nov98,fle  made INDEX.WRS and TREE.WRS generated in the docs directory
#                  and not in the ~/.wind directory anymore
# 01q,04nov98,fle  doc : fixed some typos
#                  + made windHelpInit look for the updateDocTime file
# 01p,23oct98,fle  added -raise option when calling a netscape browser
#                  + made windHelpDisplay return status
# 01o,21oct98,l_k  windHelpDisplay call to winhtml winWebBrowserNavigate added
# 01n,21oct98,fle  removec .docDir.tcl file that could become problematic
# 01m,20oct98,fle  removed sourcing time evaluations
# 01l,14oct98,fle  fixed a typo
# 01k,13oct98,jmp  modified windHelpLinkGet to support several keyWords using
#                  kWord1|kWord2|kWordn syntax.
# 01j,02oct98,fle  added possibility to retrieve help entries from books,
#                  chapters and catagories in windHelpListGet
# 01i,01oct98,jmp  renamed netNavigator in htmlBrowser to avoid conflict with
#                  pre Tornado 2.0 netNavigator setting in .wind/launch.tcl.
# 01h,30sep98,fle  made wtxExecutableFind in a catch command in case Tcl
#                  application has not loaded wtxtcl dll
# 01g,22sep98,fle  adapted windHelpDisplay to wtxExecutableFind
# 01f,02sep98,fle  added windHelpListGet and moved some APIs in other libs
# 01e,25aug98,jmp  fixed syntax error introduced by last change.
# 01d,24aug98,jmp  modified to no longer use env(WIND_HOST_TYPE).
# 01c,24aug98,jmp  moved windHelp{} to windHelp.tcl.
# 01b,21aug98,jmp  added command line parsing: windHelp{}.
#		   renamed all procedures in windHelp*
# 01a,18aug98,fle  written.
#

#
# DESCRIPTION
#
# This library contains all the Wind River Systems help related procedures.
# Those procedures allow to easily access to the Wind River Systems HTML
# reference help
# 
# RESOURCE FILES
#   fileLib.tcl
#   htmlLib.tcl
#
# FILES
# .iP `<docDir>/INDEX.WRS'
#   This is the raw text file containing the whole WRS help data base. Its
#   location is <docDir>. See `windHelpDocDirGet' for more details.
# .iP `<docDir>/TREE.WRS'
#   This file is a raw text file that contains a description of the WRS help
#   tree as given to a `controlValuesSet' function. See `windHelpDocDirGet' for
#   more informations on <docDir>
#

# INTERNAL
#
# PROCEDURES:
#
# windHelp		- windHelpLib command line parser
# windHelpDisplay	- displays specified link in a web browser
# windHelpEntryLookup	- gets the entry number in entry names list
# windHelpInit		- initializes the help data base
# windHelpLinkGet	- gets the <helpTopic> hyper link
# windHelpListGet	- gets all the list help entries

################################################################################
#
# windHelpInit - initialises the help data base
#
# This routine populates the help data base list with entries got from the LIB
# and RTN files from the <inDir> directory.
#
# <inDir> default value is set to `windHelpDocDirGet'. If <inDir> is specified,
# a `windHelpDocDirSet' is proceeded with <path> argument set to <inDir>. Thus,
# all new calls to `windHelpLib' procedures will refer to that directory for
# doc directory
#
# SYNOPSIS
# .tS
#   windHelpInit [<inDir>]
# .tE
#
# PARAMETERS
#   inDir : docs subdirectory to look LIB and RTN files in
#
# RETURNS: N/A
#
# ERRORS: N/A
#

proc windHelpInit { {inDir ""} } {

    # global variables

    global helpEntryList		;# global list of help entries
    global helpEntryNameList		;# list of all help entry names

    # variables initialization

    set helpEntryList {}
    set helpEntryNameList {}
    set entryList {}

    # documentation directory update

    if { ($inDir == "") } {
	set inDir "[wtxPath]docs"
	windHelpDocDirSet $inDir
    }

    # for a faster access to the database, if the INDEX.WRS exists, then read
    # help entry list from here, else read it from the LIB and RTN files

    if { [file exists $inDir/INDEX.WRS] } {

	set entryList [windHelpIndexGet]

    } else {

	set entryList [entryListInFileGet [glob -nocomplain "$inDir/RTN" \
		"$inDir/RTN.SUB" "$inDir/LIB" "$inDir/LIB.SUB" \
		"$inDir/*.lib" "$inDir/*.rtn"]]

	# sort help entries

	set entryList [lsort -dictionary -index 0 $entryList]
    }

    # build the INDEX.WRS and TREE.WRS if they are not presents in the <inDir>
    # directory

    if { ! [file exists $inDir/INDEX.WRS] } {
	windHelpIndexBuild $entryList $inDir
    }

    if { ! [file exists $inDir/TREE.WRS] } {
	windHelpTreeBuild $entryList $inDir
    }

    # if index has not been re-built, the help entry list may not be initialized
    # update it

    if { $helpEntryList == {} } {
	set helpEntryList [windHelpIndexGet]
    }

    unset entryList
}

################################################################################
#
# windHelpDisplay - displays specified link in a web browser
#
# This routine displays in a navigator the specified <helpLink>.
#
# The default value for <option> is set to `-usetarget'
#
# SYNOPSIS
# .tS
#   windHelpDisplay <helpLink> [-usetarget | -notarget]
# .tE
#
# PARAMETERS
#   helpLink : the link to the file to display in the browser
#   option : either `-usetarget' or `-notarget' for calling targetted URL or not
#
# KNOWN PROBLEMS:
#
# When htmlBrowser is iexplore 3.0 or less, calling for an URL that contains
# a target (<URLName>#<targetName>) fails. The iexplore 3.0 doesn't support
# call of targeted URL. As far as it is the only browser (Netscape Navigator,
# Netscape Communicator and iexplore 4.0 do support targetted URL calls), it is
# the users responsibility to get rid of the target by using the -noanchor
# option if he uses iexplore 3.0 (or any other browser that doesn't support
# targetted URL calls)
#
# RETURNS: The execution status. First member of returned list is OK or ERROR.
# On ERROR, the remaining members of list are the error messages.
#
# ERRORS: N/A
#

proc windHelpDisplay { helpLink {anchor -usetarget} } {

    global htmlBrowser			;# html browser
    global hostType			;# user environment variables
    global tcl_platform			;# tcl_platform related info
    global startNetscapeFromUI

    set status {OK}

    # first of all check that the file exists

    if { [regexp "^file://" [string trim $helpLink]] } {

	set helpLink [string trim $helpLink]
	regsub "^file://" $helpLink "" helpLink
	set helpFileName [file nativename [lindex [split $helpLink "\#"] 0]]

	if { ! [file exists $helpFileName] } {
	    set status {ERROR}
	    lappend status "File $helpFileName does not exist"
	    lappend status "Please check that help for that topic has been\
			    installed"
	    return $status
	}
    }

    # reset all the \\ characters to /

    regsub -all {\\} $helpLink {/} helpLink

    # if we are on a WIN32 host (we presume that if hot type is unknown, this
    # may be a WIN32 host) we can use the winhtml facilities

    if { ([regexp "x86-win32|unknown" $hostType]) } {

        set dllName "winhtml.dll"
	if { ! [catch {uitclinfo debugmode} debugMode] } {
	    if { $debugMode } {
		set  dllName "winhtml-d.dll"
	    }
	}

	if { ! [catch {winWebBrowserNavigate $helpLink} navStatus] } {

	    # windWebBrowseNavigate started all right, return OK

	    return $status

	} else {

	    # windWebBrowseNavigate could not be started, let's try to load
	    # the appropriate DLL

	    if { [catch {load $dllName} winHTMLdll] } {

		# could not load the DLL

		set status {ERROR}
		lappend status "Unable to open 'winhtml.dll'. Cause:\
				$winHTMLdll"
	    } else {

		# the DLL is loaded, let's try to start windWebBrowseNavigate
		# again

		if { [catch {winWebBrowserNavigate $helpLink} browserStatus] } {

		    # failed to call winWebBrowserNavigate, let's continue, and
		    # try the more direct solutions

		} else {
		    return $status
		}
	    }
	}
    }

    # look for the anchor parameter

    if { $anchor == "-notarget" } {
	set helpLink [lindex [split $helpLink "\#"] 0]
    }

    if { $htmlBrowser == "" } {

	if { [catch "wtxExecutableFind $helpLink" htmlBrowser] } {
	    set htmlBrowser netscape
	} elseif { $htmlBrowser == "not implemented" } {

	    # wtxExecutableFind was unable to find an associated browser to the
	    # helpLink variable. Let's default it to communicator

	    set htmlBrowser netscape
	}
    }

    # Check if user uses netscape or communicator. If this is the case try
    # to use the -remote option to change the URL of an already
    # running netscape. Note that this is necessary since a given
    # user should not run more that one netscape at a time.

    if { ([regexp "netscape|communicator" $htmlBrowser]) && 
	 ( ! [regexp "x86-win32|unknown" $hostType]) } {

	set cmdList "exec $htmlBrowser"

	set bSeparateProcess 0
	if { [info exists ::env(WIND_HELP_SEPARATE_PROCESS)] } {
		set bSeparateProcess $::env(WIND_HELP_SEPARATE_PROCESS)
    	}
    	
    	if {$bSeparateProcess == 1} {
    		set cmdList2 "$cmdList $helpLink"
    	} else {
    		if { $tcl_platform(platform) == "unix" } {
    			set cmdList2 "$cmdList -raise -remote openUrl($helpLink)"
    		} else {
    			set cmdList2 "$cmdList -remote openUrl($helpLink)"
    		}
    		
    	}

	if { [catch {eval $cmdList2} cmdStatus] } {

	    # communicator / netscape -remote failed, let's start a new session
	    # for communicator / netscape

	    if { ($hostType == "parisc-hpux10") && 
		 ([info exists startNetscapeFromUI]) } {

		if {$startNetscapeFromUI == "FALSE"} {

		    if {[info commands noticePost] == "noticePost"} {
		    	noticePost warning "\nCannot start \"$htmlBrowser\".\n\
					    Start manually \"$htmlBrowser\"\
					    before invoking the on-line help.\n"
		    } else {
			error "\nWarning: Cannot start \"$htmlBrowser\".\n\
				Start manually \"$htmlBrowser\" before invoking\
				the on-line help.\n"
		    }
		} else {
		    eval "$cmdList $helpLink" &
		}
	    } else {
	    	eval "$cmdList $helpLink" &
	    }
	}

    } else { 

	# navigator is not netscape / communicator just start it.

	if { [catch "exec $htmlBrowser $helpLink &" htmlBrowserStatus] } {
	    set status {ERROR}
	    lappend status "Unable to execute html browser : $htmlBrowserStatus"

	    # the html browser can not be started, erase it to try to reset
	    # it next time windHelpDisplay is called

	    set htmlBrowser ""
	}
    }

    return $status
}

################################################################################
#
# windHelpDocDirGet - gets the current doc directory
#
# This procedure returns the path to the currently used doc directory. This
# directory must be set through `windHelpInit', or through `windHelpDocDirSet'
#
# Its default value is set to `wtxPath'/docs
#
# SYNOPSIS
# .tS
#   windHelpDocDirGet
# .tE
#
# PARAMETERS: N/A
#
# ERRORS: N/A
#
# SEE ALSO
# `wtxPath'

proc windHelpDocDirGet {} {

    global docDir		;# global used doc directory

    return $docDir
}

################################################################################
#
# windHelpDocDirSet - sets the current doc directory
#
# This procedure sets the path to the doc directory to be used. After that
# setting, all the help entries reachable through `windHelpListGet' or through
# `windHelpLinkGet' procedures, will be those visible from the <path> directory.
#
# If operation failed (see ERRORS), then the docDirectory keeps its original
# value.
#
# SYNOPSIS
# .tS
#   windHelpDocDirSet <path>
# .tE
#
# PARAMETERS:
#   path : the path to set the doc directory
#
# RETURNS: a list which first element is OK or ERROR. On ERROR, the remaining
# elements of list ar the error codes.
#
# ERRORS:
# on error, the first element of the returned list is ERROR, next elements are
# error codes. The following error codes may happen :
#
# .iP PATH_IS_NOT_DIR
# the specified path is not a directory
# .iP PATH_IS_NOT_READABLE
# the specified <path> is not a readable directory
# .iP PATH_DOESNT_EXIST
# the specified <path> does not exist
#

proc windHelpDocDirSet { path } {

    global docDir		;# global used doc directory

    set status {OK}

    # check if given path is a directory

    if { ! [file isdirectory $path] } {
	set status [lreplace $status 0 0 "ERROR"]
	lappend status "PATH_IS_NOT_DIR"
    }

    # check if directory is readable

    if { ! [file readable $path] } {
	set status [lreplace $status 0 0 "ERROR"]
	lappend status "PATH_IS_NOT_READABLE"
    }

    # check for the path existency

    if { ! [file exists $path] } {
	set status [lreplace $status 0 0 "ERROR"]
	lappend status "PATH_DOESNT_EXIST"
    }

    # now set the docDir variable

    if { [lindex $status 0] == "OK" } {
	set docDir $path
    }

    return $status
}

################################################################################
#
# windHelpEntryLookup - gets the entry number in entry names list
#
# This procedure looks up for the help entry value, and selects the closer
# help topic index (alphabeticaly speaking).
#
# <rule> should be set to either `-exact' or to `-regexp'.
#
# If set to `-exact', then the <entryPattern> must be the exact match in
# <entryNameList>
#
# If set to `-regexp', lookup is done through a regexp that begins with the
# given <entryPattern>, and then, any other character. The first occurrence is
# then returned.
#
# SYNOPSIS
# .tS
#   windHelpEntryLookup <rule> <entryPattern> <entryNameList>
# .tE
#
# PARAMETERS
#   rule : the rule to lookup for <entryPattern> pattern
#   entryPattern : the help pattern to look for in <entryNameList>
#   entryNameList : list of entry names in which to look for <entryPattern>
#
# RETURNS: the index of the designed entry.
#
# ERRORS: N/A
#

proc windHelpEntryLookup { rule entryPattern entryNameList } {

    switch -- $rule {
	"-exact" {

	    set entryNum [lsearch $entryList $entryPattern]
	}

	default {
	    set pattern "^$entryPattern.*"
	    set entryNum [lsearch -regexp $entryNameList $pattern]
	}
    }

    return $entryNum
}

################################################################################
#
# windHelpIndexBuild - builds the WRS help INDEX from the <entryList>
#
# This procedure writes the <inDir>/INDEX.WRS file in order for the windHelpLib
# library to retrieve the routines / libraries faster than by reading and
# processing the LIB and RTN files.
#
# The default value for <inDir> is set to `windHelpDocDirGet'
#
# FILES
# This procedure builds the <inDir>/INDEX.WRS file.
#
# SYNOPSIS
# .tS
#   windHelpIndexBuild <entryList> [<inDir>]
# .tE
#
# PARAMETERS
#   entryList : the list from which to extract the Wind River Systems help index
#   inDir : the directory where to build the index
#
# RETURNS: N/A
#
# ERRORS: N/A
#
# NOMANUAL
#

proc windHelpIndexBuild { entryList { inDir "" } } {

    global helpEntryList		;# global help entry index

    # check for the <inDir> variable

    set helpEntryString ""
    set entryListBuilt ""

    if { $inDir == "" } {
	set inDir [windHelpDocDirGet]
    }

    # initialize the help lists for names, descriptions and links

    foreach helpEntry $entryList {

	# NOTE : the helpEntryList list only keeps the following help 
	# elements : name, description, extracted link, book, chapter, category.
	# Help entry elements pattern and location are not kept

	if { [lindex $helpEntry 6] != {} } {
	    set entryName "[lindex $helpEntry 0] \[[lindex $helpEntry 6]\]"
	} else {
	    set entryName [lindex $helpEntry 0]
	}

	set dupEntryTest [lsearch $entryListBuilt $entryName]
	if { $dupEntryTest == -1 } {
		set thisHelpEntry [list $entryName [lindex $helpEntry 1] \
				[htmlLinkExtract [lindex $helpEntry 3]] \
				[lindex $helpEntry 4] [lindex $helpEntry 5] \
				[lindex $helpEntry 6]]

		lappend helpEntryList $thisHelpEntry
		append helpEntryString "\{$thisHelpEntry\}\n"
		lappend entryListBuilt $entryName
	}
    }

    # save the INDEX into <inDir>/INDEX.WRS

    set fileId [open $inDir/INDEX.WRS w]
    puts $fileId $helpEntryString
    close $fileId
}

################################################################################
#
# windHelpIndexClean - erases the help index file
#
# This procedure erases the help index file <inDir>/INDEX.WRS.
#
# NOTE
# A call to this routine requires access rights to the <inDir> directory.
# The default value for <inDir> is the WRS doc directory : `windHelpDocDirGet'
#
# SYNOPSIS
# .tS
#   windHelpIndexClean [<inDir>]
# .tE
#
# PARAMETERS
#   inDir : the INDEX.WRS container
#
# RETURNS: a list which first element is OK or ERROR, and remaining elements are
# the error messages
#
# ERRORS:
# file delete : permission denied
#
# NOMANUAL
#

proc windHelpIndexClean { {inDir ""} } {

    set status {OK}

    # set the default directory if needed

    if { $inDir == "" } {
	set inDir [windHelpDocDirGet]
    }

    # erase the INDEX.WRS file

    if { [catch "file delete $inDir/INDEX.WRS" deleteStatus] } {
	set status [lreplace $status 0 0 "ERROR"]
	lappend status $deleteStatus
    }

    return $status
}

################################################################################
#
# windHelpIndexGet - gets the current complete help index
#
# This procedure just reads the <inDir>/INDEX.WRS file in order to have a quick
# and complete access to the WRS reference help index.
#
# The returned list is a list of help entries with the following format :
#   { {entryName} {shortDescription} {entryLink} {bookName} {chapterName} 
#     {category} }
#
# \is
# \i `entryName'
# This is the name of the help entry.
# \i `shortDescription'
# The help entry short description. Some few words to describe the entry and its
# use.
# \i `entryLink'
# The help entry html link. This value can directly be used through a call to
# `windHelpDisplay'.
# \i `bookName'
# The book name the help entry belongs to.
# \i `chapterName'
# The chapter name the help entry belongs to.
# \i `category'
# This parameter is to differenciate several help entries that could have the
# same name (for example, sysLib for several BSPs)
# \ie
#
# FILES
# <inDir>/INDEX.WRS : this file is generated when the `windHelpIndexGet'
# procedure is called and doesn't find the INDEX.WRS file.
#
# SYNOPSIS
# .tS
#   windHelpIndexGet [<inDir>]
# .tE
#
# PARAMETERS:
#   inDir : the directory where to read / write the INDEX.WRS file
#
# RETURNS: the list of available help entries or {} if INDEX.WRS is not readable
#
# ERRORS: N/A
#

proc windHelpIndexGet { {inDir ""} } {

    set entryList {}			;# list of entries in the WRS index

    # check for the inDir variable

    if { $inDir == "" } {
	set inDir [windHelpDocDirGet]
    }

    if { ! [file exists $inDir/INDEX.WRS] } {

	# INDEX.WRS has been erased or has not been generated yet. Generate it
	# just by calling windHelpInexBuild

	windHelpIndexBuild $inDir
    }

    # open the file, read it raw, we should have the entire list of help entries
    # as defined in this function header

    if { [file exists $inDir/INDEX.WRS] && 
	 [file readable $inDir/INDEX.WRS] } {

	set fileId [open $inDir/INDEX.WRS r]
	set entryList [read $fileId]
	close $fileId
    }

    # return the list. If the $inDir/INDEX.WRS could not be found or read, just
    # return an empty list

    return $entryList
}

################################################################################
#
# windHelpLinkGet - gets the <helpTopic> hyper link
#
# According to the <rule> parameter, this routine gets the list of hyperlinks
# the <helpTopic> refers to.
#
# If <rule> is set to `-name', the result should be empty or unique. If set to
# `-desc', it returns all the hyperlinks that have descriptions matching
# <helpTopic>
#
# SYNOPSIS
# .tS
#   windHelpLinkGet <rule> <helpTopic> [<keyWords>]
# .tE
#
# PARAMETERS
#   rule : retreiving rule
#   helpTopic : the help topic to look for
#   keyword : a pipe separated list of kewords to look for in order to make the #             help research more precise.
#
# RETURNS: The list of links or {} if no links found
#
# ERRORS: N/A
#

proc windHelpLinkGet { rule helpTopic {keyWordList ""} } {

    global helpEntryNameList		;# list of all help entry names
    global helpEntryList		;# global list of help entries

    set linkList {}

    # the helpEntryNameList may not have been filled in yet. Have a look at it
    # to retreive information faster (using lsearch) we need a list that
    # contains only one element wide list

    if { $helpEntryNameList == {} } {
	foreach entry $helpEntryList {
	    lappend helpEntryNameList [lindex $entry 0]
	}
    }

    set tmpNameList $helpEntryNameList	;# temp list that can be sortened

    # now retreive the element link (list) from the rule parameter and from the
    # element name. The category allows to get the right link for elements that
    # may have the same entry name (for example sysLib for all the BSPs)

    switch -- $rule {

	"-name" {		;# looking for exact match in the name list

	    set indexList {}

	    if {$keyWordList == ""} {	;# get the first entry found

		set entryNum [lsearch -exact $helpEntryNameList $helpTopic]

		if { $entryNum != -1 } {
		    set entry [lindex $helpEntryList $entryNum]
		    set entryLink [lindex $entry 2]
		    set foundLink [fileNameFixup \
			"[windHelpDocDirGet]/$entryLink"]
		    regsub -all {\\} $foundLink {/} foundLink
		    set linkList $foundLink
		}

	    } else {

	    	# get all entries and keep the first entry that matches the
		# given keyWord

		set ix 0

		foreach entryName $helpEntryNameList {
		    if { [regexp "(^$helpTopic$|^$helpTopic .*)" $entryName] } {
			lappend indexList $ix
		    }
		    incr ix
		}

		set linkFound 0
		set keyWordList [split $keyWordList |]

		foreach keyWord $keyWordList {

		    if {$linkFound == 1} {
		    	break
		    }

		    foreach foundIndex $indexList {
			set entryLink [lindex [lindex $helpEntryList \
						      $foundIndex] 2]
			set link [fileNameFixup \
				  "[windHelpDocDirGet]/$entryLink"]
			regsub -all {\\} $link {/} link

			if {[regexp $keyWord $link]} {
			    set linkList $link
			    set linkFound 1
			    continue
			}
		    }
		}
	    }
	}

	"-desc" {		;# looking for all matches in desc list

	    set indexList {}
	    set ix 0

	    foreach entry $helpEntryList {
		set entryDesc [lindex $entry 1]
		if { [regexp $helpTopic $entryDesc] } {
		    lappend indexList $ix
		}

		incr ix
	    }

	    foreach foundIndex $indexList {
		set entryLink [lindex [lindex $helpEntryList \
					      $foundIndex] 2]
		set foundLink [fileNameFixup \
		    "[windHelpDocDirGet]/$entryLink"]
		regsub -all {\\} $foundLink {/} foundLink
		lappend linkList $foundLink
	    }
	}
    }

    return $linkList
}

################################################################################
#
# windHelpListGet - gets the list of help entries
#
# This routine returns the list of help entries according to the <type>
# parameter. This allow to get a list of the entries `name' / `link' / `desc' or
# `all' of them. If <type> is set to `all', then a list of help entries is
# returned. Those help entries are made of : name, description, extracted link,
# book name, chapter name and category name. Default type is set to `all'.
#
# SYNOPSIS
# .tS
#   windHelpListGet [<type>]
# .tE
#
# PARAMETERS
#   type : the returned help entry type [desc | link | name | all]
#
# RETURNS: The list of asked type(s) or {} if none
#
# ERRORS: N/A
#

proc windHelpListGet { {type "all"} } {

    # global variables

    global helpEntryList		;# global list of entries
    global helpEntryNameList		;# list of all help entry names

    set returnedList {}

    # get entry list length

    set listLen [llength $helpEntryList]

    # create the returned list according to the type parameter

    if { $type == "desc" } {		;# getting the desc list

	for {set ix 0} {$ix < $listLen} {incr ix} {
	    lappend returnedList [lindex [lindex $helpEntryList $ix] 1]
	}
    } elseif { $type == "link" } {	;# getting the link list

	for {set ix 0} {$ix < $listLen} {incr ix} {
	    lappend returnedList [lindex [lindex $helpEntryList $ix] 2]
	}
    } elseif { $type == "name" } {	;# getting the name list

	for {set ix 0} {$ix < $listLen} {incr ix} {
	    lappend returnedList [lindex [lindex $helpEntryList $ix] 0]
	}
    } elseif { $type == "book" } {	;# getting the books list

	for {set ix 0} {$ix < $listLen} {incr ix} {
	    set bookName [lindex [lindex $helpEntryList $ix] 0]
	    if { [lsearch $returnedList $bookName] == -1 } {
		lappend returnedList $bookName
	    }
	}
    } else {				;# getting all lists
	set returnedList $helpEntryList
    }

    return $returnedList
}

################################################################################
#
# windHelpTreeBuild - returns the books structure as a tree
#
# This routine saves the wind help tree structure into the <inDir>/TREE.WRS
# file. It first procced the tree build, then save it to file.
#
# SYNOPSIS
# .tS
#   windHelpTreeBuild <entryList> [<inDir>]
# .tE
#
# PARAMETERS
#   entryList : the list of entries from which to extract help tree from
#   inDir : the directory where to save the TREE.WRS (default is 
#           `windHelpDocDirGet')
#
# RETURNS: N/A
#
# ERRORS: N/A
#
# NOMANUAL
#

proc windHelpTreeBuild { entryList {inDir ""} } {

    array set bookArray {}		;# array of book names
    array set chapterArray {}		;# array of chapter names
    set bookList {}			;# list of book names
    set bookEntryList {}		;# keep track of new books entries

    # check for the <inDir> variable

    if { $inDir == "" } {
	set inDir [windHelpDocDirGet]
    }

    # sort entries between books / chapters / entries

    foreach entry $entryList {

	# get the three required values from help entry  : name, book, chapter

	set entryName [lindex $entry 0]
	set bookName [lindex $entry 4]
	set chapterName [lindex $entry 5]
	set entryCategory [lindex $entry 6]
	if { $entryCategory != {} } {
	    set entryName "$entryName \[$entryCategory\]"
	}

	# check if book is already in list

	if { [lsearch $bookList $bookName] == -1} {
	    lappend bookList $bookName
	    lappend bookEntryList $entry
	    set bookArray($bookName) {}
	}

	# check if chapter is already in book list

	if { $chapterName != {} } {
	    set chapterList [lindex [array get bookArray $bookName] 1]
	    if { [lsearch $chapterList $chapterName] == -1 } {
		lappend bookArray($bookName) $chapterName
		set chapterArray($chapterName) {}
	    }
	}

	lappend chapterArray($chapterName) $entryName
    }

    # get back the content of each book and chapter

    set bookList [lsort $bookList]
    set bookTreeList ""

    foreach book $bookList {

	# initialize local list to be updated in loop

	set chapterTreeList ""
	set chapterList [lsort [lindex [array get bookArray $book] 1]]

	if { $chapterList != {} } {

	    foreach chapter $chapterList {

		set eltList [lsort [lindex [array get chapterArray $chapter] 1]]
		set eltTreeList ""

		foreach elt $eltList {
		    set eltTreeList "$eltTreeList\n{$elt}"
		}

		# set the chapter name between curlies only if it contains
		# spaces

		if { [llength $chapter] > 1 } {
		    set chapterTreeList "$chapterTreeList\
					 \n{\
					 \n{$chapter}\
					 \n{\
					 \n$eltTreeList\
					 \n}\
					 \n}"
		} else {
		    set chapterTreeList "$chapterTreeList\
					 \n{\
					 \n$chapter\
					 \n{\
					 \n$eltTreeList\
					 \n}\
					 \n}"
		}
	    }

	    # set the book name between curlies only if it contains spaces

	    if { [llength $book] > 1 } {
		set bookTreeList "$bookTreeList\
				  \n{\
				  \n{$book}\
				  \n$chapterTreeList\
				  \n}"
	    } else {
		set bookTreeList "$bookTreeList\
				  \n{\
				  \n$book\
				  \n$chapterTreeList\
				  \n}"
	    }

	} else {
	    # set the book name between curlies only if it contains spaces

	    if { [llength $book] > 1 } {
		set bookTreeList "$bookTreeList\n{\n{$book}\n}"
	    } else {
		set bookTreeList "$bookTreeList\n{$book}"
	    }
	}

    }

    set fileId [open $inDir/TREE.WRS w]
    puts $fileId $bookTreeList
    close $fileId
}

################################################################################
#
# windHelpTreeClean - erases the help index file
#
# This procedure erases the help index file <inDir>/TREE.WRS.
#
# NOTE
# A call to this routine requires access rights to the <inDir> directory.
# The default value for <inDir> is the WRS doc directory : `windHelpDocDirGet'
#
# SYNOPSIS
# .tS
#   windHelpTreeClean [<inDir>]
# .tE
#
# PARAMETERS
#   inDir : the TREE.WRS container
#
# RETURNS: a list which first element is OK or ERROR, and remaining elements are
# the error messages
#
# ERRORS:
# file delete : permission denied
#
# NOMANUAL
#

proc windHelpTreeClean { {inDir ""} } {

    set status {OK}

    # set the default directory if needed

    if { $inDir == "" } {
	set inDir [windHelpDocDirGet]
    }

    # erase the TREE.WRS file

    if { [catch "file delete $inDir/TREE.WRS" deleteStatus] } {
	set status [lreplace $status 0 0 "ERROR"]
	lappend status $deleteStatus
    }

    return $status
}

################################################################################
#
# windHelpTreeGet - gets the Wind River Help current tree
#
# This routine returns the WRS help tree as it can be displayed by a control
# widget (see `controlCreate', `controlValuesSet'). It directly read the tree
# from a saved file, in order to do less processing.
#
# For a faster processing, it directly reads the <inDir>/TREE.WRS file that
# contains the tree as it should be given to a controlValuesSet procedure. The
# default value for <inDir> is `windHelpDocDirGet'
#
# SYNOPSIS
# .tS
#   windHelpTreeGet [<inDir>]
# .tE
#
# PARAMETERS:
#   inDir : the directory where to find the TREE.WRS file
#
# RETURNS: The WRS help Tree
#
# ERRORS: N/A
#
# SEE ALSO
# `windHelpInit', `controlCreate', `controlValuesSet'
#
# NOMANUAL
#

proc windHelpTreeGet { {inDir ""} } {

    set bookTree {}			;# WRS book tree

    # check for the <inDir> variable

    if { $inDir == "" } {
	set inDir [windHelpDocDirGet]
    }

    if { ! [file exists $inDir/TREE.WRS] } {

	# TREE.WRS has been erased or has not been generated yet. Generate it
	# just by calling windHelpTreeBuild

	windHelpTreeBuild
    }

    # open the file, read it raw, we should have the entire help tree

    if { [file exists $inDir/TREE.WRS] && 
	 [file readable $inDir/TREE.WRS] } {

	set fileId [open $inDir/TREE.WRS r]
	set bookTree [read $fileId]
	close $fileId
    }

    # return the list. If the $inDir/TREE.WRS could not be found or read, just
    # return an empty list

    return $bookTree
}

#
# main - the windHelp main entry
#

global htmlBrowser		;# the currently used html browser
global env			;# environment variables
global docDir			;# doc dir in which to get help entry list

# try to get WIND_HOST_TYPE

if { [info exist env(WIND_HOST_TYPE)] } {
    set hostType $env(WIND_HOST_TYPE)
} elseif { [info command wtxHostType] != ""} {
    set hostType [wtxHostType]
} else {
    set hostType "unknown"
}

# try to source user's local windHelp.tcl if it exists

# look for the docDir variable

set docDir ""

set htmlBrowser ""
if { [file exists "~/.wind/windHelp.tcl"] } {
    source "~/.wind/windHelp.tcl"
}

# source Tcl libraries

source "[wtxPath host resource doctools]fileLib.tcl"
source "[wtxPath host resource doctools]htmlLib.tcl"

if { [windHelpDocDirGet] == "" } {
    windHelpDocDirSet [wtxPath docs]
}

# wtxPath should return absolute pathname, if not (windows host maybe), set it

if { [file pathtype [windHelpDocDirGet]] != "absolute" } {
    set splittedDir [file split [windHelpDocDirGet]]
    set splittedDir [lreplace $splittedDir 0 0 [lindex [file split [pwd]] 0]]
    windHelpDocDirSet [join $splittedDir /]
}

if { ! [file exists [windHelpDocDirGet]] } {
    windHelpDocDirSet "."
}

# init database

windHelpInit [windHelpDocDirGet]
