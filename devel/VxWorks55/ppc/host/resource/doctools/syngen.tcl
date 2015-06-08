# syngen.tcl - Tornado Reference documentation generator
#
# Copyright 1998 Wind River Systems, Inc.
#
# modification history
# --------------------
# 01i,06nov01,wsl  fix SPR 71527 - LOCAL routines in output
# 01h,01oct01,wsl  Fix potential for an infinite loop on faulty input
# 01g,07apr00,dgp  correct error in SYNOPSIS process of C-type comments
# 01f,29feb00,dgp  write error to log if no synopsis is generated
# 01e,29feb00,dgp  change ERROR reporting to Error to conform to build scripts
# 01d,18feb00,dgp  clean up syngen processing to eliminate build errors
# 01c,04nov99,jmp  renamed refSynGet into refSynCGet.
# 01b,25feb99,fle  fixed a problem with badly declared C routines
# 01a,09sep98,fle  written, extracted from refLib.tcl v 01y.
#

#
# DESCRIPTION
#
# Use this utility to collect synopsis-only output from <fileList>
#
# This utility opens all the files from <fileList> and appends the found
# synopses specified through the `-d <outDir>' option.
# 
# The `-config' option allows to specify which config file to use in case of
# unknown file extension (know extensions yet are .c, .cc, .cpp, .tcl, .sh,
# .java, .s, .nr and .pcl) default value for <configFile> is set to C.
#
# The `-clean' option is to specify that the synopses file will be cleaned, not
# built.
#
# SYNOPSIS
# .tS
#   syngen [-config <configFile>] [-clean] [-d <outDir>] [-l <logFile>] [-h]
#          [-V] <fileList>
# .tE
#
# PARAMETERS
#   -clean : specifies that it is a synopses cleaning operation
#   -config : specifies <configFile> is to be used to parse <fileList>
#   -d : specifies that <outDir> is to be used to append all the synopses
#   -h : displays the help message
#   -l : specifies that <logFile> will be used as the log receiver
#   -V : to set the verbose mode on
#   fileList : list of files to be parsed by syngen
#
# SEE ALSO:
# refgen, htmlLink, htmlBook, windHelpLib
#
# NOROUTINES
#

###############################################################################
#
# syngen - collect subroutine synopses
#
# Use this procedure to collect synopsis-only output
#
# SYNOPSIS
#   syngen
#
# PARAMETERS : N/A
#
# RETURNS: N/A
#
# ERRORS: N/A
#

proc syngen {} {

    global cleanOnly		;# shall we clean the synopses file first ?
    global fConfig		;# config file to use
    global fileList		;# list of files to parsed by syngen
    global INfmtSynop		;# list of synopsis substitutions
    global INfmtRawSynop	;# list of raw synopses substitutions
    global LOCALdecls   	;# list of regexps that declare subroutine local
    global outDir		;# output doc files directory
    global printInternal	;# do we have to print internal routines
    global saySomething		;# verbose mode indicator
    global srcPath		;# source file path

    global SYNTAX		;# language specific syntax array
    global SYNOP		;# language specific synopsis syntax array
    global SYNSEP		;# separation marks inside function name

    # transform path into full compatible path

    regsub -all "\\\\" $outDir "/" outDir
    set synCollector ""

    # get the collector file name

    set synCollectorFile [syngenOutFileNameGet $outDir]

    # if the -clean option was set, clean the synopses collector file and exit

    if { $cleanOnly } {
	catch [file delete $synCollectorFile]
	return
    }

    # in case of verbose mode

    if {$saySomething} {
	puts "\nsyngen: BASIC FORMATTING"
	puts -nonewline "syngen: gathering synopses in $synCollectorFile : "
    }

    foreach inFile $fileList {

	if {$saySomething} {
	    puts stdout $inFile
	}

	# Select default config based on file suffix.
	# Default default is C2html; hence no need to recognize .c

	set fileConfig ""
	set fs {}
	set fs [file extension $inFile]
	switch $fs {
	    ".tcl"	{set fileConfig Tcl2html}
	    ".sh"	{set fileConfig sh2html}
	    ".pcl"	{set fileConfig pcl2html}
	    ".s"	{set fileConfig Asm2html}
	    ".nr"	{set fileConfig bsp2html}
	    ".java"	{set fileConfig Java2html}
	    ".cpp"	{set fileConfig Cpp2html}
	}

	# Set default values for all standard configurable parameters
	# Go through refConfigParse to avoid dual maintenance of
	# defaults and C2html
	# Override defaults with user's choice if present
	# (may be only a partial override)

	if {0 != [string length "$fConfig"]} {
	    eval [refConfigParse $fConfig]
	} elseif { [string length $fileConfig] != 0 } {
	    eval [refConfigParse $fileConfig]
	} else {
	    eval [refConfigParse C2html]
	}

	if { ! [catch "open $inFile \"r\"" inFileId] } {

	    # Grab the entire source file

	    set srcText [read $inFileId]

	    # if NOROUTINES, stop now

	    if { [regexp "$SYNTAX(libCn)NOROUTINES *\n" $srcText] } {
		break
	    }
		
	    # Grab each routine synopses

	    while {[regexp -indices "$SYNTAX(rtSt)\n" $srcText rtnIx]} {

		set srcText [string range $srcText [lindex $rtnIx 0] end]

		# we have found a routine start pattern. Now look for the
		# routine end

		if { ! [regexp -indices $SYNTAX(rtEn) $srcText rtnEndIx] } {

		    refErrorLog "Error in $inFile" "Cannot find end of entry after \
				[string range $srcText [lindex $rtnIx 1] 50]..."
		    break
		}

		# now just keep the interesting routine description in nRtn,
		# while srcText contains the remaining of the text

		set zz [lindex $rtnEndIx 1]
		set nRtn [string range $srcText 0 $zz]
		set srcText [string range $srcText [expr $zz + 1] end]
		set getIt 1
		set rtnSyn ""

		# figure out if the routine description is extracted
		# from the source code (C or PCL format) or from the
		# comment

		if {[regexp "$SYNTAX(rtCn)SYNOPSIS.*(\\\\se|\.tE)" $nRtn rSynop]} {
		    set rtnSyn $rSynop
		} else {
		    if { $SYNOP(getSyn) && ($SYNTAX(langName) == "C source file") } {
			set rtnSyn [refSynCGet $srcText]
		    } else {
			if { $SYNOP(getSyn) && ($SYNTAX(langName) ==\
				"protocol description file") } {
			    set rtnSyn [refSynPclGet $srcText]
			} else {
			    set rtnSyn ""
			}
		    }
		}

		# only append synopses that are to be documented (suppress all
		# the NOMANUAL or LOCAL or static declarations)

		if { [regexp NOMANUAL $nRtn] } {
		    set getIt 0
		} else {
		    foreach localDecl $LOCALdecls {
			if { [regexp $localDecl "\n$rtnSyn"] } {
			    set getIt 0
			}
		    }
		}

		# Locate and extract parts of title line

		if { $getIt } {

		    # add the separation character to the authorized members of
		    # routines names

		    set rtNameWithSep [split $SYNTAX(rtNm) {}]
		    set pattEndIndex [lsearch -exact $rtNameWithSep "]"]
		    if { $pattEndIndex != -1 } {
			set rtNameWithSep [linsert $rtNameWithSep $pattEndIndex\
						   $SYNSEP(rtSep)]
			set rtNameWithSep [join $rtNameWithSep {}]
		    } else {
			set rtNameWithSep $SYNTAX(rtNm)
		    }

		    # now get the routine name and title from first line

		    if { ! [regexp -indices \
				   "$SYNTAX(rtSt)\[^\n]*($SYNTAX(rtCn))+ *\
				   ($rtNameWithSep) +- +(\[^\n]*)"\
				   $nRtn iNext {} iRName iRTitle]} {

			refErrorLog "Error in $inFile" "Cannot parse routine title near \
			     [string range $nRtn 0 100] \n"
			continue
		    }

		    set rNameRaw [eval "string range \$nRtn $iRName"]
		    regsub $SYNSEP(rtSep) $rNameRaw $SYNSEP(rtSepSub) rNameRaw
		    if { ! $SYNSUB(rtUnmk) } {
			regsub {.*} $rNameRaw $SYNSUB(rtMk) rNameRaw
		    }

		    set rTitle [eval "string range \$nRtn $iRTitle"]

		    # remove leading continuation char, SYNOPSIS,
		    # \n and spaces

		    regsub -all $SYNTAX(rtCn) $rtnSyn "\n" rtnSyn
		    set rtnSyn [string trim $rtnSyn]
		    regsub "SYNOPSIS *\n(\\\\ss|\.tS) *\n" $rtnSyn {} rtnSyn
		    regsub "\n(\\\\se|\.tE)" $rtnSyn {} rtnSyn
		    regsub "/\@" $rtnSyn "/\*" rtnSyn
		    regsub "\@/" $rtnSyn "\*/" rtnSyn
		    while { [string range $rtnSyn 0 0] == "\n" } {
			set rtnSyn [string trim [string range $rtnSyn 1 end]]
		    }

		    # remove trailing \n and spaces

		    set synEndIx [expr [string length $rtnSyn] - 1]
		    while { [string range $rtnSyn $synEndIx $synEndIx] 
			    == "\n" } {

			set rtnSyn [string trim [string range 0 \
							[expr rtnSyn - 1]]]
		    }

		    # use INfmtSynop to have everything right in synopsis

		    foreach fmtSynop $INfmtSynop {
			regsub -all [lindex $fmtSynop 0] $rtnSyn\
				    [lindex $fmtSynop 1] rtnSyn
		    }

		    # use INfmtRawSynop to have everything right in synopsis

		    foreach fmtRawSynop $INfmtRawSynop {
			regsub -all [lindex $fmtRawSynop 0] $rtnSyn\
				    [lindex $fmtRawSynop 1] rtnSyn
		    }

		    # log an error if there is no synopsis

		    if { $rtnSyn == "" } {
			refErrorLog "Error in $inFile" "No synopsis created for \
			     $rNameRaw \n"
		    }	

		    # now append title and synopsis to the synopses collector

		    set rtnDesc "$rNameRaw - $rTitle\n\n$rtnSyn\n-----\n"
		    append synCollector $rtnDesc
		}
	    }

	    close $inFileId

	} else {
	    refErrorLog "Error in $inFile" "*** syngen Error ***\n"
	}
    }

    # remove last \n character

    if { [string range $synCollector end end] == "\n" } {
	set synCollector [string range $synCollector 0 \
				 [expr [string length $synCollector] - 2]]
    }

    # now save the synopses into file

    if { ! [catch "open $synCollectorFile \"a\"" outFileId] } {
	if { $synCollector != "" } {
	    puts $outFileId $synCollector
	}
	close $outFileId
    } else {
	refErrorLog "open $synCollectorFile \"a\"" \
		    "*** syngen Error in $outFileId ***"
    }

    if {$saySomething} {
	puts stdout "\nDone. "
    }
}

################################################################################
#
# syngenCommandeParse - syngen commande parser
#
# This routine parses and verifies syngen commande line arguments, and updates
# some global variables according to those arguments.
#
# SYNOPSIS
#   syngenCommandeParse argv argc
#
# PARAMETERS
#   argv : arguments list vector
#   argc : arguments counter
#
# RETURNS: N/A
#
# ERRORS:
#   Displays help message on arguments error
#

proc syngenCommandeParse { argv argc } {

    global cleanOnly		;# shall we clean the synopses file first ?
    global fConfig		;# config file to use
    global fileList		;# list of files to parsed by syngen
    global outDir		;# output doc files directory
    global printInternal	;# do we have to print internal routines
    global saySomething		;# verbose mode indicator

    # global variables

    for {set ix 0} {$ix < $argc} {incr ix} {

	switch -- [lindex $argv $ix] {
	
	    "-clean" {
		set cleanOnly 1
	    }

	    "-config" {
		incr ix
		set fConfig [lindex $argv $ix]
	    }

	    "-h" {
		syngenUsage
		exit
	    }

	    "-int" {
		set printInternal 1
	    }

	    "-d" {
		incr ix
		set outDir [lindex $argv $ix]
	    }

	    "-V" {
		set saySomething 1
		puts "syngen: Verbose mode ON"
	    }

	    default {
		lappend fileList [lindex $argv $ix]
	    }
	}
    }
}

################################################################################
#
# syngenOutFileNameGet - gets the name of the synopses file collector
#
# Get the synopses collector file name. If in the target/src directory, then
# only the dir name after target src is kept. Else, only the last directory
# name is kept
#
# SYNOPSIS
#   syngenOutFileNameGet <outDir>
#
# PARAMETERS
#   outDir : synopses file collector directory
#
# RETURNS: Path to the synopses file collector
#
# ERRORS: N/A
#

proc syngenOutFileNameGet { outDir } {

    set synCollectorFile [pwd]
    set splittedPath [file split $synCollectorFile]

    set targetIx [lsearch -exact $splittedPath "target"]
    set srcIx [lsearch -exact $splittedPath "src"]

    if { ($srcIx == [expr $targetIx + 1]) &&
	 ([llength $splittedPath] > $srcIx) } {

	# we are in target/src, just keep following dir name

	set synCollectorFile "[lindex $splittedPath [expr $srcIx + 1]].syn"
	set synCollectorFile [file join $outDir $synCollectorFile]

	# if we are just in target/src/<dirName>, then delete the synopsis
	# file first

	if { [expr $srcIx + 2] == [llength $splittedPath] } {
	    catch [file delete $synCollectorFile]
	}

    } else {

	# not in target/src, keep the upper directory name

	set synCollectorFile "[lindex $splittedPath end].syn"
	set synCollectorFile [file join $outDir $synCollectorFile]
    }

    return $synCollectorFile
}

################################################################################
#
# syngenInit - initializes all syngen global variables
#
# This procedure is to initialise syngen with all its global variables and any
# other related environment
#
# SYNOPSIS
#   syngenInit
#
# PARAMETERS: N/A
#
# RETURNS: N/A
#
# ERRORS: N/A
#

proc syngenInit {} {

    # global variables

    global cleanOnly		;# shall we clean the synopses file first ?
    global env			;# user's environment variables
    global fConfig		;# config file to use
    global fileList		;# list of files to parsed by syngen
    global INfmtSynop		;# list of synopsis substitutions
    global INfmtRawSynop	;# list of raw synopses substitutions
    global logFileName		;# log file name
    global outDir		;# output doc files directory
    global printInternal	;# do we have to print internal routines
    global saySomething		;# verbose mode indicator
    global srcPath		;# source file path
    global synFile		;# output file
    global SYNTAX		;# global syntax array
    global SYNOP		;# global synopsis syntax array

    # global variables initialization

    set cleanOnly		0
    set fConfig			{}
    set fileList		{}
    set INfmtSynop		{}
    set INfmtRawSynop		{}
    set logFileName		""
    set synFile			"./synopses.syn"
    set outDir			"."
    set printInternal		0
    set saySomething		0
    set srcPath			[file dirname [info script]]
    array set SYNTAX		{}
    array set SYNOP		{}

    # source windHelp library from Tornado tree if possible, or from local dir
    # else

    if { [info exists env(WIND_BASE)] } {
	source "$env(WIND_BASE)/host/resource/doctools/refLib.tcl"
    } else {
	source refLib.tcl
    }
}

################################################################################
#
# syngenUsage - syngen usage display help
#
# This routine just displays syngen help message and exits.
#
# SYNOPSIS
#   syngenUsage
#
# RETURNS: N/A
#
# ERRORS: N/A
#

proc syngenUsage {} {

    puts stdout "\nsyngen : source file synopsis documentation generator\n"
    puts stdout "usage :\n"
    puts stdout "syngen\t\[-clean] \[-config <configFile>\] \[-d <outDir>\]"
    puts stdout "\t\[-h\] \[-l <logFile>] \[-d <outDir>\] \[-V\] <fileList>\n"
    puts stdout "with :\n"
    puts stdout "\t-clean\t\t\t: cleans the synopses file recipient"
    puts stdout "\t-config <configFile>\t: the config file to use to generate"
    puts stdout "\t\t\t\t  synopses"
    puts stdout "\t-d <outDir>\t\t: uses <outDir> as the out doc file"
    puts stdout "\t-h\t\t\t: prints this help message"
    puts stdout "\t-l <logFile>\t\t: specifies lof file location"
    puts stdout "\t-V\t\t\t: sets verbose mode on"
    puts stdout "\t<fileList>\t\t: Any other element is taken as a file name\n"
}

#
# main - the syngen main entry
#

syngenInit
syngenCommandeParse $argv $argc

# command line has been parsed, start calling syngen to generate the synopsis
# files

syngen
